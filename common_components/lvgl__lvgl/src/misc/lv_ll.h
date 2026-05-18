/**
 * @file lv_ll.h
 * Обработка названий списков. Узлы направления ослабляются с помощью модуля lv_mem.
 */

#ifndef LV_LL_H
#define LV_LL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Тип манекена для облегчения работы*/
typedef uint8_t lv_ll_node_t;

/** Описание связанного списка*/
typedef struct {
    uint32_t n_size;
    lv_ll_node_t * head;
    lv_ll_node_t * tail;
} lv_ll_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализировать связанный список
 * @param ll_p указатель на переменную lv_ll_t
 * @param node_size размер 1 узла в байтах
 */
void lv_ll_init(lv_ll_t * ll_p, uint32_t node_size);

/**
 * Добавить новую главу в связанный список
 * @param ll_p указатель на связанный список
 * @return указатель на новую голову
 */
void * lv_ll_ins_head(lv_ll_t * ll_p);

/**
 * Вставьте новый узел перед узлом n_act.
 * @param ll_p указатель на связанный список
 * @param n_act указатель на узел
 * @return указатель на новый узел
 */
void * lv_ll_ins_prev(lv_ll_t * ll_p, void * n_act);

/**
 * Добавить новый хвост в связанный список
 * @param ll_p указатель на связанный список
 * @return указатель на новый хвост
 */
void * lv_ll_ins_tail(lv_ll_t * ll_p);

/**
 * Удалите узел «node_p» из связанного списка «ll_p».
 * Он не освобождает память узла.
 * @param ll_p указатель на связанный список «node_p»
 * @param node_p указатель на узел в связанном списке ll_p
 */
void lv_ll_remove(lv_ll_t * ll_p, void * node_p);

void lv_ll_clear_custom(lv_ll_t * ll_p, void(*cleanup)(void *));

/**
 * Удалить и освободить все элементы из связанного списка. Список остается действительным, но становится пустым.
 * @param ll_p указатель на связанный список
 */
void lv_ll_clear(lv_ll_t * ll_p);

/**
 * Переместить узел в новый связанный список
 * @param ll_ori_p указатель на исходный (старый) связанный список
 * @param ll_new_p указатель на новый связанный список
 * @param node указатель на узел
 * @param head правда: быть главой в новом списке
 *             ложь будет хвостом в новом списке
 */
void lv_ll_chg_list(lv_ll_t * ll_ori_p, lv_ll_t * ll_new_p, void * node, bool head);

/**
 * Возврат с головным узлом связанного списка
 * @param ll_p указатель на связанный список
 * @return указатель на заголовок «ll_p»
 */
void * lv_ll_get_head(const lv_ll_t * ll_p);

/**
 * Возврат с хвостовым узлом связанного списка
 * @param ll_p указатель на связанный список
 * @return указатель на хвост 'll_p'
 */
void * lv_ll_get_tail(const lv_ll_t * ll_p);

/**
 * Возврат с указателем следующего узла после 'n_act'
 * @param ll_p указатель на связанный список
 * @param n_act указатель на узел
 * @return указатель на следующий узел
 */
void * lv_ll_get_next(const lv_ll_t * ll_p, const void * n_act);

/**
 * Возврат с указателем соединения после 'n_act'
 * @param ll_p указатель на связанный список
 * @param n_act указатель на узел
 * @return указатель на предыдущий узел
 */
void * lv_ll_get_prev(const lv_ll_t * ll_p, const void * n_act);

/**
 * Возвращает длину связанного списка.
 * @param ll_p указатель на связанный список
 * @return длина связанного списка
 */
uint32_t lv_ll_get_len(const lv_ll_t * ll_p);

/*
 * TODO
 * @param ll_p
 * @param n1_p
 * @param n2_p
недействительныйlv_ll_swap(lv_ll_t*ll_p, недействительный *n1_p, недействительный *n2_p);
 */

/**
 * Переместите узел перед другим узлом в том же связанном списке.
 *
 * @param ll_p указатель на связанный список
 * @param n_act указатель на узел, который нужно переместить
 * @param n_after указатель на узел, который должен быть после `n_act`
 */
void lv_ll_move_before(lv_ll_t * ll_p, void * n_act, void * n_after);

/**
 * Проверьте, пуст ли связанный список
 * @param ll_p указатель на связанный список
 * @return true: связанный список пуст; ложь: не пусто
 */
bool lv_ll_is_empty(lv_ll_t * ll_p);

/**********************
 *      MACROS
 **********************/

#define LV_LL_READ(list, i) for(i = lv_ll_get_head(list); i != NULL; i = lv_ll_get_next(list, i))

#define LV_LL_READ_BACK(list, i) for(i = lv_ll_get_tail(list); i != NULL; i = lv_ll_get_prev(list, i))

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
