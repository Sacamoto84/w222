#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_IMAGE
#include "../../lvgl_private.h"

static lv_style_t style_btn;


/*Будет вызван, когда стили базовой темы уже добавлены.
  добавить новые стили*/
static void new_theme_apply_cb(lv_theme_t * th, lv_obj_t * obj)
{
    LV_UNUSED(th);

    if(lv_obj_check_type(obj, &lv_button_class)) {
        lv_obj_add_style(obj, &style_btn, 0);
    }
}

static void on_display_delete(lv_event_t * e)
{
    lv_display_t * display = (lv_display_t *) lv_event_get_target(e);
    lv_theme_t * display_theme = lv_display_get_theme(display);
    lv_theme_delete(display_theme);
}

static void new_theme_init_and_set(void)
{
    lv_display_t * display = lv_display_get_default();

    /*Инициализируйте стили*/
    lv_style_init(&style_btn);
    lv_style_set_bg_color(&style_btn, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_border_color(&style_btn, lv_palette_darken(LV_PALETTE_GREEN, 3));
    lv_style_set_border_width(&style_btn, 3);

    /* Инициализируйте новую тему, используя текущую тему в качестве родительской.
     * Пользователь несет ответственность за освобождение темы, когда она больше не нужна. */
    lv_theme_t * th_act = lv_display_get_theme(NULL);
    lv_theme_t * th_new = lv_theme_create();
    lv_theme_copy(th_new, th_act);
    lv_theme_set_parent(th_new, th_act);

    /*Установите обратный вызов стиля для новой темы.*/
    lv_theme_set_apply_cb(th_new, new_theme_apply_cb);

    /*Назначьте новую тему текущему дисплею*/
    lv_display_set_theme(display, th_new);

    /* Назначьте обратный вызов события удаления, чтобы мы могли удалить тему при удалении дисплея.*/
    lv_display_add_event_cb(display, on_display_delete, LV_EVENT_DELETE, NULL);
}

/**
 * Расширение текущей темы
 */
void lv_example_style_15(void)
{
    lv_obj_t * btn;
    lv_obj_t * label;

    btn = lv_button_create(lv_screen_active());
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 20);

    label = lv_label_create(btn);
    lv_label_set_text(label, "Original theme");

    new_theme_init_and_set();

    btn = lv_button_create(lv_screen_active());
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);

    label = lv_label_create(btn);
    lv_label_set_text(label, "New theme");
}

#endif
