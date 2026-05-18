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
 * @param parent        указатель на объект, он будет родителем новой таблицы
 * @return              указатель на созданную таблицу
 */
lv_obj_t * lv_table_create(lv_obj_t * parent);

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите значение ячейки.
 * @param obj           указатель на объект таблицы
 * @param row           идентификатор строки [0 ..row_cnt-1]
 * @param col           идентификатор столбца [0 ..col_cnt-1]
 * @param txt           текст, который будет отображаться в ячейке. Она будет скопирована и сохранена, поэтому эта переменная не требуется после вызова этой функции.
 * @note                При необходимости новые строки/столбцы добавляются автоматически.
 */
void lv_table_set_cell_value(lv_obj_t * obj, uint32_t row, uint32_t col, const char * txt);

/**
 * Установите значение ячейки.  Для хранения текста по таблице будет выделена память.
 * @param obj           указатель на объект таблицы
 * @param row           идентификатор строки [0 ..row_cnt-1]
 * @param col           идентификатор столбца [0 ..col_cnt-1]
 * @param fmt           `printf`-подобный формат
 * @note                При необходимости новые строки/столбцы добавляются автоматически.
 */
void lv_table_set_cell_value_fmt(lv_obj_t * obj, uint32_t row, uint32_t col, const char * fmt,
                                 ...) LV_FORMAT_ATTRIBUTE(4, 5);

/**
 * Установите количество строк
 * @param obj           указатель таблицы на объект Table
 * @param row_cnt       количество строк
 */
void lv_table_set_row_count(lv_obj_t * obj, uint32_t row_cnt);

/**
 * Установите количество столбцов
 * @param obj       указатель таблицы на объект Table
 * @param col_cnt   количество столбцов.
 */
void lv_table_set_column_count(lv_obj_t * obj, uint32_t col_cnt);

/**
 * Установить ширину столбца
 * @param obj       указатель таблицы на объект Table
 * @param col_id    идентификатор столбца [0 ..LV_TABLE_COL_MAX-1]
 * @param w         ширина столбца
 */
void lv_table_set_column_width(lv_obj_t * obj, uint32_t col_id, int32_t w);

/**
 * Добавьте управляющие биты в ячейку.
 * @param obj       указатель на объект таблицы
 * @param row       идентификатор строки [0 ..row_cnt-1]
 * @param col       идентификатор столбца [0 ..col_cnt-1]
 * @param ctrl      OR -ed значения из :: lv_table_cell_ctrl_t
 */
void lv_table_set_cell_ctrl(lv_obj_t * obj, uint32_t row, uint32_t col, lv_table_cell_ctrl_t ctrl);

/**
 * Очистить управляющие биты ячейки.
 * @param obj       указатель на объект таблицы
 * @param row       идентификатор строки [0 ..row_cnt-1]
 * @param col       идентификатор столбца [0 ..col_cnt-1]
 * @param ctrl      OR -ed значения из :: lv_table_cell_ctrl_t
 */
void lv_table_clear_cell_ctrl(lv_obj_t * obj, uint32_t row, uint32_t col, lv_table_cell_ctrl_t ctrl);

/**
 * Добавьте в ячейку пользовательские данные.
 * @param obj       указатель на объект таблицы
 * @param row       идентификатор строки [0 ..row_cnt-1]
 * @param col       идентификатор столбца [0 ..col_cnt-1]
 * @param user_data указатель на новый user_data.
 *                  Должность быть выделено`lv_malloc`,
 *                  и он будет освобожден автоматически при удалении таблицы или
 *                  когда ячейка удаляется из-за меньшего количества строк или столбцов.
 */
void lv_table_set_cell_user_data(lv_obj_t * obj, uint16_t row, uint16_t col, void * user_data);

/**
 * Установить выбранную ячейку
 * @param obj       указатель на объект таблицы
 * @param row       идентификатор строки ячейки, которую нужно выбрать
 * @param col       идентификатор столбца ячейки, который нужно выбрать
 */
void lv_table_set_selected_cell(lv_obj_t * obj, uint16_t row, uint16_t col);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получить значение ячейки.
 * @param obj       указатель на объект таблицы
 * @param row       идентификатор строки [0 ..row_cnt-1]
 * @param col       идентификатор столбца [0 ..col_cnt-1]
 * @return          текст в ячейке
 */
const char * lv_table_get_cell_value(lv_obj_t * obj, uint32_t row, uint32_t col);

/**
 * Получите количество строк.
 * @param obj       указатель таблицы на объект Table
 * @return          количество рядов.
 */
uint32_t lv_table_get_row_count(lv_obj_t * obj);

/**
 * Получите количество столбцов.
 * @param obj       указатель таблицы на объект Table
 * @return          количество столбцов.
 */
uint32_t lv_table_get_column_count(lv_obj_t * obj);

/**
 * Получить ширину столбца
 * @param obj       указатель таблицы на объект Table
 * @param col       идентификатор столбца [0 ..LV_TABLE_COL_MAX-1]
 * @return          ширина столбца
 */
int32_t lv_table_get_column_width(lv_obj_t * obj, uint32_t col);

/**
 * Узнайте, есть ли в ячейке управляющие биты
 * @param obj       указатель на объект таблицы
 * @param row       идентификатор строки [0 ..row_cnt-1]
 * @param col       идентификатор столбца [0 ..col_cnt-1]
 * @param ctrl      OR -ed значения из :: lv_table_cell_ctrl_t
 * @return          true: все управляющие биты установлены; false: не все биты управления установлены
 */
bool lv_table_has_cell_ctrl(lv_obj_t * obj, uint32_t row, uint32_t col, lv_table_cell_ctrl_t ctrl);

/**
 * Получить выбранную ячейку (нажатую и/или сфокусированную)
 * @param obj       указатель на объект таблицы
 * @param row       указатель на переменную для хранения выбранной строки (LV_TABLE_CELL_NONE: если ячейка не выбрана)
 * @param col       указатель на переменную для хранения выбранного столбца (LV_TABLE_CELL_NONE: если ячейка не выбрана)
 */
void lv_table_get_selected_cell(lv_obj_t * obj, uint32_t * row, uint32_t * col);

/**
 * Получите пользовательские данные в ячейку.
 * @param obj       указатель на объект таблицы
 * @param row       идентификатор строки [0 ..row_cnt-1]
 * @param col       идентификатор столбца [0 ..col_cnt-1]
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
