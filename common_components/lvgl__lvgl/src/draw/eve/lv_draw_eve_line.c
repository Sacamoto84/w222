/**
 * @file lv_draw_eve_line.c
 *
 */

/*  Создано: 8 апр 2023
 *      Author: Хуандж
 *
 *  Изменено LVGL
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_eve_private.h"
#if LV_USE_DRAW_EVE
#include "lv_eve.h"


void lv_draw_eve_line(lv_draw_task_t * t, const lv_draw_line_dsc_t * dsc)
{

    if(dsc->width == 0)
        return;
    if(dsc->opa <= LV_OPA_MIN)
        return;
    if(dsc->p1.x == dsc->p2.x && dsc->p1.y == dsc->p2.y)
        return;



    uint32_t line_w = dsc->width * 8;
    lv_eve_scissor(t->clip_area.x1, t->clip_area.y1, t->clip_area.x2, t->clip_area.y2);
    lv_eve_save_context();
    lv_eve_color_opa(dsc->opa);
    lv_eve_color(dsc->color);

    if(dsc->dash_gap || dsc->dash_width) {
        LV_LOG_WARN("line dash_gap and dash_width not implemented by EVE yet.");
    }
    /* Проверьте, вертикальная или горизонтальная линия без закруглений. */
    bool is_vertical = (dsc->p1.x == dsc->p2.x);
    bool is_horizontal = (dsc->p1.y == dsc->p2.y);
    bool no_round = (!dsc->round_end || !dsc->round_start);

    if((is_vertical || is_horizontal) && no_round) {
        lv_eve_primitive(LV_EVE_PRIMITIVE_RECTS);
        lv_eve_vertex_2f(dsc->p1.x, dsc->p1.y);
        lv_eve_vertex_2f(dsc->p2.x, dsc->p2.y);
    }
    else {
        /* Нарисовать наклонную линию или линию с закруглением (без закругления невозможно)*/
        lv_eve_primitive(LV_EVE_PRIMITIVE_LINE_STRIP);
        lv_eve_line_width(line_w);
        lv_eve_vertex_2f(dsc->p1.x, dsc->p1.y);
        lv_eve_vertex_2f(dsc->p2.x, dsc->p2.y);
    }

    lv_eve_restore_context();
}

#endif /*LV_USE_DRAW_EVE*/
