/*******************************************************************
 *
 * @file lv_example_gestures.c
 *
 * Это простой пример программы, демонстрирующий, как использовать
 * распознавание жестовAPI, обратитесь кlv_indev_gesture.hили документации
 * для более подробной информации
 *
 * Приложение начинается с одного прямоугольника. Затем пользователь может
 * - Сожмите прямоугольник, чтобы масштабировать его.
 * - Поверните прямоугольник, чтобы повернуть его
 * - Проведите двумя пальцами по прямоугольнику, чтобы увидеть направление и расстояние смахивания.
 *
 * Copyright (c) 2024 EDGEMTech Ltd
 *
 * Author: ООО «ЭДГЕМТех», Эрик Тагиров (erik.tagiros@edgemtech.ch)
 *
 ******************************************************************/

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_examples.h"

#if LV_USE_GESTURE_RECOGNITION && LV_BUILD_EXAMPLES

/*********************
 *      DEFINES
 *********************/

#define RECT_INIT_WIDTH 300
#define RECT_INIT_HEIGHT 300
#define RECT_COLOR 0xC1BCFF

#ifndef M_PI
    #define M_PI 3.1415926f
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void label_scale(lv_event_t * gesture_event);
static void label_rotate(lv_event_t * gesture_event);
static void label_swipe(lv_event_t * gesture_event);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_obj_t * label;
static lv_style_t label_style;
static uint32_t label_width;
static uint32_t label_height;
static uint32_t label_x;
static uint32_t label_y;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


/**
 * Точка входа создает экран и метку
 * Установите обратные вызовы событий на метке
 */
