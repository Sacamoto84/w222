/**
 * @file lv_bin_decoder.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../draw/lv_image_decoder_private.h"
#include "lv_bin_decoder.h"
#include "../../draw/lv_draw_image.h"
#include "../../draw/lv_draw_buf.h"
#include "../../stdlib/lv_string.h"
#include "../../stdlib/lv_sprintf.h"
#include "../../libs/rle/lv_rle.h"
#include "../../core/lv_global.h"

#if LV_USE_LZ4_EXTERNAL
    #include <lz4.h>
#endif

#if LV_USE_LZ4_INTERNAL
    #include "../../libs/lz4/lz4.h"
#endif

/*********************
 *      DEFINES
 *********************/

#define DECODER_NAME    "BIN"

#define image_cache_draw_buf_handlers &(LV_GLOBAL_DEFAULT()->image_cache_draw_buf_handlers)

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Формат данных для сжатых данных изображения.
 */

typedef struct _lv_image_compressed_t {
    uint32_t method: 4; /*Метод сжатия см. `lv_image_compress_t`.*/
    uint32_t reserved : 28;  /*Зарезервировано для использования позже*/
    uint32_t compressed_size;  /*Размер сжатых данных в байтах*/
    uint32_t decompressed_size;  /*Размер распакованных данных в байтах*/
    const uint8_t * data; /*Сжатые данные*/
} lv_image_compressed_t;

