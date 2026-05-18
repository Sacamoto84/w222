/**
 * @file lv_test_fs.h
 *
 */

#ifndef LV_TEST_FS_H
#define LV_TEST_FS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_TEST

#include "../../misc/lv_types.h"

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
 * Инициализируйте драйвер тестовой файловой системы
 */
void lv_test_fs_init(void);

/**
 * Установите, готова ли тестовая файловая система
 * @param ready true: готов, false: не готов
 */
void lv_test_fs_set_ready(bool ready);

/**
 * Установите, очищается ли обратный вызов открытия тестовой файловой системы.
 * @param is_clear правда: ясно, ложь: не ясно
 */
void lv_test_fs_clear_open_cb(bool is_clear);

/**
 * Установите, очищается ли обратный вызов закрытия тестовой файловой системы.
 * @param is_clear правда: ясно, ложь: не ясно
 */
void lv_test_fs_clear_close_cb(bool is_clear);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TEST*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_TEST_FS_H*/


