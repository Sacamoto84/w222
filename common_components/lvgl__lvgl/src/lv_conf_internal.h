/**
 * GENERATED FILE, DO NOT EDIT IT!
 * @file lv_conf_internal.h
 * Этот файл гарантирует, что все определенияlv_conf.hимеют значение по умолчанию.
 */

#ifndef LV_CONF_INTERNAL_H
#define LV_CONF_INTERNAL_H
/* clang-format off */

/* Параметры конфигурации */
#define LV_OS_NONE          0
#define LV_OS_PTHREAD       1
#define LV_OS_FREERTOS      2
#define LV_OS_CMSIS_RTOS2   3
#define LV_OS_RTTHREAD      4
#define LV_OS_WINDOWS       5
#define LV_OS_MQX           6
#define LV_OS_SDL2          7
#define LV_OS_CUSTOM        255

#define LV_STDLIB_BUILTIN           0
#define LV_STDLIB_CLIB              1
#define LV_STDLIB_MICROPYTHON       2
#define LV_STDLIB_RTTHREAD          3
#define LV_STDLIB_CUSTOM            255

#define LV_DRAW_SW_ASM_NONE             0
#define LV_DRAW_SW_ASM_NEON             1
#define LV_DRAW_SW_ASM_HELIUM           2
#define LV_DRAW_SW_ASM_RISCV_V          3
#define LV_DRAW_SW_ASM_CUSTOM           255

#define LV_NEMA_LIB_NONE            0
#define LV_NEMA_LIB_M33_REVC        1
#define LV_NEMA_LIB_M33_NEMAPVG     2
#define LV_NEMA_LIB_M55             3
#define LV_NEMA_LIB_M7              4

#define LV_NEMA_HAL_CUSTOM          0
#define LV_NEMA_HAL_STM32           1

#define LV_NANOVG_BACKEND_GL2       1
#define LV_NANOVG_BACKEND_GL3       2
#define LV_NANOVG_BACKEND_GLES2     3
#define LV_NANOVG_BACKEND_GLES3     4

/** Обработка специальных параметров Kconfig. */
#ifndef LV_KCONFIG_IGNORE
    #include "lv_conf_kconfig.h"
    #if defined(CONFIG_LV_CONF_SKIP) && !defined(LV_CONF_SKIP)
        #define LV_CONF_SKIP
    #endif
#endif

/* Если «lv_conf.h» доступен здесь, используйте его форму позже. */
#ifdef __has_include
    #if __has_include("lv_conf.h")
        #ifndef LV_CONF_INCLUDE_SIMPLE
            #define LV_CONF_INCLUDE_SIMPLE
        #endif
    #endif
#endif

/* Еслиlv_conf.hне пропущен, включите его. */
#if !defined(LV_CONF_SKIP) || defined(LV_CONF_PATH)
    #ifdef LV_CONF_PATH                           /* Если дляlv_conf.hопределение слова, воспользуйтесь им. */
        #include LV_CONF_PATH                     /* Note: Обязательно определите пользовательскийCONF_PATHкак строку. */
    #elif defined(LV_CONF_INCLUDE_SIMPLE)         /* Или простоlv_conf.h. */
        #include "lv_conf.h"
    #else
        #include "../../lv_conf.h"                /* В противном случае предпочитаем, чтобыlv_conf.hнаходился рядом с папкой lvgl. */
    #endif
    #if !defined(LV_CONF_H) && !defined(LV_CONF_SUPPRESS_DEFINE_CHECK)
        /* #include will sometimes silently fail when __has_include is used */
        /* https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80753 */
        #pragma message("Possible failure to include lv_conf.h, please read the comment in this file if you get errors")
    #endif
#endif

#ifdef CONFIG_LV_COLOR_DEPTH
    #define LV_KCONFIG_PRESENT
#endif

/*----------------------------------
 * Начать разбор lv_conf_template.h
 -----------------------------------*/

/* Если вам нужно что-то включить сюда, сделайте это внутри защиты `__ASSEMBLY__`. */
#if  0 && defined(__ASSEMBLY__)
#include "my_include.h"
#endif

/*====================
   COLOR SETTINGS
 *====================*/

/** Глубина цвета: 1 ( I1 ), 8 ( L8 ), 16 ( RGB565 ), 24 ( RGB888 ), 32 ( XRGB8888 ) */
#ifndef LV_COLOR_DEPTH
    #ifdef CONFIG_LV_COLOR_DEPTH
        #define LV_COLOR_DEPTH CONFIG_LV_COLOR_DEPTH
    #else
        #define LV_COLOR_DEPTH 16
    #endif
#endif

/*=========================
   STDLIB WRAPPER SETTINGS
 *=========================*/

/** Возможные значения
 * - LV_STDLIB_BUILTIN: встроенная реализация LVGL.
 * - LV_STDLIB_CLIB: стандартные функции C, такие как malloc, strlen и т. д.
 * - LV_STDLIB_MICROPYTHON: реализация MicroPython.
 * - LV_STDLIB_RTTHREAD :RT— реализация потока
 * - LV_STDLIB_CUSTOM: внешняя реализация функций.
 */
#ifndef LV_USE_STDLIB_MALLOC
    #ifdef CONFIG_LV_USE_STDLIB_MALLOC
        #define LV_USE_STDLIB_MALLOC CONFIG_LV_USE_STDLIB_MALLOC
    #else
        #define LV_USE_STDLIB_MALLOC    LV_STDLIB_BUILTIN
    #endif
#endif

/** Возможные значения
 * - LV_STDLIB_BUILTIN: встроенная реализация LVGL.
 * - LV_STDLIB_CLIB: стандартные функции C, такие как malloc, strlen и т. д.
 * - LV_STDLIB_MICROPYTHON: реализация MicroPython.
 * - LV_STDLIB_RTTHREAD :RT— реализация потока
 * - LV_STDLIB_CUSTOM: внешняя реализация функций.
 */
#ifndef LV_USE_STDLIB_STRING
    #ifdef CONFIG_LV_USE_STDLIB_STRING
        #define LV_USE_STDLIB_STRING CONFIG_LV_USE_STDLIB_STRING
    #else
        #define LV_USE_STDLIB_STRING    LV_STDLIB_BUILTIN
    #endif
#endif

/** Возможные значения
 * - LV_STDLIB_BUILTIN: встроенная реализация LVGL.
 * - LV_STDLIB_CLIB: стандартные функции C, такие как malloc, strlen и т. д.
 * - LV_STDLIB_MICROPYTHON: реализация MicroPython.
 * - LV_STDLIB_RTTHREAD :RT— реализация потока
 * - LV_STDLIB_CUSTOM: внешняя реализация функций.
 */
#ifndef LV_USE_STDLIB_SPRINTF
    #ifdef CONFIG_LV_USE_STDLIB_SPRINTF
        #define LV_USE_STDLIB_SPRINTF CONFIG_LV_USE_STDLIB_SPRINTF
    #else
        #define LV_USE_STDLIB_SPRINTF   LV_STDLIB_BUILTIN
    #endif
#endif

#ifndef LV_STDINT_INCLUDE
    #ifdef CONFIG_LV_STDINT_INCLUDE
        #define LV_STDINT_INCLUDE CONFIG_LV_STDINT_INCLUDE
    #else
        #define LV_STDINT_INCLUDE       <stdint.h>
    #endif
#endif
#ifndef LV_STDDEF_INCLUDE
    #ifdef CONFIG_LV_STDDEF_INCLUDE
        #define LV_STDDEF_INCLUDE CONFIG_LV_STDDEF_INCLUDE
    #else
        #define LV_STDDEF_INCLUDE       <stddef.h>
    #endif
#endif
#ifndef LV_STDBOOL_INCLUDE
    #ifdef CONFIG_LV_STDBOOL_INCLUDE
        #define LV_STDBOOL_INCLUDE CONFIG_LV_STDBOOL_INCLUDE
    #else
        #define LV_STDBOOL_INCLUDE      <stdbool.h>
    #endif
#endif
#ifndef LV_INTTYPES_INCLUDE
    #ifdef CONFIG_LV_INTTYPES_INCLUDE
        #define LV_INTTYPES_INCLUDE CONFIG_LV_INTTYPES_INCLUDE
    #else
        #define LV_INTTYPES_INCLUDE     <inttypes.h>
    #endif
#endif
#ifndef LV_LIMITS_INCLUDE
    #ifdef CONFIG_LV_LIMITS_INCLUDE
        #define LV_LIMITS_INCLUDE CONFIG_LV_LIMITS_INCLUDE
    #else
        #define LV_LIMITS_INCLUDE       <limits.h>
    #endif
#endif
#ifndef LV_STDARG_INCLUDE
    #ifdef CONFIG_LV_STDARG_INCLUDE
        #define LV_STDARG_INCLUDE CONFIG_LV_STDARG_INCLUDE
    #else
        #define LV_STDARG_INCLUDE       <stdarg.h>
    #endif
#endif

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
    /** Размер доступной для`lv_malloc()`памяти в байтах (>= 2 КБ) */
    #ifndef LV_MEM_SIZE
        #ifdef CONFIG_LV_MEM_SIZE
            #define LV_MEM_SIZE CONFIG_LV_MEM_SIZE
        #else
            #define LV_MEM_SIZE (64 * 1024U)          /**< [bytes] */
        #endif
    #endif

    /** Размер расширения памяти для`lv_malloc()`в байтах */
    #ifndef LV_MEM_POOL_EXPAND_SIZE
        #ifdef CONFIG_LV_MEM_POOL_EXPAND_SIZE
            #define LV_MEM_POOL_EXPAND_SIZE CONFIG_LV_MEM_POOL_EXPAND_SIZE
        #else
            #define LV_MEM_POOL_EXPAND_SIZE 0
        #endif
    #endif

    /** Установите адрес пула памяти вместо того, чтобы выделять его как обычный массив. Может быть и во внешнем SRAM. */
    #ifndef LV_MEM_ADR
        #ifdef CONFIG_LV_MEM_ADR
            #define LV_MEM_ADR CONFIG_LV_MEM_ADR
        #else
            #define LV_MEM_ADR 0     /**< 0: unused*/
        #endif
    #endif
    /* Вместо этого адрес определяет распределение памяти, которое будет включать получение пула памяти дляLVGL. Например.   my_malloc */
    #if LV_MEM_ADR == 0
        #ifndef LV_MEM_POOL_INCLUDE
            #ifdef CONFIG_LV_MEM_POOL_INCLUDE
                #define LV_MEM_POOL_INCLUDE CONFIG_LV_MEM_POOL_INCLUDE
            #else
                #undef LV_MEM_POOL_INCLUDE
            #endif
        #endif
        #ifndef LV_MEM_POOL_ALLOC
            #ifdef CONFIG_LV_MEM_POOL_ALLOC
                #define LV_MEM_POOL_ALLOC CONFIG_LV_MEM_POOL_ALLOC
            #else
                #undef LV_MEM_POOL_ALLOC
            #endif
        #endif
    #endif
#endif  /*LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN*/

/*====================
   HAL SETTINGS
 *====================*/

/** Обновление дисплея по умолчанию, чтение с устройства ввода и период шага анимации. */
#ifndef LV_DEF_REFR_PERIOD
    #ifdef CONFIG_LV_DEF_REFR_PERIOD
        #define LV_DEF_REFR_PERIOD CONFIG_LV_DEF_REFR_PERIOD
    #else
        #define LV_DEF_REFR_PERIOD  33      /**< [ms] */
    #endif
#endif

/** Точек на дюйм по умолчанию. Используется для инициализации размеров по умолчанию, таких как размер виджетов и отступы стиля.
 * (Не так важно, вы можете настроить его, чтобы изменить размеры и пробелы по умолчанию.) */
#ifndef LV_DPI_DEF
    #ifdef CONFIG_LV_DPI_DEF
        #define LV_DPI_DEF CONFIG_LV_DPI_DEF
    #else
        #define LV_DPI_DEF 130              /**< [px/inch] */
    #endif
#endif

/*=================
 * OPERATING SYSTEM
 *=================*/
/** Выберите операционную систему для использования. Возможные варианты:
 * - LV_OS_NONE
 * - LV_OS_PTHREAD
 * - LV_OS_FREERTOS
 * - LV_OS_CMSIS_RTOS2
 * - LV_OS_RTTHREAD
 * - LV_OS_WINDOWS
 * - LV_OS_MQX
 * - LV_OS_SDL2
 * - LV_OS_CUSTOM */
#ifndef LV_USE_OS
    #ifdef CONFIG_LV_USE_OS
        #define LV_USE_OS CONFIG_LV_USE_OS
    #else
        #define LV_USE_OS   LV_OS_NONE
    #endif
#endif

#if LV_USE_OS == LV_OS_CUSTOM
    #ifndef LV_OS_CUSTOM_INCLUDE
        #ifdef CONFIG_LV_OS_CUSTOM_INCLUDE
            #define LV_OS_CUSTOM_INCLUDE CONFIG_LV_OS_CUSTOM_INCLUDE
        #else
            #define LV_OS_CUSTOM_INCLUDE <stdint.h>
        #endif
    #endif
#endif
#if LV_USE_OS == LV_OS_FREERTOS
    /*
     * Разблокировка задачи RTOS с помощью прямого уведомления происходит на 45 % быстрее и требует меньше RAM.
     * чем разблокировать задачу с помощью промежуточного объекта, такого как двоичный семафор.
     * Уведомления о задачах RTOS можно использовать только в том случае, если существует только одна задача, которая может быть получателем события.
     */
    #ifndef LV_USE_FREERTOS_TASK_NOTIFY
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_USE_FREERTOS_TASK_NOTIFY
                #define LV_USE_FREERTOS_TASK_NOTIFY CONFIG_LV_USE_FREERTOS_TASK_NOTIFY
            #else
                #define LV_USE_FREERTOS_TASK_NOTIFY 0
            #endif
        #else
            #define LV_USE_FREERTOS_TASK_NOTIFY 1
        #endif
    #endif
#endif

/*========================
 * RENDERING CONFIGURATION
 *========================*/

/** Выровнять шаг всех слоев и изображений по этим байтам. */
#ifndef LV_DRAW_BUF_STRIDE_ALIGN
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_BUF_STRIDE_ALIGN
            #define LV_DRAW_BUF_STRIDE_ALIGN CONFIG_LV_DRAW_BUF_STRIDE_ALIGN
        #else
            #define LV_DRAW_BUF_STRIDE_ALIGN 0
        #endif
    #else
        #define LV_DRAW_BUF_STRIDE_ALIGN                1
    #endif
#endif

/** Выровнять начальный адрес адресовdraw_bufпо этим байтам.*/
#ifndef LV_DRAW_BUF_ALIGN
    #ifdef CONFIG_LV_DRAW_BUF_ALIGN
        #define LV_DRAW_BUF_ALIGN CONFIG_LV_DRAW_BUF_ALIGN
    #else
        #define LV_DRAW_BUF_ALIGN                       4
    #endif
#endif

/** Использование матрицы для преобразований.
 * Требования:
 * - `LV_USE_MATRIX = 1`.
 * - Механизм рендеринга должен поддерживать матричные преобразования 3x3. */
#ifndef LV_DRAW_TRANSFORM_USE_MATRIX
    #ifdef CONFIG_LV_DRAW_TRANSFORM_USE_MATRIX
        #define LV_DRAW_TRANSFORM_USE_MATRIX CONFIG_LV_DRAW_TRANSFORM_USE_MATRIX
    #else
        #define LV_DRAW_TRANSFORM_USE_MATRIX            0
    #endif
#endif

/* Если виджет имеет режим наложения`style_opa < 255`(не`bg_opa`,`text_opa`и т. д.) или не NORMAL
 * перед рендерингом он буферизуется в «простой» слой. Виджет может быть буферизован небольшими частями.
 * «Преобразованные слои» (если установлены`transform_angle/zoom`) используют буферы большего размера.
 * и его нельзя рисовать частями. */

/** Размер целевого буфера для простых фрагментов слоя. */
#ifndef LV_DRAW_LAYER_SIMPLE_BUF_SIZE
    #ifdef CONFIG_LV_DRAW_LAYER_SIMPLE_BUF_SIZE
        #define LV_DRAW_LAYER_SIMPLE_BUF_SIZE CONFIG_LV_DRAW_LAYER_SIMPLE_BUF_SIZE
    #else
        #define LV_DRAW_LAYER_SIMPLE_BUF_SIZE    (24 * 1024)    /**< [bytes]*/
    #endif
#endif

/* Ограничьте максимальный объем выделенной памяти для простых и преобразованных слоев.
 * Его размер должен быть не менее `LV_DRAW_LAYER_SIMPLE_BUF_SIZE`, но если также используются преобразованные слои.
 * этого должно быть достаточно для хранения самого большого виджета (ширина x высота x 4 площади).
 * Установите значение 0, чтобы не иметь ограничений. */
#ifndef LV_DRAW_LAYER_MAX_MEMORY
    #ifdef CONFIG_LV_DRAW_LAYER_MAX_MEMORY
        #define LV_DRAW_LAYER_MAX_MEMORY CONFIG_LV_DRAW_LAYER_MAX_MEMORY
    #else
        #define LV_DRAW_LAYER_MAX_MEMORY 0  /**< No limit by default [bytes]*/
    #endif
#endif

/** Размер стопки ниток для рисования.
 * NOTE: Если FreeType или ThorVG включены, рекомендуется установить значение 32 КБ или более.
 */
#ifndef LV_DRAW_THREAD_STACK_SIZE
    #ifdef CONFIG_LV_DRAW_THREAD_STACK_SIZE
        #define LV_DRAW_THREAD_STACK_SIZE CONFIG_LV_DRAW_THREAD_STACK_SIZE
    #else
        #define LV_DRAW_THREAD_STACK_SIZE    (8 * 1024)         /**< [bytes]*/
    #endif
#endif

/** Приоритет потока задачи рисования.
 *  Более высокие значения означают более высокий приоритет.
 *  Можно использовать значения из перечисленияlv_thread_prio_tвlv_os.h:LV_THREAD_PRIO_LOWEST,
 *  LV_THREAD_PRIO_LOW, LV_THREAD_PRIO_MID, LV_THREAD_PRIO_HIGH, LV_THREAD_PRIO_HIGHEST
 *  Убедитесь, что значение приоритета соответствует уровням приоритета, специфичным для OS.
 *  Внимательность с приоритетом ограниченного уровня (например, FreeRTOS) более высокое значение может улучшиться
 *  производительность рендеринга, но может привести к зависанию других задач. */
#ifndef LV_DRAW_THREAD_PRIO
    #ifdef CONFIG_LV_DRAW_THREAD_PRIO
        #define LV_DRAW_THREAD_PRIO CONFIG_LV_DRAW_THREAD_PRIO
    #else
        #define LV_DRAW_THREAD_PRIO LV_THREAD_PRIO_HIGH
    #endif
#endif

#ifndef LV_USE_DRAW_SW
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_DRAW_SW
            #define LV_USE_DRAW_SW CONFIG_LV_USE_DRAW_SW
        #else
            #define LV_USE_DRAW_SW 0
        #endif
    #else
        #define LV_USE_DRAW_SW 1
    #endif
#endif
#if LV_USE_DRAW_SW == 1
    /*
     * Выборочно отключите поддержку цветового формата, чтобы уменьшить размер кода.
     * NOTE: некоторые функции используют определенные цветовые форматы внутри, например.
     * - градиенты используют RGB888
     * - растровые изображения с прозрачностью могут использовать ARGB8888
     */
    #ifndef LV_DRAW_SW_SUPPORT_RGB565
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_SUPPORT_RGB565
                #define LV_DRAW_SW_SUPPORT_RGB565 CONFIG_LV_DRAW_SW_SUPPORT_RGB565
            #else
                #define LV_DRAW_SW_SUPPORT_RGB565 0
            #endif
        #else
            #define LV_DRAW_SW_SUPPORT_RGB565       1
        #endif
    #endif
    #ifndef LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
                #define LV_DRAW_SW_SUPPORT_RGB565_SWAPPED CONFIG_LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
            #else
                #define LV_DRAW_SW_SUPPORT_RGB565_SWAPPED 0
            #endif
        #else
            #define LV_DRAW_SW_SUPPORT_RGB565_SWAPPED       1
        #endif
    #endif
    #ifndef LV_DRAW_SW_SUPPORT_RGB565A8
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_SUPPORT_RGB565A8
                #define LV_DRAW_SW_SUPPORT_RGB565A8 CONFIG_LV_DRAW_SW_SUPPORT_RGB565A8
            #else
                #define LV_DRAW_SW_SUPPORT_RGB565A8 0
            #endif
        #else
            #define LV_DRAW_SW_SUPPORT_RGB565A8     1
        #endif
    #endif
    #ifndef LV_DRAW_SW_SUPPORT_RGB888
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_SUPPORT_RGB888
                #define LV_DRAW_SW_SUPPORT_RGB888 CONFIG_LV_DRAW_SW_SUPPORT_RGB888
            #else
                #define LV_DRAW_SW_SUPPORT_RGB888 0
            #endif
        #else
            #define LV_DRAW_SW_SUPPORT_RGB888       1
        #endif
    #endif
    #ifndef LV_DRAW_SW_SUPPORT_XRGB8888
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_SUPPORT_XRGB8888
                #define LV_DRAW_SW_SUPPORT_XRGB8888 CONFIG_LV_DRAW_SW_SUPPORT_XRGB8888
            #else
                #define LV_DRAW_SW_SUPPORT_XRGB8888 0
            #endif
        #else
            #define LV_DRAW_SW_SUPPORT_XRGB8888     1
        #endif
    #endif
    #ifndef LV_DRAW_SW_SUPPORT_ARGB8888
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_SUPPORT_ARGB8888
                #define LV_DRAW_SW_SUPPORT_ARGB8888 CONFIG_LV_DRAW_SW_SUPPORT_ARGB8888
            #else
                #define LV_DRAW_SW_SUPPORT_ARGB8888 0
            #endif
        #else
            #define LV_DRAW_SW_SUPPORT_ARGB8888     1
        #endif
    #endif
    #ifndef LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
                #define LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED CONFIG_LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
            #else
                #define LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED 0
            #endif
        #else
            #define LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED 1
        #endif
    #endif
    #ifndef LV_DRAW_SW_SUPPORT_L8
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_SUPPORT_L8
                #define LV_DRAW_SW_SUPPORT_L8 CONFIG_LV_DRAW_SW_SUPPORT_L8
            #else
                #define LV_DRAW_SW_SUPPORT_L8 0
            #endif
        #else
            #define LV_DRAW_SW_SUPPORT_L8           1
        #endif
    #endif
    #ifndef LV_DRAW_SW_SUPPORT_AL88
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_SUPPORT_AL88
                #define LV_DRAW_SW_SUPPORT_AL88 CONFIG_LV_DRAW_SW_SUPPORT_AL88
            #else
                #define LV_DRAW_SW_SUPPORT_AL88 0
            #endif
        #else
            #define LV_DRAW_SW_SUPPORT_AL88         1
        #endif
    #endif
    #ifndef LV_DRAW_SW_SUPPORT_A8
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_SUPPORT_A8
                #define LV_DRAW_SW_SUPPORT_A8 CONFIG_LV_DRAW_SW_SUPPORT_A8
            #else
                #define LV_DRAW_SW_SUPPORT_A8 0
            #endif
        #else
            #define LV_DRAW_SW_SUPPORT_A8           1
        #endif
    #endif
    #ifndef LV_DRAW_SW_SUPPORT_I1
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_SUPPORT_I1
                #define LV_DRAW_SW_SUPPORT_I1 CONFIG_LV_DRAW_SW_SUPPORT_I1
            #else
                #define LV_DRAW_SW_SUPPORT_I1 0
            #endif
        #else
            #define LV_DRAW_SW_SUPPORT_I1           1
        #endif
    #endif

    /* Порог яркости, позволяющий считать пиксель
     * активен в индексированном цветовом формате */
    #ifndef LV_DRAW_SW_I1_LUM_THRESHOLD
        #ifdef CONFIG_LV_DRAW_SW_I1_LUM_THRESHOLD
            #define LV_DRAW_SW_I1_LUM_THRESHOLD CONFIG_LV_DRAW_SW_I1_LUM_THRESHOLD
        #else
            #define LV_DRAW_SW_I1_LUM_THRESHOLD 127
        #endif
    #endif

    /** Установите количество единиц рисования.
     *  - > 1 требует, чтобы операционная система была включена в `LV_USE_OS`.
     *  - > 1 означает, что несколько потоков будут отображать экран параллельно. */
    #ifndef LV_DRAW_SW_DRAW_UNIT_CNT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_DRAW_UNIT_CNT
                #define LV_DRAW_SW_DRAW_UNIT_CNT CONFIG_LV_DRAW_SW_DRAW_UNIT_CNT
            #else
                #define LV_DRAW_SW_DRAW_UNIT_CNT 0
            #endif
        #else
            #define LV_DRAW_SW_DRAW_UNIT_CNT    1
        #endif
    #endif

    /** Используйте Arm-2D для ускорения программного (программного) рендеринга. */
    #ifndef LV_USE_DRAW_ARM2D_SYNC
        #ifdef CONFIG_LV_USE_DRAW_ARM2D_SYNC
            #define LV_USE_DRAW_ARM2D_SYNC CONFIG_LV_USE_DRAW_ARM2D_SYNC
        #else
            #define LV_USE_DRAW_ARM2D_SYNC      0
        #endif
    #endif

    /** Включите компиляцию самостоятельной сборки Helium. */
    #ifndef LV_USE_NATIVE_HELIUM_ASM
        #ifdef CONFIG_LV_USE_NATIVE_HELIUM_ASM
            #define LV_USE_NATIVE_HELIUM_ASM CONFIG_LV_USE_NATIVE_HELIUM_ASM
        #else
            #define LV_USE_NATIVE_HELIUM_ASM    0
        #endif
    #endif

    /**
     * - 0: использовать простой рендерер, способный рисовать только простые прямоугольники с градиентом, изображениями, текстом и прямыми линиями.
     * - 1. Используйте сложный рендерер, способный рисовать закругленные углы, тени, наклонные линии и дуги. */
    #ifndef LV_DRAW_SW_COMPLEX
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_COMPLEX
                #define LV_DRAW_SW_COMPLEX CONFIG_LV_DRAW_SW_COMPLEX
            #else
                #define LV_DRAW_SW_COMPLEX 0
            #endif
        #else
            #define LV_DRAW_SW_COMPLEX          1
        #endif
    #endif

    #if LV_DRAW_SW_COMPLEX == 1
        /** Разрешить буферизацию некоторых вычислений теней.
         *  LV_DRAW_SW_SHADOW_CACHE_SIZE — максимальный размер тени в буфере, где размер тени равен
         *  `shadow_width + radius` .  Кэширование имеет стоимостьLV_DRAW_SW_SHADOW_CACHE_SIZE^2RAM. */
        #ifndef LV_DRAW_SW_SHADOW_CACHE_SIZE
            #ifdef CONFIG_LV_DRAW_SW_SHADOW_CACHE_SIZE
                #define LV_DRAW_SW_SHADOW_CACHE_SIZE CONFIG_LV_DRAW_SW_SHADOW_CACHE_SIZE
            #else
                #define LV_DRAW_SW_SHADOW_CACHE_SIZE 0
            #endif
        #endif

        /** Установите количество максимально кэшируемых данных круга.
         *  Окружность 1/4 круга сохраняется для сглаживания.
         *  По кругу используются байты`radius * 4`(наиболее часто используются радиусы направления).
         *  - 0: отключает кэширование */
        #ifndef LV_DRAW_SW_CIRCLE_CACHE_SIZE
            #ifdef CONFIG_LV_DRAW_SW_CIRCLE_CACHE_SIZE
                #define LV_DRAW_SW_CIRCLE_CACHE_SIZE CONFIG_LV_DRAW_SW_CIRCLE_CACHE_SIZE
            #else
                #define LV_DRAW_SW_CIRCLE_CACHE_SIZE 4
            #endif
        #endif
    #endif

    #ifndef LV_USE_DRAW_SW_ASM
        #ifdef CONFIG_LV_USE_DRAW_SW_ASM
            #define LV_USE_DRAW_SW_ASM CONFIG_LV_USE_DRAW_SW_ASM
        #else
            #define  LV_USE_DRAW_SW_ASM     LV_DRAW_SW_ASM_NONE
        #endif
    #endif

    #if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_CUSTOM
        #ifndef LV_DRAW_SW_ASM_CUSTOM_INCLUDE
            #ifdef CONFIG_LV_DRAW_SW_ASM_CUSTOM_INCLUDE
                #define LV_DRAW_SW_ASM_CUSTOM_INCLUDE CONFIG_LV_DRAW_SW_ASM_CUSTOM_INCLUDE
            #else
                #define  LV_DRAW_SW_ASM_CUSTOM_INCLUDE ""
            #endif
        #endif
    #endif

    /** Включите возможность рисования в программе сложных градиентов: линейных под углом, радиальных или конических. */
    #ifndef LV_USE_DRAW_SW_COMPLEX_GRADIENTS
        #ifdef CONFIG_LV_USE_DRAW_SW_COMPLEX_GRADIENTS
            #define LV_USE_DRAW_SW_COMPLEX_GRADIENTS CONFIG_LV_USE_DRAW_SW_COMPLEX_GRADIENTS
        #else
            #define LV_USE_DRAW_SW_COMPLEX_GRADIENTS    0
        #endif
    #endif

