/**
 * @file lv_xkb.h
 *
 */

#ifndef LV_XKB_H
#define LV_XKB_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if defined(LV_LIBINPUT_XKB) && LV_LIBINPUT_XKB

#include "../../misc/lv_types.h"
#include <stdbool.h>
#include <xkbcommon/xkbcommon.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте дескриптор XKB.
 * @return true, если инициализация прошла успешно
 */
bool lv_xkb_init(lv_xkb_t * dsc, struct xkb_rule_names names);

/**
 * Деинициализируйте дескриптор XKB.
 * @param dsc Указатель на дескриптор
 */
void lv_xkb_deinit(lv_xkb_t * dsc);

/**
 * Обработайте скан-код evdev, используя текущий дескриптор XKB.
 * @param dsc ДескрипторXKBдля использования
 * @param scancode скан-код evdev для обработки
 * @param down true, если клавиша была нажата, false, если она была отпущена
 * @return (первый) символUTF-8, созданный событием, или 0, если вывод не производился
 */
uint32_t lv_xkb_process_key(lv_xkb_t * dsc, uint32_t scancode, bool down);

/**********************
 *      MACROS
 **********************/

#endif /* определено( LV_LIBINPUT_XKB ) && LV_LIBINPUT_XKB */

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /* LV_XKB_H */
