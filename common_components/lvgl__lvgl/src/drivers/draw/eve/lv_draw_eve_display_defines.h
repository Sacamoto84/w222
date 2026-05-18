/**
 * @file lv_draw_eve_display_defines.h
 *
 */

#ifndef LV_DRAW_EVE_DISPLAY_DEFINES_H
#define LV_DRAW_EVE_DISPLAY_DEFINES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"
#if LV_USE_DRAW_EVE

/*********************
 *      DEFINES
 *********************/

/* Память */
#define LV_EVE_EVE_RAM_G         ((uint32_t) 0x00000000UL)
#define LV_EVE_EVE_ROM_CHIPID    ((uint32_t) 0x000C0000UL)
#define LV_EVE_EVE_ROM_FONT      ((uint32_t) 0x001E0000UL)
#define LV_EVE_EVE_ROM_FONTROOT  ((uint32_t) 0x002FFFFCUL)
#define LV_EVE_EVE_RAM_DL        ((uint32_t) 0x00300000UL)
#define LV_EVE_EVE_RAM_REG       ((uint32_t) 0x00302000UL)
#define LV_EVE_EVE_RAM_CMD       ((uint32_t) 0x00308000UL)

/* Размеры буфера памяти */
#define LV_EVE_EVE_RAM_G_SIZE   ((uint32_t) 1024U*1024UL)
#define LV_EVE_EVE_CMDFIFO_SIZE ((uint32_t) 4U*1024UL)
#define LV_EVE_EVE_RAM_DL_SIZE  ((uint32_t) 8U*1024UL)

/* список команд списка, большинство из них нужны аргументы OR */
#define LV_EVE_DL_DISPLAY       ((uint32_t) 0x00000000UL)
#define LV_EVE_DL_BITMAP_SOURCE ((uint32_t) 0x01000000UL)
#define LV_EVE_DL_CLEAR_COLOR_RGB ((uint32_t) 0x02000000UL)
#define LV_EVE_DL_TAG           ((uint32_t) 0x03000000UL)
#define LV_EVE_DL_COLOR_RGB     ((uint32_t) 0x04000000UL)
#define LV_EVE_DL_BITMAP_HANDLE ((uint32_t) 0x05000000UL)
#define LV_EVE_DL_CELL          ((uint32_t) 0x06000000UL)
#define LV_EVE_DL_BITMAP_LAYOUT ((uint32_t) 0x07000000UL)
#define LV_EVE_DL_BITMAP_SIZE   ((uint32_t) 0x08000000UL)
#define LV_EVE_DL_ALPHA_FUNC    ((uint32_t) 0x09000000UL)
#define LV_EVE_DL_STENCIL_FUNC  ((uint32_t) 0x0A000000UL)
#define LV_EVE_DL_BLEND_FUNC    ((uint32_t) 0x0B000000UL)
#define LV_EVE_DL_STENCIL_OP    ((uint32_t) 0x0C000000UL)
#define LV_EVE_DL_POINT_SIZE    ((uint32_t) 0x0D000000UL)
#define LV_EVE_DL_LINE_WIDTH    ((uint32_t) 0x0E000000UL)
#define LV_EVE_DL_CLEAR_COLOR_A ((uint32_t) 0x0F000000UL)
#define LV_EVE_DL_COLOR_A       ((uint32_t) 0x10000000UL)
#define LV_EVE_DL_CLEAR_STENCIL ((uint32_t) 0x11000000UL)
#define LV_EVE_DL_CLEAR_TAG     ((uint32_t) 0x12000000UL)
#define LV_EVE_DL_STENCIL_MASK  ((uint32_t) 0x13000000UL)
#define LV_EVE_DL_TAG_MASK      ((uint32_t) 0x14000000UL)
#define LV_EVE_DL_BITMAP_TRANSFORM_A ((uint32_t) 0x15000000UL)
#define LV_EVE_DL_BITMAP_TRANSFORM_B ((uint32_t) 0x16000000UL)
#define LV_EVE_DL_BITMAP_TRANSFORM_C ((uint32_t) 0x17000000UL)
#define LV_EVE_DL_BITMAP_TRANSFORM_D ((uint32_t) 0x18000000UL)
#define LV_EVE_DL_BITMAP_TRANSFORM_E ((uint32_t) 0x19000000UL)
#define LV_EVE_DL_BITMAP_TRANSFORM_F ((uint32_t) 0x1A000000UL)
#define LV_EVE_DL_SCISSOR_XY    ((uint32_t) 0x1B000000UL)
#define LV_EVE_DL_SCISSOR_SIZE  ((uint32_t) 0x1C000000UL)
#define LV_EVE_DL_CALL          ((uint32_t) 0x1D000000UL)
#define LV_EVE_DL_JUMP          ((uint32_t) 0x1E000000UL)
#define LV_EVE_DL_BEGIN         ((uint32_t) 0x1F000000UL)
#define LV_EVE_DL_COLOR_MASK    ((uint32_t) 0x20000000UL)
#define LV_EVE_DL_END           ((uint32_t) 0x21000000UL)
#define LV_EVE_DL_SAVE_CONTEXT  ((uint32_t) 0x22000000UL)
#define LV_EVE_DL_RESTORE_CONTEXT ((uint32_t) 0x23000000UL)
#define LV_EVE_DL_RETURN        ((uint32_t) 0x24000000UL)
#define LV_EVE_DL_MACRO         ((uint32_t) 0x25000000UL)
#define LV_EVE_DL_CLEAR         ((uint32_t) 0x26000000UL)
#define LV_EVE_DL_VERTEX_FORMAT ((uint32_t) 0x27000000UL)
#define LV_EVE_DL_BITMAP_LAYOUT_H ((uint32_t) 0x28000000UL)
#define LV_EVE_DL_BITMAP_SIZE_H ((uint32_t) 0x29000000UL)
#define LV_EVE_DL_PALETTE_SOURCE ((uint32_t) 0x2A000000UL)
#define LV_EVE_DL_VERTEX_TRANSLATE_X ((uint32_t) 0x2B000000UL)
#define LV_EVE_DL_VERTEX_TRANSLATE_Y ((uint32_t) 0x2C000000UL)
#define LV_EVE_DL_NOP            ((uint32_t) 0x2D000000UL)

#define LV_EVE_DL_VERTEX2F     ((uint32_t) 0x40000000UL)
#define LV_EVE_DL_VERTEX2II    ((uint32_t) 0x80000000UL)

#define LV_EVE_CLR_COL     ((uint8_t) 0x4U)
#define LV_EVE_CLR_STN     ((uint8_t) 0x2U)
#define LV_EVE_CLR_TAG     ((uint8_t) 0x1U)

/* Команды хоста */
#define LV_EVE_EVE_ACTIVE       ((uint8_t) 0x00U) /* перевести EVE в активное состояние */
#define LV_EVE_EVE_STANDBY      ((uint8_t) 0x41U) /* переведенEVEв режим ожидания (clk работает) */
#define LV_EVE_EVE_SLEEP        ((uint8_t) 0x42U) /* перевести EVE в спящий режим (выключить) */
#define LV_EVE_EVE_CLKEXT       ((uint8_t) 0x44U) /* выбрать внешний источник синхронизации */
#define LV_EVE_EVE_CLKINT       ((uint8_t) 0x48U) /* выберите внутренний источник синхронизации, недопустимый вариант для BT817 / BT818 */
#define LV_EVE_EVE_PWRDOWN      ((uint8_t) 0x50U) /* поместитеEVEв Power Down (ядро выключено) */
#define LV_EVE_EVE_CLKSEL       ((uint8_t) 0x61U) /* настроить системные часы */
#define LV_EVE_EVE_RST_PULSE    ((uint8_t) 0x68U) /* сброс ядра - все регистры по умолчанию и процессоры сброшены */
#define LV_EVE_EVE_CORERST      ((uint8_t) 0x68U) /* сброс ядра - все регистры по умолчанию и процессоры сброшены */
#define LV_EVE_EVE_PINDRIVE     ((uint8_t) 0x70U) /* настройка силы привода для различных контактов */
#define LV_EVE_EVE_PIN_PD_STATE ((uint8_t) 0x71U) /* настроить поведение контактов при отключении питания */

/* Графическая команда определяет */
#define LV_EVE_EVE_NEVER      ((uint8_t) 0UL)
#define LV_EVE_EVE_LESS       ((uint8_t) 1UL)
#define LV_EVE_EVE_LEQUAL     ((uint8_t) 2UL)
#define LV_EVE_EVE_GREATER    ((uint8_t) 3UL)
#define LV_EVE_EVE_GEQUAL     ((uint8_t) 4UL)
#define LV_EVE_EVE_EQUAL      ((uint8_t) 5UL)
#define LV_EVE_EVE_NOTEQUAL   ((uint8_t) 6UL)
#define LV_EVE_EVE_ALWAYS     ((uint8_t) 7UL)

/* Растровые форматы */
#define LV_EVE_EVE_ARGB1555   ((uint8_t) 0UL)
#define LV_EVE_EVE_L1         ((uint8_t) 1UL)
#define LV_EVE_EVE_L4         ((uint8_t) 2UL)
#define LV_EVE_EVE_L8         ((uint8_t) 3UL)
#define LV_EVE_EVE_RGB332     ((uint8_t) 4UL)
#define LV_EVE_EVE_ARGB2      ((uint8_t) 5UL)
#define LV_EVE_EVE_ARGB4      ((uint8_t) 6UL)
#define LV_EVE_EVE_RGB565     ((uint8_t) 7UL)
#define LV_EVE_EVE_PALETTED   ((uint8_t) 8UL)
#define LV_EVE_EVE_TEXT8X8    ((uint8_t) 9UL)
#define LV_EVE_EVE_TEXTVGA    ((uint8_t) 10UL)
#define LV_EVE_EVE_BARGRAPH   ((uint8_t) 11UL)

/* Типы растровых фильтров */
#define LV_EVE_EVE_NEAREST    ((uint8_t) 0UL)
#define LV_EVE_EVE_BILINEAR   ((uint8_t) 1UL)

/* Типы переноса растровых изображений */
#define LV_EVE_EVE_BORDER     ((uint8_t) 0UL)
#define LV_EVE_EVE_REPEAT     ((uint8_t) 1UL)

/* Трафарет определяет */
#define LV_EVE_EVE_KEEP       ((uint8_t) 1UL)
#define LV_EVE_EVE_REPLACE    ((uint8_t) 2UL)
#define LV_EVE_EVE_INCR       ((uint8_t) 3UL)
#define LV_EVE_EVE_DECR       ((uint8_t) 4UL)
#define LV_EVE_EVE_INVERT     ((uint8_t) 5UL)

