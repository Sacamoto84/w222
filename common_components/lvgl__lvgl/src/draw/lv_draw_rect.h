/**
 * @file lv_draw_rect.h
 *
 */

#ifndef LV_DRAW_RECT_H
#define LV_DRAW_RECT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "../misc/lv_color.h"
#include "../misc/lv_area.h"
#include "../misc/lv_style.h"

/*********************
 *      DEFINES
 *********************/
#define LV_RADIUS_CIRCLE        0x7FFF /**< A very big radius to always draw as circle*/
LV_EXPORT_CONST_INT(LV_RADIUS_CIRCLE);

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_dsc_base_t base;

    int32_t radius;

    /*Фоновое изображение*/
    const void * bg_image_src;
    const void * bg_image_symbol_font;
    lv_color_t bg_image_recolor;
    lv_opa_t bg_image_opa;
    lv_opa_t bg_image_recolor_opa;
    uint8_t bg_image_tiled;
    /*Фон*/
    lv_opa_t bg_opa;
    /*Граница*/
    lv_opa_t border_opa;
    /*Схема */
    lv_opa_t outline_opa;
    /*Тень*/
    lv_opa_t shadow_opa;

    /*Фон*/
    lv_color_t bg_color;        /**< First element of a gradient is a color, so it maps well here*/
    lv_grad_dsc_t bg_grad;

    const lv_image_colorkey_t * bg_image_colorkey;

    /*Граница*/
    lv_color_t border_color;
    int32_t border_width;
    lv_border_side_t border_side : 5;
    uint8_t border_post : 1; /*Граница будет проведена позже*/

    /*Схема*/
    lv_color_t outline_color;
    int32_t outline_width;
    int32_t outline_pad;

    /*Тень*/
    lv_color_t shadow_color;
    int32_t shadow_width;
    int32_t shadow_offset_x;
    int32_t shadow_offset_y;
    int32_t shadow_spread;
} lv_draw_rect_dsc_t;

typedef struct {
    lv_draw_dsc_base_t base;

    /**Radius, LV_RADIUS_CIRCLE for max. radius */
    int32_t radius;

    /**Непрозрачность в диапазоне 0...255.
     * LV_OPA_TRANSP , LV_OPA_10 , LV_OPA_20 , .. LV_OPA_COVER также можно использовать*/
    lv_opa_t opa;

    /**Цвет прямоугольника.
     * Если градиент установлен (grad.dir!=LV_GRAD_DIR_NONE), он добавляется. */
    lv_color_t color;

    /**Describe a gradient. If `grad.dir` is not `LV_GRAD_DIR_NONE` `color` will be ignored*/
    lv_grad_dsc_t grad;
} lv_draw_fill_dsc_t;

typedef struct {
    lv_draw_dsc_base_t base;

    /**Radius, LV_RADIUS_CIRCLE for max. radius */
    int32_t radius;

    /**The color of the border. */
    lv_color_t color;


    /**The width of the border in pixels */
    int32_t width;

    /**Непрозрачность в диапазоне 0...255.
     * LV_OPA_TRANSP , LV_OPA_10 , LV_OPA_20 , .. LV_OPA_COVER также можно использовать*/
    lv_opa_t opa;

    /**LV_BORDER_SIDE_NONE/LEFT/RIGHT/TOP/BOTTOM/FULL.
     * LV_BORDER_SIDE_INTERNAL — информация для верхних слоев.
     * и не должен использоваться здесь. */
    lv_border_side_t side : 5;

} lv_draw_border_dsc_t;

