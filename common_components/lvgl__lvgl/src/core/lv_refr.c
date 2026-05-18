/**
 * @file lv_refr.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_refr_private.h"
#include "lv_obj_draw_private.h"
#include "../misc/lv_area_private.h"
#include "../draw/sw/lv_draw_sw_mask_private.h"
#include "../draw/lv_draw_mask.h"
#include "lv_obj_private.h"
#include "lv_obj_event_private.h"
#include "../display/lv_display.h"
#include "../display/lv_display_private.h"
#include "../tick/lv_tick.h"
#include "../misc/lv_timer_private.h"
#include "../misc/lv_math.h"
#include "../misc/lv_profiler.h"
#include "../misc/lv_types.h"
#include "../draw/lv_draw_private.h"
#include "../stdlib/lv_string.h"
#include "lv_global.h"

/*********************
 *      DEFINES
 *********************/

/*Дисплей обновляется*/
#define disp_refr LV_GLOBAL_DEFAULT()->disp_refresh

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_refr_join_area(void);
static void refr_invalid_areas(void);
static void refr_sync_areas(void);
static void refr_area(const lv_area_t * area_p, int32_t y_offset);
static void refr_configured_layer(lv_layer_t * layer);
static void refr_obj_and_children(lv_layer_t * layer, lv_obj_t * top_obj);
static uint32_t get_max_row(lv_display_t * disp, int32_t area_w, int32_t area_h);
static void draw_buf_flush(lv_display_t * disp);
static void call_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
static void wait_for_flushing(lv_display_t * disp);
static lv_result_t layer_get_area(lv_layer_t * layer, lv_obj_t * obj, lv_layer_type_t layer_type,
                                  lv_area_t * layer_area_out, lv_area_t * obj_draw_size_out);
static bool alpha_test_area_on_obj(lv_obj_t * obj, const lv_area_t * area);
#if LV_DRAW_TRANSFORM_USE_MATRIX
    static bool refr_check_obj_clip_overflow(lv_layer_t * layer, lv_obj_t * obj);
    static void refr_obj_matrix(lv_layer_t * layer, lv_obj_t * obj);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#if LV_USE_LOG && LV_LOG_TRACE_DISP_REFR
    #define LV_TRACE_REFR(...) LV_LOG_TRACE(__VA_ARGS__)
#else
    #define LV_TRACE_REFR(...)
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Инициализировать подсистему обновления экрана
 */
void lv_refr_init(void)
{
}

void lv_refr_deinit(void)
{
}

void lv_refr_now(lv_display_t * disp)
{
    lv_anim_refr_now();

    if(disp) {
        if(disp->refr_timer) lv_display_refr_timer(disp->refr_timer);
    }
    else {
        lv_display_t * d;
        d = lv_display_get_next(NULL);
        while(d) {
            if(d->refr_timer) lv_display_refr_timer(d->refr_timer);
            d = lv_display_get_next(d);
        }
    }
}

void lv_obj_redraw(lv_layer_t * layer, lv_obj_t * obj)
{
    LV_PROFILER_REFR_BEGIN;
    lv_area_t clip_area_ori = layer->_clip_area;
    lv_area_t clip_coords_for_obj;

    /*Виджет будет отображен.
     *Поэтому сеттеры теперь должны использовать анимацию. */
    obj->rendered = 1;

    /*Обрезать область отсечения до области`obj size + ext size`.*/
    lv_area_t obj_coords_ext;
    lv_obj_get_coords(obj, &obj_coords_ext);
    int32_t ext_draw_size = lv_obj_get_ext_draw_size(obj);
    lv_area_increase(&obj_coords_ext, ext_draw_size, ext_draw_size);

    if(!lv_area_intersect(&clip_coords_for_obj, &clip_area_ori, &obj_coords_ext)) {
        LV_PROFILER_REFR_END;
        return;
    }
    /*Если объект виден в текущей области клипа*/
    layer->_clip_area = clip_coords_for_obj;

    lv_obj_send_event(obj, LV_EVENT_DRAW_MAIN_BEGIN, layer);
    lv_obj_send_event(obj, LV_EVENT_DRAW_MAIN, layer);
    lv_obj_send_event(obj, LV_EVENT_DRAW_MAIN_END, layer);
#if LV_USE_REFR_DEBUG
    lv_color_t debug_color = lv_color_make(lv_rand(0, 0xFF), lv_rand(0, 0xFF), lv_rand(0, 0xFF));
    lv_draw_rect_dsc_t draw_dsc;
    lv_draw_rect_dsc_init(&draw_dsc);
    draw_dsc.bg_color = debug_color;
    draw_dsc.bg_opa = LV_OPA_20;
    draw_dsc.border_width = 1;
    draw_dsc.border_opa = LV_OPA_30;
    draw_dsc.border_color = debug_color;
    lv_draw_rect(layer, &draw_dsc, &obj_coords_ext);
#endif

    const lv_area_t * obj_coords;
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE)) {
        obj_coords = &obj_coords_ext;
    }
    else {
        obj_coords = &obj->coords;
    }
    lv_area_t clip_coords_for_children;
    bool refr_children = true;
    if(!lv_area_intersect(&clip_coords_for_children, &layer->_clip_area, obj_coords) || layer->opa <= LV_OPA_MIN) {
        refr_children = false;
    }

    if(refr_children) {
        uint32_t i;
        uint32_t child_cnt = lv_obj_get_child_count(obj);
        if(child_cnt == 0) {
            /*Если объект был виден в области клипа, также вызовите события пост-рисования.*/
            /*Если все дочерние элементы перерисованы, сделайте отрисовку «после розыгрыша».*/
            lv_obj_send_event(obj, LV_EVENT_DRAW_POST_BEGIN, layer);
            lv_obj_send_event(obj, LV_EVENT_DRAW_POST, layer);
            lv_obj_send_event(obj, LV_EVENT_DRAW_POST_END, layer);
        }
        else {
            layer->_clip_area = clip_coords_for_children;
            bool clip_corner = lv_obj_get_style_clip_corner(obj, LV_PART_MAIN);

            int32_t radius = 0;
            if(clip_corner) {
                radius = lv_obj_get_style_radius(obj, LV_PART_MAIN);
                if(radius == 0) clip_corner = false;
            }

            if(clip_corner == false) {
                for(i = 0; i < child_cnt; i++) {
                    lv_obj_t * child = obj->spec_attr->children[i];
                    lv_obj_refr(layer, child);
                }

                /*Если объект был виден в области клипа, также вызовите события пост-рисования.*/
                /*Если все дочерние элементы перерисованы, сделайте отрисовку «после розыгрыша».*/
                lv_obj_send_event(obj, LV_EVENT_DRAW_POST_BEGIN, layer);
                lv_obj_send_event(obj, LV_EVENT_DRAW_POST, layer);
                lv_obj_send_event(obj, LV_EVENT_DRAW_POST_END, layer);
            }
            else {
                lv_layer_t * layer_children;
                lv_draw_mask_rect_dsc_t mask_draw_dsc;
                lv_draw_mask_rect_dsc_init(&mask_draw_dsc);
                mask_draw_dsc.radius = radius;
                mask_draw_dsc.area = obj->coords;

                lv_draw_image_dsc_t img_draw_dsc;
                lv_draw_image_dsc_init(&img_draw_dsc);

                int32_t short_side = LV_MIN(lv_area_get_width(&obj->coords), lv_area_get_height(&obj->coords));
                int32_t rout = LV_MIN(radius, short_side >> 1);

                lv_area_t bottom = obj->coords;
                bottom.y1 = bottom.y2 - rout + 1;
                if(lv_area_intersect(&bottom, &bottom, &layer->_clip_area)) {
                    layer_children = lv_draw_layer_create(layer, LV_COLOR_FORMAT_ARGB8888, &bottom);

                    for(i = 0; i < child_cnt; i++) {
                        lv_obj_t * child = obj->spec_attr->children[i];
                        lv_obj_refr(layer_children, child);
                    }

                    /*Если все дочерние элементы перерисованы, отправьте розыгрыш после розыгрыша.*/
                    lv_obj_send_event(obj, LV_EVENT_DRAW_POST_BEGIN, layer_children);
                    lv_obj_send_event(obj, LV_EVENT_DRAW_POST, layer_children);
                    lv_obj_send_event(obj, LV_EVENT_DRAW_POST_END, layer_children);

                    lv_draw_mask_rect(layer_children, &mask_draw_dsc);

                    img_draw_dsc.src = layer_children;
                    lv_draw_layer(layer, &img_draw_dsc, &bottom);
                }

                lv_area_t top = obj->coords;
                top.y2 = top.y1 + rout - 1;
                if(lv_area_intersect(&top, &top, &layer->_clip_area)) {
                    layer_children = lv_draw_layer_create(layer, LV_COLOR_FORMAT_ARGB8888, &top);

                    for(i = 0; i < child_cnt; i++) {
                        lv_obj_t * child = obj->spec_attr->children[i];
                        lv_obj_refr(layer_children, child);
                    }

                    /*Если все дочерние элементы перерисованы, отправьте розыгрыш после розыгрыша.*/
                    lv_obj_send_event(obj, LV_EVENT_DRAW_POST_BEGIN, layer_children);
                    lv_obj_send_event(obj, LV_EVENT_DRAW_POST, layer_children);
                    lv_obj_send_event(obj, LV_EVENT_DRAW_POST_END, layer_children);

                    lv_draw_mask_rect(layer_children, &mask_draw_dsc);

                    img_draw_dsc.src = layer_children;
                    lv_draw_layer(layer, &img_draw_dsc, &top);

                }

                lv_area_t mid = obj->coords;
                mid.y1 += rout;
                mid.y2 -= rout;
                if(lv_area_intersect(&mid, &mid, &layer->_clip_area)) {
                    layer->_clip_area = mid;
                    for(i = 0; i < child_cnt; i++) {
                        lv_obj_t * child = obj->spec_attr->children[i];
                        lv_obj_refr(layer, child);
                    }

                    /*Если все дочерние элементы перерисованы, сделайте отрисовку «после розыгрыша».*/
                    lv_obj_send_event(obj, LV_EVENT_DRAW_POST_BEGIN, layer);
                    lv_obj_send_event(obj, LV_EVENT_DRAW_POST, layer);
                    lv_obj_send_event(obj, LV_EVENT_DRAW_POST_END, layer);

                }

            }
        }
    }

    layer->_clip_area = clip_area_ori;

    LV_PROFILER_REFR_END;
}

