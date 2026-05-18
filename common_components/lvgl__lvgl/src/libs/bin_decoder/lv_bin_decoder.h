/**
 * @file lv_bin_decoder.h
 *
 */

#ifndef LV_BIN_DECODER_H
#define LV_BIN_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../draw/lv_image_decoder.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте модуль декодера двоичных изображений
 */
void lv_bin_decoder_init(void);

/**
 * Получить информацию о двойном изображении lvgl
 * @param decoder декодер, которому принадлежит эта функция
 * @param dsc Дескриптор изображения, содержащий источник и тип изображения, а также другую информацию.
 * @param header сохраните данные изображения здесь
 * @return LV_RESULT_OK: информация успешно сохранена в `header`;  LV_RESULT_INVALID: неизвестный формат или другая ошибка.
 */
lv_result_t lv_bin_decoder_info(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc, lv_image_header_t * header);

lv_result_t lv_bin_decoder_get_area(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc,
                                    const lv_area_t * full_area, lv_area_t * decoded_area);

/**
 * Открыть двойное изображение lvgl
 * @param decoder декодер, которому принадлежит эта функция
 * @param dsc указатель на дескриптор декодера.  В нем уже инициализированы `src`, `style`.
 * @return LV_RESULT_OK: информация успешно сохранена в `header`;  LV_RESULT_INVALID: неизвестный формат или другая ошибка.
 */
lv_result_t lv_bin_decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);

/**
 * Закройте ожидающее декодирование. Бесплатные ресурсы и т. д.
 * @param decoder указатель на декодер, функцию, связанную с
 * @param dsc указатель на дескриптор декодера
 */
void lv_bin_decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_BIN_DECODER_H*/
