#include "../../lv_examples.h"
#if LV_USE_LABEL && LV_BUILD_EXAMPLES

/**
 * Демонстрация настройки анимации круговой прокрутки метки с помощью `LV_LABEL_LONG_MODE_SCROLL_CIRCULAR`.
 * длинный режим.
 */
void lv_example_label_5(void)
{
    static lv_anim_t animation_template;
    static lv_style_t label_style;

    lv_anim_init(&animation_template);
    lv_anim_set_delay(&animation_template, 1000);           /*Подождите 1 секунду, чтобы начать первую прокрутку.*/
    lv_anim_set_repeat_delay(&animation_template,
                             3000);    /*Повторите прокрутку через 3 секунды после того, как метка вернется в исходное положение.*/
    lv_anim_set_repeat_count(&animation_template, LV_ANIM_REPEAT_INFINITE);

    /*Инициализируйте стиль метки с помощью шаблона анимации.*/
    lv_style_init(&label_style);
    lv_style_set_anim(&label_style, &animation_template);

    lv_obj_t * label1 = lv_label_create(lv_screen_active());
    lv_label_set_long_mode(label1, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);      /*Круговой свиток*/
    lv_obj_set_width(label1, 150);
    lv_label_set_text(label1, "It is a circularly scrolling text. ");
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 40);
    lv_obj_add_style(label1, &label_style, LV_STATE_DEFAULT);           /*Добавьте стиль к метке*/
}

#endif
