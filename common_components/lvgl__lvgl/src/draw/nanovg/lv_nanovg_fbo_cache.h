/**
 * @file lv_nanovg_fbo_cache.h
 *
 */

#ifndef LV_NANOVG_FBO_CACHE_H
#define LV_NANOVG_FBO_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_NANOVG

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_nanovg_unit_t;
struct _lv_cache_entry_t;
struct NVGLUframebuffer;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Инициализируйте кэш FBO
 * @param u указатель на блок nanovg
 */
void lv_nanovg_fbo_cache_init(struct _lv_draw_nanovg_unit_t * u);

/**
 * @brief Деинициализируйте кэш FBO.
 * @param u указатель на блок nanovg
 */
void lv_nanovg_fbo_cache_deinit(struct _lv_draw_nanovg_unit_t * u);

/**
 * @brief ДостаньтеFBOиз кеша, создайте новый, если не найден.
 * @param u указатель на блок nanovg
 * @param width ширина FBO
 * @param height высота FBO
 * @param flags флаги FBO
 * @param format формат текстуры
 * @return запись кэшаFBOили NULL, если не найдена
 */
struct _lv_cache_entry_t * lv_nanovg_fbo_cache_get(struct _lv_draw_nanovg_unit_t * u, int width, int height, int flags,
                                                   int format);

/**
 * @brief ОсвободитеFBOиз кэша.
 * @param u указатель на блок nanovg
 * @param entry запись кэшаFBOдля освобождения
 */
void lv_nanovg_fbo_cache_release(struct _lv_draw_nanovg_unit_t * u, struct _lv_cache_entry_t * entry);

/**
 * @brief Преобразование записи кэша во фреймбуфер
 * @param entry запись кэша FBO
 * @return указатель фреймбуфера
 */
struct NVGLUframebuffer * lv_nanovg_fbo_cache_entry_to_fb(struct _lv_cache_entry_t * entry);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_NANOVG*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_NANOVG_FBO_CACHE_H*/
