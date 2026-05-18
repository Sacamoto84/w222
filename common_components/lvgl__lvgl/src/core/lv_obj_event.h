/**
 * @file lv_obj_event.h
 *
 */

#ifndef LV_OBJ_EVENT_H
#define LV_OBJ_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_types.h"
#include "../misc/lv_event.h"
#include "../indev/lv_indev.h"
#include "lv_obj_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Результаты проверки покрытия.*/
typedef enum {
    LV_COVER_RES_COVER      = 0,
    LV_COVER_RES_NOT_COVER  = 1,
    LV_COVER_RES_MASKED     = 2,
} lv_cover_res_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Отправить событие на объект
 * @param obj           указатель на объект
 * @param event_code    тип события из `lv_event_t`
 * @param param         произвольные данные в зависимости от типа виджета и события. (Обычно `NULL`)
 * @return LV_RESULT_OK :`obj`не был удален в событии;  LV_RESULT_INVALID:`obj`был удален в event_code.
 */
lv_result_t lv_obj_send_event(lv_obj_t * obj, lv_event_code_t event_code, void * param);

/**
 * Используется виджетами внутри для вызова обработчика событий родительских типов виджетов.
 * @param class_p   указатель на класс виджета (NOTкласс-предок)
 * @param e         указатель на дескриптор события
 * @return          LV_RESULT_OK: целевой объект не был удален в событии;  LV_RESULT_INVALID: он был удален в event_code.
 */
lv_result_t lv_obj_event_base(const lv_obj_class_t * class_p, lv_event_t * e);

/**
 * Получите текущую цель события. Это объект, обработчик которого вызывается.
 * Если событие не всплывает, оно совпадает с «исходной» целью.
 * @param e     указатель на дескриптор события
 * @return      цель event_code
 */
lv_obj_t * lv_event_get_current_target_obj(lv_event_t * e);

/**
 * Получите объект, на который изначально нацелено событие. То же самое, даже если событие всплывает.
 * @param e     указатель на дескриптор события
 * @return      указатель на исходную цель event_code
 */
lv_obj_t * lv_event_get_target_obj(lv_event_t * e);

/**
 * Добавьте функцию обработчика событий для объекта.
 * Используется пользователем для реагирования на событие, происходящее с объектом.
 * Объект может иметь несколько обработчиков событий. Они будут вызываться в том же порядке, в котором были добавлены.
 * @param obj       указатель на объект
 * @param filter    код события (например, `LV_EVENT_CLICKED`), для которого должно быть вызвано событие. `LV_EVENT_ALL`можно использовать для получения всех событий.
 * @param event_cb  новая функция событий
 * @param           user_data пользовательские данные будут доступны в `event_cb`
 * @return          обработчик события. Его можно использовать в `lv_obj_remove_event_dsc`.
 */
lv_event_dsc_t * lv_obj_add_event_cb(lv_obj_t * obj, lv_event_cb_t event_cb, lv_event_code_t filter, void * user_data);

uint32_t lv_obj_get_event_count(lv_obj_t * obj);

lv_event_dsc_t * lv_obj_get_event_dsc(lv_obj_t * obj, uint32_t index);

bool lv_obj_remove_event(lv_obj_t * obj, uint32_t index);

bool lv_obj_remove_event_dsc(lv_obj_t * obj, lv_event_dsc_t * dsc);

/**
 * Удалитьevent_cbиз объекта
 * @param obj           указатель на объект
 * @param event_cb      event_cb события, которое нужно удалить
 * @return              количество событий удалено
 */
uint32_t lv_obj_remove_event_cb(lv_obj_t * obj, lv_event_cb_t event_cb);

/**
 * Удалитеevent_cbс помощью user_data.
 * @param obj           указатель на объект
 * @param event_cb      event_cb события, которое нужно удалить
 * @param user_data user_data
 * @return              количество событий удалено
 */
uint32_t lv_obj_remove_event_cb_with_user_data(lv_obj_t * obj, lv_event_cb_t event_cb, void * user_data);

/**
 * Получите устройство ввода, переданное в качестве параметра для событий, связанных с разработкой.
 * @param e     указатель на событие
 * @return      индев, вызвавший событие, или NULL, если вызывается событие, не связанное с индевом
 */
lv_indev_t * lv_event_get_indev(lv_event_t * e);

