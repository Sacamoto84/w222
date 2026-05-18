/**
 * @file lv_tabview.h
 *
 */

#ifndef LV_TABVIEW_H
#define LV_TABVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#include "../../core/lv_obj.h"
#include "../../core/lv_obj_property.h"

#if LV_USE_TABVIEW

/*********************
 *      DEFINES
 *********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_tabview_class;

#if LV_USE_OBJ_PROPERTY
enum _lv_property_tabview_id_t {
    LV_PROPERTY_ID(TABVIEW, TAB_ACTIVE,       LV_PROPERTY_TYPE_INT, 0),
    LV_PROPERTY_ID(TABVIEW, TAB_BAR_POSITION, LV_PROPERTY_TYPE_INT, 1),
    LV_PROPERTY_TABVIEW_END,
};
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте виджет просмотра вкладок
 * @param parent    указатель на родительский виджет
 * @return          созданная вкладка
 */
lv_obj_t * lv_tabview_create(lv_obj_t * parent);

/**
 * Добавить вкладку в представление вкладок
 * @param obj       указатель на виджет просмотра вкладок
 * @param name      название вкладки, оно будет отображаться на панели вкладок
 * @return          виджет, в котором можно создать содержимое вкладки
 */
lv_obj_t * lv_tabview_add_tab(lv_obj_t * obj, const char * name);

/**
 * Изменить название вкладки
 * @param obj       указатель на виджет просмотра вкладок
 * @param idx       индекс вкладки, которую нужно переименовать
 * @param new_name  новое имя в виде строки
 */
void lv_tabview_set_tab_text(lv_obj_t * obj, uint32_t idx, const char * new_name);

#if LV_USE_TRANSLATION

/**
 * Добавьте вкладку с тегом перевода в представление вкладок.
 * @param obj       указатель на виджет просмотра вкладок
 * @param tag       ключ перевода, используемый для метки вкладки; будет отображаться на панели вкладок
 * @return          виджет, в котором можно создать содержимое вкладки
 */
lv_obj_t * lv_tabview_set_tab_translation_tag(lv_obj_t * obj, const char * tag);

#endif

/**
 * Показать вкладку
 * @param obj       указатель на виджет просмотра вкладок
 * @param idx       индекс вкладки, которую нужно показать
 * @param anim_en   LV_ANIM_ON/OFF
 */
void lv_tabview_set_active(lv_obj_t * obj, uint32_t idx, lv_anim_enable_t anim_en);

/**
 * Установите положение панели вкладок
 * @param obj       указатель на виджет просмотра вкладок
 * @param dir       LV_DIR_TOP/BOTTOM/ЛЕВО/ПРАВО
 */
void lv_tabview_set_tab_bar_position(lv_obj_t * obj, lv_dir_t dir);

/**
 * Установите ширину или высоту панели вкладок
 * @param obj       указатель на виджет просмотра вкладок
 * @param size      размер панели вкладок в пикселях или процентах.
 *                  будет использоваться как ширина или высота в зависимости от положения панели вкладок)
 */
void lv_tabview_set_tab_bar_size(lv_obj_t * obj, int32_t size);

/**
 * Получить количество вкладок
 * @param obj       указатель на виджет просмотра вкладок
 * @return          количество вкладок
 */
uint32_t lv_tabview_get_tab_count(lv_obj_t * obj);

/**
 * Получить индекс текущей вкладки
 * @param obj       указатель на виджет просмотра вкладок
 * @return          нулевой индекс текущей вкладки
 */
uint32_t lv_tabview_get_tab_active(lv_obj_t * obj);

/**
 * Получить данную кнопку вкладки по индексу
 * @param obj       указатель на виджет просмотра вкладок
 * @param idx       индекс кнопки вкладки, начинающийся с нуля, который нужно получить.
 *                  <0 означает начало отсчета кнопки вкладки сзади (-1 — последняя кнопка вкладки)
 * @return          указатель на кнопку табуляции или NULL, если индекс находился за пределами диапазона
 */
lv_obj_t * lv_tabview_get_tab_button(lv_obj_t * obj, int32_t idx);

/**
 * Получите виджет, в котором создается контейнер каждой вкладки.
 * @param obj       указатель на виджет просмотра вкладок
 * @return          основной виджет контейнера
 */
lv_obj_t * lv_tabview_get_content(lv_obj_t * obj);

/**
 * Получить панель вкладок, где созданы кнопки
 * @param obj       указатель на виджет просмотра вкладок
 * @return          панель вкладок
 */
lv_obj_t * lv_tabview_get_tab_bar(lv_obj_t * obj);

/**
 * Получить положение панели вкладок
 * @param obj       указатель на виджет просмотра вкладок
 * @return          LV_DIR_TOP/BOTTOM/ЛЕВО/ПРАВО
 */
lv_dir_t lv_tabview_get_tab_bar_position(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TABVIEW*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_TABVIEW_H*/
