/*******************************************************************************
 * Size: 14 px
 * Bpp: 1
 * Opts: --bpp 1 --size 14 --no-compress --stride 1 --align 1 --font JetBrainsMono.ttf --range 32-127,1025,1040-1103,1105,8470,8212,171,187,8230,176,9472-9599 --format lvgl -o term_font_20.c
 ******************************************************************************/

#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif



#ifndef TERM_FONT_20
#define TERM_FONT_20 1
#endif

#if TERM_FONT_20

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0x55, 0x54, 0x30,

    /* U+0022 "\"" */
    0x99, 0x99,

    /* U+0023 "#" */
    0x12, 0x12, 0x7f, 0x24, 0x24, 0x24, 0xfe, 0x24,
    0x44, 0x48,

    /* U+0024 "$" */
    0x10, 0x43, 0x9f, 0x96, 0x4d, 0x1e, 0x1c, 0x59,
    0x65, 0x78, 0x41, 0x0,

    /* U+0025 "%" */
    0xf1, 0x92, 0x94, 0xf4, 0x8, 0x16, 0x29, 0x29,
    0x49, 0x86,

    /* U+0026 "&" */
    0x38, 0x91, 0x2, 0x6, 0x12, 0xe5, 0x44, 0x8c,
    0xec,

    /* U+0027 "'" */
    0xf0,

    /* U+0028 "(" */
    0x16, 0x48, 0x88, 0x88, 0x88, 0xc4, 0x31,

    /* U+0029 ")" */
    0x86, 0x21, 0x11, 0x11, 0x11, 0x32, 0xc8,

    /* U+002A "*" */
    0x10, 0x23, 0x5b, 0xe3, 0x5, 0x13, 0x0,

    /* U+002B "+" */
    0x10, 0x23, 0xf8, 0x81, 0x2, 0x0,

    /* U+002C "," */
    0x6d, 0x2c,

    /* U+002D "-" */
    0xf0,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0x4, 0x10, 0xc2, 0x8, 0x61, 0xc, 0x20, 0x86,
    0x10, 0x43, 0x0,

    /* U+0030 "0" */
    0x7b, 0x38, 0x61, 0x96, 0x18, 0x61, 0xcd, 0xe0,

    /* U+0031 "1" */
    0x33, 0x49, 0x4, 0x10, 0x41, 0x4, 0x13, 0xf0,

    /* U+0032 "2" */
    0x7b, 0x38, 0x41, 0xc, 0x63, 0x18, 0xc3, 0xf0,

    /* U+0033 "3" */
    0x7c, 0x23, 0xe, 0xc, 0x10, 0x61, 0xcd, 0xe0,

    /* U+0034 "4" */
    0x8, 0x61, 0x8, 0x47, 0x18, 0x7f, 0x4, 0x10,

    /* U+0035 "5" */
    0xfe, 0x8, 0x3e, 0xcc, 0x10, 0x41, 0x8d, 0xe0,

    /* U+0036 "6" */
    0x10, 0x82, 0x1e, 0x4e, 0x18, 0x61, 0xcd, 0xe0,

    /* U+0037 "7" */
    0xff, 0xa, 0x30, 0x40, 0x83, 0x4, 0x18, 0x20,
    0xc0,

    /* U+0038 "8" */
    0x7a, 0x18, 0x61, 0x79, 0xe8, 0x61, 0x85, 0xe0,

    /* U+0039 "9" */
    0x7b, 0x38, 0x61, 0xcd, 0xe0, 0x84, 0x30, 0x80,

    /* U+003A ":" */
    0xf0, 0xf,

    /* U+003B ";" */
    0x6c, 0x0, 0x1b, 0x4b, 0x0,

    /* U+003C "<" */
    0x0, 0x33, 0xb8, 0x81, 0x81, 0x81,

    /* U+003D "=" */
    0xfc, 0x0, 0x0, 0xfc,

    /* U+003E ">" */
    0x3, 0x7, 0x7, 0x4, 0x66, 0x20,

    /* U+003F "?" */
    0xf0, 0x42, 0x13, 0x10, 0x80, 0x3, 0x0,

    /* U+0040 "@" */
    0x3c, 0x8a, 0xc, 0x79, 0x32, 0x64, 0xc9, 0x93,
    0x1e, 0x2, 0x3, 0x80,

    /* U+0041 "A" */
    0x30, 0x60, 0xa3, 0x44, 0x89, 0x9f, 0x62, 0x85,
    0xc,

    /* U+0042 "B" */
    0xfa, 0x18, 0x61, 0xfa, 0x38, 0x61, 0x8f, 0xe0,

    /* U+0043 "C" */
    0x7b, 0x18, 0x60, 0x82, 0x8, 0x21, 0xc5, 0xe0,

    /* U+0044 "D" */
    0xfa, 0x38, 0x61, 0x86, 0x18, 0x61, 0x8f, 0xe0,

    /* U+0045 "E" */
    0xfe, 0x8, 0x20, 0xfa, 0x8, 0x20, 0x83, 0xf0,

    /* U+0046 "F" */
    0xfe, 0x8, 0x20, 0xfa, 0x8, 0x20, 0x82, 0x0,

    /* U+0047 "G" */
    0x7b, 0x38, 0x60, 0x82, 0x78, 0x61, 0xc5, 0xe0,

    /* U+0048 "H" */
    0x86, 0x18, 0x61, 0xfe, 0x18, 0x61, 0x86, 0x10,

    /* U+0049 "I" */
    0xf9, 0x8, 0x42, 0x10, 0x84, 0x27, 0xc0,

    /* U+004A "J" */
    0x3c, 0x10, 0x41, 0x4, 0x10, 0x61, 0xcd, 0xe0,

    /* U+004B "K" */
    0x8e, 0x29, 0xa4, 0xf2, 0x49, 0xa2, 0x8e, 0x10,

    /* U+004C "L" */
    0x82, 0x8, 0x20, 0x82, 0x8, 0x20, 0x83, 0xf0,

    /* U+004D "M" */
    0xcf, 0x3c, 0xed, 0xb6, 0xd8, 0x61, 0x86, 0x10,

    /* U+004E "N" */
    0xc7, 0x1e, 0x69, 0xa6, 0x59, 0x67, 0x8e, 0x30,

    /* U+004F "O" */
    0x7a, 0x18, 0x61, 0x86, 0x18, 0x61, 0xc5, 0xe0,

    /* U+0050 "P" */
    0xfa, 0x38, 0x61, 0x8f, 0xe8, 0x20, 0x82, 0x0,

    /* U+0051 "Q" */
    0x7b, 0x38, 0x61, 0x86, 0x18, 0x61, 0xcd, 0xe0,
    0x82, 0x4,

    /* U+0052 "R" */
    0xfa, 0x18, 0x61, 0xfa, 0x49, 0xa2, 0x8e, 0x10,

    /* U+0053 "S" */
    0x7a, 0x18, 0x60, 0x70, 0x60, 0x61, 0x85, 0xe0,

    /* U+0054 "T" */
    0xfe, 0x20, 0x40, 0x81, 0x2, 0x4, 0x8, 0x10,
    0x20,

    /* U+0055 "U" */
    0x86, 0x18, 0x61, 0x86, 0x18, 0x61, 0x85, 0xe0,

    /* U+0056 "V" */
    0x87, 0xb, 0x12, 0x24, 0xc9, 0x1a, 0x14, 0x30,
    0x60,

    /* U+0057 "W" */
    0xd9, 0x59, 0x59, 0x59, 0x5b, 0x56, 0x56, 0x66,
    0x66, 0x66,

    /* U+0058 "X" */
    0x84, 0x99, 0xa1, 0xc3, 0x6, 0xe, 0x24, 0xc5,
    0xc,

    /* U+0059 "Y" */
    0x82, 0x89, 0x11, 0x42, 0x82, 0x4, 0x8, 0x10,
    0x20,

    /* U+005A "Z" */
    0xfc, 0x30, 0x86, 0x10, 0x86, 0x10, 0xc3, 0xf0,

    /* U+005B "[" */
    0xf2, 0x49, 0x24, 0x92, 0x4e,

    /* U+005C "\\" */
    0x83, 0x4, 0x10, 0x20, 0x83, 0x4, 0x10, 0x60,
    0x82, 0xc, 0x10,

    /* U+005D "]" */
    0xe4, 0x92, 0x49, 0x24, 0x9e,

    /* U+005E "^" */
    0x10, 0xc2, 0x92, 0x4e, 0x10,

    /* U+005F "_" */
    0xfe,

    /* U+0060 "`" */
    0x90,

    /* U+0061 "a" */
    0x7b, 0x10, 0x5f, 0x86, 0x18, 0xdd,

    /* U+0062 "b" */
    0x82, 0xf, 0xb3, 0x86, 0x18, 0x61, 0xce, 0xe0,

    /* U+0063 "c" */
    0x7b, 0x18, 0x60, 0x82, 0x1c, 0x5e,

    /* U+0064 "d" */
    0x4, 0x17, 0xf3, 0x86, 0x18, 0x61, 0xcd, 0xd0,

    /* U+0065 "e" */
    0x7a, 0x18, 0x7f, 0x82, 0xc, 0x5e,

    /* U+0066 "f" */
    0x1e, 0x40, 0x87, 0xf2, 0x4, 0x8, 0x10, 0x20,
    0x40,

    /* U+0067 "g" */
    0x77, 0x38, 0x61, 0x86, 0x1c, 0xdd, 0x4, 0x17,
    0x80,

    /* U+0068 "h" */
    0x82, 0xb, 0xb3, 0x86, 0x18, 0x61, 0x86, 0x10,

    /* U+0069 "i" */
    0x0, 0x60, 0x7, 0x81, 0x2, 0x4, 0x8, 0x10,
    0x23, 0xf8,

    /* U+006A "j" */
    0x0, 0xc1, 0xf0, 0x84, 0x21, 0x8, 0x42, 0x11,
    0xf8,

    /* U+006B "k" */
    0x82, 0x8, 0xe2, 0x93, 0xc9, 0x26, 0x8a, 0x30,

    /* U+006C "l" */
    0xf0, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0xf,

    /* U+006D "m" */
    0xfe, 0x59, 0x65, 0x96, 0x59, 0x65,

    /* U+006E "n" */
    0xbb, 0x38, 0x61, 0x86, 0x18, 0x61,

    /* U+006F "o" */
    0x7b, 0x38, 0x61, 0x86, 0x1c, 0xde,

    /* U+0070 "p" */
    0xbb, 0x38, 0x61, 0x86, 0x1c, 0xfe, 0x82, 0x8,
    0x0,

    /* U+0071 "q" */
    0x77, 0x38, 0x61, 0x86, 0x1c, 0xdf, 0x4, 0x10,
    0x40,

    /* U+0072 "r" */
    0xbb, 0x38, 0x60, 0x82, 0x8, 0x20,

    /* U+0073 "s" */
    0x7a, 0x18, 0x3c, 0x3c, 0x18, 0x5e,

    /* U+0074 "t" */
    0x20, 0x43, 0xf9, 0x2, 0x4, 0x8, 0x10, 0x20,
    0x3c,

    /* U+0075 "u" */
    0x86, 0x18, 0x61, 0x86, 0x1c, 0xde,

    /* U+0076 "v" */
    0x85, 0x89, 0x12, 0x64, 0x85, 0xe, 0x18,

    /* U+0077 "w" */
    0x49, 0x59, 0x5a, 0x5a, 0x5a, 0x56, 0x66, 0x26,

    /* U+0078 "x" */
    0xc4, 0x98, 0xe1, 0x83, 0xd, 0x13, 0x62,

    /* U+0079 "y" */
    0x85, 0x89, 0x32, 0x46, 0x87, 0xc, 0x8, 0x30,
    0x40, 0x80,

    /* U+007A "z" */
    0xfc, 0x31, 0x84, 0x21, 0x8c, 0x3f,

    /* U+007B "{" */
    0xc, 0x41, 0x4, 0x10, 0x4e, 0x4, 0x10, 0x41,
    0x4, 0xc,

    /* U+007C "|" */
    0xff, 0xfc,

    /* U+007D "}" */
    0xc0, 0x82, 0x8, 0x20, 0x81, 0xc8, 0x20, 0x82,
    0x8, 0xc0,

    /* U+007E "~" */
    0xe6, 0xd9, 0xc0,

    /* U+00AB "«" */
    0x13, 0x26, 0x6c, 0xc8, 0x6c, 0x26, 0x13,

    /* U+00B0 "°" */
    0x69, 0x96,

    /* U+00BB "»" */
    0x4c, 0x24, 0x32, 0x19, 0x32, 0x24, 0x4c,

    /* U+0401 "Ё" */
    0x6d, 0xb0, 0x0, 0xfe, 0x8, 0x20, 0xfe, 0x8,
    0x20, 0x83, 0xf0,

    /* U+0410 "А" */
    0x30, 0x60, 0xa3, 0x44, 0x89, 0x9f, 0x62, 0x85,
    0xc,

    /* U+0411 "Б" */
    0xfe, 0x8, 0x20, 0xfa, 0x38, 0x61, 0x8f, 0xe0,

    /* U+0412 "В" */
    0xfa, 0x18, 0x61, 0xfa, 0x38, 0x61, 0x8f, 0xe0,

    /* U+0413 "Г" */
    0xfe, 0x8, 0x20, 0x82, 0x8, 0x20, 0x82, 0x0,

    /* U+0414 "Д" */
    0x3e, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
    0x62, 0xff, 0x81, 0x81,

    /* U+0415 "Е" */
    0xfe, 0x8, 0x20, 0xfa, 0x8, 0x20, 0x83, 0xf0,

    /* U+0416 "Ж" */
    0x49, 0x35, 0x8a, 0x85, 0x43, 0xe1, 0x50, 0xa8,
    0xd6, 0x49, 0x24, 0x80,

    /* U+0417 "З" */
    0x7a, 0x18, 0x43, 0x38, 0x30, 0x61, 0xcd, 0xe0,

    /* U+0418 "И" */
    0x8e, 0x39, 0xe5, 0x96, 0x9a, 0x79, 0xc7, 0x10,

    /* U+0419 "Й" */
    0x49, 0x27, 0x80, 0x8e, 0x39, 0xe5, 0x96, 0x9a,
    0x79, 0xc7, 0x10,

    /* U+041A "К" */
    0x8e, 0x29, 0xa4, 0xf2, 0x49, 0xa2, 0x8e, 0x10,

    /* U+041B "Л" */
    0x7d, 0x14, 0x51, 0x45, 0x14, 0x51, 0x46, 0x10,

    /* U+041C "М" */
    0xcf, 0x3c, 0xed, 0xb6, 0xd8, 0x61, 0x86, 0x10,

    /* U+041D "Н" */
    0x86, 0x18, 0x61, 0xfe, 0x18, 0x61, 0x86, 0x10,

    /* U+041E "О" */
    0x7a, 0x18, 0x61, 0x86, 0x18, 0x61, 0xc5, 0xe0,

    /* U+041F "П" */
    0xfe, 0x18, 0x61, 0x86, 0x18, 0x61, 0x86, 0x10,

    /* U+0420 "Р" */
    0xfa, 0x38, 0x61, 0x8f, 0xe8, 0x20, 0x82, 0x0,

    /* U+0421 "С" */
    0x7b, 0x18, 0x60, 0x82, 0x8, 0x21, 0xc5, 0xe0,

    /* U+0422 "Т" */
    0xfe, 0x20, 0x40, 0x81, 0x2, 0x4, 0x8, 0x10,
    0x20,

    /* U+0423 "У" */
    0x87, 0x89, 0x32, 0x42, 0x85, 0xc, 0x8, 0x30,
    0x40,

    /* U+0424 "Ф" */
    0x10, 0xfb, 0x5c, 0x99, 0x32, 0x64, 0xc9, 0xd6,
    0xf8, 0x40,

    /* U+0425 "Х" */
    0x84, 0x99, 0xa1, 0xc3, 0x6, 0xe, 0x24, 0xc5,
    0xc,

    /* U+0426 "Ц" */
    0x89, 0x12, 0x24, 0x48, 0x91, 0x22, 0x44, 0x89,
    0xfc, 0x8, 0x10,

    /* U+0427 "Ч" */
    0x86, 0x18, 0x61, 0xc5, 0xf0, 0x41, 0x4, 0x10,

    /* U+0428 "Ш" */
    0x93, 0x26, 0x4c, 0x99, 0x32, 0x64, 0xc9, 0x93,
    0xfc,

    /* U+0429 "Щ" */
    0x95, 0x2a, 0x54, 0xa9, 0x52, 0xa5, 0x4a, 0x95,
    0xfc, 0x8, 0x10,

    /* U+042A "Ъ" */
    0xe0, 0x20, 0x20, 0x20, 0x3e, 0x23, 0x21, 0x21,
    0x23, 0x3e,

    /* U+042B "Ы" */
    0x86, 0x18, 0x61, 0xe6, 0x59, 0x65, 0x97, 0x90,

    /* U+042C "Ь" */
    0x82, 0x8, 0x20, 0xfa, 0x38, 0x61, 0x8f, 0xe0,

    /* U+042D "Э" */
    0x7a, 0x18, 0x41, 0x7c, 0x10, 0x61, 0xcd, 0xe0,

    /* U+042E "Ю" */
    0x9d, 0x46, 0x8d, 0x1e, 0x34, 0x68, 0xd1, 0xa3,
    0x38,

    /* U+042F "Я" */
    0x7f, 0x18, 0x61, 0xc5, 0xf6, 0x51, 0x46, 0x10,

    /* U+0430 "а" */
    0x7b, 0x10, 0x5f, 0x86, 0x18, 0xdd,

    /* U+0431 "б" */
    0x3d, 0x8, 0x2e, 0xce, 0x18, 0x61, 0xcd, 0xe0,

    /* U+0432 "в" */
    0xfa, 0x18, 0x61, 0xfa, 0x18, 0x7e,

    /* U+0433 "г" */
    0xfc, 0x21, 0x8, 0x42, 0x10,

    /* U+0434 "д" */
    0x3c, 0x48, 0x91, 0x22, 0x44, 0x99, 0x7f, 0x83,
    0x4,

    /* U+0435 "е" */
    0x7a, 0x18, 0x7f, 0x82, 0xc, 0x5e,

    /* U+0436 "ж" */
    0x93, 0x25, 0x52, 0xa7, 0xca, 0xb5, 0xc9,

    /* U+0437 "з" */
    0x7b, 0x10, 0x4e, 0x4, 0x1c, 0x5e,

    /* U+0438 "и" */
    0x8e, 0x39, 0x65, 0xa6, 0x9c, 0x71,

    /* U+0439 "й" */
    0x49, 0xe0, 0x23, 0x8e, 0x59, 0x69, 0xa7, 0x1c,
    0x40,

    /* U+043A "к" */
    0x8e, 0x69, 0x38, 0x92, 0x68, 0xa3,

    /* U+043B "л" */
    0x7d, 0x14, 0x51, 0x45, 0x14, 0x61,

    /* U+043C "м" */
    0xcf, 0x3c, 0xed, 0xb6, 0xd8, 0x61,

    /* U+043D "н" */
    0x86, 0x18, 0x61, 0xfe, 0x18, 0x61,

    /* U+043E "о" */
    0x7b, 0x38, 0x61, 0x86, 0x1c, 0xde,

    /* U+043F "п" */
    0xfe, 0x18, 0x61, 0x86, 0x18, 0x61,

    /* U+0440 "р" */
    0xbb, 0x38, 0x61, 0x86, 0x1c, 0xfe, 0x82, 0x8,
    0x0,

    /* U+0441 "с" */
    0x7b, 0x18, 0x60, 0x82, 0x1c, 0x5e,

    /* U+0442 "т" */
    0xfe, 0x20, 0x40, 0x81, 0x2, 0x4, 0x8,

    /* U+0443 "у" */
    0x85, 0x89, 0x32, 0x46, 0x87, 0xc, 0x8, 0x30,
    0x40, 0x80,

    /* U+0444 "ф" */
    0x10, 0x21, 0xf6, 0xb9, 0x32, 0x64, 0xc9, 0xd6,
    0xf8, 0x40, 0x81, 0x0,

    /* U+0445 "х" */
    0xc4, 0x98, 0xe1, 0x83, 0xd, 0x13, 0x62,

    /* U+0446 "ц" */
    0x89, 0x12, 0x24, 0x48, 0x91, 0x22, 0x7f, 0x2,
    0x4,

    /* U+0447 "ч" */
    0x86, 0x18, 0x71, 0x7c, 0x10, 0x41,

    /* U+0448 "ш" */
    0x93, 0x26, 0x4c, 0x99, 0x32, 0x64, 0xff,

    /* U+0449 "щ" */
    0x95, 0x2a, 0x54, 0xa9, 0x52, 0xa5, 0x7f, 0x2,
    0x4,

    /* U+044A "ъ" */
    0xe0, 0x20, 0x20, 0x3e, 0x21, 0x21, 0x21, 0x3e,

    /* U+044B "ы" */
    0x86, 0x18, 0x79, 0x96, 0x59, 0x79,

    /* U+044C "ь" */
    0x82, 0x8, 0x3e, 0x86, 0x18, 0x7e,

    /* U+044D "э" */
    0x7a, 0x10, 0x4f, 0x6, 0x1c, 0xde,

    /* U+044E "ю" */
    0x9d, 0x46, 0x8f, 0x1a, 0x34, 0x68, 0xce,

    /* U+044F "я" */
    0x7e, 0x18, 0x61, 0x7d, 0x94, 0x61,

    /* U+0451 "ё" */
    0xcf, 0x30, 0x1e, 0x86, 0x1f, 0xe0, 0x83, 0x17,
    0x80,

    /* U+2014 "—" */
    0xff,

    /* U+2026 "…" */
    0x93, 0x24,

    /* U+2116 "№" */
    0x97, 0xd5, 0xd5, 0xd5, 0xd5, 0xd7, 0xb0, 0xb0,
    0xb0, 0x90,

    /* U+2500 "─" */
    0xff, 0x80,

    /* U+2501 "━" */
    0xff, 0xff, 0xff, 0xe0,

    /* U+2502 "│" */
    0xff, 0xff, 0xc0,

    /* U+2503 "┃" */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc,

    /* U+2504 "┄" */
    0xdb,

    /* U+2505 "┅" */
    0xdb, 0xdb, 0xdb,

    /* U+2506 "┆" */
    0xf3, 0xcf,

    /* U+2507 "┇" */
    0xff, 0xfe, 0x3f, 0xff, 0x81, 0xff, 0xe0,

    /* U+2508 "┈" */
    0xaa,

    /* U+2509 "┉" */
    0xab, 0x56, 0xa8,

    /* U+250A "┊" */
    0xee, 0xe7,

    /* U+250B "┋" */
    0xff, 0x8f, 0xf8, 0xff, 0x81, 0xff,

    /* U+250C "┌" */
    0xfc, 0x21, 0x8, 0x42, 0x10, 0x84, 0x0,

    /* U+250D "┍" */
    0xff, 0xff, 0x8, 0x42, 0x10, 0x84, 0x20,

    /* U+250E "┎" */
    0xff, 0x8e, 0x38, 0xe3, 0x8e, 0x38, 0xe3, 0x80,

    /* U+250F "┏" */
    0xff, 0xff, 0xf8, 0xe3, 0x8e, 0x38, 0xe3, 0x8e,
    0x0,

    /* U+2510 "┐" */
    0xf8, 0x42, 0x10, 0x84, 0x21, 0x8, 0x40,

    /* U+2511 "┑" */
    0xff, 0xfe, 0x10, 0x84, 0x21, 0x8, 0x42,

    /* U+2512 "┒" */
    0xfc, 0x71, 0xc7, 0x1c, 0x71, 0xc7, 0x1c, 0x70,

    /* U+2513 "┓" */
    0xff, 0xff, 0xc7, 0x1c, 0x71, 0xc7, 0x1c, 0x71,
    0xc0,

    /* U+2514 "└" */
    0x84, 0x21, 0x8, 0x42, 0x10, 0xf8,

    /* U+2515 "┕" */
    0x84, 0x21, 0x8, 0x42, 0x10, 0xff, 0xfe,

    /* U+2516 "┖" */
    0xe3, 0x8e, 0x38, 0xe3, 0x8e, 0x38, 0xfc,

    /* U+2517 "┗" */
    0xe3, 0x8e, 0x38, 0xe3, 0x8e, 0x38, 0xff, 0xff,
    0xc0,

    /* U+2518 "┘" */
    0x8, 0x42, 0x10, 0x84, 0x21, 0xf8,

    /* U+2519 "┙" */
    0x8, 0x42, 0x10, 0x84, 0x21, 0xff, 0xfe,

    /* U+251A "┚" */
    0x1c, 0x71, 0xc7, 0x1c, 0x71, 0xc7, 0xfc,

    /* U+251B "┛" */
    0x1c, 0x71, 0xc7, 0x1c, 0x71, 0xc7, 0xff, 0xff,
    0xc0,

    /* U+251C "├" */
    0x84, 0x21, 0x8, 0x42, 0x10, 0xff, 0xe1, 0x8,
    0x42, 0x10, 0x84, 0x0,

    /* U+251D "┝" */
    0x84, 0x21, 0x8, 0x42, 0x1f, 0xff, 0xe1, 0x8,
    0x42, 0x10, 0x84, 0x0,

    /* U+251E "┞" */
    0xe3, 0x8e, 0x38, 0xe3, 0x8e, 0x38, 0xe3, 0xf4,
    0x10, 0x41, 0x4, 0x10, 0x41, 0x4, 0x0,

    /* U+251F "┟" */
    0x41, 0x4, 0x10, 0x41, 0x4, 0x10, 0xff, 0x8e,
    0x38, 0xe3, 0x8e, 0x38, 0xe3, 0x80,

    /* U+2520 "┠" */
    0xe3, 0x8e, 0x38, 0xe3, 0x8e, 0x38, 0xff, 0xfe,
    0x38, 0xe3, 0x8e, 0x38, 0xe3, 0x80,

    /* U+2521 "┡" */
    0xe3, 0x8e, 0x38, 0xe3, 0x8e, 0x38, 0xff, 0xff,
    0xd0, 0x41, 0x4, 0x10, 0x41, 0x4, 0x0,

    /* U+2522 "┢" */
    0x41, 0x4, 0x10, 0x41, 0x4, 0x10, 0xff, 0xff,
    0xf8, 0xe3, 0x8e, 0x38, 0xe3, 0x8e, 0x0,

    /* U+2523 "┣" */
    0xe3, 0x8e, 0x38, 0xe3, 0x8e, 0x3f, 0xff, 0xfe,
    0x38, 0xe3, 0x8e, 0x38, 0xe3, 0x80,

    /* U+2524 "┤" */
    0x8, 0x42, 0x10, 0x84, 0x21, 0xff, 0xc2, 0x10,
    0x84, 0x21, 0x8, 0x40,

    /* U+2525 "┥" */
    0x8, 0x42, 0x10, 0x84, 0x3f, 0xff, 0xc2, 0x10,
    0x84, 0x21, 0x8, 0x40,

    /* U+2526 "┦" */
    0x1c, 0x71, 0xc7, 0x1c, 0x71, 0xc7, 0xfc, 0x20,
    0x82, 0x8, 0x20, 0x82, 0x8, 0x20,

    /* U+2527 "┧" */
    0x8, 0x20, 0x82, 0x8, 0x20, 0x82, 0xfc, 0x71,
    0xc7, 0x1c, 0x71, 0xc7, 0x1c, 0x70,

    /* U+2528 "┨" */
    0x1c, 0x71, 0xc7, 0x1c, 0x71, 0xc7, 0xff, 0xf1,
    0xc7, 0x1c, 0x71, 0xc7, 0x1c, 0x70,

    /* U+2529 "┩" */
    0x1c, 0x71, 0xc7, 0x1c, 0x71, 0xc7, 0xff, 0xff,
    0xc2, 0x8, 0x20, 0x82, 0x8, 0x20, 0x80,

    /* U+252A "┪" */
    0x8, 0x20, 0x82, 0x8, 0x20, 0x82, 0xff, 0xff,
    0xc7, 0x1c, 0x71, 0xc7, 0x1c, 0x71, 0xc0,

    /* U+252B "┫" */
    0x1c, 0x71, 0xc7, 0x1c, 0x71, 0xff, 0xff, 0xf1,
    0xc7, 0x1c, 0x71, 0xc7, 0x1c, 0x70,

    /* U+252C "┬" */
    0xff, 0x84, 0x2, 0x1, 0x0, 0x80, 0x40, 0x20,
    0x10, 0x8, 0x4, 0x0,

    /* U+252D "┭" */
    0xf8, 0x7f, 0xfe, 0x1, 0x0, 0x80, 0x40, 0x20,
    0x10, 0x8, 0x4, 0x2, 0x0,

    /* U+252E "┮" */
    0xf, 0xff, 0xc3, 0xe1, 0x0, 0x80, 0x40, 0x20,
    0x10, 0x8, 0x4, 0x2, 0x0,

    /* U+252F "┯" */
    0xff, 0xff, 0xff, 0xe1, 0x0, 0x80, 0x40, 0x20,
    0x10, 0x8, 0x4, 0x2, 0x0,

    /* U+2530 "┰" */
    0xff, 0x8e, 0x7, 0x3, 0x81, 0xc0, 0xe0, 0x70,
    0x38, 0x1c, 0xe, 0x0,

    /* U+2531 "┱" */
    0xfc, 0x7f, 0xff, 0x3, 0x81, 0xc0, 0xe0, 0x70,
    0x38, 0x1c, 0xe, 0x7, 0x0,

    /* U+2532 "┲" */
    0x1f, 0xff, 0xc7, 0xe3, 0x81, 0xc0, 0xe0, 0x70,
    0x38, 0x1c, 0xe, 0x7, 0x0,

    /* U+2533 "┳" */
    0xff, 0xff, 0xff, 0xe3, 0x81, 0xc0, 0xe0, 0x70,
    0x38, 0x1c, 0xe, 0x7, 0x0,

    /* U+2534 "┴" */
    0x8, 0x4, 0x2, 0x1, 0x0, 0x80, 0x40, 0x20,
    0x10, 0xff, 0x80,

    /* U+2535 "┵" */
    0x8, 0x4, 0x2, 0x1, 0x0, 0x80, 0x40, 0x20,
    0x10, 0xf8, 0x7f, 0xfe, 0x0,

    /* U+2536 "┶" */
    0x8, 0x4, 0x2, 0x1, 0x0, 0x80, 0x40, 0x20,
    0x10, 0xf, 0xff, 0xc3, 0xe0,

    /* U+2537 "┷" */
    0x8, 0x4, 0x2, 0x1, 0x0, 0x80, 0x40, 0x20,
    0x10, 0xff, 0xff, 0xff, 0xe0,

    /* U+2538 "┸" */
    0x1c, 0xe, 0x7, 0x3, 0x81, 0xc0, 0xe0, 0x70,
    0x38, 0xff, 0x80,

    /* U+2539 "┹" */
    0x1c, 0xe, 0x7, 0x3, 0x81, 0xc0, 0xe0, 0x70,
    0x38, 0xfc, 0x7f, 0xff, 0x0,

    /* U+253A "┺" */
    0x1c, 0xe, 0x7, 0x3, 0x81, 0xc0, 0xe0, 0x70,
    0x38, 0x1f, 0xff, 0xc7, 0xe0,

    /* U+253B "┻" */
    0x1c, 0xe, 0x7, 0x3, 0x81, 0xc0, 0xe0, 0x70,
    0x38, 0xff, 0xff, 0xff, 0xe0,

    /* U+253C "┼" */
    0x8, 0x4, 0x2, 0x1, 0x0, 0x80, 0x40, 0x20,
    0x10, 0xff, 0xff, 0xc2, 0x1, 0x0, 0x80, 0x40,
    0x20, 0x10, 0x8, 0x4, 0x0,

    /* U+253D "┽" */
    0x8, 0x4, 0x2, 0x1, 0x0, 0x80, 0x40, 0x21,
    0xf0, 0xff, 0xff, 0xc2, 0x1, 0x0, 0x80, 0x40,
    0x20, 0x10, 0x8, 0x4, 0x0,

    /* U+253E "┾" */
    0x8, 0x4, 0x2, 0x1, 0x0, 0x80, 0x40, 0x20,
    0x1f, 0xff, 0xff, 0xc2, 0x1, 0x0, 0x80, 0x40,
    0x20, 0x10, 0x8, 0x4, 0x0,

    /* U+253F "┿" */
    0x8, 0x4, 0x2, 0x1, 0x0, 0x80, 0x40, 0x21,
    0xff, 0xff, 0xff, 0xc2, 0x1, 0x0, 0x80, 0x40,
    0x20, 0x10, 0x8, 0x4, 0x0,

    /* U+2540 "╀" */
    0x1c, 0xe, 0x7, 0x3, 0x81, 0xc0, 0xe0, 0x70,
    0x38, 0xff, 0x84, 0x2, 0x1, 0x0, 0x80, 0x40,
    0x20, 0x10, 0x8, 0x4, 0x0,

    /* U+2541 "╁" */
    0x8, 0x4, 0x2, 0x1, 0x0, 0x80, 0x40, 0x20,
    0x10, 0xff, 0x8e, 0x7, 0x3, 0x81, 0xc0, 0xe0,
    0x70, 0x38, 0x1c, 0xe, 0x0,

    /* U+2542 "╂" */
    0x1c, 0xe, 0x7, 0x3, 0x81, 0xc0, 0xe0, 0x70,
    0x38, 0xff, 0xff, 0xc7, 0x3, 0x81, 0xc0, 0xe0,
    0x70, 0x38, 0x1c, 0xe, 0x0,

    /* U+2543 "╃" */
    0x1c, 0xe, 0x7, 0x3, 0x81, 0xc0, 0xe0, 0x70,
    0x38, 0xfc, 0x7f, 0xff, 0x1, 0x0, 0x80, 0x40,
    0x20, 0x10, 0x8, 0x4, 0x2, 0x0,

    /* U+2544 "╄" */
    0x1c, 0xe, 0x7, 0x3, 0x81, 0xc0, 0xe0, 0x70,
    0x38, 0x1f, 0xff, 0xc7, 0xe1, 0x0, 0x80, 0x40,
    0x20, 0x10, 0x8, 0x4, 0x2, 0x0,

    /* U+2545 "╅" */
    0x8, 0x4, 0x2, 0x1, 0x0, 0x80, 0x40, 0x20,
    0x10, 0xfc, 0x7f, 0xff, 0x3, 0x81, 0xc0, 0xe0,
    0x70, 0x38, 0x1c, 0xe, 0x7, 0x0,

    /* U+2546 "╆" */
    0x8, 0x4, 0x2, 0x1, 0x0, 0x80, 0x40, 0x20,
    0x10, 0x1f, 0xff, 0xc7, 0xe3, 0x81, 0xc0, 0xe0,
    0x70, 0x38, 0x1c, 0xe, 0x7, 0x0,

    /* U+2547 "╇" */
    0x1c, 0xe, 0x7, 0x3, 0x81, 0xc0, 0xe0, 0x70,
    0x38, 0xff, 0xff, 0xff, 0xe1, 0x0, 0x80, 0x40,
    0x20, 0x10, 0x8, 0x4, 0x2, 0x0,

    /* U+2548 "╈" */
    0x8, 0x4, 0x2, 0x1, 0x0, 0x80, 0x40, 0x20,
    0x10, 0xff, 0xff, 0xff, 0xe3, 0x81, 0xc0, 0xe0,
    0x70, 0x38, 0x1c, 0xe, 0x7, 0x0,

    /* U+2549 "╉" */
    0x1c, 0xe, 0x7, 0x3, 0x81, 0xc0, 0xe0, 0x71,
    0xf8, 0xff, 0xff, 0xc7, 0x3, 0x81, 0xc0, 0xe0,
    0x70, 0x38, 0x1c, 0xe, 0x0,

    /* U+254A "╊" */
    0x1c, 0xe, 0x7, 0x3, 0x81, 0xc0, 0xe0, 0x70,
    0x3f, 0xff, 0xff, 0xc7, 0x3, 0x81, 0xc0, 0xe0,
    0x70, 0x38, 0x1c, 0xe, 0x0,

    /* U+254B "╋" */
    0x1c, 0xe, 0x7, 0x3, 0x81, 0xc0, 0xe0, 0x71,
    0xff, 0xff, 0xff, 0xc7, 0x3, 0x81, 0xc0, 0xe0,
    0x70, 0x38, 0x1c, 0xe, 0x0,

    /* U+254C "╌" */
    0xcc,

    /* U+254D "╍" */
    0xcf, 0x3c, 0xc0,

    /* U+254E "╎" */
    0xfc, 0xfc,

    /* U+254F "╏" */
    0xff, 0xff, 0xc0, 0xff, 0xff, 0xc0,

    /* U+2550 "═" */
    0xff, 0x80, 0x0, 0x1f, 0xf0,

    /* U+2551 "║" */
    0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
    0x99,

    /* U+2552 "╒" */
    0xfc, 0x21, 0xf8, 0x42, 0x10, 0x84, 0x20,

    /* U+2553 "╓" */
    0xfe, 0x49, 0x24, 0x92, 0x49, 0x24, 0x92, 0x40,

    /* U+2554 "╔" */
    0xfe, 0x8, 0x27, 0x92, 0x49, 0x24, 0x92, 0x49,
    0x0,

    /* U+2555 "╕" */
    0xf8, 0x43, 0xf0, 0x84, 0x21, 0x8, 0x42,

    /* U+2556 "╖" */
    0xfc, 0x92, 0x49, 0x24, 0x92, 0x49, 0x24, 0x90,

    /* U+2557 "╗" */
    0xfc, 0x10, 0x79, 0x24, 0x92, 0x49, 0x24, 0x92,
    0x40,

    /* U+2558 "╘" */
    0x84, 0x21, 0x8, 0x42, 0x1f, 0x84, 0x3e,

    /* U+2559 "╙" */
    0x92, 0x49, 0x24, 0x92, 0x49, 0x24, 0xfc,

    /* U+255A "╚" */
    0x92, 0x49, 0x24, 0x92, 0x49, 0x27, 0x82, 0xf,
    0xc0,

    /* U+255B "╛" */
    0x8, 0x42, 0x10, 0x84, 0x3f, 0x8, 0x7e,

    /* U+255C "╜" */
    0x24, 0x92, 0x49, 0x24, 0x92, 0x49, 0xfc,

    /* U+255D "╝" */
    0x24, 0x92, 0x49, 0x24, 0x92, 0x79, 0x4, 0x1f,
    0xc0,

    /* U+255E "╞" */
    0x84, 0x21, 0x8, 0x42, 0x1f, 0x84, 0x3f, 0x8,
    0x42, 0x10, 0x84, 0x0,

    /* U+255F "╟" */
    0x92, 0x49, 0x24, 0x92, 0x49, 0x24, 0x9e, 0x79,
    0x24, 0x92, 0x49, 0x24, 0x92, 0x40,

    /* U+2560 "╠" */
    0x92, 0x49, 0x24, 0x92, 0x49, 0x27, 0x82, 0x9,
    0xe4, 0x92, 0x49, 0x24, 0x92, 0x40,

    /* U+2561 "╡" */
    0x8, 0x42, 0x10, 0x84, 0x3f, 0x8, 0x7e, 0x10,
    0x84, 0x21, 0x8, 0x40,

    /* U+2562 "╢" */
    0x24, 0x92, 0x49, 0x24, 0x92, 0x49, 0xe7, 0x92,
    0x49, 0x24, 0x92, 0x49, 0x24, 0x90,

    /* U+2563 "╣" */
    0x24, 0x92, 0x49, 0x24, 0x92, 0x79, 0x4, 0x1e,
    0x49, 0x24, 0x92, 0x49, 0x24, 0x90,

    /* U+2564 "╤" */
    0xff, 0x80, 0x0, 0x1f, 0xf0, 0x80, 0x40, 0x20,
    0x10, 0x8, 0x4, 0x2, 0x0,

    /* U+2565 "╥" */
    0xff, 0x9b, 0xd, 0x86, 0xc3, 0x61, 0xb0, 0xd8,
    0x6c, 0x36, 0x1b, 0x0,

    /* U+2566 "╦" */
    0xff, 0x80, 0x0, 0x1e, 0xf3, 0x61, 0xb0, 0xd8,
    0x6c, 0x36, 0x1b, 0xd, 0x80,

    /* U+2567 "╧" */
    0x8, 0x4, 0x2, 0x1, 0x0, 0x80, 0x40, 0x21,
    0xff, 0x0, 0x0, 0x3f, 0xe0,

    /* U+2568 "╨" */
    0x36, 0x1b, 0xd, 0x86, 0xc3, 0x61, 0xb0, 0xd8,
    0x6c, 0xff, 0x80,

    /* U+2569 "╩" */
    0x36, 0x1b, 0xd, 0x86, 0xc3, 0x61, 0xb0, 0xd9,
    0xef, 0x0, 0x0, 0x3f, 0xe0,

    /* U+256A "╪" */
    0x8, 0x4, 0x2, 0x1, 0x0, 0x80, 0x40, 0x21,
    0xff, 0x8, 0x4, 0x3f, 0xe1, 0x0, 0x80, 0x40,
    0x20, 0x10, 0x8, 0x4, 0x0,

    /* U+256B "╫" */
    0x36, 0x1b, 0xd, 0x86, 0xc3, 0x61, 0xb0, 0xd8,
    0x6c, 0xff, 0xff, 0xcd, 0x86, 0xc3, 0x61, 0xb0,
    0xd8, 0x6c, 0x36, 0x1b, 0x0,

    /* U+256C "╬" */
    0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0xe7,
    0x0, 0x0, 0xe7, 0x24, 0x24, 0x24, 0x24, 0x24,
    0x24, 0x24,

    /* U+256D "╭" */
    0x3a, 0x21, 0x8, 0x42, 0x10, 0x84, 0x0,

    /* U+256E "╮" */
    0xe0, 0x82, 0x10, 0x84, 0x21, 0x8, 0x40,

    /* U+256F "╯" */
    0x8, 0x42, 0x10, 0x84, 0x22, 0xe0,

    /* U+2570 "╰" */
    0x84, 0x21, 0x8, 0x42, 0x8, 0x38,

    /* U+2571 "╱" */
    0x1, 0x80, 0x80, 0x80, 0xc0, 0x40, 0x60, 0x20,
    0x30, 0x10, 0x18, 0x18, 0x8, 0xc, 0x4, 0x0,

    /* U+2572 "╲" */
    0x80, 0x60, 0x10, 0x4, 0x3, 0x0, 0x80, 0x60,
    0x10, 0xc, 0x2, 0x1, 0x80, 0x40, 0x10, 0xc,

    /* U+2573 "╳" */
    0x80, 0xa0, 0x98, 0xc4, 0x43, 0x60, 0xa0, 0x70,
    0x38, 0x14, 0x1b, 0x8, 0x8c, 0x64, 0x14, 0x4,

    /* U+2574 "╴" */
    0xf8,

    /* U+2575 "╵" */
    0xff, 0xc0,

    /* U+2576 "╶" */
    0xf8,

    /* U+2577 "╷" */
    0xff, 0xc0,

    /* U+2578 "╸" */
    0xff, 0xfe,

    /* U+2579 "╹" */
    0xff, 0xff, 0xff, 0xfc,

    /* U+257A "╺" */
    0xff, 0xfe,

    /* U+257B "╻" */
    0xff, 0xff, 0xff, 0xfc,

    /* U+257C "╼" */
    0xf, 0xff, 0xc3, 0xe0,

    /* U+257D "╽" */
    0x49, 0x24, 0x92, 0xff, 0xff, 0xff, 0xfc,

    /* U+257E "╾" */
    0xf8, 0x7f, 0xfe, 0x0,

    /* U+257F "╿" */
    0xff, 0xff, 0xff, 0xfd, 0x24, 0x92, 0x49, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 134, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 134, .box_w = 2, .box_h = 10, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 134, .box_w = 4, .box_h = 4, .ofs_x = 2, .ofs_y = 6},
    {.bitmap_index = 6, .adv_w = 134, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 16, .adv_w = 134, .box_w = 6, .box_h = 15, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 28, .adv_w = 134, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 38, .adv_w = 134, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 47, .adv_w = 134, .box_w = 1, .box_h = 4, .ofs_x = 3, .ofs_y = 6},
    {.bitmap_index = 48, .adv_w = 134, .box_w = 4, .box_h = 14, .ofs_x = 3, .ofs_y = -2},
    {.bitmap_index = 55, .adv_w = 134, .box_w = 4, .box_h = 14, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 62, .adv_w = 134, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 69, .adv_w = 134, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 75, .adv_w = 134, .box_w = 3, .box_h = 5, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 77, .adv_w = 134, .box_w = 4, .box_h = 1, .ofs_x = 2, .ofs_y = 4},
    {.bitmap_index = 78, .adv_w = 134, .box_w = 2, .box_h = 2, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 79, .adv_w = 134, .box_w = 6, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 90, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 98, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 106, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 114, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 122, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 130, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 138, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 146, .adv_w = 134, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 155, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 163, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 171, .adv_w = 134, .box_w = 2, .box_h = 8, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 173, .adv_w = 134, .box_w = 3, .box_h = 11, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 178, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 184, .adv_w = 134, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 188, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 194, .adv_w = 134, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 201, .adv_w = 134, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 213, .adv_w = 134, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 222, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 230, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 238, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 246, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 254, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 262, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 270, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 278, .adv_w = 134, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 285, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 293, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 301, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 309, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 317, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 325, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 333, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 341, .adv_w = 134, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 351, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 359, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 367, .adv_w = 134, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 376, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 384, .adv_w = 134, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 393, .adv_w = 134, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 403, .adv_w = 134, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 412, .adv_w = 134, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 421, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 429, .adv_w = 134, .box_w = 3, .box_h = 13, .ofs_x = 3, .ofs_y = -1},
    {.bitmap_index = 434, .adv_w = 134, .box_w = 6, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 445, .adv_w = 134, .box_w = 3, .box_h = 13, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 450, .adv_w = 134, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 455, .adv_w = 134, .box_w = 7, .box_h = 1, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 456, .adv_w = 134, .box_w = 2, .box_h = 2, .ofs_x = 3, .ofs_y = 9},
    {.bitmap_index = 457, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 463, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 471, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 477, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 485, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 491, .adv_w = 134, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 500, .adv_w = 134, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 509, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 517, .adv_w = 134, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 527, .adv_w = 134, .box_w = 5, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 536, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 544, .adv_w = 134, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 554, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 560, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 566, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 572, .adv_w = 134, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 581, .adv_w = 134, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 590, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 596, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 602, .adv_w = 134, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 611, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 617, .adv_w = 134, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 624, .adv_w = 134, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 632, .adv_w = 134, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 639, .adv_w = 134, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 649, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 655, .adv_w = 134, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 665, .adv_w = 134, .box_w = 1, .box_h = 14, .ofs_x = 4, .ofs_y = -2},
    {.bitmap_index = 667, .adv_w = 134, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 677, .adv_w = 134, .box_w = 6, .box_h = 3, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 680, .adv_w = 134, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 687, .adv_w = 134, .box_w = 4, .box_h = 4, .ofs_x = 2, .ofs_y = 6},
    {.bitmap_index = 689, .adv_w = 134, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 696, .adv_w = 134, .box_w = 6, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 707, .adv_w = 134, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 716, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 724, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 732, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 740, .adv_w = 134, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 752, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 760, .adv_w = 134, .box_w = 9, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 772, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 780, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 788, .adv_w = 134, .box_w = 6, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 799, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 807, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 815, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 823, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 831, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 839, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 847, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 855, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 863, .adv_w = 134, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 872, .adv_w = 134, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 881, .adv_w = 134, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 891, .adv_w = 134, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 900, .adv_w = 134, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 911, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 919, .adv_w = 134, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 928, .adv_w = 134, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 939, .adv_w = 134, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 949, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 957, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 965, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 973, .adv_w = 134, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 982, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 990, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 996, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1004, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1010, .adv_w = 134, .box_w = 5, .box_h = 8, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1015, .adv_w = 134, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1024, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1030, .adv_w = 134, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1037, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1043, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1049, .adv_w = 134, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1058, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1064, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1070, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1076, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1082, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1088, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1094, .adv_w = 134, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1103, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1109, .adv_w = 134, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1116, .adv_w = 134, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1126, .adv_w = 134, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1138, .adv_w = 134, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1145, .adv_w = 134, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1154, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1160, .adv_w = 134, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1167, .adv_w = 134, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1176, .adv_w = 134, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1184, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1190, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1196, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1202, .adv_w = 134, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1209, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1215, .adv_w = 134, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1224, .adv_w = 134, .box_w = 8, .box_h = 1, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 1225, .adv_w = 134, .box_w = 7, .box_h = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1227, .adv_w = 134, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1237, .adv_w = 134, .box_w = 9, .box_h = 1, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 1239, .adv_w = 134, .box_w = 9, .box_h = 3, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 1243, .adv_w = 134, .box_w = 1, .box_h = 18, .ofs_x = 4, .ofs_y = -4},
    {.bitmap_index = 1246, .adv_w = 134, .box_w = 3, .box_h = 18, .ofs_x = 3, .ofs_y = -4},
    {.bitmap_index = 1253, .adv_w = 134, .box_w = 8, .box_h = 1, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 1254, .adv_w = 134, .box_w = 8, .box_h = 3, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 1257, .adv_w = 134, .box_w = 1, .box_h = 16, .ofs_x = 4, .ofs_y = -3},
    {.bitmap_index = 1259, .adv_w = 134, .box_w = 3, .box_h = 17, .ofs_x = 3, .ofs_y = -3},
    {.bitmap_index = 1266, .adv_w = 134, .box_w = 7, .box_h = 1, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 1267, .adv_w = 134, .box_w = 7, .box_h = 3, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 1270, .adv_w = 134, .box_w = 1, .box_h = 16, .ofs_x = 4, .ofs_y = -3},
    {.bitmap_index = 1272, .adv_w = 134, .box_w = 3, .box_h = 16, .ofs_x = 3, .ofs_y = -3},
    {.bitmap_index = 1278, .adv_w = 134, .box_w = 5, .box_h = 10, .ofs_x = 4, .ofs_y = -4},
    {.bitmap_index = 1285, .adv_w = 134, .box_w = 5, .box_h = 11, .ofs_x = 4, .ofs_y = -4},
    {.bitmap_index = 1292, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 3, .ofs_y = -4},
    {.bitmap_index = 1300, .adv_w = 134, .box_w = 6, .box_h = 11, .ofs_x = 3, .ofs_y = -4},
    {.bitmap_index = 1309, .adv_w = 134, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1316, .adv_w = 134, .box_w = 5, .box_h = 11, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1323, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1331, .adv_w = 134, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1340, .adv_w = 134, .box_w = 5, .box_h = 9, .ofs_x = 4, .ofs_y = 5},
    {.bitmap_index = 1346, .adv_w = 134, .box_w = 5, .box_h = 11, .ofs_x = 4, .ofs_y = 4},
    {.bitmap_index = 1353, .adv_w = 134, .box_w = 6, .box_h = 9, .ofs_x = 3, .ofs_y = 5},
    {.bitmap_index = 1360, .adv_w = 134, .box_w = 6, .box_h = 11, .ofs_x = 3, .ofs_y = 4},
    {.bitmap_index = 1369, .adv_w = 134, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 1375, .adv_w = 134, .box_w = 5, .box_h = 11, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 1382, .adv_w = 134, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 1389, .adv_w = 134, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 1398, .adv_w = 134, .box_w = 5, .box_h = 18, .ofs_x = 4, .ofs_y = -4},
    {.bitmap_index = 1410, .adv_w = 134, .box_w = 5, .box_h = 18, .ofs_x = 4, .ofs_y = -4},
    {.bitmap_index = 1422, .adv_w = 134, .box_w = 6, .box_h = 19, .ofs_x = 3, .ofs_y = -5},
    {.bitmap_index = 1437, .adv_w = 134, .box_w = 6, .box_h = 18, .ofs_x = 3, .ofs_y = -4},
    {.bitmap_index = 1451, .adv_w = 134, .box_w = 6, .box_h = 18, .ofs_x = 3, .ofs_y = -4},
    {.bitmap_index = 1465, .adv_w = 134, .box_w = 6, .box_h = 19, .ofs_x = 3, .ofs_y = -4},
    {.bitmap_index = 1480, .adv_w = 134, .box_w = 6, .box_h = 19, .ofs_x = 3, .ofs_y = -4},
    {.bitmap_index = 1495, .adv_w = 134, .box_w = 6, .box_h = 18, .ofs_x = 3, .ofs_y = -4},
    {.bitmap_index = 1509, .adv_w = 134, .box_w = 5, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1521, .adv_w = 134, .box_w = 5, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1533, .adv_w = 134, .box_w = 6, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1547, .adv_w = 134, .box_w = 6, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1561, .adv_w = 134, .box_w = 6, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1575, .adv_w = 134, .box_w = 6, .box_h = 19, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1590, .adv_w = 134, .box_w = 6, .box_h = 19, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1605, .adv_w = 134, .box_w = 6, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1619, .adv_w = 134, .box_w = 9, .box_h = 10, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1631, .adv_w = 134, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1644, .adv_w = 134, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1657, .adv_w = 134, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1670, .adv_w = 134, .box_w = 9, .box_h = 10, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1682, .adv_w = 134, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1695, .adv_w = 134, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1708, .adv_w = 134, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1721, .adv_w = 134, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 1732, .adv_w = 134, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 1745, .adv_w = 134, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 1758, .adv_w = 134, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 1771, .adv_w = 134, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 1782, .adv_w = 134, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 1795, .adv_w = 134, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 1808, .adv_w = 134, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 1821, .adv_w = 134, .box_w = 9, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1842, .adv_w = 134, .box_w = 9, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1863, .adv_w = 134, .box_w = 9, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1884, .adv_w = 134, .box_w = 9, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1905, .adv_w = 134, .box_w = 9, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1926, .adv_w = 134, .box_w = 9, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1947, .adv_w = 134, .box_w = 9, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1968, .adv_w = 134, .box_w = 9, .box_h = 19, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1990, .adv_w = 134, .box_w = 9, .box_h = 19, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2012, .adv_w = 134, .box_w = 9, .box_h = 19, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2034, .adv_w = 134, .box_w = 9, .box_h = 19, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2056, .adv_w = 134, .box_w = 9, .box_h = 19, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2078, .adv_w = 134, .box_w = 9, .box_h = 19, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2100, .adv_w = 134, .box_w = 9, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2121, .adv_w = 134, .box_w = 9, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2142, .adv_w = 134, .box_w = 9, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2163, .adv_w = 134, .box_w = 6, .box_h = 1, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 2164, .adv_w = 134, .box_w = 6, .box_h = 3, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 2167, .adv_w = 134, .box_w = 1, .box_h = 14, .ofs_x = 4, .ofs_y = -2},
    {.bitmap_index = 2169, .adv_w = 134, .box_w = 3, .box_h = 14, .ofs_x = 3, .ofs_y = -2},
    {.bitmap_index = 2175, .adv_w = 134, .box_w = 9, .box_h = 4, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 2180, .adv_w = 134, .box_w = 4, .box_h = 18, .ofs_x = 2, .ofs_y = -4},
    {.bitmap_index = 2189, .adv_w = 134, .box_w = 5, .box_h = 11, .ofs_x = 4, .ofs_y = -4},
    {.bitmap_index = 2196, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 2, .ofs_y = -4},
    {.bitmap_index = 2204, .adv_w = 134, .box_w = 6, .box_h = 11, .ofs_x = 2, .ofs_y = -4},
    {.bitmap_index = 2213, .adv_w = 134, .box_w = 5, .box_h = 11, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2220, .adv_w = 134, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2228, .adv_w = 134, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2237, .adv_w = 134, .box_w = 5, .box_h = 11, .ofs_x = 4, .ofs_y = 3},
    {.bitmap_index = 2244, .adv_w = 134, .box_w = 6, .box_h = 9, .ofs_x = 2, .ofs_y = 5},
    {.bitmap_index = 2251, .adv_w = 134, .box_w = 6, .box_h = 11, .ofs_x = 2, .ofs_y = 3},
    {.bitmap_index = 2260, .adv_w = 134, .box_w = 5, .box_h = 11, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 2267, .adv_w = 134, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 2274, .adv_w = 134, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 2283, .adv_w = 134, .box_w = 5, .box_h = 18, .ofs_x = 4, .ofs_y = -4},
    {.bitmap_index = 2295, .adv_w = 134, .box_w = 6, .box_h = 18, .ofs_x = 2, .ofs_y = -4},
    {.bitmap_index = 2309, .adv_w = 134, .box_w = 6, .box_h = 18, .ofs_x = 2, .ofs_y = -4},
    {.bitmap_index = 2323, .adv_w = 134, .box_w = 5, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2335, .adv_w = 134, .box_w = 6, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2349, .adv_w = 134, .box_w = 6, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2363, .adv_w = 134, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2376, .adv_w = 134, .box_w = 9, .box_h = 10, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2388, .adv_w = 134, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2401, .adv_w = 134, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 2414, .adv_w = 134, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 2425, .adv_w = 134, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 2438, .adv_w = 134, .box_w = 9, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2459, .adv_w = 134, .box_w = 9, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2480, .adv_w = 134, .box_w = 8, .box_h = 18, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2498, .adv_w = 134, .box_w = 5, .box_h = 10, .ofs_x = 4, .ofs_y = -4},
    {.bitmap_index = 2505, .adv_w = 134, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 2512, .adv_w = 134, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 2518, .adv_w = 134, .box_w = 5, .box_h = 9, .ofs_x = 4, .ofs_y = 5},
    {.bitmap_index = 2524, .adv_w = 134, .box_w = 9, .box_h = 14, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 2540, .adv_w = 134, .box_w = 9, .box_h = 14, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 2556, .adv_w = 134, .box_w = 9, .box_h = 14, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 2572, .adv_w = 134, .box_w = 5, .box_h = 1, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 2573, .adv_w = 134, .box_w = 1, .box_h = 10, .ofs_x = 4, .ofs_y = 4},
    {.bitmap_index = 2575, .adv_w = 134, .box_w = 5, .box_h = 1, .ofs_x = 4, .ofs_y = 5},
    {.bitmap_index = 2576, .adv_w = 134, .box_w = 1, .box_h = 10, .ofs_x = 4, .ofs_y = -4},
    {.bitmap_index = 2578, .adv_w = 134, .box_w = 5, .box_h = 3, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 2580, .adv_w = 134, .box_w = 3, .box_h = 10, .ofs_x = 3, .ofs_y = 4},
    {.bitmap_index = 2584, .adv_w = 134, .box_w = 5, .box_h = 3, .ofs_x = 4, .ofs_y = 4},
    {.bitmap_index = 2586, .adv_w = 134, .box_w = 3, .box_h = 10, .ofs_x = 3, .ofs_y = -4},
    {.bitmap_index = 2590, .adv_w = 134, .box_w = 9, .box_h = 3, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 2594, .adv_w = 134, .box_w = 3, .box_h = 18, .ofs_x = 3, .ofs_y = -4},
    {.bitmap_index = 2601, .adv_w = 134, .box_w = 9, .box_h = 3, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 2605, .adv_w = 134, .box_w = 3, .box_h = 19, .ofs_x = 3, .ofs_y = -5}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_1[] = {
    0x0, 0x5, 0x10, 0x356
};

static const uint16_t unicode_list_3[] = {
    0x0, 0x1bc3, 0x1bd5, 0x1cc5
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 171, .range_length = 855, .glyph_id_start = 96,
        .unicode_list = unicode_list_1, .glyph_id_ofs_list = NULL, .list_length = 4, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    },
    {
        .range_start = 1040, .range_length = 64, .glyph_id_start = 100,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 1105, .range_length = 7366, .glyph_id_start = 164,
        .unicode_list = unicode_list_3, .glyph_id_ofs_list = NULL, .list_length = 4, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    },
    {
        .range_start = 9472, .range_length = 128, .glyph_id_start = 168,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 5,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif

};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t term_font_20 = {
#else
lv_font_t term_font_20 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 20,          /*The maximum line height required by the font*/
    .base_line = 5,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .static_bitmap = 0,
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if TERM_FONT_20*/