typedef struct {
    lv_fs_file_t * f;
    lv_color32_t * palette;
    lv_opa_t * opa;
    lv_image_compressed_t compressed;
    lv_draw_buf_t * decoded;            /*Буфер рисования для хранения декодированного изображения*/
    lv_draw_buf_t * decompressed;       /*Распакованные данные можно использовать напрямую, поэтому их также необходимо отрисовывать.*/
    lv_draw_buf_t c_array;              /*Изображение C-массива, которое необходимо преобразовать в буфер рисования.*/
    lv_draw_buf_t * decoded_partial;    /*Буфер отрисовки декодированного изображения через get_area_cb*/
} decoder_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static decoder_data_t * get_decoder_data(lv_image_decoder_dsc_t * dsc);
static void free_decoder_data(lv_image_decoder_dsc_t * dsc);
static lv_result_t decode_indexed(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static lv_result_t load_indexed(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
#if LV_BIN_DECODER_RAM_LOAD
    static lv_result_t decode_rgb(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
#endif
static lv_result_t decode_alpha_only(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static lv_result_t decode_indexed_line(lv_color_format_t color_format, const lv_color32_t * palette, int32_t x,
                                       int32_t w_px, const uint8_t * in, lv_color32_t * out);
static lv_result_t decode_compressed(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);

static lv_fs_res_t fs_read_file_at(lv_fs_file_t * f, uint32_t pos, void * buff, uint32_t btr, uint32_t * br);

static lv_result_t decompress_image(lv_image_decoder_dsc_t * dsc, const lv_image_compressed_t * compressed);

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
 * Инициализируйте модуль декодера двоичных изображений lvgl.
 */
void lv_bin_decoder_init(void)
{
    lv_image_decoder_t * decoder;

    decoder = lv_image_decoder_create();
    LV_ASSERT_MALLOC(decoder);
    if(decoder == NULL) {
        LV_LOG_WARN("Out of memory");
        return;
    }

    lv_image_decoder_set_info_cb(decoder, lv_bin_decoder_info);
    lv_image_decoder_set_open_cb(decoder, lv_bin_decoder_open);
    lv_image_decoder_set_get_area_cb(decoder, lv_bin_decoder_get_area);
    lv_image_decoder_set_close_cb(decoder, lv_bin_decoder_close);

    decoder->name = DECODER_NAME;
}

lv_result_t lv_bin_decoder_info(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc, lv_image_header_t * header)
{
    LV_UNUSED(decoder); /*Неиспользованный*/

    const void * src = dsc->src;
    lv_image_src_t src_type = dsc->src_type;

    if(src_type == LV_IMAGE_SRC_VARIABLE) {
        lv_image_dsc_t * image = (lv_image_dsc_t *)src;
        lv_memcpy(header, &image->header, sizeof(lv_image_header_t));
    }
    else if(src_type == LV_IMAGE_SRC_FILE) {
        /*Поддержка только файлов «*.bin».*/
        if(lv_strcmp(lv_fs_get_ext(src), "bin")) return LV_RESULT_INVALID;

        lv_fs_res_t res;
        uint32_t rn;
        res = lv_fs_read(&dsc->file, header, sizeof(lv_image_header_t), &rn);

        if(res != LV_FS_RES_OK || rn != sizeof(lv_image_header_t)) {
            LV_LOG_WARN("Read file header failed: %d with len: %" LV_PRIu32 ", expected: %zu", res, rn, sizeof(lv_image_header_t));
            return LV_RESULT_INVALID;
        }

        /**
         * @todo
         * Это временное решение обратной совместимости после добавления
         * магия в заголовке изображения.
         */
        if(header->magic != LV_IMAGE_HEADER_MAGIC) {
            LV_LOG_WARN("Legacy bin image detected: %s", (char *)src);
            header->cf = header->magic;
            header->magic = LV_IMAGE_HEADER_MAGIC;
        }

        /*Файл всегда читается в buf, поэтому данные могут быть изменены.*/
        header->flags |= LV_IMAGE_FLAGS_MODIFIABLE;
    }
    else if(src_type == LV_IMAGE_SRC_SYMBOL) {
        /*Размер зависит от шрифта, но здесь он неизвестен. Его следует обрабатывать вне
         *функция*/
        header->w = 1;
        header->h = 1;
        /*Символы всегда имеют прозрачные части. Важно, так как в розыгрыше проверяется обложка.
         *функция. Фактическое значение не имеет значения, потому что lv_draw_label нарисует его.*/
        header->cf = LV_COLOR_FORMAT_A8;
    }
    else {
        LV_LOG_WARN("Image get info found unknown src type");
        return LV_RESULT_INVALID;
    }

    if(header->cf == LV_COLOR_FORMAT_UNKNOWN) {
        LV_LOG_WARN("Image color format is unknown");
        return LV_RESULT_INVALID;
    }

    /*В целях обратной совместимости все изображения на данный момент не умножаются предварительно.*/
    if(header->magic != LV_IMAGE_HEADER_MAGIC) {
        header->flags &= ~LV_IMAGE_FLAGS_PREMULTIPLIED;
    }

    return LV_RESULT_OK;
}

/**
 * Декодировать изображение из двоичного файла
 * @param decoder pointer to the decoder
 * @param dsc     pointer to the decoder descriptor
 * @return LV_RESULT_OK: no error; LV_RESULT_INVALID: can't open the image
 */
lv_result_t lv_bin_decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);

    lv_result_t res = LV_RESULT_INVALID;
    lv_fs_res_t fs_res = LV_FS_RES_UNKNOWN;
    bool use_directly = false; /*Если изображение уже декодировано и его можно использовать напрямую*/

    /*Откройте файл, если это файл*/
    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        /*Поддержка только файлов «*.bin».*/
        if(lv_strcmp(lv_fs_get_ext(dsc->src), "bin")) return LV_RESULT_INVALID;

        /*Если файл был успешно открыт, сохраните дескриптор файла.*/
        decoder_data_t * decoder_data = get_decoder_data(dsc);
        if(decoder_data == NULL) {
            return LV_RESULT_INVALID;
        }

        dsc->user_data = decoder_data;
        lv_fs_file_t * f = lv_malloc(sizeof(*f));
        if(f == NULL) {
            free_decoder_data(dsc);
            return LV_RESULT_INVALID;
        }

        fs_res = lv_fs_open(f, dsc->src, LV_FS_MODE_RD);
        if(fs_res != LV_FS_RES_OK) {
            LV_LOG_WARN("Open file failed: %d", fs_res);
            lv_free(f);
            free_decoder_data(dsc);
            return LV_RESULT_INVALID;
        }

        decoder_data->f = f;    /*Теперь free_decoder_data позаботится о файле.*/

        lv_color_format_t cf = dsc->header.cf;

        if(dsc->header.flags & LV_IMAGE_FLAGS_COMPRESSED) {
            res = decode_compressed(decoder, dsc);
        }
        else if(LV_COLOR_FORMAT_IS_INDEXED(cf)) {
            if(dsc->args.use_indexed) {
                /*Палитра индексированного изображения и все изображение изображения A8 для простоты всегда загружаются в RAM.*/
                res = load_indexed(decoder, dsc);
            }
            else {
                res = decode_indexed(decoder, dsc);
            }
        }
        else if(LV_COLOR_FORMAT_IS_ALPHA_ONLY(cf)) {
            res = decode_alpha_only(decoder, dsc);
        }
#if LV_BIN_DECODER_RAM_LOAD
        else if(cf == LV_COLOR_FORMAT_ARGB8888      \
                || cf == LV_COLOR_FORMAT_XRGB8888   \
                || cf == LV_COLOR_FORMAT_RGB888     \
                || cf == LV_COLOR_FORMAT_RGB565     \
                || cf == LV_COLOR_FORMAT_RGB565_SWAPPED     \
                || cf == LV_COLOR_FORMAT_RGB565A8   \
                || cf == LV_COLOR_FORMAT_ARGB8565) {
            res = decode_rgb(decoder, dsc);
        }
#else
        else {
            /* декодируйте их в get_area_cb */
            res = LV_RESULT_OK;
        }
#endif
    }

    else if(dsc->src_type == LV_IMAGE_SRC_VARIABLE) {
        /*Переменные должны иметь действительные данные*/
        lv_image_dsc_t * image = (lv_image_dsc_t *)dsc->src;
        if(image->data == NULL) {
            return LV_RESULT_INVALID;
        }

        lv_color_format_t cf = image->header.cf;
        if(dsc->header.flags & LV_IMAGE_FLAGS_COMPRESSED) {
            res = decode_compressed(decoder, dsc);
        }
        else if(LV_COLOR_FORMAT_IS_INDEXED(cf)) {
            /*Нужны данные декодера для хранения преобразованного изображения*/
            decoder_data_t * decoder_data = get_decoder_data(dsc);
            if(decoder_data == NULL) {
                return LV_RESULT_INVALID;
            }

            if(dsc->args.use_indexed) {
                /*Палитра индексированного изображения и все изображение изображения A8 для простоты всегда загружаются в RAM.*/
                res = load_indexed(decoder, dsc);
                use_directly = true; /*Если модуль рисования поддерживает индексированное изображение, его можно использовать напрямую.*/
            }
            else {
                res = decode_indexed(decoder, dsc);
            }
        }
        else if(LV_COLOR_FORMAT_IS_ALPHA_ONLY(cf) && cf != LV_COLOR_FORMAT_A8) {
            /*Только альфа-изображению потребуются данные декодера для хранения указателя на декодированное изображение, чтобы освободить его при закрытии декодера.*/
            decoder_data_t * decoder_data = get_decoder_data(dsc);
            if(decoder_data == NULL) {
                return LV_RESULT_INVALID;
            }

            res = decode_alpha_only(decoder, dsc);
        }
        else {
            /*В случае несжатых форматов изображение сохраняется в ROM/RAM.
             *Поэтому просто дайте его указатель*/

            decoder_data_t * decoder_data = get_decoder_data(dsc);
            lv_draw_buf_t * decoded;
            if(image->header.flags & LV_IMAGE_FLAGS_ALLOCATED) {
                decoded = (lv_draw_buf_t *)image;
                res = LV_RESULT_OK;
            }
            else {
                decoded = &decoder_data->c_array;
                if(image->header.stride == 0) {
                    /*Если изображение не имеет четкости, рассматривайте его как устаревший формат изображения lvgl v8.*/
                    lv_image_dsc_t tmp = *image;
                    tmp.header.stride = (tmp.header.w * lv_color_format_get_bpp(cf) + 7) >> 3;
                    res = lv_draw_buf_from_image(decoded, &tmp);
                }
                else
                    res = lv_draw_buf_from_image(decoded, image);
            }

            if(res == LV_RESULT_OK) {
                dsc->decoded = decoded;

                if(decoded->header.stride == 0) {
                    /*Используйте автоматически рассчитанное значение из обратного вызова decoder_info.*/
                    decoded->header.stride = dsc->header.stride;
                }

                use_directly = true; /*Переменное изображение, которое можно использовать напрямую.*/
            }
        }
    }

    if(res != LV_RESULT_OK) {
        free_decoder_data(dsc);
        return res;
    }

    if(dsc->decoded == NULL) return LV_RESULT_OK; /*Нужно читать через get_area_cb*/

    lv_draw_buf_t * decoded = (lv_draw_buf_t *)dsc->decoded;
    if(dsc->header.flags & LV_IMAGE_FLAGS_PREMULTIPLIED) {
        lv_draw_buf_set_flag(decoded, LV_IMAGE_FLAGS_PREMULTIPLIED);
    }

    lv_draw_buf_t * adjusted = lv_image_decoder_post_process(dsc, decoded);
    if(adjusted == NULL) {
        free_decoder_data(dsc);
        return LV_RESULT_INVALID;
    }

    /*Скорректированный буфер отрисовки выделяется заново.*/
    if(adjusted != decoded) {
        use_directly = false; /*Невозможно использовать исходное изображение напрямую*/
        free_decoder_data(dsc);
        decoder_data_t * decoder_data = get_decoder_data(dsc);
        decoder_data->decoded = adjusted; /*Теперь этот новый буфер должен быть освобожден при закрытии декодера.*/
    }
    dsc->decoded = adjusted;

    /* Скопируйте пользовательские флаги в декодированное изображение */
    if(dsc->header.flags & LV_IMAGE_FLAGS_USER_MASK) {
        lv_draw_buf_set_flag((lv_draw_buf_t *)dsc->decoded, dsc->header.flags & LV_IMAGE_FLAGS_USER_MASK);
    }

    /*Не помещайте изображение в кеш, если его можно использовать напрямую.*/
    if(use_directly || dsc->args.no_cache) {
        if(dsc->args.flush_cache && use_directly) {
            dsc->args.flush_cache = false;
        }
        return LV_RESULT_OK;
    }

    /*Если кэш изображений отключен, просто верните декодированное изображение.*/
    if(!lv_image_cache_is_enabled()) return LV_RESULT_OK;

    /*Добавьте его в кеш*/
    lv_image_cache_data_t search_key;
    search_key.src_type = dsc->src_type;
    search_key.src = dsc->src;
    search_key.slot.size = dsc->decoded->data_size;

    lv_cache_entry_t * cache_entry = lv_image_decoder_add_to_cache(decoder, &search_key, dsc->decoded, dsc->user_data);
    if(cache_entry == NULL) {
        free_decoder_data(dsc);
        return LV_RESULT_INVALID;
    }
    dsc->cache_entry = cache_entry;
    decoder_data_t * decoder_data = get_decoder_data(dsc);
    decoder_data->decoded = NULL; /*Кэш об этом позаботится*/

    return LV_RESULT_OK;
}

void lv_bin_decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Неиспользованный*/

    decoder_data_t * decoder_data = dsc->user_data;
    if(decoder_data && decoder_data->decoded_partial) {
        lv_draw_buf_destroy(decoder_data->decoded_partial);
        decoder_data->decoded_partial = NULL;
    }

    free_decoder_data(dsc);
}

