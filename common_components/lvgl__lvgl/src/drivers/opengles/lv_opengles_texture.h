/**
 * @file lv_opengles_texture.h
 *
 */

#ifndef LV_OPENGLES_TEXTURE_H
#define LV_OPENGLES_TEXTURE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_OPENGLES

#include "../../display/lv_display.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте светильник, который сливается с текстурой OpenGL.
 * Если у вас уже есть текстура и вы хотите привязать ее к дисплею,
 *    см.  `lv_opengles_texture_create_from_texture_id`
 * @param w    ширина текстуры в пикселях
 * @param h    высота текстуры в пикселях
 * @return     новый дисплей илиNULLв случае неисправности
 */
lv_display_t * lv_opengles_texture_create(int32_t w, int32_t h);

/**
 * Создайте материал, который будет соответствовать предоставленной текстуре OpenGL.
 * Если у вас нет текстуры для привязки ее к дисплею,
 *    см.  `lv_opengles_texture_create`
 * @param w         ширина текстуры в пикселях
 * @param h         высота текстуры в пикселях
 * @param texture_id    текстураLVGLбудет отображаться
 * @return     новый дисплей илиNULLв случае неисправности
 */
lv_display_t * lv_opengles_texture_create_from_texture_id(int32_t w, int32_t h, unsigned int texture_id);

/**
 * Получите текстуру дисплея OpenGL ID.
 * @param disp    дисплей
 * @return        текстура ID
 */
unsigned int lv_opengles_texture_get_texture_id(lv_display_t * disp);

/**
 * Получите текстуры OpenGL, если она беспокоится о ней.
 * @param texture_id   Текстура OpenGL ID
 * @return             display или `NULL`, если дисплея с такой текстурой нет ID
 */
lv_display_t * lv_opengles_texture_get_from_texture_id(unsigned int texture_id);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_OPENGLES */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OPENGLES_TEXTURE_H*/