/* Перестановка списка графического отображения определяет */
#define LV_EVE_EVE_DLSWAP_DONE   ((uint8_t) 0UL)
#define LV_EVE_EVE_DLSWAP_LINE   ((uint8_t) 1UL)
#define LV_EVE_EVE_DLSWAP_FRAME  ((uint8_t) 2UL)

/* Биты прерываний */
#define LV_EVE_EVE_INT_SWAP          ((uint8_t) 0x01)
#define LV_EVE_EVE_INT_TOUCH         ((uint8_t) 0x02)
#define LV_EVE_EVE_INT_TAG           ((uint8_t) 0x04)
#define LV_EVE_EVE_INT_SOUND         ((uint8_t) 0x08)
#define LV_EVE_EVE_INT_PLAYBACK      ((uint8_t) 0x10)
#define LV_EVE_EVE_INT_CMDEMPTY      ((uint8_t) 0x20)
#define LV_EVE_EVE_INT_CMDFLAG       ((uint8_t) 0x40)
#define LV_EVE_EVE_INT_CONVCOMPLETE  ((uint8_t) 0x80)

/* Сенсорный режим */
#define LV_EVE_EVE_TMODE_OFF        ((uint8_t) 0U)
#define LV_EVE_EVE_TMODE_ONESHOT    ((uint8_t) 1U)
#define LV_EVE_EVE_TMODE_FRAME      ((uint8_t) 2U)
#define LV_EVE_EVE_TMODE_CONTINUOUS ((uint8_t) 3U)

/* Альфа-смешение */
#define LV_EVE_EVE_ZERO                 ((uint32_t) 0UL)
#define LV_EVE_EVE_ONE                  ((uint32_t) 1UL)
#define LV_EVE_EVE_SRC_ALPHA            ((uint32_t) 2UL)
#define LV_EVE_EVE_DST_ALPHA            ((uint32_t) 3UL)
#define LV_EVE_EVE_ONE_MINUS_SRC_ALPHA  ((uint32_t) 4UL)
#define LV_EVE_EVE_ONE_MINUS_DST_ALPHA  ((uint32_t) 5UL)

/* Графические примитивы */
#define LV_EVE_EVE_BITMAPS              ((uint32_t) 1UL)
#define LV_EVE_EVE_POINTS               ((uint32_t) 2UL)
#define LV_EVE_EVE_LINES                ((uint32_t) 3UL)
#define LV_EVE_EVE_LINE_STRIP           ((uint32_t) 4UL)
#define LV_EVE_EVE_EDGE_STRIP_R         ((uint32_t) 5UL)
#define LV_EVE_EVE_EDGE_STRIP_L         ((uint32_t) 6UL)
#define LV_EVE_EVE_EDGE_STRIP_A         ((uint32_t) 7UL)
#define LV_EVE_EVE_EDGE_STRIP_B         ((uint32_t) 8UL)
#define LV_EVE_EVE_RECTS                ((uint32_t) 9UL)
#define LV_EVE_EVE_INT_G8               ((uint32_t) 18UL)
#define LV_EVE_EVE_INT_L8C              ((uint32_t) 12UL)
#define LV_EVE_EVE_INT_VGA              ((uint32_t) 13UL)
#define LV_EVE_EVE_PALETTED565          ((uint32_t) 14UL)
#define LV_EVE_EVE_PALETTED4444         ((uint32_t) 15UL)
#define LV_EVE_EVE_PALETTED8            ((uint32_t) 16UL)
#define LV_EVE_EVE_L2                   ((uint32_t) 17UL)

/* Параметры команды виджета */
#define LV_EVE_EVE_OPT_MONO             ((uint16_t) 1U)
#define LV_EVE_EVE_OPT_NODL             ((uint16_t) 2U)
#define LV_EVE_EVE_OPT_FLAT             ((uint16_t) 256U)
#define LV_EVE_EVE_OPT_CENTERX          ((uint16_t) 512U)
#define LV_EVE_EVE_OPT_CENTERY          ((uint16_t) 1024U)
#define LV_EVE_EVE_OPT_CENTER           (LV_EVE_EVE_OPT_CENTERX | LV_EVE_EVE_OPT_CENTERY)
#define LV_EVE_EVE_OPT_NOBACK           ((uint16_t) 4096U)
#define LV_EVE_EVE_OPT_NOTICKS          ((uint16_t) 8192U)
#define LV_EVE_EVE_OPT_NOHM             ((uint16_t) 16384U)
#define LV_EVE_EVE_OPT_NOPOINTER        ((uint16_t) 16384U)
#define LV_EVE_EVE_OPT_NOSECS           ((uint16_t) 32768U)
#define LV_EVE_EVE_OPT_NOHANDS          ((uint16_t) 49152U)
#define LV_EVE_EVE_OPT_RIGHTX           ((uint16_t) 2048U)
#define LV_EVE_EVE_OPT_SIGNED           ((uint16_t) 256U)

#define LV_EVE_EVE_OPT_MEDIAFIFO        ((uint16_t) 16U)
#define LV_EVE_EVE_OPT_FULLSCREEN       ((uint16_t) 8U)
#define LV_EVE_EVE_OPT_NOTEAR           ((uint16_t) 4U)
#define LV_EVE_EVE_OPT_SOUND            ((uint16_t) 32U)

/* ADC */
#define LV_EVE_EVE_ADC_DIFFERENTIAL     ((uint32_t) 1UL)
#define LV_EVE_EVE_ADC_SINGLE_ENDED     ((uint32_t) 0UL)

/* Шрифты */
#define LV_EVE_EVE_NUMCHAR_PERFONT     ((uint32_t) 128UL)  /* количество символов шрифта на дескриптор растрового изображения */
#define LV_EVE_EVE_FONT_TABLE_SIZE     ((uint32_t) 148UL)  /* размер таблицы шрифтов — используется для цикла графическим движком */
#define LV_EVE_EVE_FONT_TABLE_POINTER  ((uint32_t) 0xFFFFCUL) /* указатель на встроенные таблицы шрифтов, начиная с дескриптора растрового изображения 16 */

/* Тип аудиосэмпла определяет */
#define LV_EVE_EVE_LINEAR_SAMPLES       ((uint32_t) 0UL) /* 8-битные подписанные образцы */
#define LV_EVE_EVE_ULAW_SAMPLES         ((uint32_t) 1UL) /* 8-битные фрагменты Улав */
#define LV_EVE_EVE_ADPCM_SAMPLES        ((uint32_t) 2UL) /* 4-битные файлы ima adpcm */

/* Синтезированный звук */
#define LV_EVE_EVE_SILENCE      ((uint8_t) 0x00U)
#define LV_EVE_EVE_SQUAREWAVE   ((uint8_t) 0x01U)
#define LV_EVE_EVE_SINEWAVE     ((uint8_t) 0x02U)
#define LV_EVE_EVE_SAWTOOTH     ((uint8_t) 0x03U)
#define LV_EVE_EVE_TRIANGLE     ((uint8_t) 0x04U)
#define LV_EVE_EVE_BEEPING      ((uint8_t) 0x05U)
#define LV_EVE_EVE_ALARM        ((uint8_t) 0x06U)
#define LV_EVE_EVE_WARBLE       ((uint8_t) 0x07U)
#define LV_EVE_EVE_CAROUSEL     ((uint8_t) 0x08U)
#define LV_EVE_EVE_PIPS(n)      ((uint8_t) (0x0FU + (n)))
#define LV_EVE_EVE_HARP         ((uint8_t) 0x40U)
#define LV_EVE_EVE_XYLOPHONE    ((uint8_t) 0x41U)
#define LV_EVE_EVE_TUBA         ((uint8_t) 0x42U)
#define LV_EVE_EVE_GLOCKENSPIEL ((uint8_t) 0x43U)
#define LV_EVE_EVE_ORGAN        ((uint8_t) 0x44U)
#define LV_EVE_EVE_TRUMPET      ((uint8_t) 0x45U)
#define LV_EVE_EVE_PIANO        ((uint8_t) 0x46U)
#define LV_EVE_EVE_CHIMES       ((uint8_t) 0x47U)
#define LV_EVE_EVE_MUSICBOX     ((uint8_t) 0x48U)
#define LV_EVE_EVE_BELL         ((uint8_t) 0x49U)
#define LV_EVE_EVE_CLICK        ((uint8_t) 0x50U)
#define LV_EVE_EVE_SWITCH       ((uint8_t) 0x51U)
#define LV_EVE_EVE_COWBELL      ((uint8_t) 0x52U)
#define LV_EVE_EVE_NOTCH        ((uint8_t) 0x53U)
#define LV_EVE_EVE_HIHAT        ((uint8_t) 0x54U)
#define LV_EVE_EVE_KICKDRUM     ((uint8_t) 0x55U)
#define LV_EVE_EVE_POP          ((uint8_t) 0x56U)
#define LV_EVE_EVE_CLACK        ((uint8_t) 0x57U)
#define LV_EVE_EVE_CHACK        ((uint8_t) 0x58U)
#define LV_EVE_EVE_MUTE         ((uint8_t) 0x60U)
#define LV_EVE_EVE_UNMUTE       ((uint8_t) 0x61U)

