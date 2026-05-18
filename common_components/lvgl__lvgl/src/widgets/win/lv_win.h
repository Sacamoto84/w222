/**
 * @file lv_win.h
 *
 */

#ifndef LV_WIN_H
#define LV_WIN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#include "../../core/lv_obj.h"
#if LV_USE_WIN
/*********************
 *      DEFINES
 *********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_win_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создать виджет окна
 * @param parent    указатель на родительский виджет
 * @return          созданное окно
 */
lv_obj_t * lv_win_create(lv_obj_t * parent);

/**
 * Добавьте заголовок в окно
 * @param win       указатель на виджет окна
 * @param txt       текст заголовка
 * @return          виджет, в котором можно создать содержимое заголовка
 */
lv_obj_t * lv_win_add_title(lv_obj_t * win, const char * txt);

/**
 * Добавляем кнопку в окно
 * @param win       указатель на виджет окна
 * @param icon      значок, который будет отображаться на кнопке
 * @param btn_w     ширина кнопки
 * @return          виджет, в котором можно создать содержимое кнопки
 */
lv_obj_t * lv_win_add_button(lv_obj_t * win, const void * icon, int32_t btn_w);

/**
 * Получить заголовок окна
 * @param win       указатель на виджет окна
 * @return          заголовок окна
 */
lv_obj_t * lv_win_get_header(lv_obj_t * win);

/**
 * Получить содержимое окна
 * @param win       указатель на виджет окна
 * @return          содержимое окна
 */
lv_obj_t * lv_win_get_content(lv_obj_t * win);
/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_WIN*/
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_WIN_H*/