#endif

/*Используйте TSi, он же (Think Silicon) NemaGFX. */
#ifndef LV_USE_NEMA_GFX
    #ifdef CONFIG_LV_USE_NEMA_GFX
        #define LV_USE_NEMA_GFX CONFIG_LV_USE_NEMA_GFX
    #else
        #define LV_USE_NEMA_GFX 0
    #endif
#endif

#if LV_USE_NEMA_GFX
    /** Выберите, какие заголовки используют статическую библиотеку NemaGFX. Возможные варианты:
     * - LV_NEMA_LIB_NONE — псевдоним LV_NEMA_LIB_M33_REVC.
     * - LV_NEMA_LIB_M33_REVC
     * - LV_NEMA_LIB_M33_NEMAPVG
     * - LV_NEMA_LIB_M55
     * - LV_NEMA_LIB_M7
     * Вы также должны позаботиться о том, чтобы связать правильную соответствующую статическую библиотеку.
     * в библиотеках/nema_gfx /lib/core/
     */
    #ifndef LV_USE_NEMA_LIB
        #ifdef CONFIG_LV_USE_NEMA_LIB
            #define LV_USE_NEMA_LIB CONFIG_LV_USE_NEMA_LIB
        #else
            #define LV_USE_NEMA_LIB LV_NEMA_LIB_NONE
        #endif
    #endif

    /** Выберите, какой NemaGFXHALиспользовать. Возможные варианты:
     * - LV_NEMA_HAL_CUSTOM
     * - LV_NEMA_HAL_STM32 */
    #ifndef LV_USE_NEMA_HAL
        #ifdef CONFIG_LV_USE_NEMA_HAL
            #define LV_USE_NEMA_HAL CONFIG_LV_USE_NEMA_HAL
        #else
            #define LV_USE_NEMA_HAL LV_NEMA_HAL_CUSTOM
        #endif
    #endif
    #if LV_USE_NEMA_HAL == LV_NEMA_HAL_STM32
        #ifndef LV_NEMA_STM32_HAL_INCLUDE
            #ifdef CONFIG_LV_NEMA_STM32_HAL_INCLUDE
                #define LV_NEMA_STM32_HAL_INCLUDE CONFIG_LV_NEMA_STM32_HAL_INCLUDE
            #else
                #define LV_NEMA_STM32_HAL_INCLUDE <stm32u5xx_hal.h>
            #endif
        #endif

        /** Подберите для него значение типа__attribute__((section("Nemagfx_Memory_Pool_Buffer")))
         * и определите раздел в скрипте компоновщика, если вам нужна память GPU для
         * быть, например в регионе, где доступы не будут кэшироваться.
         */
        #ifndef LV_NEMA_STM32_HAL_ATTRIBUTE_POOL_MEM
            #ifdef CONFIG_LV_NEMA_STM32_HAL_ATTRIBUTE_POOL_MEM
                #define LV_NEMA_STM32_HAL_ATTRIBUTE_POOL_MEM CONFIG_LV_NEMA_STM32_HAL_ATTRIBUTE_POOL_MEM
            #else
                #define LV_NEMA_STM32_HAL_ATTRIBUTE_POOL_MEM
            #endif
        #endif
    #endif

    /*Включите операцию с векторной графикой. Доступно только в наличии библиотеки NemaVG.*/
    #ifndef LV_USE_NEMA_VG
        #ifdef CONFIG_LV_USE_NEMA_VG
            #define LV_USE_NEMA_VG CONFIG_LV_USE_NEMA_VG
        #else
            #define LV_USE_NEMA_VG 0
        #endif
    #endif
    #if LV_USE_NEMA_VG
        /*Определите разрешение приложения, используемое для выделения буфера, связанного с VG. */
        #ifndef LV_NEMA_GFX_MAX_RESX
            #ifdef CONFIG_LV_NEMA_GFX_MAX_RESX
                #define LV_NEMA_GFX_MAX_RESX CONFIG_LV_NEMA_GFX_MAX_RESX
            #else
                #define LV_NEMA_GFX_MAX_RESX 800
            #endif
        #endif
        #ifndef LV_NEMA_GFX_MAX_RESY
            #ifdef CONFIG_LV_NEMA_GFX_MAX_RESY
                #define LV_NEMA_GFX_MAX_RESY CONFIG_LV_NEMA_GFX_MAX_RESY
            #else
                #define LV_NEMA_GFX_MAX_RESY 600
            #endif
        #endif
    #endif
#endif

/** ИспользуйтеPXPNXP на платформах iMX RTxxx. */
#ifndef LV_USE_PXP
    #ifdef CONFIG_LV_USE_PXP
        #define LV_USE_PXP CONFIG_LV_USE_PXP
    #else
        #define LV_USE_PXP 0
    #endif
#endif

#if LV_USE_PXP
    /** Используйте PXP для рисования.*/
    #ifndef LV_USE_DRAW_PXP
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_USE_DRAW_PXP
                #define LV_USE_DRAW_PXP CONFIG_LV_USE_DRAW_PXP
            #else
                #define LV_USE_DRAW_PXP 0
            #endif
        #else
            #define LV_USE_DRAW_PXP 1
        #endif
    #endif

    /** Используйте PXP для поворота дисплея.*/
    #ifndef LV_USE_ROTATE_PXP
        #ifdef CONFIG_LV_USE_ROTATE_PXP
            #define LV_USE_ROTATE_PXP CONFIG_LV_USE_ROTATE_PXP
        #else
            #define LV_USE_ROTATE_PXP 0
        #endif
    #endif

    #if LV_USE_DRAW_PXP && LV_USE_OS
        /** Используйте дополнительную нить для обработки PXP.*/
        #ifndef LV_USE_PXP_DRAW_THREAD
            #ifdef LV_KCONFIG_PRESENT
                #ifdef CONFIG_LV_USE_PXP_DRAW_THREAD
                    #define LV_USE_PXP_DRAW_THREAD CONFIG_LV_USE_PXP_DRAW_THREAD
                #else
                    #define LV_USE_PXP_DRAW_THREAD 0
                #endif
            #else
                #define LV_USE_PXP_DRAW_THREAD 1
            #endif
        #endif
    #endif

    /** Включить утверждения PXP. */
    #ifndef LV_USE_PXP_ASSERT
        #ifdef CONFIG_LV_USE_PXP_ASSERT
            #define LV_USE_PXP_ASSERT CONFIG_LV_USE_PXP_ASSERT
        #else
            #define LV_USE_PXP_ASSERT 0
        #endif
    #endif
#endif

/** Используйте G2D NXP на платформах MPU. */
#ifndef LV_USE_G2D
    #ifdef CONFIG_LV_USE_G2D
        #define LV_USE_G2D CONFIG_LV_USE_G2D
    #else
        #define LV_USE_G2D 0
    #endif
#endif

#if LV_USE_G2D
    /** Используйте G2D для рисования. **/
    #ifndef LV_USE_DRAW_G2D
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_USE_DRAW_G2D
                #define LV_USE_DRAW_G2D CONFIG_LV_USE_DRAW_G2D
            #else
                #define LV_USE_DRAW_G2D 0
            #endif
        #else
            #define LV_USE_DRAW_G2D 1
        #endif
    #endif

    /** Используйте G2D для поворота дисплея. **/
    #ifndef LV_USE_ROTATE_G2D
        #ifdef CONFIG_LV_USE_ROTATE_G2D
            #define LV_USE_ROTATE_G2D CONFIG_LV_USE_ROTATE_G2D
        #else
            #define LV_USE_ROTATE_G2D 0
        #endif
    #endif

    /** Максимальное количество буферов, которые можно сохранить для блока рисования G2D.
     *  Включает буферы кадров и ресурсы. */
    #ifndef LV_G2D_HASH_TABLE_SIZE
        #ifdef CONFIG_LV_G2D_HASH_TABLE_SIZE
            #define LV_G2D_HASH_TABLE_SIZE CONFIG_LV_G2D_HASH_TABLE_SIZE
        #else
            #define LV_G2D_HASH_TABLE_SIZE 50
        #endif
    #endif

    #if LV_USE_DRAW_G2D && LV_USE_OS
        /** Используйте дополнительную нить для обработки G2D.*/
        #ifndef LV_USE_G2D_DRAW_THREAD
            #ifdef LV_KCONFIG_PRESENT
                #ifdef CONFIG_LV_USE_G2D_DRAW_THREAD
                    #define LV_USE_G2D_DRAW_THREAD CONFIG_LV_USE_G2D_DRAW_THREAD
                #else
                    #define LV_USE_G2D_DRAW_THREAD 0
                #endif
            #else
                #define LV_USE_G2D_DRAW_THREAD 1
            #endif
        #endif
    #endif

    /** Включить утверждения G2D. */
    #ifndef LV_USE_G2D_ASSERT
        #ifdef CONFIG_LV_USE_G2D_ASSERT
            #define LV_USE_G2D_ASSERT CONFIG_LV_USE_G2D_ASSERT
        #else
            #define LV_USE_G2D_ASSERT 0
        #endif
    #endif
#endif

/** Используйте Renesas Dave2D на платформе RA. */
#ifndef LV_USE_DRAW_DAVE2D
    #ifdef CONFIG_LV_USE_DRAW_DAVE2D
        #define LV_USE_DRAW_DAVE2D CONFIG_LV_USE_DRAW_DAVE2D
    #else
        #define LV_USE_DRAW_DAVE2D 0
    #endif
#endif

/** Рисуйте, используя кэшированные текстуры SDL.*/
#ifndef LV_USE_DRAW_SDL
    #ifdef CONFIG_LV_USE_DRAW_SDL
        #define LV_USE_DRAW_SDL CONFIG_LV_USE_DRAW_SDL
    #else
        #define LV_USE_DRAW_SDL 0
    #endif
#endif

/** Используйте VG-Lite GPU. */
#ifndef LV_USE_DRAW_VG_LITE
    #ifdef CONFIG_LV_USE_DRAW_VG_LITE
        #define LV_USE_DRAW_VG_LITE CONFIG_LV_USE_DRAW_VG_LITE
    #else
        #define LV_USE_DRAW_VG_LITE 0
    #endif
#endif
#if LV_USE_DRAW_VG_LITE
    /** Включить пользовательскую внешнюю функциюVG-Lite «gpu_init()» */
    #ifndef LV_VG_LITE_USE_GPU_INIT
        #ifdef CONFIG_LV_VG_LITE_USE_GPU_INIT
            #define LV_VG_LITE_USE_GPU_INIT CONFIG_LV_VG_LITE_USE_GPU_INIT
        #else
            #define LV_VG_LITE_USE_GPU_INIT 0
        #endif
    #endif

    /** Включите утверждениеVG-Lite. */
    #ifndef LV_VG_LITE_USE_ASSERT
        #ifdef CONFIG_LV_VG_LITE_USE_ASSERT
            #define LV_VG_LITE_USE_ASSERT CONFIG_LV_VG_LITE_USE_ASSERT
        #else
            #define LV_VG_LITE_USE_ASSERT 0
        #endif
    #endif

    /** VG - Порог срабатывания облегченного сброса.  GPU попытается группировать эти многочисленные задачи по рисованию. */
    #ifndef LV_VG_LITE_FLUSH_MAX_COUNT
        #ifdef CONFIG_LV_VG_LITE_FLUSH_MAX_COUNT
            #define LV_VG_LITE_FLUSH_MAX_COUNT CONFIG_LV_VG_LITE_FLUSH_MAX_COUNT
        #else
            #define LV_VG_LITE_FLUSH_MAX_COUNT 8
        #endif
    #endif

    /** Включите границу для имитации тени.
     *  NOTE: что обычно повышает производительность,
     *  но не гарантирует того же качества рендеринга, что и программное обеспечение. */
    #ifndef LV_VG_LITE_USE_BOX_SHADOW
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_VG_LITE_USE_BOX_SHADOW
                #define LV_VG_LITE_USE_BOX_SHADOW CONFIG_LV_VG_LITE_USE_BOX_SHADOW
            #else
                #define LV_VG_LITE_USE_BOX_SHADOW 0
            #endif
        #else
            #define LV_VG_LITE_USE_BOX_SHADOW 1
        #endif
    #endif

    /** VG - Максимальный номер кэша облегченного градиента.
     *  @note  Использование памяти одним градиентным изображением составляет 4 КБ. */
    #ifndef LV_VG_LITE_GRAD_CACHE_CNT
        #ifdef CONFIG_LV_VG_LITE_GRAD_CACHE_CNT
            #define LV_VG_LITE_GRAD_CACHE_CNT CONFIG_LV_VG_LITE_GRAD_CACHE_CNT
        #else
            #define LV_VG_LITE_GRAD_CACHE_CNT 32
        #endif
    #endif

    /** VG - Максимальный номер кэша Lite. */
    #ifndef LV_VG_LITE_STROKE_CACHE_CNT
        #ifdef CONFIG_LV_VG_LITE_STROKE_CACHE_CNT
            #define LV_VG_LITE_STROKE_CACHE_CNT CONFIG_LV_VG_LITE_STROKE_CACHE_CNT
        #else
            #define LV_VG_LITE_STROKE_CACHE_CNT 32
        #endif
    #endif

    /** VG - Максимальный номер кэша невыровненного растрового шрифта Lite. */
    #ifndef LV_VG_LITE_BITMAP_FONT_CACHE_CNT
        #ifdef CONFIG_LV_VG_LITE_BITMAP_FONT_CACHE_CNT
            #define LV_VG_LITE_BITMAP_FONT_CACHE_CNT CONFIG_LV_VG_LITE_BITMAP_FONT_CACHE_CNT
        #else
            #define LV_VG_LITE_BITMAP_FONT_CACHE_CNT 256
        #endif
    #endif

    /** Удалить инструкцию пути VLC_OP_CLOSE (обходной путь для NXP) **/
    #ifndef LV_VG_LITE_DISABLE_VLC_OP_CLOSE
        #ifdef CONFIG_LV_VG_LITE_DISABLE_VLC_OP_CLOSE
            #define LV_VG_LITE_DISABLE_VLC_OP_CLOSE CONFIG_LV_VG_LITE_DISABLE_VLC_OP_CLOSE
        #else
            #define LV_VG_LITE_DISABLE_VLC_OP_CLOSE 0
        #endif
    #endif

    /** Отключите прямоугольное смещение, чтобы устранить аппаратные ошибки. */
    #ifndef LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET
        #ifdef CONFIG_LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET
            #define LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET CONFIG_LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET
        #else
            #define LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET 0
        #endif
    #endif

    /** Отключите расширение линейного градиента для некоторых старых версий драйверов. */
    #ifndef LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT
        #ifdef CONFIG_LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT
            #define LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT CONFIG_LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT
        #else
            #define LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT 0
        #endif
    #endif

    /** Максимальная длина печати дампа пути (в пунктах) */
    #ifndef LV_VG_LITE_PATH_DUMP_MAX_LEN
        #ifdef CONFIG_LV_VG_LITE_PATH_DUMP_MAX_LEN
            #define LV_VG_LITE_PATH_DUMP_MAX_LEN CONFIG_LV_VG_LITE_PATH_DUMP_MAX_LEN
        #else
            #define LV_VG_LITE_PATH_DUMP_MAX_LEN 1000
        #endif
    #endif

    /** Выключите использование встроенного драйвераvg_liteLVGL. */
    #ifndef LV_USE_VG_LITE_DRIVER
        #ifdef CONFIG_LV_USE_VG_LITE_DRIVER
            #define LV_USE_VG_LITE_DRIVER CONFIG_LV_USE_VG_LITE_DRIVER
        #else
            #define LV_USE_VG_LITE_DRIVER  0
        #endif
    #endif
    #if LV_USE_VG_LITE_DRIVER
        /** Используется для выбора правильной папки серии GPU. Допустимые параметры: gc255, gc355 и gc555.*/
        #ifndef LV_VG_LITE_HAL_GPU_SERIES
            #ifdef CONFIG_LV_VG_LITE_HAL_GPU_SERIES
                #define LV_VG_LITE_HAL_GPU_SERIES CONFIG_LV_VG_LITE_HAL_GPU_SERIES
            #else
                #define LV_VG_LITE_HAL_GPU_SERIES gc255
            #endif
        #endif

        /** Используется для выбора правильного заголовка версии GPU, это зависит от поставщика. */
        #ifndef LV_VG_LITE_HAL_GPU_REVISION
            #ifdef CONFIG_LV_VG_LITE_HAL_GPU_REVISION
                #define LV_VG_LITE_HAL_GPU_REVISION CONFIG_LV_VG_LITE_HAL_GPU_REVISION
            #else
                #define LV_VG_LITE_HAL_GPU_REVISION 0x40
            #endif
        #endif

        /** Память базового адресаGPUIP зависит от SoC,
         *  значение по умолчанию — для устройств на базе NXP. */
        #ifndef LV_VG_LITE_HAL_GPU_BASE_ADDRESS
            #ifdef CONFIG_LV_VG_LITE_HAL_GPU_BASE_ADDRESS
                #define LV_VG_LITE_HAL_GPU_BASE_ADDRESS CONFIG_LV_VG_LITE_HAL_GPU_BASE_ADDRESS
            #else
                #define LV_VG_LITE_HAL_GPU_BASE_ADDRESS 0x40240000
            #endif
        #endif
    #endif /*LV_USE_VG_LITE_DRIVER*/

    /** Используйте ThorVG (программную векторную библиотеку) в качестве драйвераVG-Lite, чтобы можно было протестировать VGLite на PC.
     *  Требуется: LV_USE_THORVG_INTERNAL или LV_USE_THORVG_EXTERNAL. */
    #ifndef LV_USE_VG_LITE_THORVG
        #ifdef CONFIG_LV_USE_VG_LITE_THORVG
            #define LV_USE_VG_LITE_THORVG CONFIG_LV_USE_VG_LITE_THORVG
        #else
            #define LV_USE_VG_LITE_THORVG   0
        #endif
    #endif
    #if LV_USE_VG_LITE_THORVG
        /** Включите поддержку режима наложения LVGL. */
        #ifndef LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT
            #ifdef CONFIG_LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT
                #define LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT CONFIG_LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT
            #else
                #define LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT 0
            #endif
        #endif

        /** Включить поддержку цветового формата YUV */
        #ifndef LV_VG_LITE_THORVG_YUV_SUPPORT
            #ifdef CONFIG_LV_VG_LITE_THORVG_YUV_SUPPORT
                #define LV_VG_LITE_THORVG_YUV_SUPPORT CONFIG_LV_VG_LITE_THORVG_YUV_SUPPORT
            #else
                #define LV_VG_LITE_THORVG_YUV_SUPPORT 0
            #endif
        #endif

        /** Включить поддержку расширения линейного градиента */
        #ifndef LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT
            #ifdef CONFIG_LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT
                #define LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT CONFIG_LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT
            #else
                #define LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT 0
            #endif
        #endif

        /** Включить выравнивание по 16 пикселям */
        #ifndef LV_VG_LITE_THORVG_16PIXELS_ALIGN
            #ifdef LV_KCONFIG_PRESENT
                #ifdef CONFIG_LV_VG_LITE_THORVG_16PIXELS_ALIGN
                    #define LV_VG_LITE_THORVG_16PIXELS_ALIGN CONFIG_LV_VG_LITE_THORVG_16PIXELS_ALIGN
                #else
                    #define LV_VG_LITE_THORVG_16PIXELS_ALIGN 0
                #endif
            #else
                #define LV_VG_LITE_THORVG_16PIXELS_ALIGN 1
            #endif
        #endif

        /** Выравнивание адреса буфера */
        #ifndef LV_VG_LITE_THORVG_BUF_ADDR_ALIGN
            #ifdef CONFIG_LV_VG_LITE_THORVG_BUF_ADDR_ALIGN
                #define LV_VG_LITE_THORVG_BUF_ADDR_ALIGN CONFIG_LV_VG_LITE_THORVG_BUF_ADDR_ALIGN
            #else
                #define LV_VG_LITE_THORVG_BUF_ADDR_ALIGN 64
            #endif
        #endif

        /** Включить многопоточный рендеринг */
        #ifndef LV_VG_LITE_THORVG_THREAD_RENDER
            #ifdef CONFIG_LV_VG_LITE_THORVG_THREAD_RENDER
                #define LV_VG_LITE_THORVG_THREAD_RENDER CONFIG_LV_VG_LITE_THORVG_THREAD_RENDER
            #else
                #define LV_VG_LITE_THORVG_THREAD_RENDER 0
            #endif
        #endif
    #endif /*LV_USE_VG_LITE_THORVG*/
#endif

/** Ускорьте смешивание, заливку и т. д. с помощью STM32 DMA2D. */
#ifndef LV_USE_DRAW_DMA2D
    #ifdef CONFIG_LV_USE_DRAW_DMA2D
        #define LV_USE_DRAW_DMA2D CONFIG_LV_USE_DRAW_DMA2D
    #else
        #define LV_USE_DRAW_DMA2D 0
    #endif
#endif
#if LV_USE_DRAW_DMA2D
    #ifndef LV_DRAW_DMA2D_HAL_INCLUDE
        #ifdef CONFIG_LV_DRAW_DMA2D_HAL_INCLUDE
            #define LV_DRAW_DMA2D_HAL_INCLUDE CONFIG_LV_DRAW_DMA2D_HAL_INCLUDE
        #else
            #define LV_DRAW_DMA2D_HAL_INCLUDE "stm32h7xx_hal.h"
        #endif
    #endif

    /* если включено, пользователю необходимо вызвать `lv_draw_dma2d_transfer_complete_interrupt_handler`
     * при получении глобального прерывания DMA2D
     */
    #ifndef LV_USE_DRAW_DMA2D_INTERRUPT
        #ifdef CONFIG_LV_USE_DRAW_DMA2D_INTERRUPT
            #define LV_USE_DRAW_DMA2D_INTERRUPT CONFIG_LV_USE_DRAW_DMA2D_INTERRUPT
        #else
            #define LV_USE_DRAW_DMA2D_INTERRUPT 0
        #endif
    #endif
#endif

/** Рисуйте, используя кэшированные текстуры OpenGLES. Требуется LV_USE_OPENGLES */
#ifndef LV_USE_DRAW_OPENGLES
    #ifdef CONFIG_LV_USE_DRAW_OPENGLES
        #define LV_USE_DRAW_OPENGLES CONFIG_LV_USE_DRAW_OPENGLES
    #else
        #define LV_USE_DRAW_OPENGLES 0
    #endif
