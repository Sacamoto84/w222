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
     * @warning Не изменяйте значения внутри этой структуры!
     */
    lv_fragment_managed_states_t * managed;
    /**
     * Менеджер дочерних фрагментов
     */
    lv_fragment_manager_t * child_manager;
    /**
     * lv_obj вернулся create_obj_cb
     */
    lv_obj_t * obj;

};

struct _lv_fragment_class_t {
    /**
     * Функция-конструктор для класса фрагмента
     * @param self Экземпляр фрагмента
     * @param args Аргументы, назначенные менеджером фрагментов
     */
    void (*constructor_cb)(lv_fragment_t * self, void * args);

    /**
     * Функция деструктора для класса фрагмента
     * @param self Экземпляр фрагмента будет освобожден после этого вызова.
     */
    void (*destructor_cb)(lv_fragment_t * self);

    /**
     * Фрагмент прикреплен к менеджеру
     * @param self Экземпляр фрагмента
     */
    void (*attached_cb)(lv_fragment_t * self);

    /**
     * Фрагмент отделен от менеджера
     * @param self Экземпляр фрагмента
     */
    void (*detached_cb)(lv_fragment_t * self);

    /**
     * Создание объектов
     * @param self Экземпляр фрагмента
     * @param container Контейнер объектов должен быть создан после
     * @return Созданный объект, NULL, если было создано несколько объектов.
     */
    lv_obj_t * (*create_obj_cb)(lv_fragment_t * self, lv_obj_t * container);

    /**
     *
     * @param self Экземпляр фрагмента
     * @param obj lv_obj вернулся create_obj_cb
     */
    void (*obj_created_cb)(lv_fragment_t * self, lv_obj_t * obj);

    /**
     * Вызывается перед удалением объектов во фрагменте.
     *
     * @param self Экземпляр фрагмента
     * @param obj объект с этим фрагментом
     */
    void (*obj_will_delete_cb)(lv_fragment_t * self, lv_obj_t * obj);

    /**
     * Вызывается, когда объект, созданный фрагментом, получил событие `LV_EVENT_DELETE`.
     * @param self Экземпляр фрагмента
     * @param obj объект с этим фрагментом
     */
    void (*obj_deleted_cb)(lv_fragment_t * self, lv_obj_t * obj);

