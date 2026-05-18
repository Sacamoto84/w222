/**
 * @file lv_fs_memfs.c
 *
 * Драйвер интерфейса файловой системы для файлов, отображенных в памяти.
 *
 * Этот драйвер позволяет использовать область памяти как файл, который может быть прочитан обычными файловыми операциями. Это может
 * использоваться, например, для хранения файлов шрифтов в медленной флэш-памяти и загрузки их в RAM по требованию.
 *
 * Вы можете включить его в lv_conf.h:
 *
 * #define LV_USE_FS_MEMFS 1
 *
 * Фактическая реализация использует встроенный механизм кэширования интерфейса файловой системы.
 *
 * Поскольку это не настоящая файловая система, запись в файлы и каталоги не поддерживаются.
 *
 * Буква диска по умолчанию — «M», но ее можно изменить вlv_conf.h:
 *
 * #defineLV_FS_MEMFS_LETTER'М'
 *
 * Чтобы беспрепятственно использовать его с интерфейсом файловой системы, был введен новый объект расширенного пути:
 *
 * lv_fs_path_ex_t мемпат;
 *
 * Эту структуру можно инициализировать с помощью вспомогательной функции:
 *
 * lv_fs_make_path_ex (&mempath, (constuint8_t*) &my_mem_buffer, sizeof(my_mem_buffer));
 *
 * Затем «файл» можно открыть с помощью:
 *
 * файлlv_fs_file_t;
 * lv_fs_res_t res =lv_fs_open(&file, (const char *) & mempath,LV_FS_MODE_RD);
 *
 * Объект пути можно использовать в любом месте, где требуется путь к файлу, например:
 *
 * lv_font_t *my_font=lv_binfont_create((const char *) & mempath);
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_fs_private.h"
#include "../../../lvgl.h"
#if LV_USE_FS_MEMFS

/*********************
 *      DEFINES
 *********************/

#if !LV_FS_IS_VALID_LETTER(LV_FS_MEMFS_LETTER)
    #error "Invalid drive letter"
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
*  STATIC PROTOTYPES
**********************/

static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_fs_drv_t fs_drv; /*Дескриптор драйвера*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Зарегистрируйте драйвер для интерфейса файловой системы.
 */
void lv_fs_memfs_init(void)
{
    /*---------------------------------------------------
     * Зарегистрируйте интерфейс файловой системы в LVGL.
     *--------------------------------------------------*/

    lv_fs_drv_init(&fs_drv);

    /*Настроить поля...*/
    fs_drv.letter = LV_FS_MEMFS_LETTER;
    fs_drv.cache_size = LV_FS_CACHE_FROM_BUFFER;

    fs_drv.open_cb = fs_open;
    fs_drv.close_cb = fs_close;
    fs_drv.read_cb = fs_read;
    fs_drv.write_cb = NULL;
    fs_drv.seek_cb = fs_seek;
    fs_drv.tell_cb = fs_tell;

    fs_drv.dir_close_cb = NULL;
    fs_drv.dir_open_cb = NULL;
    fs_drv.dir_read_cb = NULL;

    lv_fs_drv_register(&fs_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Открыть файл
 * @param drv   указатель на драйвер, которому принадлежит эта функция
 * @param path  указатель на объект расширенного пути, содержащий адрес и размер буфера памяти
 * @param mode  чтение:FS_MODE_RD(на данный момент поддерживается только чтение из буфера)
 * @return указатель на структуруFILилиNULLв случае неудачи
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    LV_UNUSED(drv);
    LV_UNUSED(mode);
    return (void *)path;
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
    LV_UNUSED(file_p);
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
    LV_UNUSED(file_p);
    LV_UNUSED(buf);
    LV_UNUSED(btr);
    *br = 0;
    return LV_FS_RES_OK;
}

/**
 * Установите указатель чтения.
 * @param drv       указатель на драйвер, которому принадлежит эта функция
 * @param file_p    указатель на переменную FILE. (открывается с помощьюfs_open)
 * @param pos       новое положение указателя чтения
 * @return LV_FS_RES_OK: ошибок нет, файл прочитан
 *         любая ошибка из перечисления lv_fs_res_t
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    /* NOTE: эта функция вызывается только для определения конца буфера, когдаLV_FS_SEEK_ENDбыл переданlv_fs_seek() */
    LV_UNUSED(drv);
    lv_fs_file_t * fp = (lv_fs_file_t *)file_p;
    switch(whence) {
        case LV_FS_SEEK_SET: {
                fp->cache->file_position = pos;
                break;
            }
        case LV_FS_SEEK_CUR: {
                fp->cache->file_position += pos;
                break;
            }
        case LV_FS_SEEK_END: {
                fp->cache->file_position = fp->cache->end - pos;
                break;
            }
    }
    if(fp->cache->file_position < fp->cache->start)
        fp->cache->file_position = fp->cache->start;
    else if(fp->cache->file_position > fp->cache->end)
        fp->cache->file_position = fp->cache->end;
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
    *pos_p = ((lv_fs_file_t *)file_p)->cache->file_position;
    return LV_FS_RES_OK;
}

#else /*LV_USE_FS_MEMFS == 0*/

#if defined(LV_FS_MEMFS_LETTER) && LV_FS_MEMFS_LETTER != '\0'
    #warning "LV_USE_FS_MEMFS is not enabled but LV_FS_MEMFS_LETTER is set"
#endif

#endif /*LV_USE_FS_MEMFS*/
