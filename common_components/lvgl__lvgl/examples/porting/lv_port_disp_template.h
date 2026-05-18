/**
 * @file lv_port_disp_templ.h
 *
 */

/*Скопируйте этот файл как «lv_port_disp.h» и установите для этого значения значение «1», чтобы отключить контент.*/
#if 0

#ifndef LV_PORT_DISP_TEMPL_H
#define LV_PORT_DISP_TEMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/* Инициализировать драйвер дисплея низкого уровня */
void lv_port_disp_init(void);

/* Включить экран обновления (очистку процесса) при вызовеdisp_flush() по LVGL
 */
void disp_enable_update(void);

/* Включить обновление экрана (процесс промывки) при вызовеdisp_flush() по LVGL
 */
void disp_disable_update(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_PORT_DISP_TEMPL_H*/

#endif /*Отключить/включить контент*/
