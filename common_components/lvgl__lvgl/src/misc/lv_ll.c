/**
 * @file lv_ll.c
 * Обработка связанных списков.
 * Узлы направления ослабляются с помощью модуля lv_mem.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_ll.h"
#include "../stdlib/lv_mem.h"

/*********************
 *      DEFINES
 *********************/
#define LL_NODE_META_SIZE (sizeof(lv_ll_node_t *) + sizeof(lv_ll_node_t *))
#define LL_PREV_P_OFFSET(ll_p) (ll_p->n_size)
#define LL_NEXT_P_OFFSET(ll_p) (ll_p->n_size + sizeof(lv_ll_node_t *))

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void node_set_prev(lv_ll_t * ll_p, lv_ll_node_t * act, lv_ll_node_t * prev);
static void node_set_next(lv_ll_t * ll_p, lv_ll_node_t * act, lv_ll_node_t * next);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_ll_init(lv_ll_t * ll_p, uint32_t node_size)
{
    ll_p->head = NULL;
    ll_p->tail = NULL;
#ifdef LV_ARCH_64
    /*Округляем размер до 8.*/
    node_size = (node_size + 7) & (~0x7);
#else
    /*Округляем размер до 4.*/
    node_size = (node_size + 3) & (~0x3);
#endif

    ll_p->n_size = node_size;
}

void * lv_ll_ins_head(lv_ll_t * ll_p)
{
    lv_ll_node_t * n_new;

    n_new = lv_malloc(ll_p->n_size + LL_NODE_META_SIZE);

    if(n_new != NULL) {
        node_set_prev(ll_p, n_new, NULL);       /*Нет пред. перед новым руководителем*/
        node_set_next(ll_p, n_new, ll_p->head); /*После новой приходит старая голова*/

        if(ll_p->head != NULL) { /*Если есть старая голова, то прежде чем она пойдет новая*/
            node_set_prev(ll_p, ll_p->head, n_new);
        }

        ll_p->head = n_new;      /*Установите новую головку в dsc.*/
        if(ll_p->tail == NULL) { /*Если хвоста (1. узла) нет, установите и хвост.*/
            ll_p->tail = n_new;
        }
    }

    return n_new;
}

void * lv_ll_ins_prev(lv_ll_t * ll_p, void * n_act)
{
    lv_ll_node_t * n_new;

    if(NULL == ll_p || NULL == n_act) return NULL;

    if(lv_ll_get_head(ll_p) == n_act) {
        n_new = lv_ll_ins_head(ll_p);
        if(n_new == NULL) return NULL;
    }
    else {
        n_new = lv_malloc(ll_p->n_size + LL_NODE_META_SIZE);
        if(n_new == NULL) return NULL;

        lv_ll_node_t * n_prev;
        n_prev = lv_ll_get_prev(ll_p, n_act);
        node_set_next(ll_p, n_prev, n_new);
        node_set_prev(ll_p, n_new, n_prev);
        node_set_prev(ll_p, n_act, n_new);
        node_set_next(ll_p, n_new, n_act);
    }

    return n_new;
}

void * lv_ll_ins_tail(lv_ll_t * ll_p)
{
    lv_ll_node_t * n_new;

    n_new = lv_malloc(ll_p->n_size + LL_NODE_META_SIZE);

    if(n_new != NULL) {
        node_set_next(ll_p, n_new, NULL);       /*Нет следующего после нового хвоста*/
        node_set_prev(ll_p, n_new, ll_p->tail); /*Предыдущий. прежде чем новый будет старым хвостом*/
        if(ll_p->tail != NULL) {                /*Если есть старый хвост, то за ним следует новый.*/
            node_set_next(ll_p, ll_p->tail, n_new);
        }

        ll_p->tail = n_new;      /*Установите новый хвост в dsc.*/
        if(ll_p->head == NULL) { /*Если головки (1. узла) нет, установите и головку.*/
            ll_p->head = n_new;
        }
    }

    return n_new;
}

void lv_ll_remove(lv_ll_t * ll_p, void * node_p)
{
    if(ll_p == NULL) return;

    if(lv_ll_get_head(ll_p) == node_p) {
        /*Голова будет узлом после 'node_p'.*/
        ll_p->head = lv_ll_get_next(ll_p, node_p);
        if(ll_p->head == NULL) {
            ll_p->tail = NULL;
        }
        else {
            node_set_prev(ll_p, ll_p->head, NULL);
        }
    }
    else if(lv_ll_get_tail(ll_p) == node_p) {
        /*Новый хвост будет узлом передnode_p.*/
        ll_p->tail = lv_ll_get_prev(ll_p, node_p);
        if(ll_p->tail == NULL) {
            ll_p->head = NULL;
        }
        else {
            node_set_next(ll_p, ll_p->tail, NULL);
        }
    }
    else {
        lv_ll_node_t * n_prev = lv_ll_get_prev(ll_p, node_p);
        lv_ll_node_t * n_next = lv_ll_get_next(ll_p, node_p);

        node_set_next(ll_p, n_prev, n_next);
        node_set_prev(ll_p, n_next, n_prev);
    }
}

void lv_ll_clear_custom(lv_ll_t * ll_p, void(*cleanup)(void *))
{
    void * i;
    void * i_next;

    i      = lv_ll_get_head(ll_p);
    i_next = NULL;

    while(i != NULL) {
        i_next = lv_ll_get_next(ll_p, i);
        if(cleanup == NULL) {
            lv_ll_remove(ll_p, i);
            lv_free(i);
        }
        else {
            cleanup(i);
        }
        i = i_next;
    }
}

