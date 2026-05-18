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
 * @brief Функции, связанные с работой пути
 *
 */

#ifndef __NEMA_VG_PATH_H__
#define __NEMA_VG_PATH_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "nema_interpolators.h"
#include "nema_matrix3x3.h"
#include "nema_sys_defs.h"
#include "nema_vg_context.h"

#define NEMA_VG_PRIM_CLOSE              (0x00U) /**< Close segment */
#define NEMA_VG_PRIM_MOVE               (0x01U) /**< Move segment */
#define NEMA_VG_PRIM_LINE               (0x02U) /**< Line segment */
#define NEMA_VG_PRIM_HLINE              (0x03U) /**< Horizontal line segment */
#define NEMA_VG_PRIM_VLINE              (0x04U) /**< Vertical line segment */
#define NEMA_VG_PRIM_BEZIER_QUAD        (0x05U) /**< Quadratic bezier segment */
#define NEMA_VG_PRIM_BEZIER_CUBIC       (0x06U) /**< Cubic bezier segment */
#define NEMA_VG_PRIM_BEZIER_SQUAD       (0x07U) /**< Smooth quadratic bezier segment */
#define NEMA_VG_PRIM_BEZIER_SCUBIC      (0x08U) /**< Smooth cubic bezier segment */
#define NEMA_VG_PRIM_ARC                (0x09U) /**< Arc segment */
#define NEMA_VG_PRIM_POLYGON            (0x0AU) /**< Polygon segment */
#define NEMA_VG_PRIM_POLYLINE           (0x0BU) /**< Polyline segment */
#define NEMA_VG_PRIM_MASK               (0x0FU) /**< Mask for all segments */
#define NEMA_VG_REL                     (0x10U) /**< Rel segment*/
#define NEMA_VG_ARC_LARGE               (0x20U) /**< Large arc segment */
#define NEMA_VG_ARC_CW                  (0x40U) /**< Clockwise arc segment */
#define NEMA_VG_PRIM_SCCWARC            (NEMA_VG_PRIM_ARC   ) /**< Small counterclockwise arc segment */
#define NEMA_VG_PRIM_SCWARC             (NEMA_VG_PRIM_ARC   | NEMA_VG_ARC_CW ) /**< Small clockwise arc segment */
#define NEMA_VG_PRIM_LCCWARC            (NEMA_VG_PRIM_ARC   | NEMA_VG_ARC_LARGE) /**< Large counterclockwise arc segment */
#define NEMA_VG_PRIM_LCWARC             (NEMA_VG_PRIM_ARC   | NEMA_VG_ARC_CW |NEMA_VG_ARC_LARGE) /**< Large clockwise arc segment */
#define NEMA_VG_PRIM_MOVE_REL           (NEMA_VG_PRIM_MOVE  | NEMA_VG_REL) /**< Relative move segment */
#define NEMA_VG_PRIM_LINE_REL           (NEMA_VG_PRIM_LINE  | NEMA_VG_REL) /**< Relative line segment */
#define NEMA_VG_PRIM_HLINE_REL          (NEMA_VG_PRIM_HLINE | NEMA_VG_REL) /**< Relative horizontal line segment */
#define NEMA_VG_PRIM_VLINE_REL          (NEMA_VG_PRIM_VLINE | NEMA_VG_REL) /**< Relative vertical line segment */
#define NEMA_VG_PRIM_BEZIER_QUAD_REL    (NEMA_VG_PRIM_BEZIER_QUAD   | NEMA_VG_REL) /**< Relative quadratic bezier segment */
#define NEMA_VG_PRIM_BEZIER_CUBIC_REL   (NEMA_VG_PRIM_BEZIER_CUBIC  | NEMA_VG_REL) /**< Relative cubic bezier segment */
#define NEMA_VG_PRIM_BEZIER_SQUAD_REL   (NEMA_VG_PRIM_BEZIER_SQUAD  | NEMA_VG_REL) /**< Relative smooth quadratic bezier segment */
#define NEMA_VG_PRIM_BEZIER_SCUBIC_REL  (NEMA_VG_PRIM_BEZIER_SCUBIC | NEMA_VG_REL) /**< Relative smooth cubic bezier segment */
#define NEMA_VG_PRIM_SCCWARC_REL        (NEMA_VG_PRIM_SCCWARC | NEMA_VG_REL) /**< Relative small counterclockwise arc segment */
#define NEMA_VG_PRIM_SCWARC_REL         (NEMA_VG_PRIM_SCWARC  | NEMA_VG_REL) /**< Relative small clockwise arc segment */
#define NEMA_VG_PRIM_LCCWARC_REL        (NEMA_VG_PRIM_LCCWARC | NEMA_VG_REL) /**< Relative lareg counterclockwise arc segment */
#define NEMA_VG_PRIM_LCWARC_REL         (NEMA_VG_PRIM_LCWARC  | NEMA_VG_REL) /**< Relative lareg rclockwise arc segment */
#define NEMA_VG_PRIM_POLYGON_REL        (NEMA_VG_PRIM_POLYGON | NEMA_VG_REL) /**< Relative polygon segment */
#define NEMA_VG_PRIM_POLYLINE_REL       (NEMA_VG_PRIM_POLYLINE | NEMA_VG_REL) /**< Relative polyline segment */


/** \brief Создать путь
 *
 * \return Созданный путь
 *
 */
NEMA_VG_PATH_HANDLE nema_vg_path_create();

/** \brief Уничтожить путь
 *
 * \param path Указатель пути
 * \return void
 *
 */
void nema_vg_path_destroy(NEMA_VG_PATH_HANDLE path);

/** \brief Очистить путь
 *
 * \param path Указатель пути
 * \return void
 *
 */
void nema_vg_path_clear(NEMA_VG_PATH_HANDLE path);

/** \brief Установить форму пути (буфер вершины)
 *
 * \param путь Указатель пути
 * \paramseg_sizeКоличество дополненных сегментов
 * \param seg Указатель сегментов
 * \paramdata_sizeКоличество дополненных данных
 * \param data Указатель координат
 *
 */
void nema_vg_path_set_shape(NEMA_VG_PATH_HANDLE path, const size_t seg_size , const uint8_t* seg, const size_t data_size, const nema_vg_float_t* data);

/** \brief Установите форму пути (буфер вершины) и ограничительную рамку. Та же функция, что и уnema_vg_path_set_shape()
 *         но bbox задается пользователем (уменьшает использованиеCPU)
 *
 * \param путь Указатель пути
 * \paramseg_sizeКоличество дополненных сегментов
 * \param seg Указатель сегментов
 * \paramdata_sizeКоличество дополненных данных
 * \param data Указатель координат
 * \param bbox Указатель координат связанной валюты формы {min_x,min_y,max_x,max_y}
 *
 */
void
nema_vg_path_set_shape_and_bbox(NEMA_VG_PATH_HANDLE path, const size_t seg_size, const uint8_t* seg, const size_t data_size, const nema_vg_float_t* data, const nema_vg_float_t *bbox);

/** \brief Установка матрицы аффинного преобразования
 *
 * \param путь Указатель пути
 * \param m 3x3 матрица аффинного преобразования
 *
 */
void nema_vg_path_set_matrix(NEMA_VG_PATH_HANDLE path, nema_matrix3x3_t m);

#ifdef __cplusplus
}
#endif

#endif //__NEMA_VG_PATH_H__