lv_result_t lv_inv_area(lv_display_t * disp, const lv_area_t * area_p)
{
    if(!disp) disp = lv_display_get_default();
    if(!disp) return LV_RESULT_INVALID;
    if(!lv_display_is_invalidation_enabled(disp)) return LV_RESULT_INVALID;

    /**
     * Есть две причины этой проблемы:
     *  1. LVGL API используется в разных потоках, например, для изменения свойств виджета в другом потоке.
     *    или внутри обработчика прерываний во время процесса рендеринга основного потока.
     *  2. Виджет, настраиваемый пользователем, снова изменяет свойства/стили виджета в событии DRAW.
     *
     * Поэтому убедитесь, что LVGL используется в однопоточном режиме, или обратитесь к
     * documentation: https://docs.lvgl.io/master/porting/os.html для правильных механизмов блокировки.
     * Кроме того, убедитесь, что в рамках события DRAW выполняются только задачи, связанные с чертежами.
     * и перенесите изменения свойств/стилей виджета в другие события.
     */
    LV_ASSERT_MSG(!disp->rendering_in_progress, "Invalidate area is not allowed during rendering.");

    /*Очистите буфер аннулирования, если параметр равен NULL.*/
    if(area_p == NULL) {
        disp->inv_p = 0;
        return LV_RESULT_OK;
    }

    lv_area_t scr_area;
    scr_area.x1 = 0;
    scr_area.y1 = 0;
    scr_area.x2 = lv_display_get_horizontal_resolution(disp) - 1;
    scr_area.y2 = lv_display_get_vertical_resolution(disp) - 1;

    lv_area_t com_area;
    bool suc;

    suc = lv_area_intersect(&com_area, area_p, &scr_area);
    if(suc == false)  return LV_RESULT_INVALID; /*За пределами экрана*/

    if(disp->color_format == LV_COLOR_FORMAT_I1) {
        /*Убедитесь, что координаты X начинаются и заканчиваются на границе байта.
         *например преобразовать 11;27 в 8;31*/
        com_area.x1 &= ~0x7; /*Округление вниз: Nx8*/
        com_area.x2 |= 0x7;    /*Округление вверх: Кx8 - 1*/
    }

    /*Если в режиме полного обновления была хотя бы одна недопустимая область, перерисуйте весь экран.*/
    if(disp->render_mode == LV_DISPLAY_RENDER_MODE_FULL) {
        disp->inv_areas[0] = scr_area;
        disp->inv_p = 1;
        lv_display_send_event(disp, LV_EVENT_REFR_REQUEST, NULL);
        return LV_RESULT_OK;
    }

    lv_result_t res = lv_display_send_event(disp, LV_EVENT_INVALIDATE_AREA, &com_area);
    if(res != LV_RESULT_OK) return LV_RESULT_INVALID;

    /*Сохранять только в том случае, если этой области нет ни в одной из сохраненных областей.*/
    uint16_t i;
    for(i = 0; i < disp->inv_p; i++) {
        if(lv_area_is_in(&com_area, &disp->inv_areas[i], 0) != false) return LV_RESULT_OK;
    }

    /*Сохранить территорию*/
    lv_area_t * tmp_area_p = &com_area;
    if(disp->inv_p >= LV_INV_BUF_SIZE) { /*Если нет места для области добавьте ширму*/
        disp->inv_p = 0;
        tmp_area_p = &scr_area;
    }
    lv_area_copy(&disp->inv_areas[disp->inv_p], tmp_area_p);
    disp->inv_p++;

    lv_display_send_event(disp, LV_EVENT_REFR_REQUEST, NULL);

    return LV_RESULT_OK;
}

/**
 * Получить дисплей, который обновляется
 * @return дисплей обновляется
 */
lv_display_t * lv_refr_get_disp_refreshing(void)
{
    return disp_refr;
}

/**
 * Получить дисплей, который обновляется
 * @return дисплей обновляется
 */
void lv_refr_set_disp_refreshing(lv_display_t * disp)
{
    disp_refr = disp;
}

