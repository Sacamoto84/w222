/**
 * @file lv_animimage.h
 *
 */

/**
 * Изменено NXP в 2025 г.
 */

#ifndef LV_ANIMIMAGE_H
#define LV_ANIMIMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../image/lv_image.h"
#include "../../misc/lv_types.h"

#if LV_USE_ANIMIMG != 0

/*Тестирование зависимостей*/
#if LV_USE_IMAGE == 0
#error "lv_animimg: lv_img is required. Enable it in lv_conf.h (LV_USE_IMAGE 1)"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

#if LV_USE_OBJ_PROPERTY
enum _lv_property_animimage_id_t {
    LV_PROPERTY_ID2(ANIMIMAGE, SRC,         LV_PROPERTY_TYPE_POINTER,  LV_PROPERTY_TYPE_INT,  0),
    LV_PROPERTY_ID(ANIMIMAGE, DURATION,     LV_PROPERTY_TYPE_INT,   1),
    LV_PROPERTY_ID(ANIMIMAGE, REPEAT_COUNT, LV_PROPERTY_TYPE_INT,   2),
    LV_PROPERTY_ID(ANIMIMAGE, SRC_COUNT,    LV_PROPERTY_TYPE_INT,   3),
    LV_PROPERTY_ANIMIMAGE_END,
};
#endif

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_animimg_class;

/** Части изображения */
typedef enum {
    LV_ANIM_IMAGE_PART_MAIN,
} lv_animimg_part_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создание объектов анимационного изображения
 * @param parent pointer to an object, it will be the parent of the new button
 * @return pointer to the created animation image object
 */
lv_obj_t * lv_animimg_create(lv_obj_t * parent);

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите источник изображений анимации изображения.
 * @param obj       pointer to an animation image object
 * @param dsc       pointer to a series images
 * @param num       images' number
 */
void lv_animimg_set_src(lv_obj_t * obj, const void * dsc[], size_t num);

/**
 *  Установите источник изображений для перевернутого воспроизведения анимационного изображения.
 * @param obj       pointer to an animation image object
 * @param dsc       pointer to a series images
 * @param num       images' number
 */
void lv_animimg_set_src_reverse(lv_obj_t * obj, const void * dsc[], size_t num);

/**
 * Запустите анимацию изображения.
 * @param obj   pointer to an animation image object
 */
void lv_animimg_start(lv_obj_t * obj);

/**
 * Удалите анимацию изображения.
 * @param obj   pointer to an animation image object
 */
bool lv_animimg_delete(lv_obj_t * obj);

/**
 * Установите продолжительность анимации изображения. единица измерения: мс
 * @param obj       pointer to an animation image object
 * @param duration  the duration in milliseconds
 */
void lv_animimg_set_duration(lv_obj_t * obj, uint32_t duration);

/**
 * Установите время повторного воспроизведения анимации изображения.
 * @param obj       pointer to an animation image object
 * @param count     the number of times to repeat the animation
 */
void lv_animimg_set_repeat_count(lv_obj_t * obj, uint32_t count);

/**
 * Создайте анимацию изображения, которая будет воспроизводиться, когда будет готово направление вперед.
 * @param obj   pointer to an animation image object
 * @param duration   the duration of the playback image animation in milliseconds. 0: disable playback
 */
void lv_animimg_set_reverse_duration(lv_obj_t * obj, uint32_t duration);

/**
 * Создайте анимацию изображения, которая будет воспроизводиться, когда будет готово направление вперед.
 * @param obj   pointer to an animation image object
 * @param duration   delay in milliseconds before starting the playback image animation.
 */
void lv_animimg_set_reverse_delay(lv_obj_t * obj, uint32_t duration);

/**
 * Установите вызов функции, когда анимационное изображение действительно запускается (учитывая `delay` )
 * @param obj   pointer to an animation image object
 * @param start_cb   a function call when the animation is start
 */
void lv_animimg_set_start_cb(lv_obj_t * obj, lv_anim_start_cb_t start_cb);

/**
 * Установите вызов функции после завершения анимации
 * @param obj pointer to an animation image object
 * @param completed_cb  a function call when the animation is completed
 */
void lv_animimg_set_completed_cb(lv_obj_t * obj, lv_anim_completed_cb_t completed_cb);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получите источник изображений анимации изображения.
 * @param obj   pointer to an animation image object
 * @return a     pointer that will point to a series images
 */
const void ** lv_animimg_get_src(lv_obj_t * obj);

/**
 * Получите источник изображений анимации изображения.
 * @param obj   pointer to an animation image object
 * @return      the number of source images
 */
uint8_t lv_animimg_get_src_count(lv_obj_t * obj);

/**
 * Получите время продолжительности анимации изображения. единица измерения: мс
 * @param obj   pointer to an animation image object
 * @return      the animation duration time
 */
uint32_t lv_animimg_get_duration(lv_obj_t * obj);

/**
 * Получите время повторения анимации изображения.
 * @param obj   pointer to an animation image object
 * @return      the repeat count
 */
uint32_t lv_animimg_get_repeat_count(lv_obj_t * obj);

/**
 * Получите анимацию изображения, лежащую в основе анимации.
 * @param obj   pointer to an animation image object
 * @return      the animation reference
 */
lv_anim_t * lv_animimg_get_anim(lv_obj_t * obj);

#endif /*LV_USE_ANIMIMG*/

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /*LV_ANIMIMAGE_H*/
