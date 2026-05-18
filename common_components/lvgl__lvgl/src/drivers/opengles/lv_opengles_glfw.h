/**
 * @file lv_opengles_glfw.h
 *
 */

#ifndef LV_OPENGLES_GLFW_H
#define LV_OPENGLES_GLFW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_conf_internal.h"
#if LV_USE_GLFW

#include "../../misc/lv_types.h"

/*********************
 *      INCLUDES
 *********************/

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
 * Создайте окно OpenGLGLFWбез текстуры и придумывайте OpenGL.
 * @param hor_res            ширина окна в пикселях
 * @param ver_res            высота окна в пикселях
 * @param use_mouse_indev    отправить указатель ввода indev на текстуры дисплея LVGL
 * @return                   новый дескриптор окнаGLFWOpenGL
 */
lv_opengles_window_t * lv_opengles_glfw_window_create(int32_t hor_res, int32_t ver_res, bool use_mouse_indev);

/**
 * Создайте окноGLFWбез текстуры и придумайте OpenGL.
 * @param hor_res            ширина окна в пикселях
 * @param ver_res            высота окна в пикселях
 * @param use_mouse_indev    отправить указатель ввода indev на текстуры дисплея LVGL
 * @param h_flip             Should the window contents be horizontally mirrored?
 * @param v_flip             Should the window contents be vertically mirrored?
 * @param title              Заголовок окна
 * @return                   новая оконная ручка GLFW
 */
lv_opengles_window_t * lv_opengles_glfw_window_create_ex(int32_t hor_res, int32_t ver_res, bool use_mouse_indev,
                                                         bool h_flip, bool v_flip,  const char * title);

/**
 * Установите текст заголовка окна
 * @param window     ОкноGLFWдля настройки
 * @param new_title  Новый текст заголовка
 */
void lv_opengles_glfw_window_set_title(lv_opengles_window_t * window, const char * new_title);

/**
 * Установите горизонтальное/вертикальное переворачивание окна GLFW.
 * @param window    ОкноGLFWдля настройки
 * @param h_flip    Should the window contents be horizontally mirrored?
 * @param v_flip    Should the window contents be vertically mirrored?
 */
void lv_opengles_glfw_window_set_flip(lv_opengles_window_t * window, bool h_flip, bool v_flip);

/**
 * Получите дескриптор окнаGLFWдляGLFWlv_opengles_window_t.
 * @param window        ОкноGLFWдля возврата дескриптора
 * @return              оконная ручка GLFW
 */
void * lv_opengles_glfw_window_get_glfw_window(lv_opengles_window_t * window);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GLFW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OPENGLES_GLFW_H*/