void lv_display_refr_timer(lv_timer_t * tmr)
{
    LV_PROFILER_REFR_BEGIN;
    LV_TRACE_REFR("begin");

    if(tmr) {
        disp_refr = tmr->user_data;
        /* Убедитесь, что таймер не запускается снова автоматически.
         * Это делается перед обновлением на случай, если обновление сделает что-то еще недействительным.
         * Однако, если монитор производительности включен, оставьте таймер включенным для подсчета FPS .*/
#if !LV_USE_PERF_MONITOR
        lv_timer_pause(tmr);
#endif
    }
    else {
        disp_refr = lv_display_get_default();
    }

    if(disp_refr == NULL) {
        LV_LOG_WARN("No display registered");
        LV_PROFILER_REFR_END;
        return;
    }

    lv_draw_buf_t * buf_act = disp_refr->buf_act;
    if(!(buf_act && buf_act->data && buf_act->data_size)) {
        LV_LOG_WARN("No draw buffer");
        LV_PROFILER_REFR_END;
        return;
    }

    lv_result_t res = lv_display_send_event(disp_refr, LV_EVENT_REFR_START, NULL);
    if(res == LV_RESULT_INVALID) {
        LV_TRACE_REFR("deleted");
        LV_PROFILER_REFR_END;
        return;
    }

    /*При необходимости обновите макет экрана.*/
    LV_PROFILER_LAYOUT_BEGIN_TAG("layout");
    lv_obj_update_layout(disp_refr->act_scr);
    if(disp_refr->prev_scr) lv_obj_update_layout(disp_refr->prev_scr);

    lv_obj_update_layout(disp_refr->bottom_layer);
    lv_obj_update_layout(disp_refr->top_layer);
    lv_obj_update_layout(disp_refr->sys_layer);
    LV_PROFILER_LAYOUT_END_TAG("layout");

    /*Ничего не делайте, если нет активного экрана*/
    if(disp_refr->act_scr == NULL) {
        disp_refr->inv_p = 0;
        LV_LOG_WARN("there is no active screen");
        goto refr_finish;
    }

    lv_refr_join_area();
    refr_sync_areas();
    refr_invalid_areas();

    if(disp_refr->inv_p == 0) goto refr_finish;
    /*В прямом режиме с двойной буферизацией сохраните обновленные области.
     *Они будут использоваться при следующем вызове для синхронизации буферов.*/
    if(lv_display_is_double_buffered(disp_refr) && disp_refr->render_mode == LV_DISPLAY_RENDER_MODE_DIRECT) {
        uint32_t i;
        for(i = 0; i < disp_refr->inv_p; i++) {
            if(disp_refr->inv_area_joined[i])
                continue;

            lv_area_t * sync_area = lv_ll_ins_tail(&disp_refr->sync_areas);
            *sync_area = disp_refr->inv_areas[i];
        }
    }

    lv_memzero(disp_refr->inv_areas, sizeof(disp_refr->inv_areas));
    lv_memzero(disp_refr->inv_area_joined, sizeof(disp_refr->inv_area_joined));
    disp_refr->inv_p = 0;

refr_finish:

#if LV_DRAW_SW_COMPLEX == 1
    lv_draw_sw_mask_cleanup();
#endif

    lv_display_send_event(disp_refr, LV_EVENT_REFR_READY, NULL);

    LV_TRACE_REFR("finished");
    LV_PROFILER_REFR_END;
}

/**
 * Найдите самый верхний объект, который полностью покрывает область.
 * @param area_p указатель на область
 * @param obj первый объект, с которого начинается поиск (обычно экран)
 * @return
 */
lv_obj_t * lv_refr_get_top_obj(const lv_area_t * area_p, lv_obj_t * obj)
{
    lv_obj_t * found_p = NULL;

    if(lv_area_is_in(area_p, &obj->coords, 0) == false) return NULL;
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN)) return NULL;
    if(lv_obj_get_layer_type(obj) != LV_LAYER_TYPE_NONE) return NULL;
    if(lv_obj_get_style_opa(obj, LV_PART_MAIN) < LV_OPA_MAX) return NULL;

    /*Если этот объект полностью закрывает область рисования, проверьте и детей.*/
    lv_cover_check_info_t info;
    info.res = LV_COVER_RES_COVER;
    info.area = area_p;
    lv_obj_send_event(obj, LV_EVENT_COVER_CHECK, &info);
    if(info.res == LV_COVER_RES_MASKED) return NULL;

    int32_t i;
    int32_t child_cnt = lv_obj_get_child_count(obj);
    for(i = child_cnt - 1; i >= 0; i--) {
        lv_obj_t * child = obj->spec_attr->children[i];
        found_p = lv_refr_get_top_obj(area_p, child);

        /*Если с детьми все в порядке, то перерыв*/
        if(found_p != NULL) {
            break;
        }
    }

    /*Если никто из лучших детей не использует этот предмет*/
    if(found_p == NULL && info.res == LV_COVER_RES_COVER) {
        found_p = obj;
    }

    return found_p;
}


void lv_obj_refr(lv_layer_t * layer, lv_obj_t * obj)
{
    LV_ASSERT_NULL(layer);
    LV_ASSERT_NULL(obj);
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN)) return;

    /*Если`opa_layered != LV_OPA_COVER`нарисуйте виджет нового слоя и включите этот слой с заданной непрозрачностью.*/
    const lv_opa_t opa_layered = lv_obj_get_style_opa_layered(obj, LV_PART_MAIN);
    if(opa_layered <= LV_OPA_MIN) return;

    const lv_opa_t layer_opa_ori = layer->opa;
    const lv_color32_t layer_recolor = layer->recolor;

    /*Обычный`opa`(немногослойный) просто уменьшит`bg_opa`,`text_opa`и т. д. д. на следующих рисунках.*/
    const lv_opa_t opa_main = lv_obj_get_style_opa(obj, LV_PART_MAIN);
    if(opa_main < LV_OPA_MAX) {
        layer->opa = LV_OPA_MIX2(layer_opa_ori, opa_main);
    }

    layer->recolor = lv_obj_style_apply_recolor(obj, LV_PART_MAIN, layer->recolor);

    lv_layer_type_t layer_type = lv_obj_get_layer_type(obj);
    if(layer_type == LV_LAYER_TYPE_NONE) {
        lv_obj_redraw(layer, obj);
    }
#if LV_DRAW_TRANSFORM_USE_MATRIX
    /*Если опа слоя заполнена, используйте матричное преобразование.*/
    else if(opa_layered >= LV_OPA_MAX && !refr_check_obj_clip_overflow(layer, obj)) {
        refr_obj_matrix(layer, obj);
    }
