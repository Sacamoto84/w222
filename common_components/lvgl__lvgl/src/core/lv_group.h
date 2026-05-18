/**
 * @file lv_group.h
 *
 */

#ifndef LV_GROUP_H
#define LV_GROUP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include "../misc/lv_types.h"
#include "../misc/lv_ll.h"

/*********************
 *      DEFINES
 *********************/
/** Предопределенные клавиши для управления темой, какой виджет находится в фокусе, черезlv_group_send(группа, c) */
typedef enum {
    LV_KEY_UP        = 17,  /*0x11*/
    LV_KEY_DOWN      = 18,  /*0x12*/
    LV_KEY_RIGHT     = 19,  /*0x13*/
    LV_KEY_LEFT      = 20,  /*0x14*/
    LV_KEY_ESC       = 27,  /*0x1B*/
    LV_KEY_DEL       = 127, /*0x7F*/
    LV_KEY_BACKSPACE = 8,   /*0x08*/
    LV_KEY_ENTER     = 10,  /*0x0A, '\n'*/
    LV_KEY_NEXT      = 9,   /*0x09, '\t'*/
    LV_KEY_PREV      = 11,  /*0x0B, '*/
    LV_KEY_HOME      = 2,   /*0x02, STX*/
    LV_KEY_END       = 3,   /*0x03, ETX*/
} lv_key_t;

/**********************
 *      TYPEDEFS
 **********************/

typedef void (*lv_group_focus_cb_t)(lv_group_t *);
typedef void (*lv_group_edge_cb_t)(lv_group_t *, bool);

typedef enum {
    LV_GROUP_REFOCUS_POLICY_NEXT = 0,
    LV_GROUP_REFOCUS_POLICY_PREV = 1
} lv_group_refocus_policy_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте новую группу виджетов.
 * @return          указатель на новую группу виджетов
 */
lv_group_t * lv_group_create(void);

/**
 * Удалить групповой объект.
 * @param group     указатель на группу
 */
void lv_group_delete(lv_group_t * group);

/**
 * Установить группу по умолчанию. В эту группу будут добавлены новые виджеты, если они включены в
 * их класс с `add_to_def_group = true`.
 * @param group     указатель на группу (может быть`NULL`)
 */
void lv_group_set_default(lv_group_t * group);

/**
 * Получить группу по умолчанию.
 * @return          указатель на группу по умолчанию
 */
lv_group_t * lv_group_get_default(void);

/**
 * Добавьте виджет в группу.
 * @param group     указатель на группу
 * @param obj       указатель на виджет, который нужно добавить
 */
void lv_group_add_obj(lv_group_t * group, lv_obj_t * obj);

/**
 * Поменяйте местами 2 виджета в группе.  Виджеты должны находиться в одной группе.
 * @param obj1  указатель на виджет
 * @param obj2  указатель на другой виджет
 */
void lv_group_swap_obj(lv_obj_t * obj1, lv_obj_t * obj2);

/**
 * Удалите виджет из группы.
 * @param obj       указатель на виджет, который нужно удалить
 */
void lv_group_remove_obj(lv_obj_t * obj);

/**
 * Удалить все виджеты из группы.
 * @param group     указатель на группу
 */
void lv_group_remove_all_objs(lv_group_t * group);

/**
 * Сосредоточьтесь на виджете (расфокусируйте текущий).
 * @param obj       указатель на виджет, на котором нужно сосредоточиться
 */
void lv_group_focus_obj(lv_obj_t * obj);

/**
 * Сосредоточьтесь на следующем виджете в группе (расфокусируйте текущий).
 * @param group     указатель на группу
 */
void lv_group_focus_next(lv_group_t * group);

/**
 * Сосредоточиться на предыдущем виджете в группе (расфокусировать текущий).
 * @param group     указатель на группу
 */
void lv_group_focus_prev(lv_group_t * group);

/**
 * Запретить изменение фокуса с текущего виджета.
 * @param group     указатель на группу
 * @param en        true: заморозить, false: снять заморозку (обычный режим)
 */
void lv_group_focus_freeze(lv_group_t * group, bool en);

/**
 * Отправьте управляющий символ в виджет, который находится в фокусе группы.
 * @param group     указатель на группу
 * @param c         символ (используйтеLV_KEY_.. для навигации)
 * @return          результат виджета с фокусом в группе.
 */
lv_result_t lv_group_send_data(lv_group_t * group, uint32_t c);

/**
 * Установите функцию для группы, которая будет вызываться, когда новый виджет получит фокус.
 * @param group         указатель на группу
 * @param focus_cb      функция обратного вызова или NULL, если не используется
 */
