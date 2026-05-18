/**
 * @file lv_grad.h
 *
 */

#ifndef LV_GRAD_H
#define LV_GRAD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "lv_color.h"
#include "lv_area.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Направление градиента.
 */
typedef enum {
    LV_GRAD_DIR_NONE,       /**< No gradient (the `grad_color` property is ignored)*/
    LV_GRAD_DIR_VER,        /**< Simple vertical (top to bottom) gradient*/
    LV_GRAD_DIR_HOR,        /**< Simple horizontal (left to right) gradient*/
    LV_GRAD_DIR_LINEAR,     /**< Linear gradient defined by start and end points. Can be at any angle.*/
    LV_GRAD_DIR_RADIAL,     /**< Radial gradient defined by start and end circles*/
    LV_GRAD_DIR_CONICAL,    /**< Conical gradient defined by center point, start and end angles*/
} lv_grad_dir_t;

/**
 * Градиентное поведение за пределами определенного диапазона.
 */
typedef enum {
    LV_GRAD_EXTEND_PAD,     /**< Repeat the same color*/
    LV_GRAD_EXTEND_REPEAT,  /**< Repeat the pattern*/
    LV_GRAD_EXTEND_REFLECT, /**< Repeat the pattern mirrored*/
} lv_grad_extend_t;

/** Определение остановки градиента.
 *  Это соответствует цвету и положению в виртуальной шкале от 0 до 255.
 */
typedef struct {
    lv_color_t color;   /**< The stop color */
    lv_opa_t   opa;     /**< The opacity of the color*/
    uint8_t    frac;    /**< The stop position in 1/255 unit */
} lv_grad_stop_t;

/** Дескриптор градиента. */
typedef struct {
    lv_grad_stop_t   stops[LV_GRADIENT_MAX_STOPS];  /**< A gradient stop array */
    uint8_t          stops_count;                   /**< The number of used stops in the array */
    lv_grad_dir_t    dir : 4;                       /**< Направление градиента.
                                                         * Любой из LV_GRAD_DIR_NONE, LV_GRAD_DIR_VER, LV_GRAD_DIR_HOR,
                                                         * LV_GRAD_TYPE_LINEAR, LV_GRAD_TYPE_RADIAL, LV_GRAD_TYPE_CONICAL */
    lv_grad_extend_t     extend : 3;                    /**< Поведение вне заданного диапазона.
                                                         * LV_GRAD_EXTEND_NONE, LV_GRAD_EXTEND_PAD, LV_GRAD_EXTEND_REPEAT, LV_GRAD_EXTEND_REFLECT */
    union {
        /*Параметры линейного градиента*/
        struct {
            lv_point_t  start;                          /**< Linear gradient vector start point */
            lv_point_t  end;                            /**< Linear gradient vector end point */
        } linear;
        /*Параметры радиального градиента*/
        struct {
            lv_point_t  focal;                          /**< Center of the focal (starting) circle in local coordinates */
            /* (может быть таким же, как конечный круг для создания концентрических кругов) */
            lv_point_t  focal_extent;                   /**< Point on the circle (can be the same as the center) */
            lv_point_t  end;                            /**< Center of the ending circle in local coordinates */
            lv_point_t  end_extent;                     /**< Point on the circle determining the radius of the gradient */
        } radial;
        /*Параметры конического градиента*/
        struct {
            lv_point_t  center;                         /**< Conical gradient center point */
            int16_t     start_angle;                    /**< Start angle 0..3600 */
            int16_t     end_angle;                      /**< End angle 0..3600 */
        } conical;
    } params;
    void * state;
} lv_grad_dsc_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализировать карту цветов градиента из таблицы
 * @param grad      указатель на дескриптор градиента
 * @param colors    цветовая матрица
 * @param fracs     массив позиций (0..255): если NULL, то цвета распределяются равномерно
 * @param opa       массив непрозрачности: еслиNULL, то предполагается LV_OPA_COVER
 * @param num_stops количество остановок градиента (1..LV_GRADIENT_MAX_STOPS)
 */
