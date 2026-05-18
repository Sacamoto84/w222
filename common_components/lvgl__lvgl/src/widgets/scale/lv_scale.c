/**
 * @file lv_scale.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_scale_private.h"
#include "../../core/lv_obj_private.h"
#include "../../core/lv_obj_class_private.h"
#if LV_USE_SCALE != 0

#include "../../core/lv_group.h"
#include "../../misc/lv_assert.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_text_private.h"
#include "../../core/lv_observer_private.h"
#include "../../draw/lv_draw_arc.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_scale_class)

#define LV_SCALE_LABEL_TXT_LEN          (20U)
#define LV_SCALE_DEFAULT_ANGLE_RANGE    ((uint32_t) 270U)
#define LV_SCALE_DEFAULT_ROTATION       ((int32_t) 135U)
#define LV_SCALE_TICK_IDX_DEFAULT_ID    ((uint32_t) 255U)
#define LV_SCALE_DEFAULT_LABEL_GAP      ((uint32_t) 15U)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_scale_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_scale_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_scale_event(const lv_obj_class_t * class_p, lv_event_t * event);

static void scale_draw_main(lv_obj_t * obj, lv_event_t * event);
static void scale_draw_indicator(lv_obj_t * obj, lv_event_t * event);
static void scale_draw_label(lv_obj_t * obj, lv_event_t * event, lv_draw_label_dsc_t * label_dsc,
                             const uint32_t major_tick_idx, const int32_t tick_value, lv_point_t * tick_point_b, const uint32_t tick_idx);
static void scale_calculate_main_compensation(lv_obj_t * obj);

static void scale_get_center(const lv_obj_t * obj, lv_point_t * center, int32_t * arc_r);
static void scale_get_tick_points(lv_obj_t * obj, const uint32_t tick_idx, bool is_major_tick,
                                  lv_point_t * tick_point_a, lv_point_t * tick_point_b);
static void scale_get_label_coords(lv_obj_t * obj, lv_draw_label_dsc_t * label_dsc, lv_point_t * tick_point,
                                   lv_area_t * label_coords);
static void scale_set_indicator_label_properties(lv_obj_t * obj, lv_draw_label_dsc_t * label_dsc,
                                                 const lv_style_t * indicator_section_style);
static void scale_set_line_properties(lv_obj_t * obj, lv_draw_line_dsc_t * line_dsc, const lv_style_t * section_style,
                                      lv_part_t part);
static void scale_set_arc_properties(lv_obj_t * obj, lv_draw_arc_dsc_t * arc_dsc, const lv_style_t * section_style);
/* Помощники */
static void scale_find_section_tick_idx(lv_obj_t * obj);
static void scale_store_main_line_tick_width_compensation(lv_obj_t * obj, const uint32_t tick_idx,
                                                          const bool is_major_tick, const int32_t major_tick_width, const int32_t minor_tick_width);
static void scale_store_section_line_tick_width_compensation(lv_obj_t * obj, const bool is_major_tick,
                                                             lv_draw_line_dsc_t * major_tick_dsc, lv_draw_line_dsc_t * minor_tick_dsc,
                                                             const int32_t tick_value, const uint8_t tick_idx, lv_point_t * tick_point_a);
static void scale_build_custom_label_text(lv_obj_t * obj, lv_draw_label_dsc_t * label_dsc,
                                          const uint16_t major_tick_idx);

static void scale_free_line_needle_points_cb(lv_event_t * e);

static bool scale_is_major_tick(lv_scale_t * scale, uint32_t tick_idx);

static lv_result_t update_needle(lv_scale_t * scale, lv_obj_t * needle, int32_t length, int32_t value);
static void needle_deleted_cb(lv_event_t * e);

#if LV_USE_OBSERVER
    static void scale_section_min_value_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
    static void scale_section_max_value_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
#endif /*LV_USE_OBSERVER*/

/**********************
 *  STATIC VARIABLES
 **********************/

#if LV_USE_OBJ_PROPERTY
static const lv_property_ops_t lv_scale_properties[] = {
    {
        .id = LV_PROPERTY_SCALE_MODE,
        .setter = lv_scale_set_mode,
        .getter = lv_scale_get_mode,
    },
    {
        .id = LV_PROPERTY_SCALE_TOTAL_TICK_COUNT,
        .setter = lv_scale_set_total_tick_count,
        .getter = lv_scale_get_total_tick_count,
    },
    {
        .id = LV_PROPERTY_SCALE_MAJOR_TICK_EVERY,
        .setter = lv_scale_set_major_tick_every,
        .getter = lv_scale_get_major_tick_every,
    },
    {
        .id = LV_PROPERTY_SCALE_LABEL_SHOW,
        .setter = lv_scale_set_label_show,
        .getter = lv_scale_get_label_show,
    },
    {
        .id = LV_PROPERTY_SCALE_ANGLE_RANGE,
        .setter = lv_scale_set_angle_range,
        .getter = lv_scale_get_angle_range,
    },
    {
        .id = LV_PROPERTY_SCALE_ROTATION,
        .setter = lv_scale_set_rotation,
        .getter = lv_scale_get_rotation,
    },
    {
        .id = LV_PROPERTY_SCALE_RANGE_MIN_VALUE,
        .setter = lv_scale_set_min_value,
        .getter = lv_scale_get_range_min_value,
    },
    {
        .id = LV_PROPERTY_SCALE_RANGE_MAX_VALUE,
        .setter = lv_scale_set_max_value,
        .getter = lv_scale_get_range_max_value,
    },
};
#endif

const lv_obj_class_t lv_scale_class  = {
    .constructor_cb = lv_scale_constructor,
    .destructor_cb = lv_scale_destructor,
    .event_cb = lv_scale_event,
    .instance_size = sizeof(lv_scale_t),
    .editable = LV_OBJ_CLASS_EDITABLE_TRUE,
    .base_class = &lv_obj_class,
    .name = "lv_scale",
    LV_PROPERTY_CLASS_FIELDS(scale, SCALE)
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_scale_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*======================
 * Добавить/удалить функции
 *=====================*/

/*
 * Сюда входят новые функции «добавить» или «удалить», специфичные для объекта.
 */

/*=====================
 * Функции установки
 *====================*/

void lv_scale_set_mode(lv_obj_t * obj, lv_scale_mode_t mode)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->mode = mode;

    lv_obj_invalidate(obj);
}

void lv_scale_set_total_tick_count(lv_obj_t * obj, uint32_t total_tick_count)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->total_tick_count = total_tick_count;

    lv_obj_invalidate(obj);
}

void lv_scale_set_major_tick_every(lv_obj_t * obj, uint32_t major_tick_every)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->major_tick_every = major_tick_every;

    lv_obj_invalidate(obj);
}

void lv_scale_set_label_show(lv_obj_t * obj, bool show_label)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->label_enabled = show_label;

    lv_obj_invalidate(obj);
}

void lv_scale_set_range(lv_obj_t * obj, int32_t min, int32_t max)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->range_min = min;
    scale->range_max = max;

    lv_obj_invalidate(obj);
}

void lv_scale_set_min_value(lv_obj_t * obj, int32_t min)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;
    if(scale->range_min == min) return;
    scale->range_min = min;

    lv_obj_invalidate(obj);
}

void lv_scale_set_max_value(lv_obj_t * obj, int32_t max)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;
    if(scale->range_max == max) return;
    scale->range_max = max;

    lv_obj_invalidate(obj);
}

void lv_scale_set_angle_range(lv_obj_t * obj, uint32_t angle_range)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->angle_range = angle_range;

    lv_obj_invalidate(obj);
}

void lv_scale_set_rotation(lv_obj_t * obj, int32_t rotation)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;
    int32_t normalized_angle = rotation;

    if(normalized_angle < 0 || normalized_angle > 360) {
        normalized_angle = rotation % 360;

        if(normalized_angle < 0) {
            normalized_angle += 360;
        }
    }

    scale->rotation = normalized_angle;
    lv_obj_invalidate(obj);
}

