/*******************************************************************
 *
 * @file lv_indev_gesture.h
 *
 * Copyright (c) 2024 EDGEMTech Ltd.
 *
 * Автор EDGEMTech Ltd, (erik.tagiros@edgemtech.ch)
 *
 ******************************************************************/

#ifndef LV_INDEV_GESTURE_H
#define LV_INDEV_GESTURE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../core/lv_obj.h"

#if LV_USE_GESTURE_RECOGNITION

#if LV_USE_FLOAT == 0
#error "LV_USE_FLOAT is required for gesture detection."
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/* Непрозрачные типы, определенные в частном заголовке */
struct lv_indev_gesture;
struct lv_indev_gesture_configuration;

typedef struct lv_indev_gesture_recognizer lv_indev_gesture_recognizer_t;
typedef struct lv_indev_touch_data lv_indev_touch_data_t;

typedef struct lv_indev_gesture lv_indev_gesture_t;
typedef struct lv_indev_gesture_configuration lv_indev_gesture_configuration_t;

typedef void (*lv_recognizer_func_t)(lv_indev_gesture_recognizer_t *, lv_indev_touch_data_t *, uint16_t);

/* Состояния распознавателя жестов */
typedef enum {
    LV_INDEV_GESTURE_STATE_NONE = 0,    /* Начало и конец */
    LV_INDEV_GESTURE_STATE_ONGOING,     /* Устанавливайте, когда есть вероятность */
    LV_INDEV_GESTURE_STATE_RECOGNIZED,  /* Известно, что событие будет содержать сенсорную информацию */
    LV_INDEV_GESTURE_STATE_ENDED,       /* Распознанный жест закончился */
    LV_INDEV_GESTURE_STATE_CANCELED,    /* Canceled - обычно палец поднимается */
} lv_indev_gesture_state_t;

/* Структуры данных для событий касания — используются для отображения событий libinput. */
/* Излучается устройствами, способными отслеживать идентифицируемые контакты (тип B) */
struct lv_indev_touch_data {
    lv_point_t point;                   /* Координаты касания */
    lv_indev_state_t state;             /* Состояние, т.е. PRESSED или RELEASED. */
    uint8_t id;                         /* Идентификация/слот точки контакта */
    uint32_t timestamp;                 /* Временная метка в миллисекундах */
};

/* Распознаватель жестов */
struct lv_indev_gesture_recognizer {
    lv_indev_gesture_type_t type;       /* Обнаруженный тип жеста */
    lv_indev_gesture_state_t state;     /* Состояние жеста продолжается, распознано */
    lv_indev_gesture_t * info;          /* Информация о движении каждой точки касания */
    float scale;                        /* Актуально для жеста щипка */
    float rotation;                     /* Актуально для ротации */
    float distance;                     /* Актуально для свайпов */
    float speed;
    lv_dir_t two_fingers_swipe_dir;     /* Актуально для свайпов */

