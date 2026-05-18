/*
@file EVE_commands.h
@brief   содержит прототипы функций FT8xx/BT8xx
@version 5.0
@date 29 декабря 2023 г.
@author Рудольф Ридель

@section LICENSE

Лицензия MIT

Copyright (c) 2016-2023 Rudolph Riedel

Разрешение настоящим предоставляется бесплатно любому лицу, получившему копию
данное программное обеспечение и связанные с ним файлы документации («Программное обеспечение») для решения
Программное обеспечение без ограничений, включая, помимо прочего, права
использовать, копировать, изменять, объединять, публиковать, распространять, сублицензировать,
и/или продавать копии Программного обеспечения, а также разрешать лицам, которым Программное обеспечение
предоставлено для этого при соблюдении следующих условий:

Вышеупомянутое уведомление об авторских правах и настоящее уведомление о разрешении должны быть включены во все
копии или существенные части Программного обеспечения.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

История @section

5.0
- добавлен прототип дляEVE_cmd_plkfreq()
- заменилBT81X_ENABLEна "EVE_GEN> 2"
- удален FT81X_ENABLE, поскольку FT81x уже сейчас является самой низкой поддерживаемой ревизией чипа
- удалил ранее устаревшую пометкуEVE_get_touch_tag()
- изменен EVE_color_rgb() для использования 32-битного значения, как и остальные команды цвета
- удалены метакоманды EVE_cmd_point(), EVE_cmd_line() и EVE_cmd_rect()
- удалены устаревшие функцииEVE_get_cmdoffset(void) иEVE_report_cmdoffset(void) - cmdoffset пропал
- переименовал EVE_LIB_GetProps() обратно в EVE_cmd_getprops(), поскольку он не делает ничего особенного для оправдания специального имени
- добавлен прототип вспомогательной функцииEVE_memWrite_sram_buffer()
- добавлены прототипы для EVE_cmd_bitmap_transform() и EVE_cmd_bitmap_transform_burst()
- добавлен прототип дляEVE_cmd_playvideo()
- добавлены прототипы для EVE_cmd_setfont_burst() и EVE_cmd_setfont2_burst()
- добавлен прототип дляEVE_cmd_videoframe()
- реструктуризировано: функции сортируются по поколениям чипов и внутри своей группы в алфавитном порядке
- reimplementedEVE_cmd_getmatrix() опять же, ему нужно читать значения, а не записывать их
- добавлены прототипы для EVE_cmd_fontcache() и EVE_cmd_fontcachequery()
- добавлен прототип дляEVE_cmd_flashprogram()
- добавлен прототип дляEVE_cmd_calibratesub()
- добавлены прототипы для EVE_cmd_animframeram(), EVE_cmd_animframeram_burst(), EVE_cmd_animstartram(),
EVE_cmd_animstartram_burst ()
- добавлены прототипы для EVE_cmd_apilevel(), EVE_cmd_apilevel_burst()
- добавлены прототипы для EVE_cmd_calllist(), EVE_cmd_calllist_burst()
- добавлен прототип дляEVE_cmd_getimage()
- добавлены прототипы для EVE_cmd_hsf(), EVE_cmd_hsf_burst()
- добавлен прототип дляEVE_cmd_linetime()
- добавлены прототипы для EVE_cmd_newlist(), EVE_cmd_newlist_burst()
- добавлены прототипы для EVE_cmd_runanim(), EVE_cmd_runanim_burst()
- добавлен прототип дляEVE_cmd_wait()
- удалена история до версии 4.0
- добавлено перечисление с кодами возврата, чтобы функции возвращали что-то более значимое
- наконец удаленEVE_cmd_start() после того, как он стал устаревшим в первой версии 5.0
- переименованEVE_cmd_execute() вEVE_execute_cmd() для большей согласованности, это не команда EVE
- добавлено возвращаемое значениеEVE_FIFO_HALF_EMPTYв EVE_busy(), чтобы указать, что доступно более 2048 байт
- удалил историю версии 4.0
- добавлена ширина параметра вEVE_calibrate_manual()
- изменены версииcmd_buttonс переменным числом аргументовcmd_button,cmd_textиcmd_toggleдля использования массива значенийuint32_tдля соответствияMISRA-C
- исправлены некоторые проблемы сMISRA-C
- базовое обслуживание: проверяется на наличие нарушений правил использования пробелов и отступов.
- больше исправлений линтера для мелких проблем, таких как переменные короче 3 символов.
- добавлен EVE_color_a()/EVE_color_a_burst()
- удален прототип EVE_cmd_newlist_burst(), так как функция была удалена ранее
- добавлен прототип дляEVE_write_display_parameters()
- добавленEVE_memRead_sram_buffer()
- добавленEVE_FAULT_RECOVEREDв список кодов возврата
- добавлено определение состояния внешней флэш-памяти
- добавлен прототип дляEVE_get_and_reset_fault_state()
- поместитеE_OKиE_NOT_OKв охранники #ifndef/#endif, как они обычно определяются
  уже в проектах AUTOSAR
- переименованEVE_FAIL_CHIPID_TIMEOUTв EVE_FAIL_REGID_TIMEOUT, как было предложено № 93 на github.
- изменил ряд параметров функции со знаковых на беззнаковые после
    обновленное руководство по программированию серии BT81x V2 .4
- закомментировал прототип EVE_cmd_regread()
- удален прототип дляEVE_cmd_hsf_burst()

*/

