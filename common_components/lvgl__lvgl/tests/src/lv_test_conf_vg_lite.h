#ifndef LV_TEST_CONF_VG_LITE_H
#define LV_TEST_CONF_VG_LITE_H

/* Используйте VG-Lite GPU. */
#define LV_USE_DRAW_VG_LITE 1

/* Включить пользовательскую внешнюю функциюVG-Lite «gpu_init()» */
#define LV_VG_LITE_USE_GPU_INIT 1

/* Включите утверждениеVG-Lite. */
#define LV_VG_LITE_USE_ASSERT 1

/* VG - Порог срабатывания облегченного сброса.  GPU попытается группировать эти многочисленные задачи по рисованию. */
#define LV_VG_LITE_FLUSH_MAX_COUNT 0

/* Включить границу для имитации тени
 * NOTE: что обычно повышает производительность,
 * но не гарантирует того же качества рендеринга, что и программное обеспечение. */
#define LV_VG_LITE_USE_BOX_SHADOW 1

/* VG - Максимальный номер кэша облегченного градиента.
 * NOTE: Использование памяти одним градиентным изображением составляет 4 КБ.
 */
#define LV_VG_LITE_GRAD_CACHE_CNT 32

/* Отключить выравнивание по 16 пикселей */
#define LV_VG_LITE_THORVG_16PIXELS_ALIGN 0

/* Включить рендеринг внутренних механизмов ThorVG */
#define LV_USE_THORVG_INTERNAL      1

/* Имитация оборудованияVG-Lite с помощью ThorVG */
#define LV_USE_VG_LITE_THORVG       1

/* Включить режим журнала монитора производительности для теста сборки */
#define LV_USE_PERF_MONITOR_LOG_MODE 1

/*Использование матрицы для преобразований.
 *Требования:
    `LV_USE_MATRIX = 1`.
    Механизм рендеринга должен поддерживать матричные преобразования 3x3.*/
#define LV_DRAW_TRANSFORM_USE_MATRIX            1

/* Используется для тестирования покрытия различных комбинаций конфигураций. */
#ifdef NON_AMD64_BUILD
    #define LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET  1
#else
    #define LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET  0
#endif

#endif /* LV_TEST_CONF_VG_LITE_H */
