/**
 * @file lv_draw_sw_border.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_area_private.h"
#include "lv_draw_sw_mask_private.h"
#include "../lv_draw_private.h"
#include "../lv_draw_private.h"
#include "lv_draw_sw.h"
#if LV_USE_DRAW_SW

#include "blend/lv_draw_sw_blend_private.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_text_ap.h"
#include "../../core/lv_refr.h"
#include "../../misc/lv_assert.h"
#include "../../stdlib/lv_string.h"
#include "../lv_draw_mask.h"

/*********************
 *      DEFINES
 *********************/
#define SPLIT_LIMIT             50

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void draw_border_complex(lv_draw_task_t * t, const lv_area_t * outer_area, const lv_area_t * inner_area,
                                int32_t rout, int32_t rin, lv_color_t color, lv_opa_t opa);

static void draw_border_simple(lv_draw_task_t * t, const lv_area_t * outer_area, const lv_area_t * inner_area,
                               lv_color_t color, lv_opa_t opa);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sw_border(lv_draw_task_t * t, const lv_draw_border_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width == 0) return;
    if(dsc->side == LV_BORDER_SIDE_NONE) return;

    int32_t coords_w = lv_area_get_width(coords);
    int32_t coords_h = lv_area_get_height(coords);
    int32_t rout = dsc->radius;
    int32_t short_side = LV_MIN(coords_w, coords_h);
    if(rout > short_side >> 1) rout = short_side >> 1;

    /*Получить внутреннюю область*/
    lv_area_t area_inner;
    lv_area_copy(&area_inner, coords);
    area_inner.x1 += ((dsc->side & LV_BORDER_SIDE_LEFT) ? dsc->width : - (dsc->width + rout));
    area_inner.x2 -= ((dsc->side & LV_BORDER_SIDE_RIGHT) ? dsc->width : - (dsc->width + rout));
    area_inner.y1 += ((dsc->side & LV_BORDER_SIDE_TOP) ? dsc->width : - (dsc->width + rout));
    area_inner.y2 -= ((dsc->side & LV_BORDER_SIDE_BOTTOM) ? dsc->width : - (dsc->width + rout));

    int32_t rin = rout - dsc->width;
    if(rin < 0) rin = 0;

    if(rout == 0 && rin == 0) {
        draw_border_simple(t, coords, &area_inner, dsc->color, dsc->opa);
    }
    else {
        draw_border_complex(t, coords, &area_inner, rout, rin, dsc->color, dsc->opa);
    }

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

