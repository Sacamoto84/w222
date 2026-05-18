/**
 * @file lv_x11.h
 *
 */

#ifndef LV_X11_H
#define LV_X11_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"
#include "../../indev/lv_indev.h"
#include "../../draw/lv_image_dsc.h"

#if LV_USE_X11

/*********************
 *      DEFINES
 *********************/

/** Заголовок данных пользователя частного драйвера дисплея — только для внутреннего использования. */
typedef struct {
    struct _XDisplay   *  display;  /**< X11 display object     */
    struct _x11_inp_data * inp_data; /**< input user data object */
} _x11_user_hdr_t;

/** дополнительный тип функции обратного вызова при закрытии окна
 *  @see lv_x11_window_set_close_cb
*/
typedef void(*lv_x11_close_cb)(void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * создавать и добавлять объекты клавиатуры, мыши и колеса прокрутки и подключать их к дисплею x11.
 *
 * Это удобный метод, обрабатывающий типичную инициализацию ввода окна X11:
 * - create keyboard (lv_x11_keyboard_create)
 * - create mouse (with scrollwheel, lv_x11_mouse_create lv_x11_mousewheel_create)
 *
 * @param [in] отображает созданный экранный объект X11 из @ref lv_x11_window_create.
 * @param [in] mouse_img дополнительное описание изображения для курсора мыши (NULL для отсутствия/невидимого курсора мыши)
 */
void lv_x11_inputs_create(lv_display_t * disp, lv_image_dsc_t const * mouse_img);

/**
 * создать отображение X11
 *
 * Минимальная инициализация драйвера дисплея X11 с поддержкой клавиатуры/мыши:
 * @code
 * lv_display_t * disp = lv_x11_window_create («Заголовок моего окна», window_width , window_width );
 * lv_x11_inputs_create (дисп, NULL );
 * @endcode
 * или с помощью значка курсора мыши:
 * @code
 * lv_image_dsc_t mouse_symbol = {.....};
 * lv_display_t * disp = lv_x11_window_create («Заголовок моего окна», window_width , window_width );
 * lv_x11_inputs_create (дисп, & mouse_symbol );
 * @endcode
 *
 * @param [in] title заголовок созданного окна X11
 * @param [дюймы] hor_res горизонтальное разрешение (=ширина) окна X11
 * @param [дюймы] ver_res вертикальное разрешение (=высота) окна X11
 * @return             pointer to the display object
 */
lv_display_t * lv_x11_window_create(char const * title, int32_t hor_res, int32_t ver_res);

#endif /* LV_USE_X11 */

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /* LV_X11_H */
