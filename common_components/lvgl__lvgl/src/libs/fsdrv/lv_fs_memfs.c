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
 * Буква диска по умолчанию — «M», но ее можно изменить в lv_conf.h:
 *
 * #define LV_FS_MEMFS_LETTER 'М'
 *
 * Чтобы беспрепятственно использовать его с интерфейсом файловой системы, был введен новый объект расширенного пути:
 *
 * lv_fs_path_ex_t мемпат;
 *
 * Эту структуру можно инициализировать с помощью вспомогательной функции:
 *
 * lv_fs_make_path_ex (&mempath, (const uint8_t *) & my_mem_buffer , sizeof( my_mem_buffer ));
 *
 * Затем «файл» можно открыть с помощью:
 *
 * файл lv_fs_file_t;
 * lv_fs_res_t res = lv_fs_open (&file, (const char *) & mempath, LV_FS_MODE_RD );
 *
 * Объект пути можно использовать в любом месте, где требуется путь к файлу, например:
 *
 * lv_font_t * my_font = lv_binfont_create ((const char *) & mempath);
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
 * @param drv   pointer to a driver where this function belongs
 * @param path  pointer to an extended path object containing the memory buffer address and size
 * @param mode  read: FS_MODE_RD (currently only reading from the buffer is supported)
 * @return pointer to FIL struct or NULL in case of fail
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    LV_UNUSED(drv);
    LV_UNUSED(mode);
    return (void *)path;
}

/**
 * Закрыть открытый файл
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a FILE variable. (opened with fs_open)
 * @return LV_FS_RES_OK: no error, the file is read
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
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a FILE variable.
 * @param buf       pointer to a memory block where to store the read data
 * @param btr       number of Bytes To Read
 * @param br        the real number of read bytes (Byte Read)
 * @return LV_FS_RES_OK: no error, the file is read
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
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a FILE variable. (opened with fs_open )
 * @param pos       the new position of read pointer
 * @return LV_FS_RES_OK: no error, the file is read
 *         любая ошибка из перечисления lv_fs_res_t
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    /* NOTE: this function is only called to determine the end of the buffer when LV_FS_SEEK_END was given to lv_fs_seek() */
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
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a FILE variable
 * @param pos_p     pointer to store the result
 * @return LV_FS_RES_OK: no error, the file is read
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
