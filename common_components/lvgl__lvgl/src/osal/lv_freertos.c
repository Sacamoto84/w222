/**
 * @file lv_freertos.c
 *
 */

/**
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_os_private.h"
#if LV_USE_OS == LV_OS_FREERTOS

#include "atomic.h"

#include "../tick/lv_tick.h"
#include "../misc/lv_log.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/

#define ulMAX_COUNT 10U

#define globals LV_GLOBAL_DEFAULT()

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void prvRunThread(void * pxArg);

static void prvMutexInit(lv_mutex_t * pxMutex);

static void prvCheckMutexInit(lv_mutex_t * pxMutex);

static void prvCondInit(lv_thread_sync_t * pxCond);

static void prvCheckCondInit(lv_thread_sync_t * pxCond);

static void prvCheckCondInitIsr(lv_thread_sync_t * pxCond);

#if !LV_USE_FREERTOS_TASK_NOTIFY
static void prvTestAndDecrement(lv_thread_sync_t * pxCond,
                                uint32_t ulLocalWaitingThreads);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

#ifdef ESP_PLATFORM
    static portMUX_TYPE critSectionMux = portMUX_INITIALIZER_UNLOCKED;
#endif

/**********************
 *      MACROS
 **********************/

#ifdef ESP_PLATFORM
    #define _enter_critical()   taskENTER_CRITICAL(&critSectionMux);
    #define _exit_critical()    taskEXIT_CRITICAL(&critSectionMux);
    #define _enter_critical_isr() taskENTER_CRITICAL_FROM_ISR();
    #define _exit_critical_isr(x) taskEXIT_CRITICAL_FROM_ISR(x);
