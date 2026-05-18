/**
 * @file lv_pxp_osa.h
 *
 */

/**
 * Copyright 2020, 2022-2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_PXP_OSA_H
#define LV_PXP_OSA_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_PXP
#if LV_USE_DRAW_PXP || LV_USE_ROTATE_PXP
#include "lv_pxp_cfg.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Обработчик прерываний устройства PXP. Используется для проверки статуса завершения задачи PXP.
 */
void PXP_IRQHandler(void);

/**
 * Получите конфигурацию PXP по умолчанию.
 */
pxp_cfg_t * pxp_get_default_cfg(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_PXP || LV_USE_ROTATE_PXP*/
#endif /*LV_USE_PXP*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_PXP_OSA_H*/
