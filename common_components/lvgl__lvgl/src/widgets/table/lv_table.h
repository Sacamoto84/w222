/**
 * @file lv_table.h
 *
 */

#ifndef LV_TABLE_H
#define LV_TABLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../label/lv_label.h"
#include "../../core/lv_obj_property.h"

#if LV_USE_TABLE != 0

/*Тестирование зависимостей*/
#if LV_USE_LABEL == 0
#error "lv_table: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL 1)"
#endif

/*********************
 *      DEFINES
 *********************/
#define LV_TABLE_CELL_NONE 0XFFFF
LV_EXPORT_CONST_INT(LV_TABLE_CELL_NONE);

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_TABLE_CELL_CTRL_NONE        = 0 << 0,
    LV_TABLE_CELL_CTRL_MERGE_RIGHT = 1 << 0,
    LV_TABLE_CELL_CTRL_TEXT_CROP   = 1 << 1,
    LV_TABLE_CELL_CTRL_CUSTOM_1    = 1 << 4,
    LV_TABLE_CELL_CTRL_CUSTOM_2    = 1 << 5,
    LV_TABLE_CELL_CTRL_CUSTOM_3    = 1 << 6,
    LV_TABLE_CELL_CTRL_CUSTOM_4    = 1 << 7,
} lv_table_cell_ctrl_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_table_class;

#if LV_USE_OBJ_PROPERTY
enum _lv_property_table_id_t {
    LV_PROPERTY_ID(TABLE, ROW_COUNT,    LV_PROPERTY_TYPE_INT, 0),
    LV_PROPERTY_ID(TABLE, COLUMN_COUNT, LV_PROPERTY_TYPE_INT, 1),
    LV_PROPERTY_TABLE_END,
};
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создать объект таблицы
 * @param parent        pointer to an object, it will be the parent of the new table
 * @return              pointer to the created table
 */
lv_obj_t * lv_table_create(lv_obj_t * parent);

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите значение ячейки.
 * @param obj           pointer to a Table object
 * @param row           id of the row [0 .. row_cnt -1]
 * @param col           id of the column [0 .. col_cnt -1]
 * @param txt           text to display in the cell. It will be copied and saved so this variable is not required after this function call.
 * @note                New roes/columns are added automatically if required
 */
void lv_table_set_cell_value(lv_obj_t * obj, uint32_t row, uint32_t col, const char * txt);

/**
 * Установите значение ячейки.  Для хранения текста по таблице будет выделена память.
 * @param obj           pointer to a Table object
 * @param row           id of the row [0 .. row_cnt -1]
 * @param col           id of the column [0 .. col_cnt -1]
 * @param fmt           `printf`-like format
 * @note                New roes/columns are added automatically if required
 */
void lv_table_set_cell_value_fmt(lv_obj_t * obj, uint32_t row, uint32_t col, const char * fmt,
                                 ...) LV_FORMAT_ATTRIBUTE(4, 5);

/**
 * Установите количество строк
 * @param obj           table pointer to a Table object
 * @param row_cnt       number of rows
 */
void lv_table_set_row_count(lv_obj_t * obj, uint32_t row_cnt);

/**
 * Установите количество столбцов
 * @param obj       table pointer to a Table object
 * @param col_cnt   number of columns.
 */
void lv_table_set_column_count(lv_obj_t * obj, uint32_t col_cnt);

/**
 * Установить ширину столбца
 * @param obj       table pointer to a Table object
 * @param col_id    id of the column [0 .. LV_TABLE_COL_MAX -1]
 * @param w         width of the column
 */
void lv_table_set_column_width(lv_obj_t * obj, uint32_t col_id, int32_t w);

/**
 * Добавьте управляющие биты в ячейку.
 * @param obj       pointer to a Table object
 * @param row       id of the row [0 .. row_cnt -1]
 * @param col       id of the column [0 .. col_cnt -1]
 * @param ctrl      OR-ed values from ::lv_table_cell_ctrl_t
 */
