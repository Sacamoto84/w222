#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_SLIDER

static lv_obj_t * label;

static void slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target_obj(e);

    /*Обновить текст*/
    lv_label_set_text_fmt(label, "%" LV_PRId32, lv_slider_get_value(slider));
    lv_obj_align_to(label, slider, LV_ALIGN_OUT_TOP_MID, 0, -15);    /*Выровнять верхнюю часть слайдера*/
}

/**
 * Создайте ползунок и напишите его значение на этикетке.
 */
void lv_example_get_started_4(void)
{
    /*Создайте слайдер в центре дисплея.*/
    lv_obj_t * slider = lv_slider_create(lv_screen_active());
    lv_obj_set_width(slider, 200);                          /*Установите ширину*/
    lv_obj_center(slider);                                  /*Выровнять по центру родительского элемента (экрана)*/
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);     /*Назначьте функцию события*/

    /*Создайте метку над ползунком*/
    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "0");
    lv_obj_align_to(label, slider, LV_ALIGN_OUT_TOP_MID, 0, -15);    /*Выровнять верхнюю часть слайдера*/
}

#endif