void lv_grad_init_stops(lv_grad_dsc_t * grad, const lv_color_t colors[], const lv_opa_t opa[],
                        const uint8_t fracs[], int num_stops);

/**
 * Вспомогательная функция для инициализации горизонтального градиента.
 * @param dsc      дескриптор градиента
 */
void lv_grad_horizontal_init(lv_grad_dsc_t * dsc);

/**
 * Вспомогательная функция для инициализации вертикального градиента.
 * @param dsc      дескриптор градиента
 */
void lv_grad_vertical_init(lv_grad_dsc_t * dsc);

/**
 * Вспомогательная функция для инициализации линейного градиента
 * @param dsc      дескриптор градиента
 * @param from_x   начальная позиция x: может быть координатой или значениемlv_pct()
 *                 Также можно использовать предопределенные константы LV_GRAD_LEFT, LV_GRAD_RIGHT, LV_GRAD_TOP, LV_GRAD_BOTTOM, LV_GRAD_CENTER.
 * @param from_y   стартовая позиция
 * @param to_x     конечная позиция x
 * @param to_y     конечная позиция Y
 * @param extend   один из LV_GRAD_EXTEND_PAD,LV_GRAD_EXTEND_REPEATили LV_GRAD_EXTEND_REFLECT
 */
void lv_grad_linear_init(lv_grad_dsc_t * dsc, int32_t from_x, int32_t from_y, int32_t to_x, int32_t to_y,
                         lv_grad_extend_t extend);

/**
 * Вспомогательная функция для инициализации радиального градиента
 * @param dsc      дескриптор градиента
 * @param center_x Позиция центра x: может быть координатой или значениемlv_pct()
 *                 Также можно использовать предопределенные константы LV_GRAD_LEFT, LV_GRAD_RIGHT, LV_GRAD_TOP, LV_GRAD_BOTTOM, LV_GRAD_CENTER.
 * @param center_y центральное положение по оси Y
 * @param to_x     точка на конце окружности x позиция
 * @param to_y     точка на конечном круге, позиция y
 * @param extend   один из LV_GRAD_EXTEND_PAD,LV_GRAD_EXTEND_REPEATили LV_GRAD_EXTEND_REFLECT
 */
void lv_grad_radial_init(lv_grad_dsc_t * dsc, int32_t center_x, int32_t center_y, int32_t to_x, int32_t to_y,
                         lv_grad_extend_t extend);

/**
 * Установить фокальный (начальный) круг радиального градиента
 * @param dsc      дескриптор градиента
 * @param center_x Позиция центра x: может быть координатой или значениемlv_pct()
 *                 Также можно использовать предопределенные константы LV_GRAD_LEFT, LV_GRAD_RIGHT, LV_GRAD_TOP, LV_GRAD_BOTTOM, LV_GRAD_CENTER.
 * @param center_y центральное положение по оси Y
 * @param radius   радиус начального круга (NOTE: это должно быть скалярное число, а не процент)
 */
void lv_grad_radial_set_focal(lv_grad_dsc_t * dsc, int32_t center_x, int32_t center_y, int32_t radius);

/**
 * Вспомогательная функция для инициализации конического градиента
 * @param dsc      дескриптор градиента
 * @param center_x Позиция центра x: может быть координатой или значениемlv_pct()
 *                 Также можно использовать предопределенные константы LV_GRAD_LEFT, LV_GRAD_RIGHT, LV_GRAD_TOP, LV_GRAD_BOTTOM, LV_GRAD_CENTER.
 * @param center_y центральное положение по оси Y
 * @param start_angle   начальный угол в градусах
 * @param end_angle     конечный угол в градусах
 * @param extend   один из LV_GRAD_EXTEND_PAD,LV_GRAD_EXTEND_REPEATили LV_GRAD_EXTEND_REFLECT
 */
void lv_grad_conical_init(lv_grad_dsc_t * dsc, int32_t center_x, int32_t center_y, int32_t start_angle,
                          int32_t end_angle, lv_grad_extend_t extend);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_GRAD_H*/
