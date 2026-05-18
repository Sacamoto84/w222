/**
 * @file lv_spinbox.h
 *
 */

#ifndef LV_SPINBOX_H
#define LV_SPINBOX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../textarea/lv_textarea.h"

#if LV_USE_SPINBOX

/*Тестирование зависимостей*/
#if LV_USE_TEXTAREA == 0
#error "lv_spinbox: lv_ta is required. Enable it in lv_conf.h (LV_USE_TEXTAREA  1) "
#endif

/*********************
 *      DEFINES
 *********************/
#define LV_SPINBOX_MAX_DIGIT_COUNT 10

/**********************
 *      TYPEDEFS
 **********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_spinbox_class;

#if LV_USE_OBJ_PROPERTY
enum _lv_property_spinbox_id_t {
    LV_PROPERTY_ID(SPINBOX, VALUE,                LV_PROPERTY_TYPE_INT, 0),
    LV_PROPERTY_ID(SPINBOX, ROLLOVER,             LV_PROPERTY_TYPE_BOOL, 1),
    LV_PROPERTY_ID(SPINBOX, DIGIT_COUNT,          LV_PROPERTY_TYPE_INT, 2),
    LV_PROPERTY_ID(SPINBOX, DEC_POINT_POS,        LV_PROPERTY_TYPE_INT, 3),
    LV_PROPERTY_ID(SPINBOX, STEP,                 LV_PROPERTY_TYPE_INT, 4),
    LV_PROPERTY_ID(SPINBOX, MIN_VALUE,            LV_PROPERTY_TYPE_INT, 5),
    LV_PROPERTY_ID(SPINBOX, MAX_VALUE,            LV_PROPERTY_TYPE_INT, 6),
    LV_PROPERTY_ID(SPINBOX, DIGIT_STEP_DIRECTION, LV_PROPERTY_TYPE_INT, 7),
    LV_PROPERTY_SPINBOX_END,
};
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте объект счетчика
 * @param parent    указатель на объект, он будет родителем нового счетчика
 * @return          указатель на созданный счетчик
 */
lv_obj_t * lv_spinbox_create(lv_obj_t * parent);

/*=====================
 * Функции установки
 *====================*/

/**
 * Установить значение счетчика
 * @param obj   указатель на спинбокс
 * @param v     значение, которое нужно установить
 */
void lv_spinbox_set_value(lv_obj_t * obj, int32_t v);

/**
 * Установить функцию опрокидывания счетчика
 * @param obj       указатель на спинбокс
 * @param rollover  true или false для включения или отключения (по умолчанию)
 */
void lv_spinbox_set_rollover(lv_obj_t * obj, bool rollover);

/**
 * Установить формат цифр счетчика (количество цифр и десятичный формат)
 * @param obj           указатель на спинбокс
 * @param digit_count   количество цифр без учета десятичного разделителя и знака
 * @param sep_pos       количество цифр до десятичной точки. Если 0, десятичная точка не является
 * показано
 */
void lv_spinbox_set_digit_format(lv_obj_t * obj, uint32_t digit_count, uint32_t sep_pos);

/**
 * Установите количество цифр
 * @param obj           указатель на спинбокс
 * @param digit_count   количество цифр
 */
void lv_spinbox_set_digit_count(lv_obj_t * obj, uint32_t digit_count);

/**
 * Установите положение десятичной точки
 * @param obj           указатель на спинбокс
 * @param dec_point_pos 0: разделителя нет, 2: две целые цифры
 */
void lv_spinbox_set_dec_point_pos(lv_obj_t * obj, uint32_t dec_point_pos);

/**
 * Установить шаг счетчика
 * @param obj   указатель на спинбокс
 * @param step  шаги по увеличению/уменьшению. Может быть 1, 10, 100, 1000 и т. д. — цифра, которая будет меняться.
 */
void lv_spinbox_set_step(lv_obj_t * obj, uint32_t step);

/**
 * Установить диапазон значений счетчика
 * @param obj       указатель на спинбокс
 * @param min_value минимальная стоимость, включительно
 * @param max_value максимальная стоимость включительно
 */
void lv_spinbox_set_range(lv_obj_t * obj, int32_t min_value, int32_t max_value);

