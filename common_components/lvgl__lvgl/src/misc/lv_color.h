/**
 * @file lv_color.h
 *
 */

#ifndef LV_COLOR_H
#define LV_COLOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "lv_assert.h"
#include "lv_math.h"
#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/
LV_EXPORT_CONST_INT(LV_COLOR_DEPTH);

#if LV_COLOR_DEPTH == 8
#define LV_COLOR_NATIVE_WITH_ALPHA_SIZE 2
#elif LV_COLOR_DEPTH == 16
#define LV_COLOR_NATIVE_WITH_ALPHA_SIZE 3
#elif LV_COLOR_DEPTH == 24
#define LV_COLOR_NATIVE_WITH_ALPHA_SIZE 4
#elif LV_COLOR_DEPTH == 32
#define LV_COLOR_NATIVE_WITH_ALPHA_SIZE 4
#endif

/**
 * Процент непрозрачности.
 */

enum _lv_opacity_level_t {
    LV_OPA_TRANSP = 0,
    LV_OPA_0      = 0,
    LV_OPA_10     = 25,
    LV_OPA_20     = 51,
    LV_OPA_30     = 76,
    LV_OPA_40     = 102,
    LV_OPA_50     = 127,
    LV_OPA_60     = 153,
    LV_OPA_70     = 178,
    LV_OPA_80     = 204,
    LV_OPA_90     = 229,
    LV_OPA_100    = 255,
    LV_OPA_COVER  = 255,
};

#define LV_OPA_MIN 2    /**< Fully transparent if opa <= LV_OPA_MIN */
#define LV_OPA_MAX 253  /**< Fully cover if opa >= LV_OPA_MAX */

/**
 * Получить размер пикселя цвета в битах, bpp.
 * @param cf        цветовой формат (`LV_COLOR_FORMAT_...`)
 * @return          размер пикселя в битах
 * @sa lv_color_format_get_bpp
 */
#define LV_COLOR_FORMAT_GET_BPP(cf) (       \
                                            (cf) == LV_COLOR_FORMAT_I1 ? 1 :        \
                                            (cf) == LV_COLOR_FORMAT_A1 ? 1 :        \
                                            (cf) == LV_COLOR_FORMAT_I2 ? 2 :        \
                                            (cf) == LV_COLOR_FORMAT_A2 ? 2 :        \
                                            (cf) == LV_COLOR_FORMAT_I4 ? 4 :        \
                                            (cf) == LV_COLOR_FORMAT_A4 ? 4 :        \
                                            (cf) == LV_COLOR_FORMAT_NEMA_TSC4 ? 4 : \
                                            (cf) == LV_COLOR_FORMAT_NEMA_TSC6 ? 6 : \
                                            (cf) == LV_COLOR_FORMAT_NEMA_TSC6A ? 6 : \
                                            (cf) == LV_COLOR_FORMAT_NEMA_TSC6AP ? 6 : \
                                            (cf) == LV_COLOR_FORMAT_L8 ? 8 :        \
                                            (cf) == LV_COLOR_FORMAT_A8 ? 8 :        \
                                            (cf) == LV_COLOR_FORMAT_I8 ? 8 :        \
                                            (cf) == LV_COLOR_FORMAT_ARGB2222 ? 8 :  \
                                            (cf) == LV_COLOR_FORMAT_NEMA_TSC12 ? 12 : \
                                            (cf) == LV_COLOR_FORMAT_NEMA_TSC12A ? 12 : \
                                            (cf) == LV_COLOR_FORMAT_AL88 ? 16 :     \
                                            (cf) == LV_COLOR_FORMAT_RGB565 ? 16 :   \
                                            (cf) == LV_COLOR_FORMAT_RGB565_SWAPPED ? 16 :   \
                                            (cf) == LV_COLOR_FORMAT_RGB565A8 ? 16 : \
                                            (cf) == LV_COLOR_FORMAT_YUY2 ? 16 :     \
                                            (cf) == LV_COLOR_FORMAT_ARGB1555 ? 16 : \
                                            (cf) == LV_COLOR_FORMAT_ARGB4444 ? 16 : \
                                            (cf) == LV_COLOR_FORMAT_ARGB8565 ? 24 : \
                                            (cf) == LV_COLOR_FORMAT_RGB888 ? 24 :   \
                                            (cf) == LV_COLOR_FORMAT_ARGB8888 ? 32 : \
                                            (cf) == LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED ? 32 : \
                                            (cf) == LV_COLOR_FORMAT_XRGB8888 ? 32 : \
                                            0                                       \
                                    )