lv_result_t lv_bin_decoder_get_area(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc,
                                    const lv_area_t * full_area, lv_area_t * decoded_area)
{
    LV_UNUSED(decoder); /*Неиспользованный*/

    lv_color_format_t cf = dsc->header.cf;
    /*Проверьте, поддерживается ли cf*/

    bool supported = LV_COLOR_FORMAT_IS_INDEXED(cf)
                     || cf == LV_COLOR_FORMAT_ARGB8888  \
                     || cf == LV_COLOR_FORMAT_XRGB8888  \
                     || cf == LV_COLOR_FORMAT_RGB888    \
                     || cf == LV_COLOR_FORMAT_RGB565    \
                     || cf == LV_COLOR_FORMAT_RGB565_SWAPPED    \
                     || cf == LV_COLOR_FORMAT_ARGB8565  \
                     || cf == LV_COLOR_FORMAT_RGB565A8;
    if(!supported) {
        LV_LOG_WARN("CF: %d is not supported", cf);
        return LV_RESULT_INVALID;
    }

    lv_fs_res_t res = LV_FS_RES_UNKNOWN;
    decoder_data_t * decoder_data = dsc->user_data;
    if(decoder_data == NULL) {
        LV_LOG_ERROR("Unexpected null decoder data");
        return LV_RESULT_INVALID;
    }

    lv_fs_file_t * f = decoder_data->f;
    uint32_t bpp = lv_color_format_get_bpp(cf);
    int32_t w_px = lv_area_get_width(full_area);
    uint8_t * img_data = NULL;
    lv_draw_buf_t * decoded = NULL;
    uint32_t offset = dsc->src_type == LV_IMAGE_SRC_FILE ? sizeof(lv_image_header_t) : 0;   /*Пропустить заголовок изображения*/

    /*На данный момент мы поддерживаем только построчное чтение.*/
    if(decoded_area->y1 == LV_COORD_MIN) {
        /*Проиндексированное изображение конвертируется в ARGB888.*/
        lv_color_format_t cf_decoded = LV_COLOR_FORMAT_IS_INDEXED(cf) ? LV_COLOR_FORMAT_ARGB8888 : cf;

        decoded = lv_draw_buf_reshape(decoder_data->decoded_partial, cf_decoded, w_px, 1, LV_STRIDE_AUTO);
        if(decoded == NULL) {
            if(decoder_data->decoded_partial != NULL) {
                lv_draw_buf_destroy(decoder_data->decoded_partial);
                decoder_data->decoded_partial = NULL;
            }
            decoded = lv_draw_buf_create_ex(image_cache_draw_buf_handlers, w_px, 1, cf_decoded, LV_STRIDE_AUTO);
            if(decoded == NULL) return LV_RESULT_INVALID;
            decoder_data->decoded_partial = decoded; /*Бесплатно на декодере закрыть*/
        }
        *decoded_area = *full_area;
        decoded_area->y2 = decoded_area->y1;
    }
    else {
        decoded_area->y1++;
        decoded_area->y2++;
        decoded = decoder_data->decoded_partial; /*Уже выделено*/
    }

    img_data = decoded->data; /*Получить буфер для работы*/

    if(decoded_area->y1 > full_area->y2) {
        return LV_RESULT_INVALID;
    }

    if(LV_COLOR_FORMAT_IS_INDEXED(cf)) {
        int32_t x_fraction = decoded_area->x1 % (8 / bpp);
        uint32_t len = (w_px * bpp + 7) / 8 + 1; /*10 пикселей для 1 бит на пиксель может быть в 3 байта*/
        uint8_t * buf = NULL;

        offset += dsc->palette_size * 4; /*Пропустить палитру*/
        offset += decoded_area->y1 * dsc->header.stride;
        offset += decoded_area->x1 * bpp / 8; /*Переместиться в x1*/
        if(dsc->src_type == LV_IMAGE_SRC_FILE) {
            buf = lv_malloc(len);
            LV_ASSERT_NULL(buf);
            if(buf == NULL)
                return LV_RESULT_INVALID;

            res = fs_read_file_at(f, offset, buf, len, NULL);
            if(res != LV_FS_RES_OK) {
                lv_free(buf);
                return LV_RESULT_INVALID;
            }
        }
        else {
            const lv_image_dsc_t * image = dsc->src;
            buf = (void *)(image->data + offset);
        }

        decode_indexed_line(cf, dsc->palette, x_fraction, w_px, buf, (lv_color32_t *)img_data);

        if(dsc->src_type == LV_IMAGE_SRC_FILE) lv_free((void *)buf);

        dsc->decoded = decoded; /*Вернуть декодированное изображение*/
        return LV_RESULT_OK;
    }

    if(cf == LV_COLOR_FORMAT_ARGB8888 || cf == LV_COLOR_FORMAT_XRGB8888 || cf == LV_COLOR_FORMAT_RGB888
       || cf == LV_COLOR_FORMAT_RGB565 || cf == LV_COLOR_FORMAT_RGB565_SWAPPED || cf == LV_COLOR_FORMAT_ARGB8565) {
        uint32_t len = (w_px * bpp) / 8;
        offset += decoded_area->y1 * dsc->header.stride;
        offset += decoded_area->x1 * bpp / 8; /*Переместиться в x1*/
        res = fs_read_file_at(f, offset, img_data, len, NULL);
        if(res != LV_FS_RES_OK) {
            return LV_RESULT_INVALID;
        }

        dsc->decoded = decoded; /*Вернуть декодированное изображение*/
        return LV_RESULT_OK;
    }

    if(cf == LV_COLOR_FORMAT_RGB565A8) {
        bpp = 16; /* Маска RGB565 + A8*/
        uint32_t len = decoded->header.stride;
        offset += decoded_area->y1 * dsc->header.stride; /*Переехать в y1*/
        offset += decoded_area->x1 * bpp / 8; /*Переместиться в x1*/
        res = fs_read_file_at(f, offset, img_data, len, NULL);
        if(res != LV_FS_RES_OK) {
            return LV_RESULT_INVALID;
        }

        /*Теперь маска A8*/
        offset = sizeof(lv_image_header_t);
        offset += dsc->header.h * dsc->header.stride; /*Перейти на карту A8*/
        offset += decoded_area->y1 * (dsc->header.stride / 2); /*Переехать в y1*/
        offset += decoded_area->x1 * 1; /*Переместиться в x1*/
        res = fs_read_file_at(f, offset, img_data + len, w_px * 1, NULL);
        if(res != LV_FS_RES_OK) {
            return LV_RESULT_INVALID;
        }

        dsc->decoded = decoded; /*Вернуть декодированное изображение*/
        return LV_RESULT_OK;
    }

    return LV_RESULT_INVALID;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static decoder_data_t * get_decoder_data(lv_image_decoder_dsc_t * dsc)
{
    decoder_data_t * data = dsc->user_data;
    if(data == NULL) {
        data = lv_malloc_zeroed(sizeof(decoder_data_t));
        LV_ASSERT_MALLOC(data);
        if(data == NULL) {
            LV_LOG_ERROR("Out of memory");
            return NULL;
        }

        dsc->user_data = data;
    }

    return data;
}

static void free_decoder_data(lv_image_decoder_dsc_t * dsc)
{
    decoder_data_t * decoder_data = dsc->user_data;
    if(decoder_data == NULL) return;

    if(decoder_data->f) {
        lv_fs_close(decoder_data->f);
        lv_free(decoder_data->f);
    }

    if(decoder_data->decoded) lv_draw_buf_destroy(decoder_data->decoded);
    if(decoder_data->decompressed) lv_draw_buf_destroy(decoder_data->decompressed);
    lv_free(decoder_data->palette);
    lv_free(decoder_data);
    dsc->user_data = NULL;
}

static lv_result_t decode_indexed(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Неиспользованный*/
    lv_fs_res_t res;
    uint32_t rn;
    decoder_data_t * decoder_data = dsc->user_data;
    lv_fs_file_t * f = decoder_data->f;
    lv_color_format_t cf = dsc->header.cf;
    uint32_t palette_len = sizeof(lv_color32_t) * LV_COLOR_INDEXED_PALETTE_SIZE(cf);
    const lv_color32_t * palette;
    const uint8_t * indexed_data = NULL;
    lv_draw_buf_t * draw_buf_indexed = NULL;
    uint32_t stride = dsc->header.stride;

    bool is_compressed = dsc->header.flags & LV_IMAGE_FLAGS_COMPRESSED;
    if(is_compressed) {
        uint8_t * data = decoder_data->decompressed->data;
        palette = (lv_color32_t *)data;
        indexed_data = data + palette_len;
    }
    else if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        /*прочитать палитру для индексированного изображения*/
        palette = lv_malloc(palette_len);
        LV_ASSERT_MALLOC(palette);
        if(palette == NULL) {
            LV_LOG_ERROR("Out of memory");
            return LV_RESULT_INVALID;
        }

        res = fs_read_file_at(f, sizeof(lv_image_header_t), (uint8_t *)palette, palette_len, &rn);
        if(res != LV_FS_RES_OK || rn != palette_len) {
            LV_LOG_WARN("Read palette failed: %d, with len: %" LV_PRIu32 ", expected: %" LV_PRIu32, res, rn, palette_len);
            lv_free((void *)palette);
            return LV_RESULT_INVALID;
        }

        decoder_data->palette = (void *)palette; /*Необходимо освободить, когда декодер закрывается*/

#if LV_BIN_DECODER_RAM_LOAD
        draw_buf_indexed = lv_draw_buf_create_ex(image_cache_draw_buf_handlers, dsc->header.w, dsc->header.h, cf,
                                                 dsc->header.stride);
        if(draw_buf_indexed == NULL) {
            LV_LOG_ERROR("Draw buffer alloc failed");
            goto exit_with_buf;
        }

        indexed_data = draw_buf_indexed->data;

        uint32_t data_len = 0;
        if(lv_fs_seek(f, 0, LV_FS_SEEK_END) != LV_FS_RES_OK ||
           lv_fs_tell(f, &data_len) != LV_FS_RES_OK) {
            LV_LOG_WARN("Failed to get file to size");
            goto exit_with_buf;
        }

        uint32_t data_offset = sizeof(lv_image_header_t) + palette_len;
        data_len -= data_offset;
        res = fs_read_file_at(f, data_offset, (uint8_t *)indexed_data, data_len, &rn);
        if(res != LV_FS_RES_OK || rn != data_len) {
            LV_LOG_WARN("Read indexed image failed: %d, with len: %" LV_PRIu32 ", expected: %" LV_PRIu32, res, rn, data_len);
            goto exit_with_buf;
        }
#endif
    }
    else if(dsc->src_type == LV_IMAGE_SRC_VARIABLE) {
        lv_image_dsc_t * image = (lv_image_dsc_t *)dsc->src;
        palette = (lv_color32_t *)image->data;
        indexed_data = image->data + palette_len;
    }
    else {
        return LV_RESULT_INVALID;
    }

    dsc->palette = palette;
    dsc->palette_size = LV_COLOR_INDEXED_PALETTE_SIZE(cf);

#if LV_BIN_DECODER_RAM_LOAD
    /*Конвертируйте в ARGB8888 , поскольку средство визуализации sw не может визуализировать его напрямую, даже если оно находится в RAM.*/
    lv_draw_buf_t * decoded = lv_draw_buf_create_ex(image_cache_draw_buf_handlers, dsc->header.w, dsc->header.h,
                                                    LV_COLOR_FORMAT_ARGB8888,
                                                    0);
    if(decoded == NULL) {
        LV_LOG_ERROR("No memory for indexed image");
        goto exit_with_buf;
    }

    stride = decoded->header.stride;
    uint8_t * img_data = decoded->data;

    const uint8_t * in = indexed_data;
    uint8_t * out = img_data;
    for(uint32_t y = 0; y < dsc->header.h; y++) {
        decode_indexed_line(cf, dsc->palette, 0, dsc->header.w, in, (lv_color32_t *)out);
        in += dsc->header.stride;
        out += stride;
    }

    dsc->decoded = decoded;
    decoder_data->decoded = decoded; /*Бесплатно при закрытии декодера*/
    if(dsc->src_type == LV_IMAGE_SRC_FILE && !is_compressed) {
        decoder_data->palette = (void *)palette; /*Бесплатные данные декодера при закрытии*/
        lv_draw_buf_destroy(draw_buf_indexed);
    }

    return LV_RESULT_OK;
exit_with_buf:
    if(dsc->src_type == LV_IMAGE_SRC_FILE && !is_compressed) {
        lv_free((void *)palette);
        decoder_data->palette = NULL;
    }

    if(draw_buf_indexed) lv_draw_buf_destroy(draw_buf_indexed);
    return LV_RESULT_INVALID;
#else
    LV_UNUSED(stride);
    LV_UNUSED(indexed_data);
    LV_UNUSED(draw_buf_indexed);
    /*get_area_cb должен прочитать его позже.*/
    return LV_RESULT_OK;
#endif
}

