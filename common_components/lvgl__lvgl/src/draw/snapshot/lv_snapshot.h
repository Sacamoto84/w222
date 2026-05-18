/**
 * @file lv_snapshot.h
 *
 */

#ifndef LV_SNAPSHOT_H
#define LV_SNAPSHOT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj.h"

#if LV_USE_SNAPSHOT

#include <stdint.h>
#include <stddef.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Сделайте снимок объекта с его дочерними элементами, при необходимости создайте буфер прорисовки.
 * @param obj   объект для создания снимка.
 * @param cf    цветовой формат для создаваемого изображения.
 * @return      указатель на буфер отрисовки, содержащий изображение снимка, илиNULLв случае неудачи.
 */
lv_draw_buf_t * lv_snapshot_take(lv_obj_t * obj, lv_color_format_t cf);

/**
 * Создайте буфер рисования для хранения снимка изображения объекта.
 * @param obj   объект для создания снимка.
 * @param cf    цветовой формат для создаваемого изображения.
 * @return      указатель на буфер отрисовки, готовый для создания моментального снимка, илиNULLв случае неудачи.
 */
lv_draw_buf_t * lv_snapshot_create_draw_buf(lv_obj_t * obj, lv_color_format_t cf);

/**
 * Измените форму буфера прорисовки, чтобы подготовить снимок объекта.
 * Обычно это используется для проверки того, достаточно ли существующего буфера отрисовки для
 * снимок объекта. Если возвращается LV_RESULT_INVALID , вам следует создать новый.
 * @param draw_buf  буфер рисования для изменения формы.
 * @param obj       объект для создания снимка.
 */
lv_result_t lv_snapshot_reshape_draw_buf(lv_obj_t * obj, lv_draw_buf_t * draw_buf);

/**
 * Сделайте снимок объекта с его дочерними элементами, сохраните информацию об изображении в предоставленный буфер.
 * @param obj       объект для создания снимка.
 * @param cf        цветовой формат для нового снимка изображения.
 *                  Он может отличаться от cf `draw_buf`, если подойдет новый cf.
 * @param draw_buf  буфер рисования для хранения результата изображения. Он автоматически меняет форму.
 * @return          LV_RESULT_OK в случае успеха,LV_RESULT_INVALIDв случае ошибки.
 */
lv_result_t lv_snapshot_take_to_draw_buf(lv_obj_t * obj, lv_color_format_t cf, lv_draw_buf_t * draw_buf);

/**
 * @deprecated Вместо этого используйте `lv_draw_buf_destroy`.
 *
 * Освободите изображение снимка, возвращенное@reflv_snapshot_take .
 * @param dsc   дескриптор изображения, сгенерированный lv_snapshot_take.
 */
void lv_snapshot_free(lv_image_dsc_t * dsc);

/**
 * Сделайте снимок объекта с его дочерними элементами, сохраните информацию об изображении в предоставленный буфер.
 * @param obj       объект для создания снимка.
 * @param cf        цветовой формат для создаваемого изображения.
 * @param dsc       Дескриптор изображения для хранения результата изображения.
 * @param buf       буфер для хранения данных изображения. Он должен соответствовать требованию выравнивания.
 * @param buf_size  указан размер буфера в байтах.
 * @return          LV_RESULT_OK в случае успеха,LV_RESULT_INVALIDв случае ошибки.
 * @deprecated      Вместо этого используйте lv_snapshot_take_to_draw_buf.
 */
lv_result_t lv_snapshot_take_to_buf(lv_obj_t * obj, lv_color_format_t cf, lv_image_dsc_t * dsc,
                                    void * buf,
                                    uint32_t buf_size);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_SNAPSHOT*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
