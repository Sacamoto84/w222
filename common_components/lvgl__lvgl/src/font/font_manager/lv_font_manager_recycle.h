/**
 * @file lv_font_manager_recycle.h
 *
 */

#ifndef LV_FONT_MANAGER_RECYCLE_H
#define LV_FONT_MANAGER_RECYCLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../misc/lv_types.h"

#if LV_USE_FONT_MANAGER

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_font_manager_recycle_t lv_font_manager_recycle_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте менеджер переработки шрифтов.
 * @param max_size размер переработки.
 * @return указатель на диспетчер переработки шрифтов.
 */
lv_font_manager_recycle_t * lv_font_manager_recycle_create(uint32_t max_size);

/**
 * Удалить менеджер переработки шрифтов.
 * @param manager указатель на диспетчер переработки шрифтов.
 */
void lv_font_manager_recycle_delete(lv_font_manager_recycle_t * manager);

/**
 * Приобретите многоразовый шрифт.
 * @param manager указатель на диспетчер переработки шрифтов.
 * @param ft_info информация о шрифте.
 * @return возвращает true в случае успеха.
 */
lv_font_t * lv_font_manager_recycle_get_reuse(lv_font_manager_recycle_t * manager, const lv_font_info_t * ft_info);

/**
 * Установите шрифты для повторного использования.
 * @param manager указатель на диспетчер переработки шрифтов.
 * @param ft_info информация о шрифте.
 */
void lv_font_manager_recycle_set_reuse(lv_font_manager_recycle_t * manager, lv_font_t * font,
                                       const lv_font_info_t * ft_info);

/**
 * Удалите шрифты с именем из диспетчера переработки.
 * @param manager указатель на диспетчер переработки шрифтов.
 * @param name название шрифта.
 */
void lv_font_recycle_remove_fonts(lv_font_manager_recycle_t * manager, const char * name);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_FONT_MANAGER */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_FONT_MANAGER_RECYCLE_H */