#endif
#if LV_USE_DRAW_OPENGLES
    #ifndef LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT
        #ifdef CONFIG_LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT
            #define LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT CONFIG_LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT
        #else
            #define LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT 64
        #endif
    #endif
#endif

/** Рисуйте с помощью ускорителя espressif PPA. */
#ifndef LV_USE_PPA
    #ifdef CONFIG_LV_USE_PPA
        #define LV_USE_PPA CONFIG_LV_USE_PPA
    #else
        #define LV_USE_PPA  0
    #endif
#endif
#if LV_USE_PPA
    #ifndef LV_USE_PPA_IMG
        #ifdef CONFIG_LV_USE_PPA_IMG
            #define LV_USE_PPA_IMG CONFIG_LV_USE_PPA_IMG
        #else
            #define LV_USE_PPA_IMG      0
        #endif
    #endif
    #ifndef LV_PPA_BURST_LENGTH
        #ifdef CONFIG_LV_PPA_BURST_LENGTH
            #define LV_PPA_BURST_LENGTH CONFIG_LV_PPA_BURST_LENGTH
        #else
            #define LV_PPA_BURST_LENGTH    128
        #endif
    #endif
#endif

/* Используйте EVE FT81X GPU . */
#ifndef LV_USE_DRAW_EVE
    #ifdef CONFIG_LV_USE_DRAW_EVE
        #define LV_USE_DRAW_EVE CONFIG_LV_USE_DRAW_EVE
    #else
        #define LV_USE_DRAW_EVE 0
    #endif
#endif
#if LV_USE_DRAW_EVE
    /* Значение EVE_GEN: 2, 3 или 4. */
    #ifndef LV_DRAW_EVE_EVE_GENERATION
        #ifdef CONFIG_LV_DRAW_EVE_EVE_GENERATION
            #define LV_DRAW_EVE_EVE_GENERATION CONFIG_LV_DRAW_EVE_EVE_GENERATION
        #else
            #define LV_DRAW_EVE_EVE_GENERATION 4
        #endif
    #endif

    /* Максимальное количество байтов для буферизации перед одной передачей SPI.
     * Установите значение 0, чтобы отключить буферизацию записи.
     */
    #ifndef LV_DRAW_EVE_WRITE_BUFFER_SIZE
        #ifdef CONFIG_LV_DRAW_EVE_WRITE_BUFFER_SIZE
            #define LV_DRAW_EVE_WRITE_BUFFER_SIZE CONFIG_LV_DRAW_EVE_WRITE_BUFFER_SIZE
        #else
            #define LV_DRAW_EVE_WRITE_BUFFER_SIZE 2048
        #endif
    #endif
#endif

/** Используйте рендерер NanoVG
 * - Требуется LV_USE_NANOVG, LV_USE_MATRIX.
 */
#ifndef LV_USE_DRAW_NANOVG
    #ifdef CONFIG_LV_USE_DRAW_NANOVG
        #define LV_USE_DRAW_NANOVG CONFIG_LV_USE_DRAW_NANOVG
    #else
        #define LV_USE_DRAW_NANOVG 0
    #endif
#endif
#if LV_USE_DRAW_NANOVG
    /** Выберите серверную часть OpenGL для NanoVG:
     * - LV_NANOVG_BACKEND_GL2: OpenGL 2.0
     * - LV_NANOVG_BACKEND_GL3: OpenGL 3.0+
     * - LV_NANOVG_BACKEND_GLES2 : OpenGLES2.0
     * - LV_NANOVG_BACKEND_GLES3: OpenGLES3.0+
     */
    #ifndef LV_NANOVG_BACKEND
        #ifdef CONFIG_LV_NANOVG_BACKEND
            #define LV_NANOVG_BACKEND CONFIG_LV_NANOVG_BACKEND
        #else
            #define LV_NANOVG_BACKEND   LV_NANOVG_BACKEND_GLES2
        #endif
    #endif

    /** Отрисовать счетчик текстур текстур изображения. */
    #ifndef LV_NANOVG_IMAGE_CACHE_CNT
        #ifdef CONFIG_LV_NANOVG_IMAGE_CACHE_CNT
            #define LV_NANOVG_IMAGE_CACHE_CNT CONFIG_LV_NANOVG_IMAGE_CACHE_CNT
        #else
            #define LV_NANOVG_IMAGE_CACHE_CNT 128
        #endif
    #endif

    /** Отрисовка счетчика текстур текстуры букв. */
    #ifndef LV_NANOVG_LETTER_CACHE_CNT
        #ifdef CONFIG_LV_NANOVG_LETTER_CACHE_CNT
            #define LV_NANOVG_LETTER_CACHE_CNT CONFIG_LV_NANOVG_LETTER_CACHE_CNT
        #else
            #define LV_NANOVG_LETTER_CACHE_CNT 512
        #endif
    #endif
#endif

/*=======================
 * FEATURE CONFIGURATION
 *=======================*/

/*-------------
 * Ведение журнала
 *-----------*/

/** Включить модуль журнала */
#ifndef LV_USE_LOG
    #ifdef CONFIG_LV_USE_LOG
        #define LV_USE_LOG CONFIG_LV_USE_LOG
    #else
        #define LV_USE_LOG 0
    #endif
#endif
#if LV_USE_LOG
    /** Установите значение на один из следующих уровней детализации журнала:
     *  - LV_LOG_LEVEL_TRACE Подробная информация журнала.
     *  - LV_LOG_LEVEL_INFO Регистрируйте важные события.
     *  - LV_LOG_LEVEL_WARN Зарегистрируйте, если произошло что-то нежелательное, но не вызвало проблемы.
     *  - LV_LOG_LEVEL_ERROR Регистрируйте только критические проблемы, когда система может выйти из строя.
     *  - LV_LOG_LEVEL_USER Регистрировать только пользовательские сообщения журнала, добавленные пользователем.
     *  - LV_LOG_LEVEL_NONE Ничего не регистрируйте. */
    #ifndef LV_LOG_LEVEL
        #ifdef CONFIG_LV_LOG_LEVEL
            #define LV_LOG_LEVEL CONFIG_LV_LOG_LEVEL
        #else
            #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
        #endif
    #endif

    /** - 1: Распечатать журнал с помощью «printf»;
     *  - 0: пользователю необходимо зарегистрировать обратный вызов с помощью `lv_log_register_print_cb()`. */
    #ifndef LV_LOG_PRINTF
        #ifdef CONFIG_LV_LOG_PRINTF
            #define LV_LOG_PRINTF CONFIG_LV_LOG_PRINTF
        #else
            #define LV_LOG_PRINTF 0
        #endif
    #endif

    /** Установите обратный вызов для печати журналов.
     *  Например, `my_print`. Прототип должен быть `void my_print(lv_log_level_t level, const char * buf)`.
     *  Может быть перезаписан`lv_log_register_print_cb`. */
    //#define LV_LOG_PRINT_CB

    /** - 1: включить печать временной метки;
     *  - 0: отключить печать временной метки. */
    #ifndef LV_LOG_USE_TIMESTAMP
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_USE_TIMESTAMP
                #define LV_LOG_USE_TIMESTAMP CONFIG_LV_LOG_USE_TIMESTAMP
            #else
                #define LV_LOG_USE_TIMESTAMP 0
            #endif
        #else
            #define LV_LOG_USE_TIMESTAMP 1
        #endif
    #endif

    /** - 1: Распечатать файл и номер строки журнала;
     *  - 0: Не печатать номер файла и строки журнала. */
    #ifndef LV_LOG_USE_FILE_LINE
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_USE_FILE_LINE
                #define LV_LOG_USE_FILE_LINE CONFIG_LV_LOG_USE_FILE_LINE
            #else
                #define LV_LOG_USE_FILE_LINE 0
            #endif
        #else
            #define LV_LOG_USE_FILE_LINE 1
        #endif
    #endif

    /* Включить/выключить LV_LOG_TRACE в модулях, которые создают огромное количество логов. */
    #ifndef LV_LOG_TRACE_MEM
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_MEM
                #define LV_LOG_TRACE_MEM CONFIG_LV_LOG_TRACE_MEM
            #else
                #define LV_LOG_TRACE_MEM 0
            #endif
        #else
            #define LV_LOG_TRACE_MEM        1   /**< Enable/disable trace logs in memory operations. */
        #endif
    #endif
    #ifndef LV_LOG_TRACE_TIMER
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_TIMER
                #define LV_LOG_TRACE_TIMER CONFIG_LV_LOG_TRACE_TIMER
            #else
                #define LV_LOG_TRACE_TIMER 0
            #endif
        #else
            #define LV_LOG_TRACE_TIMER      1   /**< Enable/disable trace logs in timer operations. */
        #endif
    #endif
    #ifndef LV_LOG_TRACE_INDEV
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_INDEV
                #define LV_LOG_TRACE_INDEV CONFIG_LV_LOG_TRACE_INDEV
            #else
                #define LV_LOG_TRACE_INDEV 0
            #endif
        #else
            #define LV_LOG_TRACE_INDEV      1   /**< Enable/disable trace logs in input device operations. */
        #endif
    #endif
    #ifndef LV_LOG_TRACE_DISP_REFR
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_DISP_REFR
                #define LV_LOG_TRACE_DISP_REFR CONFIG_LV_LOG_TRACE_DISP_REFR
            #else
                #define LV_LOG_TRACE_DISP_REFR 0
            #endif
        #else
            #define LV_LOG_TRACE_DISP_REFR  1   /**< Enable/disable trace logs in display re-draw operations. */
        #endif
    #endif
    #ifndef LV_LOG_TRACE_EVENT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_EVENT
                #define LV_LOG_TRACE_EVENT CONFIG_LV_LOG_TRACE_EVENT
            #else
                #define LV_LOG_TRACE_EVENT 0
            #endif
        #else
            #define LV_LOG_TRACE_EVENT      1   /**< Enable/disable trace logs in event dispatch logic. */
        #endif
    #endif
    #ifndef LV_LOG_TRACE_OBJ_CREATE
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_OBJ_CREATE
                #define LV_LOG_TRACE_OBJ_CREATE CONFIG_LV_LOG_TRACE_OBJ_CREATE
            #else
                #define LV_LOG_TRACE_OBJ_CREATE 0
            #endif
        #else
            #define LV_LOG_TRACE_OBJ_CREATE 1   /**< Enable/disable trace logs in object creation (core `obj` creation plus every widget). */
        #endif
    #endif
    #ifndef LV_LOG_TRACE_LAYOUT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_LAYOUT
                #define LV_LOG_TRACE_LAYOUT CONFIG_LV_LOG_TRACE_LAYOUT
            #else
                #define LV_LOG_TRACE_LAYOUT 0
            #endif
        #else
            #define LV_LOG_TRACE_LAYOUT     1   /**< Enable/disable trace logs in flex- and grid-layout operations. */
        #endif
    #endif
    #ifndef LV_LOG_TRACE_ANIM
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_ANIM
                #define LV_LOG_TRACE_ANIM CONFIG_LV_LOG_TRACE_ANIM
            #else
                #define LV_LOG_TRACE_ANIM 0
            #endif
        #else
            #define LV_LOG_TRACE_ANIM       1   /**< Enable/disable trace logs in animation logic. */
        #endif
    #endif
    #ifndef LV_LOG_TRACE_CACHE
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_CACHE
                #define LV_LOG_TRACE_CACHE CONFIG_LV_LOG_TRACE_CACHE
            #else
                #define LV_LOG_TRACE_CACHE 0
            #endif
        #else
            #define LV_LOG_TRACE_CACHE      1   /**< Enable/disable trace logs in cache operations. */
        #endif
    #endif
#endif  /*LV_USE_LOG*/

/*-------------
 * Утверждает
 *-----------*/

/* Включите сбои утверждений, если операция завершается неудачно или обнаружены недопустимые данные.
 * Если LV_USE_LOG включен, в случае сбоя будет напечатано сообщение об ошибке. */
#ifndef LV_USE_ASSERT_NULL
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ASSERT_NULL
            #define LV_USE_ASSERT_NULL CONFIG_LV_USE_ASSERT_NULL
        #else
            #define LV_USE_ASSERT_NULL 0
        #endif
    #else
        #define LV_USE_ASSERT_NULL          1   /**< Check if the parameter is NULL. (Very fast, recommended) */
    #endif
#endif
#ifndef LV_USE_ASSERT_MALLOC
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ASSERT_MALLOC
            #define LV_USE_ASSERT_MALLOC CONFIG_LV_USE_ASSERT_MALLOC
        #else
            #define LV_USE_ASSERT_MALLOC 0
        #endif
    #else
        #define LV_USE_ASSERT_MALLOC        1   /**< Checks is the memory is successfully allocated or no. (Very fast, recommended) */
    #endif
#endif
#ifndef LV_USE_ASSERT_STYLE
    #ifdef CONFIG_LV_USE_ASSERT_STYLE
        #define LV_USE_ASSERT_STYLE CONFIG_LV_USE_ASSERT_STYLE
    #else
        #define LV_USE_ASSERT_STYLE         0   /**< Check if the styles are properly initialized. (Very fast, recommended) */
    #endif
#endif
#ifndef LV_USE_ASSERT_MEM_INTEGRITY
    #ifdef CONFIG_LV_USE_ASSERT_MEM_INTEGRITY
        #define LV_USE_ASSERT_MEM_INTEGRITY CONFIG_LV_USE_ASSERT_MEM_INTEGRITY
    #else
        #define LV_USE_ASSERT_MEM_INTEGRITY 0   /**< Check the integrity of `lv_mem` after critical operations. (Slow) */
    #endif
#endif
#ifndef LV_USE_ASSERT_OBJ
    #ifdef CONFIG_LV_USE_ASSERT_OBJ
        #define LV_USE_ASSERT_OBJ CONFIG_LV_USE_ASSERT_OBJ
    #else
        #define LV_USE_ASSERT_OBJ           0   /**< Check the object's type and existence (e.g. not deleted). (Slow) */
    #endif
#endif

/** Добавьте собственный обработчик, когда происходит утверждение, например. чтобы перезапустить MCU. */
#ifndef LV_ASSERT_HANDLER_INCLUDE
    #ifdef CONFIG_LV_ASSERT_HANDLER_INCLUDE
        #define LV_ASSERT_HANDLER_INCLUDE CONFIG_LV_ASSERT_HANDLER_INCLUDE
    #else
        #define LV_ASSERT_HANDLER_INCLUDE <stdint.h>
    #endif
#endif
#ifndef LV_ASSERT_HANDLER
    #ifdef CONFIG_LV_ASSERT_HANDLER
        #define LV_ASSERT_HANDLER CONFIG_LV_ASSERT_HANDLER
    #else
        #define LV_ASSERT_HANDLER while(1);     /**< Halt by default */
    #endif
#endif

/*-------------
 * Отладка
 *-----------*/

/** 1: Нарисуйте прямоугольники случайного цвета поверх перерисованных областей. */
#ifndef LV_USE_REFR_DEBUG
    #ifdef CONFIG_LV_USE_REFR_DEBUG
        #define LV_USE_REFR_DEBUG CONFIG_LV_USE_REFR_DEBUG
    #else
        #define LV_USE_REFR_DEBUG 0
    #endif
#endif

/** 1: Нарисуйте красное наложение для слоев ARGB и зеленое наложение для слоев RGB.*/
#ifndef LV_USE_LAYER_DEBUG
    #ifdef CONFIG_LV_USE_LAYER_DEBUG
        #define LV_USE_LAYER_DEBUG CONFIG_LV_USE_LAYER_DEBUG
    #else
        #define LV_USE_LAYER_DEBUG 0
    #endif
#endif

/** 1: Добавляет следующие варианты поведения для отладки:
 *  - Нарисуйте наложения разных цветов для задач каждого draw_unit.
 *  - Нарисуйте индексный номер блока рисования на белом фоне.
 *  - Для слоев рисует порядковый номер единицы рисования на черном фоне. */
#ifndef LV_USE_PARALLEL_DRAW_DEBUG
    #ifdef CONFIG_LV_USE_PARALLEL_DRAW_DEBUG
        #define LV_USE_PARALLEL_DRAW_DEBUG CONFIG_LV_USE_PARALLEL_DRAW_DEBUG
    #else
        #define LV_USE_PARALLEL_DRAW_DEBUG 0
    #endif
#endif

/*-------------
 * Другие
 *-----------*/

#ifndef LV_ENABLE_GLOBAL_CUSTOM
    #ifdef CONFIG_LV_ENABLE_GLOBAL_CUSTOM
        #define LV_ENABLE_GLOBAL_CUSTOM CONFIG_LV_ENABLE_GLOBAL_CUSTOM
    #else
        #define LV_ENABLE_GLOBAL_CUSTOM 0
    #endif
#endif
#if LV_ENABLE_GLOBAL_CUSTOM
    /** Заголовок для включения пользователем функции «lv_global» */
    #ifndef LV_GLOBAL_CUSTOM_INCLUDE
        #ifdef CONFIG_LV_GLOBAL_CUSTOM_INCLUDE
            #define LV_GLOBAL_CUSTOM_INCLUDE CONFIG_LV_GLOBAL_CUSTOM_INCLUDE
        #else
            #define LV_GLOBAL_CUSTOM_INCLUDE <stdint.h>
        #endif
    #endif
#endif

/** Размер кэша по умолчанию в байтах.
 *  Используются декодеры изображений, такие как `lv_lodepng`, для хранения декодированного изображения в памяти.
 *  Если для размера не установлено значение 0, декодер не сможет декодировать, когда кэш заполнен.
 *  Если размер равен 0, функция кэширования не включена и декодированная память будет
 *  высвобождается сразу после использования. */
#ifndef LV_CACHE_DEF_SIZE
    #ifdef CONFIG_LV_CACHE_DEF_SIZE
        #define LV_CACHE_DEF_SIZE CONFIG_LV_CACHE_DEF_SIZE
    #else
        #define LV_CACHE_DEF_SIZE       0
    #endif
#endif

/** Число записей кэша заголовков изображений по умолчанию. Кэш используется для хранения заголовков изображений.
 *  Основная логика аналогична `LV_CACHE_DEF_SIZE`, но для заголовков изображений. */
#ifndef LV_IMAGE_HEADER_CACHE_DEF_CNT
    #ifdef CONFIG_LV_IMAGE_HEADER_CACHE_DEF_CNT
        #define LV_IMAGE_HEADER_CACHE_DEF_CNT CONFIG_LV_IMAGE_HEADER_CACHE_DEF_CNT
    #else
        #define LV_IMAGE_HEADER_CACHE_DEF_CNT 0
    #endif
#endif

/** Количество остановок, разрешенных для каждого уклона. Увеличьте это значение, чтобы разрешить больше остановок.
 *  Это добавляется (sizeof(lv_color_t) + 1) байт на каждую дополнительную остановку. */
#ifndef LV_GRADIENT_MAX_STOPS
    #ifdef CONFIG_LV_GRADIENT_MAX_STOPS
        #define LV_GRADIENT_MAX_STOPS CONFIG_LV_GRADIENT_MAX_STOPS
    #else
        #define LV_GRADIENT_MAX_STOPS   2
    #endif
#endif

/** Отрегулируйте округление функций смешивания цветов. Графические процессоры могут по-разному рассчитывать сочетание цветов (смешивание).
 *  - 0: округлить вниз,
 *  - 64: округлить в большую сторону от x.75,
 *  - 128: округлить до половины,
 *  - 192: округлить в большую сторону от x.25,
 *  - 254: округлить вверх */
#ifndef LV_COLOR_MIX_ROUND_OFS
    #ifdef CONFIG_LV_COLOR_MIX_ROUND_OFS
        #define LV_COLOR_MIX_ROUND_OFS CONFIG_LV_COLOR_MIX_ROUND_OFS
    #else
        #define LV_COLOR_MIX_ROUND_OFS  0
    #endif
#endif

/** Добавьте 2 32-битных переменных в каждый `lv_obj_t`, чтобы ускорить получение свойства стиля. */
#ifndef LV_OBJ_STYLE_CACHE
    #ifdef CONFIG_LV_OBJ_STYLE_CACHE
        #define LV_OBJ_STYLE_CACHE CONFIG_LV_OBJ_STYLE_CACHE
    #else
        #define LV_OBJ_STYLE_CACHE      0
    #endif
#endif

/** Добавьте поле`id`в`lv_obj_t`. */
#ifndef LV_USE_OBJ_ID
    #ifdef CONFIG_LV_USE_OBJ_ID
        #define LV_USE_OBJ_ID CONFIG_LV_USE_OBJ_ID
    #else
        #define LV_USE_OBJ_ID           0
    #endif
#endif

/**  Включить названия виджетов поддержки*/
#ifndef LV_USE_OBJ_NAME
    #ifdef CONFIG_LV_USE_OBJ_NAME
        #define LV_USE_OBJ_NAME CONFIG_LV_USE_OBJ_NAME
    #else
        #define LV_USE_OBJ_NAME         0
    #endif
#endif

/** Автоматически назначать ID при создании объекта. */
#ifndef LV_OBJ_ID_AUTO_ASSIGN
    #ifdef CONFIG_LV_OBJ_ID_AUTO_ASSIGN
        #define LV_OBJ_ID_AUTO_ASSIGN CONFIG_LV_OBJ_ID_AUTO_ASSIGN
    #else
        #define LV_OBJ_ID_AUTO_ASSIGN   LV_USE_OBJ_ID
    #endif
#endif

/** Используйте встроенные функции обработчика obj ID:
* - lv_obj_assign_id: вызывается при создании виджета. Используйте отдельный счетчик для каждого класса виджета в виде ID.
* - lv_obj_id_compare: Сравните ID, чтобы решить, соответствует ли он запрошенному значению.
* - lv_obj_stringify_id : Возвращает строковый идентификатор, например. «кнопка3».
* - lv_obj_free_id : Ничего не делает, поскольку дляIDне выделяется память.
* При отключении эти функции должны быть реализованы пользователем.*/
#ifndef LV_USE_OBJ_ID_BUILTIN
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_OBJ_ID_BUILTIN
            #define LV_USE_OBJ_ID_BUILTIN CONFIG_LV_USE_OBJ_ID_BUILTIN
        #else
            #define LV_USE_OBJ_ID_BUILTIN 0
        #endif
    #else
        #define LV_USE_OBJ_ID_BUILTIN   1
    #endif
#endif

/** Используйте набор свойств obj/получитеAPI. */
#ifndef LV_USE_OBJ_PROPERTY
    #ifdef CONFIG_LV_USE_OBJ_PROPERTY
        #define LV_USE_OBJ_PROPERTY CONFIG_LV_USE_OBJ_PROPERTY
    #else
        #define LV_USE_OBJ_PROPERTY 0
    #endif
#endif

/** Включите поддержку имен свойств. */
#ifndef LV_USE_OBJ_PROPERTY_NAME
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_OBJ_PROPERTY_NAME
            #define LV_USE_OBJ_PROPERTY_NAME CONFIG_LV_USE_OBJ_PROPERTY_NAME
        #else
            #define LV_USE_OBJ_PROPERTY_NAME 0
        #endif
    #else
        #define LV_USE_OBJ_PROPERTY_NAME 1
    #endif
#endif

/* Включите функцию распознавания жестов несколькими касаниями. */
/* Распознавание жестов требует использования поплавков. */
#ifndef LV_USE_GESTURE_RECOGNITION
    #ifdef CONFIG_LV_USE_GESTURE_RECOGNITION
        #define LV_USE_GESTURE_RECOGNITION CONFIG_LV_USE_GESTURE_RECOGNITION
    #else
        #define LV_USE_GESTURE_RECOGNITION 0
    #endif
#endif

/*=====================
 *  COMPILER SETTINGS
 *====================*/

/** Для систем с прямым порядком байтов установите значение 1. */
#ifndef LV_BIG_ENDIAN_SYSTEM
    #ifdef CONFIG_LV_BIG_ENDIAN_SYSTEM
        #define LV_BIG_ENDIAN_SYSTEM CONFIG_LV_BIG_ENDIAN_SYSTEM
    #else
        #define LV_BIG_ENDIAN_SYSTEM 0
    #endif
#endif

/** Определите пользовательские атрибуты для функции `lv_tick_inc`. */
#ifndef LV_ATTRIBUTE_TICK_INC
    #ifdef CONFIG_LV_ATTRIBUTE_TICK_INC
        #define LV_ATTRIBUTE_TICK_INC CONFIG_LV_ATTRIBUTE_TICK_INC
    #else
        #define LV_ATTRIBUTE_TICK_INC
    #endif
#endif

/** Определите пользовательские атрибуты для функции `lv_timer_handler`. */
#ifndef LV_ATTRIBUTE_TIMER_HANDLER
    #ifdef CONFIG_LV_ATTRIBUTE_TIMER_HANDLER
        #define LV_ATTRIBUTE_TIMER_HANDLER CONFIG_LV_ATTRIBUTE_TIMER_HANDLER
    #else
        #define LV_ATTRIBUTE_TIMER_HANDLER
    #endif
#endif

/** Определите пользовательские атрибуты для функции `lv_display_flush_ready`. */
#ifndef LV_ATTRIBUTE_FLUSH_READY
    #ifdef CONFIG_LV_ATTRIBUTE_FLUSH_READY
        #define LV_ATTRIBUTE_FLUSH_READY CONFIG_LV_ATTRIBUTE_FLUSH_READY
    #else
        #define LV_ATTRIBUTE_FLUSH_READY
    #endif
#endif

/** Выровняйте буферы VG_LITE по этому количеству байт.
 *  @note  vglite_src_buf_aligned() использует это значение для проверки выравнивания переданных указателей буфера. */
#ifndef LV_ATTRIBUTE_MEM_ALIGN_SIZE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_ATTRIBUTE_MEM_ALIGN_SIZE
            #define LV_ATTRIBUTE_MEM_ALIGN_SIZE CONFIG_LV_ATTRIBUTE_MEM_ALIGN_SIZE
        #else
            #define LV_ATTRIBUTE_MEM_ALIGN_SIZE 0
        #endif
    #else
        #define LV_ATTRIBUTE_MEM_ALIGN_SIZE 1
    #endif
