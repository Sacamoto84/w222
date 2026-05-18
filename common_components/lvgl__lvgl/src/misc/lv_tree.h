/**
 * @file lv_tree.h
 * Дерево. Узлы дерева стремятся привлечь модуль lv_mem.
 */

#ifndef LV_TREE_H
#define LV_TREE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/

#define LV_TREE_NODE(n) ((lv_tree_node_t*)(n))

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_tree_class_t lv_tree_class_t;
typedef struct _lv_tree_node_t lv_tree_node_t;

typedef void (*lv_tree_constructor_cb_t)(const lv_tree_class_t * class_p, lv_tree_node_t * node);
typedef void (*lv_tree_destructor_cb_t)(const lv_tree_class_t * class_p, lv_tree_node_t * node);

/**
 * Опишите общие методы каждого объекта.
 * Аналогично классу C++.
 */
struct _lv_tree_class_t {
    const lv_tree_class_t * base_class;
    uint32_t instance_size;
    lv_tree_constructor_cb_t constructor_cb;
    lv_tree_destructor_cb_t destructor_cb;
};

/** Описание узла дерева*/
struct _lv_tree_node_t {
    lv_tree_node_t * parent;
    lv_tree_node_t ** children;
    uint32_t child_cnt;
    uint32_t child_cap;
    const lv_tree_class_t * class_p;
};

enum _lv_tree_walk_mode_t {
    LV_TREE_WALK_PRE_ORDER = 0,
    LV_TREE_WALK_POST_ORDER,
};
typedef uint8_t lv_tree_walk_mode_t;

typedef bool (*lv_tree_traverse_cb_t)(const lv_tree_node_t * node, void * user_data);
typedef bool (*lv_tree_before_cb_t)(const lv_tree_node_t * node, void * user_data);
typedef void (*lv_tree_after_cb_t)(const lv_tree_node_t * node, void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

extern const lv_tree_class_t lv_tree_node_class;

/**
 * @brief Создать узел дерева
 * @param class_p указатель на класс узла
 * @param parent указатель на родительский узел (или NULL, если это корневой узел)
 * @return указатель на новый узел
 */
lv_tree_node_t * lv_tree_node_create(const lv_tree_class_t * class_p, lv_tree_node_t * parent);

/**
 * @brief Рекурсивно удалить узел дерева и все его дочерние элементы
 * @param node указатель на узел, который нужно удалить
 */
void lv_tree_node_delete(lv_tree_node_t * node);

/**
 * @brief Рекурсивно пройдитесь по дереву и вызовите функцию обратного вызова на каждом узле.
 * @param node указатель на корневой узел дерева
 * Режим @param LV_TREE_WALK_PRE_ORDER или LV_TREE_WALK_POST_ORDER
 * @param cb функция обратного вызова для вызова каждого узла
 * @param bcb функция обратного вызова для вызова перед посещением узла
 * @param acb функция обратного вызова для вызова после посещения узла
 * @param user_data пользовательские данные для передачи в функции обратного вызова
 * @return true: обход завершен; false: обход нарушен
 */
bool lv_tree_walk(const lv_tree_node_t * node,
                  lv_tree_walk_mode_t mode,
                  lv_tree_traverse_cb_t cb,
                  lv_tree_before_cb_t bcb,
                  lv_tree_after_cb_t acb,
                  void * user_data);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
