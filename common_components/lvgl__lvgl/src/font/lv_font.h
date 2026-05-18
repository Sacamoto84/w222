/**
 * @file lv_font.h
 *
 */

#ifndef LV_FONT_H
#define LV_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "../misc/lv_types.h"

#include "lv_symbol_def.h"
#include "../draw/lv_draw_buf.h"
#include "../misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*------------------
 * Общие типы
 *-----------------*/

/** Формат шрифта.*/
typedef enum {
    LV_FONT_GLYPH_FORMAT_NONE   = 0, /**< Maybe not visible*/

    /**< Legacy simple formats*/
    LV_FONT_GLYPH_FORMAT_A1     = 0x01, /**< 1 bit per pixel*/
    LV_FONT_GLYPH_FORMAT_A2     = 0x02, /**< 2 bit per pixel*/
    LV_FONT_GLYPH_FORMAT_A3     = 0x03, /**< 3 bit per pixel*/
    LV_FONT_GLYPH_FORMAT_A4     = 0x04, /**< 4 bit per pixel*/
    LV_FONT_GLYPH_FORMAT_A8     = 0x08, /**< 8 bit per pixel*/

    LV_FONT_GLYPH_FORMAT_IMAGE  = 0x19, /**< Image format*/

    /**< Advanced formats*/
    LV_FONT_GLYPH_FORMAT_VECTOR = 0x1A, /**< Vectorial format*/
    LV_FONT_GLYPH_FORMAT_SVG    = 0x1B, /**< SVG format*/
    LV_FONT_GLYPH_FORMAT_CUSTOM = 0xFF, /**< Custom format*/
} lv_font_glyph_format_t;

/** Описывает свойства глифа.*/
typedef struct {
    const lv_font_t *
    resolved_font;  /**< Pointer to a font where the glyph was actually found after handling fallbacks*/
    uint16_t adv_w; /**< The glyph needs this space. Draw the next glyph after this width.*/
    uint16_t box_w; /**< Width of the glyph's bounding box*/
    uint16_t box_h; /**< Height of the glyph's bounding box*/
    int16_t ofs_x;  /**< x offset of the bounding box*/
    int16_t ofs_y;  /**< y offset of the bounding box*/
    uint16_t stride;/**< Bytes in each line. If 0 than there is no padding at the end of the line. */
    lv_font_glyph_format_t format;  /**< Font format of the glyph see lv_font_glyph_format_t */
    uint8_t is_placeholder: 1;      /**< Glyph is missing. But placeholder will still be displayed*/

    /** 0: Функция Get bitmap должна возвращать изображениеA8или ARGB8888.
      * 1: верните растровое изображение как оно есть (возможно, A1/2/4 или любые другие собственные форматы). */
    uint8_t req_raw_bitmap: 1;

    int32_t outline_stroke_width;   /**< used with freetype vector fonts - width of the letter border */

    union {
        uint32_t index;       /**< Glyph descriptor index*/
        const void * src;     /**< Pointer to the source data used by image fonts*/
    } gid;                    /**< The index of the glyph in the font file. Used by the font cache*/
    lv_cache_entry_t * entry; /**< The cache entry of the glyph draw data. Used by the font cache*/
} lv_font_glyph_dsc_t;

/** Растровые изображения могут быть увеличены на 3 для достижения субпиксельного рендеринга.*/
typedef enum {
    LV_FONT_SUBPX_NONE,
    LV_FONT_SUBPX_HOR,
    LV_FONT_SUBPX_VER,
    LV_FONT_SUBPX_BOTH,
} lv_font_subpx_t;

/** Отрегулируйте расстояние между буквами для определенных пар символов.*/
typedef enum {
    LV_FONT_KERNING_NORMAL,
    LV_FONT_KERNING_NONE,
} lv_font_kerning_t;

/** Опишите свойства шрифта.*/
struct _lv_font_t {
    /** Получить дескриптор глифа из шрифта*/
    bool (*get_glyph_dsc)(const lv_font_t *, lv_font_glyph_dsc_t *, uint32_t letter, uint32_t letter_next);

    /** Получить растровое изображение глифа из шрифта*/
    const void * (*get_glyph_bitmap)(lv_font_glyph_dsc_t *, lv_draw_buf_t *);

    /** Выпустить глиф*/
    void (*release_glyph)(const lv_font_t *, lv_font_glyph_dsc_t *);

    /*Указатель на шрифт в пакете шрифтов (должен иметь одинаковую высоту строки)*/
    int32_t line_height;         /**< The real line height where any text fits*/
    int32_t base_line;           /**< Base line measured from the bottom of the line_height*/
    uint8_t subpx   : 2;            /**< An element of `lv_font_subpx_t`*/
    uint8_t kerning : 1;            /**< An element of `lv_font_kerning_t`*/
    uint8_t static_bitmap : 1;      /**< The font will be used as static bitmap */

