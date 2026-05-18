/**
 * @file lv_draw_nanovg_private.h
 *
 */

#ifndef LV_DRAW_NANOVG_PRIVATE_H
#define LV_DRAW_NANOVG_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_NANOVG
#include "../lv_draw.h"
#include "../lv_draw_private.h"
#include "../../draw/lv_draw_vector.h"
#include "../../draw/lv_draw_arc.h"
#include "../../draw/lv_draw_rect.h"
#include "../../draw/lv_draw_3d.h"
#include "../../draw/lv_draw_image.h"
#include "../../draw/lv_draw_label.h"
#include "../../draw/lv_draw_line.h"
#include "../../draw/lv_draw_triangle.h"
#include "../../misc/lv_area_private.h"

#if !LV_USE_NANOVG
#error "Require LV_USE_NANOVG = 1"
#endif

#if !LV_USE_MATRIX
#error "Require LV_USE_MATRIX = 1"
#endif

#include "../../libs/nanovg/nanovg.h"

/*********************
 *      DEFINES
 *********************/

/* Выберите бэкэнд NanoVG OpenGL на основе LV_NANOVG_BACKEND. */
#if LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GL2
#define NANOVG_GL2_IMPLEMENTATION
#elif LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GL3
#define NANOVG_GL3_IMPLEMENTATION
#elif LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GLES2
#define NANOVG_GLES2_IMPLEMENTATION
#elif LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GLES3
#define NANOVG_GLES3_IMPLEMENTATION
#else
#error "Invalid LV_NANOVG_BACKEND value"
#endif

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_pending_t;
struct NVGLUframebuffer;

typedef struct _lv_draw_nanovg_unit_t {
    lv_draw_unit_t base_unit;
    lv_layer_t * current_layer;
    NVGcontext * vg;
    bool is_started;
    lv_draw_buf_t * image_buf;

    lv_cache_t * image_cache;
    struct _lv_pending_t * image_pending;
    lv_ll_t image_drop_ll;
    const void * image_drop_src;

    lv_cache_t * letter_cache;
    struct _lv_pending_t * letter_pending;

    lv_cache_t * fbo_cache;
} lv_draw_nanovg_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if LV_USE_3DTEXTURE
/**
 * Нарисуйте 3D-текстуру в двадцати NanoVG.
 * @param t указатель на задачу рисования
 * @param dsc указатель на дескриптор трехмерной отрисовки
 * @param coords координаты 3D текстуры
 */
void lv_draw_nanovg_3d(lv_draw_task_t * t, const lv_draw_3d_dsc_t * dsc, const lv_area_t * coords);
#endif

/**
 * Нарисуйте дугу в двадцатикратном NanoVG
 * @param t указатель на задачу рисования
 * @param dsc указатель на дескриптор дуги
 * @param coords координаты дуги
 */
void lv_draw_nanovg_arc(lv_draw_task_t * t, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords);

/**
 * Нарисуйте границу в четвертом NanoVG
 * @param t указатель на задачу рисования
 * @param dsc указатель на дескриптор границы
 * @param coords координаты границы
 */
void lv_draw_nanovg_border(lv_draw_task_t * t, const lv_draw_border_dsc_t * dsc, const lv_area_t * coords);

/**
 * Нарисуйте рамку в двадцати NanoVG.
 * @param t указатель на задачу рисования
 * @param dsc указатель на дескриптор блока
 * @param coords координаты коробки
 */
void lv_draw_nanovg_box_shadow(lv_draw_task_t * t, const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords);

/**
 * Заполните контур в десятки NanoVG.
 * @param t указатель на задачу рисования
 * @param dsc указатель на дескриптор заполнения
 * @param coords координаты прямоугольника
 */
void lv_draw_nanovg_fill(lv_draw_task_t * t, const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);

/**
 * Нарисуйте изображение в двадцатом году NanoVG.
 * @param t указатель на задачу рисования
 * @param dsc указатель на дескриптор изображения
 * @param coords координаты изображения
 * @param image_handle дескриптор изображения для рисования
 */
void lv_draw_nanovg_image(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords,
                          int image_handle);

/**
 * Инициализируйте метку рисования в двадцатых NanoVG.
 * @param u указатель на блок NanoVG
 */