#ifndef EVE_COMMANDS_H
#define EVE_COMMANDS_H

#include "EVE.h"

#if !defined E_OK
#define E_OK 0U
#endif

#if !defined E_NOT_OK
#define E_NOT_OK 1U
#endif

#define EVE_FAIL_REGID_TIMEOUT 2U
#define EVE_FAIL_RESET_TIMEOUT 3U
#define EVE_FAIL_PCLK_FREQ 4U
#define EVE_FAIL_FLASH_STATUS_INIT 5U
#define EVE_FAIL_FLASH_STATUS_DETACHED 6U
#define EVE_FAIL_FLASHFAST_NOT_SUPPORTED 7U
#define EVE_FAIL_FLASHFAST_NO_HEADER_DETECTED 8U
#define EVE_FAIL_FLASHFAST_SECTOR0_FAILED 9U
#define EVE_FAIL_FLASHFAST_BLOB_MISMATCH 10U
#define EVE_FAIL_FLASHFAST_SPEED_TEST 11U
#define EVE_IS_BUSY 12U
#define EVE_FIFO_HALF_EMPTY 13U
#define EVE_FAULT_RECOVERED 14U

#define EVE_FLASH_STATUS_INIT 0U
#define EVE_FLASH_STATUS_DETACHED 1U
#define EVE_FLASH_STATUS_BASIC 2U
#define EVE_FLASH_STATUS_FULL 3U

/* ##################################################################
    вспомогательные функции
##################################################################### */

void EVE_cmdWrite(uint8_t const command, uint8_t const parameter);

uint8_t EVE_memRead8(uint32_t const ft_address);
uint16_t EVE_memRead16(uint32_t const ft_address);
uint32_t EVE_memRead32(uint32_t const ft_address);
void EVE_memWrite8(uint32_t const ft_address, uint8_t const ft_data);
void EVE_memWrite16(uint32_t const ft_address, uint16_t const ft_data);
void EVE_memWrite32(uint32_t const ft_address, uint32_t const ft_data);
void EVE_memWrite_flash_buffer(uint32_t const ft_address, const uint8_t *p_data, uint32_t const len);
void EVE_memWrite_sram_buffer(uint32_t const ft_address, const uint8_t *p_data, uint32_t const len);
void EVE_memRead_sram_buffer(uint32_t const ft_address, uint8_t *p_data, uint32_t const len);
uint8_t EVE_busy(void);
uint8_t EVE_get_and_reset_fault_state(void);
void EVE_execute_cmd(void);

/* ##################################################################
    команды и функции, которые будут использоваться вне списков отображения
##################################################################### */

/* EVE4: BT817 / BT818 */
#if EVE_GEN > 3

void EVE_cmd_flashprogram(uint32_t dest, uint32_t src, uint32_t num);
void EVE_cmd_fontcache(uint32_t font, uint32_t ptr, uint32_t num);
void EVE_cmd_fontcachequery(uint32_t *p_total, uint32_t *p_used);
void EVE_cmd_getimage(uint32_t *p_source, uint32_t *p_fmt, uint32_t *p_width, uint32_t *p_height, uint32_t *p_palette);
void EVE_cmd_linetime(uint32_t dest);
void EVE_cmd_newlist(uint32_t adr);
uint32_t EVE_cmd_pclkfreq(uint32_t ftarget, int32_t rounding);
void EVE_cmd_wait(uint32_t usec);

