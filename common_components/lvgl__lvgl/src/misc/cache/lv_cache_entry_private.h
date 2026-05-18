/**
 * @file lv_cache_entry_private.h
 *
 */

#ifndef LV_CACHE_ENTRY_PRIVATE_H
#define LV_CACHE_ENTRY_PRIVATE_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_cache_entry_t {
    const lv_cache_t * cache;
    int32_t ref_cnt;
    uint32_t node_size;
#define LV_CACHE_ENTRY_FLAG_INVALID (1 << 0) /** Флаг, указывающий, является ли запись недействительной и ее можно удалить. */
#define LV_CACHE_ENTRY_FLAG_DISABLE_DELETE (1 << 1) /** Этот флаг должен быть установлен, если класс кэша управляет памятью самой записи.*/
#define LV_CACHE_ENTRY_FLAG_CLASS_CUSTOM (1 << 7) /**A custom flag that can be used by the different cache classes*/
    uint8_t flags;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void   lv_cache_entry_reset_ref(lv_cache_entry_t * entry);
void   lv_cache_entry_inc_ref(lv_cache_entry_t * entry);
void   lv_cache_entry_dec_ref(lv_cache_entry_t * entry);
void   lv_cache_entry_set_node_size(lv_cache_entry_t * entry, uint32_t node_size);
void   lv_cache_entry_set_cache(lv_cache_entry_t * entry, const lv_cache_t * cache);
void * lv_cache_entry_acquire_data(lv_cache_entry_t * entry);
void   lv_cache_entry_release_data(lv_cache_entry_t * entry, void * user_data);
void   lv_cache_entry_set_flag(lv_cache_entry_t * entry, uint8_t flags);
void   lv_cache_entry_remove_flag(lv_cache_entry_t * entry, uint8_t flags);
bool   lv_cache_entry_has_flag(lv_cache_entry_t * entry, uint8_t flags);
/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_CACHE_ENTRY_PRIVATE_H */
