/* Данная форма исходного кода регулируется условиями публичной лицензии Mozilla.
 * Лицензия, версия 2.0. Если копия MPL не распространялась вместе с этим
 * файл, вы можете получить его по адресу http://mozilla.org/MPL/2.0/.. */

#pragma once

#include "../../../lv_conf_internal.h"
#include LV_STDINT_INCLUDE


/**
 * \brief       Is entry a directory?
 */
#define FROGFS_IS_DIR(e) (e->u.child_count < 0xFF00)

/**
 * \brief       Is entry a file?
 */
#define FROGFS_IS_FILE(e) (e->u.child_count >= 0xFF00)

/**
 * \brief       Is entry a compressed file?
 */
#define FROGFS_IS_COMP(e) (e->u.child_count > 0xFF00)

/**
 * \brief Заголовок файловой системы
 */
typedef struct frogfs_head_t {
    uint32_t magic; /**< filesystem magic */
    uint8_t ver_major; /**< major version */
    uint8_t ver_minor; /**< minor version */
    uint16_t num_entries; /** количество входов */
    uint32_t bin_sz; /**< binary length */
} frogfs_head_t;

/**
 * \brief Запись в хеш-таблице
 */
typedef struct frogfs_hash_t {
    uint32_t hash; /**< path hash */
    uint32_t offs; /**< object offset */
} frogfs_hash_t;

/**
 * \brief Заголовок записи
 */
struct frogfs_entry_t {
    uint32_t parent; /**< parent entry offset */
    union {
        uint16_t child_count; /**< child entry count */
        uint8_t compression; /**< compression algorithm */
    } u;
    uint8_t seg_sz; /**< path segment size (before alignment) */
    uint8_t opts; /**< compression opts */
};

/**
 * \brief Заголовок объекта каталога
 */
typedef struct frogfs_dir_t {
    const frogfs_entry_t entry;
    uint32_t children[];
} frogfs_dir_t;

/**
 * \brief Заголовок объекта файла
 */
typedef struct frogfs_file_t {
    const frogfs_entry_t entry;
    uint32_t data_offs;
    uint32_t data_sz;
} frogfs_file_t;

/**
 * \brief Заголовок объекта сжатого файла
 */
typedef struct frogfs_comp_t {
    const frogfs_entry_t entry;
    uint32_t data_offs;
    uint32_t data_sz; /**< data size (before alignment) */
    uint32_t real_sz; /**< expanded size */
} frogfs_comp_t;

/**
 * \brief Нижний колонтитул файловой системы
 */
typedef struct frogfs_foot_t {
    uint32_t crc32; /**< crc32 of entire file without this field */
} frogfs_foot_t;