#endif

/** Будет добавлено там, где необходимо выровнять память (с -Os данные по умолчанию могут быть не выровнены по границе).
 *  например__attribute__((выровнено(4)))*/
#ifndef LV_ATTRIBUTE_MEM_ALIGN
    #ifdef CONFIG_LV_ATTRIBUTE_MEM_ALIGN
        #define LV_ATTRIBUTE_MEM_ALIGN CONFIG_LV_ATTRIBUTE_MEM_ALIGN
    #else
        #define LV_ATTRIBUTE_MEM_ALIGN
    #endif
#endif

/** Атрибут для обозначения больших массивов констант, например для растровых изображений шрифтов. */
#ifndef LV_ATTRIBUTE_LARGE_CONST
    #ifdef CONFIG_LV_ATTRIBUTE_LARGE_CONST
        #define LV_ATTRIBUTE_LARGE_CONST CONFIG_LV_ATTRIBUTE_LARGE_CONST
    #else
        #define LV_ATTRIBUTE_LARGE_CONST
    #endif
#endif

/** Префикс компилятора для объявления большого массива в RAM */
#ifndef LV_ATTRIBUTE_LARGE_RAM_ARRAY
    #ifdef CONFIG_LV_ATTRIBUTE_LARGE_RAM_ARRAY
        #define LV_ATTRIBUTE_LARGE_RAM_ARRAY CONFIG_LV_ATTRIBUTE_LARGE_RAM_ARRAY
    #else
        #define LV_ATTRIBUTE_LARGE_RAM_ARRAY
    #endif
#endif

/** Поместите важные для производительности функции в более быструю память (например, RAM). */
#ifndef LV_ATTRIBUTE_FAST_MEM
    #ifdef CONFIG_LV_ATTRIBUTE_FAST_MEM
        #define LV_ATTRIBUTE_FAST_MEM CONFIG_LV_ATTRIBUTE_FAST_MEM
    #else
        #define LV_ATTRIBUTE_FAST_MEM
    #endif
#endif

/** Экспортировать целочисленную константу в привязку. Этот макрос используется с константами вида LV_ < CONST >, которые
 *  Также должно появиться в привязкеLVGLAPI , например MicroPython. */
#ifndef LV_EXPORT_CONST_INT
    #ifdef CONFIG_LV_EXPORT_CONST_INT
        #define LV_EXPORT_CONST_INT CONFIG_LV_EXPORT_CONST_INT
    #else
        #define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning  /**< The default value just prevents GCC warning */
    #endif
#endif

/** Префикс всех глобальных внешних данных с этим */
#ifndef LV_ATTRIBUTE_EXTERN_DATA
    #ifdef CONFIG_LV_ATTRIBUTE_EXTERN_DATA
        #define LV_ATTRIBUTE_EXTERN_DATA CONFIG_LV_ATTRIBUTE_EXTERN_DATA
    #else
        #define LV_ATTRIBUTE_EXTERN_DATA
    #endif
#endif

/** Используйте`float`как `lv_value_precise_t` */
#ifndef LV_USE_FLOAT
    #ifdef CONFIG_LV_USE_FLOAT
        #define LV_USE_FLOAT CONFIG_LV_USE_FLOAT
    #else
        #define LV_USE_FLOAT            0
    #endif
#endif

/** Включить поддержку матрицы
 *  - Требуется `LV_USE_FLOAT = 1` */
#ifndef LV_USE_MATRIX
    #ifdef CONFIG_LV_USE_MATRIX
        #define LV_USE_MATRIX CONFIG_LV_USE_MATRIX
    #else
        #define LV_USE_MATRIX           0
    #endif
#endif

/** Включите`lvgl_private.h`в`lvgl.h`для доступа к стандартным данным и функциям по умолчанию. */
#ifndef LV_USE_PRIVATE_API
    #ifndef LV_USE_PRIVATE_API
        #ifdef CONFIG_LV_USE_PRIVATE_API
            #define LV_USE_PRIVATE_API CONFIG_LV_USE_PRIVATE_API
        #else
            #define LV_USE_PRIVATE_API  0
        #endif
    #endif
#endif

/*==================
 *   FONT USAGE
 *===================*/

/* Шрифты Montserrat в диапазонеASCIIи некоторыми символами, использующими bpp = 4.
 * https://fonts.google.com/specimen/Montserrat */
#ifndef LV_FONT_MONTSERRAT_8
    #ifdef CONFIG_LV_FONT_MONTSERRAT_8
        #define LV_FONT_MONTSERRAT_8 CONFIG_LV_FONT_MONTSERRAT_8
    #else
        #define LV_FONT_MONTSERRAT_8  0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_10
    #ifdef CONFIG_LV_FONT_MONTSERRAT_10
        #define LV_FONT_MONTSERRAT_10 CONFIG_LV_FONT_MONTSERRAT_10
    #else
        #define LV_FONT_MONTSERRAT_10 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_12
    #ifdef CONFIG_LV_FONT_MONTSERRAT_12
        #define LV_FONT_MONTSERRAT_12 CONFIG_LV_FONT_MONTSERRAT_12
    #else
        #define LV_FONT_MONTSERRAT_12 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_14
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_FONT_MONTSERRAT_14
            #define LV_FONT_MONTSERRAT_14 CONFIG_LV_FONT_MONTSERRAT_14
        #else
            #define LV_FONT_MONTSERRAT_14 0
        #endif
    #else
        #define LV_FONT_MONTSERRAT_14 1
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_16
    #ifdef CONFIG_LV_FONT_MONTSERRAT_16
        #define LV_FONT_MONTSERRAT_16 CONFIG_LV_FONT_MONTSERRAT_16
    #else
        #define LV_FONT_MONTSERRAT_16 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_18
    #ifdef CONFIG_LV_FONT_MONTSERRAT_18
        #define LV_FONT_MONTSERRAT_18 CONFIG_LV_FONT_MONTSERRAT_18
    #else
        #define LV_FONT_MONTSERRAT_18 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_20
    #ifdef CONFIG_LV_FONT_MONTSERRAT_20
        #define LV_FONT_MONTSERRAT_20 CONFIG_LV_FONT_MONTSERRAT_20
    #else
        #define LV_FONT_MONTSERRAT_20 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_22
    #ifdef CONFIG_LV_FONT_MONTSERRAT_22
        #define LV_FONT_MONTSERRAT_22 CONFIG_LV_FONT_MONTSERRAT_22
    #else
        #define LV_FONT_MONTSERRAT_22 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_24
    #ifdef CONFIG_LV_FONT_MONTSERRAT_24
        #define LV_FONT_MONTSERRAT_24 CONFIG_LV_FONT_MONTSERRAT_24
    #else
        #define LV_FONT_MONTSERRAT_24 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_26
    #ifdef CONFIG_LV_FONT_MONTSERRAT_26
        #define LV_FONT_MONTSERRAT_26 CONFIG_LV_FONT_MONTSERRAT_26
    #else
        #define LV_FONT_MONTSERRAT_26 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_28
    #ifdef CONFIG_LV_FONT_MONTSERRAT_28
        #define LV_FONT_MONTSERRAT_28 CONFIG_LV_FONT_MONTSERRAT_28
    #else
        #define LV_FONT_MONTSERRAT_28 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_30
    #ifdef CONFIG_LV_FONT_MONTSERRAT_30
        #define LV_FONT_MONTSERRAT_30 CONFIG_LV_FONT_MONTSERRAT_30
    #else
        #define LV_FONT_MONTSERRAT_30 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_32
    #ifdef CONFIG_LV_FONT_MONTSERRAT_32
        #define LV_FONT_MONTSERRAT_32 CONFIG_LV_FONT_MONTSERRAT_32
    #else
        #define LV_FONT_MONTSERRAT_32 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_34
    #ifdef CONFIG_LV_FONT_MONTSERRAT_34
        #define LV_FONT_MONTSERRAT_34 CONFIG_LV_FONT_MONTSERRAT_34
    #else
        #define LV_FONT_MONTSERRAT_34 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_36
    #ifdef CONFIG_LV_FONT_MONTSERRAT_36
        #define LV_FONT_MONTSERRAT_36 CONFIG_LV_FONT_MONTSERRAT_36
    #else
        #define LV_FONT_MONTSERRAT_36 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_38
    #ifdef CONFIG_LV_FONT_MONTSERRAT_38
        #define LV_FONT_MONTSERRAT_38 CONFIG_LV_FONT_MONTSERRAT_38
    #else
        #define LV_FONT_MONTSERRAT_38 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_40
    #ifdef CONFIG_LV_FONT_MONTSERRAT_40
        #define LV_FONT_MONTSERRAT_40 CONFIG_LV_FONT_MONTSERRAT_40
    #else
        #define LV_FONT_MONTSERRAT_40 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_42
    #ifdef CONFIG_LV_FONT_MONTSERRAT_42
        #define LV_FONT_MONTSERRAT_42 CONFIG_LV_FONT_MONTSERRAT_42
    #else
        #define LV_FONT_MONTSERRAT_42 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_44
    #ifdef CONFIG_LV_FONT_MONTSERRAT_44
        #define LV_FONT_MONTSERRAT_44 CONFIG_LV_FONT_MONTSERRAT_44
    #else
        #define LV_FONT_MONTSERRAT_44 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_46
    #ifdef CONFIG_LV_FONT_MONTSERRAT_46
        #define LV_FONT_MONTSERRAT_46 CONFIG_LV_FONT_MONTSERRAT_46
    #else
        #define LV_FONT_MONTSERRAT_46 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_48
    #ifdef CONFIG_LV_FONT_MONTSERRAT_48
        #define LV_FONT_MONTSERRAT_48 CONFIG_LV_FONT_MONTSERRAT_48
    #else
        #define LV_FONT_MONTSERRAT_48 0
    #endif
#endif

/* Демонстрация особых возможностей */
#ifndef LV_FONT_MONTSERRAT_28_COMPRESSED
    #ifdef CONFIG_LV_FONT_MONTSERRAT_28_COMPRESSED
        #define LV_FONT_MONTSERRAT_28_COMPRESSED CONFIG_LV_FONT_MONTSERRAT_28_COMPRESSED
    #else
        #define LV_FONT_MONTSERRAT_28_COMPRESSED    0  /**< bpp = 3 */
    #endif
#endif
#ifndef LV_FONT_DEJAVU_16_PERSIAN_HEBREW
    #ifdef CONFIG_LV_FONT_DEJAVU_16_PERSIAN_HEBREW
        #define LV_FONT_DEJAVU_16_PERSIAN_HEBREW CONFIG_LV_FONT_DEJAVU_16_PERSIAN_HEBREW
    #else
        #define LV_FONT_DEJAVU_16_PERSIAN_HEBREW    0  /**< Hebrew, Arabic, Persian letters and all their forms */
    #endif
#endif
#ifndef LV_FONT_SOURCE_HAN_SANS_SC_14_CJK
    #ifdef CONFIG_LV_FONT_SOURCE_HAN_SANS_SC_14_CJK
        #define LV_FONT_SOURCE_HAN_SANS_SC_14_CJK CONFIG_LV_FONT_SOURCE_HAN_SANS_SC_14_CJK
    #else
        #define LV_FONT_SOURCE_HAN_SANS_SC_14_CJK   0  /**< 1338 most common CJK radicals */
    #endif
#endif
#ifndef LV_FONT_SOURCE_HAN_SANS_SC_16_CJK
    #ifdef CONFIG_LV_FONT_SOURCE_HAN_SANS_SC_16_CJK
        #define LV_FONT_SOURCE_HAN_SANS_SC_16_CJK CONFIG_LV_FONT_SOURCE_HAN_SANS_SC_16_CJK
    #else
        #define LV_FONT_SOURCE_HAN_SANS_SC_16_CJK   0  /**< 1338 most common CJK radicals */
    #endif
#endif

/** Пиксельные моноширинные шрифты */
#ifndef LV_FONT_UNSCII_8
    #ifdef CONFIG_LV_FONT_UNSCII_8
        #define LV_FONT_UNSCII_8 CONFIG_LV_FONT_UNSCII_8
    #else
        #define LV_FONT_UNSCII_8  0
    #endif
#endif
#ifndef LV_FONT_UNSCII_16
    #ifdef CONFIG_LV_FONT_UNSCII_16
        #define LV_FONT_UNSCII_16 CONFIG_LV_FONT_UNSCII_16
    #else
        #define LV_FONT_UNSCII_16 0
    #endif
#endif

/** При желании объявите здесь пользовательские шрифты.
 *
 *  Вы также можете использовать любой из этих шрифтов в качестве шрифта по умолчанию, и они будут доступны.
 *  глобально.  Пример:
 *
 *  @code
 *  #defineLV_FONT_CUSTOM_DECLARELV_FONT_DECLARE (my_font_1)LV_FONT_DECLARE(my_font_2)
 *  @endcode
 */
#ifndef LV_FONT_CUSTOM_DECLARE
    #ifdef CONFIG_LV_FONT_CUSTOM_DECLARE
        #define LV_FONT_CUSTOM_DECLARE CONFIG_LV_FONT_CUSTOM_DECLARE
    #else
        #define LV_FONT_CUSTOM_DECLARE
    #endif
#endif

/** Всегда устанавливать шрифт по умолчанию */
#ifndef LV_FONT_DEFAULT
    #ifdef CONFIG_LV_FONT_DEFAULT
        #define LV_FONT_DEFAULT CONFIG_LV_FONT_DEFAULT
    #else
        #define LV_FONT_DEFAULT &lv_font_montserrat_14
    #endif
#endif

/** Включите обработку большого шрифта и/или шрифтов с большим количеством символов.
 *  Ограничение зависит от размера шрифта, начертания шрифта и bpp.
 *  Ошибка компилятора будет вызвана, если шрифту это понадобится. */
#ifndef LV_FONT_FMT_TXT_LARGE
    #ifdef CONFIG_LV_FONT_FMT_TXT_LARGE
        #define LV_FONT_FMT_TXT_LARGE CONFIG_LV_FONT_FMT_TXT_LARGE
    #else
        #define LV_FONT_FMT_TXT_LARGE 0
    #endif
#endif

/** Включает/выключает поддержку сжатых шрифтов. */
#ifndef LV_USE_FONT_COMPRESSED
    #ifdef CONFIG_LV_USE_FONT_COMPRESSED
        #define LV_USE_FONT_COMPRESSED CONFIG_LV_USE_FONT_COMPRESSED
    #else
        #define LV_USE_FONT_COMPRESSED 0
    #endif
#endif

/** Включите заливку для рисования, если глиф dsc не найден. */
#ifndef LV_USE_FONT_PLACEHOLDER
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_FONT_PLACEHOLDER
            #define LV_USE_FONT_PLACEHOLDER CONFIG_LV_USE_FONT_PLACEHOLDER
        #else
            #define LV_USE_FONT_PLACEHOLDER 0
        #endif
    #else
        #define LV_USE_FONT_PLACEHOLDER 1
    #endif
#endif

/*=================
 *  TEXT SETTINGS
 *=================*/

/**
 * Выберите кодировку символов для строк.
 * Ваш IDE или редактор должен иметь ту же кодировку символов.
 * - LV_TXT_ENC_UTF8
 * - LV_TXT_ENC_ASCII
 */
#ifndef LV_TXT_ENC
    #ifdef CONFIG_LV_TXT_ENC
        #define LV_TXT_ENC CONFIG_LV_TXT_ENC
    #else
        #define LV_TXT_ENC LV_TXT_ENC_UTF8
    #endif
#endif

/** При рендеринге текстовых строк разбивайте (переносите) текст на эти символы. */
#ifndef LV_TXT_BREAK_CHARS
    #ifdef CONFIG_LV_TXT_BREAK_CHARS
        #define LV_TXT_BREAK_CHARS CONFIG_LV_TXT_BREAK_CHARS
    #else
        #define LV_TXT_BREAK_CHARS " ,.;:-_)]}"
    #endif
#endif

/** Если слово хотя бы такой длины, оно порвется там, где оно «самое красивое».
 *  Чтобы отключить, установите значение <= 0. */
#ifndef LV_TXT_LINE_BREAK_LONG_LEN
    #ifdef CONFIG_LV_TXT_LINE_BREAK_LONG_LEN
        #define LV_TXT_LINE_BREAK_LONG_LEN CONFIG_LV_TXT_LINE_BREAK_LONG_LEN
    #else
        #define LV_TXT_LINE_BREAK_LONG_LEN 0
    #endif
#endif

/** Минимальное количество символов в длинном слове, помещаемое в строку перед разрывом.
 *  Зависит от LV_TXT_LINE_BREAK_LONG_LEN. */
#ifndef LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN
    #ifdef CONFIG_LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN
        #define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN CONFIG_LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN
    #else
        #define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3
    #endif
#endif

/** Минимальное количество символов в длинном слове, помещаемое в строку после разрыва.
 *  Зависит от LV_TXT_LINE_BREAK_LONG_LEN. */
#ifndef LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN
    #ifdef CONFIG_LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN
        #define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN CONFIG_LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN
    #else
        #define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3
    #endif
#endif

/** Поддержка двунаправленного текста. Позволяет смешивать текст с письмом слева направо и справа налево.
 *  Направление будет обработано в соответствии с двунаправленным алгоритмом Unicode:
 *  https://www.w3.org/International/articles/inline-bidi-markup/uba-basics */
#ifndef LV_USE_BIDI
    #ifdef CONFIG_LV_USE_BIDI
        #define LV_USE_BIDI CONFIG_LV_USE_BIDI
    #else
        #define LV_USE_BIDI 0
    #endif
#endif
#if LV_USE_BIDI
    /*Установите направление по умолчанию. Поддерживаемые значения:
    *`LV_BASE_DIR_LTR` Слева направо
    *`LV_BASE_DIR_RTL` Справа налево
    *`LV_BASE_DIR_AUTO` определяет направление текстовой основы*/
    #ifndef LV_BIDI_BASE_DIR_DEF
        #ifdef CONFIG_LV_BIDI_BASE_DIR_DEF
            #define LV_BIDI_BASE_DIR_DEF CONFIG_LV_BIDI_BASE_DIR_DEF
        #else
            #define LV_BIDI_BASE_DIR_DEF LV_BASE_DIR_AUTO
        #endif
    #endif
#endif

/** Включить обработку на арабском/персидском языке
 *  В этих языках символы следует заменять другой формой в зависимости от их положения в тексте. */
#ifndef LV_USE_ARABIC_PERSIAN_CHARS
    #ifdef CONFIG_LV_USE_ARABIC_PERSIAN_CHARS
        #define LV_USE_ARABIC_PERSIAN_CHARS CONFIG_LV_USE_ARABIC_PERSIAN_CHARS
    #else
        #define LV_USE_ARABIC_PERSIAN_CHARS 0
    #endif
#endif

/*Управляющий символ, используемый для сигнализации об изменении цвета текста.*/
#ifndef LV_TXT_COLOR_CMD
    #ifdef CONFIG_LV_TXT_COLOR_CMD
        #define LV_TXT_COLOR_CMD CONFIG_LV_TXT_COLOR_CMD
    #else
        #define LV_TXT_COLOR_CMD "#"
    #endif
#endif

/*==================
 * WIDGETS
 *================*/
/* Документацию по виджетам можно найти здесь: https://docs.lvgl.io/master/widgets/index.html. */

/** 1: Приводит к тому, что этим виджетам присваиваются значения по умолчанию во время создания.
 *  - lv_buttonmatrix_t : Получить карты по умолчанию: {"Btn1", "Btn2", "Btn3", "\n", "Btn4", "Btn5", ""}, иначе карта не установлена.
 *  - lv_checkbox_t: для метки строки установлено значение «Флажок», в противном случае — пустая строка.
 *  - lv_dropdown_t: для параметров установлены значения «Вариант 1», «Вариант 2», «Вариант 3», иначе значения не заданы.
 *  - lv_roller_t: для параметров установлены значения «Вариант 1», «Вариант 2», «Вариант 3», «Вариант 4», «Вариант 5», иначе значения не устанавливаются.
 *  - lv_label_t: для текста установлено значение «Текст», иначе пустая строка.
 *  - lv_arclabel_t: для текста установлено значение «Дужный текст», иначе пустая строка.
 * */
#ifndef LV_WIDGETS_HAS_DEFAULT_VALUE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_WIDGETS_HAS_DEFAULT_VALUE
            #define LV_WIDGETS_HAS_DEFAULT_VALUE CONFIG_LV_WIDGETS_HAS_DEFAULT_VALUE
        #else
            #define LV_WIDGETS_HAS_DEFAULT_VALUE 0
        #endif
    #else
        #define LV_WIDGETS_HAS_DEFAULT_VALUE  1
    #endif
#endif

#ifndef LV_USE_ANIMIMG
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ANIMIMG
            #define LV_USE_ANIMIMG CONFIG_LV_USE_ANIMIMG
        #else
            #define LV_USE_ANIMIMG 0
        #endif
    #else
        #define LV_USE_ANIMIMG    1
    #endif
#endif

#ifndef LV_USE_ARC
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ARC
            #define LV_USE_ARC CONFIG_LV_USE_ARC
        #else
            #define LV_USE_ARC 0
        #endif
    #else
        #define LV_USE_ARC        1
    #endif
#endif

#ifndef LV_USE_ARCLABEL
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ARCLABEL
            #define LV_USE_ARCLABEL CONFIG_LV_USE_ARCLABEL
        #else
            #define LV_USE_ARCLABEL 0
        #endif
    #else
        #define LV_USE_ARCLABEL  1
    #endif
#endif

#ifndef LV_USE_BAR
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_BAR
            #define LV_USE_BAR CONFIG_LV_USE_BAR
        #else
            #define LV_USE_BAR 0
        #endif
    #else
        #define LV_USE_BAR        1
    #endif
#endif

#ifndef LV_USE_BUTTON
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_BUTTON
            #define LV_USE_BUTTON CONFIG_LV_USE_BUTTON
        #else
            #define LV_USE_BUTTON 0
        #endif
    #else
        #define LV_USE_BUTTON        1
    #endif
#endif

#ifndef LV_USE_BUTTONMATRIX
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_BUTTONMATRIX
            #define LV_USE_BUTTONMATRIX CONFIG_LV_USE_BUTTONMATRIX
        #else
            #define LV_USE_BUTTONMATRIX 0
        #endif
    #else
        #define LV_USE_BUTTONMATRIX  1
    #endif
#endif

#ifndef LV_USE_CALENDAR
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_CALENDAR
            #define LV_USE_CALENDAR CONFIG_LV_USE_CALENDAR
        #else
            #define LV_USE_CALENDAR 0
        #endif
    #else
        #define LV_USE_CALENDAR   1
    #endif
#endif
#if LV_USE_CALENDAR
    #ifndef LV_CALENDAR_WEEK_STARTS_MONDAY
        #ifdef CONFIG_LV_CALENDAR_WEEK_STARTS_MONDAY
            #define LV_CALENDAR_WEEK_STARTS_MONDAY CONFIG_LV_CALENDAR_WEEK_STARTS_MONDAY
        #else
            #define LV_CALENDAR_WEEK_STARTS_MONDAY 0
        #endif
    #endif
    #if LV_CALENDAR_WEEK_STARTS_MONDAY
        #ifndef LV_CALENDAR_DEFAULT_DAY_NAMES
            #ifdef CONFIG_LV_CALENDAR_DEFAULT_DAY_NAMES
                #define LV_CALENDAR_DEFAULT_DAY_NAMES CONFIG_LV_CALENDAR_DEFAULT_DAY_NAMES
            #else
                #define LV_CALENDAR_DEFAULT_DAY_NAMES {"Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"}
            #endif
        #endif
    #else
        #ifndef LV_CALENDAR_DEFAULT_DAY_NAMES
            #ifdef CONFIG_LV_CALENDAR_DEFAULT_DAY_NAMES
                #define LV_CALENDAR_DEFAULT_DAY_NAMES CONFIG_LV_CALENDAR_DEFAULT_DAY_NAMES
            #else
                #define LV_CALENDAR_DEFAULT_DAY_NAMES {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"}
            #endif
        #endif
    #endif

    #ifndef LV_CALENDAR_DEFAULT_MONTH_NAMES
        #ifdef CONFIG_LV_CALENDAR_DEFAULT_MONTH_NAMES
            #define LV_CALENDAR_DEFAULT_MONTH_NAMES CONFIG_LV_CALENDAR_DEFAULT_MONTH_NAMES
        #else
            #define LV_CALENDAR_DEFAULT_MONTH_NAMES {"January", "February", "March",  "April", "May",  "June", "July", "August", "September", "October", "November", "December"}
        #endif
    #endif
    #ifndef LV_USE_CALENDAR_HEADER_ARROW
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_USE_CALENDAR_HEADER_ARROW
                #define LV_USE_CALENDAR_HEADER_ARROW CONFIG_LV_USE_CALENDAR_HEADER_ARROW
            #else
                #define LV_USE_CALENDAR_HEADER_ARROW 0
            #endif
        #else
            #define LV_USE_CALENDAR_HEADER_ARROW 1
        #endif
    #endif
    #ifndef LV_USE_CALENDAR_HEADER_DROPDOWN
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_USE_CALENDAR_HEADER_DROPDOWN
                #define LV_USE_CALENDAR_HEADER_DROPDOWN CONFIG_LV_USE_CALENDAR_HEADER_DROPDOWN
            #else
                #define LV_USE_CALENDAR_HEADER_DROPDOWN 0
            #endif
        #else
            #define LV_USE_CALENDAR_HEADER_DROPDOWN 1
        #endif
    #endif
    #ifndef LV_USE_CALENDAR_CHINESE
        #ifdef CONFIG_LV_USE_CALENDAR_CHINESE
            #define LV_USE_CALENDAR_CHINESE CONFIG_LV_USE_CALENDAR_CHINESE
        #else
            #define LV_USE_CALENDAR_CHINESE 0
        #endif
    #endif
