/**
 * @file lv_lcd_generic_mipi.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_lcd_generic_mipi.h"

#if LV_USE_GENERIC_MIPI

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void send_cmd(lv_lcd_generic_mipi_driver_t * drv, uint8_t cmd, uint8_t * param, size_t param_size);
static void send_color(lv_lcd_generic_mipi_driver_t * drv, uint8_t cmd, uint8_t * param, size_t param_size);
static void init(lv_lcd_generic_mipi_driver_t * drv, lv_lcd_flag_t flags);
static void set_mirror(lv_lcd_generic_mipi_driver_t * drv, bool mirror_x, bool mirror_y);
static void set_swap_xy(lv_lcd_generic_mipi_driver_t * drv, bool swap);
static void set_rotation(lv_lcd_generic_mipi_driver_t * drv, lv_display_rotation_t rot);
static void res_chg_event_cb(lv_event_t * e);
static void delete_cb(lv_event_t * e);
static lv_lcd_generic_mipi_driver_t * get_driver(lv_display_t * disp);
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_lcd_generic_mipi_create(uint32_t hor_res, uint32_t ver_res, lv_lcd_flag_t flags,
                                          lv_lcd_send_cmd_cb_t send_cmd_cb, lv_lcd_send_color_cb_t send_color_cb)
{
    lv_display_t * disp = lv_display_create(hor_res, ver_res);
    if(disp == NULL) {
        return NULL;
    }

    lv_lcd_generic_mipi_driver_t * drv = (lv_lcd_generic_mipi_driver_t *)lv_malloc(sizeof(lv_lcd_generic_mipi_driver_t));
    if(drv == NULL) {
        lv_display_delete(disp);
        return NULL;
    }

    /* инициализация структуры драйвера */
    drv->disp = disp;
    drv->send_cmd = send_cmd_cb;
    drv->send_color = send_color_cb;
    lv_display_set_driver_data(disp, (void *)drv);

    /* контроллер инициализации */
    init(drv, flags);

    /* зарегистрировать обратный вызов изменения разрешения ( NOTE : он также обрабатывает поворот экрана) */
    lv_display_add_event_cb(disp, res_chg_event_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);

    /* зарегистрировать обратный вызов удаления объекта для освобождения структуры драйвера */
    lv_display_add_event_cb(disp, delete_cb, LV_EVENT_DELETE, NULL);

    /* зарегистрировать сброс обратного вызова */
    lv_display_set_flush_cb(disp, flush_cb);

    return disp;
}

void lv_lcd_generic_mipi_set_gap(lv_display_t * disp, uint16_t x, uint16_t y)
{
    lv_lcd_generic_mipi_driver_t * drv = get_driver(disp);
    drv->x_gap = x;
    drv->y_gap = y;
}

void lv_lcd_generic_mipi_set_invert(lv_display_t * disp, bool invert)
{
    lv_lcd_generic_mipi_driver_t * drv = get_driver(disp);
    send_cmd(drv, invert ? LV_LCD_CMD_ENTER_INVERT_MODE : LV_LCD_CMD_EXIT_INVERT_MODE, NULL, 0);
}

void lv_lcd_generic_mipi_set_address_mode(lv_display_t * disp, bool mirror_x, bool mirror_y, bool swap_xy, bool bgr)
{
    lv_lcd_generic_mipi_driver_t * drv = get_driver(disp);
    uint8_t mad = drv->madctl_reg & ~(LV_LCD_MASK_RGB_ORDER);
    if(bgr) {
        mad |= LV_LCD_BIT_RGB_ORDER__BGR;
    }
    drv->madctl_reg = mad;
    drv->mirror_x = mirror_x;
    drv->mirror_y = mirror_y;
    drv->swap_xy = swap_xy;
    set_rotation(drv, lv_display_get_rotation(disp));   /* экран обновления */
}

void lv_lcd_generic_mipi_set_gamma_curve(lv_display_t * disp, uint8_t gamma)
{
    lv_lcd_generic_mipi_driver_t * drv = get_driver(disp);
    send_cmd(drv, LV_LCD_CMD_SET_GAMMA_CURVE, (uint8_t[]) {
        gamma,
    }, 1);
}

