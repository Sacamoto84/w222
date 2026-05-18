/**
 * @file lv_fs_stdio.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"
#if LV_USE_FS_STDIO

#include <stdio.h>
#ifndef WIN32
    #include <dirent.h>
    #include <unistd.h>
#else
    #include <windows.h>
#endif

#include "../../core/lv_global.h"
/*********************
 *      DEFINES
 *********************/

#if !LV_FS_IS_VALID_LETTER(LV_FS_STDIO_LETTER)
    #error "Invalid drive letter"
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
#ifdef _WIN32
    HANDLE dir_p;
    char next_fn[LV_FS_MAX_PATH_LEN];
#else
    DIR * dir_p;
#endif
} dir_handle_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
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
void lv_fs_stdio_init(void)
{
    /*---------------------------------------------------
     * Зарегистрируйте интерфейс файловой системы в LVGL.
     *--------------------------------------------------*/

    lv_fs_drv_t * fs_drv_p = &(LV_GLOBAL_DEFAULT()->stdio_fs_drv);
    lv_fs_drv_init(fs_drv_p);

    /*Настроить поля...*/
    fs_drv_p->letter = LV_FS_STDIO_LETTER;
    fs_drv_p->cache_size = LV_FS_STDIO_CACHE_SIZE;

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
 * Открыть файл
 * @param drv   указатель на драйвер, которому принадлежит эта функция
 * @param path  путь к файлу, начинающийся с буквы драйвера (например, S:/folder/file.txt)
 * @param mode  читать: FS_MODE_RD, писать: FS_MODE_WR, оба:FS_MODE_RD|  FS_MODE_WR
 * @return указатель на структуруFILилиNULLв случае неудачи
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    LV_UNUSED(drv);

    const char * flags = "";

    if(mode == LV_FS_MODE_WR) flags = "wb";
    else if(mode == LV_FS_MODE_RD) flags = "rb";
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) flags = "rb+";

    /*Укажите путь относительно текущего каталога (корневой папки проекта)*/

    char buf[LV_FS_MAX_PATH_LEN];
    lv_snprintf(buf, sizeof(buf), LV_FS_STDIO_PATH "%s", path);

    return fopen(buf, flags);
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
    fclose(file_p);
    return LV_FS_RES_OK;
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
    *br = fread(buf, 1, btr, file_p);
    return (int32_t)(*br) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
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
    *bw = fwrite(buf, 1, btw, file_p);
    return (int32_t)(*bw) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
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
    int w;
    switch(whence) {
        case LV_FS_SEEK_SET:
            w = SEEK_SET;
            break;
        case LV_FS_SEEK_CUR:
            w = SEEK_CUR;
            break;
        case LV_FS_SEEK_END:
            w = SEEK_END;
            break;
        default:
            return LV_FS_RES_INV_PARAM;
    }

    fseek(file_p, pos, w);
    return LV_FS_RES_OK;
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
    *pos_p = ftell(file_p);
    return LV_FS_RES_OK;
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
#ifndef WIN32
    /*Укажите путь относительно текущего каталога (корневой папки проекта)*/
    char buf[LV_FS_MAX_PATH_LEN];
    lv_snprintf(buf, sizeof(buf), LV_FS_STDIO_PATH "%s", path);
    handle->dir_p = opendir(buf);
    if(handle->dir_p == NULL) {
        lv_free(handle);
        return NULL;
    }
    return handle;
#else
    handle->dir_p = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAA fdata;

    /*Укажите путь относительно текущего каталога (корневой папки проекта)*/
    char buf[LV_FS_MAX_PATH_LEN];
    lv_snprintf(buf, sizeof(buf), LV_FS_STDIO_PATH "%s\\*", path);

    lv_strcpy(handle->next_fn, "");
    handle->dir_p = FindFirstFileA(buf, &fdata);
    do {
        if(lv_strcmp(fdata.cFileName, ".") == 0 || lv_strcmp(fdata.cFileName, "..") == 0) {
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

    if(handle->dir_p == INVALID_HANDLE_VALUE) {
        lv_free(handle);
        return INVALID_HANDLE_VALUE;
    }
    return handle;
#endif
}

/**
 * Прочитайте следующее имя файла из каталога.
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
#ifndef WIN32
    struct dirent * entry;
    do {
        entry = readdir(handle->dir_p);
        if(entry) {
            /*Обратите внимание: DT_DIR не определен в C99.*/
            if(entry->d_type == DT_DIR) lv_snprintf(fn, fn_len, "/%s", entry->d_name);
            else lv_strlcpy(fn, entry->d_name, fn_len);
        }
        else {
            lv_strlcpy(fn, "", fn_len);
        }
    } while(lv_strcmp(fn, "/.") == 0 || lv_strcmp(fn, "/..") == 0);
#else
    lv_strlcpy(fn, handle->next_fn, fn_len);

    lv_strcpy(handle->next_fn, "");
    WIN32_FIND_DATAA fdata;

    if(FindNextFileA(handle->dir_p, &fdata) == false) return LV_FS_RES_OK;
    do {
        if(lv_strcmp(fdata.cFileName, ".") == 0 || lv_strcmp(fdata.cFileName, "..") == 0) {
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

#endif
    return LV_FS_RES_OK;
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
#ifndef WIN32
    closedir(handle->dir_p);
#else
    FindClose(handle->dir_p);
#endif
    lv_free(handle);
    return LV_FS_RES_OK;
}

#else /*LV_USE_FS_STDIO == 0*/

#if defined(LV_FS_STDIO_LETTER) && LV_FS_STDIO_LETTER != '\0'
    #warning "LV_USE_FS_STDIO is not enabled but LV_FS_STDIO_LETTER is set"
#endif

#endif /*LV_USE_FS_POSIX*/
