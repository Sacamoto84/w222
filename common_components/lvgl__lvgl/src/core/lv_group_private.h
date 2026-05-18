/**
 * @file lv_group_private.h
 *
 */

#ifndef LV_GROUP_PRIVATE_H
#define LV_GROUP_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_ext_data.h"
#include "lv_group.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Группы можно использовать для логического хранения объектов, чтобы их можно было сфокусировать индивидуально.
 * Это NOT для размещения объектов на экране (попробуйте для этого макеты).
 */
struct _lv_group_t {
#if LV_USE_EXT_DATA
    lv_ext_data_t ext_data;
#endif
    lv_ll_t obj_ll;        /**< Linked list to store the objects in the group*/
    lv_obj_t ** obj_focus; /**< The object in focus*/

    lv_group_focus_cb_t focus_cb;              /**< A function to call when a new object is focused (optional)*/
    lv_group_edge_cb_t  edge_cb;               /**< Функция, вызываемая при достижении края, фокуса больше нет
                                                    цели доступны в этом направлении (чтобы обеспечить обратную связь по фронту
                                                    например звук или отскок прокрутки) */

    void * user_data;

    uint8_t frozen : 1;         /**< 1: can't focus to new object*/
    uint8_t editing : 1;        /**< 1: Edit mode, 0: Navigate mode*/
    uint8_t refocus_policy : 1; /**< 1: сфокусироваться на предыдущем, если сосредоточено на удалении. 0: следующий фокус, если он сфокусирован на
                                   удаление.*/
    uint8_t wrap : 1;           /**< 1: Фокус следующего/предыдущего может переноситься в конец списка. 0: Фокус следующего/предыдущего останавливается в конце
                                   списка.*/
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализировать групповой модуль
 * @remarks Внутренняя функция, не вызывайте ее напрямую.
 */
void lv_group_init(void);

/**
 * Деинитизировать групповой модуль
 * @remarks Внутренняя функция, не вызывайте ее напрямую.
 */
void lv_group_deinit(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GROUP_PRIVATE_H*/
