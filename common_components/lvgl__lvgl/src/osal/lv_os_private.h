/**
 * @file lv_os_private.h
 *
 */

#ifndef LV_OS_PRIVATE_H
#define LV_OS_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *    OS OPTIONS
 *********************/

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include "lv_os.h"
#include "../misc/lv_types.h"

#ifdef __linux__
#include "lv_linux.h"
#endif

#if LV_USE_OS == LV_OS_NONE
#include "lv_os_none.h"
#elif LV_USE_OS == LV_OS_PTHREAD
#include "lv_pthread.h"
#elif LV_USE_OS == LV_OS_FREERTOS
#include "lv_freertos.h"
#elif LV_USE_OS == LV_OS_CMSIS_RTOS2
#include "lv_cmsis_rtos2.h"
#elif LV_USE_OS == LV_OS_RTTHREAD
#include "lv_rtthread.h"
#elif LV_USE_OS == LV_OS_WINDOWS
#include "lv_windows.h"
#elif LV_USE_OS == LV_OS_MQX
#include "lv_mqx.h"
#elif LV_USE_OS == LV_OS_SDL2
#include "lv_sdl2.h"
#elif LV_USE_OS == LV_OS_CUSTOM
#include LV_OS_CUSTOM_INCLUDE
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_THREAD_PRIO_LOWEST,
    LV_THREAD_PRIO_LOW,
    LV_THREAD_PRIO_MID,
    LV_THREAD_PRIO_HIGH,
    LV_THREAD_PRIO_HIGHEST,
} lv_thread_prio_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте слой OS
 */
void lv_os_init(void);

/**
 * Установите его для `LV_SYSMON_GET_IDLE`, чтобы показать использование CPU.
 * @return процент простоя с момента последнего звонка
 */
uint32_t lv_os_get_idle_percent(void);

#if LV_SYSMON_PROC_IDLE_AVAILABLE

uint32_t lv_os_get_proc_idle_percent(void);

#endif

#if LV_USE_OS != LV_OS_NONE

/*----------------------------------------
 * Эти функции необходимо реализовать.
 * для конкретных операционных систем
 *---------------------------------------*/

/**
 * Создать новую тему
 * @param thread        переменная, в которой будет храниться поток
 * @param name          название темы
 * @param prio          приоритет потока
 * @param stack_size      функция нити
 * @param stack_size    размер стека в байтах
 * @param user_data     произвольные данные, будут доступны в обратном вызове
 * @return              LV_RESULT_OK: успех; LV_RESULT_INVALID: сбой
 */
lv_result_t lv_thread_init(lv_thread_t * thread, const char * const name,
                           lv_thread_prio_t prio, void (*callback)(void *), size_t stack_size,
                           void * user_data);

/**
 * Удалить тему
 * @param thread        ветка, которую нужно удалить
 * @return              LV_RESULT_OK: успех; LV_RESULT_INVALID: сбой
 */
lv_result_t lv_thread_delete(lv_thread_t * thread);

/**
 * Создать мьютекс
 * @param mutex         переменная, в которой будет храниться поток
 * @return              LV_RESULT_OK: успех; LV_RESULT_INVALID: сбой
 */
lv_result_t lv_mutex_init(lv_mutex_t * mutex);

/**
 * Блокировка мьютекса
 * @param mutex         мьютекс для блокировки
 * @return              LV_RESULT_OK: успех; LV_RESULT_INVALID: сбой
 */
lv_result_t lv_mutex_lock(lv_mutex_t * mutex);

/**
 * Блокировка мьютекса от прерывания
 * @param mutex         мьютекс для блокировки
 * @return              LV_RESULT_OK: успех; LV_RESULT_INVALID: сбой
 */
lv_result_t lv_mutex_lock_isr(lv_mutex_t * mutex);

/**
 * Разблокировать мьютекс
 * @param mutex         мьютекс для разблокировки
 * @return              LV_RESULT_OK: успех; LV_RESULT_INVALID: сбой
 */
lv_result_t lv_mutex_unlock(lv_mutex_t * mutex);

