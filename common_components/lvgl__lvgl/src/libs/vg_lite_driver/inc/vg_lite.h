/****************************************************************************
*
*    Copyright 2012 - 2023 Vivante Corporation, Santa Clara, California.
*    Все права защищены.
*
*    Разрешение настоящим предоставляется бесплатно любому лицу, получившему
*    копию этого программного обеспечения и связанных с ним файлов документации (файл
*    «Программное обеспечение»), иметь дело с Программным обеспечением без ограничений, включая
*    без ограничений права на использование, копирование, изменение, объединение, публикацию,
*    распространять, сублицензировать и/или продавать копии Программного обеспечения, а также
*    разрешать лицам, которым предоставлено Программное обеспечение, делать это при условии, что
*    на следующие условия:
*
*    Вышеупомянутое уведомление об авторских правах и данное уведомление о разрешении (включая
*    следующий абзац) должны быть включены во все копии или существенные
*    части Программного обеспечения.
*
*    THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
*    IN NO EVENT SHALL VIVANTE AND/OR ITS SUPPLIERS BE LIABLE FOR ANY
*    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
*    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
*    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*****************************************************************************/

#ifndef VG_LITE_H
#define VG_LITE_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#define inline __inline
#endif

#include "../../../lv_conf_internal.h"
#if LV_USE_VG_LITE_DRIVER

#include <stddef.h>
#include <stdint.h>

/*  Константы VGLiteAPI***********************************************************************************************************/

#define VGLITE_HEADER_VERSION       7

#ifndef VGLITE_VERSION_3_0
#define VGLITE_VERSION_3_0          1

#define VGLITE_MAKE_VERSION(major, minor, patch) (((major) << 16) | ((minor) << 8) | (patch))
#define VGLITE_VERSION_MAJOR(version) (((uint32_t)(version) >> 16) & 0xff)
#define VGLITE_VERSION_MINOR(version) (((uint32_t)(version) >> 8) & 0xff)
#define VGLITE_VERSION_PATCH(version) ((uint32_t)(version) & 0xff)

#define VGLITE_API_VERSION_3_0      VGLITE_MAKE_VERSION(3, 0, 0)

#define VGLITE_RELEASE_VERSION      VGLITE_MAKE_VERSION(4,0,90)

#define VGL_FALSE                   0
#define VGL_TRUE                    1

/* Команда пути (код операции). */
#define VLC_OP_END                  0x00
#define VLC_OP_CLOSE                0x01
#define VLC_OP_MOVE                 0x02
#define VLC_OP_MOVE_REL             0x03
#define VLC_OP_LINE                 0x04
#define VLC_OP_LINE_REL             0x05
#define VLC_OP_QUAD                 0x06
#define VLC_OP_QUAD_REL             0x07
#define VLC_OP_CUBIC                0x08
#define VLC_OP_CUBIC_REL            0x09
#define VLC_OP_BREAK                0x0A
#define VLC_OP_HLINE                0x0B
#define VLC_OP_HLINE_REL            0x0C
#define VLC_OP_VLINE                0x0D
#define VLC_OP_VLINE_REL            0x0E
#define VLC_OP_SQUAD                0x0F
#define VLC_OP_SQUAD_REL            0x10
#define VLC_OP_SCUBIC               0x11
#define VLC_OP_SCUBIC_REL           0x12
#define VLC_OP_SCCWARC              0x13
#define VLC_OP_SCCWARC_REL          0x14
#define VLC_OP_SCWARC               0x15
#define VLC_OP_SCWARC_REL           0x16
#define VLC_OP_LCCWARC              0x17
#define VLC_OP_LCCWARC_REL          0x18
#define VLC_OP_LCWARC               0x19
#define VLC_OP_LCWARC_REL           0x1A

/* Макросы для управления путями: см. определения путей. */
#define VLM_PATH_ENABLE_UPLOAD(path)    (path).uploaded.property |= 1
#define VLM_PATH_DISABLE_UPLOAD(path)   (path).uploaded.property &= (~1)
#define VLM_PATH_GET_UPLOAD_BIT(path)   ((path).uploaded.property & 1)
#define VLM_PATH_STROKE_ENABLE_UPLOAD(path)    (path).stroke->uploaded.property |= 1
#define VLM_PATH_STROKE_DISABLE_UPLOAD(path)   (path).stroke->uploaded.property &= (~1)
#define VLM_PATH_STROKE_GET_UPLOAD_BIT(path)   ((path).stroke->uploaded.property & 1)

/* Градиентные константы. */
#define VLC_MAX_COLOR_RAMP_STOPS    256             /*! The max number of radial gradient stops. */
#define VLC_MAX_GRADIENT_STOPS      16              /*! The max number of gradient stops. */
#define VLC_GRADIENT_BUFFER_WIDTH   1024            /*! The internal gradient buffer width.*/


/* ИмяAPIустанавливает соответствие сAPIVGLite 2.0. */
#define vg_lite_buffer_upload                   vg_lite_upload_buffer
#define vg_lite_path_append                     vg_lite_append_path
#define vg_lite_path_calc_length                vg_lite_get_path_length
#define vg_lite_set_ts_buffer                   vg_lite_set_tess_buffer
#define vg_lite_set_draw_path_type              vg_lite_set_path_type
#define vg_lite_create_mask_layer               vg_lite_create_masklayer
#define vg_lite_fill_mask_layer                 vg_lite_fill_masklayer
#define vg_lite_blend_mask_layer                vg_lite_blend_masklayer
#define vg_lite_generate_mask_layer_by_path     vg_lite_render_masklayer
#define vg_lite_set_mask_layer                  vg_lite_set_masklayer
#define vg_lite_destroy_mask_layer              vg_lite_destroy_masklayer
#define vg_lite_enable_mask                     vg_lite_enable_masklayer
#define vg_lite_enable_color_transformation     vg_lite_enable_color_transform
#define vg_lite_set_color_transformation        vg_lite_set_color_transform
#define vg_lite_set_image_global_alpha          vg_lite_source_global_alpha
#define vg_lite_set_dest_global_alpha           vg_lite_dest_global_alpha
#define vg_lite_clear_rad_grad                  vg_lite_clear_radial_grad
#define vg_lite_update_rad_grad                 vg_lite_update_radial_grad
#define vg_lite_get_rad_grad_matrix             vg_lite_get_radial_grad_matrix
#define vg_lite_set_rad_grad                    vg_lite_set_radial_grad
#define vg_lite_draw_linear_gradient            vg_lite_draw_linear_grad
#define vg_lite_draw_radial_gradient            vg_lite_draw_radial_grad
#define vg_lite_draw_gradient                   vg_lite_draw_grad
#define vg_lite_mem_avail                       vg_lite_get_mem_size
#define vg_lite_set_update_stroke               vg_lite_update_stroke

#define vg_lite_buffer_image_mode_t             vg_lite_image_mode_t
#define vg_lite_draw_path_type_t                vg_lite_path_type_t
#define vg_lite_linear_gradient_ext_t           vg_lite_ext_linear_gradient_t
#define vg_lite_buffer_transparency_mode_t      vg_lite_transparency_t

/* VG_LITE_BLEND_PREMULTIPLY_SRC_OVER — то же самое, что VG_LITE_BLEND_NORMAL_LVGL. */
#define VG_LITE_BLEND_PREMULTIPLY_SRC_OVER      VG_LITE_BLEND_NORMAL_LVGL

/* Типы VGLiteAPI*****************************************************************************************************************/

typedef unsigned char       vg_lite_uint8_t;
typedef char                vg_lite_int8_t;
typedef short               vg_lite_int16_t;
typedef unsigned short      vg_lite_uint16_t;
typedef int                 vg_lite_int32_t;
typedef unsigned int        vg_lite_uint32_t;
typedef unsigned long long  vg_lite_uint64_t;
typedef float               vg_lite_float_t;
typedef double              vg_lite_double_t;
typedef char                vg_lite_char;
typedef char        *       vg_lite_string;
typedef void        *       vg_lite_pointer;
typedef void                vg_lite_void;
typedef unsigned int        vg_lite_color_t;


/* Перечисления VGLiteAPI***********************************************************************************************************/

#ifndef VG_LITE_ERROR
#define VG_LITE_ERROR  1

/* Коды ошибок, которые могут вернуть функцию vg_lite. */
typedef enum vg_lite_error {
    VG_LITE_SUCCESS = 0,            /*! Success. */
    VG_LITE_INVALID_ARGUMENT,       /*! An invalid argument was specified. */
    VG_LITE_OUT_OF_MEMORY,          /*! Out of GPU memory. */
    VG_LITE_NO_CONTEXT,             /*! No context or an unintialized context specified. */
    VG_LITE_TIMEOUT,                /*! A timeout has occurred during a wait. */
    VG_LITE_OUT_OF_RESOURCES,       /*! Out of system resources. */
    VG_LITE_GENERIC_IO,             /*! Cannot communicate with the kernel driver. */
    VG_LITE_NOT_SUPPORT,            /*! Function call not supported. */
    VG_LITE_ALREADY_EXISTS,         /*! Object already exists */
    VG_LITE_NOT_ALIGNED,            /*! Data alignment error */
    VG_LITE_FLEXA_TIME_OUT,         /*! VG timeout requesting for segment buffer */
    VG_LITE_FLEXA_HANDSHAKE_FAIL,   /*! VG and SBI synchronizer handshake failed */
} vg_lite_error_t;
#endif

