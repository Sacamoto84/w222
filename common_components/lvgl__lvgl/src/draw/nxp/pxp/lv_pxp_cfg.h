/**
 * @file lv_pxp_cfg.h
 *
 */

/**
 * Copyright 2020-2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_PXP_CFG_H
#define LV_PXP_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_PXP
#if LV_USE_DRAW_PXP || LV_USE_ROTATE_PXP
#include "fsl_cache.h"
#include "fsl_pxp.h"

#if ((LV_DRAW_BUF_ALIGN % 32) != 0)
#error "If PXP is enabled the draw buffers should be aligned to 32-byte boundary, please set LV_DRAW_BUF_ALIGN to a multiple of 32 in lv_conf.h"
#endif

#include "../../../misc/lv_log.h"

/*********************
 *      DEFINES
 *********************/

/** Экземпляр модуля PXP для использования*/
#define PXP_ID PXP

/** PXP линия прерывания ID*/
#define PXP_IRQ_ID PXP_IRQn

/**********************
 *      TYPEDEFS
 **********************/

/**
 * NXP PXP Конфигурация устройства.
 */
typedef struct {
    /** Обратный вызов для инициализации прерывания PXP*/
    void (*pxp_interrupt_init)(void);

    /** Обратный вызов для деинициализации прерывания PXP*/
    void (*pxp_interrupt_deinit)(void);

    /** Обратный вызов для запуска PXP*/
    void (*pxp_run)(void);

    /** Обратный вызов для ожидания завершения PXP*/
    void (*pxp_wait)(void);
} pxp_cfg_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Сбросьте и инициализируйте устройство PXP. Эту функцию следует вызывать как часть
 * последовательности инициализации дисплея.
 */
void lv_pxp_init(void);

/**
 * Отключите устройство PXP. Должен вызываться во время последовательности отображения deinit.
 */
void lv_pxp_deinit(void);

/**
 * Сбросьте устройство PXP.
 */
void lv_pxp_reset(void);

/**
 * Очистите кеш и запустите PXP.
 */
void lv_pxp_run(void);

/**
 * Дождитесь завершения PXP.
 */
void lv_pxp_wait(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_PXP || LV_USE_ROTATE_PXP*/
#endif /*LV_USE_PXP*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_PXP_CFG_H*/
