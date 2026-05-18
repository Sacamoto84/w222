/**
 * @file lv_fs_win32.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"
#if LV_USE_FS_WIN32

#include <windows.h>
#include <stdio.h>
#include <limits.h>

#include "../../core/lv_global.h"
/*********************
 *      DEFINES
 *********************/

#if !LV_FS_IS_VALID_LETTER(LV_FS_WIN32_LETTER)
    #error "Invalid drive letter"
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    HANDLE dir_p;
    char next_fn[LV_FS_MAX_PATH_LEN];
    lv_fs_res_t next_error;
} dir_handle_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool is_dots_name(const char * name);
static lv_fs_res_t fs_error_from_win32(DWORD error);
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path);
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * dir_p, char * fn, uint32_t fn_len);
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * dir_p);

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
 * Зарегистрируйте драйвер для интерфейса файловой системы.
 */
void lv_fs_win32_init(void)
{
    /*---------------------------------------------------
     * Зарегистрируйте интерфейс файловой системы в LVGL.
     *--------------------------------------------------*/

    /*Добавьте простой драйвер для открытия изображений*/
    lv_fs_drv_t * fs_drv_p = &(LV_GLOBAL_DEFAULT()->win32_fs_drv);
    lv_fs_drv_init(fs_drv_p);

    /*Настроить поля...*/
    fs_drv_p->letter = LV_FS_WIN32_LETTER;
    fs_drv_p->cache_size = LV_FS_WIN32_CACHE_SIZE;

    fs_drv_p->open_cb = fs_open;
    fs_drv_p->close_cb = fs_close;
    fs_drv_p->read_cb = fs_read;
    fs_drv_p->write_cb = fs_write;
    fs_drv_p->seek_cb = fs_seek;
    fs_drv_p->tell_cb = fs_tell;

    fs_drv_p->dir_close_cb = fs_dir_close;
    fs_drv_p->dir_open_cb = fs_dir_open;
    fs_drv_p->dir_read_cb = fs_dir_read;

    lv_fs_drv_register(fs_drv_p);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Проверьте название точек
 * @param name имя файла или каталога
 * @return правда, если имя состоит из точек
 */
static bool is_dots_name(const char * name)
{
    return name[0] == '.' && (!name[1] || (name[1] == '.' && !name[2]));
}

/**
 * Преобразование ошибки кода Win32 в ошибке из перечисления lv_fs_res_t
 * @param error Код ошибки Win32
 * @return LV_FS_RES_OK: ошибок нет, файл прочитан
 *         любая ошибка из перечисления lv_fs_res_t
 */
static lv_fs_res_t fs_error_from_win32(DWORD error)
{
    lv_fs_res_t res;

    switch(error) {
        case ERROR_SUCCESS:
            res = LV_FS_RES_OK;
            break;
        case ERROR_BAD_UNIT:
        case ERROR_NOT_READY:
        case ERROR_CRC:
        case ERROR_SEEK:
        case ERROR_NOT_DOS_DISK:
        case ERROR_WRITE_FAULT:
        case ERROR_READ_FAULT:
        case ERROR_GEN_FAILURE:
        case ERROR_WRONG_DISK:
            res = LV_FS_RES_HW_ERR;
            break;
        case ERROR_INVALID_HANDLE:
        case ERROR_INVALID_TARGET_HANDLE:
            res = LV_FS_RES_FS_ERR;
            break;
        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
        case ERROR_INVALID_DRIVE:
        case ERROR_NO_MORE_FILES:
        case ERROR_SECTOR_NOT_FOUND:
        case ERROR_BAD_NETPATH:
        case ERROR_BAD_NET_NAME:
        case ERROR_BAD_PATHNAME:
        case ERROR_FILENAME_EXCED_RANGE:
            res = LV_FS_RES_NOT_EX;
            break;
        case ERROR_DISK_FULL:
            res = LV_FS_RES_FULL;
            break;
        case ERROR_SHARING_VIOLATION:
        case ERROR_LOCK_VIOLATION:
        case ERROR_DRIVE_LOCKED:
            res = LV_FS_RES_LOCKED;
            break;
        case ERROR_ACCESS_DENIED:
        case ERROR_CURRENT_DIRECTORY:
        case ERROR_WRITE_PROTECT:
        case ERROR_NETWORK_ACCESS_DENIED:
        case ERROR_CANNOT_MAKE:
        case ERROR_FAIL_I24:
        case ERROR_SEEK_ON_DEVICE:
        case ERROR_NOT_LOCKED:
        case ERROR_LOCK_FAILED:
            res = LV_FS_RES_DENIED;
            break;
        case ERROR_BUSY:
            res = LV_FS_RES_BUSY;
            break;
        case ERROR_TIMEOUT:
            res = LV_FS_RES_TOUT;
            break;
        case ERROR_NOT_SAME_DEVICE:
        case ERROR_DIRECT_ACCESS_HANDLE:
            res = LV_FS_RES_NOT_IMP;
            break;
        case ERROR_TOO_MANY_OPEN_FILES:
        case ERROR_ARENA_TRASHED:
        case ERROR_NOT_ENOUGH_MEMORY:
        case ERROR_INVALID_BLOCK:
        case ERROR_OUT_OF_PAPER:
        case ERROR_SHARING_BUFFER_EXCEEDED:
        case ERROR_NOT_ENOUGH_QUOTA:
            res = LV_FS_RES_OUT_OF_MEM;
            break;
        case ERROR_INVALID_FUNCTION:
        case ERROR_INVALID_ACCESS:
        case ERROR_INVALID_DATA:
        case ERROR_BAD_COMMAND:
        case ERROR_BAD_LENGTH:
        case ERROR_INVALID_PARAMETER:
        case ERROR_NEGATIVE_SEEK:
            res = LV_FS_RES_INV_PARAM;
            break;
        default:
            res = LV_FS_RES_UNKNOWN;
            break;
    }

    return res;
}

/**
 * Открыть файл
 * @param drv   указатель на драйвер, которому принадлежит эта функция
 * @param path  путь к файлу, начинающийся с буквы драйвера (например, S:/folder/file.txt)
 * @param mode  читать: FS_MODE_RD, писать: FS_MODE_WR, оба:FS_MODE_RD|  FS_MODE_WR
 * @return указатель на структуруFILилиNULLв случае неудачи
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    LV_UNUSED(drv);

    DWORD desired_access = 0;

    if(mode & LV_FS_MODE_RD) {
        desired_access |= GENERIC_READ;
    }

    if(mode & LV_FS_MODE_WR) {
        desired_access |= GENERIC_WRITE;
    }

    /*Укажите путь относительно текущего каталога (корневой папки проекта)*/

    char buf[MAX_PATH];
    lv_snprintf(buf, sizeof(buf), LV_FS_WIN32_PATH "%s", path);

    return (void *)CreateFileA(
               buf,
               desired_access,
               FILE_SHARE_READ,
               NULL,
               OPEN_EXISTING,
               FILE_ATTRIBUTE_NORMAL,
               NULL);
}