/* Чип имеет немного функций */
typedef enum vg_lite_feature {
    gcFEATURE_BIT_VG_IM_INDEX_FORMAT,
    gcFEATURE_BIT_VG_SCISSOR,
    gcFEATURE_BIT_VG_BORDER_CULLING,
    gcFEATURE_BIT_VG_RGBA2_FORMAT,
    gcFEATURE_BIT_VG_QUALITY_8X,
    gcFEATURE_BIT_VG_IM_FASTCLAER,
    gcFEATURE_BIT_VG_RADIAL_GRADIENT,
    gcFEATURE_BIT_VG_GLOBAL_ALPHA,
    gcFEATURE_BIT_VG_RGBA8_ETC2_EAC,
    gcFEATURE_BIT_VG_COLOR_KEY,
    gcFEATURE_BIT_VG_DOUBLE_IMAGE,
    gcFEATURE_BIT_VG_YUV_OUTPUT,
    gcFEATURE_BIT_VG_FLEXA,
    gcFEATURE_BIT_VG_24BIT,
    gcFEATURE_BIT_VG_DITHER,
    gcFEATURE_BIT_VG_USE_DST,
    gcFEATURE_BIT_VG_PE_CLEAR,
    gcFEATURE_BIT_VG_IM_INPUT,
    gcFEATURE_BIT_VG_DEC_COMPRESS,
    gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT,
    gcFEATURE_BIT_VG_MASK,
    gcFEATURE_BIT_VG_MIRROR,
    gcFEATURE_BIT_VG_GAMMA,
    gcFEATURE_BIT_VG_NEW_BLEND_MODE,
    gcFEATURE_BIT_VG_STENCIL,
    gcFEATURE_BIT_VG_SRC_PREMULTIPLIED, /*! Valid only if gcFEATURE_BIT_VG_HW_PREMULTIPLY is 0   */
    gcFEATURE_BIT_VG_HW_PREMULTIPLY,    /*! HW multiplier can accept either premultiplied or not */
    gcFEATURE_BIT_VG_COLOR_TRANSFORMATION,
    gcFEATURE_BIT_VG_LVGL_SUPPORT,
    gcFEATURE_BIT_VG_INDEX_ENDIAN,
    gcFEATURE_BIT_VG_24BIT_PLANAR,
    gcFEATURE_BIT_VG_PIXEL_MATRIX,
    gcFEATURE_BIT_VG_NEW_IMAGE_INDEX,
    gcFEATURE_BIT_VG_PARALLEL_PATHS,
    gcFEATURE_BIT_VG_STRIPE_MODE,
    gcFEATURE_BIT_VG_IM_DEC_INPUT,
    gcFEATURE_BIT_VG_GAUSSIAN_BLUR,
    gcFEATURE_BIT_VG_RECTANGLE_TILED_OUT,
    gcFEATURE_BIT_VG_TESSELLATION_TILED_OUT,
    gcFEATURE_BIT_VG_IM_REPEAT_REFLECT,
    gcFEATURE_BIT_VG_YUY2_INPUT,
    gcFEATURE_BIT_VG_YUV_INPUT,
    gcFEATURE_BIT_VG_YUV_TILED_INPUT,
    gcFEATURE_BIT_VG_AYUV_INPUT,
    gcFEATURE_BIT_VG_16PIXELS_ALIGN,
    gcFEATURE_BIT_VG_DEC_COMPRESS_2_0,
    gcFEATURE_BIT_VG_NV24_INPUT,
    gcFEATURE_BIT_VG_TILED_LIMIT,
    gcFEATURE_BIT_TILED_MODE,
    gcFEATURE_BIT_VG_SRC_ADDRESS_16BYTES_ALIGNED,
    gcFEATURE_BIT_VG_SRC_ADDRESS_64BYTES_ALIGNED,
    gcFEATURE_BIT_VG_SRC_TILE_4PIXELS_ALIGNED,
    gcFEATURE_BIT_VG_SRC_BUF_ALINGED,
    gcFEATURE_BIT_VG_DST_ADDRESS_64BYTES_ALIGNED,
    gcFEATURE_BIT_VG_DST_TILE_4PIXELS_ALIGNED,
    gcFEATURE_BIT_VG_DST_BUF_ALIGNED,
    gcFEATURE_BIT_VG_DST_24BIT_PLANAR_ALIGNED,
    gcFEATURE_BIT_VG_DST_BUFLEN_ALIGNED,
    gcFEATURE_BIT_VG_FORMAT_SUPPORT_CHECK,
    gcFEATURE_BIT_VG_YUV_ALIGNED_CHECK,
    gcFEATURE_BIT_VG_512_PARALLEL_PATHS,
    gcFEATURE_COUNT
} vg_lite_feature_t;

/* Качество рендеринга перечислений. */
typedef enum vg_lite_quality {
    VG_LITE_HIGH,   /*! High quality 16x anti-aliasing path. */
    VG_LITE_UPPER,  /*! Upper quality 8x anti-aliasing path. */
    VG_LITE_MEDIUM, /*! Medium quality 4x anti-aliasing path. */
    VG_LITE_LOW,    /*! Low quality path without any anti-aliasing. */
} vg_lite_quality_t;

/* Формат координат пути. */
typedef enum vg_lite_format {
    VG_LITE_S8,     /*! Signed 8-bit coordinates. */
    VG_LITE_S16,    /*! Signed 16-bit coordinates. */
    VG_LITE_S32,    /*! Signed 32-bit coordinates. */
    VG_LITE_FP32,   /*! 32-bit floating point coordinates. */
} vg_lite_format_t;

/* Формат пиксельного буфера. */
typedef enum vg_lite_buffer_format {
    /* Следующие перечисленияOPENVG_* Необходимо в соответствии с OpenVG.
     * Перечисления VGImageFormat, поэтому VGLiteAPIможет напрямую выполнять перечисление OpenVG VGImageFormat.
     *
     * Note: Биты каждого цветового канала хранятся в машинном слове.
     * от MSB до LSB в порядке, указанном названием формата пикселей.
     * Это противоположно форматам VG_LITE_* (от LSB до MSB).
     */

    /* RGB {A,X} порядок каналов */
    OPENVG_sRGBX_8888                               =  0,
    OPENVG_sRGBA_8888                               =  1,
    OPENVG_sRGBA_8888_PRE                           =  2,
    OPENVG_sRGB_565                                 =  3,
    OPENVG_sRGBA_5551                               =  4,
    OPENVG_sRGBA_4444                               =  5,
    OPENVG_sL_8                                     =  6,
    OPENVG_lRGBX_8888                               =  7,
    OPENVG_lRGBA_8888                               =  8,
    OPENVG_lRGBA_8888_PRE                           =  9,
    OPENVG_lL_8                                     = 10,
    OPENVG_A_8                                      = 11,
    OPENVG_BW_1                                     = 12,
    OPENVG_A_1                                      = 13,
    OPENVG_A_4                                      = 14,

    /* Следующие перечисления 15 ~ 25 не существуют в OpenVG VGImageFormat.
     * Им достаточно для внутренней поддержки формирования OpenVG CTS, который установлен
     * на основе "sRGB_NONPRE", "lRGB_NONPRE", "sRGB_PRE", "lRGB_PRE"
     * конфигурации поверхности назначения.
     */
    OPENVG_sRGBX_8888_PRE                           = 15,
    OPENVG_sRGB_565_PRE                             = 16,
    OPENVG_sRGBA_5551_PRE                           = 17,
    OPENVG_sRGBA_4444_PRE                           = 18,
    OPENVG_lRGBX_8888_PRE                           = 19,
    OPENVG_lRGB_565                                 = 20,
    OPENVG_lRGB_565_PRE                             = 21,
    OPENVG_lRGBA_5551                               = 22,
    OPENVG_lRGBA_5551_PRE                           = 23,
    OPENVG_lRGBA_4444                               = 24,
    OPENVG_lRGBA_4444_PRE                           = 25,

    /* {A,X} RGB порядок каналов */
    OPENVG_sXRGB_8888                               =  0 | (1 << 6),
    OPENVG_sARGB_8888                               =  1 | (1 << 6),
    OPENVG_sARGB_8888_PRE                           =  2 | (1 << 6),
    OPENVG_sARGB_1555                               =  4 | (1 << 6),
    OPENVG_sARGB_4444                               =  5 | (1 << 6),
    OPENVG_lXRGB_8888                               =  7 | (1 << 6),
    OPENVG_lARGB_8888                               =  8 | (1 << 6),
    OPENVG_lARGB_8888_PRE                           =  9 | (1 << 6),

    /* BGR {A,X} порядок каналов */
    OPENVG_sBGRX_8888                               =  0 | (1 << 7),
    OPENVG_sBGRA_8888                               =  1 | (1 << 7),
    OPENVG_sBGRA_8888_PRE                           =  2 | (1 << 7),
    OPENVG_sBGR_565                                 =  3 | (1 << 7),
    OPENVG_sBGRA_5551                               =  4 | (1 << 7),
    OPENVG_sBGRA_4444                               =  5 | (1 << 7),
    OPENVG_lBGRX_8888                               =  7 | (1 << 7),
    OPENVG_lBGRA_8888                               =  8 | (1 << 7),
    OPENVG_lBGRA_8888_PRE                           =  9 | (1 << 7),

    /* {A,X} BGR порядок каналов */
    OPENVG_sXBGR_8888                               =  0 | (1 << 6) | (1 << 7),
    OPENVG_sABGR_8888                               =  1 | (1 << 6) | (1 << 7),
    OPENVG_sABGR_8888_PRE                           =  2 | (1 << 6) | (1 << 7),
    OPENVG_sABGR_1555                               =  4 | (1 << 6) | (1 << 7),
    OPENVG_sABGR_4444                               =  5 | (1 << 6) | (1 << 7),
    OPENVG_lXBGR_8888                               =  7 | (1 << 6) | (1 << 7),
    OPENVG_lABGR_8888                               =  8 | (1 << 6) | (1 << 7),
    OPENVG_lABGR_8888_PRE                           =  9 | (1 << 6) | (1 << 7),

    /* Следующие перечисленияVG_LITE_* являются исходными перечислениями, образующими изображение VGLite API.
     *
     * Note: Биты каждого цветового канала хранятся в машинном слове.
     * от LSB до MSB в порядке, указанном названием формата пикселей.
     * Это противоположно форматам OPENVG VG_ * (от MSB до LSB).
     */
    VG_LITE_RGBA8888                        =  0 | (1 << 10),
    VG_LITE_BGRA8888                        =  1 | (1 << 10),
    VG_LITE_RGBX8888                        =  2 | (1 << 10),
    VG_LITE_BGRX8888                        =  3 | (1 << 10),
    VG_LITE_RGB565                          =  4 | (1 << 10),
    VG_LITE_BGR565                          =  5 | (1 << 10),
    VG_LITE_RGBA4444                        =  6 | (1 << 10),
    VG_LITE_BGRA4444                        =  7 | (1 << 10),
    VG_LITE_BGRA5551                        =  8 | (1 << 10),
    VG_LITE_A4                              =  9 | (1 << 10),
    VG_LITE_A8                              = 10 | (1 << 10),
    VG_LITE_L8                              = 11 | (1 << 10),
    VG_LITE_YUYV                            = 12 | (1 << 10),
    VG_LITE_YUY2                            = 13 | (1 << 10),
    VG_LITE_ANV12                           = 14 | (1 << 10),
    VG_LITE_AYUY2                           = 15 | (1 << 10),
    VG_LITE_NV12                            = 16 | (1 << 10),
    VG_LITE_YV12                            = 17 | (1 << 10),
    VG_LITE_YV24                            = 18 | (1 << 10),
    VG_LITE_YV16                            = 19 | (1 << 10),
    VG_LITE_NV16                            = 20 | (1 << 10),
    VG_LITE_YUY2_TILED                      = 21 | (1 << 10),
    VG_LITE_NV12_TILED                      = 22 | (1 << 10),
    VG_LITE_ANV12_TILED                     = 23 | (1 << 10),
    VG_LITE_AYUY2_TILED                     = 24 | (1 << 10),
    VG_LITE_RGBA2222                        = 25 | (1 << 10),
    VG_LITE_BGRA2222                        = 26 | (1 << 10),
    VG_LITE_ABGR2222                        = 27 | (1 << 10),
    VG_LITE_ARGB2222                        = 28 | (1 << 10),
    VG_LITE_ABGR4444                        = 29 | (1 << 10),
    VG_LITE_ARGB4444                        = 30 | (1 << 10),
    VG_LITE_ABGR8888                        = 31 | (1 << 10),
    VG_LITE_ARGB8888                        = 32 | (1 << 10),
    VG_LITE_ABGR1555                        = 33 | (1 << 10),
    VG_LITE_RGBA5551                        = 34 | (1 << 10),
    VG_LITE_ARGB1555                        = 35 | (1 << 10),
    VG_LITE_XBGR8888                        = 36 | (1 << 10),
    VG_LITE_XRGB8888                        = 37 | (1 << 10),
    VG_LITE_RGBA8888_ETC2_EAC               = 38 | (1 << 10),
    VG_LITE_RGB888                          = 39 | (1 << 10),
    VG_LITE_BGR888                          = 40 | (1 << 10),
    VG_LITE_ABGR8565                        = 41 | (1 << 10),
    VG_LITE_BGRA5658                        = 42 | (1 << 10),
    VG_LITE_ARGB8565                        = 43 | (1 << 10),
    VG_LITE_RGBA5658                        = 44 | (1 << 10),
    VG_LITE_ABGR8565_PLANAR                 = 45 | (1 << 10),
    VG_LITE_BGRA5658_PLANAR                 = 46 | (1 << 10),
    VG_LITE_ARGB8565_PLANAR                 = 47 | (1 << 10),
    VG_LITE_RGBA5658_PLANAR                 = 48 | (1 << 10),
    VG_LITE_NV24                            = 49 | (1 << 10),
    VG_LITE_NV24_TILED                      = 50 | (1 << 10),

    VG_LITE_INDEX_1 = 0 | (1 << 11),  /*! Indexed format. */
    VG_LITE_INDEX_2 = 1 | (1 << 11),
    VG_LITE_INDEX_4 = 2 | (1 << 11),
    VG_LITE_INDEX_8 = 3 | (1 << 11),

} vg_lite_buffer_format_t;