void lv_ll_chg_list(lv_ll_t * ll_ori_p, lv_ll_t * ll_new_p, void * node, bool head)
{
    lv_ll_remove(ll_ori_p, node);

    if(head) {
        /*Установить узел в качестве головы*/
        node_set_prev(ll_new_p, node, NULL);
        node_set_next(ll_new_p, node, ll_new_p->head);

        if(ll_new_p->head != NULL) { /*Если есть старая голова, то прежде чем она пойдет новая*/
            node_set_prev(ll_new_p, ll_new_p->head, node);
        }

        ll_new_p->head = node;       /*Установите новую головку в dsc.*/
        if(ll_new_p->tail == NULL) { /*Если хвоста (первого узла) нет, установите и хвост.*/
            ll_new_p->tail = node;
        }
    }
    else {
        /*Установить узел как хвост*/
        node_set_prev(ll_new_p, node, ll_new_p->tail);
        node_set_next(ll_new_p, node, NULL);

        if(ll_new_p->tail != NULL) { /*Если есть старый хвост, то вслед за ним идет новый*/
            node_set_next(ll_new_p, ll_new_p->tail, node);
        }

        ll_new_p->tail = node;       /*Установите новый хвост в dsc.*/
        if(ll_new_p->head == NULL) { /*Если головы (первого узла) нет, установите и голову.*/
            ll_new_p->head = node;
        }
    }
}

void * lv_ll_get_head(const lv_ll_t * ll_p)
{
    if(ll_p == NULL) return NULL;
    return ll_p->head;
}

void * lv_ll_get_tail(const lv_ll_t * ll_p)
{
    if(ll_p == NULL) return NULL;
    return ll_p->tail;
}

void * lv_ll_get_next(const lv_ll_t * ll_p, const void * n_act)
{
    /*Указатель на следующий узел хранится в конце этого узла.
     *Идите туда и верните найденный там адрес*/
    const lv_ll_node_t * n_act_d = n_act;
    n_act_d += LL_NEXT_P_OFFSET(ll_p);
    return *((lv_ll_node_t **)n_act_d);
}

void * lv_ll_get_prev(const lv_ll_t * ll_p, const void * n_act)
{
    /*Указатель на пред. узел хранится в конце этого узла.
     *Идите туда и верните найденный там адрес*/
    const lv_ll_node_t * n_act_d = n_act;
    n_act_d += LL_PREV_P_OFFSET(ll_p);
    return *((lv_ll_node_t **)n_act_d);
}

uint32_t lv_ll_get_len(const lv_ll_t * ll_p)
{
    uint32_t len = 0;
    void * node;

    for(node = lv_ll_get_head(ll_p); node != NULL; node = lv_ll_get_next(ll_p, node)) {
        len++;
    }

    return len;
}

void lv_ll_move_before(lv_ll_t * ll_p, void * n_act, void * n_after)
{
    if(n_act == n_after) return; /*Не может двигаться перед собой*/

    void * n_before;
    if(n_after != NULL)
        n_before = lv_ll_get_prev(ll_p, n_after);
    else
        n_before = lv_ll_get_tail(ll_p); /*если`n_after`— этоNULL,`n_act`должен быть новым хвостом*/

    if(n_act == n_before) return; /*Уже до `n_after`*/

    /*Гораздо проще удалить из списка и добавить заново*/
    lv_ll_remove(ll_p, n_act);

    /*Добавьте еще раз, установив пред. и следующие узлы*/
    node_set_next(ll_p, n_before, n_act);
    node_set_prev(ll_p, n_act, n_before);
    node_set_prev(ll_p, n_after, n_act);
    node_set_next(ll_p, n_act, n_after);

    /*Если`n_act`был перенесен в NULL, то он станет новым хвостом.*/
    if(n_after == NULL) ll_p->tail = n_act;

    /*Если`n_act`был перенесен в `NULL`, то это новая голова.*/
    if(n_before == NULL) ll_p->head = n_act;
}

bool lv_ll_is_empty(lv_ll_t * ll_p)
{
    if(ll_p == NULL) return true;

    if(ll_p->head == NULL && ll_p->tail == NULL) return true;

    return false;
}

void lv_ll_clear(lv_ll_t * ll_p)
{
    lv_ll_clear_custom(ll_p, NULL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Установить указатель предыдущего узла узла
 * @param ll_p указатель на связанный список
 * @param act указатель на узел, который пред. указатель узла должен быть установлен
 * @param prev указатель на узел, который должен быть предыдущим узлом перед «действием»
 */
static void node_set_prev(lv_ll_t * ll_p, lv_ll_node_t * act, lv_ll_node_t * prev)
{
    if(act == NULL) return; /*Невозможно установить предыдущий узел `NULL`.*/

    uint8_t * act8 = (uint8_t *)act;

    act8 += LL_PREV_P_OFFSET(ll_p);

    lv_ll_node_t ** act_node_p = (lv_ll_node_t **) act8;
    lv_ll_node_t ** prev_node_p = (lv_ll_node_t **) &prev;

    *act_node_p = *prev_node_p;
}

/**
 * Установите «указатель следующего узла» узла
 * @param ll_p указатель на связанный список
 * @param act указатель на узел, указатель которого должен быть установлен на следующий узел
 * @param next указатель на узел, который должен быть следующим узлом перед «действием»
 */
static void node_set_next(lv_ll_t * ll_p, lv_ll_node_t * act, lv_ll_node_t * next)
{
    if(act == NULL) return; /*Невозможно установить следующий узел `NULL`.*/
    uint8_t * act8 = (uint8_t *)act;

    act8 += LL_NEXT_P_OFFSET(ll_p);
    lv_ll_node_t ** act_node_p = (lv_ll_node_t **) act8;
    lv_ll_node_t ** next_node_p = (lv_ll_node_t **) &next;

    *act_node_p = *next_node_p;
}
