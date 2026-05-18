#include "../../lv_examples.h"
#if LV_USE_OBSERVER && LV_USE_SLIDER && LV_USE_LABEL && LV_BUILD_EXAMPLES

/*Стиль по умолчанию для светлой темы*/
static lv_style_t style_screen;

static lv_style_t style_slider_main;
static lv_style_t style_slider_indicator;
static lv_style_t style_slider_knob;

/*Стиль, чтобы сделать тему по умолчанию темной.*/
static lv_style_t style_screen_dark;
static lv_style_t style_yellow;
static lv_style_t style_bg_dark;

/*Предметы по температуре и выбранной теме*/
static lv_subject_t subject_room_temperature;
static lv_subject_t subject_theme;

/**
 * Очень простой и элегантный способ создания светлых и темных тем с помощью предметов.
 */
void lv_example_observer_7(void)
{
    /*-------------------
     * Инициализируйте предметы
     *-------------------*/

    lv_subject_init_int(&subject_theme, 0); /*Светлая тема по умолчанию*/
    lv_subject_init_int(&subject_room_temperature, 25);

    /*-------------------
     * Инициализировать стили
     *-------------------*/

    /*Инициализировать стили освещения по умолчанию*/
    lv_style_init(&style_screen);
    lv_style_set_bg_color(&style_screen, lv_color_hex3(0xccc));

    lv_style_init(&style_slider_main);
    lv_style_set_radius(&style_slider_main, 2);
    lv_style_set_bg_color(&style_slider_main, lv_palette_main(LV_PALETTE_RED));

    lv_style_init(&style_slider_indicator);
    lv_style_set_bg_color(&style_slider_indicator, lv_palette_main(LV_PALETTE_RED));

    lv_style_init(&style_slider_knob);
    lv_style_set_bg_color(&style_slider_knob, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_outline_color(&style_slider_knob,  lv_color_hex3(0xfff));
    lv_style_set_outline_width(&style_slider_knob,  4);

    /*Инициализируйте стили, которые будут перезаписывать цвет темной темы.*/
    lv_style_init(&style_screen_dark);
    lv_style_set_bg_color(&style_screen_dark, lv_color_hex3(0x444));
    lv_style_set_text_color(&style_screen_dark, lv_color_hex3(0xeee));

    lv_style_init(&style_bg_dark);
    lv_style_set_bg_color(&style_bg_dark, lv_color_hex3(0x222));
    lv_style_set_text_color(&style_bg_dark, lv_color_hex3(0xeee));
    lv_style_set_border_opa(&style_bg_dark, LV_OPA_30);

    lv_style_init(&style_yellow);
    lv_style_set_bg_color(&style_yellow, lv_palette_main(LV_PALETTE_YELLOW));
    lv_style_set_outline_color(&style_yellow, lv_color_hex3(0x222));

    /*----------------
     * Создание виджетов
     *----------------*/

    /*Добавляйте светлые их на экран и привязывайте темный стиль только в том случае, если
     *выбрана темная тема*/
    lv_obj_add_style(lv_screen_active(), &style_screen, 0);
    lv_obj_bind_style(lv_screen_active(), &style_screen_dark, 0, &subject_theme, 1);

    /*Создайте контейнер и добавьте темный стиль, если выбрана темная тема.*/
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_bind_style(cont, &style_bg_dark, 0, &subject_theme, 1);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t * label = lv_label_create(cont);
    lv_label_bind_text(label, &subject_room_temperature, "%d °C");

    /*Привяжите ползунок к теме температуры и некоторым ее стилям.
     *предмет темы*/
    lv_obj_t * slider = lv_slider_create(cont);
    lv_slider_bind_value(slider, &subject_room_temperature);
    lv_obj_add_style(slider, &style_slider_main, 0);
    lv_obj_add_style(slider, &style_slider_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(slider, &style_slider_knob, LV_PART_KNOB);
    lv_obj_bind_style(slider, &style_yellow, 0, &subject_theme, 1);
    lv_obj_bind_style(slider, &style_yellow, LV_PART_INDICATOR, &subject_theme, 1);
    lv_obj_bind_style(slider, &style_yellow, LV_PART_KNOB, &subject_theme, 1);
    lv_slider_set_range(slider, 20, 40);

    /*Создайте раскрывающийся список для выбора темы.
     *Также привяжите затемнение, если выбрана темная тема.*/
    lv_obj_t * dropdown = lv_dropdown_create(lv_screen_active());
    lv_obj_align(dropdown, LV_ALIGN_TOP_MID, 0, 120);
    lv_dropdown_set_options(dropdown, "Light\nDark");
    lv_dropdown_bind_value(dropdown, &subject_theme);
    lv_obj_bind_style(dropdown, &style_bg_dark, 0, &subject_theme, 1);
    lv_obj_bind_style(lv_dropdown_get_list(dropdown), &style_bg_dark, 0, &subject_theme, 1);
}


#endif