/* Синтезированные звуковые частоты, миди-нота */
#define LV_EVE_EVE_MIDI_A0   ((uint8_t) 21U)
#define LV_EVE_EVE_MIDI_A_0  ((uint8_t) 22U)
#define LV_EVE_EVE_MIDI_B0   ((uint8_t) 23U)
#define LV_EVE_EVE_MIDI_C1   ((uint8_t) 24U)
#define LV_EVE_EVE_MIDI_C_1  ((uint8_t) 25U)
#define LV_EVE_EVE_MIDI_D1   ((uint8_t) 26U)
#define LV_EVE_EVE_MIDI_D_1  ((uint8_t) 27U)
#define LV_EVE_EVE_MIDI_E1   ((uint8_t) 28U)
#define LV_EVE_EVE_MIDI_F1   ((uint8_t) 29U)
#define LV_EVE_EVE_MIDI_F_1  ((uint8_t) 30U)
#define LV_EVE_EVE_MIDI_G1   ((uint8_t) 31U)
#define LV_EVE_EVE_MIDI_G_1  ((uint8_t) 32U)
#define LV_EVE_EVE_MIDI_A1   ((uint8_t) 33U)
#define LV_EVE_EVE_MIDI_A_1  ((uint8_t) 34U)
#define LV_EVE_EVE_MIDI_B1   ((uint8_t) 35U)
#define LV_EVE_EVE_MIDI_C2   ((uint8_t) 36U)
#define LV_EVE_EVE_MIDI_C_2  ((uint8_t) 37U)
#define LV_EVE_EVE_MIDI_D2   ((uint8_t) 38U)
#define LV_EVE_EVE_MIDI_D_2  ((uint8_t) 39U)
#define LV_EVE_EVE_MIDI_E2   ((uint8_t) 40U)
#define LV_EVE_EVE_MIDI_F2   ((uint8_t) 41U)
#define LV_EVE_EVE_MIDI_F_2  ((uint8_t) 42U)
#define LV_EVE_EVE_MIDI_G2   ((uint8_t) 43U)
#define LV_EVE_EVE_MIDI_G_2  ((uint8_t) 44U)
#define LV_EVE_EVE_MIDI_A2   ((uint8_t) 45U)
#define LV_EVE_EVE_MIDI_A_2  ((uint8_t) 46U)
#define LV_EVE_EVE_MIDI_B2   ((uint8_t) 47U)
#define LV_EVE_EVE_MIDI_C3   ((uint8_t) 48U)
#define LV_EVE_EVE_MIDI_C_3  ((uint8_t) 49U)
#define LV_EVE_EVE_MIDI_D3   ((uint8_t) 50U)
#define LV_EVE_EVE_MIDI_D_3  ((uint8_t) 51U)
#define LV_EVE_EVE_MIDI_E3   ((uint8_t) 52U)
#define LV_EVE_EVE_MIDI_F3   ((uint8_t) 53U)
#define LV_EVE_EVE_MIDI_F_3  ((uint8_t) 54U)
#define LV_EVE_EVE_MIDI_G3   ((uint8_t) 55U)
#define LV_EVE_EVE_MIDI_G_3  ((uint8_t) 56U)
#define LV_EVE_EVE_MIDI_A3   ((uint8_t) 57U)
#define LV_EVE_EVE_MIDI_A_3  ((uint8_t) 58U)
#define LV_EVE_EVE_MIDI_B3   ((uint8_t) 59U)
#define LV_EVE_EVE_MIDI_C4   ((uint8_t) 60U)
#define LV_EVE_EVE_MIDI_C_4  ((uint8_t) 61U)
#define LV_EVE_EVE_MIDI_D4   ((uint8_t) 62U)
#define LV_EVE_EVE_MIDI_D_4  ((uint8_t) 63U)
#define LV_EVE_EVE_MIDI_E4   ((uint8_t) 64U)
#define LV_EVE_EVE_MIDI_F4   ((uint8_t) 65U)
#define LV_EVE_EVE_MIDI_F_4  ((uint8_t) 66U)
#define LV_EVE_EVE_MIDI_G4   ((uint8_t) 67U)
#define LV_EVE_EVE_MIDI_G_4  ((uint8_t) 68U)
#define LV_EVE_EVE_MIDI_A4   ((uint8_t) 69U)
#define LV_EVE_EVE_MIDI_A_4  ((uint8_t) 70U)
#define LV_EVE_EVE_MIDI_B4   ((uint8_t) 71U)
#define LV_EVE_EVE_MIDI_C5   ((uint8_t) 72U)
#define LV_EVE_EVE_MIDI_C_5  ((uint8_t) 73U)
#define LV_EVE_EVE_MIDI_D5   ((uint8_t) 74U)
#define LV_EVE_EVE_MIDI_D_5  ((uint8_t) 75U)
#define LV_EVE_EVE_MIDI_E5   ((uint8_t) 76U)
#define LV_EVE_EVE_MIDI_F5   ((uint8_t) 77U)
#define LV_EVE_EVE_MIDI_F_5  ((uint8_t) 78U)
#define LV_EVE_EVE_MIDI_G5   ((uint8_t) 79U)
#define LV_EVE_EVE_MIDI_G_5  ((uint8_t) 80U)
#define LV_EVE_EVE_MIDI_A5   ((uint8_t) 81U)
#define LV_EVE_EVE_MIDI_A_5  ((uint8_t) 82U)
#define LV_EVE_EVE_MIDI_B5   ((uint8_t) 83U)
#define LV_EVE_EVE_MIDI_C6   ((uint8_t) 84U)
#define LV_EVE_EVE_MIDI_C_6  ((uint8_t) 85U)
#define LV_EVE_EVE_MIDI_D6   ((uint8_t) 86U)
#define LV_EVE_EVE_MIDI_D_6  ((uint8_t) 87U)
#define LV_EVE_EVE_MIDI_E6   ((uint8_t) 88U)
#define LV_EVE_EVE_MIDI_F6   ((uint8_t) 89U)
#define LV_EVE_EVE_MIDI_F_6  ((uint8_t) 90U)
#define LV_EVE_EVE_MIDI_G6   ((uint8_t) 91U)
#define LV_EVE_EVE_MIDI_G_6  ((uint8_t) 92U)
#define LV_EVE_EVE_MIDI_A6   ((uint8_t) 93U)
#define LV_EVE_EVE_MIDI_A_6  ((uint8_t) 94U)
#define LV_EVE_EVE_MIDI_B6   ((uint8_t) 95U)
#define LV_EVE_EVE_MIDI_C7   ((uint8_t) 96U)
#define LV_EVE_EVE_MIDI_C_7  ((uint8_t) 97U)
#define LV_EVE_EVE_MIDI_D7   ((uint8_t) 98U)
#define LV_EVE_EVE_MIDI_D_7  ((uint8_t) 99U)
#define LV_EVE_EVE_MIDI_E7   ((uint8_t) 100U)
#define LV_EVE_EVE_MIDI_F7   ((uint8_t) 101U)
#define LV_EVE_EVE_MIDI_F_7  ((uint8_t) 102U)
#define LV_EVE_EVE_MIDI_G7   ((uint8_t) 103U)
#define LV_EVE_EVE_MIDI_G_7  ((uint8_t) 104U)
#define LV_EVE_EVE_MIDI_A7   ((uint8_t) 105U)
#define LV_EVE_EVE_MIDI_A_7  ((uint8_t) 106U)
#define LV_EVE_EVE_MIDI_B7   ((uint8_t) 107U)
#define LV_EVE_EVE_MIDI_C8   ((uint8_t) 108U)

/* Биты GPIO */
#define LV_EVE_EVE_GPIO0  ((uint8_t) 0U)
#define LV_EVE_EVE_GPIO1  ((uint8_t) 1U) /* вывод gpio по умолчанию для отключения звука, 1 — выключить, 0 — выключить */
#define LV_EVE_EVE_GPIO7  ((uint8_t) 7U) /* PIN -код gpio по умолчанию для включения дисплеев, 1 — включить, 0 — выключить */

/* Поворот дисплея */
#define LV_EVE_EVE_DISPLAY_0   ((uint8_t) 0U) /* вращение 0 градусов */
#define LV_EVE_EVE_DISPLAY_180 ((uint8_t) 1U) /* вращение на 180 градусов */

/* Команды */
#define LV_EVE_CMD_APPEND       ((uint32_t) 0xFFFFFF1EUL)
#define LV_EVE_CMD_BGCOLOR      ((uint32_t) 0xFFFFFF09UL)
#define LV_EVE_CMD_BUTTON       ((uint32_t) 0xFFFFFF0DUL)
#define LV_EVE_CMD_CALIBRATE    ((uint32_t) 0xFFFFFF15UL)
#define LV_EVE_CMD_CLOCK        ((uint32_t) 0xFFFFFF14UL)
#define LV_EVE_CMD_COLDSTART    ((uint32_t) 0xFFFFFF32UL)
#define LV_EVE_CMD_DIAL         ((uint32_t) 0xFFFFFF2DUL)
#define LV_EVE_CMD_DLSTART      ((uint32_t) 0xFFFFFF00UL)
#define LV_EVE_CMD_FGCOLOR      ((uint32_t) 0xFFFFFF0AUL)
#define LV_EVE_CMD_GAUGE        ((uint32_t) 0xFFFFFF13UL)
#define LV_EVE_CMD_GETMATRIX    ((uint32_t) 0xFFFFFF33UL)
#define LV_EVE_CMD_GETPROPS     ((uint32_t) 0xFFFFFF25UL)
#define LV_EVE_CMD_GETPTR       ((uint32_t) 0xFFFFFF23UL)
#define LV_EVE_CMD_GRADCOLOR    ((uint32_t) 0xFFFFFF34UL)
#define LV_EVE_CMD_GRADIENT     ((uint32_t) 0xFFFFFF0BUL)
#define LV_EVE_CMD_INFLATE      ((uint32_t) 0xFFFFFF22UL)
#define LV_EVE_CMD_INTERRUPT    ((uint32_t) 0xFFFFFF02UL)
#define LV_EVE_CMD_KEYS         ((uint32_t) 0xFFFFFF0EUL)
#define LV_EVE_CMD_LOADIDENTITY ((uint32_t) 0xFFFFFF26UL)
#define LV_EVE_CMD_LOADIMAGE    ((uint32_t) 0xFFFFFF24UL)
#define LV_EVE_CMD_LOGO         ((uint32_t) 0xFFFFFF31UL)
#define LV_EVE_CMD_MEDIAFIFO    ((uint32_t) 0xFFFFFF39UL)
#define LV_EVE_CMD_MEMCPY       ((uint32_t) 0xFFFFFF1DUL)
#define LV_EVE_CMD_MEMCRC       ((uint32_t) 0xFFFFFF18UL)
#define LV_EVE_CMD_MEMSET       ((uint32_t) 0xFFFFFF1BUL)
#define LV_EVE_CMD_MEMWRITE     ((uint32_t) 0xFFFFFF1AUL)
#define LV_EVE_CMD_MEMZERO      ((uint32_t) 0xFFFFFF1CUL)
#define LV_EVE_CMD_NUMBER       ((uint32_t) 0xFFFFFF2EUL)
#define LV_EVE_CMD_PLAYVIDEO    ((uint32_t) 0xFFFFFF3AUL)
#define LV_EVE_CMD_PROGRESS     ((uint32_t) 0xFFFFFF0FUL)
#define LV_EVE_CMD_REGREAD      ((uint32_t) 0xFFFFFF19UL)
#define LV_EVE_CMD_ROMFONT      ((uint32_t) 0xFFFFFF3FUL)
#define LV_EVE_CMD_ROTATE       ((uint32_t) 0xFFFFFF29UL)
#define LV_EVE_CMD_SCALE        ((uint32_t) 0xFFFFFF28UL)
#define LV_EVE_CMD_SCREENSAVER  ((uint32_t) 0xFFFFFF2FUL)
#define LV_EVE_CMD_SCROLLBAR    ((uint32_t) 0xFFFFFF11UL)
#define LV_EVE_CMD_SETBASE      ((uint32_t) 0xFFFFFF38UL)
#define LV_EVE_CMD_SETBITMAP    ((uint32_t) 0xFFFFFF43UL)
#define LV_EVE_CMD_SETFONT      ((uint32_t) 0xFFFFFF2BUL)
#define LV_EVE_CMD_SETFONT2     ((uint32_t) 0xFFFFFF3BUL)
#define LV_EVE_CMD_SETMATRIX    ((uint32_t) 0xFFFFFF2AUL)
#define LV_EVE_CMD_SETROTATE    ((uint32_t) 0xFFFFFF36UL)
#define LV_EVE_CMD_SETSCRATCH   ((uint32_t) 0xFFFFFF3CUL)
#define LV_EVE_CMD_SKETCH       ((uint32_t) 0xFFFFFF30UL)
#define LV_EVE_CMD_SLIDER       ((uint32_t) 0xFFFFFF10UL)
#define LV_EVE_CMD_SNAPSHOT     ((uint32_t) 0xFFFFFF1FUL)
#define LV_EVE_CMD_SNAPSHOT2    ((uint32_t) 0xFFFFFF37UL)
#define LV_EVE_CMD_SPINNER      ((uint32_t) 0xFFFFFF16UL)
#define LV_EVE_CMD_STOP         ((uint32_t) 0xFFFFFF17UL)
#define LV_EVE_CMD_SWAP         ((uint32_t) 0xFFFFFF01UL)
#define LV_EVE_CMD_TEXT         ((uint32_t) 0xFFFFFF0CUL)
#define LV_EVE_CMD_TOGGLE       ((uint32_t) 0xFFFFFF12UL)
#define LV_EVE_CMD_TRACK        ((uint32_t) 0xFFFFFF2CUL)
#define LV_EVE_CMD_TRANSLATE    ((uint32_t) 0xFFFFFF27UL)
#define LV_EVE_CMD_VIDEOFRAME   ((uint32_t) 0xFFFFFF41UL)
#define LV_EVE_CMD_VIDEOSTART   ((uint32_t) 0xFFFFFF40UL)