#endif  /*LV_USE_CALENDAR*/

#ifndef LV_USE_CANVAS
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_CANVAS
            #define LV_USE_CANVAS CONFIG_LV_USE_CANVAS
        #else
            #define LV_USE_CANVAS 0
        #endif
    #else
        #define LV_USE_CANVAS     1
    #endif
#endif

#ifndef LV_USE_CHART
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_CHART
            #define LV_USE_CHART CONFIG_LV_USE_CHART
        #else
            #define LV_USE_CHART 0
        #endif
    #else
        #define LV_USE_CHART      1
    #endif
#endif

#ifndef LV_USE_CHECKBOX
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_CHECKBOX
            #define LV_USE_CHECKBOX CONFIG_LV_USE_CHECKBOX
        #else
            #define LV_USE_CHECKBOX 0
        #endif
    #else
        #define LV_USE_CHECKBOX   1
    #endif
#endif

#ifndef LV_USE_DROPDOWN
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_DROPDOWN
            #define LV_USE_DROPDOWN CONFIG_LV_USE_DROPDOWN
        #else
            #define LV_USE_DROPDOWN 0
        #endif
    #else
        #define LV_USE_DROPDOWN   1   /**< Requires: lv_label */
    #endif
#endif

#ifndef LV_USE_IMAGE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_IMAGE
            #define LV_USE_IMAGE CONFIG_LV_USE_IMAGE
        #else
            #define LV_USE_IMAGE 0
        #endif
    #else
        #define LV_USE_IMAGE      1   /**< Requires: lv_label */
    #endif
#endif

#ifndef LV_USE_IMAGEBUTTON
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_IMAGEBUTTON
            #define LV_USE_IMAGEBUTTON CONFIG_LV_USE_IMAGEBUTTON
        #else
            #define LV_USE_IMAGEBUTTON 0
        #endif
    #else
        #define LV_USE_IMAGEBUTTON     1
    #endif
#endif

#ifndef LV_USE_KEYBOARD
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_KEYBOARD
            #define LV_USE_KEYBOARD CONFIG_LV_USE_KEYBOARD
        #else
            #define LV_USE_KEYBOARD 0
        #endif
    #else
        #define LV_USE_KEYBOARD   1
    #endif
#endif

#ifndef LV_USE_LABEL
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_LABEL
            #define LV_USE_LABEL CONFIG_LV_USE_LABEL
        #else
            #define LV_USE_LABEL 0
        #endif
    #else
        #define LV_USE_LABEL      1
    #endif
#endif
#if LV_USE_LABEL
    #ifndef LV_LABEL_TEXT_SELECTION
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LABEL_TEXT_SELECTION
                #define LV_LABEL_TEXT_SELECTION CONFIG_LV_LABEL_TEXT_SELECTION
            #else
                #define LV_LABEL_TEXT_SELECTION 0
            #endif
        #else
            #define LV_LABEL_TEXT_SELECTION 1   /**< Enable selecting text of the label */
        #endif
    #endif
    #ifndef LV_LABEL_LONG_TXT_HINT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LABEL_LONG_TXT_HINT
                #define LV_LABEL_LONG_TXT_HINT CONFIG_LV_LABEL_LONG_TXT_HINT
            #else
                #define LV_LABEL_LONG_TXT_HINT 0
            #endif
        #else
            #define LV_LABEL_LONG_TXT_HINT 1    /**< Store some extra info in labels to speed up drawing of very long text */
        #endif
    #endif
    #ifndef LV_LABEL_WAIT_CHAR_COUNT
        #ifdef CONFIG_LV_LABEL_WAIT_CHAR_COUNT
            #define LV_LABEL_WAIT_CHAR_COUNT CONFIG_LV_LABEL_WAIT_CHAR_COUNT
        #else
            #define LV_LABEL_WAIT_CHAR_COUNT 3  /**< The count of wait chart */
        #endif
    #endif
#endif

#ifndef LV_USE_LED
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_LED
            #define LV_USE_LED CONFIG_LV_USE_LED
        #else
            #define LV_USE_LED 0
        #endif
    #else
        #define LV_USE_LED        1
    #endif
#endif

#ifndef LV_USE_LINE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_LINE
            #define LV_USE_LINE CONFIG_LV_USE_LINE
        #else
            #define LV_USE_LINE 0
        #endif
    #else
        #define LV_USE_LINE       1
    #endif
#endif

#ifndef LV_USE_LIST
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_LIST
            #define LV_USE_LIST CONFIG_LV_USE_LIST
        #else
            #define LV_USE_LIST 0
        #endif
    #else
        #define LV_USE_LIST       1
    #endif
#endif

#ifndef LV_USE_LOTTIE
    #ifdef CONFIG_LV_USE_LOTTIE
        #define LV_USE_LOTTIE CONFIG_LV_USE_LOTTIE
    #else
        #define LV_USE_LOTTIE     0  /**< Requires: lv_canvas, thorvg */
    #endif
#endif

#ifndef LV_USE_MENU
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_MENU
            #define LV_USE_MENU CONFIG_LV_USE_MENU
        #else
            #define LV_USE_MENU 0
        #endif
    #else
        #define LV_USE_MENU       1
    #endif
#endif

#ifndef LV_USE_MSGBOX
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_MSGBOX
            #define LV_USE_MSGBOX CONFIG_LV_USE_MSGBOX
        #else
            #define LV_USE_MSGBOX 0
        #endif
    #else
        #define LV_USE_MSGBOX     1
    #endif
#endif

#ifndef LV_USE_ROLLER
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ROLLER
            #define LV_USE_ROLLER CONFIG_LV_USE_ROLLER
        #else
            #define LV_USE_ROLLER 0
        #endif
    #else
        #define LV_USE_ROLLER     1   /**< Requires: lv_label */
    #endif
#endif

#ifndef LV_USE_SCALE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SCALE
            #define LV_USE_SCALE CONFIG_LV_USE_SCALE
        #else
            #define LV_USE_SCALE 0
        #endif
    #else
        #define LV_USE_SCALE      1
    #endif
#endif

#ifndef LV_USE_SLIDER
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SLIDER
            #define LV_USE_SLIDER CONFIG_LV_USE_SLIDER
        #else
            #define LV_USE_SLIDER 0
        #endif
    #else
        #define LV_USE_SLIDER     1   /**< Requires: lv_bar */
    #endif
#endif

#ifndef LV_USE_SPAN
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SPAN
            #define LV_USE_SPAN CONFIG_LV_USE_SPAN
        #else
            #define LV_USE_SPAN 0
        #endif
    #else
        #define LV_USE_SPAN       1
    #endif
#endif
#if LV_USE_SPAN
    /** Строка текста может содержать это максимальное количество дескрипторов диапазона. */
    #ifndef LV_SPAN_SNIPPET_STACK_SIZE
        #ifdef CONFIG_LV_SPAN_SNIPPET_STACK_SIZE
            #define LV_SPAN_SNIPPET_STACK_SIZE CONFIG_LV_SPAN_SNIPPET_STACK_SIZE
        #else
            #define LV_SPAN_SNIPPET_STACK_SIZE 64
        #endif
    #endif
#endif

#ifndef LV_USE_SPINBOX
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SPINBOX
            #define LV_USE_SPINBOX CONFIG_LV_USE_SPINBOX
        #else
            #define LV_USE_SPINBOX 0
        #endif
    #else
        #define LV_USE_SPINBOX    1
    #endif
#endif

#ifndef LV_USE_SPINNER
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SPINNER
            #define LV_USE_SPINNER CONFIG_LV_USE_SPINNER
        #else
            #define LV_USE_SPINNER 0
        #endif
    #else
        #define LV_USE_SPINNER    1
    #endif
#endif

#ifndef LV_USE_SWITCH
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SWITCH
            #define LV_USE_SWITCH CONFIG_LV_USE_SWITCH
        #else
            #define LV_USE_SWITCH 0
        #endif
    #else
        #define LV_USE_SWITCH     1
    #endif
#endif

#ifndef LV_USE_TABLE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_TABLE
            #define LV_USE_TABLE CONFIG_LV_USE_TABLE
        #else
            #define LV_USE_TABLE 0
        #endif
    #else
        #define LV_USE_TABLE      1
    #endif
#endif

#ifndef LV_USE_TABVIEW
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_TABVIEW
            #define LV_USE_TABVIEW CONFIG_LV_USE_TABVIEW
        #else
            #define LV_USE_TABVIEW 0
        #endif
    #else
        #define LV_USE_TABVIEW    1
    #endif
#endif

#ifndef LV_USE_TEXTAREA
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_TEXTAREA
            #define LV_USE_TEXTAREA CONFIG_LV_USE_TEXTAREA
        #else
            #define LV_USE_TEXTAREA 0
        #endif
    #else
        #define LV_USE_TEXTAREA   1   /**< Requires: lv_label */
    #endif
#endif
#if LV_USE_TEXTAREA != 0
    #ifndef LV_TEXTAREA_DEF_PWD_SHOW_TIME
        #ifdef CONFIG_LV_TEXTAREA_DEF_PWD_SHOW_TIME
            #define LV_TEXTAREA_DEF_PWD_SHOW_TIME CONFIG_LV_TEXTAREA_DEF_PWD_SHOW_TIME
        #else
            #define LV_TEXTAREA_DEF_PWD_SHOW_TIME 1500    /**< [ms] */
        #endif
    #endif
#endif

#ifndef LV_USE_TILEVIEW
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_TILEVIEW
            #define LV_USE_TILEVIEW CONFIG_LV_USE_TILEVIEW
        #else
            #define LV_USE_TILEVIEW 0
        #endif
    #else
        #define LV_USE_TILEVIEW   1
    #endif
#endif

#ifndef LV_USE_WIN
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_WIN
            #define LV_USE_WIN CONFIG_LV_USE_WIN
        #else
            #define LV_USE_WIN 0
        #endif
    #else
        #define LV_USE_WIN        1
    #endif
#endif

#ifndef LV_USE_3DTEXTURE
    #ifdef CONFIG_LV_USE_3DTEXTURE
        #define LV_USE_3DTEXTURE CONFIG_LV_USE_3DTEXTURE
    #else
        #define LV_USE_3DTEXTURE  0
    #endif
#endif

/*==================
 * THEMES
 *==================*/
/* Документацию по темам можно найти здесь: https://docs.lvgl.io/master/common-widget-features/styles/styles.html#themes. */

/** Простая, впечатляющая и очень полная тема. */
#ifndef LV_USE_THEME_DEFAULT
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_THEME_DEFAULT
            #define LV_USE_THEME_DEFAULT CONFIG_LV_USE_THEME_DEFAULT
        #else
            #define LV_USE_THEME_DEFAULT 0
        #endif
    #else
        #define LV_USE_THEME_DEFAULT 1
    #endif
#endif
#if LV_USE_THEME_DEFAULT
    /** 0: Светлый режим; 1: Темный режим */
    #ifndef LV_THEME_DEFAULT_DARK
        #ifdef CONFIG_LV_THEME_DEFAULT_DARK
            #define LV_THEME_DEFAULT_DARK CONFIG_LV_THEME_DEFAULT_DARK
        #else
            #define LV_THEME_DEFAULT_DARK 0
        #endif
    #endif

    /** 1: Включить увеличение при нажатии */
    #ifndef LV_THEME_DEFAULT_GROW
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_THEME_DEFAULT_GROW
                #define LV_THEME_DEFAULT_GROW CONFIG_LV_THEME_DEFAULT_GROW
            #else
                #define LV_THEME_DEFAULT_GROW 0
            #endif
        #else
            #define LV_THEME_DEFAULT_GROW 1
        #endif
    #endif

    /** Время перехода по умолчанию в мс. */
    #ifndef LV_THEME_DEFAULT_TRANSITION_TIME
        #ifdef CONFIG_LV_THEME_DEFAULT_TRANSITION_TIME
            #define LV_THEME_DEFAULT_TRANSITION_TIME CONFIG_LV_THEME_DEFAULT_TRANSITION_TIME
        #else
            #define LV_THEME_DEFAULT_TRANSITION_TIME 80
        #endif
    #endif
#endif /*LV_USE_THEME_DEFAULT*/

/** Очень простая тема, которая является хорошей отправной точкой для создания собственной темы. */
#ifndef LV_USE_THEME_SIMPLE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_THEME_SIMPLE
            #define LV_USE_THEME_SIMPLE CONFIG_LV_USE_THEME_SIMPLE
        #else
            #define LV_USE_THEME_SIMPLE 0
        #endif
    #else
        #define LV_USE_THEME_SIMPLE 1
    #endif
#endif

/** Тема, предназначенная для монохромных дисплеев. */
#ifndef LV_USE_THEME_MONO
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_THEME_MONO
            #define LV_USE_THEME_MONO CONFIG_LV_USE_THEME_MONO
        #else
            #define LV_USE_THEME_MONO 0
        #endif
    #else
        #define LV_USE_THEME_MONO 1
    #endif
#endif

/*==================
 * LAYOUTS
 *==================*/
/* Документацию по макетам можно найти здесь: https://docs.lvgl.io/master/common-widget-features/layouts/index.html. */

/** Макет похож на Flexbox в CSS. */
#ifndef LV_USE_FLEX
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_FLEX
            #define LV_USE_FLEX CONFIG_LV_USE_FLEX
        #else
            #define LV_USE_FLEX 0
        #endif
    #else
        #define LV_USE_FLEX 1
    #endif
#endif

/** Макет похож на Grid в CSS. */
#ifndef LV_USE_GRID
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_GRID
            #define LV_USE_GRID CONFIG_LV_USE_GRID
        #else
            #define LV_USE_GRID 0
        #endif
    #else
        #define LV_USE_GRID 1
    #endif
#endif

/*====================
 * 3RD PARTS LIBRARIES
 *====================*/
/* Документацию по библиотекам можно найти здесь: https://docs.lvgl.io/master/libs/index.html. */

/* Интерфейсы файловой системы для распространенных API */

/** Установка буквы драйвера по умолчанию позволяет пропустить префикс драйвера в путях к файлам.
 *  Документацию о том, как использовать указанные ниже буквы идентификатора драйвера, можно найти по адресу:
 *  https://docs.lvgl.io/master/main-modules/fs.html#lv-fs-identifier-letters . */
#ifndef LV_FS_DEFAULT_DRIVER_LETTER
    #ifdef CONFIG_LV_FS_DEFAULT_DRIVER_LETTER
        #define LV_FS_DEFAULT_DRIVER_LETTER CONFIG_LV_FS_DEFAULT_DRIVER_LETTER
    #else
        #define LV_FS_DEFAULT_DRIVER_LETTER '\0'
    #endif
#endif

/** API для fopen, fread и т. д. д. */
#ifndef LV_USE_FS_STDIO
    #ifdef CONFIG_LV_USE_FS_STDIO
        #define LV_USE_FS_STDIO CONFIG_LV_USE_FS_STDIO
    #else
        #define LV_USE_FS_STDIO 0
    #endif
#endif
#if LV_USE_FS_STDIO
    #ifndef LV_FS_STDIO_LETTER
        #ifdef CONFIG_LV_FS_STDIO_LETTER
            #define LV_FS_STDIO_LETTER CONFIG_LV_FS_STDIO_LETTER
        #else
            #define LV_FS_STDIO_LETTER '\0'     /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
        #endif
    #endif
    #ifndef LV_FS_STDIO_PATH
        #ifdef CONFIG_LV_FS_STDIO_PATH
            #define LV_FS_STDIO_PATH CONFIG_LV_FS_STDIO_PATH
        #else
            #define LV_FS_STDIO_PATH ""         /**< Set the working directory. File/directory paths will be appended to it. */
        #endif
    #endif
    #ifndef LV_FS_STDIO_CACHE_SIZE
        #ifdef CONFIG_LV_FS_STDIO_CACHE_SIZE
            #define LV_FS_STDIO_CACHE_SIZE CONFIG_LV_FS_STDIO_CACHE_SIZE
        #else
            #define LV_FS_STDIO_CACHE_SIZE 0    /**< >0 to cache this number of bytes in lv_fs_read() */
        #endif
    #endif
#endif

/** API для открытия, чтения и т. д. */
#ifndef LV_USE_FS_POSIX
    #ifdef CONFIG_LV_USE_FS_POSIX
        #define LV_USE_FS_POSIX CONFIG_LV_USE_FS_POSIX
    #else
        #define LV_USE_FS_POSIX 0
    #endif
#endif
#if LV_USE_FS_POSIX
    #ifndef LV_FS_POSIX_LETTER
        #ifdef CONFIG_LV_FS_POSIX_LETTER
            #define LV_FS_POSIX_LETTER CONFIG_LV_FS_POSIX_LETTER
        #else
            #define LV_FS_POSIX_LETTER '\0'     /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
        #endif
    #endif
    #ifndef LV_FS_POSIX_PATH
        #ifdef CONFIG_LV_FS_POSIX_PATH
            #define LV_FS_POSIX_PATH CONFIG_LV_FS_POSIX_PATH
        #else
            #define LV_FS_POSIX_PATH ""         /**< Set the working directory. File/directory paths will be appended to it. */
        #endif
    #endif
    #ifndef LV_FS_POSIX_CACHE_SIZE
        #ifdef CONFIG_LV_FS_POSIX_CACHE_SIZE
            #define LV_FS_POSIX_CACHE_SIZE CONFIG_LV_FS_POSIX_CACHE_SIZE
        #else
            #define LV_FS_POSIX_CACHE_SIZE 0    /**< >0 to cache this number of bytes in lv_fs_read() */
        #endif
    #endif
#endif

/** API для CreateFile, ReadFile и т. д. д. */
#ifndef LV_USE_FS_WIN32
    #ifdef CONFIG_LV_USE_FS_WIN32
        #define LV_USE_FS_WIN32 CONFIG_LV_USE_FS_WIN32
    #else
        #define LV_USE_FS_WIN32 0
    #endif
#endif
#if LV_USE_FS_WIN32
    #ifndef LV_FS_WIN32_LETTER
        #ifdef CONFIG_LV_FS_WIN32_LETTER
            #define LV_FS_WIN32_LETTER CONFIG_LV_FS_WIN32_LETTER
        #else
            #define LV_FS_WIN32_LETTER '\0'     /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
        #endif
    #endif
    #ifndef LV_FS_WIN32_PATH
        #ifdef CONFIG_LV_FS_WIN32_PATH
            #define LV_FS_WIN32_PATH CONFIG_LV_FS_WIN32_PATH
        #else
            #define LV_FS_WIN32_PATH ""         /**< Set the working directory. File/directory paths will be appended to it. */
        #endif
    #endif
    #ifndef LV_FS_WIN32_CACHE_SIZE
        #ifdef CONFIG_LV_FS_WIN32_CACHE_SIZE
            #define LV_FS_WIN32_CACHE_SIZE CONFIG_LV_FS_WIN32_CACHE_SIZE
        #else
            #define LV_FS_WIN32_CACHE_SIZE 0    /**< >0 to cache this number of bytes in lv_fs_read() */
        #endif
    #endif
#endif

/** API дляFATFS(нужно добавлять отдельно). Используетf_open,f_readи т. д. д. */
#ifndef LV_USE_FS_FATFS
    #ifdef CONFIG_LV_USE_FS_FATFS
        #define LV_USE_FS_FATFS CONFIG_LV_USE_FS_FATFS
    #else
        #define LV_USE_FS_FATFS 0
    #endif
#endif
#if LV_USE_FS_FATFS
    #ifndef LV_FS_FATFS_LETTER
        #ifdef CONFIG_LV_FS_FATFS_LETTER
            #define LV_FS_FATFS_LETTER CONFIG_LV_FS_FATFS_LETTER
        #else
            #define LV_FS_FATFS_LETTER '\0'     /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
        #endif
    #endif
    #ifndef LV_FS_FATFS_PATH
        #ifdef CONFIG_LV_FS_FATFS_PATH
            #define LV_FS_FATFS_PATH CONFIG_LV_FS_FATFS_PATH
        #else
            #define LV_FS_FATFS_PATH ""         /**< Set the working directory. File/directory paths will be appended to it. */
        #endif
    #endif
    #ifndef LV_FS_FATFS_CACHE_SIZE
        #ifdef CONFIG_LV_FS_FATFS_CACHE_SIZE
            #define LV_FS_FATFS_CACHE_SIZE CONFIG_LV_FS_FATFS_CACHE_SIZE
        #else
            #define LV_FS_FATFS_CACHE_SIZE 0    /**< >0 to cache this number of bytes in lv_fs_read() */
        #endif
    #endif
#endif

/** API для доступа к файлам, отображенным в памяти. */
#ifndef LV_USE_FS_MEMFS
    #ifdef CONFIG_LV_USE_FS_MEMFS
        #define LV_USE_FS_MEMFS CONFIG_LV_USE_FS_MEMFS
    #else
        #define LV_USE_FS_MEMFS 0
    #endif
#endif
#if LV_USE_FS_MEMFS
    #ifndef LV_FS_MEMFS_LETTER
        #ifdef CONFIG_LV_FS_MEMFS_LETTER
            #define LV_FS_MEMFS_LETTER CONFIG_LV_FS_MEMFS_LETTER
        #else
            #define LV_FS_MEMFS_LETTER '\0'     /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
        #endif
    #endif
#endif

/** API для LittleFs. */
#ifndef LV_USE_FS_LITTLEFS
    #ifdef CONFIG_LV_USE_FS_LITTLEFS
        #define LV_USE_FS_LITTLEFS CONFIG_LV_USE_FS_LITTLEFS
    #else
        #define LV_USE_FS_LITTLEFS 0
    #endif
#endif
#if LV_USE_FS_LITTLEFS
    #ifndef LV_FS_LITTLEFS_LETTER
        #ifdef CONFIG_LV_FS_LITTLEFS_LETTER
            #define LV_FS_LITTLEFS_LETTER CONFIG_LV_FS_LITTLEFS_LETTER
        #else
            #define LV_FS_LITTLEFS_LETTER '\0'  /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
        #endif
    #endif
    #ifndef LV_FS_LITTLEFS_PATH
        #ifdef CONFIG_LV_FS_LITTLEFS_PATH
            #define LV_FS_LITTLEFS_PATH CONFIG_LV_FS_LITTLEFS_PATH
        #else
            #define LV_FS_LITTLEFS_PATH ""      /**< Set the working directory. File/directory paths will be appended to it. */
        #endif
    #endif
#endif

/** API для Arduino LittleFs. */
#ifndef LV_USE_FS_ARDUINO_ESP_LITTLEFS
    #ifdef CONFIG_LV_USE_FS_ARDUINO_ESP_LITTLEFS
        #define LV_USE_FS_ARDUINO_ESP_LITTLEFS CONFIG_LV_USE_FS_ARDUINO_ESP_LITTLEFS
    #else
        #define LV_USE_FS_ARDUINO_ESP_LITTLEFS 0
    #endif
#endif
#if LV_USE_FS_ARDUINO_ESP_LITTLEFS
    #ifndef LV_FS_ARDUINO_ESP_LITTLEFS_LETTER
        #ifdef CONFIG_LV_FS_ARDUINO_ESP_LITTLEFS_LETTER
            #define LV_FS_ARDUINO_ESP_LITTLEFS_LETTER CONFIG_LV_FS_ARDUINO_ESP_LITTLEFS_LETTER
        #else
            #define LV_FS_ARDUINO_ESP_LITTLEFS_LETTER '\0'  /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
        #endif
    #endif
    #ifndef LV_FS_ARDUINO_ESP_LITTLEFS_PATH
        #ifdef CONFIG_LV_FS_ARDUINO_ESP_LITTLEFS_PATH
            #define LV_FS_ARDUINO_ESP_LITTLEFS_PATH CONFIG_LV_FS_ARDUINO_ESP_LITTLEFS_PATH
        #else
            #define LV_FS_ARDUINO_ESP_LITTLEFS_PATH ""      /**< Set the working directory. File/directory paths will be appended to it. */
        #endif
    #endif
#endif

/** API для Arduino Sd. */
#ifndef LV_USE_FS_ARDUINO_SD
    #ifdef CONFIG_LV_USE_FS_ARDUINO_SD
        #define LV_USE_FS_ARDUINO_SD CONFIG_LV_USE_FS_ARDUINO_SD
    #else
        #define LV_USE_FS_ARDUINO_SD 0
    #endif
#endif
#if LV_USE_FS_ARDUINO_SD
    #ifndef LV_FS_ARDUINO_SD_LETTER
        #ifdef CONFIG_LV_FS_ARDUINO_SD_LETTER
            #define LV_FS_ARDUINO_SD_LETTER CONFIG_LV_FS_ARDUINO_SD_LETTER
        #else
            #define LV_FS_ARDUINO_SD_LETTER '\0'  /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
        #endif
    #endif
    #ifndef LV_FS_ARDUINO_SD_PATH
        #ifdef CONFIG_LV_FS_ARDUINO_SD_PATH
            #define LV_FS_ARDUINO_SD_PATH CONFIG_LV_FS_ARDUINO_SD_PATH
        #else
            #define LV_FS_ARDUINO_SD_PATH ""      /**< Set the working directory. File/directory paths will be appended to it. */
        #endif
    #endif
#endif

/** API для UEFI */
#ifndef LV_USE_FS_UEFI
    #ifdef CONFIG_LV_USE_FS_UEFI
        #define LV_USE_FS_UEFI CONFIG_LV_USE_FS_UEFI
    #else
        #define LV_USE_FS_UEFI 0
    #endif
#endif
#if LV_USE_FS_UEFI
    #ifndef LV_FS_UEFI_LETTER
        #ifdef CONFIG_LV_FS_UEFI_LETTER
            #define LV_FS_UEFI_LETTER CONFIG_LV_FS_UEFI_LETTER
        #else
            #define LV_FS_UEFI_LETTER '\0'      /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
        #endif
    #endif
