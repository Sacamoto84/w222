/**
 * @file lv_draw_sw_mask.h
 *
 */

#ifndef LV_DRAW_SW_MASK_H
#define LV_DRAW_SW_MASK_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_area.h"
#include "../../misc/lv_color.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/
#define LV_MASK_ID_INV  (-1)
#if LV_DRAW_SW_COMPLEX
# define LV_MASK_MAX_NUM     16
#else
# define LV_MASK_MAX_NUM     1
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_DRAW_SW_MASK_RES_TRANSP,
    LV_DRAW_SW_MASK_RES_FULL_COVER,
    LV_DRAW_SW_MASK_RES_CHANGED,
    LV_DRAW_SW_MASK_RES_UNKNOWN
} lv_draw_sw_mask_res_t;

#if LV_DRAW_SW_COMPLEX

typedef enum {
    LV_DRAW_SW_MASK_TYPE_LINE,
    LV_DRAW_SW_MASK_TYPE_ANGLE,
    LV_DRAW_SW_MASK_TYPE_RADIUS,
    LV_DRAW_SW_MASK_TYPE_FADE,
    LV_DRAW_SW_MASK_TYPE_MAP,
} lv_draw_sw_mask_type_t;

typedef enum {
    LV_DRAW_SW_MASK_LINE_SIDE_LEFT = 0,
    LV_DRAW_SW_MASK_LINE_SIDE_RIGHT,
    LV_DRAW_SW_MASK_LINE_SIDE_TOP,
    LV_DRAW_SW_MASK_LINE_SIDE_BOTTOM,
} lv_draw_sw_mask_line_side_t;

/**
 * Общий тип обратного вызова для каждого типа маски.
 * Используется внутри библиотеки.
 */
typedef lv_draw_sw_mask_res_t (*lv_draw_sw_mask_xcb_t)(lv_opa_t * mask_buf, int32_t abs_x, int32_t abs_y,
                                                       int32_t len,
                                                       void * p);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_sw_mask_init(void);

void lv_draw_sw_mask_deinit(void);

/**
 * Примените добавленные буферы к строке. Используется внутри библиотеки подпрограммами рисования.
 * @param masks список применяемых масок должен заканчиваться указателемNULLв массиве.
 * @param mask_buf сохраните маску результата здесь. Должен быть длиной`len`в байтах. Должен быть инициализирован с помощью`0xFF`.
 * @param abs_x абсолютная координата X, где начинается линия для расчета
 * @param abs_y абсолютная координата Y, где начинается линия для расчета
 * @param len длина линии для расчета (в пикселях)
 * @return Одно из этих значений:
 * - `LV_DRAW_MASK_RES_FULL_TRANSP`: вся строка прозрачна. `mask_buf`не установлен в ноль.
 * - `LV_DRAW_MASK_RES_FULL_COVER`: вся строка видна полностью. `mask_buf`без изменений
 * - `LV_DRAW_MASK_RES_CHANGED`:`mask_buf`изменился, он показывает желаемую непрозрачность каждого пикселя в данной строке.
 */
lv_draw_sw_mask_res_t /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_sw_mask_apply(void * masks[], lv_opa_t * mask_buf,
                                                                        int32_t abs_x,
                                                                        int32_t abs_y,
                                                                        int32_t len);

/**
 * Освободите данные из параметра.
 * Внутри он называется`lv_draw_sw_mask_remove_id`и `lv_draw_sw_mask_remove_custom`.
 * Внесение необходимо только в особых случаях, когда маска не добавляется `lv_draw_mask_add`.
 * и не удаляется`lv_draw_mask_remove_id`или `lv_draw_mask_remove_custom`
 * @param p указатель на параметр маски
 */
void lv_draw_sw_mask_free_param(void * p);

