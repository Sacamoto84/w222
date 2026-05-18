/**
 * @file lv_port_indev_template.c
 *
 */

/*Скопируйте этот файл как «lv_port_indev.c» и установите для этого значения значение «1», чтобы отключить контент.*/
#if 0

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev_template.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void touchpad_init(void);
static void touchpad_read(lv_indev_t * indev, lv_indev_data_t * data);
static bool touchpad_is_pressed(void);
static void touchpad_get_xy(int32_t * x, int32_t * y);

static void mouse_init(void);
static void mouse_read(lv_indev_t * indev, lv_indev_data_t * data);
static bool mouse_is_pressed(void);
static void mouse_get_xy(int32_t * x, int32_t * y);

static void keypad_init(void);
static void keypad_read(lv_indev_t * indev, lv_indev_data_t * data);
static uint32_t keypad_get_key(void);

static void encoder_init(void);
static void encoder_read(lv_indev_t * indev, lv_indev_data_t * data);
static void encoder_handler(void);

static void button_init(void);
static void button_read(lv_indev_t * indev, lv_indev_data_t * data);
static int8_t button_get_pressed_id(void);
static bool button_is_pressed(uint8_t id);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t * indev_touchpad;
lv_indev_t * indev_mouse;
lv_indev_t * indev_keypad;
lv_indev_t * indev_encoder;
lv_indev_t * indev_button;

