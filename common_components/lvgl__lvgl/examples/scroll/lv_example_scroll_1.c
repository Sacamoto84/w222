#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

static lv_obj_t  * panel;
static lv_obj_t  * save_button;
static lv_obj_t  * restore_button;
static int         saved_scroll_x;
static int         saved_scroll_y;

static void scroll_update_cb(lv_event_t * e);
static void button_event_cb(lv_event_t * e);

static void scroll_update_cb(lv_event_t * e)
{
    LV_UNUSED(e);

    LV_LOG("scroll info: x:%3" LV_PRId32", y:%3" LV_PRId32", top:%3" LV_PRId32", "
           "bottom:%3" LV_PRId32", left:%3" LV_PRId32", right:%3" LV_PRId32"\n",
           lv_obj_get_scroll_x(panel),
           lv_obj_get_scroll_y(panel),
           lv_obj_get_scroll_top(panel),
           lv_obj_get_scroll_bottom(panel),
           lv_obj_get_scroll_left(panel),
           lv_obj_get_scroll_right(panel)
          );
}

static void button_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target_obj(e);

    if(obj == save_button) {
        saved_scroll_x = lv_obj_get_scroll_x(panel);
        saved_scroll_y = lv_obj_get_scroll_y(panel);
    }
    else {
        lv_obj_scroll_to(panel, saved_scroll_x, saved_scroll_y, LV_ANIM_ON);
    }
}

/**
 * Продемонстрируйте, как прокрутка появляется автоматически
 */
void lv_example_scroll_1(void)
{
    /*Создайте объект с новым стилем*/
    lv_obj_t * scr;
    scr = lv_screen_active();
    panel = lv_obj_create(scr);
    lv_obj_set_size(panel, 200, 200);
    lv_obj_align(panel, LV_ALIGN_CENTER, 44, 0);

    lv_obj_t * child;
    lv_obj_t * label;

    child = lv_obj_create(panel);
    lv_obj_set_pos(child, 0, 0);
    lv_obj_set_size(child, 70, 70);
    label = lv_label_create(child);
    lv_label_set_text(label, "Zero");
    lv_obj_center(label);

    child = lv_obj_create(panel);
    lv_obj_set_pos(child, 160, 80);
    lv_obj_set_size(child, 80, 80);

    lv_obj_t * child2 = lv_button_create(child);
    lv_obj_set_size(child2, 100, 50);

    label = lv_label_create(child2);
    lv_label_set_text(label, "Right");
    lv_obj_center(label);

    child = lv_obj_create(panel);
    lv_obj_set_pos(child, 40, 160);
    lv_obj_set_size(child, 100, 70);
    label = lv_label_create(child);
    lv_label_set_text(label, "Bottom");
    lv_obj_center(label);

    /* Когда LV_OBJ_FLAG_SCROLL_ELASTIC очищен, прокрутка не выходит за границы границ. */
    /* lv_obj_remove_flag (панель,LV_OBJ_FLAG_SCROLL_ELASTIC); */

    /* Вызовите`scroll_update_cb`во время прокрутки панели. */
    lv_obj_add_event_cb(panel, scroll_update_cb, LV_EVENT_SCROLL, NULL);

    /* Настройте кнопки, которые сохраняют и восстанавливают положение прокрутки. */
    save_button = lv_button_create(scr);
    restore_button = lv_button_create(scr);
    lv_obj_t * lbl;
    lbl = lv_label_create(save_button);
    lv_label_set_text_static(lbl, "Save");
    lbl = lv_label_create(restore_button);
    lv_label_set_text_static(lbl, "Restore");
    lv_obj_align_to(save_button, panel, LV_ALIGN_OUT_LEFT_MID, -10, -20);
    lv_obj_align_to(restore_button, panel, LV_ALIGN_OUT_LEFT_MID, -10, 20);
    lv_obj_add_event_cb(save_button, button_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(restore_button, button_event_cb, LV_EVENT_CLICKED, NULL);
}

#endif
