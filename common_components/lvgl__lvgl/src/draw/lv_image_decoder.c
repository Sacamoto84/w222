/**
 * @file lv_image_decoder.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_image_decoder_private.h"
#include "../misc/lv_assert.h"
#include "../draw/lv_draw_image.h"
#include "../misc/lv_ll.h"
#include "../misc/lv_profiler.h"
#include "../stdlib/lv_string.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/
#define img_decoder_ll_p &(LV_GLOBAL_DEFAULT()->img_decoder_ll)
#define img_cache_p (LV_GLOBAL_DEFAULT()->img_cache)
#define img_header_cache_p (LV_GLOBAL_DEFAULT()->img_header_cache)
#define image_cache_draw_buf_handlers &(LV_GLOBAL_DEFAULT()->image_cache_draw_buf_handlers)

#if LV_USE_OS != LV_OS_NONE
    #define img_decoder_info_lock_p &(LV_GLOBAL_DEFAULT()->img_decoder_info_lock)
    #define img_decoder_open_lock_p &(LV_GLOBAL_DEFAULT()->img_decoder_open_lock)
#else
    #define img_decoder_info_lock_p NULL
    #define img_decoder_open_lock_p NULL
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static uint32_t img_width_to_stride(lv_image_header_t * header);

/**
 * Получите информацию заголовка источника изображения и верните указатель на декодер, который может его открыть.
 * @param dsc       Дескриптор изображения, содержащий источник и тип изображения, а также другую информацию.
 * @param header    Заголовок изображения
 * @return Декодер, который может открыть источник изображения или NULL, если он не найден (или не может его открыть).
 */
static lv_image_decoder_t * image_decoder_get_info(lv_image_decoder_dsc_t * dsc, lv_image_header_t * header);

