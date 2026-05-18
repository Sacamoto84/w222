/**
 * @file lv_indev_scroll.h
 *
 */

#ifndef LV_INDEV_SCROLL_H
#define LV_INDEV_SCROLL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../core/lv_obj.h"

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
 * Управление прокруткой. Вызывается LVGL во время обработки устройства ввода.
 * @param indev      указатель на устройство ввода
 */
void lv_indev_scroll_handler(lv_indev_t * indev);

/**
 * Ручка бросания после прокрутки. Вызывается LVGL во время обработки устройства ввода.
 * @param indev      указатель на устройство ввода
 */
void lv_indev_scroll_throw_handler(lv_indev_t * indev);

/**
 * Предсказать, где закончится прокрутка
 * @param indev     указатель на устройство ввода
 * @param  dir  `     LV_DIR_VER`  or  `LV_DIR_HOR`
 * @return          разница по сравнению с текущей позицией, когда бросок будет завершен
 */
int32_t lv_indev_scroll_throw_predict(lv_indev_t * indev, lv_dir_t dir);

/**
 * Получить расстояние до ближайшей точки привязки
 * @param obj       объект, на котором должны быть найдены точки привязки
 * @param p         сохраните туда расстояние найденной точки привязки
 */
void lv_indev_scroll_get_snap_dist(lv_obj_t * obj, lv_point_t * p);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_INDEV_SCROLL_H*/