static lv_result_t load_indexed(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
#if LV_BIN_DECODER_RAM_LOAD == 0
    LV_UNUSED(decoder); /*Неиспользованный*/
    LV_UNUSED(dsc); /*Неиспользованный*/
    LV_LOG_ERROR("LV_BIN_DECODER_RAM_LOAD is disabled");
    return LV_RESULT_INVALID;
#else

    LV_UNUSED(decoder); /*Неиспользованный*/

    lv_fs_res_t res;
    uint32_t rn;
    decoder_data_t * decoder_data = dsc->user_data;

    if(dsc->header.flags & LV_IMAGE_FLAGS_COMPRESSED) {
        /*Распакованный образ уже загружен в RAM.*/
        dsc->decoded = decoder_data->decompressed;

        /*Передайте право собственности на декодированный указатель, потому что это конечные данные, которые мы используем.*/
        decoder_data->decoded = decoder_data->decompressed;
        decoder_data->decompressed = NULL;
        return LV_RESULT_OK;
    }

    if(dsc->src_type == LV_IMAGE_SRC_VARIABLE) {
        lv_image_dsc_t * image = (lv_image_dsc_t *)dsc->src;
        lv_draw_buf_t * decoded;
        if(image->header.flags & LV_IMAGE_FLAGS_ALLOCATED) {
            decoded = (lv_draw_buf_t *)image;
        }
        else {
            decoded = &decoder_data->c_array;
            lv_result_t result = lv_draw_buf_from_image(decoded, image);
            if(result != LV_RESULT_OK) {
                return result;
            }
        }

        dsc->decoded = decoded;

        if(decoded->header.stride == 0) {
            /*Используйте автоматически рассчитанное значение из обратного вызова decoder_info.*/
            decoded->header.stride = dsc->header.stride;
        }

        return LV_RESULT_OK;
    }

    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        lv_color_format_t cf = dsc->header.cf;
        lv_fs_file_t * f = decoder_data->f;
        lv_draw_buf_t * decoded = lv_draw_buf_create_ex(image_cache_draw_buf_handlers, dsc->header.w, dsc->header.h, cf,
                                                        dsc->header.stride);
        if(decoded == NULL) {
            LV_LOG_ERROR("Draw buffer alloc failed");
            return LV_RESULT_INVALID;
        }

        uint8_t * data = decoded->data;
        uint32_t palette_len = sizeof(lv_color32_t) * LV_COLOR_INDEXED_PALETTE_SIZE(cf);
        res = fs_read_file_at(f, sizeof(lv_image_header_t), data, palette_len, &rn);
        if(res != LV_FS_RES_OK || rn != palette_len) {
            LV_LOG_WARN("Read palette failed: %d, with len: %" LV_PRIu32 ", expected: %" LV_PRIu32, res, rn, palette_len);
            lv_draw_buf_destroy(decoded);
            return LV_RESULT_INVALID;
        }

        uint32_t data_len = 0;
        if(lv_fs_seek(f, 0, LV_FS_SEEK_END) != LV_FS_RES_OK ||
           lv_fs_tell(f, &data_len) != LV_FS_RES_OK) {
            LV_LOG_WARN("Failed to get file to size");
            lv_draw_buf_destroy(decoded);
            return LV_RESULT_INVALID;
        }

        uint32_t data_offset = sizeof(lv_image_header_t) + palette_len;
        data_len -= data_offset;
        data += palette_len;
        res = fs_read_file_at(f, data_offset, data, data_len, &rn);
        if(res != LV_FS_RES_OK || rn != data_len) {
            LV_LOG_WARN("Read indexed image failed: %d, with len: %" LV_PRIu32 ", expected: %" LV_PRIu32, res, rn, data_len);
            lv_draw_buf_destroy(decoded);
            return LV_RESULT_INVALID;
        }

        decoder_data->decoded = decoded;
        dsc->decoded = decoded;
        return LV_RESULT_OK;
    }

    LV_LOG_ERROR("Unknown src type: %d", dsc->src_type);
    return LV_RESULT_INVALID;
