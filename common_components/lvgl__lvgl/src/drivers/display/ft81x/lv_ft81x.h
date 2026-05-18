/**
 * @file lv_ft81x.h
 *
 */

#ifndef LV_FT81X_H
#define LV_FT81X_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"
#if LV_USE_FT81X

#include "../../../display/lv_display.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint16_t hor_res;   /**< active display width */
    uint16_t ver_res;   /**< active display height */

    uint16_t hcycle;    /**< total number of clocks per line, incl front/back porch */
    uint16_t hoffset;   /**< start of active line */
    uint16_t hsync0;    /**< start of horizontal sync pulse */
    uint16_t hsync1;    /**< end of horizontal sync pulse */
    uint16_t vcycle;    /**< total number of lines per screen, including pre/post */
    uint16_t voffset;   /**< start of active screen */
    uint16_t vsync0;    /**< start of vertical sync pulse */
    uint16_t vsync1;    /**< end of vertical sync pulse */
    uint8_t swizzle;    /**< FT8xx output to LCD - pin order */
    uint8_t pclkpol;    /**< LCD data is clocked in on this PCLK edge */
    uint8_t cspread;    /**< helps with noise, when set to 1 fewer signals are changed simultaneously, reset-default: 1 */
    uint8_t pclk;       /**< 60MHz / pclk = pclk frequency */

    bool has_crystal;   /**< has an external clock crystal */
    bool is_bt81x;      /**< is a BT series model, not FT */
} lv_ft81x_parameters_t;

typedef enum {
    LV_FT81X_SPI_OPERATION_CS_ASSERT,
    LV_FT81X_SPI_OPERATION_CS_DEASSERT,
    LV_FT81X_SPI_OPERATION_SEND,
    LV_FT81X_SPI_OPERATION_RECEIVE
} lv_ft81x_spi_operation_t;

typedef void (*lv_ft81x_spi_cb_t)(lv_display_t * disp, lv_ft81x_spi_operation_t operation, void * data,
                                  uint32_t length);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте дисплей драйвера ft81x на основе кадрового буфера.
 * @param params      указатель на структуру свойств панели отображения. не обязательно должен быть статичным.
 * @param partial_buf один частичный буфер
 * @param buf_size    размер частичного буфера
 * @param spi_cb      обратный вызов, вызываемый драйвером для выполнения операций SPI
 * @param user_data   используйте `lv_ft81x_get_user_data`, чтобы получить этот указатель внутри обратного вызова SPI
 * @return указатель на дисплей
 */
lv_display_t * lv_ft81x_create(const lv_ft81x_parameters_t * params, void * partial_buf, uint32_t buf_size,
                               lv_ft81x_spi_cb_t spi_cb, void * user_data);

/**
 * Введите параметр `user_data`, который был передан `lv_ft81x_create`. Полезно при обратном вызове SPI.
 * @param disp      указатель на дисплей ft81x
 * @return          указатель `user_data`
 */
void * lv_ft81x_get_user_data(lv_display_t * disp);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_FT81X*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_FT81X_H*/
