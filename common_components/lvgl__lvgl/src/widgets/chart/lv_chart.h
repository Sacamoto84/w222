/**
 * @file lv_chart.h
 *
 */

#ifndef LV_CHART_H
#define LV_CHART_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#include "../../core/lv_obj.h"

#if LV_USE_CHART != 0

/*********************
 *      DEFINES
 *********************/

/**Default value of points. Can be used to not draw a point*/
#define LV_CHART_POINT_NONE     (INT32_MAX)
LV_EXPORT_CONST_INT(LV_CHART_POINT_NONE);

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Типы диаграмм
 */
typedef enum {
    LV_CHART_TYPE_NONE,     /**< Don't draw the series*/
    LV_CHART_TYPE_LINE,     /**< Connect the points with lines*/
    LV_CHART_TYPE_CURVE,    /**< Connect the points with curves*/
    LV_CHART_TYPE_BAR,      /**< Draw bars for each series*/
    LV_CHART_TYPE_STACKED,  /**< Draw a single stacked bar for each data point. Supports only positive values*/
    LV_CHART_TYPE_SCATTER,  /**< Draw points and lines in 2D (x,y coordinates)*/
} lv_chart_type_t;

/**
 * Режим обновления графики для `lv_chart_set_next`
 */
typedef enum {
    LV_CHART_UPDATE_MODE_SHIFT,     /**< Shift old data to the left and add the new one the right*/
    LV_CHART_UPDATE_MODE_CIRCULAR,  /**< Add the new data in a circular way*/
} lv_chart_update_mode_t;

/**
 * Нумерация осей'
 */
typedef enum {
    LV_CHART_AXIS_PRIMARY_Y     = 0x00,
    LV_CHART_AXIS_SECONDARY_Y   = 0x01,
    LV_CHART_AXIS_PRIMARY_X     = 0x02,
    LV_CHART_AXIS_SECONDARY_X   = 0x04,
    LV_CHART_AXIS_LAST
} lv_chart_axis_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_chart_class;

#if LV_USE_OBJ_PROPERTY
enum _lv_property_chart_id_t {
    LV_PROPERTY_ID(CHART, TYPE,               LV_PROPERTY_TYPE_INT,   0),
    LV_PROPERTY_ID(CHART, POINT_COUNT,        LV_PROPERTY_TYPE_INT,   1),
    LV_PROPERTY_ID(CHART, UPDATE_MODE,        LV_PROPERTY_TYPE_INT,   2),
    LV_PROPERTY_ID(CHART, HOR_DIV_LINE_COUNT, LV_PROPERTY_TYPE_INT,   3),
    LV_PROPERTY_ID(CHART, VER_DIV_LINE_COUNT, LV_PROPERTY_TYPE_INT,   4),
    LV_PROPERTY_CHART_END,
};
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создание объекта диаграммы
 * @param parent    указатель на объект, он будет родительским элементом новой диаграммы
 * @return          указатель на созданный график
 */
lv_obj_t * lv_chart_create(lv_obj_t * parent);

/**
 * Установить новый тип диаграммы
 * @param obj       указатель на объект диаграммы
 * @param type      новый тип диаграммы (из перечисления 'lv_chart_type_t')
 */
void lv_chart_set_type(lv_obj_t * obj, lv_chart_type_t type);
/**
 * Установите количество точек на линии данных на диаграмме
 * @param obj       указатель на объект диаграммы
 * @param cnt       новое количество точек на линиях данных
 */
void lv_chart_set_point_count(lv_obj_t * obj, uint32_t cnt);

/**
 * Установите минимальное и максимальное значения y на оси
 * @param obj       указатель на объект диаграммы
 * @param axis      `LV_CHART_AXIS_PRIMARY_Y` или `LV_CHART_AXIS_SECONDARY_Y`
 * @param min       минимальное значение оси Y
 * @param max       максимальное значение оси Y
 */
void lv_chart_set_axis_range(lv_obj_t * obj, lv_chart_axis_t axis, int32_t min, int32_t max);

/**
 * Установите минимальные значения на оси
 * @param obj       указатель на объект диаграммы
 * @param axis      `LV_CHART_AXIS_PRIMARY_Y` или `LV_CHART_AXIS_SECONDARY_Y`
 * @param min       минимальное значение оси Y
 */
void lv_chart_set_axis_min_value(lv_obj_t * obj, lv_chart_axis_t axis, int32_t min);

/**
 * Установите максимальные значения y на оси
 * @param obj       указатель на объект диаграммы
 * @param axis      `LV_CHART_AXIS_PRIMARY_Y` или `LV_CHART_AXIS_SECONDARY_Y`
 * @param max       максимальное значение оси Y
 */