void lv_scale_set_line_needle_value(lv_obj_t * obj, lv_obj_t * needle_line, int32_t needle_length,
                                    int32_t value)
{
    int32_t angle;
    int32_t scale_width, scale_height;
    int32_t actual_needle_length;
    int32_t needle_length_x, needle_length_y;
    lv_point_precise_t * needle_line_points = NULL;

    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;
    if((scale->mode != LV_SCALE_MODE_ROUND_INNER) &&
       (scale->mode != LV_SCALE_MODE_ROUND_OUTER)) {
        return;
    }

    lv_obj_align(needle_line, LV_ALIGN_TOP_LEFT, 0, 0);

    scale_width = lv_obj_get_style_width(obj, LV_PART_MAIN);
    scale_height = lv_obj_get_style_height(obj, LV_PART_MAIN);

    if(scale_width != scale_height) {
        return;
    }

    if(needle_length >= scale_width / 2) {
        actual_needle_length = scale_width / 2;
    }
    else if(needle_length >= 0) {
        actual_needle_length = needle_length;
    }
    else if(needle_length + scale_width / 2 < 0) {
        actual_needle_length = 0;
    }
    else {
        actual_needle_length = scale_width / 2 + needle_length;
    }

    if(value < scale->range_min) {
        angle = 0;
    }
    else if(value > scale->range_max) {
        angle = scale->angle_range;
    }
    else {
        angle = scale->angle_range * (value - scale->range_min) / (scale->range_max - scale->range_min);
    }

    needle_length_x = (actual_needle_length * lv_trigo_cos(scale->rotation + angle)) >> LV_TRIGO_SHIFT;
    needle_length_y = (actual_needle_length * lv_trigo_sin(scale->rotation + angle)) >> LV_TRIGO_SHIFT;

    if(lv_line_is_point_array_mutable(needle_line) && lv_line_get_point_count(needle_line) >= 2) {
        needle_line_points = lv_line_get_points_mutable(needle_line);
    }

    if(needle_line_points == NULL) {
        uint32_t i;
        uint32_t line_event_cnt = lv_obj_get_event_count(needle_line);
        for(i = 0; i < line_event_cnt; i++) {
            lv_event_dsc_t * dsc = lv_obj_get_event_dsc(needle_line, i);
            if(lv_event_dsc_get_cb(dsc) == scale_free_line_needle_points_cb) {
                needle_line_points = lv_event_dsc_get_user_data(dsc);
                break;
            }
        }
    }

    if(needle_line_points == NULL) {
        needle_line_points = lv_malloc(sizeof(lv_point_precise_t) * 2);
        LV_ASSERT_MALLOC(needle_line_points);
        if(needle_line_points == NULL) return;
        lv_obj_add_event_cb(needle_line, scale_free_line_needle_points_cb, LV_EVENT_DELETE, needle_line_points);
    }

    needle_line_points[0].x = scale_width / 2;
    needle_line_points[0].y = scale_height / 2;
    needle_line_points[1].x = scale_width / 2 + needle_length_x;
    needle_line_points[1].y = scale_height / 2 + needle_length_y;

    lv_line_set_points_mutable(needle_line, needle_line_points, 2);

    update_needle(scale, needle_line, needle_length, value);
}

void lv_scale_set_image_needle_value(lv_obj_t * obj, lv_obj_t * needle_img, int32_t value)
{
    int32_t angle;
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;
    if((scale->mode != LV_SCALE_MODE_ROUND_INNER) &&
       (scale->mode != LV_SCALE_MODE_ROUND_OUTER)) {
        return;
    }

    if(value < scale->range_min) {
        angle = 0;
    }
    else if(value > scale->range_max) {
        angle = scale->angle_range;
    }
    else {
        angle = scale->angle_range * (value - scale->range_min) / (scale->range_max - scale->range_min);
    }

    lv_image_set_rotation(needle_img, (scale->rotation + angle) * 10);
    update_needle(scale, needle_img, 0, value);
}

void lv_scale_set_text_src(lv_obj_t * obj, const char * txt_src[])
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->txt_src = txt_src;
    scale->custom_label_cnt = 0;
    if(scale->txt_src) {
        int32_t idx;
        for(idx = 0; txt_src[idx]; ++idx) {
            scale->custom_label_cnt++;
        }
    }

    lv_obj_invalidate(obj);
}

void lv_scale_set_post_draw(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->post_draw = en;

    lv_obj_invalidate(obj);
}

void lv_scale_set_draw_ticks_on_top(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_scale_t * scale = (lv_scale_t *)obj;

    scale->draw_ticks_on_top = en;

    lv_obj_invalidate(obj);
}

lv_scale_section_t * lv_scale_add_section(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_scale_section_t * section = lv_ll_ins_head(&scale->section_ll);
    LV_ASSERT_MALLOC(section);
    if(section == NULL) return NULL;

    /* Значения раздела по умолчанию */
    lv_memzero(section, sizeof(lv_scale_section_t));
    section->first_tick_idx_in_section = LV_SCALE_TICK_IDX_DEFAULT_ID;
    section->last_tick_idx_in_section = LV_SCALE_TICK_IDX_DEFAULT_ID;
    /* Начальный диапазон — [0..-1], что делает его «нейтральным» (т. е. не будет отображаться до тех пор, пока пользователь не
     * установить другой диапазон).   `range_min`уже равен 0, начиная с`lv_memzero()`выше. */
    section->range_max = -1;

    return section;
}


void lv_scale_set_section_range(lv_obj_t * scale, lv_scale_section_t * section, int32_t min, int32_t max)
{
    LV_ASSERT_OBJ(scale, MY_CLASS);
    LV_ASSERT_NULL(section);

    lv_scale_set_section_min_value(scale, section, min);
    lv_scale_set_section_max_value(scale, section, max);
}

void lv_scale_set_section_min_value(lv_obj_t * scale, lv_scale_section_t * section, int32_t min)
{
    LV_ASSERT_OBJ(scale, MY_CLASS);
    LV_ASSERT_NULL(section);

    if(section->range_min == min) return;
    section->range_min = min;
    lv_obj_invalidate(scale);
}

void lv_scale_set_section_max_value(lv_obj_t * scale, lv_scale_section_t * section, int32_t max)
{
    LV_ASSERT_OBJ(scale, MY_CLASS);
    LV_ASSERT_NULL(section);

    if(section->range_max == max) return;
    section->range_max = max;
    lv_obj_invalidate(scale);
}

void lv_scale_section_set_range(lv_scale_section_t * section, int32_t min, int32_t max)
{
    if(NULL == section) return;

    section->range_min = min;
    section->range_max = max;
}


void lv_scale_set_section_style_main(lv_obj_t * scale, lv_scale_section_t * section, const lv_style_t * style)
{
    LV_ASSERT_OBJ(scale, MY_CLASS);
    LV_ASSERT_NULL(section);

    section->main_style = style;
    lv_obj_invalidate(scale);
}

void lv_scale_set_section_style_indicator(lv_obj_t * scale, lv_scale_section_t * section, const lv_style_t * style)
{
    LV_ASSERT_OBJ(scale, MY_CLASS);
    LV_ASSERT_NULL(section);

    section->indicator_style = style;
    lv_obj_invalidate(scale);
}

void lv_scale_set_section_style_items(lv_obj_t * scale, lv_scale_section_t * section, const lv_style_t * style)
{
    LV_ASSERT_OBJ(scale, MY_CLASS);
    LV_ASSERT_NULL(section);

    section->items_style = style;
    lv_obj_invalidate(scale);
}

void lv_scale_section_set_style(lv_scale_section_t * section, lv_part_t part, lv_style_t * section_part_style)
{
    LV_LOG_WARN("Deprecated, use lv_scale_set_section_style_main/indicator/items instead");

    if(NULL == section) return;

    switch(part) {
        case LV_PART_MAIN:
            section->main_style = section_part_style;
            break;
        case LV_PART_INDICATOR:
            section->indicator_style = section_part_style;
            break;
        case LV_PART_ITEMS:
            section->items_style = section_part_style;
            break;
        default:
            /* Неверная часть */
            break;
    }
}

/*=====================
 * Геттерные функции
 *====================*/

lv_scale_mode_t lv_scale_get_mode(lv_obj_t * obj)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    return scale->mode;
}

int32_t lv_scale_get_total_tick_count(lv_obj_t * obj)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    return scale->total_tick_count;
}

int32_t lv_scale_get_major_tick_every(lv_obj_t * obj)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    return scale->major_tick_every;
}

int32_t lv_scale_get_rotation(lv_obj_t * obj)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    return scale->rotation;
}

bool lv_scale_get_label_show(lv_obj_t * obj)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    return scale->label_enabled;
}

uint32_t lv_scale_get_angle_range(lv_obj_t * obj)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    return scale->angle_range;
}

int32_t lv_scale_get_range_min_value(lv_obj_t * obj)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    return scale->range_min;
}

int32_t lv_scale_get_range_max_value(lv_obj_t * obj)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    return scale->range_max;
}

/*=====================
 * Другие функции
 *====================*/

#if LV_USE_OBSERVER

lv_observer_t * lv_scale_bind_section_min_value(lv_obj_t * obj, lv_scale_section_t * section, lv_subject_t * subject)
{
    LV_ASSERT_NULL(subject);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(section);

    if(subject->type != LV_SUBJECT_TYPE_INT) {
        LV_LOG_WARN("Incompatible subject type: %d", subject->type);
        return NULL;
    }

    lv_observer_t * observer = lv_subject_add_observer_obj(subject, scale_section_min_value_observer_cb, obj, section);

    return observer;
}

lv_observer_t * lv_scale_bind_section_max_value(lv_obj_t * obj, lv_scale_section_t * section, lv_subject_t * subject)
{
    LV_ASSERT_NULL(subject);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(section);

    if(subject->type != LV_SUBJECT_TYPE_INT) {
        LV_LOG_WARN("Incompatible subject type: %d", subject->type);
        return NULL;
    }

    lv_observer_t * observer = lv_subject_add_observer_obj(subject, scale_section_max_value_observer_cb, obj, section);

    return observer;
}