    lv_indev_gesture_configuration_t * config;  /* Конфигурация распознавателя, содержащая жесты
                                                   пороги */
    lv_recognizer_func_t recog_fn;      /* Функция распознавателя, которую должен выполнить этот распознаватель */
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте распознаватели этого индев. Он определил их функцию определения
 * @param indev             указатель на indev, содержащий распознаватели для инициализации
 */
void lv_indev_gesture_init(lv_indev_t * indev);

/* PINCH Жест */

/**
 * Функция распознавания жестов щипка
 * Обновит данные распознавателя
 * @param recognizer        указатель на распознаватель жестов
 * @param touches           указатель на первый элемент собранных событий касания
 * @param touch_cnt         длина переданного массива событий касания.
 */
void lv_indev_gesture_detect_pinch(lv_indev_gesture_recognizer_t * recognizer, lv_indev_touch_data_t * touches,
                                   uint16_t touch_cnt);

/**
 * Функция распознавания жестов вращения
 * Обновит данные распознавателя
 * @param recognizer        указатель на распознаватель жестов
 * @param touches           указатель на первый элемент собранных событий касания
 * @param touch_cnt         длина переданного массива событий касания.
 */
void lv_indev_gesture_detect_rotation(lv_indev_gesture_recognizer_t * recognizer, lv_indev_touch_data_t * touches,
                                      uint16_t touch_cnt);

/**
 * Функция распознавания жестов двумя пальцами
 * Обновит данные распознавателя
 * @param recognizer        указатель на распознаватель жестов
 * @param touches           указатель на первый элемент собранных событий касания
 * @param touch_cnt         длина переданного массива событий касания.
 */
void lv_indev_gesture_detect_two_fingers_swipe(lv_indev_gesture_recognizer_t * recognizer,
                                               lv_indev_touch_data_t * touches,
                                               uint16_t touch_cnt);

/**
 * Установите порог масштабирования жеста сведения, когда масштабный коэффициент жеста увеличивается.
 * достигает порогового значения, события отправляются
 * @param indev             указатель на устройство разработки, содержащее распознаватель сжатия
 * @param threshold         порог распознавания жеста сжатия вверх
 */
void lv_indev_set_pinch_up_threshold(lv_indev_t * indev, float threshold);

/**
 * Уменьшите порог масштабирования жеста разведения, когда масштабный коэффициент жеста
 * достигает порогового значения, события отправляются
 * @param indev             указатель на устройство разработки, содержащее распознаватель сжатия
 * @param threshold         порог распознавания жеста сжатия вниз
 */
void lv_indev_set_pinch_down_threshold(lv_indev_t * indev, float threshold);

/**
 * Установите порог вращения в радианах для жеста вращения.
 * @param indev             указатель на устройство разработки, содержащее распознаватель вращения
 * @param threshold         порог в радианах для распознавания жеста вращения
 */
void lv_indev_set_rotation_rad_threshold(lv_indev_t * indev, float threshold);

/**
 * Получает текущий масштаб жеста сжатия.
 * @param gesture_event     указатель на событие жеста
 * @return                  масштаб текущего жеста
 */
float lv_event_get_pinch_scale(lv_event_t * gesture_event);

/**
 * Получает текущий угол в радианах жеста вращения.
 * @param gesture_event     указатель на событие жеста
 * @return                  угол поворота в радианах текущего жеста
 */
float lv_event_get_rotation(lv_event_t * gesture_event);

/**
 * Получает текущее расстояние в пикселях по жесту смахивания двумя пальцами от начального центра.
 * @param gesture_event     указатель на событие жеста
 * @return                  расстояние от центра текущего жеста в пикселях
 */
float lv_event_get_two_fingers_swipe_distance(lv_event_t * gesture_event);

/**
 * Получает текущее направление от центра пролистывания двумя пальцами
 * @param gesture_event     указатель на событие жеста
 * @return                  угол поворота в радианах текущего жеста
 */
lv_dir_t lv_event_get_two_fingers_swipe_dir(lv_event_t * gesture_event);

/**
 * Устанавливает состояние распознавателя в независимую структуру данных,
 * обычно он возникает из-за обратного вызова чтения indev
 * @param data данные разработки
 * @param recognizer указатель на распознаватель жестов
 */
void lv_indev_set_gesture_data(lv_indev_data_t * data, lv_indev_gesture_recognizer_t * recognizer,
                               lv_indev_gesture_type_t type);

/**
 * Получает центральную точку жеста
 * @param recognizer     указатель на событие распознавания жестов
 * @param point             указатель на точку
 */
void lv_indev_get_gesture_center_point(lv_indev_gesture_recognizer_t * recognizer, lv_point_t * point);

/**
 * Получает текущее состояние распознавателя жестов, прикрепленного к событию.
 * @param gesture_event     указатель на событие распознавания жестов
 * @return                  текущее состояние распознавателя жестов
 */
lv_indev_gesture_state_t lv_event_get_gesture_state(lv_event_t * gesture_event, lv_indev_gesture_type_t type);

/**
 * Получает текущий тип события распознавателя жестов, прикрепленного к событию.
 * @param gesture_event     указатель на событие распознавания жестов
 * @return                  текущий тип события распознавателя жестов
 */
lv_indev_gesture_type_t lv_event_get_gesture_type(lv_event_t * gesture_event);

/**
 * Получает координаты текущей основной точки
 * @param recognizer        указатель на распознаватель жестов
 * @param point             указатель на точку
 */
void lv_indev_get_gesture_primary_point(lv_indev_gesture_recognizer_t * recognizer, lv_point_t * point);

/**
 * Позволяет определить, выполняется ли текущий жест
 * @param recognizer        указатель на распознаватель жестов
 * @return                  false, если нет точек контакта или жест завершился — в противном случае true
 */
bool lv_indev_recognizer_is_active(lv_indev_gesture_recognizer_t * recognizer);

/**
 * Обновите распознаватели. Он выполняет функции распознавателя и проверяет
 * Жесты LV_GESTURE_STATE_RECOGNIZED или LV_GESTURE_STATE_ENDED.
 * Будет сохраняться в indevread_cb.
 * @param indev             указатель на indev, из которого реконайзеру требуется обновление
 * @param touches           массив данных касания indev, содержащий данные о последнем касании из indev
 *                          с момента последнего обновления распознавателей
 * @param touch_cnt         количество данных indev touch в касаниях
 */
void lv_indev_gesture_recognizers_update(lv_indev_t * indev, lv_indev_touch_data_t * touches, uint16_t touch_cnt);

/**
 * Установите структуруlv_indev_data_tиз данных распознавателя.
 * Будет сохраняться в indevread_cb.
 */
void lv_indev_gesture_recognizers_set_data(lv_indev_t * indev, lv_indev_data_t * data);


/**********************
 *      MACROS
 **********************/

#endif /* END LV_USE_RECOGNITION */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_INDEV_GESTURE_H */
