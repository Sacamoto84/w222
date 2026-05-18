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

/**
 * @file
 * @brief Рендеринг векторного шрифта
 *
 * Этот файл включает в себя необходимые структуры и функции, которые используются для рендеринга текста (строки и отдельные символы).
 * с использованием векторных шрифтов. Сопутствующая утилита конвертера векторных шрифтов преобразует шрифты TrueType (файлы TTF) в отдельных экземплярах.
 * структура, определенная здесь. Вариант использования этого модуля включен в соответствующие примеры (examples/NemaVG/ render_vg_font).
 */

#ifndef NEMA_VG_FONT_H_
#define NEMA_VG_FONT_H_

#include "nema_matrix3x3.h"
#include "nema_vg.h"
#include "nema_vg_context.h"
#include "nema_font.h" //преобразование вектора в растр

#ifdef __cplusplus
extern "C" {
#endif

#define NEMA_VG_ALIGNX_LEFT    (0x00U) /**< Align horizontally to the left */
#define NEMA_VG_ALIGNX_RIGHT   (0x01U) /**< Align horizontally to the right */
#define NEMA_VG_ALIGNX_CENTER  (0x02U) /**< Align horizontally centered */
#define NEMA_VG_ALIGNX_JUSTIFY (0x03U) /**< Justify horizontally */
#define NEMA_VG_ALIGNX_MASK    (0x03U) /**< Horizontal alignment mask */
#define NEMA_VG_ALIGNY_TOP     (0x00U) /**< Align vertically to the top */
#define NEMA_VG_ALIGNY_BOTTOM  (0x04U) /**< Align vertically to the bottom */
#define NEMA_VG_ALIGNY_CENTER  (0x08U) /**< Align vertically centered */
#define NEMA_VG_ALIGNY_JUSTIFY (0x0cU) /**< Justify vertically */
#define NEMA_VG_ALIGNY_MASK    (0x0cU) /**< Vertical alignment mask */
#define NEMA_VG_TEXT_WRAP      (0x10U) /**< Use text wrapping */

#define NEMA_VG_CHAR_LTR       (0x00U) /**< Character follows left to right orientation */
#define NEMA_VG_CHAR_RTL       (0x01U) /**< Character follows right to left orientation */
#define NEMA_VG_CHAR_TTB       (0x00U) /**< Character follows top to bottom orientation */
#define NEMA_VG_CHAR_BTT       (0x02U) /**< Character follows bottom to top orientation */


/** Структура данных информационной пары NemaVG Кернинга */
typedef struct {
    const uint32_t   left;     /**< Neighbor character to the left of the current one (Unicode value) */
    const float      x_offset; /**< Kerning offset value (horizontally) */
} nema_vg_kern_pair_t;

/** Структура данных NemaVG глифа в векторном формате*/
typedef struct {
    const uint32_t  data_offset;    /**< Offset value for the data of the glyph in the respective data array */
    const size_t    data_length;    /**< Length of the data in the respective data array */
    const uint32_t  segment_offset; /**< Offset value for the segments of the glyph in the respective segment array */
    const size_t    segment_length; /**< Length of the segments in the respective segment array */
    const float     xAdvance;       /**< Advance width*/
    const uint32_t  kern_offset;    /**< Kerning offset  of the glyph in the respective kerning array */
    const uint8_t   kern_length;    /**< Length of the kerning information of the glyph */
    const int16_t   bbox_xmin;      /**< Minimum x of the glyph's bounding box */
    const int16_t   bbox_ymin;      /**< Minimum y of the glyph's bounding box */
    const int16_t   bbox_xmax;      /**< Maximum x of the glyph's bounding box */
    const int16_t   bbox_ymax;      /**< Maximum y of the glyph's bounding box */
} nema_vg_glyph_t;

/** Структура данных связанных векторных шрифтов NemaVG */
typedef struct {
    const uint32_t first;          /**< Unicode value of the first value of the range */
    const uint32_t last;           /**< Unicode value of the last value of the range */
    const nema_vg_glyph_t *glyphs; /**< Pointer to the array of glyphs */
} nema_vg_font_range_t;

/** Структура данных векторного шрифта NemaVG*/
typedef struct {
    const uint32_t             version;          /**< Font version */
    const nema_vg_font_range_t *ranges;          /**< Pointer to the array of ranges */
    const nema_vg_float_t      *data;            /**< Pointer to the data of the vector font */
    const size_t                data_length;     /**< Length of the vector font data*/
    const uint8_t              *segment;         /**< Pointer to the segments of the vector font */
    const size_t                segment_length;  /**< Length of the vector font segments */
    const float                 size;            /**< Default font size (height) */
    const float                 xAdvance;        /**< Default advance width. If the space character is included in the ranges, then its advance width is set */
    const float                 ascender;        /**< Vertical distance from the baseline to the highest point of the font */
    const float                 descender;       /**< Vertical distance from the baseline to the lowest point of the font */
    const nema_vg_kern_pair_t  *kern_pairs;      /**< Pointer to the array of the font's kerning pairs */
    uint32_t                    flags;           /**< Bit field, reserved for future use */
} nema_vg_font_t;

/** \brief Свяжите шрифт, который будет использоваться в последующих вызовах nema_vg_print(). Устанавливает код неправильно, если шрифт не указан.
 *
 * \param font Указатель векторного шрифта
 *
 */
void nema_vg_bind_font(nema_vg_font_t *font);

/** \brief Устанавливает размер связанного шрифта. Будущие вызовыnema_vg_print() иnema_vg_print_char() будут печатать с использованием последнего установленного размера.
 *
 * \param font Указатель векторного шрифта
 *
 */
void nema_vg_set_font_size(float size);


/** \brief Распечатать отформатированный текст
 *
 * \param Paint Указатель текущего объекта рисования (содержит цвет текста)
 * \param str Указатель текста
 * \param x X координата верхнего левого угла текстовой области
 * \param y Y координата верхнего левого угла текстовой области
 * \param w Макс. разрешенная ширина
 * \param h Макс. разрешенная высота
 * \param align Режим соревнований и переноса
 * \param m Матрица конвертировать
 *
 */
void nema_vg_print(NEMA_VG_PAINT_HANDLE paint, const char *str, float x, float y, float w, float h, uint32_t align, nema_matrix3x3_t m);


/** \brief Используйте поддержку и высоту, огибающую структуру векторной строки. Перед вызовом этой функции необходимо сначала вызвать «nema_vg_set_font_size».
 *
 * \param str Указатель текста
 * \param w Указатель на переменную, которую следует записать
 * \param h Указатель на переменную, которую следует записать высоту
 * \parammax_wМаксимально допустимая ширина
 * \param size размер шрифта
 * \param Wrap включить перенос текста
 * \return Количество возвратов каретки
 *
 */
int  nema_vg_string_get_bbox(const char *str, float *w, float *h, float max_w, uint32_t wrap);


/** \brief Присвойте значение верхнему элементу текста в пунктах. Размер шрифта должен быть установлен перед вызовом этой функции.
 *
 * \return Восходящий пункт
 *
 */
int nema_vg_get_ascender_pt(void);

/** \brief Печать одного символа
 *
 * \details Положение символа Определено аргументом «ориентация».
 * Аргументы x и y определяют точку на базовой линии. Если ориентация слева направо ( LTR ),
 * символ будет помещен справа от точки (x, y). Справа налево ( RTL ) разместится
 * символ слева от точки (x, y). Сверху вниз ( TTB ) будет иметь тот же эффект, что и
 * RTL и снизу вверх ( BTT ) поместят символ выше точки (x, y) на смещение.
 * равен высоте шрифта.
 *
 * \param Paint Указатель текущего объекта рисования (содержит цвет текста)
 * \param ch Символ для печати
 * \param x X координата верхнего левого или верхнего правого угла символа (управляется параметром «ориентация»)
 * \param y Координата Y верхнего левого или нижнего левого угла персонажа (управляется параметром «ориентация»)
 * \param m Матрица конвертировать
 * \param ориентация Символы ориентации (см. определениеNEMA_VG_CHAR_*)
 * \return Символьная ширина в пикселях
 *
 */
float nema_vg_print_char(NEMA_VG_PAINT_HANDLE paint, char ch, float x, float y, nema_matrix3x3_t m, uint32_t orientation);

/** \brief Генерирует растровый шрифт из векторного шрифта
 *
 * \details Создана растровая версия связанного векторного шрифта с развитием 8 бит на сель пик. Вы выполняете динамическое распределение памяти в
 * графическая память (для растровых изображений шрифтов) и в куче (для структур данных, к которым обращается CPU ).
 * Если шрифт большего размера не нужен, можно использовать функцию «nema_vg_destroy_raster_font()» для освобождения выделенного.
 * память. Генерация шрифта может завершиться неудачно, если для создания шрифта недостаточно памяти или когда шрифт
 * размер больше высоты фреймбуфера.
 *
 * \param size Размер создаваемого шрифта.
 * \param пул Пул памяти для хранения растровых изображений шрифтов
 * \return Указатель структуры данных сгенерированного растрового шрифта. Если шрифт не был сгенерирован (из-за нехватки памяти), он возвращаетNULL.
 *
 */
nema_font_t* nema_vg_generate_raster_font(int size, int pool);

/** \brief Освобождает память, выделенную для структуры данных шрифта.
 *
 * \details Эта функция освобождает память, выделенную во время выполнения. Ввод должен быть
 *  структура данных шрифта, созданная упрощенная «nema_vg_generate_raster_font».
 *
 * \param font Указатель структуры данных растрового шрифта, который будет удален из памяти
 *
 */
void nema_vg_destroy_raster_font(nema_font_t *font);

#ifdef __cplusplus
}
#endif
#endif // NEMA_VG_FONT_H_