#endif /*LV_USE_OBSERVER*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_scale_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_scale_t * scale = (lv_scale_t *)obj;

    lv_ll_init(&scale->section_ll, sizeof(lv_scale_section_t));

    scale->total_tick_count = LV_SCALE_TOTAL_TICK_COUNT_DEFAULT;
    scale->major_tick_every = LV_SCALE_MAJOR_TICK_EVERY_DEFAULT;
    scale->mode = LV_SCALE_MODE_HORIZONTAL_BOTTOM;
    scale->label_enabled = LV_SCALE_LABEL_ENABLED_DEFAULT;
    scale->angle_range = LV_SCALE_DEFAULT_ANGLE_RANGE;
    scale->rotation = LV_SCALE_DEFAULT_ROTATION;
    scale->range_min = 0;
    scale->range_max = 100;
    scale->last_tick_width = 0;
    scale->first_tick_width = 0;
    scale->post_draw = false;
    scale->draw_ticks_on_top = false;
    scale->custom_label_cnt = 0;
    scale->txt_src = NULL;
    lv_array_init(&scale->needles, 0, sizeof(lv_scale_needle_t));

    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_scale_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_scale_section_t * section;
    while(scale->section_ll.head) {
        section = lv_ll_get_head(&scale->section_ll);
        lv_ll_remove(&scale->section_ll, section);
        lv_free(section);
    }
    lv_ll_clear(&scale->section_ll);

    size_t needle_count = lv_array_size(&scale->needles);
    for(size_t i = 0; i < needle_count; ++i) {
        lv_scale_needle_t * scale_needle = lv_array_at(&scale->needles, i);
        lv_obj_remove_event_cb(scale_needle->obj, needle_deleted_cb);
    }
    lv_array_deinit(&scale->needles);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_scale_event(const lv_obj_class_t * class_p, lv_event_t * event)
{
    LV_UNUSED(class_p);

    /*Вызов обработчика событий предка*/
    lv_result_t res = lv_obj_event_base(MY_CLASS, event);
    if(res != LV_RESULT_OK) return;

    lv_event_code_t event_code = lv_event_get_code(event);
    lv_obj_t * obj = lv_event_get_current_target(event);
    lv_scale_t * scale = (lv_scale_t *) obj;
    LV_UNUSED(scale);

    if(event_code == LV_EVENT_DRAW_MAIN) {
        if(scale->post_draw == false) {
            scale_find_section_tick_idx(obj);
            scale_calculate_main_compensation(obj);

            if(scale->draw_ticks_on_top) {
                scale_draw_main(obj, event);
                scale_draw_indicator(obj, event);
            }
            else {
                scale_draw_indicator(obj, event);
                scale_draw_main(obj, event);
            }
        }
    }
    if(event_code == LV_EVENT_DRAW_POST) {
        if(scale->post_draw == true) {
            scale_find_section_tick_idx(obj);
            scale_calculate_main_compensation(obj);

            if(scale->draw_ticks_on_top) {
                scale_draw_main(obj, event);
                scale_draw_indicator(obj, event);
            }
            else {
                scale_draw_indicator(obj, event);
                scale_draw_main(obj, event);
            }
        }
    }
    else if(event_code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        /* NOTE: Увеличьте размер изображения шкалы, чтобы можно было отобразить первую метку деления. */
        lv_event_set_ext_draw_size(event, 100);
    }
    else if(event_code == LV_EVENT_STYLE_CHANGED) {
        size_t needle_count = lv_array_size(&scale->needles);
        for(size_t i = 0; i < needle_count; ++i) {
            lv_scale_needle_t * needle = lv_array_at(&scale->needles, i);
            if(lv_obj_has_class(needle->obj, &lv_line_class)) {
                lv_scale_set_line_needle_value(obj, needle->obj, needle->length, needle->value);
            }
            else {
                lv_scale_set_image_needle_value(obj, needle->obj, needle->value);
            }
        }
    }
    else {
        /* Нечего делать. Недопустимое событие */
    }
}

static void scale_draw_indicator(lv_obj_t * obj, lv_event_t * event)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_layer_t * layer = lv_event_get_layer(event);

    if(scale->total_tick_count <= 1) return;

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.base.layer = layer;
    /* Форматирование меток с настроенным стилем для LV_PART_INDICATOR */
    lv_obj_init_draw_label_dsc(obj, LV_PART_INDICATOR, &label_dsc);


    /* Основной стиль галочки */
    lv_draw_line_dsc_t major_tick_dsc;
    lv_draw_line_dsc_init(&major_tick_dsc);
    major_tick_dsc.base.layer = layer;
    lv_obj_init_draw_line_dsc(obj, LV_PART_INDICATOR, &major_tick_dsc);
    if(LV_SCALE_MODE_ROUND_OUTER == scale->mode || LV_SCALE_MODE_ROUND_INNER == scale->mode) {
        major_tick_dsc.raw_end = 0;
    }

    /* Настройте дескриптор рисования линий для рисования второстепенных делений. */
    lv_draw_line_dsc_t minor_tick_dsc;
    lv_draw_line_dsc_init(&minor_tick_dsc);
    minor_tick_dsc.base.layer = layer;
    lv_obj_init_draw_line_dsc(obj, LV_PART_ITEMS, &minor_tick_dsc);

    /* Основной стиль линии */
    lv_draw_line_dsc_t main_line_dsc;
    lv_draw_line_dsc_init(&main_line_dsc);
    main_line_dsc.base.layer = layer;
    lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &main_line_dsc);

    /* Эти два значения должны быть подписаны, поскольку они передаются.
     * до `lv_map()`, который ожидает целые числа со знакомым. */
    const int32_t total_tick_count = scale->total_tick_count;
    int32_t tick_idx = 0;
    uint32_t major_tick_idx = 0U;
    for(tick_idx = 0; tick_idx < total_tick_count; tick_idx++) {
        /* Главный тик — это тот, в котором есть метка. */
        bool is_major_tick = scale_is_major_tick(scale, tick_idx);
        if(is_major_tick) major_tick_idx++;

        const int32_t tick_value = lv_map(tick_idx, 0, total_tick_count - 1, scale->range_min, scale->range_max);

        label_dsc.base.id1 = tick_idx;
        label_dsc.base.id2 = tick_value;
        label_dsc.base.layer = layer;

        /* Перезаписать свойства метки и галочки, если значение галочки находится в пределах диапазона раздела. */
        lv_scale_section_t * section;
        LV_LL_READ_BACK(&scale->section_ll, section) {
            if(section->range_min <= tick_value && section->range_max >= tick_value) {
                if(is_major_tick) {
                    scale_set_indicator_label_properties(obj, &label_dsc, section->indicator_style);
                    scale_set_line_properties(obj, &major_tick_dsc, section->indicator_style, LV_PART_INDICATOR);
                }
                else {
                    scale_set_line_properties(obj, &minor_tick_dsc, section->items_style, LV_PART_ITEMS);
                }
                break;
            }
            else {
                /* Галочка не в разделе, установите нужные стили */
                lv_obj_init_draw_label_dsc(obj, LV_PART_INDICATOR, &label_dsc);
                lv_obj_init_draw_line_dsc(obj, LV_PART_INDICATOR, &major_tick_dsc);
                lv_obj_init_draw_line_dsc(obj, LV_PART_ITEMS, &minor_tick_dsc);
            }
        }

        /* Галочка представлена линией. Нам нужны две точки, чтобы нарисовать его. */
        lv_point_t tick_point_a;
        lv_point_t tick_point_b;
        scale_get_tick_points(obj, tick_idx, is_major_tick, &tick_point_a, &tick_point_b);

        /* Настройте метки, если они включены и мы рисуем большую галочку. */
        if(scale->label_enabled && is_major_tick) {
            scale_draw_label(obj, event, &label_dsc, major_tick_idx, tick_value, &tick_point_b, tick_idx);
        }

        if(is_major_tick) {
            major_tick_dsc.p1 = lv_point_to_precise(&tick_point_a);
            major_tick_dsc.p2 = lv_point_to_precise(&tick_point_b);
            major_tick_dsc.base.id1 = tick_idx;
            major_tick_dsc.base.id2 = tick_value;
            lv_draw_line(layer, &major_tick_dsc);
        }
        else {
            minor_tick_dsc.p1 = lv_point_to_precise(&tick_point_a);
            minor_tick_dsc.p2 = lv_point_to_precise(&tick_point_b);
            minor_tick_dsc.base.id1 = tick_idx;
            minor_tick_dsc.base.id2 = tick_value;
            lv_draw_line(layer, &minor_tick_dsc);
        }
    }
}

