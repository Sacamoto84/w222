/**
 * Публичный заголовок для фрагмента
 * @file lv_fragment.h
 */

#ifndef LV_FRAGMENT_H
#define LV_FRAGMENT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj.h"

#if LV_USE_FRAGMENT

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_fragment_manager_t lv_fragment_manager_t;

struct _lv_fragment_t {
    /**
     * Класс этого фрагмента
     */
    const lv_fragment_class_t * cls;
    /**
     * Управляемые состояния фрагментов. Если не ноль, то этот фрагмент является управляемым.
     *
     * @warning Don't modify values inside this struct!
     */
    lv_fragment_managed_states_t * managed;
    /**
     * Менеджер дочерних фрагментов
     */
    lv_fragment_manager_t * child_manager;
    /**
     * lv_obj возвращен create_obj_cb
     */
    lv_obj_t * obj;

};

struct _lv_fragment_class_t {
    /**
     * Функция-конструктор для класса фрагмента
     * @param self Fragment instance
     * @param args Arguments assigned by fragment manager
     */
    void (*constructor_cb)(lv_fragment_t * self, void * args);

    /**
     * Функция деструктора для класса фрагмента
     * @param self Fragment instance, will be freed after this call
     */
    void (*destructor_cb)(lv_fragment_t * self);

    /**
     * Фрагмент прикреплен к менеджеру
     * @param self Fragment instance
     */
    void (*attached_cb)(lv_fragment_t * self);

    /**
     * Фрагмент отделен от менеджера
     * @param self Fragment instance
     */
    void (*detached_cb)(lv_fragment_t * self);

    /**
     * Создание объектов
     * @param self Fragment instance
     * @param container Container of the objects should be created upon
     * @return Created object, NULL if multiple objects has been created
     */
    lv_obj_t * (*create_obj_cb)(lv_fragment_t * self, lv_obj_t * container);

    /**
     *
     * @param self Fragment instance
     * @param obj lv_obj возвращен create_obj_cb
     */
    void (*obj_created_cb)(lv_fragment_t * self, lv_obj_t * obj);

    /**
     * Вызывается перед удалением объектов во фрагменте.
     *
     * @param self Fragment instance
     * @param obj object with this fragment
     */
    void (*obj_will_delete_cb)(lv_fragment_t * self, lv_obj_t * obj);

    /**
     * Вызывается, когда объект, созданный фрагментом, получил событие `LV_EVENT_DELETE`.
     * @param self Fragment instance
     * @param obj object with this fragment
     */
    void (*obj_deleted_cb)(lv_fragment_t * self, lv_obj_t * obj);

    /**
     * Обработать событие
     * @param self Fragment instance
     * @param which User-defined ID of event
     * @param data1 User-defined data
     * @param data2 User-defined data
     */
    bool (*event_cb)(lv_fragment_t * self, int code, void * userdata);

