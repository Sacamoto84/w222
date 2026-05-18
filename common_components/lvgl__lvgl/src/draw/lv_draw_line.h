/**
 * @file lv_draw_line.h
 *
 */

#ifndef LV_DRAW_LINE_H
#define LV_DRAW_LINE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "../misc/lv_color.h"
#include "../misc/lv_area.h"
#include "../misc/lv_style.h"

/*********************
 *      DEFINES
 *********************/
#if LV_USE_FLOAT
#include <float.h>
#define LV_DRAW_LINE_POINT_NONE     FLT_MAX
#else
#define LV_DRAW_LINE_POINT_NONE     INT32_MAX
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_dsc_base_t base;

    /**Первая точка линии. Если `LV_USE_FLOAT` включен, можно также использовать число с плавающей запятой.
     *Игнорируется, если установлен`points`.*/
    lv_point_precise_t p1;

    /**Вторая точка линии. Если `LV_USE_FLOAT` включен, можно также использовать число с плавающей запятой.
     * Игнорируется, если установлен`points`.*/
    lv_point_precise_t p2;

    /**Array of points to draw. If `LV_USE_FLOAT` is enabled, float numbers can also be used.*/
    lv_point_precise_t * points;

    /**
     * Количество очков в `points`
     */
    int32_t point_cnt;

    /**The color of the line*/
    lv_color_t color;

    /**The width (thickness) of the line*/
    int32_t width;

    /** Длина тире (0: не тире)*/
    int32_t dash_width;

    /** Длина промежутков между тире (0: не тире)*/
    int32_t dash_gap;

    /**Непрозрачность линии в диапазоне 0...255.
     * LV_OPA_TRANSP , LV_OPA_10 , LV_OPA_20 , .. LV_OPA_COVER также можно использовать*/
    lv_opa_t opa;

    /**Make the line start rounded*/
    uint8_t round_start : 1;

    /**Make the line end rounded*/
    uint8_t round_end   : 1;

    /**1: Do not bother with line ending (if it's not visible for any reason) */
    uint8_t raw_end     : 1;
} lv_draw_line_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализировать дескриптор рисования линии
 * @param dsc       указатель на дескриптор отрисовки
 */
void lv_draw_line_dsc_init(lv_draw_line_dsc_t * dsc);

/**
 * Попробуйте получить дескриптор рисования линии из задачи рисования.
 * @param task      нарисовать задачу
 * @return          дескриптор отрисовки задачи или NULL, если задача не относится к типу LV_DRAW_TASK_TYPE_LINE
 */
lv_draw_line_dsc_t * lv_draw_task_get_line_dsc(lv_draw_task_t * task);

/**
 * Создать задачу рисования линий
 * @param layer     указатель на слой
 * @param dsc       указатель на инициализированную переменную `lv_draw_line_dsc_t`
 */
void lv_draw_line(lv_layer_t * layer, const lv_draw_line_dsc_t * dsc);

/**
 * Вспомогательная функция для вызова обратного вызова, который рисует линию между двумя точками.
 * Таким образом, не имеет значения, использовались ли точки `` p1, p2 `` или ``, ``, поскольку он вызывает
 * `` обратный вызов `` при необходимости.
 * @param t             нарисовать задачу
 * @param dsc           указатель на дескриптор отрисовки
 * @param draw_line_cb  обратный вызов, который рисует линию между``dsc->p1``и``dsc->p2 ``
 */
void lv_draw_line_iterate(lv_draw_task_t * t, lv_draw_line_dsc_t * dsc,
                          void (*draw_line_cb)(lv_draw_task_t * t, const lv_draw_line_dsc_t * dsc));

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_DRAW_LINE_H*/