/**
 * Удаление мьютекса
 * @param mutex         мьютекс, который нужно удалить
 * @return              LV_RESULT_OK: успех; LV_RESULT_INVALID: сбой
 */
lv_result_t lv_mutex_delete(lv_mutex_t * mutex);

/**
 * Создайте объект синхронизации потоков
 * @param sync          переменная, в которой будет храниться синхронизация
 * @return              LV_RESULT_OK: успех; LV_RESULT_INVALID: сбой
 */
lv_result_t lv_thread_sync_init(lv_thread_sync_t * sync);

/**
 * Дождитесь «сигнала» на объекте синхронизации.
 * @param sync      объект синхронизации
 * @return          LV_RESULT_OK: успех; LV_RESULT_INVALID: сбой
 */
lv_result_t lv_thread_sync_wait(lv_thread_sync_t * sync);

/**
 * Отправить сигнал пробуждения объекту синхронизации
 * @param sync      объект синхронизации
 * @return          LV_RESULT_OK: успех; LV_RESULT_INVALID: сбой
 */
lv_result_t lv_thread_sync_signal(lv_thread_sync_t * sync);

/**
 * Отправьте сигнал пробуждения объекту синхронизации из прерывания
 * @param sync      объект синхронизации
 * @return          LV_RESULT_OK: успех; LV_RESULT_INVALID: сбой
 */
lv_result_t lv_thread_sync_signal_isr(lv_thread_sync_t * sync);

/**
 * Удаление объекта синхронизации
 * @param sync      объект синхронизации для удаления
 * @return          LV_RESULT_OK: успех; LV_RESULT_INVALID: сбой
 */
lv_result_t lv_thread_sync_delete(lv_thread_sync_t * sync);

#else

/* Поскольку компиляция не обязательно хорошо оптимизирует пустые межфайловые функции.
 * (- Одной оптимизации O3 недостаточно, если не включена оптимизация LTO),
 * При отсутствии операционной системы используйте встроенные функции для облегчения компиляции.
 * оптимизации и избежать накладных расходов на вызовы OS API, чтобы избежать снижения производительности.
 */

static inline lv_result_t lv_thread_init(lv_thread_t * thread, const char * const name, lv_thread_prio_t prio,
                                         void (*callback)(void *), size_t stack_size, void * user_data)
{
    LV_UNUSED(thread);
    LV_UNUSED(name);
    LV_UNUSED(callback);
    LV_UNUSED(prio);
    LV_UNUSED(stack_size);
    LV_UNUSED(user_data);
    return LV_RESULT_INVALID;
}

static inline lv_result_t lv_thread_delete(lv_thread_t * thread)
{
    LV_UNUSED(thread);
    return LV_RESULT_INVALID;
}

static inline lv_result_t lv_mutex_init(lv_mutex_t * mutex)
{
    LV_UNUSED(mutex);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_mutex_lock(lv_mutex_t * mutex)
{
    LV_UNUSED(mutex);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_mutex_lock_isr(lv_mutex_t * mutex)
{
    LV_UNUSED(mutex);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_mutex_unlock(lv_mutex_t * mutex)
{
    LV_UNUSED(mutex);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_mutex_delete(lv_mutex_t * mutex)
{
    LV_UNUSED(mutex);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_thread_sync_init(lv_thread_sync_t * sync)
{
    LV_UNUSED(sync);
    return LV_RESULT_INVALID;
}

static inline lv_result_t lv_thread_sync_wait(lv_thread_sync_t * sync)
{
    LV_UNUSED(sync);
    return LV_RESULT_INVALID;
}

static inline lv_result_t lv_thread_sync_signal(lv_thread_sync_t * sync)
{
    LV_UNUSED(sync);
    return LV_RESULT_INVALID;
}

static inline lv_result_t lv_thread_sync_signal_isr(lv_thread_sync_t * sync)
{
    LV_UNUSED(sync);
    return LV_RESULT_INVALID;
}

static inline lv_result_t lv_thread_sync_delete(lv_thread_sync_t * sync)
{
    LV_UNUSED(sync);
    return LV_RESULT_INVALID;
}

#endif /*LV_USE_OS != LV_OS_NONE*/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_OS_PRIVATE_H*/
