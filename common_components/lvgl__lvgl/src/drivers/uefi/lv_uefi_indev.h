/**
 * @file lv_uefi_indev.h
 *
 */

#ifndef __LV_UEFI_INDEV_H__
#define __LV_UEFI_INDEV_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../indev/lv_indev.h"

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
 * @brief Создайте объект indev.
 * @param display_res Разрешение дисплея в пикселях, необходимое для масштабирования ввода.
 * Если NULL, будет использоваться разрешение текущего дисплея по умолчанию.
 * @return Созданный объект разработки LVGL.
*/
lv_indev_t * lv_uefi_simple_pointer_indev_create(lv_point_t * display_res);

/**
 * @brief Добавьте интерфейсEFI_SIMPLE_POINTER_PROTOCOLв файл indev.
 * @param indev Indev, созданный с помощью lv_uefi_simple_pointer_indev_create.
 * @param handle Дескриптор, на котором установлен экземпляр протокола EFI_SIMPLE_POINTER_PROTOCOL.
 * @return True, если интерфейс был добавлен.
*/
bool lv_uefi_simple_pointer_indev_add_handle(lv_indev_t * indev, EFI_HANDLE handle);

/**
 * @brief Добавьте все доступные интерфейсыEFI_SIMPLE_POINTER_PROTOCOLв файл indev.
 * @param indev Indev, созданный с помощью lv_uefi_simple_pointer_indev_create.
*/
void lv_uefi_simple_pointer_indev_add_all(lv_indev_t * indev);

/**
 * @brief Создайте объект indev LVGL.
 * @param display_res Разрешение дисплея в пикселях, необходимое для масштабирования ввода.
 * @return Созданный объект разработки LVGL.
*/
lv_indev_t * lv_uefi_absolute_pointer_indev_create(lv_point_t * display_res);

/**
 * @brief Добавьте интерфейсEFI_ABSOLUTE_POINTER_PROTOCOLв файл indev.
 * @param indev Indev, созданный с помощью lv_uefi_absolute_pointer_indev_create.
 * @param handle Дескриптор, на котором установлен экземпляр протокола EFI_ABSOLUTE_POINTER_PROTOCOL.
 * @return True, если интерфейс был добавлен.
*/
bool lv_uefi_absolute_pointer_indev_add_handle(lv_indev_t * indev, EFI_HANDLE handle);

/**
 * @brief Добавьте все доступные интерфейсыEFI_ABSOLUTE_POINTER_PROTOCOLв файл indev.
 * @param indev Indev, созданный с помощью lv_uefi_absolute_pointer_indev_create.
*/
void lv_uefi_absolute_pointer_indev_add_all(lv_indev_t * indev);

/**
 * @brief Создайте объект indev.
 * @return Созданный объект разработки LVGL.
*/
lv_indev_t * lv_uefi_simple_text_input_indev_create(void);

/**
 * @brief Добавьте интерфейсEFI_SIMPLE_TEXT_INPUT_EX_PROTOCOLв файл indev.
 * @param indev Indev, созданный с помощью lv_uefi_simple_text_input_indev_create.
 * @param handle Дескриптор, на котором установлен экземпляр протокола EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL.
 * @return True, если интерфейс был добавлен.
*/
bool lv_uefi_simple_text_input_indev_add_handle(lv_indev_t * indev, EFI_HANDLE handle);

/**
 * @brief Добавьте все доступные интерфейсыEFI_SIMPLE_TEXT_INPUT_EX_PROTOCOLв файл indev.
 * @param indev Indev, созданный с помощью lv_uefi_simple_text_input_indev_create.
*/
void lv_uefi_simple_text_input_indev_add_all(lv_indev_t * indev);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
}
#endif

#endif //__LV_UEFI_INDEV_H__

