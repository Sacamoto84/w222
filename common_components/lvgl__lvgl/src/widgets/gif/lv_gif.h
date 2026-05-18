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
 * @param parent    указатель на объект, он будет родителем нового gif.
 * @return          указатель на объект gif
 */
lv_obj_t * lv_gif_create(lv_obj_t * parent);

/**
 * Установите цветной формат внутреннего выделенного фреймбуфера, в котором находится gif.
 * будет декодировано. По умолчанию — LV_COLOR_FORMAT_ARGB8888.
 * Вызовите это перед`lv_gif_set_src`, чтобы избежать перераспределения кадрового буфера.
 * @param obj            указатель на объект gif
 * @param color_format   цветовой формат фреймбуфера gif
 */
void lv_gif_set_color_format(lv_obj_t * obj, lv_color_format_t color_format);

/**
 * Установите данные gif для отображения на объекте
 * @param obj       указатель на объект gif
 * @param src       1) указатель на дескриптор ::lv_image_dsc_t (который содержит необработанные данные gif) или
 *                  2) путь к файлу gif (например, « S:/dir/anim.gif")
 */
void lv_gif_set_src(lv_obj_t * obj, const void * src);

/**
 * Перезапустите GIF-анимацию.
 * @param obj указатель на объект gif
 */
void lv_gif_restart(lv_obj_t * obj);

/**
 * Приостановите GIF-анимацию.
 * @param obj указатель на объект gif
 */
void lv_gif_pause(lv_obj_t * obj);

/**
 * Возобновите GIF-анимацию.
 * @param obj указатель на объект gif
 */
void lv_gif_resume(lv_obj_t * obj);

/**
 * Проверяет, правильно ли загружен GIF.
 * @param obj указатель на объект gif
 */
bool lv_gif_is_loaded(lv_obj_t * obj);

/**
 * Получите количество циклов для GIF.
 * @param obj указатель на объект gif
 */
int32_t lv_gif_get_loop_count(lv_obj_t * obj);

/**
 * Установите количество циклов для GIF.
 * @param obj   указатель на объект gif
 * @param count количество циклов, которое нужно установить
 */
void lv_gif_set_loop_count(lv_obj_t * obj, int32_t count);

/**
 * Установите, следует ли декодировать невидимый объект.
 * @param obj        указатель на объект gif
 * @param auto_pause true: автоматическая пауза, когда невидим, false: не делать автоматическую паузу
 */
void lv_gif_set_auto_pause_invisible(lv_obj_t * obj, bool auto_pause);

/**
 * Получить ширину и высоту GIF
 * @param src указатель на gif-файл
 * @param w указатель для сохранения ширины
 * @param h указатель для сохранения высоты
 * @return правда: успех; ложь: не удалось
 */
bool lv_gif_get_size(const char * src, uint16_t * w, uint16_t * h);

/**
 * Получите количество кадров GIF.
 * @param obj указатель на объект gif
 * @return количество кадров GIF
 */
int32_t lv_gif_get_frame_count(lv_obj_t * obj);

/**
 * Получите текущий индекс кадра GIF.
 * @param obj указатель на объект gif
 * @return текущий индекс кадра GIF
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
