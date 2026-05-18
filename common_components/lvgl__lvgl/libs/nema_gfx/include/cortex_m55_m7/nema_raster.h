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


#ifndef NEMA_RASTER_H__
#define NEMA_RASTER_H__

#ifdef __cplusplus
extern "C" {
#endif

/** \private */
void nema_set_raster_color(uint32_t rgba8888);

/** \private */
void nema_raster_pixel(int x, int y);

/** \private */
void nema_raster_line(int x0, int y0, int x1, int y1);

/** \private */
void nema_raster_triangle_fx(int x0fx, int y0fx, int x1fx, int y1fx, int x2fx, int y2fx);


/** \private */
void nema_raster_rect(int x, int y, int w, int h);

/** \private */
void nema_raster_rounded_rect(int x0, int y0, int w, int h, int r);


/** \private */
void nema_raster_quad_fx(int x0fx, int y0fx,
                      int x1fx, int y1fx, int x2fx, int y2fx, int x3fx, int y3fx);

/** \private */
void nema_raster_triangle (int x0, int y0, int x1, int y1, int x2, int y2);

/** \private */
void nema_raster_quad (int x0,int y0,int x1,int y1,int x2,int y2,int x3,int y3);

/** \private */
void nema_raster_circle(float x, float y, float r);

/** \private */
void nema_raster_circle_aa(float x, float y, float r);

/** \private */
void nema_raster_stroked_circle_aa(float x, float y, float r, float w);

/** \private */
void nema_raster_rect_fx(int xfx, int yfx, int wfx, int hfx);

/** \private */
void nema_raster_rect_f(float x, float y, float w, float h);

/** \private */
void
nema_raster_triangle_f(float x0, float y0, float x1, float y1, float x2, float y2);

/** \private */
void
nema_raster_triangle_p0_f(float x0, float y0);

/** \private */
void
nema_raster_triangle_p1_f(float x1, float y1);

/** \private */
void
nema_raster_triangle_p2_f(float x2, float y2);

/** \private */
void
nema_raster_quad_f(float x0, float y0, float x1, float y1,
                        float x2, float y2, float x3, float y3);

/** \private */
void
nema_raster_stroked_arc_aa( float x0, float y0, float r, float w, float start_angle, float end_angle);

/** \private */
// aa_mask: 
//   RAST_AA_E0: AA on first ending
//   RAST_AA_E2: AA on last ending
void
nema_raster_stroked_arc_aa_mask( float x0, float y0, float r, float w, float start_angle, float end_angle, uint32_t aa_mask);

#ifdef __cplusplus
}
#endif

#endif //NEMA_RASTER_H__
