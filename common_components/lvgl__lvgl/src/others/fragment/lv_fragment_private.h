/**
 * @file lv_fragment_private.h
 *
 */

#ifndef LV_FRAGMENT_PRIVATE_H
#define LV_FRAGMENT_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_fragment.h"

#if LV_USE_FRAGMENT

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Состояния фрагмента
 */
struct _lv_fragment_managed_states_t  {
    /**
     * Класс фрагмента
     */
    const lv_fragment_class_t * cls;
    /**
     * Менеджер фрагмента, прикрепленного к
     */
    lv_fragment_manager_t * manager;
    /**
     * Объект-контейнер, к которому добавляется представление фрагмента
     */
    lv_obj_t * const * container;
    /**
     * Экземпляр фрагмента
     */
    lv_fragment_t * instance;
    /**
     * существует между`create_obj_cb`и `obj_deleted_cb`
     */
    bool obj_created;
    /**
     * true до вызова `lv_fragment_delete_obj`. Не прикасайтесь ни к какому предмету, если это правда
     */
    bool destroying_obj;
    /**
     * правда, если этот фрагмент находится в стеке навигации, который можно потерять
     */
    bool in_stack;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_FRAGMENT */

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_FRAGMENT_PRIVATE_H*/
