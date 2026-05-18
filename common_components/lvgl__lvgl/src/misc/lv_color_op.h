/**
 * @file lv_color_op.h
 *
 */

#ifndef LV_COLOR_OP_H
#define LV_COLOR_OP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_assert.h"
#include "lv_math.h"
#include "lv_color.h"
#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_color_filter_dsc_t;

typedef lv_color_t (*lv_color_filter_cb_t)(const struct _lv_color_filter_dsc_t *, lv_color_t, lv_opa_t);

struct _lv_color_filter_dsc_t {
    lv_color_filter_cb_t filter_cb;
    void * user_data;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Смешайте два цвета в заданном соотношении.
 * @param c1 первый цвет, который нужно смешать (обычно передний план)
 * @param c2 второй цвет для смешивания (обычно фон)
 * @param mix Соотношение цветов. 0: полный `c2`, 255: полный `c1`, 127: половина`c1`и половина `c2`
 * @return смешанный цвет
 */
lv_color_t LV_ATTRIBUTE_FAST_MEM lv_color_mix(lv_color_t c1, lv_color_t c2, uint8_t mix);

/**
 *
 * @param fg
 * @param бг
 * @return
 * @note Используйте bg.alpha в возвращаемом значении
 * @note Используйте fg.alpha в качестве соотношения смешивания.
 */
lv_color32_t lv_color_mix32(lv_color32_t fg, lv_color32_t bg);

/**
 * @brief Смешивает два предварительно умноженных цвета ARGB8888, сохраняя при этом правильную альфа-композицию.
 *
 * Эта функция правильно смешивает цвета переднего плана (fg) и фона (bg).
 * обеспечение того, чтобы выходные данные оставались в предварительно умноженном альфа-формате.
 *
 * @param fg Цвет переднего плана в предварительно умноженном формате ARGB8888.
 * @param bg Цвет фона в предварительно умноженном формате ARGB8888.
 * @return Полученный смешанный цвет в предварительно умноженном формате ARGB8888.
 *
 * @note Если передний план полностью непрозрачен, он возвращается как есть.
 * @note Если передний план полностью прозрачен, возвращается фон.
 */
lv_color32_t lv_color_mix32_premultiplied(lv_color32_t fg, lv_color32_t bg);

/**
 * Получить яркость цвета
 * @param c   цвет
 * @return яркость в диапазоне [0..255]
 */
uint8_t lv_color_brightness(lv_color_t c);

void lv_color_filter_dsc_init(lv_color_filter_dsc_t * dsc, lv_color_filter_cb_t cb);

/**
 * Смешайте два цвета, которые не были предварительно умножены, используя их альфа-значения.
 * @param fg цвет переднего плана
 * @param bg цвет фона
 * @return цвет результата
 */
lv_color32_t lv_color_over32(lv_color32_t fg, lv_color32_t bg);

/**********************
 *  PREDEFINED COLORS
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_COLOR_OP_H*/
