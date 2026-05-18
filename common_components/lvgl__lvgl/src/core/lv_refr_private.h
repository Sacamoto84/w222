/**
 * @file lv_refr_private.h
 *
 */

#ifndef LV_REFR_PRIVATE_H
#define LV_REFR_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_refr.h"

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
 * Инициализировать подсистему обновления экрана
 */
void lv_refr_init(void);

/**
 * Деинициализируйте подсистему обновления экрана.
 */
void lv_refr_deinit(void);

/**
 * Сделайте недействительной отображаемую область, чтобы перерисовать ее.
 * @param area_p указатель на область, которая должна быть признана недействительной (NULL: удалить недействительные области)
 * @param disp указатель для отображения места, где область должна быть признана недействительной (можно использовать NULL, если есть
 * только один дисплей)
 * @return LV_RESULT_OK : область признана недействительной; LV_RESULT_INVALID: область не была признана недействительной.
 */
lv_result_t lv_inv_area(lv_display_t * disp, const lv_area_t * area_p);

/**
 * Получить дисплей, который обновляется
 * @return дисплей обновляется
 */
lv_display_t * lv_refr_get_disp_refreshing(void);

/**
 * Установите обновляемый дисплей
 * @param disp дисплей обновляется
 */
void lv_refr_set_disp_refreshing(lv_display_t * disp);

/**
 * Найдите самый верхний объект, который полностью покрывает область.
 * @param area_p указатель на область
 * @param obj первый объект, с которого начинается поиск (обычно экран)
 * @return
 */
lv_obj_t * lv_refr_get_top_obj(const lv_area_t * area_p, lv_obj_t * obj);

/**
 * Рендеринг объекта в слой
 * @param layer целевой слой чертежа
 * @param obj   объект для рендеринга
 */
void lv_obj_refr(lv_layer_t * layer, lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_REFR_PRIVATE_H*/
