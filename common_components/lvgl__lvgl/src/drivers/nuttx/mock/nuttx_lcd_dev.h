/**
 * @file nuttx_lcd_dev.h
 *
 */

#ifndef NUTTX_LCD_DEV_H
#define NUTTX_LCD_DEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "nuttx_video_fb.h"

/*********************
 *      DEFINES
 *********************/

#define _LCDIOC(x) (x)

#define LCDDEVIO_PUTRUN       _LCDIOC(0)  /* Arg: константная структураlcddev_run_s* */
#define LCDDEVIO_PUTAREA      _LCDIOC(1)  /* Arg: константная структураlcddev_area_s* */
#define LCDDEVIO_GETRUN       _LCDIOC(2)  /* Arg: структураlcddev_run_s* */
#define LCDDEVIO_GETAREA      _LCDIOC(3)  /* Arg: структураlcddev_area_s* */
#define LCDDEVIO_GETPOWER     _LCDIOC(4)  /* Arg: интервал* */
#define LCDDEVIO_SETPOWER     _LCDIOC(5)  /* Arg: интервал */
#define LCDDEVIO_GETCONTRAST  _LCDIOC(6)  /* Arg: интервал* */
#define LCDDEVIO_SETCONTRAST  _LCDIOC(7)  /* Arg: беззнаковое целое число */
#define LCDDEVIO_GETPLANEINFO _LCDIOC(8)  /* Arg: структураlcd_planeinfo_s* */
#define LCDDEVIO_GETVIDEOINFO _LCDIOC(9)  /* Arg: структураfb_videoinfo_s* */
#define LCDDEVIO_SETPLANENO   _LCDIOC(10) /* Arg: интервал */

#define LCDDEVIO_GETAREAALIGN _LCDIOC(17) /* Arg: структураlcddev_area_align_s* */

/**********************
 *      TYPEDEFS
 **********************/

struct lcddev_area_s {
    fb_coord_t row_start, row_end;
    fb_coord_t col_start, col_end;
    fb_coord_t stride;               /* шаг строки в байтах */
    uint8_t * data;
};

/* Некоторые специальные драйверы LCD требуют выравнивания входных данных.
 * Например, начальная строка и столбец, ширина, высота, адрес данных и т. д.
 */

struct lcddev_area_align_s {
    uint16_t row_start_align; /* Начать выравнивание индекса строки */
    uint16_t height_align;    /* Выравнивание по высоте */
    uint16_t col_start_align; /* Начать выравнивание индекса столбца */
    uint16_t width_align;     /* Выравнивание по ширине */
    uint16_t buf_align;       /* Выравнивание адреса буфера */
};

/* Эта структура описывает одну цветовую плоскость.  Некоторые форматы YUV могут поддерживать
 * до 4 плоскостей (хотя они, вероятно, не будут использоваться на оборудовании LCD).
 * Драйвер кадрового буфера предоставляет адрес видеопамяти в своем
 * соответствующая структураfb_planeinfo_s.  Вместо этого драйверLCDобеспечивает
 * методы для передачи данных в/из цветовой плоскости LCD.
 */

struct lcd_planeinfo_s {
    /* Это рабочая память, выделяемая драйвером LCD для каждого устройства LCD.
    * и для каждой цветовой плоскости.  Эта память будет содержать одну растровую строку
    * данные. Поэтому размер выделенного буфера выполнения должен быть не менее
    * (бпп*xres/8).  Фактическое соревнование буфера должно соответствовать
    * разрядность базового типа пикселя.
    *
    * Если существует несколько плоскостей, они могут использовать один и тот же рабочий буфер.
    * потому что разные самолеты не будут работать одновременно.  Однако,
    * если имеется несколько устройств LCD, каждое из них должно иметь уникальный запуск
    * буферы.
    */

    uint8_t * buffer;

    /* Это количество бит в одном пикселе.  Это может быть один из {1, 2, 4,
     * 8, 16, 24 или 32}, если не поддерживается одно или несколько из этих разрешений.
     * был отключен.
     */

    uint8_t  bpp;
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

#endif /*NUTTX_LCD_DEV_H*/