/**
 * Закрыть открытый файл
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param file_p    указатель на переменную FILE. (открывается с помощьюfs_open)
 * @return LV_FS_RES_OK: ошибок нет, файл прочитан
 *         любая ошибка из перечисления lv_fs_res_t
 */
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    LV_UNUSED(drv);
    return CloseHandle((HANDLE)file_p)
           ? LV_FS_RES_OK
           : fs_error_from_win32(GetLastError());
}

/**
 * Чтение данных из открытого файла
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param file_p    указатель на переменную FILE.
 * @param buf       указатель на блок памяти, в котором хранятся считанные данные
 * @param btr       количество байтов для чтения
 * @param br        реальное количество прочитанных байт (Byte Read)
 * @return LV_FS_RES_OK: ошибок нет, файл прочитан
 *         любая ошибка из перечисления lv_fs_res_t
 */
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    LV_UNUSED(drv);
    return ReadFile((HANDLE)file_p, buf, btr, (LPDWORD)br, NULL)
           ? LV_FS_RES_OK
           : fs_error_from_win32(GetLastError());
}

/**
 * Записать в файл
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param file_p    указатель на переменную FILE
 * @param buf       указатель на буфер с байтами для записи
 * @param btw       Байты для записи
 * @param bw        количество реальных записанных байт (Bytes Written).  NULL, если не используется.
 * @return LV_FS_RES_OK или любая ошибка из перечисления lv_fs_res_t
 */
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    LV_UNUSED(drv);
    return WriteFile((HANDLE)file_p, buf, btw, (LPDWORD)bw, NULL)
           ? LV_FS_RES_OK
           : fs_error_from_win32(GetLastError());
}