void lv_chart_set_axis_max_value(lv_obj_t * obj, lv_chart_axis_t axis, int32_t max);


/**
 * Установите режим обновления объекта диаграммы. влияет
 * @param obj              указатель на объект диаграммы
 * @param update_mode      режим обновления
 */
void lv_chart_set_update_mode(lv_obj_t * obj, lv_chart_update_mode_t update_mode);

/**
 * Установите количество горизонтальных и вертикальных линий разделения.
 * @param obj       указатель на объект диаграммы
 * @param hdiv      количество горизонтальных разделительных линий
 * @param vdiv      количество вертикальных разделительных линий
 */
void lv_chart_set_div_line_count(lv_obj_t * obj, uint32_t hdiv, uint32_t vdiv);

/**
 * Установите количество горизонтальных линий разделения.
 * @param obj       указатель на объект диаграммы
 * @param cnt       количество горизонтальных разделительных линий
 */
void lv_chart_set_hor_div_line_count(lv_obj_t * obj, uint32_t cnt);

/**
 * Установите количество вертикальных линий разделения.
 * @param obj       указатель на объект диаграммы
 * @param cnt       количество вертикальных разделительных линий
 */
void lv_chart_set_ver_div_line_count(lv_obj_t * obj, uint32_t cnt);

/**
 * Получить тип диаграммы
 * @param obj       указатель на объект диаграммы
 * @return          тип диаграммы (из перечисления 'lv_chart_t')
 */
lv_chart_type_t lv_chart_get_type(const lv_obj_t * obj);

/**
 * Получите номер точки данных для каждой строки данных на диаграмме.
 * @param obj       указатель на объект диаграммы
 * @return          номер точки в каждой строке данных
 */
uint32_t lv_chart_get_point_count(const lv_obj_t * obj);

/**
 * Получить режим обновления диаграммы
 * @param obj       указатель на объект диаграммы
 * @return          режим обновления
 */
lv_chart_update_mode_t lv_chart_get_update_mode(const lv_obj_t * obj);

/**
 * Получить количество горизонтальных линий разделения
 * @param obj       указатель на объект диаграммы
 * @return          количество горизонтальных разделительных линий
 */
uint32_t lv_chart_get_hor_div_line_count(const lv_obj_t * obj);

/**
 * Получить количество вертикальных линий разделения
 * @param obj       указатель на объект диаграммы
 * @return          количество вертикальных разделительных линий
 */
uint32_t lv_chart_get_ver_div_line_count(const lv_obj_t * obj);

/**
 * Получите текущий индекс начальной точки оси X в массиве данных.
 * @param obj       указатель на объект диаграммы
 * @param ser       указатель на ряд данных на «диаграмме»
 * @return          индекс текущей начальной точки x в массиве данных
 */
uint32_t lv_chart_get_x_start_point(const lv_obj_t * obj, lv_chart_series_t * ser);

/**
 * Получить положение точки на графике.
 * @param obj       указатель на объект диаграммы
 * @param ser       указатель на серию
 * @param id        индекс.
 * @param p_out     сохраните позицию результата здесь
 */
void lv_chart_get_point_pos_by_id(lv_obj_t * obj, lv_chart_series_t * ser, uint32_t id, lv_point_t * p_out);

/**
 * Обновите диаграмму, если ее строка данных изменилась.
 * @param   obj   указатель на объект диаграммы
 */
void lv_chart_refresh(lv_obj_t * obj);

/*======================
 * Серия
 *=====================*/

/**
 * Выделение и добавление ряда данных на диаграмму
 * @param obj       указатель на объект диаграммы
 * @param color     цвет ряда данных
 * @param axis      ось y, к которой должна быть привязана серия (::LV_CHART_AXIS_PRIMARY_Y или ::LV_CHART_AXIS_SECONDARY_Y)
 * @return          указатель на выделенный ряд данных илиNULLв случае сбоя
 */
lv_chart_series_t * lv_chart_add_series(lv_obj_t * obj, lv_color_t color, lv_chart_axis_t axis);

/**
 * Освобождение и удаление ряда данных из диаграммы
 * @param obj       указатель на объект диаграммы
 * @param series    указатель на ряд данных на «диаграмме»
 */
void lv_chart_remove_series(lv_obj_t * obj, lv_chart_series_t * series);

/**
 * Скрыть/показать одну серию диаграммы.
 * @param chart     указатель на объект диаграммы.
 * @param series    указатель на объект серии
 * @param hide      правда: скрыть серию
 */
void lv_chart_hide_series(lv_obj_t * chart, lv_chart_series_t * series, bool hide);

/**
 * Изменение цвета серии
 * @param chart     указатель на объект диаграммы.
 * @param series    указатель на объект серии
 * @param color     новый цвет серии
 */
