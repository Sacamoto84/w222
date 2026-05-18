/**
 * @file lv_draw_vg_lite_layer.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vg_lite.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_draw_vg_lite_type.h"
#include "lv_vg_lite_utils.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_vg_lite_layer(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                           const lv_area_t * coords)
{
    lv_layer_t * layer = (lv_layer_t *)draw_dsc->src;
    lv_draw_vg_lite_unit_t * u = (lv_draw_vg_lite_unit_t *)t->draw_unit;

    /*Может случиться так, что на слое ничего не было нарисовано и поэтому его буфер не выделен.
     *В этом случае просто вернитесь. */
    if(layer->draw_buf == NULL)
        return;

    LV_PROFILER_DRAW_BEGIN;

    /* Выход GPU уже должен быть предварительно умножен на RGB. */
    if(!lv_draw_buf_has_flag(layer->draw_buf, LV_IMAGE_FLAGS_PREMULTIPLIED)) {
        LV_LOG_WARN("Non-premultiplied layer buffer for GPU to draw.");
    }

    lv_draw_image_dsc_t new_draw_dsc = *draw_dsc;
    new_draw_dsc.src = layer->draw_buf;
    lv_draw_vg_lite_img(t, &new_draw_dsc, coords, true);

    /* Подождите завершения рисования GPU здесь,
     * в противном случае это может привести к сбою рисунка. */
    lv_vg_lite_finish(u);

    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/
