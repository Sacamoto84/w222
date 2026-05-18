/**
 * @file lv_ili9341.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_ili9341.h"

#if LV_USE_ILI9341

/*********************
 *      DEFINES
 *********************/

#define CMD_FRMCTR1     0xB1    /* Управление частотой кадров (в обычном режиме/полноцветах) */
#define CMD_FRMCTR2     0xB2    /* Управление частотой кадров (в режиме ожидания/8 цветов) */
#define CMD_FRMCTR3     0xB3    /* Управление частотой кадров (в частичном режиме/полноцветах) */
#define CMD_INVCTR      0xB4    /* Управление инверсией дисплея */
#define CMD_DFUNCTR     0xB6    /* Управление функциями дисплея */
#define CMD_PWCTR1      0xC0    /* Контроль мощности 1 */
#define CMD_PWCTR2      0xC1    /* Контроль мощности 2 */
#define CMD_VMCTR1      0xC5    /* VCOM Управление 1 */
#define CMD_VMCTR2      0xC7    /* VCOM Управление 2 */
#define CMD_PWCTRA      0xCB    /* Регулятор мощности А */
#define CMD_PWCTRB      0xCF    /* Регулятор мощности Б */
#define CMD_GMCTRP1     0xE0    /* Положительная гамма-коррекция */
#define CMD_GMCTRN1     0xE1    /* Отрицательная гамма-коррекция */
#define CMD_DTCTRA      0xE8    /* Контроль времени водителя A */
#define CMD_DTCTRB      0xEA    /* Контроль времени водителя B */
#define CMD_PONSEQ      0xED    /* Последовательность включения питания */
#define CMD_RDINDEX     0xD9    /* ili9341 */
#define CMD_IDXRD       0xDD    /* Только ILI9341, чтение индексного регистра управления */
#define CMD_ENA3G       0xF2    /* Включить 3 управления гаммой */
#define CMD_IFCTR       0xF6    /* Управление интерфейсом */
#define CMD_PRCTR       0xF7    /* Контроль соотношения насосов */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC CONSTANTS
 **********************/

/* создание команды на основе драйвера LovyanGFX ILI9341 */
static const uint8_t init_cmd_list[] = {
    CMD_PWCTRB,     3,  0x00, 0xC1, 0x30,
    CMD_PONSEQ,     4,  0x64, 0x03, 0x12, 0x81,
    CMD_DTCTRA,     3,  0x85, 0x00, 0x78,
    CMD_PWCTRA,     5,  0x39, 0x2C, 0x00, 0x34, 0x02,
    CMD_PRCTR,      1,  0x20,
    CMD_DTCTRB,     2,  0x00, 0x00,
    CMD_PWCTR1,     1,  0x23,
    CMD_PWCTR2,     1,  0x10,
    CMD_VMCTR1,     2,  0x3e, 0x28,
    CMD_VMCTR2,     1,  0x86,
    CMD_FRMCTR1,    2,  0x00, 0x13,
    CMD_DFUNCTR,    2,  0x0A, 0xA2,
    CMD_IFCTR,      3,  0x09, 0x30, 0x00,
    CMD_ENA3G,      1,  0x00,
    LV_LCD_CMD_SET_GAMMA_CURVE, 1, 0x01,
    CMD_GMCTRP1,    15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
    CMD_GMCTRN1,    15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
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

lv_display_t * lv_ili9341_create(uint32_t hor_res, uint32_t ver_res, lv_lcd_flag_t flags,
                                 lv_ili9341_send_cmd_cb_t send_cmd_cb, lv_ili9341_send_color_cb_t send_color_cb)
{
    lv_display_t * disp = lv_lcd_generic_mipi_create(hor_res, ver_res, flags, send_cmd_cb, send_color_cb);
    lv_lcd_generic_mipi_send_cmd_list(disp, init_cmd_list);
    return disp;
}

void lv_ili9341_set_gap(lv_display_t * disp, uint16_t x, uint16_t y)
{
    lv_lcd_generic_mipi_set_gap(disp, x, y);
}

void lv_ili9341_set_invert(lv_display_t * disp, bool invert)
{
    lv_lcd_generic_mipi_set_invert(disp, invert);
}

void lv_ili9341_set_gamma_curve(lv_display_t * disp, uint8_t gamma)
{
    lv_lcd_generic_mipi_set_gamma_curve(disp, gamma);
}

void lv_ili9341_send_cmd_list(lv_display_t * disp, const uint8_t * cmd_list)
{
    lv_lcd_generic_mipi_send_cmd_list(disp, cmd_list);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_ILI9341*/
