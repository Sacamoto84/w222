/* TSI 2023.xmo */
/*******************************************************************************
 * Copyright (c) 2023 Think Silicon Single Member PC
 *
 * Разрешение настоящим предоставляется бесплатно любому лицу, получившему копию.
 * этого файла заголовка и/или связанных с ним файлов документации для использования, копирования,
 * изменять, объединять, публиковать, распространять, сублицензировать и/или продавать копии
 * Материалы и разрешать лицам, которым предоставлены Материалы, делать
 * Итак, при соблюдении следующих условий:
 *
 * Вышеупомянутое уведомление об авторских правах и настоящее уведомление о разрешении должны быть включены в
 * все копии или существенные части Материалов.
 *
 * MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
 * NEMAGFX API. THE UNMODIFIED, NORMATIVE VERSIONS OF THINK-SILICON NEMAGFX
 * SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT:
 *   https://think-silicon.com/products/software/nemagfx-api
 *
 *  Программное обеспечение предоставляется «как есть», без каких-либо явных или явных гарантий.
 *  подразумеваемые, включая, помимо прочего, гарантии товарной пригодности,
 *  пригодность для конкретной цели и отсутствие нарушений. Ни в коем случае нельзя
 *  Считайте, что Silicon Single MemberPCнесет ответственность за любые претензии, ущерб или другие
 *  ответственность, будь то по договору, правонарушению или иным образом, возникающая
 *  из, вне или в связи с программным обеспечением, использованием или другими сделками
 *  в программном обеспечении.
 ******************************************************************************/


#ifndef NEMA_FONT_H__
#define NEMA_FONT_H__

#include "nema_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NEMA_ALIGNX_LEFT    (0x00U) /**< Align horizontally to the left */
#define NEMA_ALIGNX_RIGHT   (0x01U) /**< Align horizontally to the right */
#define NEMA_ALIGNX_CENTER  (0x02U) /**< Align horizontally centered */
#define NEMA_ALIGNX_JUSTIFY (0x03U) /**< Justify horizontally */
#define NEMA_ALIGNX_MASK    (0x03U) /**< Horizontal alignment mask */
#define NEMA_ALIGNY_TOP     (0x00U) /**< Align vertically to the top */
#define NEMA_ALIGNY_BOTTOM  (0x04U) /**< Align vertically to the bottom */
#define NEMA_ALIGNY_CENTER  (0x08U) /**< Align vertically centered */
#define NEMA_ALIGNY_JUSTIFY (0x0cU) /**< Justify vertically */
#define NEMA_ALIGNY_MASK    (0x0cU) /**< Vertical alignment mask */
#define NEMA_TEXT_WRAP      (0x10U) /**< Use text wrapping */

typedef struct {
    uint32_t   left;     /**< Neighbor character to the left of the current one (Unicode value) */
    int8_t     x_offset; /**< Kerning offset (horizontally) */
} nema_kern_pair_t;

typedef struct {
    uint32_t bitmapOffset;
    uint8_t  width;
    uint8_t  xAdvance;
    int8_t   xOffset;
    int8_t   yOffset;
    uint32_t kern_offset;
    uint8_t  kern_length;
} nema_glyph_t;

typedef struct {
    int      bitmapOffset;
    uint8_t  width;
    uint8_t  xAdvance;
    int8_t   xOffset;
    int8_t   yOffset;
    int      id;
} nema_glyph_indexed_t;

typedef struct {
    uint32_t  first, last;
    const nema_glyph_t *glyphs;
} nema_font_range_t;

typedef struct {
    nema_buffer_t   bo;
    const nema_font_range_t         *ranges;
    const int                        bitmap_size;
    const uint8_t                   *bitmap;
    uint32_t                         flags;
    uint8_t                          xAdvance; //по умолчанию xAdvance
    uint8_t                          yAdvance;
    uint8_t                          max_ascender;
    uint8_t                          bpp;
    const nema_kern_pair_t          *kern_pairs;
    const nema_glyph_indexed_t      *indexed_glyphs;
} nema_font_t;

