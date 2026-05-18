/**
 * @file lv_linux_drm.h
 *
 */

#ifndef LV_LINUX_DRM_H
#define LV_LINUX_DRM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../display/lv_display.h"

#if LV_USE_LINUX_DRM
#include <xf86drmMode.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef drmModeModeInfo lv_linux_drm_mode_t;

/**
 * Тип функции обратного вызова для выбора режима отображения DRM
 * @param disp pointer to the display object
 * @param modes array of available DRM modes
 * @param mode_count number of modes in the array
 * @return index of the selected mode from the modes array
 */
typedef size_t (*lv_linux_drm_select_mode_cb_t)(lv_display_t * disp,
                                                const lv_linux_drm_mode_t * modes,
                                                size_t mode_count);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Create a new Linux DRM display
 *
 * Создает и инициализирует новый дисплей LVGL с помощью Linux DRM (Direct Rendering Manager).
 * подсистема вывода графики с аппаратным ускорением.
 *
 * @return Pointer to the created display object, or NULL on failure
 */
lv_display_t * lv_linux_drm_create(void);

/**
 * @brief Configure the DRM device file and connector for a display
 *
 * Задает путь к файлу устройства DRM и разъем ID, которые будут использоваться для указанного дисплея.
 * Файл устройства DRM обычно находится в каталоге /dev/dri/cardN, где N — номер карты.
 * Разъем ID указывает, какой физический выход (HDMI, VGA и т. д.) использовать.
 *
 * @param disp         Pointer to the display object created with lv_linux_drm_create()
 * @param file         Path to the DRM device file (e.g., "/dev/dri/card0")
 * @param connector_id ID of the DRM connector to use, or -1 to auto-select the first available
 * @return LV_RESULT_OK if the initialization succeeeded or LV_RESULT_INVALID if it failed
 */
lv_result_t lv_linux_drm_set_file(lv_display_t * disp, const char * file, int64_t connector_id);

/**
 * @brief Automatically find a suitable DRM device path
 *
 * Сканирует систему на наличие доступных устройств DRM и возвращает путь к подходящему
 * файл устройства, который можно использовать с lv_linux_drm_set_file ().
 *
 * @return Dynamically allocated string containing the device path (must be freed with lv_free()),
 *         или NULL, если подходящее устройство не найдено
 */
char * lv_linux_drm_find_device_path(void);

/**
 * Установите функцию обратного вызова для выбора пользовательского режима DRM, чтобы переопределить поведение выбора режима по умолчанию.
 *
 * Поведение выбора режима по умолчанию — выбор собственного режима.
 *
 * @param disp pointer to the display object
 * @param callback function to be called when a display mode needs to be selected,
 *                 или NULL, чтобы использовать поведение выбора режима по умолчанию.
 */
void lv_linux_drm_set_mode_cb(lv_display_t * disp, lv_linux_drm_select_mode_cb_t callback);

/**
 * Получите горизонтальное разрешение режима DRM.
 * @param mode pointer to the DRM mode object
 * @return horizontal resolution in pixels, or 0 if mode is invalid
 */
int32_t lv_linux_drm_mode_get_horizontal_resolution(const lv_linux_drm_mode_t * mode);

/**
 * Получите вертикальное разрешение режима DRM.
 * @param mode pointer to the DRM mode object
 * @return vertical resolution in pixels, or 0 if mode is invalid
 */
int32_t lv_linux_drm_mode_get_vertical_resolution(const lv_linux_drm_mode_t * mode);

/**
 * Получите частоту обновления режима DRM.
 * @param mode pointer to the DRM mode object
 * @return refresh rate in Hz, or 0 if mode is invalid
 */
int32_t lv_linux_drm_mode_get_refresh_rate(const lv_linux_drm_mode_t * mode);

/**
 * Проверьте, является ли режим DRM предпочтительным режимом для дисплея.
 * @param mode pointer to the DRM mode object
 * @return true if this is the preferred/native mode, false otherwise
 */
bool lv_linux_drm_mode_is_preferred(const lv_linux_drm_mode_t * mode);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_LINUX_DRM */

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /* LV_LINUX_DRM_H */