/* Регистры */
#define LV_EVE_REG_ANA_COMP         ((uint32_t) 0x00302184UL) /* указано только в даташите */
#define LV_EVE_REG_BIST_EN          ((uint32_t) 0x00302174UL) /* указано только в даташите */
#define LV_EVE_REG_CLOCK            ((uint32_t) 0x00302008UL)
#define LV_EVE_REG_CMDB_SPACE       ((uint32_t) 0x00302574UL)
#define LV_EVE_REG_CMDB_WRITE       ((uint32_t) 0x00302578UL)
#define LV_EVE_REG_CMD_DL           ((uint32_t) 0x00302100UL)
#define LV_EVE_REG_CMD_READ         ((uint32_t) 0x003020f8UL)
#define LV_EVE_REG_CMD_WRITE        ((uint32_t) 0x003020fcUL)
#define LV_EVE_REG_CPURESET         ((uint32_t) 0x00302020UL)
#define LV_EVE_REG_CSPREAD          ((uint32_t) 0x00302068UL)
#define LV_EVE_REG_CTOUCH_EXTENDED  ((uint32_t) 0x00302108UL)
#define LV_EVE_REG_CTOUCH_TOUCH0_XY ((uint32_t) 0x00302124UL) /* указано только в даташите */
#define LV_EVE_REG_CTOUCH_TOUCH4_X  ((uint32_t) 0x0030216cUL)
#define LV_EVE_REG_CTOUCH_TOUCH4_Y  ((uint32_t) 0x00302120UL)
#define LV_EVE_REG_CTOUCH_TOUCH1_XY ((uint32_t) 0x0030211cUL)
#define LV_EVE_REG_CTOUCH_TOUCH2_XY ((uint32_t) 0x0030218cUL)
#define LV_EVE_REG_CTOUCH_TOUCH3_XY ((uint32_t) 0x00302190UL)
#define LV_EVE_REG_TOUCH_CONFIG     ((uint32_t) 0x00302168UL)
#define LV_EVE_REG_DATESTAMP        ((uint32_t) 0x00302564UL) /* указано только в даташите */
#define LV_EVE_REG_DITHER           ((uint32_t) 0x00302060UL)
#define LV_EVE_REG_DLSWAP           ((uint32_t) 0x00302054UL)
#define LV_EVE_REG_FRAMES           ((uint32_t) 0x00302004UL)
#define LV_EVE_REG_FREQUENCY        ((uint32_t) 0x0030200cUL)
#define LV_EVE_REG_GPIO             ((uint32_t) 0x00302094UL)
#define LV_EVE_REG_GPIOX            ((uint32_t) 0x0030209cUL)
#define LV_EVE_REG_GPIOX_DIR        ((uint32_t) 0x00302098UL)
#define LV_EVE_REG_GPIO_DIR         ((uint32_t) 0x00302090UL)
#define LV_EVE_REG_HCYCLE           ((uint32_t) 0x0030202cUL)
#define LV_EVE_REG_HOFFSET          ((uint32_t) 0x00302030UL)
#define LV_EVE_REG_HSIZE            ((uint32_t) 0x00302034UL)
#define LV_EVE_REG_HSYNC0           ((uint32_t) 0x00302038UL)
#define LV_EVE_REG_HSYNC1           ((uint32_t) 0x0030203cUL)
#define LV_EVE_REG_ID               ((uint32_t) 0x00302000UL)
#define LV_EVE_REG_INT_EN           ((uint32_t) 0x003020acUL)
#define LV_EVE_REG_INT_FLAGS        ((uint32_t) 0x003020a8UL)
#define LV_EVE_REG_INT_MASK         ((uint32_t) 0x003020b0UL)
#define LV_EVE_REG_MACRO_0          ((uint32_t) 0x003020d8UL)
#define LV_EVE_REG_MACRO_1          ((uint32_t) 0x003020dcUL)
#define LV_EVE_REG_MEDIAFIFO_READ   ((uint32_t) 0x00309014UL) /* указан только в руководстве для программистов */
#define LV_EVE_REG_MEDIAFIFO_WRITE  ((uint32_t) 0x00309018UL) /* указан только в руководстве для программистов */
#define LV_EVE_REG_OUTBITS          ((uint32_t) 0x0030205cUL)
#define LV_EVE_REG_PCLK             ((uint32_t) 0x00302070UL)
#define LV_EVE_REG_PCLK_POL         ((uint32_t) 0x0030206cUL)
#define LV_EVE_REG_PLAY             ((uint32_t) 0x0030208cUL)
#define LV_EVE_REG_PLAYBACK_FORMAT  ((uint32_t) 0x003020c4UL)
#define LV_EVE_REG_PLAYBACK_FREQ    ((uint32_t) 0x003020c0UL)
#define LV_EVE_REG_PLAYBACK_LENGTH  ((uint32_t) 0x003020b8UL)
#define LV_EVE_REG_PLAYBACK_LOOP    ((uint32_t) 0x003020c8UL)
#define LV_EVE_REG_PLAYBACK_PLAY    ((uint32_t) 0x003020ccUL)
#define LV_EVE_REG_PLAYBACK_READPTR ((uint32_t) 0x003020bcUL)
#define LV_EVE_REG_PLAYBACK_START   ((uint32_t) 0x003020b4UL)
#define LV_EVE_REG_PWM_DUTY         ((uint32_t) 0x003020d4UL)
#define LV_EVE_REG_PWM_HZ           ((uint32_t) 0x003020d0UL)
#define LV_EVE_REG_RENDERMODE       ((uint32_t) 0x00302010UL) /* указано только в даташите */
#define LV_EVE_REG_ROTATE           ((uint32_t) 0x00302058UL)
#define LV_EVE_REG_SNAPFORMAT       ((uint32_t) 0x0030201cUL) /* указано только в даташите */
#define LV_EVE_REG_SNAPSHOT         ((uint32_t) 0x00302018UL) /* указано только в даташите */
#define LV_EVE_REG_SNAPY            ((uint32_t) 0x00302014UL) /* указано только в даташите */
#define LV_EVE_REG_SOUND            ((uint32_t) 0x00302088UL)
#define LV_EVE_REG_SPI_WIDTH        ((uint32_t) 0x00302188UL) /* указан с ложным смещением в руководстве для программистов V1 .1 */
#define LV_EVE_REG_SWIZZLE          ((uint32_t) 0x00302064UL)
#define LV_EVE_REG_TAG              ((uint32_t) 0x0030207cUL)
#define LV_EVE_REG_TAG_X            ((uint32_t) 0x00302074UL)
#define LV_EVE_REG_TAG_Y            ((uint32_t) 0x00302078UL)
#define LV_EVE_REG_TAP_CRC          ((uint32_t) 0x00302024UL) /* указано только в даташите */
#define LV_EVE_REG_TAP_MASK         ((uint32_t) 0x00302028UL) /* указано только в даташите */
#define LV_EVE_REG_TOUCH_ADC_MODE   ((uint32_t) 0x00302108UL)
#define LV_EVE_REG_TOUCH_CHARGE     ((uint32_t) 0x0030210cUL)
#define LV_EVE_REG_TOUCH_DIRECT_XY  ((uint32_t) 0x0030218cUL)
#define LV_EVE_REG_TOUCH_DIRECT_Z1Z2 ((uint32_t) 0x00302190UL)
#define LV_EVE_REG_TOUCH_MODE       ((uint32_t) 0x00302104UL)
#define LV_EVE_REG_TOUCH_OVERSAMPLE ((uint32_t) 0x00302114UL)
#define LV_EVE_REG_TOUCH_RAW_XY     ((uint32_t) 0x0030211cUL)
#define LV_EVE_REG_TOUCH_RZ         ((uint32_t) 0x00302120UL)
#define LV_EVE_REG_TOUCH_RZTHRESH   ((uint32_t) 0x00302118UL)
#define LV_EVE_REG_TOUCH_SCREEN_XY  ((uint32_t) 0x00302124UL)
#define LV_EVE_REG_TOUCH_SETTLE     ((uint32_t) 0x00302110UL)
#define LV_EVE_REG_TOUCH_TAG        ((uint32_t) 0x0030212cUL)
#define LV_EVE_REG_TOUCH_TAG1       ((uint32_t) 0x00302134UL) /* указано только в даташите */
#define LV_EVE_REG_TOUCH_TAG1_XY    ((uint32_t) 0x00302130UL) /* указано только в даташите */
#define LV_EVE_REG_TOUCH_TAG2       ((uint32_t) 0x0030213cUL) /* указано только в даташите */
#define LV_EVE_REG_TOUCH_TAG2_XY    ((uint32_t) 0x00302138UL) /* указано только в даташите */
#define LV_EVE_REG_TOUCH_TAG3       ((uint32_t) 0x00302144UL) /* указано только в даташите */
#define LV_EVE_REG_TOUCH_TAG3_XY    ((uint32_t) 0x00302140UL) /* указано только в даташите */
#define LV_EVE_REG_TOUCH_TAG4       ((uint32_t) 0x0030214cUL)/* указано только в даташите */
#define LV_EVE_REG_TOUCH_TAG4_XY    ((uint32_t) 0x00302148UL) /* указано только в даташите */
#define LV_EVE_REG_TOUCH_TAG_XY     ((uint32_t) 0x00302128UL)
#define LV_EVE_REG_TOUCH_TRANSFORM_A ((uint32_t) 0x00302150UL)
#define LV_EVE_REG_TOUCH_TRANSFORM_B ((uint32_t) 0x00302154UL)
#define LV_EVE_REG_TOUCH_TRANSFORM_C ((uint32_t) 0x00302158UL)
#define LV_EVE_REG_TOUCH_TRANSFORM_D ((uint32_t) 0x0030215cUL)
#define LV_EVE_REG_TOUCH_TRANSFORM_E ((uint32_t) 0x00302160UL)
#define LV_EVE_REG_TOUCH_TRANSFORM_F ((uint32_t) 0x00302164UL)
#define LV_EVE_REG_TRACKER          ((uint32_t) 0x00309000UL) /* указан только в руководстве для программистов */
#define LV_EVE_REG_TRACKER_1        ((uint32_t) 0x00309004UL) /* указан только в руководстве для программистов */
#define LV_EVE_REG_TRACKER_2        ((uint32_t) 0x00309008UL) /* указан только в руководстве для программистов */
#define LV_EVE_REG_TRACKER_3        ((uint32_t) 0x0030900cUL) /* указан только в руководстве для программистов */
#define LV_EVE_REG_TRACKER_4        ((uint32_t) 0x00309010UL) /* указан только в руководстве для программистов */
#define LV_EVE_REG_TRIM             ((uint32_t) 0x00302180UL)
#define LV_EVE_REG_VCYCLE           ((uint32_t) 0x00302040UL)
#define LV_EVE_REG_VOFFSET          ((uint32_t) 0x00302044UL)
#define LV_EVE_REG_VOL_PB           ((uint32_t) 0x00302080UL)
#define LV_EVE_REG_VOL_SOUND        ((uint32_t) 0x00302084UL)
#define LV_EVE_REG_VSIZE            ((uint32_t) 0x00302048UL)
#define LV_EVE_REG_VSYNC0           ((uint32_t) 0x0030204cUL)
#define LV_EVE_REG_VSYNC1           ((uint32_t) 0x00302050UL)


