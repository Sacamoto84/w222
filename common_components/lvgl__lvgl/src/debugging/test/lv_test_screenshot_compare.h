/**
 * @file lv_test_screenshot_compare.h
 *
 */

#ifndef LV_TEST_SCREENSHOT_COMPARE_H
#define LV_TEST_SCREENSHOT_COMPARE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_TEST && defined(LV_USE_TEST_SCREENSHOT_COMPARE) && LV_USE_TEST_SCREENSHOT_COMPARE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Возвращаемое значение `lv_test_screenshot_compare`
 */
typedef enum {
    /**
     * Скриншот отличается от эталонного изображения.
     */
    LV_TEST_SCREENSHOT_RESULT_FAILED,

    /**
     * Скриншот такой же, как эталонное изображение.
     * Он также возвращается, если `LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE` включен.
     * и эталонное изображение отсутствовало.
     */
    LV_TEST_SCREENSHOT_RESULT_PASSED,

    /**
     * Если `LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE` не включен
     * и эталонное изображение отсутствует.
     */
    LV_TEST_SCREENSHOT_RESULT_NO_REFERENCE_IMAGE,

} lv_test_screenshot_result_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Сравните текущее содержимое тестового экрана с эталонным изображением PNG.
 * - Если эталонное изображение не найдено, оно будет создано автоматически из визуализированного экрана.
 * - Если сравнение не удалось, будет создан файл`<image_name>_err.png`с визуализированным содержимым рядом с эталонным изображением.
 *
 * Требуется lodepng.
 *
 * @param fn_ref    путь к эталонному изображению. Будет добавлен к REF_IMGS_PATH, если установлено.
 * @return          Элемент `lv_test_screenshot_result_t`
 * @note            Эта функция предполагает, что дисплеем по умолчанию является тестовый дисплей, созданный
 *                  `lv_test_display_create()`
 */
lv_test_screenshot_result_t lv_test_screenshot_compare(const char * fn_ref);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TEST_SCREENSHOT_COMPARE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEST_SCREENSHOT_COMPARE_H*/