#endif

#ifndef LV_USE_FS_FROGFS
    #ifdef CONFIG_LV_USE_FS_FROGFS
        #define LV_USE_FS_FROGFS CONFIG_LV_USE_FS_FROGFS
    #else
        #define LV_USE_FS_FROGFS 0
    #endif
#endif
#if LV_USE_FS_FROGFS
    #ifndef LV_FS_FROGFS_LETTER
        #ifdef CONFIG_LV_FS_FROGFS_LETTER
            #define LV_FS_FROGFS_LETTER CONFIG_LV_FS_FROGFS_LETTER
        #else
            #define LV_FS_FROGFS_LETTER '\0'
        #endif
    #endif
#endif

/** Библиотека декодера LODEPNG */
#ifndef LV_USE_LODEPNG
    #ifdef CONFIG_LV_USE_LODEPNG
        #define LV_USE_LODEPNG CONFIG_LV_USE_LODEPNG
    #else
        #define LV_USE_LODEPNG 0
    #endif
#endif

/** Библиотека декодераPNG(libpng) */
#ifndef LV_USE_LIBPNG
    #ifdef CONFIG_LV_USE_LIBPNG
        #define LV_USE_LIBPNG CONFIG_LV_USE_LIBPNG
    #else
        #define LV_USE_LIBPNG 0
    #endif
#endif

/** Библиотека декодера BMP */
#ifndef LV_USE_BMP
    #ifdef CONFIG_LV_USE_BMP
        #define LV_USE_BMP CONFIG_LV_USE_BMP
    #else
        #define LV_USE_BMP 0
    #endif
#endif

/** JPG + разделенная библиотека декодера JPG.
 *  SplitJPG— это специальный формат, безопасный для встроенных систем. */
#ifndef LV_USE_TJPGD
    #ifdef CONFIG_LV_USE_TJPGD
        #define LV_USE_TJPGD CONFIG_LV_USE_TJPGD
    #else
        #define LV_USE_TJPGD 0
    #endif
#endif

/** библиотека декодера libjpeg-turbo.
 *  - Поддерживает полные спецификацииJPEGи высокопроизводительное декодирование JPEG. */
#ifndef LV_USE_LIBJPEG_TURBO
    #ifdef CONFIG_LV_USE_LIBJPEG_TURBO
        #define LV_USE_LIBJPEG_TURBO CONFIG_LV_USE_LIBJPEG_TURBO
    #else
        #define LV_USE_LIBJPEG_TURBO 0
    #endif
#endif

/** Библиотека декодера WebP */
#ifndef LV_USE_LIBWEBP
    #ifdef CONFIG_LV_USE_LIBWEBP
        #define LV_USE_LIBWEBP CONFIG_LV_USE_LIBWEBP
    #else
        #define LV_USE_LIBWEBP 0
    #endif
#endif

/** Библиотека декодера GIF */
#ifndef LV_USE_GIF
    #ifdef CONFIG_LV_USE_GIF
        #define LV_USE_GIF CONFIG_LV_USE_GIF
    #else
        #define LV_USE_GIF 0
    #endif
#endif
#if LV_USE_GIF
    /** Ускорение декодера GIF */
    #ifndef LV_GIF_CACHE_DECODE_DATA
        #ifdef CONFIG_LV_GIF_CACHE_DECODE_DATA
            #define LV_GIF_CACHE_DECODE_DATA CONFIG_LV_GIF_CACHE_DECODE_DATA
        #else
            #define LV_GIF_CACHE_DECODE_DATA 0
        #endif
    #endif
#endif

/** библиотека GStreamer */
#ifndef LV_USE_GSTREAMER
    #ifdef CONFIG_LV_USE_GSTREAMER
        #define LV_USE_GSTREAMER CONFIG_LV_USE_GSTREAMER
    #else
        #define LV_USE_GSTREAMER 0
    #endif
#endif

/** Декодировать изображения бинов в RAM */
#ifndef LV_BIN_DECODER_RAM_LOAD
    #ifdef CONFIG_LV_BIN_DECODER_RAM_LOAD
        #define LV_BIN_DECODER_RAM_LOAD CONFIG_LV_BIN_DECODER_RAM_LOAD
    #else
        #define LV_BIN_DECODER_RAM_LOAD 0
    #endif
#endif

/** RLE распаковать библиотеку */
#ifndef LV_USE_RLE
    #ifdef CONFIG_LV_USE_RLE
        #define LV_USE_RLE CONFIG_LV_USE_RLE
    #else
        #define LV_USE_RLE 0
    #endif
#endif

/** Библиотека кода QR */
#ifndef LV_USE_QRCODE
    #ifdef CONFIG_LV_USE_QRCODE
        #define LV_USE_QRCODE CONFIG_LV_USE_QRCODE
    #else
        #define LV_USE_QRCODE 0
    #endif
#endif

/** Библиотека штрих-кодов */
#ifndef LV_USE_BARCODE
    #ifdef CONFIG_LV_USE_BARCODE
        #define LV_USE_BARCODE CONFIG_LV_USE_BARCODE
    #else
        #define LV_USE_BARCODE 0
    #endif
#endif

/** Библиотека FreeType */
#ifndef LV_USE_FREETYPE
    #ifdef CONFIG_LV_USE_FREETYPE
        #define LV_USE_FREETYPE CONFIG_LV_USE_FREETYPE
    #else
        #define LV_USE_FREETYPE 0
    #endif
#endif
#if LV_USE_FREETYPE
    /** Разрешить FreeType использовать памятьLVGLи портировать файлы */
    #ifndef LV_FREETYPE_USE_LVGL_PORT
        #ifdef CONFIG_LV_FREETYPE_USE_LVGL_PORT
            #define LV_FREETYPE_USE_LVGL_PORT CONFIG_LV_FREETYPE_USE_LVGL_PORT
        #else
            #define LV_FREETYPE_USE_LVGL_PORT 0
        #endif
    #endif

    /** Кэшируйте количество глифов во FreeType, то есть количество глифов, которые можно кэшировать.
     *  Чем выше значение, тем больше памяти будет использоваться. */
    #ifndef LV_FREETYPE_CACHE_FT_GLYPH_CNT
        #ifdef CONFIG_LV_FREETYPE_CACHE_FT_GLYPH_CNT
            #define LV_FREETYPE_CACHE_FT_GLYPH_CNT CONFIG_LV_FREETYPE_CACHE_FT_GLYPH_CNT
        #else
            #define LV_FREETYPE_CACHE_FT_GLYPH_CNT 256
        #endif
    #endif
#endif

/** Встроенный декодер TTF. */
#ifndef LV_USE_TINY_TTF
    #ifdef CONFIG_LV_USE_TINY_TTF
        #define LV_USE_TINY_TTF CONFIG_LV_USE_TINY_TTF
    #else
        #define LV_USE_TINY_TTF 0
    #endif
#endif
#if LV_USE_TINY_TTF
    /* Включить загрузку данных TTF из файлов. */
    #ifndef LV_TINY_TTF_FILE_SUPPORT
        #ifdef CONFIG_LV_TINY_TTF_FILE_SUPPORT
            #define LV_TINY_TTF_FILE_SUPPORT CONFIG_LV_TINY_TTF_FILE_SUPPORT
        #else
            #define LV_TINY_TTF_FILE_SUPPORT 0
        #endif
    #endif
    #ifndef LV_TINY_TTF_CACHE_GLYPH_CNT
        #ifdef CONFIG_LV_TINY_TTF_CACHE_GLYPH_CNT
            #define LV_TINY_TTF_CACHE_GLYPH_CNT CONFIG_LV_TINY_TTF_CACHE_GLYPH_CNT
        #else
            #define LV_TINY_TTF_CACHE_GLYPH_CNT 128
        #endif
    #endif
    #ifndef LV_TINY_TTF_CACHE_KERNING_CNT
        #ifdef CONFIG_LV_TINY_TTF_CACHE_KERNING_CNT
            #define LV_TINY_TTF_CACHE_KERNING_CNT CONFIG_LV_TINY_TTF_CACHE_KERNING_CNT
        #else
            #define LV_TINY_TTF_CACHE_KERNING_CNT 256
        #endif
    #endif
#endif

/** Библиотека Рлотти */
#ifndef LV_USE_RLOTTIE
    #ifdef CONFIG_LV_USE_RLOTTIE
        #define LV_USE_RLOTTIE CONFIG_LV_USE_RLOTTIE
    #else
        #define LV_USE_RLOTTIE 0
    #endif
#endif

/** Требуется `LV_USE_3DTEXTURE = 1` */
#ifndef LV_USE_GLTF
    #ifdef CONFIG_LV_USE_GLTF
        #define LV_USE_GLTF CONFIG_LV_USE_GLTF
    #else
        #define LV_USE_GLTF  0
    #endif
#endif

/** Включить API векторной графики
 *  Требуется `LV_USE_MATRIX = 1`
 *  и механизм рендеринга, поддерживающий векторную графику, например.
 *  ( LV_USE_DRAW_SW и LV_USE_THORVG ) или LV_USE_DRAW_VG_LITE или LV_USE_NEMA_VG . */
#ifndef LV_USE_VECTOR_GRAPHIC
    #ifdef CONFIG_LV_USE_VECTOR_GRAPHIC
        #define LV_USE_VECTOR_GRAPHIC CONFIG_LV_USE_VECTOR_GRAPHIC
    #else
        #define LV_USE_VECTOR_GRAPHIC  0
    #endif
#endif

/** Включите ThorVG (библиотеку векторной графики) из папок src/libs.
 *  Требуется LV_USE_VECTOR_GRAPHIC */
#ifndef LV_USE_THORVG_INTERNAL
    #ifdef CONFIG_LV_USE_THORVG_INTERNAL
        #define LV_USE_THORVG_INTERNAL CONFIG_LV_USE_THORVG_INTERNAL
    #else
        #define LV_USE_THORVG_INTERNAL 0
    #endif
#endif

/** Включите ThorVG, предположив, что он установлен и связан с проектом.
 *  Требуется LV_USE_VECTOR_GRAPHIC */
#ifndef LV_USE_THORVG_EXTERNAL
    #ifdef CONFIG_LV_USE_THORVG_EXTERNAL
        #define LV_USE_THORVG_EXTERNAL CONFIG_LV_USE_THORVG_EXTERNAL
    #else
        #define LV_USE_THORVG_EXTERNAL 0
    #endif
#endif

/** Включить NanoVG (библиотека векторной графики) */
#ifndef LV_USE_NANOVG
    #ifdef CONFIG_LV_USE_NANOVG
        #define LV_USE_NANOVG CONFIG_LV_USE_NANOVG
    #else
        #define LV_USE_NANOVG 0
    #endif
#endif

/** Используйте встроенную функцию lvgl LZ4. */
#ifndef LV_USE_LZ4_INTERNAL
    #ifdef CONFIG_LV_USE_LZ4_INTERNAL
        #define LV_USE_LZ4_INTERNAL CONFIG_LV_USE_LZ4_INTERNAL
    #else
        #define LV_USE_LZ4_INTERNAL  0
    #endif
#endif

/** Использовать внешнюю библиотеку LZ4. */
#ifndef LV_USE_LZ4_EXTERNAL
    #ifdef CONFIG_LV_USE_LZ4_EXTERNAL
        #define LV_USE_LZ4_EXTERNAL CONFIG_LV_USE_LZ4_EXTERNAL
    #else
        #define LV_USE_LZ4_EXTERNAL  0
    #endif
#endif

/*библиотека SVG
 *  - Требуется `LV_USE_VECTOR_GRAPHIC = 1` */
#ifndef LV_USE_SVG
    #ifdef CONFIG_LV_USE_SVG
        #define LV_USE_SVG CONFIG_LV_USE_SVG
    #else
        #define LV_USE_SVG 0
    #endif
#endif
#ifndef LV_USE_SVG_ANIMATION
    #ifdef CONFIG_LV_USE_SVG_ANIMATION
        #define LV_USE_SVG_ANIMATION CONFIG_LV_USE_SVG_ANIMATION
    #else
        #define LV_USE_SVG_ANIMATION 0
    #endif
#endif
#ifndef LV_USE_SVG_DEBUG
    #ifdef CONFIG_LV_USE_SVG_DEBUG
        #define LV_USE_SVG_DEBUG CONFIG_LV_USE_SVG_DEBUG
    #else
        #define LV_USE_SVG_DEBUG 0
    #endif
#endif

/** Библиотека FFmpeg для декодирования изображений и записи видео.
 *  Поддерживает все основные форматы изображений, поэтому не включайте с его помощью другие декодеры изображений. */
#ifndef LV_USE_FFMPEG
    #ifdef CONFIG_LV_USE_FFMPEG
        #define LV_USE_FFMPEG CONFIG_LV_USE_FFMPEG
    #else
        #define LV_USE_FFMPEG 0
    #endif
#endif
#if LV_USE_FFMPEG
    /** Выгрузить входную информацию в stderr */
    #ifndef LV_FFMPEG_DUMP_FORMAT
        #ifdef CONFIG_LV_FFMPEG_DUMP_FORMAT
            #define LV_FFMPEG_DUMP_FORMAT CONFIG_LV_FFMPEG_DUMP_FORMAT
        #else
            #define LV_FFMPEG_DUMP_FORMAT 0
        #endif
    #endif
    /** Используйте путь к файлу lvgl в виджете FFmpeg Player
     *  После включения этой функции вы не сможете открывать URL-адреса.
     *  Обратите внимание, что декодер изображений FFmpeg всегда будет использовать файловую систему lvgl. */
    #ifndef LV_FFMPEG_PLAYER_USE_LV_FS
        #ifdef CONFIG_LV_FFMPEG_PLAYER_USE_LV_FS
            #define LV_FFMPEG_PLAYER_USE_LV_FS CONFIG_LV_FFMPEG_PLAYER_USE_LV_FS
        #else
            #define LV_FFMPEG_PLAYER_USE_LV_FS 0
        #endif
    #endif
#endif

/*==================
 * OTHERS
 *==================*/
/* Документацию по представителям Великобритании ниже элементы можно найти здесь: https://docs.lvgl.io/master/auxiliary-modules/index.html. */

/** 1: Включить API для создания снимка объекта. */
#ifndef LV_USE_SNAPSHOT
    #ifdef CONFIG_LV_USE_SNAPSHOT
        #define LV_USE_SNAPSHOT CONFIG_LV_USE_SNAPSHOT
    #else
        #define LV_USE_SNAPSHOT 0
    #endif
#endif

/** 1: Включить компонент системного монитора. */
#ifndef LV_USE_SYSMON
    #ifdef CONFIG_LV_USE_SYSMON
        #define LV_USE_SYSMON CONFIG_LV_USE_SYSMON
    #else
        #define LV_USE_SYSMON   0
    #endif
#endif
#if LV_USE_SYSMON
    /** Получите процент простоя. Например.  uint32_tmy_get_idle (недействительно); */
    #ifndef LV_SYSMON_GET_IDLE
        #ifdef CONFIG_LV_SYSMON_GET_IDLE
            #define LV_SYSMON_GET_IDLE CONFIG_LV_SYSMON_GET_IDLE
        #else
            #define LV_SYSMON_GET_IDLE lv_os_get_idle_percent
        #endif
    #endif
    /** 1: разрешить использование lv_os_get_proc_idle_percent.*/
    #ifndef LV_SYSMON_PROC_IDLE_AVAILABLE
        #ifdef CONFIG_LV_SYSMON_PROC_IDLE_AVAILABLE
            #define LV_SYSMON_PROC_IDLE_AVAILABLE CONFIG_LV_SYSMON_PROC_IDLE_AVAILABLE
        #else
            #define LV_SYSMON_PROC_IDLE_AVAILABLE 0
        #endif
    #endif
    #if LV_SYSMON_PROC_IDLE_AVAILABLE
        /** Получите процент простоя приложений.
         * - Требуется `LV_USE_OS == LV_OS_PTHREAD` */
        #ifndef LV_SYSMON_GET_PROC_IDLE
            #ifdef CONFIG_LV_SYSMON_GET_PROC_IDLE
                #define LV_SYSMON_GET_PROC_IDLE CONFIG_LV_SYSMON_GET_PROC_IDLE
            #else
                #define LV_SYSMON_GET_PROC_IDLE lv_os_get_proc_idle_percent
            #endif
        #endif
    #endif

    /** 1: Показать использованиеCPUи количество FPS.
     *  - Требуется `LV_USE_SYSMON = 1` */
    #ifndef LV_USE_PERF_MONITOR
        #ifdef CONFIG_LV_USE_PERF_MONITOR
            #define LV_USE_PERF_MONITOR CONFIG_LV_USE_PERF_MONITOR
        #else
            #define LV_USE_PERF_MONITOR 0
        #endif
    #endif
    #if LV_USE_PERF_MONITOR
        #ifndef LV_USE_PERF_MONITOR_POS
            #ifdef CONFIG_LV_USE_PERF_MONITOR_POS
                #define LV_USE_PERF_MONITOR_POS CONFIG_LV_USE_PERF_MONITOR_POS
            #else
                #define LV_USE_PERF_MONITOR_POS LV_ALIGN_BOTTOM_RIGHT
            #endif
        #endif

        /** 0: отображает данные о производительности на экране; 1: печатает данные о производительности с использованием журнала. */
        #ifndef LV_USE_PERF_MONITOR_LOG_MODE
            #ifdef CONFIG_LV_USE_PERF_MONITOR_LOG_MODE
                #define LV_USE_PERF_MONITOR_LOG_MODE CONFIG_LV_USE_PERF_MONITOR_LOG_MODE
            #else
                #define LV_USE_PERF_MONITOR_LOG_MODE 0
            #endif
        #endif
    #endif

    /** 1: Показать используемую память и фрагментацию памяти.
     *     - Требуется `LV_USE_STDLIB_MALLOC = LV_STDLIB_BUILTIN`
     *     - Требуется `LV_USE_SYSMON = 1`*/
    #ifndef LV_USE_MEM_MONITOR
        #ifdef CONFIG_LV_USE_MEM_MONITOR
            #define LV_USE_MEM_MONITOR CONFIG_LV_USE_MEM_MONITOR
        #else
            #define LV_USE_MEM_MONITOR 0
        #endif
    #endif
    #if LV_USE_MEM_MONITOR
        #ifndef LV_USE_MEM_MONITOR_POS
            #ifdef CONFIG_LV_USE_MEM_MONITOR_POS
                #define LV_USE_MEM_MONITOR_POS CONFIG_LV_USE_MEM_MONITOR_POS
            #else
                #define LV_USE_MEM_MONITOR_POS LV_ALIGN_BOTTOM_LEFT
            #endif
        #endif
    #endif
#endif /*LV_USE_SYSMON*/

/** 1: Включить профилировщик производительности во время выполнения. */
#ifndef LV_USE_PROFILER
    #ifdef CONFIG_LV_USE_PROFILER
        #define LV_USE_PROFILER CONFIG_LV_USE_PROFILER
    #else
        #define LV_USE_PROFILER 0
    #endif
#endif
#if LV_USE_PROFILER
    /** 1: Включить встроенный профилировщик */
    #ifndef LV_USE_PROFILER_BUILTIN
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_USE_PROFILER_BUILTIN
                #define LV_USE_PROFILER_BUILTIN CONFIG_LV_USE_PROFILER_BUILTIN
            #else
                #define LV_USE_PROFILER_BUILTIN 0
            #endif
        #else
            #define LV_USE_PROFILER_BUILTIN 1
        #endif
    #endif
    #if LV_USE_PROFILER_BUILTIN
        /** Размер буфера трассировки профилировщика по умолчанию */
        #ifndef LV_PROFILER_BUILTIN_BUF_SIZE
            #ifdef CONFIG_LV_PROFILER_BUILTIN_BUF_SIZE
                #define LV_PROFILER_BUILTIN_BUF_SIZE CONFIG_LV_PROFILER_BUILTIN_BUF_SIZE
            #else
                #define LV_PROFILER_BUILTIN_BUF_SIZE (16 * 1024)     /**< [bytes] */
            #endif
        #endif
        #ifndef LV_PROFILER_BUILTIN_DEFAULT_ENABLE
            #ifdef LV_KCONFIG_PRESENT
                #ifdef CONFIG_LV_PROFILER_BUILTIN_DEFAULT_ENABLE
                    #define LV_PROFILER_BUILTIN_DEFAULT_ENABLE CONFIG_LV_PROFILER_BUILTIN_DEFAULT_ENABLE
                #else
                    #define LV_PROFILER_BUILTIN_DEFAULT_ENABLE 0
                #endif
            #else
                #define LV_PROFILER_BUILTIN_DEFAULT_ENABLE 1
            #endif
        #endif
        #ifndef LV_USE_PROFILER_BUILTIN_POSIX
            #ifdef CONFIG_LV_USE_PROFILER_BUILTIN_POSIX
                #define LV_USE_PROFILER_BUILTIN_POSIX CONFIG_LV_USE_PROFILER_BUILTIN_POSIX
            #else
                #define LV_USE_PROFILER_BUILTIN_POSIX 0 /**< Enable POSIX profiler port */
            #endif
        #endif
    #endif

    /** Заголовок для профилировщика */
    #ifndef LV_PROFILER_INCLUDE
        #ifdef CONFIG_LV_PROFILER_INCLUDE
            #define LV_PROFILER_INCLUDE CONFIG_LV_PROFILER_INCLUDE
        #else
            #define LV_PROFILER_INCLUDE "lvgl/src/misc/lv_profiler_builtin.h"
        #endif
    #endif

    /** Функция начальной точки профилировщика */
    #ifndef LV_PROFILER_BEGIN
        #ifdef CONFIG_LV_PROFILER_BEGIN
            #define LV_PROFILER_BEGIN CONFIG_LV_PROFILER_BEGIN
        #else
            #define LV_PROFILER_BEGIN    LV_PROFILER_BUILTIN_BEGIN
        #endif
    #endif

    /** Функция конечной точки профилировщика */
    #ifndef LV_PROFILER_END
        #ifdef CONFIG_LV_PROFILER_END
            #define LV_PROFILER_END CONFIG_LV_PROFILER_END
        #else
            #define LV_PROFILER_END      LV_PROFILER_BUILTIN_END
        #endif
    #endif

    /** Функция начальной точки профилировщика с пользовательским тегом */
    #ifndef LV_PROFILER_BEGIN_TAG
        #ifdef CONFIG_LV_PROFILER_BEGIN_TAG
            #define LV_PROFILER_BEGIN_TAG CONFIG_LV_PROFILER_BEGIN_TAG
        #else
            #define LV_PROFILER_BEGIN_TAG LV_PROFILER_BUILTIN_BEGIN_TAG
        #endif
    #endif

    /** Функция конечной точки профилировщика с пользовательским тегом */
    #ifndef LV_PROFILER_END_TAG
        #ifdef CONFIG_LV_PROFILER_END_TAG
            #define LV_PROFILER_END_TAG CONFIG_LV_PROFILER_END_TAG
        #else
            #define LV_PROFILER_END_TAG   LV_PROFILER_BUILTIN_END_TAG
        #endif
    #endif

    /*Включить профилировщик макета*/
    #ifndef LV_PROFILER_LAYOUT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_PROFILER_LAYOUT
                #define LV_PROFILER_LAYOUT CONFIG_LV_PROFILER_LAYOUT
            #else
                #define LV_PROFILER_LAYOUT 0
            #endif
        #else
            #define LV_PROFILER_LAYOUT 1
        #endif
    #endif

    /*Включить профилировщик disp Refr*/
    #ifndef LV_PROFILER_REFR
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_PROFILER_REFR
                #define LV_PROFILER_REFR CONFIG_LV_PROFILER_REFR
            #else
                #define LV_PROFILER_REFR 0
            #endif
        #else
            #define LV_PROFILER_REFR 1
        #endif
    #endif

    /*Включить профилировщик отрисовки*/
    #ifndef LV_PROFILER_DRAW
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_PROFILER_DRAW
                #define LV_PROFILER_DRAW CONFIG_LV_PROFILER_DRAW
            #else
                #define LV_PROFILER_DRAW 0
            #endif
        #else
            #define LV_PROFILER_DRAW 1
        #endif
    #endif

    /*Включить профилировщик разработки*/
    #ifndef LV_PROFILER_INDEV
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_PROFILER_INDEV
                #define LV_PROFILER_INDEV CONFIG_LV_PROFILER_INDEV
            #else
                #define LV_PROFILER_INDEV 0
            #endif
        #else
            #define LV_PROFILER_INDEV 1
        #endif
    #endif

    /*Включить профилировщик декодера*/
    #ifndef LV_PROFILER_DECODER
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_PROFILER_DECODER
                #define LV_PROFILER_DECODER CONFIG_LV_PROFILER_DECODER
            #else
                #define LV_PROFILER_DECODER 0
            #endif
        #else
            #define LV_PROFILER_DECODER 1
        #endif
    #endif

    /*Включить профилировщик шрифтов*/
    #ifndef LV_PROFILER_FONT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_PROFILER_FONT
                #define LV_PROFILER_FONT CONFIG_LV_PROFILER_FONT
            #else
                #define LV_PROFILER_FONT 0
            #endif
        #else
            #define LV_PROFILER_FONT 1
        #endif
    #endif

    /*Включить профилировщик ФС*/
    #ifndef LV_PROFILER_FS
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_PROFILER_FS
                #define LV_PROFILER_FS CONFIG_LV_PROFILER_FS
            #else
                #define LV_PROFILER_FS 0
            #endif
        #else
            #define LV_PROFILER_FS 1
        #endif
    #endif

    /*Включить профилировщик стилей*/
    #ifndef LV_PROFILER_STYLE
        #ifdef CONFIG_LV_PROFILER_STYLE
            #define LV_PROFILER_STYLE CONFIG_LV_PROFILER_STYLE
        #else
            #define LV_PROFILER_STYLE 0
        #endif
    #endif

    /*Включить профилировщик таймера*/
    #ifndef LV_PROFILER_TIMER
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_PROFILER_TIMER
                #define LV_PROFILER_TIMER CONFIG_LV_PROFILER_TIMER
            #else
                #define LV_PROFILER_TIMER 0
            #endif
        #else
            #define LV_PROFILER_TIMER 1
        #endif
    #endif

    /*Включить профилировщик кэша*/
    #ifndef LV_PROFILER_CACHE
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_PROFILER_CACHE
                #define LV_PROFILER_CACHE CONFIG_LV_PROFILER_CACHE
            #else
                #define LV_PROFILER_CACHE 0
            #endif
        #else
            #define LV_PROFILER_CACHE 1
        #endif
    #endif

    /*Включить профилировщик событий*/
    #ifndef LV_PROFILER_EVENT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_PROFILER_EVENT
                #define LV_PROFILER_EVENT CONFIG_LV_PROFILER_EVENT
            #else
                #define LV_PROFILER_EVENT 0
            #endif
        #else
            #define LV_PROFILER_EVENT 1
        #endif
    #endif
