/**
 * @file lv_port_disp_template.c
 *
 */

/*Скопируйте этот файл как «lv_port_disp.c» и установите для этого значения значение «1», чтобы отключить контент.*/
#if 0

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp_template.h"
#include <stdbool.h>

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

#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) /*будет 2 для RGB565 */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Инициализируйте свой дисплей
     * -----------------------*/
    disp_init();

    /*------------------------------------
     * Создайте устройство и установите flush_cb.
     * -----------------------------------*/
    lv_display_t * disp = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_display_set_flush_cb(disp, disp_flush);

    /* Пример 1
     * Один буфер для частичного рендеринга*/
    LV_ATTRIBUTE_MEM_ALIGN
    static uint8_t buf_1_1[MY_DISP_HOR_RES * 10 * BYTE_PER_PIXEL];            /*Буфер на 10 строк*/
    lv_display_set_buffers(disp, buf_1_1, NULL, sizeof(buf_1_1), LV_DISPLAY_RENDER_MODE_PARTIAL);

    /* Пример 2
     * Два буфера для частичного рендеринга
     * Вflush_cbDMA или аналогичном оборудовании должно быть предусмотрено обновление мониторов в фоновом режиме.*/
    LV_ATTRIBUTE_MEM_ALIGN
    static uint8_t buf_2_1[MY_DISP_HOR_RES * 10 * BYTE_PER_PIXEL];

    LV_ATTRIBUTE_MEM_ALIGN
    static uint8_t buf_2_2[MY_DISP_HOR_RES * 10 * BYTE_PER_PIXEL];
    lv_display_set_buffers(disp, buf_2_1, buf_2_2, sizeof(buf_2_1), LV_DISPLAY_RENDER_MODE_PARTIAL);

    /* Пример 3
     * Два буфера размером с экран для двойной буферизации.
     * И LV_DISPLAY_RENDER_MODE_DIRECT, и LV_DISPLAY_RENDER_MODE_FULL работают, смотрите их комментарии.*/
    LV_ATTRIBUTE_MEM_ALIGN
    static uint8_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES * BYTE_PER_PIXEL];

    LV_ATTRIBUTE_MEM_ALIGN
    static uint8_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES * BYTE_PER_PIXEL];
    lv_display_set_buffers(disp, buf_3_1, buf_3_2, sizeof(buf_3_1), LV_DISPLAY_RENDER_MODE_DIRECT);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Инициализируйте дисплей и необходимые периферийные устройства.*/
static void disp_init(void)
{
    /*Вы кодируете здесь*/
}

volatile bool disp_flush_enabled = true;

/* Включить экран обновления (очистку процесса) при вызовеdisp_flush() по LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Включить обновление экрана (процесс промывки) при вызовеdisp_flush() по LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/*Сбрасывает содержимое внутреннего буфера в определенную область дисплея.
 *`px_map` содержит визуализированное изображение в виде необработанной карты изображения, и его следует скопировать в`area`на дисплей.
 *Вы можете использовать DMA или любое аппаратное ускорение, чтобы выполнить эту операцию в фоновом режиме, но
 *'lv_display_flush_ready()' обязательно вызывается после завершения.*/
static void disp_flush(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
    if(disp_flush_enabled) {
        /*Самый простой (но и самый медленный) вариант вывода всех пикселей на экран по одному.*/

        int32_t x;
        int32_t y;
        for(y = area->y1; y <= area->y2; y++) {
            for(x = area->x1; x <= area->x2; x++) {
                /*Поместите пиксель на дисплей. Например:*/
                /*put_px (x, y, *px_map)*/
                px_map++;
            }
        }
    }

    /*IMPORTANT!!!
     *Сообщите графической библиотеке, что вы готовы к очистке.*/
    lv_display_flush_ready(disp_drv);
}

#else /*Включите этот файл вверху*/

/*Этот вымышленный typedef существует исключительно для того, чтобы успеть замолчать -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