#else
    #define _enter_critical()   taskENTER_CRITICAL();
    #define _exit_critical()    taskEXIT_CRITICAL();
    #define _enter_critical_isr() taskENTER_CRITICAL_FROM_ISR();
    #define _exit_critical_isr(x) taskEXIT_CRITICAL_FROM_ISR(x);
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_thread_init(lv_thread_t * pxThread,  const char * const name,
                           lv_thread_prio_t xSchedPriority,
                           void (*pvStartRoutine)(void *), size_t usStackSize,
                           void * xAttr)
{
    pxThread->pTaskArg = xAttr;
    pxThread->pvStartRoutine = pvStartRoutine;

    BaseType_t xTaskCreateStatus = xTaskCreate(
                                       prvRunThread,
                                       name,
                                       (configSTACK_DEPTH_TYPE)(usStackSize / sizeof(StackType_t)),
                                       (void *)pxThread,
                                       tskIDLE_PRIORITY + xSchedPriority,
                                       &pxThread->xTaskHandle);

    /* Убедитесь, что задача FreeRTOS успешно установлена. */
    if(xTaskCreateStatus != pdPASS) {
        LV_LOG_ERROR("xTaskCreate failed!");
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;
}

lv_result_t lv_thread_delete(lv_thread_t * pxThread)
{
    vTaskDelete(pxThread->xTaskHandle);

    return LV_RESULT_OK;
}

lv_result_t lv_mutex_init(lv_mutex_t * pxMutex)
{
    /* Если мьютекс не инициализирован, выполните инициализацию. */
    prvCheckMutexInit(pxMutex);

    return LV_RESULT_OK;
}

lv_result_t lv_mutex_lock(lv_mutex_t * pxMutex)
{
    /* Если мьютекс не инициализирован, выполните инициализацию. */
    prvCheckMutexInit(pxMutex);

    BaseType_t xMutexTakeStatus = xSemaphoreTakeRecursive(pxMutex->xMutex, portMAX_DELAY);
    if(xMutexTakeStatus != pdTRUE) {
        LV_LOG_ERROR("xSemaphoreTake failed!");
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;
}

lv_result_t lv_mutex_lock_isr(lv_mutex_t * pxMutex)
{
    /* Если мьютекс не инициализирован, выполните инициализацию. */
    prvCheckMutexInit(pxMutex);

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    BaseType_t xMutexTakeStatus = xSemaphoreTakeFromISR(pxMutex->xMutex, &xHigherPriorityTaskWoken);
    if(xMutexTakeStatus != pdTRUE) {
        LV_LOG_ERROR("xSemaphoreTake failed!");
        return LV_RESULT_INVALID;
    }

    /* Если для xHigherPriorityTaskWoken теперь установлено значение pdTRUE, произойдет переключение контекста.
    должно быть выполнено, чтобы гарантировать возврат прерывания непосредственно к самому высокому значению.
    приоритетная задача.  Макрос, используемый для этой цели, зависит от порта в
    использовать и может называться portEND_SWITCHING_ISR(). */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    return LV_RESULT_OK;
}

lv_result_t lv_mutex_unlock(lv_mutex_t * pxMutex)
{
    /* Если мьютекс не инициализирован, выполните инициализацию. */
    prvCheckMutexInit(pxMutex);

    BaseType_t xMutexGiveStatus = xSemaphoreGiveRecursive(pxMutex->xMutex);
    if(xMutexGiveStatus != pdTRUE) {
        LV_LOG_ERROR("xSemaphoreGive failed!");
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;
}

lv_result_t lv_mutex_delete(lv_mutex_t * pxMutex)
{
    if(pxMutex->xIsInitialized == pdFALSE)
        return LV_RESULT_INVALID;
    vSemaphoreDelete(pxMutex->xMutex);
    pxMutex->xIsInitialized = pdFALSE;

    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_init(lv_thread_sync_t * pxCond)
{
    /* Если cond не инициализирован, осуществите принципизацию. */
    prvCheckCondInit(pxCond);

    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_wait(lv_thread_sync_t * pxCond)
{
    lv_result_t lvRes = LV_RESULT_OK;

    /* Если cond не инициализирован, осуществите принципизацию. */
    prvCheckCondInit(pxCond);

#if LV_USE_FREERTOS_TASK_NOTIFY
    TaskHandle_t xCurrentTaskHandle = xTaskGetCurrentTaskHandle();

    _enter_critical();
    BaseType_t xSyncSygnal = pxCond->xSyncSignal;
    pxCond->xSyncSignal = pdFALSE;
    if(xSyncSygnal == pdFALSE) {
        /* Сигнал еще не отправлен. Попросите отправителя уведомить об этой задаче */
        pxCond->xTaskToNotify = xCurrentTaskHandle;
    }
    /* Если у нас есть сигнал от другой задачи, нам не следует запрашивать уведомление. */
    _exit_critical();

    if(xSyncSygnal == pdFALSE) {
        /* Подождите, пока другая задача уведомит эту задачу. */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
    /* Если сигнал был получен, ждать не нужно. */
#else
    uint32_t ulLocalWaitingThreads;

    /* Получите мьютекс. */
    xSemaphoreTake(pxCond->xSyncMutex, portMAX_DELAY);

    while(!pxCond->xSyncSignal) {
        /* Увеличьте счетчик блокировки потоков по условной переменной, затем
         * освободить мьютекс. */

        /* Атомарно увеличивает ожидание потока на 1 и
         * хранит количество потоков, ожидающих перед приращением. */
        ulLocalWaitingThreads = Atomic_Increment_u32(&pxCond->ulWaitingThreads);

        BaseType_t xMutexStatus = xSemaphoreGive(pxCond->xSyncMutex);

        /* Дождитесь условной переменной. */
        if(xMutexStatus == pdTRUE) {
            BaseType_t xCondWaitStatus = xSemaphoreTake(
                                             pxCond->xCondWaitSemaphore,
                                             portMAX_DELAY);

            /* Повторно заблокируйте мьютекс. */
            xSemaphoreTake(pxCond->xSyncMutex, portMAX_DELAY);

            if(xCondWaitStatus != pdTRUE) {
                LV_LOG_ERROR("xSemaphoreTake(xCondWaitSemaphore) failed!");
                lvRes = LV_RESULT_INVALID;

                /* Атомарно уменьшает ожидание потока на 1.
                 * Если iLocalWaitingThreads обновляется другими потоками между ними,
                 * эта реализация гарантирует уменьшение на 1 на основе
                 * текущее значение в pxCond->ulWaitingThreads. */
                prvTestAndDecrement(pxCond, ulLocalWaitingThreads + 1);
            }
        }
        else {
            LV_LOG_ERROR("xSemaphoreGive(xSyncMutex) failed!");
            lvRes = LV_RESULT_INVALID;

            /* Атомарно уменьшает ожидание потока на 1.
             * Если iLocalWaitingThreads обновляется другими потоками между ними,
             * эта реализация гарантирует уменьшение на 1 на основе
             * текущее значение в pxCond->ulWaitingThreads. */
            prvTestAndDecrement(pxCond, ulLocalWaitingThreads + 1);
        }
    }

    pxCond->xSyncSignal = pdFALSE;

    /* Освободите мьютекс. */
    xSemaphoreGive(pxCond->xSyncMutex);
#endif

    return lvRes;
}

lv_result_t lv_thread_sync_signal(lv_thread_sync_t * pxCond)
{
    /* Если cond не инициализирован, осуществите принципизацию. */
    prvCheckCondInit(pxCond);

#if LV_USE_FREERTOS_TASK_NOTIFY
    _enter_critical();
    TaskHandle_t xTaskToNotify = pxCond->xTaskToNotify;
    pxCond->xTaskToNotify = NULL;
    if(xTaskToNotify == NULL) {
        /* Нет задач, ожидающих уведомления. Отправьте этот сигнал позже */
        pxCond->xSyncSignal = pdTRUE;
    }
    /* Если задача уже ожидает, нет необходимости устанавливать сигнал синхронизации. */
    _exit_critical();

    if(xTaskToNotify != NULL) {
        /* Есть задача, ожидающая. Отправьте ему уведомление */
        xTaskNotifyGive(xTaskToNotify);
    }
    /* Если не было задачи, ожидающей уведомления, мы отправляли сигнал, чтобы ее можно было увидеть позже. */
#else
    /* Получите мьютекс. */
    xSemaphoreTake(pxCond->xSyncMutex, portMAX_DELAY);

    pxCond->xSyncSignal = pdTRUE;

    /* Локальная копия количества ожидающих потоков. */
    uint32_t ulLocalWaitingThreads = pxCond->ulWaitingThreads;

    /* Тестовая локальная копия ожидающих потоков больше нуля. */
    while(ulLocalWaitingThreads > 0) {
        /* Атомарно проверьте, изменилась ли копия в памяти.
         * Если нет, установите копию потоков, ожидающих в памяти, на ноль. */
        if(ATOMIC_COMPARE_AND_SWAP_SUCCESS == Atomic_CompareAndSwap_u32(
               &pxCond->ulWaitingThreads,
               0,
               ulLocalWaitingThreads)) {
            /* Разблокировать все. */
            for(uint32_t i = 0; i < ulLocalWaitingThreads; i++) {
                xSemaphoreGive(pxCond->xCondWaitSemaphore);
            }

            break;
        }

        /* Локальная копия устарела. Перезагрузите из памяти и повторите попытку. */
        ulLocalWaitingThreads = pxCond->ulWaitingThreads;
    }

    /* Освободите мьютекс. */
    xSemaphoreGive(pxCond->xSyncMutex);
#endif

    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_delete(lv_thread_sync_t * pxCond)
{
#if !LV_USE_FREERTOS_TASK_NOTIFY
    /* Очистите все ресурсы, используйте их в условиях. */
    vSemaphoreDelete(pxCond->xCondWaitSemaphore);
    vSemaphoreDelete(pxCond->xSyncMutex);
    pxCond->ulWaitingThreads = 0;
#endif
    pxCond->xSyncSignal = pdFALSE;
    pxCond->xIsInitialized = pdFALSE;

    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_signal_isr(lv_thread_sync_t * pxCond)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Если cond не инициализирован, осуществите принципизацию. */
    prvCheckCondInitIsr(pxCond);

#if LV_USE_FREERTOS_TASK_NOTIFY
    uint32_t mask = _enter_critical_isr();
    TaskHandle_t xTaskToNotify = pxCond->xTaskToNotify;
    pxCond->xTaskToNotify = NULL;
    if(xTaskToNotify == NULL) {
        /* Нет задач, ожидающих уведомления. Отправьте этот сигнал позже */
        pxCond->xSyncSignal = pdTRUE;
    }
    /* Если задача уже ожидает, нет необходимости устанавливать сигнал синхронизации. */
    _exit_critical_isr(mask);

    if(xTaskToNotify != NULL) {
        /* Есть задача, ожидающая. Отправьте ему уведомление */
        vTaskNotifyGiveFromISR(xTaskToNotify, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    /* Если не было задачи, ожидающей уведомления, мы отправляли сигнал, чтобы ее можно было увидеть позже. */
#else
    /* Войдите в критический раздел, чтобы предотвратить вытеснение. */
    uint32_t mask = _enter_critical_isr();

    pxCond->xSyncSignal = pdTRUE;
    BaseType_t xAnyHigherPriorityTaskWoken = pdFALSE;

    /* Разблокировать все. */
    for(uint32_t i = 0; i < pxCond->ulWaitingThreads; i++) {
        xSemaphoreGiveFromISR(pxCond->xCondWaitSemaphore, &xAnyHigherPriorityTaskWoken);
        xHigherPriorityTaskWoken |= xAnyHigherPriorityTaskWoken;
    }

    _exit_critical_isr(mask);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif

    return LV_RESULT_OK;
}


void lv_freertos_task_switch_in(const char * name)
{
    if(lv_strcmp(name, "IDLE")) globals->freertos_idle_task_running = false;
    else globals->freertos_idle_task_running = true;

    globals->freertos_task_switch_timestamp = lv_tick_get();
}

void lv_freertos_task_switch_out(void)
{
    uint32_t elaps = lv_tick_elaps(globals->freertos_task_switch_timestamp);
    if(globals->freertos_idle_task_running) globals->freertos_idle_time_sum += elaps;
    else globals->freertos_non_idle_time_sum += elaps;
}

uint32_t lv_os_get_idle_percent(void)
{
    if(globals->freertos_non_idle_time_sum + globals->freertos_idle_time_sum == 0) {
        LV_LOG_WARN("Not enough time elapsed to provide idle percentage");
        return 0;
    }

    uint32_t pct = (globals->freertos_idle_time_sum * 100) / (globals->freertos_idle_time_sum +
                                                              globals->freertos_non_idle_time_sum);

    globals->freertos_non_idle_time_sum = 0;
    globals->freertos_idle_time_sum = 0;

    return pct;
}

void lv_sleep_ms(uint32_t ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void prvRunThread(void * pxArg)
{
    lv_thread_t * pxThread = (lv_thread_t *)pxArg;

    /* Запустите процедуру потока. */
    pxThread->pvStartRoutine((void *)pxThread->pTaskArg);

    vTaskDelete(NULL);
}

static void prvMutexInit(lv_mutex_t * pxMutex)
{
    pxMutex->xMutex = xSemaphoreCreateRecursiveMutex();

    /* Убедитесь, что мьютекс FreeRTOS успешно создан. */
    if(pxMutex->xMutex == NULL) {
        LV_LOG_ERROR("xSemaphoreCreateMutex failed!");
        return;
    }

    /* Мьютекс успешно создан. */
    pxMutex->xIsInitialized = pdTRUE;
}

static void prvCheckMutexInit(lv_mutex_t * pxMutex)
{
    /* Проверьте, нужно ли инициализировать мьютекс. */
    if(pxMutex->xIsInitialized == pdFALSE) {
        /* Инициализация мьютекса должна находиться в критическом разделе, чтобы предотвратить два потока.
         * от его одновременной инициализации. */
        _enter_critical();

        /* Еще раз проверьте, что мьютекс все еще не инициализирован, т. е. он не был
         * инициализируется, пока эта функция ожидает входа в критический режим
         * раздел. */
        if(pxMutex->xIsInitialized == pdFALSE) {
            prvMutexInit(pxMutex);
        }

        /* Выход из критического раздела. */
        _exit_critical();
    }
}

static void prvCondInit(lv_thread_sync_t * pxCond)
{
    pxCond->xIsInitialized = pdTRUE;
    pxCond->xSyncSignal = pdFALSE;

#if LV_USE_FREERTOS_TASK_NOTIFY
    pxCond->xTaskToNotify = NULL;
#else
    pxCond->xCondWaitSemaphore = xSemaphoreCreateCounting(ulMAX_COUNT, 0U);

    /* Убедитесь, что семафор FreeRTOS был успешно создан. */
    if(pxCond->xCondWaitSemaphore == NULL) {
        LV_LOG_ERROR("xSemaphoreCreateCounting failed!");
        return;
    }

    pxCond->xSyncMutex = xSemaphoreCreateMutex();

    /* Убедитесь, что мьютекс FreeRTOS успешно создан. */
    if(pxCond->xSyncMutex == NULL) {
        LV_LOG_ERROR("xSemaphoreCreateMutex failed!");
        /* Уборка. */
        vSemaphoreDelete(pxCond->xCondWaitSemaphore);
        return;
    }

    /* Условная переменная успешно создана. */
    pxCond->ulWaitingThreads = 0;
#endif
}

static void prvCheckCondInit(lv_thread_sync_t * pxCond)
{
    /* Проверьте, нужно ли инициализировать переменную условия. */
    if(pxCond->xIsInitialized == pdFALSE) {
        /* Инициализация Cond должна находиться в критической секции, чтобы предотвратить два
         * потоки от его одновременной инициализации. */
        _enter_critical();

        /* Еще раз проверьте, что условие все еще не инициализировано, т. е. оно не было
         * инициализируется, пока эта функция ожидает входа в критический режим
         * раздел. */
        if(pxCond->xIsInitialized == pdFALSE) {
            prvCondInit(pxCond);
        }

        /* Выход из критического раздела. */
        _exit_critical();
    }
}

static void prvCheckCondInitIsr(lv_thread_sync_t * pxCond)
{
    /* Проверьте, нужно ли инициализировать переменную условия. */
    if(pxCond->xIsInitialized == pdFALSE) {
        /* Инициализация Cond должна находиться в критической секции, чтобы предотвратить два
         * потоки от его одновременной инициализации. */
        uint32_t mask = _enter_critical_isr();

        /* Еще раз проверьте, что условие все еще не инициализировано, т. е. оно не было
         * инициализируется, пока эта функция ожидает входа в критический режим
         * раздел. */
        if(pxCond->xIsInitialized == pdFALSE) {
            prvCondInit(pxCond);
        }

        /* Выход из критического раздела. */
        _exit_critical_isr(mask);
    }
}

#if !LV_USE_FREERTOS_TASK_NOTIFY
static void prvTestAndDecrement(lv_thread_sync_t * pxCond,
                                uint32_t ulLocalWaitingThreads)
{
    /* Тестовая локальная копия ожидающих потоков больше нуля. */
    while(ulLocalWaitingThreads > 0) {
        /* Атомарно проверьте, изменилась ли копия в памяти.
         * Если нет, уменьшите количество копий потоков, ожидающих в памяти. */
        if(ATOMIC_COMPARE_AND_SWAP_SUCCESS == Atomic_CompareAndSwap_u32(
               &pxCond->ulWaitingThreads,
               ulLocalWaitingThreads - 1,
               ulLocalWaitingThreads)) {
            /* Сигнал один удался. Перерыв. */
            break;
        }

        /* Локальная копия может быть устаревшей. Перезагрузите из памяти и повторите попытку. */
        ulLocalWaitingThreads = pxCond->ulWaitingThreads;
    }
}
#endif

#endif /*LV_USE_OS == LV_OS_FREERTOS*/
