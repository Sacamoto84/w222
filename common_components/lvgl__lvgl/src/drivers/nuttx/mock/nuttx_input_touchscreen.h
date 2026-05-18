/**
 * @file nuttx_input_touchscreen.h
 *
 */

#ifndef NUTTX_INPUT_TOUCHSCREEN_H
#define NUTTX_INPUT_TOUCHSCREEN_H

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

#define _TSIOC(x) (x)

/* Общие команды TSC IOCTL */

#define TSIOC_SETXRCAL       _TSIOC(0x0001) /* arg: Указатель на
                                             * int Xplate R калибровочное значение
                                             */
#define TSIOC_GETXRCAL       _TSIOC(0x0002) /* arg: Указатель на
                                             * int Xplate R калибровочное значение
                                             */
#define TSIOC_SETFREQUENCY   _TSIOC(0x0003) /* arg: Указатель на
                                             * Значение цикла uint32_t
                                             */
#define TSIOC_GETFREQUENCY   _TSIOC(0x0004) /* arg: Указатель на
                                             *  Значение цикла uint32_t
                                             */
#define TSIOC_GETFWVERSION   _TSIOC(0x0005) /* arg: Указатель на
                                             * Версия прошивки uint32_t
                                             * ценность
                                             * */
#define TSIOC_ENABLEGESTURE  _TSIOC(0x0006) /* arg: Указатель на
                                             * int для включения функции жестов
                                             */
#define TSIOC_DOACALIB       _TSIOC(0x0007) /* arg: нет.
                                             * Запустить автоматическую калибровку TS
                                             */
#define TSIOC_CALDATA        _TSIOC(0x0008) /* arg: Указатель на
                                             * структура g_tscaldata_s
                                             */
#define TSIOC_USESCALED      _TSIOC(0x0009) /* arg: блин, да/нет */
#define TSIOC_GETOFFSETX     _TSIOC(0x000a) /* arg: Указатель на
                                             * int значение смещения X
                                             */
#define TSIOC_GETOFFSETY     _TSIOC(0x000b) /* arg: Указатель на
                                             * int значение смещения Y
                                             */
#define TSIOC_GETTHRESHX     _TSIOC(0x000c) /* arg: Указатель на
                                             * int X пороговое значение
                                             */
#define TSIOC_GETTHRESHY     _TSIOC(0x000d) /* arg: Указатель на
                                             * int Y пороговое значение
                                             */

#define TSIOC_GRAB           _TSIOC(0x000e) /* arg: Указатель на
                                             * int для включения захвата
                                             */

#define TSIOC_GETMAXPOINTS   _TSIOC(0x000f) /* arg: Указатель на
                                             * uint8_t максимальная точка касания
                                             */
#define TSIOC_GETRESOLUTION  _TSIOC(0x0010) /* arg: Указатель на
                                             * структура touch_resolution_s
                                             */

/* Эти определения определяют значение всех битов, которые могут быть
 * сообщается в флагах структуры touch_point_s.
 */

#define TOUCH_DOWN           (1 << 0) /* Установлен новый сенсорный контакт */
#define TOUCH_MOVE           (1 << 1) /* Передвижение произошло при ранее зарегистрированном контакте */
#define TOUCH_UP             (1 << 2) /* Сенсорный контакт пропал */
#define TOUCH_ID_VALID       (1 << 3) /* Касание ID обязательно */
#define TOUCH_POS_VALID      (1 << 4) /* Аппаратное обеспечение предоставило допустимую позицию X/Y. */
#define TOUCH_PRESSURE_VALID (1 << 5) /* Аппаратное обеспечение обеспечивает допустимое давление */
#define TOUCH_SIZE_VALID     (1 << 6) /* Аппаратное обеспечение предоставило действительный размер контактов аппаратного обеспечения. */
#define TOUCH_GESTURE_VALID  (1 << 7) /* Аппаратное обеспечение предоставило действительный жест */

/**********************
 *      TYPEDEFS
 **********************/

struct touch_point_s {
    uint8_t  id;        /* Уникальный идентифицирует контакт; То же во всех отчетах по контакту */
    uint8_t  flags;     /* См. определения TOUCH_ * выше. */
    int16_t  x;         /* Координата X точки касания (некалиброванная) */
    int16_t  y;         /* Координата Y точки касания (некалиброванная) */
    int16_t  h;         /* Высота точки касания (некалиброванная) */
    int16_t  w;         /* Ширина точки касания (некалиброванная) */
    uint16_t gesture;   /* Жест касания сенсорного экрана */
    uint16_t pressure;  /* Сенсорное давление */
    uint16_t dummy;     /* Дополнено 2 байтами здесь */
    uint64_t timestamp; /* Отметка времени события касания, в микросекундах */
};

struct touch_sample_s {
    int32_t npoints;               /* Количество точек касания в точке[] */
    int32_t dummy;                 /* Дополнено 4 байтами здесь */
    struct touch_point_s point[1]; /* Фактический размер – n пунктов. */
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*NUTTX_INPUT_TOUCHSCREEN_H*/
