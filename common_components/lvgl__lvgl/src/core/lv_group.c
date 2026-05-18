/**
 * @file lv_group.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_group_private.h"
#include "../core/lv_obj_private.h"
#include "../core/lv_global.h"
#include "../indev/lv_indev.h"
#include "../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/
#define default_group LV_GLOBAL_DEFAULT()->group_default
#define group_ll_p &(LV_GLOBAL_DEFAULT()->group_ll)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool focus_next_core(lv_group_t * group, void * (*begin)(const lv_ll_t *),
                            void * (*move)(const lv_ll_t *, const void *));
static void lv_group_refocus(lv_group_t * g);
static lv_indev_t * get_indev(const lv_group_t * g);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_group_init(void)
{
    lv_ll_init(group_ll_p, sizeof(lv_group_t));
}

void lv_group_deinit(void)
{
    lv_ll_clear(group_ll_p);
}

lv_group_t * lv_group_create(void)
{
    lv_group_t * group = lv_ll_ins_head(group_ll_p);
    LV_ASSERT_MALLOC(group);
    if(group == NULL) return NULL;
    lv_ll_init(&group->obj_ll, sizeof(lv_obj_t *));

    group->obj_focus      = NULL;
    group->frozen         = 0;
    group->focus_cb       = NULL;
    group->edge_cb        = NULL;
    group->editing        = 0;
    group->refocus_policy = LV_GROUP_REFOCUS_POLICY_PREV;
    group->wrap           = 1;
    group->user_data      = NULL;
#if LV_USE_EXT_DATA
    group->ext_data.free_cb = NULL;
    group->ext_data.data = NULL;
#endif

    return group;
}

void lv_group_delete(lv_group_t * group)
{
    /*Расфокусировать текущий сфокусированный объект*/
    LV_ASSERT_NULL(group);
    if(group->obj_focus != NULL) {
        lv_obj_send_event(*group->obj_focus, LV_EVENT_DEFOCUSED, get_indev(group));
        lv_obj_invalidate(*group->obj_focus);
    }

    /*Удалить объекты из группы*/
    lv_obj_t ** obj;
    LV_LL_READ(&group->obj_ll, obj) {
        if((*obj)->spec_attr)(*obj)->spec_attr->group_p = NULL;
    }

    /*Удалите группу со всех устройств разработки. */
    lv_indev_t * indev = lv_indev_get_next(NULL);
    while(indev) {
        if(lv_indev_get_group(indev) == group) {
            lv_indev_set_group(indev, NULL);
        }
        indev = lv_indev_get_next(indev);
    }

    /*Если группа является группой по умолчанию, установите группу по умолчанию как NULL.*/
    if(group == lv_group_get_default()) lv_group_set_default(NULL);

    lv_ll_clear(&(group->obj_ll));
    lv_ll_remove(group_ll_p, group);
#if LV_USE_EXT_DATA
    if(group->ext_data.free_cb) {
        group->ext_data.free_cb(group->ext_data.data);
        group->ext_data.data = NULL;
    }
#endif
    lv_free(group);
}

void lv_group_set_default(lv_group_t * group)
{
    default_group = group;
}

lv_group_t * lv_group_get_default(void)
{
    return default_group;
}

void lv_group_add_obj(lv_group_t * group, lv_obj_t * obj)
{
    if(group == NULL) return;

    LV_LOG_TRACE("begin");

    /*Убедитесь, что объект удален из текущей группы.*/
    lv_group_remove_obj(obj);

    if(obj->spec_attr == NULL) lv_obj_allocate_spec_attr(obj);
    obj->spec_attr->group_p = group;

    lv_obj_t ** next = lv_ll_ins_tail(&group->obj_ll);
    LV_ASSERT_MALLOC(next);
    if(next == NULL) return;
    *next = obj;

    /*Если голова и хвост равны, то в связанном списке только один объект.
     *В этом случае автоматически активируйте его*/
    if(lv_ll_get_head(&group->obj_ll) == next) {
        lv_group_refocus(group);
    }

    LV_LOG_TRACE("finished");
}

