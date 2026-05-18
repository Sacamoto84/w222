
/**
 * @file lv_wl_window.h
 *
 */

#ifndef LV_WL_WINDOW_H
#define LV_WL_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"

#if LV_USE_WAYLAND

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef bool (*lv_wayland_display_close_cb_t)(lv_display_t * disp);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создает окно
 * @param hor_res Ширина окна в пикселях
 * @param ver_res Высота окна в пикселях
 * @param title Название окна
 * @param close_cb Обратный вызов, который будет выполнен, когда пользователь закроет окно.
 * @return Дисплей LVGL, связанный с окном
 */
lv_display_t * lv_wayland_window_create(uint32_t hor_res, uint32_t ver_res, char * title,
                                        lv_wayland_display_close_cb_t close_cb);

/**
 * Закрывает окно программно
 * @param disp Ссылка на дисплей LVGL, связанный с окном.
 */
void lv_wayland_window_close(lv_display_t * disp);

/**
 * Проверьте, открыто ли окно
 * @param disp Ссылка на дисплей LVGL, связанный с окном.
 * @return правда: окно открыто
 */
bool lv_wayland_window_is_open(lv_display_t * disp);

/**
 * Назначает окно конкретному физическому дисплею
 * @param disp Ссылка на дисплей LVGL, связанный с окном.
 * @param display Физический номер дисплея
 */
void lv_wayland_assign_physical_display(lv_display_t * disp, uint8_t display);

/**
 * Отменяет назначение текущего физического дисплея, прикрепленного к окну.
 * @param disp Ссылка на дисплей LVGL, связанный с окном.
 */
void lv_wayland_unassign_physical_display(lv_display_t * disp);

/**
 * Устанавливает полноэкранное состояние окна
 * @param disp Ссылка на дисплей LVGL, связанный с окном.
 * @param fullscreen Если это правда, окно переходит в полноэкранный режим.
 */

void lv_wayland_window_set_fullscreen(lv_display_t * disp, bool fullscreen);

/**
 * Устанавливает максимальное состояние окна
 * @param disp Ссылка на дисплей LVGL, связанный с окном.
 * @param maximize Если это правда, окно развернуто до максимума.
 */
void lv_wayland_window_set_maximized(lv_display_t * disp, bool maximize);

/**
 * Сворачивает окно
 * @param disp Ссылка на дисплей LVGL, связанный с окном.
 */
void lv_wayland_window_set_minimized(lv_display_t * disp);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_WAYLAND */

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_WL_WINDOW_H*/