void lv_lcd_generic_mipi_send_cmd_list(lv_display_t * disp, const uint8_t * cmd_list)
{
    lv_lcd_generic_mipi_driver_t * drv = get_driver(disp);
    while(1) {
        uint8_t cmd = *cmd_list++;
        uint8_t num = *cmd_list++;
        if(cmd == LV_LCD_CMD_DELAY_MS) {
            if(num == LV_LCD_CMD_EOF)   /* конец списка */
                break;
            else {                      /* задержка с шагом 10 мс*/
                lv_delay_ms((uint32_t)(num) * 10);
            }
        }
        else {
            drv->send_cmd(drv->disp, &cmd, 1, cmd_list, num);
            cmd_list += num;
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Вспомогательная функция для вызова предоставленной пользователем функции send_cmd.
 * @param drv           LCD driver object
 * @param cmd           command byte
 * @param param         parameter buffer
 * @param param_size    number of bytes of the parameters
 */
static void send_cmd(lv_lcd_generic_mipi_driver_t * drv, uint8_t cmd, uint8_t * param, size_t param_size)
{
    uint8_t cmdbuf = cmd;       /* MIPI использует 8-битные команды. */
    drv->send_cmd(drv->disp, &cmdbuf, 1, param, param_size);
}

/**
 * Вспомогательная функция для вызова предоставленной пользователем функции send_color.
 * @param drv           LCD driver object
 * @param cmd           command byte
 * @param param         parameter buffer
 * @param param_size    number of bytes of the parameters
 */
static void send_color(lv_lcd_generic_mipi_driver_t * drv, uint8_t cmd, uint8_t * param, size_t param_size)
{
    uint8_t cmdbuf = cmd;       /* MIPI использует 8-битные команды. */
    drv->send_color(drv->disp, &cmdbuf, 1, param, param_size);
    /* note: LVGL waits for your callback to call `lv_display_flush_ready` to know when the transfer has finished. */
}

/**
 * Инициализируйте драйвер LCD после аппаратного сброса
 * @param drv           LCD driver object
 */
static void init(lv_lcd_generic_mipi_driver_t * drv, lv_lcd_flag_t flags)
{
    drv->x_gap = 0;
    drv->y_gap = 0;

    /* цветовой режим инициализации и порядок RGB */
    drv->madctl_reg = flags & LV_LCD_FLAG_BGR ? LV_LCD_BIT_RGB_ORDER__BGR : LV_LCD_BIT_RGB_ORDER__RGB;
    drv->colmod_reg = flags & LV_LCD_FLAG_RGB666 ? LV_LCD_PIXEL_FORMAT_RGB666 : LV_LCD_PIXEL_FORMAT_RGB565;

    /* начальная ориентация */
    drv->mirror_x = flags & LV_LCD_FLAG_MIRROR_X;
    drv->mirror_y = flags & LV_LCD_FLAG_MIRROR_Y;
    drv->swap_xy = false;
    /* обновить madctl_reg */
    set_swap_xy(drv, drv->swap_xy);
    set_mirror(drv, drv->mirror_x, drv->mirror_y);

    /* сначала войдите в спящий режим */
    send_cmd(drv, LV_LCD_CMD_ENTER_SLEEP_MODE, NULL, 0);
    lv_delay_ms(10);

    /* выполнить программный сброс */
    send_cmd(drv, LV_LCD_CMD_SOFT_RESET, NULL, 0);
    lv_delay_ms(200);

    /* LCD переходит в спящий режим, и дисплей выключается после перезагрузки при включении питания, сначала выйдите из спящего режима. */
    send_cmd(drv, LV_LCD_CMD_EXIT_SLEEP_MODE, NULL, 0);
    lv_delay_ms(300);

    send_cmd(drv, LV_LCD_CMD_ENTER_NORMAL_MODE, NULL, 0);

    send_cmd(drv, LV_LCD_CMD_SET_ADDRESS_MODE, (uint8_t[]) {
        drv->madctl_reg,
    }, 1);
    send_cmd(drv, LV_LCD_CMD_SET_PIXEL_FORMAT, (uint8_t[]) {
        drv->colmod_reg,
    }, 1);
    send_cmd(drv, LV_LCD_CMD_SET_DISPLAY_ON, NULL, 0);
}

/**
 * Установить направления считывания (используется для вращения дисплея)
 * @param drv           LCD driver object
 * @param mirror_x      false: normal, true: mirrored
 * @param mirror_y      false: normal, true: mirrored
 */
static void set_mirror(lv_lcd_generic_mipi_driver_t * drv, bool mirror_x, bool mirror_y)
{
    uint8_t mad = drv->madctl_reg & ~(LV_LCD_MASK_COLUMN_ADDRESS_ORDER | LV_LCD_MASK_PAGE_ADDRESS_ORDER);
    if(mirror_x) {
        mad |= LV_LCD_BIT_COLUMN_ADDRESS_ORDER__RTOL;
    }
    if(mirror_y) {
        mad |= LV_LCD_BIT_PAGE_ADDRESS_ORDER__BTOT;
    }
    drv->madctl_reg = mad;
}

/**
 * Поменяйте местами горизонтальное и вертикальное показания (используется для поворота дисплея)
 * @param drv           LCD driver object
 * @param swap          false: normal, true: swapped
 */
static void set_swap_xy(lv_lcd_generic_mipi_driver_t * drv, bool swap)
{
    uint8_t mad = drv->madctl_reg & ~(LV_LCD_MASK_PAGE_COLUMN_ORDER);
    if(swap) {
        mad |= LV_LCD_BIT_PAGE_COLUMN_ORDER__REVERSE;
    }
    drv->madctl_reg = mad;
}

/**
 * Сбросить буфер дисплея в LCD.
 * @param disp          display object
 * @param hor_res       horizontal resolution
 * @param area          area stored in the buffer
 * @param px_map        buffer containing pixel data
 * @note                transfers pixel data to the LCD controller using the callbacks 'send_cmd' and 'send_color', which were
 *                      передается в функцию 'lv_st7789_create()'
 */
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    lv_lcd_generic_mipi_driver_t * drv = get_driver(disp);

    int32_t x_start = area->x1;
    int32_t x_end = area->x2 + 1;
    int32_t y_start = area->y1;
    int32_t y_end = area->y2 + 1;

    LV_ASSERT((x_start < x_end) && (y_start < y_end) && "start position must be smaller than end position");

    x_start += drv->x_gap;
    x_end += drv->x_gap;
    y_start += drv->y_gap;
    y_end += drv->y_gap;

    /* определить область памяти кадров, к которой MCU может получить доступ */
    send_cmd(drv, LV_LCD_CMD_SET_COLUMN_ADDRESS, (uint8_t[]) {
        (x_start >> 8) & 0xFF,
        x_start & 0xFF,
        ((x_end - 1) >> 8) & 0xFF,
        (x_end - 1) & 0xFF,
    }, 4);
    send_cmd(drv, LV_LCD_CMD_SET_PAGE_ADDRESS, (uint8_t[]) {
        (y_start >> 8) & 0xFF,
        y_start & 0xFF,
        ((y_end - 1) >> 8) & 0xFF,
        (y_end - 1) & 0xFF,
    }, 4);
    /* буфер кадра передачи */
    size_t len = (x_end - x_start) * (y_end - y_start) * lv_color_format_get_size(lv_display_get_color_format(disp));
    send_color(drv, LV_LCD_CMD_WRITE_MEMORY_START, px_map, len);
}

/**
 * Установите вращение с учетом текущих настроек зеркала и подкачки
 * @param drv           LCD driver object
 * @param rot           rotation
 */
static void set_rotation(lv_lcd_generic_mipi_driver_t * drv, lv_display_rotation_t rot)
{
    switch(rot) {
        case LV_DISPLAY_ROTATION_0:
            set_swap_xy(drv, drv->swap_xy);
            set_mirror(drv, drv->mirror_x, drv->mirror_y);
            break;
        case LV_DISPLAY_ROTATION_90:
            set_swap_xy(drv, !drv->swap_xy);
            set_mirror(drv, drv->mirror_x, !drv->mirror_y);
            break;
        case LV_DISPLAY_ROTATION_180:
            set_swap_xy(drv, drv->swap_xy);
            set_mirror(drv, !drv->mirror_x, !drv->mirror_y);
            break;
        case LV_DISPLAY_ROTATION_270:
            set_swap_xy(drv, !drv->swap_xy);
            set_mirror(drv, !drv->mirror_x, drv->mirror_y);
            break;
    }
    send_cmd(drv, LV_LCD_CMD_SET_ADDRESS_MODE, (uint8_t[]) {
        drv->madctl_reg
    }, 1);
}

/**
 * Обработка события LV_EVENT_RESOLUTION_CHANGED (обрабатывает изменение разрешения и поворота)
 * @param e             LV_EVENT_RESOLUTION_CHANGED event
 */
static void res_chg_event_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_current_target(e);
    lv_lcd_generic_mipi_driver_t * drv = get_driver(disp);

    uint16_t hor_res = lv_display_get_horizontal_resolution(disp);
    uint16_t ver_res = lv_display_get_vertical_resolution(disp);
    lv_display_rotation_t rot = lv_display_get_rotation(disp);

    /* TODO: implement resolution change */
    LV_UNUSED(hor_res);
    LV_UNUSED(ver_res);

    /* вращение ручки */
    set_rotation(drv, rot);
}

static void delete_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_current_target(e);
    lv_lcd_generic_mipi_driver_t * drv = get_driver(disp);
    LV_ASSERT_NULL(drv);
    lv_free(drv);
    lv_display_set_driver_data(disp, NULL);
}

static lv_lcd_generic_mipi_driver_t * get_driver(lv_display_t * disp)
{
    return (lv_lcd_generic_mipi_driver_t *)lv_display_get_driver_data(disp);
}

#endif /*LV_USE_GENERIC_MIPI*/
