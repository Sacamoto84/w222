/**
 * @file lv_obj_class.h
 *
 */

#ifndef LV_OBJ_CLASS_H
#define LV_OBJ_CLASS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_types.h"
#include "../misc/lv_area.h"
#include "lv_obj_property.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_OBJ_CLASS_EDITABLE_INHERIT,      /**< Проверять базовый класс; 0 нужен, чтобы нулевая инициализация означала наследование*/
    LV_OBJ_CLASS_EDITABLE_TRUE,
    LV_OBJ_CLASS_EDITABLE_FALSE,
} lv_obj_class_editable_t;

typedef enum {
    LV_OBJ_CLASS_GROUP_DEF_INHERIT,      /**< Проверять базовый класс; 0 нужен, чтобы нулевая инициализация означала наследование*/
    LV_OBJ_CLASS_GROUP_DEF_TRUE,
    LV_OBJ_CLASS_GROUP_DEF_FALSE,
} lv_obj_class_group_def_t;

typedef enum {
    LV_OBJ_CLASS_THEME_INHERITABLE_FALSE,    /**< Не наследовать тему из базового класса */
    LV_OBJ_CLASS_THEME_INHERITABLE_TRUE,
} lv_obj_class_theme_inheritable_t;

typedef void (*lv_obj_class_event_cb_t)(lv_obj_class_t * class_p, lv_event_t * e);
/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте объект из дескриптора класса.
 * @param class_p   указатель на класс
 * @param parent    указатель на объект, где должен быть создан новый объект
 * @return          указатель на созданный объект
 */
lv_obj_t * lv_obj_class_create_obj(const lv_obj_class_t * class_p, lv_obj_t * parent);

void lv_obj_class_init_obj(lv_obj_t * obj);

bool lv_obj_is_editable(lv_obj_t * obj);

bool lv_obj_is_group_def(lv_obj_t * obj);

#if LV_USE_EXT_DATA
/**
 * @brief Связывает массив указателей на внешние данные с объектом LVGL.
 *
 * Связывает пользовательские данные с объектом LVGL и определяет функцию деструктора.
 * который будет автоматически вызываться при удалении объекта для правильной очистки.
 * связанные ресурсы.
 *
 * @param obj          Целевой объект LVGL
 * @param data         Пользовательский указатель данных для связи с объектом
 * @param free_cb      Функция очистки вызывается для каждого указателя данных, отличного от NULL, во время
 *                     удаление объекта. Получает один указатель данных в качестве параметра.
 *                     NULL означает отсутствие автоматической очистки.
 */
void lv_obj_set_external_data(lv_obj_t * obj, void * data, void (* free_cb)(void * data));
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_CLASS_H*/