#endif
}

#if LV_BIN_DECODER_RAM_LOAD
static lv_result_t decode_rgb(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);
    lv_fs_res_t res;
    decoder_data_t * decoder_data = dsc->user_data;
    lv_fs_file_t * f = decoder_data->f;
    lv_color_format_t cf = dsc->header.cf;

    uint32_t len = dsc->header.stride * dsc->header.h;
    if(cf == LV_COLOR_FORMAT_RGB565A8) {
        len += (dsc->header.stride / 2) * dsc->header.h; /*Маска A8*/
    }

    lv_draw_buf_t * decoded = lv_draw_buf_create_ex(image_cache_draw_buf_handlers, dsc->header.w, dsc->header.h, cf,
                                                    dsc->header.stride);
    if(decoded == NULL) {
        LV_LOG_ERROR("No memory for rgb file read");
        return LV_RESULT_INVALID;
    }

    uint8_t * img_data = decoded->data;

    uint32_t rn;
    res = fs_read_file_at(f, sizeof(lv_image_header_t), img_data, len, &rn);
    if(res != LV_FS_RES_OK || rn != len) {
        LV_LOG_WARN("Read rgb file failed: %d, with len: %" LV_PRIu32 ", expected: %" LV_PRIu32, res, rn, len);
        lv_draw_buf_destroy(decoded);
        return LV_RESULT_INVALID;
    }

    dsc->decoded = decoded;
    decoder_data->decoded = decoded; /*Бесплатно при закрытии декодера*/
    return LV_RESULT_OK;
}
#endif

