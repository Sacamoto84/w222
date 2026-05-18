/**
 * @file lv_uefi_context.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"

#if LV_USE_UEFI

#include "lv_uefi_context.h"
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
 * @brief Инициализируйте переменные кэша UEFI.
 * @param image_handle Дескриптор текущего изображения
 * @param system_table Указатель на системную таблицу
 * @remark Это должно быть вызвано передlv_init().
*/
void lv_uefi_init(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE * system_table)
{
    LV_ASSERT_NULL(image_handle);
    LV_ASSERT_NULL(system_table);

    gLvEfiImageHandle = image_handle;
    gLvEfiST = system_table;
    gLvEfiBS = gLvEfiST->BootServices;
    gLvEfiRT = gLvEfiST->RuntimeServices;
}

/**
 * @brief Инициализируйте серверную частьLVGLUEFI.
 * @remark Это частный API, который используется для бэкэндаLVGLUEFI.
 *         реализация.  Пользователям LVGL не следует использовать это, потому что
 *         LVGL уже использовал его в lv_init.
 */
void lv_uefi_platform_init(void)
{
    LV_ASSERT_NULL(gLvEfiImageHandle);
    LV_ASSERT_NULL(gLvEfiST);
    LV_ASSERT_NULL(gLvEfiBS);
    LV_ASSERT_NULL(gLvEfiRT);
}

/**
 * @brief Очистите серверную частьLVGLUEFI.
 * @remark Это частный API, который используется для бэкэндаLVGLUEFI.
 *         реализация.  Пользователям LVGL не следует использовать это, потому что
 *         LVGL уже использовал его в lv_deinit.
*/
void lv_uefi_platform_deinit(void)
{
    ;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