void lv_table_set_cell_ctrl(lv_obj_t * obj, uint32_t row, uint32_t col, lv_table_cell_ctrl_t ctrl);

/**
 * Очистить управляющие биты ячейки.
 * @param obj       pointer to a Table object
 * @param row       id of the row [0 .. row_cnt -1]
 * @param col       id of the column [0 .. col_cnt -1]
 * @param ctrl      OR-ed values from ::lv_table_cell_ctrl_t
 */
void lv_table_clear_cell_ctrl(lv_obj_t * obj, uint32_t row, uint32_t col, lv_table_cell_ctrl_t ctrl);

/**
 * Добавьте в ячейку пользовательские данные.
 * @param obj       pointer to a Table object
 * @param row       id of the row [0 .. row_cnt -1]
 * @param col       id of the column [0 .. col_cnt -1]
 * @param user_data pointer to the new user_data.
 *                  Должно быть выделено `lv_malloc` ,
 *                  и он будет освобожден автоматически при удалении таблицы или
 *                  когда ячейка удаляется из-за меньшего количества строк или столбцов.
 */
void lv_table_set_cell_user_data(lv_obj_t * obj, uint16_t row, uint16_t col, void * user_data);

/**
 * Установить выбранную ячейку
 * @param obj       pointer to a table object
 * @param row       id of the cell row to select
 * @param col       id of the cell column to select
 */
void lv_table_set_selected_cell(lv_obj_t * obj, uint16_t row, uint16_t col);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получить значение ячейки.
 * @param obj       pointer to a Table object
 * @param row       id of the row [0 .. row_cnt -1]
 * @param col       id of the column [0 .. col_cnt -1]
 * @return          text in the cell
 */
const char * lv_table_get_cell_value(lv_obj_t * obj, uint32_t row, uint32_t col);

/**
 * Получите количество строк.
 * @param obj       table pointer to a Table object
 * @return          number of rows.
 */
uint32_t lv_table_get_row_count(lv_obj_t * obj);

/**
 * Получите количество столбцов.
 * @param obj       table pointer to a Table object
 * @return          number of columns.
 */
uint32_t lv_table_get_column_count(lv_obj_t * obj);

/**
 * Получить ширину столбца
 * @param obj       table pointer to a Table object
 * @param col       id of the column [0 .. LV_TABLE_COL_MAX -1]
 * @return          width of the column
 */
int32_t lv_table_get_column_width(lv_obj_t * obj, uint32_t col);

/**
 * Узнайте, есть ли в ячейке управляющие биты
 * @param obj       pointer to a Table object
 * @param row       id of the row [0 .. row_cnt -1]
 * @param col       id of the column [0 .. col_cnt -1]
 * @param ctrl      OR-ed values from ::lv_table_cell_ctrl_t
 * @return          true: all control bits are set; false: not all control bits are set
 */
bool lv_table_has_cell_ctrl(lv_obj_t * obj, uint32_t row, uint32_t col, lv_table_cell_ctrl_t ctrl);

/**
 * Получить выбранную ячейку (нажатую и/или сфокусированную)
 * @param obj       pointer to a table object
 * @param row       pointer to variable to store the selected row (LV_TABLE_CELL_NONE: if no cell selected)
 * @param col       pointer to variable to store the selected column  (LV_TABLE_CELL_NONE: if no cell selected)
 */
void lv_table_get_selected_cell(lv_obj_t * obj, uint32_t * row, uint32_t * col);

/**
 * Получите пользовательские данные в ячейку.
 * @param obj       pointer to a Table object
 * @param row       id of the row [0 .. row_cnt -1]
 * @param col       id of the column [0 .. col_cnt -1]
 */
void * lv_table_get_cell_user_data(lv_obj_t * obj, uint16_t row, uint16_t col);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TABLE*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_TABLE_H*/