/**
 * Расширьте A1 /2/4 до A8 с помощью интерполяции, чтобы уменьшить ошибку округления.
 */
static inline uint8_t bit_extend(uint8_t value, uint8_t bpp)
{
    if(value == 0) return 0;

    uint8_t res = value;
    uint8_t bpp_now = bpp;
    while(bpp_now < 8) {
        res |= value << (8 - bpp_now);
        bpp_now += bpp;
    };

    return res;
}

static lv_result_t decode_alpha_only(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);
    lv_fs_res_t res;
    uint32_t rn;
    decoder_data_t * decoder_data = dsc->user_data;
    uint8_t bpp = lv_color_format_get_bpp(dsc->header.cf);

    if(bpp == 0) {
        LV_LOG_ERROR("Error color format: %d", dsc->header.cf);
        return LV_RESULT_INVALID;
    }

    uint32_t w = (dsc->header.stride * 8) / bpp;
    uint32_t buf_stride = (w * 8 + 7) >> 3; /*шаг за img_data*/
    uint32_t buf_len = w * dsc->header.h; /*всегда декодировать в формат A8*/
    lv_draw_buf_t * decoded;
    uint32_t file_len = (uint32_t)dsc->header.stride * dsc->header.h;

    decoded = lv_draw_buf_create_ex(image_cache_draw_buf_handlers, dsc->header.w, dsc->header.h, LV_COLOR_FORMAT_A8,
                                    buf_stride);
    if(decoded == NULL) {
        LV_LOG_ERROR("Out of memory");
        return LV_RESULT_INVALID;
    }

    uint8_t * img_data = decoded->data;

    if(dsc->header.flags & LV_IMAGE_FLAGS_COMPRESSED) {
        /*Копировать данные изображения*/
        lv_memcpy(img_data, decoder_data->decompressed->data, file_len);
    }
    else if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        res = fs_read_file_at(decoder_data->f, sizeof(lv_image_header_t), img_data, file_len, &rn);
        if(res != LV_FS_RES_OK || rn != file_len) {
            LV_LOG_WARN("Read header failed: %d, with len: %" LV_PRIu32 ", expected: %" LV_PRIu32, res, rn, file_len);
            lv_draw_buf_destroy(decoded);
            return LV_RESULT_INVALID;
        }
    }
    else if(dsc->src_type == LV_IMAGE_SRC_VARIABLE) {
        /*Копировать данные изображения*/
        lv_memcpy(img_data, ((lv_image_dsc_t *)dsc->src)->data, file_len);
    }

    if(dsc->header.cf != LV_COLOR_FORMAT_A8) {
        /*Преобразование A1/2/4 в A8 из последнего пикселя в первый пиксель.*/
        uint8_t * in = img_data + file_len - 1;
        uint8_t * out = img_data + buf_len - 1;
        uint8_t mask = (1 << bpp) - 1;
        uint8_t shift = 0;
        for(uint32_t i = 0; i < buf_len; i++) {
            /**
             * Ошибка округления:
             * Возьмем, к примеру, bpp = 4, альфа-значение от 0x0 до 0x0F должно быть
             * сопоставлен с 0x00 до 0xFF. Используя приведенное ниже уравнение, мы получим от 0x00 до 0xF0.
             * таким образом вызывает ошибку. Мы можем просто интерполировать значение, чтобы исправить это.
             *
             * Equation: *out = ((*in >> shift) & mask) << (8 - bpp);
             * Ideal: *out = ((*in >> shift) & mask) * 255 / ((1L << bpp) - 1)
             */
            uint8_t value = ((*in >> shift) & mask);
            *out = bit_extend(value, bpp);
            shift += bpp;
            if(shift >= 8) {
                shift = 0;
                in--;
            }
            out--;
        }
    }

    decoder_data->decoded = decoded;
    dsc->decoded = decoded;
    return LV_RESULT_OK;
}

