/**
 * @file lv_lcd_generic_mipi.h
 *
 * Общий драйвер для контроллеров, соответствующих спецификации MIPI DBI/DCS.
 *
 * Работает с:
 *
 * ST7735
 * ST7789
 * ST7796
 * ILI9341
 * ILI9488 ( NOTE : в режиме SPI ILI9488 поддерживает только режим RGB666, который в настоящее время не поддерживается)
 *
 * возможно, еще много
 *
 */

#ifndef LV_LCD_GENERIC_MIPI_H
#define LV_LCD_GENERIC_MIPI_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../display/lv_display.h"

#if LV_USE_GENERIC_MIPI

/*********************
 *      DEFINES
 *********************/

/* MIPI DCS (Набор команд дисплея) v1.02.00 Набор пользовательских команд */
#define LV_LCD_CMD_NOP                      0x00    /* Нет операции */
#define LV_LCD_CMD_SOFT_RESET               0x01    /* Программный сброс */
#define LV_LCD_CMD_GET_POWER_MODE           0x0A    /* Получить текущий режим питания */
#define LV_LCD_CMD_GET_ADDRESS_MODE         0x0B    /* Получите порядок данных для передачи от хоста к модулю дисплея и из памяти кадра на устройство отображения. */
#define LV_LCD_CMD_GET_PIXEL_FORMAT         0x0C    /* Получить текущий формат пикселей */
#define LV_LCD_CMD_GET_DISPLAY_MODE         0x0D    /* Получить текущий режим отображения с периферийного устройства */
#define LV_LCD_CMD_GET_SIGNAL_MODE          0x0E    /* Получить режим сигнализации модуля дисплея */
#define LV_LCD_CMD_GET_DIAGNOSTIC_RESULT    0x0F    /* Получите результат самодиагностики периферийного устройства */
#define LV_LCD_CMD_ENTER_SLEEP_MODE         0x10    /* Питание панели дисплея отключено */
#define LV_LCD_CMD_EXIT_SLEEP_MODE          0x11    /* Питание панели дисплея включено */
#define LV_LCD_CMD_ENTER_PARTIAL_MODE       0x12    /* Часть площади дисплея используется для отображения изображений. */
#define LV_LCD_CMD_ENTER_NORMAL_MODE        0x13    /* Вся область дисплея используется для отображения изображения. */
#define LV_LCD_CMD_EXIT_INVERT_MODE         0x20    /* Цвета отображаемого изображения не инвертируются. */
#define LV_LCD_CMD_ENTER_INVERT_MODE        0x21    /* Цвета отображаемого изображения инвертированы */
#define LV_LCD_CMD_SET_GAMMA_CURVE          0x26    /* Выбор гамма-кривой, используемой устройством отображения. */
#define LV_LCD_CMD_SET_DISPLAY_OFF          0x28    /* Отключает устройство отображения */
#define LV_LCD_CMD_SET_DISPLAY_ON           0x29    /* Показать изображение на устройстве отображения */
#define LV_LCD_CMD_SET_COLUMN_ADDRESS       0x2A    /* Установите размер столбца */
#define LV_LCD_CMD_SET_PAGE_ADDRESS         0x2B    /* Установите экстент страницы */
#define LV_LCD_CMD_WRITE_MEMORY_START       0x2C    /* Перенесите данные изображения с хост-процессора на периферийное устройство, исходя из местоположения, указанногоset_column_addressиset_page_address. */
#define LV_LCD_CMD_READ_MEMORY_START        0x2E    /* Передача данных с периферийного устройства на интерфейс хост-процессора, начиная с местоположения, указанногоset_column_addressиset_page_address. */
#define LV_LCD_CMD_SET_PARTIAL_ROWS         0x30    /* Определяет количество строк в частичной области отображения на устройстве отображения. */
#define LV_LCD_CMD_SET_PARTIAL_COLUMNS      0x31    /* Определяет количество столбцов в частичной области отображения на устройстве отображения. */
#define LV_LCD_CMD_SET_SCROLL_AREA          0x33    /* Определяет вертикальную прокрутку и фиксированную область на устройстве отображения. */
#define LV_LCD_CMD_SET_TEAR_OFF             0x34    /* Информация о синхронизации не отправляется с модуля дисплея на хост-процессор. */
#define LV_LCD_CMD_SET_TEAR_ON              0x35    /* Информация о синхронизации отправляется с модуля дисплея на хост-процессор в начале VFP. */
#define LV_LCD_CMD_SET_ADDRESS_MODE         0x36    /* Установите порядок передачи данных от хоста к модулю дисплея и из памяти кадров на устройство отображения. */
#define LV_LCD_CMD_SET_SCROLL_START         0x37    /* Определяет начальную точку вертикальной прокрутки */
#define LV_LCD_CMD_EXIT_IDLE_MODE           0x38    /* На панели дисплея используется полная глубина цвета. */
#define LV_LCD_CMD_ENTER_IDLE_MODE          0x39    /* На панели дисплея используется уменьшенная глубина цвета. */
#define LV_LCD_CMD_SET_PIXEL_FORMAT         0x3A    /* Определяет, сколько бит на пиксель используется в интерфейсе. */
#define LV_LCD_CMD_WRITE_MEMORY_CONTINUE    0x3C    /* Передача информации изображения из интерфейса хост-процессора на периферийное устройство из последнего записанного места. */
#define LV_LCD_CMD_READ_MEMORY_CONTINUE     0x3E    /* Считайте данные изображения с периферийного устройства, продолжающегося после последнегоread_memory_continueили read_memory_start. */
#define LV_LCD_CMD_SET_TEAR_SCANLINE        0x44    /* Информация о синхронизации отправляется с модуля дисплея на главный процессор, когда обновление устройства отображения достигает предоставленной строки развертки. */
#define LV_LCD_CMD_GET_SCANLINE             0x45    /* Получить текущую строку сканирования */
#define LV_LCD_CMD_READ_DDB_CONTINUE        0xA8    /* Продолжить чтение DDB с места последнего чтения. */
#define LV_LCD_CMD_READ_DDB_START           0xA1    /* Прочтите DDB из указанного места. */

