/**
 * @file lv_draw_vector.h
 *
 */

#ifndef LV_DRAW_VECTOR_H
#define LV_DRAW_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_array.h"
#include "../misc/lv_matrix.h"
#include "lv_draw_image.h"

#if LV_USE_VECTOR_GRAPHIC

#if !LV_USE_MATRIX
#error "lv_draw_vector needs LV_USE_MATRIX = 1"
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_VECTOR_FILL_NONZERO = 0,
    LV_VECTOR_FILL_EVENODD,
} lv_vector_fill_t;

typedef enum {
    LV_VECTOR_STROKE_CAP_BUTT = 0,
    LV_VECTOR_STROKE_CAP_SQUARE,
    LV_VECTOR_STROKE_CAP_ROUND,
} lv_vector_stroke_cap_t;

typedef enum {
    LV_VECTOR_STROKE_JOIN_MITER = 0,
    LV_VECTOR_STROKE_JOIN_BEVEL,
    LV_VECTOR_STROKE_JOIN_ROUND,
} lv_vector_stroke_join_t;

typedef enum {
    LV_VECTOR_PATH_QUALITY_MEDIUM = 0, /* по умолчанию*/
    LV_VECTOR_PATH_QUALITY_HIGH,
    LV_VECTOR_PATH_QUALITY_LOW,
} lv_vector_path_quality_t;

typedef enum {
    LV_VECTOR_BLEND_SRC_OVER = 0,
    LV_VECTOR_BLEND_SRC_IN,
    LV_VECTOR_BLEND_DST_OVER,
    LV_VECTOR_BLEND_DST_IN,
    LV_VECTOR_BLEND_SCREEN,
    LV_VECTOR_BLEND_MULTIPLY,
    LV_VECTOR_BLEND_NONE,
    LV_VECTOR_BLEND_ADDITIVE,
    LV_VECTOR_BLEND_SUBTRACTIVE,
} lv_vector_blend_t;

typedef enum {
    LV_VECTOR_PATH_OP_MOVE_TO = 0,
    LV_VECTOR_PATH_OP_LINE_TO,
    LV_VECTOR_PATH_OP_QUAD_TO,
    LV_VECTOR_PATH_OP_CUBIC_TO,
    LV_VECTOR_PATH_OP_CLOSE,
} lv_vector_path_op_t;

typedef enum {
    LV_VECTOR_DRAW_STYLE_SOLID = 0,
    LV_VECTOR_DRAW_STYLE_PATTERN,
    LV_VECTOR_DRAW_STYLE_GRADIENT,
} lv_vector_draw_style_t;

typedef enum {
    LV_VECTOR_GRADIENT_SPREAD_PAD = 0,
    LV_VECTOR_GRADIENT_SPREAD_REPEAT,
    LV_VECTOR_GRADIENT_SPREAD_REFLECT,
} lv_vector_gradient_spread_t;

typedef enum {
    LV_VECTOR_GRADIENT_STYLE_LINEAR = 0,
    LV_VECTOR_GRADIENT_STYLE_RADIAL,
} lv_vector_gradient_style_t;

typedef enum {
    LV_VECTOR_FILL_UNITS_OBJECT_BOUNDING_BOX = 0, /** Относительные координаты относительно ограничивающей рамки объекта. */
    LV_VECTOR_FILL_UNITS_USER_SPACE_ON_USE, /** Абсолютные координаты относительно системы координат слоя. */
} lv_vector_fill_units_t;