static lv_result_t try_cache(lv_image_decoder_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Инициализируйте модуль декодера изображений
 */
void lv_image_decoder_init(uint32_t image_cache_size, uint32_t image_header_count)
{
    lv_ll_init(img_decoder_ll_p, sizeof(lv_image_decoder_t));

    /*Инициализировать кеш*/
    lv_image_cache_init(image_cache_size);
    lv_image_header_cache_init(image_header_count);

    lv_mutex_init(img_decoder_info_lock_p);
    lv_mutex_init(img_decoder_open_lock_p);
}

/**
 * Деинициализируйте модуль декодера изображений.
 */
void lv_image_decoder_deinit(void)
{
    lv_cache_destroy(img_cache_p, NULL);
    lv_cache_destroy(img_header_cache_p, NULL);

    lv_mutex_delete(img_decoder_info_lock_p);
    lv_mutex_delete(img_decoder_open_lock_p);

    lv_ll_clear(img_decoder_ll_p);
}

lv_result_t lv_image_decoder_get_info(const void * src, lv_image_header_t * header)
{
    LV_PROFILER_DECODER_BEGIN;
    lv_image_decoder_dsc_t dsc;
    lv_memzero(&dsc, sizeof(lv_image_decoder_dsc_t));
    dsc.src = src;
    dsc.src_type = lv_image_src_get_type(src);

    lv_mutex_lock(img_decoder_info_lock_p);
    lv_image_decoder_t * decoder = image_decoder_get_info(&dsc, header);
    lv_mutex_unlock(img_decoder_info_lock_p);

    LV_PROFILER_DECODER_END;
    return decoder ? LV_RESULT_OK : LV_RESULT_INVALID;
}

lv_result_t lv_image_decoder_open(lv_image_decoder_dsc_t * dsc, const void * src, const lv_image_decoder_args_t * args)
{
    LV_PROFILER_DECODER_BEGIN;
    lv_memzero(dsc, sizeof(lv_image_decoder_dsc_t));

    if(src == NULL) {
        LV_PROFILER_DECODER_END;
        return LV_RESULT_INVALID;
    }
    dsc->src = src;
    dsc->src_type = lv_image_src_get_type(src);

    lv_mutex_lock(img_decoder_open_lock_p);

    if(lv_image_cache_is_enabled()) {
        dsc->cache = img_cache_p;
        /*Сначала попробуйте кеш, если только нам не скажут игнорировать кеш.*/
        if(!(args && args->no_cache)) {
            /*
            * Сначала проверь кэш
            * Если изображение найдено в кеше, просто верните его.*/
            if(try_cache(dsc) == LV_RESULT_OK) {
                lv_mutex_unlock(img_decoder_open_lock_p);
                LV_PROFILER_DECODER_END;
                return LV_RESULT_OK;
            }
        }
    }

    /*Найдите декодер, который может открыть источник изображения и одновременно получить информацию о заголовке.*/
    dsc->decoder = image_decoder_get_info(dsc, &dsc->header);
    if(dsc->decoder == NULL) {
        lv_mutex_unlock(img_decoder_open_lock_p);
        LV_PROFILER_DECODER_END;
        return LV_RESULT_INVALID;
    }

    /*Сделать макияж*/
    dsc->args = args ? *args : (lv_image_decoder_args_t) {
        .stride_align = LV_DRAW_BUF_STRIDE_ALIGN != 1,
        .premultiply = false,
        .no_cache = false,
        .use_indexed = false,
        .flush_cache = false,
    };

    /*
     * Мы предполагаем, что если декодер сможет получить информацию, он сможет открыть изображение.
     * Если открытие декодера не удалось, освободите источник и верните ошибку.
     * Если открытие декодера прошло успешно, добавьте изображение в кеш, если оно включено.
     * */
    LV_PROFILER_DECODER_BEGIN_TAG(dsc->decoder->name);
    lv_result_t res = dsc->decoder->open_cb(dsc->decoder, dsc);
    LV_PROFILER_DECODER_END_TAG(dsc->decoder->name);

    if(res == LV_RESULT_OK && dsc->decoded != NULL) {
        LV_ASSERT_MSG(dsc->decoded->unaligned_data && dsc->decoded->handlers, "Invalid draw buffer");

        /* Очистите D-кэш, если он включен и изображение было успешно открыто. */
        if(dsc->args.flush_cache) {
            lv_draw_buf_flush_cache(dsc->decoded, NULL);
            LV_LOG_INFO("Flushed D-cache: src %p (%s) (W%d x H%d, data: %p cf: %d)",
                        src,
                        dsc->src_type == LV_IMAGE_SRC_FILE ? (const char *)src : "c-array",
                        dsc->decoded->header.w,
                        dsc->decoded->header.h,
                        (void *)dsc->decoded->data,
                        dsc->decoded->header.cf);
        }
    }

    lv_mutex_unlock(img_decoder_open_lock_p);
    LV_PROFILER_DECODER_END;
    return res;
}

lv_result_t lv_image_decoder_get_area(lv_image_decoder_dsc_t * dsc, const lv_area_t * full_area,
                                      lv_area_t * decoded_area)
{
    LV_PROFILER_DECODER_BEGIN;
    lv_result_t res = LV_RESULT_INVALID;
    if(dsc->decoder->get_area_cb) {
        LV_PROFILER_DECODER_BEGIN_TAG(dsc->decoder->name);
        res = dsc->decoder->get_area_cb(dsc->decoder, dsc, full_area, decoded_area);
        LV_PROFILER_DECODER_END_TAG(dsc->decoder->name);
    }

    LV_PROFILER_DECODER_END;
    return res;
}

void lv_image_decoder_close(lv_image_decoder_dsc_t * dsc)
{
    LV_PROFILER_DECODER_BEGIN;
    if(!dsc->decoder) {
        LV_PROFILER_DECODER_END;
        return;
    }

    lv_mutex_lock(img_decoder_open_lock_p);

    if(dsc->decoder->close_cb) {
        LV_PROFILER_DECODER_BEGIN_TAG(dsc->decoder->name);
        dsc->decoder->close_cb(dsc->decoder, dsc);
        LV_PROFILER_DECODER_END_TAG(dsc->decoder->name);
    }

    if(lv_image_cache_is_enabled() && dsc->cache && dsc->cache_entry) {
        /*Декодированные данные находятся в кеше, освободите их из обратного вызова кеша.*/
        lv_cache_release(dsc->cache, dsc->cache_entry, NULL);
    }

    lv_mutex_unlock(img_decoder_open_lock_p);
    LV_PROFILER_DECODER_END;
}

/**
 * Создайте новый декодер изображений
 * @return указатель на новый декодер изображений
 */
lv_image_decoder_t * lv_image_decoder_create(void)
{
    lv_image_decoder_t * decoder;
    decoder = lv_ll_ins_head(img_decoder_ll_p);
    LV_ASSERT_MALLOC(decoder);
    if(decoder == NULL) return NULL;

    lv_memzero(decoder, sizeof(lv_image_decoder_t));

    return decoder;
}

void lv_image_decoder_delete(lv_image_decoder_t * decoder)
{
    lv_ll_remove(img_decoder_ll_p, decoder);
    lv_free(decoder);
}

lv_image_decoder_t * lv_image_decoder_get_next(lv_image_decoder_t * decoder)
{
    if(decoder == NULL)
        return lv_ll_get_head(img_decoder_ll_p);
    else
        return lv_ll_get_next(img_decoder_ll_p, decoder);
}

void lv_image_decoder_set_info_cb(lv_image_decoder_t * decoder, lv_image_decoder_info_f_t info_cb)
{
    decoder->info_cb = info_cb;
}

void lv_image_decoder_set_open_cb(lv_image_decoder_t * decoder, lv_image_decoder_open_f_t open_cb)
{
    decoder->open_cb = open_cb;
}

void lv_image_decoder_set_get_area_cb(lv_image_decoder_t * decoder, lv_image_decoder_get_area_cb_t get_area_cb)
{
    decoder->get_area_cb = get_area_cb;
}

void lv_image_decoder_set_close_cb(lv_image_decoder_t * decoder, lv_image_decoder_close_f_t close_cb)
{
    decoder->close_cb = close_cb;
}

lv_cache_entry_t * lv_image_decoder_add_to_cache(lv_image_decoder_t * decoder,
                                                 lv_image_cache_data_t * search_key,
                                                 const lv_draw_buf_t * decoded, void * user_data)
{
    LV_PROFILER_DECODER_BEGIN;
    lv_cache_entry_t * cache_entry = lv_cache_add(img_cache_p, search_key, NULL);
    if(cache_entry == NULL) {
        LV_PROFILER_DECODER_END;
        return NULL;
    }

    lv_image_cache_data_t * cached_data;
    cached_data = lv_cache_entry_get_data(cache_entry);

    /*Установите запись кэша для данных декодера*/
    cached_data->decoded = decoded;
    if(cached_data->src_type == LV_IMAGE_SRC_FILE) {
        cached_data->src = lv_strdup(cached_data->src);
    }
    cached_data->user_data = user_data; /*Необходимо уменьшить данные при аннулировании кеша вместо decoder_close*/
    cached_data->decoder = decoder;

    LV_PROFILER_DECODER_END;
    return cache_entry;
}

lv_draw_buf_t * lv_image_decoder_post_process(lv_image_decoder_dsc_t * dsc, lv_draw_buf_t * decoded)
{
    LV_PROFILER_DECODER_BEGIN;
    if(decoded == NULL) {
        LV_PROFILER_DECODER_END;
        return NULL; /*Нет необходимости настраивать*/
    }

    lv_image_decoder_args_t * args = &dsc->args;
    if(args->stride_align && decoded->header.cf != LV_COLOR_FORMAT_RGB565A8) {
        uint32_t stride_expect = lv_draw_buf_width_to_stride(decoded->header.w, decoded->header.cf);
        if(decoded->header.stride != stride_expect) {
            LV_LOG_TRACE("Stride mismatch");
            lv_result_t res = lv_draw_buf_adjust_stride(decoded, stride_expect);
            if(res != LV_RESULT_OK) {
                lv_draw_buf_t * aligned = lv_draw_buf_create_ex(image_cache_draw_buf_handlers, decoded->header.w, decoded->header.h,
                                                                decoded->header.cf, stride_expect);
                if(aligned == NULL) {
                    LV_LOG_ERROR("No memory for Stride adjust.");
                    LV_PROFILER_DECODER_END;
                    return NULL;
                }

                lv_draw_buf_copy(aligned, NULL, decoded, NULL);
                decoded = aligned;
            }
        }
    }

    /*Предварительно умножить альфа-канал*/
    if(args->premultiply
       && !LV_COLOR_FORMAT_IS_ALPHA_ONLY(decoded->header.cf)
       && lv_color_format_has_alpha(decoded->header.cf)
       && !lv_draw_buf_has_flag(decoded, LV_IMAGE_FLAGS_PREMULTIPLIED)
       && decoded->header.cf != LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED /*еще не сделал*/
      ) {
        LV_LOG_TRACE("Alpha premultiply.");
        if(lv_draw_buf_has_flag(decoded, LV_IMAGE_FLAGS_MODIFIABLE)) {
            /*Сделайте это напрямую*/
            lv_draw_buf_premultiply(decoded);
        }
        else {
            decoded = lv_draw_buf_dup_ex(image_cache_draw_buf_handlers, decoded);
            if(decoded == NULL) {
                LV_LOG_ERROR("No memory for premultiplying.");
                LV_PROFILER_DECODER_END;
                return NULL;
            }

            lv_draw_buf_premultiply(decoded);
        }
    }

    LV_PROFILER_DECODER_END;
    return decoded;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_image_decoder_t * image_decoder_get_info(lv_image_decoder_dsc_t * dsc, lv_image_header_t * header)
{
    LV_PROFILER_DECODER_BEGIN;
    lv_memzero(header, sizeof(lv_image_header_t));

    const void * src = dsc->src;
    lv_image_src_t src_type = dsc->src_type;

    if(src_type == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t * img_dsc = src;
        if(img_dsc->data == NULL) {
            LV_PROFILER_DECODER_END;
            return NULL;
        }
    }

    if(src_type == LV_IMAGE_SRC_FILE) LV_LOG_TRACE("Try to find decoder for %s", (const char *)src);
    else LV_LOG_TRACE("Try to find decoder for %p", src);

    lv_image_decoder_t * decoder;
    bool is_header_cache_enabled = lv_image_header_cache_is_enabled();

    if(is_header_cache_enabled && src_type == LV_IMAGE_SRC_FILE) {
        lv_image_header_cache_data_t search_key;
        search_key.src_type = src_type;
        search_key.src = src;

        lv_cache_entry_t * entry = lv_cache_acquire(img_header_cache_p, &search_key, NULL);

        if(entry) {
            lv_image_header_cache_data_t * cached_data = lv_cache_entry_get_data(entry);
            *header = cached_data->header;
            decoder = cached_data->decoder;
            lv_cache_release(img_header_cache_p, entry, NULL);

            LV_LOG_TRACE("Found decoder %s in header cache", decoder->name);
            LV_PROFILER_DECODER_END;
            return decoder;
        }
    }

    if(src_type == LV_IMAGE_SRC_FILE) {
        lv_fs_res_t fs_res = lv_fs_open(&dsc->file, src, LV_FS_MODE_RD);
        if(fs_res != LV_FS_RES_OK) {
            LV_LOG_ERROR("File open failed: %" LV_PRIu32, (uint32_t)fs_res);
            LV_PROFILER_DECODER_END;
            return NULL;
        }
    }

    /*Поиск декодеров*/
    LV_LL_READ(img_decoder_ll_p, decoder) {
        /*Требуется информация и открытые обратные вызовы.*/
        if(decoder->info_cb && decoder->open_cb) {
            lv_fs_seek(&dsc->file, 0, LV_FS_SEEK_SET);
            LV_PROFILER_DECODER_BEGIN_TAG(decoder->name);
            lv_result_t res = decoder->info_cb(decoder, dsc, header);
            LV_PROFILER_DECODER_END_TAG(decoder->name);

            if(res == LV_RESULT_OK) {
                if(header->stride == 0) {
                    LV_LOG_INFO("Image decoder didn't set stride. Calculate it from width.");
                    header->stride = img_width_to_stride(header);
                }
                break;
            }
            else {
                LV_LOG_TRACE("Can't open image with decoder %s. Trying next decoder.", decoder->name);
            }
        }
    }

    if(decoder == NULL) LV_LOG_TRACE("No decoder found");
    else LV_LOG_TRACE("Found decoder %s", decoder->name);

    if(src_type == LV_IMAGE_SRC_FILE) {
        lv_fs_close(&dsc->file);
    }

    if(is_header_cache_enabled && src_type == LV_IMAGE_SRC_FILE && decoder) {
        lv_cache_entry_t * entry;
        lv_image_header_cache_data_t search_key;
        search_key.src_type = src_type;
        search_key.src = lv_strdup(src);
        search_key.decoder = decoder;
        search_key.header = *header;
        entry = lv_cache_add(img_header_cache_p, &search_key, NULL);

        if(entry == NULL) {
            if(src_type == LV_IMAGE_SRC_FILE) lv_free((void *)search_key.src);
            LV_PROFILER_DECODER_END;
            return NULL;
        }

        lv_cache_release(img_header_cache_p, entry, NULL);
    }

    LV_PROFILER_DECODER_END;
    return decoder;
}

static uint32_t img_width_to_stride(lv_image_header_t * header)
{
    if(header->cf == LV_COLOR_FORMAT_RGB565A8) {
        return header->w * 2;
    }
    else {
        return ((uint32_t)header->w * lv_color_format_get_bpp(header->cf) + 7) >> 3;
    }
}

static lv_result_t try_cache(lv_image_decoder_dsc_t * dsc)
{
    LV_PROFILER_DECODER_BEGIN;
    lv_cache_t * cache = dsc->cache;

    lv_image_cache_data_t search_key;
    search_key.src_type = dsc->src_type;
    search_key.src = dsc->src;

    lv_cache_entry_t * entry = lv_cache_acquire(cache, &search_key, NULL);

    if(entry) {
        lv_image_cache_data_t * cached_data = lv_cache_entry_get_data(entry);
        dsc->decoded = cached_data->decoded;
        dsc->decoder = (lv_image_decoder_t *)cached_data->decoder;
        dsc->cache_entry = entry;     /*Сохраните кеш, чтобы положить его в decoder_close.*/
        LV_PROFILER_DECODER_END;
        return LV_RESULT_OK;
    }

    LV_PROFILER_DECODER_END;
    return LV_RESULT_INVALID;
}