void lv_draw_nanovg_label_init(lv_draw_nanovg_unit_t * u);

/**
 * Деинициализировать метку рисования в двадцатых NanoVG
 * @param u указатель на блок NanoVG
 */
void lv_draw_nanovg_label_deinit(lv_draw_nanovg_unit_t * u);

/**
 * Нарисуйте письмо в двадцатом NanoVG.
 * @param t указатель на задачу рисования
 * @param dsc указатель на дескриптор буквы
 * @param coords координаты письма
 */
void lv_draw_nanovg_letter(lv_draw_task_t * t, const lv_draw_letter_dsc_t * dsc, const lv_area_t * coords);

/**
 * Нарисуйте метку в двадцати NanoVG.
 * @param t указатель на задачу рисования
 * @param dsc указатель на дескриптор метки
 * @param coords координаты этикетки
 */
void lv_draw_nanovg_label(lv_draw_task_t * t, const lv_draw_label_dsc_t * dsc, const lv_area_t * coords);

/**
 * Нарисуйте слой в несколько десятков NanoVG.
 * @param t указатель на задачу рисования
 * @param draw_dsc указатель на дескриптор изображения
 * @param coords координаты слоя
 */
void lv_draw_nanovg_layer(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * coords);

/**
 * Нарисуйте строку в двадцати NanoVG
 * @param t указатель на задачу рисования
 * @param dsc указатель на дескриптор строки
 */
void lv_draw_nanovg_line(lv_draw_task_t * t, const lv_draw_line_dsc_t * dsc);

/**
 * Нарисуйте треугольник в несколько десятков NanoVG.
 * @param t указатель на задачу рисования
 * @param dsc указатель на дескриптор треугольника
 */
void lv_draw_nanovg_triangle(lv_draw_task_t * t, const lv_draw_triangle_dsc_t * dsc);

/**
 * Нарисуйте контурные маски в двадцати размерах NanoVG.
 * @param t указатель на задачу рисования
 * @param dsc указатель на дескриптор маски
 */
void lv_draw_nanovg_mask_rect(lv_draw_task_t * t, const lv_draw_mask_rect_dsc_t * dsc);

/**
 * Получить дескриптор изображения из фреймбуфера
 * @param fb фреймбуфер, из которого нужно получить дескриптор изображения
 * @return дескриптор изображения
 */
int lv_nanovg_fb_get_image_handle(struct NVGLUframebuffer * fb);

#if LV_USE_VECTOR_GRAPHIC
/**
 * Рисование векторной графики в двадцатых NanoVG.
 * @param t указатель на задачу рисования
 * @param dsc указатель на векторный дескриптор
 */
void lv_draw_nanovg_vector(lv_draw_task_t * t, const lv_draw_vector_dsc_t * dsc);

/**
 * @brief Преобразование градиента в краску
 * @param ctx контекст нановг
 * @param grad дескриптор градиента
 * @param paint краска для сохранения результата
 */
bool lv_nanovg_grad_to_paint(NVGcontext * ctx, const lv_vector_gradient_t * grad, NVGpaint * paint);

/**
 * @brief Нарисуйте градиент
 * @param ctx контекст нановг
 * @param grad дескриптор градиента
 * @param winding правило заполнения
 * @param composite_operation режим наложения
 */
void lv_nanovg_draw_grad(
    NVGcontext * ctx,
    const lv_vector_gradient_t * grad,
    enum NVGwinding winding,
    enum NVGcompositeOperation composite_operation);

/**
 * @brief Нарисуйте градиент с помощью помощника
 * @param ctx контекст нановг
 * @param area область для рисования градиента
 * @param grad_dsc дескриптор градиента
 * @param winding правило заполнения
 * @param composite_operation режим наложения
 */
void lv_nanovg_draw_grad_helper(
    NVGcontext * ctx,
    const lv_area_t * area,
    const lv_grad_dsc_t * grad_dsc,
    enum NVGwinding winding,
    enum NVGcompositeOperation composite_operation);

#endif /*LV_USE_VECTOR_GRAPHIC*/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_USE_DRAW_NANOVG*/

#endif /*LV_DRAW_NANOVG_PRIVATE_H*/