/* Макросы для создания статического списка отображения */

//#defineLV_EVE_ALPHA_FUNC(func,ref) ((LV_EVE_DL_ALPHA_FUNC) | (((func) & 7UL) << 8U) | ((ref) & 0xFFUL))
/**
 * @brief Установите функцию альфа-тестирования.
 *
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_ALPHA_FUNC(uint8_t func, uint8_t ref)
{
    uint32_t const funcv = ((uint32_t) func & 7U) << 8U;
    return (LV_EVE_DL_ALPHA_FUNC | funcv | ref);
}

//#defineLV_EVE_BITMAP_HANDLE(дескриптор) ((LV_EVE_DL_BITMAP_HANDLE) | ((дескриптор) & 0x1FUL))
/**
 * @brief Установите дескриптор растрового изображения.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_BITMAP_HANDLE(uint8_t handle)
{
    return (LV_EVE_DL_BITMAP_HANDLE | ((handle) & 0x1FUL));
}

//#defineLV_EVE_BITMAP_LAYOUT(формат,линейный шаг,высота) ((LV_EVE_DL_BITMAP_LAYOUT) | (((формат) & 0x1FUL) << 19U) | (((линейный шаг) & 0x3FFUL) << 9U) | ((высота) & 0x1FFUL))
/**
 * @brief Установите исходный формат памяти растрового изображения и макет для текущего дескриптора.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_BITMAP_LAYOUT(uint8_t format, uint16_t linestride, uint16_t height)
{
    uint32_t const formatv = ((uint32_t) format & 0x1FUL) << 19U;
    uint32_t const linestridev = ((uint32_t) linestride & 0x3FFUL) << 9U;
    uint32_t const heightv = height & 0x1FFUL;
    return (LV_EVE_DL_BITMAP_LAYOUT | formatv | linestridev | heightv);
}

//#defineLV_EVE_BITMAP_SIZE(filter,wrapx,wrapy,width,height) ((LV_EVE_DL_BITMAP_SIZE) | (((фильтр) & 1UL) << 20U) | (((wrapx) & 1UL) << 19U) | (((wrapy) & 1UL) << 18U) | (((ширина) & 0x1FFUL) << 9U) | ((высота) и 0x1FFUL))
/**
 * @brief Установите исходный формат памяти растрового изображения и макет для текущего дескриптора.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_BITMAP_SIZE(uint8_t filter, uint8_t wrapx, uint8_t wrapy, uint16_t width, uint16_t height)
{
    uint32_t const filterv = (filter & 0x1UL) << 20U;
    uint32_t const wrapxv = (wrapx & 0x1UL) << 19U;
    uint32_t const wrapyv = (wrapy & 0x1UL) << 18U;
    uint32_t const widthv = (width & 0x1FFUL) << 9U;
    uint32_t const heightv = height & 0x1FFUL;
    return (LV_EVE_DL_BITMAP_SIZE | filterv | wrapxv | wrapyv | widthv | heightv);
}

//#defineLV_EVE_BITMAP_LAYOUT_H(линейный шаг, высота) ((LV_EVE_DL_BITMAP_LAYOUT_H) | (((((линейный шаг) & 0xC00U) >> 10U)&3UL) << 2U) | ((((высота) & 0x600U) >> 9U) & 3UL))
/**
 * @brief Установите 2 старших бита формата и макета памяти исходного растрового изображения для текущего дескриптора.
 * @param linestride 12-битное значение, указанное в BITMAP_LAYOUT
 * @param height 11-битное значение, указанное для BITMAP_LAYOUT
 * @note это отличается от реализации FTDI, поскольку в качестве параметров принимаются исходные значения, а не только старшие биты.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_BITMAP_LAYOUT_H(uint16_t linestride, uint16_t height)
{
    uint32_t const linestridev = (uint32_t)((((linestride & 0xC00U) >> 10U) & 3UL) << 2U);
    uint32_t const heightv = (uint32_t)(((height & 0x600U) >> 9U) & 3UL);
    return (LV_EVE_DL_BITMAP_LAYOUT_H | linestridev | heightv);
}

//#defineLV_EVE_BITMAP_SIZE_H(ширина,высота) ((LV_EVE_DL_BITMAP_SIZE_H) | (((((ширина) & 0x600U) >> 9U) & 3UL) << 2U) | ((((высота) & 0x600U) >> 9U) & 3UL))
/**
 * @brief Установите 2 старших бита размера растрового изображения для текущего дескриптора.
 * @param linestride 11-битное значение ширины растрового изображения, используются 2 старших бита.
 * @param height 11-битное значение ширины растрового изображения, используются 2 старших бита.
 * @note это отличается от реализации FTDI, поскольку в качестве параметров принимаются исходные значения, а не только старшие биты.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_BITMAP_SIZE_H(uint16_t width, uint16_t height)
{
    uint32_t const widthv = (uint32_t)((((width & 0x600U) >> 9U) & 3UL) << 2U);
    uint32_t const heightv = (uint32_t)(((height & 0x600U) >> 9U) & 3UL);
    return ((LV_EVE_DL_BITMAP_SIZE_H) | widthv | heightv);
}

//#defineLV_EVE_BITMAP_SOURCE(адрес) ((LV_EVE_DL_BITMAP_SOURCE) | ((адрес) & 0x3FFFFFUL))
/**
 * @brief Установите исходный адрес растровых данных вRAM_Gили флэш-памяти.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_BITMAP_SOURCE(uint32_t addr)
{
    return (LV_EVE_DL_BITMAP_SOURCE | (addr & 0x3FFFFFUL));
}

#if LV_DRAW_EVE_EVE_GENERATION < 3 /* определите их только для FT81x */
//#defineLV_EVE_BITMAP_TRANSFORM_A(a) ((LV_EVE_DL_BITMAP_TRANSFORM_A) | ((a) & 0x1FFFFUL))
/**
 * @brief Установите коэффициент A матрицы преобразования растрового изображения.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_BITMAP_TRANSFORM_A(uint32_t val)
{
    return (LV_EVE_DL_BITMAP_TRANSFORM_A | (val & 0x1FFFFUL));
}

//#defineLV_EVE_BITMAP_TRANSFORM_B(b) ((LV_EVE_DL_BITMAP_TRANSFORM_B) | ((b) & 0x1FFFFUL))
/**
 * @brief Установите коэффициент B матрицы преобразования растрового изображения.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_BITMAP_TRANSFORM_B(uint32_t val)
{
    return (LV_EVE_DL_BITMAP_TRANSFORM_B | (val & 0x1FFFFUL));
}

//#defineLV_EVE_BITMAP_TRANSFORM_D(d) ((LV_EVE_DL_BITMAP_TRANSFORM_D) | ((d) & 0x1FFFFUL))
/**
 * @brief Установите коэффициент D матрицы преобразования растрового изображения.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_BITMAP_TRANSFORM_D(uint32_t val)
{
    return (LV_EVE_DL_BITMAP_TRANSFORM_D | (val & 0x1FFFFUL));
}

//#defineLV_EVE_BITMAP_TRANSFORM_E(e) ((LV_EVE_DL_BITMAP_TRANSFORM_E) | ((e) & 0x1FFFFUL))
/**
 * @brief Установите коэффициент E матрицы преобразования растрового изображения.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_BITMAP_TRANSFORM_E(uint32_t val)
{
    return (LV_EVE_DL_BITMAP_TRANSFORM_E | (val & 0x1FFFFUL));
}

#endif

//#defineLV_EVE_BITMAP_TRANSFORM_C(c) ((LV_EVE_DL_BITMAP_TRANSFORM_C) | ((c) & 0x1FFFFUL))
/**
 * @brief Установите коэффициент C матрицы преобразования растрового изображения.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_BITMAP_TRANSFORM_C(uint32_t val)
{
    return (LV_EVE_DL_BITMAP_TRANSFORM_C | (val & 0x1FFFFUL));
}

//#defineLV_EVE_BITMAP_TRANSFORM_F(f) ((LV_EVE_DL_BITMAP_TRANSFORM_F) | ((f) & 0x1FFFFUL))
/**
 * @brief Установите коэффициент F матрицы преобразования растрового изображения.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_BITMAP_TRANSFORM_F(uint32_t val)
{
    return (LV_EVE_DL_BITMAP_TRANSFORM_F | (val & 0x1FFFFUL));
}

//#defineLV_EVE_BLEND_FUNC(src,dst) ((LV_EVE_DL_BLEND_FUNC) | (((источник) & 7UL) << 3U) | ((dst) & 7UL))
/**
 * @brief Выполните последовательность команд в другом месте списка отображения.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_BLEND_FUNC(uint8_t src, uint8_t dst)
{
    uint32_t const srcv = (uint32_t)((src & 7UL) << 3U);
    uint32_t const dstv = (uint32_t)(dst & 7UL);
    return (LV_EVE_DL_BLEND_FUNC | srcv | dstv);
}

//#defineLV_EVE_CALL(назначение) ((LV_EVE_DL_CALL) | ((назначение) & 0xFFFFUL))
/**
 * @brief Выполните последовательность команд в другом месте списка отображения.
 * @note допустимый диапазон для dest — от нуля до 2047.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_CALL(uint16_t dest)
{
    return (LV_EVE_DL_CALL | (dest & 0x7FFUL));
}

//#defineLV_EVE_JUMP(назначение) ((LV_EVE_DL_JUMP) | ((назначение) & 0xFFFFUL))
/**
 * @brief Выполнять команды в другом месте списка отображения.
 * @note допустимый диапазон для dest — от нуля до 2047.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_JUMP(uint16_t dest)
{
    return (LV_EVE_DL_JUMP | (dest & 0x7FFUL));
}

//#defineLV_EVE_CELL(ячейка) ((LV_EVE_DL_CELL) | ((ячейка) & 0x7FUL))
/**
 * @brief Установите номер ячейки растрового изображения для команды VERTEX2F.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_CELL(uint8_t cell)
{
    return (LV_EVE_DL_CELL | (cell & 0x7FUL));
}

//#defineLV_EVE_CLEAR(c,s,t) ((LV_EVE_DL_CLEAR) | (((c) & 1UL) << 2U) | (((s) & 1UL) << 1U) | ((t) & 1UL))
/**
 * @brief Очистите буферы до заданных значений.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_CLEAR(uint8_t color, uint8_t stencil, uint8_t tag)
{
    uint32_t const colorv = (color & 1UL) << 2U;
    uint32_t const stencilv = (stencil & 1UL) << 1U;
    uint32_t const tagv = (tag & 1UL);
    return (LV_EVE_DL_CLEAR | colorv | stencilv | tagv);
}

//#defineLV_EVE_CLEAR_COLOR_A(альфа) ((LV_EVE_DL_CLEAR_COLOR_A) | ((альфа) & 0xFFUL))
/**
 * @brief Установите четкое значение для альфа-канала.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_CLEAR_COLOR_A(uint8_t alpha)
{
    return (LV_EVE_DL_CLEAR_COLOR_A | alpha);
}

//#defineLV_EVE_CLEAR_COLOR_RGB(красный,зеленый,синий) ((LV_EVE_DL_CLEAR_COLOR_RGB) | (((красный) & 0xFFUL) << 16U) | (((зеленый) & 0xFFUL) << 8U) | ((синий) & 0xFFUL))
/**
 * @brief Установите четкие значения для красного, зеленого и синего каналов.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_CLEAR_COLOR_RGB(uint8_t red, uint8_t green, uint8_t blue)
{
    uint32_t const redv = ((red & 0xFFUL) << 16U);
    uint32_t const greenv = ((green & 0xFFUL) << 8U);
    uint32_t const bluev = (blue & 0xFFUL);
    return (LV_EVE_DL_CLEAR_COLOR_RGB | redv | greenv | bluev);
}

//#defineLV_EVE_CLEAR_STENCIL(s) ((LV_EVE_DL_CLEAR_STENCIL) | ((s) & 0xFFUL))
/**
 * @brief Установите значение очистки для буфера трафарета.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_CLEAR_STENCIL(uint8_t val)
{
    return (LV_EVE_DL_CLEAR_STENCIL | val);
}

//#defineLV_EVE_CLEAR_TAG(s) ((LV_EVE_DL_CLEAR_TAG) | ((s) & 0xFFUL))
/**
 * @brief Установите значение очистки для буфера тегов.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_CLEAR_TAG(uint8_t val)
{
    return (LV_EVE_DL_CLEAR_TAG | val);
}

//#defineLV_EVE_COLOR_A(альфа) ((LV_EVE_DL_COLOR_A) | ((альфа) & 0xFFUL))
/**
 * @brief Установите текущий цвет альфа.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_COLOR_A(uint8_t alpha)
{
    return (LV_EVE_DL_COLOR_A | alpha);
}

//#defineLV_EVE_COLOR_MASK(r,g,b,a) ((LV_EVE_DL_COLOR_MASK) | (((r) & 1UL) << 3U) | (((g) & 1UL) << 2U) | (((b) & 1UL) << 1U) | ((a) & 1UL))
/**
 * @brief Включить или отключить запись компонентов цвета.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_COLOR_MASK(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    uint32_t const redv = ((red & 1UL) << 3U);
    uint32_t const greenv = ((green & 1UL) << 2U);
    uint32_t const bluev = ((blue & 1UL) << 1U);
    uint32_t const alphav = (alpha & 1UL);
    return (LV_EVE_DL_COLOR_MASK | redv | greenv | bluev | alphav);
}

//#defineLV_EVE_COLOR_RGB(красный,зеленый,синий) ((LV_EVE_DL_COLOR_RGB) | (((красный) & 0xFFUL) << 16U) | (((зеленый) & 0xFFUL) << 8U) | ((синий) & 0xFFUL))
/**
 * @brief Установите текущий цвет: красный, зеленый и синий.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_COLOR_RGB(uint8_t red, uint8_t green, uint8_t blue)
{
    uint32_t const redv = ((red & 0xFFUL) << 16U);
    uint32_t const greenv = ((green & 0xFFUL) << 8U);
    uint32_t const bluev = (blue & 0xFFUL);
    return (LV_EVE_DL_COLOR_RGB | redv | greenv | bluev);
}

//#defineLV_EVE_LINE_WIDTH(ширина) ((LV_EVE_DL_LINE_WIDTH) | (((uint32_t) (ширина)) & 0xFFFUL))
/**
 * @brief Установите ширину рисуемых линий с помощью примитиваLINESс точностью до 1/16 пикселя.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_LINE_WIDTH(uint16_t width)
{
    return (LV_EVE_DL_LINE_WIDTH | (width & 0xFFFUL));
}

//#defineLV_EVE_MACRO(м) ((LV_EVE_DL_MACRO) | ((м) & 1UL))
/**
 * @brief Выполнить одну команду из макрорегистра.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_MACRO(uint8_t macro)
{
    return (LV_EVE_DL_MACRO | (macro & 0x1UL));
}

//#defineLV_EVE_PALETTE_SOURCE(адрес) ((LV_EVE_DL_PALETTE_SOURCE) | ((адрес) & 0x3FFFFF3UL))
/**
 * @brief Установите базовый адрес палитры.
 * @note Выравнивание по 2 байтам требуется, если формат пикселя —PALETTE4444или PALETTE565.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_PALETTE_SOURCE(uint32_t addr)
{
    return (LV_EVE_DL_PALETTE_SOURCE | (addr & 0x3FFFFFUL));
}

//#defineLV_EVE_POINT_SIZE(размер) ((LV_EVE_DL_POINT_SIZE) | ((размер) & 0x1FFFUL))
/**
 * @brief Задайте радиус рисуемых точек с помощью примитиваPOINTSс точностью до 1/16 пикселя.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_POINT_SIZE(uint16_t size)
{
    return (LV_EVE_DL_POINT_SIZE | (size & 0x1FFFUL));
}

//#defineLV_EVE_SCISSOR_SIZE(ширина,высота) ((LV_EVE_DL_SCISSOR_SIZE) | (((ширина) & 0xFFFUL) << 12U) | ((высота) & 0xFFFUL))
/**
 * @brief Установите размер прямоугольника ножничного зажима.
 * @note допустимый диапазон ширины и высоты от нуля до 2048.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_SCISSOR_SIZE(uint16_t width, uint16_t height)
{
    uint32_t const widthv = (uint32_t)((width & 0xFFFUL) << 12U);
    uint32_t const heightv = (uint32_t)(height & 0xFFFUL);
    return (LV_EVE_DL_SCISSOR_SIZE | widthv | heightv);
}

//#defineLV_EVE_SCISSOR_XY(x,y) ((LV_EVE_DL_SCISSOR_XY) | (((x) & 0x7FFUL) << 11U) | ((y) & 0x7FFUL))
/**
 * @brief Установите верхний левый угол прямоугольника ножничного зажима.
 * @note допустимый диапазон ширины и высоты — от нуля до 2047.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_SCISSOR_XY(uint16_t xc0, uint16_t yc0)
{
    uint32_t const xc0v = (uint32_t)((xc0 & 0x7FFUL) << 11U);
    uint32_t const yc0v = (uint32_t)(yc0 & 0x7FFUL);
    return (LV_EVE_DL_SCISSOR_XY | xc0v | yc0v);
}

//#defineLV_EVE_STENCIL_FUNC(func,ref,mask) ((LV_EVE_DL_STENCIL_FUNC) | (((func) & 7UL) << 16U) | (((ref) & 0xFFUL) << 8U)|((маска) & 0xFFUL))
/**
 * @brief Установите функцию и опорное значение для тестирования трафарета.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_STENCIL_FUNC(uint8_t func, uint8_t ref, uint8_t mask)
{
    uint32_t const funcv = (uint32_t)((func & 7UL) << 16U);
    uint32_t const refv = (uint32_t)((ref & 0xFFUL) << 8U);
    uint32_t const maskv = (uint32_t)(mask & 0xFFUL);
    return (LV_EVE_DL_STENCIL_FUNC | funcv | refv | maskv);
}

//#defineLV_EVE_STENCIL_MASK(маска) ((LV_EVE_DL_STENCIL_MASK) | ((маска) & 0xFFUL))
/**
 * @brief Управляйте записью отдельных битов в плоскостях трафарета.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_STENCIL_MASK(uint8_t mask)
{
    return (LV_EVE_DL_STENCIL_MASK | mask);
}

//#defineLV_EVE_STENCIL_OP(sfail,spass) ((LV_EVE_DL_STENCIL_OP) | (((sfail) & 7UL) << 3U) | ((spass) & 7UL))
/**
 * @brief Установите действия по тестированию трафарета.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_STENCIL_OP(uint8_t sfail, uint8_t spass)
{
    uint32_t const sfailv = (uint32_t)((sfail & 0x07UL) << 3U);
    uint32_t const spassv = (uint32_t)(spass & 0x07UL);
    return (LV_EVE_DL_STENCIL_OP | sfailv | spassv);
}

//#defineLV_EVE_TAG(s) ((LV_EVE_DL_TAG) | ((s) & 0xFFUL))
/**
 * @brief Прикрепите значение тега для следующих графических объектов, отображаемых на экране.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_TAG(uint8_t tagval)
{
    return (LV_EVE_DL_TAG | tagval);
}

//#defineLV_EVE_TAG_MASK(маска) ((LV_EVE_DL_TAG_MASK) | ((маска) & 1UL))
/**
 * @brief Управляйте записью буфера тегов.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_TAG_MASK(uint8_t mask)
{
    return (LV_EVE_DL_TAG_MASK | ((mask) & 1UL));
}

//#defineLV_EVE_VERTEX2F(x,y) ((LV_EVE_DL_VERTEX2F) | ((((uint32_t) (x)) & 0x7FFFUL) << 15U) | (((uint32_t) (y)) & 0x7FFFUL))
/**
 * @brief Установите координаты для графических примитивов.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_VERTEX2F(int16_t xc0, int16_t yc0)
{
    uint32_t const xc0v = ((((uint32_t)((uint16_t) xc0)) & 0x7FFFUL) << 15U);
    uint32_t const yc0v = (((uint32_t)((uint16_t) yc0)) & 0x7FFFUL);
    return (LV_EVE_DL_VERTEX2F | xc0v | yc0v);
}

//#defineLV_EVE_VERTEX2II(x,y,дескриптор,ячейка) ((LV_EVE_DL_VERTEX2II) | (((x) & 0x1FFUL) << 21U) | (((y) & 0x1FFUL) << 12U) | (((дескриптор) & 0x1FUL) << 7U) | ((ячейка) & 0x7FUL))
/**
 * @brief Установите координаты, дескриптор растрового изображения и номер ячейки для графических примитивов.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_VERTEX2II(uint16_t xc0, uint16_t yc0, uint8_t handle, uint8_t cell)
{
    uint32_t const xc0v = ((((uint32_t) xc0) & 0x1FFUL) << 21U);
    uint32_t const yc0v = ((((uint32_t) yc0) & 0x1FFUL) << 12U);
    uint32_t const handlev = ((((uint32_t) handle) & 0x1FUL) << 7U);
    uint32_t const cellv = (((uint32_t) cell) & 0x7FUL);
    return (LV_EVE_DL_VERTEX2II | xc0v | yc0v | handlev | cellv);
}

//#defineLV_EVE_VERTEX_FORMAT(ГРП) ((LV_EVE_DL_VERTEX_FORMAT) | ((ГРП) & 7UL))
/**
 * @brief Установите точность координат VERTEX2F.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_VERTEX_FORMAT(uint8_t frac)
{
    return (LV_EVE_DL_VERTEX_FORMAT | ((frac) & 7UL));
}

//#defineLV_EVE_VERTEX_TRANSLATE_X(x) ((LV_EVE_DL_VERTEX_TRANSLATE_X) | ((x) & 0x1FFFFUL))
/**
 * @brief Установите компонент преобразования X преобразований вершин.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_VERTEX_TRANSLATE_X(int32_t xco)
{
    return (LV_EVE_DL_VERTEX_TRANSLATE_X | (((uint32_t) xco) & 0x1FFFFUL));
}

//#defineLV_EVE_VERTEX_TRANSLATE_Y(y) ((LV_EVE_DL_VERTEX_TRANSLATE_Y) | ((y) & 0x1FFFFUL))
/**
 * @brief Установите компонент преобразования вершин Y преобразования.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_VERTEX_TRANSLATE_Y(int32_t yco)
{
    return (LV_EVE_DL_VERTEX_TRANSLATE_Y | (((uint32_t) yco) & 0x1FFFFUL));
}

/* #define LV_EVE_BEGIN(prim) ((LV_EVE_DL_BEGIN) | ((prim) & 15UL)) */ /* используйте определение LV_EVE_DL_BEGIN */
/* #define LV_EVE_DISPLAY() ((LV_EVE_DL_DISPLAY)) */ /* используйте определение LV_EVE_DL_DISPLAY */
/* #define LV_EVE_END() ((LV_EVE_DL_END)) */ /* используйте определение LV_EVE_DL_END */
/* #define LV_EVE_RESTORE_CONTEXT() ((LV_EVE_DL_RESTORE_CONTEXT)) */ /* используйте определение LV_EVE_DL_RESTORE_CONTEXT */
/* #define LV_EVE_RETURN() ((LV_EVE_DL_RETURN)) */ /* используйте определение LV_EVE_DL_RETURN */
/* #define LV_EVE_SAVE_CONTEXT() ((LV_EVE_DL_SAVE_CONTEXT)) */ /* используйте определение LV_EVE_DL_SAVE_CONTEXT */
/* #define LV_EVE_NOP() ((LV_EVE_DL_NOP)) */

