/**
 * @file lv_windows_display.h
 *
 */

#ifndef LV_WINDOWS_DISPLAY_H
#define LV_WINDOWS_DISPLAY_H

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

#define LV_WINDOWS_ZOOM_BASE_LEVEL 100

#ifndef USER_DEFAULT_SCREEN_DPI
#define USER_DEFAULT_SCREEN_DPI 96
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Create a LVGL display object.
 * @param title The window title of LVGL display.
 * @param hor_res The horizontal resolution value of LVGL display.
 * @param ver_res The vertical resolution value of LVGL display.
 * @param zoom_level The zoom level value. Base value is 100 a.k.a 100%.
 * @param allow_dpi_override Allow DPI override if true, or follow the
 *                           Динамическая настройка масштабирования Windows DPI.
 * @param simulator_mode Create simulator mode display if true (not resizable),
 *                       или создать отображение режима приложения (с возможностью изменения размера).
 * @return The created LVGL display object.
*/
lv_display_t * lv_windows_create_display(
    const wchar_t * title,
    int32_t hor_res,
    int32_t ver_res,
    int32_t zoom_level,
    bool allow_dpi_override,
    bool simulator_mode);

/**
 * @brief Get the window handle from specific LVGL display object.
 * @param display The specific LVGL display object.
 * @return The window handle from specific LVGL display object.
*/
HWND lv_windows_get_display_window_handle(lv_display_t * display);

/**
 * @brief Get logical pixel value from physical pixel value taken account
 *        с уровнем масштабирования.
 * @param physical The physical pixel value taken account with zoom level.
 * @param zoom_level The zoom level value. Base value is 100 a.k.a 100%.
 * @return The logical pixel value.
 * @remark Он использует тот же стиль вычислений, что и реализация Windows OS.
 *         Будет полезно для интеграции серверной части Windows LVGL с другими
 *         Windows-приложения.
*/
int32_t lv_windows_zoom_to_logical(int32_t physical, int32_t zoom_level);

/**
 * @brief Get physical pixel value taken account with zoom level from
 *        значение логического пикселя.
 * @param logical The logical pixel value.
 * @param zoom_level The zoom level value. Base value is 100 a.k.a 100%.
 * @return The physical pixel value taken account with zoom level.
 * @remark Он использует тот же стиль вычислений, что и реализация Windows OS.
 *         Будет полезно для интеграции серверной части Windows LVGL с другими
 *         Windows-приложения.
*/
int32_t lv_windows_zoom_to_physical(int32_t logical, int32_t zoom_level);

/**
 * @brief Get logical pixel value from physical pixel value taken account
 *        с масштабированием DPI.
 * @param physical The physical pixel value taken account with DPI scaling.
 * @param dpi The DPI scaling value. Base value is USER_DEFAULT_SCREEN_DPI.
 * @return The logical pixel value.
 * @remark Он использует тот же стиль вычислений, что и реализация Windows OS.
 *         Будет полезно для интеграции серверной части Windows LVGL с другими
 *         Windows-приложения.
*/
int32_t lv_windows_dpi_to_logical(int32_t physical, int32_t dpi);

/**
 * @brief Get physical pixel value taken account with DPI scaling from
 *        значение логического пикселя.
 * @param logical The logical pixel value.
 * @param dpi The DPI scaling value. Base value is USER_DEFAULT_SCREEN_DPI.
 * @return The physical pixel value taken account with DPI scaling.
 * @remark Он использует тот же стиль вычислений, что и реализация Windows OS.
 *         Будет полезно для интеграции серверной части Windows LVGL с другими
 *         Windows-приложения.
*/
int32_t lv_windows_dpi_to_physical(int32_t logical, int32_t dpi);

/**********************
 *      MACROS
 **********************/

#endif // LV_USE_WINDOWS

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_WINDOWS_DISPLAY_H*/