/* Набор упакованных каналов YUV формата UV. */
typedef enum vg_lite_swizzle {
    VG_LITE_SWIZZLE_UV,
    VG_LITE_SWIZZLE_VU,
} vg_lite_swizzle_t;

/* Правило преобразования YUV <-> RGB. */
typedef enum vg_lite_yuv2rgb {
    VG_LITE_YUV601,
    VG_LITE_YUV709,
} vg_lite_yuv2rgb_t;

/* Расположение пикселей в буфере. */
typedef enum vg_lite_buffer_layout {
    VG_LITE_LINEAR,
    VG_LITE_TILED,
} vg_lite_buffer_layout_t;

/* Режим рендеринга изображения (буфера). Соответствие перечислению OpenVG VGImageMode */
typedef enum vg_lite_image_mode {
    /* Для обратной совместимости значений перечисления */
    VG_LITE_ZERO                            = 0,
    VG_LITE_NORMAL_IMAGE_MODE               = 0x1F00,
    VG_LITE_MULTIPLY_IMAGE_MODE             = 0x1F01,
    VG_LITE_STENCIL_MODE                    = 0x1F02,
    VG_LITE_NONE_IMAGE_MODE                 = 0x1F03,
    VG_LITE_RECOLOR_MODE                    = 0x1F04,
} vg_lite_image_mode_t;

/* Режим прозрачности изображения (буфера). */
typedef enum vg_lite_transparency {
    VG_LITE_IMAGE_OPAQUE,
    VG_LITE_IMAGE_TRANSPARENT
} vg_lite_transparency_t;

/* Режимы переключения.  OPENVG_BLEND_* следует перечислению OpenVG VGBlendMode.
 * S и D представляют собой каналы цвета RGB без предварительного умножения источника и назначения.
 * Sa и Da представляют собой альфа-каналы источника и назначения.
 * SP и DP представляют собой цветовые каналы RGB с предварительным альфа-умножением источника и назначения (S*Sa, D*Da).
 */
typedef enum vg_lite_blend {
    /* Режимы смешивания без предварительного умножения!*/
    VG_LITE_BLEND_NONE                      = 0,        /*!  RGB: S, No blend                                !*/
    /*!  A:   Sa                                         !*/
    VG_LITE_BLEND_SRC_OVER                  = 1,        /*!  RGB: S + D*(1 - Sa)                             !*/
    /*!  A:   Sa + Da*(1 - Sa)                           !*/
    VG_LITE_BLEND_DST_OVER                  = 2,        /*!  RGB: S*(1 - Da) + D                             !*/
    /*!  A:   Sa*(1 - Da) + Da                           !*/
    VG_LITE_BLEND_SRC_IN                    = 3,        /*!  RGB: S*Da                                       !*/
    /*!  A:   Sa*Da                                      !*/
    VG_LITE_BLEND_DST_IN                    = 4,        /*!  RGB: D*Sa                                       !*/
    /*!  A:   Da*Sa                                      !*/
    VG_LITE_BLEND_MULTIPLY                  = 5,        /*!  RGB: S*(1 - Da) + D*(1 - Sa) + S*D              !*/
    /*!  A:   Sa*(1 - Da) + Da*(1 - Sa) + Sa*Da          !*/
    VG_LITE_BLEND_SCREEN                    = 6,        /*!  RGB: S + D - S*D                                !*/
    /*!  A:   Sa + Da - Sa*Da                            !*/
    VG_LITE_BLEND_DARKEN                    = 7,        /*!  RGB: min(SrcOver, DstOver)                      !*/
    /*!  A:   min(SrcOver, DstOver)                      !*/
    VG_LITE_BLEND_LIGHTEN                   = 8,        /*!  RGB: max(SrcOver, DstOver)                      !*/
    /*!  A:   max(SrcOver, DstOver)                      !*/
    VG_LITE_BLEND_ADDITIVE                  = 9,        /*!  RGB: S + D                                      !*/
    /*!  A:   Sa + Da                                    !*/
    VG_LITE_BLEND_SUBTRACT                  = 10,       /*!  RGB: D*(1 - Sa)                                 !*/
    /*!  A:   Da*(1 - Sa)                                !*/
    VG_LITE_BLEND_NORMAL_LVGL               = 11,       /*!  RGB: S*Sa + D*(1 - Sa)                          !*/
    /*!  A:   0xFF                                       !*/
    VG_LITE_BLEND_ADDITIVE_LVGL             = 12,       /*!  RGB: (S + D)*Sa + D*(1 - Sa)                    !*/
    /*!  A:   0xFF                                       !*/
    VG_LITE_BLEND_SUBTRACT_LVGL             = 13,       /*!  RGB: (S - D)*Sa + D*(1 - Sa)                    !*/
    /*!  A:   0xFF                                       !*/
    VG_LITE_BLEND_MULTIPLY_LVGL             = 14,       /*!  RGB: (S*D)*Sa + D*(1 - Sa)                      !*/
    /*!  A:   0xFF                                       !*/

    /* Режимы переключения с предварительным умножением Портера Даффа!*/
    OPENVG_BLEND_SRC                        = 0x2000,   /*!  RGB: SP / Sa,  No blend                                                    !*/
    /*!  A:   Sa                                                                    !*/
    OPENVG_BLEND_SRC_OVER                   = 0x2001,   /*!  RGB: (SP + DP*(1 - Sa)) / (Sa + Da*(1 - Sa))                               !*/
    /*!  A:   (Sa + Da*(1 - Sa))                                                    !*/
    OPENVG_BLEND_DST_OVER                   = 0x2002,   /*!  RGB: (SP*(1 - Da) + DP) / (Sa*(1 - Da) + Da)                               !*/
    /*!  A:   (Sa*(1 - Da) + Da)                                                    !*/
    OPENVG_BLEND_SRC_IN                     = 0x2003,   /*!  RGB: (SP*Da) / (Sa*Da)                                                     !*/
    /*!  A:   (Sa*Da)                                                               !*/
    OPENVG_BLEND_DST_IN                     = 0x2004,   /*!  RGB: (DP*Sa) / (Sa*Da)                                                     !*/
    /*!  A:   (Sa*Da)                                                               !*/
    OPENVG_BLEND_MULTIPLY                   = 0x2005,   /*!  RGB: (SP*DP + SP*(1 - Da) + DP*(1 - Sa)) / (Sa + Da*(1 - Sa))              !*/
    /*!  A:   (Sa + Da*(1 - Sa))                                                    !*/
    OPENVG_BLEND_SCREEN                     = 0x2006,   /*!  RGB: (SP + DP - (SP*DP)) / (Sa + Da*(1 - Sa))                              !*/
    /*!  A:   (Sa + Da*(1 - Sa))                                                    !*/
    OPENVG_BLEND_DARKEN                     = 0x2007,   /*!  RGB: (min(SP*Da, DP*Sa) + SP*(1 - Da) + DP*(1 - Sa)) / (Sa + Da*(1 - Sa))  !*/
    /*!  A:   (Sa + Da*(1 - Sa))                                                    !*/
    OPENVG_BLEND_LIGHTEN                    = 0x2008,   /*!  RGB: (max(SP*Da, DP*Sa) + SP*(1 - Da) + DP*(1 - Sa)) / (Sa + Da*(1 - Sa))  !*/
    /*!  A:   (Sa + Da*(1 - Sa))                                                    !*/
    OPENVG_BLEND_ADDITIVE                   = 0x2009,   /*!  RGB: (SP + DP) / (Sa + Da)                                                 !*/
    /*!  A:   (Sa + Da)                                                             !*/
} vg_lite_blend_t;

