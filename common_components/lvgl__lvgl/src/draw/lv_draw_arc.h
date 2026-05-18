/**
 * @file lv_draw_arc.h
 *
 */

#ifndef LV_DRAW_ARC_H
#define LV_DRAW_ARC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "../lv_conf_internal.h"
#include "../misc/lv_color.h"
#include "../misc/lv_area.h"
#include "../misc/lv_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_dsc_base_t base;

    /**The color of the arc*/
    lv_color_t color;

    /**The width (thickness) of the arc */
    int32_t width;

    /**Начальный угол в единицах 1 градус (если `LV_USE_FLOAT` включен, можно также использовать число с плавающей запятой)
     * 0° — положение «3 часа», 90° — положение «6 часов» и т. д. */
    lv_value_precise_t start_angle;

    /**The end angle, similarly to start_angle. */
    lv_value_precise_t end_angle;

    /**The center point of the arc. */
    lv_point_t center;

    /**An image source to be used instead of `color`. `NULL` if unused*/
    const void * img_src;

    /**The outer radius of the arc*/
    uint16_t radius;

    /**Непрозрачность дуги в диапазоне 0...255.
     * LV_OPA_TRANSP , LV_OPA_10 , LV_OPA_20 , .. LV_OPA_COVER также можно использовать*/
    lv_opa_t opa;

    /**1: Make the arc ends rounded*/
    uint8_t rounded : 1;
} lv_draw_arc_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте дескриптор рисования дуги.
 * @param dsc       указатель на дескриптор отрисовки
 */
void lv_draw_arc_dsc_init(lv_draw_arc_dsc_t * dsc);

/**
 * Попробуйте получить дескриптор рисования дуги из задачи рисования.
 * @param task      нарисовать задачу
 * @return          дескриптор отрисовки задачи или NULL, если задача не относится к типу LV_DRAW_TASK_TYPE_ARC
 */
lv_draw_arc_dsc_t * lv_draw_task_get_arc_dsc(lv_draw_task_t * task);

/**
 * Создайте задачу рисования дуги.
 * @param layer         указатель на слой
 * @param dsc           указатель на инициализированную переменную дескриптора отрисовки
 */
void lv_draw_arc(lv_layer_t * layer, const lv_draw_arc_dsc_t * dsc);

/**
 * Получите область, которая должна быть признана недействительной при отклонении угла междуstart_angleи end_ange.
 * @param x             координата x центра дуги
 * @param y             координата y центра дуги
 * @param radius        радиус дуги
 * @param start_angle   начальный угол дуги (0 градусов внизу, 90 градусов справа)
 * @param end_angle     конечный угол дуги
 * @param w             ширина дуги
 * @param rounded       верно: дуга закруглена
 * @param area          сохраните здесь область, которую нужно сделать недействительной
 */
void lv_draw_arc_get_area(int32_t x, int32_t y, uint16_t radius,  lv_value_precise_t start_angle,
                          lv_value_precise_t end_angle,
                          int32_t w, bool rounded, lv_area_t * area);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_DRAW_ARC_H*/
