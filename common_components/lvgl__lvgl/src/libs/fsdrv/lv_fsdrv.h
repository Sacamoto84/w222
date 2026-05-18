/**
 * @file lv_fsdrv.h
 *
 */

#ifndef LV_FSDRV_H
#define LV_FSDRV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

/*********************
 *      DEFINES
 *********************/

#define LV_FS_MAX_PATH_LEN 256

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if LV_USE_FS_FATFS
void lv_fs_fatfs_init(void);
#endif

#if LV_USE_FS_STDIO
void lv_fs_stdio_init(void);
#endif

#if LV_USE_FS_POSIX
void lv_fs_posix_init(void);
#endif

#if LV_USE_FS_WIN32
void lv_fs_win32_init(void);
#endif

#if LV_USE_FS_MEMFS
void lv_fs_memfs_init(void);
#endif

#if LV_USE_FS_LITTLEFS
#include "lfs.h"
struct lfs;
/**
 * Установите обработчик LittleFS по умолчанию, который будет использоватьсяLVGL.
 * @param  lfs     указатель на инициализированную структуру файловой системы LittleFS
 */
void lv_littlefs_set_handler(struct lfs * lfs);

/**
 * Инициализация драйвера файловой системы LittleFS
 */
void lv_fs_littlefs_init(void);

/**
 * Зарегистрируйте диск LittleFS с помощью LVGL
 * @param  lfs     указатель на инициализированную структуру файловой системы LittleFS
 * @param  letter  буква водителя для регистрации (например, «А»)
 * @return         LV_FS_RES_OK : успех,LV_FS_RES_INV_PARAM: lfs — этоNULLили буква не в диапазоне от A до Z,
 *                 LV_FS_RES_DRIVE_LETTER_ALREADY_USED: Диск с этой буквой уже зарегистрирован
 */
lv_fs_res_t lv_fs_littlefs_register_drive(lfs_t * lfs, char letter);
#endif

#if LV_USE_FS_ARDUINO_ESP_LITTLEFS
void lv_fs_arduino_esp_littlefs_init(void);
#endif

#if LV_USE_FS_ARDUINO_SD
void lv_fs_arduino_sd_init(void);
#endif

#if LV_USE_FS_UEFI
void lv_fs_uefi_init(void);
#endif

#if LV_USE_FS_FROGFS
void lv_fs_frogfs_init(void);
void lv_fs_frogfs_deinit(void);

/**
 * Подключите объект frogfs по префиксу пути. Если есть файл "foo.txt"
 * в большом двойном объекте, и этот большой двойной объект зарегистрирован в`path_prefix`как "my_blob",
 * его можно открыть позже по пути «my_blob/foo.txt».
 * @param blob         blob/изображение frogfs из mkfrogfs.py
 * @param path_prefix  префикс, который будет использоваться для ссылки на этот большой двоичный объект при доступе к нему.
 * @return             LV_RESULT_OK или LV_RESULT_INVALID, если возникла проблема с большим двоичным объектом.
 */
lv_result_t lv_fs_frogfs_register_blob(const void * blob, const char * path_prefix);

/**
 * Отключите объект frogfs, который ранее был смонтирован`lv_fs_frogfs_register_blob`.
 * Перед вызовом этого метода все файлы и каталоги должны быть закрыты.
 * @param path_prefix  префикс пути, под которым был зарегистрирован большой двоичный объект
 */
void lv_fs_frogfs_unregister_blob(const char * path_prefix);

#endif /*LV_USE_FS_FROGFS*/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /*LV_FSDRV_H*/
