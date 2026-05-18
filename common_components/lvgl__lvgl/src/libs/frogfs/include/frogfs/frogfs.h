/* Данная форма исходного кода регулируется условиями публичной лицензии Mozilla.
 * Лицензия, версия 2.0. Если копия MPL не распространялась вместе с этим
 * файл, вы можете получить его по адресу http://mozilla.org/MPL/2.0/.. */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../../lv_conf_internal.h"
#include LV_STDDEF_INCLUDE
#include LV_STDINT_INCLUDE
#include "frogfs_types.h"

/**
 * \brief Магическое число, используемое в заголовке файла frogfs
 */
#define FROGFS_MAGIC 0x474F5246 /** FROG */

/**
 * \brief Основная версия, поддерживаемая этим исходным дистрибутивом
 */
#define FROGFS_VER_MAJOR 1

/**
 * \brief Дополнительная версия, поддерживаемая этим исходным дистрибутивом
 */
#define FROGFS_VER_MINOR 0

/**
 * \brief Флаг для \a frogfs_open, позволяющий открывать любой файл как необработанный. Полезно для
 *              передавать сжатые данные через транспорт, например HTTP.
 */
#define FROGFS_OPEN_RAW (1 << 0)

/**
 * \brief Перечисление типов записей frogfs
 */
typedef enum frogfs_entry_type_t {
    FROGFS_ENTRY_TYPE_DIR,
    FROGFS_ENTRY_TYPE_FILE,
} frogfs_entry_type_t;

/**
 * \brief Идентификаторы алгоритмов сжатия
 */
typedef enum frogfs_comp_algo_t {
    FROGFS_COMP_ALGO_NONE,
    FROGFS_COMP_ALGO_ZLIB,
    FROGFS_COMP_ALGO_HEATSHRINK,
    FROGFS_COMP_ALGO_GZIP,
} frogfs_comp_algo_t;

/**
 * \brief Конфигурация функции \a frogfs_init
 */
typedef struct frogfs_config_t {
    const void *addr; /**< address of an frogfs filesystem in memory */
} frogfs_config_t;

/**
 * \brief Дескриптор файловой системы frogfs
 */
typedef struct frogfs_fs_t frogfs_fs_t;

/**
 * \brief Структура, заполняемая функцией \a frogfs_stat
 */
typedef struct frogfs_stat_t {
    frogfs_entry_type_t type; /**< entry type */
    size_t size; /**< uncompressed file size */
    frogfs_comp_algo_t compression; /**< compression type */
    size_t compressed_sz; /**< compressed file size */
} frogfs_stat_t;

/**
 * \brief Указатель входа в файловую систему
*/
typedef struct frogfs_entry_t frogfs_entry_t;
typedef struct frogfs_dh_t frogfs_dh_t;
typedef struct frogfs_fh_t frogfs_fh_t;

#if !defined(FROGFS_PRIVATE_STRUCTS)
/**
 * \brief Дескриптор каталога frogfs
 */
struct frogfs_dh_t {
    const frogfs_fs_t *fs; /**< filesystem handle */
    frogfs_entry_t *entry; /**< directory entry */
};

/**
 * \brief Дескриптор файла frogfs
 */
struct frogfs_fh_t {
    const frogfs_fs_t *fs; /**< filesystem handle */
    frogfs_entry_t *entry; /**< file entry */
};
#endif

/**
 * \brief Инициализация и возврат экземпляра \a frogfs_fs_t
 * \param[in] config конфигурация frogfs
 * \return \a указатель frogfs_fs_t или \a NULL в случае ошибки
 */
frogfs_fs_t *frogfs_init(const frogfs_config_t *conf);

/**
 * \brief Удалить экземпляр \a frogfs_fs_t
 * \param[in] fs \a указатель frogfs_fs_t
 */
void frogfs_deinit(frogfs_fs_t *fs);

/**
 * \brief Получить запись frogfs для пути
 * \param[in] fs \a указатель frogfs_fs_t
 * \param[in] путь строка пути
 * \return \a указатель frogfs_entry_t или \a NULL, если путь не указан
 *                      нашел
 */
const frogfs_entry_t *frogfs_get_entry(const frogfs_fs_t *fs,
        const char *path);

/**
 * \brief Получить имя для записи frogfs
 * \param[in] запись \a указатель frogfs_entry_t
 * \return строка имени, ожидается, что вызывающий абонент освободится
 */
char *frogfs_get_name(const frogfs_entry_t *entry);

/**
 * \brief Получение полного пути к записи frogfs
 * \param[in] fs \a указатель frogfs_fs_t
 * \param[in] запись \a указатель frogfs_entry_t
 * \return строка полного пути или \a NULL, если запись NULL, вызывающий абонент
 *                      ожидается освобождение
 */