/* Заполните правила. Соответствие перечислению OpenVG VGFillRule */
typedef enum vg_lite_fill {
    VG_LITE_FILL_EVEN_ODD                   = 0x1900,   /*! A pixel is drawn it it crosses an odd number of path pixels. */
    VG_LITE_FILL_NON_ZERO                   = 0x1901,   /*! A pixel is drawn if it crosses at least one path pixel. */
} vg_lite_fill_t;

/* Глобальные альфа-режимы. */
typedef enum vg_lite_global_alpha {
    VG_LITE_NORMAL = 0,                     /*! Use original src/dst alpha value. */
    VG_LITE_GLOBAL,                         /*! Use global src/dst alpha value to replace original src/dst alpha value. */
    VG_LITE_SCALED,                         /*! Multiply global src/dst alpha value and original src/dst alpha value. */
} vg_lite_global_alpha_t;

/* Режимы фильтров. */
typedef enum vg_lite_filter {
    VG_LITE_FILTER_POINT     = 0,           /*! Fetch the nearest image pixel. */
    VG_LITE_FILTER_LINEAR    = 0x1000,      /*! Used for linear paint. */
    VG_LITE_FILTER_BI_LINEAR = 0x2000,      /*! Use a 2x2 box around the image pixel and perform an interpolation. */
    VG_LITE_FILTER_GAUSSIAN  = 0x3000,      /*! Perform 3x3 gaussian blur with the convolution for image pixel. */
} vg_lite_filter_t;

/* Режим формы шаблона. Соответствует перечислению OpenVG VGTilingMode. */
typedef enum vg_lite_pattern_mode {
    VG_LITE_PATTERN_COLOR   = 0x1D00,       /*! Pixel outside the bounds of sourceimage should be taken as the color */
    VG_LITE_PATTERN_PAD     = 0x1D01,       /*! Pixel outside the bounds of sourceimage should be taken as having the same color as the closest edge pixel */
    VG_LITE_PATTERN_REPEAT  = 0x1D02,       /*! Pixel outside the bounds of sourceimage should be repeated indefinitely in all directions */
    VG_LITE_PATTERN_REFLECT = 0x1D03,       /*! Pixel outside the bounds of sourceimage should be reflected indefinitely in all directions */
} vg_lite_pattern_mode_t;

/* Тип краски. Соответствует перечислению OpenVG VGPaintType. */
typedef enum vg_lite_paint_type {
    /* Для обратной совместимости значений перечисления */
    VG_LITE_PAINT_ZERO            = 0,
    VG_LITE_PAINT_COLOR           = 0x1B00,
    VG_LITE_PAINT_LINEAR_GRADIENT = 0x1B01,
    VG_LITE_PAINT_RADIAL_GRADIENT = 0x1B02,
    VG_LITE_PAINT_PATTERN         = 0x1B03,
} vg_lite_paint_type_t;

/* Режим минимального радиального градиента. Соответствие перечислению OpenVG VGColorRampSpreadMode */
typedef enum {
    VG_LITE_GRADIENT_SPREAD_FILL     = 0,
    VG_LITE_GRADIENT_SPREAD_PAD      = 0x1C00,
    VG_LITE_GRADIENT_SPREAD_REPEAT   = 0x1C01,
    VG_LITE_GRADIENT_SPREAD_REFLECT  = 0x1C02,
} vg_lite_gradient_spreadmode_t;

/* Режим Decnano Compress. */
typedef enum vg_lite_compress_mode {
    VG_LITE_DEC_DISABLE = 0,                /*! disable compress */
    VG_LITE_DEC_NON_SAMPLE,                 /*! compress ratio is 1.6 if use ARGB8888, compress ratio is 2 if use XRGB8888 */
    VG_LITE_DEC_HSAMPLE,                    /*! compress ratio is 2 if use ARGB8888, compress ratio is 2.6 if use XRGB8888 */
    VG_LITE_DEC_HV_SAMPLE,                  /*! compress ratio is 2.6 if use ARGB8888, compress ratio is 4 if use XRGB8888 */
} vg_lite_compress_mode_t;

/* Нарисовать тип пути. Соответствие перечислению OpenVG VGPaintMode */
typedef enum vg_lite_path_type {
    /* Для обратной совместимости значений перечисления */
    VG_LITE_DRAW_ZERO                       = 0,
    VG_LITE_DRAW_STROKE_PATH                = (1 << 0),
    VG_LITE_DRAW_FILL_PATH                  = (1 << 1),
    VG_LITE_DRAW_FILL_STROKE_PATH           = (1 << 1 | 1 << 0),
} vg_lite_path_type_t;

/* Стиль торцевой крышки. Соответствие перечислению OpenVG VGCapStyle */
typedef enum vg_lite_cap_style {
    VG_LITE_CAP_BUTT                        = 0x1700,
    VG_LITE_CAP_ROUND                       = 0x1701,
    VG_LITE_CAP_SQUARE                      = 0x1702,
} vg_lite_cap_style_t;

/* Стили соединения линий. Соответствие перечислению OpenVG VGJoinStyle */
typedef enum vg_lite_join_style {
    VG_LITE_JOIN_MITER                      = 0x1800,
    VG_LITE_JOIN_ROUND                      = 0x1801,
    VG_LITE_JOIN_BEVEL                      = 0x1802,
} vg_lite_join_style_t;

/* Режим работы маски. Соответствие перечислению OpenVG VGMaskOperation */
typedef enum vg_lite_mask_operation {
    VG_LITE_CLEAR_MASK                      = 0x1500,   /*! Set all dest mask values to 0 */
    VG_LITE_FILL_MASK                       = 0x1501,   /*! Set all dest mask values to 1 */
    VG_LITE_SET_MASK                        = 0x1502,   /*! Copy from src masklayer to dest masklayer. */
    VG_LITE_UNION_MASK                      = 0x1503,   /*! Replace dest masklayer by its union with src masklayer. */
    VG_LITE_INTERSECT_MASK                  = 0x1504,   /*! Replace dest masklayer by its intersection with src masklayer. */
    VG_LITE_SUBTRACT_MASK                   = 0x1505,   /*! Subtract src mask in dest masklayer */
} vg_lite_mask_operation_t;

/* Режим зеркальной ориентации. */
typedef enum vg_lite_orientation {
    VG_LITE_ORIENTATION_TOP_BOTTOM,
    VG_LITE_ORIENTATION_BOTTOM_TOP,
} vg_lite_orientation_t;

/* Режим преобразования гаммы. */
typedef enum vg_lite_gamma_conversion {
    VG_LITE_GAMMA_NO_CONVERSION,            /*! Leave color as is. */
    VG_LITE_GAMMA_LINEAR,                   /*! Convert from sRGB to linear space. */
    VG_LITE_GAMMA_NON_LINEAR                /*! Convert from linear to sRGB space. */
} vg_lite_gamma_conversion_t;

/* Порядок байтов индекса */
typedef enum vg_lite_index_endian {
    VG_LITE_INDEX_LITTLE_ENDIAN,            /*! Разобрать индексный пиксель от низкого к высокому,
                                                 *! при использовании индекса1 порядок анализа — бит0~бит7.
                                                 *! при использовании index2 порядок анализа следующий: бит0:1,бит2:3,бит4:5.бит6:7.
                                                 *! при использовании порядка анализа index4 — бит0:3, бит4:7.
                                                 */
    VG_LITE_INDEX_BIG_ENDIAN,               /*! Разобрать индексный пиксель от низкого к высокому,
                                                 *! при использовании индекса1 порядок анализа — бит7~бит0.
                                                 *! при использовании index2 порядок анализа следующий: бит7:6,бит5:4,бит3:2.бит1:0.
                                                 *! при использовании порядка анализа index4 — бит4:7, бит0:3.
                                                 */
} vg_lite_index_endian_t;

/* Флаг карты*/
typedef enum vg_lite_map_flag {
    VG_LITE_MAP_USER_MEMORY             = 0,
    VG_LITE_MAP_DMABUF                  = 0x01,
} vg_lite_map_flag_t;

/*Переменные параметры VGLite*/
typedef enum vg_lite_param_type {
    VG_LITE_GPU_IDLE_STATE,                 /*! count must be 1 for GPU idle state TRUE or FALSE */
    VG_LITE_SCISSOR_RECT,                   /*! count must be 4n for x, y, right, bottom */
    VG_LITE_HARDWARE_RUNNING_TIME,          /*! count must be 1 */
} vg_lite_param_type_t;

/* Тип буфера Vg Lite */
typedef enum vg_lite_buffer_type {
    VG_LITE_COMMAND_BUFFER,
    VG_LITE_TESSELLATION_BUFFER,
    VG_LITE_RENDER_BUFFER,
} vg_lite_buffer_type_t;

/* Индекс резервной памяти */
typedef enum vg_lite_memory_pool {
    VG_LITE_MEMORY_POOL_1 = 0,
    VG_LITE_MEMORY_POOL_2 = 1,
} vg_lite_memory_pool_t;

typedef enum vg_lite_frame_flag {
    VG_LITE_FRAME_END_FLAG = 1,
} vg_lite_frame_flag_t;

/* Структуры VGLiteAPI**************************************************************************************************************/

/* Информация о драйвере VGLite */
typedef struct vg_lite_info {
    vg_lite_uint32_t api_version;
    vg_lite_uint32_t header_version;
    vg_lite_uint32_t release_version;
    vg_lite_uint32_t reserved;
} vg_lite_info_t;

/* Определение 2D-точки. */
typedef struct vg_lite_point {
    vg_lite_int32_t x;
    vg_lite_int32_t y;
} vg_lite_point_t;

/* Четыре 2D-точки, образующие многоугольник */
typedef vg_lite_point_t vg_lite_point4_t[4];

/* Определение 2D-точки с плавающей запятой. */
typedef struct vg_lite_float_point {
    vg_lite_float_t x;
    vg_lite_float_t y;
} vg_lite_float_point_t;

/* Четыре 2D-точки с плавающей запятой, образующие многоугольник. */
typedef vg_lite_float_point_t vg_lite_float_point4_t[4];

/* Прямоугольник.*/
typedef struct vg_lite_rectangle {
    vg_lite_int32_t x;                      /*! Left coordinate of rectangle. */
    vg_lite_int32_t y;                      /*! Top coordinate of rectangle. */
    vg_lite_int32_t width;                  /*! Width of rectangle. */
    vg_lite_int32_t height;                 /*! Height of rectangle. */
} vg_lite_rectangle_t;

