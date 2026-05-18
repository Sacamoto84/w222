/**
 * @file lv_menu.h
 *
 */

#ifndef LV_MENU_H
#define LV_MENU_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj.h"
#include "../../core/lv_obj_property.h"

#if LV_USE_MENU

#if LV_USE_FLEX == 0
#error "lv_menu: lv_flex is required. Enable it in lv_conf.h (LV_USE_FLEX 1)"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_MENU_HEADER_TOP_FIXED,   /**< Header is positioned at the top */
    LV_MENU_HEADER_TOP_UNFIXED, /**< Header is positioned at the top and can be scrolled out of view*/
    LV_MENU_HEADER_BOTTOM_FIXED /**< Header is positioned at the bottom */
} lv_menu_mode_header_t;

typedef enum {
    LV_MENU_ROOT_BACK_BUTTON_DISABLED,
    LV_MENU_ROOT_BACK_BUTTON_ENABLED
} lv_menu_mode_root_back_button_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_menu_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_menu_page_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_menu_cont_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_menu_section_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_menu_separator_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_menu_sidebar_cont_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_menu_main_cont_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_menu_sidebar_header_cont_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_menu_main_header_cont_class;

#if LV_USE_OBJ_PROPERTY
enum __lv_property_menu_id_t {
    LV_PROPERTY_ID(MENU, MODE_HEADER,           LV_PROPERTY_TYPE_INT, 0),
    LV_PROPERTY_ID(MENU, MODE_ROOT_BACK_BUTTON, LV_PROPERTY_TYPE_INT, 1),
    LV_PROPERTY_MENU_END,
};
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создать объект меню
 * @param parent    указатель на объект, он будет родительским элементом нового меню
 * @return          указатель на созданное меню
 */
lv_obj_t * lv_menu_create(lv_obj_t * parent);

/**
 * Создайте объект страницы меню.
 *
 * Этот вызов вставляет новую страницу в меню->хранилище в качестве ее родительской страницы, которая сама по себе является
 * дополнительный элемент меню, поэтому результирующая иерархия элементов следующая: меню => хранилище => new_page
 * где`storage`— базовый виджет.
 * @param menu      указатель на объект меню.
 * @param title     указатель на текст заголовка в заголовке (NULL, чтобы заголовок не отображался)
 * @return          указатель на созданную страницу меню
 */
lv_obj_t * lv_menu_page_create(lv_obj_t * menu, char const * const title);

/**
 * Создайте объект продолжения меню
 * @param parent    указатель на страницу меню или объект раздела меню, он будет родительским для нового объекта cont меню
 * @return          указатель на созданное меню продолжение
 */
lv_obj_t * lv_menu_cont_create(lv_obj_t * parent);

/**
 * Создать объект раздела меню
 * @param parent    указатель на объект страницы меню, он будет родительским для нового объекта раздела меню
 * @return          указатель на созданный раздел меню
 */
lv_obj_t * lv_menu_section_create(lv_obj_t * parent);

/**
 * Создайте объект-разделитель меню
 * @param parent    указатель на объект страницы меню, он будет родительским для нового объекта-разделителя меню
 * @return          указатель на созданный разделитель меню
 */
lv_obj_t * lv_menu_separator_create(lv_obj_t * parent);
/*=====================
 * Функции установки
 *====================*/
/**
 * Установить страницу меню для отображения в главном
 * @param obj       указатель на меню
 * @param page      указатель на страницу меню для настройки (NULLдля очистки главного меню и истории меню)
 */
void lv_menu_set_page(lv_obj_t * obj, lv_obj_t * page);

/**
 * Установить заголовок страницы меню
 * @param page      указатель на страницу меню
 * @param title     указатель на текст заголовка в заголовке (NULL, чтобы заголовок не отображался)
 */
void lv_menu_set_page_title(lv_obj_t * page, char const * const title);

/**
 * Установите заголовок страницы меню со статическим текстом. Он не будет сохранен с помощью метки, поэтому переменный текст
 * должен быть «живым», пока существует страница.
 * @param page      указатель на страницу меню
 * @param title     указатель на текст заголовка в заголовке (NULL, чтобы заголовок не отображался)
 */