/**
 * Установите минимальное значение
 * @param obj       указатель на спинбокс
 * @param min_value минимальное значение
 */
void lv_spinbox_set_min_value(lv_obj_t * obj, int32_t min_value);

/**
 * Установите максимальное значение
 * @param obj       указатель на спинбокс
 * @param max_value максимальное значение
 */
void lv_spinbox_set_max_value(lv_obj_t * obj, int32_t max_value);

/**
 * Установите позицию курсора на определенную цифру для редактирования
 * @param obj   указатель на спинбокс
 * @param pos   выбранная позиция в счетчике
 */
void lv_spinbox_set_cursor_pos(lv_obj_t * obj, uint32_t pos);

/**
 * Установите направление шага цифр при нажатии кнопки кодировщика в режиме редактирования.
 * @param obj           указатель на спинбокс
 * @param direction     направление (LV_DIR_RIGHTилиLV_DIR_LEFT)
 */
void lv_spinbox_set_digit_step_direction(lv_obj_t * obj, lv_dir_t direction);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получить статус функции опрокидывания счетчика
 * @param obj   указатель на спинбокс
 */
bool lv_spinbox_get_rollover(lv_obj_t * obj);

/**
 * Получите числовое значение счетчика (пользователь должен преобразовать его в число с плавающей запятой в соответствии с его цифровым форматом)
 * @param obj   указатель на спинбокс
 * @return      value целое значение счетчика
 */
int32_t lv_spinbox_get_value(lv_obj_t * obj);

/**
 * Получите значение шага счетчика (пользователь должен преобразовать его в число с плавающей запятой в соответствии с его цифровым форматом)
 * @param obj   указатель на спинбокс
 * @return      значение целочисленного значения шага счетчика
 */
int32_t lv_spinbox_get_step(lv_obj_t * obj);

/**
 * Получить количество цифр счетчика
 * @param obj   указатель на спинбокс
 * @return      количество цифр
 */
uint32_t lv_spinbox_get_digit_count(lv_obj_t * obj);

/**
 * Получить позицию десятичной точки
 * @param obj   указатель на спинбокс
 * @return      положение десятичной точки
 */
uint32_t lv_spinbox_get_dec_point_pos(lv_obj_t * obj);

/**
 * Получить минимальное значение счетчика
 * @param obj   указатель на спинбокс
 * @return      минимальное значение
 */
int32_t lv_spinbox_get_min_value(lv_obj_t * obj);

/**
 * Получить максимальное значение счетчика
 * @param obj   указатель на спинбокс
 * @return      максимальное значение
 */
int32_t lv_spinbox_get_max_value(lv_obj_t * obj);

/**
 * Получить направление шага цифры
 * @param obj   указатель на спинбокс
 * @return      направление (LV_DIR_RIGHTилиLV_DIR_LEFT)
 */
lv_dir_t lv_spinbox_get_digit_step_direction(lv_obj_t * obj);

/*=====================
 * Другие функции
 *====================*/

/**
 * Выберите следующую младшую цифру для редактирования, разделив шаг на 10.
 * @param obj   указатель на спинбокс
 */
void lv_spinbox_step_next(lv_obj_t * obj);

/**
 * Выберите следующую более старшую цифру для редактирования, умножив шаг на 10.
 * @param obj   указатель на спинбокс
 */
void lv_spinbox_step_prev(lv_obj_t * obj);

/**
 * Увеличение значения счетчика на один шаг
 * @param obj   указатель на спинбокс
 */
void lv_spinbox_increment(lv_obj_t * obj);

/**
 * Уменьшить значение счетчика на один шаг
 * @param obj   указатель на спинбокс
 */
void lv_spinbox_decrement(lv_obj_t * obj);



#if LV_USE_OBSERVER
/**
 * Привяжите значения к последствиям Spinbox.
 * @param obj       указатель на Spinbox
 * @param subject   указатель на тему
 * @return          указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_spinbox_bind_value(lv_obj_t * obj, lv_subject_t * subject);
#endif

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SPINBOX*/

#ifdef __cplusplus
} /*extern "C"*/
#endif
#endif /*LV_SPINBOX_H*/