/**
 * Получить размер пикселя цветового формата в байтах.
 * @param cf        цветовой формат (`LV_COLOR_FORMAT_...`)
 * @return          размер пикселя в байтах
 * @sa lv_color_format_get_size
 */
#define LV_COLOR_FORMAT_GET_SIZE(cf) ((LV_COLOR_FORMAT_GET_BPP(cf) + 7) >> 3)

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} lv_color_t;

typedef struct {
    uint16_t blue : 5;
    uint16_t green : 6;
    uint16_t red : 5;
} lv_color16_t;

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
} lv_color32_t;

typedef struct {
    uint16_t h;
    uint8_t s;
    uint8_t v;
} lv_color_hsv_t;

typedef struct {
    uint8_t lumi;
    uint8_t alpha;
} lv_color16a_t;

typedef enum {
    LV_COLOR_FORMAT_UNKNOWN           = 0,

    LV_COLOR_FORMAT_RAW               = 0x01,
    LV_COLOR_FORMAT_RAW_ALPHA         = 0x02,

    /*<=1-байтовые (+альфа) форматы*/
    LV_COLOR_FORMAT_L8                = 0x06,
    LV_COLOR_FORMAT_I1                = 0x07,
    LV_COLOR_FORMAT_I2                = 0x08,
    LV_COLOR_FORMAT_I4                = 0x09,
    LV_COLOR_FORMAT_I8                = 0x0A,
    LV_COLOR_FORMAT_A8                = 0x0E,

    /*2-байтовые (+альфа) форматы*/
    LV_COLOR_FORMAT_RGB565            = 0x12,
    LV_COLOR_FORMAT_ARGB8565          = 0x13,   /**< Not supported by sw renderer yet. */
    LV_COLOR_FORMAT_RGB565A8          = 0x14,   /**< Color array followed by Alpha array*/
    LV_COLOR_FORMAT_AL88              = 0x15,   /**< L8 with alpha >*/
    LV_COLOR_FORMAT_RGB565_SWAPPED    = 0x1B,

    /*3-байтовые (+альфа) форматы*/
    LV_COLOR_FORMAT_RGB888            = 0x0F,
    LV_COLOR_FORMAT_ARGB8888          = 0x10,
    LV_COLOR_FORMAT_XRGB8888          = 0x11,
    LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED          = 0x1A,

    /*Форматы не поддерживаются программным рендерером, но сохраняются здесь, чтобы GPU мог их использовать.*/
    LV_COLOR_FORMAT_A1                = 0x0B,
    LV_COLOR_FORMAT_A2                = 0x0C,
    LV_COLOR_FORMAT_A4                = 0x0D,
    LV_COLOR_FORMAT_ARGB1555          = 0x16,
    LV_COLOR_FORMAT_ARGB4444          = 0x17,
    LV_COLOR_FORMAT_ARGB2222          = 0X18,

    /* ссылка на https://wiki.videolan.org/YUV/ */
    /*YUV планарные форматы*/
    LV_COLOR_FORMAT_YUV_START         = 0x20,
    LV_COLOR_FORMAT_I420              = LV_COLOR_FORMAT_YUV_START,  /*YUV420 планарный (3 плоскости)*/
    LV_COLOR_FORMAT_I422              = 0x21,  /*YUV422 планарный (3 плоскости)*/
    LV_COLOR_FORMAT_I444              = 0x22,  /*YUV444 планарный (3 плоскости)*/
    LV_COLOR_FORMAT_I400              = 0x23,  /*YUV400 нет канала цветности*/
    LV_COLOR_FORMAT_NV21              = 0x24,  /*YUV420 плоский (плоскость 2), плоскость UV в 'V, U, V, U'*/
    LV_COLOR_FORMAT_NV12              = 0x25,  /*YUV420 плоский (плоскость 2), плоскость UV в 'U, V, U, V'*/

    /*YUV упакованные форматы*/
    LV_COLOR_FORMAT_YUY2              = 0x26,  /*YUV422 упакован как «Y U Y V»*/
    LV_COLOR_FORMAT_UYVY              = 0x27,  /*YUV422 упакован как «U Y V Y»*/

    LV_COLOR_FORMAT_YUV_END           = LV_COLOR_FORMAT_UYVY,

    LV_COLOR_FORMAT_PROPRIETARY_START = 0x30,

    LV_COLOR_FORMAT_NEMA_TSC_START    = LV_COLOR_FORMAT_PROPRIETARY_START,
    LV_COLOR_FORMAT_NEMA_TSC4         = LV_COLOR_FORMAT_NEMA_TSC_START,
    LV_COLOR_FORMAT_NEMA_TSC6         = 0x31,
    LV_COLOR_FORMAT_NEMA_TSC6A        = 0x32,
    LV_COLOR_FORMAT_NEMA_TSC6AP       = 0x33,
    LV_COLOR_FORMAT_NEMA_TSC12        = 0x34,
    LV_COLOR_FORMAT_NEMA_TSC12A       = 0x35,
    LV_COLOR_FORMAT_NEMA_TSC_END      = LV_COLOR_FORMAT_NEMA_TSC12A,

    /*Цветовые форматы, в которых LVGL может отображать*/
#if LV_COLOR_DEPTH == 1
    LV_COLOR_FORMAT_NATIVE            = LV_COLOR_FORMAT_I1,
    LV_COLOR_FORMAT_NATIVE_WITH_ALPHA = LV_COLOR_FORMAT_I1,
#elif LV_COLOR_DEPTH == 8
    LV_COLOR_FORMAT_NATIVE            = LV_COLOR_FORMAT_L8,
    LV_COLOR_FORMAT_NATIVE_WITH_ALPHA = LV_COLOR_FORMAT_AL88,
#elif LV_COLOR_DEPTH == 16
    LV_COLOR_FORMAT_NATIVE            = LV_COLOR_FORMAT_RGB565,
    LV_COLOR_FORMAT_NATIVE_WITH_ALPHA = LV_COLOR_FORMAT_RGB565A8,
#elif LV_COLOR_DEPTH == 24
    LV_COLOR_FORMAT_NATIVE            = LV_COLOR_FORMAT_RGB888,
    LV_COLOR_FORMAT_NATIVE_WITH_ALPHA = LV_COLOR_FORMAT_ARGB8888,
#elif LV_COLOR_DEPTH == 32
    LV_COLOR_FORMAT_NATIVE            = LV_COLOR_FORMAT_XRGB8888,
    LV_COLOR_FORMAT_NATIVE_WITH_ALPHA = LV_COLOR_FORMAT_ARGB8888,
#else
#error "LV_COLOR_DEPTH should be 1, 8, 16, 24 or 32"
#endif

} lv_color_format_t;