/* маски флагов режима адреса */
#define LV_LCD_MASK_FLIP_VERTICAL                       (1 << 0)    /* Этот бит переворачивает изображение, отображаемое на устройстве отображения, сверху вниз. Никаких изменений в памяти кадров не происходит. */
#define LV_LCD_MASK_FLIP_HORIZONTAL                     (1 << 1)    /* Этот бит переворачивает изображение, показанное на устройстве отображения, слева направо. Никаких изменений в памяти кадров не происходит. */
#define LV_LCD_MASK_DATA_LATCH_DATA_ORDER               (1 << 2)    /* Отображение порядка фиксации данных */
#define LV_LCD_MASK_RGB_ORDER                           (1 << 3)    /* RGB / BGR Заказ */
#define LV_LCD_MASK_LINE_ADDRESS_ORDER                  (1 << 4)    /* Порядок адресов линий */
#define LV_LCD_MASK_PAGE_COLUMN_ORDER                   (1 << 5)    /* Порядок страниц/столбцов */
#define LV_LCD_MASK_COLUMN_ADDRESS_ORDER                (1 << 6)    /* Порядок адресов столбцов */
#define LV_LCD_MASK_PAGE_ADDRESS_ORDER                  (1 << 7)    /* Порядок адресов страниц */

#define LV_LCD_BIT_FLIP_VERTICAL__NOT_FLIPPED           0
#define LV_LCD_BIT_FLIP_VERTICAL__FLIPPED               LV_LCD_MASK_FLIP_VERTICAL           /* Этот бит переворачивает изображение, отображаемое на устройстве отображения, сверху вниз. Никаких изменений в памяти кадров не происходит. */
#define LV_LCD_BIT_FLIP_HORIZONTAL__NOT_FLIPPED         0
#define LV_LCD_BIT_FLIP_HORIZONTAL__FLIPPED             LV_LCD_MASK_FLIP_HORIZONTAL         /* Этот бит переворачивает изображение, показанное на устройстве отображения, слева направо. Никаких изменений в памяти кадров не происходит. */
#define LV_LCD_BIT_DATA_LATCH_DATA_ORDER__LTOR          0                                   /* Порядок фиксации данных на дисплее: LCD Обновить слева направо */
#define LV_LCD_BIT_DATA_LATCH_DATA_ORDER__RTOL          LV_LCD_MASK_DATA_LATCH_DATA_ORDER   /* Порядок фиксации данных на дисплее: LCD Обновить справа налево */
#define LV_LCD_BIT_RGB_ORDER__RGB                       0                                   /* RGB / BGR Заказ: RGB */
#define LV_LCD_BIT_RGB_ORDER__BGR                       LV_LCD_MASK_RGB_ORDER               /* RGB / BGR Заказ: BGR */
#define LV_LCD_BIT_LINE_ADDRESS_ORDER__TTOB             0                                   /* Порядок адресов строк: LCD Обновить сверху вниз */
#define LV_LCD_BIT_LINE_ADDRESS_ORDER__BTOT             LV_LCD_MASK_LINE_ADDRESS_ORDER      /* Порядок адресов строк: LCD Обновить снизу вверх */
#define LV_LCD_BIT_PAGE_COLUMN_ORDER__NORMAL            0                                   /* Порядок страниц/столбцов: обычный режим */
#define LV_LCD_BIT_PAGE_COLUMN_ORDER__REVERSE           LV_LCD_MASK_PAGE_COLUMN_ORDER       /* Порядок страниц/столбцов: обратный режим */
#define LV_LCD_BIT_COLUMN_ADDRESS_ORDER__LTOR           0                                   /* Порядок адресов столбцов: слева направо */
#define LV_LCD_BIT_COLUMN_ADDRESS_ORDER__RTOL           LV_LCD_MASK_COLUMN_ADDRESS_ORDER    /* Порядок адресов столбцов: справа налево. */
#define LV_LCD_BIT_PAGE_ADDRESS_ORDER__TTOB             0                                   /* Порядок адресов страниц: сверху вниз. */
#define LV_LCD_BIT_PAGE_ADDRESS_ORDER__BTOT             LV_LCD_MASK_PAGE_ADDRESS_ORDER      /* Порядок адресов страниц: снизу вверх */

