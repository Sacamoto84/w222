/**
 * @file lv_ftsystem.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl.h"
#if LV_USE_FREETYPE && LV_FREETYPE_USE_LVGL_PORT

#include <ft2build.h>
#include FT_CONFIG_CONFIG_H
#include <freetype/internal/ftdebug.h>
#include <freetype/internal/ftstream.h>
#include <freetype/ftsystem.h>
#include <freetype/fterrors.h>
#include <freetype/fttypes.h>

/*********************
 *      DEFINES
 *********************/

/* Макрос FT_COMPONENT используется в режиме трассировки.  Это неявное
 * параметр макросов FT_TRACE() и FT_ERROR(), используемый для печати/логирования
 * сообщения во время выполнения.
 */
#undef  FT_COMPONENT
#define FT_COMPONENT  io

/* Для удобства используем макрос STREAM_FILE для извлечения       */
/* специальный для системы дескриптор потока из данного объекта поток FreeType */
#define STREAM_FILE( stream )  ( (lv_fs_file_t*)stream->descriptor.pointer )

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

FT_CALLBACK_DEF(unsigned long)
ft_lv_fs_stream_io(FT_Stream       stream,
                   unsigned long   offset,
                   unsigned char * buffer,
                   unsigned long   count);
FT_CALLBACK_DEF(void)
ft_lv_fs_stream_close(FT_Stream  stream);
FT_CALLBACK_DEF(void *)
ft_alloc(FT_Memory  memory,
         long       size);
FT_CALLBACK_DEF(void *)
ft_realloc(FT_Memory  memory,
           long       cur_size,
           long       new_size,
           void   *   block);
FT_CALLBACK_DEF(void)
ft_free(FT_Memory  memory,
        void   *   block);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef FT_DEBUG_MEMORY

    extern FT_Int
    ft_mem_debug_init(FT_Memory  memory);

    extern void
    ft_mem_debug_done(FT_Memory  memory);

#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#ifndef FT_CONFIG_OPTION_DISABLE_STREAM_SUPPORT

/* документация находится в ftstream.h */

FT_BASE_DEF(FT_Error)
FT_Stream_Open(FT_Stream    stream,
               const char * filepathname)
{
    lv_fs_file_t  file;

    if(!stream)
        return FT_THROW(Invalid_Stream_Handle);

    stream->descriptor.pointer = NULL;
    stream->pathname.pointer   = (char *)filepathname;
    stream->base               = NULL;
    stream->pos                = 0;
    stream->read               = NULL;
    stream->close              = NULL;

    lv_fs_res_t res = lv_fs_open(&file, filepathname, LV_FS_MODE_RD);

    if(res != LV_FS_RES_OK) {
        FT_ERROR(("FT_Stream_Open:"
                  " could not open `%s'\n", filepathname));

        return FT_THROW(Cannot_Open_Resource);
    }

    lv_fs_seek(&file, 0, LV_FS_SEEK_END);

    uint32_t pos;
    res = lv_fs_tell(&file, &pos);
    if(res != LV_FS_RES_OK) {
        FT_ERROR(("FT_Stream_Open:"));
        FT_ERROR((" opened `%s' but zero-sized\n", filepathname));
        lv_fs_close(&file);
        return FT_THROW(Cannot_Open_Stream);
    }
    stream->size = pos;
    lv_fs_seek(&file, 0, LV_FS_SEEK_SET);

    lv_fs_file_t * file_p = lv_malloc(sizeof(lv_fs_file_t));
    LV_ASSERT_MALLOC(file_p);

    if(!file_p) {
        FT_ERROR(("FT_Stream_Open: malloc failed for file_p"));
        lv_fs_close(&file);
        return FT_THROW(Cannot_Open_Stream);
    }

    *file_p = file;

    stream->descriptor.pointer = file_p;
    stream->read  = ft_lv_fs_stream_io;
    stream->close = ft_lv_fs_stream_close;

    FT_TRACE1(("FT_Stream_Open:"));
    FT_TRACE1((" opened `%s' (%ld bytes) successfully\n",
               filepathname, stream->size));

    return FT_Err_Ok;
}

