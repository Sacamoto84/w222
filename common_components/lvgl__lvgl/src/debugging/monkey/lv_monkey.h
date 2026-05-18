/**
 * @file lv_monkey.h
 *
 */
#ifndef LV_MONKEY_H
#define LV_MONKEY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#include "../../indev/lv_indev.h"

#if LV_USE_MONKEY != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_monkey_t lv_monkey_t;

typedef struct {
    int32_t min;
    int32_t max;
} lv_range_t;

typedef struct {
    uint32_t min;
    uint32_t max;
} lv_urange_t;

struct _lv_monkey_config_t {
    /** Тип устройства ввода */
    lv_indev_type_t type;

    /** Период казни обезьяны */
    lv_urange_t period_range;

    /** Диапазон входного значения */
    lv_range_t input_range;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте конфигурацию обезьяны со значениями по умолчанию.
 * @param config указатель на переменную 'lv_monkey_config_t' для инициализации
 */
void lv_monkey_config_init(lv_monkey_config_t * config);

/**
 * Создать обезьянку для теста
 * @param config указатель на переменную 'lv_monkey_config_t'
 * @return указатель на созданную обезьяну
 */
lv_monkey_t * lv_monkey_create(const lv_monkey_config_t * config);

/**
 * Получить устройство ввода обезьяны
 * @param monkey указатель на обезьяну
 * @return указатель на устройство ввода
 */
lv_indev_t * lv_monkey_get_indev(lv_monkey_t * monkey);

/**
 * Включить обезьяну
 * @param monkey указатель на обезьяну
 * @param en установите значение true, чтобы включить
 */
void lv_monkey_set_enable(lv_monkey_t * monkey, bool en);

/**
 * Узнайте, включена ли обезьяна
 * @param monkey указатель на обезьяну
 * @return вернуть true, если обезьяна включена
 */
bool lv_monkey_get_enable(lv_monkey_t * monkey);

/**
 * Установите полеuser_dataобезьяны.
 * @param monkey   указатель на обезьяну
 * @param user_data   указатель на новый user_data.
 */
void lv_monkey_set_user_data(lv_monkey_t * monkey, void * user_data);

/**
 * Получите полеuser_dataобезьяны.
 * @param monkey указатель на обезьяну
 * @return указатель наuser_dataобезьяны
 */
void * lv_monkey_get_user_data(lv_monkey_t * monkey);

/**
 * Удалить обезьяну
 * @param monkey указатель на обезьяну
 */
void lv_monkey_delete(lv_monkey_t * monkey);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_MONKEY*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_MONKEY_H*/
