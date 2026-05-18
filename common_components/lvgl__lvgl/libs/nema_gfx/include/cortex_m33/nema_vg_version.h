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
 * @brief Содержит номера версий NemaVGAPIи поддерживаемую в данный момент версию шрифта.
 *
 */

#ifndef NEMA_VG_VERSION_H__
#define NEMA_VG_VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

#define NEMA_VG_MAJOR_VERSION    0x01U       /**< NemaVG API version, major number */
#define NEMA_VG_MINOR_VERSION    0x01U       /**< NemaVG API version, minor number */
#define NEMA_VG_REVISION_VERSION 0x07U       /**< NemaVG API version, revision number */
#define NEMA_VG_IMP_VERSION      0x00240600U /**< NemaVG API version, implementation in format 0x00YYMM00 (Y: year, M: month) */

#define NEMA_VG_API_VERSION ((NEMA_VG_MAJOR_VERSION << 16) + (NEMA_VG_MINOR_VERSION << 8) + (NEMA_VG_REVISION_VERSION)) /**< NemaVG API version in format 0x00MMmmrr (M:major, m:minor, r:revision if any) */

#define NEMA_VG_FONT_VERSION     0x01U       /**< Current font version */


#ifdef __cplusplus
}
#endif

#endif //NEMA_VG_VERSION_H__