void lv_chart_set_series_color(lv_obj_t * chart, lv_chart_series_t * series, lv_color_t color);

/**
 * Получить цвет серии
 * @param chart     указатель на объект диаграммы.
 * @param series    указатель на объект серии
 * @return          цвет сериала
 */
lv_color_t lv_chart_get_series_color(lv_obj_t * chart, const lv_chart_series_t * series);

/**
 * Установите индекс начальной точки оси X в массиве данных.
 * Эта точка будет считаться первой (левой) точкой, а остальные точки будут рисоваться после нее.
 * @param obj       указатель на объект диаграммы
 * @param ser       указатель на ряд данных на «диаграмме»
 * @param id        индекс точки x в массиве данных
 */
void lv_chart_set_x_start_point(lv_obj_t * obj, lv_chart_series_t * ser, uint32_t id);

/**
 * Берите следующую серию.
 * @param chart     указатель на график
 * @param ser      предыдущую серию или NULL, чтобы получить первую
 * @return          следующая серия или NULL, если больше нет.
 */
lv_chart_series_t * lv_chart_get_series_next(const lv_obj_t * chart, const lv_chart_series_t * ser);

/*=====================
 * Курсор
 *====================*/

/**
 * Добавить курсор заданного цвета
 * @param obj       указатель на объект диаграммы
 * @param color     цвет курсора
 * @param dir       направление курсора. `LV_DIR_RIGHT/LEFT/TOP/DOWN/HOR/VER/ALL`.  Возможны значенияOR-ed
 * @return          указатель на созданный курсор
 */
lv_chart_cursor_t  * lv_chart_add_cursor(lv_obj_t * obj, lv_color_t color, lv_dir_t dir);

/**
 * Удалить курсор
 * @param obj       указатель на объект диаграммы
 * @param cursor    указатель на курсор
 */
void lv_chart_remove_cursor(lv_obj_t * obj, lv_chart_cursor_t * cursor);

/**
 * Установите координату курсора относительно полей
 * @param chart     указатель на объект диаграммы
 * @param cursor    указатель на курсор
 * @param pos       новая координата курсора относительно графика
 */
void lv_chart_set_cursor_pos(lv_obj_t * chart, lv_chart_cursor_t * cursor, lv_point_t * pos);

/**
 * Установите координату X курсора относительно полей.
 * @param chart     указатель на объект диаграммы
 * @param cursor    указатель на курсор
 * @param x         новая координата X курсора относительно диаграммы
 */
void lv_chart_set_cursor_pos_x(lv_obj_t * chart, lv_chart_cursor_t * cursor, int32_t x);

/**
 * Установите координату курсора относительно полей
 * @param chart     указатель на объект диаграммы
 * @param cursor    указатель на курсор
 * @param y         новая координата Y курсора относительно диаграммы
 */
void lv_chart_set_cursor_pos_y(lv_obj_t * chart, lv_chart_cursor_t * cursor, int32_t y);

/**
 * Наведите курсор на точку
 * @param chart     указатель на объект диаграммы
 * @param cursor    указатель на курсор
 * @param ser       указатель на серию
 * @param point_id  индекс точки или `LV_CHART_POINT_NONE`, чтобы не назначать ее ни одной точке.
 */
void lv_chart_set_cursor_point(lv_obj_t * chart, lv_chart_cursor_t * cursor, lv_chart_series_t * ser,
                               uint32_t point_id);

/**
 * Получить координату курсора относительно полей
 * @param chart     указатель на объект диаграммы
 * @param cursor    указатель на курсор
 * @return          координата курсора как lv_point_t
 */
lv_point_t lv_chart_get_cursor_point(lv_obj_t * chart, lv_chart_cursor_t * cursor);

/*=====================
 * Установить/получить значение(я)
 *====================*/

/**
 * Инициализировать все точки данных серии со значением
 * @param obj       указатель на объект диаграммы
 * @param ser       указатель на ряд данных на «диаграмме»
 * @param value     новое значение для всех точек. `LV_CHART_POINT_NONE`можно использовать, чтобы скрыть точки.
 */
void lv_chart_set_all_values(lv_obj_t * obj, lv_chart_series_t * ser, int32_t value);

/**
 * Установите значение Y следующей точки в соответствии с политикой режима обновления.
 * @param obj       указатель на объект диаграммы
 * @param ser       указатель на ряд данных на «диаграмме»
 * @param value     новое значение следующих данных
 */
void lv_chart_set_next_value(lv_obj_t * obj, lv_chart_series_t * ser, int32_t value);