void lv_group_swap_obj(lv_obj_t * obj1, lv_obj_t * obj2)
{
    lv_group_t * g1 = lv_obj_get_group(obj1);
    lv_group_t * g2 = lv_obj_get_group(obj2);
    if(g1 != g2) return;
    if(g1 == NULL) return;

    /*Не добавляйте объект дважды*/
    lv_obj_t ** obj_i;
    LV_LL_READ(&g1->obj_ll, obj_i) {
        if((*obj_i) == obj1)(*obj_i) = obj2;
        else if((*obj_i) == obj2)(*obj_i) = obj1;
    }

    lv_obj_t * focused = lv_group_get_focused(g1);
    if(focused == obj1) lv_group_focus_obj(obj2);
    else if(focused == obj2) lv_group_focus_obj(obj1);

}

void lv_group_remove_obj(lv_obj_t * obj)
{
    lv_group_t * g = lv_obj_get_group(obj);
    if(g == NULL) return;

    LV_LOG_TRACE("begin");

    /*Сосредоточьтесь на следующем объекте*/
    if(g->obj_focus && *g->obj_focus == obj) {
        if(g->frozen) g->frozen = 0;

        /*Если это единственный объект в группе, фокусируйтесь ни на чем.*/
        if(lv_ll_get_head(&g->obj_ll) == g->obj_focus && lv_ll_get_tail(&g->obj_ll) == g->obj_focus) {
            lv_obj_send_event(*g->obj_focus, LV_EVENT_DEFOCUSED, get_indev(g));
        }
        /*Если в группе больше объектов, сосредоточьтесь на следующем/предыдущем объекте.*/
        else {
            lv_group_refocus(g);
        }
    }

    /*Если объект фокуса остался прежним, то это был единственный объект в группе, но он будет
     *быть удалены. Установите для`obj_focus`значениеNULL, чтобы вернуться в исходное состояние группы с помощью
     *ноль объектов*/
    if(g->obj_focus && *g->obj_focus == obj) {
        g->obj_focus = NULL;
    }

    /*Найдите объект и удалите его из группы.*/
    lv_obj_t ** i;
    LV_LL_READ(&g->obj_ll, i) {
        if(*i == obj) {
            lv_ll_remove(&g->obj_ll, i);
            lv_free(i);
            if(obj->spec_attr) obj->spec_attr->group_p = NULL;
            break;
        }
    }
    LV_LOG_TRACE("finished");
}

void lv_group_remove_all_objs(lv_group_t * group)
{
    LV_ASSERT_NULL(group);

    /*Расфокусировать текущий сфокусированный объект*/
    if(group->obj_focus != NULL) {
        lv_obj_send_event(*group->obj_focus, LV_EVENT_DEFOCUSED, get_indev(group));
        lv_obj_invalidate(*group->obj_focus);
        group->obj_focus = NULL;
    }

    /*Удалить объекты из группы*/
    lv_obj_t ** obj;
    LV_LL_READ(&group->obj_ll, obj) {
        if((*obj)->spec_attr)(*obj)->spec_attr->group_p = NULL;
    }

    lv_ll_clear(&(group->obj_ll));
}

void lv_group_focus_obj(lv_obj_t * obj)
{
    if(obj == NULL) return;
    lv_group_t * g = lv_obj_get_group(obj);
    if(g == NULL) return;

    if(g->frozen != 0) return;

    /*Режим редактирования при расфокусировке необходимо оставить*/
    lv_group_set_editing(g, false);

    lv_obj_t ** i;
    LV_LL_READ(&g->obj_ll, i) {
        if(*i == obj) {
            if(g->obj_focus != NULL && obj != *g->obj_focus) {  /*Не расфокусируйте, если тот же объект необходимо снова сфокусировать.*/
                lv_result_t res = lv_obj_send_event(*g->obj_focus, LV_EVENT_DEFOCUSED, get_indev(g));
                if(res != LV_RESULT_OK) return;
                lv_obj_invalidate(*g->obj_focus);
            }

            g->obj_focus = i;

            if(g->obj_focus != NULL) {
                if(g->focus_cb) g->focus_cb(g);
                lv_result_t res = lv_obj_send_event(*g->obj_focus, LV_EVENT_FOCUSED, get_indev(g));
                if(res != LV_RESULT_OK) return;
                lv_obj_invalidate(*g->obj_focus);
            }
            break;
        }
    }
}

void lv_group_focus_next(lv_group_t * group)
{
    LV_ASSERT_NULL(group);

    bool focus_changed = focus_next_core(group, lv_ll_get_head, lv_ll_get_next);
    if(group->edge_cb) {
        if(!focus_changed)
            group->edge_cb(group, true);
    }
}

