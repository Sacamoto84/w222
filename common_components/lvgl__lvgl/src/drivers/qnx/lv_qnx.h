/**
 * @file lv_qnx.h
 * @brief   ДрайверLVGLдля оконного менеджера компоновки экранаQNXScreen
 */

#ifndef LV_QNX_H
#define LV_QNX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"
#include "../../indev/lv_indev.h"

#if LV_USE_QNX

#include <stdbool.h>
#include <screen/screen.h>

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
 * Создайте окно для отображения LVGL.
 * @param   hor_res     Горизонтальное разрешение (размер) окна
 * @param   ver_res     Вертикальное разрешение (размер) окна
 * @return  Указатель на новый экранный объект в случае успеха, в противном случае NULL.
 */
lv_display_t * lv_qnx_window_create(int32_t hor_res, int32_t ver_res);

/**
 * Установите заголовок окна, идентифицируемого данным дисплеем.
 * @param   disp    Объект отображения для окна
 * @param   title   Новый заголовок для установки
 */
void lv_qnx_window_set_title(lv_display_t * disp, const char * title);

/**
 * Создайте устройство ввода указателя для дисплея.
 * В настоящее время поддерживается только один объект-указатель.
 * @param   disp    Объект отображения, связанный с устройством
 * @return  true в случае успеха, false в противном случае
 */
bool lv_qnx_add_pointer_device(lv_display_t * disp);

/**
 * Создайте устройство ввода с клавиатуры для дисплея.
 * В настоящее время поддерживается только один объект клавиатуры.
 * @param   disp    Объект отображения, связанный с устройством
 * @return  true в случае успеха, false в противном случае
 */
bool lv_qnx_add_keyboard_device(lv_display_t * disp);

/**
 * Запускает цикл событий для дисплея.
 * Функция возвращает значение только в ответ на событие закрытия.
 * @param   disp    Отображение цикла событий
 * @return  Код выхода
 */
int lv_qnx_event_loop(lv_display_t * disp);

/**********************
 *      MACROS
 **********************/

#endif /* LV_DRV_QNX */

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /* LV_QNX_H */