void lv_example_gestures(void)
{
    lv_obj_t * root_view;

    label_width = RECT_INIT_WIDTH;
    label_height = RECT_INIT_HEIGHT;
    label_x = LV_HOR_RES / 2 - (label_width / 2);
    label_y = LV_VER_RES / 2 - (label_height / 2);

    root_view = lv_screen_active();

    lv_obj_set_style_bg_color(root_view, lv_color_hex(0xffffff), LV_PART_MAIN);
    label = lv_label_create(root_view);
    lv_obj_remove_flag(root_view, LV_OBJ_FLAG_SCROLLABLE);

    lv_label_set_text(label, "Zoom, rotate or move");
    lv_obj_add_flag(label, LV_OBJ_FLAG_CLICKABLE);

    lv_style_init(&label_style);
    lv_style_set_bg_color(&label_style, lv_color_hex(RECT_COLOR));
    lv_style_set_bg_opa(&label_style, LV_OPA_COVER);

    lv_style_set_width(&label_style, (int)label_width);
    lv_style_set_height(&label_style, (int)label_height);

    lv_style_set_x(&label_style, (int)label_x);
    lv_style_set_y(&label_style, (int)label_y);

    lv_obj_add_style(label, &label_style, LV_STATE_DEFAULT);

    lv_obj_add_event_cb(label, label_rotate, LV_EVENT_GESTURE, label);
    lv_obj_add_event_cb(label, label_scale, LV_EVENT_GESTURE, label);
    lv_obj_add_event_cb(label, label_swipe, LV_EVENT_GESTURE, label);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Вызывается при возникновении события LV_EVENT_GESTURE — обновляет метку, если жест представляет собой пролистывание.
 * @param gesture_event             указатель на событие LV_EVENT_GESTURE
 */
static void label_swipe(lv_event_t * gesture_event)
{

    lv_dir_t dir;
    lv_indev_gesture_state_t state;
    const char * text;

    if(lv_event_get_gesture_type(gesture_event) != LV_INDEV_GESTURE_TWO_FINGERS_SWIPE) {
        return;
    }

    state = lv_event_get_gesture_state(gesture_event, LV_INDEV_GESTURE_TWO_FINGERS_SWIPE);
    dir = lv_event_get_two_fingers_swipe_dir(gesture_event);

    if(state == LV_INDEV_GESTURE_STATE_ENDED) {

        text = "NONE";
        lv_label_set_text(label, text);
    }
    else if(state == LV_INDEV_GESTURE_STATE_RECOGNIZED) {

        switch(dir) {
            case LV_DIR_LEFT:
                text = "LEFT";
                break;
            case LV_DIR_RIGHT:
                text = "RIGHT";
                break;
            case LV_DIR_TOP:
                text = "TOP";
                break;
            case LV_DIR_BOTTOM:
                text = "BOTTOM";
                break;
            default:
                text = "???";
                break;
        }

        lv_label_set_text_fmt(label, "%s - %f", text, lv_event_get_two_fingers_swipe_distance(gesture_event));
    }

}

/**
 * Вызывается при возникновении события LV_EVENT_GESTURE — масштабирует метку, если жест представляет собой сжатие.
 * @param gesture_event указатель на событие LV_EVENT_GESTURE
 */
static void label_scale(lv_event_t * gesture_event)
{

    static int initial_w = -1;
    static int initial_h = -1;
    lv_indev_gesture_state_t state;
    static lv_point_t center_pnt;
    static float base_scale = 1.0;
    float scale;
    float label_width_float;
    float label_height_float;

    /* Убедитесь, что жест является щипком */
    if(lv_event_get_gesture_type(gesture_event) != LV_INDEV_GESTURE_PINCH) {
        return;
    }

    state = lv_event_get_gesture_state(gesture_event, LV_INDEV_GESTURE_PINCH);

    scale = base_scale * lv_event_get_pinch_scale(gesture_event);

    /* Сбросить состояние после завершения жеста */
    if(state == LV_INDEV_GESTURE_STATE_ENDED) {

        /* Жест сжатия завершился: сброс ширины/высоты для следующего жеста сжатия.*/
        initial_w = -1;
        initial_h = -1;

        base_scale = scale;

        return;
    }

    /* При первом распознавании жеста сохраните его центр. */
    if(state == LV_INDEV_GESTURE_STATE_RECOGNIZED) {

        if((initial_h == -1 || initial_w == -1)) {

            /* Распознан щипковый жест - это первое событие из серии признанных событий. */
            /* Масштабирование применяется относительно исходной ширины/высоты прямоугольника. */
            initial_w = label_width;
            initial_h = label_height;
            center_pnt.x = lv_obj_get_x(label) + label_width / 2;
            center_pnt.y = lv_obj_get_y(label) + label_height / 2;
        }

        /* Жест распознан, теперь мы можем использовать шкалу */

        /* Позволяет избежать ситуации, когда прямоугольник становится слишком маленьким или слишком большим,
        * добавление ограничений */
        if(scale < 0.4f) {
            scale = 0.4f;
        }
        else if(scale > 2.0f) {
            scale = 2.0f;
        }

        label_x = center_pnt.x - label_width / 2;
        label_y = center_pnt.y - label_height / 2;

        label_width_float = (float)RECT_INIT_WIDTH * scale;
        label_height_float = (float)RECT_INIT_HEIGHT * scale;

        /* Обновить положение и размер */
        lv_style_set_width(&label_style, (int)label_width_float);
        lv_style_set_height(&label_style, (int)label_height_float);
        lv_style_set_x(&label_style, (int)label_x);
        lv_style_set_y(&label_style, (int)label_y);

        lv_obj_add_style(label, &label_style, LV_STATE_DEFAULT);

        label_width = (int)label_width_float;
        label_height = (int)label_height_float;
    }
}


/**
 * Вызывается при возникновении события LV_EVENT_GESTURE — поверните метку, если жест является вращением.
 * @param gesture_event             указатель на событие LV_EVENT_GESTURE
 */
static void label_rotate(lv_event_t * gesture_event)
{

    float angle_degrees = 0.f;
    static float start_angle = 0.f;
    lv_indev_gesture_state_t state;

    if(lv_event_get_gesture_type(gesture_event) != LV_INDEV_GESTURE_ROTATE) {
        return;
    }

    state = lv_event_get_gesture_state(gesture_event, LV_INDEV_GESTURE_ROTATE);

    /* Вычислите новый угол. Значение x10 связано с тем, чтоlv_obj_set_style_transform_rotationиспользует угол x10 в параметре. */
    angle_degrees = start_angle + 10.0f * (lv_event_get_rotation(gesture_event) * 180.0f / M_PI);

    /* Как только жестом закончится, сохраните текущий угол вstart_angle. */
    if(state == LV_INDEV_GESTURE_STATE_ENDED) {

        start_angle = angle_degrees;
    }

    /* Если жест распознан, поверните метку */
    if(state == LV_INDEV_GESTURE_STATE_RECOGNIZED) {

        /* Необходимо установить ось в центр виджета, чтобы он не вращался.
        вокруг верхнего левого угла */
        lv_obj_set_style_transform_pivot_x(label, lv_obj_get_width(label) / 2, 0);
        lv_obj_set_style_transform_pivot_y(label, lv_obj_get_height(label) / 2, 0);

        lv_obj_set_style_transform_rotation(label, (int) angle_degrees, 0);
    }

    return;
}

#endif /* LV_USE_GESTURE_RECOGNITION && LV_USE_FLOAT */
