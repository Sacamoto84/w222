/*******************************************************************
 *
 * @file lv_indev_gesture_private.h
 *
 * Содержит объявления и определения, которые являются внутренними.
 * к логике обнаружения жестов
 *
 * Copyright (c) 2024 EDGEMTech Ltd.
 *
 * Автор EDGEMTech Ltd, (erik.tagiros@edgemtech.ch)
 *
 ******************************************************************/

#ifndef LV_INDEV_GESTURE_PRIVATE_H
#define LV_INDEV_GESTURE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../core/lv_obj.h"

#if LV_USE_GESTURE_RECOGNITION

/*********************
 *      DEFINES
 *********************/

#define LV_GESTURE_MAX_POINTS 2


/**********************
 *      TYPEDEFS
 **********************/

/* Представлять движение пальца */
struct lv_indev_gesture_motion {
    int8_t finger;                      /* ID отслеживаемого пальца */
    lv_point_t start_point;             /* Координаты, где произошло событие DOWN */
    lv_point_t point;                   /* Текущие координаты */
    lv_indev_state_t state;             /* DEBUG: Состояние, т.е. PRESSED или RELEASED. */
};

typedef struct lv_indev_gesture_motion lv_indev_gesture_motion_t;

/* Общий дескриптор жеста, используемый конечными автоматами распознавателя для отслеживания.
 * масштаб, вращение и перевод NOTE : (вероятно, это станет конфиденциальным) */
struct lv_indev_gesture {

    /* Дескриптор движения, хранит координаты и скорость точки контакта. */
    lv_indev_gesture_motion_t motions[LV_GESTURE_MAX_POINTS];

    lv_point_t center;                  /* Центральная точка */
    float scale;                        /* Масштабный коэффициент и предыдущий масштабный коэффициент */
    float p_scale;
    float scale_factors_x[LV_GESTURE_MAX_POINTS];   /* Масштабный коэффициент относительно центра для каждой точки */
    float scale_factors_y[LV_GESTURE_MAX_POINTS];

    float delta_x;                      /* Перевод и предыдущий перевод */
    float delta_y;
    float p_delta_x;
    float p_delta_y;
    float rotation;                     /* Ротация и предыдущая ротация*/
    float p_rotation;
    uint8_t finger_cnt;                 /* Текущее количество контактных лиц */

};

/* Настройка распознавательного устройства. Он хранит пороговые значения, необходимые для обнаружения жестов и
 * считать их признанными. После того как indev начинает отправлять событиеLV_GESTURE.
 */
struct lv_indev_gesture_configuration {

    float pinch_up_threshold;           /* Порог распознавания жеста сведения пальца вверх (в пикселях). */
    float pinch_down_threshold;         /* Порог распознавания жеста сжатия вниз (в пикселях). */
    float rotation_angle_rad_threshold; /* Порог распознавания жеста вращения (в радианах). */

};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* END LV_USE_RECOGNITION */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_INDEV_GESTURE_PRIVATE_H */
