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


#ifndef NEMA_TRANSITIONS_H__
#define NEMA_TRANSITIONS_H__

#include "nema_blender.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NEMA_TRANS_LINEAR_H,
    NEMA_TRANS_CUBE_H,
    NEMA_TRANS_INNERCUBE_H,
    NEMA_TRANS_STACK_H,
    NEMA_TRANS_LINEAR_V,
    NEMA_TRANS_CUBE_V,
    NEMA_TRANS_INNERCUBE_V,
    NEMA_TRANS_STACK_V,
    NEMA_TRANS_FADE,
    NEMA_TRANS_FADE_ZOOM,
    NEMA_TRANS_MAX,
    NEMA_TRANS_NONE,
} nema_transition_t;

/** \brief Переход от «начальной» текстуры к «окончательной». Переход завершается, когда «шаг» равен 0 или 1.
 *
 * \param effect Эффект перехода
 * \param Исходная Исходная текстура
 * \param Final Окончательная текстура
 * \paramblending_modeРежим приложения
 * \param шаг Шаг перехода в сторону [0.f, 1.f]
 * \param width Ширина текстуры
 * \param height Высота текстуры
 *
 */
void nema_transition(nema_transition_t effect, nema_tex_t initial, nema_tex_t final,
                    uint32_t blending_mode, float step, int width, int height);


/** \brief Линейный переход по горизонтали. Когда «шаг» изменяется с нуля на единицу, текстуры перемещаются вправо-налево.
в противном случае текстуры перемещаются слева направо. Переход завершен, когда «шаг» равен 0 или 1.
 *
 * \param left Текстура слева
 * \param right Текстура справа
 * \paramblending_modeРежим приложения
 * \param шаг Текущий шаг в отдельности [0.f, 1.f]
 * \param width Ширина текстуры
 *
 */
void nema_transition_linear_hor(nema_tex_t left, nema_tex_t right,
                                uint32_t blending_mode, float step, int width);

/** \brief Линейный переход по вертикали. Когда «шаг» меняется с нуля на единицу, текстуры перемещаются сверху вниз.
в противном случае текстуры перемещаются снизу вверх. Переход завершен, когда «шаг» равен 0 или 1.
 *
 * \param up Текстура на верхней стороне
 * \param down Текстура на нижней стороне
 * \paramblending_modeРежим приложения
 * \param шаг Текущий шаг в отдельности [0.f, 1.f]
 * \param height Высота текстуры
 *
 */
void nema_transition_linear_ver(nema_tex_t up, nema_tex_t down,
                                uint32_t blending_mode, float step, int height);

/** \brief Кубический (текстуры накладываются на внешнюю границу куба) переход по горизонтали. Когда «шаг» меняется с нуля на единицу, текстуры перемещаются вправо-влево.
в противном случае текстуры перемещаются справа налево. Переход завершен, когда «шаг» равен 0 или 1.
 *
 * \param left Текстура слева
 * \param right Текстура справа
 * \paramblending_modeРежим приложения
 * \param шаг Текущий шаг в отдельности [0.f, 1.f]
 * \param width Ширина текстуры
 * \param height Высота текстуры
 *
 */
void nema_transition_cube_hor(nema_tex_t left, nema_tex_t right,
                              uint32_t blending_mode, float step, int width, int height);

/** \brief Куб (текстуры накладываются на внешнюю грани куба) выходят вертикально. Когда «шаг» меняется с нуля на единицу, текстуры перемещаются сверху вниз.
в противном случае текстуры перемещаются снизу вверх. Переход завершен, когда «шаг» равен 0 или 1.
 *
 * \param up Текстура на верхней стороне
 * \param down Текстура на нижней стороне
 * \paramblending_modeРежим приложения
 * \param шаг Текущий шаг в отдельности [0.f, 1.f]
 * \param width Ширина текстуры
 * \param height Высота текстуры
 *
 */
void nema_transition_cube_ver(nema_tex_t up, nema_tex_t down,
                              uint32_t blending_mode, float step, int width, int height);

