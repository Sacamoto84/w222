/**
 * @file lv_demo_benchmark.h
 *
 */

#ifndef LV_DEMO_BENCHMARK_H
#define LV_DEMO_BENCHMARK_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_demos.h"

#if LV_USE_DEMO_BENCHMARK

/*********************
 *      DEFINES
 *********************/
#if LV_DEMO_BENCHMARK_ALIGNED_FONTS

LV_FONT_DECLARE(lv_font_benchmark_montserrat_12_aligned)
LV_FONT_DECLARE(lv_font_benchmark_montserrat_14_aligned)
LV_FONT_DECLARE(lv_font_benchmark_montserrat_16_aligned)
LV_FONT_DECLARE(lv_font_benchmark_montserrat_18_aligned)
LV_FONT_DECLARE(lv_font_benchmark_montserrat_20_aligned)
LV_FONT_DECLARE(lv_font_benchmark_montserrat_24_aligned)
LV_FONT_DECLARE(lv_font_benchmark_montserrat_26_aligned)

#else

/** Убедитесь, что выровненные шрифты не создаются, когда они не используются. */
#define LV_FONT_BENCHMARK_MONTSERRAT_12_ALIGNED 0
#define LV_FONT_BENCHMARK_MONTSERRAT_14_ALIGNED 0
#define LV_FONT_BENCHMARK_MONTSERRAT_16_ALIGNED 0
#define LV_FONT_BENCHMARK_MONTSERRAT_18_ALIGNED 0
#define LV_FONT_BENCHMARK_MONTSERRAT_20_ALIGNED 0
#define LV_FONT_BENCHMARK_MONTSERRAT_22_ALIGNED 0
#define LV_FONT_BENCHMARK_MONTSERRAT_24_ALIGNED 0
#define LV_FONT_BENCHMARK_MONTSERRAT_26_ALIGNED 0

#endif /*LV_DEMO_BENCHMARK_ALIGNED_FONTS*/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const char * name;
    void (*create_cb)(void);
    uint32_t scene_time;
    uint32_t cpu_avg_usage;
    uint32_t fps_avg;
    uint32_t render_avg_time;
    uint32_t flush_avg_time;
    uint32_t measurement_cnt;
} lv_demo_benchmark_scene_dsc_t;

typedef struct {
    /*
     * Список сцен
     * Последняя сцена в этом массиве сцен завершена.
     * сторожевой сцены, которая имеет`create_cb`== NULL
     * Не должно быть свободным
     */
    lv_demo_benchmark_scene_dsc_t * scenes;

    int32_t total_avg_fps;
    int32_t total_avg_cpu;
    int32_t total_avg_render_time;
    int32_t total_avg_flush_time;
    int32_t valid_scene_cnt; /* Количество сцен в`scenes`с`measurement_cnt`больше 0 */
} lv_demo_benchmark_summary_t;

typedef void (*lv_demo_benchmark_on_end_cb_t)(const lv_demo_benchmark_summary_t *);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Запустите все тестовые сцены.
 *
 * На конечном экране сводки значения интерпретируются следующим образом:
 * - Использование CPU:
 *    - Если`LV_SYSMON_GET_IDLE`не изменен, он измеряется на основе времени, проведенного в
 *      `lv_timer_handler` .
 *    - Если используется (RT) OS,`LV_SYSMON_GET_IDLE`можно изменить на пользовательскую функцию.
 *      который возвращает процент простоя простаивающей задачи.
 *
 * - FPS :LVGLпытался отрендерить это много раз в секунду. Это ограничено на основе `LV_DEF_REFR_PERIOD`.
 *
 * - Время рендеринга:LVGLпотратил столько времени только на рендеринг. Он не знает о выполнении задач,
 *   а просто измеряется разница во времени между началом и концом рендеринга
 *
 * - Время промывки: это сумма
 *     - время, проведенное в`flush_cb`и
 *     - время, потраченное на ожидание готовности флеша.
 */
void lv_demo_benchmark(void);

/*
 * Зарегистрируйте функцию для вызова после завершения демонстрационного теста.
 * @param cb    функция для вызова, когда демонстрация закончится
 */
void lv_demo_benchmark_set_end_cb(lv_demo_benchmark_on_end_cb_t cb);


/*
 * Отображение и запись сводки
 * Эта функция появляется автоматически, если`lv_on_benchmark_end_cb`не установлен.
 * @param summary   сводка результатов тестов
 */
void lv_demo_benchmark_summary_display(const lv_demo_benchmark_summary_t * summary);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_BENCHMARK*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_BENCHMARK_H*/