#endif /* LV_DRAW_TRANSFORM_USE_MATRIX */
    else {
        lv_area_t layer_area_full;
        lv_area_t obj_draw_size;
        lv_result_t res = layer_get_area(layer, obj, layer_type, &layer_area_full, &obj_draw_size);
        if(res != LV_RESULT_OK) {
            layer->opa = layer_opa_ori;
            layer->recolor = layer_recolor;
            return;
        }

        /*Простые слои можно разделить на более мелкие слои.*/
        uint32_t max_rgb_row_height = lv_area_get_height(&layer_area_full);
        uint32_t max_argb_row_height = lv_area_get_height(&layer_area_full);
        if(layer_type == LV_LAYER_TYPE_SIMPLE) {
            int32_t w = lv_area_get_width(&layer_area_full);
            uint8_t px_size = lv_color_format_get_size(disp_refr->color_format);
            max_rgb_row_height = LV_DRAW_LAYER_SIMPLE_BUF_SIZE / w / px_size;
            max_argb_row_height = LV_DRAW_LAYER_SIMPLE_BUF_SIZE / w / sizeof(lv_color32_t);
        }

        lv_area_t layer_area_act;
        layer_area_act.x1 = layer_area_full.x1;
        layer_area_act.x2 = layer_area_full.x2;
        layer_area_act.y1 = layer_area_full.y1;
        layer_area_act.y2 = layer_area_full.y1;

        while(layer_area_act.y2 < layer_area_full.y2) {
            /* Тестирование с размером слоя RGB (который больше размера слоя ARGB)
             * Если альфа действительно не нужна, используйте ее. В противном случае переключитесь на размер ARGB.*/
            layer_area_act.y2 = layer_area_act.y1 + max_rgb_row_height - 1;
            if(layer_area_act.y2 > layer_area_full.y2) layer_area_act.y2 = layer_area_full.y2;

            const void * bitmap_mask_src = lv_obj_get_style_bitmap_mask_src(obj, LV_PART_MAIN);
            bool area_need_alpha = bitmap_mask_src || alpha_test_area_on_obj(obj, &layer_area_act);

            if(area_need_alpha) {
                layer_area_act.y2 = layer_area_act.y1 + max_argb_row_height - 1;
                if(layer_area_act.y2 > layer_area_full.y2) layer_area_act.y2 = layer_area_full.y2;
            }

            lv_layer_t * new_layer = lv_draw_layer_create(layer,
                                                          area_need_alpha ? LV_COLOR_FORMAT_ARGB8888 : LV_COLOR_FORMAT_NATIVE, &layer_area_act);
            lv_obj_redraw(new_layer, obj);

            lv_point_t pivot = {
                .x = lv_obj_get_style_transform_pivot_x(obj, LV_PART_MAIN),
                .y = lv_obj_get_style_transform_pivot_y(obj, LV_PART_MAIN)
            };

            if(LV_COORD_IS_PCT(pivot.x)) {
                pivot.x = (LV_COORD_GET_PCT(pivot.x) * lv_area_get_width(&obj->coords)) / 100;
            }
            if(LV_COORD_IS_PCT(pivot.y)) {
                pivot.y = (LV_COORD_GET_PCT(pivot.y) * lv_area_get_height(&obj->coords)) / 100;
            }

            lv_draw_image_dsc_t layer_draw_dsc;
            lv_draw_image_dsc_init(&layer_draw_dsc);
            layer_draw_dsc.pivot.x = obj->coords.x1 + pivot.x - new_layer->buf_area.x1;
            layer_draw_dsc.pivot.y = obj->coords.y1 + pivot.y - new_layer->buf_area.y1;

            layer_draw_dsc.opa = opa_layered;
            layer_draw_dsc.rotation = lv_obj_get_style_transform_rotation(obj, LV_PART_MAIN);
            while(layer_draw_dsc.rotation > 3600) layer_draw_dsc.rotation -= 3600;
            while(layer_draw_dsc.rotation < 0) layer_draw_dsc.rotation += 3600;
            layer_draw_dsc.scale_x = lv_obj_get_style_transform_scale_x(obj, LV_PART_MAIN);
            layer_draw_dsc.scale_y = lv_obj_get_style_transform_scale_y(obj, LV_PART_MAIN);
            layer_draw_dsc.skew_x = lv_obj_get_style_transform_skew_x(obj, LV_PART_MAIN);
            layer_draw_dsc.skew_y = lv_obj_get_style_transform_skew_y(obj, LV_PART_MAIN);
            layer_draw_dsc.blend_mode = lv_obj_get_style_blend_mode(obj, LV_PART_MAIN);
            layer_draw_dsc.antialias = disp_refr->antialiasing;
            layer_draw_dsc.bitmap_mask_src = bitmap_mask_src;
            layer_draw_dsc.image_area = obj_draw_size;
            layer_draw_dsc.src = new_layer;

            lv_draw_layer(layer, &layer_draw_dsc, &layer_area_act);

            layer_area_act.y1 = layer_area_act.y2 + 1;
        }
    }

    /* Восстановите исходный опа слоя и перекрасьте его. */
    layer->opa = layer_opa_ori;
    layer->recolor = layer_recolor;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Присоединяйтесь к областям, имеющим общие части
 */
static void lv_refr_join_area(void)
{
    LV_PROFILER_REFR_BEGIN;
    uint32_t join_from;
    uint32_t join_in;
    lv_area_t joined_area;
    for(join_in = 0; join_in < disp_refr->inv_p; join_in++) {
        if(disp_refr->inv_area_joined[join_in] != 0) continue;

        /*Проверьте все области, которые относятся к ним в «join_in».*/
        for(join_from = 0; join_from < disp_refr->inv_p; join_from++) {
            /*Обрабатывать только несвязанные области и игнорировать себя.*/
            if(disp_refr->inv_area_joined[join_from] != 0 || join_in == join_from) {
                continue;
            }

            /*Проверьте, находятся ли области друг на друге*/
            if(lv_area_is_on(&disp_refr->inv_areas[join_in], &disp_refr->inv_areas[join_from]) == false) {
                continue;
            }

            lv_area_join(&joined_area, &disp_refr->inv_areas[join_in], &disp_refr->inv_areas[join_from]);

            /*Объединяйте две области только в том случае, если размер объединяемой области меньше.*/
            if(lv_area_get_size(&joined_area) < (lv_area_get_size(&disp_refr->inv_areas[join_in]) +
                                                 lv_area_get_size(&disp_refr->inv_areas[join_from]))) {
                lv_area_copy(&disp_refr->inv_areas[join_in], &joined_area);

                /*Маркировка «join_form» объединяется с «join_in».*/
                disp_refr->inv_area_joined[join_from] = 1;
            }
        }
    }
    LV_PROFILER_REFR_END;
}

/**
 * Обновите области синхронизации.
 */
static void refr_sync_areas(void)
{
    /*Не синхронизировать, если не используется прямая или двойная буферизация.*/
    if(disp_refr->render_mode != LV_DISPLAY_RENDER_MODE_DIRECT) return;

    /*Не синхронизировать, если нет двойной буферизации*/
    if(!lv_display_is_double_buffered(disp_refr)) return;

    /*Не синхронизировать, если нет областей синхронизации*/
    if(lv_ll_is_empty(&disp_refr->sync_areas)) return;

    LV_PROFILER_REFR_BEGIN;
    /*В режиме прямой двойной буферизации синхронизируйте визуализированные области с другим буфером.*/
    /*Здесь нужно дождаться готовности, чтобы не испортить активный экран*/
    wait_for_flushing(disp_refr);

    /*Буферы уже поменяны местами.
     *Таким образом, активный буфер — это внеэкранный буфер, в котором LVGL будет отображать*/
    lv_draw_buf_t * off_screen = disp_refr->buf_act;
    /*Тройной буфер синхронизации для обновлений вне экрана2.*/
    lv_draw_buf_t * off_screen2;
    lv_draw_buf_t * on_screen;

    if(disp_refr->buf_act == disp_refr->buf_1) {
        off_screen2 = disp_refr->buf_2;
        on_screen = disp_refr->buf_3 ? disp_refr->buf_3 : disp_refr->buf_2;
    }
    else if(disp_refr->buf_act == disp_refr->buf_2) {
        off_screen2 = disp_refr->buf_3 ? disp_refr->buf_3 : disp_refr->buf_1;
        on_screen = disp_refr->buf_1;
    }
    else {
        off_screen2 = disp_refr->buf_1;
        on_screen = disp_refr->buf_2;
    }

    uint32_t hor_res = lv_display_get_horizontal_resolution(disp_refr);
    uint32_t ver_res = lv_display_get_vertical_resolution(disp_refr);

    /*Переберите недействительные области, чтобы увидеть, следует ли копировать область синхронизации.*/
    uint16_t i;
    int8_t j;
    lv_area_t res[4] = {0};
    int8_t res_c;
    lv_area_t * sync_area, * new_area, * next_area;
    for(i = 0; i < disp_refr->inv_p; i++) {
        /*Пропускать соединенные области*/
        if(disp_refr->inv_area_joined[i]) continue;

        /*Перебирать области синхронизации*/
        sync_area = lv_ll_get_head(&disp_refr->sync_areas);
        while(sync_area != NULL) {
            /*Получить следующую область синхронизации*/
            next_area = lv_ll_get_next(&disp_refr->sync_areas, sync_area);

            /*Удалить пересечение области перерисовки из области синхронизации и получить оставшиеся области.*/
            res_c = lv_area_diff(res, sync_area, &disp_refr->inv_areas[i]);

            /*Новые подобласти, созданные после удаления пересечения*/
            if(res_c != -1) {
                /*Заменить старую область синхронизации новыми областями*/
                for(j = 0; j < res_c; j++) {
                    new_area = lv_ll_ins_prev(&disp_refr->sync_areas, sync_area);
                    *new_area = res[j];
                }
                lv_ll_remove(&disp_refr->sync_areas, sync_area);
                lv_free(sync_area);
            }

            /*Перейти к следующей области синхронизации*/
            sync_area = next_area;
        }
    }

    lv_area_t disp_area = {0, 0, (int32_t)hor_res - 1, (int32_t)ver_res - 1};
    /*Скопировать области синхронизации (если они остались)*/
    for(sync_area = lv_ll_get_head(&disp_refr->sync_areas); sync_area != NULL;
        sync_area = lv_ll_get_next(&disp_refr->sync_areas, sync_area)) {
        /**
         * @todo Изменение размера окнаSDLприческа к себе, посколькуsync_areaбольше, чем disp_area
         */
        if(!lv_area_intersect(sync_area, sync_area, &disp_area)) {
            continue;
        }
#if LV_DRAW_TRANSFORM_USE_MATRIX
        if(lv_display_get_matrix_rotation(disp_refr)) {
            lv_display_rotate_area(disp_refr, sync_area);
        }
#endif
        lv_draw_buf_copy(off_screen, sync_area, on_screen, sync_area);
        if(off_screen2 != on_screen)
            lv_draw_buf_copy(off_screen2, sync_area, on_screen, sync_area);
    }

    /*Очистить области синхронизации*/
    lv_ll_clear(&disp_refr->sync_areas);
    LV_PROFILER_REFR_END;
}