/**
 * Установите значения X и Y следующей точки в соответствии с политикой режима обновления.
 * @param obj       указатель на объект диаграммы
 * @param ser       указатель на ряд данных на «диаграмме»
 * @param x_value   новое значение X следующих данных
 * @param y_value   новое значение Y следующих данных
 */
void lv_chart_set_next_value2(lv_obj_t * obj, lv_chart_series_t * ser, int32_t x_value, int32_t y_value);

/**
 * То же самое, что`lv_chart_set_next_value`, но устанавливает значения из массива.
 * @param obj           указатель на объект диаграммы
 * @param ser           указатель на ряд данных на «диаграмме»
 * @param values        новые значения для установки
 * @param values_cnt    количество элементов в `values`
 */
void lv_chart_set_series_values(lv_obj_t * obj, lv_chart_series_t * ser, const int32_t values[], size_t values_cnt);

/**
 * То же самое, что`lv_chart_set_next_value2`, но устанавливает значения из массива.
 * @param obj           указатель на объект диаграммы
 * @param ser           указатель на ряд данных на «диаграмме»
 * @param x_values      новые значения для установки на оси X
 * @param y_values      новые значения для установки по оси Y
 * @param values_cnt    количество элементов в`x_values`и `y_values`
 */
void lv_chart_set_series_values2(lv_obj_t * obj, lv_chart_series_t * ser, const int32_t x_values[],
                                 const int32_t y_values[], size_t values_cnt);

/**
 * Установите значение y отдельной точки серии диаграммы непосредственно на основе ее индекса.
 * @param obj     указатель на объект диаграммы
 * @param ser     указатель на ряд данных на «диаграмме»
 * @param id      индекс точки x в массиве
 * @param value   значение, которое нужно присвоить точке массива
 */
void lv_chart_set_series_value_by_id(lv_obj_t * obj, lv_chart_series_t * ser, uint32_t id, int32_t value);

/**
 * Установите значение x и y отдельной точки серии диаграммы непосредственно на основе ее индекса.
 * Может использоваться только с `LV_CHART_TYPE_SCATTER`.
 * @param obj       указатель на объект диаграммы
 * @param ser       указатель на ряд данных на «диаграмме»
 * @param id        индекс точки x в массиве
 * @param x_value   новое значение X следующих данных
 * @param y_value   новое значение Y следующих данных
 */
void lv_chart_set_series_value_by_id2(lv_obj_t * obj, lv_chart_series_t * ser, uint32_t id, int32_t x_value,
                                      int32_t y_value);

/**
 * Установите внешний массив для точек данных y, которые будут использоваться для диаграммы.
 * NOTE: Пользователь несет ответственность за то, чтобы`point_cnt`соответствовал размеру внешнего массива.
 * @param obj       указатель на объект диаграммы
 * @param ser       указатель на ряд данных на «диаграмме»
 * @param array     внешний массив точек для графика
 */
void lv_chart_set_series_ext_y_array(lv_obj_t * obj, lv_chart_series_t * ser, int32_t array[]);

/**
 * Установите внешний массив для точек данных x, которые будут использоваться для диаграммы.
 * NOTE: Пользователь несет ответственность за то, чтобы`point_cnt`соответствовал размеру внешнего массива.
 * @param obj       указатель на объект диаграммы
 * @param ser       указатель на ряд данных на «диаграмме»
 * @param array     внешний массив точек для графика
 */
void lv_chart_set_series_ext_x_array(lv_obj_t * obj, lv_chart_series_t * ser, int32_t array[]);

/**
 * Получить массив значений y серии
 * @param obj   указатель на объект диаграммы
 * @param ser   указатель на ряд данных на «диаграмме»
 * @return      массив значений с элементами 'point_count'
 */
int32_t * lv_chart_get_series_y_array(const lv_obj_t * obj, lv_chart_series_t * ser);

/**
 * Получить массив значений x серии
 * @param obj   указатель на объект диаграммы
 * @param ser   указатель на ряд данных на «диаграмме»
 * @return      массив значений с элементами 'point_count'
 */
int32_t * lv_chart_get_series_x_array(const lv_obj_t * obj, lv_chart_series_t * ser);

/**
 * Получить индекс текущей нажатой точки. И так для каждой серии.
 * @param obj       указатель на объект диаграммы
 * @return          индекс точки [0 .. количество точек] или LV_CHART_POINT_ID_NONE, если ни одна точка не нажимается
 */
uint32_t lv_chart_get_pressed_point(const lv_obj_t * obj);

/**
 * Получите общее смещение от края диаграммы до центра первой точки.
 * В случае гистограммы это будет центр первой группы столбцов.
 * @param obj       указатель на объект диаграммы
 * @return          смещение центра
 */
int32_t lv_chart_get_first_point_center_offset(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CHART*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_CHART_H*/