#define LV_COLOR_FORMAT_IS_ALPHA_ONLY(cf) ((cf) >= LV_COLOR_FORMAT_A1 && (cf) <= LV_COLOR_FORMAT_A8)
#define LV_COLOR_FORMAT_IS_INDEXED(cf) ((cf) >= LV_COLOR_FORMAT_I1 && (cf) <= LV_COLOR_FORMAT_I8)
#define LV_COLOR_FORMAT_IS_YUV(cf)  ((cf) >= LV_COLOR_FORMAT_YUV_START && (cf) <= LV_COLOR_FORMAT_YUV_END)
#define LV_COLOR_INDEXED_PALETTE_SIZE(cf) ((cf) == LV_COLOR_FORMAT_I1 ? 2 :\
                                           (cf) == LV_COLOR_FORMAT_I2 ? 4 :\
                                           (cf) == LV_COLOR_FORMAT_I4 ? 16 :\
                                           (cf) == LV_COLOR_FORMAT_I8 ? 256 : 0)

/**********************
 * MACROS
 **********************/

#define LV_COLOR_MAKE(r8, g8, b8) {b8, g8, r8}

#define LV_OPA_MIX2(a1, a2) ((lv_opa_t)(((int32_t)(a1) * (a2)) >> 8))
#define LV_OPA_MIX3(a1, a2, a3) ((lv_opa_t)(((int32_t)(a1) * (a2) * (a3)) >> 16))

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Получить размер пикселя цвета в битах, bpp.
 * @param cf        цветовой формат (`LV_COLOR_FORMAT_...`)
 * @return          размер пикселя в битах
 * @sa              LV_COLOR_FORMAT_GET_BPP
 */
