/**
 * @file lv_tiny_ttf.h
 *
 */

#ifndef LV_TINY_TTF_H
#define LV_TINY_TTF_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_TINY_TTF

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const char * path; /**< Path to the font file*/
    const void * data; /**< Pointer to the font data*/
    size_t data_size; /**< Size of the font data*/
    size_t cache_size; /**< Size of the font cache*/
} lv_tiny_ttf_font_src_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_font_class_t lv_tiny_ttf_font_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if LV_TINY_TTF_FILE_SUPPORT != 0
/**
 * Создайте шрифт из указанного файла или пути с указанной высотой строки.
 * @param path        путь или имя файла шрифта
 * @param font_size   размер шрифта в пикселях
 * @return объект шрифта
 */
lv_font_t * lv_tiny_ttf_create_file(const char * path, int32_t font_size);

/**
 * Создайте шрифт из указанного файла или пути с указанной высотой строки и указанным размером кэша.
 * @param path        путь или имя файла шрифта
 * @param font_size   размер шрифта в пикселях
 * @param kerning     значение кернинга в пикселях
 * @param cache_size  размер кэша в счетчике
 * @return объект шрифта
 */
lv_font_t * lv_tiny_ttf_create_file_ex(const char * path, int32_t font_size, lv_font_kerning_t kerning,
                                       size_t cache_size);
#endif

/**
 * Создайте шрифт из указанного указателя данных с указанной высотой строки.
 * @param data        указатель данных
 * @param data_size   размер данных
 * @param font_size   размер шрифта в пикселях
 * @return объект шрифта
 */
lv_font_t * lv_tiny_ttf_create_data(const void * data, size_t data_size, int32_t font_size);

/**
 * Создайте шрифт из указанного указателя данных с указанной высотой строки и указанным размером кэша.
 * @param data        указатель данных
 * @param data_size   размер данных
 * @param font_size   размер шрифта в пикселях
 * @param kerning     значение кернинга в пикселях
 * @param cache_size  размер кэша в счетчике
 * @return
 */
lv_font_t * lv_tiny_ttf_create_data_ex(const void * data, size_t data_size, int32_t font_size,
                                       lv_font_kerning_t kerning, size_t cache_size);

/**
 * Выберите размер шрифта для нового font_size.
 * @note Кэш растровых изображений шрифтов и кеш глифов будут очищены.
 * @param font        объект шрифта
 * @param font_size   размер шрифта в пикселях
 */
void lv_tiny_ttf_set_size(lv_font_t * font, int32_t font_size);

/**
 * Уничтожить шрифт, ранее созданный с помощью lv_tiny_ttf_create_xxxx()
 * @param font        объект шрифта
 */
void lv_tiny_ttf_destroy(lv_font_t * font);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TINY_TTF*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_TINY_TTF_H*/