/** \brief Привяжите шрифт для использования в последующих вызовахnema_print()
 *
 * \param font Указатель шрифта
 *
 */
void nema_bind_font(nema_font_t *font);

/** \brief помочь и поднять ограждающую структуру рамки.
 *
 * \param str Указатель текста
 * \param w Указатель на переменную, которую следует записать
 * \param h Указатель на переменную, которую следует записать высоту
 * \parammax_wМаксимально допустимая ширина
 * \return Количество возвратов каретки
 *
 */
int  nema_string_get_bbox(const char *str, int *w, int *h, int max_w, uint32_t wrap);

/** \brief Распечатать отформатированный текст
 *
 * \param str Указатель текста
 * \param x X координата верхнего левого угла текстовой области
 * \param y Y координата верхнего левого угла текстовой области
 * \param w Ширина текстовой области
 * \param h Высота текста области
 * \paramfg_colЦвет текста переднего плана
 * \param align Режим соревнований и переноса
 *
 */
void nema_print(const char *str, int x, int y, int w, int h, uint32_t fg_col, uint32_t align);

/** \brief Распечатать отформатированный текст
 *
 * \param *str Указатель текста
 * \param *cursor_xX-позиция следующего символа, который будет нарисован. Обычно возникает пользовательское значение 0, а затем обновляется внутри библиотеки.
 * \param *cursor_yПозиция Y следующего символа, который будет нарисован. Обычно возникает пользовательское значение 0, а затем обновляется внутри библиотеки.
 * \param x X координата верхнего левого угла текстовой области
 * \param y Y координата верхнего левого угла текстовой области
 * \param w Ширина текстовой области
 * \param h Высота текста области
 * \paramfg_colЦвет текста переднего плана
 * \param align Режим соревнований и переноса
 *
 */
void nema_print_to_position(const char *str, int *pos_x, int *pos_y, int x, int y, int w, int h, uint32_t fg_col, uint32_t align);


/** \brief Распечатать текст (неотформатированный) с индексированными глифами. Текст печатается в одной строке слева направо.
 *
 * \param ids Массив с индексами глифов
 * \paramid_countКоличество символов, которые будут нарисованы
 * \param x X координата верхнего левого угла текстовой области
 * \param y Y координата верхнего левого угла текстовой области
 * \paramfg_colЦвет текста переднего плана
 *
 */
void nema_print_indexed(const int *ids, int id_count, int x, int y, uint32_t fg_col);

/** \brief Печать одного символа с индексированным глифом
 *
 * \param id Массив с индексами глифов
 * \param x X координата верхнего левого угла персонажа
 * \param y координата Y верхнего левого угла символа
 * \paramfg_colЦвет персонажа
 *
 */
void nema_print_char_indexed(const int id, int x, int y, uint32_t fg_col);

/** \brief Возвращает нестабильность и высоту возвышающейся рамки строк с индексированными глифами.
 *
 * \details Строка должна быть указана как однострочный текст из-за ограничений
 * что символы описываются соответствующими индексами глифов. Высота
 * ограничивающая рамка будет равна высоте связанного шрифта.
 *
 * \param ids Массив с индексами глифов
 * \paramid_countКоличество символов, содержащихся в массиве с индексами глифов
 * \param w Указатель на переменную, которую следует записать
 * \param h Указатель на переменную, которую следует записать высоту
 * \parammax_wМаксимально допустимая ширина (если w больше этого значения, оно будет насыщено до этого значения)
 */
void nema_string_indexed_get_bbox(const int *ids, int id_count, int *w, int *h, int max_w);

/** \brief Возвращает горизонтальное перемещение (в пикселях) связанного шрифта.
 *
 */
int nema_font_get_x_advance(void);

#ifdef __cplusplus
}
#endif

#endif // NEMA_FONT_H__