uint8_t lv_color_format_get_bpp(lv_color_format_t cf);

/**
 * Получить размер пикселя цветового формата в байтах.
 * @param cf        цветовой формат (`LV_COLOR_FORMAT_...`)
 * @return          размер пикселя в байтах
 * @sa              LV_COLOR_FORMAT_GET_SIZE
 */
uint8_t lv_color_format_get_size(lv_color_format_t cf);

/**
 * Проверьте, имеет ли цветовой формат альфа-канал или нет.
 * @param src_cf    цветовой формат (`LV_COLOR_FORMAT_...`)
 * @return          true: есть альфа-канал; false: нет альфа-канала
 */
bool lv_color_format_has_alpha(lv_color_format_t src_cf);

/**
 * Создайте цвет ARGB8888 из RGB888 + альфа.
 * @param color     цвет RGB888
 * @param opa       альфа-значение
 * @return          цвет ARGB8888
 */
lv_color32_t lv_color_to_32(lv_color_t color, lv_opa_t opa);

/**
 * Преобразование цвета RGB888 в целое число
 * @param c     цвет RGB888
 * @return      `c` как целое число
 */
uint32_t lv_color_to_int(lv_color_t c);

/**
 * Проверьте, равны ли два цвета RGB888
 * @param c1    первый цвет
 * @param c2    второй цвет
 * @return      правда: равно
 */
bool lv_color_eq(lv_color_t c1, lv_color_t c2);

/**
 * Проверьте, равны ли два цвета ARGB8888
 * @param c1    первый цвет
 * @param c2    второй цвет
 * @return      правда: равно
 */
bool lv_color32_eq(lv_color32_t c1, lv_color32_t c2);

/**
 * Создайте цвет ввода0x000000..0xffffff.
 * @param c     шестнадцатеричный ввод
 * @return      цвет
 */
lv_color_t lv_color_hex(uint32_t c);

/**
 * Создайте цвет RGB888.
 * @param r     красный канал (0..255)
 * @param g     зеленый канал (0..255)
 * @param b     синий канал (0..255)
 * @return      цвет
 */
lv_color_t lv_color_make(uint8_t r, uint8_t g, uint8_t b);

/**
 * Создайте цвет ARGB8888.
 * @param r     красный канал (0..255)
 * @param g     зеленый канал (0..255)
 * @param b     синий канал (0..255)
 * @param a     альфа-канал (0..255)
 * @return      цвет
 */