struct _lv_fpoint_t {
    float x;
    float y;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Преобразуйте координаты точки, используя заданную матрицу
 * @param matrix           указатель на матрицу
 * @param point            указатель на точку
 */
void lv_matrix_transform_point(const lv_matrix_t * matrix, lv_fpoint_t * point);

/**
 * Преобразуйте все координаты пути, используя заданную матрицу
 * @param matrix           указатель на матрицу
 * @param path             указатель на путь
 */
void lv_matrix_transform_path(const lv_matrix_t * matrix, lv_vector_path_t * path);

/**
 * Создайте объект векторного графического контура
 * @param quality       Качественный намек на путь
 * @return              указатель на созданный объект пути
 */
lv_vector_path_t * lv_vector_path_create(lv_vector_path_quality_t quality);

/**
 * Скопируйте данные пути в другой
 * @param target_path       указатель на путь
 * @param path              указатель на исходный путь
 */
void lv_vector_path_copy(lv_vector_path_t * target_path, const lv_vector_path_t * path);

/**
 * Очистить данные пути
 * @param path              указатель на путь
 */
void lv_vector_path_clear(lv_vector_path_t * path);

/**
 * Удалить объект графического пути
 * @param path              указатель на путь
 */
void lv_vector_path_delete(lv_vector_path_t * path);

/**
 * Начните новый подпуть и установите точку на пути.
 * @param path              указатель на путь
 * @param p                 указатель на переменную `lv_fpoint_t`
 */
void lv_vector_path_move_to(lv_vector_path_t * path, const lv_fpoint_t * p);

/**
 * Добавьте линию к пути от последней точки до точки
 * @param path              указатель на путь
 * @param p                 указатель на переменную `lv_fpoint_t`
 */
void lv_vector_path_line_to(lv_vector_path_t * path, const lv_fpoint_t * p);

/**
 * Добавьте квадратичную линию Безье к пути от последней точки до точки.
 * @param path              указатель на путь
 * @param p1                указатель на переменную`lv_fpoint_t`для контрольной точки
 * @param p2                указатель на переменную`lv_fpoint_t`для конечной точки
 */
void lv_vector_path_quad_to(lv_vector_path_t * path, const lv_fpoint_t * p1, const lv_fpoint_t * p2);

/**
 * Добавьте кубическую линию Безье к пути от последней точки до точки.
 * @param path              указатель на путь
 * @param p1                указатель на переменную`lv_fpoint_t`для первой контрольной точки
 * @param p2                указатель на переменную`lv_fpoint_t`для второй контрольной точки
 * @param p3                указатель на переменную`lv_fpoint_t`для конечной точки
 */
void lv_vector_path_cubic_to(lv_vector_path_t * path, const lv_fpoint_t * p1, const lv_fpoint_t * p2,
                             const lv_fpoint_t * p3);

/**
 * Добавить дугу эллипса к пути от последней точки до точки
 * @param path              указатель на путь
 * @param radius_x          радиус x для дуги эллипса
 * @param radius_y          радиус y для дуги эллипса
 * @param rotate_angle      угол поворота дуги
 * @param large_arc         верно для большой дуги, в противном случае — для маленькой
 * @param clockwise         верно для по часовой стрелке, в противном случае против часовой стрелки
 * @param p                 указатель на переменную`lv_fpoint_t`для конечной точки
 */
void lv_vector_path_arc_to(lv_vector_path_t * path, float radius_x, float radius_y, float rotate_angle,
                           bool large_arc,
                           bool clockwise, const lv_fpoint_t * p);

/**
 * Закрыть дополнительный путь
 * @param path              указатель на путь
 */
void lv_vector_path_close(lv_vector_path_t * path);

/**
 * Получить ограничивающую рамку пути
 * @param path              указатель на путь
 * @param area              указатель на переменную`lv_area_t`для ограничивающей рамки
 */
void lv_vector_path_get_bounding(const lv_vector_path_t * path, lv_area_t * area);

/**
 * Добавьте прямоугольник к пути по x/y/w/h. rx/ry — угловые радиусы
 * @param path              указатель на путь
 * @param x                 координата x верхнего левого угла прямоугольника
 * @param y                 координата y верхнего левого угла прямоугольника
 * @param w                 ширина прямоугольника
 * @param h                 высота прямоугольника
 * @param rx                горизонтальный радиус для скругленного прямоугольника
 * @param ry                вертикальный радиус для скругленного прямоугольника
 */
void lv_vector_path_append_rectangle(lv_vector_path_t * path, float x, float y, float w, float h, float rx, float ry);

/**
 * Добавьте вариант к пути (устаревший API, вместо этого рекомендуется использовать lv_vector_path_append_rectangle)
 * @param path              указатель на путь
 * @param rect              указатель на переменную `lv_area_t`
 * @param rx                горизонтальный радиус для скругленного прямоугольника
 * @param ry                вертикальный радиус для скругленного прямоугольника
 */
static inline void lv_vector_path_append_rect(lv_vector_path_t * path, const lv_area_t * rect, float rx, float ry)
{
    LV_ASSERT_NULL(path);
    LV_ASSERT_NULL(rect);

    lv_vector_path_append_rectangle(path, rect->x1, rect->y1, (float)lv_area_get_width(rect),
                                    (float)lv_area_get_height(rect), rx, ry);
}

/**
 * Добавьте круг к пути
 * @param path              указатель на путь
 * @param c                 указатель на переменную`lv_fpoint_t`для центра круга
 * @param rx                горизонтальный радиус круга
 * @param ry                вертикальный радиус круга
 */
void lv_vector_path_append_circle(lv_vector_path_t * path, const lv_fpoint_t * c, float rx, float ry);

/**
 * Добавьте дугу к пути
 * @param path              указатель на путь
 * @param c                 указатель на переменную`lv_fpoint_t`для центра круга
 * @param radius            радиус дуги
 * @param start_angle       начальный угол дуги
 * @param sweep             угол поворота дуги может быть отрицательным
 * @param pie               true: нарисовать круг, false: нарисовать дугу
 */
void lv_vector_path_append_arc(lv_vector_path_t * path, const lv_fpoint_t * c, float radius, float start_angle,
                               float sweep, bool pie);

/**
 * Добавьте дополнительный путь к пути
 * @param path              указатель на путь
 * @param subpath           указатель на другой путь, который будет добавлен
 */
void lv_vector_path_append_path(lv_vector_path_t * path, const lv_vector_path_t * subpath);

/**
 * Создайте векторный графический дескриптор
 * @param layer         указатель на слой
 * @return              указатель на созданный дескриптор
 */
lv_draw_vector_dsc_t * lv_draw_vector_dsc_create(lv_layer_t * layer);

/**
 * Удалить дескриптор векторной графики
 * @param dsc              указатель на дескриптор векторной графики
 */
void lv_draw_vector_dsc_delete(lv_draw_vector_dsc_t * dsc);

/**
 * Установите матрицу в качестве текущей матрицы преобразования.
 * Новые формы путей, добавленные `lv_draw_vector_dsc_add_path`, будут использовать эту матрицу.
 * @param dsc              указатель на дескриптор векторной графики
 * @param matrix           указатель на матрицу
 */
void lv_draw_vector_dsc_set_transform(lv_draw_vector_dsc_t * dsc, const lv_matrix_t * matrix);

/**
 * Установите режим наложения для дескриптора.
 * Новые формы контуров, добавленные`lv_draw_vector_dsc_add_path`, будут использовать этот режим приложения.
 * @param dsc              указатель на дескриптор векторной графики
 * @param blend            режим наложения, который будет установлен в `lv_vector_blend_t`
 */
void lv_draw_vector_dsc_set_blend_mode(lv_draw_vector_dsc_t * dsc, lv_vector_blend_t blend);

/**
 * Установите цвет заливки для дескриптора.
 * Новые формы способов, добавленные `lv_draw_vector_dsc_add_path`, будут использовать этот цвет.
 * @param dsc              указатель на дескриптор векторной графики
 * @param color            цвет, который нужно установить в формате lv_color32_t
 */
void lv_draw_vector_dsc_set_fill_color32(lv_draw_vector_dsc_t * dsc, lv_color32_t color);

/**
 * Установите цвет заливки для дескриптора.
 * Новые формы способов, добавленные `lv_draw_vector_dsc_add_path`, будут использовать этот цвет.
 * @param dsc              указатель на дескриптор векторной графики
 * @param color            цвет, который нужно установить в формате lv_color_t
 */
void lv_draw_vector_dsc_set_fill_color(lv_draw_vector_dsc_t * dsc, lv_color_t color);

/**
 * Установите непрозрачность заливки для дескриптора.
 * Новые формы путей, добавленные`lv_draw_vector_dsc_add_path`, будут использовать эту непрозрачность.
 * @param dsc              указатель на дескриптор векторной графики
 * @param opa              непрозрачность должна быть установлена в формате lv_opa_t
 */
void lv_draw_vector_dsc_set_fill_opa(lv_draw_vector_dsc_t * dsc, lv_opa_t opa);

/**
 * Установите правило заполнения для дескриптора.
 * Новые формы контуров, добавленные`lv_draw_vector_dsc_add_path`, будут использовать это правило заливки.
 * @param dsc              указатель на дескриптор векторной графики
 * @param rule             правило заполнения задается в формате lv_vector_fill_t
 */
void lv_draw_vector_dsc_set_fill_rule(lv_draw_vector_dsc_t * dsc, lv_vector_fill_t rule);

/**
 * Установите единицы заполнения для дескриптора.
 * Новые формы контуров, добавленные `lv_draw_vector_dsc_add_path`, будут использоваться в этих заливах.
 * @param dsc              указатель на дескриптор векторной графики
 * @param units            единицы измерения должны быть установлены в формате lv_vector_fill_units_t
 * @note Единицы могут быть либо относительными к ограничивающей рамке объекта, либо абсолютными в пользовательском пространстве.
 *       Этот API конкретно влияет на положение изображения заливки и не влияет на другие элементы.
 */
void lv_draw_vector_dsc_set_fill_units(lv_draw_vector_dsc_t * dsc, const lv_vector_fill_units_t units);

/**
 * Установите изображение заливки для дескриптора.
 * Новые формы контуров, добавленные`lv_draw_vector_dsc_add_path`, будут использовать это изображение заливки.
 * @param dsc              указатель на дескриптор векторной графики
 * @param img_dsc          указатель на переменную `lv_draw_image_dsc_t`
 */
void lv_draw_vector_dsc_set_fill_image(lv_draw_vector_dsc_t * dsc, const lv_draw_image_dsc_t * img_dsc);

/**
 * Установить линейный градиент заливки для дескриптора
 * Новые формы, добавленные `lv_draw_vector_dsc_add_path`, будут использовать этот градиент.
 * @param dsc указатель на дескриптор векторной графики
 * @param x1 x для начальной точки
 * @param y1 y для начальной точки
 * @param x2 x для конечной точки
 * @param y2 y для конечной точки
 */
void lv_draw_vector_dsc_set_fill_linear_gradient(lv_draw_vector_dsc_t * dsc, float x1, float y1, float x2, float y2);

/**

 * Установить радиус радиального градиента заливки для дескриптора
 * Новые формы, добавленные `lv_draw_vector_dsc_add_path`, будут использовать этот градиент.
 * @param dsc указатель на дескриптор векторной графики
 * @param cx x означает центр круга
 * @param cy y для центра круга
 * @param radius радиус круга
 */
void lv_draw_vector_dsc_set_fill_radial_gradient(lv_draw_vector_dsc_t * dsc, float cx, float cy, float radius);

/**
 * Установить распространение радиального градиента заливки для дескриптора
 * @param dsc указатель на дескриптор векторной графики
 * @param spread распространение градиента должно быть установлено в формате lv_vector_gradient_spread_t
 */
void lv_draw_vector_dsc_set_fill_gradient_spread(lv_draw_vector_dsc_t * dsc, lv_vector_gradient_spread_t spread);

/**
 * Установите ограничители цвета градиента заливки для дескриптора.
 * Новые формы пути, добавленные `lv_draw_vector_dsc_add_path`, будут использовать остановку этого цвета градиента.
 * @param dsc              указатель на дескриптор векторной графики
 * @param stops            массив переменных `lv_grad_stop_t`
 * @param count            количество остановок в массиве, диапазон: 0.. LV_GRADIENT_MAX_STOPS
 */
void lv_draw_vector_dsc_set_fill_gradient_color_stops(lv_draw_vector_dsc_t * dsc, const lv_grad_stop_t * stops,
                                                      uint16_t count);

/**
 * Установите матрицу для текущей матрицы преобразования заполнения
 * Новые формы путей, добавленные `lv_draw_vector_dsc_add_path`, будут использовать эту матрицу.
 * @param dsc              указатель на дескриптор векторной графики
 * @param matrix           указатель на матрицу
 */
void lv_draw_vector_dsc_set_fill_transform(lv_draw_vector_dsc_t * dsc, const lv_matrix_t * matrix);

/**
 * Установите цвет обводки для дескриптора.
 * Новые формы способов, добавленные `lv_draw_vector_dsc_add_path`, будут использовать этот цвет.
 * @param dsc              указатель на дескриптор векторной графики
 * @param color            цвет, который нужно установить в формате lv_color32_t
 */
void lv_draw_vector_dsc_set_stroke_color32(lv_draw_vector_dsc_t * dsc, lv_color32_t color);

/**
 * Установите цвет обводки для дескриптора.
 * Новые формы способов, добавленные `lv_draw_vector_dsc_add_path`, будут использовать этот цвет.
 * @param dsc              указатель на дескриптор векторной графики
 * @param color            цвет, который нужно установить в формате lv_color_t
 */
void lv_draw_vector_dsc_set_stroke_color(lv_draw_vector_dsc_t * dsc, lv_color_t color);

/**
 * Установить непрозрачность обводки для дескриптора
 * @param dsc              указатель на дескриптор векторной графики
 * @param opa              непрозрачность должна быть установлена в формате lv_opa_t
 */
void lv_draw_vector_dsc_set_stroke_opa(lv_draw_vector_dsc_t * dsc, lv_opa_t opa);

/**
 * Установите толщину линии обводки для дескриптора.
 * Новые формы контуров, добавленные`lv_draw_vector_dsc_add_path`, будут использовать эти дополнительные обводки.
 * @param dsc              указатель на дескриптор векторной графики
 * @param width            ширина линии штриха
 */
void lv_draw_vector_dsc_set_stroke_width(lv_draw_vector_dsc_t * dsc, float width);

/**
 * Установить образец штриховой линии для дескриптора
 * Новые формы путей, добавленные `lv_draw_vector_dsc_add_path`, будут использовать эту шину.
 * @param dsc              указатель на дескриптор векторной графики
 * @param dash_pattern     массив значений, определяющих сегменты пунктирной линии
 * @param dash_count       длина массива штриховых шаблонов
 */
void lv_draw_vector_dsc_set_stroke_dash(lv_draw_vector_dsc_t * dsc, float * dash_pattern, uint16_t dash_count);

/**
 * Установить стиль окончания линии штриха для дескриптора
 * Новые формы пути, добавленные `lv_draw_vector_dsc_add_path`, будут использовать эту конечную линию.
 * @param dsc              указатель на дескриптор векторной графики
 * @param cap              ограничение строки должно быть установлено в формате lv_vector_stroke_cap_t
 */
void lv_draw_vector_dsc_set_stroke_cap(lv_draw_vector_dsc_t * dsc, lv_vector_stroke_cap_t cap);

/**
 * Установить стиль соединения линий штриха для дескриптора
 * @param dsc              указатель на дескриптор векторной графики
 * @param join             соединение линий должно быть установлено в формате lv_vector_stroke_join_t
 */
void lv_draw_vector_dsc_set_stroke_join(lv_draw_vector_dsc_t * dsc, lv_vector_stroke_join_t join);

/**
 * Установить предел угла хода для дескриптора
 * Новые формы траекторий, добавленные`lv_draw_vector_dsc_add_path`, будут использовать этот предел среза.
 * @param dsc              указатель на дескриптор векторной графики
 * @param miter_limit      инсульт miter_limit
 */
void lv_draw_vector_dsc_set_stroke_miter_limit(lv_draw_vector_dsc_t * dsc, uint16_t miter_limit);

/**
 * Установить линейный градиент штриха для дескриптора
 * Новые формы, добавленные `lv_draw_vector_dsc_add_path`, будут использовать этот градиент.
 * @param dsc              указатель на дескриптор векторной графики
 * @param x1               x для начальной точки
 * @param y1               y для начальной точки
 * @param x2               x для конечной точки
 * @param y2               y для конечной точки
 */
void lv_draw_vector_dsc_set_stroke_linear_gradient(lv_draw_vector_dsc_t * dsc, float x1, float y1, float x2, float y2);
/**
 * Установить радиальный градиент обводки для дескриптора
 * Новые формы, добавленные `lv_draw_vector_dsc_add_path`, будут использовать этот градиент.
 * @param dsc              указатель на дескриптор векторной графики
 * @param cx               x означает центр круга
 * @param cy               y для центра круга
 * @param radius           радиус круга
 */
void lv_draw_vector_dsc_set_stroke_radial_gradient(lv_draw_vector_dsc_t * dsc, float cx, float cy, float radius);

/**
 * Установить ограничители цвета обводки для дескриптора
 * Новые формы, добавленные `lv_draw_vector_dsc_add_path`, будут использовать этот градиент.
 * @param dsc              указатель на дескриптор векторной графики
 * @param spread           распространение градиента должно быть установлено в формате lv_vector_gradient_spread_t
 */
void lv_draw_vector_dsc_set_stroke_gradient_spread(lv_draw_vector_dsc_t * dsc, lv_vector_gradient_spread_t spread);

/**
 * Установить ограничители цвета обводки для дескриптора
 * Новые формы пути, добавленные `lv_draw_vector_dsc_add_path`, будут использовать остановку этого цвета.
 * @param dsc              указатель на дескриптор векторной графики
 * @param stops            массив переменных `lv_grad_stop_t`
 * @param count            количество остановок в массиве
 */
void lv_draw_vector_dsc_set_stroke_gradient_color_stops(lv_draw_vector_dsc_t * dsc, const lv_grad_stop_t * stops,
                                                        uint16_t count);

/**
 * Установите матрицу в качестве текущей матрицы преобразования штрихов.
 * Новые формы путей, добавленные `lv_draw_vector_dsc_add_path`, будут использовать эту матрицу.
 * @param dsc              указатель на дескриптор векторной графики
 * @param matrix           указатель на матрицу
 */
void lv_draw_vector_dsc_set_stroke_transform(lv_draw_vector_dsc_t * dsc, const lv_matrix_t * matrix);

/**
 * Установите текущую матрицу преобразования в единичную матрицу
 * Новые формы путей, добавленные `lv_draw_vector_dsc_add_path`, будут использовать эту матрицу.
 * @param dsc           указатель на дескриптор векторной графики
 */
void lv_draw_vector_dsc_identity(lv_draw_vector_dsc_t * dsc);

/**
 * Измените масштабный коэффициент текущей матрицы преобразования
 * Новые формы, добавленные `lv_draw_vector_dsc_add_path`, будут использовать этот масштаб.
 * @param dsc           указатель на дескриптор векторной графики
 * @param scale_x       масштабный коэффициент для направления X
 * @param scale_y       масштабный коэффициент для направления Y
 */
void lv_draw_vector_dsc_scale(lv_draw_vector_dsc_t * dsc, float scale_x, float scale_y);

/**
 * Поворот текущей матрицы преобразования с началом координат
 * Новые формы пути, добавленные `lv_draw_vector_dsc_add_path`, будут использоваться в обращении.
 * @param dsc           указатель на дескриптор векторной графики
 * @param degree        угол для поворота
 */
void lv_draw_vector_dsc_rotate(lv_draw_vector_dsc_t * dsc, float degree);

/**
 * Перевести текущую матрицу преобразования в новую позицию
 * Новые формы пути, добавленные `lv_draw_vector_dsc_add_path`, будут использоваться в обращении.
 * @param dsc           указатель на дескриптор векторной графики
 * @param tx            величина перевода в направлении x
 * @param ty            сумма перевода в направлении Y
 */
void lv_draw_vector_dsc_translate(lv_draw_vector_dsc_t * dsc, float tx, float ty);

/**
 * Измените коэффициент перекоса текущей матрицы преобразования
 * Новые формы пути, добавленные `lv_draw_vector_dsc_add_path`, будут использовать этот наклон.
 * @param dsc           указатель на дескриптор векторной графики
 * @param skew_x        коэффициент перекоса для направления x
 * @param skew_y        коэффициент перекоса для направления y
 */
void lv_draw_vector_dsc_skew(lv_draw_vector_dsc_t * dsc, float skew_x, float skew_y);

/**
 * Добавьте графический путь в список прорисовки.
 * Он будет использовать цвета, непрозрачность, матрицу и другие установленные параметры.
 * с помощью`lv_draw_vector_dsc_set_fill_color()`и других функций.
 * @param dsc           указатель на дескриптор векторной графики
 * @param path          указатель на путь
 */
void lv_draw_vector_dsc_add_path(lv_draw_vector_dsc_t * dsc, const lv_vector_path_t * path);

/**
 * Очистить область прямоугольника, используя текущий цвет заливки
 * @param dsc           указатель на дескриптор векторной графики
 * @param rect          область, которую нужно очистить в буфере
 */
void lv_draw_vector_dsc_clear_area(lv_draw_vector_dsc_t * dsc, const lv_area_t * rect);

/**
 * Нарисуйте все векторные графические пути
 * @param dsc           указатель на дескриптор векторной графики
 */
void lv_draw_vector(lv_draw_vector_dsc_t * dsc);

/**
 * Попробуйте получить дескриптор векторной отрисовки из задачи рисования.
 * @param task      нарисовать задачу
 * @return          дескриптор отрисовки задачи или NULL, если задача не относится к типу LV_DRAW_TASK_TYPE_VECTOR
 */
lv_draw_vector_dsc_t * lv_draw_task_get_vector_dsc(lv_draw_task_t * task);

/* Траверсер для списка задач */
typedef void (*vector_draw_task_cb)(void * ctx, const lv_vector_path_t * path, const lv_vector_path_ctx_t * dsc);

#endif /* LV_USE_VECTOR_GRAPHIC */

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /* LV_DRAW_VECTOR_H */
