/**
 * @file lv_scale.h
 *
 */

#ifndef LV_SCALE_H
#define LV_SCALE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_SCALE != 0

#include "../../core/lv_obj.h"
#include "../line/lv_line.h"
#include "../image/lv_image.h"
#include "../../core/lv_observer.h"

/*********************
 *      DEFINES
 *********************/

/**Default value of total minor ticks. */
#define LV_SCALE_TOTAL_TICK_COUNT_DEFAULT (11U)
LV_EXPORT_CONST_INT(LV_SCALE_TOTAL_TICK_COUNT_DEFAULT);

/**Default value of major tick every nth ticks. */
#define LV_SCALE_MAJOR_TICK_EVERY_DEFAULT (5U)
LV_EXPORT_CONST_INT(LV_SCALE_MAJOR_TICK_EVERY_DEFAULT);

/**Default value of scale label enabled. */
#define LV_SCALE_LABEL_ENABLED_DEFAULT (1U)
LV_EXPORT_CONST_INT(LV_SCALE_LABEL_ENABLED_DEFAULT);

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Режим масштабирования
 */
typedef enum {
    LV_SCALE_MODE_HORIZONTAL_TOP    = 0x00U,
    LV_SCALE_MODE_HORIZONTAL_BOTTOM = 0x01U,
    LV_SCALE_MODE_VERTICAL_LEFT     = 0x02U,
    LV_SCALE_MODE_VERTICAL_RIGHT    = 0x04U,
    LV_SCALE_MODE_ROUND_INNER       = 0x08U,
    LV_SCALE_MODE_ROUND_OUTER      = 0x10U,
    LV_SCALE_MODE_LAST
} lv_scale_mode_t;

#define LV_SCALE_LABEL_ROTATE_MATCH_TICKS  0x100000
LV_EXPORT_CONST_INT(LV_SCALE_LABEL_ROTATE_MATCH_TICKS);

#define LV_SCALE_LABEL_ROTATE_KEEP_UPRIGHT 0x80000
LV_EXPORT_CONST_INT(LV_SCALE_LABEL_ROTATE_KEEP_UPRIGHT);

#define LV_SCALE_ROTATION_ANGLE_MASK 0x7FFFF
LV_EXPORT_CONST_INT(LV_SCALE_ROTATION_ANGLE_MASK);

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_scale_class;

#if LV_USE_OBJ_PROPERTY
enum _lv_property_scale_id_t {
    LV_PROPERTY_ID(SCALE, MODE,               LV_PROPERTY_TYPE_INT,   0),
    LV_PROPERTY_ID(SCALE, TOTAL_TICK_COUNT,   LV_PROPERTY_TYPE_INT,   1),
    LV_PROPERTY_ID(SCALE, MAJOR_TICK_EVERY,   LV_PROPERTY_TYPE_INT,   2),
    LV_PROPERTY_ID(SCALE, LABEL_SHOW,         LV_PROPERTY_TYPE_BOOL,  3),
    LV_PROPERTY_ID(SCALE, ANGLE_RANGE,        LV_PROPERTY_TYPE_INT,   4),
    LV_PROPERTY_ID(SCALE, ROTATION,           LV_PROPERTY_TYPE_INT,   5),
    LV_PROPERTY_ID(SCALE, RANGE_MIN_VALUE,    LV_PROPERTY_TYPE_INT,   6),
    LV_PROPERTY_ID(SCALE, RANGE_MAX_VALUE,    LV_PROPERTY_TYPE_INT,   7),
    LV_PROPERTY_SCALE_END,
};
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте масштабный объект
 * @param parent    pointer to an object, it will be the parent of the new scale
 * @return          pointer to created Scale Widget
 */
lv_obj_t * lv_scale_create(lv_obj_t * parent);

/*======================
 * Добавить/удалить функции
 *=====================*/

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите режим масштабирования. См. lv_scale_mode_t.
 * @param obj       pointer to Scale Widget
 * @param mode      the new scale mode
 */
void lv_scale_set_mode(lv_obj_t * obj, lv_scale_mode_t mode);

/**
 * Установите общее количество тиков шкалы (включая второстепенные и основные тики).
 * @param obj       pointer to Scale Widget
 * @param total_tick_count    New total tick count
 */
void lv_scale_set_total_tick_count(lv_obj_t * obj, uint32_t total_tick_count);

/**
 * Устанавливает частоту рисования основных тиков.
 * @param obj                 pointer to Scale Widget
 * @param major_tick_every    the new count for major tick drawing
 */
void lv_scale_set_major_tick_every(lv_obj_t * obj, uint32_t major_tick_every);

/**
 * Устанавливает видимость метки.
 * @param obj           pointer to Scale Widget
 * @param show_label    true/false to enable tick label
 */
