/**
 * @file lv_span.h
 *
 */

#ifndef LV_SPAN_H
#define LV_SPAN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#include "../../core/lv_obj.h"
#include "../../core/lv_observer.h"
#include "../../core/lv_obj_property.h"

#if LV_USE_SPAN != 0

/*********************
 *      DEFINES
 *********************/
#ifndef LV_SPAN_SNIPPET_STACK_SIZE
#define LV_SPAN_SNIPPET_STACK_SIZE 64
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_SPAN_OVERFLOW_CLIP,
    LV_SPAN_OVERFLOW_ELLIPSIS,
    LV_SPAN_OVERFLOW_LAST,  /**< Fence member*/
} lv_span_overflow_t;

typedef enum {
    LV_SPAN_MODE_FIXED,     /**< fixed the obj size */
    LV_SPAN_MODE_EXPAND,    /**< Expand the object size to the text size */
    LV_SPAN_MODE_BREAK,     /**< Keep width, break the too long lines and expand height */
    LV_SPAN_MODE_LAST       /**< Fence member */
} lv_span_mode_t;

/** Координаты пролета */
typedef struct _lv_span_coords_t {
    lv_area_t heading;
    lv_area_t middle;
    lv_area_t trailing;
} lv_span_coords_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_spangroup_class;

#if LV_USE_OBJ_PROPERTY
enum _lv_property_span_id_t {
    LV_PROPERTY_ID(SPAN, ALIGN,     LV_PROPERTY_TYPE_INT, 0),
    LV_PROPERTY_ID(SPAN, OVERFLOW,  LV_PROPERTY_TYPE_INT, 1),
    LV_PROPERTY_ID(SPAN, INDENT,    LV_PROPERTY_TYPE_INT, 2),
    LV_PROPERTY_ID(SPAN, MODE,      LV_PROPERTY_TYPE_INT, 3),
    LV_PROPERTY_ID(SPAN, MAX_LINES, LV_PROPERTY_TYPE_INT, 4),
    LV_PROPERTY_SPAN_END,
};
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_span_stack_init(void);
void lv_span_stack_deinit(void);

/**
 * Создать объект группы диапазонов
 * @param parent    указатель на объект, он будет родительским элементом новой группы объектов
 * @return          указатель на созданную группу спанов
 */
lv_obj_t * lv_spangroup_create(lv_obj_t * parent);

/**
 * Создайте дескриптор строки диапазона и добавьте его в группу диапазона.
 * @param obj       указатель на объект группы диапазонов.
 * @return          указатель на созданный диапазон.
 */
lv_span_t * lv_spangroup_add_span(lv_obj_t * obj);

/**
 * Удалите диапазон из группы диапазонов и освободите память.
 * @param obj   указатель на объект группы диапазонов.
 * @param span  указатель на диапазон.
 * @note        Обратите внимание, что перед вызовом `lv_spangroup_delete_span`
 *              `lv_observer_remove` необходимо обрабатывать вручную, посколькуLVGLне может удалить
 *              привязка автоматически.
 */
void lv_spangroup_delete_span(lv_obj_t * obj, lv_span_t * span);

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите новый текст для диапазона. Память будет выделена для хранения текста по диапазону.
 * Поскольку группа диапазона не передана, перерисовка (аннулирование) не может быть запущена автоматически.
 * Поэтому`lv_spangroup_refresh(spangroup)`необходимо лечить вручную,
 * @param span  указатель на диапазон.
 * @param text  указатель на текст.
 */
void lv_span_set_text(lv_span_t * span, const char * text);


/**
 * Установите новый текст для настройки, используя текстовый формат, аналогичный printf.
 * Память будет выделена для хранения текста по диапазону.
 * Поскольку группа диапазона не передана, перерисовка (аннулирование) не может быть запущена автоматически.
 * Поэтому`lv_spangroup_refresh(spangroup)`необходимо лечить вручную,
 * @param span  указатель на диапазон.
 * @param fmt   Строка формата, подобная `printf`
 */
void lv_span_set_text_fmt(lv_span_t * span, const char * fmt, ...) LV_FORMAT_ATTRIBUTE(2, 3);

/**
 * Установите статический текст. Он не будет сохранен в составе, поэтому переменный текст
 * должен быть «живым», пока существует диапазон.
 * Поскольку группа диапазона не передана, перерисовка (аннулирование) не может быть запущена автоматически.
 * Поэтому`lv_spangroup_refresh(spangroup)`необходимо лечить вручную,
 *
 * @param span  указатель на диапазон.
 * @param text  указатель на текст.
 */
void lv_span_set_text_static(lv_span_t * span, const char * text);

