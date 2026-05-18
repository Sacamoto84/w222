/**
 * @file lv_obj_draw.h
 *
 */

#ifndef LV_OBJ_DRAW_H
#define LV_OBJ_DRAW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_types.h"
#include "../draw/lv_draw_rect.h"
#include "../draw/lv_draw_label.h"
#include "../draw/lv_draw_image.h"
#include "../draw/lv_draw_line.h"
#include "../draw/lv_draw_arc.h"
#include "../draw/lv_draw_triangle.h"
#include "../draw/lv_draw_blur.h"
#include "lv_obj_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Сохраните тип слоя, необходимый для рендеринга виджета.*/
typedef enum {
    /**No layer is needed. */
    LV_LAYER_TYPE_NONE,

    /**Простой слой означает, что слой можно визуализировать частями.
     * Например, приopa_layered= 140 можно отобразить только 10 строк.
     * из слоя. Когда он будет готов, перейдите к следующим 10 строкам.
     * Это позволяет избежать выделения большого количества памяти для буфера слоя.
     * Размер буфера для фрагмента можно установить с помощью`LV_DRAW_LAYER_SIMPLE_BUF_SIZE`в lv_conf.h.*/
    LV_LAYER_TYPE_SIMPLE,

    /**Виджет трансформируется и не может отображаться частями.
     * Это потому, что из-за преобразований пиксель за пределами
     * данная область также будет способствовать формированию окончательного изображения.
     * В этом случае ограничений на размер буфера нет.
     * LVGL выделит буфер настолько большого размера, насколько необходимо для рендеринга преобразованной области.*/
    LV_LAYER_TYPE_TRANSFORM,
} lv_layer_type_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализировать дескриптор рисования прямоугольника из стилей объекта в его текущем состоянии.
 * @param obj       указатель на объект
 * @param part      часть объекта, напр.  `LV_PART_MAIN`, `LV_PART_SCROLLBAR`,`LV_PART_KNOB`и т. д.
 * @param draw_dsc  дескриптор для инициализации.
 *                  Если для поля`..._opa`установлено значение`LV_OPA_TRANSP`, связанные свойства не будут инициализированы.
 *                  Должен быть инициализирован с помощью `lv_draw_rect_dsc_init(draw_dsc)`.
 * @note Будут установлены только соответствующие поля.
 *       например, если`border width == 0`, другие свойства границ не будут оцениваться.
 */
void lv_obj_init_draw_rect_dsc(lv_obj_t * obj, lv_part_t part, lv_draw_rect_dsc_t * draw_dsc);

/**
 * Инициализировать дескриптор рисования метки из стилей объекта в его текущем состоянии.
 * @param obj       указатель на объект
 * @param part      часть объекта, напр.  `LV_PART_MAIN`, `LV_PART_SCROLLBAR`,`LV_PART_KNOB`и т. д.
 * @param draw_dsc  дескриптор для инициализации.
 *                  Если в поле`opa`установлено значение или свойство, равное`LV_OPA_TRANSP`, остальное не будет обосновано.
 *                  Должен быть инициализирован с помощью `lv_draw_label_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_label_dsc(lv_obj_t * obj, lv_part_t part, lv_draw_label_dsc_t * draw_dsc);

/**
 * Инициализировать дескриптор рисования изображения из стилей объекта в его текущем состоянии.
 * @param obj       указатель на объект
 * @param part      часть объекта, напр.  `LV_PART_MAIN`, `LV_PART_SCROLLBAR`,`LV_PART_KNOB`и т. д.
 * @param draw_dsc  дескриптор для инициализации.
 *                  Должен быть инициализирован с помощью `lv_draw_image_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_image_dsc(lv_obj_t * obj, lv_part_t part, lv_draw_image_dsc_t * draw_dsc);

/**
 * Инициализировать дескриптор рисования линии из стилей объекта в его текущем состоянии.
 * @param obj указатель на объект
 * @param part      часть объекта, напр.  `LV_PART_MAIN`, `LV_PART_SCROLLBAR`,`LV_PART_KNOB`и т. д.
 * @param draw_dsc  дескриптор для инициализации.
 *                  Должен быть инициализирован с помощью `lv_draw_line_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_line_dsc(lv_obj_t * obj, lv_part_t part, lv_draw_line_dsc_t * draw_dsc);

/**
 * Инициализировать дескриптор рисования дуги из стилей объекта в его текущем состоянии.
 * @param obj       указатель на объект
 * @param part      часть объекта, напр.  `LV_PART_MAIN`, `LV_PART_SCROLLBAR`,`LV_PART_KNOB`и т. д.
 * @param draw_dsc  дескриптор для инициализации.
 *                  Должен быть инициализирован с помощью `lv_draw_arc_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_arc_dsc(lv_obj_t * obj, lv_part_t part, lv_draw_arc_dsc_t * draw_dsc);


/**
 * Инициализируйте дескриптор рисования размытия из стилей объекта в его текущем состоянии.
 * draw_dsc -> радиус будет рассчитываться только в том случае, если изначально он равен 0. Радиус можно установить перед вызовом этой функции.
 * чтобы не получить его дважды.
 * @param obj       указатель на объект
 * @param part      часть объекта, напр.  `LV_PART_MAIN`, `LV_PART_SCROLLBAR`,`LV_PART_KNOB`и т. д.
 * @param draw_dsc  дескриптор для инициализации.
 *                  Должен быть инициализирован с помощью `lv_draw_blur_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_blur_dsc(lv_obj_t * obj, lv_part_t part, lv_draw_blur_dsc_t * draw_dsc);

/**
 * Получите необходимый дополнительный размер (вокруг части объекта), чтобы нарисовать тень, контур, значение и т. д.
 * @param obj       указатель на объект
 * @param part      часть объекта
 * @return          дополнительный размер, необходимый вокруг объекта
 */
int32_t lv_obj_calculate_ext_draw_size(lv_obj_t * obj, lv_part_t part);

/**
 * Отправьте ' LV_EVENT_REFR_EXT_DRAW_SIZE ' Вызовите обработчик событий предка для объекта, чтобы обновить значение расширенного размера отрисовки.
 * Результат будет сохранен в `obj`.
 * @param obj       указатель на объект
 */
void lv_obj_refresh_ext_draw_size(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_OBJ_DRAW_H*/