void lv_scale_set_label_show(lv_obj_t * obj, bool show_label);

/**
 * Установите минимальные и максимальные значения на шкале.
 * @param obj       pointer to Scale Widget
 * @param min       minimum value of Scale
 * @param max       maximum value of Scale
 */
void lv_scale_set_range(lv_obj_t * obj, int32_t min, int32_t max);

/**
 * Установите минимальные значения на шкале.
 * @param obj       pointer to Scale Widget
 * @param min       minimum value of Scale
 */
void lv_scale_set_min_value(lv_obj_t * obj, int32_t min);

/**
 * Установите максимальные значения на шкале.
 * @param obj       pointer to Scale Widget
 * @param min       minimum value of Scale
 */
void lv_scale_set_max_value(lv_obj_t * obj, int32_t max);

/**
 * Установите угол между нижним и верхним пределом шкалы.
 * (Применимо только к круглым весам.)
 * @param obj         pointer to Scale Widget
 * @param max_angle   angle in degrees from Scale minimum where top end of Scale will be drawn
 */
void lv_scale_set_angle_range(lv_obj_t * obj, uint32_t angle_range);

/**
 * Установите угловое смещение от 3-часового положения нижнего конца шкалы.
 * (Применимо только к круглым весам.)
 * @param obj       pointer to Scale Widget
 * @param rotation  clockwise angular offset (in degrees) from the 3-o'clock position
 *                  нижнего предела шкалы; отрицательные значения и значения >360 сначала нормализуются
 *                  в диапазоне [0..360].
 *                  Примеры:
 *                      -   0 = 3 o'clock (right side)
 *                      -  30 = 4 o'clock
 *                      -  60 = 5 o'clock
 *                      -  90 = 6 o'clock
 *                      - 135 = midway between 7 and 8 o'clock (default)
 *                      - 180 = 9 o'clock
 *                      - 270 = 12 o'clock
 *                      - 300 = 1 o'clock
 *                      - 330 = 2 o'clock
 *                      - -30 = 2 o'clock
 *                      - 390 = 4 o'clock
 */
void lv_scale_set_rotation(lv_obj_t * obj, int32_t rotation);

/**
 * Наведите иглу линии на указанное значение.
 * @param obj              pointer to Scale Widget
 * @param needle_line      needle_line of the Scale. The line points will be allocated and
 *                         управляется Масштабом, если ранее не был установлен массив точек линии
 *                         используя `lv_line_set_points_mutable`.
 * @param needle_length    length of the needle
 *                         - needle_length>0: needle_length=needle_length;
 *                         - needle_length<0: needle_length=radius-|needle_length|;
 * @param value            Scale value needle will point to
 */
void lv_scale_set_line_needle_value(lv_obj_t * obj, lv_obj_t * needle_line, int32_t needle_length,
                                    int32_t value);

/**
 * Наведите иглу изображения на указанное значение;
   изображение должно указывать вправо. Например. -О------>
 * @param obj              pointer to Scale Widget
 * @param needle_img       pointer to needle's Image
 * @param value            Scale value needle will point to
 */
void lv_scale_set_image_needle_value(lv_obj_t * obj, lv_obj_t * needle_img, int32_t value);

/**
 * Установите собственный источник текста для основных меток галочек.
 * @param obj       pointer to Scale Widget
 * @param txt_src   pointer to an array of strings which will be display at major ticks;
 *                  последний элемент должен быть указателем NULL.
 */
void lv_scale_set_text_src(lv_obj_t * obj, const char * txt_src[]);

/**
 * Нарисовать масштаб после того, как будут нарисованы все его дочерние элементы.
 * @param obj       pointer to Scale Widget
 * @param en        true: enable post draw
 */
void lv_scale_set_post_draw(lv_obj_t * obj, bool en);

/**
 * Нарисуйте галочки Scale поверх всех остальных частей.
 * @param obj       pointer to Scale Widget
 * @param en        true: enable draw ticks on top of all parts
 */
void lv_scale_set_draw_ticks_on_top(lv_obj_t * obj, bool en);

/**
 * Добавьте раздел к указанному масштабу.  Раздел не будет нарисован до тех пор, пока
 * для него устанавливается допустимый диапазон с помощью `lv_scale_set_section_range()`.
 * @param obj       pointer to Scale Widget
 * @return          pointer to new Section
 */
lv_scale_section_t * lv_scale_add_section(lv_obj_t * obj);

/**
 * DEPRECATED , вместо этого используйте lv_scale_set_section_range.
 * Установить диапазон для указанного раздела шкалы
 * @param section       pointer to Section
 * @param range_min     Section new minimum value
 * @param range_max     Section new maximum value
 */
void lv_scale_section_set_range(lv_scale_section_t * section, int32_t min, int32_t max);

