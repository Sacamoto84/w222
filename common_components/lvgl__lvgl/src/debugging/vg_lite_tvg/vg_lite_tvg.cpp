/**
 * @file vg_lite_tvg.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_DRAW_VG_LITE && LV_USE_VG_LITE_THORVG

#include "vg_lite.h"
#include "../../lvgl.h"
#include "../../libs/thorvg/thorvg.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <vector>

#if LV_VG_LITE_THORVG_YUV_SUPPORT
    #include <libyuv/convert_argb.h>
#endif

/*********************
 *      DEFINES
 *********************/

#define TVG_CANVAS_ENGINE CanvasEngine::Sw
#define TVG_COLOR(COLOR) B(COLOR), G(COLOR), R(COLOR), A(COLOR)
#define TVG_IS_VG_FMT_SUPPORT(fmt) ((fmt) == VG_LITE_BGRA8888 || (fmt) == VG_LITE_BGRX8888)

#define TVG_CHECK_RETURN_VG_ERROR(FUNC)                               \
    do {                                                              \
        Result res = FUNC;                                            \
        if (res != Result::Success) {                                 \
            LV_LOG_ERROR("Executed '" #FUNC "' error: %d", (int)res);                        \
            return vg_lite_error_conv(res);                           \
        }                                                             \
    } while (0)
#define TVG_CHECK_RETURN_RESULT(FUNC)                                 \
    do {                                                              \
        Result res = FUNC;                                            \
        if (res != Result::Success) {                                 \
            LV_LOG_ERROR("Executed '" #FUNC "' error: %d", (int)res);\
            return res;                                               \
        }                                                             \
    } while (0)

/* clang-format off */

#define IS_INDEX_FMT(fmt)           \
    ((fmt) == VG_LITE_INDEX_1       \
     || (fmt) == VG_LITE_INDEX_2 \
     || (fmt) == VG_LITE_INDEX_4 \
     || (fmt) == VG_LITE_INDEX_8)

#define VLC_GET_ARG(CUR, INDEX) vlc_get_arg((cur + (INDEX) * fmt_len), path->format);
#define VLC_GET_OP_CODE(ptr) (*((uint8_t*)ptr))
#define VLC_OP_ARG_LEN(OP, LEN) \
    case VLC_OP_##OP:           \
    return (LEN)

#define A(color) ((color) >> 24)
#define R(color) (((color) & 0x00ff0000) >> 16)
#define G(color) (((color) & 0x0000ff00) >> 8)
#define B(color) ((color) & 0xff)
#define ARGB(a, r, g, b) ((a) << 24) | ((r) << 16) | ((g) << 8) | (b)
#define MIN(a, b) (a) > (b) ? (b) : (a)
#define MAX(a, b) (a) > (b) ? (a) : (b)
#define UDIV255(x) (((x) * 0x8081U) >> 0x17)
#define LERP(v1, v2, w) ((v1) * (w) + (v2) * (1.0f - (w)))
#define CLAMP(x, min, max) (((x) < (min)) ? (min) : ((x) > (max)) ? (max) : (x))
#define COLOR_FROM_RAMP(ColorRamp) (((vg_lite_float_t*)ColorRamp) + 1)

#define VG_LITE_RETURN_ERROR(func)         \
    if ((error = func) != VG_LITE_SUCCESS) \
        return error

#define VG_LITE_ALIGN(number, align_bytes) \
    (((number) + ((align_bytes)-1)) & ~((align_bytes)-1))

#define VG_LITE_IS_ALIGNED(num, align) (((uintptr_t)(num) & ((align)-1)) == 0)

#define VG_LITE_IS_ALPHA_FORMAT(format) \
    ((format) == VG_LITE_A8 || (format) == VG_LITE_A4)

/* clang-format on */

/**********************
 *      TYPEDEFS
 **********************/

using namespace tvg;

#pragma pack(1)
typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} vg_color24_t;

typedef struct {
    uint16_t blue : 5;
    uint16_t green : 6;
    uint16_t red : 5;
} vg_color16_t;

typedef struct {
    vg_color16_t c;
    uint8_t alpha;
} vg_color16_alpha_t;

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
} vg_color32_t;

typedef struct {
    uint8_t blue : 4;
    uint8_t green : 4;
    uint8_t red : 4;
    uint8_t alpha : 4;
} vg_color_bgra4444_t;

typedef struct {
    uint8_t blue : 2;
    uint8_t green : 2;
    uint8_t red : 2;
    uint8_t alpha : 2;
} vg_color_bgra2222_t;

typedef struct {
    uint8_t blue : 5;
    uint8_t green : 5;
    uint8_t red : 5;
    uint8_t alpha : 1;
} vg_color_bgra5551_t;

typedef struct {
    vg_lite_float_t x;
    vg_lite_float_t y;
} vg_lite_fpoint_t;

#pragma pack()

class vg_lite_ctx
{
    public:
        std::unique_ptr<SwCanvas> canvas;
        void * target_buffer;
        void * tvg_target_buffer;
        vg_lite_uint32_t target_px_size;
        vg_lite_buffer_format_t target_format;
        vg_lite_rectangle_t scissor_rect;
        bool scissor_is_set;

    public:
        vg_lite_ctx()
            : target_buffer { nullptr }
            , tvg_target_buffer { nullptr }
            , target_px_size { 0 }
            , target_format { VG_LITE_BGRA8888 }
            , scissor_rect { 0, 0, 0, 0 }
            , scissor_is_set { false }
            , clut_2colors { 0 }
            , clut_4colors { 0 }
            , clut_16colors { 0 }
            , clut_256colors { 0 }
        {
            canvas = SwCanvas::gen();
        }

        vg_lite_uint32_t * get_image_buffer(vg_lite_uint32_t w, vg_lite_uint32_t h)
        {
            src_buffer.resize(w * h);
            return src_buffer.data();
        }

        vg_lite_uint32_t * get_temp_target_buffer(vg_lite_uint32_t w, vg_lite_uint32_t h)
        {
            vg_lite_uint32_t px_size = w * h;
            if(px_size > dest_buffer.size()) {
                dest_buffer.resize(w * h);
            }
            return dest_buffer.data();
        }

        vg_lite_uint32_t * get_temp_target_buffer()
        {
            return dest_buffer.data();
        }

        void set_CLUT(vg_lite_uint32_t count, const vg_lite_uint32_t * colors)
        {
            switch(count) {
                case 2:
                    memcpy(clut_2colors, colors, sizeof(clut_2colors));
                    break;
                case 4:
                    memcpy(clut_4colors, colors, sizeof(clut_4colors));
                    break;
                case 16:
                    memcpy(clut_16colors, colors, sizeof(clut_16colors));
                    break;
                case 256:
                    memcpy(clut_256colors, colors, sizeof(clut_256colors));
                    break;
                default:
                    LV_ASSERT(false);
                    break;
            }
        }

        const vg_lite_uint32_t * get_CLUT(vg_lite_buffer_format_t format)
        {
            switch(format) {
                case VG_LITE_INDEX_1:
                    return clut_2colors;

                case VG_LITE_INDEX_2:
                    return clut_2colors;

                case VG_LITE_INDEX_4:
                    return clut_4colors;

                case VG_LITE_INDEX_8:
                    return clut_256colors;

                default:
                    break;
            }

            LV_ASSERT(false);
            return nullptr;
        }

        static vg_lite_ctx * get_instance()
        {
            static vg_lite_ctx instance;
            return &instance;
        }

    private:
        /*  */
        std::vector<vg_lite_uint32_t> src_buffer;
        std::vector<vg_lite_uint32_t> dest_buffer;

        vg_lite_uint32_t clut_2colors[2];
        vg_lite_uint32_t clut_4colors[4];
        vg_lite_uint32_t clut_16colors[16];
        vg_lite_uint32_t clut_256colors[256];
};

template <typename DEST_TYPE, typename SRC_TYPE>
class vg_lite_converter
{
    public:
        typedef void (*converter_cb_t)(DEST_TYPE * dest, const SRC_TYPE * src, vg_lite_uint32_t px_size,
                                       vg_lite_uint32_t color);

    public:
        vg_lite_converter(converter_cb_t converter)
            : _converter_cb(converter)
        {
        }

        void convert(vg_lite_buffer_t * dest_buf, const vg_lite_buffer_t * src_buf, vg_lite_uint32_t color = 0)
        {
            LV_ASSERT(_converter_cb);
            uint8_t * dest = (uint8_t *)dest_buf->memory;
            const uint8_t * src = (const uint8_t *)src_buf->memory;
            vg_lite_uint32_t h = src_buf->height;

            while(h--) {
                _converter_cb((DEST_TYPE *)dest, (const SRC_TYPE *)src, src_buf->width, color);
                dest += dest_buf->stride;
                src += src_buf->stride;
            }
        }

    private:
        converter_cb_t _converter_cb;
};

typedef vg_lite_float_t FLOATVECTOR4[4];

/**********************
 *  STATIC PROTOTYPES
 **********************/

static vg_lite_error_t vg_lite_error_conv(Result result);
static Matrix matrix_conv(const vg_lite_matrix_t * matrix);
static FillRule fill_rule_conv(vg_lite_fill_t fill);
static BlendMethod blend_method_conv(vg_lite_blend_t blend);
static StrokeCap stroke_cap_conv(vg_lite_cap_style_t cap);
static StrokeJoin stroke_join_conv(vg_lite_join_style_t join);
static FillSpread fill_spread_conv(vg_lite_gradient_spreadmode_t spread);
static Result shape_append_path(std::unique_ptr<Shape> & shape, vg_lite_path_t * path, vg_lite_matrix_t * matrix);
static Result shape_append_rect(std::unique_ptr<Shape> & shape, const vg_lite_buffer_t * target,
                                const vg_lite_rectangle_t * rect);
static Result canvas_set_target(vg_lite_ctx * ctx, vg_lite_buffer_t * target);
static Result picture_load(vg_lite_ctx * ctx, std::unique_ptr<Picture> & picture, const vg_lite_buffer_t * source,
                           vg_lite_color_t color = 0);

static inline bool math_zero(float a)
{
    return (fabs(a) < FLT_EPSILON);
}

static inline bool math_equal(float a, float b)
{
    return math_zero(a - b);
}

static void ClampColor(FLOATVECTOR4 Source, FLOATVECTOR4 Target, uint8_t Premultiplied);
static uint8_t PackColorComponent(vg_lite_float_t value);
static void get_format_bytes(vg_lite_buffer_format_t format,
                             vg_lite_uint32_t * mul,
                             vg_lite_uint32_t * div,
                             vg_lite_uint32_t * bytes_align);

static vg_lite_fpoint_t matrix_transform_point(const vg_lite_matrix_t * matrix, const vg_lite_fpoint_t * point);
static Result vg_lite_grad_matrix_conv(vg_lite_matrix_t * result, const vg_lite_matrix_t * grad_matrix,
                                       const vg_lite_matrix_t * path_matrix);

/**********************
 *  STATIC VARIABLES
 **********************/

/* преобразователи цвета */

static vg_lite_converter<vg_color16_t, vg_color32_t> conv_bgra8888_to_bgr565(
    [](vg_color16_t * dest, const vg_color32_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t /* цвет */)
{
    while(px_size--) {
        dest->red = src->red * 0x1F / 0xFF;
        dest->green = src->green * 0x3F / 0xFF;
        dest->blue = src->blue * 0x1F / 0xFF;
        src++;
        dest++;
    }
});

static vg_lite_converter<vg_color16_alpha_t, vg_color32_t> conv_bgra8888_to_bgra5658(
    [](vg_color16_alpha_t * dest, const vg_color32_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t /* цвет */)
{
    while(px_size--) {
        dest->c.red = src->red * 0x1F / 0xFF;
        dest->c.green = src->green * 0x3F / 0xFF;
        dest->c.blue = src->blue * 0x1F / 0xFF;
        dest->alpha = src->alpha;
        src++;
        dest++;
    }
});

static vg_lite_converter<vg_color32_t, vg_color16_t> conv_bgr565_to_bgra8888(
    [](vg_color32_t * dest, const vg_color16_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t /* цвет */)
{
    while(px_size--) {
        dest->red = src->red * 0xFF / 0x1F;
        dest->green = src->green * 0xFF / 0x3F;
        dest->blue = src->blue * 0xFF / 0x1F;
        dest->alpha = 0xFF;
        src++;
        dest++;
    }
});

static vg_lite_converter<vg_color32_t, vg_color16_alpha_t> conv_bgra5658_to_bgra8888(
    [](vg_color32_t * dest, const vg_color16_alpha_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t /* цвет */)
{
    while(px_size--) {
        dest->red = src->c.red * 0xFF / 0x1F;
        dest->green = src->c.green * 0xFF / 0x3F;
        dest->blue = src->c.blue * 0xFF / 0x1F;
        dest->alpha = src->alpha;
        src++;
        dest++;
    }
});

static vg_lite_converter<vg_color32_t, vg_color32_t> conv_bgrx8888_to_bgra8888(
    [](vg_color32_t * dest, const vg_color32_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t /* цвет */)
{
    while(px_size--) {
        *dest = *src;
        dest->alpha = 0xFF;
        dest++;
        src++;
    }
});

static vg_lite_converter<vg_color32_t, vg_color24_t> conv_bgr888_to_bgra8888(
    [](vg_color32_t * dest, const vg_color24_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t /* цвет */)
{
    while(px_size--) {
        dest->red = src->red;
        dest->green = src->green;
        dest->blue = src->blue;
        dest->alpha = 0xFF;
        src++;
        dest++;
    }
});

