/* Данная форма исходного кода регулируется условиями публичной лицензии Mozilla.
 * Лицензия, версия 2.0. Если копия MPL не распространялась вместе с этим
 * файл, вы можете получить его по адресу http://mozilla.org/MPL/2.0/.. */

#pragma once

#include "../include/frogfs/frogfs_types.h"

#define FROGFS_PRIVATE_STRUCTS
#include "../include/frogfs/frogfs.h"
#include "frogfs_format.h"


typedef struct frogfs_decomp_funcs_t frogfs_decomp_funcs_t;

/**
 * \brief Структура, описывающая запись файла frogfs
 */
struct frogfs_fh_t {
    const frogfs_fs_t *fs; /**< frogfs fs pointer */
    const frogfs_file_t *file; /**< file header pointer */
    const void *data_start; /**< data start pointer */
    const void *data_ptr; /**< current data pointer */
    size_t data_sz; /**< data size */
    size_t real_sz; /**< real (expanded) size */
    unsigned int flags; /** открытые флаги */
    const frogfs_decomp_funcs_t *decomp_funcs; /**< decompresor funcs */
    void *decomp_priv; /**< decompressor private data */
};

/**
 * \brief Структура, описывающая запись каталога frogfs
 */
struct frogfs_dh_t {
    const frogfs_fs_t *fs; /**< frogfs fs pointer */
    const frogfs_dir_t *dir; /**< frogfs entry */
    long index; /**< current index */
};

/**
 * \brief Структура указателей на функции, описывающие декомпрессор
 */
struct frogfs_decomp_funcs_t {
    int (*open)(frogfs_fh_t *f, unsigned int flags);
    void (*close)(frogfs_fh_t *f);
    ssize_t (*read)(frogfs_fh_t *f, void *buf, size_t len);
    ssize_t (*seek)(frogfs_fh_t *f, long offset, int mode);
    size_t (*tell)(frogfs_fh_t *f);
};

/**
 * \brief Функции декомпрессора Raw
 */
extern const frogfs_decomp_funcs_t frogfs_decomp_raw;

/**
 * \brief Функции термоусадочного декомпрессора
 */
extern const frogfs_decomp_funcs_t frogfs_decomp_heatshrink;

/**
 * \brief Функции декомпрессора Miniz
 */
extern const frogfs_decomp_funcs_t frogfs_decomp_miniz;

/**
 * \brief Функции декомпрессора Zlib
 */
extern const frogfs_decomp_funcs_t frogfs_decomp_zlib;

#include "../include/frogfs/frogfs.h"
