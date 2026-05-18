/**
 * @file lv_arclabel.h
 *
 */

#ifndef LV_ARCLABEL_H
#define LV_ARCLABEL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_ARCLABEL != 0

/*********************
 *      DEFINES
 *********************/

#define LV_ARCLABEL_DOT_NUM 3
#if LV_WIDGETS_HAS_DEFAULT_VALUE
#define LV_ARCLABEL_DEFAULT_TEXT "Arced Text"
#else
#define LV_ARCLABEL_DEFAULT_TEXT ""
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_ARCLABEL_DIR_CLOCKWISE = 0,
    LV_ARCLABEL_DIR_COUNTER_CLOCKWISE = 1
} lv_arclabel_dir_t;

typedef enum {
    LV_ARCLABEL_TEXT_ALIGN_DEFAULT = 0,
    LV_ARCLABEL_TEXT_ALIGN_LEADING = 1,
    LV_ARCLABEL_TEXT_ALIGN_CENTER = 2,
    LV_ARCLABEL_TEXT_ALIGN_TRAILING = 3,
} lv_arclabel_text_align_t;

typedef enum {
    LV_ARCLABEL_OVERFLOW_VISIBLE,  /**< Show full text, may overflow object area */
    LV_ARCLABEL_OVERFLOW_ELLIPSIS, /**< Show ellipsis (...) when text overflows */
    LV_ARCLABEL_OVERFLOW_CLIP      /**< Clip text at arc boundary */
} lv_arclabel_overflow_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_arclabel_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте объект метки дуги
 * @param parent    pointer to an object, it will be the parent of the new arc label
 * @return          pointer to the created arc label
 */
lv_obj_t * lv_arclabel_create(lv_obj_t * parent);

/*======================
 * Добавить/удалить функции
 *=====================*/

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите текст метки дуги.
 *
 * Эта функция устанавливает текст, отображаемый объектом метки дуги.
 *
 * @param obj       Pointer to the arc label object.
 * @param text      Pointer to a null-terminated string containing the new text for the label.
 */
void lv_arclabel_set_text(lv_obj_t * obj, const char * text);

/**
 * Задайте форматированный текст объекта метки дуги.
 *
 * Эта функция устанавливает текст объекта метки дуги с поддержкой
 * форматирование переменных аргументов, аналогично `printf` .
 *
 * @param obj       The arc label object to set the text for.
 * @param fmt       A format string that specifies how subsequent arguments are converted to text.
 * @param ... Аргументы, следующие за строкой формата, которые используются для замены спецификаторов формата в строке формата.
 */
void lv_arclabel_set_text_fmt(lv_obj_t * obj, const char * fmt, ...) LV_FORMAT_ATTRIBUTE(2, 3);

/**
 * Устанавливает новый статический текст для метки дуги или обновляет его текущим текстом.
 * «Текст» должен оставаться действительным в памяти; метка дуги не управляет своим жизненным циклом.
 *
 * @param obj       Pointer to the arc label object.
 * @param text      Pointer to the new text. If NULL, the label is refreshed with its current text.
 */
void lv_arclabel_set_text_static(lv_obj_t * obj, const char * text);

/**
 * Установите начальный угол дуги. 0 градусов: вправо, 90 вниз и т. д.
 * @param obj       pointer to an arc label object
 * @param start     the start angle. (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 */
void lv_arclabel_set_angle_start(lv_obj_t * obj, lv_value_precise_t start);

/**
 * Установите конечный угол дуги. 0 градусов: вправо, 90 вниз и т. д.
 * @param obj       pointer to an arc label object
 * @param size      the angle size (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 */
void lv_arclabel_set_angle_size(lv_obj_t * obj, lv_value_precise_t size);

/**
 * Установите вращение для всей дуги
 * @param obj       pointer to an arc label object
 * @param offset    rotation angle
 */
void lv_arclabel_set_offset(lv_obj_t * obj, int32_t offset);

/**
 * Установите тип дуги.
 * @param obj       pointer to and arc label object
 * @param dir      arc label's direction
 */
void lv_arclabel_set_dir(lv_obj_t * obj, lv_arclabel_dir_t dir);

/**
 * Включите перекрашивание с помощью встроенных команд
 * @param obj       pointer to an arc label object
 * @param en        true: enable recoloring, false: disable
 * Example: "This is a #ff0000 red# word"
 */
void lv_arclabel_set_recolor(lv_obj_t * obj, bool en);

/**
 * Установите радиус объекта метки дуги.
 *
 * @param obj       pointer to the arc label object.
 * @param radius    The radius value to set for the label's curvature, in pixels.
 */
void lv_arclabel_set_radius(lv_obj_t * obj, uint32_t radius);

/**
 * Установите смещение центра x для объекта метки дуги.
 * @param obj       pointer to an arc label object
 * @param x         the x offset
 */
