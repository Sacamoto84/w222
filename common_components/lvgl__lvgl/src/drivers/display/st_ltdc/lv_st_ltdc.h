/**
 * @file lv_st_ltdc.h
 *
 */

#ifndef LV_ST_LTDC_H
#define LV_ST_LTDC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"
#if LV_USE_ST_LTDC

#include "../../../display/lv_display.h"

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
 * Создайте отображение режима прямого рендеринга, привязанное к слою LTDC.
 * @param fb_adr_1      Адрес памяти кадрового буфера уровня LTDC.
 * @param fb_adr_2      Дополнительный буфер размером с кадровый буфер, используемый для двойной буферизации, или`NULL`.
 * @param layer_idx     Номер слоя LTDC, к которому необходимо привязать отображение. Обычно 0 или 1.
 * @return              Дисплей.
 */
lv_display_t * lv_st_ltdc_create_direct(void * fb_adr_1, void * fb_adr_2, uint32_t layer_idx);

/**
 * Создайте отображение режима частичной визуализации, привязанное к слою LTDC. Фреймбуфер слоя очищается внутри.
 * Включите `LV_ST_LTDC_USE_DMA2D_FLUSH` для параллельной промывки.
 * @param render_buf_1  Буфер рендеринга.
 * @param render_buf_2  Дополнительный буфер рендеринга для двойной буферизации или `NULL`.
 * @param buf_size      Размер буфера(ов) в байтах.
 * @param layer_idx     Номер слоя LTDC, к которому необходимо привязать отображение. Обычно 0 или 1.
 * @return              Дисплей.
 */
lv_display_t * lv_st_ltdc_create_partial(void * render_buf_1, void * render_buf_2, uint32_t buf_size,
                                         uint32_t layer_idx);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_ST_LTDC*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ST_LTDC_H*/
