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
 * @brief Создайте экранный объект LVGL.
 * @param title Заголовок окна дисплея LVGL.
 * @param hor_res Значение горизонтального разрешения дисплея LVGL.
 * @param ver_res Значение вертикального разрешения дисплея LVGL.
 * @param zoom_level Значение уровня масштабирования. Базовое значение составляет 100, то есть 100%.
 * @param allow_dpi_override Разрешите переопределение DPI, если это правда, или следуйте инструкциям
 *                           Динамическая настройка масштабирования WindowsDPI.
 * @param simulator_mode Создать отображение режима симулятора, если это правда (размер не изменяется),
 *                       или создать отображение режима приложения (с возможностью изменения размера).
 * @return Созданный экранный объект LVGL.
*/
lv_display_t * lv_windows_create_display(
    const wchar_t * title,
    int32_t hor_res,
    int32_t ver_res,
    int32_t zoom_level,
    bool allow_dpi_override,
    bool simulator_mode);

/**
 * @brief Получите дескриптор окна из конкретного экранного объекта LVGL.
 * @param display Конкретный экранный объект LVGL.
 * @return Дескриптор окна конкретного экранного объекта LVGL.
*/
HWND lv_windows_get_display_window_handle(lv_display_t * display);

/**
 * @brief Получить значение логического пикселя из принятого значения физического пикселя.
 *        с уровнем масштабирования.
 * @param physical Значение физического пикселя учитывается с уровнем масштабирования.
 * @param zoom_level Значение уровня масштабирования. Базовое значение составляет 100, то есть 100%.
 * @return Значение логического пикселя.
 * @remark Он использует тот же стиль компьютера, что и версия Windows OS.
 *         Будет полезно для офисной серверной части WindowsLVGLс другими
 *         Windows-приложения.
*/
int32_t lv_windows_zoom_to_logical(int32_t physical, int32_t zoom_level);

/**
 * @brief Получите значение физического пикселя, учитываемое с уровнем масштабирования из
 *        значение логического пикселя.
 * @param logical Значение логического пикселя.
 * @param zoom_level Значение уровня масштабирования. Базовое значение составляет 100, то есть 100%.
 * @return Значение физического пикселя учитывается с уровнем масштабирования.
 * @remark Он использует тот же стиль компьютера, что и версия Windows OS.
 *         Будет полезно для офисной серверной части WindowsLVGLс другими
 *         Windows-приложения.
*/
int32_t lv_windows_zoom_to_physical(int32_t logical, int32_t zoom_level);

/**
 * @brief Получить значение логического пикселя из принятого значения физического пикселя.
 *        с масштабированием DPI.
 * @param physical Значение физического пикселя, учитываемое с помощью масштабирования DPI.
 * @param dpi Значение масштабирования DPI. Базовое значение — USER_DEFAULT_SCREEN_DPI.
 * @return Значение логического пикселя.
 * @remark Он использует тот же стиль компьютера, что и версия Windows OS.
 *         Будет полезно для офисной серверной части WindowsLVGLс другими
 *         Windows-приложения.
*/
int32_t lv_windows_dpi_to_logical(int32_t physical, int32_t dpi);

/**
 * @brief Получите значение физического пикселя, учтенное с помощью масштабированияDPIиз
 *        значение логического пикселя.
 * @param logical Значение логического пикселя.
 * @param dpi Значение масштабирования DPI. Базовое значение — USER_DEFAULT_SCREEN_DPI.
 * @return Значение физического пикселя, учитываемое с помощью масштабирования DPI.
 * @remark Он использует тот же стиль компьютера, что и версия Windows OS.
 *         Будет полезно для офисной серверной части WindowsLVGLс другими
 *         Windows-приложения.
*/
int32_t lv_windows_dpi_to_physical(int32_t logical, int32_t dpi);

/**********************
 *      MACROS
 **********************/

#endif // LV_USE_WINDOWS

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_WINDOWS_DISPLAY_H*/
