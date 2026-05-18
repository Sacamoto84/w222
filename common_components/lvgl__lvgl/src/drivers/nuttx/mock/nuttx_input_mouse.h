/**
 * @file nuttx_input_mouse.h
 *
 */

#ifndef NUTTX_INPUT_MOUSE_H
#define NUTTX_INPUT_MOUSE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/* Эти определения определяют значение всех битов, которые могут быть
 * сообщается в функциональных функциях mouse_report_s.
 */

#define MOUSE_BUTTON_1       (1 << 0) /* True: Нажата левая кнопка мыши */
#define MOUSE_BUTTON_2       (1 << 1) /* True: Нажата правая кнопка мыши */
#define MOUSE_BUTTON_3       (1 << 2) /* True: Средняя кнопка мыши нажата */

/**********************
 *      TYPEDEFS
 **********************/

struct mouse_report_s {
    uint8_t buttons;  /* См. определения MOUSE_ * выше. */
    uint8_t dummy;    /* Дополнено 1 байтом здесь */
    int16_t x;        /* Координата X положения мыши */
    int16_t y;        /* Координата Y положения мыши */
    int16_t wheel;    /* Положение колеса мыши */
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*NUTTX_INPUT_MOUSE_H*/