    /**
     * * REQUIRED *: Размер выделения фрагмента
     */
    size_t instance_size;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создать экземпляр диспетчера фрагментов
 * @param parent Parent fragment if this manager is placed inside another fragment, can be null.
 * @return Fragment manager instance
 */
lv_fragment_manager_t * lv_fragment_manager_create(lv_fragment_t * parent);

/**
 * Уничтожить экземпляр диспетчера фрагментов
 * @param manager Fragment manager instance
 */
void lv_fragment_manager_delete(lv_fragment_manager_t * manager);

/**
 * Создать объект всех фрагментов, управляемых этим менеджером.
 * @param manager Fragment manager instance
 */
void lv_fragment_manager_create_obj(lv_fragment_manager_t * manager);

/**
 * Удалить объект, созданный всеми фрагментами, управляемыми этим менеджером. Экземпляр фрагментов не будет удален.
 * @param manager Fragment manager instance
 */
void lv_fragment_manager_delete_obj(lv_fragment_manager_t * manager);

/**
 * Прикрепите фрагмент к менеджеру и добавьте в контейнер.
 * @param manager Fragment manager instance
 * @param fragment Fragment instance
 * @param container Pointer to container object for manager to add objects to
 */
void lv_fragment_manager_add(lv_fragment_manager_t * manager, lv_fragment_t * fragment, lv_obj_t * const * container);

/**
 * Отсоединить и уничтожить фрагмент. Если фрагмент находится в стеке навигации, удалите его из него.
 * @param manager Fragment manager instance
 * @param fragment Fragment instance
 */
void lv_fragment_manager_remove(lv_fragment_manager_t * manager, lv_fragment_t * fragment);

/**
 * Прикрепите фрагмент к менеджеру и добавьте в стек навигации.
 * @param manager Fragment manager instance
 * @param fragment Fragment instance
 * @param container Pointer to container object for manager to add objects to
 */
void lv_fragment_manager_push(lv_fragment_manager_t * manager, lv_fragment_t * fragment, lv_obj_t * const * container);

/**
 * Удалить самый верхний фрагмент для стека
 * @param manager Fragment manager instance
 * @return true if there is fragment to pop
 */
bool lv_fragment_manager_pop(lv_fragment_manager_t * manager);

/**
 * Заменить фрагмент. Старый элемент в стопке будет удален.
 * @param manager Fragment manager instance
 * @param fragment Fragment instance
 * @param container Pointer to container object for manager to add objects to
 */
void lv_fragment_manager_replace(lv_fragment_manager_t * manager, lv_fragment_t * fragment,
                                 lv_obj_t * const * container);

/**
 * Отправить событие в самый верхний фрагмент
 * @param manager Fragment manager instance
 * @param code User-defined ID of event
 * @param userdata User-defined data
 * @return true if fragment returned true
 */
bool lv_fragment_manager_send_event(lv_fragment_manager_t * manager, int code, void * userdata);

/**
 * Получить размер стека этого менеджера фрагментов
 * @param manager Fragment manager instance
 * @return Stack size of this fragment manager
 */
size_t lv_fragment_manager_get_stack_size(lv_fragment_manager_t * manager);

/**
 * Получить экземпляр самого верхнего фрагмента
 * @param manager Fragment manager instance
 * @return Top most fragment instance
 */
lv_fragment_t * lv_fragment_manager_get_top(lv_fragment_manager_t * manager);

/**
 * Найти первый экземпляр фрагмента в контейнере
 * @param manager Fragment manager instance
 * @param container Container which target fragment added to
 * @return First fragment instance in the container
 */
lv_fragment_t * lv_fragment_manager_find_by_container(lv_fragment_manager_t * manager, const lv_obj_t * container);

/**
 * Получить родительский фрагмент
 * @param manager Fragment manager instance
 * @return Parent fragment instance
 */
lv_fragment_t * lv_fragment_manager_get_parent_fragment(lv_fragment_manager_t * manager);

/**
 * Создайте экземпляр фрагмента.
 *
 * @param cls Fragment class. This fragment must return non null object.
 * @param args Arguments assigned by fragment manager
 * @return Fragment instance
 */
lv_fragment_t * lv_fragment_create(const lv_fragment_class_t * cls, void * args);

/**
 * Уничтожить фрагмент.
 * @param fragment Fragment instance.
 */
void lv_fragment_delete(lv_fragment_t * fragment);

/**
 * Получить связанный менеджер этого фрагмента
 * @param fragment Fragment instance
 * @return Fragment manager instance
 */
lv_fragment_manager_t * lv_fragment_get_manager(lv_fragment_t * fragment);

/**
 * Получить объект-контейнер этого фрагмента
 * @param fragment Fragment instance
 * @return Reference to container object
 */
lv_obj_t * const * lv_fragment_get_container(lv_fragment_t * fragment);

/**
 * Получить родительский фрагмент этого фрагмента
 * @param fragment Fragment instance
 * @return Parent fragment
 */
lv_fragment_t * lv_fragment_get_parent(lv_fragment_t * fragment);

/**
 * Создать объект по фрагменту.
 *
 * @param fragment Fragment instance.
 * @param container Container of the objects should be created upon.
 * @return Created object
 */
lv_obj_t * lv_fragment_create_obj(lv_fragment_t * fragment, lv_obj_t * container);

/**
 * Удалить созданный объект фрагмента
 *
 * @param fragment Fragment instance.
 */
void lv_fragment_delete_obj(lv_fragment_t * fragment);

/**
 * Уничтожьте объекты во фрагменте и воссоздайте их.
 * @param fragment Fragment instance
 */
void lv_fragment_recreate_obj(lv_fragment_t * fragment);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_FRAGMENT*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_FRAGMENT_H*/
