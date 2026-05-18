/**
 * @file lv_port_fs_template.c
 *
 */

/*Скопируйте этот файл как «lv_port_fs.c» и установите для этого значения значение «1», чтобы отключить контент.*/
#if 0

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_fs_template.h"
#include "../../lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void fs_init(void);

static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_size(lv_fs_drv_t * drv, void * file_p, uint32_t * size_p);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

static void * fs_dir_open(lv_fs_drv_t * drv, const char * path);
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * rddir_p, char * fn, uint32_t fn_len);
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * rddir_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_fs_init(void)
{
    /*----------------------------------------------------
     * Инициализируйте устройство хранения и файловую систему.
     * -------------------------------------------------*/
    fs_init();

    /*---------------------------------------------------
     * Зарегистрируйте интерфейс файловой системы в LVGL.
     *--------------------------------------------------*/

    static lv_fs_drv_t fs_drv;
    lv_fs_drv_init(&fs_drv);

    /*Настроить поля...*/
    fs_drv.letter = 'P';
    fs_drv.open_cb = fs_open;
    fs_drv.close_cb = fs_close;
    fs_drv.read_cb = fs_read;
    fs_drv.write_cb = fs_write;
    fs_drv.seek_cb = fs_seek;
    fs_drv.tell_cb = fs_tell;

    fs_drv.dir_close_cb = fs_dir_close;
    fs_drv.dir_open_cb = fs_dir_open;
    fs_drv.dir_read_cb = fs_dir_read;

    lv_fs_drv_register(&fs_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Инициализируйте устройство хранения и файловую систему.*/
static void fs_init(void)
{
    /*например для FatFS необходимо придумать картуSDи саму FatFS*/

    /*Вы кодируете здесь*/
}

/**
 * Открыть файл
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param path      путь к файлу, начинающийся с буквы драйвера (например, S:/folder/file.txt)
 * @param mode      читать: FS_MODE_RD, писать: FS_MODE_WR, оба:FS_MODE_RD|  FS_MODE_WR
 * @return          дескриптор файла илиNULLв случае ошибки
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    void * f = NULL;

    if(mode == LV_FS_MODE_WR) {
        /*Открыть файл для записи*/
        f = ...         /*Добавьте сюда свой код*/
    }
    else if(mode == LV_FS_MODE_RD) {
        /*Открыть файл для чтения*/
        f = ...         /*Добавьте сюда свой код*/
    }
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) {
        /*Открыть файл для чтения и записи*/
        f = ...         /*Добавьте сюда свой код*/
    }

    return f;
}

/**
 * Закрыть открытый файл
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param file_p    указатель на переменную file_t. (открывается с помощьюfs_open)
 * @return          LV_FS_RES_OK: нет ошибок или ошибок из перечисления @lv_fs_res_t.
 */
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /*Добавьте сюда свой код*/

    return res;
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
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /*Добавьте сюда свой код*/

    return res;
}

/**
 * Записать в файл
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param file_p    указатель на переменную file_t
 * @param buf       указатель на буфер с байтами для записи
 * @param btw       Байты для записи
 * @param bw        количество реальных записанных байт (Bytes Written).  NULL, если не используется.
 * @return          LV_FS_RES_OK: нет ошибок или ошибок из перечисления @lv_fs_res_t.
 */
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /*Добавьте сюда свой код*/

    return res;
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
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /*Добавьте сюда свой код*/

    return res;
}
/**
 * Укажите положение указателя чтения и записи.
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param file_p    указатель на переменную file_t
 * @param pos_p     указатель для сохранения результата
 * @return          LV_FS_RES_OK: нет ошибок или ошибок из перечисления @lv_fs_res_t.
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /*Добавьте сюда свой код*/

    return res;
}

/**
 * Инициализируйте переменнуюlv_fs_dir_tдля чтения каталога.
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param path      путь к каталогу
 * @return          указатель на дескриптор чтения каталога илиNULLв случае ошибки
 */
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path)
{
    void * dir = NULL;
    /*Добавьте сюда свой код*/
    dir = ...           /*Добавьте сюда свой код*/
          return dir;
}

/**
 * Прочитайте следующее имя файла из каталога.
 * Название каталогов начинается с '/'
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param rddir_p   указатель на инициализированную переменную lv_fs_dir_t
 * @param fn        указатель на буфер для хранения имени файла
 * @param fn_len    длина буфера для хранения имени файла
 * @return          LV_FS_RES_OK: нет ошибок или ошибок из перечисления @lv_fs_res_t.
 */
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * rddir_p, char * fn, uint32_t fn_len)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /*Добавьте сюда свой код*/

    return res;
}

/**
 * Закройте чтение каталога
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param rddir_p   указатель на инициализированную переменную lv_fs_dir_t
 * @return          LV_FS_RES_OK: нет ошибок или ошибок из перечисления @lv_fs_res_t.
 */
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * rddir_p)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /*Добавьте сюда свой код*/

    return res;
}

#else /*Включите этот файл вверху*/

/*Этот вымышленный typedef существует исключительно для того, чтобы успеть замолчать -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
