/**
 * @file lv_msgbox.h
 *
 */

#ifndef LV_MSGBOX_H
#define LV_MSGBOX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj.h"

#if LV_USE_MSGBOX

/*********************
 *      DEFINES
 *********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_msgbox_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_msgbox_header_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_msgbox_content_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_msgbox_footer_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_msgbox_header_button_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_msgbox_footer_button_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_msgbox_backdrop_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте пустое окно сообщения
 * @param parent        the parent or NULL to create a modal msgbox
 * @return              the created message box
 */
lv_obj_t * lv_msgbox_create(lv_obj_t * parent);

/**
 * Добавьте заголовок в окно сообщения. Он также создает заголовок для заголовка.
 * @param obj           pointer to a message box
 * @param title         the text of the tile
 * @return              the created title label
 */
lv_obj_t * lv_msgbox_add_title(lv_obj_t * obj, const char * title);

/**
 * Добавьте кнопку в заголовок окна сообщения. Он также создает заголовок.
 * @param obj           pointer to a message box
 * @param icon          the icon of the button
 * @return              the created button
 */
lv_obj_t * lv_msgbox_add_header_button(lv_obj_t * obj, const void * icon);

/**
 * Добавьте текст в область содержимого окна сообщения. Несколько текстов будут созданы друг под другом.
 * @param obj           pointer to a message box
 * @param text          text to add
 * @return              the created label
 */
lv_obj_t * lv_msgbox_add_text(lv_obj_t * obj, const char * text);

/**
 * Добавьте форматированный текст в область содержимого окна сообщения. Несколько текстов будут созданы друг под другом.
 * @param obj           pointer to a message box
 * @param fmt           `printf`-like format string
 * @return              the created label
 */
lv_obj_t * lv_msgbox_add_text_fmt(lv_obj_t * obj, const char * fmt, ...) LV_FORMAT_ATTRIBUTE(2, 3);

/**
 * Добавьте кнопку в нижний колонтитул окна сообщения. Он также создает нижний колонтитул.
 * @param obj           pointer to a message box
 * @param text          the text of the button
 * @return              the created button
 */
lv_obj_t * lv_msgbox_add_footer_button(lv_obj_t * obj, const char * text);

/**
 * Добавьте кнопку закрытия в окно сообщения. Он также создает заголовок.
 * @param obj           pointer to a message box
 * @return              the created close button
 */
lv_obj_t * lv_msgbox_add_close_button(lv_obj_t * obj);

/**
 * Получить виджет заголовка
 * @param obj           pointer to a message box
 * @return              the header, or NULL if not exists
 */
lv_obj_t * lv_msgbox_get_header(lv_obj_t * obj);

/**
 * Получить виджет нижнего колонтитула
 * @param obj           pointer to a message box
 * @return              the footer, or NULL if not exists
 */
lv_obj_t * lv_msgbox_get_footer(lv_obj_t * obj);

/**
 * Получить виджет контента
 * @param obj           pointer to a message box
 * @return              the content
 */
lv_obj_t * lv_msgbox_get_content(lv_obj_t * obj);

/**
 * Получить титульную метку
 * @param obj           pointer to a message box
 * @return              the title, or NULL if it does not exist
 */
lv_obj_t * lv_msgbox_get_title(lv_obj_t * obj);

/**
 * Закрыть окно сообщения
 * @param mbox           pointer to a message box
 */
void lv_msgbox_close(lv_obj_t * mbox);

/**
 * Закрыть окно сообщения при следующем вызове окна сообщения
 * @param mbox           pointer to a message box
 */
void lv_msgbox_close_async(lv_obj_t * mbox);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_MSGBOX*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_MSGBOX_H*/