static lv_result_t decode_compressed(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
#if LV_BIN_DECODER_RAM_LOAD
    uint32_t rn;
    uint32_t len;
    uint32_t compressed_len;
    decoder_data_t * decoder_data = get_decoder_data(dsc);
    lv_result_t res;
    lv_fs_res_t fs_res;
    uint8_t * file_buf = NULL;
    lv_image_compressed_t * compressed = &decoder_data->compressed;

    lv_memzero(compressed, sizeof(lv_image_compressed_t));

    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        lv_fs_file_t * f = decoder_data->f;

        if(lv_fs_seek(f, 0, LV_FS_SEEK_END) != LV_FS_RES_OK ||
           lv_fs_tell(f, &compressed_len) != LV_FS_RES_OK) {
            LV_LOG_WARN("Failed to get compressed file len");
            return LV_RESULT_INVALID;
        }

        compressed_len -= sizeof(lv_image_header_t);
        compressed_len -= 12;

        /*Чтение заголовка сжатия*/
        len = 12;
        fs_res = fs_read_file_at(f, sizeof(lv_image_header_t), compressed, len, &rn);
        if(fs_res != LV_FS_RES_OK || rn != len) {
            LV_LOG_WARN("Read compressed header failed: %d, with len: %" LV_PRIu32 ", expected: %" LV_PRIu32, fs_res, rn, len);
            return LV_RESULT_INVALID;
        }

        if(compressed->compressed_size != compressed_len) {
            LV_LOG_WARN("Compressed size mismatch: %" LV_PRIu32" != %" LV_PRIu32, compressed->compressed_size, compressed_len);
            return LV_RESULT_INVALID;
        }

        file_buf = lv_malloc(compressed_len);
        if(file_buf == NULL) {
            LV_LOG_WARN("No memory for compressed file");
            return LV_RESULT_INVALID;

        }

        /*Продолжайте читать сжатые данные после заголовка сжатия.*/
        fs_res = lv_fs_read(f, file_buf, compressed_len, &rn);
        if(fs_res != LV_FS_RES_OK || rn != compressed_len) {
            LV_LOG_WARN("Read compressed file failed: %d, with len: %" LV_PRIu32 ", expected: %" LV_PRIu32, fs_res, rn,
                        compressed_len);
            lv_free(file_buf);
            return LV_RESULT_INVALID;
        }

        /*Распаковать изображение*/
        compressed->data = file_buf;
    }
    else if(dsc->src_type == LV_IMAGE_SRC_VARIABLE) {
        lv_image_dsc_t * image = (lv_image_dsc_t *)dsc->src;
        compressed_len = image->data_size;

        /*Чтение заголовка сжатия*/
        len = 12;
        compressed_len -= len;
        lv_memcpy(compressed, image->data, len);
        compressed->data = image->data + len;
        if(compressed->compressed_size != compressed_len) {
            LV_LOG_WARN("Compressed size mismatch: %" LV_PRIu32" != %" LV_PRIu32, compressed->compressed_size, compressed_len);
            return LV_RESULT_INVALID;
        }
    }
    else {
        LV_LOG_WARN("Compressed image only support file or variable");
        return LV_RESULT_INVALID;
    }

    res = decompress_image(dsc, compressed);
    compressed->data = NULL; /*Больше не нужно хранить данные*/
    lv_free(file_buf);
    if(res != LV_RESULT_OK) {
        LV_LOG_WARN("Decompress failed");
        return LV_RESULT_INVALID;
    }

    /*Зависит от cf, необходимо дополнительно декодировать изображение, например изображение C-массива.*/
    lv_image_dsc_t * image = (lv_image_dsc_t *)dsc->src;
    if(dsc->src_type == LV_IMAGE_SRC_VARIABLE && image->data == NULL) {
        return LV_RESULT_INVALID;
    }

    lv_color_format_t cf = dsc->header.cf;
    if(LV_COLOR_FORMAT_IS_INDEXED(cf)) {
        if(dsc->args.use_indexed) res = load_indexed(decoder, dsc);
        else res = decode_indexed(decoder, dsc);
    }
    else if(LV_COLOR_FORMAT_IS_ALPHA_ONLY(cf)) {
        res = decode_alpha_only(decoder, dsc);
    }
    else {
        /*Распакованные данные представляют собой исходные данные изображения.*/
        dsc->decoded = decoder_data->decompressed;

        /*Перенесите право собственности на распакованный файл в `decoded`, поскольку его можно использовать напрямую.*/
        decoder_data->decoded = decoder_data->decompressed;
        decoder_data->decompressed = NULL;
        res = LV_RESULT_OK;
    }

    return res;