void draw_border_complex(lv_draw_task_t * t, const lv_area_t * outer_area, const lv_area_t * inner_area,
                         int32_t rout, int32_t rin, lv_color_t color, lv_opa_t opa)
{
#if LV_DRAW_SW_COMPLEX
    /*Получите обрезанную область прорисовки, которая является настоящей областью прорисовки.
     *Всегда одно и то же или внутри `coords`*/
    lv_area_t draw_area;
    if(!lv_area_intersect(&draw_area, outer_area, &t->clip_area)) return;
    int32_t draw_area_w = lv_area_get_width(&draw_area);

    lv_draw_sw_blend_dsc_t blend_dsc;
    lv_memzero(&blend_dsc, sizeof(blend_dsc));
    lv_opa_t * mask_buf = lv_malloc(draw_area_w);
    blend_dsc.mask_buf = mask_buf;

    void * mask_list[3] = {0};

    /*Создайте маску для внутренней маски*/
    lv_draw_sw_mask_radius_param_t mask_rin_param;
    lv_draw_sw_mask_radius_init(&mask_rin_param, inner_area, rin, true);
    mask_list[0] = &mask_rin_param;

    /*Создайте маску для внешней области*/
    lv_draw_sw_mask_radius_param_t mask_rout_param;
    if(rout > 0) {
        lv_draw_sw_mask_radius_init(&mask_rout_param, outer_area, rout, false);
        mask_list[1] = &mask_rout_param;
    }

    int32_t h;
    lv_area_t blend_area;
    blend_dsc.blend_area = &blend_area;
    blend_dsc.mask_area = &blend_area;
    blend_dsc.color = color;
    blend_dsc.opa = opa;

    /*Вычислите координаты x и y, в которых находится площадь прямой детали.*/
    lv_area_t core_area;
    core_area.x1 = LV_MAX(outer_area->x1 + rout, inner_area->x1);
    core_area.x2 = LV_MIN(outer_area->x2 - rout, inner_area->x2);
    core_area.y1 = LV_MAX(outer_area->y1 + rout, inner_area->y1);
    core_area.y2 = LV_MIN(outer_area->y2 - rout, inner_area->y2);
    int32_t core_w = lv_area_get_width(&core_area);

    bool top_side = outer_area->y1 <= inner_area->y1;
    bool bottom_side = outer_area->y2 >= inner_area->y2;

    /*Без масок*/
    bool left_side = outer_area->x1 <= inner_area->x1;
    bool right_side = outer_area->x2 >= inner_area->x2;

    bool split_hor = true;
    if(left_side && right_side && top_side && bottom_side &&
       core_w < SPLIT_LIMIT) {
        split_hor = false;
    }

    blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_FULL_COVER;
    /*Сначала нарисуйте прямые линии, если они достаточно длинные.*/
    if(top_side && split_hor) {
        blend_area.x1 = core_area.x1;
        blend_area.x2 = core_area.x2;
        blend_area.y1 = outer_area->y1;
        blend_area.y2 = inner_area->y1 - 1;
        lv_draw_sw_blend(t, &blend_dsc);
    }

    if(bottom_side && split_hor) {
        blend_area.x1 = core_area.x1;
        blend_area.x2 = core_area.x2;
        blend_area.y1 = inner_area->y2 + 1;
        blend_area.y2 = outer_area->y2;
        lv_draw_sw_blend(t, &blend_dsc);
    }

    /*Если граница очень толстая и вертикальные стороны перекрываются по горизонтали, нарисуйте один прямоугольник.*/
    if(inner_area->x1 >= inner_area->x2 && left_side && right_side) {
        blend_area.x1 = outer_area->x1;
        blend_area.x2 = outer_area->x2;
        blend_area.y1 = core_area.y1;
        blend_area.y2 = core_area.y2;
        lv_draw_sw_blend(t, &blend_dsc);
    }
    else {
        if(left_side) {
            blend_area.x1 = outer_area->x1;
            blend_area.x2 = inner_area->x1 - 1;
            blend_area.y1 = core_area.y1;
            blend_area.y2 = core_area.y2;
            lv_draw_sw_blend(t, &blend_dsc);
        }

        if(right_side) {
            blend_area.x1 = inner_area->x2 + 1;
            blend_area.x2 = outer_area->x2;
            blend_area.y1 = core_area.y1;
            blend_area.y2 = core_area.y2;
            lv_draw_sw_blend(t, &blend_dsc);
        }
    }

    /*Нарисуйте углы*/
    int32_t blend_w;

    /*Левый и правый угол вместе, если они расположены близко друг к другу*/
    if(!split_hor) {
        /*Вычислите верхний угол и отразите его в нижней части.*/
        blend_area.x1 = draw_area.x1;
        blend_area.x2 = draw_area.x2;
        int32_t max_h = LV_MAX(rout, inner_area->y1 - outer_area->y1);
        for(h = 0; h < max_h; h++) {
            int32_t top_y = outer_area->y1 + h;
            int32_t bottom_y = outer_area->y2 - h;
            if(top_y < draw_area.y1 && bottom_y > draw_area.y2) continue;   /*Эта строка сейчас обрезана*/

            lv_memset(mask_buf, 0xff, draw_area_w);
            blend_dsc.mask_res = lv_draw_sw_mask_apply(mask_list, mask_buf, blend_area.x1, top_y, draw_area_w);

            if(top_y >= draw_area.y1) {
                blend_area.y1 = top_y;
                blend_area.y2 = top_y;
                lv_draw_sw_blend(t, &blend_dsc);
            }

            if(bottom_y <= draw_area.y2) {
                blend_area.y1 = bottom_y;
                blend_area.y2 = bottom_y;
                lv_draw_sw_blend(t, &blend_dsc);
            }
        }
    }
    else {
        /*Левые углы*/
        blend_area.x1 = draw_area.x1;
        blend_area.x2 = LV_MIN(draw_area.x2, core_area.x1 - 1);
        blend_w = lv_area_get_width(&blend_area);
        if(blend_w > 0) {
            if(left_side || top_side) {
                for(h = draw_area.y1; h < core_area.y1; h++) {
                    blend_area.y1 = h;
                    blend_area.y2 = h;

                    lv_memset(mask_buf, 0xff, blend_w);
                    blend_dsc.mask_res = lv_draw_sw_mask_apply(mask_list, mask_buf, blend_area.x1, h, blend_w);
                    lv_draw_sw_blend(t, &blend_dsc);
                }
            }

            if(left_side || bottom_side) {
                for(h = core_area.y2 + 1; h <= draw_area.y2; h++) {
                    blend_area.y1 = h;
                    blend_area.y2 = h;

                    lv_memset(mask_buf, 0xff, blend_w);
                    blend_dsc.mask_res = lv_draw_sw_mask_apply(mask_list, mask_buf, blend_area.x1, h, blend_w);
                    lv_draw_sw_blend(t, &blend_dsc);
                }
            }
        }

        /*Правые углы*/
        blend_area.x1 = LV_MAX(draw_area.x1, blend_area.x2 + 1);    /*Чтобы не пересекаться с левой стороной*/
        blend_area.x1 = LV_MAX(draw_area.x1, core_area.x2 + 1);

        blend_area.x2 = draw_area.x2;
        blend_w = lv_area_get_width(&blend_area);

        if(blend_w > 0) {
            if(right_side || top_side) {
                for(h = draw_area.y1; h < core_area.y1; h++) {
                    blend_area.y1 = h;
                    blend_area.y2 = h;

                    lv_memset(mask_buf, 0xff, blend_w);
                    blend_dsc.mask_res = lv_draw_sw_mask_apply(mask_list, mask_buf, blend_area.x1, h, blend_w);
                    lv_draw_sw_blend(t, &blend_dsc);
                }
            }

            if(right_side || bottom_side) {
                for(h = core_area.y2 + 1; h <= draw_area.y2; h++) {
                    blend_area.y1 = h;
                    blend_area.y2 = h;

                    lv_memset(mask_buf, 0xff, blend_w);
                    blend_dsc.mask_res = lv_draw_sw_mask_apply(mask_list, mask_buf, blend_area.x1, h, blend_w);
                    lv_draw_sw_blend(t, &blend_dsc);
                }
            }
        }
    }

    lv_draw_sw_mask_free_param(&mask_rin_param);
    if(rout > 0) lv_draw_sw_mask_free_param(&mask_rout_param);
    lv_free(mask_buf);

#else
    LV_UNUSED(t);
    LV_UNUSED(outer_area);
    LV_UNUSED(inner_area);
    LV_UNUSED(rout);
    LV_UNUSED(rin);
    LV_UNUSED(color);
    LV_UNUSED(opa);
#endif /*LV_DRAW_SW_COMPLEX*/
}
static void draw_border_simple(lv_draw_task_t * t, const lv_area_t * outer_area, const lv_area_t * inner_area,
                               lv_color_t color, lv_opa_t opa)
{
    lv_area_t a;
    lv_draw_sw_blend_dsc_t blend_dsc;
    lv_memzero(&blend_dsc, sizeof(lv_draw_sw_blend_dsc_t));
    blend_dsc.blend_area = &a;
    blend_dsc.color = color;
    blend_dsc.opa = opa;

    bool top_side = outer_area->y1 <= inner_area->y1;
    bool bottom_side = outer_area->y2 >= inner_area->y2;
    bool left_side = outer_area->x1 <= inner_area->x1;
    bool right_side = outer_area->x2 >= inner_area->x2;

    /*Топ*/
    a.x1 = outer_area->x1;
    a.x2 = outer_area->x2;
    a.y1 = outer_area->y1;
    a.y2 = inner_area->y1 - 1;
    if(top_side) {
        lv_draw_sw_blend(t, &blend_dsc);
    }

    /*Внизу*/
    a.y1 = inner_area->y2 + 1;
    a.y2 = outer_area->y2;
    if(bottom_side) {
        lv_draw_sw_blend(t, &blend_dsc);
    }

    /*Левый*/
    a.x1 = outer_area->x1;
    a.x2 = inner_area->x1 - 1;
    a.y1 = (top_side) ? inner_area->y1 : outer_area->y1;
    a.y2 = (bottom_side) ? inner_area->y2 : outer_area->y2;
    if(left_side) {
        lv_draw_sw_blend(t, &blend_dsc);
    }

    /*Правильно*/
    a.x1 = inner_area->x2 + 1;
    a.x2 = outer_area->x2;
    if(right_side) {
        lv_draw_sw_blend(t, &blend_dsc);
    }
}

#endif /*LV_USE_DRAW_SW*/
