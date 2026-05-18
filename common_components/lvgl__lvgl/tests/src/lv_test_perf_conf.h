/**
 * @file lv_conf.h
 * Конфигурационный файл для v9.3.0-dev
 */

/*
 * Скопируйте этот файл как `lv_conf.h`
 * 1. просто рядом с папкой `lvgl`
 * 2. или в любое другое место и
 *    - определить`LV_CONF_INCLUDE_SIMPLE`;
 *    - добавьте путь как включаемый путь.
 */

/* clang-format off */
#if 1 /* Установите значение «1», чтобы включить контент. */

    #ifndef LV_CONF_H
        #define LV_CONF_H

        #define LV_BUILD_TEST_PERF 1
        #define LV_USE_TEST 1
        /* Если вам нужно что-то включить сюда, сделайте это внутри защиты `__ASSEMBLY__`. */
        #if  0 && defined(__ASSEMBLY__)
            #include "my_include.h"
        #endif

        /*====================
        COLOR SETTINGS
        *====================*/

        /** Глубина цвета: 1 ( I1 ), 8 ( L8 ), 16 ( RGB565 ), 24 ( RGB888 ), 32 ( XRGB8888 ) */
        #define LV_COLOR_DEPTH 32

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
        #define LV_USE_STDLIB_MALLOC    LV_STDLIB_BUILTIN

        /** Возможные значения
        * - LV_STDLIB_BUILTIN: встроенная реализация LVGL.
        * - LV_STDLIB_CLIB: стандартные функции C, такие как malloc, strlen и т. д.
        * - LV_STDLIB_MICROPYTHON: реализация MicroPython.
        * - LV_STDLIB_RTTHREAD :RT— реализация потока
        * - LV_STDLIB_CUSTOM: внешняя реализация функций.
        */
        #define LV_USE_STDLIB_STRING    LV_STDLIB_BUILTIN

        /** Возможные значения
        * - LV_STDLIB_BUILTIN: встроенная реализация LVGL.
        * - LV_STDLIB_CLIB: стандартные функции C, такие как malloc, strlen и т. д.
        * - LV_STDLIB_MICROPYTHON: реализация MicroPython.
        * - LV_STDLIB_RTTHREAD :RT— реализация потока
        * - LV_STDLIB_CUSTOM: внешняя реализация функций.
        */
        #define LV_USE_STDLIB_SPRINTF   LV_STDLIB_BUILTIN

        #define LV_STDINT_INCLUDE       <stdint.h>
        #define LV_STDDEF_INCLUDE       <stddef.h>
        #define LV_STDBOOL_INCLUDE      <stdbool.h>
        #define LV_INTTYPES_INCLUDE     <inttypes.h>
        #define LV_LIMITS_INCLUDE       <limits.h>
        #define LV_STDARG_INCLUDE       <stdarg.h>

        #if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
            /** Размер доступной для`lv_malloc()`памяти в байтах (>= 2 КБ) */
            #define LV_MEM_SIZE ((32 * 1024 * 1024))          /**< [bytes] */

            /** Размер расширения памяти для`lv_malloc()`в байтах */
            #define LV_MEM_POOL_EXPAND_SIZE 0

            /** Установите адрес пула памяти вместо того, чтобы выделять его как обычный массив. Может быть и во внешнем SRAM. */
            #define LV_MEM_ADR 0     /**< 0: unused*/
            /* Вместо этого адрес определяет распределение памяти, которое будет включать получение пула памяти дляLVGL. Например.   my_malloc */
            #if LV_MEM_ADR == 0
                #undef LV_MEM_POOL_INCLUDE
                #undef LV_MEM_POOL_ALLOC
            #endif
        #endif  /*LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN*/

        /*====================
        HAL SETTINGS
        *====================*/

        /** Обновление дисплея по умолчанию, чтение с устройства ввода и период шага анимации. */
        #define LV_DEF_REFR_PERIOD  16      /**< [ms] */

        /** Точек на дюйм по умолчанию. Используется для инициализации размеров по умолчанию, таких как размер виджетов и отступы стиля.
        * (Не так важно, вы можете настроить его, чтобы изменить размеры и пробелы по умолчанию.) */
        #define LV_DPI_DEF 130              /**< [px/inch] */

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
        #define LV_USE_OS   LV_OS_NONE

        #if LV_USE_OS == LV_OS_CUSTOM
            #define LV_OS_CUSTOM_INCLUDE <stdint.h>
        #endif
        #if LV_USE_OS == LV_OS_FREERTOS
            /*
            * Разблокировка задачи RTOS с помощью прямого уведомления происходит на 45 % быстрее и требует меньше RAM.
            * чем разблокировать задачу с помощью промежуточного объекта, такого как двоичный семафор.
            * Уведомления о задачах RTOS можно использовать только в том случае, если существует только одна задача, которая может быть получателем события.
            */
            #define LV_USE_FREERTOS_TASK_NOTIFY 1
        #endif

        /*========================
        * RENDERING CONFIGURATION
        *========================*/

        /** Выровнять шаг всех слоев и изображений по этим байтам. */
        #define LV_DRAW_BUF_STRIDE_ALIGN                1

        /** Выровнять начальный адрес адресовdraw_bufпо этим байтам.*/
        #define LV_DRAW_BUF_ALIGN                       4

        /** Использование матрицы для преобразований.
        * Требования:
        * - `LV_USE_MATRIX = 1`.
        * - Механизм рендеринга должен поддерживать матричные преобразования 3x3. */
        #define LV_DRAW_TRANSFORM_USE_MATRIX            0

        /* Если виджет имеет режим наложения`style_opa < 255`(не`bg_opa`,`text_opa`и т. д.) или не NORMAL
        * перед рендерингом он буферизуется в «простой» слой. Виджет может быть буферизован небольшими частями.
        * «Преобразованные слои» (если установлены`transform_angle/zoom`) используют буферы большего размера.
        * и его нельзя рисовать частями. */

        /** Размер целевого буфера для простых фрагментов слоя. */
        #define LV_DRAW_LAYER_SIMPLE_BUF_SIZE    (24 * 1024)    /**< [bytes]*/

        /* Ограничьте максимальный объем выделенной памяти для простых и преобразованных слоев.
        * Его размер должен быть не менее `LV_DRAW_LAYER_SIMPLE_BUF_SIZE`, но если также используются преобразованные слои.
        * этого должно быть достаточно для хранения самого большого виджета (ширина x высота x 4 площади).
        * Установите значение 0, чтобы не иметь ограничений. */
        #define LV_DRAW_LAYER_MAX_MEMORY 0  /**< No limit by default [bytes]*/

        /** Размер стопки ниток для рисования.
        * NOTE: Если FreeType или ThorVG включены, рекомендуется установить значение 32 КБ или более.
        */
        #define LV_DRAW_THREAD_STACK_SIZE    (8 * 1024)         /**< [bytes]*/

        #define LV_USE_DRAW_SW 1
        #if LV_USE_DRAW_SW == 1
            /*
            * Выборочно отключите поддержку цветового формата, чтобы уменьшить размер кода.
            * NOTE: некоторые функции используют определенные цветовые форматы внутри, например.
            * - градиенты используют RGB888
            * - растровые изображения с прозрачностью могут использовать ARGB8888
            */
            #define LV_DRAW_SW_SUPPORT_RGB565       1
            #define LV_DRAW_SW_SUPPORT_RGB565A8     1
            #define LV_DRAW_SW_SUPPORT_RGB888       1
            #define LV_DRAW_SW_SUPPORT_XRGB8888     1
            #define LV_DRAW_SW_SUPPORT_ARGB8888     1
            #define LV_DRAW_SW_SUPPORT_L8           1
            #define LV_DRAW_SW_SUPPORT_AL88         1
            #define LV_DRAW_SW_SUPPORT_A8           1
            #define LV_DRAW_SW_SUPPORT_I1           1

            /* Порог яркости, позволяющий считать пиксель
            * активен в индексированном цветовом формате */
            #define LV_DRAW_SW_I1_LUM_THRESHOLD 127

            /** Установите количество единиц рисования.
            *  - > 1 требует, чтобы операционная система была включена в `LV_USE_OS`.
            *  - > 1 означает, что несколько потоков будут отображать экран параллельно. */
            #define LV_DRAW_SW_DRAW_UNIT_CNT    1

            /** Используйте Arm-2D для ускорения программного (программного) рендеринга. */
            #define LV_USE_DRAW_ARM2D_SYNC      0

            /** Включите компиляцию самостоятельной сборки Helium. */
            #define LV_USE_NATIVE_HELIUM_ASM    0

            /**
            * - 0: использовать простой рендерер, способный рисовать только простые прямоугольники с градиентом, изображениями, текстом и прямыми линиями.
            * - 1. Используйте сложный рендерер, способный рисовать закругленные углы, тени, наклонные линии и дуги. */
            #define LV_DRAW_SW_COMPLEX          1

            #if LV_DRAW_SW_COMPLEX == 1
                /** Разрешить буферизацию некоторых вычислений теней.
                *  LV_DRAW_SW_SHADOW_CACHE_SIZE — максимальный размер тени в буфере, где размер тени равен
                *  `shadow_width + radius` .  Кэширование имеет стоимостьLV_DRAW_SW_SHADOW_CACHE_SIZE^2RAM. */
                #define LV_DRAW_SW_SHADOW_CACHE_SIZE 0

                /** Установите количество максимально кэшируемых данных круга.
                *  Окружность 1/4 круга сохраняется для сглаживания.
                *  По кругу используются байты`radius * 4`(наиболее часто используются радиусы направления).
                *  - 0: отключает кэширование */
                #define LV_DRAW_SW_CIRCLE_CACHE_SIZE 4
            #endif

            #define  LV_USE_DRAW_SW_ASM     LV_DRAW_SW_ASM_NONE

            #if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_CUSTOM
                #define  LV_DRAW_SW_ASM_CUSTOM_INCLUDE ""
            #endif

            /** Включите возможность рисования в программе сложных градиентов: линейных под углом, радиальных или конических. */
            #define LV_USE_DRAW_SW_COMPLEX_GRADIENTS    0
        #endif

        /*Используйте TSi, он же (Think Silicon) NemaGFX. */
        #define LV_USE_NEMA_GFX 0

        #if LV_USE_NEMA_GFX
            /** Выберите, какой NemaGFXHALиспользовать. Возможные варианты:
            * - LV_NEMA_HAL_CUSTOM
            * - LV_NEMA_HAL_STM32 */
            #define LV_USE_NEMA_HAL LV_NEMA_HAL_CUSTOM
            #if LV_USE_NEMA_HAL == LV_NEMA_HAL_STM32
                #define LV_NEMA_STM32_HAL_INCLUDE <stm32u5xx_hal.h>
            #endif

            /*Включите операцию с векторной графикой. Доступно только в наличии библиотеки NemaVG.*/
            #define LV_USE_NEMA_VG 0
            #if LV_USE_NEMA_VG
                /*Определите разрешение приложения, используемое для выделения буфера, связанного с VG. */
                #define LV_NEMA_GFX_MAX_RESX 800
                #define LV_NEMA_GFX_MAX_RESY 600
            #endif
        #endif

        /** ИспользуйтеVG-LiteGPUNXP на платформах iMX RTxxx. */
        #define LV_USE_DRAW_VGLITE 0

        #if LV_USE_DRAW_VGLITE
            /** Включить обходной путь снижения качества печати, рекомендуемый для размера экрана > 352 пикселей. */
            #define LV_USE_VGLITE_BLIT_SPLIT 0

            #if LV_USE_OS
                /** Используйте дополнительное устройство для обработкиVG-Lite. */
                #define LV_USE_VGLITE_DRAW_THREAD 1

                #if LV_USE_VGLITE_DRAW_THREAD
                    /** Включите асинхронную отрисовку VGLite. Поставьте по очереди несколько задач и запишите их один раз на GPU. */
                    #define LV_USE_VGLITE_DRAW_ASYNC 1
                #endif
            #endif

            /** Включите заявление VGLite. */
            #define LV_USE_VGLITE_ASSERT 0
        #endif

        /** ИспользуйтеPXPNXP на платформах iMX RTxxx. */
        #define LV_USE_PXP 0

        #if LV_USE_PXP
            /** Используйте PXP для рисования.*/
            #define LV_USE_DRAW_PXP 1

            /** Используйте PXP для поворота дисплея.*/
            #define LV_USE_ROTATE_PXP 0

            #if LV_USE_DRAW_PXP && LV_USE_OS
                /** Используйте дополнительную нить для обработки PXP.*/
                #define LV_USE_PXP_DRAW_THREAD 1
            #endif

            /** Включить утверждения PXP. */
            #define LV_USE_PXP_ASSERT 0
        #endif

        /** Используйте Renesas Dave2D на платформе RA. */
        #define LV_USE_DRAW_DAVE2D 0

        /** Рисуйте, используя кэшированные текстуры SDL.*/
        #define LV_USE_DRAW_SDL 0

        /** Используйте VG-Lite GPU. */
        #define LV_USE_DRAW_VG_LITE 0

        #if LV_USE_DRAW_VG_LITE
            /** Включить пользовательскую внешнюю функциюVG-Lite «gpu_init()» */
            #define LV_VG_LITE_USE_GPU_INIT 0

            /** Включите утверждениеVG-Lite. */
            #define LV_VG_LITE_USE_ASSERT 0

            /** VG - Порог срабатывания облегченного сброса.  GPU попытается группировать эти многочисленные задачи по рисованию. */
            #define LV_VG_LITE_FLUSH_MAX_COUNT 8

            /** Включите границу для имитации тени.
            *  NOTE: что обычно повышает производительность,
            *  но не гарантирует того же качества рендеринга, что и программное обеспечение. */
            #define LV_VG_LITE_USE_BOX_SHADOW 0

            /** VG - Максимальный номер кэша облегченного градиента.
            *  @note  Использование памяти одним градиентным изображением составляет 4 КБ. */
            #define LV_VG_LITE_GRAD_CACHE_CNT 32

            /** VG - Максимальный номер кэша Lite. */
            #define LV_VG_LITE_STROKE_CACHE_CNT 32
        #endif

        /** Ускорьте смешивание, заливку и т. д. с помощью STM32 DMA2D. */
        #define LV_USE_DRAW_DMA2D 0

        #if LV_USE_DRAW_DMA2D
            #define LV_DRAW_DMA2D_HAL_INCLUDE "stm32h7xx_hal.h"

            /* если включено, пользователю необходимо вызвать `lv_draw_dma2d_transfer_complete_interrupt_handler`
            * при получении глобального прерывания DMA2D
            */
            #define LV_USE_DRAW_DMA2D_INTERRUPT 0
        #endif

        /** Рисуйте с использованием кэшированных текстур OpenGLES. */
        #define LV_USE_DRAW_OPENGLES 0

        /*=======================
        * FEATURE CONFIGURATION
        *=======================*/

        /*-------------
        * Ведение журнала
        *-----------*/

        /** Включить модуль журнала */
        #define LV_USE_LOG 1
        #if LV_USE_LOG
            /** Установите значение на один из следующих уровней детализации журнала:
            *  - LV_LOG_LEVEL_TRACE Подробная информация журнала.
            *  - LV_LOG_LEVEL_INFO Регистрируйте важные события.
            *  - LV_LOG_LEVEL_WARN Зарегистрируйте, если произошло что-то нежелательное, но не вызвало проблемы.
            *  - LV_LOG_LEVEL_ERROR Регистрируйте только критические проблемы, когда система может выйти из строя.
            *  - LV_LOG_LEVEL_USER Регистрировать только пользовательские сообщения журнала, добавленные пользователем.
            *  - LV_LOG_LEVEL_NONE Ничего не регистрируйте. */
            #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

            /** - 1: Распечатать журнал с помощью «printf»;
            *  - 0: пользователю необходимо зарегистрировать обратный вызов с помощью `lv_log_register_print_cb()`. */
            #define LV_LOG_PRINTF 1

            /** Установите обратный вызов для печати журналов.
            *  Например, `my_print`. Прототип должен быть `void my_print(lv_log_level_t level, const char * buf)`.
            *  Может быть перезаписан`lv_log_register_print_cb`. */
            //#define LV_LOG_PRINT_CB

            /** - 1: включить печать временной метки;
            *  - 0: отключить печать временной метки. */
            #define LV_LOG_USE_TIMESTAMP 1

            /** - 1: Распечатать файл и номер строки журнала;
            *  - 0: Не печатать номер файла и строки журнала. */
            #define LV_LOG_USE_FILE_LINE 1

            /* Включить/выключить LV_LOG_TRACE в модулях, которые создают огромное количество логов. */
            #define LV_LOG_TRACE_MEM        1   /**< Enable/disable trace logs in memory operations. */
            #define LV_LOG_TRACE_TIMER      1   /**< Enable/disable trace logs in timer operations. */
            #define LV_LOG_TRACE_INDEV      1   /**< Enable/disable trace logs in input device operations. */
            #define LV_LOG_TRACE_DISP_REFR  1   /**< Enable/disable trace logs in display re-draw operations. */
            #define LV_LOG_TRACE_EVENT      1   /**< Enable/disable trace logs in event dispatch logic. */
            #define LV_LOG_TRACE_OBJ_CREATE 1   /**< Enable/disable trace logs in object creation (core `obj` creation plus every widget). */
            #define LV_LOG_TRACE_LAYOUT     1   /**< Enable/disable trace logs in flex- and grid-layout operations. */
            #define LV_LOG_TRACE_ANIM       1   /**< Enable/disable trace logs in animation logic. */
            #define LV_LOG_TRACE_CACHE      1   /**< Enable/disable trace logs in cache operations. */
        #endif  /*LV_USE_LOG*/

        /*-------------
        * Утверждает
        *-----------*/

        /* Включите сбои утверждений, если операция завершается неудачно или обнаружены недопустимые данные.
        * Если LV_USE_LOG включен, в случае сбоя будет напечатано сообщение об ошибке. */
        #define LV_USE_ASSERT_NULL          1   /**< Check if the parameter is NULL. (Very fast, recommended) */
        #define LV_USE_ASSERT_MALLOC        1   /**< Checks is the memory is successfully allocated or no. (Very fast, recommended) */
        #define LV_USE_ASSERT_STYLE         0   /**< Check if the styles are properly initialized. (Very fast, recommended) */
        #define LV_USE_ASSERT_MEM_INTEGRITY 0   /**< Check the integrity of `lv_mem` after critical operations. (Slow) */
        #define LV_USE_ASSERT_OBJ           0   /**< Check the object's type and existence (e.g. not deleted). (Slow) */

        /** Добавьте собственный обработчик, когда происходит утверждение, например. чтобы перезапустить MCU. */
        #define LV_ASSERT_HANDLER_INCLUDE <stdint.h>
        #define LV_ASSERT_HANDLER while(1);     /**< Halt by default */

        /*-------------
        * Отладка
        *-----------*/

        /** 1: Нарисуйте прямоугольники случайного цвета поверх перерисованных областей. */
        #define LV_USE_REFR_DEBUG 0

        /** 1: Нарисуйте красное наложение для слоев ARGB и зеленое наложение для слоев RGB.*/
        #define LV_USE_LAYER_DEBUG 0

        /** 1: Добавляет следующие варианты поведения для отладки:
        *  - Нарисуйте наложения разных цветов для задач каждого draw_unit.
        *  - Нарисуйте индексный номер блока рисования на белом фоне.
        *  - Для слоев рисует порядковый номер единицы рисования на черном фоне. */
        #define LV_USE_PARALLEL_DRAW_DEBUG 0

        /*-------------
        * Другие
        *-----------*/

        #define LV_ENABLE_GLOBAL_CUSTOM 0
        #if LV_ENABLE_GLOBAL_CUSTOM
            /** Заголовок для включения пользователем функции «lv_global» */
            #define LV_GLOBAL_CUSTOM_INCLUDE <stdint.h>
        #endif

        /** Размер кэша по умолчанию в байтах.
        *  Используются декодеры изображений, такие как `lv_lodepng`, для хранения декодированного изображения в памяти.
        *  Если для размера не установлено значение 0, декодер не сможет декодировать, когда кэш заполнен.
        *  Если размер равен 0, функция кэширования не включена и декодированная память будет
        *  высвобождается сразу после использования. */
        #define LV_CACHE_DEF_SIZE       0

        /** Число записей кэша заголовков изображений по умолчанию. Кэш используется для хранения заголовков изображений.
        *  Основная логика аналогична `LV_CACHE_DEF_SIZE`, но для заголовков изображений. */
        #define LV_IMAGE_HEADER_CACHE_DEF_CNT 0

        /** Количество остановок, разрешенных для каждого уклона. Увеличьте это значение, чтобы разрешить больше остановок.
        *  Это добавляется (sizeof(lv_color_t) + 1) байт на каждую дополнительную остановку. */
        #define LV_GRADIENT_MAX_STOPS   2

        /** Отрегулируйте округление функций смешивания цветов. Графические процессоры могут по-разному рассчитывать сочетание цветов (смешивание).
        *  - 0: округлить вниз,
        *  - 64: округлить в большую сторону от x.75,
        *  - 128: округлить до половины,
        *  - 192: округлить в большую сторону от x.25,
        *  - 254: округлить вверх */
        #define LV_COLOR_MIX_ROUND_OFS  0

        /** Добавьте 2 32-битных переменных в каждый `lv_obj_t`, чтобы ускорить получение свойства стиля. */
        #define LV_OBJ_STYLE_CACHE      0

        /** Добавьте поле`id`в`lv_obj_t`. */
        #define LV_USE_OBJ_ID           0

        /** Автоматически назначать ID при создании объекта. */
        #define LV_OBJ_ID_AUTO_ASSIGN   LV_USE_OBJ_ID

        /** Используйте встроенные функции обработчика obj ID:
        * - lv_obj_assign_id: вызывается при создании виджета. Используйте отдельный счетчик для каждого класса виджета в виде ID.
        * - lv_obj_id_compare: Сравните ID, чтобы решить, соответствует ли он запрошенному значению.
        * - lv_obj_stringify_id : Возвращает строковый идентификатор, например. «кнопка3».
        * - lv_obj_free_id : Ничего не делает, поскольку дляIDне выделяется память.
        * При отключении эти функции должны быть реализованы пользователем.*/
        #define LV_USE_OBJ_ID_BUILTIN   1

        /** Используйте набор свойств obj/получитеAPI. */
        #define LV_USE_OBJ_PROPERTY 0

        /** Включите поддержку имен свойств. */
        #define LV_USE_OBJ_PROPERTY_NAME 1

        /* ИспользуйтеVG-Lite Simulator.
        * - Требуется: LV_USE_THORVG_INTERNAL или LV_USE_THORVG_EXTERNAL. */
        #define LV_USE_VG_LITE_THORVG  0

        #if LV_USE_VG_LITE_THORVG
            /** Включите поддержку режима наложения LVGL. */
            #define LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT 0

            /** Включить поддержку цветового формата YUV */
            #define LV_VG_LITE_THORVG_YUV_SUPPORT 0

            /** Включить поддержку расширения линейного градиента */
            #define LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT 0

            /** Включить выравнивание по 16 пикселям */
            #define LV_VG_LITE_THORVG_16PIXELS_ALIGN 1

            /** Выравнивание адреса буфера */
            #define LV_VG_LITE_THORVG_BUF_ADDR_ALIGN 64

            /** Включить многопоточный рендеринг */
            #define LV_VG_LITE_THORVG_THREAD_RENDER 0
        #endif

        /* Включите функцию распознавания жестов несколькими касаниями. */
        /* Распознавание жестов требует использования поплавков. */
        #define LV_USE_GESTURE_RECOGNITION 0

        /*=====================
        *  COMPILER SETTINGS
        *====================*/

        /** Для систем с прямым порядком байтов установите значение 1. */
        #define LV_BIG_ENDIAN_SYSTEM 0

        /** Определите пользовательские атрибуты для функции `lv_tick_inc`. */
        #define LV_ATTRIBUTE_TICK_INC

        /** Определите пользовательские атрибуты для функции `lv_timer_handler`. */
        #define LV_ATTRIBUTE_TIMER_HANDLER

        /** Определите пользовательские атрибуты для функции `lv_display_flush_ready`. */
        #define LV_ATTRIBUTE_FLUSH_READY

        /** Выровняйте буферы VG_LITE по этому количеству байт.
        *  @note  vglite_src_buf_aligned() использует это значение для проверки выравнивания переданных указателей буфера. */
        #define LV_ATTRIBUTE_MEM_ALIGN_SIZE 1

        /** Будет добавлено там, где необходимо выровнять память (с -Os данные по умолчанию могут быть не выровнены по границе).
        *  например__attribute__((выровнено(4)))*/
        #define LV_ATTRIBUTE_MEM_ALIGN

        /** Атрибут для обозначения больших массивов констант, например для растровых изображений шрифтов. */
        #define LV_ATTRIBUTE_LARGE_CONST

        /** Префикс компилятора для объявления большого массива в RAM */
        #define LV_ATTRIBUTE_LARGE_RAM_ARRAY

        /** Поместите важные для производительности функции в более быструю память (например, RAM). */
        #define LV_ATTRIBUTE_FAST_MEM

        /** Экспортировать целочисленную константу в привязку. Этот макрос используется с константами вида LV_ < CONST >, которые
        *  Также должно появиться в привязкеLVGLAPI , например MicroPython. */
        #define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning  /**< The default value just prevents GCC warning */

        /** Префикс всех глобальных внешних данных с этим */
        #define LV_ATTRIBUTE_EXTERN_DATA

        /** Используйте`float`как `lv_value_precise_t` */
        #define LV_USE_FLOAT            0

        /** Включить поддержку матрицы
        *  - Требуется `LV_USE_FLOAT = 1` */
        #define LV_USE_MATRIX           0

        /** Включите`lvgl_private.h`в`lvgl.h`для доступа к стандартным данным и функциям по умолчанию. */
        #define LV_USE_PRIVATE_API      0

        /*==================
        *   FONT USAGE
        *===================*/

        /* Шрифты Montserrat в диапазонеASCIIи некоторыми символами, использующими bpp = 4.
        * https://fonts.google.com/specimen/Montserrat */
        #define LV_FONT_MONTSERRAT_8  0
        #define LV_FONT_MONTSERRAT_10 1
        #define LV_FONT_MONTSERRAT_12 1
        #define LV_FONT_MONTSERRAT_14 1
        #define LV_FONT_MONTSERRAT_16 1
        #define LV_FONT_MONTSERRAT_18 1
        #define LV_FONT_MONTSERRAT_20 1
        #define LV_FONT_MONTSERRAT_22 1
        #define LV_FONT_MONTSERRAT_24 1
        #define LV_FONT_MONTSERRAT_26 1
        #define LV_FONT_MONTSERRAT_28 1
        #define LV_FONT_MONTSERRAT_30 0
        #define LV_FONT_MONTSERRAT_32 0
        #define LV_FONT_MONTSERRAT_34 0
        #define LV_FONT_MONTSERRAT_36 0
        #define LV_FONT_MONTSERRAT_38 0
        #define LV_FONT_MONTSERRAT_40 0
        #define LV_FONT_MONTSERRAT_42 0
        #define LV_FONT_MONTSERRAT_44 0
        #define LV_FONT_MONTSERRAT_46 0
        #define LV_FONT_MONTSERRAT_48 0

        /* Демонстрация особых возможностей */
        #define LV_FONT_MONTSERRAT_28_COMPRESSED 0  /**< bpp = 3 */
        #define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0  /**< Hebrew, Arabic, Persian letters and all their forms */

        /** Пиксельные моноширинные шрифты */
        #define LV_FONT_UNSCII_8  0
        #define LV_FONT_UNSCII_16 0

        /** При желании объявите здесь пользовательские шрифты.
        *
        *  Вы также можете использовать любой из этих шрифтов в качестве шрифта по умолчанию, и они будут доступны.
        *  глобально.  Пример:
        *
        *  @code
        *  #defineLV_FONT_CUSTOM_DECLARELV_FONT_DECLARE (my_font_1)LV_FONT_DECLARE(my_font_2)
        *  @endcode
        */
        #define LV_FONT_CUSTOM_DECLARE

        /** Всегда устанавливать шрифт по умолчанию */
        #define LV_FONT_DEFAULT &lv_font_montserrat_14

        /** Включите обработку большого шрифта и/или шрифтов с большим количеством символов.
        *  Ограничение зависит от размера шрифта, начертания шрифта и bpp.
        *  Ошибка компилятора будет вызвана, если шрифту это понадобится. */
        #define LV_FONT_FMT_TXT_LARGE 0

        /** Включает/выключает поддержку сжатых шрифтов. */
        #define LV_USE_FONT_COMPRESSED 0

        /** Включите заливку для рисования, если глиф dsc не найден. */
        #define LV_USE_FONT_PLACEHOLDER 1

        /*=================
        *  TEXT SETTINGS
        *=================*/

        /**
        * Выберите кодировку символов для строк.
        * Ваш IDE или редактор должен иметь ту же кодировку символов.
        * - LV_TXT_ENC_UTF8
        * - LV_TXT_ENC_ASCII
        */
        #define LV_TXT_ENC LV_TXT_ENC_UTF8

        /** При рендеринге текстовых строк разбивайте (переносите) текст на эти символы. */
        #define LV_TXT_BREAK_CHARS " ,.;:-_)]}"

        /** Если слово хотя бы такой длины, оно порвется там, где оно «самое красивое».
        *  Чтобы отключить, установите значение <= 0. */
        #define LV_TXT_LINE_BREAK_LONG_LEN 0

        /** Минимальное количество символов в длинном слове, помещаемое в строку перед разрывом.
        *  Зависит от LV_TXT_LINE_BREAK_LONG_LEN. */
        #define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3

        /** Минимальное количество символов в длинном слове, помещаемое в строку после разрыва.
        *  Зависит от LV_TXT_LINE_BREAK_LONG_LEN. */
        #define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3

        /** Поддержка двунаправленного текста. Позволяет смешивать текст с письмом слева направо и справа налево.
        *  Направление будет обработано в соответствии с двунаправленным алгоритмом Unicode:
        *  https://www.w3.org/International/articles/inline-bidi-markup/uba-basics */
        #define LV_USE_BIDI 0
        #if LV_USE_BIDI
            /*Установите направление по умолчанию. Поддерживаемые значения:
            *`LV_BASE_DIR_LTR` Слева направо
            *`LV_BASE_DIR_RTL` Справа налево
            *`LV_BASE_DIR_AUTO` определяет направление текстовой основы*/
            #define LV_BIDI_BASE_DIR_DEF LV_BASE_DIR_AUTO
        #endif

        /** Включить обработку на арабском/персидском языке
        *  В этих языках символы следует заменять другой формой в зависимости от их положения в тексте. */
        #define LV_USE_ARABIC_PERSIAN_CHARS 0

        /*Управляющий символ, используемый для сигнализации об изменении цвета текста.*/
        #define LV_TXT_COLOR_CMD "#"

        /*==================
        * WIDGETS
        *================*/
        /* Документацию по виджетам можно найти здесь: https://docs.lvgl.io/latest/en/html/widgets/index.html. */

        /** 1: Приводит к тому, что этим виджетам присваиваются значения по умолчанию во время создания.
        *  - lv_buttonmatrix_t : Получить карты по умолчанию: {"Btn1", "Btn2", "Btn3", "\n", "Btn4", "Btn5", ""}, иначе карта не установлена.
        *  - lv_checkbox_t: для метки строки установлено значение «Флажок», в противном случае — пустая строка.
        *  - lv_dropdown_t: для параметров установлены значения «Вариант 1», «Вариант 2», «Вариант 3», иначе значения не заданы.
        *  - lv_roller_t: для параметров установлены значения «Вариант 1», «Вариант 2», «Вариант 3», «Вариант 4», «Вариант 5», иначе значения не устанавливаются.
        *  - lv_label_t: для текста установлено значение «Текст», иначе пустая строка.
        * */
        #define LV_WIDGETS_HAS_DEFAULT_VALUE  1

        #define LV_USE_ANIMIMG    1

        #define LV_USE_ARC        1

        #define LV_USE_BAR        1

        #define LV_USE_BUTTON        1

        #define LV_USE_BUTTONMATRIX  1

        #define LV_USE_CALENDAR   1
        #if LV_USE_CALENDAR
            #define LV_CALENDAR_WEEK_STARTS_MONDAY 0
            #if LV_CALENDAR_WEEK_STARTS_MONDAY
                #define LV_CALENDAR_DEFAULT_DAY_NAMES {"Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"}
            #else
                #define LV_CALENDAR_DEFAULT_DAY_NAMES {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"}
            #endif

            #define LV_CALENDAR_DEFAULT_MONTH_NAMES {"January", "February", "March",  "April", "May",  "June", "July", "August", "September", "October", "November", "December"}
            #define LV_USE_CALENDAR_HEADER_ARROW 1
            #define LV_USE_CALENDAR_HEADER_DROPDOWN 1
            #define LV_USE_CALENDAR_CHINESE 0
        #endif  /*LV_USE_CALENDAR*/

        #define LV_USE_CANVAS     1

        #define LV_USE_CHART      1

        #define LV_USE_CHECKBOX   1

        #define LV_USE_DROPDOWN   1   /**< Requires: lv_label */

        #define LV_USE_IMAGE      1   /**< Requires: lv_label */

        #define LV_USE_IMAGEBUTTON     1

        #define LV_USE_KEYBOARD   1

        #define LV_USE_LABEL      1
        #if LV_USE_LABEL
            #define LV_LABEL_TEXT_SELECTION 1   /**< Enable selecting text of the label */
            #define LV_LABEL_LONG_TXT_HINT 1    /**< Store some extra info in labels to speed up drawing of very long text */
            #define LV_LABEL_WAIT_CHAR_COUNT 3  /**< The count of wait chart */
        #endif

        #define LV_USE_LED        1

        #define LV_USE_LINE       1

        #define LV_USE_LIST       1

        #define LV_USE_LOTTIE     0  /**< Requires: lv_canvas, thorvg */

        #define LV_USE_MENU       1

        #define LV_USE_MSGBOX     1

        #define LV_USE_ROLLER     1   /**< Requires: lv_label */

        #define LV_USE_SCALE      1

        #define LV_USE_SLIDER     1   /**< Requires: lv_bar */

        #define LV_USE_SPAN       1
        #if LV_USE_SPAN
            /** Строка текста может содержать это максимальное количество дескрипторов диапазона. */
            #define LV_SPAN_SNIPPET_STACK_SIZE 64
        #endif

        #define LV_USE_SPINBOX    1

        #define LV_USE_SPINNER    1

        #define LV_USE_SWITCH     1

        #define LV_USE_TABLE      1

        #define LV_USE_TABVIEW    1

        #define LV_USE_TEXTAREA   1   /**< Requires: lv_label */
        #if LV_USE_TEXTAREA != 0
            #define LV_TEXTAREA_DEF_PWD_SHOW_TIME 1500    /**< [ms] */
        #endif

        #define LV_USE_TILEVIEW   1

        #define LV_USE_WIN        1

        /*==================
        * THEMES
        *==================*/
        /* Документацию по темам можно найти здесь: https://docs.lvgl.io/master/overview/style.html#themes. */

        /** Простая, впечатляющая и очень полная тема. */
        #define LV_USE_THEME_DEFAULT 1
        #if LV_USE_THEME_DEFAULT
            /** 0: Светлый режим; 1: Темный режим */
            #define LV_THEME_DEFAULT_DARK 0

            /** 1: Включить увеличение при нажатии */
            #define LV_THEME_DEFAULT_GROW 1

            /** Время перехода по умолчанию в мс. */
            #define LV_THEME_DEFAULT_TRANSITION_TIME 80
        #endif /*LV_USE_THEME_DEFAULT*/

        /** Очень простая тема, которая является хорошей отправной точкой для создания собственной темы. */
        #define LV_USE_THEME_SIMPLE 1

        /** Тема, предназначенная для монохромных дисплеев. */
        #define LV_USE_THEME_MONO 1

        /*==================
        * LAYOUTS
        *==================*/
        /* Документацию по макетам можно найти здесь: https://docs.lvgl.io/master/layouts/index.html. */

        /** Макет похож на Flexbox в CSS. */
        #define LV_USE_FLEX 1

        /** Макет похож на Grid в CSS. */
        #define LV_USE_GRID 1

        /*====================
        * 3RD PARTS LIBRARIES
        *====================*/
        /* Документацию по библиотекам можно найти здесь: https://docs.lvgl.io/master/libs/index.html. */

        /* Интерфейсы файловой системы для распространенных API */

        /** Установка буквы драйвера по умолчанию позволяет пропустить префикс драйвера в путях к файлам.
        *  Документацию о том, как использовать указанные ниже буквы идентификатора драйвера, можно найти по адресу:
        *  https://docs.lvgl.io/master/main-modules/fs.html#lv-fs-identifier-letters . */
        #define LV_FS_DEFAULT_DRIVER_LETTER '\0'

        /** API для fopen, fread и т. д. д. */
        #define LV_USE_FS_STDIO 0
        #if LV_USE_FS_STDIO
            #define LV_FS_STDIO_LETTER '\0'     /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
            #define LV_FS_STDIO_PATH ""         /**< Set the working directory. File/directory paths will be appended to it. */
            #define LV_FS_STDIO_CACHE_SIZE 0    /**< >0 to cache this number of bytes in lv_fs_read() */
        #endif

        /** API для открытия, чтения и т. д. */
        #define LV_USE_FS_POSIX 0
        #if LV_USE_FS_POSIX
            #define LV_FS_POSIX_LETTER '\0'     /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
            #define LV_FS_POSIX_PATH ""         /**< Set the working directory. File/directory paths will be appended to it. */
            #define LV_FS_POSIX_CACHE_SIZE 0    /**< >0 to cache this number of bytes in lv_fs_read() */
        #endif

        /** API для CreateFile, ReadFile и т. д. д. */
        #define LV_USE_FS_WIN32 0
        #if LV_USE_FS_WIN32
            #define LV_FS_WIN32_LETTER '\0'     /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
            #define LV_FS_WIN32_PATH ""         /**< Set the working directory. File/directory paths will be appended to it. */
            #define LV_FS_WIN32_CACHE_SIZE 0    /**< >0 to cache this number of bytes in lv_fs_read() */
        #endif

        /** API дляFATFS(нужно добавлять отдельно). Используетf_open,f_readи т. д. д. */
        #define LV_USE_FS_FATFS 0
        #if LV_USE_FS_FATFS
            #define LV_FS_FATFS_LETTER '\0'     /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
            #define LV_FS_FATFS_PATH ""         /**< Set the working directory. File/directory paths will be appended to it. */
            #define LV_FS_FATFS_CACHE_SIZE 0    /**< >0 to cache this number of bytes in lv_fs_read() */
        #endif

        /** API для доступа к файлам, отображенным в памяти. */
        #define LV_USE_FS_MEMFS 0
        #if LV_USE_FS_MEMFS
            #define LV_FS_MEMFS_LETTER '\0'     /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
        #endif

        /** API для LittleFs. */
        #define LV_USE_FS_LITTLEFS 0
        #if LV_USE_FS_LITTLEFS
            #define LV_FS_LITTLEFS_LETTER '\0'  /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
            #define LV_FS_LITTLEFS_PATH ""      /**< Set the working directory. File/directory paths will be appended to it. */
        #endif

        /** API для Arduino LittleFs. */
        #define LV_USE_FS_ARDUINO_ESP_LITTLEFS 0
        #if LV_USE_FS_ARDUINO_ESP_LITTLEFS
            #define LV_FS_ARDUINO_ESP_LITTLEFS_LETTER '\0'  /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
            #define LV_FS_ARDUINO_ESP_LITTLEFS_PATH ""      /**< Set the working directory. File/directory paths will be appended to it. */
        #endif

        /** API для Arduino Sd. */
        #define LV_USE_FS_ARDUINO_SD 0
        #if LV_USE_FS_ARDUINO_SD
            #define LV_FS_ARDUINO_SD_LETTER '\0'  /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
            #define LV_FS_ARDUINO_SD_PATH ""      /**< Set the working directory. File/directory paths will be appended to it. */
        #endif

        /** API для UEFI */
        #define LV_USE_FS_UEFI 0
        #if LV_USE_FS_UEFI
            #define LV_FS_UEFI_LETTER '\0'      /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
        #endif

        /** Библиотека декодера LODEPNG */
        #define LV_USE_LODEPNG 0

        /** Библиотека декодераPNG(libpng) */
        #define LV_USE_LIBPNG 0

        /** Библиотека декодера BMP */
        #define LV_USE_BMP 0

        /** JPG + разделенная библиотека декодера JPG.
        *  SplitJPG— это специальный формат, безопасный для встроенных систем. */
        #define LV_USE_TJPGD 0

        /** библиотека декодера libjpeg-turbo.
        *  - Поддерживает полные спецификацииJPEGи высокопроизводительное декодирование JPEG. */
        #define LV_USE_LIBJPEG_TURBO 0

        /** Библиотека декодера GIF */
        #define LV_USE_GIF 0
        #if LV_USE_GIF
            /** Ускорение декодера GIF */
            #define LV_GIF_CACHE_DECODE_DATA 0
        #endif


        /** Декодировать изображения бинов в RAM */
        #define LV_BIN_DECODER_RAM_LOAD 0

        /** RLE распаковать библиотеку */
        #define LV_USE_RLE 0

        /** Библиотека кода QR */
        #define LV_USE_QRCODE 0

        /** Библиотека штрих-кодов */
        #define LV_USE_BARCODE 0

        /** Библиотека FreeType */
        #define LV_USE_FREETYPE 0
        #if LV_USE_FREETYPE
            /** Разрешить FreeType использовать памятьLVGLи портировать файлы */
            #define LV_FREETYPE_USE_LVGL_PORT 0

            /** Кэшируйте количество глифов во FreeType, то есть количество глифов, которые можно кэшировать.
            *  Чем выше значение, тем больше памяти будет использоваться. */
            #define LV_FREETYPE_CACHE_FT_GLYPH_CNT 256
        #endif

        /** Встроенный декодер TTF. */
        #define LV_USE_TINY_TTF 0
        #if LV_USE_TINY_TTF
            /* Включить загрузку данных TTF из файлов. */
            #define LV_TINY_TTF_FILE_SUPPORT 0
            #define LV_TINY_TTF_CACHE_GLYPH_CNT 256
        #endif

        /** Библиотека Рлотти */
        #define LV_USE_RLOTTIE 0

        /** Включить API векторной графики
        *  - Требуется `LV_USE_MATRIX = 1` */
        #define LV_USE_VECTOR_GRAPHIC  0

        /** Включите ThorVG (библиотеку векторной графики) из папок src/libs. */
        #define LV_USE_THORVG_INTERNAL 0

        /** Включите ThorVG, предположив, что он установлен и связан с проектом. */
        #define LV_USE_THORVG_EXTERNAL 0

        /** Используйте встроенную функцию lvgl LZ4. */
        #define LV_USE_LZ4_INTERNAL  0

        /** Использовать внешнюю библиотеку LZ4. */
        #define LV_USE_LZ4_EXTERNAL  0

        /*библиотека SVG
        *  - Требуется `LV_USE_VECTOR_GRAPHIC = 1` */
        #define LV_USE_SVG 0
        #define LV_USE_SVG_ANIMATION 0
        #define LV_USE_SVG_DEBUG 0

        /** Библиотека FFmpeg для декодирования изображений и записи видео.
        *  Поддерживает все основные форматы изображений, поэтому не включайте с его помощью другие декодеры изображений. */
        #define LV_USE_FFMPEG 0
        #if LV_USE_FFMPEG
            /** Выгрузить входную информацию в stderr */
            #define LV_FFMPEG_DUMP_FORMAT 0
            /** Используйте путь к файлу lvgl в виджете FFmpeg Player
            *  После включения этой функции вы не сможете открывать URL-адреса.
            *  Обратите внимание, что декодер изображений FFmpeg всегда будет использовать файловую систему lvgl. */
            #define LV_FFMPEG_PLAYER_USE_LV_FS 0
        #endif

        /*==================
        * OTHERS
        *==================*/
        /* Документацию по представителям Великобритании ниже элементы можно найти здесь: https://docs.lvgl.io/master/others/index.html. */

        /** 1: Включить API для создания снимка объекта. */
        #define LV_USE_SNAPSHOT 0

        /** 1: Включить компонент системного монитора. */
        #define LV_USE_SYSMON   1
        #if LV_USE_SYSMON
            /** Получите процент простоя. Например.  uint32_tmy_get_idle (недействительно); */
            #define LV_SYSMON_GET_IDLE lv_os_get_idle_percent

            /** 1: Показать использованиеCPUи количество FPS.
            *  - Требуется `LV_USE_SYSMON = 1` */
            #define LV_USE_PERF_MONITOR 1
            #if LV_USE_PERF_MONITOR
                #define LV_USE_PERF_MONITOR_POS LV_ALIGN_BOTTOM_RIGHT

                /** 0: отображает данные о производительности на экране; 1: печатает данные о производительности с использованием журнала. */
                #define LV_USE_PERF_MONITOR_LOG_MODE 0
            #endif

            /** 1: Показать используемую память и фрагментацию памяти.
            *     - Требуется `LV_USE_STDLIB_MALLOC = LV_STDLIB_BUILTIN`
            *     - Требуется `LV_USE_SYSMON = 1`*/
            #define LV_USE_MEM_MONITOR 1
            #if LV_USE_MEM_MONITOR
                #define LV_USE_MEM_MONITOR_POS LV_ALIGN_BOTTOM_LEFT
            #endif
        #endif /*LV_USE_SYSMON*/

        /** 1: Включить профилировщик производительности во время выполнения. */
        #define LV_USE_PROFILER 0
        #if LV_USE_PROFILER
            /** 1: Включить встроенный профилировщик */
            #define LV_USE_PROFILER_BUILTIN 1
            #if LV_USE_PROFILER_BUILTIN
                /** Размер буфера трассировки профилировщика по умолчанию */
                #define LV_PROFILER_BUILTIN_BUF_SIZE (16 * 1024)     /**< [bytes] */
            #endif

            /** Заголовок для профилировщика */
            #define LV_PROFILER_INCLUDE "lvgl/src/misc/lv_profiler_builtin.h"

            /** Функция начальной точки профилировщика */
            #define LV_PROFILER_BEGIN    LV_PROFILER_BUILTIN_BEGIN

            /** Функция конечной точки профилировщика */
            #define LV_PROFILER_END      LV_PROFILER_BUILTIN_END

            /** Функция начальной точки профилировщика с пользовательским тегом */
            #define LV_PROFILER_BEGIN_TAG LV_PROFILER_BUILTIN_BEGIN_TAG

            /** Функция конечной точки профилировщика с пользовательским тегом */
            #define LV_PROFILER_END_TAG   LV_PROFILER_BUILTIN_END_TAG

            /*Включить профилировщик макета*/
            #define LV_PROFILER_LAYOUT 1

            /*Включить профилировщик disp Refr*/
            #define LV_PROFILER_REFR 1

            /*Включить профилировщик отрисовки*/
            #define LV_PROFILER_DRAW 1

            /*Включить профилировщик разработки*/
            #define LV_PROFILER_INDEV 1

            /*Включить профилировщик декодера*/
            #define LV_PROFILER_DECODER 1

            /*Включить профилировщик шрифтов*/
            #define LV_PROFILER_FONT 1

            /*Включить профилировщик ФС*/
            #define LV_PROFILER_FS 1

            /*Включить профилировщик стилей*/
            #define LV_PROFILER_STYLE 0

            /*Включить профилировщик таймера*/
            #define LV_PROFILER_TIMER 1

            /*Включить профилировщик кэша*/
            #define LV_PROFILER_CACHE 1

            /*Включить профилировщик событий*/
            #define LV_PROFILER_EVENT 1
        #endif

        /** 1: Включить тест обезьяны */
        #define LV_USE_MONKEY 0

        /** 1: Включить навигацию по сетке */
        #define LV_USE_GRIDNAV 0

        /** 1: Включить логику фрагмента `lv_obj`. */
        #define LV_USE_FRAGMENT 0

        /** 1: Поддержка использования изображений в качестве шрифта в виджетах меток или интервалов. */
        #define LV_USE_IMGFONT 0

        /** 1. Включить реализацию шаблона наблюдателя. */
        #define LV_USE_OBSERVER 1

        /** 1: Включить метод ввода пиньинь
        *  - Требуется: lv_keyboard */
        #define LV_USE_IME_PINYIN 0
        #if LV_USE_IME_PINYIN
            /** 1: Используйте тезаурус по умолчанию.
            *  @note  Если вы не используете тезаурус по умолчанию, обязательно используйте `lv_ime_pinyin` после установки тезауруса. */
            #define LV_IME_PINYIN_USE_DEFAULT_DICT 1
            /** Установите максимальное количество панелей-кандидатов, которые можно отобразить.
            *  @note  Это необходимо настроить в соответствии с размером экрана. */
            #define LV_IME_PINYIN_CAND_TEXT_NUM 6

            /** Используйте 9-клавишный ввод (k9). */
            #define LV_IME_PINYIN_USE_K9_MODE      1
            #if LV_IME_PINYIN_USE_K9_MODE == 1
                #define LV_IME_PINYIN_K9_CAND_TEXT_NUM 3
            #endif /*LV_IME_PINYIN_USE_K9_MODE*/
        #endif

        /** 1: Включите файловый менеджер.
        *  - Требуется: lv_table */
        #define LV_USE_FILE_EXPLORER                     0
        #if LV_USE_FILE_EXPLORER
            /** Максимальная длина пути */
            #define LV_FILE_EXPLORER_PATH_MAX_LEN        (128)
            /** Панель быстрого доступа, 1: использовать, 0: не использовать.
            *  - Требуется: lv_list */
            #define LV_FILE_EXPLORER_QUICK_ACCESS        1
        #endif

        /** 1: Включить менеджер шрифтов произвольного типа
        *  - Требуется: LV_USE_FREETYPE */
        #define LV_USE_FONT_MANAGER                     0
        #if LV_USE_FONT_MANAGER

            /**Font manager name max length*/
            #define LV_FONT_MANAGER_NAME_MAX_LEN            32

        #endif

        /** Включить загрузку среды выполнения пользовательского интерфейса XML. */
        #define LV_USE_XML    0

        /*==================
        * DEVICES
        *==================*/

        /** Используйте SDL, чтобы открыть окно на PC и управлять мышью и клавиатурой. */
        #define LV_USE_SDL              0
        #if LV_USE_SDL
            #define LV_SDL_INCLUDE_PATH     <SDL2/SDL.h>
            #define LV_SDL_RENDER_MODE      LV_DISPLAY_RENDER_MODE_DIRECT   /**< LV_DISPLAY_RENDER_MODE_DIRECT is recommended for best performance */
            #define LV_SDL_BUF_COUNT        1    /**< 1 or 2 */
            #define LV_SDL_ACCELERATED      1    /**< 1: Use hardware acceleration*/
            #define LV_SDL_FULLSCREEN       0    /**< 1: Make the window full screen by default */
            #define LV_SDL_DIRECT_EXIT      1    /**< 1: Exit the application when all SDL windows are closed */
            #define LV_SDL_MOUSEWHEEL_MODE  LV_SDL_MOUSEWHEEL_MODE_ENCODER  /*LV_SDL_MOUSEWHEEL_MODE_ENCODER/CROWN*/
        #endif

        /** Используйте X11, чтобы открыть окно на рабочем столе Linux и управлять мышью и клавиатурой. */
        #define LV_USE_X11              0
        #if LV_USE_X11
            #define LV_X11_DIRECT_EXIT         1  /**< Exit application when all X11 windows have been closed */
            #define LV_X11_DOUBLE_BUFFER       1  /**< Use double buffers for rendering */
            /* Выберите только 1 из следующих режимов рендеринга (предпочтительно LV_X11_RENDER_MODE_PARTIAL!). */
            #define LV_X11_RENDER_MODE_PARTIAL 1  /**< Partial render mode (preferred) */
            #define LV_X11_RENDER_MODE_DIRECT  0  /**< Direct render mode */
            #define LV_X11_RENDER_MODE_FULL    0  /**< Full render mode */
        #endif

        /** Используйте Wayland, чтобы открыть и закрыть окно на рабочих столах Linux или BSD. */
        #define LV_USE_WAYLAND          0

        /** Драйвер для /dev/fb */
        #define LV_USE_LINUX_FBDEV      0
        #if LV_USE_LINUX_FBDEV
            #define LV_LINUX_FBDEV_BSD           0
            #define LV_LINUX_FBDEV_RENDER_MODE   LV_DISPLAY_RENDER_MODE_PARTIAL
            #define LV_LINUX_FBDEV_BUFFER_COUNT  0
            #define LV_LINUX_FBDEV_BUFFER_SIZE   60
        #endif

        /** Используйте Nuttx, чтобы открыть окно и управлять сенсорным дисплеем. */
        #define LV_USE_NUTTX    0

        #if LV_USE_NUTTX
            #define LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP 0

            #define LV_USE_NUTTX_LIBUV    0

            /** Используйте идею реализации Nuttx API, чтобы открыть окно и обработать сенсорный экран. */
            #define LV_USE_NUTTX_CUSTOM_INIT    0

            /** Драйвер для /dev/lcd */
            #define LV_USE_NUTTX_LCD      0
            #if LV_USE_NUTTX_LCD
                #define LV_NUTTX_LCD_BUFFER_COUNT    0
                #define LV_NUTTX_LCD_BUFFER_SIZE     60
            #endif

            /** Драйвер для /dev/input */
            #define LV_USE_NUTTX_TOUCHSCREEN    0

            /*Размер курсора сенсорного экрана в пикселях (<=0: отключить курсор)*/
            #define LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE    0
        #endif

        /** Драйвер для /dev/dri/card */
        #define LV_USE_LINUX_DRM        0

        #if LV_USE_LINUX_DRM

            /* Используйте библиотеку MESA GBM для выделения буферов DMA, которые можно
            * Совместно используется подсистемами и библиотеками с использованием LinuxDMA—BUFAPI.
            * Библиотека GBM призвана предоставить независимую от платформы систему управления памятью.
            * он поддерживает основных поставщиков GPU. Этот параметр требует связи с libgbm. */
            #define LV_USE_LINUX_DRM_GBM_BUFFERS 0
        #endif

        /** Интерфейс для TFT_eSPI */
        #define LV_USE_TFT_ESPI         0

        /** Драйвер для устройств ввода evdev */
        #define LV_USE_EVDEV    0

        /** Драйвер для устройств ввода libinput */
        #define LV_USE_LIBINPUT    0

        #if LV_USE_LIBINPUT
            #define LV_LIBINPUT_BSD    0

            /** Полная поддержка клавиатуры */
            #define LV_LIBINPUT_XKB             0
            #if LV_LIBINPUT_XKB
                /** «setxkbmap -query» может помочь найти важные значения для вашей клавиатуры. */
                #define LV_LIBINPUT_XKB_KEY_MAP { .rules = NULL, .model = "pc101", .layout = "us", .variant = NULL, .options = NULL }
            #endif
        #endif

        /* Драйверы для устройств LCD, подключаемых через SPI/параллельный порт */
        #define LV_USE_ST7735        0
        #define LV_USE_ST7789        0
        #define LV_USE_ST7796        0
        #define LV_USE_ILI9341       0
        #define LV_USE_GENERIC_MIPI  0

        /** Драйвер для Renesas GLCD */
        #define LV_USE_RENESAS_GLCDC    0

        /** Драйвер для ST LTDC */
        #define LV_USE_ST_LTDC    0
        #if LV_USE_ST_LTDC
            /* Используется только частично. */
            #define LV_ST_LTDC_USE_DMA2D_FLUSH 0
        #endif

        /** LVGL Серверная часть Windows */
        #define LV_USE_WINDOWS    0

        /** LVGL UEFI серверная часть */
        #define LV_USE_UEFI 0
        #if LV_USE_UEFI
            #define LV_USE_UEFI_INCLUDE "myefi.h"   /**< Header that hides the actual framework (EDK2, gnu-efi, ...) */
            #define LV_UEFI_USE_MEMORY_SERVICES 0   /**< Use the memory functions from the boot services table */
        #endif

        /** Используйте OpenGL, чтобы открыть окноPCи управлять мышью и клавиатурой. */
        #define LV_USE_OPENGLES   0
        #if LV_USE_OPENGLES
            #define LV_USE_OPENGLES_DEBUG        1    /**< Enable or disable debug for opengles */
        #endif

        /** QNX Экран дисплея и драйверы ввода */
        #define LV_USE_QNX              0
        #if LV_USE_QNX
            #define LV_QNX_BUF_COUNT        1    /**< 1 or 2 */
        #endif

        /*==================
        * EXAMPLES
        *==================*/

        /** Включите создание примеров с помощью библиотеки. */
        #define LV_BUILD_EXAMPLES 0

        /*===================
        * DEMO USAGE
        ====================*/

        /** Покажите несколько виджетов. Это может потребоваться для увеличения `LV_MEM_SIZE`. */
        #define LV_USE_DEMO_WIDGETS 0

        /** Продемонстрируйте использование кодировщика и клавиатуры. */
        #define LV_USE_DEMO_KEYPAD_AND_ENCODER 0

        /** Проверьте свою систему */
        #define LV_USE_DEMO_BENCHMARK 0

        /** Тест рендеринга для каждого примитива.
        *  - Требуется дисплей не менее 480x272. */
        #define LV_USE_DEMO_RENDER 0

        /** Стресс-тест для LVGL */
        #define LV_USE_DEMO_STRESS 0

        /** Демо-версия музыкального плеера */
        #define LV_USE_DEMO_MUSIC 0
        #if LV_USE_DEMO_MUSIC
            #define LV_DEMO_MUSIC_SQUARE    0
            #define LV_DEMO_MUSIC_LANDSCAPE 0
            #define LV_DEMO_MUSIC_ROUND     0
            #define LV_DEMO_MUSIC_LARGE     0
            #define LV_DEMO_MUSIC_AUTO_PLAY 0
        #endif

        /** Демонстрация гибкого макета */
        #define LV_USE_DEMO_FLEX_LAYOUT     0

        /** Многоязычная демо-версия в стиле смартфона */
        #define LV_USE_DEMO_MULTILANG       0

        /** Векторная графическая демонстрация */
        #define LV_USE_DEMO_VECTOR_GRAPHIC  0

        /*Демонстрация возможности велосипеда с анимацией Лотти (еслиLV_USE_LOTTIEвключен)*/
        #define LV_USE_DEMO_EBIKE           0
        #if LV_USE_DEMO_EBIKE
            #define LV_DEMO_EBIKE_PORTRAIT  0    /*0: для 480x270..480x320, 1: для 480x800..720x1280*/
        #endif

        /** Демо в высоком разрешении */
        #define LV_USE_DEMO_HIGH_RES        0

        /* Демонстрация умных часов */
        #define LV_USE_DEMO_SMARTWATCH      0

        /*--END OF LV_CONF_H--*/

    #endif /*LV_CONF_H*/

#endif /*Конец «Включения контента»*/