#endif /* EVE_GEN > 3 */

/* EVE3: BT815 / BT816 */
#if EVE_GEN > 2

void EVE_cmd_clearcache(void);
void EVE_cmd_flashattach(void);
void EVE_cmd_flashdetach(void);
void EVE_cmd_flasherase(void);
uint32_t EVE_cmd_flashfast(void);
void EVE_cmd_flashspidesel(void);
void EVE_cmd_flashread(uint32_t dest, uint32_t src, uint32_t num);
void EVE_cmd_flashsource(uint32_t ptr);
void EVE_cmd_flashspirx(uint32_t dest, uint32_t num);
void EVE_cmd_flashspitx(uint32_t num, const uint8_t *p_data);
void EVE_cmd_flashupdate(uint32_t dest, uint32_t src, uint32_t num);
void EVE_cmd_flashwrite(uint32_t ptr, uint32_t num, const uint8_t *p_data);
void EVE_cmd_inflate2(uint32_t ptr, uint32_t options, const uint8_t *p_data, uint32_t len);

#endif /* EVE_GEN > 2 */

void EVE_cmd_getprops(uint32_t *p_pointer, uint32_t *p_width, uint32_t *p_height);
uint32_t EVE_cmd_getptr(void);
void EVE_cmd_inflate(uint32_t ptr, const uint8_t *p_data, uint32_t len);
void EVE_cmd_interrupt(uint32_t msec);
void EVE_cmd_loadimage(uint32_t ptr, uint32_t options, const uint8_t *p_data, uint32_t len);
void EVE_cmd_mediafifo(uint32_t ptr, uint32_t size);
void EVE_cmd_memcpy(uint32_t dest, uint32_t src, uint32_t num);
uint32_t EVE_cmd_memcrc(uint32_t ptr, uint32_t num);
void EVE_cmd_memset(uint32_t ptr, uint8_t value, uint32_t num);
void EVE_cmd_memzero(uint32_t ptr, uint32_t num);
void EVE_cmd_playvideo(uint32_t options, const uint8_t *p_data, uint32_t len);
void EVE_cmd_setrotate(uint32_t rotation);
void EVE_cmd_snapshot(uint32_t ptr);
void EVE_cmd_snapshot2(uint32_t fmt, uint32_t ptr, int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt);
void EVE_cmd_track(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t tag);
void EVE_cmd_videoframe(uint32_t dest, uint32_t result_ptr);
/*voidEVE_cmd_memwrite(uint32_tdest,uint32_tчисло, constuint8_t*p_data);*/
/*uint32_tEVE_cmd_regread(uint32_tptr);*/

/* ##################################################################
    исправление и инициализация
##################################################################### */

#if EVE_GEN > 2
uint8_t EVE_init_flash(void);
#endif /* EVE_GEN > 2 */

void EVE_write_display_parameters(void);
uint8_t EVE_init(void);

/* ##################################################################
    функции для отображения списков
##################################################################### */

void EVE_start_cmd_burst(void);
void EVE_end_cmd_burst(void);

/* EVE4: BT817 / BT818 */
#if EVE_GEN > 3

void EVE_cmd_animframeram(int16_t xc0, int16_t yc0, uint32_t aoptr, uint32_t frame);
void EVE_cmd_animframeram_burst(int16_t xc0, int16_t yc0, uint32_t aoptr, uint32_t frame);
void EVE_cmd_animstartram(int32_t chnl, uint32_t aoptr, uint32_t loop);
void EVE_cmd_animstartram_burst(int32_t chnl, uint32_t aoptr, uint32_t loop);
void EVE_cmd_apilevel(uint32_t level);
void EVE_cmd_apilevel_burst(uint32_t level);
void EVE_cmd_calibratesub(uint16_t xc0, uint16_t yc0, uint16_t width, uint16_t height);
void EVE_cmd_calllist(uint32_t adr);
void EVE_cmd_calllist_burst(uint32_t adr);
void EVE_cmd_hsf(uint32_t hsf);
void EVE_cmd_runanim(uint32_t waitmask, uint32_t play);
void EVE_cmd_runanim_burst(uint32_t waitmask, uint32_t play);

#endif /* EVE_GEN > 3 */

/* EVE3: BT815 / BT816 */
#if EVE_GEN > 2

