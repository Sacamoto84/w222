/**
 * @file lv_anim_timeline.h
 *
 */

#ifndef LV_ANIM_TIMELINE_H
#define LV_ANIM_TIMELINE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_anim.h"

/*********************
 *      DEFINES
 *********************/

#define LV_ANIM_TIMELINE_PROGRESS_MAX 0xFFFF

/**********************
 *      TYPEDEFS
 **********************/

/*Данные anim_timeline_dsc*/
typedef struct _lv_anim_timeline_dsc_t {
    lv_anim_t anim;
    uint32_t start_time;
    uint8_t is_started : 1;
    uint8_t is_completed : 1;
} lv_anim_timeline_dsc_t;


/**********************
* GLOBAL PROTOTYPES
**********************/

/**
 * Создайте временную шкалу анимации.
 * @return указатель на временную шкалу анимации.
 */
lv_anim_timeline_t * lv_anim_timeline_create(void);

/**
 * Удалить временную шкалу анимации.
 * @param at    указатель на временную шкалу анимации.
 */
void lv_anim_timeline_delete(lv_anim_timeline_t * at);

/**
 * Добавьте анимацию на временную шкалу анимации.
 * @param at            указатель на временную шкалу анимации.
 * @param start_time    время начала анимации на временной шкале. Обратите внимание, чтоstart_timeпереопределит значение задержки.
 * @param a             указатель на анимацию.
 */
void lv_anim_timeline_add(lv_anim_timeline_t * at, uint32_t start_time, const lv_anim_t * a);

/**
 * Запустите временную шкалу анимации.
 * @param at    указатель на временную шкалу анимации.
 * @return      общее время, проведенное на временной шкале анимации.
 */
uint32_t lv_anim_timeline_start(lv_anim_timeline_t * at);

/**
 * Приостановите временную шкалу анимации.
 * @param at    указатель на временную шкалу анимации.
 */
void lv_anim_timeline_pause(lv_anim_timeline_t * at);

/**
 * Установите направление воспроизведения временной шкалы анимации.
 * @param at        указатель на временную шкалу анимации.
 * @param reverse   стоит ли играть наоборот.
 */
void lv_anim_timeline_set_reverse(lv_anim_timeline_t * at, bool reverse);

/**
 * Установите время ожидания перед запуском анимации.
 * Применяется только при воспроизведении с самого начала или наоборот с самого конца.
 * @param at        указатель на временную шкалу анимации
 * @param delay     время задержки в миллисекундах
 */
void lv_anim_timeline_set_delay(lv_anim_timeline_t * at, uint32_t delay);

/**
 * Заставьте временную шкалу анимации повторяться.
 * @param at        указатель на временную шкалу анимации.
 * @param cnt       количество повторов или`LV_ANIM_REPEAT_INFINITE`для бесконечного повторения. 0: отключить повторение.
 */
void lv_anim_timeline_set_repeat_count(lv_anim_timeline_t * at, uint32_t cnt);

/**
 * Установите задержку перед повторением временной шкалы анимации.
 * @param at        указатель на временную шкалу анимации.
 * @param delay     задержка в миллисекундах перед повторением временной шкалы анимации.
 */
void lv_anim_timeline_set_repeat_delay(lv_anim_timeline_t * at, uint32_t delay);

/**
 * Установите ход временной шкалы анимации.
 * @param at        указатель на временную шкалу анимации.
 * @param progress  установите значение 0~65535, чтобы отобразить прогресс анимации 0~100%.
 */
void lv_anim_timeline_set_progress(lv_anim_timeline_t * at, uint16_t progress);

/**
 * Установите временную границу анимации user_data.
 * @param at        указатель на временную шкалу анимации.
 * @param user_data указатель на любые данные. Сохранится только указатель.
 */
void lv_anim_timeline_set_user_data(lv_anim_timeline_t * at, void * user_data);

/**
 * Получите время, используемое для воспроизведения временной шкалы анимации.
 * @param at        указатель на временную шкалу анимации.
 * @return общее время, проведенное на временной шкале анимации.
 */
uint32_t lv_anim_timeline_get_playtime(lv_anim_timeline_t * at);

/**
 * Узнайте, воспроизводится ли временная шкала анимации в обратном порядке.
 * @param at        указатель на временную шкалу анимации.
 * @return верните true, если это обратное воспроизведение.
 */
bool lv_anim_timeline_get_reverse(lv_anim_timeline_t * at);

/**
 * Получите время ожидания при игре с самого начала или наоборот с самого конца.
 * @param at    указатель на временную шкалу анимации
 * @return      оставшееся время в миллисекундах
 */
uint32_t lv_anim_timeline_get_delay(lv_anim_timeline_t * at);

/**
 * Получите прогресс на временной шкале анимации.
 * @param at    указатель на временную шкалу анимации.
 * @return      возвращаемое значение 0~65535 для отображения прогресса анимации 0~100%.
 */
uint16_t lv_anim_timeline_get_progress(lv_anim_timeline_t * at);

/**
 * Получите количество повторений временной шкалы анимации.
 * @param at    указатель на временную шкалу анимации.
 */
uint32_t lv_anim_timeline_get_repeat_count(lv_anim_timeline_t * at);

/**
 * Получите задержку повторения временной шкалы анимации.
 * @param at    указатель на временную шкалу анимации.
 */
uint32_t lv_anim_timeline_get_repeat_delay(lv_anim_timeline_t * at);

/**
 * Получитеuser_dataвременной шкалы анимации.
 * @param at    указатель на временную шкалу анимации.
 */
void * lv_anim_timeline_get_user_data(lv_anim_timeline_t * at);

/**
 * Объединить (добавить) все анимации одной временной шкалы в другую.
 * @param dest      объединить анимацию в эту временную шкалу
 * @param src       объединить анимации этой временной шкалы
 * @param delay     добавьте анимацию с этой дополнительной задержкой
 */
void lv_anim_timeline_merge(lv_anim_timeline_t * dest, const lv_anim_timeline_t * src, int32_t delay);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_ANIM_TIMELINE_H*/