lv_color32_t lv_color32_make(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/**
 * Создайте цвет ввода0x000..0xfff.
 * @param c     шестнадцатеричный ввод (например,0x123будет0x112233)
 * @return      цвет
 */
lv_color_t lv_color_hex3(uint32_t c);

/**
 * Проверьте, находится ли цвет с цветомRGB888в цвете, длинаl_colorиh_color.
 * @param color   цвет, который нужно проверить
 * @param l_color цвет нижней границы
 * @param h_color цвет верхней границы
 * @return        true: пиксель находится в цветовом диапазоне
 */
static inline bool lv_color_is_in_range(lv_color_t color, lv_color_t l_color, lv_color_t h_color)
{
    return (color.red <= h_color.red &&
            color.green <= h_color.green &&
            color.blue <= h_color.blue &&
            color.red >= l_color.red &&
            color.green >= l_color.green &&
            color.blue >= l_color.blue);
}

/**
 * Преобразование цвета RGB565 в RGB888
 * @param c       цветRGB565на lv_color16_t
 * @return        цвет
 */
static inline lv_color_t lv_color16_to_color(lv_color16_t c)
{
    return lv_color_make(c.red << 3, c.green << 2, c.blue << 3);
}

/**
 * Преобразование цветаRGB888в RGB565, хранящегося в `uint16_t`.
 * @param color     и цвет RGB888
 * @return          `color` какRGB565на `uin16_t`
 */
uint16_t lv_color_to_u16(lv_color_t color);

/**
 * Преобразование цветаRGB888в XRGB8888, хранящегося в `uint32_t`.
 * @param color     и цвет RGB888
 * @return          `color` какXRGB8888на`uin32_t`(альфа-канал всегда установлен на 0xFF)
 */
uint32_t lv_color_to_u32(lv_color_t color);

/**
 * Смешайте два цвета RGB565
 * @param c1        первый цвет (обычно цвет переднего плана)
 * @param c2        второй цвет (обычно цвет фона)
 * @param микс 0..255 или LV_OPA_0 /10/20...
 * @return          микс == 0: c2
 *                  микс == 255:с1
 *                  смесь == 128: 0,5 х с1 + 0,5 х с2
 */
uint16_t LV_ATTRIBUTE_FAST_MEM lv_color_16_16_mix(uint16_t c1, uint16_t c2, uint8_t mix);

/**
 * Смешать белый цвет с цветом
 * @param c     базовый цвет
 * @param lvl   интенсивность белого (0: без изменений, 255: полностью белый)
 * @return      смешанный цвет
 */
lv_color_t lv_color_lighten(lv_color_t c, lv_opa_t lvl);

/**
 * Смешать черный с цветом
 * @param c     базовый цвет
 * @param lvl   интенсивность черного цвета (0: без изменений, 255: полностью черный)
 * @return      смешанный цвет
 */
lv_color_t lv_color_darken(lv_color_t c, lv_opa_t lvl);

/**
 * Преобразование цвета HSV в RGB
 * @param h оттенок [0..359]
 * @param s насыщенность [0..100]
 * @param v значение [0..100]
 * @return заданный цветRGBвRGB(с глубиной LV_COLOR_DEPTH)
 */
lv_color_t lv_color_hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v);

/**
 * Преобразование 32-битного цвета RGB в HSV.
 * @param r8 8-битный красный
 * @param g8 8-битный зеленый
 * @param b8 8-битный синий
 * @return заданный цветRGBв HSV
 */
lv_color_hsv_t lv_color_rgb_to_hsv(uint8_t r8, uint8_t g8, uint8_t b8);

/**
 * Преобразовать цвет в HSV
 * @param color цвет
 * @return данный цвет в HSV
 */
lv_color_hsv_t lv_color_to_hsv(lv_color_t color);

/*Source: https://vuetifyjs.com/en/styles/colors/#material-colors*/

/**
 * Помощник для белого цвета
 * @return      белый цвет
 */
lv_color_t lv_color_white(void);

/**
 * Помощник для черного цвета
 * @return      черный цвет
 */
lv_color_t lv_color_black(void);

void lv_color_premultiply(lv_color32_t * c);

void lv_color16_premultiply(lv_color16_t * c, lv_opa_t a);

/**
 * Получите яркость цвета: яркость = 0,3 R + 0,59 G + 0,11 B.
 * @param c цвет
 * @return яркость [0..255]
 */
uint8_t lv_color_luminance(lv_color_t c);

/**
 * Получите яркость цвета16: яркость = 0,3 R + 0,59 G + 0,11 B.
 * @param c цвет
 * @return яркость [0..255]
 */
uint8_t lv_color16_luminance(const lv_color16_t c);

/**
 * Получите яркость цвета24: яркость = 0,3 R + 0,59 G + 0,11 B.
 * @param c цвет
 * @return яркость [0..255]
 */
uint8_t lv_color24_luminance(const uint8_t * c);

/**
 * Получите яркость цвета32: яркость = 0,3 R + 0,59 G + 0,11 B.
 * @param c цвет
 * @return яркость [0..255]
 */
uint8_t lv_color32_luminance(lv_color32_t c);


/**
 * Поменяйте порядок байтов цвета rgb565.
 * @param c цвет
 * @return замененный цвет
 */
static inline uint16_t LV_ATTRIBUTE_FAST_MEM lv_color_swap_16(uint16_t c)
{
    return (c >> 8) | (c << 8);
}

/**********************
 *      MACROS
 **********************/

#include "lv_palette.h"
#include "lv_color_op.h"

LV_ATTRIBUTE_EXTERN_DATA extern const lv_color_filter_dsc_t lv_color_filter_shade;

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_COLOR_H*/