/**
 * Обновить соединенные области
 */
static void refr_invalid_areas(void)
{
    if(disp_refr->inv_p == 0) return;
    LV_PROFILER_REFR_BEGIN;

    /*Уведомить о начале рендеринга на основе дисплея*/
    lv_display_send_event(disp_refr, LV_EVENT_RENDER_START, NULL);

    /*Найдите последнюю область, которая будет нарисована*/
    int32_t i;
    int32_t last_i = 0;
    for(i = disp_refr->inv_p - 1; i >= 0; i--) {
        if(disp_refr->inv_area_joined[i] == 0) {
            last_i = i;
            break;
        }
    }

    disp_refr->last_area = 0;
    disp_refr->last_part = 0;
    disp_refr->rendering_in_progress = true;

    for(i = 0; i < (int32_t)disp_refr->inv_p; i++) {
        /*Обновить несвязанные области*/
        if(disp_refr->inv_area_joined[i]) continue;

        if(i == last_i) disp_refr->last_area = 1;
        disp_refr->last_part = 0;

        lv_area_t inv_a = disp_refr->inv_areas[i];
        if(disp_refr->render_mode == LV_DISPLAY_RENDER_MODE_PARTIAL) {
            /*Вычислить максимальное количество строк*/
            int32_t w = lv_area_get_width(&inv_a);
            int32_t h = lv_area_get_height(&inv_a);

            int32_t max_row = get_max_row(disp_refr, w, h);

            int32_t row;
            int32_t row_last = 0;
            lv_area_t sub_area;
            sub_area.x1 = inv_a.x1;
            sub_area.x2 = inv_a.x2;
            int32_t y_off = 0;
            for(row = inv_a.y1; row + max_row - 1 <= inv_a.y2; row += max_row) {
                /*Вычисление следующих координат по draw_buf*/
                sub_area.y1 = row;
                sub_area.y2 = row + max_row - 1;
                if(sub_area.y2 > inv_a.y2) sub_area.y2 = inv_a.y2;
                row_last = sub_area.y2;
                if(inv_a.y2 == row_last) disp_refr->last_part = 1;
                refr_area(&sub_area, y_off);
                y_off += lv_area_get_height(&sub_area);
                draw_buf_flush(disp_refr);
            }

            /*Если последние координаты y еще не обработаны...*/
            if(inv_a.y2 != row_last) {
                /*Вычисление следующих координат по draw_buf*/
                sub_area.y1 = row;
                sub_area.y2 = inv_a.y2;
                disp_refr->last_part = 1;
                refr_area(&sub_area, y_off);
                y_off += lv_area_get_height(&sub_area);
                draw_buf_flush(disp_refr);
            }
        }
        else if(disp_refr->render_mode == LV_DISPLAY_RENDER_MODE_FULL ||
                disp_refr->render_mode == LV_DISPLAY_RENDER_MODE_DIRECT) {
            disp_refr->last_part = 1;
            refr_area(&disp_refr->inv_areas[i], 0);
            draw_buf_flush(disp_refr);
        }
    }

    lv_display_send_event(disp_refr, LV_EVENT_RENDER_READY, NULL);
    disp_refr->rendering_in_progress = false;
    LV_PROFILER_REFR_END;
}

/**
 * При необходимости измените форму буфера прорисовки.
 * @param layer  указатель на слой, который будет нарисован
 */
static void layer_reshape_draw_buf(lv_layer_t * layer, uint32_t stride)
{
    lv_draw_buf_t * ret = lv_draw_buf_reshape(
                              layer->draw_buf,
                              layer->color_format,
                              lv_area_get_width(&layer->buf_area),
                              lv_area_get_height(&layer->buf_area),
                              stride);
    LV_UNUSED(ret);
    LV_ASSERT_NULL(ret);
}

/**
 * Обновить область, если имеется буфер виртуального отображения.
 * @param area_p  указатель на область для обновления
 */
