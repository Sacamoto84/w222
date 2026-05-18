/**
 * @file lv_draw_label.h
 *
 */

#ifndef LV_DRAW_LABEL_H
#define LV_DRAW_LABEL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "lv_draw_rect.h"
#include "../misc/lv_bidi.h"
#include "../misc/lv_text.h"
#include "../misc/lv_color.h"
#include "../misc/lv_style.h"

/*********************
 *      DEFINES
 *********************/
#define LV_DRAW_LABEL_NO_TXT_SEL (0xFFFF)

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_dsc_base_t base;

    /**The text to draw*/
    const char * text;

    /**The size of the text*/
    lv_point_t text_size;

    /**The font to use. Fallback fonts are also handled.*/
    const lv_font_t * font;

    /**Color of the text*/
    lv_color_t color;

    /**Extra space between the lines*/
    int32_t line_space;

    /**Extra space between the characters*/
    int32_t letter_space;

    /**Offset the text with this value horizontally*/
    int32_t ofs_x;

    /**Offset the text with this value vertically*/
    int32_t ofs_y;

    /**Rotation of the letters in 0.1 degree unit*/
    int32_t rotation;

    /**The first characters index for selection (not byte index). `LV_DRAW_LABEL_NO_TXT_SEL` for no selection*/
    uint32_t sel_start;

    /**The last characters's index for selection (not byte index). `LV_DRAW_LABEL_NO_TXT_SEL` for no selection*/
    uint32_t sel_end;

    /**Color of the selected characters*/
    lv_color_t sel_color;

    /**Background color of the selected characters*/
    lv_color_t sel_bg_color;

    /**The number of characters to render. 0: means render until reaching the `\0` termination.*/
    uint32_t text_length;

    /**The alignment of the text `LV_TEXT_ALIGN_LEFT/RIGHT/CENTER`*/
    lv_text_align_t align;

    /**The base direction. Used when type setting Right-to-left (e.g. Arabic) texts*/
    lv_base_dir_t bidi_dir;

    /**Непрозрачность текста в диапазоне 0...255.
     * LV_OPA_TRANSP , LV_OPA_10 , LV_OPA_20 , .. LV_OPA_COVER также можно использовать*/
    lv_opa_t opa;

    /**Letter outline stroke opacity */
    lv_opa_t outline_stroke_opa;

    /**Text decoration, e.g. underline*/
    lv_text_decor_t decor : 3;

    /**Some flags to control type setting*/
    lv_text_flag_t flag : 5;

    /**1: malloc буфер и скопируйте туда `text`.
     * 0: `text` будет действителен во время рендеринга.*/
    uint8_t text_local : 1;

    /**Indicate that the text is constant and its pointer can be safely saved e.g. in a cache.*/
    uint8_t text_static : 1;

    /**1: уже выполненlv_bidi_process_paragraph.
     * 0: не был выполненlv_bidi_process_paragraph.*/
    uint8_t has_bided : 1;

    /**Pointer to an externally stored struct where some data can be cached to speed up rendering*/
    lv_draw_label_hint_t * hint;

    /* Свойства контуров букв */
    lv_color_t outline_stroke_color;
    int32_t outline_stroke_width;

} lv_draw_label_dsc_t;

typedef struct {
    lv_draw_dsc_base_t base;

    uint32_t unicode;
    const lv_font_t * font;
    lv_color_t color;

    int32_t rotation;
    int32_t scale_x;
    int32_t scale_y;
    int32_t skew_x;
    int32_t skew_y;
    lv_point_t pivot;

    lv_opa_t opa;
    lv_text_decor_t decor : 3;
    lv_blend_mode_t blend_mode : 4;

    /* Свойства контуров букв */
    lv_opa_t outline_stroke_opa;
    int32_t outline_stroke_width;
    lv_color_t outline_stroke_color;

} lv_draw_letter_dsc_t;

/**
 * Передано в качестве параметра`lv_draw_label_iterate_characters`для
 * рисуй персонажей один за другим
 * @param fill_area             указатель на задачу рисования
 * @param dsc           указатель на`lv_draw_glyph_dsc_t`для описания рисуемого символа
 *                      если NULL не рисует персонажа
 * @param fill_dsc      указатель на дескриптор заливки для рисования фона персонажа или
 *                      подчеркнуть или зачеркнуть
 *                      если NULL ничего не заполняем
 * @param fill_area     область для заполнения
 *                      если NULL ничего не заполняем
 */