/* ########## EVE Generation 3: BT815 / BT816 definitions ########## */

#if LV_DRAW_EVE_EVE_GENERATION > 2

#define LV_EVE_EVE_GLFORMAT  ((uint32_t) 31UL) /* используется с BITMAP_LAYOUT для указания формата растрового изображения, заданного BITMAP_EXT_FORMAT */

#define LV_EVE_DL_BITMAP_EXT_FORMAT ((uint32_t) 0x2E000000UL) /* требуются аргументы OR */
#define LV_EVE_DL_BITMAP_SWIZZLE    ((uint32_t) 0x2F000000UL)
/* #define LV_EVE_DL_INT_FRR           ((uint32_t) 0x30000000UL) */ /* ESE отображает «Внутренний: результат чтения флэш-памяти» — недокументированная команда списка отображения. */

/* Расширенные форматы растровых изображений */
#define LV_EVE_EVE_ASTC_4X4   ((uint32_t) 37808UL)
#define LV_EVE_EVE_ASTC_5X4   ((uint32_t) 37809UL)
#define LV_EVE_EVE_ASTC_5X5   ((uint32_t) 37810UL)
#define LV_EVE_EVE_ASTC_6X5   ((uint32_t) 37811UL)
#define LV_EVE_EVE_ASTC_6X6   ((uint32_t) 37812UL)
#define LV_EVE_EVE_ASTC_8X5   ((uint32_t) 37813UL)
#define LV_EVE_EVE_ASTC_8X6   ((uint32_t) 37814UL)
#define LV_EVE_EVE_ASTC_8X8   ((uint32_t) 37815UL)
#define LV_EVE_EVE_ASTC_10X5  ((uint32_t) 37816UL)
#define LV_EVE_EVE_ASTC_10X6  ((uint32_t) 37817UL)
#define LV_EVE_EVE_ASTC_10X8  ((uint32_t) 37818UL)
#define LV_EVE_EVE_ASTC_10X10 ((uint32_t) 37819UL)
#define LV_EVE_EVE_ASTC_12X10 ((uint32_t) 37820UL)
#define LV_EVE_EVE_ASTC_12X12 ((uint32_t) 37821UL)