void lv_menu_set_page_title_static(lv_obj_t * page, char const * const title);

/**
 * Установить страницу меню для отображения на боковой панели
 * @param obj       указатель на меню
 * @param page      указатель на страницу меню для установки (NULLдля очистки боковой панели)
 */
void lv_menu_set_sidebar_page(lv_obj_t * obj, lv_obj_t * page);

/**
 * Установите, как должен вести себя заголовок и его положение.
 * @param obj       указатель на меню
 * @param mode      LV_MENU_HEADER_TOP_FIXED/TOP_UNFIXED/BOTTOM_FIXED
 */
void lv_menu_set_mode_header(lv_obj_t * obj, lv_menu_mode_header_t mode);

/**
 * Установите, должна ли кнопка «Назад» отображаться в корне
 * @param obj       указатель на меню
 * @param mode      LV_MENU_ROOT_BACK_BUTTON_DISABLED/ENABLED
 */
void lv_menu_set_mode_root_back_button(lv_obj_t * obj, lv_menu_mode_root_back_button_t mode);

/**
 * Добавить меню к пункту меню
 * @param menu      указатель на меню
 * @param obj       указатель на объект
 * @param page      указатель на страницу, которая загружается при нажатии obj
 */
void lv_menu_set_load_page_event(lv_obj_t * menu, lv_obj_t * obj, lv_obj_t * page);

/*=====================
 * Геттерные функции
 *====================*/
/**
* Получите указатель на страницу меню, которая в данный момент отображается в главном меню.
* @param obj        указатель на меню
* @return           указатель на текущую страницу
*/
lv_obj_t * lv_menu_get_cur_main_page(lv_obj_t * obj);

/**
* Получите указатель на страницу меню, которая в данный момент отображается на боковой панели.
* @param obj        указатель на меню
* @return           указатель на текущую страницу
*/
lv_obj_t * lv_menu_get_cur_sidebar_page(lv_obj_t * obj);

/**
* Получить указатель на объект основного заголовка
* @param obj        указатель на меню
* @return           указатель на объект основного заголовка
*/
lv_obj_t * lv_menu_get_main_header(lv_obj_t * obj);

/**
* Получить указатель на основной заголовок назад btn obj
* @param obj        указатель на меню
* @return           указатель на главный заголовок назад btn obj
*/
lv_obj_t * lv_menu_get_main_header_back_button(lv_obj_t * obj);

/**
* Получить указатель на объект заголовка боковой панели
* @param obj        указатель на меню
* @return           указатель на объект заголовка боковой панели
*/
lv_obj_t * lv_menu_get_sidebar_header(lv_obj_t * obj);

/**
* Получить указатель на объект заголовка боковой панели
* @param obj        указатель на меню
* @return           указатель на заголовок боковой панели назад btn obj
*/
lv_obj_t * lv_menu_get_sidebar_header_back_button(lv_obj_t * obj);

/**
 * Проверьте, является ли объект корневым
 * @param menu      указатель на меню
 * @param obj       указатель на кнопку «Назад»
 * @return          правда, если это root-бэк, кстати
 */
bool lv_menu_back_button_is_root(lv_obj_t * menu, lv_obj_t * obj);

/**
 * Получить режим заголовка меню
 * @param obj       указатель на меню
 * @return          LV_MENU_HEADER_TOP_FIXED/TOP_UNFIXED/BOTTOM_FIXED
 */
lv_menu_mode_header_t lv_menu_get_mode_header(lv_obj_t * obj);

/**
 * Получить режим кнопки возврата в меню
 * @param obj       указатель на меню
 * @return          LV_MENU_ROOT_BACK_BUTTON_DISABLED/ENABLED
 */
lv_menu_mode_root_back_button_t lv_menu_get_mode_root_back_button(lv_obj_t * obj);

/**
 * Очистить историю меню
 * @param obj       указатель на меню
 */
void lv_menu_clear_history(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_MENU*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_MENU_H*/