#else
    LV_UNUSED(decompress_image);
    LV_UNUSED(decoder);
    LV_UNUSED(dsc);
    LV_LOG_ERROR("Need LV_BIN_DECODER_RAM_LOAD to be enabled");
    return LV_RESULT_INVALID;
#endif
}

static lv_result_t decode_indexed_line(lv_color_format_t color_format, const lv_color32_t * palette, int32_t x,
                                       int32_t w_px, const uint8_t * in, lv_color32_t * out)
{
    uint8_t px_size;
    uint16_t mask;

    int8_t shift   = 0;
    switch(color_format) {
        case LV_COLOR_FORMAT_I1:
            px_size = 1;
            in += x / 8;                /*8 пикселей на байт*/
            shift = 7 - (x & 0x7);
            break;
        case LV_COLOR_FORMAT_I2:
            px_size = 2;
            in += x / 4;                /*4 пикселя на байт*/
            shift = 6 - 2 * (x & 0x3);
            break;
        case LV_COLOR_FORMAT_I4:
            px_size = 4;
            in += x / 2;                /*2 пикселя на байт*/
            shift = 4 - 4 * (x & 0x1);
            break;
        case LV_COLOR_FORMAT_I8:
            px_size = 8;
            in += x;
            shift = 0;
            break;
        default:
            return LV_RESULT_INVALID;
    }

    mask   = (1 << px_size) - 1; /*Например.  px_size = 2; маска = 0x03*/

    int32_t i;
    for(i = 0; i < w_px; i++) {
        uint8_t val_act = (*in >> shift) & mask;
        out[i] = palette[val_act];

        shift -= px_size;
        if(shift < 0) {
            shift = 8 - px_size;
            in++;
        }
    }
    return LV_RESULT_OK;
}

static lv_fs_res_t fs_read_file_at(lv_fs_file_t * f, uint32_t pos, void * buff, uint32_t btr, uint32_t * br)
{
    lv_fs_res_t res;
    if(br) *br = 0;

    res = lv_fs_seek(f, pos, LV_FS_SEEK_SET);
    if(res != LV_FS_RES_OK) {
        return res;
    }

    res |= lv_fs_read(f, buff, btr, br);
    if(res != LV_FS_RES_OK) {
        return res;
    }

    return LV_FS_RES_OK;
}

static lv_result_t decompress_image(lv_image_decoder_dsc_t * dsc, const lv_image_compressed_t * compressed)
{
    /* Должен быть включен хотя бы один метод сжатия. */
#if (LV_USE_LZ4 || LV_USE_RLE)
    /* Проверьте, включен ли и действителен ли метод декомпрессии. */
    if(compressed->method == LV_IMAGE_COMPRESS_RLE) {
#if !LV_USE_RLE
        LV_LOG_WARN("RLE decompression is not enabled");
        return LV_RESULT_INVALID;
#endif
    }
    else if(compressed->method == LV_IMAGE_COMPRESS_LZ4) {
#if !LV_USE_LZ4
        LV_LOG_WARN("LZ4 decompression is not enabled");
        return LV_RESULT_INVALID;
#endif
    }
    else {
        LV_LOG_WARN("Unknown compression method: %" LV_PRIu32, compressed->method);
        return LV_RESULT_INVALID;
    }

    /*Необходимо сохранить распакованные данные в декодере, чтобы освободить их при закрытии.*/
    decoder_data_t * decoder_data = get_decoder_data(dsc);
    if(decoder_data == NULL) {
        return LV_RESULT_INVALID;
    }

    uint8_t * img_data;
    uint32_t out_len = compressed->decompressed_size;
    uint32_t input_len = compressed->compressed_size;
    uint32_t len = 0;

    lv_draw_buf_t * decompressed = lv_draw_buf_create_ex(image_cache_draw_buf_handlers, dsc->header.w, dsc->header.h,
                                                         dsc->header.cf,
                                                         dsc->header.stride);
    if(decompressed == NULL) {
        LV_LOG_WARN("No memory for decompressed image, input: %" LV_PRIu32 ", output: %" LV_PRIu32, input_len, out_len);
        return LV_RESULT_INVALID;
    }

    if(out_len > decompressed->data_size) {
        LV_LOG_ERROR("Decompressed size mismatch: %" LV_PRIu32 " > %" LV_PRIu32, out_len, decompressed->data_size);
        lv_draw_buf_destroy(decompressed);
        return LV_RESULT_INVALID;
    }

    img_data = decompressed->data;

    if(compressed->method == LV_IMAGE_COMPRESS_RLE) {
#if LV_USE_RLE
        /*Сжатие всегда происходит побайтно*/
        uint32_t pixel_byte;
        if(dsc->header.cf == LV_COLOR_FORMAT_RGB565A8)
            pixel_byte = 2;
        else
            pixel_byte = (lv_color_format_get_bpp(dsc->header.cf) + 7) >> 3;

        const uint8_t * input = compressed->data;
        uint8_t * output = img_data;

        len = lv_rle_decompress(input, input_len, output, out_len, pixel_byte);
#endif /* LV_USE_RLE */
    }
    else if(compressed->method == LV_IMAGE_COMPRESS_LZ4) {
#if LV_USE_LZ4
        const char * input = (const char *)compressed->data;
        char * output = (char *)img_data;

        int ret = LZ4_decompress_safe(input, output, (int)input_len, (int)out_len);
        if(ret >= 0) {
            /* Приведение безопасно из-за вышеуказанной проверки */
            len = (uint32_t)ret;
        }
#endif /* LV_USE_LZ4 */
    }

    if(len != compressed->decompressed_size) {
        LV_LOG_WARN("Decompress failed: %" LV_PRIu32 ", got: %" LV_PRIu32, out_len, len);
        lv_draw_buf_destroy(decompressed);
        return LV_RESULT_INVALID;
    }

    decoder_data->decompressed = decompressed; /*Бесплатно на декодере закрыть*/
    return LV_RESULT_OK;
#else
    LV_UNUSED(dsc);
    LV_UNUSED(compressed);
    LV_LOG_WARN("At least one compression method must be enabled");
    return LV_RESULT_INVALID;
#endif /* (LV_USE_LZ4 || LV_USE_RLE) */
}