static vg_lite_converter<vg_color32_t, uint8_t> conv_alpha8_to_bgra8888(
    [](vg_color32_t * dest, const uint8_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t color)
{
    while(px_size--) {
        uint8_t alpha = *src;
        dest->alpha = alpha;
        dest->red = UDIV255(B(color) * alpha);
        dest->green = UDIV255(G(color) * alpha);
        dest->blue = UDIV255(R(color) * alpha);
        dest++;
        src++;
    }
});

static vg_lite_converter<vg_color32_t, uint8_t> conv_alpha4_to_bgra8888(
    [](vg_color32_t * dest, const uint8_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t color)
{
    /* 1 байт -> 2 пикселя */
    px_size /= 2;

    while(px_size--) {
        /* высокий 4бит */
        uint8_t alpha = (*src & 0xF0);
        dest->alpha = alpha;
        dest->red = UDIV255(B(color) * alpha);
        dest->green = UDIV255(G(color) * alpha);
        dest->blue = UDIV255(R(color) * alpha);
        dest++;

        /* низкий 4бит */
        alpha = (*src & 0x0F) << 4;
        dest->alpha = alpha;
        dest->red = UDIV255(B(color) * alpha);
        dest->green = UDIV255(G(color) * alpha);
        dest->blue = UDIV255(R(color) * alpha);

        dest++;
        src++;
    }
});

static vg_lite_converter<vg_color32_t, uint8_t> conv_l8_to_bgra8888(
    [](vg_color32_t * dest, const uint8_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t /* цвет */)
{
    while(px_size--) {
        dest->alpha = 0xFF;
        dest->red = *src;
        dest->green = *src;
        dest->blue = *src;
        dest++;
        src++;
    }
});

static vg_lite_converter<vg_color32_t, vg_color_bgra5551_t> conv_bgra5551_to_bgra8888(
    [](vg_color32_t * dest, const vg_color_bgra5551_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t /* цвет */)
{
    while(px_size--) {
        dest->red = src->red * 0xFF / 0x1F;
        dest->green = src->green * 0xFF / 0x1F;
        dest->blue = src->blue * 0xFF / 0x1F;
        dest->alpha = src->alpha ? 0xFF : 0;
        src++;
        dest++;
    }
});

static vg_lite_converter<vg_color32_t, vg_color_bgra4444_t> conv_bgra4444_to_bgra8888(
    [](vg_color32_t * dest, const vg_color_bgra4444_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t /* цвет */)
{
    while(px_size--) {
        dest->red = src->red * 0xFF / 0xF;
        dest->green = src->green * 0xFF / 0xF;
        dest->blue = src->blue * 0xFF / 0xF;
        dest->alpha = src->alpha * 0xFF / 0xF;
        src++;
        dest++;
    }
});

static vg_lite_converter<vg_color32_t, vg_color_bgra2222_t> conv_bgra2222_to_bgra8888(
    [](vg_color32_t * dest, const vg_color_bgra2222_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t /* цвет */)
{
    while(px_size--) {
        dest->red = src->red * 0xFF / 0x3;
        dest->green = src->green * 0xFF / 0x3;
        dest->blue = src->blue * 0xFF / 0x3;
        dest->alpha = src->alpha * 0xFF / 0x3;
        src++;
        dest++;
    }
});

/* Используется для копирования изображений с непоследовательными шагами, но в том же цветовом формате. */
static vg_lite_converter<vg_color32_t, vg_color32_t> conv_bgra8888_to_bgra8888(
    [](vg_color32_t * dest, const vg_color32_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t /* цвет */)
{
    memcpy(dest, src, sizeof(vg_color32_t) * px_size);
});

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