/**
 * Установите новый текст для диапазона. Память будет выделена для хранения текста по диапазону.
 * @param obj   указатель на виджет группы диапазонов.
 * @param span  указатель на диапазон.
 * @param text  указатель на текст.
 */
void lv_spangroup_set_span_text(lv_obj_t * obj, lv_span_t * span, const char * text);

/**
 * Установите новый текст для диапазона. Память будет выделена для хранения текста по диапазону.
 * @param obj   указатель на виджет группы диапазонов.
 * @param span  указатель на диапазон.
 * @param text  указатель на текст.
 */
void lv_spangroup_set_span_text_static(lv_obj_t * obj, lv_span_t * span, const char * text);

/**
 * Установите новый текст для настройки, используя текстовый формат, аналогичный printf.
 * Память будет выделена для хранения текста по диапазону.
 * @param obj   указатель на виджет группы диапазонов.
 * @param span  указатель на диапазон.
 * @param fmt   Строка формата, подобная `printf`
 */
void lv_spangroup_set_span_text_fmt(lv_obj_t * obj, lv_span_t * span, const char * fmt, ...) LV_FORMAT_ATTRIBUTE(3, 4);

/**
 * Установите статический текст. Он не будет сохранен в составе, поэтому переменный текст
 * должен быть «живым», пока существует диапазон.
 * @param span  указатель на диапазон.
 * @param text  указатель на текст.
 */
void lv_span_set_text_static(lv_span_t * span, const char * text);

/**
 * Скопируйте все свойства стиля во встроенный статический стиль диапазона b.
 * @param obj       pointer_to группа диапазонов
 * @param span      указатель на диапазон.
 * @param style     указатель на стиль для копирования во встроенный стиль диапазона
 */
void lv_spangroup_set_span_style(lv_obj_t * obj, lv_span_t * span, const lv_style_t * style);

/**
 * DEPRECATED . Вместо этого воспользуйтесь свойством стиля text_align.
 * Установите выравнивание группы диапазонов.
 * @param obj   указатель на объект группы диапазонов.
 * @param align подробности см. в lv_text_align_t.
 */
void lv_spangroup_set_align(lv_obj_t * obj, lv_text_align_t align);

/**
 * Установите переполнение группы диапазонов.
 * @param obj       указатель на объект группы диапазонов.
 * @param overflow  подробности см. в lv_span_overflow_t.
 */
void lv_spangroup_set_overflow(lv_obj_t * obj, lv_span_overflow_t overflow);

/**
 * Установите отступ группы диапазона.
 * @param obj       указатель на объект группы диапазонов.
 * @param indent    отступ первой строки
 */
void lv_spangroup_set_indent(lv_obj_t * obj, int32_t indent);

/**
 * DEPRECATED , установите ширину LV_SIZE_CONTENT или фиксированное значение для управления расширением/обертыванием"
 * Установите режим группы диапазонов.
 * @param obj       указатель на объект группы диапазонов.
 * @param mode      подробности см. в lv_span_mode_t.
 */
void lv_spangroup_set_mode(lv_obj_t * obj, lv_span_mode_t mode);

/**
 * Установите максимальное количество строк в группе диапазонов.
 * @param obj       указатель на объект группы диапазонов.
 * @param lines     Максимальное количество строк, которые могут отображаться в режиме LV_SPAN_MODE_BREAK. < 0 означает отсутствие ограничений.
 */
void lv_spangroup_set_max_lines(lv_obj_t * obj, int32_t lines);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получите указатель на стиль встроенного стиля диапазона.
 * К возвращающемуся любому стилю можно применить функциюlv_style_set_....
 * @param span  указатель на диапазон
 * @return      указатель на стиль. (действителен до тех пор, пока действителен диапазон)
 */
lv_style_t * lv_span_get_style(lv_span_t * span);

/**
 * Получить указатель на текст диапазона
 * @param span  указатель на диапазон
 * @return      указатель на текст
*/
const char * lv_span_get_text(lv_span_t * span);

/**
 * Получите дочернюю группу диапазона по ее индексу.
 *
 * @param obj   Объект группы диапазонов
 * @param id    индекс ребенка.
 *              0: самый старший (первый созданный) ребенок
 *              1: второй по возрасту
 *              количество детей-1: самый младший
 *              -1: самый младший
 *              -2: второй младший
 * @return      Дочерний диапазон по индексу`id`или NULL, еслиIDне существует.
 */
lv_span_t * lv_spangroup_get_child(const lv_obj_t * obj, int32_t id);

/**
 * Получить количество пролетов
 * @param obj   объект группы диапазонов, для которого нужно получить количество дочерних элементов.
 * @return      количество промежутков в группе промежутков.
 */
