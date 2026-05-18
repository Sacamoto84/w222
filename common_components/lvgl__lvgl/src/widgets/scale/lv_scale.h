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
 * @param parent    указатель на объект, он будет родителем нового масштаба
 * @return          указатель на созданный виджет масштабирования
 */
lv_obj_t * lv_scale_create(lv_obj_t * parent);

/*======================
 * Добавить/удалить функции
 *=====================*/

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите режимы масштабирования. См. lv_scale_mode_t.
 * @param obj       указатель на виджет масштабирования
 * @param mode      новый режим масштабирования
 */
void lv_scale_set_mode(lv_obj_t * obj, lv_scale_mode_t mode);

/**
 * Установите общее количество тиков шкалы (включая второстепенные и основные тики).
 * @param obj       указатель на виджет масштабирования
 * @param total_tick_count    Новое общее количество тиков
 */
void lv_scale_set_total_tick_count(lv_obj_t * obj, uint32_t total_tick_count);

/**
 * Устанавливает частоту рисования основных тиков.
 * @param obj                 указатель на виджет масштабирования
 * @param major_tick_every    новый счетчик для крупных тиков
 */
void lv_scale_set_major_tick_every(lv_obj_t * obj, uint32_t major_tick_every);

/**
 * Устанавливает видимость метки.
 * @param obj           указатель на виджет масштабирования
 * @param show_label    true/false, чтобы включить галочку
 */
void lv_scale_set_label_show(lv_obj_t * obj, bool show_label);

/**
 * Установите минимальные и максимальные значения на шкале.
 * @param obj       указатель на виджет масштабирования
 * @param min       минимальное значение шкалы
 * @param max       максимальное значение шкалы
 */
void lv_scale_set_range(lv_obj_t * obj, int32_t min, int32_t max);

/**
 * Установите минимальные значения на шкале.
 * @param obj       указатель на виджет масштабирования
 * @param min       минимальное значение шкалы
 */
void lv_scale_set_min_value(lv_obj_t * obj, int32_t min);

/**
 * Установите максимальные значения на шкале.
 * @param obj       указатель на виджет масштабирования
 * @param max       минимальное значение шкалы
 */
void lv_scale_set_max_value(lv_obj_t * obj, int32_t max);

/**
 * Установите угол между нижним и верхним пределом шкалы.
 * (Применимо только к круглым весам.)
 * @param obj         указатель на виджет масштабирования
 * @param angle_range   угол в градусах от минимума шкалы, где будет нарисован верхний конец шкалы
 */
void lv_scale_set_angle_range(lv_obj_t * obj, uint32_t angle_range);

/**
 * Установите угловое смещение от 3-часового положения нижнего конца шкалы.
 * (Применимо только к круглым весам.)
 * @param obj       указатель на виджет масштабирования
 * @param rotation  Угловое смещение по часовой стрелке (в градусах) от положения на 3 часа
 *                  нижнего предела шкалы; отрицательные значения и значения >360 сначала нормализуются
 *                  в диапазоне [0..360].
 *                  Примеры:
 *                      -   0 = 3 часа (правая сторона)
 *                      -  30 = 4 часа
 *                      -  60 = 5 часов
 *                      -  90 = 6 часов
 *                      - 135 = посередине между 7 и 8 часами (по умолчанию).
 *                      - 180 = 9 часов
 *                      - 270 = 12 часов
 *                      - 300 = 1 час
 *                      - 330 = 2 часа
 *                      - -30 = 2 часа
 *                      - 390 = 4 часа
 */
void lv_scale_set_rotation(lv_obj_t * obj, int32_t rotation);

/**
 * Наведите иглу линии на указанное значение.
 * @param obj              указатель на виджет масштабирования
 * @param needle_line      needle_line Масштаба. Точки линии будут выделены и
 *                         управляется Масштабом, если ранее не был установлен массив точек линии
 *                         с помощью `lv_line_set_points_mutable`.
 * @param needle_length    длина иглы
 *                         - needle_length >0:needle_length=needle_length;
 *                         - needle_length <0:needle_length=радиус-|needle_length|;
 * @param value            Стрелка значения шкалы будет указывать на
 */
void lv_scale_set_line_needle_value(lv_obj_t * obj, lv_obj_t * needle_line, int32_t needle_length,
                                    int32_t value);

/**
 * Наведите иглу изображения на указанное значение;
   изображение должно указывать вправо. Например. -О------>
 * @param obj              указатель на виджет масштабирования
 * @param needle_img       указатель на изображение иглы
 * @param value            Стрелка значения шкалы будет указывать на
 */
void lv_scale_set_image_needle_value(lv_obj_t * obj, lv_obj_t * needle_img, int32_t value);

/**
 * Установите собственный источник текста для основных меток галочек.
 * @param obj       указатель на виджет масштабирования
 * @param txt_src   указатель на массив строк, который будет отображаться на основных тиках;
 *                  последний элемент должен быть указателем NULL.
 */
void lv_scale_set_text_src(lv_obj_t * obj, const char * txt_src[]);

/**
 * Нарисовать масштаб после того, как будут нарисованы все его дочерние элементы.
 * @param obj       указатель на виджет масштабирования
 * @param en        true: включить отрисовку сообщения
 */
void lv_scale_set_post_draw(lv_obj_t * obj, bool en);

/**
 * Нарисуйте галочки. Масштабируйте поверхность всех остальных частей.
 * @param obj       указатель на виджет масштабирования
 * @param en        true: включить отрисовку меток поверх всех частей
 */
void lv_scale_set_draw_ticks_on_top(lv_obj_t * obj, bool en);

