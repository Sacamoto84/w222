#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_FLEX

static void event_cb(lv_event_t * e)
{
    /*Исходная цель мероприятия. Это могут быть кнопки или контейнер*/
    lv_obj_t * target = lv_event_get_target_obj(e);

    /*Текущей целью всегда является контейнер, поскольку в него добавляется событие.*/
    lv_obj_t * cont = lv_event_get_current_target_obj(e);

    /*Если контейнер был нажат, ничего не делайте*/
    if(target == cont) return;

    /*Сделайте нажатые кнопки красными*/
    lv_obj_set_style_bg_color(target, lv_palette_main(LV_PALETTE_RED), 0);
}

/**
 * Демонстрация всплытия событий
 */
void lv_example_event_bubble(void)
{

    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, 290, 200);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);

    uint32_t i;
    for(i = 0; i < 30; i++) {
        lv_obj_t * btn = lv_button_create(cont);
        lv_obj_set_size(btn, 70, 50);
        lv_obj_add_flag(btn, LV_OBJ_FLAG_EVENT_BUBBLE);

        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text_fmt(label, "%" LV_PRIu32, i);
        lv_obj_center(label);
    }

    lv_obj_add_event_cb(cont, event_cb, LV_EVENT_CLICKED, NULL);
}

#endif