#endif

/** 1: Включить тест обезьяны */
#ifndef LV_USE_MONKEY
    #ifdef CONFIG_LV_USE_MONKEY
        #define LV_USE_MONKEY CONFIG_LV_USE_MONKEY
    #else
        #define LV_USE_MONKEY 0
    #endif
#endif

/** 1: Включить навигацию по сетке */
#ifndef LV_USE_GRIDNAV
    #ifdef CONFIG_LV_USE_GRIDNAV
        #define LV_USE_GRIDNAV CONFIG_LV_USE_GRIDNAV
    #else
        #define LV_USE_GRIDNAV 0
    #endif
#endif

/** 1: Включить логику фрагмента `lv_obj`. */
#ifndef LV_USE_FRAGMENT
    #ifdef CONFIG_LV_USE_FRAGMENT
        #define LV_USE_FRAGMENT CONFIG_LV_USE_FRAGMENT
    #else
        #define LV_USE_FRAGMENT 0
    #endif
#endif

/** 1: Поддержка использования изображений в качестве шрифта в виджетах меток или интервалов. */
#ifndef LV_USE_IMGFONT
    #ifdef CONFIG_LV_USE_IMGFONT
        #define LV_USE_IMGFONT CONFIG_LV_USE_IMGFONT
    #else
        #define LV_USE_IMGFONT 0
    #endif
#endif

/** 1. Включить реализацию шаблона наблюдателя. */
#ifndef LV_USE_OBSERVER
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_OBSERVER
            #define LV_USE_OBSERVER CONFIG_LV_USE_OBSERVER
        #else
            #define LV_USE_OBSERVER 0
        #endif
    #else
        #define LV_USE_OBSERVER 1
    #endif
#endif

/** 1: Включить метод ввода пиньинь
 *  - Требуется: lv_keyboard */
#ifndef LV_USE_IME_PINYIN
    #ifdef CONFIG_LV_USE_IME_PINYIN
        #define LV_USE_IME_PINYIN CONFIG_LV_USE_IME_PINYIN
    #else
        #define LV_USE_IME_PINYIN 0
    #endif
#endif
#if LV_USE_IME_PINYIN
    /** 1: Используйте тезаурус по умолчанию.
     *  @note  Если вы не используете тезаурус по умолчанию, обязательно используйте`lv_ime_pinyin`после настройки тезауруса. */
    #ifndef LV_IME_PINYIN_USE_DEFAULT_DICT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_IME_PINYIN_USE_DEFAULT_DICT
                #define LV_IME_PINYIN_USE_DEFAULT_DICT CONFIG_LV_IME_PINYIN_USE_DEFAULT_DICT
            #else
                #define LV_IME_PINYIN_USE_DEFAULT_DICT 0
            #endif
        #else
            #define LV_IME_PINYIN_USE_DEFAULT_DICT 1
        #endif
    #endif
    /** Установите максимальное количество панелей-кандидатов, которые можно отобразить.
     *  @note  Это необходимо настроить в соответствии с размером экрана. */
    #ifndef LV_IME_PINYIN_CAND_TEXT_NUM
        #ifdef CONFIG_LV_IME_PINYIN_CAND_TEXT_NUM
            #define LV_IME_PINYIN_CAND_TEXT_NUM CONFIG_LV_IME_PINYIN_CAND_TEXT_NUM
        #else
            #define LV_IME_PINYIN_CAND_TEXT_NUM 6
        #endif
    #endif

    /** Используйте 9-клавишный ввод (k9). */
    #ifndef LV_IME_PINYIN_USE_K9_MODE
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_IME_PINYIN_USE_K9_MODE
                #define LV_IME_PINYIN_USE_K9_MODE CONFIG_LV_IME_PINYIN_USE_K9_MODE
            #else
                #define LV_IME_PINYIN_USE_K9_MODE 0
            #endif
        #else
            #define LV_IME_PINYIN_USE_K9_MODE      1
        #endif
    #endif
    #if LV_IME_PINYIN_USE_K9_MODE == 1
        #ifndef LV_IME_PINYIN_K9_CAND_TEXT_NUM
            #ifdef CONFIG_LV_IME_PINYIN_K9_CAND_TEXT_NUM
                #define LV_IME_PINYIN_K9_CAND_TEXT_NUM CONFIG_LV_IME_PINYIN_K9_CAND_TEXT_NUM
            #else
                #define LV_IME_PINYIN_K9_CAND_TEXT_NUM 3
            #endif
        #endif
    #endif /*LV_IME_PINYIN_USE_K9_MODE*/
#endif

/** 1: Включите файловый менеджер.
 *  - Требуется: lv_table */
#ifndef LV_USE_FILE_EXPLORER
    #ifdef CONFIG_LV_USE_FILE_EXPLORER
        #define LV_USE_FILE_EXPLORER CONFIG_LV_USE_FILE_EXPLORER
    #else
        #define LV_USE_FILE_EXPLORER                     0
    #endif
#endif
#if LV_USE_FILE_EXPLORER
    /** Максимальная длина пути */
    #ifndef LV_FILE_EXPLORER_PATH_MAX_LEN
        #ifdef CONFIG_LV_FILE_EXPLORER_PATH_MAX_LEN
            #define LV_FILE_EXPLORER_PATH_MAX_LEN CONFIG_LV_FILE_EXPLORER_PATH_MAX_LEN
        #else
            #define LV_FILE_EXPLORER_PATH_MAX_LEN        (128)
        #endif
    #endif
    /** Панель быстрого доступа, 1: использовать, 0: не использовать.
     *  - Требуется: lv_list */
    #ifndef LV_FILE_EXPLORER_QUICK_ACCESS
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_FILE_EXPLORER_QUICK_ACCESS
                #define LV_FILE_EXPLORER_QUICK_ACCESS CONFIG_LV_FILE_EXPLORER_QUICK_ACCESS
            #else
                #define LV_FILE_EXPLORER_QUICK_ACCESS 0
            #endif
        #else
            #define LV_FILE_EXPLORER_QUICK_ACCESS        1
        #endif
    #endif
#endif

/** 1: Включить диспетчер шрифтов. */
#ifndef LV_USE_FONT_MANAGER
    #ifdef CONFIG_LV_USE_FONT_MANAGER
        #define LV_USE_FONT_MANAGER CONFIG_LV_USE_FONT_MANAGER
    #else
        #define LV_USE_FONT_MANAGER                     0
    #endif
#endif
#if LV_USE_FONT_MANAGER

/**Font manager name max length*/
#ifndef LV_FONT_MANAGER_NAME_MAX_LEN
    #ifdef CONFIG_LV_FONT_MANAGER_NAME_MAX_LEN
        #define LV_FONT_MANAGER_NAME_MAX_LEN CONFIG_LV_FONT_MANAGER_NAME_MAX_LEN
    #else
        #define LV_FONT_MANAGER_NAME_MAX_LEN            32
    #endif
#endif

#endif

/** Включите эмулируемые устройства ввода, эмуляцию времени и сравнение снимков экрана. */
#ifndef LV_USE_TEST
    #ifdef CONFIG_LV_USE_TEST
        #define LV_USE_TEST CONFIG_LV_USE_TEST
    #else
        #define LV_USE_TEST 0
    #endif
#endif
#if LV_USE_TEST

/** Включите `lv_test_screenshot_compare`.
 * Требуется lodepng и несколько дополнительных устройствMBRAM. */
#ifndef LV_USE_TEST_SCREENSHOT_COMPARE
    #ifdef CONFIG_LV_USE_TEST_SCREENSHOT_COMPARE
        #define LV_USE_TEST_SCREENSHOT_COMPARE CONFIG_LV_USE_TEST_SCREENSHOT_COMPARE
    #else
        #define LV_USE_TEST_SCREENSHOT_COMPARE 0
    #endif
#endif

#if LV_USE_TEST_SCREENSHOT_COMPARE
    /** 1. Автоматически создавать недостающие эталонные изображения.*/
    #ifndef LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE
                #define LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE CONFIG_LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE
            #else
                #define LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE 0
            #endif
        #else
            #define LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE 1
        #endif
    #endif
#endif /*LV_USE_TEST_SCREENSHOT_COMPARE*/

#endif /*LV_USE_TEST*/

/** 1. Включить поддержку перевода текста. */
#ifndef LV_USE_TRANSLATION
    #ifdef CONFIG_LV_USE_TRANSLATION
        #define LV_USE_TRANSLATION CONFIG_LV_USE_TRANSLATION
    #else
        #define LV_USE_TRANSLATION 0
    #endif
#endif

/*1: Включить стиль цветового фильтра*/
#ifndef LV_USE_COLOR_FILTER
    #ifdef CONFIG_LV_USE_COLOR_FILTER
        #define LV_USE_COLOR_FILTER CONFIG_LV_USE_COLOR_FILTER
    #else
        #define LV_USE_COLOR_FILTER     0
    #endif
#endif

/*==================
 * DEVICES
 *==================*/

/** Используйте SDL, чтобы открыть окно на PC и управлять мышью и клавиатурой. */
#ifndef LV_USE_SDL
    #ifdef CONFIG_LV_USE_SDL
        #define LV_USE_SDL CONFIG_LV_USE_SDL
    #else
        #define LV_USE_SDL              0
    #endif
#endif
#if LV_USE_SDL
    #ifndef LV_SDL_INCLUDE_PATH
        #ifdef CONFIG_LV_SDL_INCLUDE_PATH
            #define LV_SDL_INCLUDE_PATH CONFIG_LV_SDL_INCLUDE_PATH
        #else
            #define LV_SDL_INCLUDE_PATH     <SDL2/SDL.h>
        #endif
    #endif
    #ifndef LV_SDL_RENDER_MODE
        #ifdef CONFIG_LV_SDL_RENDER_MODE
            #define LV_SDL_RENDER_MODE CONFIG_LV_SDL_RENDER_MODE
        #else
            #define LV_SDL_RENDER_MODE      LV_DISPLAY_RENDER_MODE_DIRECT   /**< LV_DISPLAY_RENDER_MODE_DIRECT is recommended for best performance */
        #endif
    #endif
    #ifndef LV_SDL_BUF_COUNT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_SDL_BUF_COUNT
                #define LV_SDL_BUF_COUNT CONFIG_LV_SDL_BUF_COUNT
            #else
                #define LV_SDL_BUF_COUNT 0
            #endif
        #else
            #define LV_SDL_BUF_COUNT        1    /**< 1 or 2 */
        #endif
    #endif
    #ifndef LV_SDL_ACCELERATED
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_SDL_ACCELERATED
                #define LV_SDL_ACCELERATED CONFIG_LV_SDL_ACCELERATED
            #else
                #define LV_SDL_ACCELERATED 0
            #endif
        #else
            #define LV_SDL_ACCELERATED      1    /**< 1: Use hardware acceleration*/
        #endif
    #endif
    #ifndef LV_SDL_FULLSCREEN
        #ifdef CONFIG_LV_SDL_FULLSCREEN
            #define LV_SDL_FULLSCREEN CONFIG_LV_SDL_FULLSCREEN
        #else
            #define LV_SDL_FULLSCREEN       0    /**< 1: Make the window full screen by default */
        #endif
    #endif
    #ifndef LV_SDL_DIRECT_EXIT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_SDL_DIRECT_EXIT
                #define LV_SDL_DIRECT_EXIT CONFIG_LV_SDL_DIRECT_EXIT
            #else
                #define LV_SDL_DIRECT_EXIT 0
            #endif
        #else
            #define LV_SDL_DIRECT_EXIT      1    /**< 1: Exit the application when all SDL windows are closed */
        #endif
    #endif
    #ifndef LV_SDL_MOUSEWHEEL_MODE
        #ifdef CONFIG_LV_SDL_MOUSEWHEEL_MODE
            #define LV_SDL_MOUSEWHEEL_MODE CONFIG_LV_SDL_MOUSEWHEEL_MODE
        #else
            #define LV_SDL_MOUSEWHEEL_MODE  LV_SDL_MOUSEWHEEL_MODE_ENCODER  /*LV_SDL_MOUSEWHEEL_MODE_ENCODER/CROWN*/
        #endif
    #endif
#endif

/** Используйте X11, чтобы открыть окно на рабочем столе Linux и управлять мышью и клавиатурой. */
#ifndef LV_USE_X11
    #ifdef CONFIG_LV_USE_X11
        #define LV_USE_X11 CONFIG_LV_USE_X11
    #else
        #define LV_USE_X11              0
    #endif
#endif
#if LV_USE_X11
    #ifndef LV_X11_DIRECT_EXIT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_X11_DIRECT_EXIT
                #define LV_X11_DIRECT_EXIT CONFIG_LV_X11_DIRECT_EXIT
            #else
                #define LV_X11_DIRECT_EXIT 0
            #endif
        #else
            #define LV_X11_DIRECT_EXIT         1  /**< Exit application when all X11 windows have been closed */
        #endif
    #endif
    #ifndef LV_X11_DOUBLE_BUFFER
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_X11_DOUBLE_BUFFER
                #define LV_X11_DOUBLE_BUFFER CONFIG_LV_X11_DOUBLE_BUFFER
            #else
                #define LV_X11_DOUBLE_BUFFER 0
            #endif
        #else
            #define LV_X11_DOUBLE_BUFFER       1  /**< Use double buffers for rendering */
        #endif
    #endif
    /* Выберите только 1 из следующих режимов рендеринга (предпочтительно LV_X11_RENDER_MODE_PARTIAL!). */
    #ifndef LV_X11_RENDER_MODE_PARTIAL
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_X11_RENDER_MODE_PARTIAL
                #define LV_X11_RENDER_MODE_PARTIAL CONFIG_LV_X11_RENDER_MODE_PARTIAL
            #else
                #define LV_X11_RENDER_MODE_PARTIAL 0
            #endif
        #else
            #define LV_X11_RENDER_MODE_PARTIAL 1  /**< Partial render mode (preferred) */
        #endif
    #endif
    #ifndef LV_X11_RENDER_MODE_DIRECT
        #ifdef CONFIG_LV_X11_RENDER_MODE_DIRECT
            #define LV_X11_RENDER_MODE_DIRECT CONFIG_LV_X11_RENDER_MODE_DIRECT
        #else
            #define LV_X11_RENDER_MODE_DIRECT  0  /**< Direct render mode */
        #endif
    #endif
    #ifndef LV_X11_RENDER_MODE_FULL
        #ifdef CONFIG_LV_X11_RENDER_MODE_FULL
            #define LV_X11_RENDER_MODE_FULL CONFIG_LV_X11_RENDER_MODE_FULL
        #else
            #define LV_X11_RENDER_MODE_FULL    0  /**< Full render mode */
        #endif
    #endif
#endif

/** Используйте Wayland, чтобы открыть и закрыть окно на рабочих столах Linux или BSD. */
#ifndef LV_USE_WAYLAND
    #ifdef CONFIG_LV_USE_WAYLAND
        #define LV_USE_WAYLAND CONFIG_LV_USE_WAYLAND
    #else
        #define LV_USE_WAYLAND          0
    #endif
#endif
#if LV_USE_WAYLAND
    #ifndef LV_WAYLAND_DIRECT_EXIT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_WAYLAND_DIRECT_EXIT
                #define LV_WAYLAND_DIRECT_EXIT CONFIG_LV_WAYLAND_DIRECT_EXIT
            #else
                #define LV_WAYLAND_DIRECT_EXIT 0
            #endif
        #else
            #define LV_WAYLAND_DIRECT_EXIT          1     /**< 1: Exit the application when all Wayland windows are closed */
        #endif
    #endif
#endif

/** Драйвер для /dev/fb */
#ifndef LV_USE_LINUX_FBDEV
    #ifdef CONFIG_LV_USE_LINUX_FBDEV
        #define LV_USE_LINUX_FBDEV CONFIG_LV_USE_LINUX_FBDEV
    #else
        #define LV_USE_LINUX_FBDEV      0
    #endif
#endif
#if LV_USE_LINUX_FBDEV
    #ifndef LV_LINUX_FBDEV_BSD
        #ifdef CONFIG_LV_LINUX_FBDEV_BSD
            #define LV_LINUX_FBDEV_BSD CONFIG_LV_LINUX_FBDEV_BSD
        #else
            #define LV_LINUX_FBDEV_BSD           0
        #endif
    #endif
    #ifndef LV_LINUX_FBDEV_RENDER_MODE
        #ifdef CONFIG_LV_LINUX_FBDEV_RENDER_MODE
            #define LV_LINUX_FBDEV_RENDER_MODE CONFIG_LV_LINUX_FBDEV_RENDER_MODE
        #else
            #define LV_LINUX_FBDEV_RENDER_MODE   LV_DISPLAY_RENDER_MODE_PARTIAL
        #endif
    #endif
    #ifndef LV_LINUX_FBDEV_BUFFER_COUNT
        #ifdef CONFIG_LV_LINUX_FBDEV_BUFFER_COUNT
            #define LV_LINUX_FBDEV_BUFFER_COUNT CONFIG_LV_LINUX_FBDEV_BUFFER_COUNT
        #else
            #define LV_LINUX_FBDEV_BUFFER_COUNT  0
        #endif
    #endif
    #ifndef LV_LINUX_FBDEV_BUFFER_SIZE
        #ifdef CONFIG_LV_LINUX_FBDEV_BUFFER_SIZE
            #define LV_LINUX_FBDEV_BUFFER_SIZE CONFIG_LV_LINUX_FBDEV_BUFFER_SIZE
        #else
            #define LV_LINUX_FBDEV_BUFFER_SIZE   60
        #endif
    #endif
    #ifndef LV_LINUX_FBDEV_MMAP
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LINUX_FBDEV_MMAP
                #define LV_LINUX_FBDEV_MMAP CONFIG_LV_LINUX_FBDEV_MMAP
            #else
                #define LV_LINUX_FBDEV_MMAP 0
            #endif
        #else
            #define LV_LINUX_FBDEV_MMAP          1
        #endif
    #endif
#endif

/** Используйте Nuttx, чтобы открыть окно и управлять сенсорным дисплеем. */
#ifndef LV_USE_NUTTX
    #ifdef CONFIG_LV_USE_NUTTX
        #define LV_USE_NUTTX CONFIG_LV_USE_NUTTX
    #else
        #define LV_USE_NUTTX    0
    #endif
#endif

#if LV_USE_NUTTX
    #ifndef LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP
        #ifdef CONFIG_LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP
            #define LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP CONFIG_LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP
        #else
            #define LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP 0
        #endif
    #endif

    /** Использовать независимую кучу изображений для буфера прорисовки по умолчанию. */
    #ifndef LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP
        #ifdef CONFIG_LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP
            #define LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP CONFIG_LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP
        #else
            #define LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP    0
        #endif
    #endif

    #ifndef LV_USE_NUTTX_LIBUV
        #ifdef CONFIG_LV_USE_NUTTX_LIBUV
            #define LV_USE_NUTTX_LIBUV CONFIG_LV_USE_NUTTX_LIBUV
        #else
            #define LV_USE_NUTTX_LIBUV    0
        #endif
    #endif

    /** Используйте идею реализации Nuttx API, чтобы открыть окно и обработать сенсорный экран. */
    #ifndef LV_USE_NUTTX_CUSTOM_INIT
        #ifdef CONFIG_LV_USE_NUTTX_CUSTOM_INIT
            #define LV_USE_NUTTX_CUSTOM_INIT CONFIG_LV_USE_NUTTX_CUSTOM_INIT
        #else
            #define LV_USE_NUTTX_CUSTOM_INIT    0
        #endif
    #endif

    /** Драйвер для /dev/lcd */
    #ifndef LV_USE_NUTTX_LCD
        #ifdef CONFIG_LV_USE_NUTTX_LCD
            #define LV_USE_NUTTX_LCD CONFIG_LV_USE_NUTTX_LCD
        #else
            #define LV_USE_NUTTX_LCD      0
        #endif
    #endif
    #if LV_USE_NUTTX_LCD
        #ifndef LV_NUTTX_LCD_BUFFER_COUNT
            #ifdef CONFIG_LV_NUTTX_LCD_BUFFER_COUNT
                #define LV_NUTTX_LCD_BUFFER_COUNT CONFIG_LV_NUTTX_LCD_BUFFER_COUNT
            #else
                #define LV_NUTTX_LCD_BUFFER_COUNT    0
            #endif
        #endif
        #ifndef LV_NUTTX_LCD_BUFFER_SIZE
            #ifdef CONFIG_LV_NUTTX_LCD_BUFFER_SIZE
                #define LV_NUTTX_LCD_BUFFER_SIZE CONFIG_LV_NUTTX_LCD_BUFFER_SIZE
            #else
                #define LV_NUTTX_LCD_BUFFER_SIZE     60
            #endif
        #endif
    #endif

    /** Драйвер для /dev/input */
    #ifndef LV_USE_NUTTX_TOUCHSCREEN
        #ifdef CONFIG_LV_USE_NUTTX_TOUCHSCREEN
            #define LV_USE_NUTTX_TOUCHSCREEN CONFIG_LV_USE_NUTTX_TOUCHSCREEN
        #else
            #define LV_USE_NUTTX_TOUCHSCREEN    0
        #endif
    #endif

    /** Размер курсора сенсорного экрана в пикселях (<=0: отключить курсор) */
    #ifndef LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE
        #ifdef CONFIG_LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE
            #define LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE CONFIG_LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE
        #else
            #define LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE    0
        #endif
    #endif

    /** Драйвер для /dev/мыши */
    #ifndef LV_USE_NUTTX_MOUSE
        #ifdef CONFIG_LV_USE_NUTTX_MOUSE
            #define LV_USE_NUTTX_MOUSE CONFIG_LV_USE_NUTTX_MOUSE
        #else
            #define LV_USE_NUTTX_MOUSE    0
        #endif
    #endif

    /** Шаг движения мыши (пиксели) */
    #ifndef LV_USE_NUTTX_MOUSE_MOVE_STEP
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_USE_NUTTX_MOUSE_MOVE_STEP
                #define LV_USE_NUTTX_MOUSE_MOVE_STEP CONFIG_LV_USE_NUTTX_MOUSE_MOVE_STEP
            #else
                #define LV_USE_NUTTX_MOUSE_MOVE_STEP 0
            #endif
        #else
            #define LV_USE_NUTTX_MOUSE_MOVE_STEP    1
        #endif
    #endif

    /*Файл трассировки NuttX и его путь*/
    #ifndef LV_USE_NUTTX_TRACE_FILE
        #ifdef CONFIG_LV_USE_NUTTX_TRACE_FILE
            #define LV_USE_NUTTX_TRACE_FILE CONFIG_LV_USE_NUTTX_TRACE_FILE
        #else
            #define LV_USE_NUTTX_TRACE_FILE 0
        #endif
    #endif
    #if LV_USE_NUTTX_TRACE_FILE
        #ifndef LV_NUTTX_TRACE_FILE_PATH
            #ifdef CONFIG_LV_NUTTX_TRACE_FILE_PATH
                #define LV_NUTTX_TRACE_FILE_PATH CONFIG_LV_NUTTX_TRACE_FILE_PATH
            #else
                #define LV_NUTTX_TRACE_FILE_PATH "/data/lvgl-trace.log"
            #endif
        #endif
    #endif

#endif

/** Драйвер для /dev/dri/card */
#ifndef LV_USE_LINUX_DRM
    #ifdef CONFIG_LV_USE_LINUX_DRM
        #define LV_USE_LINUX_DRM CONFIG_LV_USE_LINUX_DRM
    #else
        #define LV_USE_LINUX_DRM        0
    #endif
#endif

#if LV_USE_LINUX_DRM

    /* Используйте библиотеку MESA GBM для выделения буферов DMA, которые можно
     * Совместно используется подсистемами и библиотеками с использованием LinuxDMA—BUFAPI.
     * Библиотека GBM призвана предоставить независимую от платформы систему управления памятью.
     * он поддерживает основных поставщиков GPU. Этот параметр требует связи с libgbm. */
    #ifndef LV_USE_LINUX_DRM_GBM_BUFFERS
        #ifdef CONFIG_LV_USE_LINUX_DRM_GBM_BUFFERS
            #define LV_USE_LINUX_DRM_GBM_BUFFERS CONFIG_LV_USE_LINUX_DRM_GBM_BUFFERS
        #else
            #define LV_USE_LINUX_DRM_GBM_BUFFERS 0
        #endif
    #endif
#endif

/** Интерфейс для TFT_eSPI */
#ifndef LV_USE_TFT_ESPI
    #ifdef CONFIG_LV_USE_TFT_ESPI
        #define LV_USE_TFT_ESPI CONFIG_LV_USE_TFT_ESPI
    #else
        #define LV_USE_TFT_ESPI         0
    #endif
#endif

/** Интерфейс для Lovyan_GFX */
#ifndef LV_USE_LOVYAN_GFX
    #ifdef CONFIG_LV_USE_LOVYAN_GFX
        #define LV_USE_LOVYAN_GFX CONFIG_LV_USE_LOVYAN_GFX
    #else
        #define LV_USE_LOVYAN_GFX         0
    #endif