/* предопределенные гамма-кривые */
#define LV_LCD_GAMMA_2_2                                0x01    /* 2.2 */
#define LV_LCD_GAMMA_1_8                                0x02    /* 1.8 */
#define LV_LCD_GAMMA_2_5                                0x04    /* 2.5 */
#define LV_LCD_GAMMA_1_0                                0x08    /* 1.0 */

/* распространенные форматы пикселей */
#define LV_LCD_PIXEL_FORMAT_RGB565                      0x55    /* bus: 16 бит, пиксель: 16 бит */
#define LV_LCD_PIXEL_FORMAT_RGB666                      0x66    /* bus: 18 бит, пиксель: 18 бит */

/* флаги дляlv_lcd_xxx_create() */
#define LV_LCD_FLAG_NONE                                0x00000000UL
#define LV_LCD_FLAG_MIRROR_X                            0x00000001UL
#define LV_LCD_FLAG_MIRROR_Y                            0x00000002UL
#define LV_LCD_FLAG_BGR                                 0x00000008UL
#define LV_LCD_FLAG_RGB666                              0x00000010UL

/* список команд */
#define LV_LCD_CMD_DELAY_MS     0xff
#define LV_LCD_CMD_EOF          0xff

/**********************
 *      TYPEDEFS
 **********************/

/**
 *  Флаги конфигурации дляlv_lcd_xxx_create()
 *
 */
typedef uint32_t lv_lcd_flag_t;

/**
 * Прототип платформозависимого обратного вызова для передачи команд и данных контроллеру LCD.
 * @param param_size          объект отображения
 * @param cmd           командный буфер (также может обрабатывать 16-битные команды)
 * @param cmd_size      количество байтов команды
 * @param param         буфер параметров
 * @param param_size    количество байтов параметров
 */
typedef void (*lv_lcd_send_cmd_cb_t)(lv_display_t * disp, const uint8_t * cmd, size_t cmd_size, const uint8_t * param,
                                     size_t param_size);

