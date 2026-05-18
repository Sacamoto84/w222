/**
 * @file lv_test_helpers.h
 *
 */

#ifndef LV_TEST_HELPERS_H
#define LV_TEST_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_TEST

#include "../../misc/lv_types.h"
#include "../../stdlib/lv_mem.h"

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
 * Эмулировать задержку. Это не настоящая задержка, но LVGL заставляет думать, что
 * необходимое время истекло.
 * `lv_timer_handler` вызывается после каждой миллисекунды, то есть всех событий.
 * будет запущен внутри этой функции.
 * В конце анимация и отображение также будут обновлены.
 * @param ms    the number of milliseconds to pass
 */
void lv_test_wait(uint32_t ms);

/**
 * Эмулирует течение некоторого времени.
 * Обновляйте анимацию и отображение только один раз в конце.
 * @param ms    the number of milliseconds to pass
 */
void lv_test_fast_forward(uint32_t ms);

#if LV_USE_STDLIB_MALLOC != LV_STDLIB_BUILTIN
/* Пропустить проверку кучи, так как у нас нет доступной информации. */
#define LV_HEAP_CHECK(x) do {} while(0)
/* Выберите ненулевое значение */
#define lv_test_get_free_mem() (65536)
#else
#define LV_HEAP_CHECK(x) x

static inline size_t lv_test_get_free_mem(void)
{
    lv_mem_monitor_t m1;
    lv_mem_monitor(&m1);
    return m1.free_size;
}
#endif /* LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN */

/**********************
 *      MACROS
 **********************/

#define LV_TEST_WIDTH_TO_STRIDE(w, px_size) ((((w) * (px_size) + (LV_DRAW_BUF_STRIDE_ALIGN - 1)) / LV_DRAW_BUF_STRIDE_ALIGN) * LV_DRAW_BUF_STRIDE_ALIGN)

#endif /*LV_USE_TEST*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_TEST_HELPERS_H*/
