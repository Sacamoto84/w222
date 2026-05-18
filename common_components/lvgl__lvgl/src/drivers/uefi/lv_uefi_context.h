/**
 * @file lv_uefi_context.h
 *
 */

#ifndef __LV_UEFI_CONTEXT_H__
#define __LV_UEFI_CONTEXT_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_UEFI

#include "lv_uefi.h"

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
 * @brief Initialize the UEFI cache variables.
 * @param image_handle The handle of the current image
 * @param system_table Pointer to the system table
 * @remark Это должно быть вызвано перед lv_init ().
*/
void lv_uefi_init(
    EFI_HANDLE image_handle,
    EFI_SYSTEM_TABLE * system_table);

/**
 * @brief Initialize the LVGL UEFI backend.
 * @remark Это частный API, который используется для бэкэнда LVGL UEFI.
 *         реализация.  Пользователям LVGL не следует использовать это, потому что
 *         LVGL уже использовал его в lv_init.
*/
void lv_uefi_platform_init(void);

/**
 * @brief Cleanup the LVGL UEFI backend.
 * @remark Это частный API, который используется для бэкэнда LVGL UEFI.
 *         реализация.  Пользователям LVGL не следует использовать это, потому что
 *         LVGL уже использовал его в lv_deinit.
*/
void lv_uefi_platform_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
}
#endif

#endif //__LV_UEFI_CONTEXT_H__