#endif

#if LV_USE_LOVYAN_GFX
    #ifndef LV_LGFX_USER_INCLUDE
        #ifdef CONFIG_LV_LGFX_USER_INCLUDE
            #define LV_LGFX_USER_INCLUDE CONFIG_LV_LGFX_USER_INCLUDE
        #else
            #define LV_LGFX_USER_INCLUDE "lv_lgfx_user.hpp"
        #endif
    #endif

#endif /*LV_USE_LOVYAN_GFX*/

/** Драйвер для устройств ввода evdev */
#ifndef LV_USE_EVDEV
    #ifdef CONFIG_LV_USE_EVDEV
        #define LV_USE_EVDEV CONFIG_LV_USE_EVDEV
    #else
        #define LV_USE_EVDEV    0
    #endif
#endif

/** Драйвер для устройств ввода libinput */
#ifndef LV_USE_LIBINPUT
    #ifdef CONFIG_LV_USE_LIBINPUT
        #define LV_USE_LIBINPUT CONFIG_LV_USE_LIBINPUT
    #else
        #define LV_USE_LIBINPUT    0
    #endif
#endif

#if LV_USE_LIBINPUT
    #ifndef LV_LIBINPUT_BSD
        #ifdef CONFIG_LV_LIBINPUT_BSD
            #define LV_LIBINPUT_BSD CONFIG_LV_LIBINPUT_BSD
        #else
            #define LV_LIBINPUT_BSD    0
        #endif
    #endif

    /** Полная поддержка клавиатуры */
    #ifndef LV_LIBINPUT_XKB
        #ifdef CONFIG_LV_LIBINPUT_XKB
            #define LV_LIBINPUT_XKB CONFIG_LV_LIBINPUT_XKB
        #else
            #define LV_LIBINPUT_XKB             0
        #endif
    #endif
    #if LV_LIBINPUT_XKB
        /** «setxkbmap -query» может помочь найти важные значения для вашей клавиатуры. */
        #ifndef LV_LIBINPUT_XKB_KEY_MAP
            #ifdef CONFIG_LV_LIBINPUT_XKB_KEY_MAP
                #define LV_LIBINPUT_XKB_KEY_MAP CONFIG_LV_LIBINPUT_XKB_KEY_MAP
            #else
                #define LV_LIBINPUT_XKB_KEY_MAP { .rules = NULL, .model = "pc101", .layout = "us", .variant = NULL, .options = NULL }
            #endif
        #endif
    #endif
#endif

/* Драйверы для устройств LCD, подключаемых через SPI/параллельный порт */
#ifndef LV_USE_ST7735
    #ifdef CONFIG_LV_USE_ST7735
        #define LV_USE_ST7735 CONFIG_LV_USE_ST7735
    #else
        #define LV_USE_ST7735        0
    #endif
#endif
#ifndef LV_USE_ST7789
    #ifdef CONFIG_LV_USE_ST7789
        #define LV_USE_ST7789 CONFIG_LV_USE_ST7789
    #else
        #define LV_USE_ST7789        0
    #endif
#endif
#ifndef LV_USE_ST7796
    #ifdef CONFIG_LV_USE_ST7796
        #define LV_USE_ST7796 CONFIG_LV_USE_ST7796
    #else
        #define LV_USE_ST7796        0
    #endif
#endif
#ifndef LV_USE_ILI9341
    #ifdef CONFIG_LV_USE_ILI9341
        #define LV_USE_ILI9341 CONFIG_LV_USE_ILI9341
    #else
        #define LV_USE_ILI9341       0
    #endif
#endif
#ifndef LV_USE_FT81X
    #ifdef CONFIG_LV_USE_FT81X
        #define LV_USE_FT81X CONFIG_LV_USE_FT81X
    #else
        #define LV_USE_FT81X         0
    #endif
#endif
#ifndef LV_USE_NV3007
    #ifdef CONFIG_LV_USE_NV3007
        #define LV_USE_NV3007 CONFIG_LV_USE_NV3007
    #else
        #define LV_USE_NV3007        0
    #endif
#endif

#if (LV_USE_ST7735 | LV_USE_ST7789 | LV_USE_ST7796 | LV_USE_ILI9341 | LV_USE_NV3007)
    #ifndef LV_USE_GENERIC_MIPI
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_USE_GENERIC_MIPI
                #define LV_USE_GENERIC_MIPI CONFIG_LV_USE_GENERIC_MIPI
            #else
                #define LV_USE_GENERIC_MIPI 0
            #endif
        #else
            #define LV_USE_GENERIC_MIPI 1
        #endif
    #endif
#else
    #ifndef LV_USE_GENERIC_MIPI
        #ifdef CONFIG_LV_USE_GENERIC_MIPI
            #define LV_USE_GENERIC_MIPI CONFIG_LV_USE_GENERIC_MIPI
        #else
            #define LV_USE_GENERIC_MIPI 0
        #endif
    #endif
#endif

/** Драйвер для Renesas GLCD */
#ifndef LV_USE_RENESAS_GLCDC
    #ifdef CONFIG_LV_USE_RENESAS_GLCDC
        #define LV_USE_RENESAS_GLCDC CONFIG_LV_USE_RENESAS_GLCDC
    #else
        #define LV_USE_RENESAS_GLCDC    0
    #endif
#endif

/** Драйвер для ST LTDC */
#ifndef LV_USE_ST_LTDC
    #ifdef CONFIG_LV_USE_ST_LTDC
        #define LV_USE_ST_LTDC CONFIG_LV_USE_ST_LTDC
    #else
        #define LV_USE_ST_LTDC    0
    #endif
#endif
#if LV_USE_ST_LTDC
    /* Используется только частично. */
    #ifndef LV_ST_LTDC_USE_DMA2D_FLUSH
        #ifdef CONFIG_LV_ST_LTDC_USE_DMA2D_FLUSH
            #define LV_ST_LTDC_USE_DMA2D_FLUSH CONFIG_LV_ST_LTDC_USE_DMA2D_FLUSH
        #else
            #define LV_ST_LTDC_USE_DMA2D_FLUSH 0
        #endif
    #endif
#endif

/** Драйвер для NXP ELCDIF */
#ifndef LV_USE_NXP_ELCDIF
    #ifdef CONFIG_LV_USE_NXP_ELCDIF
        #define LV_USE_NXP_ELCDIF CONFIG_LV_USE_NXP_ELCDIF
    #else
        #define LV_USE_NXP_ELCDIF   0
    #endif
#endif

/** LVGL Серверная часть Windows */
#ifndef LV_USE_WINDOWS
    #ifdef CONFIG_LV_USE_WINDOWS
        #define LV_USE_WINDOWS CONFIG_LV_USE_WINDOWS
    #else
        #define LV_USE_WINDOWS    0
    #endif
#endif

/** LVGL UEFI серверная часть */
#ifndef LV_USE_UEFI
    #ifdef CONFIG_LV_USE_UEFI
        #define LV_USE_UEFI CONFIG_LV_USE_UEFI
    #else
        #define LV_USE_UEFI 0
    #endif
#endif
#if LV_USE_UEFI
    #ifndef LV_USE_UEFI_INCLUDE
        #ifdef CONFIG_LV_USE_UEFI_INCLUDE
            #define LV_USE_UEFI_INCLUDE CONFIG_LV_USE_UEFI_INCLUDE
        #else
            #define LV_USE_UEFI_INCLUDE "myefi.h"   /**< Header that hides the actual framework (EDK2, gnu-efi, ...) */
        #endif
    #endif
    #ifndef LV_UEFI_USE_MEMORY_SERVICES
        #ifdef CONFIG_LV_UEFI_USE_MEMORY_SERVICES
            #define LV_UEFI_USE_MEMORY_SERVICES CONFIG_LV_UEFI_USE_MEMORY_SERVICES
        #else
            #define LV_UEFI_USE_MEMORY_SERVICES 0   /**< Use the memory functions from the boot services table */
        #endif
    #endif
#endif

/** Используйте универсальный драйвер OpenGL, который можно использовать для встраивания в другие приложения или использовать с GLFW/EGL.
 * - Требуется LV_USE_MATRIX.
 */
#ifndef LV_USE_OPENGLES
    #ifdef CONFIG_LV_USE_OPENGLES
        #define LV_USE_OPENGLES CONFIG_LV_USE_OPENGLES
    #else
        #define LV_USE_OPENGLES   0
    #endif
#endif
#if LV_USE_OPENGLES
    #ifndef LV_USE_OPENGLES_DEBUG
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_USE_OPENGLES_DEBUG
                #define LV_USE_OPENGLES_DEBUG CONFIG_LV_USE_OPENGLES_DEBUG
            #else
                #define LV_USE_OPENGLES_DEBUG 0
            #endif
        #else
            #define LV_USE_OPENGLES_DEBUG        1    /**< Enable or disable debug for opengles */
        #endif
    #endif
#endif

/** Используйте GLFW, чтобы открыть окно на PC и управлять мышью и клавиатурой. Требуется*/
#ifndef LV_USE_GLFW
    #ifdef CONFIG_LV_USE_GLFW
        #define LV_USE_GLFW CONFIG_LV_USE_GLFW
    #else
        #define LV_USE_GLFW   0
    #endif
#endif


/** QNX Экран дисплея и драйверы ввода */
#ifndef LV_USE_QNX
    #ifdef CONFIG_LV_USE_QNX
        #define LV_USE_QNX CONFIG_LV_USE_QNX
    #else
        #define LV_USE_QNX              0
    #endif
#endif
#if LV_USE_QNX
    #ifndef LV_QNX_BUF_COUNT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_QNX_BUF_COUNT
                #define LV_QNX_BUF_COUNT CONFIG_LV_QNX_BUF_COUNT
            #else
                #define LV_QNX_BUF_COUNT 0
            #endif
        #else
            #define LV_QNX_BUF_COUNT        1    /**< 1 or 2 */
        #endif
    #endif
#endif

/** Включить или отключить функцию внешних данных и деструктора. */
#ifndef LV_USE_EXT_DATA
    #ifdef CONFIG_LV_USE_EXT_DATA
        #define LV_USE_EXT_DATA CONFIG_LV_USE_EXT_DATA
    #else
        #define LV_USE_EXT_DATA   0
    #endif
#endif

/*=====================
* BUILD OPTIONS
*======================*/

/** Включите создание примеров с помощью библиотеки. */
#ifndef LV_BUILD_EXAMPLES
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_BUILD_EXAMPLES
            #define LV_BUILD_EXAMPLES CONFIG_LV_BUILD_EXAMPLES
        #else
            #define LV_BUILD_EXAMPLES 0
        #endif
    #else
        #define LV_BUILD_EXAMPLES 1
    #endif
#endif

/** Создание демоверсий */
#ifndef LV_BUILD_DEMOS
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_BUILD_DEMOS
            #define LV_BUILD_DEMOS CONFIG_LV_BUILD_DEMOS
        #else
            #define LV_BUILD_DEMOS 0
        #endif
    #else
        #define LV_BUILD_DEMOS 1
    #endif
#endif

/*===================
 * DEMO USAGE
 ====================*/

#if LV_BUILD_DEMOS
    /** Покажите несколько виджетов. Это может потребоваться для увеличения `LV_MEM_SIZE`. */
    #ifndef LV_USE_DEMO_WIDGETS
        #ifdef CONFIG_LV_USE_DEMO_WIDGETS
            #define LV_USE_DEMO_WIDGETS CONFIG_LV_USE_DEMO_WIDGETS
        #else
            #define LV_USE_DEMO_WIDGETS 0
        #endif
    #endif

    /** Продемонстрируйте использование кодировщика и клавиатуры. */
    #ifndef LV_USE_DEMO_KEYPAD_AND_ENCODER
        #ifdef CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER
            #define LV_USE_DEMO_KEYPAD_AND_ENCODER CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER
        #else
            #define LV_USE_DEMO_KEYPAD_AND_ENCODER 0
        #endif
    #endif

    /** Проверьте свою систему */
    #ifndef LV_USE_DEMO_BENCHMARK
        #ifdef CONFIG_LV_USE_DEMO_BENCHMARK
            #define LV_USE_DEMO_BENCHMARK CONFIG_LV_USE_DEMO_BENCHMARK
        #else
            #define LV_USE_DEMO_BENCHMARK 0
        #endif
    #endif

    #if LV_USE_DEMO_BENCHMARK
        /** Используйте шрифты, в которых растровые изображения выровнены по 16 байтам и имеют шаг Nx16 байт. */
        #ifndef LV_DEMO_BENCHMARK_ALIGNED_FONTS
            #ifdef CONFIG_LV_DEMO_BENCHMARK_ALIGNED_FONTS
                #define LV_DEMO_BENCHMARK_ALIGNED_FONTS CONFIG_LV_DEMO_BENCHMARK_ALIGNED_FONTS
            #else
                #define LV_DEMO_BENCHMARK_ALIGNED_FONTS 0
            #endif
        #endif
    #endif

    /** Тест рендеринга для каждого примитива.
     *  - Требуется дисплей не менее 480x272. */
    #ifndef LV_USE_DEMO_RENDER
        #ifdef CONFIG_LV_USE_DEMO_RENDER
            #define LV_USE_DEMO_RENDER CONFIG_LV_USE_DEMO_RENDER
        #else
            #define LV_USE_DEMO_RENDER 0
        #endif
    #endif

    /** Стресс-тест для LVGL */
    #ifndef LV_USE_DEMO_STRESS
        #ifdef CONFIG_LV_USE_DEMO_STRESS
            #define LV_USE_DEMO_STRESS CONFIG_LV_USE_DEMO_STRESS
        #else
            #define LV_USE_DEMO_STRESS 0
        #endif
    #endif

    /** Демо-версия музыкального плеера */
    #ifndef LV_USE_DEMO_MUSIC
        #ifdef CONFIG_LV_USE_DEMO_MUSIC
            #define LV_USE_DEMO_MUSIC CONFIG_LV_USE_DEMO_MUSIC
        #else
            #define LV_USE_DEMO_MUSIC 0
        #endif
    #endif
    #if LV_USE_DEMO_MUSIC
        #ifndef LV_DEMO_MUSIC_SQUARE
            #ifdef CONFIG_LV_DEMO_MUSIC_SQUARE
                #define LV_DEMO_MUSIC_SQUARE CONFIG_LV_DEMO_MUSIC_SQUARE
            #else
                #define LV_DEMO_MUSIC_SQUARE    0
            #endif
        #endif
        #ifndef LV_DEMO_MUSIC_LANDSCAPE
            #ifdef CONFIG_LV_DEMO_MUSIC_LANDSCAPE
                #define LV_DEMO_MUSIC_LANDSCAPE CONFIG_LV_DEMO_MUSIC_LANDSCAPE
            #else
                #define LV_DEMO_MUSIC_LANDSCAPE 0
            #endif
        #endif
        #ifndef LV_DEMO_MUSIC_ROUND
            #ifdef CONFIG_LV_DEMO_MUSIC_ROUND
                #define LV_DEMO_MUSIC_ROUND CONFIG_LV_DEMO_MUSIC_ROUND
            #else
                #define LV_DEMO_MUSIC_ROUND     0
            #endif
        #endif
        #ifndef LV_DEMO_MUSIC_LARGE
            #ifdef CONFIG_LV_DEMO_MUSIC_LARGE
                #define LV_DEMO_MUSIC_LARGE CONFIG_LV_DEMO_MUSIC_LARGE
            #else
                #define LV_DEMO_MUSIC_LARGE     0
            #endif
        #endif
        #ifndef LV_DEMO_MUSIC_AUTO_PLAY
            #ifdef CONFIG_LV_DEMO_MUSIC_AUTO_PLAY
                #define LV_DEMO_MUSIC_AUTO_PLAY CONFIG_LV_DEMO_MUSIC_AUTO_PLAY
            #else
                #define LV_DEMO_MUSIC_AUTO_PLAY 0
            #endif
        #endif
    #endif

    /** Векторная графическая демонстрация */
    #ifndef LV_USE_DEMO_VECTOR_GRAPHIC
        #ifdef CONFIG_LV_USE_DEMO_VECTOR_GRAPHIC
            #define LV_USE_DEMO_VECTOR_GRAPHIC CONFIG_LV_USE_DEMO_VECTOR_GRAPHIC
        #else
            #define LV_USE_DEMO_VECTOR_GRAPHIC  0
        #endif
    #endif

    /** GLTF демо */
    #ifndef LV_USE_DEMO_GLTF
        #ifdef CONFIG_LV_USE_DEMO_GLTF
            #define LV_USE_DEMO_GLTF CONFIG_LV_USE_DEMO_GLTF
        #else
            #define LV_USE_DEMO_GLTF            0
        #endif
    #endif

    /*---------------------------
     * Демо с lvgl/ lv_demos
      ---------------------------*/

    /** Демонстрация гибкого макета */
    #ifndef LV_USE_DEMO_FLEX_LAYOUT
        #ifdef CONFIG_LV_USE_DEMO_FLEX_LAYOUT
            #define LV_USE_DEMO_FLEX_LAYOUT CONFIG_LV_USE_DEMO_FLEX_LAYOUT
        #else
            #define LV_USE_DEMO_FLEX_LAYOUT     0
        #endif
    #endif

    /** Многоязычная демо-версия в стиле смартфона */
    #ifndef LV_USE_DEMO_MULTILANG
        #ifdef CONFIG_LV_USE_DEMO_MULTILANG
            #define LV_USE_DEMO_MULTILANG CONFIG_LV_USE_DEMO_MULTILANG
        #else
            #define LV_USE_DEMO_MULTILANG       0
        #endif
    #endif

    /*Демонстрация возможности велосипеда с анимацией Лотти (еслиLV_USE_LOTTIEвключен)*/
    #ifndef LV_USE_DEMO_EBIKE
        #ifdef CONFIG_LV_USE_DEMO_EBIKE
            #define LV_USE_DEMO_EBIKE CONFIG_LV_USE_DEMO_EBIKE
        #else
            #define LV_USE_DEMO_EBIKE           0
        #endif
    #endif
    #if LV_USE_DEMO_EBIKE
        #ifndef LV_DEMO_EBIKE_PORTRAIT
            #ifdef CONFIG_LV_DEMO_EBIKE_PORTRAIT
                #define LV_DEMO_EBIKE_PORTRAIT CONFIG_LV_DEMO_EBIKE_PORTRAIT
            #else
                #define LV_DEMO_EBIKE_PORTRAIT  0    /*0: для 480x270..480x320, 1: для 480x800..720x1280*/
            #endif
        #endif
    #endif

    /** Демо в высоком разрешении */
    #ifndef LV_USE_DEMO_HIGH_RES
        #ifdef CONFIG_LV_USE_DEMO_HIGH_RES
            #define LV_USE_DEMO_HIGH_RES CONFIG_LV_USE_DEMO_HIGH_RES
        #else
            #define LV_USE_DEMO_HIGH_RES        0
        #endif
    #endif

    /* Демонстрация умных часов */
    #ifndef LV_USE_DEMO_SMARTWATCH
        #ifdef CONFIG_LV_USE_DEMO_SMARTWATCH
            #define LV_USE_DEMO_SMARTWATCH CONFIG_LV_USE_DEMO_SMARTWATCH
        #else
            #define LV_USE_DEMO_SMARTWATCH      0
        #endif
    #endif
#endif /* LV_BUILD_DEMOS */



/*----------------------------------
 * Окончание разбора lv_conf_template.h
 -----------------------------------*/

/*Исправить противоречивое имя*/
#define LV_USE_ANIMIMAGE LV_USE_ANIMIMG

#ifndef __ASSEMBLY__
LV_EXPORT_CONST_INT(LV_DPI_DEF);
LV_EXPORT_CONST_INT(LV_DRAW_BUF_STRIDE_ALIGN);
LV_EXPORT_CONST_INT(LV_DRAW_BUF_ALIGN);
#endif

#undef LV_KCONFIG_PRESENT

/* Отключите драйверы VGLite, если отрисовка VGLite отключена. */
#ifndef LV_USE_VG_LITE_DRIVER
    #define LV_USE_VG_LITE_DRIVER 0
#endif

#ifndef LV_USE_VG_LITE_THORVG
    #define LV_USE_VG_LITE_THORVG 0
#endif

/* Установите некоторые, определяет, отключена ли зависимость. */
#if LV_USE_LOG == 0
    #define LV_LOG_LEVEL            LV_LOG_LEVEL_NONE
    #define LV_LOG_TRACE_MEM        0
    #define LV_LOG_TRACE_TIMER      0
    #define LV_LOG_TRACE_INDEV      0
    #define LV_LOG_TRACE_DISP_REFR  0
    #define LV_LOG_TRACE_EVENT      0
    #define LV_LOG_TRACE_OBJ_CREATE 0
    #define LV_LOG_TRACE_LAYOUT     0
    #define LV_LOG_TRACE_ANIM       0
#endif  /*LV_USE_LOG*/

#if LV_USE_WAYLAND
    /*Автоматическое определение серверной части Wayland*/
    #if LV_USE_OPENGLES
        #define LV_WAYLAND_USE_EGL 1
        #define LV_WAYLAND_USE_G2D 0
        #define LV_WAYLAND_USE_SHM 0
    #elif LV_USE_G2D
        #define LV_WAYLAND_USE_EGL 0
        #define LV_WAYLAND_USE_G2D 1
        #define LV_WAYLAND_USE_SHM 0
    #else
        #define LV_WAYLAND_USE_EGL 0
        #define LV_WAYLAND_USE_G2D 0
        #define LV_WAYLAND_USE_SHM 1
    #endif
#else
    #define LV_WAYLAND_USE_G2D 0
    #define LV_WAYLAND_USE_SHM 0
    #define LV_WAYLAND_USE_EGL 0
#endif

#if LV_USE_LINUX_DRM
    #if LV_USE_OPENGLES
        #define LV_LINUX_DRM_USE_EGL 1
    #else
        #define LV_LINUX_DRM_USE_EGL 0
    #endif /* LV_USE_OPENGLES */
#else
    #define LV_LINUX_DRM_USE_EGL 0
#endif /*LV_USE_LINUX_DRM*/

#if LV_USE_SYSMON == 0
    #define LV_USE_PERF_MONITOR 0
    #define LV_USE_MEM_MONITOR 0
    #define LV_SYSMON_PROC_IDLE_AVAILABLE 0
#endif /*LV_USE_SYSMON*/

#if LV_USE_PERF_MONITOR == 0
    #define LV_USE_PERF_MONITOR_LOG_MODE 0
#endif /*LV_USE_PERF_MONITOR*/

#if LV_BUILD_DEMOS == 0
    #define LV_USE_DEMO_WIDGETS 0
    #define LV_USE_DEMO_KEYPAD_AND_ENCODER 0
    #define LV_USE_DEMO_BENCHMARK 0
    #define LV_USE_DEMO_RENDER 0
    #define LV_USE_DEMO_STRESS 0
    #define LV_USE_DEMO_MUSIC 0
    #define LV_USE_DEMO_VECTOR_GRAPHIC  0
    #define LV_USE_DEMO_FLEX_LAYOUT     0
    #define LV_USE_DEMO_MULTILANG       0
    #define LV_USE_DEMO_EBIKE           0
    #define LV_USE_DEMO_HIGH_RES        0
    #define LV_USE_DEMO_SMARTWATCH      0
    #define LV_USE_DEMO_GLTF            0
#endif /* LV_BUILD_DEMOS */

#ifndef LV_USE_LZ4
    #if (LV_USE_LZ4_INTERNAL || LV_USE_LZ4_EXTERNAL)
        #define LV_USE_LZ4 1
    #else
        #define LV_USE_LZ4 0
    #endif
#endif

#ifndef LV_USE_THORVG
    #if (LV_USE_THORVG_INTERNAL || LV_USE_THORVG_EXTERNAL)
        #define LV_USE_THORVG 1
    #else
        #define LV_USE_THORVG 0
    #endif
#endif

#if LV_USE_SDL && LV_USE_OPENGLES && (LV_USE_DRAW_OPENGLES || LV_USE_DRAW_NANOVG)
    #define LV_SDL_USE_EGL 1
#else
    #define LV_SDL_USE_EGL 0
#endif

#ifndef LV_USE_EGL
    #if LV_LINUX_DRM_USE_EGL || LV_WAYLAND_USE_EGL || LV_SDL_USE_EGL
        #define LV_USE_EGL 1
    #else
        #define LV_USE_EGL 0
    #endif
#endif /* LV_USE_EGL */


#if LV_USE_OS
    #if (LV_USE_FREETYPE || LV_USE_THORVG) && LV_DRAW_THREAD_STACK_SIZE < (32 * 1024)
        #error "Increase LV_DRAW_THREAD_STACK_SIZE to at least 32KB for FreeType or ThorVG."
    #endif

    #if defined(LV_DRAW_THREAD_STACKSIZE) && !defined(LV_DRAW_THREAD_STACK_SIZE)
        #warning "LV_DRAW_THREAD_STACKSIZE was renamed to LV_DRAW_THREAD_STACK_SIZE. Please update lv_conf.h or run menuconfig again."
        #define LV_DRAW_THREAD_STACK_SIZE LV_DRAW_THREAD_STACKSIZE
    #endif
#endif

/*Разрешить только буквы верхнего регистра и '/' ('/' – это особый случай для обратной совместимости).*/
#define LV_FS_IS_VALID_LETTER(l) ((l) == '/' || ((l) >= 'A' && (l) <= 'Z'))

/* Если вы работаете безlv_conf.h, ссылки typedefs со значением по умолчанию. */
#ifdef LV_CONF_SKIP
    #if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)    /*Отключить преобразование для Visual Studio*/
        #define _CRT_SECURE_NO_WARNINGS
    #endif
#endif  /*определено( LV_CONF_SKIP )*/

#endif  /*LV_CONF_INTERNAL_H*/