/**
 * Установите указатель чтения и записи. Также увеличьте размер файла, если необходимо.
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param file_p    указатель на переменную FILE. (открывается с помощьюfs_open)
 * @param pos       новая позиция указателя чтения и записи
 * @return LV_FS_RES_OK: ошибок нет, файл прочитан
 *         любая ошибка из перечисления lv_fs_res_t
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    LV_UNUSED(drv);

    DWORD move_method = (DWORD) -1;
    if(whence == LV_FS_SEEK_SET) {
        move_method = FILE_BEGIN;
    }
    else if(whence == LV_FS_SEEK_CUR) {
        move_method = FILE_CURRENT;
    }
    else if(whence == LV_FS_SEEK_END) {
        move_method = FILE_END;
    }

    LARGE_INTEGER distance_to_move;
    distance_to_move.QuadPart = pos;
    return SetFilePointerEx((HANDLE)file_p, distance_to_move, NULL, move_method)
           ? LV_FS_RES_OK
           : fs_error_from_win32(GetLastError());
}

/**
 * Укажите положение указателя чтения и записи.
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param file_p    указатель на переменную FILE
 * @param pos_p     указатель для сохранения результата
 * @return LV_FS_RES_OK: ошибок нет, файл прочитан
 *         любая ошибка из перечисления lv_fs_res_t
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    LV_UNUSED(drv);

    if(!pos_p) {
        return LV_FS_RES_INV_PARAM;
    }

    LARGE_INTEGER file_pointer;
    file_pointer.QuadPart = 0;

    LARGE_INTEGER distance_to_move;
    distance_to_move.QuadPart = 0;
    if(SetFilePointerEx(
           (HANDLE)file_p,
           distance_to_move,
           &file_pointer,
           FILE_CURRENT)) {
        if(file_pointer.QuadPart > LONG_MAX) {
            return LV_FS_RES_INV_PARAM;
        }
        else {
            *pos_p = file_pointer.LowPart;
            return LV_FS_RES_OK;
        }
    }
    else {
        return fs_error_from_win32(GetLastError());
    }
}

/**
 * Инициализируйте переменную «DIR» или «HANDLE» для чтения каталога.
 * @param drv   указатель на драйвер, которому принадлежит эта функция
 * @param path  путь к каталогу
 * @return указатель на инициализированную переменнуюDIRили HANDLE
 */
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path)
{
    LV_UNUSED(drv);
    dir_handle_t * handle = (dir_handle_t *)lv_malloc(sizeof(dir_handle_t));
    handle->dir_p = INVALID_HANDLE_VALUE;
    handle->next_error = LV_FS_RES_OK;
    WIN32_FIND_DATAA fdata;

    /*Укажите путь относительно текущего каталога (корневой папки проекта)*/
    char buf[LV_FS_MAX_PATH_LEN];
    lv_snprintf(buf, sizeof(buf), LV_FS_WIN32_PATH "%s\\*", path);

    lv_strcpy(handle->next_fn, "");
    handle->dir_p = FindFirstFileA(buf, &fdata);

    if(handle->dir_p != INVALID_HANDLE_VALUE) {
        do {
            if(is_dots_name(fdata.cFileName)) {
                continue;
            }
            else {
                if(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    lv_snprintf(handle->next_fn, sizeof(handle->next_fn), "/%s", fdata.cFileName);
                }
                else {
                    lv_snprintf(handle->next_fn, sizeof(handle->next_fn), "%s", fdata.cFileName);
                }
                break;
            }
        } while(FindNextFileA(handle->dir_p, &fdata));
    }

    if(handle->dir_p == INVALID_HANDLE_VALUE) {
        lv_free(handle);
        handle->next_error = fs_error_from_win32(GetLastError());
        return INVALID_HANDLE_VALUE;
    }
    else {
        handle->next_error = LV_FS_RES_OK;
        return handle;
    }
}

/**
 * Прочитать следующее имя файла из каталога.
 * Название каталогов начинается с '/'
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param dir_p     указатель на инициализированную переменнуюDIRили HANDLE
 * @param fn        указатель на буфер для хранения имени файла
 * @param fn_len    длина буфера для хранения имени файла
 * @return LV_FS_RES_OK или любая ошибка из перечисления lv_fs_res_t
 */
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * dir_p, char * fn, uint32_t fn_len)
{
    LV_UNUSED(drv);
    if(fn_len == 0) return LV_FS_RES_INV_PARAM;

    dir_handle_t * handle = (dir_handle_t *)dir_p;
    lv_strlcpy(fn, handle->next_fn, fn_len);
    lv_fs_res_t current_error = handle->next_error;
    lv_strcpy(handle->next_fn, "");

    WIN32_FIND_DATAA fdata;

    while(FindNextFileA(handle->dir_p, &fdata)) {
        if(is_dots_name(fdata.cFileName)) {
            continue;
        }
        else {
            if(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                lv_snprintf(handle->next_fn, sizeof(handle->next_fn), "/%s", fdata.cFileName);
            }
            else {
                lv_snprintf(handle->next_fn, sizeof(handle->next_fn), "%s", fdata.cFileName);
            }
            break;
        }
    }

    if(handle->next_fn[0] == '\0') {
        handle->next_error = fs_error_from_win32(GetLastError());
    }

    return current_error;
}

/**
 * Закройте чтение каталога
 * @param drv   указатель на драйвер, которому принадлежит эта функция
 * @param dir_p указатель на инициализированную переменнуюDIRили HANDLE
 * @return LV_FS_RES_OK или любая ошибка из перечисления lv_fs_res_t
 */
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * dir_p)
{
    LV_UNUSED(drv);
    dir_handle_t * handle = (dir_handle_t *)dir_p;
    lv_fs_res_t res = FindClose(handle->dir_p)
                      ? LV_FS_RES_OK
                      : fs_error_from_win32(GetLastError());
    lv_free(handle);
    return res;
}

#else /*LV_USE_FS_WIN32 == 0*/

#if defined(LV_FS_WIN32_LETTER) && LV_FS_WIN32_LETTER != '\0'
    #warning "LV_USE_FS_WIN32 is not enabled but LV_FS_WIN32_LETTER is set"
#endif

#endif
