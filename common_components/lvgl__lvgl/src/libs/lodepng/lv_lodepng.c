/**
 * @file lv_lodepng.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../draw/lv_image_decoder_private.h"
#include "../../../lvgl.h"
#include "../../core/lv_global.h"
#if LV_USE_LODEPNG

#include "lv_lodepng.h"
#include "lodepng.h"
#include <stdlib.h>

/*********************
 *      DEFINES
 *********************/

#define DECODER_NAME    "LODEPNG"

#define image_cache_draw_buf_handlers &(LV_GLOBAL_DEFAULT()->image_cache_draw_buf_handlers)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_result_t decoder_info(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * src, lv_image_header_t * header);
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static void decoder_close(lv_image_decoder_t * dec, lv_image_decoder_dsc_t * dsc);
static void convert_color_depth(uint8_t * img_p, uint32_t px_cnt);
static lv_draw_buf_t * decode_png_data(const void * png_data, size_t png_data_size);
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
 * Зарегистрируйте функции декодера PNG в LVGL.
 */
void lv_lodepng_init(void)
{
    lv_image_decoder_t * dec = lv_image_decoder_create();
    lv_image_decoder_set_info_cb(dec, decoder_info);
    lv_image_decoder_set_open_cb(dec, decoder_open);
    lv_image_decoder_set_close_cb(dec, decoder_close);

    dec->name = DECODER_NAME;
}

void lv_lodepng_deinit(void)
{
    lv_image_decoder_t * dec = NULL;
    while((dec = lv_image_decoder_get_next(dec)) != NULL) {
        if(dec->info_cb == decoder_info) {
            lv_image_decoder_delete(dec);
            break;
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Получить информацию об изображении PNG
 * @param decoder   указатель на декодер, которому принадлежит эта функция
 * @param dsc       Дескриптор изображения, содержащий источник и тип изображения, а также другую информацию.
 * @param header    информация об изображении задается в параметре заголовка
 * @return          LV_RESULT_OK: нет ошибок;  LV_RESULT_INVALID: не могу получить информацию
 */
static lv_result_t decoder_info(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc, lv_image_header_t * header)
{
    LV_UNUSED(decoder); /*Неиспользованный*/

    lv_image_src_t src_type = dsc->src_type;          /*Получить тип источника*/

    if(src_type == LV_IMAGE_SRC_FILE || src_type == LV_IMAGE_SRC_VARIABLE) {
        uint32_t * size;
        static const uint8_t magic[] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
        uint8_t buf[24];

        /*Если это файл PNG...*/
        if(src_type == LV_IMAGE_SRC_FILE) {
            /* Прочтите ширину и высоту из файла. Они имеют постоянное местонахождение:
            * [16..19]: ширина
            * [20..23]: высота
            */
            uint32_t rn;
            lv_fs_read(&dsc->file, buf, sizeof(buf), &rn);

            if(rn != sizeof(buf)) return LV_RESULT_INVALID;

            if(lv_memcmp(buf, magic, sizeof(magic)) != 0) return LV_RESULT_INVALID;

            size = (uint32_t *)&buf[16];
        }
        /*Если это файл PNG в массиве C...*/
        else {
            const lv_image_dsc_t * img_dsc = dsc->src;
            const uint32_t data_size = img_dsc->data_size;
            size = ((uint32_t *)img_dsc->data) + 4;

            if(data_size < sizeof(magic)) return LV_RESULT_INVALID;
            if(lv_memcmp(img_dsc->data, magic, sizeof(magic)) != 0) return LV_RESULT_INVALID;
        }

        /*Сохраняем данные в шапке*/
        header->cf = LV_COLOR_FORMAT_ARGB8888;
        /*Ширина и высота хранятся в формате с прямым порядком байтов, поэтому преобразуйте их в формат с прямым порядком байтов.*/
        header->w = (int32_t)((size[0] & 0xff000000) >> 24) + ((size[0] & 0x00ff0000) >> 8);
        header->h = (int32_t)((size[1] & 0xff000000) >> 24) + ((size[1] & 0x00ff0000) >> 8);

        return LV_RESULT_OK;
    }

    return LV_RESULT_INVALID;         /*Если раньше это не удалось, то это ошибка*/
}

/**
 * Откройте изображениеPNGи декодируйте его в dsc.decoded.
 * @param decoder   указатель на декодер, которому принадлежит эта функция
 * @param dsc       дескриптор декодированного изображения
 * @return          LV_RESULT_OK: нет ошибок;  LV_RESULT_INVALID: не могу открыть изображение
 */
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);
    LV_PROFILER_DECODER_BEGIN_TAG("lv_lodepng_decoder_open");

    const uint8_t * png_data = NULL;
    size_t png_data_size = 0;
    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        const char * fn = dsc->src;

        /*Загрузите файл*/
        unsigned error = lodepng_load_file((void *)&png_data, &png_data_size, fn);
        if(error) {
            if(png_data != NULL) {
                lv_free((void *)png_data);
            }
            LV_LOG_WARN("error %u: %s\n", error, lodepng_error_text(error));
            LV_PROFILER_DECODER_END_TAG("lv_lodepng_decoder_open");
            return LV_RESULT_INVALID;
        }
    }
    else if(dsc->src_type == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t * img_dsc = dsc->src;
        png_data = img_dsc->data;
        png_data_size = img_dsc->data_size;
    }
    else {
        LV_PROFILER_DECODER_END_TAG("lv_lodepng_decoder_open");
        return LV_RESULT_INVALID;
    }

    lv_draw_buf_t * decoded = decode_png_data(png_data, png_data_size);

    if(dsc->src_type == LV_IMAGE_SRC_FILE) lv_free((void *)png_data);

    if(!decoded) {
        LV_LOG_WARN("Error decoding PNG");
        LV_PROFILER_DECODER_END_TAG("lv_lodepng_decoder_open");
        return LV_RESULT_INVALID;
    }

    lv_draw_buf_t * adjusted = lv_image_decoder_post_process(dsc, decoded);
    if(adjusted == NULL) {
        lv_draw_buf_destroy(decoded);
        LV_PROFILER_DECODER_END_TAG("lv_lodepng_decoder_open");
        return LV_RESULT_INVALID;
    }

    /*Скорректированный буфер отрисовки выделяется заново.*/
    if(adjusted != decoded) {
        lv_draw_buf_destroy(decoded);
        decoded = adjusted;
    }

    dsc->decoded = decoded;

    if(dsc->args.no_cache) {
        LV_PROFILER_DECODER_END_TAG("lv_lodepng_decoder_open");
        return LV_RESULT_OK;
    }

    /*Если кэш изображений отключен, просто верните декодированное изображение.*/
    if(!lv_image_cache_is_enabled()) {
        LV_PROFILER_DECODER_END_TAG("lv_lodepng_decoder_open");
        return LV_RESULT_OK;
    }

    /*Добавьте декодированное изображение в кеш*/
    lv_image_cache_data_t search_key;
    search_key.src_type = dsc->src_type;
    search_key.src = dsc->src;
    search_key.slot.size = decoded->data_size;

    lv_cache_entry_t * entry = lv_image_decoder_add_to_cache(decoder, &search_key, decoded, NULL);

    if(entry == NULL) {
        lv_draw_buf_destroy(decoded);
        dsc->decoded = NULL;
        LV_PROFILER_DECODER_END_TAG("lv_lodepng_decoder_open");
        return LV_RESULT_INVALID;
    }
    dsc->cache_entry = entry;

    LV_PROFILER_DECODER_END_TAG("lv_lodepng_decoder_open");
    return LV_RESULT_OK;    /*Если не вернулся раньше, значит, это не удалось*/
}