void EVE_cmd_animdraw(int32_t chnl);
void EVE_cmd_animdraw_burst(int32_t chnl);
void EVE_cmd_animframe(int16_t xc0, int16_t yc0, uint32_t aoptr, uint32_t frame);
void EVE_cmd_animframe_burst(int16_t xc0, int16_t yc0, uint32_t aoptr, uint32_t frame);
void EVE_cmd_animstart(int32_t chnl, uint32_t aoptr, uint32_t loop);
void EVE_cmd_animstart_burst(int32_t chnl, uint32_t aoptr, uint32_t loop);
void EVE_cmd_animstop(int32_t chnl);
void EVE_cmd_animstop_burst(int32_t chnl);
void EVE_cmd_animxy(int32_t chnl, int16_t xc0, int16_t yc0);
void EVE_cmd_animxy_burst(int32_t chnl, int16_t xc0, int16_t yc0);
void EVE_cmd_appendf(uint32_t ptr, uint32_t num);
void EVE_cmd_appendf_burst(uint32_t ptr, uint32_t num);
uint16_t EVE_cmd_bitmap_transform(int32_t xc0, int32_t yc0, int32_t xc1, int32_t yc1, int32_t xc2, int32_t yc2, int32_t tx0,
                                  int32_t ty0, int32_t tx1, int32_t ty1, int32_t tx2, int32_t ty2);
void EVE_cmd_bitmap_transform_burst(int32_t xc0, int32_t yc0, int32_t xc1, int32_t yc1, int32_t xc2, int32_t yc2, int32_t tx0,
                                    int32_t ty0, int32_t tx1, int32_t ty1, int32_t tx2, int32_t ty2);
void EVE_cmd_fillwidth(uint32_t pixel);
void EVE_cmd_fillwidth_burst(uint32_t pixel);
void EVE_cmd_gradienta(int16_t xc0, int16_t yc0, uint32_t argb0, int16_t xc1, int16_t yc1, uint32_t argb1);
void EVE_cmd_gradienta_burst(int16_t xc0, int16_t yc0, uint32_t argb0, int16_t xc1, int16_t yc1, uint32_t argb1);
void EVE_cmd_rotatearound(int32_t xc0, int32_t yc0, uint32_t angle, int32_t scale);
void EVE_cmd_rotatearound_burst(int32_t xc0, int32_t yc0, uint32_t angle, int32_t scale);

void EVE_cmd_button_var(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t font, uint16_t options, const char *p_text, uint8_t num_args, const uint32_t p_arguments[]);
void EVE_cmd_button_var_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t font, uint16_t options, const char *p_text, uint8_t num_args, const uint32_t p_arguments[]);
void EVE_cmd_text_var(int16_t xc0, int16_t yc0, uint16_t font, uint16_t options, const char *p_text, uint8_t num_args, const uint32_t p_arguments[]);
void EVE_cmd_text_var_burst(int16_t xc0, int16_t yc0, uint16_t font, uint16_t options, const char *p_text, uint8_t num_args, const uint32_t p_arguments[]);
void EVE_cmd_toggle_var(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t font, uint16_t options, uint16_t state, const char *p_text, uint8_t num_args, const uint32_t p_arguments[]);
void EVE_cmd_toggle_var_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t font, uint16_t options, uint16_t state, const char *p_text, uint8_t num_args, const uint32_t p_arguments[]);

#endif /* EVE_GEN > 2 */

void EVE_cmd_dl(uint32_t command);
void EVE_cmd_dl_burst(uint32_t command);