typedef struct vg_lite_matrix {
    vg_lite_float_t m[3][3];                /*! The 3x3 matrix is in [row][column] order. */
    vg_lite_float_t scaleX;
    vg_lite_float_t scaleY;
    vg_lite_float_t angle;
} vg_lite_matrix_t;

typedef struct vg_lite_yuvinfo {
    vg_lite_swizzle_t swizzle;              /*! UV swizzle. */
    vg_lite_yuv2rgb_t yuv2rgb;              /*! 601 or 709 conversion standard. */
    vg_lite_uint32_t uv_planar;             /*! UV(U) planar address. */
    vg_lite_uint32_t v_planar;              /*! V planar address. */
    vg_lite_uint32_t alpha_planar;          /*! Alpha planar address. */
    vg_lite_uint32_t uv_stride;             /*! UV(U) stride. */
    vg_lite_uint32_t v_stride;              /*! V stride. */
    vg_lite_uint32_t alpha_stride;          /*! Alpha stride. */
    vg_lite_uint32_t uv_height;             /*! UV(U) height. */
    vg_lite_uint32_t v_height;              /*! V height. */
    vg_lite_pointer uv_memory;              /*! The logical pointer to the UV(U) planar memory. */
    vg_lite_pointer v_memory;               /*! The logical pointer to the V planar memory. */
    vg_lite_pointer uv_handle;              /*! The memory handle of the UV(U) planar. */
    vg_lite_pointer v_handle;               /*! The memory handle of the V planar. */
} vg_lite_yuvinfo_t;

typedef struct vg_lite_path_point * vg_lite_path_point_ptr;
typedef struct vg_lite_path_point {
    /* Координата Х. */
    vg_lite_float_t x;

    /* Координата Y. */
    vg_lite_float_t y;

    /* Флаг Flatten для сплющенного пути. */
    vg_lite_uint8_t flatten_flag;

    /* Тип кривой для траектории штриха. */
    vg_lite_uint8_t curve_type;

    /* касательная X. */
    vg_lite_float_t tangentX;

    /* Y касательная. */
    vg_lite_float_t tangentY;

    /* Длина линии. */
    vg_lite_float_t length;

    /* Указатель на следующий точечный узел. */
    vg_lite_path_point_ptr next;

    /* Указатель на предыдущий узел точки. */
    vg_lite_path_point_ptr prev;

} vg_lite_path_point_t;

typedef struct vg_lite_sub_path * vg_lite_sub_path_ptr;
typedef struct vg_lite_sub_path {
    /* Указатель на следующий подпуть. */
    vg_lite_sub_path_ptr next;

    /* Количество очков. */
    vg_lite_uint32_t point_count;

    /* Список точек. */
    vg_lite_path_point_ptr point_list;

    /* Последний пункт. */
    vg_lite_path_point_ptr end_point;

    /* То ли путь закрыт. */
    vg_lite_uint8_t closed;

    /* Длина подпути. */
    vg_lite_float_t length;

} vg_lite_sub_path_t;

/* Сохраните данные разделенного пути в соответствии с MOVE/MOVE_REL. */
typedef struct vg_lite_path_list * vg_lite_path_list_ptr;
typedef struct vg_lite_path_list {
    vg_lite_path_point_ptr              path_points;
    vg_lite_path_point_ptr              path_end;
    vg_lite_uint32_t                    point_count;
    vg_lite_path_list_ptr               next;
    vg_lite_uint8_t                     closed;

} vg_lite_path_list_t;

/* Информация о выделении памяти ядром. */
typedef struct vg_lite_hw_memory {
    vg_lite_pointer handle;                 /*! gpu memory object handle. */
    vg_lite_pointer memory;                 /*! logical memory address. */
    vg_lite_uint32_t address;               /*! GPU memory address. */
    vg_lite_uint32_t bytes;                 /*! Size of memory. */
    vg_lite_uint32_t property;              /*! В настоящее время бит 0 используется для состояния загрузки пути:
                                                 *!   1 : включить автоматическую загрузку данных пути.
                                                 *!   0 : отключить загрузку данных о пути. данные пути встроены в буфер команд. */
} vg_lite_hw_memory_t;

typedef struct vg_lite_stroke {
    /* Параметры хода */
    vg_lite_cap_style_t                 cap_style;
    vg_lite_join_style_t                join_style;
    vg_lite_float_t                     line_width;
    vg_lite_float_t                     miter_limit;
    vg_lite_float_t          *          dash_pattern;
    vg_lite_uint32_t                    pattern_count;
    vg_lite_float_t                     dash_phase;
    vg_lite_float_t                     dash_length;
    vg_lite_uint32_t                    dash_index;
    vg_lite_float_t                     half_width;

    /* Общая длина штриховых штрихов. */
    vg_lite_float_t                     pattern_length;

    /* Для быстрой проверки. */
    vg_lite_float_t                     miter_square;

    /* Временное проведение штриха subPath. */
    vg_lite_path_point_ptr              path_points;
    vg_lite_path_point_ptr              path_end;
    vg_lite_uint32_t                    point_count;
    vg_lite_path_point_ptr              left_point;
    vg_lite_path_point_ptr              right_point;
    vg_lite_path_point_ptr              stroke_points;
    vg_lite_path_point_ptr              stroke_end;
    vg_lite_uint32_t                    stroke_count;

    /* Разделите траекторию движения в соответствии с перемещением илиmove_rel, чтобы избежать неявного закрытия. */
    vg_lite_path_list_ptr               path_list_divide;

    /* указатель на текущие данные разделенного пути. */
    vg_lite_path_list_ptr               cur_list;

    /* Флаг, добавляющийend_pathв драйвер. */
    vg_lite_uint8_t                     add_end;
    vg_lite_uint8_t                     dash_reset;

    /* Список дополнительных путей. */
    vg_lite_sub_path_ptr                stroke_paths;

    /* Последний подпуть. */
    vg_lite_sub_path_ptr                last_stroke;

    /* Обработка подменной зоны. */
    vg_lite_uint32_t                    swing_handling;
    vg_lite_float_t                     swing_deltax;
    vg_lite_float_t                     swing_deltay;
    vg_lite_path_point_ptr              swing_start;
    vg_lite_path_point_ptr              swing_stroke;
    vg_lite_float_t                     swing_length;
    vg_lite_float_t                     swing_centlen;
    vg_lite_uint32_t                    swing_count;
    vg_lite_uint8_t                     need_swing;
    vg_lite_uint8_t                     swing_ccw;

    vg_lite_float_t                     stroke_length;
    vg_lite_uint32_t                    stroke_size;

    /* Линия штриха жирная. */
    vg_lite_uint8_t                     fattened;
    vg_lite_uint8_t                     closed;
    vg_lite_hw_memory_t                 uploaded;

} vg_lite_stroke_t;

/* Быстрая очистка буфера. */
typedef struct vg_lite_fc_buffer {
    vg_lite_int32_t width;                  /*! Width of the buffer in pixels. */
    vg_lite_int32_t height;                 /*! height of the buffer in pixels. */
    vg_lite_int32_t stride;                 /*! The number of bytes to move from one line in the buffer to the next line. */
    vg_lite_pointer
    handle;                 /*! The memory handle of the buffer's memory as allocated by the VGLite kernel. */
    vg_lite_pointer memory;                 /*! The logical pointer to the buffer's memory for the CPU. */
    vg_lite_uint32_t address;               /*! The address to the buffer's memory for the hardware. */
    vg_lite_uint32_t color;                 /*! The fastclear color value. */
} vg_lite_fc_buffer_t;

/* Структура для любого изображения или цели рендеринга. */
typedef struct vg_lite_buffer {
    vg_lite_int32_t width;                  /*! Width of the buffer in pixels. */
    vg_lite_int32_t height;                 /*! Height of the buffer in pixels. */
    vg_lite_int32_t stride;                 /*! The number of bytes to move from one line in the buffer to the next line. */
    vg_lite_buffer_layout_t tiled;          /*! Indicating the buffer memory layout is linear or tiled. */
    vg_lite_buffer_format_t format;         /*! The pixel format of the buffer. */
    vg_lite_pointer
    handle;                 /*! The memory handle of the buffer's memory as allocated by the VGLite kernel. */
    vg_lite_pointer memory;                 /*! The logical pointer to the buffer's memory for the CPU. */
    vg_lite_uint32_t address;               /*! The address to the buffer's memory for the hardware. */
    vg_lite_memory_pool_t pool;             /*! The buffer's memory pool. */
    vg_lite_yuvinfo_t yuv;                  /*! The yuv format details. */
    vg_lite_image_mode_t image_mode;        /*! The blit image mode. */
    vg_lite_transparency_t transparency_mode; /*! image transparency mode. */
    vg_lite_fc_buffer_t fc_buffer[3];       /*! 3 fastclear buffers,reserved YUV format. */
    vg_lite_compress_mode_t compress_mode;  /*! Refer to the definition by vg_lite_compress_mode_t. */
    vg_lite_index_endian_t index_endian;    /*! Refer to the definition by vg_lite_index_endian_t. */
    vg_lite_paint_type_t paintType;         /*! Get paintcolor from different paint types. */
    vg_lite_uint8_t fc_enable;              /*! enable im fastclear. */
    vg_lite_uint8_t scissor_buffer;         /*! The buffer is scissor mask buffer. */
    vg_lite_uint8_t premultiplied;          /*! The RGB pixel values are alpha-premultipled */
    vg_lite_uint8_t apply_premult;          /*! Need to apply alpha-premultiply */
    struct vg_lite_buffer * lvgl_buffer;    /*! Buffer for SW LVGL blending support */
    vg_lite_color_t bg_color;               /*! Background for edge filter */
} vg_lite_buffer_t;

