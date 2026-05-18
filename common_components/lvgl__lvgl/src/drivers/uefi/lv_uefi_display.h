/**
 * @file lv_uefi_display.h
 *
 */

#ifndef __LV_UEFI_DISPLAY_H__
#define __LV_UEFI_DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"

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
 * @brief Создайте экранный объект LVGL.
 * @param handle Дескриптор, на котором установлен экземпляр протокола EFI_GRAPHICS_OUTPUT_PROTOCOL.
 * @return Созданный экранный объект LVGL.
*/
lv_display_t * lv_uefi_display_create(void * handle);

/**
 * @brief Попробуйте найти активный дескриптор дисплея.
 * @return Дескриптор или NULL, если не найден.
 * @remark Активному дисплею необходимы интерфейсы дляEFI_GRAPHICS_OUTPUT_PROTOCOLи EFI_EDID_ACTIVE_PROTOCOL.
*/
void * lv_uefi_display_get_active(void);

/**
 * @brief Попробуйте найти любой дескриптор дисплея.
 * @return Дескриптор или NULL, если не найден.
*/
void * lv_uefi_display_get_any(void);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
}
#endif

#endif //__LV_UEFI_DISPLAY_H__

