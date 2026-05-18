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
 * @brief Функции, связанные с покраской. Paint — это внутренняя (непрозрачная) структура NemaVG.
 * Определенные здесь функции можно использовать для доступа к его параметрам.
 *
 */

#ifndef __NEMA_VG_PAINT_H__
#define __NEMA_VG_PAINT_H__

#include "nema_interpolators.h"
#include "nema_matrix3x3.h"
#include "nema_vg_context.h"
#include "nema_graphics.h"

#ifdef __cplusplus
extern "C" {
#endif

#define  NEMA_VG_PAINT_COLOR        (0x00U) /**< Fill with color */
#define  NEMA_VG_PAINT_FILL         (0x00U) /**< Deprecated - Fill with color (same as NEMA_VG_PAINT_COLOR) */
#define  NEMA_VG_PAINT_GRAD_LINEAR  (0x01U) /**< Fill with linear gradient */
#define  NEMA_VG_PAINT_TEXTURE      (0x02U) /**< Fill with texture */
#define  NEMA_VG_PAINT_GRAD_RADIAL  (0x03U) /**< Fill with radial gradient */
#define  NEMA_VG_PAINT_GRAD_CONICAL (0x04U) /**< Fill with conical gradient */

#define  NEMA_VG_PAINT_MAX_GRAD_STOPS (32) /**< Maximum gradient stops*/

/** \brief создать объект рисования.
 *
 * \return Дескриптор созданного объекта рисования
 *
 */
NEMA_VG_PAINT_HANDLE nema_vg_paint_create();

/** \brief Уничтожить объект рисования.
 *
 * \param Дескриптор Paint для рисования объекта, который должен быть уничтожен.
 *
 */
void nema_vg_paint_destroy(NEMA_VG_PAINT_HANDLE paint);

/** \brief Очистка параметров объекта рисования.
 *
 * \param Paint Указатель (дескриптор) для рисования объекта
 *
 */
void nema_vg_paint_clear(NEMA_VG_PAINT_HANDLE paint);

/** \brief Установка типа краски
 *
 * \param Paint Указатель (дескриптор) для рисования
 * \param type Тип краски (NEMA_VG_PAINT_COLOR,NEMA_VG_PAINT_GRAD_LINEAR,NEMA_VG_PAINT_TEXTURE,NEMA_VG_PAINT_GRAD_RADIAL,NEMA_VG_PAINT_GRAD_CONICAL)
 *
 */
void nema_vg_paint_set_type(NEMA_VG_PAINT_HANDLE paint, uint8_t type);


/** \brief Заблокируйте преобразование краски по пути. Если заблокировано, путь и краска
 *  трансформация будет синхронизирована.
 *
 * \param Paint Указатель для рисования объекта
 * \param locked 1, если заблокировано (по умолчанию), 0, если не заблокировано
 *
 */
void nema_vg_paint_lock_tran_to_path(NEMA_VG_PAINT_HANDLE paint, int locked);

/** \brief Установка линейного градиента для объекта рисования
 *
 * \param Paint Указатель для рисования объекта
 * \param grad Указатель объекта градиента
 * \param x0 Координата x начальные точки линейного градиента
 * \param y0 Координата исходных точек линейного градиента
 * \param x1 Координата x конечной точки линейного градиента
 * \param y1 Координата и конечные точки линейного градиента
 * \paramsampling_modeРежим выбора.  NEMA_TEX_BORDERпо умолчанию равенNEMA_TEX_CLAMP.
 *
 */
void nema_vg_paint_set_grad_linear(NEMA_VG_PAINT_HANDLE paint,
                                  NEMA_VG_GRAD_HANDLE grad,
                                  float x0, float y0,
                                  float x1, float y1,
                                  nema_tex_mode_t sampling_mode);

 /** \brief Установка цвета краски
 *
 * \param Paint Указатель (дескриптор) для рисования объекта
 * \param rgba Устанавливаемый цвет в формате rgba (шестнадцатеричный 0xAABBGGRR).
 *
 */
void nema_vg_paint_set_paint_color(NEMA_VG_PAINT_HANDLE paint, uint32_t rgba);

 /** \brief Установка непрозрачности краски
 *
 * \param Paint Указатель (указатель) для рисования объекта
 * \param opacity Устанавливаемая непрозрачность: 1 — полностью непрозрачный, 0 — полностью прозрачный.
 *
 */
void nema_vg_paint_set_opacity(NEMA_VG_PAINT_HANDLE paint, float opacity);

 /** \brief Установка направления хода -DEPRECATEDUSE nema_vg_stroke_set_width
 *
 * \param Paint Указатель (дескриптор) для рисования объекта
 * \paramstroke_widthРегулируемая ширина обводки
 *
 */
void nema_vg_paint_set_stroke_width(NEMA_VG_PAINT_HANDLE paint, float stroke_width);

 /** \brief Установка матрицы конвертировать текстуры
 *
 * \param Paint Указатель (дескриптор) для рисования объекта
 * \param m матрица конвертировать 3x3
 *
 */
void nema_vg_paint_set_tex_matrix(NEMA_VG_PAINT_HANDLE paint, nema_matrix3x3_t m);

 /** \brief Установка текстур для рисования объекта
 *
 * \param Paint Указатель (дескриптор) для рисования
 * \param text Указатель объекта изображения текстуры
 *
 */
void nema_vg_paint_set_tex(NEMA_VG_PAINT_HANDLE paint, nema_img_obj_t* tex);

 /** \brief Установите текстуру на основе Lut (справочная таблица) для рисования объекта. См. руководство пользователя Nema Pixpresso относительно форматов Lut.
 *
 * \param Paint Указатель (дескриптор) для рисования объекта
 * \paramlut_paletteУказатель на палитру объекта изображения Лут
 * \paramlut_indicesУказатель индекса объекта изображения Lut
 *
 */
void nema_vg_paint_set_lut_tex(NEMA_VG_PAINT_HANDLE paint, nema_img_obj_t* lut_palette, nema_img_obj_t* lut_indices);

/** \brief Установите конический градиент для рисования объекта
 *
 * \param Paint Указатель (дескриптор) для рисования
 * \param grad Указатель (дескриптор) градиента
 * \param cx Координата x центральных точек конического градиента
 * \param cy Координата и центральные точки конического градиента
 * \paramsampling_modeРежим выбора
 *
 */
void nema_vg_paint_set_grad_conical(NEMA_VG_PAINT_HANDLE paint,
                                    NEMA_VG_GRAD_HANDLE grad,
                                    float cx, float cy,
                                    nema_tex_mode_t sampling_mode);

/** \brief Установка радиального градиента для рисования объекта
 *
 * \param Paint Указатель (дескриптор) для рисования
 * \param grad Указатель (дескриптор) градиента
 * \param x0 Координата x центральных точек радиального градиента
 * \param y0 Координата центра радиального градиента по оси y
 * \param r Радиусного градиента
 * \paramsampling_modeРежим выбора
 *
 */
void
nema_vg_paint_set_grad_radial(NEMA_VG_PAINT_HANDLE paint,
                              NEMA_VG_GRAD_HANDLE grad,
                              float x0, float y0,
                              float r,
                              nema_tex_mode_t sampling_mode);


/** \brief Установите радиальный градиент для рисования объекта с разным горизонтальным и вертикальным радиусом
 *
 * \param Paint Указатель (дескриптор) для рисования
 * \param grad Указатель (дескриптор) градиента
 * \param x0 Координата x центральных точек радиального градиента
 * \param y0 Координата центра радиального градиента по оси y
 * \param rx Радиальный градиент градиента по оси x
 * \param ry Радиусного градиента оси Y
 * \paramsampling_modeРежим выбора
 *
 */
void
nema_vg_paint_set_grad_radial2(NEMA_VG_PAINT_HANDLE paint,
                               NEMA_VG_GRAD_HANDLE grad,
                               float x0, float y0,
                               float rx, float ry,
                               nema_tex_mode_t sampling_mode);

/** \brief Создание объекта градиента
 *
 * \return Дескриптор (указатель) созданного объекта градиента
 */
NEMA_VG_GRAD_HANDLE
nema_vg_grad_create(void);

/** \brief Уничтожить объект градиента
 *
 * \param grad Указатель объекта градиента
 *
 */
void
nema_vg_grad_destroy(NEMA_VG_GRAD_HANDLE grad);

/** \brief Установка параметров градиента для объекта градиента
 *
 * \param grad Указатель (дескриптор) объекта градиента
 * \paramstops_countКоличество стоп-цветов
 * \param stop Указатель для остановки координат цветов
 * \paramcolors Указатель для остановки измерений цвета
 *
 */
void
nema_vg_grad_set(NEMA_VG_GRAD_HANDLE grad, int stops_count, float *stops, color_var_t* colors);

#ifdef __cplusplus
}
#endif

#endif //__NEMA_VG_PAINT_H__
