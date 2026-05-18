/**
 * @file lv_draw_eve_display.h
 *
 */

#ifndef LV_DRAW_EVE_DISPLAY_H
#define LV_DRAW_EVE_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"
#if LV_USE_DRAW_EVE

#include "../../../draw/eve/lv_draw_eve_target.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте отображение для блока рисования EVE.
 * @param params      Указатель на структуру параметров отображения. Может быть временной переменной
 * @param op_cb       Обратный вызов, который будет вызываться для выполнения операций вывода иSPIIO с микросхемой EVE.
 * @param user_data   используйте `lv_draw_eve_display_get_user_data`, чтобы получить этот указатель внутри `op_cb`
 * @return            дисплей EVE
 */
lv_display_t * lv_draw_eve_display_create(const lv_draw_eve_parameters_t * params, lv_draw_eve_operation_cb_t op_cb,
                                          void * user_data);

/**
 * Введите параметр `user_data`, который был передан `lv_draw_eve_display_create`. Полезно при обратном вызове операции.
 * @param disp      указатель на дисплей lv_draw_eve
 * @return          указатель `user_data`
 */
void * lv_draw_eve_display_get_user_data(lv_display_t * disp);

/**
 * Создайте проект сенсорного экрана для дисплея EVE.
 * @param disp        указатель на дисплей lv_draw_eve
 * @return            разработка сенсорного экрана EVE
 */
lv_indev_t * lv_draw_eve_touch_create(lv_display_t * disp);


/* Функции предварительной загрузки активов RAM_G */

/**
 * Загрузите источник изображения в RAM_G сейчас, а не по мере необходимости во время рендеринга.
 * @param disp    указатель на дисплей lv_draw_eve
 * @param src     источник изображения. Значение, переданное в `lv_image_set_src`
 */
void lv_draw_eve_pre_upload_image(lv_display_t * disp, const void * src);

/**
 * Загрузите глифы шрифтов в RAM_G сейчас, а не по мере необходимости во время рендеринга.
 * Загрузите все глифы в диапазоне кодовых точек Юникода (включая начальное и конечное значения).
 * Его можно вызывать несколько раз в разных диапазонах.
 * @param disp                  указатель на дисплей lv_draw_eve
 * @param font                  шрифт для загрузки глифов
 * @param unicode_range_start   первая кодовая точка Юникода в диапазоне глифов для загрузки
 * @param unicode_range_end     последняя кодовая точка Юникода (включительно) в диапазоне глифов для загрузки
 */
void lv_draw_eve_pre_upload_font_range(lv_display_t * disp, const lv_font_t * font, uint32_t unicode_range_start,
                                       uint32_t unicode_range_end);

/**
 * Загрузите глифы шрифтов в RAM_G сейчас, а не по мере необходимости во время рендеринга.
 * Он загружает все глифы, необходимые для отображения строк `text`.
 * Его можно вызывать несколько раз с разными строками.
 * @param disp    указатель на дисплей lv_draw_eve
 * @param font    шрифт для загрузки глифов
 * @param text    строкаASCIIилиUTF-8, которая будет повторяться для загрузки глифов
 */
void lv_draw_eve_pre_upload_font_text(lv_display_t * disp, const lv_font_t * font, const char * text);


/* Функции управления низкого уровня EVE */

/**
 * Позвоните`EVE_memRead8`для индивидуального низкоуровневого управления дисплеями.
 * @param disp    дисплей, возвращаемый `lv_draw_eve_display_create`
 * @param address адресEVEдля чтения
 * @return        считанное значение
 */
uint8_t lv_draw_eve_memread8(lv_display_t * disp, uint32_t address);

/**
 * Позвоните`EVE_memRead16`для индивидуального низкоуровневого управления дисплеями.
 * @param disp    дисплей, возвращаемый `lv_draw_eve_display_create`
 * @param address адресEVEдля чтения
 * @return        считанное значение
 */
uint16_t lv_draw_eve_memread16(lv_display_t * disp, uint32_t address);

/**
 * Позвоните`EVE_memRead32`для индивидуального низкоуровневого управления дисплеями.
 * @param disp    дисплей, возвращаемый `lv_draw_eve_display_create`
 * @param address адресEVEдля чтения
 * @return        считанное значение
 */
uint32_t lv_draw_eve_memread32(lv_display_t * disp, uint32_t address);

/**
 * Позвоните`EVE_memWrite8`для индивидуального низкоуровневого управления дисплеями.
 * @param disp    дисплей, возвращаемый `lv_draw_eve_display_create`
 * @param address адресEVEдля записи
 * @param data    значение для записи
 */
void lv_draw_eve_memwrite8(lv_display_t * disp, uint32_t address, uint8_t data);

/**
 * Позвоните`EVE_memWrite16`для индивидуального низкоуровневого управления дисплеями.
 * @param disp    дисплей, возвращаемый `lv_draw_eve_display_create`
 * @param address адресEVEдля записи
 * @param data    значение для записи
 */
void lv_draw_eve_memwrite16(lv_display_t * disp, uint32_t address, uint16_t data);

/**
 * Позвоните`EVE_memWrite32`для индивидуального низкоуровневого управления дисплеями.
 * @param disp    дисплей, возвращаемый `lv_draw_eve_display_create`
 * @param address адресEVEдля записи
 * @param data    значение для записи
 */
void lv_draw_eve_memwrite32(lv_display_t * disp, uint32_t address, uint32_t data);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_EVE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_EVE_DISPLAY_H*/
