
/**
 * @file lv_demo_vector_graphic.h
 *
 */

#ifndef LV_DEMO_VECTOR_GRAPHIC_H
#define LV_DEMO_VECTOR_GRAPHIC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_demos.h"

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
 * Нарисуйте на холсте множество векторных фигур и путей.
 * Требуется большое количество RAM для буфера холста.
 */
void lv_demo_vector_graphic_buffered(void);

/**
 * Рисуйте множество векторных фигур и путей на холсте прямо на экране.
 * Это медленнее, поскольку графика должна отображаться в каждом цикле рендеринга.
 */
void lv_demo_vector_graphic_not_buffered(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /*LV_DEMO_VECTOR_GRAPHIC_H*/
