/**
 * @file lv_obj_style_private.h
 *
 */

#ifndef LV_OBJ_STYLE_PRIVATE_H
#define LV_OBJ_STYLE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_obj_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_obj_style_t {
    const lv_style_t * style;
    uint32_t selector : 24;
    uint32_t is_local : 1;
    uint32_t is_trans : 1;
    uint32_t is_disabled : 1;
    uint32_t is_theme : 1;  /**< The style is added by a theme */
};

struct _lv_obj_style_transition_dsc_t {
    uint16_t time;
    uint16_t delay;
    lv_style_selector_t selector;
    lv_style_prop_t prop;
    lv_anim_path_cb_t path_cb;
    void * user_data;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте модуль менеджера стилей, связанных с объектами.
 * ВызываетсяLVGLв `lv_init()`
 */
void lv_obj_style_init(void);

/**
 * Деинициализируйте модуль менеджера стилей, связанных с объектами.
 * ВызываетсяLVGLв `lv_deinit()`
 */
void lv_obj_style_deinit(void);

/**
 * Используется внутри для создания перехода стиля.
 * @param объект
 * @param часть
 * @param prev_state
 * @param new_state
 * @param тр
 */
void lv_obj_style_create_transition(lv_obj_t * obj, lv_part_t part, lv_state_t prev_state,
                                    lv_state_t new_state, const lv_obj_style_transition_dsc_t * tr);

/**
 * Используется внутри компании для сравнения внешнего вида объекта в двух состояниях.
 * @param объект
 * @param состояние1
 * @param состояние2
 * @return
 */
lv_style_state_cmp_t lv_obj_style_state_compare(lv_obj_t * obj, lv_state_t state1, lv_state_t state2);

/**
 * Обновите тип слоя виджета, привязанного к его текущим стилям.
 * Результат будет сохранен в `obj->spec_attr->layer_type`.
 * @param obj       объект, слой которого следует обновить
 */
void lv_obj_update_layer_type(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_OBJ_STYLE_PRIVATE_H*/
