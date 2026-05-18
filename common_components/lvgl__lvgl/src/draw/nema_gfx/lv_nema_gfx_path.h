/**
 * Лицензия MIT
 *
 * -----------------------------------------------------------------------------
 * Copyright (c) 2008-24 Think Silicon Single Member PC
 * -----------------------------------------------------------------------------
 *
 * Разрешение настоящим предоставляется бесплатно любому лицу, получившему копию.
 * данного программного обеспечения и связанных с ним файлов документации («Программное обеспечение») для решения
 * в Программном обеспечении без ограничений, включая, помимо прочего, права на
 * использовать, копировать, изменять, объединять, публиковать, распространять, сублицензировать и/или продавать копии
 * Программное обеспечение и разрешать лицам, которым предоставлено Программное обеспечение, делать это,
 * при соблюдении следующих условий:
 *
 * Приведенное выше уведомление об авторских правах и данное уведомление о разрешении (включая следующий абзац)
 * должны быть включены во все копии или существенные части Программного обеспечения.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/**
 * @file lv_nema_gfx_path.h
 *
 */

#ifndef LV_NEMA_GFX_PATH_H
#define LV_NEMA_GFX_PATH_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nema_gfx.h"

#if LV_USE_NEMA_GFX
#if LV_USE_NEMA_VG

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    NEMA_VG_PATH_HANDLE path;
    NEMA_VG_PAINT_HANDLE paint;
    float * data;
    uint8_t * seg;
    uint32_t data_size;
    uint32_t seg_size;
} lv_nema_gfx_path_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_nema_gfx_path_t * lv_nema_gfx_path_create(void);

void lv_nema_gfx_path_alloc(lv_nema_gfx_path_t * nema_gfx_path);

void lv_nema_gfx_path_destroy(lv_nema_gfx_path_t * nema_gfx_path);

void lv_nema_gfx_path_move_to(lv_nema_gfx_path_t * nema_gfx_path,
                              float x, float y);

void lv_nema_gfx_path_line_to(lv_nema_gfx_path_t * nema_gfx_path,
                              float x, float y);

void lv_nema_gfx_path_quad_to(lv_nema_gfx_path_t * nema_gfx_path,
                              float cx, float cy,
                              float x, float y);

void lv_nema_gfx_path_cubic_to(lv_nema_gfx_path_t * nema_gfx_path,
                               float cx1, float cy1,
                               float cx2, float cy2,
                               float x, float y);

void lv_nema_gfx_path_end(lv_nema_gfx_path_t * nema_gfx_path);



#endif  /*LV_USE_NEMA_VG*/
#endif  /*LV_USE_NEMA_GFX*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_NEMA_GFX_PATH_H*/