    int8_t underline_position;      /**< Distance between the top of the underline and base line (< 0 means below the base line)*/
    int8_t underline_thickness;     /**< Thickness of the underline*/

    const void * dsc;               /**< Store implementation specific or run_time data or caching here*/
    const lv_font_t * fallback;     /**< Fallback font for missing glyph. Resolved recursively */
    void * user_data;               /**< Custom user data for font.*/
};

struct _lv_font_class_t {
    lv_font_t * (*create_cb)(const lv_font_info_t * info, const void * src); /**< Font creation callback function*/
    void (*delete_cb)(lv_font_t * font);    /**< Font deletion callback function*/
    void * (*dup_src_cb)(const void * src); /**< Font source duplication callback function*/
    void (*free_src_cb)(void * src);        /**< Font source free callback function*/
};

struct _lv_font_info_t {
    const char * name;               /**< Font name, used to distinguish different font resources*/
    const lv_font_class_t * class_p; /**< Font backend implementation*/
    uint32_t size;                   /**< Font size in pixel*/
    uint32_t render_mode;            /**< Font rendering mode, see `lv_freetype_font_render_mode_t`*/
    uint32_t style;                  /**< Font style, see `lv_freetype_font_style_t`*/
    lv_font_kerning_t kerning;       /**< Font kerning, see `lv_font_kerning_t`*/
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Вернитесь с растровым изображением шрифта.
 * Он всегда конвертирует обычные шрифты в форматA8вdraw_bufс
 * LV_DRAW_BUF_ALIGN и LV_DRAW_BUF_STRIDE_ALIGN
 * @note Вы должны вызвать lv_font_get_glyph_dsc(), чтобы получить`g_dsc`(lv_font_glyph_dsc_t)
 *       прежде чем вы сможете вызвать эту функцию.
 * @param g_dsc         дескриптор глифа, включая используемый шрифт, который предоставляет glyph_index
 *                      и формат.
 * @param draw_buf      буфер рисования, который можно использовать для хранения растрового изображения глифа.
 * @return              указатель на данные глифа.
 *                      Это может быть буфер отрисовки для растровых шрифтов или источник изображения для imgfonts.
 */
const void * lv_font_get_glyph_bitmap(lv_font_glyph_dsc_t * g_dsc, lv_draw_buf_t * draw_buf);


/**
 * Верните растровое изображение как есть. Это работает, только если шрифт хранит растровое изображение в
 * неизменяемая память.
 * @param g_dsc         дескриптор глифа, включая используемый шрифт, который предоставляет glyph_index
 *                      и формат.
 * @return              растровое изображение как оно есть
 */
const void * lv_font_get_glyph_static_bitmap(lv_font_glyph_dsc_t * g_dsc);

/**
 * Получить дескриптор глифа
 * @param font          указатель на шрифт
 * @param dsc_out       сохраните дескриптор результата здесь
 * @param letter        буквенный код UNICODE
 * @param letter_next   следующая буква после `letter`. Используется для кернинга
 * @return true: дескриптор успешно загружен в `dsc_out`.
 *         false: письмо не найдено, в`dsc_out`данные не загружаются
 */
bool lv_font_get_glyph_dsc(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t letter,
                           uint32_t letter_next);
/**
 * Освободите растровое изображение шрифта.
 * @note Прежде чем вы сможете вызвать эту функцию, вы должны вызвать lv_font_get_glyph_dsc(), чтобы получить`g_dsc`(lv_font_glyph_dsc_t).
 * @param g_dsc         дескриптор глифа, включая используемый шрифт, который предоставляетglyph_indexи формат.
 */
void lv_font_glyph_release_draw_data(lv_font_glyph_dsc_t * g_dsc);

/**
 * Получить ширину глифа с помощью кернинга
 * @param font          указатель на шрифт
 * @param letter        письмо UNICODE
 * @param letter_next   следующая буква после `letter`. Используется для кернинга
 * @return ширина глифа
 */
uint16_t lv_font_get_glyph_width(const lv_font_t * font, uint32_t letter, uint32_t letter_next);

/**
 * Получите высоту строки шрифта. Все персонажи вписываются в этот рост
 * @param font      указатель на шрифт
 * @return высота шрифта
 */
int32_t lv_font_get_line_height(const lv_font_t * font);

/**
 * Настройка использования информации кернинга, хранящейся в шрифте.
 * @param font    указатель на шрифт
 * @param kerning `LV_FONT_KERNING_NORMAL` (по умолчанию) или `LV_FONT_KERNING_NONE`
 */
void lv_font_set_kerning(lv_font_t * font, lv_font_kerning_t kerning);

/**
 * Получите шрифт по умолчанию, определенный LV_FONT_DEFAULT.
 * @return  вернуть указатель на шрифт по умолчанию
 */
const lv_font_t * lv_font_get_default(void);

/**
 * Сравните информацию о шрифтах.
 * @param ft_info_1 информация о шрифте 1.
 * @param ft_info_2 информация о шрифте 2.
 * @return верните true, если шрифты равны.
 */
bool lv_font_info_is_equal(const lv_font_info_t * ft_info_1, const lv_font_info_t * ft_info_2);

/**
 * Проверяет, имеет ли шрифт статическое растровое изображение рендеринга.
 * @param font    указатель на шрифт
 * @return верните true, если шрифт имеет растровое изображение, созданное для статического рендеринга.
 */
bool lv_font_has_static_bitmap(const lv_font_t * font);

/**********************
 *      MACROS
 **********************/

#define LV_FONT_DECLARE(font_name) LV_ATTRIBUTE_EXTERN_DATA extern const lv_font_t font_name;

#if LV_FONT_MONTSERRAT_8
LV_FONT_DECLARE(lv_font_montserrat_8)
#endif

#if LV_FONT_MONTSERRAT_10
LV_FONT_DECLARE(lv_font_montserrat_10)
#endif

#if LV_FONT_MONTSERRAT_12
LV_FONT_DECLARE(lv_font_montserrat_12)
#endif

#if LV_FONT_MONTSERRAT_14
LV_FONT_DECLARE(lv_font_montserrat_14)
#endif

#if LV_FONT_MONTSERRAT_16
LV_FONT_DECLARE(lv_font_montserrat_16)
#endif

#if LV_FONT_MONTSERRAT_18
LV_FONT_DECLARE(lv_font_montserrat_18)
#endif

#if LV_FONT_MONTSERRAT_20
LV_FONT_DECLARE(lv_font_montserrat_20)
#endif

#if LV_FONT_MONTSERRAT_22
LV_FONT_DECLARE(lv_font_montserrat_22)
#endif

#if LV_FONT_MONTSERRAT_24
LV_FONT_DECLARE(lv_font_montserrat_24)
#endif

#if LV_FONT_MONTSERRAT_26
LV_FONT_DECLARE(lv_font_montserrat_26)
#endif

#if LV_FONT_MONTSERRAT_28
LV_FONT_DECLARE(lv_font_montserrat_28)
#endif

#if LV_FONT_MONTSERRAT_30
LV_FONT_DECLARE(lv_font_montserrat_30)
#endif

#if LV_FONT_MONTSERRAT_32
LV_FONT_DECLARE(lv_font_montserrat_32)
#endif

#if LV_FONT_MONTSERRAT_34
LV_FONT_DECLARE(lv_font_montserrat_34)
#endif

#if LV_FONT_MONTSERRAT_36
LV_FONT_DECLARE(lv_font_montserrat_36)
#endif

#if LV_FONT_MONTSERRAT_38
LV_FONT_DECLARE(lv_font_montserrat_38)
#endif

#if LV_FONT_MONTSERRAT_40
LV_FONT_DECLARE(lv_font_montserrat_40)
#endif

#if LV_FONT_MONTSERRAT_42
LV_FONT_DECLARE(lv_font_montserrat_42)
#endif

#if LV_FONT_MONTSERRAT_44
LV_FONT_DECLARE(lv_font_montserrat_44)
#endif

#if LV_FONT_MONTSERRAT_46
LV_FONT_DECLARE(lv_font_montserrat_46)
#endif

#if LV_FONT_MONTSERRAT_48
LV_FONT_DECLARE(lv_font_montserrat_48)
#endif

#if LV_FONT_MONTSERRAT_28_COMPRESSED
LV_FONT_DECLARE(lv_font_montserrat_28_compressed)
#endif

#if LV_FONT_DEJAVU_16_PERSIAN_HEBREW
LV_FONT_DECLARE(lv_font_dejavu_16_persian_hebrew)
#endif

#if LV_FONT_SOURCE_HAN_SANS_SC_14_CJK
LV_FONT_DECLARE(lv_font_source_han_sans_sc_14_cjk)
#endif

#if LV_FONT_SOURCE_HAN_SANS_SC_16_CJK
LV_FONT_DECLARE(lv_font_source_han_sans_sc_16_cjk)
#endif

#if LV_FONT_UNSCII_8
LV_FONT_DECLARE(lv_font_unscii_8)
#endif

#if LV_FONT_UNSCII_16
LV_FONT_DECLARE(lv_font_unscii_16)
#endif

/*Объявите пользовательские (определяемые пользователем) шрифты*/
#ifdef LV_FONT_CUSTOM_DECLARE
LV_FONT_CUSTOM_DECLARE
#endif

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_FONT_H*/