/**
 * Установить диапазон раздела шкалы
 * @param scale         pointer to scale
 * @param section       pointer to section
 * @param range_min     the section's new minimum value
 * @param range_max     the section's new maximum value
 */
void lv_scale_set_section_range(lv_obj_t * scale, lv_scale_section_t * section, int32_t min, int32_t max);

/**
 * Установить минимальное значение сечения шкалы
 * @param scale         pointer to scale
 * @param section       pointer to section
 * @param min           the section's new minimum value
 */
void lv_scale_set_section_min_value(lv_obj_t * scale, lv_scale_section_t * section, int32_t min);

/**
 * Установить максимальное значение участка шкалы
 * @param scale         pointer to scale
 * @param section       pointer to section
 * @param max           the section's new maximum value
 */
void lv_scale_set_section_max_value(lv_obj_t * scale, lv_scale_section_t * section, int32_t max);

/**
 * DEPRECATED , вместо этого используйте lv_scale_set_section_style_main /indicator/items.
 * Установить стиль для указанной части раздела.
 * @param section             pointer to Section
 * @param part                the part of the Scale the style will apply to, e.g. LV_PART_INDICATOR
 * @param section_part_style  pointer to style to apply
 */
void lv_scale_section_set_style(lv_scale_section_t * section, lv_part_t part, lv_style_t * section_part_style);

/**
 * Установите стиль линии на участке.
 * @param scale         pointer to scale
 * @param section       pointer to section
 * @param style         point to a style
 */
void lv_scale_set_section_style_main(lv_obj_t * scale, lv_scale_section_t * section, const lv_style_t * style);

/**
 * Установите стиль основных отметок и меток раздела.
 * @param scale         pointer to scale
 * @param section       pointer to section
 * @param style         point to a style
 */
void lv_scale_set_section_style_indicator(lv_obj_t * scale, lv_scale_section_t * section, const lv_style_t * style);

/**
 * Установите стиль второстепенных отметок раздела.
 * @param scale         pointer to scale
 * @param section       pointer to section
 * @param style         point to a style
 */
void lv_scale_set_section_style_items(lv_obj_t * scale, lv_scale_section_t * section, const lv_style_t * style);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получить режим масштабирования. См. lv_scale_mode_t.
 * @param obj   pointer to Scale Widget
 * @return      Режим масштабирования
 */
lv_scale_mode_t lv_scale_get_mode(lv_obj_t * obj);

/**
 * Получить общее количество тиков шкалы (включая второстепенные и основные тики)
 * @param obj   pointer to Scale Widget
 * @return      Scale total tick count
 */
int32_t lv_scale_get_total_tick_count(lv_obj_t * obj);

/**
 * Узнайте, как часто будет выпадать основной тик
 * @param obj   pointer to Scale Widget
 * @return      Scale major tick every count
 */
int32_t lv_scale_get_major_tick_every(lv_obj_t * obj);

/**
 * Получите угловое расположение нижнего конца шкалы.
 * @param obj   pointer to Scale Widget
 * @return      Scale low end angular location
 */
int32_t lv_scale_get_rotation(lv_obj_t * obj);

/**
 * Получает видимость метки
 * @param obj   pointer to Scale Widget
 * @return      true if tick label is enabled, false otherwise
 */
bool lv_scale_get_label_show(lv_obj_t * obj);

/**
 * Получить диапазон шкалы в градусах
 * @param obj   pointer to Scale Widget
 * @return      Scale's angle_range
 */
uint32_t lv_scale_get_angle_range(lv_obj_t * obj);

/**
 * Получить минимальное значение для масштаба
 * @param obj   pointer to Scale Widget
 * @return      Scale's minimum value
 */
int32_t lv_scale_get_range_min_value(lv_obj_t * obj);

/**
 * Получите максимальное значение Scale
 * @param obj   pointer to Scale Widget
 * @return      Scale's maximum value
 */
int32_t lv_scale_get_range_max_value(lv_obj_t * obj);

/*=====================
 * Другие функции
 *====================*/

#if LV_USE_OBSERVER

/**
 * Привязать целое число к минимальному значению раздела весов
 * @param obj       pointer to a Scale
 * @param section   pointer to a Scale section
 * @param subject   pointer to a Subject
 * @return          pointer to newly-created Observer
 */
lv_observer_t * lv_scale_bind_section_min_value(lv_obj_t * obj, lv_scale_section_t * section, lv_subject_t * subject);

/**
 * Привязать целое число к максимальному значению раздела весов
 * @param obj       pointer to an Scale
 * @param section   pointer to a Scale section
 * @param subject   pointer to a Subject
 * @return          pointer to newly-created Observer
 */
lv_observer_t * lv_scale_bind_section_max_value(lv_obj_t * obj, lv_scale_section_t * section, lv_subject_t * subject);

#endif

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SCALE*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_SCALE_H*/
