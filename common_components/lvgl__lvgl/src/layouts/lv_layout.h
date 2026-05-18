/**
 * @file lv_layout.h
 *
 */

#ifndef LV_LAYOUT_H
#define LV_LAYOUT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef void (*lv_layout_update_cb_t)(lv_obj_t *, void * user_data);
typedef bool (*lv_layout_get_min_size_cb_t)(lv_obj_t *, int32_t * req_size, bool width, void * user_data);

typedef struct {
    lv_layout_update_cb_t layout_update_cb;
    lv_layout_get_min_size_cb_t get_min_size_cb;
} lv_layout_callbacks_t;


typedef enum {
    LV_LAYOUT_NONE = 0,

#if LV_USE_FLEX
    LV_LAYOUT_FLEX,
#endif

#if LV_USE_GRID
    LV_LAYOUT_GRID,
#endif

    LV_LAYOUT_LAST
} lv_layout_t;


/**
 * Создать новый макет
 * @param callbacks обратные вызовы макета
 * @param user_data пользовательские данные, которые будут переданы при вызове обратного вызова
 * @return          ID новой раскладки
 */
uint32_t lv_layout_create(lv_layout_callbacks_t callbacks, void * user_data);

/**
 * DEPRECATED: `lv_layout_register` устарел.  Вместо этого следует использовать `lv_layout_create`.
 *
 * Зарегистрируйте новый макет
 * @param cb        обратный вызов обновления макета
 * @param user_data пользовательские данные, которые будут переданы в `cb`
 * @return          ID новой раскладки
 */
uint32_t lv_layout_register(lv_layout_update_cb_t cb, void * user_data);

/**********************
 *      MACROS
 **********************/

#if LV_USE_FLEX
#include "flex/lv_flex.h"
#endif /* LV_USE_FLEX */

#if LV_USE_GRID
#include "grid/lv_grid.h"
#endif /* LV_USE_GRID */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LAYOUT_H*/