static void scale_draw_label(lv_obj_t * obj, lv_event_t * event, lv_draw_label_dsc_t * label_dsc,
                             const uint32_t major_tick_idx, const int32_t tick_value, lv_point_t * tick_point_b,
                             const uint32_t tick_idx)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_layer_t * layer = lv_event_get_layer(event);

    /* Настройка текста этикетки */
    char text_buffer[LV_SCALE_LABEL_TXT_LEN] = {0};
    lv_area_t label_coords;

    /* Проверьте, есть ли в пользовательском текстовом массиве элемент для этого основного тикающего индекса. */
    if(scale->txt_src) {
        scale_build_custom_label_text(obj, label_dsc, major_tick_idx);
    }
    else { /* Добавить метку с сопоставленными значениями */
        lv_snprintf(text_buffer, sizeof(text_buffer), "%" LV_PRId32, tick_value);
        label_dsc->text = text_buffer;
        label_dsc->text_local = 1;
    }

    int32_t translate_x = lv_obj_get_style_translate_x(obj, LV_PART_INDICATOR);
    int32_t translate_y = lv_obj_get_style_translate_y(obj, LV_PART_INDICATOR);
    int32_t label_rotation = lv_obj_get_style_transform_rotation(obj, LV_PART_INDICATOR);
    int32_t translate_rotation = 0;

    if((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)
       || (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode || LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode)) {
        lv_point_t label_origin;
        label_origin.x = tick_point_b->x + translate_x;
        label_origin.y = tick_point_b->y + translate_y;
        scale_get_label_coords(obj, label_dsc, &label_origin, &label_coords);
        label_rotation = (label_rotation & LV_SCALE_ROTATION_ANGLE_MASK);
    }
    else if(LV_SCALE_MODE_ROUND_OUTER == scale->mode || LV_SCALE_MODE_ROUND_INNER == scale->mode) {
        translate_rotation = lv_obj_get_style_translate_radial(obj, LV_PART_INDICATOR);
        uint32_t label_gap = lv_obj_get_style_pad_radial(obj, LV_PART_INDICATOR) + LV_SCALE_DEFAULT_LABEL_GAP;

        lv_area_t scale_area;
        lv_obj_get_content_coords(obj, &scale_area);

        /* Найдите центр шкалы */
        lv_point_t center_point;
        int32_t radius_edge = LV_MIN(lv_area_get_width(&scale_area) / 2, lv_area_get_height(&scale_area) / 2);
        center_point.x = scale_area.x1 + radius_edge;
        center_point.y = scale_area.y1 + radius_edge;

        const int32_t major_len = lv_obj_get_style_length(obj, LV_PART_INDICATOR);

        /* Также обратите внимание на пространство между буквами стиля. */
        int32_t angle_upscale = ((tick_idx * scale->angle_range) * 10U) / (scale->total_tick_count - 1U) +
                                (translate_rotation * 10);
        angle_upscale += scale->rotation * 10;

        uint32_t radius_text = 0;
        if(LV_SCALE_MODE_ROUND_INNER == scale->mode) {
            radius_text = (radius_edge - major_len) - (label_gap + label_dsc->letter_space);
        }
        else if(LV_SCALE_MODE_ROUND_OUTER == scale->mode) {
            radius_text = (radius_edge + major_len) + (label_gap + label_dsc->letter_space);
        }
        else { /* Нечего делать */ }

        lv_point_t point;
        point.x = center_point.x + radius_text + translate_x;
        point.y = center_point.y + translate_y;
        int32_t label_rotation_temp = 0;

        if(label_rotation & LV_SCALE_LABEL_ROTATE_MATCH_TICKS) {
            label_rotation_temp = (label_rotation & LV_SCALE_ROTATION_ANGLE_MASK) + angle_upscale;

            /* держите текст вертикально, если пользователь попросил об этом, иначе на половине циферблата он будет перевернут */
            if(label_rotation & LV_SCALE_LABEL_ROTATE_KEEP_UPRIGHT) {
                while(label_rotation_temp > 3600) {
                    label_rotation_temp -= 3600;
                }
                if(label_rotation_temp > 900 && label_rotation_temp < 2400) {
                    label_rotation_temp += 1800;
                }
            }
            label_rotation = label_rotation_temp;
        }
        else {
            label_rotation = label_rotation & LV_SCALE_ROTATION_ANGLE_MASK;
        }

        lv_point_transform(&point, angle_upscale, LV_SCALE_NONE, LV_SCALE_NONE, &center_point, false);
        scale_get_label_coords(obj, label_dsc, &point, &label_coords);
    }
    /* Неверный режим */
    else {
        return;
    }

    if(label_rotation > 0) {
        /*Нарисуйте метку на новом слое и нарисуйте повернутый слой.*/
        lv_layer_t * layer_label = lv_draw_layer_create(layer, LV_COLOR_FORMAT_ARGB8888, &label_coords);
        lv_draw_label(layer_label, label_dsc, &label_coords);

        lv_point_t pivot_point;
        /* Установите точку поворота в центре метки, чтобы она соответствовала кривой масштаба. */
        pivot_point.x = lv_area_get_width(&label_coords) / 2;
        pivot_point.y = lv_area_get_height(&label_coords) / 2;

        lv_draw_image_dsc_t layer_draw_dsc;
        lv_draw_image_dsc_init(&layer_draw_dsc);
        layer_draw_dsc.src = layer_label;
        layer_draw_dsc.rotation = label_rotation;
        layer_draw_dsc.pivot = pivot_point;
        lv_draw_layer(layer, &layer_draw_dsc, &label_coords);
    }
    else {
        lv_draw_label(layer, label_dsc, &label_coords);
    }

    if(label_dsc->text_local) {
        /* очистить ссылку на текстовый буфер в стеке */
        label_dsc->text = NULL;
        label_dsc->text_local = false;
    }
}

static void scale_calculate_main_compensation(lv_obj_t * obj)
{
    lv_scale_t * scale = (lv_scale_t *)obj;

    const uint32_t total_tick_count = scale->total_tick_count;

    if(total_tick_count <= 1) return;
    /* Не поддерживается в раундовых режимах */
    if(LV_SCALE_MODE_ROUND_OUTER == scale->mode || LV_SCALE_MODE_ROUND_INNER == scale->mode) return;

    /* Основной стиль галочки */
    lv_draw_line_dsc_t major_tick_dsc;
    lv_draw_line_dsc_init(&major_tick_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_INDICATOR, &major_tick_dsc);

    /* Настройте дескриптор рисования линий для рисования второстепенных делений. */
    lv_draw_line_dsc_t minor_tick_dsc;
    lv_draw_line_dsc_init(&minor_tick_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_ITEMS, &minor_tick_dsc);

    uint32_t tick_idx = 0;
    for(tick_idx = 0; tick_idx < total_tick_count; tick_idx++) {

        const bool is_major_tick = scale_is_major_tick(scale, tick_idx);

        const int32_t tick_value = lv_map(tick_idx, 0, total_tick_count - 1, scale->range_min, scale->range_max);

        /* Перезаписать свойства метки и галочки, если значение галочки находится в пределах диапазона раздела. */
        lv_scale_section_t * section;
        LV_LL_READ_BACK(&scale->section_ll, section) {
            if(section->range_min <= tick_value && section->range_max >= tick_value) {
                if(is_major_tick) {
                    scale_set_line_properties(obj, &major_tick_dsc, section->indicator_style, LV_PART_INDICATOR);
                }
                else {
                    scale_set_line_properties(obj, &minor_tick_dsc, section->items_style, LV_PART_ITEMS);
                }
                break;
            }
            else {
                /* Галочка не в разделе, установите нужные стили */
                lv_obj_init_draw_line_dsc(obj, LV_PART_INDICATOR, &major_tick_dsc);
                lv_obj_init_draw_line_dsc(obj, LV_PART_ITEMS, &minor_tick_dsc);
            }
        }

        /* Галочка представлена линией. Нам нужны две точки, чтобы нарисовать его. */
        lv_point_t tick_point_a;
        lv_point_t tick_point_b;
        scale_get_tick_points(obj, tick_idx, is_major_tick, &tick_point_a, &tick_point_b);

        /* Сохраните начальную и последнюю ширину деления, которая будет использоваться при рисовании основной линии. */
        scale_store_main_line_tick_width_compensation(obj, tick_idx, is_major_tick, major_tick_dsc.width, minor_tick_dsc.width);
        /* Сохраните первый и последний разделы, отметив вертикальное/горизонтальное положение. */
        scale_store_section_line_tick_width_compensation(obj, is_major_tick, &major_tick_dsc, &minor_tick_dsc,
                                                         tick_value, tick_idx, &tick_point_a);
    }
}

