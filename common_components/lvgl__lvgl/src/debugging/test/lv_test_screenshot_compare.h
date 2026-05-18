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
 * - If the reference image is not found it will be created automatically from the rendered screen.
 * - If the compare fails an `<image_name>_err.png` file will be created with the rendered content next to the reference image.
 *
 * Требуется lodepng.
 *
 * @param fn_ref    path to the reference image. Will be appended to REF_IMGS_PATH if set.
 * @return          An element of `lv_test_screenshot_result_t`
 * @note            This function assumes that the default display is the test display that was created by
 *                  `lv_test_display_create()`
 */
lv_test_screenshot_result_t lv_test_screenshot_compare(const char * fn_ref);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TEST_SCREENSHOT_COMPARE*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_TEST_SCREENSHOT_COMPARE_H*/
