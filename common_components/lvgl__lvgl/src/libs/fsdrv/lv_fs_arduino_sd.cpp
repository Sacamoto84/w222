#include "../../../lvgl.h"
#if LV_USE_FS_ARDUINO_SD

#include "../../core/lv_global.h"
#include <SPI.h>
#include "SD.h"

#if !LV_FS_IS_VALID_LETTER(LV_FS_ARDUINO_SD_LETTER)
    #error "Invalid drive letter"
#endif

typedef struct SdFile {
    File file;
} SdFile;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

/**
 * Зарегистрируйте драйвер для интерфейса файловой системы SD.
 */
extern "C" void lv_fs_arduino_sd_init(void)
{
    lv_fs_drv_t * fs_drv = &(LV_GLOBAL_DEFAULT()->arduino_sd_fs_drv);
    lv_fs_drv_init(fs_drv);

    fs_drv->letter = LV_FS_ARDUINO_SD_LETTER;
    fs_drv->open_cb = fs_open;
    fs_drv->close_cb = fs_close;
    fs_drv->read_cb = fs_read;
    fs_drv->write_cb = fs_write;
    fs_drv->seek_cb = fs_seek;
    fs_drv->tell_cb = fs_tell;

    fs_drv->dir_close_cb = NULL;
    fs_drv->dir_open_cb = NULL;
    fs_drv->dir_read_cb = NULL;

    lv_fs_drv_register(fs_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Открыть файл
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param path      путь к файлу, начинающийся с буквы драйвера (например, S:/folder/file.txt)
 * @param mode      читать: FS_MODE_RD, писать: FS_MODE_WR, оба:FS_MODE_RD|  FS_MODE_WR
 * @return          дескриптор файла илиNULLв случае ошибки
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    LV_UNUSED(drv);

    const char * flags;
    if(mode == LV_FS_MODE_WR)
        flags = FILE_WRITE;
    else if(mode == LV_FS_MODE_RD)
        flags = FILE_READ;
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD))
        flags = FILE_WRITE;

    char buf[LV_FS_MAX_PATH_LEN];
    lv_snprintf(buf, sizeof(buf), LV_FS_ARDUINO_SD_PATH "%s", path);

    File file = SD.open(buf, flags);
    if(!file) {
        return NULL;
    }

    SdFile * lf = new SdFile{file};

    return (void *)lf;
}

/**
 * Закрыть открытый файл
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param file_p    указатель на переменную file_t. (открывается с помощьюfs_open)
 * @return          LV_FS_RES_OK: нет ошибок или ошибок из перечисления @lv_fs_res_t.
 */
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    LV_UNUSED(drv);
    SdFile * lf = (SdFile *)file_p;
    lf->file.close();
    delete lf;

    return LV_FS_RES_OK;
}

/**
 * Чтение данных из открытого файла
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param file_p    указатель на переменную file_t.
 * @param buf       указатель на блок памяти, в котором хранятся считанные данные
 * @param btr       количество байтов для чтения
 * @param br        реальное количество прочитанных байт (Byte Read)
 * @return          LV_FS_RES_OK: нет ошибок или ошибок из перечисления @lv_fs_res_t.
 */
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    LV_UNUSED(drv);
    SdFile * lf = (SdFile *)file_p;
    *br = lf->file.read((uint8_t *)buf, btr);

    return (int32_t)(*br) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

/**
 * Записать в файл
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param file_p    указатель на переменную file_t
 * @param buf       указатель на буфер с байтами для записи
 * @param btw       Байты для записи
 * @param bw        количество реальных записанных байт (Bytes Written)
 * @return          LV_FS_RES_OK: нет ошибок или ошибок из перечисления @lv_fs_res_t.
 */
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    LV_UNUSED(drv);
    SdFile * lf = (SdFile *)file_p;
    *bw = lf->file.write((uint8_t *)buf, btw);

    return (int32_t)(*bw) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

/**
 * Установите указатель чтения и записи. Также увеличьте размер файла, если необходимо.
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param file_p    указатель на переменную file_t. (открывается с помощьюfs_open)
 * @param pos       новая позиция указателя чтения и записи
 * @param whence    сообщает, откуда интерпретировать `pos`. См. @lv_fs_whence_t.
 * @return          LV_FS_RES_OK: нет ошибок или ошибок из перечисления @lv_fs_res_t.
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    LV_UNUSED(drv);
    SeekMode mode;
    if(whence == LV_FS_SEEK_SET)
        mode = SeekSet;
    else if(whence == LV_FS_SEEK_CUR)
        mode = SeekCur;
    else if(whence == LV_FS_SEEK_END)
        mode = SeekEnd;

    SdFile * lf = (SdFile *)file_p;

    int rc = lf->file.seek(pos, mode);

    return rc < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

/**
 * Укажите положение указателя чтения и записи.
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param file_p    указатель на переменную file_p
 * @param pos_p     указатель для сохранения результата
 * @return          LV_FS_RES_OK: нет ошибок или ошибок из перечисления @lv_fs_res_t.
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    LV_UNUSED(drv);
    SdFile * lf = (SdFile *)file_p;

    *pos_p = lf->file.position();

    return (int32_t)(*pos_p) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

#else /*LV_USE_FS_ARDUINO_SD == 0*/

#if defined(LV_FS_ARDUINO_SD_LETTER) && LV_FS_ARDUINO_SD_LETTER != '\0'
    #warning "LV_USE_FS_ARDUINO_SD is not enabled but LV_FS_ARDUINO_SD_LETTER is set"
#endif

#endif /*LV_USE_FS_ARDUINO_SD*/
