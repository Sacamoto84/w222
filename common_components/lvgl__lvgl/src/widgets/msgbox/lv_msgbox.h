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
 * @param parent        родительский элемент илиNULLдля создания модального окна сообщений
 * @return              созданное окно сообщения
 */
lv_obj_t * lv_msgbox_create(lv_obj_t * parent);

/**
 * Добавьте заголовок в окно сообщения. Он также создает заголовок для заголовка.
 * @param obj           указатель на окно сообщения
 * @param title         текст плитки
 * @return              созданная метка заголовка
 */
lv_obj_t * lv_msgbox_add_title(lv_obj_t * obj, const char * title);

/**
 * Добавьте кнопку в заголовок окна сообщения. Он также создает заголовок.
 * @param obj           указатель на окно сообщения
 * @param icon          значок кнопки
 * @return              созданная кнопка
 */
lv_obj_t * lv_msgbox_add_header_button(lv_obj_t * obj, const void * icon);

/**
 * Добавьте текст в область содержимого окна сообщения. Несколько текстов будут созданы друг под другом.
 * @param obj           указатель на окно сообщения
 * @param text          текст для добавления
 * @return              созданный ярлык
 */
lv_obj_t * lv_msgbox_add_text(lv_obj_t * obj, const char * text);

/**
 * Добавьте форматированный текст в область содержимого окна сообщения. Несколько текстов будут созданы друг под другом.
 * @param obj           указатель на окно сообщения
 * @param fmt           Строка формата, подобная `printf`
 * @return              созданный ярлык
 */
lv_obj_t * lv_msgbox_add_text_fmt(lv_obj_t * obj, const char * fmt, ...) LV_FORMAT_ATTRIBUTE(2, 3);

/**
 * Добавьте кнопку в нижний колонтитул окна сообщения. Он также создает нижний колонтитул.
 * @param obj           указатель на окно сообщения
 * @param text          текст кнопки
 * @return              созданная кнопка
 */
lv_obj_t * lv_msgbox_add_footer_button(lv_obj_t * obj, const char * text);

/**
 * Добавьте кнопку закрытия в окно сообщения. Он также создает заголовок.
 * @param obj           указатель на окно сообщения
 * @return              созданная кнопка закрытия
 */
lv_obj_t * lv_msgbox_add_close_button(lv_obj_t * obj);

/**
 * Получить виджет заголовка
 * @param obj           указатель на окно сообщения
 * @return              заголовок или NULL, если он не существует.
 */
lv_obj_t * lv_msgbox_get_header(lv_obj_t * obj);

/**
 * Получить виджет нижнего колонтитула
 * @param obj           указатель на окно сообщения
 * @return              нижний колонтитул или NULL, если он не существует.
 */
lv_obj_t * lv_msgbox_get_footer(lv_obj_t * obj);

/**
 * Получить виджет контента
 * @param obj           указатель на окно сообщения
 * @return              содержание
 */
lv_obj_t * lv_msgbox_get_content(lv_obj_t * obj);

/**
 * Получить титульную метку
 * @param obj           указатель на окно сообщения
 * @return              заголовок или NULL, если он не существует
 */
lv_obj_t * lv_msgbox_get_title(lv_obj_t * obj);

/**
 * Закрыть окно сообщения
 * @param mbox           указатель на окно сообщения
 */
void lv_msgbox_close(lv_obj_t * mbox);

/**
 * Закрыть окно сообщения при следующем вызове окна сообщения
 * @param mbox           указатель на окно сообщения
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