static void refr_area(const lv_area_t * area_p, int32_t y_offset)
{
    LV_PROFILER_REFR_BEGIN;
    lv_layer_t * layer = disp_refr->layer_head;
    layer->draw_buf = disp_refr->buf_act;
    layer->_clip_area = *area_p;
    layer->phy_clip_area = *area_p;
    layer->partial_y_offset = y_offset;
    layer->all_tasks_added = false;

    if(disp_refr->render_mode == LV_DISPLAY_RENDER_MODE_PARTIAL) {
        /*В частичном режиме визуализируйте эту область в буфер.*/
        layer->buf_area = *area_p;
        layer_reshape_draw_buf(layer, LV_STRIDE_AUTO);
    }
    else if(disp_refr->render_mode == LV_DISPLAY_RENDER_MODE_DIRECT ||
            disp_refr->render_mode == LV_DISPLAY_RENDER_MODE_FULL) {
        /*В прямом и полном режиме буферная область всегда занимает весь экран, без учета поворота.*/
        layer->buf_area.x1 = 0;
        layer->buf_area.y1 = 0;
        if(lv_display_get_matrix_rotation(disp_refr)) {
            layer->buf_area.x2 = lv_display_get_original_horizontal_resolution(disp_refr) - 1;
            layer->buf_area.y2 = lv_display_get_original_vertical_resolution(disp_refr) - 1;
        }
        else {
            layer->buf_area.x2 = lv_display_get_horizontal_resolution(disp_refr) - 1;
            layer->buf_area.y2 = lv_display_get_vertical_resolution(disp_refr) - 1;
        }
        layer_reshape_draw_buf(layer, disp_refr->stride_is_auto ? LV_STRIDE_AUTO : layer->draw_buf->header.stride);
    }

    /*Попробуйте разделить площадь на более мелкие плитки.*/
    uint32_t tile_cnt = 1;
    int32_t tile_h = lv_area_get_height(area_p);
    if(LV_COLOR_FORMAT_IS_INDEXED(layer->color_format) == false) {
        /* Предположим, что размер буфера (может быть размером с экран или меньше в случае частичного режима)
         * и максимальный размер плитки являются оптимальным сценарием. Исходя из этого, рассчитайте идеальный размер плитки.
         * и установите количество плиток и высоту плитки соответственно.
         */
        uint32_t max_tile_cnt = disp_refr->tile_cnt;
        uint32_t total_buf_size = layer->draw_buf->data_size;
        uint32_t ideal_tile_size = total_buf_size / max_tile_cnt;
        uint32_t area_buf_size = lv_area_get_size(area_p) * lv_color_format_get_size(layer->color_format);

        tile_cnt = (area_buf_size + (ideal_tile_size - 1)) / ideal_tile_size; /*Округлить вверх*/
        tile_h = lv_area_get_height(area_p) / tile_cnt;
    }

    if(tile_cnt == 1) {
        refr_configured_layer(layer);
        layer->all_tasks_added = true;
    }
    else {
        /* Не рисуйте в буфере слоев дисплея, а создавайте фиктивные слои меньшего размера, которые используют
         * буфер слоя дисплея. Это будут плитки. При использовании плиток более вероятно, что
         * быть независимыми зонами для каждой вытяжной единицы. */
        lv_layer_t * tile_layers = lv_malloc(tile_cnt * sizeof(lv_layer_t));
        LV_ASSERT_MALLOC(tile_layers);
        if(tile_layers == NULL) {
            disp_refr->refreshed_area = *area_p;
            LV_PROFILER_REFR_END;
            return;
        }
        uint32_t i;
        for(i = 0; i < tile_cnt; i++) {
            lv_area_t tile_area;
            lv_area_set(&tile_area, area_p->x1, area_p->y1 + i * tile_h,
                        area_p->x2, area_p->y1 + (i + 1) * tile_h - 1);

            if(i == tile_cnt - 1) {
                tile_area.y2 = area_p->y2;
            }

            lv_layer_t * tile_layer = &tile_layers[i];
            lv_draw_layer_init(tile_layer, NULL, layer->color_format, &tile_area);
            tile_layer->buf_area = layer->buf_area; /*буфер все еще большой*/
            tile_layer->draw_buf = layer->draw_buf;
            refr_configured_layer(tile_layer);
            tile_layer->all_tasks_added = true;
        }


        /*Подождите, пока все плитки будут готовы, и уничтожьте их.*/
        for(i = 0; i < tile_cnt; i++) {
            lv_layer_t * tile_layer = &tile_layers[i];
            while(tile_layer->draw_task_head) {
                lv_draw_dispatch_wait_for_request();
                lv_draw_dispatch();
            }

            lv_layer_t * layer_i = disp_refr->layer_head;
            while(layer_i) {
                if(layer_i->next == tile_layer) {
                    layer_i->next = tile_layer->next;
                    break;
                }
                layer_i = layer_i->next;
            }

            lv_draw_unit_send_event(NULL, LV_EVENT_CHILD_DELETED, tile_layer);
            if(disp_refr->layer_deinit) disp_refr->layer_deinit(disp_refr, tile_layer);
        }
        lv_free(tile_layers);

        layer->all_tasks_added = true;
    }

    disp_refr->refreshed_area = *area_p;
    LV_PROFILER_REFR_END;
}

static void refr_configured_layer(lv_layer_t * layer)
{
    LV_PROFILER_REFR_BEGIN;

    lv_layer_reset(layer);

#if LV_DRAW_TRANSFORM_USE_MATRIX
    if(lv_display_get_matrix_rotation(disp_refr)) {
        const lv_display_rotation_t rotation = lv_display_get_rotation(disp_refr);
        if(rotation != LV_DISPLAY_ROTATION_0) {
            lv_display_rotate_area(disp_refr, &layer->phy_clip_area);

            /**
             * Направление вращения экрана, определенное LVGL, противоположно углу рисования.
             * Используйте прямое присвоение матрицы, чтобы уменьшить потери точности и повысить эффективность.
             */
            switch(rotation) {
                case LV_DISPLAY_ROTATION_90:
                    /**
                     * lv_matrix_rotate (&слой->матрица, 270);
                     * lv_matrix_translate (&layer->матрица, -disp_refr->ver_res, 0);
                     */
                    layer->matrix.m[0][0] = 0;
                    layer->matrix.m[0][1] = 1;
                    layer->matrix.m[0][2] = 0;
                    layer->matrix.m[1][0] = -1;
                    layer->matrix.m[1][1] = 0;
                    layer->matrix.m[1][2] = disp_refr->ver_res;
                    break;

                case LV_DISPLAY_ROTATION_180:
                    /**
                     * lv_matrix_rotate (&слой->матрица, 180);
                     * lv_matrix_translate (&layer->матрица, -disp_refr->hor_res, -disp_refr->ver_res);
                     */
                    layer->matrix.m[0][0] = -1;
                    layer->matrix.m[0][1] = 0;
                    layer->matrix.m[0][2] = disp_refr->hor_res;
                    layer->matrix.m[1][0] = 0;
                    layer->matrix.m[1][1] = -1;
                    layer->matrix.m[1][2] = disp_refr->ver_res;
                    break;

                case LV_DISPLAY_ROTATION_270:
                    /**
                     * lv_matrix_rotate (&слой->матрица, 90);
                     * lv_matrix_translate (&layer->matrix, 0, -disp_refr->hor_res);
                     */
                    layer->matrix.m[0][0] = 0;
                    layer->matrix.m[0][1] = -1;
                    layer->matrix.m[0][2] = disp_refr->hor_res;
                    layer->matrix.m[1][0] = 1;
                    layer->matrix.m[1][1] = 0;
                    layer->matrix.m[1][2] = 0;
                    break;

                default:
                    LV_LOG_WARN("Invalid rotation: %d", rotation);
                    break;
            }
        }
    }
#endif /* LV_DRAW_TRANSFORM_USE_MATRIX */

    /* В режиме с одной буферизацией подождите здесь, пока буфер не освободится.
     * В противном случае мы бы рисовали в буфере, пока он еще передается на дисплей.*/
    if(!lv_display_is_double_buffered(disp_refr)) {
        wait_for_flushing(disp_refr);
    }
    /*Если экран прозрачный, инициализируйте его, когда промывка будет готова.*/
    if(lv_color_format_has_alpha(disp_refr->color_format)) {
        lv_area_t clear_area = layer->_clip_area;
        lv_area_move(&clear_area, -layer->buf_area.x1, -layer->buf_area.y1);
        lv_draw_buf_clear(layer->draw_buf, &clear_area);
    }

    lv_obj_t * top_act_scr = NULL;
    lv_obj_t * top_prev_scr = NULL;

    /*Получите самый верхний объект, который не закрыт другими*/
    top_act_scr = lv_refr_get_top_obj(&layer->_clip_area, lv_display_get_screen_active(disp_refr));
    if(disp_refr->prev_scr) {
        top_prev_scr = lv_refr_get_top_obj(&layer->_clip_area, disp_refr->prev_scr);
    }

    /*Нарисуйте фон нижнего слоя, если верхнего объекта нет.*/
    if(top_act_scr == NULL && top_prev_scr == NULL) {
        refr_obj_and_children(layer, lv_display_get_layer_bottom(disp_refr));
    }

    if(disp_refr->draw_prev_over_act) {
        if(top_act_scr == NULL) top_act_scr = disp_refr->act_scr;
        refr_obj_and_children(layer, top_act_scr);

        /*Обновите предыдущий экран, если таковой имеется.*/
        if(disp_refr->prev_scr) {
            if(top_prev_scr == NULL) top_prev_scr = disp_refr->prev_scr;
            refr_obj_and_children(layer, top_prev_scr);
        }
    }
    else {
        /*Обновите предыдущий экран, если таковой имеется.*/
        if(disp_refr->prev_scr) {
            if(top_prev_scr == NULL) top_prev_scr = disp_refr->prev_scr;
            refr_obj_and_children(layer, top_prev_scr);
        }

        if(top_act_scr == NULL) top_act_scr = disp_refr->act_scr;
        refr_obj_and_children(layer, top_act_scr);
    }

    /*Также безоговорочно обновить верхний и системный слои.*/
    refr_obj_and_children(layer, lv_display_get_layer_top(disp_refr));
    refr_obj_and_children(layer, lv_display_get_layer_sys(disp_refr));

    LV_PROFILER_REFR_END;
}

