/**
 * @file lv_uefi_private.h
 *
 */

#ifndef __LV_UEFI_PRIVATE_H__
#define __LV_UEFI_PRIVATE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"

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
 * Внутренний кеш для дескриптора изображения (источник: точка входа приложения)
 */
extern EFI_HANDLE gLvEfiImageHandle;
/**
 * Внутренний кеш для системной таблицы (источник: точка входа приложения)
 */
extern EFI_SYSTEM_TABLE * gLvEfiST;
/**
 * Внутренний кеш для таблицы загрузки грузов (источник: gLvEfiST)
 */
extern EFI_BOOT_SERVICES * gLvEfiBS;
/**
 * Внутренний кеш для таблиц служб времени выполнения загрузки (источник: gLvEfiST)
 */
extern EFI_RUNTIME_SERVICES * gLvEfiRT;

/**
 * @brief Проверьте, установлен ли протокол в дескрипторе.
 * @param handle Дескриптор, на котором может быть установлен протокол.
 * @param protocol Руководство протокола.
 * @return TRUE, если протокол установлен, FALSE, если нет.
*/
bool lv_uefi_protocol_test(EFI_HANDLE handle, EFI_GUID * protocol);

/**
 * @brief Откройте протокол.
 * @param handle Дескриптор, на котором установлен протокол.
 * @param protocol Руководство протокола.
 * @return Указатель на интерфейс NULL, если протокол не удалось открыть.
*/
void * lv_uefi_protocol_open(EFI_HANDLE handle, EFI_GUID * protocol);

/**
 * @brief Закройте протокол.
 * @param handle Дескриптор, на котором установлен протокол.
 * @param protocol Руководство протокола.
*/
void lv_uefi_protocol_close(EFI_HANDLE handle, EFI_GUID * protocol);

/**
 * @brief Преобразуйте строкуUCS-2 в строку ASCII.
 * Строка должна содержать только символы >= 0x20 и <= 0X7E.
 * @param ucs2 СтрокаUCS-2.
 * @param ascii Буфер для хранения строки ASCII.
 * @param ascii_len Размер буфера в символах ASCII.
 * @return Количество символов, записанных в буфер, или 0, если
 * произошла ошибка.
*/
size_t lv_uefi_ucs2_to_ascii(const CHAR16 * ucs2, char * ascii, size_t ascii_len);

/**
 * @brief Преобразуйте строкуASCIIв строкуUCS-2.
 * Строка должна содержать только символы >= 0x20 и <= 0X7E.
 * @param ascii Строка ASCII.
 * @param ucs2 Буфер для хранения строкиUCS-2.
 * @param ucs2_len Размер буфера вUCS-2 символа.
 * @return Количество байт, записанных в буфер, или 0, если
 * произошла ошибка.
*/
size_t lv_uefi_ascii_to_ucs2(const char * ascii, CHAR16 * ucs2, size_t ucs2_len);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
}
#endif

#endif //__LV_UEFI_PRIVATE_H__