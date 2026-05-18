/**
 * @file lv_windows_input.h
 *
 */

#ifndef LV_WINDOWS_INPUT_H
#define LV_WINDOWS_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"
#include "../../indev/lv_indev.h"

#if LV_USE_WINDOWS

#include <windows.h>

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
 * @brief Получите дескриптор окна из конкретного объекта устройства ввода LVGL.
 * @param indev Конкретный объект устройства ввода LVGL.
 * @return Дескриптор окна конкретного объекта устройства ввода LVGL.
*/
HWND lv_windows_get_indev_window_handle(lv_indev_t * indev);

/**
 * @brief Откройте объект устройства ввода указателяLVGLдля конкретного LVGL.
 *        отображаемый объект или создайте его, если устройство ввода указателя LVGL
 *        объект не создавался и не удалялся ранее.
 * @param display Конкретный экранный объект LVGL.
 * @return Объект устройства ввода указателяLVGLдля конкретного LVGL.
 *         объект отображения.
*/
lv_indev_t * lv_windows_acquire_pointer_indev(lv_display_t * display);

/**
 * @brief Откройте объект устройства ввода клавиатурыLVGLдля конкретного LVGL.
 *        объект отображения или создайте его, если устройство ввода с клавиатуры LVGL
 *        объект не создавался и не удалялся ранее.
 * @param display Конкретный экранный объект LVGL.
 * @return Объект устройства ввода с клавиатурыLVGLдля конкретного LVGL
 *         объект отображения.
*/
lv_indev_t * lv_windows_acquire_keypad_indev(lv_display_t * display);

/**
 * @brief Откройте объект устройства ввода кодировщикаLVGLдля конкретного LVGL.
 *        отображаемый объект или создайте его, если устройство ввода кодировщика LVGL
 *        объект не создавался и не удалялся ранее.
 * @param display Конкретный экранный объект LVGL.
 * @return Объект устройства ввода энкодераLVGLдля конкретного LVGL
 *         объект отображения.
*/
lv_indev_t * lv_windows_acquire_encoder_indev(lv_display_t * display);

/**********************
 *      MACROS
 **********************/

#endif // LV_USE_WINDOWS

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_WINDOWS_INPUT_H*/
