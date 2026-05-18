/**
 * @file lv_port_lcd_stm32_template.c
 *
 * Пример реализации драйверов дисплея LVGL LCD на платформе STM32
 */

/*Скопируйте этот файл как «lv_port_disp.c» и установите для этого значения значение «1», чтобы отключить контент.*/
#if 0

/*********************
 *      INCLUDES
 *********************/
/* Включите сюда файлы STM32Cube, например:
#включить "stm32f7xx_hal.h"
*/

#include "lv_port_disp.h"
#include "./src/drivers/display/st7789/lv_st7789.h"

/*********************
 *      DEFINES
 *********************/
#ifndef MY_DISP_HOR_RES
    #warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen width, default value 320 is used for now.
    #define MY_DISP_HOR_RES    320
#endif

#ifndef MY_DISP_VER_RES
    #warning Please define or replace the macro MY_DISP_VER_RES with the actual screen height, default value 240 is used for now.
    #define MY_DISP_VER_RES    240
#endif

#define BUS_SPI1_POLL_TIMEOUT 0x1000U

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lcd_color_transfer_ready_cb(SPI_HandleTypeDef * hspi);
static int32_t lcd_io_init(void);
static void lcd_send_cmd(lv_display_t * disp, const uint8_t * cmd, size_t cmd_size, const uint8_t * param,
                         size_t param_size);
static void lcd_send_color(lv_display_t * disp, const uint8_t * cmd, size_t cmd_size, uint8_t * param,
                           size_t param_size);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_display_t * lcd_disp;
static volatile int lcd_bus_busy = 0;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_display_init(void)
{
    /* Инициализация ввода-вывода LCD */
    if(lcd_io_init() != 0)
        return;

    /* Создайте объект отображения LVGL и драйвер дисплея ST7789 LCD. */
    lcd_disp = lv_st7789_create(MY_DISP_HOR_RES, MY_DISP_VER_RES, LV_LCD_FLAG_NONE, lcd_send_cmd, lcd_send_color);
    lv_display_set_rotation(lcd_disp, LV_DISPLAY_ROTATION_270);     /* установить альбомную ориентацию */

    /* Example: два динамически выделяемых буфера для частичного рендеринга */
    uint8_t * buf1 = NULL;
    uint8_t * buf2 = NULL;

    uint32_t buf_size = MY_DISP_HOR_RES * MY_DISP_VER_RES / 10 * lv_color_format_get_size(lv_display_get_color_format(
                                                                                              lcd_disp));

    buf1 = lv_malloc(buf_size);
    if(buf1 == NULL) {
        LV_LOG_ERROR("display draw buffer malloc failed");
        return;
    }

    buf2 = lv_malloc(buf_size);
    if(buf2 == NULL) {
        LV_LOG_ERROR("display buffer malloc failed");
        lv_free(buf1);
        return;
    }
    lv_display_set_buffers(lcd_disp, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Обратный вызов вызывается, когда фоновая передача завершена */
static void lcd_color_transfer_ready_cb(SPI_HandleTypeDef * hspi)
{
    /* CS высокий */
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
    lcd_bus_busy = 0;
    lv_display_flush_ready(lcd_disp);
}

/* Инициализируйте шину ввода-вывода LCD, сбросьте LCD. */
static int32_t lcd_io_init(void)
{
    /* РегистрацияSPITx Полный обратный вызов */
    HAL_SPI_RegisterCallback(&hspi1, HAL_SPI_TX_COMPLETE_CB_ID, lcd_color_transfer_ready_cb);

    /* сброс LCD */
    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(100);

    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_SET);

    return HAL_OK;
}

/* Реализация функции отправки команды LCD для конкретной платформы. В общем случае следует использовать передачу по запросу. */
static void lcd_send_cmd(lv_display_t * disp, const uint8_t * cmd, size_t cmd_size, const uint8_t * param,
                         size_t param_size)
{
    LV_UNUSED(disp);
    while(lcd_bus_busy);    /* дождитесь завершения предыдущей передачи */
    /* Установите SPI в 8-битный режим. */
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    HAL_SPI_Init(&hspi1);
    /* DCX низкий уровень (команда) */
    HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_RESET);
    /* CS низкий */
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    /* отправить команду */
    if(HAL_SPI_Transmit(&hspi1, cmd, cmd_size, BUS_SPI1_POLL_TIMEOUT) == HAL_OK) {
        /* DCX высокий (данные) */
        HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_SET);
        /* для коротких блоков данных мы используем передачу по запросу */
        HAL_SPI_Transmit(&hspi1, (uint8_t *)param, (uint16_t)param_size, BUS_SPI1_POLL_TIMEOUT);
        /* CS высокий */
        HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
    }
}

/* Специфическая для платформы реализация функции отправки цвета LCD. Для повышения производительности следует использовать передачу DMA.
 * В случае передачи DMA должен быть установлен обратный вызов для уведомления LVGL об окончании передачи.
 */
static void lcd_send_color(lv_display_t * disp, const uint8_t * cmd, size_t cmd_size, uint8_t * param,
                           size_t param_size)
{
    LV_UNUSED(disp);
    while(lcd_bus_busy);    /* дождитесь завершения предыдущей передачи */
    /* Установите SPI в 8-битный режим. */
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    HAL_SPI_Init(&hspi1);
    /* DCX низкий уровень (команда) */
    HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_RESET);
    /* CS низкий */
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    /* отправить команду */
    if(HAL_SPI_Transmit(&hspi1, cmd, cmd_size, BUS_SPI1_POLL_TIMEOUT) == HAL_OK) {
        /* DCX высокий (данные) */
        HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_SET);
        /* для цветовых данных используйте передачу DMA */
        /* Установите SPI в 16-битный режим, чтобы соответствовать порядку байтов. */
        hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
        HAL_SPI_Init(&hspi1);
        lcd_bus_busy = 1;
        HAL_SPI_Transmit_DMA(&hspi1, param, (uint16_t)param_size / 2);
        /* NOTE: CS будет сброшен в обратном вызове готовности к передаче. */
    }
}

#else /*Включите этот файл вверху*/

/*Этот вымышленный typedef существует исключительно для того, чтобы успеть замолчать -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