#endif /* !FT_CONFIG_OPTION_DISABLE_STREAM_SUPPORT */

/* документация находится в ftobjs.h */

FT_BASE_DEF(FT_Memory)
FT_New_Memory(void)
{
    FT_Memory  memory;

    memory = (FT_Memory)lv_malloc(sizeof(*memory));
    if(memory) {
        memory->user    = NULL;
        memory->alloc   = ft_alloc;
        memory->realloc = ft_realloc;
        memory->free    = ft_free;
#ifdef FT_DEBUG_MEMORY
        ft_mem_debug_init(memory);
#endif
    }

    return memory;
}

/* документация находится в ftobjs.h */

FT_BASE_DEF(void)
FT_Done_Memory(FT_Memory  memory)
{
#ifdef FT_DEBUG_MEMORY
    ft_mem_debug_done(memory);
#endif
    lv_free(memory);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Функция распределения памяти.
 * @param memory Указатель на объект памяти.
 * @param size Запрошенный размер в байтах.
 * @return Адрес вновь выделенного блока.
 */
FT_CALLBACK_DEF(void *)
ft_alloc(FT_Memory  memory,
         long       size)
{
    FT_UNUSED(memory);

    return lv_malloc((size_t)size);
}

/**
 * Функция перераспределения памяти.
 * @param memory Указатель на объект памяти.
 * @param cur_size Текущий размер выделенного блока памяти.
 * @param new_size Новый запрошенный размер в байтах.
 * @param block Текущий адрес блока в памяти.
 * @return Адрес перераспределенного блока памяти.
 */
FT_CALLBACK_DEF(void *)
ft_realloc(FT_Memory  memory,
           long       cur_size,
           long       new_size,
           void   *   block)
{
    FT_UNUSED(memory);
    FT_UNUSED(cur_size);

    return lv_realloc(block, (size_t)new_size);
}

/**
 * Функция освобождения памяти.
 * @param memory Указатель на объект памяти.
 * @param block Адрес блока памяти, который необходимо освободить.
 */
FT_CALLBACK_DEF(void)
ft_free(FT_Memory  memory,
        void   *   block)
{
    FT_UNUSED(memory);

    lv_free(block);
}

#ifndef FT_CONFIG_OPTION_DISABLE_STREAM_SUPPORT

/**
 * Функция закрытия потока.
 * @param stream Указатель на объект потока.
 */
FT_CALLBACK_DEF(void)
ft_lv_fs_stream_close(FT_Stream  stream)
{
    lv_fs_file_t * file_p = STREAM_FILE(stream);
    lv_fs_close(file_p);
    lv_free(file_p);

    stream->descriptor.pointer = NULL;
    stream->size               = 0;
    stream->base               = NULL;
}

/**
 * Функция открытия потока.
 * @param stream Указатель на объект потока.
 * @param offset Позиция в потоке данных, с которой начинается чтение.
 * @param buffer Адрес буфера для хранения считанных данных.
 * @param count Количество байтов, которые необходимо прочитать из потока.
 * @return Количество фактически прочитанных байтов.  Если `count' равен нулю (это
 *         функция используется для поиска), ненулевое возвращаемое значение
 *         указывает на ошибку.
 */
FT_CALLBACK_DEF(unsigned long)
ft_lv_fs_stream_io(FT_Stream       stream,
                   unsigned long   offset,
                   unsigned char * buffer,
                   unsigned long   count)
{
    lv_fs_file_t * file_p;

    if(!count && offset > stream->size)
        return 1;

    file_p = STREAM_FILE(stream);

    if(stream->pos != offset)
        lv_fs_seek(file_p, (long)offset, LV_FS_SEEK_SET);

    if(count == 0)
        return 0;

    uint32_t br;
    lv_fs_res_t res = lv_fs_read(file_p, buffer, count, &br);

    return res == LV_FS_RES_OK ? br : 0;
}

#endif /* !FT_CONFIG_OPTION_DISABLE_STREAM_SUPPORT */

#endif/*LV_FREETYPE_USE_LV_FTSYSTEM*/
