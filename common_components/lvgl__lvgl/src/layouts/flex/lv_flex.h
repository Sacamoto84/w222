/**
 * @file lv_flex.h
 *
 */

#ifndef LV_FLEX_H
#define LV_FLEX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#include "../../misc/lv_area.h"

#if LV_USE_FLEX

/*********************
 *      DEFINES
 *********************/

#define LV_FLEX_COLUMN        (1 << 0)
#define LV_FLEX_WRAP       (1 << 2)
#define LV_FLEX_REVERSE    (1 << 3)

/**********************
 *      TYPEDEFS
 **********************/

/*Невозможно включитьlv_obj.h, так как он включает этот заголовочный файл.*/

typedef enum {
    LV_FLEX_ALIGN_START,
    LV_FLEX_ALIGN_END,
    LV_FLEX_ALIGN_CENTER,
    LV_FLEX_ALIGN_SPACE_EVENLY,
    LV_FLEX_ALIGN_SPACE_AROUND,
    LV_FLEX_ALIGN_SPACE_BETWEEN,
} lv_flex_align_t;

typedef enum {
    LV_FLEX_FLOW_ROW                 = 0x00,
    LV_FLEX_FLOW_COLUMN              = LV_FLEX_COLUMN,
    LV_FLEX_FLOW_ROW_WRAP            = LV_FLEX_FLOW_ROW | LV_FLEX_WRAP,
    LV_FLEX_FLOW_ROW_REVERSE         = LV_FLEX_FLOW_ROW | LV_FLEX_REVERSE,
    LV_FLEX_FLOW_ROW_WRAP_REVERSE    = LV_FLEX_FLOW_ROW | LV_FLEX_WRAP | LV_FLEX_REVERSE,
    LV_FLEX_FLOW_COLUMN_WRAP         = LV_FLEX_FLOW_COLUMN | LV_FLEX_WRAP,
    LV_FLEX_FLOW_COLUMN_REVERSE      = LV_FLEX_FLOW_COLUMN | LV_FLEX_REVERSE,
    LV_FLEX_FLOW_COLUMN_WRAP_REVERSE = LV_FLEX_FLOW_COLUMN | LV_FLEX_WRAP | LV_FLEX_REVERSE,
} lv_flex_flow_t;

/**********************
 * GLOBAL VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализировать гибкий макет со значениями по умолчанию
 */
void lv_flex_init(void);

/**
 * Установите, как элемент должен течь
 * @param obj указатель на объект. Родительский элемент должен иметь гибкий макет, иначе ничего не произойдет.
 * @param flow элемент `lv_flex_flow_t`.
 */
void lv_obj_set_flex_flow(lv_obj_t * obj, lv_flex_flow_t flow);

/**
 * Установите, как размещать (где выравнивать) элементы и дорожки.
 * @param obj указатель на объект. Родительский элемент должен иметь гибкий макет, иначе ничего не произойдет.
 * @param main_place где разместить элементы на главной оси (по их дорожке). Любое значение `lv_flex_align_t`.
 * @param cross_place где разместить элемент на их дорожке по поперечной оси.  `LV_FLEX_ALIGN_START/END/CENTER`
 * @param track_cross_place где разместить дорожки в поперечном направлении. Любое значение `lv_flex_align_t`.
 */
void lv_obj_set_flex_align(lv_obj_t * obj, lv_flex_align_t main_place, lv_flex_align_t cross_place,
                           lv_flex_align_t track_cross_place);

/**
 * Устанавливает ширину или высоту (по главной оси) для увеличения объекта, чтобы заполнить свободное пространство.
 * @param obj указатель на объект. Родительский элемент должен иметь гибкий макет, иначе ничего не произойдет.
 * @param grow значение, определяющее, сколько свободного места нужно занять пропорционально другим растущим объектам.
 */
void lv_obj_set_flex_grow(lv_obj_t * obj, uint8_t grow);

/**********************
 *      MACROS
 **********************/

#endif  /*LV_USE_FLEX*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_FLEX_H*/
