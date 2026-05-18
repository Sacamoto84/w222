#include "../../lv_examples.h"

#if LV_BUILD_EXAMPLES

#if LV_USE_TINY_TTF && LV_TINY_TTF_FILE_SUPPORT

/**
 * Загрузите шрифт сTiny_TTFиз файла
 */
void lv_example_tiny_ttf_2(void)
{
    /*Создайте стиль с помощью нового шрифта*/
    static lv_style_t style;
    lv_style_init(&style);
    lv_font_t * font = lv_tiny_ttf_create_file("A:lvgl/examples/libs/tiny_ttf/Ubuntu-Medium.ttf", 30);
    lv_style_set_text_font(&style, font);
    lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);

    /*Создайте этикетку с новым стилем*/
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_add_style(label, &style, 0);
    lv_label_set_text(label, "Hello world\nI'm a font\ncreated\nwith Tiny TTF");
    lv_obj_center(label);
}
#else

void lv_example_tiny_ttf_2(void)
{
    /*TODO
     *запасной вариант для онлайн-примеров*/

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "TinyTTF can't load files in the browser");
    lv_obj_center(label);
}

#endif /*LV_USE_TINY_TTF && LV_TINY_TTF_FILE_SUPPORT*/

#endif /*LV_BUILD_EXAMPLES*/