/**
 * Закрыть изображение PNG и бесплатные данные
 * @param decoder   указатель на декодер, которому принадлежит эта функция
 * @param dsc       дескриптор декодированного изображения
 * @return          LV_RESULT_OK: нет ошибок;  LV_RESULT_INVALID: не могу открыть изображение
 */
static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);

    if(dsc->args.no_cache ||
       !lv_image_cache_is_enabled()) lv_draw_buf_destroy((lv_draw_buf_t *)dsc->decoded);
}

static lv_draw_buf_t * decode_png_data(const void * png_data, size_t png_data_size)
{
    unsigned png_width;             /*Не используется, просто требуется декодером*/
    unsigned png_height;            /*Не используется, просто требуется декодером*/
    lv_draw_buf_t * decoded = NULL;

    /*Раскодируйте изображение в ARGB8888 */
    unsigned error = lodepng_decode32((unsigned char **)&decoded, &png_width, &png_height, png_data, png_data_size);
    if(error) {
        if(decoded != NULL)  lv_draw_buf_destroy(decoded);
        return NULL;
    }

    /*Преобразуйте изображение в глубину цвета системы.*/
    convert_color_depth(decoded->data,  png_width * png_height);

    return decoded;
}

/**
 * Если дисплей не в 32-битном формате ( ARGB888 ), преобразуйте изображение в текущую глубину цвета.
 * @param img_p изображение ARGB888
 * @param px_cnt количество пикселей в `img`
 */
static void convert_color_depth(uint8_t * img_p, uint32_t px_cnt)
{
    lv_color32_t * img_argb = (lv_color32_t *)img_p;
    uint32_t i;
    for(i = 0; i < px_cnt; i++) {
        uint8_t blue = img_argb[i].blue;
        img_argb[i].blue = img_argb[i].red;
        img_argb[i].red = blue;
    }
}

#endif /*LV_USE_LODEPNG*/
