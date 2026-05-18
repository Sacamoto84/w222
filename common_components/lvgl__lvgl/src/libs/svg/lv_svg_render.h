/**
 * @file lv_svg_render.h
 *
 */

#ifndef LV_SVG_RENDER_H
#define LV_SVG_RENDER_H

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_SVG
#if !LV_USE_VECTOR_GRAPHIC
    #error "LV_USE_SVG requires LV_USE_VECTOR_GRAPHIC = 1"
#endif

#include "lv_svg.h"
#include "../../misc/lv_types.h"
#include "../../draw/lv_draw_vector_private.h"

/*********************
 *      DEFINES
 *********************/

#define LV_SVG_RENDER_OBJ(n) ((lv_svg_render_obj_t*)(n))

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_svg_render_class;

typedef struct _lv_svg_render_obj {
    struct _lv_svg_render_obj * next;
    lv_svg_tag_t tag;
    uint32_t flags;
    char * id;
    lv_vector_path_ctx_t dsc;
    lv_matrix_t matrix;

    /* для ссылки на URL ( XXX ) */
    struct _lv_svg_render_obj * head;
    char * fill_ref;
    char * stroke_ref;
    struct _lv_svg_render_class * clz;
} lv_svg_render_obj_t;

typedef struct _lv_svg_render_class {
    void (*set_paint_ref)(struct _lv_svg_render_obj * obj, lv_vector_path_ctx_t * dsc,
                          const struct _lv_svg_render_obj * target_obj, bool fill);

    void (*init)(struct _lv_svg_render_obj * obj, const lv_svg_node_t * node);
    void (*render)(const struct _lv_svg_render_obj * obj, lv_draw_vector_dsc_t * dsc, const lv_matrix_t * matrix);
    void (*set_attr)(struct _lv_svg_render_obj * obj, lv_vector_path_ctx_t * dsc, const lv_svg_attr_t * attr);
    void (*get_bounds)(const struct _lv_svg_render_obj * obj, lv_area_t * area);
    void (*get_size)(const struct _lv_svg_render_obj * obj, uint32_t * size);
    void (*destroy)(struct _lv_svg_render_obj * obj);
} lv_svg_render_class;

typedef struct _lv_svg_render_hal {
    void (*load_image)(const char * image_url, lv_draw_image_dsc_t * img_dsc);
    const char * (*get_font_path)(const char * font_family);
} lv_svg_render_hal_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Инициализируйте рендер SVG
 * @param hal указатель на структуру с функциями рендеринга
 */
void lv_svg_render_init(const lv_svg_render_hal_t * hal);

/**
 * @brief Создайте новый рендерSVGиз документа SVG.
 * @param svg_doc указатель на документ SVG
 * @return указатель на новый объект рендеринга SVG
 */
lv_svg_render_obj_t * lv_svg_render_create(const lv_svg_node_t * svg_doc);

/**
 * @brief Удалить объект рендеринга SVG
 * @param render указатель на объект рендеринга SVG, который нужно удалить
 */
void lv_svg_render_delete(lv_svg_render_obj_t * render);

/**
 * @brief Получить размер объектов рендеринга
 * @param render указатель на объект рендеринга SVG
 * @return байты объектов рендеринга SVG
 */
uint32_t lv_svg_render_get_size(const lv_svg_render_obj_t * render);

/**
 * @brief Получить ширину и высоту области просмотра объекта рендеринга.
 * @param render указатель на объект рендеринга SVG
 * @param width указатель для сохранения ширины области просмотра объекта рендеринга SVG
 * @param height указатель для сохранения высоты области просмотра объекта рендеринга SVG
 * @return lv_result_t ,LV_RESULT_OKв случае успеха,LV_RESULT_INVALIDв случае неудачи.
 */
lv_result_t lv_svg_render_get_viewport_size(const lv_svg_render_obj_t * render, float * width, float * height);

/**
 * @brief Преобразуйте объектSVGв векторную графику.
 * @param dsc указатель на дескриптор векторной графики
 * @param render указатель на объект рендерингаSVGдля рендеринга
 */
void lv_draw_svg_render(lv_draw_vector_dsc_t * dsc, const lv_svg_render_obj_t * render);

/**
 * @brief Нарисуйте документSVGна слое.
 * @param layer указатель на целевой слой
 * @param svg_doc указатель на документSVGдля рисования
 */
void lv_draw_svg(lv_layer_t * layer, const lv_svg_node_t * svg_doc);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SVG*/

#endif /*LV_SVG_RENDER_H*/
