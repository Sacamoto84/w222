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
 * @param base              The NXP eLCD controller base address
 * @param config            NXP eLCD config object
 * @param frame_buffer1     pointer the first frame buffers
 * @param frame_buffer2     pointer the second frame buffers
 * @param buf_size          size of a buffer in bytes (must be at least as large as the screen)
 * @return                  a display object initialized and registerd on the LVGL runtime
 */
lv_display_t * lv_nxp_display_elcdif_create_direct(LCDIF_Type * base, const elcdif_rgb_mode_config_t * config,
                                                   void * frame_buffer1,
                                                   void * frame_buffer2, size_t buf_size);


/**
* Присоедините LVGL к ELCDIF, используя режим рендеринга PARTIAL.
* ELCDIF должен быть уже инициализирован.
* @param base              The NXP eLCD controller base address
* @param config            NXP eLCD config object
* @param frame_buffer1     pointer the first frame buffers
* @param frame_buffer2     pointer the second frame buffers
* @param buf_size          size of a buffer in bytes
* @return                  a display object initialized and registerd on the LVGL runtime
*/
lv_display_t * lv_nxp_display_elcdif_create_partial(LCDIF_Type * base, const elcdif_rgb_mode_config_t * config,
                                                    void * frame_buffer1,
                                                    void * frame_buffer2, size_t buf_size);

/**
 * Вызовите эту функцию в процедуре обслуживания прерываний LCD.
 * Он сообщает LVGL, что делать при передаче кадрового буфера.
 * на панель LCD
 * @param disp              The display instance that contains the eLCD related data
 *
 * @note: the parameter disp is tipycally the return value after
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
