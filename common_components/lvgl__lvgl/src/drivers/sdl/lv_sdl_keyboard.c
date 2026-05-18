/**
 * @file lv_sdl_keyboard.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_sdl_keyboard.h"
#if LV_USE_SDL

#include "../../core/lv_group.h"
#include "../../stdlib/lv_string.h"
#include "../../misc/lv_text_private.h"
#include "lv_sdl_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    char buf[KEYBOARD_BUFFER_SIZE];
    bool dummy_read;
} lv_sdl_keyboard_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void sdl_keyboard_read(lv_indev_t * indev, lv_indev_data_t * data);
static uint32_t keycode_to_ctrl_key(SDL_Keycode sdl_key);
static void release_indev_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_indev_t * lv_sdl_keyboard_create(void)
{
    lv_sdl_keyboard_t * dsc = lv_malloc_zeroed(sizeof(lv_sdl_keyboard_t));
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) return NULL;

    lv_indev_t * indev = lv_indev_create();
    LV_ASSERT_MALLOC(indev);
    if(indev == NULL) {
        lv_free(dsc);
        return NULL;
    }

    lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(indev, sdl_keyboard_read);
    lv_indev_set_driver_data(indev, dsc);
    lv_indev_set_mode(indev, LV_INDEV_MODE_EVENT);
    lv_indev_add_event_cb(indev, release_indev_cb, LV_EVENT_DELETE, indev);

    return indev;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void sdl_keyboard_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_sdl_keyboard_t * dev = lv_indev_get_driver_data(indev);

    const size_t len = lv_strlen(dev->buf);

    /*Отправить релиз вручную*/
    if(dev->dummy_read) {
        dev->dummy_read = false;
        data->state = LV_INDEV_STATE_RELEASED;
    }
    /*Отправить нажатый символ*/
    else if(len > 0) {
        dev->dummy_read = true;
        data->state = LV_INDEV_STATE_PRESSED;
        data->key = 0;
        /*Скопируйте первый символ UTF8 из буфера.*/
        uint32_t utf8_len = lv_text_encoded_size(dev->buf);
        if(utf8_len == 0) utf8_len = 1; /*Убедитесь, что прочитан хотя бы 1 символ*/
        lv_memcpy(&data->key, dev->buf, utf8_len);

        /*Удалить первый символ*/
        lv_memmove(dev->buf, dev->buf + utf8_len, len - utf8_len + 1);
    }
}

static void release_indev_cb(lv_event_t * e)
{
    lv_indev_t * indev = (lv_indev_t *) lv_event_get_user_data(e);
    lv_sdl_keyboard_t * dev = lv_indev_get_driver_data(indev);
    if(dev) {
        lv_indev_set_driver_data(indev, NULL);
        lv_indev_set_read_cb(indev, NULL);
        lv_free(dev);
        LV_LOG_INFO("done");
    }
}

void lv_sdl_keyboard_handler(SDL_Event * event)
{
    uint32_t win_id = UINT32_MAX;
    switch(event->type) {
        case SDL_KEYDOWN:
            win_id = event->key.windowID;
            break;
        case SDL_TEXTINPUT:
            win_id = event->text.windowID;
            break;
        default:
            return;
    }

    lv_display_t * disp = lv_sdl_get_disp_from_win_id(win_id);


    /*Найдите подходящего разработчика*/
    lv_indev_t * indev = lv_indev_get_next(NULL);
    while(indev) {
        if(lv_indev_get_read_cb(indev) == sdl_keyboard_read) {
            /*Если по какой-либо причине disp равен NULL, используйте первый indev правильного типа.*/
            if(disp == NULL || lv_indev_get_display(indev) == disp) break;
        }
        indev = lv_indev_get_next(indev);
    }
    if(indev == NULL) return;
    lv_sdl_keyboard_t * dsc = lv_indev_get_driver_data(indev);

    /* Нас интересуют только события SDL_KEYDOWN и SDL_TEXTINPUT. */
    switch(event->type) {
        case SDL_KEYDOWN: {                     /*Нажатие кнопки*/
                const uint32_t ctrl_key = keycode_to_ctrl_key(event->key.keysym.sym);
                if(ctrl_key == '\0')
                    return;
                const size_t len = lv_strlen(dsc->buf);
                if(len < KEYBOARD_BUFFER_SIZE - 1) {
                    dsc->buf[len] = ctrl_key;
                    dsc->buf[len + 1] = '\0';
                }
                break;
            }
        case SDL_TEXTINPUT: {                   /*Ввод текста*/
                const size_t len = lv_strlen(dsc->buf) + lv_strlen(event->text.text);
                if(len < KEYBOARD_BUFFER_SIZE - 1)
                    lv_strcat(dsc->buf, event->text.text);
            }
            break;
        default:
            break;

    }

    size_t len = lv_strlen(dsc->buf);
    while(len) {
        lv_indev_read(indev);

        /*Вызовите еще раз, чтобы обработать фиктивное чтение в `sdl_keyboard_read`.*/
        lv_indev_read(indev);
        len--;
    }
}

/**
 * Преобразуйте код клавиши SDL в его аналог LV_KEY_ * или верните '\0', если это не управляющий символ.
 * @param sdl_key the key code
 * @return LV_KEY_* control character or '\0'
 */
static uint32_t keycode_to_ctrl_key(SDL_Keycode sdl_key)
{
    /*Переназначить какой-то ключ на LV_KEY_... для управления группами*/
    switch(sdl_key) {
        case SDLK_RIGHT:
        case SDLK_KP_PLUS:
            return LV_KEY_RIGHT;

        case SDLK_LEFT:
        case SDLK_KP_MINUS:
            return LV_KEY_LEFT;

        case SDLK_UP:
            return LV_KEY_UP;

        case SDLK_DOWN:
            return LV_KEY_DOWN;

        case SDLK_ESCAPE:
            return LV_KEY_ESC;

        case SDLK_BACKSPACE:
            return LV_KEY_BACKSPACE;

        case SDLK_DELETE:
            return LV_KEY_DEL;

        case SDLK_KP_ENTER:
        case '\r':
            return LV_KEY_ENTER;

        case SDLK_TAB:
        case SDLK_PAGEDOWN:
            return LV_KEY_NEXT;

        case SDLK_PAGEUP:
            return LV_KEY_PREV;

        case SDLK_HOME:
            return LV_KEY_HOME;

        case SDLK_END:
            return LV_KEY_END;

        default:
            return '\0';
    }
}

#endif /*LV_USE_SDL*/