/* Информация о пути для команды рисования. */
typedef struct vg_lite_path {
    vg_lite_float_t bounding_box[4];        /*! Bounding box specified as left, top, right, and bottom. */
    vg_lite_quality_t quality;              /*! Quality hint for the path. */
    vg_lite_format_t format;                /*! Coordinate format. */
    vg_lite_hw_memory_t uploaded;           /*! Path data that has been upload into GPU addressable memory. */
    vg_lite_uint32_t path_length;           /*! Number of bytes in the path data. */
    vg_lite_pointer path;                   /*! Pointer to the physical description of the path. */
    vg_lite_int8_t
    path_changed;            /*! Indicate whether path data is synced with command buffer (uploaded) or not. */
    vg_lite_int8_t pdata_internal;          /*! Indicate whether path data memory is allocated by driver. */
    vg_lite_path_type_t path_type;          /*! Refer to the definition by vg_lite_path_type_t. */
    vg_lite_stroke_t * stroke;              /*! Pointer to a vg_lite_stroke_t structure.*/
    vg_lite_pointer stroke_path;            /*! Pointer to the physical description of the stroke path. */
    vg_lite_uint32_t stroke_size;           /*! Number of bytes in the stroke path data. */
    vg_lite_color_t stroke_color;           /*! The stroke path fill color. */
    vg_lite_int8_t add_end;                 /*! Flag that add end_path in driver. */
    vg_lite_int8_t
    stroke_valid;            /*! Flag that judge whether current stroke data is come from current pathdata. */
} vg_lite_path_t;

/* Определение цветовой шкалы. */
typedef struct vg_lite_color_ramp {
    vg_lite_float_t stop;                   /*! Value for the color stop. */
    vg_lite_float_t red;                    /*! Red color channel value for the color stop. */
    vg_lite_float_t green;                  /*! Green color channel value for the color stop. */
    vg_lite_float_t blue;                   /*! Blue color channel value for the color stop. */
    vg_lite_float_t alpha;                  /*! Alpha color channel value for the color stop. */
} vg_lite_color_ramp_t;

/* Параметр линейного градиента */
typedef struct vg_lite_linear_gradient_parameter {
    vg_lite_float_t X0;
    vg_lite_float_t Y0;
    vg_lite_float_t X1;
    vg_lite_float_t Y1;
} vg_lite_linear_gradient_parameter_t;

typedef struct vg_lite_radial_gradient_parameter {
    vg_lite_float_t cx;                                 /*! x coordinate of the center point. */
    vg_lite_float_t cy;                                 /*! y coordinate of the center point. */
    vg_lite_float_t r;                                  /*! radius. */
    vg_lite_float_t fx;                                 /*! x coordinate of the focal point. */
    vg_lite_float_t fy;                                 /*! y coordinate of the focal point. */
} vg_lite_radial_gradient_parameter_t;

/* Определение линейного градиента. */
typedef struct vg_lite_linear_gradient {
    vg_lite_uint32_t colors[VLC_MAX_GRADIENT_STOPS];    /*! Colors for stops. */
    vg_lite_uint32_t count;                             /*! Count of colors, up to 16. */
    vg_lite_uint32_t stops[VLC_MAX_GRADIENT_STOPS];     /*! Color stops, value from 0 to 255. */
    vg_lite_matrix_t matrix;                            /*! The matrix to transform the gradient. */
    vg_lite_buffer_t image;                             /*! The image for rendering as gradient pattern. */
} vg_lite_linear_gradient_t;

/* Расширенное определение линейного градиента. */
typedef struct vg_lite_ext_linear_gradient {
    vg_lite_uint32_t count;                             /*! Count of colors, up to 256. */
    vg_lite_matrix_t matrix;                            /*! The matrix to transform the gradient. */
    vg_lite_buffer_t image;                             /*! The image for rendering as gradient pattern. */
    vg_lite_linear_gradient_parameter_t linear_grad;    /*!  Include center point,focal point and radius.*/

    vg_lite_uint32_t ramp_length;                       /*! Color ramp for gradient paints provided to driver. */
    vg_lite_color_ramp_t color_ramp[VLC_MAX_COLOR_RAMP_STOPS];

    vg_lite_uint32_t converted_length;                  /*! Converted internal color ramp. */
    vg_lite_color_ramp_t converted_ramp[VLC_MAX_COLOR_RAMP_STOPS + 2];

    vg_lite_uint8_t
    pre_multiplied;                     /*! If color values of color_ramp[] are multiply by alpha value of color_ramp[]. */
    vg_lite_gradient_spreadmode_t
    spread_mode;          /*! The spread mode that applied to the pixels out of the image after transformed. */
} vg_lite_ext_linear_gradient_t;

/* Определение радиального градиента. */
typedef struct vg_lite_radial_gradient {
    vg_lite_uint32_t count;                             /*! Count of colors, up to 256. */
    vg_lite_matrix_t matrix;                            /*! The matrix to transform the gradient. */
    vg_lite_buffer_t image;                             /*! The image for rendering as gradient pattern. */
    vg_lite_radial_gradient_parameter_t radial_grad;    /*!  Include center point,focal point and radius.*/

    vg_lite_uint32_t ramp_length;                       /*! Color ramp for gradient paints provided to the driver. */
    vg_lite_color_ramp_t color_ramp[VLC_MAX_COLOR_RAMP_STOPS];

    vg_lite_uint32_t converted_length;                  /*! Converted internal color ramp. */
    vg_lite_color_ramp_t converted_ramp[VLC_MAX_COLOR_RAMP_STOPS + 2];

    vg_lite_uint8_t
    pre_multiplied;                     /*! If color values of color_ramp[] are multiply by alpha value of color_ramp[]. */
    vg_lite_gradient_spreadmode_t
    spread_mode;          /*! The spread mode that applied to the pixels out of the image after transformed. */
} vg_lite_radial_gradient_t;

/* Определение цветовой ключа */
typedef struct vg_lite_color_key {
    vg_lite_uint8_t enable;                 /*! The color key is effective only when "enable" is ture, */
    vg_lite_uint8_t low_r;                  /*! The R chanel of low_rgb. */
    vg_lite_uint8_t low_g;                  /*! The G chanel of low_rgb. */
    vg_lite_uint8_t low_b;                  /*! The B chanel of low_rgb. */
    vg_lite_uint8_t alpha;                  /*! The alpha channel to replace destination pixel alpha channel.*/
    vg_lite_uint8_t high_r;                 /*! The R chanel of high_rgb. */
    vg_lite_uint8_t high_g;                 /*! The G chanel of high_rgb. */
    vg_lite_uint8_t high_b;                 /*! The B chanel of high_rgb. */
} vg_lite_color_key_t;

/* Определение четырех цветовых клавиш.
 * rgb_hi_0 ,rgb_lo_0,alpha_0,enable_0;
 * rgb_hi_1 ,rgb_lo_1,alpha_1,enable_1;
 * rgb_hi_2 ,rgb_lo_2,alpha_2,enable_2;
 * rgb_hi_3 ,rgb_lo_3,alpha_3,enable_3;
 * Приоритетный порядок:color_key_0>color_key_1>color_key_2>color_key_3.
*/
typedef vg_lite_color_key_t vg_lite_color_key4_t[4];

/* Значения пиксельной матрицы */
typedef vg_lite_float_t vg_lite_pixel_matrix_t[20];

/* HW Флаги включения пиксельного канала */
typedef struct vg_lite_pixel_channel_enable {
    vg_lite_uint8_t enable_a;               /*! Enable A channel.*/
    vg_lite_uint8_t enable_b;               /*! Enable B channel. */
    vg_lite_uint8_t enable_g;               /*! Enable G channel. */
    vg_lite_uint8_t enable_r;               /*! Enable R channel. */
} vg_lite_pixel_channel_enable_t;

/* Преобразование цвета пикселей */
typedef struct vg_lite_color_transform {
    vg_lite_float_t a_scale;
    vg_lite_float_t a_bias;
    vg_lite_float_t r_scale;
    vg_lite_float_t r_bias;
    vg_lite_float_t g_scale;
    vg_lite_float_t g_bias;
    vg_lite_float_t b_scale;
    vg_lite_float_t b_bias;
} vg_lite_color_transform_t;

/* Функции VGLiteAPI***********************************************************************************************************/

/* Инициализируйте контекст vglite. */
vg_lite_error_t vg_lite_init(vg_lite_uint32_t tess_width, vg_lite_uint32_t tess_height);

/* Уничтожьте контекст вглите. */
vg_lite_error_t vg_lite_close(void);

/* Получите информацию о драйвере VGLite. */
vg_lite_error_t vg_lite_get_info(vg_lite_info_t * info);

/* Получите информацию о чипе GPU. */
vg_lite_uint32_t vg_lite_get_product_info(vg_lite_char * name, vg_lite_uint32_t * chip_id, vg_lite_uint32_t * chip_rev);

/* Запросите, поддерживается ли конкретная функция. */
vg_lite_uint32_t vg_lite_query_feature(vg_lite_feature_t feature);

/* Очистите буфер команд и дождитесь завершения GPU. */
vg_lite_error_t vg_lite_finish(void);

/* Очистите буфер команд, не дожидаясь завершения GPU. */
vg_lite_error_t vg_lite_flush(void);

/* Получите значение регистра по адресу регистра. */
vg_lite_error_t vg_lite_get_register(vg_lite_uint32_t address, vg_lite_uint32_t * result);

/* Создайте однородную матрицу 3x3 для преобразования 4 исходных координат в 4 целевые координаты. */
vg_lite_error_t vg_lite_get_transform_matrix(vg_lite_float_point4_t src, vg_lite_float_point4_t dst,
                                             vg_lite_matrix_t * mat);

/* Выделите буфер из аппаратно доступной памяти GPU. */
vg_lite_error_t vg_lite_allocate(vg_lite_buffer_t * buffer);

/* Освободить буфер, выделенныйvg_lite_allocate() */
vg_lite_error_t vg_lite_free(vg_lite_buffer_t * buffer);

/* Загрузите данные пикселей RGB или YUV в выделенный буфер. */
vg_lite_error_t vg_lite_upload_buffer(vg_lite_buffer_t * buffer, vg_lite_uint8_t * data[3], vg_lite_uint32_t stride[3]);

/* Сопоставьте буфер с аппаратно доступным адресным пространством. */
vg_lite_error_t vg_lite_map(vg_lite_buffer_t * buffer, vg_lite_map_flag_t flag, int32_t fd);

/* Отменить сопоставление буфера, который сопоставлен */
vg_lite_error_t vg_lite_unmap(vg_lite_buffer_t * buffer);

/* очистить кеш */
vg_lite_error_t vg_lite_flush_mapped_buffer(vg_lite_buffer_t * buffer);

/* Заполните область прямоугольника буфера указанным цветом. */
vg_lite_error_t vg_lite_clear(vg_lite_buffer_t * target, vg_lite_rectangle_t * rect, vg_lite_color_t color);