/**
 * Сделайте обновление с объекта. Нарисуйте всех его детей и младших тоже.
 * @param top_p указатель на объекты. Начните рисунок с него.
 * @param mask_p указатель на область, объекты будут рисоваться только здесь
 */
static void refr_obj_and_children(lv_layer_t * layer, lv_obj_t * top_obj)
{
    /*Обычно всегда будетtop_obj(по крайней мере, экран)
     *но в особых случаях (например, если на экране есть альфа) этого не произойдет.
     *В этом случае используйте экран напрямую*/
    if(top_obj == NULL) top_obj = lv_display_get_screen_active(disp_refr);
    if(top_obj == NULL) return;  /*Не должно случиться*/

    LV_PROFILER_REFR_BEGIN;
    /*Нарисуйте «младшие» одноуровневые объекты, потому что они могут находиться на top_obj.*/
    lv_obj_t * parent;
    lv_obj_t * border_p = top_obj;

    parent = lv_obj_get_parent(top_obj);

    /*Вычислить перекрашивание перед родительским элементом*/
    if(parent) {
        layer->recolor = lv_obj_get_style_recolor_recursive(parent, LV_PART_MAIN);
    }

    /*Обновить верхний объект и его дочерние элементы.*/
    lv_obj_refr(layer, top_obj);

    /*Делайте, пока не доберетесь до экрана*/
    while(parent != NULL) {
        bool go = false;
        uint32_t i;
        uint32_t child_cnt = lv_obj_get_child_count(parent);
        for(i = 0; i < child_cnt; i++) {
            lv_obj_t * child = parent->spec_attr->children[i];
            if(!go) {
                if(child == border_p) go = true;
            }
            else {
                /*Обновить объекты*/
                lv_obj_refr(layer, child);
            }
        }

        /*Вызов функции пост розыгрыша родительского объекта.*/
        lv_obj_send_event(parent, LV_EVENT_DRAW_POST_BEGIN, (void *)layer);
        lv_obj_send_event(parent, LV_EVENT_DRAW_POST, (void *)layer);
        lv_obj_send_event(parent, LV_EVENT_DRAW_POST_END, (void *)layer);

        /*На новой границе будут последние родители,
         *так что «младшие» братья родителя будут обновлены*/
        border_p = parent;
        /*Перейти на уровень глубже*/
        parent = lv_obj_get_parent(parent);
    }
    LV_PROFILER_REFR_END;
}

