/**
 * @file lv_roller.h
 *
 */

#ifndef LV_ROLLER_H
#define LV_ROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj.h"

#if LV_USE_ROLLER != 0

/*Тестирование зависимостей*/
#if LV_USE_LABEL == 0
#error "lv_roller: lv_label is required. Enable it in lv_conf.h (LV_USE_ROLLER 1)"
#endif

#include "../label/lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Роликовый режим. */
typedef enum {
    LV_ROLLER_MODE_NORMAL,   /**< Normal mode (roller ends at the end of the options). */
    LV_ROLLER_MODE_INFINITE, /**< Infinite mode (roller can be scrolled forever). */
} lv_roller_mode_t;

#if LV_USE_OBJ_PROPERTY
enum _lv_property_roller_id_t {
    LV_PROPERTY_ID2(ROLLER, OPTIONS,            LV_PROPERTY_TYPE_TEXT,  LV_PROPERTY_TYPE_INT,   0),
    LV_PROPERTY_ID2(ROLLER, SELECTED,           LV_PROPERTY_TYPE_INT,   LV_PROPERTY_TYPE_INT, 1),
    LV_PROPERTY_ID(ROLLER, VISIBLE_ROW_COUNT,   LV_PROPERTY_TYPE_INT,   2),
    LV_PROPERTY_ROLLER_END,
};
#endif

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_roller_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте объект ролика
 * @param parent    указатель на объект, он будет родительским для нового ролика.
 * @return          указатель на созданный ролик
 */
lv_obj_t * lv_roller_create(lv_obj_t * parent);

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите параметры на ролике
 * @param obj       указатель на объект ролика
 * @param options   строка с опциями, разделенными '\n'. Например. «Один\nДва\nТри»
 * @param mode      `LV_ROLLER_MODE_NORMAL` или `LV_ROLLER_MODE_INFINITE`
 */
void lv_roller_set_options(lv_obj_t * obj, const char * options, lv_roller_mode_t mode);

/**
 * Установить выбранный вариант
 * @param obj       указатель на объект ролика
 * @param sel_opt   индекс выбранной опции (0...номер опции - 1);
 * @param anim   LV_ANIM_ON: набор с анимацией; LV_ANIM_OFFустанавливается сразу
 */
void lv_roller_set_selected(lv_obj_t * obj, uint32_t sel_opt, lv_anim_enable_t anim);

/**
 * Устанавливает данную строку в качестве выделенной на ролике. Не изменяет текущий выбор в случае сбоя.
 * @param obj               указатель на объект ролика
 * @param sel_opt   указатель на строку, которую вы хотите установить в качестве опции
 * @param anim          LV_ANIM_ON: набор с анимацией; LV_ANIM_OFFустанавливается сразу
 * @return                  `true`, если установлена успешно, и `false`, если данная строка не существует в качестве опции в ролике.
 */
bool lv_roller_set_selected_str(lv_obj_t * obj, const char * sel_opt, lv_anim_enable_t anim);

/**
 * Установите высоту для отображения заданного количества строк (варианты)
 * @param obj       указатель на объект ролика
 * @param row_cnt   количество желаемых видимых строк
 */
void lv_roller_set_visible_row_count(lv_obj_t * obj, uint32_t row_cnt);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получить индекс выбранного варианта
 * @param obj       указатель на объект ролика
 * @return          индекс выбранной опции (0...номер опции - 1);
 */
uint32_t lv_roller_get_selected(const lv_obj_t * obj);

/**
 * Получить текущий выбранный вариант в виде строки.
 * @param obj       указатель на объект ролика
 * @param buf       указатель на массив для хранения строки
 * @param buf_size  размер`buf`в байтах. 0: игнорировать это.
 */
void lv_roller_get_selected_str(const lv_obj_t * obj, char * buf, uint32_t buf_size);

/**
 * Получить варианты ролика
 * @param obj       указатель на объект ролика
 * @return          параметры, разделенные символом '\n'-s (например, «Option1\nOption2\nOption3»)
 */
const char * lv_roller_get_options(const lv_obj_t * obj);

/**
 * Получить общее количество вариантов
 * @param obj   указатель на объект ролика
 * @return      общее количество вариантов
 */
uint32_t lv_roller_get_option_count(const lv_obj_t * obj);

/**
 * Получите опцию в виде строки.
 * @param obj       указатель на объект ролика
 * @param option    индекс выбранного варианта
 * @param buf       указатель на массив для хранения строки
 * @param buf_size  размер`buf`в байтах. 0: игнорировать это.
 * @return          LV_RESULT_OK, если вариант найден
 */
lv_result_t lv_roller_get_option_str(const lv_obj_t * obj, uint32_t option, char * buf, uint32_t buf_size);

#if LV_USE_OBSERVER
/**
 * Привяжите целое число к значению ролика.
 * @param obj       указатель на ролик
 * @param subject   указатель на тему
 * @return          указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_roller_bind_value(lv_obj_t * obj, lv_subject_t * subject);
#endif

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_ROLLER*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_ROLLER_H*/
