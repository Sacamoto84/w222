/**
 * @file lv_eve.h
 *
 */

/*  Создано: 8 июня 2023 г.
 *      Author: Хуандж
 *
 *  Изменено LVGL
 */

#ifndef LV_EVE_H
#define LV_EVE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_eve.h"

#if LV_USE_DRAW_EVE

#include "../../misc/lv_types.h"
#include "../../misc/lv_color.h"
#include "../../libs/FT800-FT813/EVE.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_EVE_PRIMITIVE_ZERO_VALUE,
    LV_EVE_PRIMITIVE_BITMAPS = 1UL,      /* Примитив растрового рисунка */
    LV_EVE_PRIMITIVE_POINTS = 2UL,       /* Примитив рисования точек */
    LV_EVE_PRIMITIVE_LINES = 3UL,        /* Примитив рисования линий */
    LV_EVE_PRIMITIVE_LINE_STRIP = 4UL,   /* Примитив рисования полосок линий */
    LV_EVE_PRIMITIVE_EDGE_STRIP_R = 5UL, /* Примитив рисования правой стороны краевой полосы */
    LV_EVE_PRIMITIVE_EDGE_STRIP_L = 6UL, /* Примитив рисования левой стороны краевой полосы */
    LV_EVE_PRIMITIVE_EDGE_STRIP_A = 7UL, /* Краевая полоса над примитивом рисования */
    LV_EVE_PRIMITIVE_EDGE_STRIP_B = 8UL, /* Краевая полоса под примитивом бокового рисунка */
    LV_EVE_PRIMITIVE_RECTS = 9UL,        /* Примитив рисования прямоугольника */
} lv_eve_primitive_t;


typedef struct {
    lv_eve_primitive_t primitive;
    lv_color_t color;
    lv_opa_t opa;
    int32_t line_width;
    uint16_t point_size;
    uint8_t color_mask[4];
    uint8_t stencil_func[3];
    uint8_t stencil_op[2];
    uint8_t blend_func[2];
    uint16_t scx;
    uint16_t scy;
} lv_eve_drawing_context_t;

/* контекст рисования, который не сохраняется и не восстанавливается
 * от SAVE_CONTEXT и RESTORE_CONTEXT
 */
typedef struct {
    uint32_t bitmap_source;
    uint32_t bitmap_size;
    uint32_t bitmap_size_h;
    uint32_t bitmap_layout;
    uint32_t bitmap_layout_h;
} lv_eve_drawing_state_t;

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

void lv_eve_save_context(void);
void lv_eve_restore_context(void);
void lv_eve_scissor(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void lv_eve_primitive(uint8_t context);
void lv_eve_color(lv_color_t color);
void lv_eve_color_opa(lv_opa_t opa);
void lv_eve_line_width(int32_t width);
void lv_eve_point_size(uint16_t radius);
void lv_eve_vertex_2f(int16_t x, int16_t y);
void lv_eve_color_mask(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void lv_eve_stencil_func(uint8_t func, uint8_t ref, uint8_t mask);
void lv_eve_stencil_op(uint8_t sfail, uint8_t spass);
void lv_eve_blend_func(uint8_t src, uint8_t dst);

void lv_eve_draw_circle_simple(int16_t coord_x1, int16_t coord_y1, uint16_t radius_t);
void lv_eve_draw_rect_simple(int16_t coord_x1, int16_t coord_y1, int16_t coord_x2, int16_t coord_y2,
                             uint16_t radius);
void lv_eve_mask_round(int16_t coord_x1, int16_t coord_y1, int16_t coord_x2, int16_t coord_y2, int16_t radius);

/**
 * Установите источник растрового изображения на `addr`.  ПередачаSPIбудет осуществляться, если для него еще не установлено это значение.
 * Источник растрового изображения не является частью сохраненного и восстановленного контекста.
 * @param addr    адрес удаленной памяти EVE, который нужно установить в качестве источника растрового изображения
 */
void lv_eve_bitmap_source(uint32_t addr);

/**
 * Установите размер растрового изображения и параметры выборки.  Передача SPI будет происходить, если текущие установленные параметры уже не являются такими.
 * Размер растрового изображения не является частью сохраненного и восстановленного контекста.
 * @param filter   метод выборки. Либо EVE_NEAREST, либо EVE_BILINEAR.
 * @param wrapx    поведение выборки за пределами границ в направлении X. Либо EVE_BORDER, либо EVE_REPEAT.
 * @param wrapy    поведение выборки за пределами границ в направлении Y. Либо EVE_BORDER, либо EVE_REPEAT.
 * @param width    ширина растрового изображения в пикселях
 * @param height   высота растрового изображения в пикселях
 */
void lv_eve_bitmap_size(uint8_t filter, uint8_t wrapx, uint8_t wrapy, uint16_t width, uint16_t height);

/**
 * Установите параметры формата/макета растрового изображения.  Передача SPI будет происходить, если текущие установленные параметры уже не являются такими.
 * Макет растрового изображения не является частью сохраненного и восстановленного контекста.
 * @param format      значение формата цвета eve, например EVE_RGB565
 * @param linestride  шаг строк растрового изображения в байтах
 * @param height      количество строк в растровом изображении
 */
void lv_eve_bitmap_layout(uint8_t format, uint16_t linestride, uint16_t height);

/**********************
 *  EXTERN VARIABLES
 **********************/


/**********************
 *      MACROS
 **********************/

/**********************
 *   INLINE FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_EVE*/


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_EVE_H */
