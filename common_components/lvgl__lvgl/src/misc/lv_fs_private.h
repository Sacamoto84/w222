/**
 * @file lv_fs_private.h
 *
 */

#ifndef LV_FS_PRIVATE_H
#define LV_FS_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_fs.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_fs_file_cache_t {
    uint32_t start;
    uint32_t end;
    uint32_t file_position;
    void * buffer;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте интерфейс файловой системы
 */
void lv_fs_init(void);

/**
 * Деинициализировать интерфейс файловой системы
 */
void lv_fs_deinit(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_FS_PRIVATE_H*/
