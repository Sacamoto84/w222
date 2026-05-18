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
 * @param parent    pointer to an object, it will be the parent of the new spangroup
 * @return          pointer to the created spangroup
 */
lv_obj_t * lv_spangroup_create(lv_obj_t * parent);

/**
 * Создайте дескриптор строки диапазона и добавьте его в группу диапазона.
 * @param obj       pointer to a spangroup object.
 * @return          pointer to the created span.
 */
lv_span_t * lv_spangroup_add_span(lv_obj_t * obj);

/**
 * Удалите диапазон из группы диапазонов и освободите память.
 * @param obj   pointer to a spangroup object.
 * @param span  pointer to a span.
 * @note        Note that before calling `lv_spangroup_delete_span`
 *              `lv_observer_remove` необходимо вызывать вручную, поскольку LVGL не может удалить
 *              привязка автоматически.
 */
void lv_spangroup_delete_span(lv_obj_t * obj, lv_span_t * span);

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите новый текст для диапазона. Память будет выделена для хранения текста по диапазону.
 * Поскольку группа диапазона не передана, перерисовка (аннулирование) не может быть запущена автоматически.
 * Поэтому `lv_spangroup_refresh(spangroup)` необходимо вызывать вручную,
 * @param span  pointer to a span.
 * @param text  pointer to a text.
 */
void lv_span_set_text(lv_span_t * span, const char * text);


/**
 * Установите новый текст для диапазона, используя строку форматирования, подобную printf.
 * Память будет выделена для хранения текста по диапазону.
 * Поскольку группа диапазона не передана, перерисовка (аннулирование) не может быть запущена автоматически.
 * Поэтому `lv_spangroup_refresh(spangroup)` необходимо вызывать вручную,
 * @param span  pointer to a span.
 * @param fmt   `printf`-like format string
 */
void lv_span_set_text_fmt(lv_span_t * span, const char * fmt, ...) LV_FORMAT_ATTRIBUTE(2, 3);

/**
 * Установите статический текст. Он не будет сохранен в диапазоне, поэтому переменная text
 * должен быть «живым», пока существует диапазон.
 * Поскольку группа диапазона не передана, перерисовка (аннулирование) не может быть запущена автоматически.
 * Поэтому `lv_spangroup_refresh(spangroup)` необходимо вызывать вручную,
 *
 * @param span  pointer to a span.
 * @param text  pointer to a text.
 */
void lv_span_set_text_static(lv_span_t * span, const char * text);

/**
 * Установите новый текст для диапазона. Память будет выделена для хранения текста по диапазону.
 * @param obj   pointer to a spangroup widget.
 * @param span  pointer to a span.
 * @param text  pointer to a text.
 */
void lv_spangroup_set_span_text(lv_obj_t * obj, lv_span_t * span, const char * text);

/**
 * Установите новый текст для диапазона. Память будет выделена для хранения текста по диапазону.
 * @param obj   pointer to a spangroup widget.
 * @param span  pointer to a span.
 * @param text  pointer to a text.
 */
void lv_spangroup_set_span_text_static(lv_obj_t * obj, lv_span_t * span, const char * text);

/**
 * Установите новый текст для диапазона, используя строку форматирования, подобную printf.
 * Память будет выделена для хранения текста по диапазону.
 * @param obj   pointer to a spangroup widget.
 * @param span  pointer to a span.
 * @param fmt   `printf`-like format string
 */
void lv_spangroup_set_span_text_fmt(lv_obj_t * obj, lv_span_t * span, const char * fmt, ...) LV_FORMAT_ATTRIBUTE(3, 4);

/**
 * Установите статический текст. Он не будет сохранен в диапазоне, поэтому переменная text
 * должен быть «живым», пока существует диапазон.
 * @param span  pointer to a span.
 * @param text  pointer to a text.
 */
void lv_span_set_text_static(lv_span_t * span, const char * text);

/**
 * Скопируйте все свойства стиля во встроенный статический стиль диапазона b.
 * @param obj       pointer_to a spangroup
 * @param span      pointer to a span.
 * @param style     pointer to a style to copy into the span's built-in style
 */
void lv_spangroup_set_span_style(lv_obj_t * obj, lv_span_t * span, const lv_style_t * style);

/**
 * DEPRECATED . Вместо этого используйте свойство стиля text_align.
 * Установите выравнивание группы диапазонов.
 * @param obj   pointer to a spangroup object.
 * @param align see lv_text_align_t for details.
 */
void lv_spangroup_set_align(lv_obj_t * obj, lv_text_align_t align);

/**
 * Установите переполнение группы диапазонов.
 * @param obj       pointer to a spangroup object.
 * @param overflow  see lv_span_overflow_t for details.
 */
void lv_spangroup_set_overflow(lv_obj_t * obj, lv_span_overflow_t overflow);

/**
 * Установите отступ группы диапазона.
 * @param obj       pointer to a spangroup object.
 * @param indent    the first line indentation
 */
void lv_spangroup_set_indent(lv_obj_t * obj, int32_t indent);

/**
 * DEPRECATED , установите ширину LV_SIZE_CONTENT или фиксированное значение для управления расширением/обертыванием"
 * Установите режим группы диапазонов.
 * @param obj       pointer to a spangroup object.
 * @param mode      see lv_span_mode_t for details.
 */
