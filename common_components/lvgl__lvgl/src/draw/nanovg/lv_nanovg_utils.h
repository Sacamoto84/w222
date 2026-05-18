/**
 * @file lv_nanovg_utils.h
 *
 */

#ifndef LV_NANOVG_UTILS_H
#define LV_NANOVG_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_NANOVG

#include "../../misc/lv_assert.h"
#include "../../misc/lv_matrix.h"
#include "../../misc/lv_color.h"
#include "../../libs/nanovg/nanovg.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_nanovg_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализация утилиты NanoVG
 * @param u указатель на блок nanovg
 */
void lv_nanovg_utils_init(struct _lv_draw_nanovg_unit_t * u);

/**
 * Деинициализация утилиты NanoVG
 * @param u указатель на блок nanovg
 */
void lv_nanovg_utils_deinit(struct _lv_draw_nanovg_unit_t * u);

/**
 * Преобразование матрицыLVGLв преобразование NanoVG (матрица 3x2)
 * @param xform массив преобразования NanoVG (6 чисел с плавающей запятой)
 * @param matrix матрица LVGL
 */
static inline void lv_nanovg_matrix_convert(float * xform, const lv_matrix_t * matrix)
{
    LV_ASSERT_NULL(xform);
    LV_ASSERT_NULL(matrix);
    xform[0] = matrix->m[0][0];
    xform[1] = matrix->m[1][0];
    xform[2] = matrix->m[0][1];
    xform[3] = matrix->m[1][1];
    xform[4] = matrix->m[0][2];
    xform[5] = matrix->m[1][2];
}

/**
 * Преобразование цветаLVGLв цвет NanoVG.
 * @param color цвет LVGL
 * @param opa непрозрачность
 * @return цвет НаноВГ
 */
static inline NVGcolor lv_nanovg_color_convert(lv_color_t color, lv_opa_t opa)
{
    return nvgRGBA(color.red, color.green, color.blue, opa);
}

/**
 * Преобразование матрицы в контекст NanoVG
 * @param ctx контекст НаноВГ
 * @param matrix матрица преобразования
 */
void lv_nanovg_transform(NVGcontext * ctx, const lv_matrix_t * matrix);

/**
 * Установите область отсечения
 * @param ctx контекст НаноВГ
 * @param area область обрезки
 */
void lv_nanovg_set_clip_area(NVGcontext * ctx, const lv_area_t * area);

/**
 * Добавить прямоугольник к пути
 * @param ctx контекст НаноВГ
 * @param x координата x прямоугольника
 * @param y координата y прямоугольника
 * @param w ширина прямоугольника
 * @param h высота прямоугольника
 * @param r радиус прямоугольника (0 — без округления)
 */
void lv_nanovg_path_append_rect(NVGcontext * ctx, float x, float y, float w, float h, float r);

/**
 * Добавить область к пути
 * @param ctx контекст НаноВГ
 * @param area область
 */
void lv_nanovg_path_append_area(NVGcontext * ctx, const lv_area_t * area);

/**
 * Добавить дугу под прямым углом к пути
 * @param ctx контекст НаноВГ
 * @param start_x начальная координата x
 * @param start_y начальная координата y
 * @param center_x координата центра x
 * @param center_y координата центра y
 * @param end_x конечная координата x
 * @param end_y конечная координата y
 */
void lv_nanovg_path_append_arc_right_angle(NVGcontext * ctx,
                                           float start_x, float start_y,
                                           float center_x, float center_y,
                                           float end_x, float end_y);

/**
 * Добавить дугу к пути
 * @param ctx контекст НаноВГ
 * @param cx координата центра x
 * @param cy координата центра y
 * @param radius радиус
 * @param start_angle начальный угол в радианах
 * @param sweep угол стреловидности в радианах
 * @param pie рисовать ли кусок пирога (подключенный к центру)
 */
void lv_nanovg_path_append_arc(NVGcontext * ctx,
                               float cx, float cy,
                               float radius,
                               float start_angle,
                               float sweep,
                               bool pie);

/**
 * Заполните текущий путь
 * @param ctx контекст НаноВГ
 * @param winding правило намотки
 * @param composite_operation режим наложения
 * @param color цвет заливки
 */
void lv_nanovg_fill(NVGcontext * ctx, enum NVGwinding winding, enum NVGcompositeOperation composite_operation,
                    NVGcolor color);

/**
 * Завершить текущий кадр
 * @param u указатель на блок nanovg
 */
void lv_nanovg_end_frame(struct _lv_draw_nanovg_unit_t * u);

/**
 * Очистите блок NanoVG (например, в конце задания).
 * @param u указатель на блок nanovg
 */
void lv_nanovg_clean_up(struct _lv_draw_nanovg_unit_t * u);

/**
 * Измените форму глобального буфера изображения
 * @param u указатель на блок nanovg
 * @param cf цветовой формат
 * @param w новая ширина
 * @param h новая высота
 * @return указатель на буфер отрисовки измененного размера
 */
lv_draw_buf_t * lv_nanovg_reshape_global_image(struct _lv_draw_nanovg_unit_t * u,
                                               lv_color_format_t cf,
                                               uint32_t w,
                                               uint32_t h);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRAW_NANOVG */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_NANOVG_UTILS_H*/
