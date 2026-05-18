/**
 * @file lv_fs_posix.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"

#if LV_USE_FS_POSIX

#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include "../../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/

#if !LV_FS_IS_VALID_LETTER(LV_FS_POSIX_LETTER)
    #error "Invalid drive letter"
#endif

/** Причина «fd+1» заключается в том, чтоopen()может возвращать допустимый fd со значением 0,
  * предотвращая его оценку как NULL при преобразовании в тип указателя.
  */
#define FILEP2FD(file_p) ((lv_uintptr_t)file_p - 1)
#define FD2FILEP(fd) ((void *)(lv_uintptr_t)(fd + 1))

/**********************
 *      TYPEDEFS
 **********************/

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
static lv_fs_res_t fs_errno_to_res(int errno_val);

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
void lv_fs_posix_init(void)
{
    /*---------------------------------------------------
     * Зарегистрируйте интерфейс файловой системы в LVGL.
     *--------------------------------------------------*/

    lv_fs_drv_t * fs_drv_p = &(LV_GLOBAL_DEFAULT()->posix_fs_drv);
    lv_fs_drv_init(fs_drv_p);

    /*Настроить поля...*/
    fs_drv_p->letter = LV_FS_POSIX_LETTER;
    fs_drv_p->cache_size = LV_FS_POSIX_CACHE_SIZE;

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
 * @return дескриптор файла или -1 в случае неудачи
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    LV_UNUSED(drv);

    int flags = 0;
    if(mode == LV_FS_MODE_WR) flags = O_WRONLY | O_CREAT;
    else if(mode == LV_FS_MODE_RD) flags = O_RDONLY;
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) flags = O_RDWR | O_CREAT;

    /*Укажите путь относительно текущего каталога (корневой папки проекта)*/
    char buf[LV_FS_MAX_PATH_LEN];
    lv_snprintf(buf, sizeof(buf), LV_FS_POSIX_PATH "%s", path);

    int fd = open(buf, flags, 0666);
    if(fd < 0) {
        LV_LOG_WARN("Could not open file: %s, flags: 0x%x, errno: %d", buf, flags, errno);
        return NULL;
    }

    return FD2FILEP(fd);
}

/**
 * Закрыть открытый файл
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param file_p    дескриптор файла. (открывается с помощьюfs_open)
 * @return LV_FS_RES_OK: ошибок нет, файл прочитан
 *         любая ошибка из перечисления lv_fs_res_t
 */
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    LV_UNUSED(drv);

    int fd = FILEP2FD(file_p);
    int ret = close(fd);
    if(ret < 0) {
        LV_LOG_WARN("Could not close file: %d, errno: %d", fd, errno);
        return fs_errno_to_res(errno);
    }

    return LV_FS_RES_OK;
}

/**
 * Чтение данных из открытого файла
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param file_p    переменная дескриптора файла.
 * @param buf       указатель на блок памяти, в котором хранятся считанные данные
 * @param btr       количество байтов для чтения
 * @param br        реальное количество прочитанных байт (Byte Read)
 * @return LV_FS_RES_OK: ошибок нет, файл прочитан
 *         любая ошибка из перечисления lv_fs_res_t
 */
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    LV_UNUSED(drv);

    int fd = FILEP2FD(file_p);
    ssize_t ret = read(fd, buf, btr);
    if(ret < 0) {
        LV_LOG_WARN("Could not read file: %d, errno: %d", fd, errno);
        return fs_errno_to_res(errno);
    }

    *br = (uint32_t)ret;
    return LV_FS_RES_OK;
}

/**
 * Записать в файл
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param file_p    переменная дескриптора файла
 * @param buf       указатель на буфер с байтами для записи
 * @param btw       Байты для записи
 * @param bw        количество реальных записанных байт (Bytes Written).  NULL, если не используется.
 * @return LV_FS_RES_OK или любая ошибка из перечисления lv_fs_res_t
 */
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    LV_UNUSED(drv);

    int fd = FILEP2FD(file_p);
    ssize_t ret = write(fd, buf, btw);
    if(ret < 0) {
        LV_LOG_WARN("Could not write file: %d, errno: %d", fd, errno);
        return fs_errno_to_res(errno);
    }

    *bw = (uint32_t)ret;
    return LV_FS_RES_OK;
}