void lv_spangroup_set_mode(lv_obj_t * obj, lv_span_mode_t mode);

/**
 * Установите максимальное количество строк в группе диапазонов.
 * @param obj       pointer to a spangroup object.
 * @param lines     max lines that can be displayed in LV_SPAN_MODE_BREAK mode. < 0 means no limit.
 */
void lv_spangroup_set_max_lines(lv_obj_t * obj, int32_t lines);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получите указатель на стиль встроенного стиля диапазона.
 * К возвращаемому стилю можно применять любые функции lv_style_set_ ....
 * @param span  pointer to the span
 * @return      pointer to the style. (valid as long as the span is valid)
 */
lv_style_t * lv_span_get_style(lv_span_t * span);

/**
 * Получить указатель на текст диапазона
 * @param span  pointer to the span
 * @return      pointer to the text
*/
const char * lv_span_get_text(lv_span_t * span);

/**
 * Получите дочернюю группу диапазона по ее индексу.
 *
 * @param obj   The spangroup object
 * @param id    the index of the child.
 *              0: the oldest (firstly created) child
 *              1: the second oldest
 *              количество детей-1: самый младший
 *              -1: самый младший
 *              -2: второй младший
 * @return      The child span at index `id`, or NULL if the ID does not exist
 */
lv_span_t * lv_spangroup_get_child(const lv_obj_t * obj, int32_t id);

/**
 * Получить количество пролетов
 * @param obj   the spangroup object to get the child count of.
 * @return      the span count of the spangroup.
 */
uint32_t lv_spangroup_get_span_count(const lv_obj_t * obj);

/**
 * Получите выравнивание группы диапазонов.
 * @param obj   pointer to a spangroup object.
 * @return      the align value.
 */
lv_text_align_t lv_spangroup_get_align(lv_obj_t * obj);

/**
 * Получите переполнение группы span.
 * @param obj   pointer to a spangroup object.
 * @return      the overflow value.
 */
lv_span_overflow_t lv_spangroup_get_overflow(lv_obj_t * obj);

/**
 * Получите отступ группы диапазона.
 * @param obj   pointer to a spangroup object.
 * @return      the indent value.
 */
int32_t lv_spangroup_get_indent(lv_obj_t * obj);

/**
 * Получите режим группы span.
 * @param obj   pointer to a spangroup object.
 */
lv_span_mode_t lv_spangroup_get_mode(lv_obj_t * obj);

/**
 * Получите максимальное количество строк группы span.
 * @param obj   pointer to a spangroup object.
 * @return      the max lines value.
 */
int32_t lv_spangroup_get_max_lines(lv_obj_t * obj);

/**
 * Получите максимальную высоту строки для всего диапазона в группе диапазонов.
 * @param obj   pointer to a spangroup object.
 */
int32_t lv_spangroup_get_max_line_height(lv_obj_t * obj);

/**
 * Получите ширину текстового содержимого, когда весь диапазон группы диапазонов находится в строке.
 * @param obj       pointer to a spangroup object.
 * @param max_width if text content width >= max_width, return max_width
 * чтобы сократить вычисления, если max_width == 0, возвращает ширину текстового содержимого.
 * @return text     content width or max_width.
 */
uint32_t lv_spangroup_get_expand_width(lv_obj_t * obj, uint32_t max_width);

/**
 * Получите высоту текстового содержимого с фиксированной шириной.
 * @param obj       pointer to a spangroup object.
 * @param width     the width of the span group.

 */
int32_t lv_spangroup_get_expand_height(lv_obj_t * obj, int32_t width);

/**
 * Получите координаты диапазона в группе диапазонов.
 * @note Before calling this function, please make sure that the layout of span group has been updated.
 * Подобно вызову функции lv_obj_update_layout().
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
 * @param obj       pointer to a spangroup object.
 * @param span      pointer to a span.
 * @return the span's coords in the spangroup.
 */
lv_span_coords_t lv_spangroup_get_span_coords(lv_obj_t * obj, const lv_span_t * span);

/**
 * Получите объект диапазона по точкам.
 * @param obj       pointer to a spangroup object.
 * @param point     pointer to point containing absolute coordinates
 * @return          pointer to the span under the point or `NULL` if not found.
 */
lv_span_t * lv_spangroup_get_span_by_point(lv_obj_t * obj, const lv_point_t * point);

/*=====================
 * Другие функции
 *====================*/

/**
 * Обновите режим группы span.
 * @param obj   pointer to a spangroup object.
 */
void lv_spangroup_refresh(lv_obj_t * obj);

#if LV_USE_OBSERVER

/**
 * Привяжите целое число, строку или указатель к диапазону Span-группы.
 * @param obj       pointer to Spangroup
 * @param span      pointer to Span
 * @param subject   pointer to Subject
 * @param fmt       optional printf-like format string with 1 format specifier (e.g. "%d °C")
 *                  или NULL для прямой привязки к значению.
 * @return          pointer to newly-created Observer
 * @note            If `fmt == NULL` strings and pointers (`\0` terminated string) will be shown
 *                  в виде текста, целые числа — %d, числа с плавающей запятой — %0.1f.
 */
lv_observer_t * lv_spangroup_bind_span_text(lv_obj_t * obj, lv_span_t * span, lv_subject_t * subject, const char * fmt);

#endif

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SPAN*/

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /*LV_SPAN_H*/