/** \brief Внутренний куб (текстуры накладываются на внутреннюю границу куба) проходят горизонтально. Когда «шаг» меняется с нуля на единицу, текстуры перемещаются вправо-влево.
в противном случае текстуры перемещаются справа налево. Переход завершен, когда «шаг» равен 0 или 1.
 *
 * \param left Текстура слева
 * \param right Текстура справа
 * \paramblending_modeРежим приложения
 * \param шаг Текущий шаг в отдельности [0.f, 1.f]
 * \param width Ширина текстуры
 * \param height Высота текстуры
 *
 */
void nema_transition_innercube_hor(nema_tex_t left, nema_tex_t right,
                                   uint32_t blending_mode, float step, int width, int height);

/** \brief Внутренний куб (текстуры накладываются на внутреннюю границу куба) проходят вертикально. Когда «шаг» меняется с нуля на единицу, текстуры перемещаются сверху вниз.
в противном случае текстуры перемещаются снизу вверх. Переход Переход завершен, когда «шаг» равен 0 или 1.
 *
 * \param up Текстура на верхней стороне
 * \param down Текстура на нижней стороне
 * \paramblending_modeРежим приложения
 * \param шаг Текущий шаг в отдельности [0.f, 1.f]
 * \param width Ширина текстуры
 * \param height Высота текстуры
 *
 */
void nema_transition_innercube_ver(nema_tex_t up, nema_tex_t down,
                                   uint32_t blending_mode, float step, int width, int height);

/** \brief Переход стека по горизонтали. Когда «шаг» меняется с нуля на единицу, текстуры перемещаются вправо-влево.
в противном случае текстуры перемещаются справа налево. Переход завершен, когда «шаг» равен 0 или 1.
 *
 * \param up Текстура на верхней стороне
 * \param down Текстура на нижней стороне
 * \paramblending_modeРежим приложения
 * \param шаг Текущий шаг в отдельности [0.f, 1.f]
 * \param width Ширина текстуры
 * \param height Высота текстуры
 *
 */
void nema_transition_stack_hor(nema_tex_t left, nema_tex_t right, float step,
                                int width, int height);

/** \brief Переход стека по вертикали. Когда «шаг» перемещается от нуля до конца, текстуры перемещаются сверху вниз.
в противном случае текстуры перемещаются снизу вверх. Переход завершен, когда «шаг» равен 0 или 1.
 *
 * \param up Текстура на верхней стороне
 * \param down Текстура на нижней стороне
 * \paramblending_modeРежим приложения
 * \param шаг Текущий шаг в отдельности [0.f, 1.f]
 * \param width Ширина текстуры
 * \param height Высота текстуры
 *
 */
void nema_transition_stack_ver(nema_tex_t up, nema_tex_t down, float step,
                                int width, int height);

/** \brief Переход Fade. Первоначальная текстура исчезает, появляется окончательная текстура.
Переход завершен, когда «шаг» равен 0 или 1.
 *
 * \param left Текстура слева
 * \param right Текстура справа
 * \paramblending_modeРежим приложения
 * \param шаг Текущий шаг в отдельности [0.f, 1.f]
 * \param width Ширина текстуры
 * \param height Высота текстуры
 *
 */
void nema_transition_fade(nema_tex_t initial, nema_tex_t final,
						  uint32_t blending_mode, float step, int width, int height);

/** \краткий Переход «Затухание-зум». Первоначальная текстура увеличивается и исчезает, а окончательная текстура увеличивается и увеличивается.
Переход завершен, когда «шаг» равен 0 или 1.
 *
 * \param Исходная Исходная текстура
 * \param Final Окончательная текстура
 * \paramblending_modeРежим приложения
 * \param шаг Текущий шаг в отдельности [0.f, 1.f]
 * \param width Ширина текстуры
 * \param height Высота текстуры
 *
 */
void nema_transition_fade_zoom(nema_tex_t initial, nema_tex_t final,
                                uint32_t blending_mode, float step, int width, int height);

#ifdef __cplusplus
}
#endif

#endif