    /**
     * Обработать событие
     * @param self Экземпляр фрагмента
     * @param which Определяемый пользователемIDсобытия
     * @param data1 Пользовательские данные
     * @param data2 Пользовательские данные
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
 * @param parent Родительский фрагмент, если этот менеджер размещен внутри другого фрагмента, может иметь значение null.
 * @return Экземпляр диспетчера фрагментов
 */
lv_fragment_manager_t * lv_fragment_manager_create(lv_fragment_t * parent);

/**
 * Уничтожить экземпляр диспетчера фрагментов
 * @param manager Экземпляр диспетчера фрагментов
 */
void lv_fragment_manager_delete(lv_fragment_manager_t * manager);

/**
 * Создать объект всех фрагментов, управляемых этим менеджером.
 * @param manager Экземпляр диспетчера фрагментов
 */
void lv_fragment_manager_create_obj(lv_fragment_manager_t * manager);

/**
 * Удалить объект, созданный всеми фрагментами, управляемыми этим менеджером. Экземпляр фрагментов не будет удален.
 * @param manager Экземпляр диспетчера фрагментов
 */
void lv_fragment_manager_delete_obj(lv_fragment_manager_t * manager);

/**
 * Прикрепите фрагмент к менеджеру и добавьте в контейнер.
 * @param manager Экземпляр диспетчера фрагментов
 * @param fragment Экземпляр фрагмента
 * @param container Указатель на объект-контейнер, в который менеджер может добавлять объекты.
 */
void lv_fragment_manager_add(lv_fragment_manager_t * manager, lv_fragment_t * fragment, lv_obj_t * const * container);

/**
 * Отсоединить и уничтожить фрагмент. Если фрагмент находится в стеке навигации, удалите его из него.
 * @param manager Экземпляр диспетчера фрагментов
 * @param fragment Экземпляр фрагмента
 */
void lv_fragment_manager_remove(lv_fragment_manager_t * manager, lv_fragment_t * fragment);

/**
 * Прикрепите фрагмент к менеджеру и добавьте в стек навигации.
 * @param manager Экземпляр диспетчера фрагментов
 * @param fragment Экземпляр фрагмента
 * @param container Указатель на объект-контейнер, в который менеджер может добавлять объекты.
 */
void lv_fragment_manager_push(lv_fragment_manager_t * manager, lv_fragment_t * fragment, lv_obj_t * const * container);

/**
 * Удалить самый верхний фрагмент для стека
 * @param manager Экземпляр диспетчера фрагментов
 * @return true, если есть фрагмент, который нужно вытащить
 */
bool lv_fragment_manager_pop(lv_fragment_manager_t * manager);

/**
 * Заменить фрагмент. Старый элемент в стопке будет удален.
 * @param manager Экземпляр диспетчера фрагментов
 * @param fragment Экземпляр фрагмента
 * @param container Указатель на объект-контейнер, в который менеджер может добавлять объекты.
 */
void lv_fragment_manager_replace(lv_fragment_manager_t * manager, lv_fragment_t * fragment,
                                 lv_obj_t * const * container);

/**
 * Отправить событие в самый верхний фрагмент
 * @param manager Экземпляр диспетчера фрагментов
 * @param code Определяемый пользователемIDсобытия
 * @param userdata Пользовательские данные
 * @return true, если фрагмент вернул true
 */
bool lv_fragment_manager_send_event(lv_fragment_manager_t * manager, int code, void * userdata);

/**
 * Получить размер стека этого менеджера фрагментов
 * @param manager Экземпляр диспетчера фрагментов
 * @return Размер стека этого менеджера фрагментов
 */
size_t lv_fragment_manager_get_stack_size(lv_fragment_manager_t * manager);

/**
 * Получить экземпляр самого верхнего фрагмента
 * @param manager Экземпляр диспетчера фрагментов
 * @return Самый высокий экземпляр фрагмента
 */
lv_fragment_t * lv_fragment_manager_get_top(lv_fragment_manager_t * manager);

/**
 * Найти первый экземпляр фрагмента в контейнере
 * @param manager Экземпляр диспетчера фрагментов
 * @param container Контейнер, в который добавлен целевой фрагмент
 * @return Первый экземпляр фрагмента в контейнере
 */
lv_fragment_t * lv_fragment_manager_find_by_container(lv_fragment_manager_t * manager, const lv_obj_t * container);

/**
 * Получить родительский фрагмент
 * @param manager Экземпляр диспетчера фрагментов
 * @return Экземпляр родительского фрагмента
 */
lv_fragment_t * lv_fragment_manager_get_parent_fragment(lv_fragment_manager_t * manager);

/**
 * Создайте экземпляр фрагмента.
 *
 * @param cls Класс фрагмента. Этот фрагмент должен возвращать ненулевой объект.
 * @param args Аргументы, назначенные менеджером фрагментов
 * @return Экземпляр фрагмента
 */
lv_fragment_t * lv_fragment_create(const lv_fragment_class_t * cls, void * args);

/**
 * Уничтожить фрагмент.
 * @param fragment Экземпляр фрагмента.
 */
void lv_fragment_delete(lv_fragment_t * fragment);

/**
 * Получить связанный менеджер этого фрагмента
 * @param fragment Экземпляр фрагмента
 * @return Экземпляр диспетчера фрагментов
 */
lv_fragment_manager_t * lv_fragment_get_manager(lv_fragment_t * fragment);

/**
 * Получить объект-контейнер этого фрагмента
 * @param fragment Экземпляр фрагмента
 * @return Ссылка на объект-контейнер
 */
lv_obj_t * const * lv_fragment_get_container(lv_fragment_t * fragment);

/**
 * Получить родительский фрагмент этого фрагмента
 * @param fragment Экземпляр фрагмента
 * @return Родительский фрагмент
 */
lv_fragment_t * lv_fragment_get_parent(lv_fragment_t * fragment);

/**
 * Создать объект по фрагменту.
 *
 * @param fragment Экземпляр фрагмента.
 * @param container Контейнер объектов должен быть создан.
 * @return Созданный объект
 */
lv_obj_t * lv_fragment_create_obj(lv_fragment_t * fragment, lv_obj_t * container);

/**
 * Удалить созданный объект фрагмента
 *
 * @param fragment Экземпляр фрагмента.
 */
void lv_fragment_delete_obj(lv_fragment_t * fragment);

/**
 * Уничтожьте объекты во фрагменте и воссоздайте их.
 * @param fragment Экземпляр фрагмента
 */
void lv_fragment_recreate_obj(lv_fragment_t * fragment);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_FRAGMENT*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FRAGMENT_H*/