typedef struct {
    lv_draw_dsc_base_t base;

    /**Radius, LV_RADIUS_CIRCLE for max. radius */
    int32_t radius;

    /**Color of shadow */
    lv_color_t color;

    /**Width of the shadow. (radius of the blur)*/
    int32_t width;

    /**Make the rectangle larger with this value in all directions. Can be negative too. */
    int32_t spread;

    /**Offset the rectangle horizontally.*/
    int32_t ofs_x;

    /**Offset the rectangle vertically.*/
    int32_t ofs_y;

    /**Непрозрачность в диапазоне 0...255.
     * LV_OPA_TRANSP , LV_OPA_10 , LV_OPA_20 , .. LV_OPA_COVER также можно использовать*/
    lv_opa_t opa;

    /**Установите`bg_cover`на 1, если фон будет закрывать тень.
     * Это подсказка рендереру о том, что некоторые маскировки могут быть пропущены.*/
    uint8_t bg_cover    : 1;
} lv_draw_box_shadow_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте дескриптор рисования прямоугольника.
 * @param dsc       указатель на дескриптор отрисовки
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_rect_dsc_init(lv_draw_rect_dsc_t * dsc);

/**
 * Инициализируйте дескриптор рисования заливки.
 * @param dsc       указатель на дескриптор отрисовки
 */
void lv_draw_fill_dsc_init(lv_draw_fill_dsc_t * dsc);

/**
 * Попробуйте получить дескриптор отрисовки заливки из задачи рисования.
 * @param task      нарисовать задачу
 * @return          дескриптор отрисовки задачи или NULL, если задача не относится к типу LV_DRAW_TASK_TYPE_FILL
 */
lv_draw_fill_dsc_t * lv_draw_task_get_fill_dsc(lv_draw_task_t * task);

/**
 * Заполните область
 * @param layer         указатель на слой
 * @param dsc           указатель на инициализированную переменную дескриптора отрисовки
 * @param coords        координаты прямоугольника
 */
void lv_draw_fill(lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);

/**
 * Инициализируйте дескриптор отрисовки границы.
 * @param dsc       указатель на дескриптор отрисовки
 */
void lv_draw_border_dsc_init(lv_draw_border_dsc_t * dsc);

/**
 * Попробуйте получить дескриптор отрисовки границы из задачи рисования.
 * @param task      нарисовать задачу
 * @return          дескриптор отрисовки задачи или NULL, если задача не относится к типу LV_DRAW_TASK_TYPE_BORDER
 */
lv_draw_border_dsc_t * lv_draw_task_get_border_dsc(lv_draw_task_t * task);

/**
 * Нарисуйте границу
 * @param layer         указатель на слой
 * @param dsc           указатель на инициализированную переменную дескриптора отрисовки
 * @param coords        координаты прямоугольника
 */
void lv_draw_border(lv_layer_t * layer, const lv_draw_border_dsc_t * dsc, const lv_area_t * coords);

/**
 * Инициализируйте дескриптор отрисовки тени блока.
 * @param dsc       указатель на дескриптор отрисовки
 */
void lv_draw_box_shadow_dsc_init(lv_draw_box_shadow_dsc_t * dsc);

/**
 * Попробуйте получить дескриптор отрисовки тени блока из задачи рисования.
 * @param task      нарисовать задачу
 * @return          дескриптор отрисовки задачи или NULL, если задача не относится к типу LV_DRAW_TASK_TYPE_BOX_SHADOW
 */
lv_draw_box_shadow_dsc_t * lv_draw_task_get_box_shadow_dsc(lv_draw_task_t * task);

/**
 * Нарисуйте тень от коробки
 * @param layer         указатель на слой
 * @param dsc           указатель на инициализированную переменную дескриптора отрисовки
 * @param coords        координаты прямоугольника
 */
void lv_draw_box_shadow(lv_layer_t * layer, const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords);

/**
 * Прямоугольник — это оболочка для заливки, границы и фона. изображение и тень окна.
 * Будут созданы задачи внутренней заливки, границы, изображения и тени блока.
 * @param layer         указатель на слой
 * @param dsc           указатель на инициализированную переменную дескриптора отрисовки
 * @param coords        координаты прямоугольника
 */
void lv_draw_rect(lv_layer_t * layer, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_RECT_H*/