void lv_arclabel_set_center_offset_x(lv_obj_t * obj, uint32_t x);

/**
 * Установите смещение центра y для объекта метки дуги.
 * @param obj       pointer to an arc label object
 * @param y         the y offset
 */
void lv_arclabel_set_center_offset_y(lv_obj_t * obj, uint32_t y);

/**
 * Установите вертикальное выравнивание текста для объекта метки дуги.
 * @param obj       pointer to an arc label object
 * @param align     the vertical alignment
 */
void lv_arclabel_set_text_vertical_align(lv_obj_t * obj, lv_arclabel_text_align_t align);

/**
 * Установите горизонтальное выравнивание текста для объекта метки дуги.
 * @param obj       pointer to an arc label object
 * @param align     the horizontal alignment
 */
void lv_arclabel_set_text_horizontal_align(lv_obj_t * obj, lv_arclabel_text_align_t align);

/**
 * Установите поведение переполнения для объекта метки дуги.
 * @param obj       pointer to an arc label object
 * @param overflow  the overflow mode (visible, ellipsis, clip)
 */
void lv_arclabel_set_overflow(lv_obj_t * obj, lv_arclabel_overflow_t overflow);

/**
 * Установите поведение конечного перекрытия для объекта метки дуги.
 * Это контролирует, как обрабатывается текст, когда он перекрывается в конце дуги в 360 градусов.
 * @param obj       pointer to an arc label object
 * @param overlap   set the arc label's end overlap behavior
 */
void lv_arclabel_set_end_overlap(lv_obj_t * obj, bool overlap);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получите начальный угол метки дуги.
 * @param obj       pointer to an arc label object
 * @return          the start angle [0..360]  (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 */
lv_value_precise_t lv_arclabel_get_angle_start(lv_obj_t * obj);

/**
 * Получите размер угла метки дуги.
 * @param obj       pointer to an arc label object
 * @return          the end angle [0..360]  (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 */
lv_value_precise_t lv_arclabel_get_angle_size(lv_obj_t * obj);

/**
 * Узнайте, является ли метка дуги типом или нет.
 * @param obj       pointer to an arc label object
 * @return          arc label's direction
 */
lv_arclabel_dir_t lv_arclabel_get_dir(const lv_obj_t * obj);

/**
 * Включите перекрашивание с помощью встроенных команд
 *
 * @see lv_arclabel_set_recolor
 *
 * @param obj       pointer to a label object
 * @return          true: enable recoloring, false: disable
 */
bool lv_arclabel_get_recolor(lv_obj_t * obj);

/**
 * Получите текст метки дуги.
 * @param obj       pointer to an arc label object
 * @return          the radius of the arc label
 */
uint32_t lv_arclabel_get_radius(lv_obj_t * obj);

/**
 * Получите смещение центра x для объекта метки дуги.
 * @param obj       pointer to an arc label object
 * @return          the x offset
 */
uint32_t lv_arclabel_get_center_offset_x(lv_obj_t * obj);

/**
 * Получите смещение центра y для объекта метки дуги.
 * @param obj       pointer to an arc label object
 * @return          the y offset
 */
uint32_t lv_arclabel_get_center_offset_y(lv_obj_t * obj);

/**
 * Получите вертикальное выравнивание текста для объекта метки дуги.
 * @param obj       pointer to an arc label object
 * @return          the vertical alignment
 */
lv_arclabel_text_align_t lv_arclabel_get_text_vertical_align(lv_obj_t * obj);

/**
 * Получите горизонтальное выравнивание текста для объекта метки дуги.
 * @param obj       pointer to an arc label object
 * @return          the horizontal alignment
 */
lv_arclabel_text_align_t lv_arclabel_get_text_horizontal_align(lv_obj_t * obj);

/**
 * Получите поведение переполнения для объекта метки дуги.
 * @param obj       pointer to an arc label object
 * @return          the overflow mode
 */
lv_arclabel_overflow_t lv_arclabel_get_overflow(lv_obj_t * obj);

/**
 * Получите поведение конечного перекрытия для объекта метки дуги.
 * @param obj       pointer to an arc label object
 * @return          the end overlap mode
 */
bool lv_arclabel_get_end_overlap(lv_obj_t * obj);

/**
 * Получите угол текста для объекта метки дуги.
 * @note            The text angle is calculated at runtime. You can get the updated value
 *                  после обновления размера arclabel.
 *                  Возвращает реальный угол отрисовки текста в градусах, за исключением
 *                  Режим `LV_ARCLABEL_OVERFLOW_VISIBLE`.
 * @param obj       pointer to an arc label object
 * @return          the text angle (if `LV_USE_FLOAT` is enabled it can be fractional too.)
 */
lv_value_precise_t lv_arclabel_get_text_angle(lv_obj_t * obj);

/*=====================
 * Другие функции
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_ARCLABEL*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_ARCLABEL_H*/