static void scale_draw_main(lv_obj_t * obj, lv_event_t * event)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    lv_layer_t * layer = lv_event_get_layer(event);

    if(scale->total_tick_count <= 1) return;

    if((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)
       || (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode || LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode)) {

        /* Настройте дескрипторы рисования линий и меток для рисунков галочек и меток. */
        lv_draw_line_dsc_t line_dsc;
        lv_draw_line_dsc_init(&line_dsc);
        line_dsc.base.layer = layer;
        lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &line_dsc);

        /* Получите свойства стиля, чтобы их можно было использовать в рисовании основной линии. */
        const int32_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
        const int32_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + border_width;
        const int32_t pad_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN) + border_width;
        const int32_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + border_width;
        const int32_t pad_right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN) + border_width;

        int32_t x_ofs = 0;
        int32_t y_ofs = 0;

        if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) {
            x_ofs = obj->coords.x2 + (line_dsc.width / 2) - pad_right;
            y_ofs = obj->coords.y1 + pad_top;
        }
        else if(LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
            x_ofs = obj->coords.x1 + (line_dsc.width / 2) + pad_left;
            y_ofs = obj->coords.y1 + pad_top;
        }
        if(LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) {
            x_ofs = obj->coords.x1 + pad_right;
            y_ofs = obj->coords.y1 + (line_dsc.width / 2) + pad_top;
        }
        else if(LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) {
            x_ofs = obj->coords.x1 + pad_left;
            y_ofs = obj->coords.y2 + (line_dsc.width / 2) - pad_bottom;
        }
        else { /* Нечего делать */ }

        lv_point_t main_line_point_a;
        lv_point_t main_line_point_b;

        /* Настройте точки отсчета */
        if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
            main_line_point_a.x = x_ofs - 1;
            main_line_point_a.y = y_ofs;
            main_line_point_b.x = x_ofs - 1;
            main_line_point_b.y = obj->coords.y2 - pad_bottom;

            /* Отрегулируйте основную линию с шириной начального и последнего тика. */
            main_line_point_a.y -= scale->last_tick_width / 2;
            main_line_point_b.y += scale->first_tick_width / 2;
        }
        else {
            main_line_point_a.x = x_ofs;
            main_line_point_a.y = y_ofs;
            /* X второй точки начинается на краю объекта минус левая площадка. */
            main_line_point_b.x = obj->coords.x2 - (pad_left);
            main_line_point_b.y = y_ofs;

            /* Отрегулируйте основную линию с шириной начального и последнего тика. */
            main_line_point_a.x -= scale->last_tick_width / 2;
            main_line_point_b.x += scale->first_tick_width / 2;
        }

        line_dsc.p1 = lv_point_to_precise(&main_line_point_a);
        line_dsc.p2 = lv_point_to_precise(&main_line_point_b);
        lv_draw_line(layer, &line_dsc);

        lv_scale_section_t * section;
        LV_LL_READ_BACK(&scale->section_ll, section) {
            lv_draw_line_dsc_t section_line_dsc;
            lv_draw_line_dsc_init(&section_line_dsc);
            section_line_dsc.base.layer = layer;
            lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &section_line_dsc);

            /* Вычислить точки линии сечения */
            lv_point_t section_point_a;
            lv_point_t section_point_b;

            const int32_t first_tick_width_halved = (int32_t)(section->first_tick_in_section_width / 2);
            const int32_t last_tick_width_halved = (int32_t)(section->last_tick_in_section_width / 2);

            /* Рассчитать положение раздела на основе тиков (первого и последнего) индекса. */
            if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
                /* Вычислить положение первого тика в разделе */
                section_point_a.x = main_line_point_a.x;
                section_point_a.y = section->first_tick_in_section.y + first_tick_width_halved;

                /* Вычислить позицию последнего тика в разделе */
                section_point_b.x = main_line_point_a.x;
                section_point_b.y = section->last_tick_in_section.y - last_tick_width_halved;
            }
            else {
                /* Вычислить положение первого тика в разделе */
                section_point_a.x = section->first_tick_in_section.x - first_tick_width_halved;
                section_point_a.y = main_line_point_a.y;

                /* Вычислить позицию последнего тика в разделе */
                section_point_b.x = section->last_tick_in_section.x + last_tick_width_halved;
                section_point_b.y = main_line_point_a.y;
            }

            scale_set_line_properties(obj, &section_line_dsc, section->main_style, LV_PART_MAIN);

            section_line_dsc.p1.x = section_point_a.x;
            section_line_dsc.p1.y = section_point_a.y;
            section_line_dsc.p2.x = section_point_b.x;
            section_line_dsc.p2.y = section_point_b.y;
            lv_draw_line(layer, &section_line_dsc);
        }
    }
    else if(LV_SCALE_MODE_ROUND_OUTER == scale->mode || LV_SCALE_MODE_ROUND_INNER == scale->mode) {
        /* Настройте дескрипторы рисования дуг для главной детали. */
        lv_draw_arc_dsc_t arc_dsc;
        lv_draw_arc_dsc_init(&arc_dsc);
        arc_dsc.base.layer = layer;
        lv_obj_init_draw_arc_dsc(obj, LV_PART_MAIN, &arc_dsc);

        lv_point_t arc_center;
        int32_t arc_radius;
        scale_get_center(obj, &arc_center, &arc_radius);

        /* TODO: Добавьте компенсацию ширины первого и последнего тика по дуге. */
        const int32_t start_angle = lv_map(scale->range_min, scale->range_min, scale->range_max, scale->rotation,
                                           scale->rotation + scale->angle_range);
        const int32_t end_angle = lv_map(scale->range_max, scale->range_min, scale->range_max, scale->rotation,
                                         scale->rotation + scale->angle_range);

        arc_dsc.center = arc_center;
        arc_dsc.radius = arc_radius;
        arc_dsc.start_angle = start_angle;
        arc_dsc.end_angle = end_angle;

        lv_draw_arc(layer, &arc_dsc);

        lv_scale_section_t * section;
        LV_LL_READ_BACK(&scale->section_ll, section) {
            lv_draw_arc_dsc_t main_arc_section_dsc;
            lv_draw_arc_dsc_init(&main_arc_section_dsc);
            main_arc_section_dsc.base.layer = layer;
            lv_obj_init_draw_arc_dsc(obj, LV_PART_MAIN, &main_arc_section_dsc);

            lv_point_t section_arc_center;
            int32_t section_arc_radius;
            scale_get_center(obj, &section_arc_center, &section_arc_radius);

            /* TODO: Добавьте компенсацию ширины первого и последнего тика по дуге. */
            const int32_t section_start_angle = lv_map(section->range_min, scale->range_min, scale->range_max, scale->rotation,
                                                       scale->rotation + scale->angle_range);
            const int32_t section_end_angle = lv_map(section->range_max, scale->range_min, scale->range_max, scale->rotation,
                                                     scale->rotation + scale->angle_range);

            scale_set_arc_properties(obj, &main_arc_section_dsc, section->main_style);

            main_arc_section_dsc.center = section_arc_center;
            main_arc_section_dsc.radius = section_arc_radius;
            main_arc_section_dsc.start_angle = section_start_angle;
            main_arc_section_dsc.end_angle = section_end_angle;

            lv_draw_arc(layer, &main_arc_section_dsc);
        }
    }
    else { /* Нечего делать */ }
}

/**
 * Получить центральную точку и радиус масштабной дуги
 * @param obj       указатель на масштабируемый объект
 * @param center    указатель на центр
 * @param arc_r     указатель радиуса дуги
 */
static void scale_get_center(const lv_obj_t * obj, lv_point_t * center, int32_t * arc_r)
{
    int32_t left_bg = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    int32_t right_bg = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
    int32_t top_bg = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    int32_t bottom_bg = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);

    int32_t r = (LV_MIN(lv_obj_get_width(obj) - left_bg - right_bg, lv_obj_get_height(obj) - top_bg - bottom_bg)) / 2;

    center->x = obj->coords.x1 + r + left_bg;
    center->y = obj->coords.y1 + r + top_bg;

    if(arc_r) *arc_r = r;
}

/**
 * Получайте баллы за галочки
 *
 * Для рисования делений нам нужны две точки, этот интерфейс возвращает обе точки для всех режимов масштабирования.
 *
 * @param obj       указатель на масштабируемый объект
 * @param tick_idx  индекс текущего тика
 * @param is_major_tick true, еслиtick_idxявляется основным тиком
 * @param tick_point_a  указатель на точку «а» галочки
 * @param tick_point_b  указатель на точку «b» галочки
 */
