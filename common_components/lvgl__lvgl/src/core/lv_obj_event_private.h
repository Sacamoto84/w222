/**
 * @file lv_obj_event_private.h
 *
 */

#ifndef LV_OBJ_EVENT_PRIVATE_H
#define LV_OBJ_EVENT_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_obj_event.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Используется в качестве параметра событий ::LV_EVENT_HIT_TEST, чтобы проверить, может ли`point`щелкнуть объект или нет.
 * `res` должен быть установлен следующим образом:
 *   - Если уже установлено значение `false`, другое событие не хочет, чтобы эта точка была доступна для кликов. Если вы хотите соблюдать его, оставьте его как`false`или установите `true`, чтобы перезаписать его.
 *   - Если они уже установлены,`true`и`point`не должны быть кликабельными, установите `false`.
 *   - Если уже установлено значение `true`, вы соглашаетесь с тем, что`point`может щелкнуть объект, оставьте его как `true`.
 */
struct _lv_hit_test_info_t {
    const lv_point_t * point;   /**< A point relative to screen to check if it can click the object or not*/
    bool res;                   /**< true: `point` can click the object; false: it cannot*/
};

/**
 * Используется в качестве параметра события :: LV_EVENT_COVER_CHECK для проверки того, покрыта ли область объектом или нет.
 * В этом случае воспользуйтесь`const lv_area_t * area = lv_event_get_cover_area(e)`, чтобы проверить регион.
 * и`lv_event_set_cover_res(e, res)`, чтобы установить результат.
 */
struct _lv_cover_check_info_t {
    lv_cover_res_t res;
    const lv_area_t * area;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_OBJ_EVENT_PRIVATE_H*/
