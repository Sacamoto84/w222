/**
 * @file lv_gif.h
 *
 */

#ifndef LV_GIF_H
#define LV_GIF_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GIF

#include "../../misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_gif_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создать GIF-объект
 * @param parent    pointer to an object, it will be the parent of the new gif.
 * @return          pointer to the gif obj
 */
lv_obj_t * lv_gif_create(lv_obj_t * parent);

/**
 * Установите цветовой формат внутреннего выделенного фреймбуфера, в котором находится gif.
 * будет декодировано. По умолчанию — LV_COLOR_FORMAT_ARGB8888.
 * Вызовите это перед `lv_gif_set_src`, чтобы избежать перераспределения кадрового буфера.
 * @param obj            pointer to a gif object
 * @param color_format   the color format of the gif framebuffer
 */
void lv_gif_set_color_format(lv_obj_t * obj, lv_color_format_t color_format);

/**
 * Установите данные gif для отображения на объекте
 * @param obj       pointer to a gif object
 * @param src       1) pointer to an ::lv_image_dsc_t descriptor (which contains gif raw data) or
 *                  2) path to a gif file (e.g. "S:/dir/anim.gif")
 */
void lv_gif_set_src(lv_obj_t * obj, const void * src);

/**
 * Перезапустите GIF-анимацию.
 * @param obj pointer to a gif obj
 */
void lv_gif_restart(lv_obj_t * obj);

/**
 * Приостановите GIF-анимацию.
 * @param obj pointer to a gif obj
 */
void lv_gif_pause(lv_obj_t * obj);

/**
 * Возобновите GIF-анимацию.
 * @param obj pointer to a gif obj
 */
void lv_gif_resume(lv_obj_t * obj);

/**
 * Проверяет, правильно ли загружен GIF.
 * @param obj pointer to a gif obj
 */
bool lv_gif_is_loaded(lv_obj_t * obj);

/**
 * Получите количество циклов для GIF.
 * @param obj pointer to a gif obj
 */
int32_t lv_gif_get_loop_count(lv_obj_t * obj);

/**
 * Установите количество циклов для GIF.
 * @param obj   pointer to a gif obj
 * @param count the loop count to set
 */
void lv_gif_set_loop_count(lv_obj_t * obj, int32_t count);

/**
 * Установите, следует ли декодировать невидимый объект.
 * @param obj        pointer to a gif object
 * @param auto_pause true: auto pause when invisible, false: don't auto pause
 */
void lv_gif_set_auto_pause_invisible(lv_obj_t * obj, bool auto_pause);

/**
 * Получить ширину и высоту GIF
 * @param src pointer to a gif file
 * @param w pointer to store width
 * @param h pointer to store height
 * @return true: success; false: failed
 */
bool lv_gif_get_size(const char * src, uint16_t * w, uint16_t * h);

/**
 * Получите количество кадров GIF.
 * @param obj pointer to a gif object
 * @return frame count of the GIF
 */
int32_t lv_gif_get_frame_count(lv_obj_t * obj);

/**
 * Получите текущий индекс кадра GIF.
 * @param obj pointer to a gif object
 * @return current frame index of the GIF
 */
int32_t lv_gif_get_current_frame_index(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GIF*/

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /*LV_GIF_H*/