static void scale_get_tick_points(lv_obj_t * obj, const uint32_t tick_idx, bool is_major_tick,
                                  lv_point_t * tick_point_a, lv_point_t * tick_point_b)
{
    lv_scale_t * scale = (lv_scale_t *)obj;

    /* Основной стиль линии */
    lv_draw_line_dsc_t main_line_dsc;
    lv_draw_line_dsc_init(&main_line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &main_line_dsc);

    int32_t minor_len = 0;
    int32_t major_len = 0;
    int32_t radial_offset = 0;

    if(is_major_tick) {
        major_len = lv_obj_get_style_length(obj, LV_PART_INDICATOR);
        radial_offset = lv_obj_get_style_radial_offset(obj, LV_PART_INDICATOR);
    }
    else {
        minor_len = lv_obj_get_style_length(obj, LV_PART_ITEMS);
        radial_offset = lv_obj_get_style_radial_offset(obj, LV_PART_ITEMS);
    }

    if((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)
       || (LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode || LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode)) {

        /* Получите свойства стиля, чтобы их можно было использовать при рисовании отметок и меток. */
        const int32_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
        const int32_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + border_width;
        const int32_t pad_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN) + border_width;
        const int32_t pad_right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN) + border_width;
        const int32_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + border_width;
        const int32_t tick_pad_right = lv_obj_get_style_pad_right(obj, LV_PART_ITEMS);
        const int32_t tick_pad_left = lv_obj_get_style_pad_left(obj, LV_PART_ITEMS);
        const int32_t tick_pad_top = lv_obj_get_style_pad_top(obj, LV_PART_ITEMS);
        const int32_t tick_pad_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_ITEMS);

        int32_t x_ofs = 0;
        int32_t y_ofs = 0;

        if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) {
            x_ofs = obj->coords.x2 + (main_line_dsc.width / 2) - pad_right;
            y_ofs = obj->coords.y1 + (pad_top + tick_pad_top);
        }
        else if(LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
            x_ofs = obj->coords.x1 + (main_line_dsc.width / 2) + pad_left;
            y_ofs = obj->coords.y1 + (pad_top + tick_pad_top);
        }
        else if(LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) {
            x_ofs = obj->coords.x1 + (pad_right + tick_pad_right);
            y_ofs = obj->coords.y1 + (main_line_dsc.width / 2) + pad_top;
        }
        /* LV_SCALE_MODE_HORIZONTAL_TOP == масштаб->режим */
        else {
            x_ofs = obj->coords.x1 + (pad_left + tick_pad_left);
            y_ofs = obj->coords.y2 + (main_line_dsc.width / 2) - pad_bottom;
        }

        /* Отрегулируйте длину, когда отметка будет отображаться на горизонтальной верхней или вертикальной правой шкале. */
        if((LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode) || (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)) {
            if(is_major_tick) {
                major_len *= -1;
            }
            else {
                minor_len *= -1;
            }
        }
        else { /* Нечего делать */ }

        const int32_t tick_length = is_major_tick ? major_len : minor_len;
        /* NOTE
         * Минус 1, поскольку счетчик тиков начинается с 0.
         * TODO
         * What if total_tick_count is 1? This will lead to an division by 0 further down */
        const uint32_t tmp_tick_count = scale->total_tick_count - 1U;

        /* Рассчитайте положение тиковых точек на основе режима и тикового индекса. */
        if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
            /* Вертикальное положение начинается с y2 основной линии шкалы, мы начинаем с y2, потому что деления рисуются снизу вверх. */
            int32_t vertical_position = obj->coords.y2 - (pad_bottom + tick_pad_bottom);

            /* Поместите последнюю галочку */
            if(tmp_tick_count == tick_idx) {
                vertical_position = y_ofs;
            }
            /* В противном случае отрегулируйте положение галочки в зависимости от ее индекса и количества делений на шкале. */
            else if(0 != tick_idx) {
                const int32_t scale_total_height = lv_obj_get_height(obj) - (pad_top + pad_bottom + tick_pad_top + tick_pad_bottom);
                const int32_t offset = ((int32_t) tick_idx * (int32_t) scale_total_height) / (int32_t)(tmp_tick_count);
                vertical_position -= offset;
            }
            else { /* Нечего делать */ }

            tick_point_a->x = x_ofs - 1; /* Вынести лишний пиксель за пределы масштаба */
            tick_point_a->y = vertical_position;
            tick_point_b->x = tick_point_a->x - tick_length;
            tick_point_b->y = vertical_position;
        }
        else {
            /* Горизонтальное положение начинается с точки x1 основной линии шкалы. */
            int32_t horizontal_position = x_ofs;

            /* Поместите последнюю галочку */
            if(tmp_tick_count == tick_idx) {
                horizontal_position = obj->coords.x2 - (pad_left + tick_pad_left);
            }
            /* В противном случае отрегулируйте положение галочки в зависимости от ее индекса и количества делений на шкале. */
            else if(0U != tick_idx) {
                const int32_t scale_total_width = lv_obj_get_width(obj) - (pad_right + pad_left + tick_pad_right + tick_pad_left);
                const int32_t offset = ((int32_t) tick_idx * (int32_t) scale_total_width) / (int32_t)(tmp_tick_count);
                horizontal_position += offset;
            }
            else { /* Нечего делать */ }

            tick_point_a->x = horizontal_position;
            tick_point_a->y = y_ofs;
            tick_point_b->x = horizontal_position;
            tick_point_b->y = tick_point_a->y + tick_length;
        }
    }
    else if(LV_SCALE_MODE_ROUND_OUTER == scale->mode || LV_SCALE_MODE_ROUND_INNER == scale->mode) {
        lv_area_t scale_area;
        lv_obj_get_content_coords(obj, &scale_area);

        /* Найдите центр шкалы */
        lv_point_t center_point;
        const int32_t radius_edge = LV_MIN(lv_area_get_width(&scale_area) / 2, lv_area_get_height(&scale_area) / 2);
        center_point.x = scale_area.x1 + radius_edge;
        center_point.y = scale_area.y1 + radius_edge;

        int32_t angle_upscale = (int32_t)((tick_idx * scale->angle_range) * 10U) / (scale->total_tick_count - 1U);
        angle_upscale += scale->rotation * 10;

        /* Нарисуйте немного более длинные линии, чтобы быть уверенным, что маска обрежет их правильно.
         * и получить лучшую точность. Добавляем в расчет ширину основной линии, чтобы не было пробелов.
         * между дугой и тиками */
        int32_t point_closer_to_arc = 0;
        int32_t adjusted_radio_with_tick_len = 0;
        if(LV_SCALE_MODE_ROUND_INNER == scale->mode) {
            point_closer_to_arc = radius_edge - main_line_dsc.width;
            adjusted_radio_with_tick_len = point_closer_to_arc - (is_major_tick ? major_len : minor_len);
        }
        /* LV_SCALE_MODE_ROUND_OUTER == масштаб->режим */
        else {
            point_closer_to_arc = radius_edge - main_line_dsc.width;
            adjusted_radio_with_tick_len = point_closer_to_arc + (is_major_tick ? major_len : minor_len);
        }

        tick_point_a->x = center_point.x + point_closer_to_arc + radial_offset;
        tick_point_a->y = center_point.y;
        lv_point_transform(tick_point_a, angle_upscale, LV_SCALE_NONE, LV_SCALE_NONE, &center_point, false);

        tick_point_b->x = center_point.x + adjusted_radio_with_tick_len + radial_offset;
        tick_point_b->y = center_point.y;
        lv_point_transform(tick_point_b, angle_upscale, LV_SCALE_NONE, LV_SCALE_NONE, &center_point, false);
    }
    else { /* Нечего делать */ }
}

/**
 * Получить координаты для метки
 *
 * @param obj       указатель на масштабируемый объект
 * @param label_dsc указатель на дескриптор метки
 * @param tick_point    указатель на ссылочный тик
 * @param label_coords  указатель на вывод координат метки
 */
static void scale_get_label_coords(lv_obj_t * obj, lv_draw_label_dsc_t * label_dsc, lv_point_t * tick_point,
                                   lv_area_t * label_coords)
{
    lv_scale_t * scale = (lv_scale_t *)obj;

    lv_text_attributes_t attributes = {0};
    attributes.letter_space = label_dsc->letter_space;
    attributes.line_space = label_dsc->line_space;
    attributes.max_width = LV_COORD_MAX;
    attributes.text_flags = LV_TEXT_FLAG_NONE;

    /* Зарезервируйте соответствующий размер для галочки. */
    lv_point_t label_size;

    if(label_dsc->text != NULL) {
        lv_text_get_size_attributes(&label_size, label_dsc->text, label_dsc->font, &attributes);
    }
    else {
        label_size.x = 0;
        label_size.y = 0;
    }

    /* Установите область рисования метки на некотором расстоянии от главного тика. */
    if((LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) || (LV_SCALE_MODE_HORIZONTAL_TOP == scale->mode)) {
        label_coords->x1 = tick_point->x - (label_size.x / 2);
        label_coords->x2 = tick_point->x + (label_size.x / 2);

        if(LV_SCALE_MODE_HORIZONTAL_BOTTOM == scale->mode) {
            label_coords->y1 = tick_point->y + lv_obj_get_style_pad_bottom(obj, LV_PART_INDICATOR);
            label_coords->y2 = label_coords->y1 + label_size.y;
        }
        else {
            label_coords->y2 = tick_point->y - lv_obj_get_style_pad_top(obj, LV_PART_INDICATOR);
            label_coords->y1 = label_coords->y2 - label_size.y;
        }
    }
    else if((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) || (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)) {
        label_coords->y1 = tick_point->y - (label_size.y / 2);
        label_coords->y2 = tick_point->y + (label_size.y / 2);

        if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) {
            label_coords->x1 = tick_point->x - label_size.x - lv_obj_get_style_pad_left(obj, LV_PART_INDICATOR);
            label_coords->x2 = tick_point->x - lv_obj_get_style_pad_left(obj, LV_PART_INDICATOR);
        }
        else {
            label_coords->x1 = tick_point->x + lv_obj_get_style_pad_right(obj, LV_PART_INDICATOR);
            label_coords->x2 = tick_point->x + label_size.x + lv_obj_get_style_pad_right(obj, LV_PART_INDICATOR);
        }
    }
    else if(LV_SCALE_MODE_ROUND_OUTER == scale->mode || LV_SCALE_MODE_ROUND_INNER == scale->mode) {
        label_coords->x1 = tick_point->x - (label_size.x / 2);
        label_coords->y1 = tick_point->y - (label_size.y / 2);
        label_coords->x2 = label_coords->x1 + label_size.x;
        label_coords->y2 = label_coords->y1 + label_size.y;
    }
    else { /* Нечего делать */ }
}