extern "C" {

    void gpu_init(void)
    {
        vg_lite_init(0, 0);
    }

    vg_lite_error_t vg_lite_allocate(vg_lite_buffer_t * buffer)
    {
        if(buffer->format == VG_LITE_RGBA8888_ETC2_EAC && (buffer->width % 16 || buffer->height % 4)) {
            return VG_LITE_INVALID_ARGUMENT;
        }

        /* Сброс планарности. */
        buffer->yuv.uv_planar = buffer->yuv.v_planar = buffer->yuv.alpha_planar = 0;

        /* Выровнять высоту в случае, если формат плиточный. */
        if(buffer->format >= VG_LITE_YUY2 && buffer->format <= VG_LITE_NV16) {
            buffer->height = VG_LITE_ALIGN(buffer->height, 4);
            buffer->yuv.swizzle = VG_LITE_SWIZZLE_UV;
        }

        if(buffer->format >= VG_LITE_YUY2_TILED && buffer->format <= VG_LITE_AYUY2_TILED) {
            buffer->height = VG_LITE_ALIGN(buffer->height, 4);
            buffer->tiled = VG_LITE_TILED;
            buffer->yuv.swizzle = VG_LITE_SWIZZLE_UV;
        }

        vg_lite_uint32_t mul, div, align;
        get_format_bytes(buffer->format, &mul, &div, &align);
        vg_lite_uint32_t stride = VG_LITE_ALIGN((buffer->width * mul / div), align);

        buffer->stride = stride;

        /* Размер соревнований должен быть уменьшен. См.: https://en.cppreference.com/w/c/memory/aligned_alloc. */
        size_t size = VG_LITE_ALIGN(buffer->height * stride, LV_VG_LITE_THORVG_BUF_ADDR_ALIGN);
#ifndef _WIN32
        buffer->memory = aligned_alloc(LV_VG_LITE_THORVG_BUF_ADDR_ALIGN, size);
#else
        buffer->memory = _aligned_malloc(size, LV_VG_LITE_THORVG_BUF_ADDR_ALIGN);
#endif
        LV_ASSERT(buffer->memory);
        buffer->address = (vg_lite_uint32_t)(uintptr_t)buffer->memory;
        buffer->handle = buffer->memory;
        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_free(vg_lite_buffer_t * buffer)
    {
        LV_ASSERT(buffer->memory);
#ifndef _WIN32
        free(buffer->memory);
#else
        _aligned_free(buffer->memory);
#endif
        memset(buffer, 0, sizeof(vg_lite_buffer_t));
        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_upload_buffer(vg_lite_buffer_t * buffer, vg_lite_uint8_t * data[3], vg_lite_uint32_t stride[3])
    {
        LV_UNUSED(buffer);
        LV_UNUSED(data);
        LV_UNUSED(stride);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_map(vg_lite_buffer_t * buffer, vg_lite_map_flag_t flag, int32_t fd)
    {
        LV_UNUSED(buffer);
        LV_UNUSED(flag);
        LV_UNUSED(fd);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_unmap(vg_lite_buffer_t * buffer)
    {
        LV_UNUSED(buffer);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_clear(vg_lite_buffer_t * target, vg_lite_rectangle_t * rectangle, vg_lite_color_t color)
    {
        auto ctx = vg_lite_ctx::get_instance();
        TVG_CHECK_RETURN_VG_ERROR(canvas_set_target(ctx, target));

        auto shape = Shape::gen();
        TVG_CHECK_RETURN_VG_ERROR(shape_append_rect(shape, target, rectangle));
        TVG_CHECK_RETURN_VG_ERROR(shape->blend(BlendMethod::SrcOver));
        TVG_CHECK_RETURN_VG_ERROR(shape->fill(TVG_COLOR(color)));
        TVG_CHECK_RETURN_VG_ERROR(ctx->canvas->push(std::move(shape)));

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_blit(vg_lite_buffer_t * target,
                                 vg_lite_buffer_t * source,
                                 vg_lite_matrix_t * matrix,
                                 vg_lite_blend_t blend,
                                 vg_lite_color_t color,
                                 vg_lite_filter_t filter)
    {
        LV_UNUSED(filter);
        auto ctx = vg_lite_ctx::get_instance();
        canvas_set_target(ctx, target);

        auto picture = Picture::gen();

        TVG_CHECK_RETURN_VG_ERROR(picture_load(ctx, picture, source, color));
        TVG_CHECK_RETURN_VG_ERROR(picture->transform(matrix_conv(matrix)));
        TVG_CHECK_RETURN_VG_ERROR(picture->blend(blend_method_conv(blend)));
        TVG_CHECK_RETURN_VG_ERROR(ctx->canvas->push(std::move(picture)));

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_blit2(vg_lite_buffer_t * target,
                                  vg_lite_buffer_t * source0,
                                  vg_lite_buffer_t * source1,
                                  vg_lite_matrix_t * matrix0,
                                  vg_lite_matrix_t * matrix1,
                                  vg_lite_blend_t blend,
                                  vg_lite_filter_t filter)
    {
        if(!vg_lite_query_feature(gcFEATURE_BIT_VG_DOUBLE_IMAGE)) {
            return VG_LITE_NOT_SUPPORT;
        }

        vg_lite_error_t error;

        VG_LITE_RETURN_ERROR(vg_lite_blit(target, source0, matrix0, blend, 0, filter));
        VG_LITE_RETURN_ERROR(vg_lite_blit(target, source1, matrix1, blend, 0, filter));

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_blit_rect(vg_lite_buffer_t * target,
                                      vg_lite_buffer_t * source,
                                      vg_lite_rectangle_t * rect,
                                      vg_lite_matrix_t * matrix,
                                      vg_lite_blend_t blend,
                                      vg_lite_color_t color,
                                      vg_lite_filter_t filter)
    {
        LV_UNUSED(filter);
        auto ctx = vg_lite_ctx::get_instance();
        TVG_CHECK_RETURN_VG_ERROR(canvas_set_target(ctx, target));

        vg_lite_matrix_t new_matrix = *matrix;
        if(rect->x || rect->y) {
            /* имитировать поведение отсечения аппаратного устройства */
            vg_lite_translate(-rect->x, -rect->y, &new_matrix);
        }

        auto shape = Shape::gen();
        TVG_CHECK_RETURN_VG_ERROR(shape_append_rect(shape, target, rect));
        TVG_CHECK_RETURN_VG_ERROR(shape->transform(matrix_conv(&new_matrix)));

        auto picture = tvg::Picture::gen();
        TVG_CHECK_RETURN_VG_ERROR(picture_load(ctx, picture, source, color));
        TVG_CHECK_RETURN_VG_ERROR(picture->transform(matrix_conv(&new_matrix)));
        TVG_CHECK_RETURN_VG_ERROR(picture->blend(blend_method_conv(blend)));
        TVG_CHECK_RETURN_VG_ERROR(picture->composite(std::move(shape), CompositeMethod::ClipPath));
        TVG_CHECK_RETURN_VG_ERROR(ctx->canvas->push(std::move(picture)));

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_init(int32_t tessellation_width, int32_t tessellation_height)
    {
        LV_UNUSED(tessellation_width);
        LV_UNUSED(tessellation_height);
#if LV_VG_LITE_THORVG_THREAD_RENDER
        /* Число потоков */
        auto threads = std::thread::hardware_concurrency();
        if(threads > 0) {
            --threads; /* Разрешить назначенную мощность основного потока */
        }
#endif

        /* Инициализация двигателя ThorVG */
        TVG_CHECK_RETURN_VG_ERROR(Initializer::init(TVG_CANVAS_ENGINE, 0));
        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_close(void)
    {
        TVG_CHECK_RETURN_VG_ERROR(Initializer::term(TVG_CANVAS_ENGINE));
        return VG_LITE_SUCCESS;
    }

    static void picture_bgra8888_to_bgr565(vg_color16_t * dest, const vg_color32_t * src, vg_lite_uint32_t px_size)
    {
        while(px_size--) {
            dest->red = src->red * 0x1F / 0xFF;
            dest->green = src->green * 0x3F / 0xFF;
            dest->blue = src->blue * 0x1F / 0xFF;
            src++;
            dest++;
        }
    }

    static void picture_bgra8888_to_bgra5658(vg_color16_alpha_t * dest, const vg_color32_t * src, vg_lite_uint32_t px_size)
    {
        while(px_size--) {
            dest->c.red = src->red * 0x1F / 0xFF;
            dest->c.green = src->green * 0x3F / 0xFF;
            dest->c.blue = src->blue * 0x1F / 0xFF;
            dest->alpha = src->alpha;
            src++;
            dest++;
        }
    }

    static void picture_bgra8888_to_bgr888(vg_color24_t * dest, const vg_color32_t * src, vg_lite_uint32_t px_size)
    {
        while(px_size--) {
            dest->red = src->red;
            dest->green = src->green;
            dest->blue = src->blue;
            src++;
            dest++;
        }
    }

    static void picture_bgra8888_to_l8(uint8_t * dest, const vg_color32_t * src, vg_lite_uint32_t px_size)
    {
        while(px_size--) {
            *dest = (src->red * 19595 + src->green * 38469 + src->blue * 7472) >> 16;
            src++;
            dest++;
        }
    }

    static void picture_bgra8888_to_alpha8(uint8_t * dest, const vg_color32_t * src, vg_lite_uint32_t px_size)
    {
        while(px_size--) {
            *dest = src->alpha;
            src++;
            dest++;
        }
    }

    static void picture_bgra8888_to_bgra5551(vg_color_bgra5551_t * dest, const vg_color32_t * src, vg_lite_uint32_t px_size)
    {
        while(px_size--) {
            dest->red = src->red * 0x1F / 0xFF;
            dest->green = src->green * 0x1F / 0xFF;
            dest->blue = src->blue * 0x1F / 0xFF;
            dest->alpha = src->alpha > (0xFF / 2) ? 1 : 0;
            src++;
            dest++;
        }
    }

    static void picture_bgra8888_to_bgra4444(vg_color_bgra4444_t * dest, const vg_color32_t * src, vg_lite_uint32_t px_size)
    {
        while(px_size--) {
            dest->red = src->red * 0xF / 0xFF;
            dest->green = src->green * 0xF / 0xFF;
            dest->blue = src->blue * 0xF / 0xFF;
            dest->alpha = src->alpha * 0xF / 0xFF;
            src++;
            dest++;
        }
    }

    static void picture_bgra8888_to_bgra2222(vg_color_bgra2222_t * dest, const vg_color32_t * src, vg_lite_uint32_t px_size)
    {
        while(px_size--) {
            dest->red = src->red * 0x3 / 0xFF;
            dest->green = src->green * 0x3 / 0xFF;
            dest->blue = src->blue * 0x3 / 0xFF;
            dest->alpha = src->alpha * 0x3 / 0xFF;
            src++;
            dest++;
        }
    }


    vg_lite_error_t vg_lite_finish(void)
    {
        vg_lite_ctx * ctx = vg_lite_ctx::get_instance();

        if(ctx->canvas->draw() == Result::InsufficientCondition) {
            return VG_LITE_SUCCESS;
        }

        TVG_CHECK_RETURN_VG_ERROR(ctx->canvas->sync());
        TVG_CHECK_RETURN_VG_ERROR(ctx->canvas->clear(true));

        /* убедитесь, что целевой буфер действителен */
        LV_ASSERT_NULL(ctx->target_buffer);

        /* Если форматtarget_bufferне соответствует требованиям, необходимо преобразовать программное обеспечение. */
        switch(ctx->target_format) {
            case VG_LITE_BGR565:
                picture_bgra8888_to_bgr565(
                    (vg_color16_t *)ctx->target_buffer,
                    (const vg_color32_t *)ctx->get_temp_target_buffer(),
                    ctx->target_px_size);
                break;
            case VG_LITE_BGRA5658:
                picture_bgra8888_to_bgra5658(
                    (vg_color16_alpha_t *)ctx->target_buffer,
                    (const vg_color32_t *)ctx->get_temp_target_buffer(),
                    ctx->target_px_size);
                break;
            case VG_LITE_BGR888:
                picture_bgra8888_to_bgr888(
                    (vg_color24_t *)ctx->target_buffer,
                    (const vg_color32_t *)ctx->get_temp_target_buffer(),
                    ctx->target_px_size);
                break;
            case VG_LITE_L8:
                picture_bgra8888_to_l8(
                    (uint8_t *)ctx->target_buffer,
                    (const vg_color32_t *)ctx->get_temp_target_buffer(),
                    ctx->target_px_size);
                break;
            case VG_LITE_A8:
                picture_bgra8888_to_alpha8(
                    (uint8_t *)ctx->target_buffer,
                    (const vg_color32_t *)ctx->get_temp_target_buffer(),
                    ctx->target_px_size);
                break;
            case VG_LITE_BGRA5551:
                picture_bgra8888_to_bgra5551((vg_color_bgra5551_t *)ctx->target_buffer,
                                             (const vg_color32_t *)ctx->get_temp_target_buffer(),
                                             ctx->target_px_size);
                break;
            case VG_LITE_BGRA4444:
                picture_bgra8888_to_bgra4444((vg_color_bgra4444_t *)ctx->target_buffer,
                                             (const vg_color32_t *)ctx->get_temp_target_buffer(),
                                             ctx->target_px_size);
                break;
            case VG_LITE_BGRA2222:
                picture_bgra8888_to_bgra2222((vg_color_bgra2222_t *)ctx->target_buffer,
                                             (const vg_color32_t *)ctx->get_temp_target_buffer(),
                                             ctx->target_px_size);
                break;
            case VG_LITE_BGRA8888:
            case VG_LITE_BGRX8888:
                /* Никакого преобразования не требуется. */
                break;
            default:
                LV_LOG_ERROR("unsupported format: %d", ctx->target_format);
                LV_ASSERT(false);
                break;
        }

        /* завершить преобразование, очистить информацию о целевом буфере */
        ctx->target_buffer = nullptr;
        ctx->tvg_target_buffer = nullptr;
        ctx->target_px_size = 0;

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_flush(void)
    {
        return vg_lite_finish();
    }

    vg_lite_error_t vg_lite_draw(vg_lite_buffer_t * target,
                                 vg_lite_path_t * path,
                                 vg_lite_fill_t fill_rule,
                                 vg_lite_matrix_t * matrix,
                                 vg_lite_blend_t blend,
                                 vg_lite_color_t color)
    {
        auto ctx = vg_lite_ctx::get_instance();

        /**
         * Специальная обработка режима приложенияDST_INс использованием CompositeMethod::InvAlphaMask от ThorVG.
         * DST_IN (Sa*D): пиксели внутри фигуры очищаются, пиксели снаружи сохраняются.
         * Детали реализации:
         *   - Форма маски заполнена (255 - A(цвет)), используемым с InvAlphaMask.
         *   - Формула InvAlphaMask: dst = dst * (255 — mask_alpha) / 255.
         *   - Когда цвет = 0, маска альфа = 255 внутри фигуры, поэтому (255-255 = 0): внутренняя часть очищается; снаружи (255-0=255): сохранено.
         *   - Целевой буфер очищается с помощьюlv_memzeroперед рендерингом, обеспечивая обнуление внутренних пикселей.
         *   - Это имитирует эффект стирания VG_LITE_BLEND_DST_IN.
         */
        if(blend == VG_LITE_BLEND_DST_IN) {
            /* Сначала очистите все ожидающие операции */
            vg_lite_error_t error;
            VG_LITE_RETURN_ERROR(vg_lite_finish());

            TVG_CHECK_RETURN_VG_ERROR(canvas_set_target(ctx, target));

            /* Создайте форму маски белого цвета (полная альфа) */
            auto mask = Shape::gen();
            TVG_CHECK_RETURN_VG_ERROR(shape_append_path(mask, path, matrix));
            TVG_CHECK_RETURN_VG_ERROR(mask->transform(matrix_conv(matrix)));
            TVG_CHECK_RETURN_VG_ERROR(mask->fill(fill_rule_conv(fill_rule)));
            TVG_CHECK_RETURN_VG_ERROR(mask->fill(255 - B(color), 255 - G(color), 255 - R(color), 255 - A(color)));

            /* Создать изображение из текущего содержимого целевого буфера */
            auto picture = Picture::gen();
            TVG_CHECK_RETURN_VG_ERROR(picture_load(ctx, picture, target));

            /**
             * Поскольку содержимое целевого буфера уже скопировано в изображение,
             * теперь мы можем очистить целевой буфер для подготовки к рендерингу замаскированного изображения.
             */
            lv_memzero(target->memory, target->stride * target->height);

            /* Вернуть замаскированное изображение */
            TVG_CHECK_RETURN_VG_ERROR(picture->composite(std::move(mask), CompositeMethod::InvAlphaMask));
            TVG_CHECK_RETURN_VG_ERROR(ctx->canvas->push(std::move(picture)));

            return VG_LITE_SUCCESS;
        }

        TVG_CHECK_RETURN_VG_ERROR(canvas_set_target(ctx, target));

        auto shape = Shape::gen();
        TVG_CHECK_RETURN_VG_ERROR(shape_append_path(shape, path, matrix));
        TVG_CHECK_RETURN_VG_ERROR(shape->transform(matrix_conv(matrix)));
        TVG_CHECK_RETURN_VG_ERROR(shape->fill(fill_rule_conv(fill_rule)););
        TVG_CHECK_RETURN_VG_ERROR(shape->blend(blend_method_conv(blend)));
        TVG_CHECK_RETURN_VG_ERROR(shape->fill(TVG_COLOR(color)));
        TVG_CHECK_RETURN_VG_ERROR(ctx->canvas->push(std::move(shape)));

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_set_stroke(vg_lite_path_t * path,
                                       vg_lite_cap_style_t cap_style,
                                       vg_lite_join_style_t join_style,
                                       vg_lite_float_t line_width,
                                       vg_lite_float_t miter_limit,
                                       vg_lite_float_t * dash_pattern,
                                       vg_lite_uint32_t pattern_count,
                                       vg_lite_float_t dash_phase,
                                       vg_lite_color_t color)
    {
        if(!path || line_width <= 0) {
            return VG_LITE_INVALID_ARGUMENT;
        }

        if(miter_limit < 1.0f) {
            miter_limit = 1.0f;
        }

        if(!path->stroke) {
            path->stroke = (vg_lite_stroke_t *)lv_malloc_zeroed(sizeof(vg_lite_stroke_t));

            if(!path->stroke) {
                return VG_LITE_OUT_OF_RESOURCES;
            }
        }

        path->stroke->cap_style = cap_style;
        path->stroke->join_style = join_style;
        path->stroke->line_width = line_width;
        path->stroke->miter_limit = miter_limit;
        path->stroke->half_width = line_width / 2.0f;
        path->stroke->miter_square = path->stroke->miter_limit * path->stroke->miter_limit;
        path->stroke->dash_pattern = dash_pattern;
        path->stroke->pattern_count = pattern_count;
        path->stroke->dash_phase = dash_phase;
        path->stroke_color = color;
        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_update_stroke(vg_lite_path_t * path)
    {
        LV_UNUSED(path);
        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_set_path_type(vg_lite_path_t * path, vg_lite_path_type_t path_type)
    {
        if(!path ||
           (path_type != VG_LITE_DRAW_FILL_PATH &&
            path_type != VG_LITE_DRAW_STROKE_PATH &&
            path_type != VG_LITE_DRAW_FILL_STROKE_PATH)
          )
            return VG_LITE_INVALID_ARGUMENT;

        path->path_type = path_type;

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_get_register(vg_lite_uint32_t address, vg_lite_uint32_t * result)
    {
        LV_UNUSED(address);
        LV_UNUSED(result);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_get_info(vg_lite_info_t * info)
    {
        info->api_version = VGLITE_API_VERSION_3_0;
        info->header_version = VGLITE_HEADER_VERSION;
        info->release_version = VGLITE_RELEASE_VERSION;
        info->reserved = 0;
        return VG_LITE_SUCCESS;
    }

    vg_lite_uint32_t vg_lite_get_product_info(char * name, vg_lite_uint32_t * chip_id, vg_lite_uint32_t * chip_rev)
    {
        strcpy(name, "GCNanoLiteV");
        *chip_id = 0x265;
        *chip_rev = 0x2000;
        return 1;
    }

    vg_lite_uint32_t vg_lite_query_feature(vg_lite_feature_t feature)
    {
        switch(feature) {
            case gcFEATURE_BIT_VG_IM_INDEX_FORMAT:
            case gcFEATURE_BIT_VG_BORDER_CULLING:
            case gcFEATURE_BIT_VG_RGBA2_FORMAT:
            case gcFEATURE_BIT_VG_IM_FASTCLAER:
            case gcFEATURE_BIT_VG_GLOBAL_ALPHA:
            case gcFEATURE_BIT_VG_24BIT:
            case gcFEATURE_BIT_VG_DITHER:
            case gcFEATURE_BIT_VG_USE_DST:
            case gcFEATURE_BIT_VG_RADIAL_GRADIENT:
            case gcFEATURE_BIT_VG_IM_REPEAT_REFLECT:
            case gcFEATURE_BIT_VG_SCISSOR:

#if LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT
            case gcFEATURE_BIT_VG_LVGL_SUPPORT:
#endif

#if LV_VG_LITE_THORVG_YUV_SUPPORT
            case gcFEATURE_BIT_VG_YUV_INPUT:
#endif

#if LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT
            case gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT:
#endif

#if LV_VG_LITE_THORVG_16PIXELS_ALIGN
            case gcFEATURE_BIT_VG_16PIXELS_ALIGN:
#endif
                return 1;
            default:
                break;
        }
        return 0;
    }

    vg_lite_error_t vg_lite_init_path(vg_lite_path_t * path,
                                      vg_lite_format_t data_format,
                                      vg_lite_quality_t quality,
                                      vg_lite_uint32_t path_length,
                                      void * path_data,
                                      vg_lite_float_t min_x, vg_lite_float_t min_y,
                                      vg_lite_float_t max_x, vg_lite_float_t max_y)
    {
        if(!path) {
            return VG_LITE_INVALID_ARGUMENT;
        }

        lv_memzero(path, sizeof(vg_lite_path_t));

        path->format = data_format;
        path->quality = quality;
        path->bounding_box[0] = min_x;
        path->bounding_box[1] = min_y;
        path->bounding_box[2] = max_x;
        path->bounding_box[3] = max_y;

        path->path_length = path_length;
        path->path = path_data;

        path->path_changed = 1;
        path->uploaded.address = 0;
        path->uploaded.bytes = 0;
        path->uploaded.handle = NULL;
        path->uploaded.memory = NULL;
        path->pdata_internal = 0;

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_init_arc_path(vg_lite_path_t * path,
                                          vg_lite_format_t data_format,
                                          vg_lite_quality_t quality,
                                          vg_lite_uint32_t path_length,
                                          void * path_data,
                                          vg_lite_float_t min_x, vg_lite_float_t min_y,
                                          vg_lite_float_t max_x, vg_lite_float_t max_y)
    {
        LV_UNUSED(path);
        LV_UNUSED(data_format);
        LV_UNUSED(quality);
        LV_UNUSED(path_length);
        LV_UNUSED(path_data);
        LV_UNUSED(min_x);
        LV_UNUSED(min_y);
        LV_UNUSED(max_x);
        LV_UNUSED(max_y);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_clear_path(vg_lite_path_t * path)
    {
        LV_ASSERT_NULL(path);

        if(path->stroke) {
            lv_free(path->stroke);
            path->stroke = NULL;
        }

        return VG_LITE_SUCCESS;
    }

    vg_lite_uint32_t vg_lite_get_path_length(vg_lite_uint8_t * opcode,
                                             vg_lite_uint32_t count,
                                             vg_lite_format_t format)
    {
        LV_UNUSED(opcode);
        LV_UNUSED(count);
        LV_UNUSED(format);
        return 0;
    }

    vg_lite_error_t vg_lite_append_path(vg_lite_path_t * path,
                                        uint8_t * cmd,
                                        void * data,
                                        vg_lite_uint32_t seg_count)
    {
        LV_UNUSED(path);
        LV_UNUSED(cmd);
        LV_UNUSED(data);
        LV_UNUSED(seg_count);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_upload_path(vg_lite_path_t * path)
    {
        LV_UNUSED(path);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_set_CLUT(vg_lite_uint32_t count,
                                     vg_lite_uint32_t * colors)
    {
        if(!vg_lite_query_feature(gcFEATURE_BIT_VG_IM_INDEX_FORMAT)) {
            return VG_LITE_NOT_SUPPORT;
        }
        LV_ASSERT(colors);

        auto ctx = vg_lite_ctx::get_instance();
        ctx->set_CLUT(count, colors);
        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_draw_pattern(vg_lite_buffer_t * target,
                                         vg_lite_path_t * path,
                                         vg_lite_fill_t fill_rule,
                                         vg_lite_matrix_t * path_matrix,
                                         vg_lite_buffer_t * pattern_image,
                                         vg_lite_matrix_t * pattern_matrix,
                                         vg_lite_blend_t blend,
                                         vg_lite_pattern_mode_t pattern_mode,
                                         vg_lite_color_t pattern_color,
                                         vg_lite_color_t color,
                                         vg_lite_filter_t filter)
    {
        LV_UNUSED(pattern_mode);
        LV_UNUSED(pattern_color);
        LV_UNUSED(filter);

        auto ctx = vg_lite_ctx::get_instance();
        TVG_CHECK_RETURN_VG_ERROR(canvas_set_target(ctx, target));

        auto shape = Shape::gen();
        TVG_CHECK_RETURN_VG_ERROR(shape_append_path(shape, path, path_matrix));
        TVG_CHECK_RETURN_VG_ERROR(shape->fill(fill_rule_conv(fill_rule)));
        TVG_CHECK_RETURN_VG_ERROR(shape->transform(matrix_conv(path_matrix)));

        auto picture = tvg::Picture::gen();
        TVG_CHECK_RETURN_VG_ERROR(picture_load(ctx, picture, pattern_image, color));
        TVG_CHECK_RETURN_VG_ERROR(picture->transform(matrix_conv(pattern_matrix)));
        TVG_CHECK_RETURN_VG_ERROR(picture->blend(blend_method_conv(blend)));
        TVG_CHECK_RETURN_VG_ERROR(picture->composite(std::move(shape), CompositeMethod::ClipPath));
        TVG_CHECK_RETURN_VG_ERROR(ctx->canvas->push(std::move(picture)));

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_init_grad(vg_lite_linear_gradient_t * grad)
    {
        vg_lite_error_t error = VG_LITE_SUCCESS;

        /* Установите значения элементов в соответствии со значениями драйвера по умолчанию. */
        grad->image.width = VLC_GRADIENT_BUFFER_WIDTH;
        grad->image.height = 1;
        grad->image.stride = 0;
        grad->image.format = VG_LITE_BGRA8888;

        /* Выделите изображение для градиента. */
        error = vg_lite_allocate(&grad->image);

        grad->count = 0;

        return error;
    }

    vg_lite_error_t vg_lite_set_linear_grad(vg_lite_ext_linear_gradient_t * grad,
                                            vg_lite_uint32_t count,
                                            vg_lite_color_ramp_t * color_ramp,
                                            vg_lite_linear_gradient_parameter_t linear_gradient,
                                            vg_lite_gradient_spreadmode_t spread_mode,
                                            vg_lite_uint8_t pre_multiplied)
    {
        static vg_lite_color_ramp_t default_ramp[] = {
            {
                0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            },
            {
                1.0f,
                1.0f, 1.0f, 1.0f, 1.0f
            }
        };

        vg_lite_uint32_t i, trg_count;
        vg_lite_float_t prev_stop;
        vg_lite_color_ramp_t * src_ramp;
        vg_lite_color_ramp_t * src_ramp_last;
        vg_lite_color_ramp_t * trg_ramp;

        /* Сбросьте счетчик. */
        trg_count = 0;

        if((linear_gradient.X0 == linear_gradient.X1) && (linear_gradient.Y0 == linear_gradient.Y1))
            return VG_LITE_INVALID_ARGUMENT;

        grad->linear_grad = linear_gradient;
        grad->pre_multiplied = pre_multiplied;
        grad->spread_mode = spread_mode;

        if(!count || count > VLC_MAX_COLOR_RAMP_STOPS || color_ramp == NULL)
            goto Empty_sequence_handler;

        for(i = 0; i < count; i++)
            grad->color_ramp[i] = color_ramp[i];
        grad->ramp_length = count;

        /* Определите последнюю исходную рампу. */
        src_ramp_last
            = grad->color_ramp
              + grad->ramp_length;

        /* Установите начальную предыдущую остановку. */
        prev_stop = -1;

        /* Сбросьте счетчик. */
        trg_count = 0;

        /* Пройдите через рампу источника. */
        for(
            src_ramp = grad->color_ramp, trg_ramp = grad->converted_ramp;
            (src_ramp < src_ramp_last) && (trg_count < VLC_MAX_COLOR_RAMP_STOPS + 2);
            src_ramp += 1) {
            /* Должно быть в порядке возрастания. */
            if(src_ramp->stop < prev_stop) {
                /* Игнорируйте всю последовательность. */
                trg_count = 0;
                break;
            }

            /* Обновите предыдущее значение остановки. */
            prev_stop = src_ramp->stop;

            /* Должно находиться в диапазоне [0..1]. */
            if((src_ramp->stop < 0.0f) || (src_ramp->stop > 1.0f)) {
                /* Игнорировать. */
                continue;
            }

            /* Цвет зажима. */
            ClampColor(COLOR_FROM_RAMP(src_ramp), COLOR_FROM_RAMP(trg_ramp), 0);

            /* First stop greater than zero? */
            if((trg_count == 0) && (src_ramp->stop > 0.0f)) {
                /* Принудительно установите первую остановку на 0,0f. */
                trg_ramp->stop = 0.0f;

                /* Повторите запись. */
                trg_ramp[1] = *trg_ramp;
                trg_ramp[1].stop = src_ramp->stop;

                /* Продвижение. */
                trg_ramp += 2;
                trg_count += 2;
            }
            else {
                /* Установите стоповое значение. */
                trg_ramp->stop = src_ramp->stop;

                /* Продвижение. */
                trg_ramp += 1;
                trg_count += 1;
            }
        }

        /* Empty sequence? */
        if(trg_count == 0) {
            memcpy(grad->converted_ramp, default_ramp, sizeof(default_ramp));
            grad->converted_length = sizeof(default_ramp) / 5;
        }
        else {
            /* Последняя остановка должна быть на отметке 1,0. */
            if(trg_ramp[-1].stop != 1.0f) {
                /* Повторите последнюю запись. */
                *trg_ramp = trg_ramp[-1];

                /* Принудительно установите последнюю остановку на 1.0f. */
                trg_ramp->stop = 1.0f;

                /* Обновите итоговое количество записей. */
                trg_count += 1;
            }

            /* Установите новую длину. */
            grad->converted_length = trg_count;
        }
        return VG_LITE_SUCCESS;

Empty_sequence_handler:
        memcpy(grad->converted_ramp, default_ramp, sizeof(default_ramp));
        grad->converted_length = sizeof(default_ramp) / 5;

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_update_linear_grad(vg_lite_ext_linear_gradient_t * grad)
    {
        vg_lite_uint32_t ramp_length;
        vg_lite_color_ramp_t * color_ramp;
        vg_lite_uint32_t common, stop;
        vg_lite_uint32_t i, width;
        uint8_t * bits;
        vg_lite_float_t x0, y0, x1, y1, length;
        vg_lite_error_t error = VG_LITE_SUCCESS;

        /* Получите ярлыки для цветовой шкалы. */
        ramp_length = grad->converted_length;
        color_ramp = grad->converted_ramp;

        x0 = grad->linear_grad.X0;
        y0 = grad->linear_grad.Y0;
        x1 = grad->linear_grad.X1;
        y1 = grad->linear_grad.Y1;
        length = (vg_lite_float_t)sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));

        if(length <= 0)
            return VG_LITE_INVALID_ARGUMENT;
        /* Найдите общий знаменатель остановок цветовой рампы. */
        if(length < 1) {
            common = 1;
        }
        else {
            common = (vg_lite_uint32_t)length;
        }

        for(i = 0; i < ramp_length; ++i) {
            if(color_ramp[i].stop != 0.0f) {
                vg_lite_float_t mul = common * color_ramp[i].stop;
                vg_lite_float_t frac = mul - (vg_lite_float_t)floor(mul);
                if(frac > 0.00013f) { /* Предположим, что ошибка для нуля равна 0,00013. */
                    common = MAX(common, (vg_lite_uint32_t)(1.0f / frac + 0.5f));
                }
            }
        }

        /* Вычислите ширину требуемого цветового массива. */
        width = common + 1;

        /* Выделите поверхность цветовой шкалы. */
        memset(&grad->image, 0, sizeof(grad->image));
        grad->image.width = width;
        grad->image.height = 1;
        grad->image.stride = 0;
        grad->image.image_mode = VG_LITE_NONE_IMAGE_MODE;
        grad->image.format = VG_LITE_ABGR8888;

        /* Выделите изображение для градиента. */
        VG_LITE_RETURN_ERROR(vg_lite_allocate(&grad->image));
        memset(grad->image.memory, 0, grad->image.stride * grad->image.height);
        width = common + 1;
        /* Установите указатель на массив цветов. */
        bits = (uint8_t *)grad->image.memory;

        /* Начните заполнять цветовой массив. */
        stop = 0;
        for(i = 0; i < width; ++i) {
            vg_lite_float_t gradient;
            vg_lite_float_t color[4];
            vg_lite_float_t color1[4];
            vg_lite_float_t color2[4];
            vg_lite_float_t weight;

            if(i == 241)
                i = 241;
            /* Вычислить градиент для текущей записи массива цветов. */
            gradient = (vg_lite_float_t)i / (vg_lite_float_t)(width - 1);

            /* Найдите запись на цветовой шкале, которая соответствует этому значению или превосходит его.
            ** градиент. */
            while(gradient > color_ramp[stop].stop) {
                ++stop;
            }

            if(gradient == color_ramp[stop].stop) {
                /* Идеальное соответствие весу 1.0. */
                weight = 1.0f;

                /* Используйте цвет цветовой шкалы. */
                color1[3] = color_ramp[stop].alpha;
                color1[2] = color_ramp[stop].blue;
                color1[1] = color_ramp[stop].green;
                color1[0] = color_ramp[stop].red;

                color2[3] = color2[2] = color2[1] = color2[0] = 0.0f;
            }
            else {
                if(stop == 0) {
                    return VG_LITE_INVALID_ARGUMENT;
                }
                /* Вычислить вес. */
                weight = (color_ramp[stop].stop - gradient)
                         / (color_ramp[stop].stop - color_ramp[stop - 1].stop);

                /* Захватите цвет цветовой шкалы предыдущей остановки. */
                color1[3] = color_ramp[stop - 1].alpha;
                color1[2] = color_ramp[stop - 1].blue;
                color1[1] = color_ramp[stop - 1].green;
                color1[0] = color_ramp[stop - 1].red;

                /* Захватить цвет цветовой рампы текущей остановки. */
                color2[3] = color_ramp[stop].alpha;
                color2[2] = color_ramp[stop].blue;
                color2[1] = color_ramp[stop].green;
                color2[0] = color_ramp[stop].red;
            }

            if(grad->pre_multiplied) {
                /* Предварительно умножьте первый цвет. */
                color1[2] *= color1[3];
                color1[1] *= color1[3];
                color1[0] *= color1[3];

                /* Предварительно умножьте второй цвет. */
                color2[2] *= color2[3];
                color2[1] *= color2[3];
                color2[0] *= color2[3];
            }

            /* Фильтруйте цвета по каналам. */
            color[3] = LERP(color1[3], color2[3], weight);
            color[2] = LERP(color1[2], color2[2], weight);
            color[1] = LERP(color1[1], color2[1], weight);
            color[0] = LERP(color1[0], color2[0], weight);

            /* Упакуйте окончательный цвет. */
            *bits++ = PackColorComponent(color[3]);
            *bits++ = PackColorComponent(color[2]);
            *bits++ = PackColorComponent(color[1]);
            *bits++ = PackColorComponent(color[0]);
        }

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_draw_linear_grad(vg_lite_buffer_t * target,
                                             vg_lite_path_t * path,
                                             vg_lite_fill_t fill_rule,
                                             vg_lite_matrix_t * path_matrix,
                                             vg_lite_ext_linear_gradient_t * grad,
                                             vg_lite_color_t paint_color,
                                             vg_lite_blend_t blend,
                                             vg_lite_filter_t filter)
    {
        LV_UNUSED(paint_color);
        LV_UNUSED(filter);

        auto ctx = vg_lite_ctx::get_instance();
        TVG_CHECK_RETURN_VG_ERROR(canvas_set_target(ctx, target));

        auto shape = Shape::gen();
        TVG_CHECK_RETURN_VG_ERROR(shape_append_path(shape, path, path_matrix));
        TVG_CHECK_RETURN_VG_ERROR(shape->transform(matrix_conv(path_matrix)));
        TVG_CHECK_RETURN_VG_ERROR(shape->fill(fill_rule_conv(fill_rule)););
        TVG_CHECK_RETURN_VG_ERROR(shape->blend(blend_method_conv(blend)));

        vg_lite_matrix_t grad_matrix;
        TVG_CHECK_RETURN_VG_ERROR(vg_lite_grad_matrix_conv(&grad_matrix, &grad->matrix, path_matrix));

        auto linearGrad = LinearGradient::gen();
        TVG_CHECK_RETURN_VG_ERROR(linearGrad->linear(grad->linear_grad.X0, grad->linear_grad.Y0, grad->linear_grad.X1,
                                                     grad->linear_grad.Y1));
        TVG_CHECK_RETURN_VG_ERROR(linearGrad->transform(matrix_conv(&grad_matrix)));
        TVG_CHECK_RETURN_VG_ERROR(linearGrad->spread(fill_spread_conv(grad->spread_mode)));

        tvg::Fill::ColorStop colorStops[VLC_MAX_COLOR_RAMP_STOPS];
        for(vg_lite_uint32_t i = 0; i < grad->ramp_length; i++) {
            colorStops[i].offset = grad->color_ramp[i].stop;
            colorStops[i].r = grad->color_ramp[i].red * 255.0f;
            colorStops[i].g = grad->color_ramp[i].green * 255.0f;
            colorStops[i].b = grad->color_ramp[i].blue * 255.0f;
            colorStops[i].a = grad->color_ramp[i].alpha * 255.0f;
        }
        TVG_CHECK_RETURN_VG_ERROR(linearGrad->colorStops(colorStops, grad->ramp_length));

        TVG_CHECK_RETURN_VG_ERROR(shape->fill(std::move(linearGrad)));
        TVG_CHECK_RETURN_VG_ERROR(ctx->canvas->push(std::move(shape)));

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_set_radial_grad(vg_lite_radial_gradient_t * grad,
                                            vg_lite_uint32_t count,
                                            vg_lite_color_ramp_t * color_ramp,
                                            vg_lite_radial_gradient_parameter_t radial_grad,
                                            vg_lite_gradient_spreadmode_t spread_mode,
                                            vg_lite_uint8_t pre_multiplied)
    {
        static vg_lite_color_ramp_t defaultRamp[] = {
            {
                0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            },
            {
                1.0f,
                1.0f, 1.0f, 1.0f, 1.0f
            }
        };

        vg_lite_uint32_t i, trgCount;
        vg_lite_float_t prevStop;
        vg_lite_color_ramp_t * srcRamp;
        vg_lite_color_ramp_t * srcRampLast;
        vg_lite_color_ramp_t * trgRamp;

        /* Сбросьте счетчик. */
        trgCount = 0;

        if(radial_grad.r <= 0)
            return VG_LITE_INVALID_ARGUMENT;

        grad->radial_grad = radial_grad;
        grad->pre_multiplied = pre_multiplied;
        grad->spread_mode = spread_mode;

        if(!count || count > VLC_MAX_COLOR_RAMP_STOPS || color_ramp == NULL)
            goto Empty_sequence_handler;

        for(i = 0; i < count; i++)
            grad->color_ramp[i] = color_ramp[i];
        grad->ramp_length = count;

        /* Определите последнюю исходную рампу. */
        srcRampLast
            = grad->color_ramp
              + grad->ramp_length;

        /* Установите начальную предыдущую остановку. */
        prevStop = -1;

        /* Сбросьте счетчик. */
        trgCount = 0;

        /* Пройдите через рампу источника. */
        for(
            srcRamp = grad->color_ramp, trgRamp = grad->converted_ramp;
            (srcRamp < srcRampLast) && (trgCount < VLC_MAX_COLOR_RAMP_STOPS + 2);
            srcRamp += 1) {
            /* Должно быть в порядке возрастания. */
            if(srcRamp->stop < prevStop) {
                /* Игнорируйте всю последовательность. */
                trgCount = 0;
                break;
            }

            /* Обновите предыдущее значение остановки. */
            prevStop = srcRamp->stop;

            /* Должно находиться в диапазоне [0..1]. */
            if((srcRamp->stop < 0.0f) || (srcRamp->stop > 1.0f)) {
                /* Игнорировать. */
                continue;
            }

            /* Цвет зажима. */
            ClampColor(COLOR_FROM_RAMP(srcRamp), COLOR_FROM_RAMP(trgRamp), 0);

            /* First stop greater than zero? */
            if((trgCount == 0) && (srcRamp->stop > 0.0f)) {
                /* Принудительно установите первую остановку на 0,0f. */
                trgRamp->stop = 0.0f;

                /* Повторите запись. */
                trgRamp[1] = *trgRamp;
                trgRamp[1].stop = srcRamp->stop;

                /* Продвижение. */
                trgRamp += 2;
                trgCount += 2;
            }
            else {
                /* Установите стоповое значение. */
                trgRamp->stop = srcRamp->stop;

                /* Продвижение. */
                trgRamp += 1;
                trgCount += 1;
            }
        }

        /* Empty sequence? */
        if(trgCount == 0) {
            memcpy(grad->converted_ramp, defaultRamp, sizeof(defaultRamp));
            grad->converted_length = sizeof(defaultRamp) / 5;
        }
        else {
            /* Последняя остановка должна быть на отметке 1,0. */
            if(trgRamp[-1].stop != 1.0f) {
                /* Повторите последнюю запись. */
                *trgRamp = trgRamp[-1];

                /* Принудительно установите последнюю остановку на 1.0f. */
                trgRamp->stop = 1.0f;

                /* Обновите итоговое количество записей. */
                trgCount += 1;
            }

            /* Установите новую длину. */
            grad->converted_length = trgCount;
        }
        return VG_LITE_SUCCESS;

Empty_sequence_handler:
        memcpy(grad->converted_ramp, defaultRamp, sizeof(defaultRamp));
        grad->converted_length = sizeof(defaultRamp) / 5;

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_update_radial_grad(vg_lite_radial_gradient_t * grad)
    {
        vg_lite_uint32_t ramp_length;
        vg_lite_color_ramp_t * colorRamp;
        vg_lite_uint32_t common, stop;
        vg_lite_uint32_t i, width;
        uint8_t * bits;
        vg_lite_error_t error = VG_LITE_SUCCESS;
        vg_lite_uint32_t align, mul, div;

        /* Получите ярлыки для цветовой шкалы. */
        ramp_length = grad->converted_length;
        colorRamp = grad->converted_ramp;

        if(grad->radial_grad.r <= 0)
            return VG_LITE_INVALID_ARGUMENT;

        /* Найдите общий знаменатель остановок цветовой рампы. */
        if(grad->radial_grad.r < 1) {
            common = 1;
        }
        else {
            common = (vg_lite_uint32_t)grad->radial_grad.r;
        }

        for(i = 0; i < ramp_length; ++i) {
            if(colorRamp[i].stop != 0.0f) {
                vg_lite_float_t m = common * colorRamp[i].stop;
                vg_lite_float_t frac = m - (vg_lite_float_t)floor(m);
                if(frac > 0.00013f) { /* Предположим, что ошибка для нуля равна 0,00013. */
                    common = MAX(common, (vg_lite_uint32_t)(1.0f / frac + 0.5f));
                }
            }
        }

        /* Вычислите ширину требуемого цветового массива. */
        width = common + 1;
        width = (width + 15) & (~0xf);

        /* Выделите поверхность цветовой шкалы. */
        memset(&grad->image, 0, sizeof(grad->image));
        grad->image.width = width;
        grad->image.height = 1;
        grad->image.stride = 0;
        grad->image.image_mode = VG_LITE_NONE_IMAGE_MODE;
        grad->image.format = VG_LITE_ABGR8888;

        /* Выделите изображение для градиента. */
        VG_LITE_RETURN_ERROR(vg_lite_allocate(&grad->image));

        get_format_bytes(VG_LITE_ABGR8888, &mul, &div, &align);
        width = grad->image.stride * div / mul;

        /* Установите указатель на массив цветов. */
        bits = (uint8_t *)grad->image.memory;

        /* Начните заполнять цветовой массив. */
        stop = 0;
        for(i = 0; i < width; ++i) {
            vg_lite_float_t gradient;
            vg_lite_float_t color[4];
            vg_lite_float_t color1[4];
            vg_lite_float_t color2[4];
            vg_lite_float_t weight;

            /* Вычислить градиент для текущей записи массива цветов. */
            gradient = (vg_lite_float_t)i / (vg_lite_float_t)(width - 1);

            /* Найдите запись на цветовой шкале, которая соответствует этому значению или превосходит его.
            ** градиент. */
            while(gradient > colorRamp[stop].stop) {
                ++stop;
            }

            if(gradient == colorRamp[stop].stop) {
                /* Идеальное соответствие весу 1.0. */
                weight = 1.0f;

                /* Используйте цвет цветовой шкалы. */
                color1[3] = colorRamp[stop].alpha;
                color1[2] = colorRamp[stop].blue;
                color1[1] = colorRamp[stop].green;
                color1[0] = colorRamp[stop].red;

                color2[3] = color2[2] = color2[1] = color2[0] = 0.0f;
            }
            else {
                /* Вычислить вес. */
                weight = (colorRamp[stop].stop - gradient)
                         / (colorRamp[stop].stop - colorRamp[stop - 1].stop);

                /* Захватите цвет цветовой шкалы предыдущей остановки. */
                color1[3] = colorRamp[stop - 1].alpha;
                color1[2] = colorRamp[stop - 1].blue;
                color1[1] = colorRamp[stop - 1].green;
                color1[0] = colorRamp[stop - 1].red;

                /* Захватить цвет цветовой рампы текущей остановки. */
                color2[3] = colorRamp[stop].alpha;
                color2[2] = colorRamp[stop].blue;
                color2[1] = colorRamp[stop].green;
                color2[0] = colorRamp[stop].red;
            }

            if(grad->pre_multiplied) {
                /* Предварительно умножьте первый цвет. */
                color1[2] *= color1[3];
                color1[1] *= color1[3];
                color1[0] *= color1[3];

                /* Предварительно умножьте второй цвет. */
                color2[2] *= color2[3];
                color2[1] *= color2[3];
                color2[0] *= color2[3];
            }

            /* Фильтруйте цвета по каналам. */
            color[3] = LERP(color1[3], color2[3], weight);
            color[2] = LERP(color1[2], color2[2], weight);
            color[1] = LERP(color1[1], color2[1], weight);
            color[0] = LERP(color1[0], color2[0], weight);

            /* Упакуйте окончательный цвет. */
            *bits++ = PackColorComponent(color[3]);
            *bits++ = PackColorComponent(color[2]);
            *bits++ = PackColorComponent(color[1]);
            *bits++ = PackColorComponent(color[0]);
        }

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_set_grad(vg_lite_linear_gradient_t * grad,
                                     vg_lite_uint32_t count,
                                     vg_lite_uint32_t * colors,
                                     vg_lite_uint32_t * stops)
    {
        vg_lite_uint32_t i;

        grad->count = 0; /* Непрозрачный черно-белый градиент */
        if(!count || count > VLC_MAX_GRADIENT_STOPS || colors == NULL || stops == NULL)
            return VG_LITE_SUCCESS;

        /* Проверка прекращает действие */
        for(i = 0; i < count; i++)
            if(stops[i] < VLC_GRADIENT_BUFFER_WIDTH) {
                if(!grad->count || stops[i] > grad->stops[grad->count - 1]) {
                    grad->stops[grad->count] = stops[i];
                    grad->colors[grad->count] = colors[i];
                    grad->count++;
                }
                else if(stops[i] == grad->stops[grad->count - 1]) {
                    /* Равные остановки: используйте цвет, соответствующий последней остановке.
                    в последовательности */
                    grad->colors[grad->count - 1] = colors[i];
                }
            }

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_update_grad(vg_lite_linear_gradient_t * grad)
    {
        vg_lite_error_t error = VG_LITE_SUCCESS;
        int32_t r0, g0, b0, a0;
        int32_t r1, g1, b1, a1;
        int32_t lr, lg, lb, la;
        vg_lite_uint32_t i;
        int32_t j;
        int32_t ds, dr, dg, db, da;
        vg_lite_uint32_t * buffer = (vg_lite_uint32_t *)grad->image.memory;

        if(grad->count == 0) {
            /* Если не было указано допустимых остановок (например, из-за пустого ввода
             * остановки массива, вне диапазона или вне порядка), остановка в 0 с цветом
             * 0xFF000000 (непрозрачный черный) и остановка на 255 с цветом 0xFFFFFFFF
             * (непрозрачный белый) определены неявно. */
            grad->stops[0] = 0;
            grad->colors[0] = 0xFF000000; /* Непрозрачный черный */
            grad->stops[1] = 255;
            grad->colors[1] = 0xFFFFFFFF; /* Непрозрачный белый */
            grad->count = 2;
        }
        else if(grad->count && grad->stops[0] != 0) {
            /* Если была указана хотя бы одна действительная остановка, но ни одна не была
             * определено со смещением 0, неявная остановка добавляется с помощью
             * смещение равно 0 и того же цвета, что и первая определяемая пользователем остановка. */
            for(i = 0; i < grad->stops[0]; i++)
                buffer[i] = grad->colors[0];
        }
        a0 = A(grad->colors[0]);
        r0 = R(grad->colors[0]);
        g0 = G(grad->colors[0]);
        b0 = B(grad->colors[0]);

        /* Рассчитайте цвета для каждого пикселя изображения. */
        for(i = 0; i < grad->count - 1; i++) {
            buffer[grad->stops[i]] = grad->colors[i];
            ds = grad->stops[i + 1] - grad->stops[i];
            a1 = A(grad->colors[i + 1]);
            r1 = R(grad->colors[i + 1]);
            g1 = G(grad->colors[i + 1]);
            b1 = B(grad->colors[i + 1]);

            da = a1 - a0;
            dr = r1 - r0;
            dg = g1 - g0;
            db = b1 - b0;

            for(j = 1; j < ds; j++) {
                la = a0 + da * j / ds;
                lr = r0 + dr * j / ds;
                lg = g0 + dg * j / ds;
                lb = b0 + db * j / ds;

                buffer[grad->stops[i] + j] = ARGB(la, lr, lg, lb);
            }

            a0 = a1;
            r0 = r1;
            g0 = g1;
            b0 = b1;
        }

        /* Если была указана хотя бы одна допустимая остановка, но ни одна не была определена
         * со смещением 255 добавляется неявный стоп со смещением 255
         * и того же цвета, что и последняя остановка, определенная пользователем. */
        for(i = grad->stops[grad->count - 1]; i < VLC_GRADIENT_BUFFER_WIDTH; i++)
            buffer[i] = grad->colors[grad->count - 1];

        return error;
    }

    vg_lite_error_t vg_lite_clear_linear_grad(vg_lite_ext_linear_gradient_t * grad)
    {
        vg_lite_error_t error = VG_LITE_SUCCESS;

        grad->count = 0;
        /* Освободите ресурс изображения. */
        if(grad->image.handle != NULL) {
            error = vg_lite_free(&grad->image);
        }

        return error;
    }

    vg_lite_error_t vg_lite_clear_grad(vg_lite_linear_gradient_t * grad)
    {
        vg_lite_error_t error = VG_LITE_SUCCESS;

        grad->count = 0;
        /* Освободите ресурс изображения. */
        if(grad->image.handle != NULL) {
            error = vg_lite_free(&grad->image);
        }

        return error;
    }

    vg_lite_error_t vg_lite_clear_radial_grad(vg_lite_radial_gradient_t * grad)
    {
        vg_lite_error_t error = VG_LITE_SUCCESS;

        grad->count = 0;
        /* Освободите ресурс изображения. */
        if(grad->image.handle != NULL) {
            error = vg_lite_free(&grad->image);
        }

        return error;
    }

    vg_lite_matrix_t * vg_lite_get_linear_grad_matrix(vg_lite_ext_linear_gradient_t * grad)
    {
        return &grad->matrix;
    }

    vg_lite_matrix_t * vg_lite_get_grad_matrix(vg_lite_linear_gradient_t * grad)
    {
        return &grad->matrix;
    }

    vg_lite_matrix_t * vg_lite_get_radial_grad_matrix(vg_lite_radial_gradient_t * grad)
    {
        return &grad->matrix;
    }

    vg_lite_error_t vg_lite_draw_grad(vg_lite_buffer_t * target,
                                      vg_lite_path_t * path,
                                      vg_lite_fill_t fill_rule,
                                      vg_lite_matrix_t * matrix,
                                      vg_lite_linear_gradient_t * grad,
                                      vg_lite_blend_t blend)
    {
        auto ctx = vg_lite_ctx::get_instance();
        TVG_CHECK_RETURN_VG_ERROR(canvas_set_target(ctx, target));

        auto shape = Shape::gen();
        TVG_CHECK_RETURN_VG_ERROR(shape_append_path(shape, path, matrix));
        TVG_CHECK_RETURN_VG_ERROR(shape->transform(matrix_conv(matrix)));
        TVG_CHECK_RETURN_VG_ERROR(shape->fill(fill_rule_conv(fill_rule)););
        TVG_CHECK_RETURN_VG_ERROR(shape->blend(blend_method_conv(blend)));

        vg_lite_matrix_t grad_matrix;
        TVG_CHECK_RETURN_VG_ERROR(vg_lite_grad_matrix_conv(&grad_matrix, &grad->matrix, matrix));

        vg_lite_fpoint_t p1 = {0.0f, 0.0f};
        vg_lite_fpoint_t p2 = {1.0f, 0};

        vg_lite_fpoint_t p1_trans = p1;
        vg_lite_fpoint_t p2_trans = p2;

        p1_trans = matrix_transform_point(&grad_matrix, &p1);
        p2_trans = matrix_transform_point(&grad_matrix, &p2);
        float dx = (p2_trans.x - p1_trans.x);
        float dy = (p2_trans.y - p1_trans.y);
        float scale = sqrtf(dx * dx + dy * dy);
        float angle = (float)(atan2f(dy, dx));
        float dlen = 256 * scale;
        float x_min = grad_matrix.m[0][2];
        float y_min = grad_matrix.m[1][2];
        float x_max = x_min + dlen * cosf(angle);
        float y_max = y_min + dlen * sinf(angle);
        LV_LOG_TRACE("linear gradient {%.2f, %.2f} ~ {%.2f, %.2f}", x_min, y_min, x_max, y_max);
        auto linearGrad = LinearGradient::gen();
        TVG_CHECK_RETURN_VG_ERROR(linearGrad->linear(x_min, y_min, x_max, y_max));
        TVG_CHECK_RETURN_VG_ERROR(linearGrad->spread(FillSpread::Pad));

        tvg::Fill::ColorStop colorStops[VLC_MAX_GRADIENT_STOPS];
        for(vg_lite_uint32_t i = 0; i < grad->count; i++) {
            colorStops[i].offset = grad->stops[i] / 255.0f;
            colorStops[i].r = R(grad->colors[i]);
            colorStops[i].g = G(grad->colors[i]);
            colorStops[i].b = B(grad->colors[i]);
            colorStops[i].a = A(grad->colors[i]);
        }
        TVG_CHECK_RETURN_VG_ERROR(linearGrad->colorStops(colorStops, grad->count));

        TVG_CHECK_RETURN_VG_ERROR(shape->fill(std::move(linearGrad)));
        TVG_CHECK_RETURN_VG_ERROR(ctx->canvas->push(std::move(shape)));

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_draw_radial_grad(vg_lite_buffer_t * target,
                                             vg_lite_path_t * path,
                                             vg_lite_fill_t fill_rule,
                                             vg_lite_matrix_t * path_matrix,
                                             vg_lite_radial_gradient_t * grad,
                                             vg_lite_color_t paint_color,
                                             vg_lite_blend_t blend,
                                             vg_lite_filter_t filter)
    {
        LV_UNUSED(paint_color);
        LV_UNUSED(filter);

        auto ctx = vg_lite_ctx::get_instance();
        TVG_CHECK_RETURN_VG_ERROR(canvas_set_target(ctx, target));

        auto shape = Shape::gen();
        TVG_CHECK_RETURN_VG_ERROR(shape_append_path(shape, path, path_matrix));
        TVG_CHECK_RETURN_VG_ERROR(shape->transform(matrix_conv(path_matrix)));
        TVG_CHECK_RETURN_VG_ERROR(shape->fill(fill_rule_conv(fill_rule)););
        TVG_CHECK_RETURN_VG_ERROR(shape->blend(blend_method_conv(blend)));

        vg_lite_matrix_t grad_matrix;
        TVG_CHECK_RETURN_VG_ERROR(vg_lite_grad_matrix_conv(&grad_matrix, &grad->matrix, path_matrix));

        auto radialGrad = RadialGradient::gen();
        TVG_CHECK_RETURN_VG_ERROR(radialGrad->transform(matrix_conv(&grad_matrix)));
        TVG_CHECK_RETURN_VG_ERROR(radialGrad->radial(grad->radial_grad.cx, grad->radial_grad.cy, grad->radial_grad.r));
        TVG_CHECK_RETURN_VG_ERROR(radialGrad->spread(fill_spread_conv(grad->spread_mode)));

        tvg::Fill::ColorStop colorStops[VLC_MAX_COLOR_RAMP_STOPS];
        for(vg_lite_uint32_t i = 0; i < grad->ramp_length; i++) {
            colorStops[i].offset = grad->color_ramp[i].stop;
            colorStops[i].r = grad->color_ramp[i].red * 255.0f;
            colorStops[i].g = grad->color_ramp[i].green * 255.0f;
            colorStops[i].b = grad->color_ramp[i].blue * 255.0f;
            colorStops[i].a = grad->color_ramp[i].alpha * 255.0f;
        }
        TVG_CHECK_RETURN_VG_ERROR(radialGrad->colorStops(colorStops, grad->ramp_length));

        TVG_CHECK_RETURN_VG_ERROR(shape->fill(std::move(radialGrad)));
        TVG_CHECK_RETURN_VG_ERROR(ctx->canvas->push(std::move(shape)));

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_set_command_buffer_size(vg_lite_uint32_t size)
    {
        LV_UNUSED(size);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_set_scissor(vg_lite_int32_t x, vg_lite_int32_t y, vg_lite_int32_t right, vg_lite_int32_t bottom)
    {
        auto ctx = vg_lite_ctx::get_instance();
        vg_lite_int32_t width = right - x;
        vg_lite_int32_t height = bottom - y;

        if(width <= 0 || height <= 0) {
            return VG_LITE_INVALID_ARGUMENT;
        }

        if(ctx->scissor_rect.x == x && ctx->scissor_rect.y == y &&
           ctx->scissor_rect.width == width && ctx->scissor_rect.height == height) {
            return VG_LITE_SUCCESS;
        }

        /*Завершите предыдущий рендеринг, прежде чем устанавливать новые ножницы.*/
        vg_lite_error_t error;
        VG_LITE_RETURN_ERROR(vg_lite_finish());

        ctx->scissor_rect.x = x;
        ctx->scissor_rect.y = y;
        ctx->scissor_rect.width = width;
        ctx->scissor_rect.height = height;
        ctx->scissor_is_set = true;
        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_enable_scissor(void)
    {
        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_disable_scissor(void)
    {
        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_get_mem_size(vg_lite_uint32_t * size)
    {
        *size = 0;
        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_source_global_alpha(vg_lite_global_alpha_t alpha_mode, uint8_t alpha_value)
    {
        LV_UNUSED(alpha_mode);
        LV_UNUSED(alpha_value);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_dest_global_alpha(vg_lite_global_alpha_t alpha_mode, uint8_t alpha_value)
    {
        LV_UNUSED(alpha_mode);
        LV_UNUSED(alpha_value);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_set_color_key(vg_lite_color_key4_t colorkey)
    {
        LV_UNUSED(colorkey);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_set_flexa_stream_id(uint8_t stream_id)
    {
        LV_UNUSED(stream_id);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_set_flexa_current_background_buffer(uint8_t stream_id,
                                                                vg_lite_buffer_t * buffer,
                                                                vg_lite_uint32_t background_segment_count,
                                                                vg_lite_uint32_t background_segment_size)
    {
        LV_UNUSED(stream_id);
        LV_UNUSED(buffer);
        LV_UNUSED(background_segment_count);
        LV_UNUSED(background_segment_size);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_enable_flexa(void)
    {
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_disable_flexa(void)
    {
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_set_flexa_stop_frame(void)
    {
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_enable_dither(void)
    {
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_disable_dither(void)
    {
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_set_tess_buffer(vg_lite_uint32_t physical, vg_lite_uint32_t size)
    {
        LV_UNUSED(physical);
        LV_UNUSED(size);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_set_command_buffer(vg_lite_uint32_t physical, vg_lite_uint32_t size)
    {
        LV_UNUSED(physical);
        LV_UNUSED(size);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_get_parameter(vg_lite_param_type_t type,
                                          vg_lite_int32_t count,
                                          vg_lite_float_t * params)
    {
        switch(type) {
            case VG_LITE_GPU_IDLE_STATE:
                if(count != 1 || params == NULL) {
                    return VG_LITE_INVALID_ARGUMENT;
                }

                *(vg_lite_uint32_t *)params = 1;
                return VG_LITE_SUCCESS;

            default:
                break;
        }

        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_dump_command_buffer(void)
    {
        LV_LOG_USER("command:");
        LV_LOG_USER("@[commit]");
        return VG_LITE_SUCCESS;
    }
} /*extern "C"*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static vg_lite_error_t vg_lite_error_conv(Result result)
{
    switch(result) {
        case Result::Success:
            return VG_LITE_SUCCESS;

        case Result::InvalidArguments:
            return VG_LITE_INVALID_ARGUMENT;

        case Result::InsufficientCondition:
            return VG_LITE_OUT_OF_RESOURCES;

        case Result::FailedAllocation:
            return VG_LITE_OUT_OF_MEMORY;

        case Result::NonSupport:
            return VG_LITE_NOT_SUPPORT;

        default:
            break;
    }

    return VG_LITE_TIMEOUT;
}

static Matrix matrix_conv(const vg_lite_matrix_t * matrix)
{
    static const Matrix identity = {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    };
    if(!matrix) {
        return identity;
    }

    return *(Matrix *)matrix;
}

static FillRule fill_rule_conv(vg_lite_fill_t fill)
{
    if(fill == VG_LITE_FILL_EVEN_ODD) {
        return FillRule::EvenOdd;
    }

    return FillRule::Winding;
}

static BlendMethod blend_method_conv(vg_lite_blend_t blend)
{
    switch(blend) {
        case VG_LITE_BLEND_NONE:
            return BlendMethod::SrcOver;

        case VG_LITE_BLEND_NORMAL_LVGL:
            return BlendMethod::Normal;

        case VG_LITE_BLEND_SRC_OVER:
            return BlendMethod::Normal;

        case VG_LITE_BLEND_SCREEN:
            return BlendMethod::Screen;

        case VG_LITE_BLEND_ADDITIVE:
            return BlendMethod::Add;

        case VG_LITE_BLEND_MULTIPLY:
            return BlendMethod::Multiply;

        default:
            break;
    }

    return BlendMethod::Normal;
}

static StrokeCap stroke_cap_conv(vg_lite_cap_style_t cap)
{
    switch(cap) {
        case VG_LITE_CAP_SQUARE:
            return StrokeCap::Square;
        case VG_LITE_CAP_ROUND:
            return StrokeCap::Round;
        case VG_LITE_CAP_BUTT:
            return StrokeCap::Butt;
        default:
            break;
    }

    return StrokeCap::Square;
}

static StrokeJoin stroke_join_conv(vg_lite_join_style_t join)
{
    switch(join) {
        case VG_LITE_JOIN_BEVEL:
            return StrokeJoin::Bevel;
        case VG_LITE_JOIN_ROUND:
            return StrokeJoin::Round;
        case VG_LITE_JOIN_MITER:
            return StrokeJoin::Miter;
        default:
            break;
    }

    return StrokeJoin::Bevel;
}

static FillSpread fill_spread_conv(vg_lite_gradient_spreadmode_t spread)
{
    switch(spread) {
        case VG_LITE_GRADIENT_SPREAD_PAD:
            return FillSpread::Pad;
        case VG_LITE_GRADIENT_SPREAD_REPEAT:
            return FillSpread::Repeat;
        case VG_LITE_GRADIENT_SPREAD_REFLECT:
            return FillSpread::Reflect;
        default:
            return FillSpread::Pad;
    }
}

static float vlc_get_arg(const void * data, vg_lite_format_t format)
{
    switch(format) {
        case VG_LITE_S8:
            return *((int8_t *)data);

        case VG_LITE_S16:
            return *((int16_t *)data);

        case VG_LITE_S32:
            return *((int32_t *)data);

        case VG_LITE_FP32:
            return *((float *)data);

        default:
            LV_LOG_ERROR("UNKNOWN_FORMAT: %d", format);
            break;
    }

    return 0;
}

static uint8_t vlc_format_len(vg_lite_format_t format)
{
    switch(format) {
        case VG_LITE_S8:
            return 1;
        case VG_LITE_S16:
            return 2;
        case VG_LITE_S32:
            return 4;
        case VG_LITE_FP32:
            return 4;
        default:
            LV_LOG_ERROR("UNKNOWN_FORMAT: %d", format);
            LV_ASSERT(false);
            break;
    }

    return 0;
}

static uint8_t vlc_op_arg_len(uint8_t vlc_op)
{
    switch(vlc_op) {
            VLC_OP_ARG_LEN(END, 0);
            VLC_OP_ARG_LEN(CLOSE, 0);
            VLC_OP_ARG_LEN(MOVE, 2);
            VLC_OP_ARG_LEN(MOVE_REL, 2);
            VLC_OP_ARG_LEN(LINE, 2);
            VLC_OP_ARG_LEN(LINE_REL, 2);
            VLC_OP_ARG_LEN(QUAD, 4);
            VLC_OP_ARG_LEN(QUAD_REL, 4);
            VLC_OP_ARG_LEN(CUBIC, 6);
            VLC_OP_ARG_LEN(CUBIC_REL, 6);
            VLC_OP_ARG_LEN(SCCWARC, 5);
            VLC_OP_ARG_LEN(SCCWARC_REL, 5);
            VLC_OP_ARG_LEN(SCWARC, 5);
            VLC_OP_ARG_LEN(SCWARC_REL, 5);
            VLC_OP_ARG_LEN(LCCWARC, 5);
            VLC_OP_ARG_LEN(LCCWARC_REL, 5);
            VLC_OP_ARG_LEN(LCWARC, 5);
            VLC_OP_ARG_LEN(LCWARC_REL, 5);
        default:
            LV_LOG_ERROR("UNKNOWN_VLC_OP: 0x%x", vlc_op);
            LV_ASSERT(false);
            break;
    }

    return 0;
}

static Result shape_set_stroke(std::unique_ptr<Shape> & shape, const vg_lite_path_t * path)
{
    switch(path->path_type) {
        case VG_LITE_DRAW_ZERO:
        case VG_LITE_DRAW_FILL_PATH:
            /* если путь не является штрихом, верните */
            return Result::Success;

        case VG_LITE_DRAW_STROKE_PATH:
        case VG_LITE_DRAW_FILL_STROKE_PATH:
            break;

        default:
            LV_LOG_ERROR("unknown path type: %d", path->path_type);
            return Result::InvalidArguments;
    }

    LV_ASSERT_NULL(path->stroke);
    TVG_CHECK_RETURN_RESULT(shape->stroke(path->stroke->line_width));
    TVG_CHECK_RETURN_RESULT(shape->strokeMiterlimit(path->stroke->miter_limit));
    TVG_CHECK_RETURN_RESULT(shape->stroke(stroke_cap_conv(path->stroke->cap_style)));
    TVG_CHECK_RETURN_RESULT(shape->stroke(stroke_join_conv(path->stroke->join_style)));
    TVG_CHECK_RETURN_RESULT(shape->stroke(TVG_COLOR(path->stroke_color)));

    if(path->stroke->pattern_count) {
        LV_ASSERT_NULL(path->stroke->dash_pattern);
        TVG_CHECK_RETURN_RESULT(shape->stroke(path->stroke->dash_pattern, path->stroke->pattern_count));
    }

    return Result::Success;
}

static Result shape_append_path(std::unique_ptr<Shape> & shape, vg_lite_path_t * path, vg_lite_matrix_t * matrix)
{
    uint8_t fmt_len = vlc_format_len(path->format);
    uint8_t * cur = (uint8_t *)path->path;
    uint8_t * end = cur + path->path_length;

    while(cur < end) {
        /* получить код операции */
        uint8_t op_code = VLC_GET_OP_CODE(cur);

        /* получить длину аргументов */
        uint8_t arg_len = vlc_op_arg_len(op_code);

        /* пропустить код операции */
        cur += fmt_len;

        switch(op_code) {
            case VLC_OP_MOVE: {
                    float x = VLC_GET_ARG(cur, 0);
                    float y = VLC_GET_ARG(cur, 1);
                    TVG_CHECK_RETURN_RESULT(shape->moveTo(x, y));
                }
                break;

            case VLC_OP_LINE: {
                    float x = VLC_GET_ARG(cur, 0);
                    float y = VLC_GET_ARG(cur, 1);
                    TVG_CHECK_RETURN_RESULT(shape->lineTo(x, y));
                }
                break;

            case VLC_OP_QUAD: {
                    /* взломать предварительную точку */
                    float qcx0 = VLC_GET_ARG(cur, -3);
                    float qcy0 = VLC_GET_ARG(cur, -2);
                    float qcx1 = VLC_GET_ARG(cur, 0);
                    float qcy1 = VLC_GET_ARG(cur, 1);
                    float x = VLC_GET_ARG(cur, 2);
                    float y = VLC_GET_ARG(cur, 3);

                    qcx0 += (qcx1 - qcx0) * 2 / 3;
                    qcy0 += (qcy1 - qcy0) * 2 / 3;
                    qcx1 = x + (qcx1 - x) * 2 / 3;
                    qcy1 = y + (qcy1 - y) * 2 / 3;

                    TVG_CHECK_RETURN_RESULT(shape->cubicTo(qcx0, qcy0, qcx1, qcy1, x, y));
                }
                break;

            case VLC_OP_CUBIC: {
                    float cx1 = VLC_GET_ARG(cur, 0);
                    float cy1 = VLC_GET_ARG(cur, 1);
                    float cx2 = VLC_GET_ARG(cur, 2);
                    float cy2 = VLC_GET_ARG(cur, 3);
                    float x = VLC_GET_ARG(cur, 4);
                    float y = VLC_GET_ARG(cur, 5);
                    TVG_CHECK_RETURN_RESULT(shape->cubicTo(cx1, cy1, cx2, cy2, x, y));
                }
                break;

            case VLC_OP_CLOSE:
                TVG_CHECK_RETURN_RESULT(shape->close());
                break;

            default:
                break;
        }

        cur += arg_len * fmt_len;
    }

    TVG_CHECK_RETURN_RESULT(shape_set_stroke(shape, path));

    float x_min = path->bounding_box[0];
    float y_min = path->bounding_box[1];
    float x_max = path->bounding_box[2];
    float y_max = path->bounding_box[3];

    if(math_equal(x_min, -FLT_MAX) && math_equal(y_min, -FLT_MAX)
       && math_equal(x_max, FLT_MAX) && math_equal(y_max, FLT_MAX)) {
        return Result::Success;
    }

    auto cilp = Shape::gen();
    TVG_CHECK_RETURN_RESULT(cilp->appendRect(x_min, y_min, x_max - x_min, y_max - y_min, 0, 0));
    TVG_CHECK_RETURN_RESULT(cilp->transform(matrix_conv(matrix)));
    TVG_CHECK_RETURN_RESULT(shape->composite(std::move(cilp), CompositeMethod::ClipPath));

    return Result::Success;
}

static Result shape_append_rect(std::unique_ptr<Shape> & shape, const vg_lite_buffer_t * target,
                                const vg_lite_rectangle_t * rect)
{
    if(rect) {
        TVG_CHECK_RETURN_RESULT(shape->appendRect(rect->x, rect->y, rect->width, rect->height, 0, 0));
    }
    else if(target) {
        TVG_CHECK_RETURN_RESULT(shape->appendRect(0, 0, target->width, target->height, 0, 0));
    }
    else {
        return Result::InvalidArguments;
    }

    return Result::Success;
}

static Result canvas_set_target(vg_lite_ctx * ctx, vg_lite_buffer_t * target)
{
    /* еслиtarget_bufferнеобходимо изменить, завершите данный рисунок */
    if(ctx->target_buffer && ctx->target_buffer != target->memory) {
        vg_lite_finish();
    }

    ctx->target_buffer = target->memory;
    ctx->target_format = target->format;
    ctx->target_px_size = target->width * target->height;

    void * canvas_target_buffer;
    uint32_t stride = 0;

    if(TVG_IS_VG_FMT_SUPPORT(target->format)) {
        /* если целевой формат поддерживается VG, используйте целевой буфер напрямую */
        canvas_target_buffer = target->memory;

        /* поддержка целевого шага */
        LV_ASSERT(target->stride >= target->width);
        LV_ASSERT(VG_LITE_IS_ALIGNED(target->stride, sizeof(uint32_t)));
        stride = target->stride / sizeof(uint32_t);
    }
    else {
        /* если целевой формат не поддерживается VG, используйте внутренний буфер */
        canvas_target_buffer = ctx->get_temp_target_buffer(target->width, target->height);
        stride = target->width;
    }

    /* Предотвратить повторную установку цели */
    if(ctx->tvg_target_buffer == canvas_target_buffer) {
        return Result::Success;
    }

    ctx->tvg_target_buffer = canvas_target_buffer;

    TVG_CHECK_RETURN_RESULT(ctx->canvas->target(
                                (uint32_t *)ctx->tvg_target_buffer,
                                stride,
                                target->width,
                                target->height,
                                SwCanvas::ARGB8888));

    if(ctx->scissor_is_set) {
        TVG_CHECK_RETURN_RESULT(
            ctx->canvas->viewport(
                ctx->scissor_rect.x, ctx->scissor_rect.y,
                ctx->scissor_rect.width, ctx->scissor_rect.height));
    }

    return Result::Success;
}

static bool decode_indexed_line(
    vg_lite_buffer_format_t color_format,
    const vg_lite_uint32_t * palette,
    int32_t x, int32_t y,
    int32_t w_px, uint32_t stride, const uint8_t * in, vg_lite_uint32_t * out)
{
    uint8_t px_size;
    uint16_t mask;

    in += stride * y; /*Первый пиксель*/
    out += w_px * y;

    int8_t shift = 0;
    switch(color_format) {
        case VG_LITE_INDEX_1:
            px_size = 1;
            in += x / 8; /*8 пикселей на байт*/
            shift = 7 - (x & 0x7);
            break;
        case VG_LITE_INDEX_2:
            px_size = 2;
            in += x / 4; /*4 пикселя на байт*/
            shift = 6 - 2 * (x & 0x3);
            break;
        case VG_LITE_INDEX_4:
            px_size = 4;
            in += x / 2; /*2 пикселя на байт*/
            shift = 4 - 4 * (x & 0x1);
            break;
        case VG_LITE_INDEX_8:
            px_size = 8;
            in += x;
            shift = 0;
            break;
        default:
            LV_ASSERT(false);
            return false;
    }

    mask = (1 << px_size) - 1; /*Например.  px_size= 2; маска = 0x03*/

    int32_t i;
    for(i = 0; i < w_px; i++) {
        uint8_t val_act = (*in >> shift) & mask;
        out[i] = palette[val_act];

        shift -= px_size;
        if(shift < 0) {
            shift = 8 - px_size;
            in++;
        }
    }
    return true;
}

static Result picture_load(vg_lite_ctx * ctx, std::unique_ptr<Picture> & picture, const vg_lite_buffer_t * source,
                           vg_lite_color_t color)
{
    vg_lite_uint32_t * image_buffer;

    /* Выравнивание не менее 8 байт */
    LV_ASSERT(VG_LITE_IS_ALIGNED(source->memory, 8));

    /**
     * поскольку картинка->загрузка ThorVG не поддерживает шаг,
     * повторное преобразование требуется, когда шаг и ширина не совпадают.
     */
    if(source->format == VG_LITE_BGRA8888
       && source->image_mode == VG_LITE_NORMAL_IMAGE_MODE
       && (size_t)source->stride == (size_t)(source->width * sizeof(vg_lite_uint32_t))) {
        image_buffer = (vg_lite_uint32_t *)source->memory;
    }
    else {
        vg_lite_uint32_t width = source->width;
        vg_lite_uint32_t height = source->height;
        image_buffer = ctx->get_image_buffer(width, height);

        vg_lite_buffer_t target;
        memset(&target, 0, sizeof(target));
        target.memory = image_buffer;
        target.format = VG_LITE_BGRA8888;
        target.width = width;
        target.height = height;
        target.stride = width * sizeof(vg_lite_uint32_t);

        switch(source->format) {
            case VG_LITE_INDEX_1:
            case VG_LITE_INDEX_2:
            case VG_LITE_INDEX_4:
            case VG_LITE_INDEX_8: {
                    const vg_lite_uint32_t * clut_colors = ctx->get_CLUT(source->format);
                    for(vg_lite_uint32_t y = 0; y < height; y++) {
                        decode_indexed_line(source->format, clut_colors, 0, y, width, source->stride, (uint8_t *)source->memory, image_buffer);
                    }
                }
                break;

            case VG_LITE_A4: {
                    conv_alpha4_to_bgra8888.convert(&target, source, color);
                }
                break;

            case VG_LITE_A8: {
                    conv_alpha8_to_bgra8888.convert(&target, source, color);
                }
                break;

            case VG_LITE_L8: {
                    conv_l8_to_bgra8888.convert(&target, source);
                }
                break;

            case VG_LITE_BGRX8888: {
                    conv_bgrx8888_to_bgra8888.convert(&target, source);
                }
                break;

            case VG_LITE_BGR888: {
                    conv_bgr888_to_bgra8888.convert(&target, source);
                }
                break;

            case VG_LITE_BGRA5658: {
                    conv_bgra5658_to_bgra8888.convert(&target, source);
                }
                break;

            case VG_LITE_BGR565: {
                    conv_bgr565_to_bgra8888.convert(&target, source);
                }
                break;

            case VG_LITE_BGRA5551: {
                    conv_bgra5551_to_bgra8888.convert(&target, source);
                }
                break;

            case VG_LITE_BGRA4444: {
                    conv_bgra4444_to_bgra8888.convert(&target, source);
                }
                break;

            case VG_LITE_BGRA2222: {
                    conv_bgra2222_to_bgra8888.convert(&target, source);
                }
                break;

#if LV_VG_LITE_THORVG_YUV_SUPPORT
            case VG_LITE_NV12: {
                    libyuv::NV12ToARGB((const uint8_t *)source->memory, source->stride, (const uint8_t *)source->yuv.uv_memory,
                                       source->yuv.uv_stride,
                                       (uint8_t *)image_buffer, source->width * sizeof(vg_lite_uint32_t), width, height);
                }
                break;
#endif

            case VG_LITE_BGRA8888: {
                    /* Для преобразования шага */
                    conv_bgra8888_to_bgra8888.convert(&target, source);
                }
                break;

            default:
                LV_LOG_ERROR("unsupported format: %d", source->format);
                LV_ASSERT(false);
                break;
        }

        /* умножить цвет */
        if(source->image_mode == VG_LITE_MULTIPLY_IMAGE_MODE && !VG_LITE_IS_ALPHA_FORMAT(source->format)) {
            vg_color32_t * dest = (vg_color32_t *)image_buffer;
            vg_lite_uint32_t px_size = width * height;
            while(px_size--) {
                dest->alpha = UDIV255(dest->alpha * A(color));
                dest->red = UDIV255(dest->red * B(color));
                dest->green = UDIV255(dest->green * G(color));
                dest->blue = UDIV255(dest->blue * R(color));
                dest++;
            }
        }
    }

    TVG_CHECK_RETURN_RESULT(picture->load((uint32_t *)image_buffer, source->width, source->height, true));

    return Result::Success;
}

static void ClampColor(FLOATVECTOR4 Source, FLOATVECTOR4 Target, uint8_t Premultiplied)
{
    vg_lite_float_t colorMax;
    /* Зафиксируйте альфа-канал. */
    Target[3] = CLAMP(Source[3], 0.0f, 1.0f);

    /* Определите максимальное значение для цветовых каналов. */
    colorMax = Premultiplied ? Target[3] : 1.0f;

    /* Зажмите цветовые каналы. */
    Target[0] = CLAMP(Source[0], 0.0f, colorMax);
    Target[1] = CLAMP(Source[1], 0.0f, colorMax);
    Target[2] = CLAMP(Source[2], 0.0f, colorMax);
}

static uint8_t PackColorComponent(vg_lite_float_t value)
{
    /* Вычислите округленное нормализованное значение. */
    vg_lite_float_t rounded = value * 255.0f + 0.5f;

    /* Получите целую часть. */
    int32_t roundedInt = (int32_t)rounded;

    /* Ограничьте диапазон до 0..1. */
    uint8_t clamped = (uint8_t)CLAMP(roundedInt, 0, 255);

    /* Возврат результата. */
    return clamped;
}

/* Получите информацию в формате bpp о цветах. */
static void get_format_bytes(vg_lite_buffer_format_t format,
                             vg_lite_uint32_t * mul,
                             vg_lite_uint32_t * div,
                             vg_lite_uint32_t * bytes_align)
{
    *mul = *div = 1;
    *bytes_align = 4;
    switch(format) {
        case VG_LITE_L8:
        case VG_LITE_A8:
        case VG_LITE_RGBA8888_ETC2_EAC:
            break;

        case VG_LITE_A4:
            *div = 2;
            break;

        case VG_LITE_ABGR1555:
        case VG_LITE_ARGB1555:
        case VG_LITE_BGRA5551:
        case VG_LITE_RGBA5551:
        case VG_LITE_RGBA4444:
        case VG_LITE_BGRA4444:
        case VG_LITE_ABGR4444:
        case VG_LITE_ARGB4444:
        case VG_LITE_RGB565:
        case VG_LITE_BGR565:
        case VG_LITE_YUYV:
        case VG_LITE_YUY2:
        case VG_LITE_YUY2_TILED:
        /* Буферная память AYUY2 = YUY2 + альфа. */
        case VG_LITE_AYUY2:
        case VG_LITE_AYUY2_TILED:
        /* Буферная память ABGR8565_PLANAR = RGB565 + альфа. */
        case VG_LITE_ABGR8565_PLANAR:
        case VG_LITE_ARGB8565_PLANAR:
        case VG_LITE_RGBA5658_PLANAR:
        case VG_LITE_BGRA5658_PLANAR:
            *mul = 2;
            break;

        case VG_LITE_RGBA8888:
        case VG_LITE_BGRA8888:
        case VG_LITE_ABGR8888:
        case VG_LITE_ARGB8888:
        case VG_LITE_RGBX8888:
        case VG_LITE_BGRX8888:
        case VG_LITE_XBGR8888:
        case VG_LITE_XRGB8888:
            *mul = 4;
            break;

        case VG_LITE_NV12:
        case VG_LITE_NV12_TILED:
            *mul = 3;
            break;

        case VG_LITE_ANV12:
        case VG_LITE_ANV12_TILED:
            *mul = 4;
            break;

        case VG_LITE_INDEX_1:
            *div = 8;
            *bytes_align = 8;
            break;

        case VG_LITE_INDEX_2:
            *div = 4;
            *bytes_align = 8;
            break;

        case VG_LITE_INDEX_4:
            *div = 2;
            *bytes_align = 8;
            break;

        case VG_LITE_INDEX_8:
            *bytes_align = 1;
            break;

        case VG_LITE_RGBA2222:
        case VG_LITE_BGRA2222:
        case VG_LITE_ABGR2222:
        case VG_LITE_ARGB2222:
            *mul = 1;
            break;

        case VG_LITE_RGB888:
        case VG_LITE_BGR888:
        case VG_LITE_ABGR8565:
        case VG_LITE_BGRA5658:
        case VG_LITE_ARGB8565:
        case VG_LITE_RGBA5658:
            *mul = 3;
            break;

        /* формат OpenVG*/
        case VG_sRGBX_8888:
        case VG_sRGBA_8888:
        case VG_sRGBA_8888_PRE:
        case VG_lRGBX_8888:
        case VG_lRGBA_8888:
        case VG_lRGBA_8888_PRE:
        case VG_sXRGB_8888:
        case VG_sARGB_8888:
        case VG_sARGB_8888_PRE:
        case VG_lXRGB_8888:
        case VG_lARGB_8888:
        case VG_lARGB_8888_PRE:
        case VG_sBGRX_8888:
        case VG_sBGRA_8888:
        case VG_sBGRA_8888_PRE:
        case VG_lBGRX_8888:
        case VG_lBGRA_8888:
        case VG_sXBGR_8888:
        case VG_sABGR_8888:
        case VG_lBGRA_8888_PRE:
        case VG_sABGR_8888_PRE:
        case VG_lXBGR_8888:
        case VG_lABGR_8888:
        case VG_lABGR_8888_PRE:
            *mul = 4;
            break;

        case VG_sRGBA_5551:
        case VG_sRGBA_4444:
        case VG_sARGB_1555:
        case VG_sARGB_4444:
        case VG_sBGRA_5551:
        case VG_sBGRA_4444:
        case VG_sABGR_1555:
        case VG_sABGR_4444:
        case VG_sRGB_565:
        case VG_sBGR_565:
            *mul = 2;
            break;

        case VG_sL_8:
        case VG_lL_8:
        case VG_A_8:
            break;

        case VG_BW_1:
        case VG_A_4:
        case VG_A_1:
            *div = 2;
            break;

        default:
            break;
    }
}

static vg_lite_fpoint_t matrix_transform_point(const vg_lite_matrix_t * matrix, const vg_lite_fpoint_t * point)
{
    vg_lite_fpoint_t p;
    p.x = (vg_lite_float_t)(point->x * matrix->m[0][0] + point->y * matrix->m[0][1] + matrix->m[0][2]);
    p.y = (vg_lite_float_t)(point->x * matrix->m[1][0] + point->y * matrix->m[1][1] + matrix->m[1][2]);
    return p;
}

static bool vg_lite_matrix_inverse(vg_lite_matrix_t * result, const vg_lite_matrix_t * matrix)
{
    vg_lite_float_t det00, det01, det02;
    vg_lite_float_t d;
    bool is_affine;

    /* Тест на идентификационную матрицу. */
    if(matrix == NULL) {
        result->m[0][0] = 1.0f;
        result->m[0][1] = 0.0f;
        result->m[0][2] = 0.0f;
        result->m[1][0] = 0.0f;
        result->m[1][1] = 1.0f;
        result->m[1][2] = 0.0f;
        result->m[2][0] = 0.0f;
        result->m[2][1] = 0.0f;
        result->m[2][2] = 1.0f;

        /* Успех. */
        return true;
    }

    det00 = (matrix->m[1][1] * matrix->m[2][2]) - (matrix->m[2][1] * matrix->m[1][2]);
    det01 = (matrix->m[2][0] * matrix->m[1][2]) - (matrix->m[1][0] * matrix->m[2][2]);
    det02 = (matrix->m[1][0] * matrix->m[2][1]) - (matrix->m[2][0] * matrix->m[1][1]);

    /* Вычислить определитель. */
    d = (matrix->m[0][0] * det00) + (matrix->m[0][1] * det01) + (matrix->m[0][2] * det02);

    /* Верните 0, если обратной матрицы нет. */
    if(d == 0.0f)
        return false;

    /* Вычислить обратную величину. */
    d = 1.0f / d;

    /* Определите, является ли матрица аффинной. */
    is_affine = (matrix->m[2][0] == 0.0f) && (matrix->m[2][1] == 0.0f) && (matrix->m[2][2] == 1.0f);

    result->m[0][0] = d * det00;
    result->m[0][1] = d * ((matrix->m[2][1] * matrix->m[0][2]) - (matrix->m[0][1] * matrix->m[2][2]));
    result->m[0][2] = d * ((matrix->m[0][1] * matrix->m[1][2]) - (matrix->m[1][1] * matrix->m[0][2]));
    result->m[1][0] = d * det01;
    result->m[1][1] = d * ((matrix->m[0][0] * matrix->m[2][2]) - (matrix->m[2][0] * matrix->m[0][2]));
    result->m[1][2] = d * ((matrix->m[1][0] * matrix->m[0][2]) - (matrix->m[0][0] * matrix->m[1][2]));
    result->m[2][0] = is_affine ? 0.0f : d * det02;
    result->m[2][1] = is_affine ? 0.0f : d * ((matrix->m[2][0] * matrix->m[0][1]) - (matrix->m[0][0] * matrix->m[2][1]));
    result->m[2][2] = is_affine ? 1.0f : d * ((matrix->m[0][0] * matrix->m[1][1]) - (matrix->m[1][0] * matrix->m[0][1]));

    /* Успех. */
    return true;
}

static void vg_lite_matrix_multiply(vg_lite_matrix_t * matrix, const vg_lite_matrix_t * mult)
{
    vg_lite_matrix_t temp;
    int row, column;

    /* Обработать все строки. */
    for(row = 0; row < 3; row++) {
        /* Обработать все столбцы. */
        for(column = 0; column < 3; column++) {
            /* Вычислить запись матрицы. */
            temp.m[row][column] = (matrix->m[row][0] * mult->m[0][column])
                                  + (matrix->m[row][1] * mult->m[1][column])
                                  + (matrix->m[row][2] * mult->m[2][column]);
        }
    }

    /* Скопируйте временную матрицу в результат. */
    lv_memcpy(matrix->m, &temp.m, sizeof(temp.m));
}

static Result vg_lite_grad_matrix_conv(vg_lite_matrix_t * result, const vg_lite_matrix_t * grad_matrix,
                                       const vg_lite_matrix_t * path_matrix)
{
    /**
     * Поскольку Торвг внутренне умножает матрицу пути (формы) на матрицу градиента, чтобы получить
     * результат рендеринга, матрица градиентов и матрица путейVG-Lite полностью независимы,
     * требуя предыдущего умножения для достижения того же результата рендеринга,
     * для эмулятораVG-Lite необходимо переместить матрицу градиента, чтобы получить исходную матрицу градиента пользователя
     * для имитации поведения оборудования:
     * matrix_out = path_matrix * gradient_matrix
     * =>
     * gradient_matrix = инв(path_matrix) * matrix_out
     */
    if(!vg_lite_matrix_inverse(result, path_matrix)) {
        return Result::InvalidArguments;
    }

    vg_lite_matrix_multiply(result, grad_matrix);
    return Result::Success;
}

#endif
