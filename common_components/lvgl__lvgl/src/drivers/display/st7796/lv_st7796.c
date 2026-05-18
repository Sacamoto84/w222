/**
 * @file lv_st7796.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_st7796.h"

#if LV_USE_ST7796

/*********************
 *      DEFINES
 *********************/

#define CMD_FRMCTR1     0xB1
#define CMD_FRMCTR2     0xB2
#define CMD_FRMCTR3     0xB3
#define CMD_INVCTR      0xB4
#define CMD_DFUNCTR     0xB6
#define CMD_ETMOD       0xB7
#define CMD_PWCTR1      0xC0
#define CMD_PWCTR2      0xC1
#define CMD_PWCTR3      0xC2
#define CMD_PWCTR4      0xC3
#define CMD_PWCTR5      0xC4
#define CMD_VMCTR       0xC5
#define CMD_GMCTRP1     0xE0
#define CMD_GMCTRN1     0xE1
#define CMD_DOCA        0xE8
#define CMD_CSCON       0xF0

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC CONSTANTS
 **********************/

/* команды инициализации на основе LovyanGFX */
static const uint8_t init_cmd_list[] = {
    CMD_CSCON,      1,  0xC3,                           /* Включить команду расширения 2 часть I */
    CMD_CSCON,      1,  0x96,                           /* Включить команду расширения 2, часть II */
    CMD_INVCTR,     1,  0x01,                           /* 1-точечная инверсия */
    CMD_DFUNCTR,    3,  0x80,                           /* Управление функцией дисплея: Байпас */
    0x22,                           /* Сканирование выхода источника от S1 до S960, сканирование выхода вентиля от G1 до G480, цикл сканирования = 2 */
    0x3B,                           /* LCD Приводная линия = 8 * (59 + 1) */
    CMD_DOCA,       8,  0x40, 0x8A, 0x00, 0x00,
    0x29,                           /* Время периода выравнивания источника = 22,5 мкс. */
    0x19,                           /* Время для «Начала ворот» = 25 (Tclk) */
    0xA5,                           /* Время для «Gate End» = 37 (Tclk), функция драйвера ворот EQ ON */
    0x33,
    CMD_PWCTR2,     1,  0x06,                           /* Управление мощностью2: VAP ( GVDD ) = 3,85 + (vcom + смещение vcom), VAN ( GVCL ) = -3,85 + (vcom + смещение vcom) */
    CMD_PWCTR3,     1,  0xA7,                           /* Регулятор мощности 3: Уровень управляющего тока источника = низкий, Уровень управляющего тока гамма = Высокий */
    CMD_VMCTR,      1,  0x18,                           /* VCOM Управление: VCOM = 0,9 */
    LV_LCD_CMD_DELAY_MS,    12,                         /* задержка 120 мс */
    CMD_GMCTRP1,    14,                                 /* Гамма */
    0xF0, 0x09, 0x0B, 0x06, 0x04, 0x15, 0x2F,
    0x54, 0x42, 0x3C, 0x17, 0x14, 0x18, 0x1B,
    CMD_GMCTRN1,    14,
    0xE0, 0x09, 0x0B, 0x06, 0x04, 0x03, 0x2B,
    0x43, 0x42, 0x3B, 0x16, 0x14, 0x17, 0x1B,
    LV_LCD_CMD_DELAY_MS, 12,                            /* задержка 120 мс */
    CMD_CSCON,      1,  0x3C,                           /* Отключить команду расширения 2 часть I */
    CMD_CSCON,      1,  0x69,                           /* Отключить команду расширения 2 часть II */
    LV_LCD_CMD_DELAY_MS, LV_LCD_CMD_EOF
};

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_st7796_create(uint32_t hor_res, uint32_t ver_res, lv_lcd_flag_t flags,
                                lv_st7796_send_cmd_cb_t send_cmd_cb, lv_st7796_send_color_cb_t send_color_cb)
{
    lv_display_t * disp = lv_lcd_generic_mipi_create(hor_res, ver_res, flags, send_cmd_cb, send_color_cb);
    lv_lcd_generic_mipi_send_cmd_list(disp, init_cmd_list);
    return disp;
}

void lv_st7796_set_gap(lv_display_t * disp, uint16_t x, uint16_t y)
{
    lv_lcd_generic_mipi_set_gap(disp, x, y);
}

void lv_st7796_set_invert(lv_display_t * disp, bool invert)
{
    lv_lcd_generic_mipi_set_invert(disp, invert);
}

void lv_st7796_set_gamma_curve(lv_display_t * disp, uint8_t gamma)
{
    /* NOTE: the generic method is not supported on ST7796, TODO: implement gamma tables */
    LV_UNUSED(disp);
    LV_UNUSED(gamma);
}

void lv_st7796_send_cmd_list(lv_display_t * disp, const uint8_t * cmd_list)
{
    lv_lcd_generic_mipi_send_cmd_list(disp, cmd_list);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_ST7796*/