/**
 * Получите контекст рисования, который должен быть первым параметром функций рисования.
 * А именно: `LV_EVENT_DRAW_MAIN/POST`, `LV_EVENT_DRAW_MAIN/POST_BEGIN`, `LV_EVENT_DRAW_MAIN/POST_END`
 * @param e     указатель на событие
 * @return      указатель на контекст отрисовки или NULL, если вызывается несвязанное событие
 */
lv_layer_t * lv_event_get_layer(lv_event_t * e);

/**
 * Получите старую площадь объекта до изменения его размера. Может использоваться в `LV_EVENT_SIZE_CHANGED`.
 * @param e     указатель на событие
 * @return      старая абсолютная площадь объекта или NULL, если вызывается несвязанное событие
 */
const lv_area_t * lv_event_get_old_size(lv_event_t * e);

/**
 * Получите ключ, переданный в качестве параметра событию. Может использоваться в `LV_EVENT_KEY`.
 * @param e     указатель на событие
 * @return      триггерная клавиша или NULL, если вызывается несвязанное событие
 */
uint32_t lv_event_get_key(lv_event_t * e);

/**
 * Получите подписанный дифф поворотного энкодера. передается в качестве параметра событию. Может использоваться в `LV_EVENT_ROTARY`.
 * @param e     указатель на событие
 * @return      триггерная клавиша или NULL, если вызывается несвязанное событие
 */
int32_t lv_event_get_rotary_diff(lv_event_t * e);

/**
 * Получите дескриптор анимации прокрутки. Может использоваться в `LV_EVENT_SCROLL_BEGIN`.
 * @param e     указатель на событие
 * @return      анимация, которая будет прокручивать объект. (можно изменить по желанию)
 */
lv_anim_t * lv_event_get_scroll_anim(lv_event_t * e);

/**
 * Установите новый дополнительный размер рисунка. Может использоваться в `LV_EVENT_REFR_EXT_DRAW_SIZE`.
 * @param e     указатель на событие
 * @param size  Новый дополнительный размер розыгрыша
 */
void lv_event_set_ext_draw_size(lv_event_t * e, int32_t size);

/**
 * Получите указатель на переменную`lv_point_t`, в которой должна быть сохранена небольшая величина (ширина в`point->x`и высота`point->y`).
 * Может использоваться в `LV_EVENT_GET_SELF_SIZE`.
 * @param e     указатель на событие
 * @return      указатель на`lv_point_t`или NULL, если вызывается несвязанное событие
 */
lv_point_t * lv_event_get_self_size_info(lv_event_t * e);

/**
 * Получите указатель на переменную`lv_hit_test_info_t`, в котором должен быть сохранен результат проверки проверки. Может использоваться в `LV_EVENT_HIT_TEST`.
 * @param e     указатель на событие
 * @return      указатель на`lv_hit_test_info_t`или NULL, если вызывается несвязанное событие
 */
lv_hit_test_info_t * lv_event_get_hit_test_info(lv_event_t * e);

/**
 * Получите указатель на область, которую необходимо проверить, полностью ее закрывает объект или нет.
 * Может использоваться в `LV_EVENT_HIT_TEST`.
 * @param e     указатель на событие
 * @return      область с абсолютными координатами для проверки
 */
const lv_area_t * lv_event_get_cover_area(lv_event_t * e);

/**
 * Установите результат проверки покрытия. Может использоваться в `LV_EVENT_COVER_CHECK`.
 * @param e     указатель на событие
 * @param res   элемент :: lv_cover_check_info_t
 */
void lv_event_set_cover_res(lv_event_t * e, lv_cover_res_t res);

/**
 * Получите только что добавленную задачу рисования.
 * Может использоваться в `LV_EVENT_DRAW_TASK_ADDED event`.
 * @param e     указатель на событие
 * @return      добавлено задание на рисование
 */
lv_draw_task_t * lv_event_get_draw_task(lv_event_t * e);

/**
 * Получите предыдущее состояние до изменения состояния.
 * Может использоваться в событии `LV_EVENT_STATE_CHANGED`.
 * @param e     указатель на событие
 * @return      предыдущее состояние
 */
lv_state_t lv_event_get_prev_state(lv_event_t * e);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_OBJ_EVENT_H*/