char *frogfs_get_path(const frogfs_fs_t *fs, const frogfs_entry_t *entry);

/**
 * \brief Возврат, если запись является каталогом
 * \param[in] запись \a указатель frogfs_entry_t
 * \return 1, если каталог, 0 в противном случае
 */
int frogfs_is_dir(const frogfs_entry_t *entry);

/**
 * \brief Возврат, если запись является файлом
 * \param[in] запись \a указатель frogfs_entry_t
 * \return 1, если файл, 0 в противном случае
 */
int frogfs_is_file(const frogfs_entry_t *entry);

/**
 * \brief Получение информации о записи frogfs
 * \param[in] fs \a указатель frogfs_fs_t
 * \param[in] запись \a указатель frogfs_entry_t
 * \param[out] st \a структура frogfs_stat_t
 */
void frogfs_stat(const frogfs_fs_t *fs, const frogfs_entry_t *entry,
        frogfs_stat_t *st);

/**
 * \brief Откройте запись frogfs как файл из экземпляра \a frogfs_fs_t.
 * \param[in] fs \a frogfs_fs_t указатель
 * \param[in] запись \a указатель frogfs_entry_t
 * \param[in] флаги открытые флаги
 * \return \a frogfs_fh_t или \a NULL, если не найден
 */
frogfs_fh_t *frogfs_open(const frogfs_fs_t *fs, const frogfs_entry_t *entry,
        unsigned int flags);

/**
 * \brief Закрытие открытой записи файла
 * \param[in] f \a указатель frogfs_fh_t
 */
void frogfs_close(frogfs_fh_t *fh);

/**
 * \brief Определяет, открыт ли дескриптор файла в исходном виде.
 * \param[in] f \a указатель frogfs_fh_t
 * \return 1, если файл открыт в исходном виде, 0 в противном случае
*/
int frogfs_is_raw(frogfs_fh_t *fh);

/**
 * \brief Чтение данных из записи открытого файла
 * \param[in] f \a указатель frogfs_fh_t
 * \param[out] буфер буфера для чтения
 * \param[in] len максимальное количество байтов для чтения
 * \возвращает фактическое количество прочитанных байтов, ноль, если конец файла
 *                      достиг
 */
ssize_t frogfs_read(frogfs_fh_t *fh, void *buf, size_t len);

/**
 * \brief Поиск позиции в записи открытого файла
 * \param[in] f \a указатель frogfs_fh_t
 * \param[in] смещение позиции файла (относительное или абсолютное)
 * Режим \param[in] \a SEEK_SET , \a SEEK_CUR или \a SEEK_END
 * \вернуть текущую позицию в файле или <0 в случае ошибки
 */
ssize_t frogfs_seek(frogfs_fh_t *fh, long offset, int mode);

/**
 * \brief Получить текущую позицию в записи открытого файла
 * \param[in] f \a указатель frogfs_fh_t
 * \вернуть текущую позицию в файле или <0 в случае ошибки
 */
size_t frogfs_tell(frogfs_fh_t *fh);

/**
 * \brief Получение необработанной памяти для записи необработанного файла
 * \param[in] f \a указатель frogfs_fh_t
 * \param[out] указатель buf указатель на buf
 * \возвращает длину необработанных данных
 */
size_t frogfs_access(frogfs_fh_t *fh, const void **buf);

/**
 * \brief Открыть каталог для чтения дочерних записей
 * \param[in] fs \a указатель frogfs_fs_t
 * \param[in] запись \a frogfs_entry_t указатель на корневой директорий
 * \return \a указатель frogfs_dh_t или \a NULL, если он недействителен
 */
frogfs_dh_t *frogfs_opendir(frogfs_fs_t *fs, const frogfs_entry_t *entry);

/**
 * \brief Закрыть каталог
 * \param[in] d \a указатель frogfs_dh_t
 */
void frogfs_closedir(frogfs_dh_t *dh);

/**
 * \brief Получить следующую дочернюю запись в каталоге
 * \param[in] d \a указатель frogfs_dh_t
 * \return \a указатель frogfs_entry_t или \a NULL, если был достигнут конец
 *                      достиг
 */
const frogfs_entry_t *frogfs_readdir(frogfs_dh_t *dh);

/**
 * \brief Установить индекс записи каталога в значение, возвращаемое \a frogfs_telldir
 *              для текущего указателя \a frogfs_dh_t
 * \param[in] d \a указатель frogfs_dh_t
 * \param[in] индекс записи loc
 */
void frogfs_seekdir(frogfs_dh_t *dh, long loc);

/**
 * \brief Возвращает текущий индекс записи для каталога
 * \param[in] d \a указатель frogfs_dh_t
 * \вернуть индекс записи
 */
long frogfs_telldir(frogfs_dh_t *dh);

#ifdef __cplusplus
} /* внешний "С" */
#endif