/* Скопируйте исходное изображение в целевой буфер с преобразованием, смешиванием, смешиванием цветов и фильтрацией. */
vg_lite_error_t vg_lite_blit(vg_lite_buffer_t * target,
                             vg_lite_buffer_t * source,
                             vg_lite_matrix_t * matrix,
                             vg_lite_blend_t   blend,
                             vg_lite_color_t   color,
                             vg_lite_filter_t  filter);

/* Скопируйте прямоугольную область исходного изображения в целевой буфер с преобразованием, смешиванием, смешиванием цветов и фильтрацией. */
vg_lite_error_t vg_lite_blit_rect(vg_lite_buffer_t * target,
                                  vg_lite_buffer_t * source,
                                  vg_lite_rectangle_t * rect,
                                  vg_lite_matrix_t * matrix,
                                  vg_lite_blend_t   blend,
                                  vg_lite_color_t   color,
                                  vg_lite_filter_t  filter);

/* Скопируйте два исходных изображения в целевой буфер с преобразованием, смешиванием и фильтрацией. */
vg_lite_error_t vg_lite_blit2(vg_lite_buffer_t * target,
                              vg_lite_buffer_t * source0,
                              vg_lite_buffer_t * source1,
                              vg_lite_matrix_t * matrix0,
                              vg_lite_matrix_t * matrix1,
                              vg_lite_blend_t blend,
                              vg_lite_filter_t  filter);

/* Скопируйте прямоугольную область исходного изображения в целевой буфер без преобразования, смешивания, смешивания цветов и фильтрации. */
vg_lite_error_t vg_lite_copy_image(vg_lite_buffer_t * target,
                                   vg_lite_buffer_t * source,
                                   vg_lite_int32_t   sx,
                                   vg_lite_int32_t   sy,
                                   vg_lite_int32_t   dx,
                                   vg_lite_int32_t   dy,
                                   vg_lite_uint32_t  width,
                                   vg_lite_uint32_t  height);

/* Нарисуйте путь к целевому буферу с преобразованием, цветом и смешиванием. */
vg_lite_error_t vg_lite_draw(vg_lite_buffer_t * target,
                             vg_lite_path_t  * path,
                             vg_lite_fill_t    fill_rule,
                             vg_lite_matrix_t * matrix,
                             vg_lite_blend_t   blend,
                             vg_lite_color_t   color);

/* Установите атрибуты пути обводки. */
vg_lite_error_t vg_lite_set_stroke(vg_lite_path_t * path,
                                   vg_lite_cap_style_t cap_style,
                                   vg_lite_join_style_t join_style,
                                   vg_lite_float_t line_width,
                                   vg_lite_float_t miter_limit,
                                   vg_lite_float_t * dash_pattern,
                                   vg_lite_uint32_t pattern_count,
                                   vg_lite_float_t dash_phase,
                                   vg_lite_color_t color);

/* Обновите путь обводки. */
vg_lite_error_t vg_lite_update_stroke(vg_lite_path_t * path);

/* Установите тип пути. */
vg_lite_error_t vg_lite_set_path_type(vg_lite_path_t * path, vg_lite_path_type_t path_type);

/* Очищает все атрибуты пути. */
vg_lite_error_t vg_lite_clear_path(vg_lite_path_t * path);

/* Загрузите путь к памяти GPU, чтобы GPU мог получить к ней прямой доступ. */
vg_lite_error_t vg_lite_upload_path(vg_lite_path_t * path);

/* Инициализируйте объект пути с атрибутами. */
vg_lite_error_t vg_lite_init_path(vg_lite_path_t * path,
                                  vg_lite_format_t format,
                                  vg_lite_quality_t quality,
                                  vg_lite_uint32_t length,
                                  vg_lite_pointer data,
                                  vg_lite_float_t min_x,
                                  vg_lite_float_t min_y,
                                  vg_lite_float_t max_x,
                                  vg_lite_float_t max_y);

/* Инициализирует путь дуги с атрибутами. */
vg_lite_error_t vg_lite_init_arc_path(vg_lite_path_t * path,
                                      vg_lite_format_t format,
                                      vg_lite_quality_t quality,
                                      vg_lite_uint32_t length,
                                      vg_lite_pointer data,
                                      vg_lite_float_t min_x,
                                      vg_lite_float_t min_y,
                                      vg_lite_float_t max_x,
                                      vg_lite_float_t max_y);

/* Возвращает размер (в байтах) командного буфера для массива кодов операций пути. */
vg_lite_uint32_t vg_lite_get_path_length(vg_lite_uint8_t * opcode,
                                         vg_lite_uint32_t count,
                                         vg_lite_format_t format);

/* Сгенерируйте командный буфер для (пути) на основе входных кодов операций (код операции) и координат (данных). */
vg_lite_error_t vg_lite_append_path(vg_lite_path_t * path,
                                    vg_lite_uint8_t * opcode,
                                    vg_lite_pointer data,
                                    vg_lite_uint32_t seg_count);

/* Установите CLUT (таблицу поиска цветов) для индексного изображения. (Цвета) имеют формат ARGB. */
vg_lite_error_t vg_lite_set_CLUT(vg_lite_uint32_t count, vg_lite_uint32_t * colors);

/* Нарисуйте путь, заполненный шаблоном преобразованного изображения. */
vg_lite_error_t vg_lite_draw_pattern(vg_lite_buffer_t * target,
                                     vg_lite_path_t * path,
                                     vg_lite_fill_t fill_rule,
                                     vg_lite_matrix_t * path_matrix,
                                     vg_lite_buffer_t * pattern_image,
                                     vg_lite_matrix_t * pattern_matrix,
                                     vg_lite_blend_t blend,
                                     vg_lite_pattern_mode_t pattern_mode,
                                     vg_lite_color_t  pattern_color,
                                     vg_lite_color_t  color,
                                     vg_lite_filter_t filter);

/* Инициализируйте объект линейного градиента с атрибутами по умолчанию. */
vg_lite_error_t vg_lite_init_grad(vg_lite_linear_gradient_t * grad);

/* Сбросьте атрибуты объекта линейного градиента. */
vg_lite_error_t vg_lite_clear_grad(vg_lite_linear_gradient_t * grad);

/* Обновите объект линейного градиента. */
vg_lite_error_t vg_lite_update_grad(vg_lite_linear_gradient_t * grad);

/* Возвращает указатель на матрицу объекта линейного градиента. */
vg_lite_matrix_t * vg_lite_get_grad_matrix(vg_lite_linear_gradient_t * grad);

/* Установите атрибуты для объекта линейного градиента. */
vg_lite_error_t vg_lite_set_grad(vg_lite_linear_gradient_t * grad,
                                 vg_lite_uint32_t count,
                                 vg_lite_uint32_t * colors,
                                 vg_lite_uint32_t * stops);

/* Нарисуйте путь с помощью шаблона объекта с линейным градиентом. */
vg_lite_error_t vg_lite_draw_grad(vg_lite_buffer_t * target,
                                  vg_lite_path_t * path,
                                  vg_lite_fill_t fill_rule,
                                  vg_lite_matrix_t * matrix,
                                  vg_lite_linear_gradient_t * grad,
                                  vg_lite_blend_t blend);

/* Сбросьте атрибуты объекта расширенного линейного градиента и освободите буфер изображения. */
vg_lite_error_t vg_lite_clear_linear_grad(vg_lite_ext_linear_gradient_t * grad);

/* Обновите объект расширенного линейного градиента. */
vg_lite_error_t vg_lite_update_linear_grad(vg_lite_ext_linear_gradient_t * grad);

/* Возвращает указатель на матрицу объекта расширенного линейного градиента. */
vg_lite_matrix_t * vg_lite_get_linear_grad_matrix(vg_lite_ext_linear_gradient_t * grad);

/* Установите атрибуты для объекта расширенного линейного градиента. */
vg_lite_error_t vg_lite_set_linear_grad(vg_lite_ext_linear_gradient_t * grad,
                                        vg_lite_uint32_t count,
                                        vg_lite_color_ramp_t * color_ramp,
                                        vg_lite_linear_gradient_parameter_t grad_param,
                                        vg_lite_gradient_spreadmode_t spread_mode,
                                        vg_lite_uint8_t pre_mult);

/* Нарисуйте путь с помощью объекта расширенного линейного градиента. */
vg_lite_error_t vg_lite_draw_linear_grad(vg_lite_buffer_t * target,
                                         vg_lite_path_t * path,
                                         vg_lite_fill_t fill_rule,
                                         vg_lite_matrix_t * path_matrix,
                                         vg_lite_ext_linear_gradient_t * grad,
                                         vg_lite_color_t paint_color,
                                         vg_lite_blend_t blend,
                                         vg_lite_filter_t filter);

/* Сбросьте атрибуты объекта радиального градиента и освободите буфер изображения. */
vg_lite_error_t vg_lite_clear_radial_grad(vg_lite_radial_gradient_t * grad);

/* Обновите объект радиального градиента. */
vg_lite_error_t vg_lite_update_radial_grad(vg_lite_radial_gradient_t * grad);

/* Возвращает указатель на матрицу объекта радиального градиента. */
vg_lite_matrix_t * vg_lite_get_radial_grad_matrix(vg_lite_radial_gradient_t * grad);

/* Установите атрибуты для объекта радиального градиента. */
vg_lite_error_t vg_lite_set_radial_grad(vg_lite_radial_gradient_t * grad,
                                        vg_lite_uint32_t count,
                                        vg_lite_color_ramp_t * color_ramp,
                                        vg_lite_radial_gradient_parameter_t grad_param,
                                        vg_lite_gradient_spreadmode_t spread_mode,
                                        vg_lite_uint8_t pre_mult);

/* Нарисуйте путь с узором объекта радиального градиента. */
vg_lite_error_t vg_lite_draw_radial_grad(vg_lite_buffer_t * target,
                                         vg_lite_path_t * path,
                                         vg_lite_fill_t fill_rule,
                                         vg_lite_matrix_t * path_matrix,
                                         vg_lite_radial_gradient_t * grad,
                                         vg_lite_color_t  paint_color,
                                         vg_lite_blend_t blend,
                                         vg_lite_filter_t filter);

/* Загрузите идентификационную матрицу. */
vg_lite_error_t vg_lite_identity(vg_lite_matrix_t * matrix);

/* Перевести матрицу. */
vg_lite_error_t vg_lite_translate(vg_lite_float_t x, vg_lite_float_t y, vg_lite_matrix_t * matrix);