typedef void(*lv_draw_glyph_cb_t)(lv_draw_task_t * t, lv_draw_glyph_dsc_t * dsc, lv_draw_fill_dsc_t * fill_dsc,
                                  const lv_area_t * fill_area);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_letter_dsc_init(lv_draw_letter_dsc_t * dsc);

/**
 * Инициализировать дескриптор рисования метки
 * @param dsc       указатель на дескриптор отрисовки
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_label_dsc_init(lv_draw_label_dsc_t * dsc);

/**
 * Попробуйте получить дескриптор отрисовки метки из задачи рисования.
 * @param task      нарисовать задачу
 * @return          дескриптор отрисовки задачи или NULL, если задача не относится к типу LV_DRAW_TASK_TYPE_LABEL
 */
lv_draw_label_dsc_t * lv_draw_task_get_label_dsc(lv_draw_task_t * task);

/**
 * Инициализируйте дескриптор отрисовки глифа.
 * Используется внутренне.
 * @param dsc       указатель на дескриптор отрисовки
 */
void lv_draw_glyph_dsc_init(lv_draw_glyph_dsc_t * dsc);

/**
 * Создайте задачу рисования для визуализации текста.
 * @param layer         указатель на слой
 * @param dsc           указатель для рисования дескриптора
 * @param coords        координаты персонажа
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_label(lv_layer_t * layer, const lv_draw_label_dsc_t * dsc,
                                               const lv_area_t * coords);

/**
 * Создайте задачу рисования для визуализации одного символа.
 * @param layer          указатель на слой
 * @param dsc            указатель для рисования дескриптора
 * @param point          положение этикетки
 * @param unicode_letter письмо, которое нужно нарисовать
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_character(lv_layer_t * layer, lv_draw_label_dsc_t * dsc,
                                                   const lv_point_t * point, uint32_t unicode_letter);

/**
 * Нарисуй одну букву
 * @param layer          указатель на слой
 * @param dsc            указатель для рисования дескриптора
 * @param point          положение этикетки
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_letter(lv_layer_t * layer, lv_draw_letter_dsc_t * dsc,
                                                const lv_point_t * point);

/**
 * Следует использовать во время рендеринга символов для получения положения и других
 * параметры персонажей
 * @param t             указатель на задачу рисования
 * @param dsc           указатель для рисования дескриптора
 * @param coords        координаты метки
 * @param cb            обратный вызов для вызова для рисования каждого глифа один за другим
 */
void lv_draw_label_iterate_characters(lv_draw_task_t * t, const lv_draw_label_dsc_t * dsc,
                                      const lv_area_t * coords, lv_draw_glyph_cb_t cb);

/**
 * @brief Нарисуйте одну букву, используя предоставленный блок рисования, дескриптор глифа, положение, шрифт и обратный вызов.
 *
 * Эта функция отвечает за рендеринг одного символа из текстовой строки.
 * Применение необходимого стиля, описанного дескриптором глифа (`dsc`). Он обрабатывает
 * при получении описания глифа проверяется его видимость в области обрезки,
 * и вызов обратного вызова (`cb`) для отображения глифа в указанной позиции (`pos`)
 * с помощью данного шрифта (`font`).
 *
 * @param t             Указатель на задачу рисования.
 * @param dsc           Указатель на дескриптор, содержащий стиль рисуемого глифа.
 * @param pos           Указатель на координаты точки, где должна быть нарисована буква.
 * @param font          Указатель на шрифт, содержащий глиф.
 * @param letter        Код Unicode для рисуемой буквы.
 * @param cb            Функция обратного вызова для выполнения фактического рендеринга глифа.
 */
void lv_draw_unit_draw_letter(lv_draw_task_t * t, lv_draw_glyph_dsc_t * dsc,  const lv_point_t * pos,
                              const lv_font_t * font, uint32_t letter, lv_draw_glyph_cb_t cb);

/***********************
 * GLOBAL VARIABLES
 ***********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_LABEL_H*/
