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
 * @param mode_count указатель на отображаемый объект
 * @param modes массив доступных режимов DRM
 * @param mode_count количество мод в массиве
 * @return индекс выбранного режима из массива режимов
 */
typedef size_t (*lv_linux_drm_select_mode_cb_t)(lv_display_t * disp,
                                                const lv_linux_drm_mode_t * modes,
                                                size_t mode_count);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Создайте новый дисплей Linux DRM.
 *
 * Создает идеи и формирует новый светильникLVGLс помощью LinuxDRM(Direct Rendering Manager).
 * подсистема вывода графики с аппаратным ускорением.
 *
 * @return Указатель на созданный экранный объект илиNULLв случае сбоя.
 */
lv_display_t * lv_linux_drm_create(void);

/**
 * @brief Настройте файл устройстваDRMи разъем для дисплея.
 *
 * Задает путь к файлу устройства DRM и разъем ID, которые будут использоваться для указанного дисплея.
 * Файл устройстваDRMобычно находится в каталоге /dev/dri/cardN, где N — номер карты.
 * Разъем ID указывает, какой физический выход (HDMI, VGA и т. д.) использовать.
 *
 * @param disp         Указатель на экранный объект, созданный с помощьюlv_linux_drm_create()
 * @param file         Путь к файлу устройстваDRM(например, «/dev/dri/card0»)
 * @param connector_id ID используемого разъемаDRMили -1 для автоматического выбора первого доступного
 * @return LV_RESULT_OK, если инициализация прошла успешно, или LV_RESULT_INVALID, если инициализация не удалась.
 */
lv_result_t lv_linux_drm_set_file(lv_display_t * disp, const char * file, int64_t connector_id);

/**
 * @brief Автоматически найти подходящий путь к устройству DRM
 *
 * Сканирует систему на наличие доступных устройств DRM и возвращает путь к подходящему
 * файл устройства, который можно использовать с lv_linux_drm_set_file().
 *
 * @return Динамически выделяемая строка, содержащая путь к устройству (необходимо освободить с помощью lv_free()),
 *         или NULL, если подходящее устройство не найдено
 */
char * lv_linux_drm_find_device_path(void);

/**
 * Установите функцию обратного вызова для выбора пользовательского режима DRM, чтобы переопределить поведение выбора режима по умолчанию.
 *
 * Поведение выбора режима по умолчанию — выбор собственного режима.
 *
 * @param disp указатель на отображаемый объект
 * @param callback функция, вызываемая, когда необходимо выбрать режим отображения,
 *                 или NULL, чтобы использовать поведение выбора режима по умолчанию.
 */
void lv_linux_drm_set_mode_cb(lv_display_t * disp, lv_linux_drm_select_mode_cb_t callback);

/**
 * Получите горизонтальное разрешение режима DRM.
 * @param mode указатель на объект режима DRM
 * @return горизонтальное разрешение в пикселях или 0, если режим недействителен
 */
int32_t lv_linux_drm_mode_get_horizontal_resolution(const lv_linux_drm_mode_t * mode);

/**
 * Получите вертикальное разрешение режима DRM.
 * @param mode указатель на объект режима DRM
 * @return вертикальное разрешение в пикселях или 0, если режим недействителен
 */
int32_t lv_linux_drm_mode_get_vertical_resolution(const lv_linux_drm_mode_t * mode);

/**
 * Получите частоту обновления режима DRM.
 * @param mode указатель на объект режима DRM
 * @return частота обновления в Гц или 0, если режим недействителен
 */
int32_t lv_linux_drm_mode_get_refresh_rate(const lv_linux_drm_mode_t * mode);

/**
 * Проверьте, является ли режим DRM предпочтительным режимом для дисплея.
 * @param mode указатель на объект режима DRM
 * @return true, если это предпочтительный/собственный режим, в противном случае — false
 */
bool lv_linux_drm_mode_is_preferred(const lv_linux_drm_mode_t * mode);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_LINUX_DRM */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_LINUX_DRM_H */
