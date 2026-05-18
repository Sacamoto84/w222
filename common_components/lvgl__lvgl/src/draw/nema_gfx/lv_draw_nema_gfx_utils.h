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
 * @file lv_draw_nema_gfx_utils.h
 *
 */

#ifndef LV_DRAW_NEMA_GFX_UTILS_H
#define LV_DRAW_NEMA_GFX_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_NEMA_GFX
#include "../sw/lv_draw_sw.h"

#if LV_USE_NEMA_LIB == LV_NEMA_LIB_NONE
#warning since v9.5, LV_USE_NEMA_LIB should be specified in lv_conf.h. LV_NEMA_LIB_M33_REVC will be used by default.
#endif

#if LV_USE_NEMA_LIB == LV_NEMA_LIB_NONE || LV_USE_NEMA_LIB == LV_NEMA_LIB_M33_REVC || LV_USE_NEMA_LIB == LV_NEMA_LIB_M33_NEMAPVG
#include "../../../libs/nema_gfx/include/cortex_m33/build_version.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_blender.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_cmdlist.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_core.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_easing.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_error.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_font.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_graphics.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_hal.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_interpolators.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_math.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_matrix3x3.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_matrix4x4.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_provisional.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_raster.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_sys_defs.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_transitions.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_utils.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_version.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_vg_context.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_vg_font.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_vg.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_vg_paint.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_vg_path.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_vg_tsvg.h"
#include "../../../libs/nema_gfx/include/cortex_m33/nema_vg_version.h"
#include "../../../libs/nema_gfx/include/cortex_m33/tsi_malloc.h"
#elif LV_USE_NEMA_LIB == LV_NEMA_LIB_M55 || LV_USE_NEMA_LIB == LV_NEMA_LIB_M7
#include "../../../libs/nema_gfx/include/cortex_m55_m7/build_version.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_blender.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_cmdlist.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_core.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_easing.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_error.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_font.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_graphics.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_hal.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_interpolators.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_math.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_matrix3x3.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_matrix4x4.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_provisional.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_raster.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_sys_defs.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_transitions.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_version.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_vg_context.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_vg_font.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_vg.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_vg_paint.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_vg_path.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_vg_tsvg.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/nema_vg_version.h"
#include "../../../libs/nema_gfx/include/cortex_m55_m7/tsi_malloc.h"
#endif

/*********************
 *      DEFINES
 *********************/

#ifndef NEMA_VIRT2PHYS
#define NEMA_VIRT2PHYS
#else
uintptr_t NEMA_VIRT2PHYS(void * addr);
#endif

/*Глубина цвета: 1 (1 байт на пиксель), 8 ( RGB332 ), 16 ( RGB565 ), 32 ( ARGB8888 )*/
#if LV_COLOR_DEPTH == 8
#define LV_NEMA_GFX_COLOR_FORMAT NEMA_L8
#define LV_NEMA_GFX_FORMAT_MULTIPLIER 1
#elif LV_COLOR_DEPTH == 16
#define LV_NEMA_GFX_COLOR_FORMAT NEMA_RGB565
#define LV_NEMA_GFX_FORMAT_MULTIPLIER 2
#elif LV_COLOR_DEPTH == 24
#define LV_NEMA_GFX_COLOR_FORMAT NEMA_BGR24
#define LV_NEMA_GFX_FORMAT_MULTIPLIER 3
#elif LV_COLOR_DEPTH == 32
#define LV_NEMA_GFX_COLOR_FORMAT NEMA_BGRA8888
#define LV_NEMA_GFX_FORMAT_MULTIPLIER 4
#else
/*Невозможно использовать GPU с другими форматами.*/
#error Selected Color Depth Not Supported
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Проверьте, поддерживается ли `lv_color_format_t`.
 * @param     cf  The LVGL color format
 * @return        True/false
 */
bool lv_nemagfx_is_cf_supported(lv_color_format_t cf);

/**
 * Преобразуйте `lv_color_format_t` в цветовой формат Nema.
 * @param     cf  The LVGL color format
 * @return        The Nema color format
 */
uint32_t lv_nemagfx_cf_to_nema(lv_color_format_t cf);

/**
 * Получите режим наложения NemaGFX.
 *
 * @param [in] lv_blend_mode Режим наложения LVGL
 *
 * @return NemaGFX blending mode
 *
 */
uint32_t lv_nemagfx_blending_mode(lv_blend_mode_t lv_blend_mode);


/**
 * Получите режим наложения NemaGFX.
 *
 * @param [дюйм] градиент Градиентный буфер NemaGFX
 *
 * @param [in] lv_grad Дескриптор градиента
 *
 * @param [in] opa Непрозрачность дескриптора
 *
*/
void lv_nemagfx_grad_set(NEMA_VG_GRAD_HANDLE gradient, lv_grad_dsc_t lv_grad, lv_opa_t opa);

/**********************
 *      MACROS
 **********************/

#endif  /*LV_USE_NEMA_GFX*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_DRAW_NEMA_GFX_UTILS_H*/
