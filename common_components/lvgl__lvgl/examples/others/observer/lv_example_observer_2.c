#include "../../lv_examples.h"
#if LV_USE_OBSERVER && LV_USE_SLIDER && LV_USE_LABEL && LV_USE_KEYBOARD && LV_BUILD_EXAMPLES

/*Это единственный интерфейс между UI и приложением.*/
static lv_subject_t engine_subject;

static void app_init(void);
static void ui_init(void);

/**
 * Простой экран входа в систему PIN для запуска двигателя.
 * Единственный интерфейс между UI и приложением — это один «субъект».
 */
void lv_example_observer_2(void)
{
    lv_subject_init_int(&engine_subject, 0);
    app_init();
    ui_init();
}

/*--------------------------------------------------
 * APPLICATION
 *
 * Эта часть содержит логику демонстрационного приложения.
 * Он ничего не знает о внутреннем устройстве UI.
 * в качестве интерфейса использую любой `engine_subject`.
 * -------------------------------------------------*/
static void engine_state_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);

    int32_t v = lv_subject_get_int(subject);
    LV_UNUSED(v);
    /*В реальном приложении установите/очистите пин-код здесь*/
    LV_LOG_USER("Engine state: %" LV_PRId32, v);
}

static void app_init(void)
{
    lv_subject_add_observer(&engine_subject, engine_state_observer_cb, NULL);
}

/*--------------------------------------------------
 * USER INTERFACE
 *
 * Эта часть содержит только код и данные, связанные с UI.
 * В проекте это будет отдельный файл и
 * приложение не могло видеть свои внутренности
 * -------------------------------------------------*/

typedef enum {
    LOGGED_OUT,
    LOGGED_IN,
    AUTH_FAILED,
} auth_state_t;

static lv_subject_t auth_state_subject;

static void textarea_event_cb(lv_event_t * e)
{
    lv_obj_t * ta = lv_event_get_target_obj(e);
    if(lv_strcmp(lv_textarea_get_text(ta), "hello") == 0) {
        lv_subject_set_int(&auth_state_subject, LOGGED_IN);
    }
    else {
        lv_subject_set_int(&auth_state_subject, AUTH_FAILED);
    }
}

static void info_label_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * label = (lv_obj_t *) lv_observer_get_target(observer);
    switch(lv_subject_get_int(subject)) {
        case LOGGED_IN:
            lv_label_set_text(label, "Login successful");
            break;
        case LOGGED_OUT:
            lv_label_set_text(label, "Logged out");
            break;
        case AUTH_FAILED:
            lv_label_set_text(label, "Login failed");
            break;
    }
}

static void log_out_click_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_subject_set_int(&auth_state_subject, LOGGED_OUT);
}

static void ui_init(void)
{
    lv_subject_init_int(&auth_state_subject, LOGGED_OUT);

    /*Создайте слайдер в центре дисплея.*/
    lv_obj_t * ta = lv_textarea_create(lv_screen_active());
    lv_obj_set_pos(ta, 10, 10);
    lv_obj_set_width(ta, 200);
    lv_textarea_set_one_line(ta, true);
    lv_textarea_set_password_mode(ta, true);
    lv_textarea_set_placeholder_text(ta, "The password is: hello");
    lv_obj_add_event_cb(ta, textarea_event_cb, LV_EVENT_READY, NULL);
    lv_obj_bind_state_if_eq(ta, &auth_state_subject, LV_STATE_DISABLED, LOGGED_IN);

    lv_obj_t * kb = lv_keyboard_create(lv_screen_active());
    lv_keyboard_set_textarea(kb, ta);

    lv_obj_t * btn;
    lv_obj_t * label;

    /*Создайте кнопку выхода из системы, которая будет активна только при входе в систему.*/
    btn = lv_button_create(lv_screen_active());
    lv_obj_set_pos(btn, 220, 10);
    lv_obj_add_event_cb(btn, log_out_click_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_bind_state_if_not_eq(btn, &auth_state_subject, LV_STATE_DISABLED, LOGGED_IN);

    label = lv_label_create(btn);
    lv_label_set_text(label, "LOG OUT");

    /*Создайте ярлык для отображения информации*/
    label = lv_label_create(lv_screen_active());
    lv_obj_set_pos(label, 10, 60);
    lv_subject_add_observer_obj(&auth_state_subject, info_label_observer_cb, label, NULL);

    /*Создать кнопку, которая будет активна только при входе в систему*/
    btn = lv_button_create(lv_screen_active());
    lv_obj_set_pos(btn, 10, 80);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_bind_state_if_not_eq(btn, &auth_state_subject, LV_STATE_DISABLED, LOGGED_IN);
    lv_obj_bind_checked(btn, &engine_subject);
    label = lv_label_create(btn);
    lv_label_set_text(label, "START ENGINE");
}

#endif