/**
 * Установить свойства линии
 *
 * Проверяет, имеет ли линия конфигурацию настраиваемого раздела или нет, и соответствующим образом устанавливает свойства.
 *
 * @param obj       указатель на масштабируемый объект
 * @param line_dsc  указатель на дескриптор строки
 * @param section_style  указатель на стиль раздела индикатора
 * @param part      часть линии, пример:LV_PART_INDICATOR,LV_PART_ITEMS, LV_PART_MAIN
 */
static void scale_set_line_properties(lv_obj_t * obj, lv_draw_line_dsc_t * line_dsc, const lv_style_t * section_style,
                                      lv_part_t part)
{
    if(section_style) {
        lv_style_value_t value;
        lv_style_res_t res;

        /* Ширина линии */
        res = lv_style_get_prop(section_style, LV_STYLE_LINE_WIDTH, &value);
        if(res == LV_STYLE_RES_FOUND) {
            line_dsc->width = (int32_t)value.num;
        }
        else {
            line_dsc->width = lv_obj_get_style_line_width(obj, part);
        }

        /* Цвет линии */
        res = lv_style_get_prop(section_style, LV_STYLE_LINE_COLOR, &value);
        if(res == LV_STYLE_RES_FOUND) {
            line_dsc->color = value.color;
        }
        else {
            line_dsc->color = lv_obj_get_style_line_color(obj, part);
        }

        /* Линия опа */
        res = lv_style_get_prop(section_style, LV_STYLE_LINE_OPA, &value);
        if(res == LV_STYLE_RES_FOUND) {
            line_dsc->opa = (lv_opa_t)value.num;
        }
        else {
            line_dsc->opa = lv_obj_get_style_line_opa(obj, part);
        }
    }
    else {
        line_dsc->color = lv_obj_get_style_line_color(obj, part);
        line_dsc->opa = lv_obj_get_style_line_opa(obj, part);
        line_dsc->width = lv_obj_get_style_line_width(obj, part);
    }
}

/**
 * Установить свойства дуги
 *
 * Проверяет, имеет ли дуга пользовательскую конфигурацию сечения, и соответствующим образом устанавливает свойства.
 *
 * @param obj       указатель на масштабируемый объект
 * @param arc_dsc  указатель на дескриптор дуги
 * @param section_style  указатель на стиль раздела индикатора
 */
static void scale_set_arc_properties(lv_obj_t * obj, lv_draw_arc_dsc_t * arc_dsc, const lv_style_t * section_style)
{
    if(section_style) {
        lv_style_value_t value;
        lv_style_res_t res;

        /* ширина дуги */
        res = lv_style_get_prop(section_style, LV_STYLE_ARC_WIDTH, &value);
        if(res == LV_STYLE_RES_FOUND) {
            arc_dsc->width = (int32_t)value.num;
        }
        else {
            arc_dsc->width = lv_obj_get_style_arc_width(obj, LV_PART_MAIN);
        }

        /* цвет дуги */
        res = lv_style_get_prop(section_style, LV_STYLE_ARC_COLOR, &value);
        if(res == LV_STYLE_RES_FOUND) {
            arc_dsc->color = value.color;
        }
        else {
            arc_dsc->color = lv_obj_get_style_arc_color(obj, LV_PART_MAIN);
        }

        /* дуговая опа */
        res = lv_style_get_prop(section_style, LV_STYLE_ARC_OPA, &value);
        if(res == LV_STYLE_RES_FOUND) {
            arc_dsc->opa = (lv_opa_t)value.num;
        }
        else {
            arc_dsc->opa = lv_obj_get_style_arc_opa(obj, LV_PART_MAIN);
        }

        /* дуга закругленная */
        res = lv_style_get_prop(section_style, LV_STYLE_ARC_ROUNDED, &value);
        if(res == LV_STYLE_RES_FOUND) {
            arc_dsc->rounded = (uint8_t)value.num;
        }
        else {
            arc_dsc->rounded = lv_obj_get_style_arc_rounded(obj, LV_PART_MAIN);
        }

        /* источник изображения дуги */
        res = lv_style_get_prop(section_style, LV_STYLE_ARC_IMAGE_SRC, &value);
        if(res == LV_STYLE_RES_FOUND) {
            arc_dsc->img_src = (const void *)value.ptr;
        }
        else {
            arc_dsc->img_src = lv_obj_get_style_arc_image_src(obj, LV_PART_MAIN);
        }
    }
    else {
        arc_dsc->color = lv_obj_get_style_arc_color(obj, LV_PART_MAIN);
        arc_dsc->opa = lv_obj_get_style_arc_opa(obj, LV_PART_MAIN);
        arc_dsc->width = lv_obj_get_style_arc_width(obj, LV_PART_MAIN);
        arc_dsc->rounded = lv_obj_get_style_arc_rounded(obj, LV_PART_MAIN);
        arc_dsc->img_src = lv_obj_get_style_arc_image_src(obj, LV_PART_MAIN);
    }
}

/**
 * Установить свойства метки индикатора
 *
 * Проверяет, имеет ли индикатор пользовательскую конфигурацию раздела или нет, и соответствующим образом устанавливает свойства.
 *
 * @param obj       указатель на масштабируемый объект
 * @param label_dsc  указатель на дескриптор метки
 * @param indicator_section_style  указатель на стиль раздела индикатора
 */
static void scale_set_indicator_label_properties(lv_obj_t * obj, lv_draw_label_dsc_t * label_dsc,
                                                 const lv_style_t * indicator_section_style)
{
    if(indicator_section_style) {
        lv_style_value_t value;
        lv_style_res_t res;

        /* Цвет текста */
        res = lv_style_get_prop(indicator_section_style, LV_STYLE_TEXT_COLOR, &value);
        if(res == LV_STYLE_RES_FOUND) {
            label_dsc->color = value.color;
        }
        else {
            label_dsc->color = lv_obj_get_style_text_color(obj, LV_PART_INDICATOR);
        }

        /* Текст опа */
        res = lv_style_get_prop(indicator_section_style, LV_STYLE_TEXT_OPA, &value);
        if(res == LV_STYLE_RES_FOUND) {
            label_dsc->opa = (lv_opa_t)value.num;
        }
        else {
            label_dsc->opa = lv_obj_get_style_text_opa(obj, LV_PART_INDICATOR);
        }

        /* Текстовое пространство */
        res = lv_style_get_prop(indicator_section_style, LV_STYLE_TEXT_LETTER_SPACE, &value);
        if(res == LV_STYLE_RES_FOUND) {
            label_dsc->letter_space = (int32_t)value.num;
        }
        else {
            label_dsc->letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_INDICATOR);
        }

        /* Текстовый шрифт */
        res = lv_style_get_prop(indicator_section_style, LV_STYLE_TEXT_FONT, &value);
        if(res == LV_STYLE_RES_FOUND) {
            label_dsc->font = (const lv_font_t *)value.ptr;
        }
        else {
            label_dsc->font = lv_obj_get_style_text_font(obj, LV_PART_INDICATOR);
        }
    }
    else {
        /* Если метка находится за пределами диапазона, получите стиль индикатора. */
        label_dsc->color = lv_obj_get_style_text_color(obj, LV_PART_INDICATOR);
        label_dsc->opa = lv_obj_get_style_text_opa(obj, LV_PART_INDICATOR);
        label_dsc->letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_INDICATOR);
        label_dsc->font = lv_obj_get_style_text_font(obj, LV_PART_INDICATOR);
    }
}

static void scale_find_section_tick_idx(lv_obj_t * obj)
{
    lv_scale_t * scale = (lv_scale_t *)obj;

    const int32_t min_out = scale->range_min;
    const int32_t max_out = scale->range_max;
    const uint32_t total_tick_count = scale->total_tick_count;

    /* Обработка разделов */
    uint32_t tick_idx = 0;
    for(tick_idx = 0; tick_idx < total_tick_count; tick_idx++) {
        bool is_major_tick = scale_is_major_tick(scale, tick_idx);

        const int32_t tick_value = lv_map(tick_idx, 0, total_tick_count - 1, min_out, max_out);

        lv_scale_section_t * section;
        LV_LL_READ_BACK(&scale->section_ll, section) {
            if(section->range_min <= tick_value && section->range_max >= tick_value) {
                if(LV_SCALE_TICK_IDX_DEFAULT_ID == section->first_tick_idx_in_section) {
                    section->first_tick_idx_in_section = tick_idx;
                    section->first_tick_idx_is_major = is_major_tick;
                }
                if(LV_SCALE_TICK_IDX_DEFAULT_ID == section->last_tick_idx_in_section) {
                    /* Это инициализирует его, когда начальное и конечное значения диапазона совпадают. */
                    section->last_tick_idx_in_section = tick_idx;
                    section->last_tick_idx_is_major = is_major_tick;
                }
                /* Теперь продолжайте сохранять значение`last_tick_idx_...`, как мы.
                 * пройдите через цикл `for`, чтобы он остался с
                 * фактическое значение последнего тика, находящееся в пределах диапазона шкалы. */
                else if(section->first_tick_idx_in_section != tick_idx) {
                    section->last_tick_idx_in_section = tick_idx;
                    section->last_tick_idx_is_major = is_major_tick;
                }
            }
            else {
                /* `tick_value` находится за пределами зоны действия раздела.
                 * Нечего делать. */
            }
        }
    }
}