static lv_result_t layer_get_area(lv_layer_t * layer, lv_obj_t * obj, lv_layer_type_t layer_type,
                                  lv_area_t * layer_area_out, lv_area_t * obj_draw_size_out)
{
    int32_t ext_draw_size = lv_obj_get_ext_draw_size(obj);
    lv_obj_get_coords(obj, obj_draw_size_out);
    lv_area_increase(obj_draw_size_out, ext_draw_size, ext_draw_size);

    if(layer_type == LV_LAYER_TYPE_TRANSFORM) {
        /*Получите преобразованную область и прикрепите ее к текущей области обрезки.
         *Эту область необходимо обновить на экране.*/
        lv_area_t clip_coords_for_obj;
        lv_area_t tranf_coords = *obj_draw_size_out;
        lv_obj_get_transformed_area(obj, &tranf_coords, LV_OBJ_POINT_TRANSFORM_FLAG_NONE);
        if(!lv_area_intersect(&clip_coords_for_obj, &layer->_clip_area, &tranf_coords)) {
            return LV_RESULT_INVALID;
        }

        /*Преобразуйте обратно (инвертируйте) преобразованную область.
         *Он подскажет, какую область нетрансформированного виджета необходимо перерисовать.
         *для того, чтобы охватить трансформированную территорию после трансформации.*/
        lv_area_t inverse_clip_coords_for_obj = clip_coords_for_obj;
        lv_obj_get_transformed_area(obj, &inverse_clip_coords_for_obj, LV_OBJ_POINT_TRANSFORM_FLAG_INVERSE);
        if(!lv_area_intersect(&inverse_clip_coords_for_obj, &inverse_clip_coords_for_obj, obj_draw_size_out)) {
            return LV_RESULT_INVALID;
        }

        *layer_area_out = inverse_clip_coords_for_obj;
        lv_area_increase(layer_area_out, 5, 5); /*Чтобы избежать ошибки округления*/
    }
    else if(layer_type == LV_LAYER_TYPE_SIMPLE) {
        lv_area_t clip_coords_for_obj;
        if(!lv_area_intersect(&clip_coords_for_obj, &layer->_clip_area, obj_draw_size_out)) {
            return LV_RESULT_INVALID;
        }
        *layer_area_out = clip_coords_for_obj;
    }
    else {
        LV_LOG_WARN("Unhandled layer type");
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;
}

static bool alpha_test_area_on_obj(lv_obj_t * obj, const lv_area_t * area)
{
    /*Проверьте наличие альфа, предполагая, что альфа отсутствует. Если это не помогло, вернитесь к рендерингу с альфа-версией.*/
    /*Если область слоя не полностью охватывает объект, она не сможет полностью покрыть его.*/
    if(!lv_area_is_on(area, &obj->coords)) return true;

    lv_cover_check_info_t info;
    info.res = LV_COVER_RES_COVER;
    info.area = area;
    lv_obj_send_event(obj, LV_EVENT_COVER_CHECK, &info);
    if(info.res == LV_COVER_RES_COVER) return false;
    else return true;
}

#if LV_DRAW_TRANSFORM_USE_MATRIX

static bool obj_get_matrix(lv_obj_t * obj, lv_matrix_t * matrix)
{
    lv_matrix_identity(matrix);

    const lv_matrix_t * obj_matrix = lv_obj_get_transform(obj);
    if(obj_matrix) {
        lv_matrix_translate(matrix, obj->coords.x1, obj->coords.y1);
        lv_matrix_multiply(matrix, obj_matrix);
        lv_matrix_translate(matrix, -obj->coords.x1, -obj->coords.y1);
        return true;
    }

    lv_point_t pivot = {
        .x = lv_obj_get_style_transform_pivot_x(obj, LV_PART_MAIN),
        .y = lv_obj_get_style_transform_pivot_y(obj, LV_PART_MAIN)
    };

    pivot.x = obj->coords.x1 + lv_pct_to_px(pivot.x, lv_area_get_width(&obj->coords));
    pivot.y = obj->coords.y1 + lv_pct_to_px(pivot.y, lv_area_get_height(&obj->coords));

    int32_t rotation = lv_obj_get_style_transform_rotation(obj, LV_PART_MAIN);
    int32_t scale_x = lv_obj_get_style_transform_scale_x(obj, LV_PART_MAIN);
    int32_t scale_y = lv_obj_get_style_transform_scale_y(obj, LV_PART_MAIN);
    int32_t skew_x = lv_obj_get_style_transform_skew_x(obj, LV_PART_MAIN);
    int32_t skew_y = lv_obj_get_style_transform_skew_y(obj, LV_PART_MAIN);

    if(scale_x <= 0 || scale_y <= 0) {
        /* NOT рисует, если масштаб отрицательный или нулевой */
        return false;
    }

    /* создать матрицу obj */
    lv_matrix_translate(matrix, pivot.x, pivot.y);
    if(rotation != 0) {
        lv_matrix_rotate(matrix, rotation * 0.1f);
    }

    if(scale_x != LV_SCALE_NONE || scale_y != LV_SCALE_NONE) {
        lv_matrix_scale(
            matrix,
            (float)scale_x / LV_SCALE_NONE,
            (float)scale_y / LV_SCALE_NONE
        );
    }

    if(skew_x != 0 || skew_y != 0) {
        lv_matrix_skew(matrix, skew_x, skew_y);
    }

    lv_matrix_translate(matrix, -pivot.x, -pivot.y);
    return true;
}

static void refr_obj_matrix(lv_layer_t * layer, lv_obj_t * obj)
{
    LV_PROFILER_REFR_BEGIN;
    lv_matrix_t obj_matrix;
    if(!obj_get_matrix(obj, &obj_matrix)) {
        /* NOT нарисовать, если матрица obj недоступна */
        LV_PROFILER_REFR_END;
        return;
    }

    lv_matrix_t matrix_inv;
    if(!lv_matrix_inverse(&matrix_inv, &obj_matrix)) {
        /* NOT рисует, если матрица не обратима */
        LV_PROFILER_REFR_END;
        return;
    }

    /* save original matrix */
    lv_matrix_t ori_matrix = layer->matrix;

    /* apply the obj matrix */
    lv_matrix_multiply(&layer->matrix, &obj_matrix);

    /* вычислить область отсечения без преобразования */
    lv_area_t clip_area = layer->_clip_area;
    lv_area_t clip_area_ori = layer->_clip_area;
    clip_area = lv_matrix_transform_area(&matrix_inv, &clip_area);

    /* увеличьте область обрезки на 1 пиксель, чтобы избежать ошибок округления */
    if(!lv_matrix_is_identity_or_translation(&obj_matrix)) {
        lv_area_increase(&clip_area, 1, 1);
    }

    layer->_clip_area = clip_area;

    /* redraw obj */
    lv_obj_redraw(layer, obj);

    /* restore original matrix */
    layer->matrix = ori_matrix;
    /* restore clip area */
    layer->_clip_area = clip_area_ori;
    LV_PROFILER_REFR_END;
}

static bool refr_check_obj_clip_overflow(lv_layer_t * layer, lv_obj_t * obj)
{
    if(lv_obj_get_style_transform_rotation(obj, LV_PART_MAIN) == 0) {
        return false;
    }

    /*Усечь область до объекта*/
    lv_area_t obj_coords;
    int32_t ext_size = lv_obj_get_ext_draw_size(obj);
    lv_area_copy(&obj_coords, &obj->coords);
    lv_area_increase(&obj_coords, ext_size, ext_size);

    lv_obj_get_transformed_area(obj, &obj_coords, LV_OBJ_POINT_TRANSFORM_FLAG_RECURSIVE);

    lv_area_t clip_coords_for_obj;
    if(!lv_area_intersect(&clip_coords_for_obj, &layer->_clip_area, &obj_coords)) {
        return false;
    }

    bool has_clip = lv_memcmp(&clip_coords_for_obj, &obj_coords, sizeof(lv_area_t)) != 0;
    return has_clip;
}

#endif /* LV_DRAW_TRANSFORM_USE_MATRIX */

static uint32_t get_max_row(lv_display_t * disp, int32_t area_w, int32_t area_h)
{
    lv_color_format_t cf = disp->color_format;
    uint32_t stride = lv_draw_buf_width_to_stride(area_w, cf);
    uint32_t overhead = LV_COLOR_INDEXED_PALETTE_SIZE(cf) * sizeof(lv_color32_t);

    if(stride == 0) {
        LV_LOG_WARN("Invalid stride. Value is 0");
        return 0;
    }

    int32_t max_row = (uint32_t)(disp->buf_act->data_size - overhead) / stride;

    if(max_row > area_h) max_row = area_h;

    /*Округлите строкиdraw_buf, если добавлено округление.*/
    lv_area_t tmp;
    tmp.x1 = 0;
    tmp.x2 = 0;
    tmp.y1 = 0;

    int32_t h_tmp = max_row;
    do {
        tmp.y2 = h_tmp - 1;
        lv_display_send_event(disp_refr, LV_EVENT_INVALIDATE_AREA, &tmp);

        /*Если эта высота записана в `max_row`, то отлично*/
        if(lv_area_get_height(&tmp) <= max_row) break;

        /*Уменьшите высоту области до тех пор, пока она не впишется в`max_row`после округления.*/
        h_tmp--;
    } while(h_tmp > 0);

    if(h_tmp <= 0) {
        LV_LOG_WARN("Can't set draw_buf height using the round function. (Wrong round_cb or too "
                    "small draw_buf)");
        return 0;
    }
    else {
        max_row = tmp.y2 + 1;
    }

    return max_row;
}

/**
 * Очистить содержимое буфера отрисовки
 */
static void draw_buf_flush(lv_display_t * disp)
{
    /*Вывод визуализированного контента на дисплей*/
    lv_layer_t * layer = disp->layer_head;

    while(layer->draw_task_head) {
        lv_draw_dispatch_wait_for_request();
        lv_draw_dispatch();
    }

    /* В режиме двойной буферизации дождитесь освобождения другого буфера.
     * и драйвер готов принять новый буфер.
     * Если нам нужно здесь подождать, это означает, что содержимое одного буфера отправляется на отображение.
     * а другой буфер уже содержит новое визуализированное изображение. */
    if(lv_display_is_double_buffered(disp)) {
        wait_for_flushing(disp_refr);
    }

    disp->flushing = 1;

    if(disp->last_area && disp->last_part) disp->flushing_last = 1;
    else disp->flushing_last = 0;

    bool flushing_last = disp->flushing_last;

    if(disp->flush_cb) {
        call_flush_cb(disp, &disp->refreshed_area, layer->draw_buf->data);
    }
    /*Если есть 2 буфера, поменяйте их местами. При прямом переключении режима только на последней области*/
    if(lv_display_is_double_buffered(disp) && (disp->render_mode != LV_DISPLAY_RENDER_MODE_DIRECT || flushing_last)) {
        if(disp->buf_act == disp->buf_1) {
            disp->buf_act = disp->buf_2;
        }
        else if(disp->buf_act == disp->buf_2) {
            disp->buf_act = disp->buf_3 ? disp->buf_3 : disp->buf_1;
        }
        else {
            disp->buf_act = disp->buf_1;
        }
    }
}

static void call_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_PROFILER_REFR_BEGIN;
    LV_TRACE_REFR("Calling flush_cb on (%d;%d)(%d;%d) area with %p image pointer",
                  (int)area->x1, (int)area->y1, (int)area->x2, (int)area->y2, (void *)px_map);

    lv_area_t offset_area = {
        .x1 = area->x1 + disp->offset_x,
        .y1 = area->y1 + disp->offset_y,
        .x2 = area->x2 + disp->offset_x,
        .y2 = area->y2 + disp->offset_y
    };

    lv_display_send_event(disp, LV_EVENT_FLUSH_START, &offset_area);

    /*Для обратной совместимости поддержка LV_COLOR_16_SWAP (начиная с v8)*/
#if defined(LV_COLOR_16_SWAP) && LV_COLOR_16_SWAP
    lv_draw_sw_rgb565_swap(px_map, lv_area_get_size(&offset_area));
#endif

    disp->flush_cb(disp, &offset_area, px_map);
    lv_display_send_event(disp, LV_EVENT_FLUSH_FINISH, &offset_area);

    LV_PROFILER_REFR_END;
}

static void wait_for_flushing(lv_display_t * disp)
{
    LV_PROFILER_REFR_BEGIN;
    LV_LOG_TRACE("begin");

    lv_display_send_event(disp, LV_EVENT_FLUSH_WAIT_START, NULL);

    if(disp->flush_wait_cb) {
        if(disp->flushing) {
            disp->flush_wait_cb(disp);
            disp->flushing = 0;
        }
    }
    else {
        while(disp->flushing);
    }
    disp->flushing_last = 0;

    lv_display_send_event(disp, LV_EVENT_FLUSH_WAIT_FINISH, NULL);

    LV_LOG_TRACE("end");
    LV_PROFILER_REFR_END;
}