void lv_group_focus_prev(lv_group_t * group)
{
    LV_ASSERT_NULL(group);

    bool focus_changed = focus_next_core(group, lv_ll_get_tail, lv_ll_get_prev);
    if(group->edge_cb) {
        if(!focus_changed)
            group->edge_cb(group, false);
    }
}

void lv_group_focus_freeze(lv_group_t * group, bool en)
{
    LV_ASSERT_NULL(group);

    if(en == false) group->frozen = 0;
    else group->frozen = 1;
}

lv_result_t lv_group_send_data(lv_group_t * group, uint32_t c)
{
    LV_ASSERT_NULL(group);

    lv_obj_t * act = lv_group_get_focused(group);
    if(act == NULL) return LV_RESULT_OK;

    if(lv_obj_has_state(act, LV_STATE_DISABLED)) return LV_RESULT_OK;

    return lv_obj_send_event(act, LV_EVENT_KEY, &c);
}

void lv_group_set_focus_cb(lv_group_t * group, lv_group_focus_cb_t focus_cb)
{
    if(group == NULL) return;

    group->focus_cb = focus_cb;
}

void lv_group_set_edge_cb(lv_group_t * group, lv_group_edge_cb_t edge_cb)
{
    LV_ASSERT_NULL(group);

    group->edge_cb = edge_cb;
}

void lv_group_set_editing(lv_group_t * group, bool edit)
{
    LV_ASSERT_NULL(group);
    uint8_t en_val = edit ? 1 : 0;

    if(en_val == group->editing) return; /*Не устанавливайте тот же режим снова*/

    group->editing     = en_val;
    lv_obj_t * focused = lv_group_get_focused(group);

    if(focused) {
        lv_result_t res = lv_obj_send_event(*group->obj_focus, LV_EVENT_FOCUSED, get_indev(group));
        if(res != LV_RESULT_OK) return;

        lv_obj_invalidate(focused);
    }
}

void lv_group_set_refocus_policy(lv_group_t * group, lv_group_refocus_policy_t policy)
{
    LV_ASSERT_NULL(group);
    group->refocus_policy = policy & 0x01;
}

void lv_group_set_wrap(lv_group_t * group, bool en)
{
    LV_ASSERT_NULL(group);
    group->wrap = en ? 1 : 0;
}

lv_obj_t * lv_group_get_focused(const lv_group_t * group)
{
    if(!group) return NULL;
    if(group->obj_focus == NULL) return NULL;

    return *group->obj_focus;
}

lv_group_focus_cb_t lv_group_get_focus_cb(const lv_group_t * group)
{
    if(!group) return NULL;
    return group->focus_cb;
}

lv_group_edge_cb_t lv_group_get_edge_cb(const lv_group_t * group)
{
    if(!group) return NULL;
    return group->edge_cb;
}

bool lv_group_get_editing(const lv_group_t * group)
{
    if(!group) return false;
    return group->editing;
}

bool lv_group_get_wrap(lv_group_t * group)
{
    if(!group) return false;
    return group->wrap;
}

uint32_t lv_group_get_obj_count(lv_group_t * group)
{
    LV_ASSERT_NULL(group);
    return lv_ll_get_len(&group->obj_ll);
}

lv_obj_t * lv_group_get_obj_by_index(lv_group_t * group, uint32_t index)
{
    uint32_t len = 0;
    lv_obj_t ** obj;

    LV_LL_READ(&group->obj_ll, obj) {
        if(len == index) {
            return *obj;
        }
        len++;
    }
    return NULL;
}

uint32_t lv_group_get_count(void)
{
    return lv_ll_get_len(group_ll_p);
}

lv_group_t  * lv_group_by_index(uint32_t index)
{
    uint32_t len = 0;
    lv_group_t * group;

    LV_LL_READ_BACK(group_ll_p, group) {
        if(len == index) {
            return group;
        }
        len++;
    }

    return NULL;
}

#if LV_USE_EXT_DATA
void lv_group_set_external_data(lv_group_t * group, void * data, void (* free_cb)(void * data))
{
    if(!group) {
        LV_LOG_WARN("Can't attach external user data and destructor callback to a NULL group");
        return;
    }

    group->ext_data.data = data;
    group->ext_data.free_cb = free_cb;
}
#endif

void lv_group_set_user_data(lv_group_t * group, void * user_data)
{
    if(group == NULL) return;
    group->user_data = user_data;
}