/**
 * Сохраняет ширину начального и последнего тика основной линии.
 *
 * Эта ширина используется для компенсации рисования основной линии с учетом ширины обеих делений.
 *
 * @param obj       указатель на масштабируемый объект
 * @param tick_idx  индекс текущего тика
 * @param is_major_tick true, еслиtick_idxявляется основным тиком
 * @param major_tick_width ширина основного тика
 * @param minor_tick_width ширина второстепенного тика
 */
static void scale_store_main_line_tick_width_compensation(lv_obj_t * obj, const uint32_t tick_idx,
                                                          const bool is_major_tick, const int32_t major_tick_width, const int32_t minor_tick_width)
{
    lv_scale_t * scale = (lv_scale_t *)obj;
    const bool is_first_tick = 0U == tick_idx;
    const bool is_last_tick = scale->total_tick_count == tick_idx;
    const int32_t tick_width = is_major_tick ? major_tick_width : minor_tick_width;

    /* Выходит раньше, еслиtick_idxне является ни первым, ни последним тиком на основной линии. */
    if(((!is_last_tick) && (!is_first_tick))
       /* Выйдите раньше, если режим масштабирования круглый. Он не поддерживает компенсацию основной линии. */
       || ((LV_SCALE_MODE_ROUND_INNER == scale->mode) || (LV_SCALE_MODE_ROUND_OUTER == scale->mode))) {
        return;
    }

    if(is_last_tick) {
        /* Режим вертикальный */
        if((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) || (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)) {
            scale->last_tick_width = tick_width;
        }
        /* Режим горизонтальный */
        else {
            scale->first_tick_width = tick_width;
        }
    }
    /* is_first_tick */
    else {
        /* Режим вертикальный */
        if((LV_SCALE_MODE_VERTICAL_LEFT == scale->mode) || (LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode)) {
            scale->first_tick_width = tick_width;
        }
        /* Режим горизонтальный */
        else {
            scale->last_tick_width = tick_width;
        }
    }
}

/**
 * Устанавливает текст дескриптора метки галочки при использовании пользовательских меток.
 *
 * Устанавливает текстовый указатель, если доступна действующая пользовательская метка, в противном случае установите для него значение NULL .
 *
 * @param obj       указатель на масштабируемый объект
 * @param label_dsc указатель на дескриптор метки
 * @param major_tick_idx  индекс текущего основного тика
 */
static void scale_build_custom_label_text(lv_obj_t * obj, lv_draw_label_dsc_t * label_dsc,
                                          const uint16_t major_tick_idx)
{
    lv_scale_t * scale = (lv_scale_t *) obj;

    /* Проверьте, имеются ли на весах допустимые пользовательские этикетки.
     * это позволяет избежать чтения массиваtxt_src, когда для измерения требуется больше меток деления, чем доступно */
    if(major_tick_idx <= scale->custom_label_cnt) {
        if(scale->txt_src[major_tick_idx - 1U]) {
            label_dsc->text = scale->txt_src[major_tick_idx - 1U];
            label_dsc->text_local = 0;
        }
        else {
            label_dsc->text = NULL;
        }
    }
    else {
        label_dsc->text = NULL;
    }
}

/**
 * Сохраняет информацию о компенсации ширины деления для основных участков строки.
 *
 * @param obj       указатель на масштабируемый объект
 * @param is_major_tick Указывает, является ли галочка основной или нет
 * @param major_tick_dsc указатель на major_tick_dsc
 * @param minor_tick_dsc указатель на minor_tick_dsc
 * @param tick_value Текущее значение тика, используемое для определения того, принадлежит лиtick_idxразделу или нет.
 * @param tick_idx Текущий тиковый индекс
 * @param tick_point_a Указатель для отметки точки a
 */
static void scale_store_section_line_tick_width_compensation(lv_obj_t * obj, const bool is_major_tick,
                                                             lv_draw_line_dsc_t * major_tick_dsc, lv_draw_line_dsc_t * minor_tick_dsc,
                                                             const int32_t tick_value, const uint8_t tick_idx, lv_point_t * tick_point_a)
{
    lv_scale_t * scale = (lv_scale_t *) obj;
    lv_scale_section_t * section;

    LV_LL_READ_BACK(&scale->section_ll, section) {
        if(section->range_min <= tick_value && section->range_max >= tick_value) {
            if(is_major_tick) {
                scale_set_line_properties(obj, major_tick_dsc, section->indicator_style, LV_PART_INDICATOR);
            }
            else {
                scale_set_line_properties(obj, minor_tick_dsc, section->items_style, LV_PART_ITEMS);
            }
        }

        int32_t tmp_width = 0;

        if(tick_idx == section->first_tick_idx_in_section) {
            if(section->first_tick_idx_is_major) {
                tmp_width = major_tick_dsc->width;
            }
            else {
                tmp_width = minor_tick_dsc->width;
            }

            section->first_tick_in_section = *tick_point_a;
            /* Добавьте 1 пиксель в качестве корректировки, еслиtmp_widthнечетное значение. */
            if(tmp_width & 0x01U) {
                if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
                    tmp_width += 1;
                }
                else {
                    tmp_width -= 1;
                }
            }
            section->first_tick_in_section_width = tmp_width;
        }

        /* Это также может применяться, когда
         * (tick_idx== раздел->first_tick_idx_in_section), когда
         * начальное и конечное значения диапазона одинаковы. */
        if(tick_idx == section->last_tick_idx_in_section) {
            if(section->last_tick_idx_is_major) {
                tmp_width = major_tick_dsc->width;
            }
            else {
                tmp_width = minor_tick_dsc->width;
            }

            section->last_tick_in_section = *tick_point_a;
            /* Добавьте 1 пиксель в качестве корректировки, еслиtmp_widthнечетное значение. */
            if(tmp_width & 0x01U) {
                if(LV_SCALE_MODE_VERTICAL_LEFT == scale->mode || LV_SCALE_MODE_VERTICAL_RIGHT == scale->mode) {
                    tmp_width -= 1;
                }
                else {
                    tmp_width += 1;
                }
            }
            section->last_tick_in_section_width = tmp_width;
        }
        else { /* Нечего делать */ }
    }
}

static void scale_free_line_needle_points_cb(lv_event_t * e)
{
    lv_point_precise_t * needle_line_points = lv_event_get_user_data(e);
    lv_free(needle_line_points);
}

static bool scale_is_major_tick(lv_scale_t * scale, uint32_t tick_idx)
{
    return scale->major_tick_every != 0 && tick_idx % scale->major_tick_every == 0;
}

static lv_result_t update_needle(lv_scale_t * scale, lv_obj_t * needle, int32_t length, int32_t value)
{
    /* Сначала попытайтесь найти иголку в стоге сена (список иголок весов) */
    size_t needle_count = lv_array_size(&scale->needles);
    for(size_t i = 0; i < needle_count; ++i) {
        lv_scale_needle_t * scale_needle = lv_array_at(&scale->needles, i);
        if(scale_needle->obj == needle) {
            scale_needle->value = value;
            scale_needle->length = length;
            return LV_RESULT_OK;
        }
    }

    /* Игла еще не часть стога сена */
    lv_scale_needle_t scale_needle = {.obj = needle, .length = length, .value = value};
    lv_result_t res = lv_array_push_back(&scale->needles, &scale_needle);
    if(res != LV_RESULT_OK) {
        LV_LOG_WARN("Failed to attach needle to scale - not enough memory");
        return LV_RESULT_INVALID;
    }

    lv_obj_add_event_cb(needle, needle_deleted_cb, LV_EVENT_DELETE, scale);
    return LV_RESULT_OK;
}

static void needle_deleted_cb(lv_event_t * e)
{
    lv_scale_t * scale = lv_event_get_user_data(e);
    lv_obj_t * needle = lv_event_get_target_obj(e);

    size_t needle_count = lv_array_size(&scale->needles);
    for(size_t i = 0; i < needle_count; ++i) {
        lv_scale_needle_t * scale_needle = lv_array_at(&scale->needles, i);
        if(scale_needle->obj == needle) {
            lv_array_remove(&scale->needles, i);
            return;
        }
    }
}

#if LV_USE_OBSERVER

static void scale_section_min_value_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_scale_section_t * section = observer->user_data;
    lv_scale_set_section_min_value(observer->target, section, subject->value.num);
}

static void scale_section_max_value_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_scale_section_t * section = observer->user_data;
    lv_scale_set_section_max_value(observer->target, section, subject->value.num);
}

#endif /*LV_USE_OBSERVER*/


#endif
