/**
 * @file lv_draw_sw.h
 *
 */

#ifndef LV_DRAW_SW_H
#define LV_DRAW_SW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_draw.h"
#if LV_USE_DRAW_SW

#include "../../misc/lv_area.h"
#include "../../misc/lv_color.h"
#include "../../display/lv_display.h"

#include "../lv_draw_vector.h"
#include "../lv_draw_triangle.h"
#include "../lv_draw_label.h"
#include "../lv_draw_image.h"
#include "../lv_draw_line.h"
#include "../lv_draw_mask.h"
#include "../lv_draw_arc.h"
#include "../lv_draw_blur.h"
#include "lv_draw_sw_utils.h"
#include "blend/lv_draw_sw_blend.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте рендерер SW. Позвонил внутрь.
 * Он создает столько рендереров SW, сколько определено в LV_DRAW_SW_DRAW_UNIT_CNT.
 */
void lv_draw_sw_init(void);

/**
 * Деинициализируйте средства рендеринга SW.
 */
void lv_draw_sw_deinit(void);

/**
 * Заполните область, используя рендер SW. Обработка градиента и радиуса.
 * @param t             указатель на задачу рисования
 * @param dsc           дескриптор отрисовки
 * @param coords        координаты прямоугольника
 */
void lv_draw_sw_fill(lv_draw_task_t * t, lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);

/**
 * Нарисуйте границу с помощью рендеринга SW.
 * @param t             указатель на задачу рисования
 * @param dsc           дескриптор отрисовки
 * @param coords        координаты прямоугольника
 */
void lv_draw_sw_border(lv_draw_task_t * t, const lv_draw_border_dsc_t * dsc, const lv_area_t * coords);

/**
 * Нарисуйте тень блока с помощью рендеринга SW.
 * @param t             указатель на задачу рисования
 * @param dsc           дескриптор отрисовки
 * @param coords        координаты прямоугольника, для которого должна быть нарисована тень блока
 */
void lv_draw_sw_box_shadow(lv_draw_task_t * t, const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords);

/**
 * Нарисуйте изображение с помощью рендеринга SW. Он обрабатывает декодирование изображений, мозаику, преобразования и перекрашивание.
 * @param t             указатель на задачу рисования
 * @param draw_dsc      дескриптор отрисовки
 * @param coords        координаты изображения
 */
void lv_draw_sw_image(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                      const lv_area_t * coords);

void lv_draw_sw_letter(lv_draw_task_t * t, const lv_draw_letter_dsc_t * dsc, const lv_area_t * coords);

/**
 * Нарисуйте метку с помощью рендеринга SW.
 * @param t             указатель на задачу рисования
 * @param dsc           дескриптор отрисовки
 * @param coords        координаты этикетки
 */
void lv_draw_sw_label(lv_draw_task_t * t, const lv_draw_label_dsc_t * dsc, const lv_area_t * coords);

/**
 * Нарисуйте дугу с помощью рендеринга SW.
 * @param t             указатель на задачу рисования
 * @param dsc           дескриптор отрисовки
 * @param coords        координаты дуги
 */
void lv_draw_sw_arc(lv_draw_task_t * t, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords);

/**
 * Нарисуйте линию с помощью рендера SW.
 * @param t             указатель на задачу рисования
 * @param dsc           дескриптор отрисовки
 */
void lv_draw_sw_line(lv_draw_task_t * t, const lv_draw_line_dsc_t * dsc);

/**
 * Смешайте слой с помощью рендеринга SW.
 * @param t             указатель на задачу рисования
 * @param draw_dsc      дескриптор отрисовки
 * @param coords        координаты слоя
 */
void lv_draw_sw_layer(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * coords);

/**
 * Нарисуйте треугольник с помощью рендеринга SW.
 * @param t             указатель на задачу рисования
 * @param dsc           дескриптор отрисовки
 */
void lv_draw_sw_triangle(lv_draw_task_t * t, const lv_draw_triangle_dsc_t * dsc);


/**
 * Размытие области с помощью рендеринга SW.
 * @param t             указатель на задачу рисования
 * @param dsc           дескриптор отрисовки
 * @param coords        область для размытия
 */
void lv_draw_sw_blur(lv_draw_task_t * t, const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords);

/**
 * Замаскируйте прямоугольник радиусом текущего слоя.
 * @param t             указатель на задачу рисования
 * @param dsc           дескриптор отрисовки
 * @param coords        координаты маски
 */
void lv_draw_sw_mask_rect(lv_draw_task_t * t, const lv_draw_mask_rect_dsc_t * dsc);

/**
 * Используется внутренне для получения преобразованного изображения.
 * @param dest_area     область для расчета, т.е. получить эту площадь из преобразованного изображения
 * @param src_buf       исходный буфер
 * @param src_w         ширина исходного буфера в пикселях
 * @param src_h         высота исходного буфера в пикселях
 * @param src_stride    шаг исходного буфера в байтах
 * @param draw_dsc      нарисовать дескриптор
 * @param sup           дополнительные данные
 * @param cf            цветовой формат исходного буфера
 * @param dest_buf      буфер назначения
 */
void lv_draw_sw_transform(const lv_area_t * dest_area, const void * src_buf,
                          int32_t src_w, int32_t src_h, int32_t src_stride,
                          const lv_draw_image_dsc_t * draw_dsc, const lv_draw_image_sup_t * sup, lv_color_format_t cf, void * dest_buf);

#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
/**
 * Рисуйте векторную графику с помощью рендеринга SW.
 * @param t             указатель на задачу рисования
 * @param dsc           дескриптор отрисовки
 */
void lv_draw_sw_vector(lv_draw_task_t * t, lv_draw_vector_dsc_t * dsc);
#endif

/**
 * Зарегистрируйте собственный обработчик смешивания для цветового формата.
 * Обработчик будет вызываться при смешивании цвета или
 * изображение в буфер с заданным цветовым форматом.
 * Для цветового формата можно зарегистрировать не более одного обработчика.
 * Последующие регистрации перезапишут предыдущий обработчик.
 *
 * @param handler указатель на обработчик смешивания
 * @return true, если обработчик был зарегистрирован, false, если обработчик не удалось зарегистрировать
 */
bool lv_draw_sw_register_blend_handler(lv_draw_sw_custom_blend_handler_t * handler);

/**
 * Отмените регистрацию пользовательского обработчика смешивания для цветового формата.
 * @param dest_cf цветовой формат
 * @return true, если обработчик не был зарегистрирован, false, если обработчик не был зарегистрирован
 */
bool lv_draw_sw_unregister_blend_handler(lv_color_format_t dest_cf);

/**
 * Получите обработчик смешивания для цветового формата.
 * @param dest_cf цветовой формат
 * @return указатель на обработчик смешивания или NULL, если обработчик не зарегистрирован
 */
lv_draw_sw_blend_handler_t lv_draw_sw_get_blend_handler(lv_color_format_t dest_cf);

/***********************
 * GLOBAL VARIABLES
 ***********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_SW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_H*/