void * lv_group_get_user_data(const lv_group_t * group)
{
    if(group == NULL) return NULL;
    return group->user_data;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_group_refocus(lv_group_t * g)
{
    /*Перефокусировка должна временно разрешить правильную работу переноса.*/
    uint8_t temp_wrap = g->wrap;
    g->wrap           = 1;

    if(g->refocus_policy == LV_GROUP_REFOCUS_POLICY_NEXT)
        lv_group_focus_next(g);
    else if(g->refocus_policy == LV_GROUP_REFOCUS_POLICY_PREV)
        lv_group_focus_prev(g);
    /*Восстановить свойство переноса*/
    g->wrap = temp_wrap;
}

static bool focus_next_core(lv_group_t * group, void * (*begin)(const lv_ll_t *),
                            void * (*move)(const lv_ll_t *, const void *))
{
    bool focus_changed = false;
    if(group->frozen) return focus_changed;

    lv_obj_t ** obj_next     = group->obj_focus;
    lv_obj_t ** obj_sentinel = NULL;
    bool can_move            = true;
    bool can_begin           = true;

    for(;;) {
        if(obj_next == NULL) {
            if(group->wrap || obj_sentinel == NULL) {
                if(!can_begin) return focus_changed;
                obj_next  = begin(&group->obj_ll);
                can_move  = false;
                can_begin = false;
            }
            else {
                /*Объект, сфокусированный в данный момент, является последним/первым в группе, оставьте его в таком положении.*/
                return focus_changed;
            }
        }

        if(obj_sentinel == NULL) {
            obj_sentinel = obj_next;
            if(obj_sentinel == NULL) return focus_changed; /*Группа пуста*/
        }

        if(can_move) {
            obj_next = move(&group->obj_ll, obj_next);

            /*Сдавайтесь, если мы прошли весь список и не нашли ни одного видимого объекта*/
            if(obj_next == obj_sentinel) return focus_changed;
        }

        can_move = true;

        if(obj_next == NULL) continue;
        if(lv_obj_get_state(*obj_next) & LV_STATE_DISABLED) continue;

        /*Скрытые объекты не получают фокуса.
         *Если какой-либо родитель скрыт, объект также скрыт)*/
        lv_obj_t * parent = *obj_next;
        while(parent) {
            if(lv_obj_has_flag(parent, LV_OBJ_FLAG_HIDDEN)) break;
            parent = lv_obj_get_parent(parent);
        }

        if(parent && lv_obj_has_flag(parent, LV_OBJ_FLAG_HIDDEN)) continue;

        /*Если мы ее получим, будет найден хороший кандидат.*/
        break;
    }

    if(obj_next == group->obj_focus) return focus_changed; /*Виден только один объект, и он уже сфокусирован.*/

    if(group->obj_focus) {
        lv_result_t res = lv_obj_send_event(*group->obj_focus, LV_EVENT_DEFOCUSED, get_indev(group));
        if(res != LV_RESULT_OK) return focus_changed;
        lv_obj_invalidate(*group->obj_focus);
    }

    group->obj_focus = obj_next;

    lv_result_t res = lv_obj_send_event(*group->obj_focus, LV_EVENT_FOCUSED, get_indev(group));
    if(res != LV_RESULT_OK) return focus_changed;

    lv_obj_invalidate(*group->obj_focus);

    if(group->focus_cb) group->focus_cb(group);
    focus_changed = true;
    return focus_changed;
}

/**
 * Найдите indev-зависимость типаPOINTER(потому что он наиболее общий), который использует данную группу.
 * Другими словами, найдите индев, относящийся к данной группе.
 * В худшем случае просто верните последнюю версию разработки.
 * @param g     группа находка в индевс
 * @return      предлагаемый индев
 */
static lv_indev_t * get_indev(const lv_group_t * g)
{
    lv_indev_t * indev_guess = NULL;
    lv_indev_t * indev = lv_indev_get_next(NULL);

    while(indev) {
        lv_indev_type_t indev_type = lv_indev_get_type(indev);
        /*Предпочитаю POINTER*/
        if(indev_type == LV_INDEV_TYPE_POINTER) return indev;
        if(lv_indev_get_group(indev) == g) {
            indev_guess = indev;
        }
        indev = lv_indev_get_next(indev);
    }

    return indev_guess;
}