uint32_t lv_spangroup_get_span_count(const lv_obj_t * obj);

/**
 * Получите выравнивание группы диапазонов.
 * @param obj   указатель на объект группы диапазонов.
 * @return      значение выравнивания.
 */
lv_text_align_t lv_spangroup_get_align(lv_obj_t * obj);

/**
 * Получите переполнение группы span.
 * @param obj   указатель на объект группы диапазонов.
 * @return      значение переполнения.
 */
lv_span_overflow_t lv_spangroup_get_overflow(lv_obj_t * obj);

/**
 * Получите отступ группы диапазона.
 * @param obj   указатель на объект группы диапазонов.
 * @return      значение отступа.
 */
int32_t lv_spangroup_get_indent(lv_obj_t * obj);

/**
 * Получите режим группы.
 * @param obj   указатель на объект группы диапазонов.
 */
lv_span_mode_t lv_spangroup_get_mode(lv_obj_t * obj);

/**
 * Получите максимальное количество строк в группе.
 * @param obj   указатель на объект группы диапазонов.
 * @return      максимальное значение строк.
 */
int32_t lv_spangroup_get_max_lines(lv_obj_t * obj);

/**
 * Получите максимальную высоту строки для всего диапазона в группе диапазонов.
 * @param obj   указатель на объект группы диапазонов.
 */
int32_t lv_spangroup_get_max_line_height(lv_obj_t * obj);

/**
 * Получите ширину текстового содержимого, когда весь диапазон группы диапазонов находится в строке.
 * @param obj       указатель на объект группы диапазонов.
 * @param max_width если ширина текстового содержимого >= max_width, верните max_width
 * для сокращения расходов, еслиmax_width== 0, необходимо вернуть резерв текстового значения.
 * @return ширина текстового содержимого илиmax_width.
 */
uint32_t lv_spangroup_get_expand_width(lv_obj_t * obj, uint32_t max_width);

/**
 * Получите высоту текстового содержимого с фиксированной шириной.
 * @param obj       указатель на объект группы диапазонов.
 * @param width     ширина группы пролетов.

 */
int32_t lv_spangroup_get_expand_height(lv_obj_t * obj, int32_t width);

/**
 * Получите координаты диапазона в группе диапазонов.
 * @note Прежде чем вызывать эту функцию, убедитесь, что макет группы интервалов обновлен.
 * Аналогичный вызов функции lv_obj_update_layout().
 *
 *     +--------+
 *     |Heading +--->------------------+
 *     |  Пос |   |     Рубрика |
 *     +--------+---+------------------+
 *     |                               |
 *     |                               |
 *     |                               |
 *     |            Средний +--------+|
 *     |                     |Конечный||
 *     |                   +-|  Поз ||
 *     |                   | +--------+|
 *     +-------------------v-----------+
 *     |     Трейлинг |
 *     +-------------------+
 * @param obj       указатель на объект группы диапазонов.
 * @param span      указатель на диапазон.
 * @return координаты диапазона в группе диапазона.
 */
lv_span_coords_t lv_spangroup_get_span_coords(lv_obj_t * obj, const lv_span_t * span);

/**
 * Получите объект диапазона по точкам.
 * @param obj       указатель на объект группы диапазонов.
 * @param point     указатель на точку, содержащую абсолютные координаты
 * @return          указатель на диапазон под точкой или `NULL`, если не найден.
 */
lv_span_t * lv_spangroup_get_span_by_point(lv_obj_t * obj, const lv_point_t * point);

/*=====================
 * Другие функции
 *====================*/

/**
 * Обновите режим группы.
 * @param obj   указатель на объект группы диапазонов.
 */
void lv_spangroup_refresh(lv_obj_t * obj);

#if LV_USE_OBSERVER

/**
 * Привяжите число, символ или указатель к диапазону Span-групп.
 * @param obj       указатель на Spangroup
 * @param span      указатель на диапазон
 * @param subject   указатель на тему
 * @param fmt       необязательная строка формата в формате printf с 1 спецификатором формата (например, «%d °C»)
 *                  или NULL для прямой привязки к значению.
 * @return          указатель на вновь созданный наблюдатель
 * @note            Если будут показаны строки и указатели`fmt == NULL`(завершаемая строка `\0`)
 *                  в виде текста, целые числа — %d, числа с плавающей запятой — %0.1f.
 */
lv_observer_t * lv_spangroup_bind_span_text(lv_obj_t * obj, lv_span_t * span, lv_subject_t * subject, const char * fmt);

#endif

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SPAN*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SPAN_H*/
