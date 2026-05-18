#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_BUTTON

static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target_obj(e);
    if(code == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;

        /*Получите первый дочерний элемент кнопки, которая является меткой, и измените ее текст.*/
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "Button: %d", cnt);
    }
}

/**
 * Создайте кнопку с меткой и реагируйте на событие нажатия.
 */
void lv_example_get_started_2(void)
{
    lv_obj_t * btn = lv_button_create(lv_screen_active());     /*Добавить кнопку на текущий экран*/
    lv_obj_set_pos(btn, 10, 10);                            /*Установите его положение*/
    lv_obj_set_size(btn, 120, 50);                          /*Установите его размер*/
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Назначьте обратный вызов кнопке*/

    lv_obj_t * label = lv_label_create(btn);          /*Добавьте метку к кнопке*/
    lv_label_set_text(label, "Button");                     /*Установите текст меток*/
    lv_obj_center(label);
}

#endif