/**
 * Установите указатель чтения и записи. Также увеличьте размер файла, если необходимо.
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param file_p    переменная дескриптора файла. (открывается с помощьюfs_open)
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

    int fd = FILEP2FD(file_p);
    off_t offset = lseek(fd, pos, w);
    if(offset < 0) {
        LV_LOG_WARN("Could not seek file: %d, errno: %d", fd, errno);
        return fs_errno_to_res(errno);
    }

    return LV_FS_RES_OK;
}

/**
 * Укажите положение указателя чтения и записи.
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param file_p    переменная дескриптора файла
 * @param pos_p     указатель для сохранения результата
 * @return LV_FS_RES_OK: ошибок нет, файл прочитан
 *         любая ошибка из перечисления lv_fs_res_t
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    LV_UNUSED(drv);

    int fd = FILEP2FD(file_p);
    off_t offset = lseek(fd, 0, SEEK_CUR);
    if(offset < 0) {
        LV_LOG_WARN("Could not get position of file: %d, errno: %d", fd, errno);
        return fs_errno_to_res(errno);
    }

    *pos_p = (uint32_t)offset;
    return LV_FS_RES_OK;
}

/**
 * Инициализируйте переменнуюfs_read_dir_tдля чтения каталога.
 * @param drv   указатель на драйвер, которому принадлежит эта функция
 * @param path  путь к каталогу
 * @return указатель на инициализированную переменнуюDIRили HANDLE
 */
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path)
{
    LV_UNUSED(drv);

    /*Укажите путь относительно текущего каталога (корневой папки проекта)*/
    char buf[256];
    lv_snprintf(buf, sizeof(buf), LV_FS_POSIX_PATH "%s", path);

    void * dir = opendir(buf);
    if(!dir) {
        LV_LOG_WARN("Could not open directory: %s, errno: %d", buf, errno);
        return NULL;
    }

    return dir;
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

    struct dirent * entry;
    do {
        entry = readdir(dir_p);
        if(entry) {
            if(entry->d_type == DT_DIR) lv_snprintf(fn, fn_len, "/%s", entry->d_name);
            else lv_strlcpy(fn, entry->d_name, fn_len);
        }
        else {
            lv_strlcpy(fn, "", fn_len);
        }
    } while(lv_strcmp(fn, "/.") == 0 || lv_strcmp(fn, "/..") == 0);

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

    int ret = closedir(dir_p);
    if(ret < 0) {
        LV_LOG_WARN("Could not close directory: errno: %d", errno);
        return fs_errno_to_res(errno);
    }

    return LV_FS_RES_OK;
}

/**
 * Преобразуйте значение ошибки в значение lv_fs_res_t.
 * @param errno_val ошибочное значение
 * @return соответствующее значение lv_fs_res_t
 */
static lv_fs_res_t fs_errno_to_res(int errno_val)
{
    switch(errno_val) {
        case 0:
            return LV_FS_RES_OK;

        case EIO: /* Ошибка ввода-вывода */
            return LV_FS_RES_HW_ERR;

        case EFAULT: /* Неверный адрес */
            return LV_FS_RES_FS_ERR;

        case ENOENT: /* Нет такого файла или каталога */
            return LV_FS_RES_NOT_EX;

        case ENOSPC: /* На устройстве не осталось места */
            return LV_FS_RES_FULL;

        case EALREADY: /* Операция уже идет */
            return LV_FS_RES_LOCKED;

        case EACCES: /* Разрешение отклонено */
            return LV_FS_RES_DENIED;

        case EBUSY: /* Устройство или ресурс заняты */
            return LV_FS_RES_BUSY;

        case ETIMEDOUT: /* Время подключения истекло */
            return LV_FS_RES_TOUT;

        case ENOSYS: /* Неверный номер системного вызова */
            return LV_FS_RES_NOT_IMP;

        case ENOMEM: /* Недостаточно памяти */
            return LV_FS_RES_OUT_OF_MEM;

        case EINVAL: /* «Неверный аргумент» */
            return LV_FS_RES_INV_PARAM;

        default:
            break;
    }

    return LV_FS_RES_UNKNOWN;
}

#else /*LV_USE_FS_POSIX == 0*/

#if defined(LV_FS_POSIX_LETTER) && LV_FS_POSIX_LETTER != '\0'
    #warning "LV_USE_FS_POSIX is not enabled but LV_FS_POSIX_LETTER is set"
#endif

#endif /*LV_USE_FS_POSIX*/