/* Масштабируйте матрицу. */
vg_lite_error_t vg_lite_scale(vg_lite_float_t scale_x, vg_lite_float_t scale_y, vg_lite_matrix_t * matrix);

/* Поворот матрицы. */
vg_lite_error_t vg_lite_rotate(vg_lite_float_t degrees, vg_lite_matrix_t * matrix);

/* Установите и включите прямоугольник-ножницы для цели рендеринга. */
vg_lite_error_t vg_lite_set_scissor(vg_lite_int32_t x, vg_lite_int32_t y, vg_lite_int32_t right,
                                    vg_lite_int32_t bottom);

/* Установите прямоугольники-ножницы на слое маски. Ножничные прямоугольники включаются/отключаются с помощью следующих API. */
vg_lite_error_t vg_lite_scissor_rects(vg_lite_buffer_t * target, vg_lite_uint32_t nums, vg_lite_rectangle_t rect[]);

/* Включите ножничные прямоугольники, определенные на слое маски. */
vg_lite_error_t vg_lite_enable_scissor(void);

/* Отключите ножничные прямоугольники, определенные на слое маски. */
vg_lite_error_t vg_lite_disable_scissor(void);

/* Запрос размера доступной непрерывной видеопамяти. */
vg_lite_error_t vg_lite_get_mem_size(vg_lite_uint32_t * size);

/* Установить глобальное значение альфа для исходного изображения */
vg_lite_error_t vg_lite_source_global_alpha(vg_lite_global_alpha_t alpha_mode, vg_lite_uint8_t alpha_value);

/* Установите глобальное значение альфа для конечного изображения. */
vg_lite_error_t vg_lite_dest_global_alpha(vg_lite_global_alpha_t alpha_mode, vg_lite_uint8_t alpha_value);

/* Установить цветовую клавишу. */
vg_lite_error_t vg_lite_set_color_key(vg_lite_color_key4_t colorkey);

/* Включите функцию дизеринга. По умолчанию дизеринг — OFF. */
vg_lite_error_t vg_lite_enable_dither(void);

/* Отключите функцию дизеринга. По умолчанию дизеринг — OFF. */
vg_lite_error_t vg_lite_disable_dither(void);

/* Установите 64-байтовый выровненный буфер памяти (физический) в качестве буфера тесселяции VGLite. */
vg_lite_error_t vg_lite_set_tess_buffer(vg_lite_uint32_t physical, vg_lite_uint32_t size);

/* Может быть вызван перед vg_lite_init(), чтобы перезаписатьVG_LITE_COMMAND_BUFFER_SIZEпо умолчанию. */
vg_lite_error_t vg_lite_set_command_buffer_size(vg_lite_uint32_t size);

/* Установите настраиваемый пользователем буфер внешней памяти (физический, выровненный по 64 байта) в качестве командного буфера VGLite.
   Его следует оставить послеvg_lite_init(). */
vg_lite_error_t vg_lite_set_command_buffer(vg_lite_uint32_t physical, vg_lite_uint32_t size);

/* Настройте матрицу преобразования пикселей m[20], которая преобразует каждый пиксель следующим образом:
 *
 *              |a'|   |m0  m1  m2  m3  m4 | |a|
 *              |r'|   |m5  m6  m7  m8  m9 | |r|
 *              |g'| = |m10 m11 m12 m13 m14|.|g|
 *              |b'|   |m15 m16 m17 m18 m19| |b|
 *              |1 |   |0   0   0   0   1  | |1|
 *
 *  Преобразование пикселей для каналов A, R, G, B можно включить/отключить индивидуально с помощью параметра (канал).
 */
vg_lite_error_t vg_lite_set_pixel_matrix(vg_lite_pixel_matrix_t matrix, vg_lite_pixel_channel_enable_t * channel);

/* Установите значения веса размытия по Гауссу 3x3 для фильтрации пикселей изображения.
 *
 *  Параметры w0, w1, w2 определяют матрицу весов размытия по Гауссу 3x3, как показано ниже.
 *
 *                  |  w2   w1   w2 |
 *                  |  w1   w0   w1 |
 *                  |  w2   w1   w2 |
 *
 *  Сумма 9 весов ядра должна быть равна 1,0, чтобы избежать переполнения свертки ( w0 + 4*w1 + 4*w2 = 1,0).
 *  Весовая матрица 3x3 применяется к блоку пикселей 3x3.
 *
 *                  | пиксель[i-1][j-1] пиксель[i][j-1] пиксель[i+1][j-1]|
 *                  | пиксель[i-1][j] пиксель[i][j] пиксель[i+1][j] |
 *                  | пиксель[i-1][j+1] пиксель[i][j+1] пиксель[i+1][j+1]|
 *
 *  Со следующим уравнением скалярного произведения:
 *
 *     цвет[i][j] = w2*пиксель[i-1][j-1] + w1*пиксель[i][j-1] + w2*пиксель[i+1][j-1]
 *                 + w1*пиксель[i-1][j] + w0*пиксель[i][j] + w1*пиксель[i+1][j]
 *                 + w2*пиксель[i-1][j+1] + w1*пиксель[i][j+1] + w2*пиксель[i+1][j+1];
 */
vg_lite_error_t vg_lite_gaussian_filter(vg_lite_float_t w0, vg_lite_float_t w1, vg_lite_float_t w2);

/*  Включите функцию маскировки. По умолчанию Маскоубийца —OFF. */
vg_lite_error_t vg_lite_enable_masklayer(void);

/*  Отключите функцию маскировки. По умолчанию Маскоубийца —OFF. */
vg_lite_error_t vg_lite_disable_masklayer(void);

/* Настройте слой-маску. */
vg_lite_error_t vg_lite_set_masklayer(vg_lite_buffer_t * masklayer);

/* Освободите слой маски и отключите операцию маски. */
vg_lite_error_t vg_lite_destroy_masklayer(vg_lite_buffer_t * masklayer);

/* Создайте слой маски с форматом по умолчанию A8 и значением пикселя по умолчанию 255. */
vg_lite_error_t vg_lite_create_masklayer(vg_lite_buffer_t * masklayer,
                                         vg_lite_uint32_t width,
                                         vg_lite_uint32_t height);

/* Установите значения пикселей для прямоугольной области в маске */
vg_lite_error_t vg_lite_fill_masklayer(vg_lite_buffer_t * masklayer,
                                       vg_lite_rectangle_t * rect,
                                       vg_lite_uint8_t value);

/* Сложите прямоугольную область слоя маски src со слоем маски dst в соответствии с (операция). */
vg_lite_error_t vg_lite_blend_masklayer(vg_lite_buffer_t * dst,
                                        vg_lite_buffer_t * src,
                                        vg_lite_mask_operation_t operation,
                                        vg_lite_rectangle_t * rect);

/* Отобразите (путь) с помощью (fill_rule), (цвет), (матрица) в слой маски. */
vg_lite_error_t vg_lite_render_masklayer(vg_lite_buffer_t * masklayer,
                                         vg_lite_mask_operation_t operation,
                                         vg_lite_path_t * path,
                                         vg_lite_fill_t fill_rule,
                                         vg_lite_color_t color,
                                         vg_lite_matrix_t * matrix);

/* Установите зеркальную ориентацию. */
vg_lite_error_t vg_lite_set_mirror(vg_lite_orientation_t orientation);

/* Установите значение гаммы. */
vg_lite_error_t vg_lite_set_gamma(vg_lite_gamma_conversion_t gamma_value);

/* Включите преобразование цвета, по умолчанию это OFF. */
vg_lite_error_t vg_lite_enable_color_transform(void);

/* Отключите преобразование цвета, по умолчанию это OFF. */
vg_lite_error_t vg_lite_disable_color_transform(void);

/* Установите масштаб преобразования цвета пикселей и значения смещения для каждого пиксельного канала. */
vg_lite_error_t vg_lite_set_color_transform(vg_lite_color_transform_t * values);

/* Установите идентификатор потока Flexa. */
vg_lite_error_t vg_lite_flexa_set_stream(vg_lite_uint8_t stream_id);

/* установите фоновый буфер Flexa.*/
vg_lite_error_t vg_lite_flexa_bg_buffer(vg_lite_uint8_t stream_id,
                                        vg_lite_buffer_t * buffer,
                                        vg_lite_uint32_t seg_count,
                                        vg_lite_uint32_t seg_size);

/* Включить флексу. */
vg_lite_error_t vg_lite_flexa_enable(void);

/* Отключить флексу.*/
vg_lite_error_t vg_lite_flexa_disable(void);

/* Установите флаг остановки гибкости после последнего кадра. */
vg_lite_error_t vg_lite_flexa_stop_frame(void);

/* Дамп командного буфера */
vg_lite_error_t vg_lite_dump_command_buffer(void);

/* Дамп изображенияvg_lite_buffer_tв файл png. Поддержка Linux на данный момент. */
vg_lite_error_t vg_lite_dump_png(const char * filename, vg_lite_buffer_t * buffer);

/* Вернуть параметры VGLite в массив params[] */
vg_lite_error_t vg_lite_get_parameter(vg_lite_param_type_t type,
                                      vg_lite_int32_t count,
                                      vg_lite_pointer params);

/* Установите пул памяти для разных распределений буферов. По умолчанию все буферы памяти выделяются из VG_LITE_MEMORY_POOL_1.
 * ЭтотAPIдолжен перейти передvg_lite_init() для установки пулов памятиVG_LITE_COMMAND_BUFFERили VG_LITE_TESSELLATION_BUFFER.
 * ЭтотAPIможно вызвать в любое время, чтобыVG_LITE_RENDER_BUFFERсоответствовал следующим вызовам vg_lite_allocate().
 */
vg_lite_error_t vg_lite_set_memory_pool(vg_lite_buffer_type_t type, vg_lite_memory_pool_t pool);

/* Установите флаг завершения для GPU, чтобы сигнализировать о завершении текущего кадра.
 * ЭтотAPIможет быть вызван в конце кадра, а vg_lite_finish() находится внутриAPI.
 * Будет получено прерывание, указывающее, что GPU простаивает.
 */
vg_lite_error_t vg_lite_frame_delimiter(vg_lite_frame_flag_t flag);

#endif /* VGLITE_VERSION_3_0 */

#endif /* LV_USE_VG_LITE_DRIVER */

#ifdef __cplusplus
}
#endif

#endif /* VG_LITE_H */