/**
 * Добавьте раздел к указанному масштабу.  Раздел не будет нарисован до тех пор, пока
 * для него предусмотрен допустимый диапазон с помощью `lv_scale_set_section_range()`.
 * @param obj       указатель на виджет масштабирования
 * @return          указатель на новый раздел
 */
lv_scale_section_t * lv_scale_add_section(lv_obj_t * obj);

/**
 * DEPRECATED, вместо этого викорируйте lv_scale_set_section_range.
 * Установить диапазон для указанного раздела шкалы
 * @param section       указатель на раздел
 * @param min     Новое минимальное значение раздела
 * @param max     Новое максимальное значение раздела
 */
void lv_scale_section_set_range(lv_scale_section_t * section, int32_t min, int32_t max);

/**
 * Установить диапазон раздела шкалы
 * @param scale         указатель масштаба
 * @param section       указатель на раздел
 * @param min     новое минимальное значение раздела
 * @param max     новое максимальное значение секции
 */
void lv_scale_set_section_range(lv_obj_t * scale, lv_scale_section_t * section, int32_t min, int32_t max);

/**
 * Установить минимальное значение сечения шкалы
 * @param scale         указатель масштаба
 * @param section       указатель на раздел
 * @param min           новое минимальное значение раздела
 */
void lv_scale_set_section_min_value(lv_obj_t * scale, lv_scale_section_t * section, int32_t min);

/**
 * Установить максимальное значение участка шкалы
 * @param scale         указатель масштаба
 * @param section       указатель на раздел
 * @param max           новое максимальное значение секции
 */
void lv_scale_set_section_max_value(lv_obj_t * scale, lv_scale_section_t * section, int32_t max);

/**
 * DEPRECATED , вместо этого воспользуйтесьlv_scale_set_section_style_main/indicator/items.
 * Установить стиль для указанной части раздела.
 * @param section             указатель на раздел
 * @param part                часть шкалы, к которой будет применен стиль, например.  LV_PART_INDICATOR
 * @param section_part_style  указатель на стиль, который нужно применить
 */
void lv_scale_section_set_style(lv_scale_section_t * section, lv_part_t part, lv_style_t * section_part_style);

/**
 * Установите стиль линии на участке.
 * @param scale         указатель масштаба
 * @param section       указатель на раздел
 * @param style         указать на стиль
 */
void lv_scale_set_section_style_main(lv_obj_t * scale, lv_scale_section_t * section, const lv_style_t * style);

/**
 * Установите стиль основных отметок и меток раздела.
 * @param scale         указатель масштаба
 * @param section       указатель на раздел
 * @param style         указать на стиль
 */
void lv_scale_set_section_style_indicator(lv_obj_t * scale, lv_scale_section_t * section, const lv_style_t * style);

/**
 * Установите стиль второстепенных отметок раздела.
 * @param scale         указатель масштаба
 * @param section       указатель на раздел
 * @param style         указать на стиль
 */
void lv_scale_set_section_style_items(lv_obj_t * scale, lv_scale_section_t * section, const lv_style_t * style);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получить режимы масштабирования. См. lv_scale_mode_t.
 * @param obj   указатель на виджет масштабирования
 * @return      Режим масштабирования
 */
lv_scale_mode_t lv_scale_get_mode(lv_obj_t * obj);

/**
 * Получить общее количество тиков шкалы (включая второстепенные и основные тики)
 * @param obj   указатель на виджет масштабирования
 * @return      Масштабировать общее количество тиков
 */
int32_t lv_scale_get_total_tick_count(lv_obj_t * obj);

/**
 * Узнайте, как часто будет выпадать основной тик
 * @param obj   указатель на виджет масштабирования
 * @return      Масштабируйте главный тик при каждом отсчете
 */
int32_t lv_scale_get_major_tick_every(lv_obj_t * obj);

/**
 * Получите угловое расположение нижнего конца шкалы.
 * @param obj   указатель на виджет масштабирования
 * @return      Масштабируйте нижнее угловое положение
 */
int32_t lv_scale_get_rotation(lv_obj_t * obj);

/**
 * Получает видимость метки
 * @param obj   указатель на виджет масштабирования
 * @return      true, если галочка включена, в противном случае — false
 */
bool lv_scale_get_label_show(lv_obj_t * obj);

/**
 * Получить диапазон шкалы в градусах
 * @param obj   указатель на виджет масштабирования
 * @return      Весы angle_range
 */
uint32_t lv_scale_get_angle_range(lv_obj_t * obj);

/**
 * Получить минимальное значение для масштаба
 * @param obj   указатель на виджет масштабирования
 * @return      Минимальное значение шкалы
 */
int32_t lv_scale_get_range_min_value(lv_obj_t * obj);

/**
 * Получите максимальное значение Масштаб
 * @param obj   указатель на виджет масштабирования
 * @return      Максимальное значение шкалы
 */
int32_t lv_scale_get_range_max_value(lv_obj_t * obj);

/*=====================
 * Другие функции
 *====================*/

#if LV_USE_OBSERVER

/**
 * Привязать целое число к минимальному значению раздела весов
 * @param obj       указатель на шкалу
 * @param section   указатель на раздел «Масштаб»
 * @param subject   указатель на тему
 * @return          указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_scale_bind_section_min_value(lv_obj_t * obj, lv_scale_section_t * section, lv_subject_t * subject);

/**
 * Привязать целое число к максимальному значению раздела весов
 * @param obj       указатель на шкалу
 * @param section   указатель на раздел «Масштаб»
 * @param subject   указатель на тему
 * @return          указатель на вновь созданный наблюдатель
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