#define LV_EVE_EVE_RAM_ERR_REPORT      ((uint32_t) 0x309800UL) /* строка длиной не более 128 байт, завершающаяся нулем */
#define LV_EVE_EVE_RAM_FLASH           ((uint32_t) 0x800000UL)
#define LV_EVE_EVE_RAM_FLASH_POSTBLOB  ((uint32_t) 0x801000UL)

#define LV_EVE_EVE_OPT_FLASH  ((uint16_t) 64U)
#define LV_EVE_EVE_OPT_OVERLAY ((uint16_t) 128U)
#define LV_EVE_EVE_OPT_FORMAT ((uint16_t) 4096U)
#define LV_EVE_EVE_OPT_FILL   ((uint16_t) 8192U)

/* Команды для BT815/BT816 */
#define LV_EVE_CMD_BITMAP_TRANSFORM ((uint32_t) 0xFFFFFF21UL)
#define LV_EVE_CMD_SYNC             ((uint32_t) 0xFFFFFF42UL) /* не нужна специальная функция, просто воспользуйтесьEVE_cmd_dl(CMD_SYNC) */
#define LV_EVE_CMD_FLASHERASE       ((uint32_t) 0xFFFFFF44UL) /* не нужна специальная функция, просто воспользуйтесьEVE_cmd_dl(CMD_FLASHERASE) */
#define LV_EVE_CMD_FLASHWRITE       ((uint32_t) 0xFFFFFF45UL)
#define LV_EVE_CMD_FLASHREAD        ((uint32_t) 0xFFFFFF46UL)
#define LV_EVE_CMD_FLASHUPDATE      ((uint32_t) 0xFFFFFF47UL)
#define LV_EVE_CMD_FLASHDETACH      ((uint32_t) 0xFFFFFF48UL) /* не нужна специальная функция, просто воспользуйтесьEVE_cmd_dl(CMD_FLASHDETACH) */
#define LV_EVE_CMD_FLASHATTACH      ((uint32_t) 0xFFFFFF49UL) /* не нужна специальная функция, просто воспользуйтесьEVE_cmd_dl(CMD_FLASHATTACH) */
#define LV_EVE_CMD_FLASHFAST        ((uint32_t) 0xFFFFFF4AUL)
#define LV_EVE_CMD_FLASHSPIDESEL    ((uint32_t) 0xFFFFFF4BUL) /* не нужна специальная функция, просто воспользуйтесьEVE_cmd_dl(CMD_FLASHSPIDESEL) */
#define LV_EVE_CMD_FLASHSPITX       ((uint32_t) 0xFFFFFF4CUL)
#define LV_EVE_CMD_FLASHSPIRX       ((uint32_t) 0xFFFFFF4DUL)
#define LV_EVE_CMD_FLASHSOURCE      ((uint32_t) 0xFFFFFF4EUL)
#define LV_EVE_CMD_CLEARCACHE       ((uint32_t) 0xFFFFFF4FUL) /* не нужна специальная функция, просто воспользуйтесьEVE_cmd_dl(CMD_CLEARCACHE) */
#define LV_EVE_CMD_INFLATE2         ((uint32_t) 0xFFFFFF50UL)
#define LV_EVE_CMD_ROTATEAROUND     ((uint32_t) 0xFFFFFF51UL)
#define LV_EVE_CMD_RESETFONTS       ((uint32_t) 0xFFFFFF52UL) /* не нужна специальная функция, просто воспользуйтесьEVE_cmd_dl(CMD_RESETFONTS) */
#define LV_EVE_CMD_ANIMSTART        ((uint32_t) 0xFFFFFF53UL)
#define LV_EVE_CMD_ANIMSTOP         ((uint32_t) 0xFFFFFF54UL)
#define LV_EVE_CMD_ANIMXY           ((uint32_t) 0xFFFFFF55UL)
#define LV_EVE_CMD_ANIMDRAW         ((uint32_t) 0xFFFFFF56UL)
#define LV_EVE_CMD_GRADIENTA        ((uint32_t) 0xFFFFFF57UL)
#define LV_EVE_CMD_FILLWIDTH        ((uint32_t) 0xFFFFFF58UL)
#define LV_EVE_CMD_APPENDF          ((uint32_t) 0xFFFFFF59UL)
#define LV_EVE_CMD_ANIMFRAME        ((uint32_t) 0xFFFFFF5AUL)
#define LV_EVE_CMD_VIDEOSTARTF      ((uint32_t) 0xFFFFFF5FUL) /* не нужна специальная функция, просто воспользуйтесьEVE_cmd_dl(CMD_VIDEOSTARTF) */

