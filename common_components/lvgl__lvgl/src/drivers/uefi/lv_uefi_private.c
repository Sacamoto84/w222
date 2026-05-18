/**
 * @file lv_uefi_private.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"

#if LV_USE_UEFI

#include "lv_uefi_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/
EFI_HANDLE gLvEfiImageHandle = NULL;
EFI_SYSTEM_TABLE * gLvEfiST = NULL;
EFI_BOOT_SERVICES * gLvEfiBS = NULL;
EFI_RUNTIME_SERVICES * gLvEfiRT = NULL;

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief Проверьте, установлен ли протокол в дескрипторе.
 * @param handle Дескриптор, на котором может быть установлен протокол.
 * @param protocol Руководство протокола.
 * @return TRUE, если протокол установлен, FALSE, если нет.
*/
bool lv_uefi_protocol_test(EFI_HANDLE handle, EFI_GUID * protocol)
{
    EFI_STATUS status;
    void * interface = NULL;

    if(handle == NULL) return false;
    if(protocol == NULL) return false;

    status = gLvEfiBS->OpenProtocol(
                 handle,
                 protocol,
                 &interface,
                 gLvEfiImageHandle,
                 NULL,
                 EFI_OPEN_PROTOCOL_TEST_PROTOCOL);
    if(status != EFI_SUCCESS && status != EFI_UNSUPPORTED) {
        LV_LOG_WARN("couldn't test protocol");
        return FALSE;
    }
    else if(status == EFI_UNSUPPORTED) {
        return FALSE;
    }

    return TRUE;
}

/**
 * @brief Откройте протокол.
 * @param handle Дескриптор, на котором установлен протокол.
 * @param protocol Руководство протокола.
 * @return Указатель на интерфейс NULL, если протокол не удалось открыть.
*/
void * lv_uefi_protocol_open(EFI_HANDLE handle, EFI_GUID * protocol)
{
    EFI_STATUS status;
    void * interface = NULL;

    if(handle == NULL) return NULL;
    if(protocol == NULL) return NULL;

    status = gLvEfiBS->OpenProtocol(
                 handle,
                 protocol,
                 &interface,
                 gLvEfiImageHandle,
                 NULL,
                 EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if(status != EFI_SUCCESS) {
        LV_LOG_ERROR("[lv_uefi] Couldn't open protocol %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X, error code: %llx.",
                     protocol->Data1,
                     protocol->Data2,
                     protocol->Data3,
                     protocol->Data4[0],
                     protocol->Data4[1],
                     protocol->Data4[2],
                     protocol->Data4[3],
                     protocol->Data4[4],
                     protocol->Data4[5],
                     protocol->Data4[6],
                     protocol->Data4[7],
                     status);
        interface = NULL;
    }

    return interface;
}

/**
 * @brief Закройте протокол.
 * @param handle Дескриптор, на котором установлен протокол.
 * @param protocol Руководство протокола.
*/
void lv_uefi_protocol_close(EFI_HANDLE handle, EFI_GUID * protocol)
{
    EFI_STATUS status;

    if(handle == NULL) return;
    if(protocol == NULL) return;

    status = gLvEfiBS->CloseProtocol(
                 handle,
                 protocol,
                 gLvEfiImageHandle,
                 NULL);
    if(status != EFI_SUCCESS) {
        LV_LOG_WARN("[lv_uefi] Couldn't close protocol %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X, error code: %llx.",
                    protocol->Data1,
                    protocol->Data2,
                    protocol->Data3,
                    protocol->Data4[0],
                    protocol->Data4[1],
                    protocol->Data4[2],
                    protocol->Data4[3],
                    protocol->Data4[4],
                    protocol->Data4[5],
                    protocol->Data4[6],
                    protocol->Data4[7],
                    status);
    }
}

/**
 * @brief Преобразуйте строкуUCS-2 в строку ASCII.
 * Строка должна содержать только символы >= 0x20 и <= 0X7E.
 * @param ucs2 СтрокаUCS-2.
 * @param ascii Буфер для хранения строки ASCII.
 * @param ascii_len Размер буфера в символах ASCII.
 * @return Количество символов, записанных в буфер, или 0, если
 * произошла ошибка.
*/
size_t lv_uefi_ucs2_to_ascii(const CHAR16 * ucs2, char * ascii, size_t ascii_len)
{
    size_t invalid_character_count;
    size_t string_index;

    if(ucs2 == NULL || ascii == NULL || ascii_len == 0) {
        return 0;
    }

    invalid_character_count = 0;

    for(string_index = 0; ucs2[string_index] != 0x0000 && string_index < ascii_len - 1; string_index++) {
        if(ucs2[string_index] < 0x20 || ucs2[string_index] > 0x7E) {
            invalid_character_count++;
        }
        ascii[string_index] = (char) ucs2[string_index];
    }

    /* завершить строку, даже если произошла ошибка */
    ascii[string_index] = 0x00;

    return invalid_character_count == 0 ? string_index : 0;
}

/**
 * @brief Преобразуйте строкуASCIIв строкуUCS-2.
 * Строка должна содержать только символы >= 0x20 и <= 0X7E.
 * @param ascii Строка ASCII.
 * @param ucs2 Буфер для хранения строкиUCS-2.
 * @param ucs2_len Размер буфера вUCS-2 символа.
 * @return Количество байт, записанных в буфер, или 0, если
 * произошла ошибка.
*/
size_t lv_uefi_ascii_to_ucs2(const char * ascii, CHAR16 * ucs2, size_t ucs2_len)
{
    size_t invalid_character_count;
    size_t string_index;

    if(ascii == NULL || ucs2 == NULL || ucs2_len == 0) {
        return 0;
    }

    invalid_character_count = 0;

    for(string_index = 0; ascii[string_index] != 0x0000 && string_index < ucs2_len - 1; string_index++) {
        if(ascii[string_index] < 0x20 || ascii[string_index] > 0x7E) {
            invalid_character_count++;
        }
        ucs2[string_index] = (CHAR16) ascii[string_index];
    }

    /* завершить строку, даже если произошла ошибка */
    ucs2[string_index] = 0x0000;

    return invalid_character_count == 0 ? string_index : 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
