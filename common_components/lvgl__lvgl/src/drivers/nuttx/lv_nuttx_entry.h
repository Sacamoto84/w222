/**
 * @file lv_nuttx_entry.h
 *
 */

/*********************
 *      INCLUDES
 *********************/

#ifndef LV_NUTTX_ENTRY_H
#define LV_NUTTX_ENTRY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#include "../../display/lv_display.h"
#include "../../indev/lv_indev.h"

#if LV_USE_NUTTX

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    const char * fb_path;
    const char * input_path;
    const char * utouch_path;
    const char * mouse_path;
    const char * trace_path;
} lv_nuttx_dsc_t;

typedef struct {
    lv_display_t * disp;
    lv_indev_t * indev;
    lv_indev_t * utouch_indev;
    lv_indev_t * mouse_indev;
} lv_nuttx_result_t;

typedef struct _lv_nuttx_ctx_t {
    void * image_cache;
    int trace_fd;
} lv_nuttx_ctx_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте структуруlv_nuttx_dsc_tзначениями по умолчанию для порта NuttXLVGL.
 * @param dsc Указатель на структуру lv_nuttx_dsc_t, которую необходимо инициализировать.
 */
void lv_nuttx_dsc_init(lv_nuttx_dsc_t * dsc);

/**
 * Инициализируйте драйвер видеокартыLVGLдля NuttX, используя предоставленную информацию о конфигурации.
 * @param dsc Указатель на структуру lv_nuttx_dsc_t, содержащую информацию о конфигурации драйвера дисплея.
 * @param result Указатель на структуру lv_nuttx_result_t, содержащую обработчик устройства отображения и ввода.
 */
void lv_nuttx_init(const lv_nuttx_dsc_t * dsc, lv_nuttx_result_t * result);

/**
 * Деинициализируйте драйвер видеокартыLVGLдля NuttX.
 * @param result Указатель на структуру lv_nuttx_result_t, содержащую обработчик устройства отображения и ввода.
 */
void lv_nuttx_deinit(lv_nuttx_result_t * result);

#if LV_USE_NUTTX_CUSTOM_INIT
/**
 * Инициализируйте драйвер видеокартыLVGLдля NuttX, используя предоставленную информацию о пользовательской конфигурации.
 * @param dsc Указатель на структуру lv_nuttx_dsc_t, содержащую пользовательскую конфигурацию драйвера дисплея.
 * @param result Указатель на структуру lv_nuttx_result_t, содержащую обработчик устройства отображения и ввода.
 */
void lv_nuttx_init_custom(const lv_nuttx_dsc_t * dsc, lv_nuttx_result_t * result);

/**
 * Деинициализируйте драйвер видеокартыLVGLдля NuttX, используя предоставленную информацию о пользовательской конфигурации.
 * @param result Указатель на структуру lv_nuttx_result_t, содержащую обработчик устройства отображения и ввода.
 */
void lv_nuttx_deinit_custom(lv_nuttx_result_t * result);
#endif /* LV_USE_NUTTX_CUSTOM_INIT */

/**
 * Вызов`lv_timer_handler()`(суперциклLVGL) в бесконечном цикле.
 * Если LV_USE_NUTTX_LIBUV включен, будет создан таймер UV.
 * В противном случае`lv_timer_handler()`будет переходить в цикл с некоторым спойлерным режимом.
 * @param result указатель на переменную, инициализированную`lv_nuttx_init()`или `lv_nuttx_init_custom()`
 */
void lv_nuttx_run(lv_nuttx_result_t * result);

/**
 * Получите процент простоя системы.
 * @return Процент простоя системы.
 */
uint32_t lv_nuttx_get_idle(void);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_NUTTX*/

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /* LV_NUTTX_ENTRY_H */