/* Регистры для BT815/BT816 */
#define LV_EVE_REG_ADAPTIVE_FRAMERATE ((uint32_t) 0x0030257cUL)
#define LV_EVE_REG_PLAYBACK_PAUSE     ((uint32_t) 0x003025ecUL)
#define LV_EVE_REG_FLASH_STATUS       ((uint32_t) 0x003025f0UL)
#define LV_EVE_REG_FLASH_SIZE         ((uint32_t) 0x00309024UL)
#define LV_EVE_REG_PLAY_CONTROL       ((uint32_t) 0x0030914eUL)
#define LV_EVE_REG_COPRO_PATCH_PTR    ((uint32_t) 0x00309162UL)

/* Макросы для BT815/BT816 */

//#defineLV_EVE_BITMAP_EXT_FORMAT(формат) ((LV_EVE_DL_BITMAP_EXT_FORMAT) | ((формат) & 0xFFFFUL))
/**
 * @brief Установите расширенный формат растрового изображения.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_BITMAP_EXT_FORMAT(uint16_t format)
{
    return (LV_EVE_DL_BITMAP_EXT_FORMAT | format);
}

//#defineLV_EVE_BITMAP_SWIZZLE(r,g,b,a) ((LV_EVE_DL_BITMAP_SWIZZLE) | (((r) & 7UL) << 9U) | (((g) & 7UL) << 6U) | (((b) & 7UL) << 3U) | ((a) & 7UL))
/**
 * @brief Установите источник красного, зеленого, синего и альфа-каналов растрового изображения.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_BITMAP_SWIZZLE(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    uint32_t const redv = ((red & 7UL) << 9U);
    uint32_t const greenv = ((green & 7UL) << 6U);
    uint32_t const bluev = ((blue & 7UL) << 3U);
    uint32_t const alphav = (alpha & 7UL);
    return (LV_EVE_DL_BITMAP_SWIZZLE | redv | greenv | bluev | alphav);
}

//#defineLV_EVE_BITMAP_TRANSFORM_A_EXT(p,v) ((LV_EVE_DL_BITMAP_TRANSFORM_A) | (((p) & 1UL) << 17U) | ((v) & 0x1FFFFUL))
/**
 * @brief Установите коэффициент A матрицы преобразования растрового изображения.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_BITMAP_TRANSFORM_A(uint8_t prc, uint32_t val)
{
    uint32_t const prcv = ((prc & 1UL) << 17U);
    uint32_t const valv = (val & 0x1FFFFUL);
    return (LV_EVE_DL_BITMAP_TRANSFORM_A | prcv | valv);
}

//#defineLV_EVE_BITMAP_TRANSFORM_B_EXT(p,v) ((LV_EVE_DL_BITMAP_TRANSFORM_B) | (((p) & 1UL) << 17U) | ((v) & 0x1FFFFUL))
/**
 * @brief Установите коэффициент B матрицы преобразования растрового изображения.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_BITMAP_TRANSFORM_B(uint8_t prc, uint32_t val)
{
    uint32_t const prcv = ((prc & 1UL) << 17U);
    uint32_t const valv = (val & 0x1FFFFUL);
    return (LV_EVE_DL_BITMAP_TRANSFORM_B | prcv | valv);
}

//#defineLV_EVE_BITMAP_TRANSFORM_D_EXT(p,v) ((LV_EVE_DL_BITMAP_TRANSFORM_D) | (((p) & 1UL) << 17U) | ((v) & 0x1FFFFUL))
/**
 * @brief Установите коэффициент D матрицы преобразования растрового изображения.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_BITMAP_TRANSFORM_D(uint8_t prc, uint32_t val)
{
    uint32_t const prcv = ((prc & 1UL) << 17U);
    uint32_t const valv = (val & 0x1FFFFUL);
    return (LV_EVE_DL_BITMAP_TRANSFORM_D | prcv | valv);
}

//#defineLV_EVE_BITMAP_TRANSFORM_E_EXT(p,v) ((LV_EVE_DL_BITMAP_TRANSFORM_E) | (((p) & 1UL) << 17U) | ((v) & 0x1FFFFUL))
/**
 * @brief Установите коэффициент E матрицы преобразования растрового изображения.
 * @return 32-битное слово для использования сEVE_cmd_dl()
 */
static inline uint32_t LV_EVE_BITMAP_TRANSFORM_E(uint8_t prc, uint32_t val)
{
    uint32_t const prcv = ((prc & 1UL) << 17U);
    uint32_t const valv = (val & 0x1FFFFUL);
    return (LV_EVE_DL_BITMAP_TRANSFORM_E | prcv | valv);
}

//#defineLV_EVE_BITMAP_TRANSFORM_A(а)LV_EVE_BITMAP_TRANSFORM_A_EXT(0UL,(а))
//#defineLV_EVE_BITMAP_TRANSFORM_B(б)LV_EVE_BITMAP_TRANSFORM_B_EXT(0UL,(б))
//#defineLV_EVE_BITMAP_TRANSFORM_D(д)LV_EVE_BITMAP_TRANSFORM_D_EXT(0UL,(д))
//#defineLV_EVE_BITMAP_TRANSFORM_E(e)LV_EVE_BITMAP_TRANSFORM_E_EXT(0UL,(e))

#endif  /* LV_DRAW_EVE_EVE_GENERATION > 2 */

/* ########## EVE Generation 4: BT817 / BT818 definitions ########## */

#if LV_DRAW_EVE_EVE_GENERATION > 3

/* Команды для BT817/BT818 */
#define LV_EVE_CMD_ANIMFRAMERAM   ((uint32_t) 0xFFFFFF6DUL)
#define LV_EVE_CMD_ANIMSTARTRAM   ((uint32_t) 0xFFFFFF6EUL)
#define LV_EVE_CMD_APILEVEL       ((uint32_t) 0xFFFFFF63UL)
#define LV_EVE_CMD_CALIBRATESUB   ((uint32_t) 0xFFFFFF60UL)
#define LV_EVE_CMD_CALLLIST       ((uint32_t) 0xFFFFFF67UL)
#define LV_EVE_CMD_ENDLIST        ((uint32_t) 0xFFFFFF69UL) /* не нужна специальная функция, просто воспользуйтесьEVE_cmd_dl(CMD_ENDLIST) */
#define LV_EVE_CMD_FLASHPROGRAM   ((uint32_t) 0xFFFFFF70UL)
#define LV_EVE_CMD_FONTCACHE      ((uint32_t) 0xFFFFFF6BUL)
#define LV_EVE_CMD_FONTCACHEQUERY ((uint32_t) 0xFFFFFF6CUL)
#define LV_EVE_CMD_GETIMAGE       ((uint32_t) 0xFFFFFF64UL)
#define LV_EVE_CMD_HSF            ((uint32_t) 0xFFFFFF62UL)
#define LV_EVE_CMD_LINETIME       ((uint32_t) 0xFFFFFF5EUL)
#define LV_EVE_CMD_NEWLIST        ((uint32_t) 0xFFFFFF68UL)
#define LV_EVE_CMD_PCLKFREQ       ((uint32_t) 0xFFFFFF6AUL)
#define LV_EVE_CMD_RETURN         ((uint32_t) 0xFFFFFF66UL) /* не нужна специальная функция, просто воспользуйтесьEVE_cmd_dl(CMD_RETURN) */
#define LV_EVE_CMD_RUNANIM        ((uint32_t) 0xFFFFFF6FUL)
#define LV_EVE_CMD_TESTCARD       ((uint32_t) 0xFFFFFF61UL) /* не нужна специальная функция, просто воспользуйтесьEVE_cmd_dl(CMD_TESTCARD) */
#define LV_EVE_CMD_WAIT           ((uint32_t) 0xFFFFFF65UL)

/* Регистры для BT817/BT818 */
#define LV_EVE_REG_UNDERRUN      ((uint32_t) 0x0030260cUL)
#define LV_EVE_REG_AH_HCYCLE_MAX ((uint32_t) 0x00302610UL)
#define LV_EVE_REG_PCLK_FREQ     ((uint32_t) 0x00302614UL)
#define LV_EVE_REG_PCLK_2X       ((uint32_t) 0x00302618UL)
#define LV_EVE_REG_ANIM_ACTIVE   ((uint32_t) 0x0030902CUL)

#endif /*  LV_DRAW_EVE_EVE_GENERATION > 3 */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_EVE*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_DRAW_EVE_DISPLAY_DEFINES_H*/