/**
 *Инициализируйте маску линии из двух точек.
 * @param param указатель на`lv_draw_mask_param_t`для инициализации
 * @param p1x Координата X первой точки линии
 * @param p1y Координата Y первой точки линии
 * @param p2x Координата X второй точки линии
 * @param p2y координата y второй точки линии
 * @param side и элемент `lv_draw_mask_line_side_t`, чтобы описать, какую сторону сохранять.
 * При использовании `LV_DRAW_MASK_LINE_SIDE_LEFT/RIGHT` и горизонтальной линии все пиксели сохраняются.
 * При использовании `LV_DRAW_MASK_LINE_SIDE_TOP/BOTTOM` и вертикальной линии все пиксели сохраняются.
 */
void lv_draw_sw_mask_line_points_init(lv_draw_sw_mask_line_param_t * param, int32_t p1x, int32_t p1y,
                                      int32_t p2x,
                                      int32_t p2y, lv_draw_sw_mask_line_side_t side);

/**
 *Инициализируйте маску линии из точки и угла.
 * @param param  указатель на`lv_draw_mask_param_t`для инициализации
 * @param px     Координата X точки линии
 * @param py     Координата X точки линии
 * @param angle  вправо 0 град, внизу: 90
 * @param side   элемент `lv_draw_mask_line_side_t`, описывающий, какую сторону сохранять.
 * При использовании `LV_DRAW_MASK_LINE_SIDE_LEFT/RIGHT` и горизонтальной линии все пиксели сохраняются.
 * При использовании `LV_DRAW_MASK_LINE_SIDE_TOP/BOTTOM` и вертикальной линии все пиксели сохраняются.
 */
void lv_draw_sw_mask_line_angle_init(lv_draw_sw_mask_line_param_t * param, int32_t px, int32_t py, int16_t angle,
                                     lv_draw_sw_mask_line_side_t side);

/**
 * Инициализируйте угловую маску.
 * @param param указатель на`lv_draw_mask_param_t`для инициализации
 * @param vertex_x Координата X вершины угла (абсолютные координаты)
 * @param vertex_y Координата Y вершины угла (абсолютные координаты)
 * @param start_angle Начальный угол в градусах. 0 градусов справа, 90 градусов внизу
 * @param end_angle конечный угол
 */
void lv_draw_sw_mask_angle_init(lv_draw_sw_mask_angle_param_t * param, int32_t vertex_x, int32_t vertex_y,
                                int32_t start_angle, int32_t end_angle);

/**
 * Инициализируйте маску затухания.
 * @param param указатель на`lv_draw_mask_radius_param_t`для инициализации
 * @param rect координаты прямоугольника, на который воздействует (абсолютные координаты)
 * @param radius радиус прямоугольника
 * @param inv true: оставить пиксели внутри прямоугольника; держите пиксели за пределами прямоугольника
 */
void lv_draw_sw_mask_radius_init(lv_draw_sw_mask_radius_param_t * param, const lv_area_t * rect, int32_t radius,
                                 bool inv);

/**
 * Инициализируйте маску затухания.
 * @param param указатель на`lv_draw_mask_param_t`для инициализации
 * @param coords координаты зоны воздействия (абсолютные координаты)
 * @param opa_top непрозрачность сверху
 * @param y_top в какой координате начало меняться на непрозрачность на `opa_bottom`
 * @param opa_bottom непрозрачность внизу
 * @param y_bottom в которой координата достигает`opa_bottom`.
 */
void lv_draw_sw_mask_fade_init(lv_draw_sw_mask_fade_param_t * param, const lv_area_t * coords, lv_opa_t opa_top,
                               int32_t y_top,
                               lv_opa_t opa_bottom, int32_t y_bottom);

/**
 * Инициализируйте маску карты.
 * @param param указатель на`lv_draw_mask_param_t`для инициализации
 * @param coords координаты карты (абсолютные координаты)
 * @param map массив байтов со значениями маски
 */
void lv_draw_sw_mask_map_init(lv_draw_sw_mask_map_param_t * param, const lv_area_t * coords, const lv_opa_t * map);

#endif /*LV_DRAW_SW_COMPLEX*/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_MASK_H*/
