/**
 * @file lv_grid.h
 *
 */

#ifndef LV_GRID_H
#define LV_GRID_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#include "../../misc/lv_area.h"

#if LV_USE_GRID

/*********************
 *      DEFINES
 *********************/
/**
 * Можно использовать размер дорожки, чтобы дорожка заполняла свободное пространство.
 * @param x сколько места занять пропорционально другим трекам FR
 * @return специальный размер дорожки
 */
#define LV_GRID_FR(x)          (LV_COORD_MAX - 100 + x)

#define LV_GRID_CONTENT        (LV_COORD_MAX - 101)
LV_EXPORT_CONST_INT(LV_GRID_CONTENT);

#define LV_GRID_TEMPLATE_LAST  (LV_COORD_MAX)
LV_EXPORT_CONST_INT(LV_GRID_TEMPLATE_LAST);

/**********************
 *      TYPEDEFS
 **********************/

/*Невозможно включитьlv_obj.h, так как он включает этот заголовочный файл.*/

typedef enum {
    LV_GRID_ALIGN_START,
    LV_GRID_ALIGN_CENTER,
    LV_GRID_ALIGN_END,
    LV_GRID_ALIGN_STRETCH,
    LV_GRID_ALIGN_SPACE_EVENLY,
    LV_GRID_ALIGN_SPACE_AROUND,
    LV_GRID_ALIGN_SPACE_BETWEEN,
} lv_grid_align_t;

/**********************
 * GLOBAL VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_grid_init(void);

void lv_obj_set_grid_dsc_array(lv_obj_t * obj, const int32_t col_dsc[], const int32_t row_dsc[]);

void lv_obj_set_grid_align(lv_obj_t * obj, lv_grid_align_t column_align, lv_grid_align_t row_align);

/**
 * Установите ячейку объекта. Родитель объекта должен иметь сетку, иначе ничего не произойдет.
 * @param obj указатель на объект
 * @param column_align вертикальное выравнивание в ячейке.  `LV_GRID_START/END/CENTER/STRETCH`
 * @param col_pos столбец ID
 * @param col_span количество столбцов, которые нужно взять (>= 1)
 * @param row_align горизонтальное выравнивание в ячейке.  `LV_GRID_START/END/CENTER/STRETCH`
 * @param row_pos строка ID
 * @param row_span количество строк (>= 1)
 */
void lv_obj_set_grid_cell(lv_obj_t * obj, lv_grid_align_t column_align, int32_t col_pos, int32_t col_span,
                          lv_grid_align_t row_align, int32_t row_pos, int32_t row_span);

/**
 * Просто обертка для `LV_GRID_FR` для привязок.
 */
int32_t lv_grid_fr(uint8_t x);

/**********************
 * GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GRID*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_GRID_H*/