void lv_group_set_focus_cb(lv_group_t * group, lv_group_focus_cb_t focus_cb);

/**
 * Установите функцию для группы, которая будет вызываться при достижении края фокуса.
 * @param group         указатель на группу
 * @param edge_cb      функция обратного вызова или NULL, если не используется
 */
void lv_group_set_edge_cb(lv_group_t * group, lv_group_edge_cb_t edge_cb);

/**
 * Установите, будет ли фокусироваться следующий или предыдущий виджет в группе, когда виджет, у которого есть
 * фокус удаляется.
 * @param group         указатель на группу
 * @param policy        новое перечисление политики перефокусировки
 */
void lv_group_set_refocus_policy(lv_group_t * group, lv_group_refocus_policy_t policy);

/**
 * Вручную установите текущий режим (редактирование или навигация).
 * @param group         указатель на группу
 * @param edit          true: режим редактирования; ложь: режим навигации
 */
void lv_group_set_editing(lv_group_t * group, bool edit);

/**
 * Установите, будет ли перемещение фокуса на следующий/предыдущий виджет разрешен перенос с
 * первый->последний или последний->первый виджет.
 * @param group         указатель на группу
 * @param               en true: перенос включен; false: перенос отключен
 */
void lv_group_set_wrap(lv_group_t * group, bool en);

/**
 * Получите виджет с фокусом или NULL, если его нет.
 * @param group         указатель на группу
 * @return              указатель на виджет с фокусом
 */
lv_obj_t * lv_group_get_focused(const lv_group_t * group);

/**
 * Получите функцию обратного вызова фокуса группы.
 * @param group указатель на группу
 * @return функция обратного вызова или NULL, если не установлено
 */
lv_group_focus_cb_t lv_group_get_focus_cb(const lv_group_t * group);

/**
 * Получите функцию обратного вызова группы.
 * @param group указатель на группу
 * @return функция обратного вызова или NULL, если не установлено
 */
lv_group_edge_cb_t lv_group_get_edge_cb(const lv_group_t * group);

/**
 * Получить текущий режим (редактировать или перемещаться).
 * @param group         указатель на группу
 * @return              true: режим редактирования; ложь: режим навигации
 */
bool lv_group_get_editing(const lv_group_t * group);

/**
 * Узнайте, позволит ли перемещение фокуса на следующий/предыдущий виджет выполнить перенос из
 * первый->последний или последний->первый виджет.
 * @param group         указатель на группу
 */
bool lv_group_get_wrap(lv_group_t * group);

/**
 * Получить количество виджетов в группе.
 * @param group         указатель на группу
 * @return              количество виджетов в группе
 */
uint32_t lv_group_get_obj_count(lv_group_t * group);

/**
 * Получите n-й виджет в группе.
 * @param group         указатель на группу
 * @param index         индекс виджета внутри группы
 * @return              указатель на виджет
 */
lv_obj_t * lv_group_get_obj_by_index(lv_group_t * group, uint32_t index);

/**
 * Получите количество групп.
 * @return              количество групп
 */
uint32_t lv_group_get_count(void);

/**
 * Получить группу по ее индексу.
 * @param index         индекс группы
 * @return              указатель на группу
 */
lv_group_t  * lv_group_by_index(uint32_t index);

#if LV_USE_EXT_DATA
/**
 * @brief Прикрепляет внешние пользовательские данные и обратный вызов деструктора к группе.
 *
 * Связывает пользовательские данные с группой LVGL и определяет функцию деструктора.
 * который будет автоматически вызываться при удалении группы для правильной очистки.
 * связанные ресурсы.
 *
 * @param group      Указатель на группу
 * @param data       Пользовательский указатель данных для связи с группой
 * @param free_cb    Функция обратного вызова для очисткиext_dataпри удалении группы.
 *                   Получаетext_dataв качестве параметра.  NULLозначает, что очистка не требуется.
 */
void lv_group_set_external_data(lv_group_t * group, void * data, void (* free_cb)(void * data));
#endif

/**
 * Установить данные пользователя в группу
 * @param group указатель на группу
 * @param user_data указатель на данные пользователя
 */
void lv_group_set_user_data(lv_group_t * group, void * user_data);

/**
 * Получить указатель на данные пользователя группы
 * @param indev указатель на группу
 * @return указатель на данные пользователя или NULL, если группа NULL
 */
void * lv_group_get_user_data(const lv_group_t * group);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_GROUP_H*/