void EVE_cmd_append(uint32_t ptr, uint32_t num);
void EVE_cmd_append_burst(uint32_t ptr, uint32_t num);
void EVE_cmd_bgcolor(uint32_t color);
void EVE_cmd_bgcolor_burst(uint32_t color);
void EVE_cmd_button(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t font, uint16_t options, const char *p_text);
void EVE_cmd_button_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t font, uint16_t options, const char *p_text);
void EVE_cmd_calibrate(void);
void EVE_cmd_clock(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options, uint16_t hours, uint16_t mins, uint16_t secs, uint16_t msecs);
void EVE_cmd_clock_burst(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options, uint16_t hours, uint16_t mins, uint16_t secs, uint16_t msecs);
void EVE_cmd_dial(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options, uint16_t val);
void EVE_cmd_dial_burst(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options, uint16_t val);
void EVE_cmd_fgcolor(uint32_t color);
void EVE_cmd_fgcolor_burst(uint32_t color);
void EVE_cmd_gauge(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range);
void EVE_cmd_gauge_burst(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range);
void EVE_cmd_getmatrix(int32_t *p_a, int32_t *p_b, int32_t *p_c, int32_t *p_d, int32_t *p_e, int32_t *p_f);
void EVE_cmd_gradcolor(uint32_t color);
void EVE_cmd_gradcolor_burst(uint32_t color);
void EVE_cmd_gradient(int16_t xc0, int16_t yc0, uint32_t rgb0, int16_t xc1, int16_t yc1, uint32_t rgb1);
void EVE_cmd_gradient_burst(int16_t xc0, int16_t yc0, uint32_t rgb0, int16_t xc1, int16_t yc1, uint32_t rgb1);
void EVE_cmd_keys(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t font, uint16_t options, const char *p_text);
void EVE_cmd_keys_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t font, uint16_t options, const char *p_text);
void EVE_cmd_number(int16_t xc0, int16_t yc0, uint16_t font, uint16_t options, int32_t number);
void EVE_cmd_number_burst(int16_t xc0, int16_t yc0, uint16_t font, uint16_t options, int32_t number);
void EVE_cmd_progress(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t options, uint16_t val, uint16_t range);
void EVE_cmd_progress_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t options, uint16_t val, uint16_t range);
void EVE_cmd_romfont(uint32_t font, uint32_t romslot);
void EVE_cmd_romfont_burst(uint32_t font, uint32_t romslot);
void EVE_cmd_rotate(uint32_t angle);
void EVE_cmd_rotate_burst(uint32_t angle);
void EVE_cmd_scale(int32_t scx, int32_t scy);
void EVE_cmd_scale_burst(int32_t scx, int32_t scy);
void EVE_cmd_scrollbar(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t options, uint16_t val, uint16_t size, uint16_t range);
void EVE_cmd_scrollbar_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t options, uint16_t val, uint16_t size, uint16_t range);
void EVE_cmd_setbase(uint32_t base);
void EVE_cmd_setbase_burst(uint32_t base);
void EVE_cmd_setbitmap(uint32_t addr, uint16_t fmt, uint16_t width, uint16_t height);
void EVE_cmd_setbitmap_burst(uint32_t addr, uint16_t fmt, uint16_t width, uint16_t height);
void EVE_cmd_setfont(uint32_t font, uint32_t ptr);
void EVE_cmd_setfont_burst(uint32_t font, uint32_t ptr);
void EVE_cmd_setfont2(uint32_t font, uint32_t ptr, uint32_t firstchar);
void EVE_cmd_setfont2_burst(uint32_t font, uint32_t ptr, uint32_t firstchar);
void EVE_cmd_setscratch(uint32_t handle);
void EVE_cmd_setscratch_burst(uint32_t handle);
void EVE_cmd_sketch(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint32_t ptr, uint16_t format);
void EVE_cmd_sketch_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint32_t ptr, uint16_t format);
void EVE_cmd_slider(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t options, uint16_t val, uint16_t range);
void EVE_cmd_slider_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t options, uint16_t val, uint16_t range);
void EVE_cmd_spinner(int16_t xc0, int16_t yc0, uint16_t style, uint16_t scale);
void EVE_cmd_spinner_burst(int16_t xc0, int16_t yc0, uint16_t style, uint16_t scale);
void EVE_cmd_text(int16_t xc0, int16_t yc0, uint16_t font, uint16_t options, const char *p_text);
void EVE_cmd_text_burst(int16_t xc0, int16_t yc0, uint16_t font, uint16_t options, const char *p_text);
void EVE_cmd_toggle(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t font, uint16_t options, uint16_t state, const char *p_text);
void EVE_cmd_toggle_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t font, uint16_t options, uint16_t state, const char *p_text);
void EVE_cmd_translate(int32_t tr_x, int32_t tr_y);
void EVE_cmd_translate_burst(int32_t tr_x, int32_t tr_y);

void EVE_color_rgb(uint32_t color);
void EVE_color_rgb_burst(uint32_t color);
void EVE_color_a(uint8_t alpha);
void EVE_color_a_burst(uint8_t alpha);

/* ##################################################################
    функции специального назначения
##################################################################### */

void EVE_calibrate_manual(uint16_t width, uint16_t height);

#endif /* EVE_COMMANDS_H */