static int32_t encoder_diff;
static lv_indev_state_t encoder_state;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
    /**
     * Здесь вы найдете пример реализации устройства ввода, поддерживающего LittelvGL:
     *  - Тачпад
     *  - Мышь (с поддержкой курсора)
     *  - Клавиатура (поддерживает использованиеGUIтолько с ключом)
     *  - Энкодер (поддерживает использованиеGUIтолько с: влево, вправо, нажатие)
     *  - Кнопка (внешние кнопки для нажатия точек на экране)
     *
     *  Функция`..._read()`реализована лишь.
     *  Вы должны сформировать их в соответствии с вашим оборудованием.
     */

    /*------------------
     * Тачпад
     * -----------------*/

    /*Инициализируйте сенсорную панель, если у вас есть*/
    touchpad_init();

    /*Регистрация устройства ввода с сенсорной панелью*/
    indev_touchpad = lv_indev_create();
    lv_indev_set_type(indev_touchpad, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev_touchpad, touchpad_read);

    /*------------------
     * Мышь
     * -----------------*/

    /*Инициализируйте мышь, если у вас есть*/
    mouse_init();

    /*Зарегистрируйте устройство ввода мыши*/
    indev_mouse = lv_indev_create();
    lv_indev_set_type(indev_mouse, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev_mouse, mouse_read);

    /*Установить курсор. Для простоты установите сейчас символ HOME.*/
    lv_obj_t * mouse_cursor = lv_image_create(lv_screen_active());
    lv_image_set_src(mouse_cursor, LV_SYMBOL_HOME);
    lv_indev_set_cursor(indev_mouse, mouse_cursor);

    /*------------------
     * Клавиатура
     * -----------------*/

    /*Инициализируйте клавиатуру или клавиатуру, если у вас есть*/
    keypad_init();

    /*Регистрация устройства ввода с клавиатуры*/
    indev_keypad = lv_indev_create();
    lv_indev_set_type(indev_keypad, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(indev_keypad, keypad_read);

    /*Позже вам следует создать группу(ы) с помощью`lv_group_t * group = lv_group_create()`,
     *добавить элементы в группу с помощью `lv_group_add_obj(group, obj)`
     *и назначьте это устройство ввода в группу для навигации по нему:
     *`lv_indev_set_group(indev_keypad, group);`*/

    /*------------------
     * Кодер
     * -----------------*/

    /*Инициализируйте свой кодер, если у вас есть*/
    encoder_init();

    /*Зарегистрируйте устройство ввода кодера*/
    indev_encoder = lv_indev_create();
    lv_indev_set_type(indev_encoder, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(indev_encoder, encoder_read);

    /*Позже вам следует создать группу(ы) с помощью`lv_group_t * group = lv_group_create()`,
     *добавить элементы в группу с помощью `lv_group_add_obj(group, obj)`
     *и назначьте это устройство ввода в группу для навигации по нему:
     *`lv_indev_set_group(indev_encoder, group);`*/

    /*------------------
     * Кнопка
     * -----------------*/

    /*Инициализируйте кнопку, если у вас есть*/
    button_init();

    /*Регистрация устройства кнопочного ввода*/
    indev_button = lv_indev_create();
    lv_indev_set_type(indev_button, LV_INDEV_TYPE_BUTTON);
    lv_indev_set_read_cb(indev_button, button_read);

    /*Назначение кнопок точкам на экране*/
    static const lv_point_t btn_points[2] = {
        {10, 10},   /*Кнопка 0 -> х:10; у:10*/
        {40, 100},  /*Кнопка 1 -> х:40; г:100*/
    };
    lv_indev_set_button_points(indev_button, btn_points);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*------------------
 * Тачпад
 * -----------------*/

/*Инициализируйте сенсорную панель*/
static void touchpad_init(void)
{
    /*Ваш код находится здесь*/
}

/*Будет вызван библиотекой для чтения тачпада*/
static void touchpad_read(lv_indev_t * indev_drv, lv_indev_data_t * data)
{
    static int32_t last_x = 0;
    static int32_t last_y = 0;

    /*Сохраните нажатые координаты и состояние*/
    if(touchpad_is_pressed()) {
        touchpad_get_xy(&last_x, &last_y);
        data->state = LV_INDEV_STATE_PRESSED;
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
    }

    /*Установить последние нажатые координаты*/
    data->point.x = last_x;
    data->point.y = last_y;
}

/*Возвращает true, если нажата сенсорная панель.*/
static bool touchpad_is_pressed(void)
{
    /*Ваш код находится здесь*/

    return false;
}

/*Получите координаты x и y, если нажата сенсорная панель.*/
static void touchpad_get_xy(int32_t * x, int32_t * y)
{
    /*Ваш код находится здесь*/

    (*x) = 0;
    (*y) = 0;
}

/*------------------
 * Мышь
 * -----------------*/

/*Инициализируйте мышь*/
static void mouse_init(void)
{
    /*Ваш код находится здесь*/
}

/*Будет вызван библиотекой для чтения мыши*/
static void mouse_read(lv_indev_t * indev_drv, lv_indev_data_t * data)
{
    /*Получить текущие координаты x и y*/
    mouse_get_xy(&data->point.x, &data->point.y);

    /*Узнайте, нажата или отпущена кнопка мыши*/
    if(mouse_is_pressed()) {
        data->state = LV_INDEV_STATE_PRESSED;
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

/*Возвращает true, если нажата кнопка мыши.*/
static bool mouse_is_pressed(void)
{
    /*Ваш код находится здесь*/

    return false;
}

/*Получите координаты x и y, если нажата мышь.*/
static void mouse_get_xy(int32_t * x, int32_t * y)
{
    /*Ваш код находится здесь*/

    (*x) = 0;
    (*y) = 0;
}

/*------------------
 * Клавиатура
 * -----------------*/

/*Инициализируйте клавиатуру*/
static void keypad_init(void)
{
    /*Ваш код находится здесь*/
}

/*Будет вызван библиотекой для чтения мыши*/
static void keypad_read(lv_indev_t * indev_drv, lv_indev_data_t * data)
{
    static uint32_t last_key = 0;

    /*Получить текущие координаты x и y*/
    mouse_get_xy(&data->point.x, &data->point.y);

    /*Узнайте, нажата ли клавиша, и сохраните нажатую клавишу.*/
    uint32_t act_key = keypad_get_key();
    if(act_key != 0) {
        data->state = LV_INDEV_STATE_PRESSED;

        /*Переведите клавиши в управляющие символы LVGL в соответствии с вашими определениями клавиш.*/
        switch(act_key) {
            case 1:
                act_key = LV_KEY_NEXT;
                break;
            case 2:
                act_key = LV_KEY_PREV;
                break;
            case 3:
                act_key = LV_KEY_LEFT;
                break;
            case 4:
                act_key = LV_KEY_RIGHT;
                break;
            case 5:
                act_key = LV_KEY_ENTER;
                break;
        }

        last_key = act_key;
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
    }

    data->key = last_key;
}

/*Получить нажимаемую в данный момент клавишу.  0, если ни одна клавиша не нажата*/
static uint32_t keypad_get_key(void)
{
    /*Ваш код находится здесь*/

    return 0;
}

/*------------------
 * Кодер
 * -----------------*/

/*Инициализируйте свой кодировщик*/
static void encoder_init(void)
{
    /*Ваш код находится здесь*/
}

/*Будет вызван библиотекой для чтения кодировщика*/
static void encoder_read(lv_indev_t * indev_drv, lv_indev_data_t * data)
{

    data->enc_diff = encoder_diff;
    data->state = encoder_state;
}

/*Вызов этой функции в прерывании для обработки событий энкодера (поворот, нажатие)*/
static void encoder_handler(void)
{
    /*Ваш код находится здесь*/

    encoder_diff += 0;
    encoder_state = LV_INDEV_STATE_RELEASED;
}

/*------------------
 * Кнопка
 * -----------------*/

/*Инициализируйте свои кнопки*/
static void button_init(void)
{
    /*Ваш код находится здесь*/
}

/*Будет вызван библиотекой для чтения кнопки*/
static void button_read(lv_indev_t * indev_drv, lv_indev_data_t * data)
{

    static uint8_t last_btn = 0;

    /*Получить ID нажатой кнопки*/
    int8_t btn_act = button_get_pressed_id();

    if(btn_act >= 0) {
        data->state = LV_INDEV_STATE_PRESSED;
        last_btn = btn_act;
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
    }

    /*Сохраните ID последней нажатой кнопки.*/
    data->btn_id = last_btn;
}

/*Получить ID (0, 1, 2..) нажатой кнопки*/
static int8_t button_get_pressed_id(void)
{
    uint8_t i;

    /*Проверьте кнопки, чтобы увидеть, какая из них нажимается (предположим, есть 2 кнопки)*/
    for(i = 0; i < 2; i++) {
        /*Вернуть ID нажатой кнопки*/
        if(button_is_pressed(i)) {
            return i;
        }
    }

    /*Ни одна кнопка не нажата*/
    return -1;
}

/*Проверьте, нажата ли кнопка `id` или нет.*/
static bool button_is_pressed(uint8_t id)
{

    /*Ваш код находится здесь*/

    return false;
}

#else /*Включите этот файл вверху*/

/*Этот вымышленный typedef существует исключительно для того, чтобы успеть замолчать -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
