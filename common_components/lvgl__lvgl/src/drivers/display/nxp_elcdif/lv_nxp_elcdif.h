/**
 * @file lv_nxp_elcdif.h
 * Драйвер для ELCD NXP
 */

#ifndef LV_NXP_ELCDIF_H
#define LV_NXP_ELCDIF_H

#include "../../../lvgl.h"
#include "../../../display/lv_display.h"

#if LV_USE_NXP_ELCDIF == 1

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "fsl_elcdif.h"
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
 * Присоедините LVGL к ELCDIF, используя режим рендеринга DIRECT.
 * ELCDIF должен быть уже инициализирован.
 * @param base              Базовый адрес контроллера eLCD NXP
 * @param config            NXP Объект конфигурации eLCD
 * @param frame_buffer1     указатель на буферы первых кадров
 * @param frame_buffer2     указатель на второй буфер кадров
 * @param buf_size          размер буфера в байтах (должен быть не меньше экрана)
 * @return                  объект отображения, инициализированный и зарегистрированный в среде выполнения LVGL
 */
lv_display_t * lv_nxp_display_elcdif_create_direct(LCDIF_Type * base, const elcdif_rgb_mode_config_t * config,
                                                   void * frame_buffer1,
                                                   void * frame_buffer2, size_t buf_size);


/**
* Присоедините LVGL к ELCDIF, используя режим рендеринга PARTIAL.
* ELCDIF должен быть уже инициализирован.
* @param base              Базовый адрес контроллера eLCD NXP
* @param config            NXP Объект конфигурации eLCD
* @param frame_buffer1     указатель на буферы первых кадров
* @param frame_buffer2     указатель на второй буфер кадров
* @param buf_size          размер буфера в байтах
* @return                  объект отображения, инициализированный и зарегистрированный в среде выполнения LVGL
*/
lv_display_t * lv_nxp_display_elcdif_create_partial(LCDIF_Type * base, const elcdif_rgb_mode_config_t * config,
                                                    void * frame_buffer1,
                                                    void * frame_buffer2, size_t buf_size);

/**
 * Вызовите эту функцию в процедуре обслуживания прерываний LCD.
 * Он сообщает LVGL, что делать при передаче кадрового буфера.
 * на панель LCD
 * @param disp              Экземпляр дисплея, содержащий данные, связанные с eLCD.
 *
 * @note: параметр disp обычно является возвращаемым значением после
 *        `lv_nxp_display_elcdif_create_direct` успешно выполнен.
 */
void lv_nxp_display_elcdif_event_handler(const lv_display_t * disp);
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_USE_NXP_ELCDIF*/

#endif /* LV_NXP_ELCDIF_H */
