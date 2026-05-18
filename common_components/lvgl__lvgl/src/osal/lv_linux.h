/**
 * @file lv_linux.h
 *
 */

#ifndef LV_LINUX_H
#define LV_LINUX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_os.h"
#ifdef __linux__
/*********************
 *      DEFINES
 *********************/

#define LV_PROC_STAT_PARAMS_LEN 7

/**********************
 *      TYPEDEFS
 **********************/

typedef union {
    struct {
        /*
         *  Мы игнорируем столбец iowait, потому что он ненадежен.
         *  Мы игнорируем столбцы Guest и guest_nice, поскольку они наблюдаются.
         *   для пользователя и приятно соответственно
         */
        uint32_t user, nice, system, idle, /*ай подожди,*/ irq, softirq,
                 steal /*, гость, guest_nice*/;
    } fields;
    uint32_t buffer[LV_PROC_STAT_PARAMS_LEN];
} lv_linux_proc_stat_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*__linux__*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LINUX_H*/
