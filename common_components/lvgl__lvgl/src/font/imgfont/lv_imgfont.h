/**
 * @file lv_imgfont.h
 *
 */

#ifndef LV_IMGFONT_H
#define LV_IMGFONT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#include "../../font/lv_font.h"

#if LV_USE_IMGFONT

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/* получает путь к изображению этого персонажа */
typedef const void * (*lv_imgfont_get_path_cb_t)(const lv_font_t * font,
                                                 uint32_t unicode, uint32_t unicode_next,
                                                 int32_t * offset_y, void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создает графический шрифт с указанием параметра info.
 * @param height размер шрифта
 * @param path_cb функция для получения пути к изображению персонажа.
 * @param user_data указатель на данные пользователя
 * @return указатель на новый imgfont или NULL, если возникла ошибка.
 */
lv_font_t * lv_imgfont_create(uint16_t height, lv_imgfont_get_path_cb_t path_cb, void * user_data);

/**
 * Уничтожить созданный графический шрифт.
 * @param font указатель на дескриптор шрифта изображения.
 */
void lv_imgfont_destroy(lv_font_t * font);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_IMGFONT*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_IMGFONT_H */