/**
 * Прототип платформозависимого обратного вызова для передачи данных пикселей в контроллер LCD.
 * @param param_size          объект отображения
 * @param cmd           командный буфер (также может обрабатывать 16-битные команды)
 * @param cmd_size      количество байтов команды
 * @param param         буфер параметров
 * @param param_size    количество байтов параметров
 */
typedef void (*lv_lcd_send_color_cb_t)(lv_display_t * disp, const uint8_t * cmd, size_t cmd_size, uint8_t * param,
                                       size_t param_size);

/**
 * Универсальный MIPI-совместимый драйвер LCD
 */
typedef struct {
    lv_display_t      *     disp;          /* связанный экранный объект LVGL */
    lv_lcd_send_cmd_cb_t    send_cmd;       /* реализация для конкретной платформы для отправки команды на контроллер LCD */
    lv_lcd_send_color_cb_t  send_color;     /* реализация для конкретной платформы для отправки данных пикселей на контроллер LCD */
    uint16_t                x_gap;          /* смещение x пикселя (0,0) в VRAM */
    uint16_t                y_gap;          /* смещение y пикселя (0,0) в VRAM */
    uint8_t                 madctl_reg;     /* текущее значение регистра MADCTL */
    uint8_t                 colmod_reg;     /* текущее значение регистра COLMOD */
    bool                    mirror_x;
    bool                    mirror_y;
    bool                    swap_xy;
} lv_lcd_generic_mipi_driver_t;

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте MIPI DCS-совместимый дисплей LCD.
 * @param hor_res       горизонтальное разрешение
 * @param ver_res       вертикальное разрешение
 * @param flags         настройки конфигурации по умолчанию (зеркало, порядокRGBи т. д.)
 * @param send_cmd_cb      платформо-зависимая функция для отправки команды контроллеруLCD(обычно используется передача по опросу)
 * @param send_color_cb    платформо-зависимая функция для отправки данных пикселей на контроллерLCD(обычно используется передача DMA).
 *                      `lv_display_flush_ready` должен быть вызван после завершения передачи.
 * @return              указатель на созданный дисплей
 */
lv_display_t * lv_lcd_generic_mipi_create(uint32_t hor_res, uint32_t ver_res, lv_lcd_flag_t flags,
                                          lv_lcd_send_cmd_cb_t send_cmd_cb, lv_lcd_send_color_cb_t send_color_cb);

/**
 * Установите зазор, то есть смещение пикселя (0,0) в VRAM.
 * @param disp          объект отображения
 * @param x             смещение по оси x
 * @param y             смещение по оси y
 */
void lv_lcd_generic_mipi_set_gap(lv_display_t * disp, uint16_t x, uint16_t y);

/**
 * Установить инверсию цвета
 * @param disp          объект отображения
 * @param invert        false: нормально, true: инвертировать
 */
void lv_lcd_generic_mipi_set_invert(lv_display_t * disp, bool invert);

/**
 * Установить режим адреса
 * @param disp          объект отображения
 * @param mirror_x      горизонтальное зеркало (false: нормальное, true: зеркальное)
 * @param mirror_y      вертикальное зеркало (false: нормальное, true: зеркальное)
 * @param swap_xy       поменять местами оси (false: нормально, true: поменять местами)
 * @param bgr           ПорядокRGB/BGR(ложь: RGB, правда: BGR)
 */
void lv_lcd_generic_mipi_set_address_mode(lv_display_t * disp, bool mirror_x, bool mirror_y, bool swap_xy, bool bgr);

/**
 * Установить гамма-кривую
 * @param disp          объект отображения
 * @param gamma         гамма-кривая
 */
void lv_lcd_generic_mipi_set_gamma_curve(lv_display_t * disp, uint8_t gamma);

/**
 * Отправить список команд.
 * @param disp          объект отображения
 * @param cmd_list      команды контроллера и панели
 */
void lv_lcd_generic_mipi_send_cmd_list(lv_display_t * disp, const uint8_t * cmd_list);

/**********************
 *      OTHERS
 **********************/

/**********************
 *      MACROS
 **********************/


#endif /*LV_USE_GENERIC_MIPI*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_LCD_GENERIC_MIPI_H*/
