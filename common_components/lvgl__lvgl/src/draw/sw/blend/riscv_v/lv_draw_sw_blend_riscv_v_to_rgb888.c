/**
 * @file lv_draw_sw_blend_riscv_v_to_rgb888.c
 * Реализация смеси RGB888/XRGB8888 для векторного расширения RISC -V (RVV 1.0)
 *
 * Поддерживает как dest_px_size =3 ( RGB888 ), так и dest_px_size =4 ( XRGB8888 ).
 * Ссылка: lv_draw_sw_blend_neon_to_rgb888.c
 *
 * NOTE: All RVV blend logic is inlined to avoid passing vuint32m4_t as function
 * параметры, что вызывает сложные операции со стеком, которые могут повредить стек.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_blend_riscv_v_to_rgb888.h"
#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_RISCV_V

#include "../../../../misc/lv_color.h"
#include "../../../../misc/lv_types.h"
#include "../lv_draw_sw_blend_private.h"
#include "lv_blend_riscv_v_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Залейте сплошным цветом (смешивание не требуется, непрозрачность >= 255).
 */
lv_result_t lv_draw_sw_blend_riscv_v_color_to_rgb888(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);

    const int32_t w           = dsc->dest_w;
    const int32_t h           = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    uint8_t * dest_buf        = dsc->dest_buf;
    size_t vl;

    if(dest_px_size == 3) {
        /* RGB888: 3 bytes per pixel (B, G, R) - use RVV segmented store */
        /* Инициализируйте цветовые векторы один раз с максимальным vl */
        size_t vlmax = __riscv_vsetvlmax_e8m2();
        vuint8m2_t v_b = __riscv_vmv_v_x_u8m2(dsc->color.blue, vlmax);
        vuint8m2_t v_g = __riscv_vmv_v_x_u8m2(dsc->color.green, vlmax);
        vuint8m2_t v_r = __riscv_vmv_v_x_u8m2(dsc->color.red, vlmax);

        for(int32_t y = 0; y < h; y++) {
            /* Обработка с помощью RVV с использованием сегментированного хранилища для 3-байтовых пикселей. */
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m2(w - x);
                LV_RVV_VSSEG3E8_U8M2(dest_buf + x * 3, v_b, v_g, v_r, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
        }
    }
    else { /* dest_px_size == 4 */
        /* XRGB8888: 4 bytes per pixel */
        const uint32_t color32 = 0xFF000000 | ((uint32_t)dsc->color.red << 16) |
                                 ((uint32_t)dsc->color.green << 8) | dsc->color.blue;

        /* Инициализировать цветовой вектор один раз с максимальным vl */
        size_t vlmax = __riscv_vsetvlmax_e32m4();
        vuint32m4_t v_color = __riscv_vmv_v_x_u32m4(color32, vlmax);

        for(int32_t y = 0; y < h; y++) {
            /* Процесс с RVV — используйте m4 для снижения давления регистра */
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e32m4(w - x);
                __riscv_vse32_v_u32m4((uint32_t *)(dest_buf + x * 4), v_color, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
        }
    }

    return LV_RESULT_OK;
}

/**
 * Залейте цветом и непрозрачностью (opa < 255).
 * формула смешивания: результат = (fg * opa + bg * (255 - opa)) >> 8
 */
lv_result_t lv_draw_sw_blend_riscv_v_color_to_rgb888_with_opa(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);

    const int32_t w           = dsc->dest_w;
    const int32_t h           = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t opa         = dsc->opa;
    const uint8_t opa_inv     = 255 - opa;
    const uint16_t fg_b_opa   = (uint16_t)dsc->color.blue * opa;
    const uint16_t fg_g_opa   = (uint16_t)dsc->color.green * opa;
    const uint16_t fg_r_opa   = (uint16_t)dsc->color.red * opa;
    uint8_t * dest_buf        = dsc->dest_buf;
    size_t vl;

    /* Досрочный выход, если полностью прозрачен */
    if(opa == 0) return LV_RESULT_OK;

    if(dest_px_size == 3) {
        for(int32_t y = 0; y < h; y++) {
            /* Обработка с помощью RVV с использованием сегментированной загрузки/сохранения для 3-байтовых пикселей. */
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m2(w - x);

                /* Загрузка каналов назначения B, G, R с использованием сегментированной нагрузки */
                vuint8m2_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_VLSEG3E8_U8M2(dest_buf + x * 3, vl, v_dst_b, v_dst_g, v_dst_r);

                /* Смешайте однотонный цвет с местом назначения */
                vuint8m2_t v_b, v_g, v_r;
                LV_RVV_BLEND_SOLID_RGB_U8M2(v_dst_r, v_dst_g, v_dst_b,
                                            fg_r_opa, fg_g_opa, fg_b_opa, opa_inv,
                                            v_r, v_g, v_b, vl);

                /* Сохраните результат, используя сегментированное хранилище */
                LV_RVV_VSSEG3E8_U8M2(dest_buf + x * 3, v_b, v_g, v_r, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
        }
    }
    else { /* dest_px_size == 4 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m2(w - x);

                /* Загрузка каналов назначения B, G, R, X с использованием сегментированной нагрузки */
                vuint8m2_t v_dst_b, v_dst_g, v_dst_r, v_dst_x;
                LV_RVV_VLSEG4E8_U8M2(dest_buf + x * 4, vl, v_dst_b, v_dst_g, v_dst_r, v_dst_x);
                /* v_dst_x — это X/Alpha, игнорируется при вводе. */
                (void)v_dst_x;

                /* Смешайте однотонный цвет с местом назначения */
                vuint8m2_t v_b, v_g, v_r;
                LV_RVV_BLEND_SOLID_RGB_U8M2(v_dst_r, v_dst_g, v_dst_b,
                                            fg_r_opa, fg_g_opa, fg_b_opa, opa_inv,
                                            v_r, v_g, v_b, vl);

                vuint8m2_t v_x = __riscv_vmv_v_x_u8m2(0xFF, vl);  /* Alpha = 0xFF */

                /* Сохраните результат, используя сегментированное хранилище */
                LV_RVV_VSSEG4E8_U8M2(dest_buf + x * 4, v_b, v_g, v_r, v_x, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
        }
    }

    return LV_RESULT_OK;
}

/**
 * Заливка цветом и попиксельной маской (opa >= 255)
 */
lv_result_t lv_draw_sw_blend_riscv_v_color_to_rgb888_with_mask(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);

    const int32_t w           = dsc->dest_w;
    const int32_t h           = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t * mask_buf  = dsc->mask_buf;
    const uint8_t fg_b        = dsc->color.blue;
    const uint8_t fg_g        = dsc->color.green;
    const uint8_t fg_r        = dsc->color.red;
    uint8_t * dest_buf        = dsc->dest_buf;
    size_t vl;

    if(dest_px_size == 3) {
        /* RGB888: 3 bytes per pixel - use RVV for blending with mask */
        for(int32_t y = 0; y < h; y++) {
            /* Обработка с помощью RVV с использованием сегментированной загрузки/сохранения для 3-байтовых пикселей. */
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m2(w - x);

                /* Загрузить значения маски */
                vuint8m2_t v_mask8 = __riscv_vle8_v_u8m2(&mask_buf[x], vl);

                /* Загрузка каналов назначения B, G, R с использованием сегментированной нагрузки */
                vuint8m2_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_VLSEG3E8_U8M2(dest_buf + x * 3, vl, v_dst_b, v_dst_g, v_dst_r);

                /* Смешайте сплошной цвет с маской */
                vuint8m2_t v_b, v_g, v_r;
                LV_RVV_BLEND_SOLID_RGB_VMASK_U8M2(v_dst_r, v_dst_g, v_dst_b,
                                                  fg_r, fg_g, fg_b, v_mask8,
                                                  v_r, v_g, v_b, vl);

                /* Optional: Handle special cases for mask == 0 or mask >= 255.
                 * Без этого максимальная ошибка равна ±1 (например, (x*255)>>8 ≈ x*0,996).
                 * Для рендеринга графики обычно допустима ошибка ±1.
                 * Раскомментируйте ниже, если требуются точные значения. */
                LV_RVV_BLEND_OPTIMIZE_MASK_SCALAR_U8M2(v_r, v_g, v_b,
                                                       fg_r, fg_g, fg_b,
                                                       v_dst_r, v_dst_g, v_dst_b,
                                                       v_mask8, vl);
                /* Сохраните результат, используя сегментированное хранилище */
                LV_RVV_VSSEG3E8_U8M2(dest_buf + x * 3, v_b, v_g, v_r, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            mask_buf += mask_stride;
        }
    }
    else { /* dest_px_size == 4 */
        /* XRGB8888: 4 bytes per pixel - use segmented load/store like RGB888 */
        for(int32_t y = 0; y < h; y++) {
            /* Обработка с помощью RVV с использованием сегментированной загрузки/сохранения для 4-байтовых пикселей. */
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m2(w - x);

                /* Загрузить значения маски */
                vuint8m2_t v_mask8 = __riscv_vle8_v_u8m2(&mask_buf[x], vl);

                /* Загрузка каналов назначения B, G, R, X с использованием сегментированной нагрузки */
                vuint8m2_t v_dst_b, v_dst_g, v_dst_r, v_dst_x;
                LV_RVV_VLSEG4E8_U8M2(dest_buf + x * 4, vl, v_dst_b, v_dst_g, v_dst_r, v_dst_x);
                /* v_dst_x — это X/Alpha, игнорируется при вводе. */
                (void)v_dst_x;

                /* Смешайте сплошной цвет с маской */
                vuint8m2_t v_b, v_g, v_r;
                LV_RVV_BLEND_SOLID_RGB_VMASK_U8M2(v_dst_r, v_dst_g, v_dst_b,
                                                  fg_r, fg_g, fg_b, v_mask8,
                                                  v_r, v_g, v_b, vl);
                vuint8m2_t v_x = __riscv_vmv_v_x_u8m2(0xFF, vl);  /* Alpha = 0xFF */

                /* Optional: Handle special cases for mask == 0 or mask >= 255.
                 * Без этого максимальная ошибка составляет ±1. Раскомментируйте, если требуются точные значения. */
                LV_RVV_BLEND_OPTIMIZE_MASK_SCALAR_U8M2(v_r, v_g, v_b,
                                                       fg_r, fg_g, fg_b,
                                                       v_dst_r, v_dst_g, v_dst_b,
                                                       v_mask8, vl);

                /* Сохраните результат, используя сегментированное хранилище */
                LV_RVV_VSSEG4E8_U8M2(dest_buf + x * 4, v_b, v_g, v_r, v_x, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            mask_buf += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

/**
 * Заполните цветом, непрозрачностью и попиксельной маской.
 * Эффективная смесь = (маска * опа) >> 8
 */
lv_result_t lv_draw_sw_blend_riscv_v_color_to_rgb888_with_opa_mask(lv_draw_sw_blend_fill_dsc_t * dsc,
                                                                   uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);

    const int32_t w           = dsc->dest_w;
    const int32_t h           = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t opa         = dsc->opa;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t * mask_buf  = dsc->mask_buf;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint8_t fg_b = dsc->color.blue;
    const uint8_t fg_g = dsc->color.green;
    const uint8_t fg_r = dsc->color.red;
    size_t vl;

    /* Досрочный выход, если полностью прозрачен */
    if(opa == 0) return LV_RESULT_OK;

    if(dest_px_size == 3) {
        /* RGB888: 3 bytes per pixel - use RVV for blending with opa and mask */

        for(int32_t y = 0; y < h; y++) {
            /* Обработка с помощью RVV с использованием сегментированной загрузки/сохранения для 3-байтовых пикселей. */
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m2(w - x);

                /* Загрузить значения маски */
                vuint8m2_t v_mask8 = __riscv_vle8_v_u8m2(&mask_buf[x], vl);

                /* Загрузка каналов назначения B, G, R с использованием сегментированной нагрузки */
                vuint8m2_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_VLSEG3E8_U8M2(dest_buf + x * 3, vl, v_dst_b, v_dst_g, v_dst_r);

                /* Вычислить смесь = (маска * опа) >> 8, используя расширяющее умножение, а затем суженное */
                vuint16m4_t v_mix16 = __riscv_vwmulu_vx_u16m4(v_mask8, opa, vl);
                vuint8m2_t v_mix8 = __riscv_vnsrl_wx_u8m2(v_mix16, 8, vl);

                /* Смешайте сплошной цвет со смесью (маска * опа) */
                vuint8m2_t v_b, v_g, v_r;
                LV_RVV_BLEND_SOLID_RGB_VMASK_U8M2(v_dst_r, v_dst_g, v_dst_b,
                                                  fg_r, fg_g, fg_b, v_mix8,
                                                  v_r, v_g, v_b, vl);

                /* Optional: Handle special cases for mix == 0 or mix >= 255.
                 * Без этого максимальная ошибка составляет ±1. Раскомментируйте, если требуются точные значения. */

                LV_RVV_BLEND_OPTIMIZE_MASK_SCALAR_U8M2(v_r, v_g, v_b,
                                                       fg_r, fg_g, fg_b,
                                                       v_dst_r, v_dst_g, v_dst_b,
                                                       v_mix8, vl);

                /* Сохраните результат, используя сегментированное хранилище */
                LV_RVV_VSSEG3E8_U8M2(dest_buf + x * 3, v_b, v_g, v_r, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            mask_buf += mask_stride;
        }
    }
    else { /* dest_px_size == 4 */
        /* XRGB8888: 4 bytes per pixel - use segmented load/store like RGB888 */


        for(int32_t y = 0; y < h; y++) {
            /* Обработка с помощью RVV с использованием сегментированной загрузки/сохранения для 4-байтовых пикселей. */
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m2(w - x);

                /* Загрузить значения маски */
                vuint8m2_t v_mask8 = __riscv_vle8_v_u8m2(&mask_buf[x], vl);

                /* Вычислить смесь = (маска * опа) >> 8, используя расширяющее умножение */
                vuint16m4_t v_mix16 = __riscv_vsrl_vx_u16m4(
                                          __riscv_vwmulu_vx_u16m4(v_mask8, opa, vl), 8, vl);
                vuint8m2_t v_mix8 = __riscv_vnsrl_wx_u8m2(v_mix16, 0, vl);

                /* Загрузка каналов назначения B, G, R, X с использованием сегментированной нагрузки */
                vuint8m2_t v_dst_b, v_dst_g, v_dst_r, v_dst_x;
                LV_RVV_VLSEG4E8_U8M2(dest_buf + x * 4, vl, v_dst_b, v_dst_g, v_dst_r, v_dst_x);
                (void)v_dst_x;  /* v_dst_x — это X/Alpha, игнорируется при вводе. */

                /* Смешайте сплошной цвет со смесью (маска * опа) */
                vuint8m2_t v_b, v_g, v_r;
                LV_RVV_BLEND_SOLID_RGB_VMASK_U8M2(v_dst_r, v_dst_g, v_dst_b,
                                                  fg_r, fg_g, fg_b, v_mix8,
                                                  v_r, v_g, v_b, vl);

                LV_RVV_BLEND_OPTIMIZE_MASK_SCALAR_U8M2(v_r, v_g, v_b,
                                                       fg_r, fg_g, fg_b,
                                                       v_dst_r, v_dst_g, v_dst_b,
                                                       v_mix8, vl);

                vuint8m2_t v_x = __riscv_vmv_v_x_u8m2(0xFF, vl);  /* Alpha = 0xFF */

                /* Сохраните результат, используя сегментированное хранилище */
                LV_RVV_VSSEG4E8_U8M2(dest_buf + x * 4, v_b, v_g, v_r, v_x, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            mask_buf += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

/**********************
 *  RGB565 TO RGB888 BLEND FUNCTIONS
 **********************/

/**
 * От RGB565 до RGB888 / XRGB8888 простая копия (без смешивания, непрозрачность >= 255)
 * Формат RGB565: RRRRRGGGGGGBBBBB (5-6-5 бит)
 */
lv_result_t lv_draw_sw_blend_riscv_v_rgb565_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint16_t * src_buf = dsc->src_buf;
    size_t vl;

    if(dest_px_size == 3) {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e16m2(w - x);

                /* Загрузить пиксели RGB565 */
                vuint16m2_t v_rgb565 = __riscv_vle16_v_u16m2(&src_buf[x], vl);

                /* Извлеките компоненты R5, G6, B5. */
                vuint16m2_t v_r5 = __riscv_vand_vx_u16m2(__riscv_vsrl_vx_u16m2(v_rgb565, 11, vl), 0x1F, vl);
                vuint16m2_t v_g6 = __riscv_vand_vx_u16m2(__riscv_vsrl_vx_u16m2(v_rgb565, 5, vl), 0x3F, vl);
                vuint16m2_t v_b5 = __riscv_vand_vx_u16m2(v_rgb565, 0x1F, vl);

                /* Преобразование в 8-битное: R8 = ( R5 * 2106) >> 8, G8 = ( G6 * 1037) >> 8, B8 = ( B5 * 2106) >> 8 */
                vuint16m2_t v_r8_16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_r5, 2106, vl), 8, vl);
                vuint16m2_t v_g8_16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_g6, 1037, vl), 8, vl);
                vuint16m2_t v_b8_16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_b5, 2106, vl), 8, vl);

                /* Ограничить до 8-битного */
                vuint8m1_t v_r = __riscv_vnsrl_wx_u8m1(v_r8_16, 0, vl);
                vuint8m1_t v_g = __riscv_vnsrl_wx_u8m1(v_g8_16, 0, vl);
                vuint8m1_t v_b = __riscv_vnsrl_wx_u8m1(v_b8_16, 0, vl);

                /* Сохранение с использованием хранилища шагов для RGB888 (3 байта на пиксель) */
                LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }
    else { /* dest_px_size == 4 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e16m2(w - x);

                /* Загрузить пиксели RGB565 */
                vuint16m2_t v_rgb565 = __riscv_vle16_v_u16m2(&src_buf[x], vl);

                /* Извлеките компоненты R5, G6, B5. */
                vuint16m2_t v_r5 = __riscv_vand_vx_u16m2(__riscv_vsrl_vx_u16m2(v_rgb565, 11, vl), 0x1F, vl);
                vuint16m2_t v_g6 = __riscv_vand_vx_u16m2(__riscv_vsrl_vx_u16m2(v_rgb565, 5, vl), 0x3F, vl);
                vuint16m2_t v_b5 = __riscv_vand_vx_u16m2(v_rgb565, 0x1F, vl);

                /* Преобразовать в 8-битный */
                vuint16m2_t v_r8_16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_r5, 2106, vl), 8, vl);
                vuint16m2_t v_g8_16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_g6, 1037, vl), 8, vl);
                vuint16m2_t v_b8_16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_b5, 2106, vl), 8, vl);

                /* Ограничить до 8-битного */
                vuint8m1_t v_r = __riscv_vnsrl_wx_u8m1(v_r8_16, 0, vl);
                vuint8m1_t v_g = __riscv_vnsrl_wx_u8m1(v_g8_16, 0, vl);
                vuint8m1_t v_b = __riscv_vnsrl_wx_u8m1(v_b8_16, 0, vl);
                vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, vl);

                /* Сохранение с использованием хранилища шагов для XRGB8888 (4 байта на пиксель) */
                LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }

    return LV_RESULT_OK;
}

/**
 * От RGB565 до RGB888 / XRGB8888 с непрозрачностью
 * формула смешивания: результат = (src * opa + dst * (255 - opa)) >> 8
 * Оптимизировано использование vwmaccu для расчета смеси.
 */
lv_result_t lv_draw_sw_blend_riscv_v_rgb565_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc,
                                                               uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const uint8_t opa = dsc->opa;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint16_t * src_buf = dsc->src_buf;
    size_t vl;

    if(dest_px_size == 3) {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);

                /* Загрузите исходные пиксели RGB565 */
                vuint16m2_t v_rgb565 = __riscv_vle16_v_u16m2(&src_buf[x], vl);

                /* Преобразуйте RGB565 в RGB888 с помощью макроса, затем сузьте его до 8-битного. */
                vuint16m2_t v_src_r16, v_src_g16, v_src_b16;
                LV_RVV_RGB565_TO_RGB888_U16M2(v_rgb565, v_src_r16, v_src_g16, v_src_b16, vl);
                vuint8m1_t v_src_r = __riscv_vnsrl_wx_u8m1(v_src_r16, 0, vl);
                vuint8m1_t v_src_g = __riscv_vnsrl_wx_u8m1(v_src_g16, 0, vl);
                vuint8m1_t v_src_b = __riscv_vnsrl_wx_u8m1(v_src_b16, 0, vl);

                /* Загрузите пункт назначения RGB888, используя нагрузку на шаг */
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);

                /* Смешайте с помощью vwmaccu */
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_U8M1(v_src_r, v_src_g, v_src_b,
                                      v_dst_r, v_dst_g, v_dst_b,
                                      opa, v_r, v_g, v_b, vl);

                /* Сохранить результат */
                LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }
    else { /* dest_px_size == 4 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);

                /* Загрузите исходные пиксели RGB565 */
                vuint16m2_t v_rgb565 = __riscv_vle16_v_u16m2(&src_buf[x], vl);

                /* Преобразуйте RGB565 в RGB888 с помощью макроса, затем сузьте его до 8-битного. */
                vuint16m2_t v_src_r16, v_src_g16, v_src_b16;
                LV_RVV_RGB565_TO_RGB888_U16M2(v_rgb565, v_src_r16, v_src_g16, v_src_b16, vl);
                vuint8m1_t v_src_r = __riscv_vnsrl_wx_u8m1(v_src_r16, 0, vl);
                vuint8m1_t v_src_g = __riscv_vnsrl_wx_u8m1(v_src_g16, 0, vl);
                vuint8m1_t v_src_b = __riscv_vnsrl_wx_u8m1(v_src_b16, 0, vl);

                /* Загрузите пункт назначения XRGB8888, используя нагрузку на шаг */
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);

                /* Смешайте с помощью vwmaccu */
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_U8M1(v_src_r, v_src_g, v_src_b,
                                      v_dst_r, v_dst_g, v_dst_b,
                                      opa, v_r, v_g, v_b, vl);
                vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, vl);

                /* Сохранить результат */
                LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }

    return LV_RESULT_OK;
}

/**
 * От RGB565 до RGB888 / XRGB8888 с попиксельной маской
 * формула смешивания: результат = (src * маска + dst * (255 - маска)) >> 8
 * Оптимизировано использование vwmaccu для расчета смеси.
 */
lv_result_t lv_draw_sw_blend_riscv_v_rgb565_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->mask_stride;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint16_t * src_buf = dsc->src_buf;
    const uint8_t * mask_buf = dsc->mask_buf;
    size_t vl;

    if(dest_px_size == 3) {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);

                /* Загрузить маску */
                vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);

                /* Загрузить исходный код RGB565 */
                vuint16m2_t v_rgb565 = __riscv_vle16_v_u16m2(&src_buf[x], vl);

                /* Преобразуйте RGB565 в RGB888 и сузьте до 8-битного. */
                vuint16m2_t v_src_r16, v_src_g16, v_src_b16;
                LV_RVV_RGB565_TO_RGB888_U16M2(v_rgb565, v_src_r16, v_src_g16, v_src_b16, vl);
                vuint8m1_t v_src_r = __riscv_vnsrl_wx_u8m1(v_src_r16, 0, vl);
                vuint8m1_t v_src_g = __riscv_vnsrl_wx_u8m1(v_src_g16, 0, vl);
                vuint8m1_t v_src_b = __riscv_vnsrl_wx_u8m1(v_src_b16, 0, vl);

                /* Загрузить пункт назначения */
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);

                /* Смешайте с маской, используя vwmaccu */
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                            v_dst_r, v_dst_g, v_dst_b,
                                            v_mask, v_r, v_g, v_b, vl);
                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_mask, vl);
                LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
            mask_buf += mask_stride;
        }
    }
    else { /* dest_px_size == 4 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);

                /* Загрузить маску */
                vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);

                /* Загрузить исходный код RGB565 */
                vuint16m2_t v_rgb565 = __riscv_vle16_v_u16m2(&src_buf[x], vl);

                /* Преобразуйте RGB565 в RGB888 и сузьте до 8-битного. */
                vuint16m2_t v_src_r16, v_src_g16, v_src_b16;
                LV_RVV_RGB565_TO_RGB888_U16M2(v_rgb565, v_src_r16, v_src_g16, v_src_b16, vl);
                vuint8m1_t v_src_r = __riscv_vnsrl_wx_u8m1(v_src_r16, 0, vl);
                vuint8m1_t v_src_g = __riscv_vnsrl_wx_u8m1(v_src_g16, 0, vl);
                vuint8m1_t v_src_b = __riscv_vnsrl_wx_u8m1(v_src_b16, 0, vl);

                /* Загрузить пункт назначения */
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);

                /* Смешайте с маской, используя vwmaccu */
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                            v_dst_r, v_dst_g, v_dst_b,
                                            v_mask, v_r, v_g, v_b, vl);

                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_mask, vl);

                vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, vl);

                LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
            mask_buf += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

/**
 * От RGB565 до RGB888 / XRGB8888 с непрозрачностью и попиксельной маской.
 * эффективная смесь = (маска * опа) >> 8
 * формула смешивания: результат = (src * mix + dst * (255 - mix)) >> 8
 *
 * Note: with_opa_mask needs 16-bit intermediate for mix calculation,
 * поэтому мы не можем напрямую использовать оптимизацию vwmaccu для этого случая.
 */
lv_result_t lv_draw_sw_blend_riscv_v_rgb565_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                    uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t opa = dsc->opa;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint16_t * src_buf = dsc->src_buf;
    const uint8_t * mask_buf = dsc->mask_buf;
    size_t vl;

    if(dest_px_size == 3) {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);

                /* Загрузите маску и вычислите эффективное сочетание = (маска * opa) >> 8 */
                vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                vuint16m2_t v_mask16 = __riscv_vzext_vf2_u16m2(v_mask, vl);
                vuint16m2_t v_mix16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_mask16, opa, vl), 8, vl);
                vuint8m1_t v_mix = __riscv_vnsrl_wx_u8m1(v_mix16, 0, vl);

                /* Загрузить исходный код RGB565 */
                vuint16m2_t v_rgb565 = __riscv_vle16_v_u16m2(&src_buf[x], vl);

                /* Преобразуйте RGB565 в RGB888 и сузьте до 8-битного. */
                vuint16m2_t v_src_r16, v_src_g16, v_src_b16;
                LV_RVV_RGB565_TO_RGB888_U16M2(v_rgb565, v_src_r16, v_src_g16, v_src_b16, vl);
                vuint8m1_t v_src_r = __riscv_vnsrl_wx_u8m1(v_src_r16, 0, vl);
                vuint8m1_t v_src_g = __riscv_vnsrl_wx_u8m1(v_src_g16, 0, vl);
                vuint8m1_t v_src_b = __riscv_vnsrl_wx_u8m1(v_src_b16, 0, vl);

                /* Загрузить пункт назначения */
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);

                /* Смешайте с эффективной смесью, используя vwmaccu */
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                            v_dst_r, v_dst_g, v_dst_b,
                                            v_mix, v_r, v_g, v_b, vl);

                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_mix, vl);

                LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
            mask_buf += mask_stride;
        }
    }
    else { /* dest_px_size == 4 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);

                /* Загрузите маску и вычислите эффективное сочетание */
                vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                vuint16m2_t v_mask16 = __riscv_vzext_vf2_u16m2(v_mask, vl);
                vuint16m2_t v_mix16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_mask16, opa, vl), 8, vl);
                vuint8m1_t v_mix = __riscv_vnsrl_wx_u8m1(v_mix16, 0, vl);

                /* Загрузить исходный код RGB565 */
                vuint16m2_t v_rgb565 = __riscv_vle16_v_u16m2(&src_buf[x], vl);

                /* Преобразуйте RGB565 в RGB888 и сузьте до 8-битного. */
                vuint16m2_t v_src_r16, v_src_g16, v_src_b16;
                LV_RVV_RGB565_TO_RGB888_U16M2(v_rgb565, v_src_r16, v_src_g16, v_src_b16, vl);
                vuint8m1_t v_src_r = __riscv_vnsrl_wx_u8m1(v_src_r16, 0, vl);
                vuint8m1_t v_src_g = __riscv_vnsrl_wx_u8m1(v_src_g16, 0, vl);
                vuint8m1_t v_src_b = __riscv_vnsrl_wx_u8m1(v_src_b16, 0, vl);

                /* Загрузить пункт назначения */
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);

                /* Смешайте с эффективной смесью, используя vwmaccu */
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                            v_dst_r, v_dst_g, v_dst_b,
                                            v_mix, v_r, v_g, v_b, vl);
                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_mix, vl);
                vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, vl);

                LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
            mask_buf += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

/**********************
 *  RGB888/XRGB8888 TO RGB888/XRGB8888 BLEND FUNCTIONS
 **********************/

/**
 * RGB888 / XRGB8888 — RGB888 / XRGB8888 простая копия (без смешивания, непрозрачность >= 255)
 * src_px_size: 3 for RGB888, 4 for XRGB8888
 * dest_px_size: 3 for RGB888, 4 for XRGB8888
 */
lv_result_t lv_draw_sw_blend_riscv_v_rgb888_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc,
                                                      uint32_t dest_px_size, uint32_t src_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(src_px_size == 3 || src_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint8_t * src_buf = dsc->src_buf;
    size_t vl;

    /* Быстрый путь: тот же размер пикселей, используйте RVV memcpy */
    if(src_px_size == dest_px_size) {
        const int32_t row_bytes = w * dest_px_size;
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < row_bytes; x += vl) {
                vl = __riscv_vsetvl_e8m8(row_bytes - x);
                vuint8m8_t v_data = __riscv_vle8_v_u8m8(src_buf + x, vl);
                __riscv_vse8_v_u8m8(dest_buf + x, v_data, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
        return LV_RESULT_OK;
    }

    /* Различные размеры пикселей: требуется попиксельное преобразование */
    if(dest_px_size == 3) {
        /* Источник: XRGB8888 -> RGB888 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_src_b, v_src_g, v_src_r;
                LV_RVV_LOAD_XRGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_src_b, v_src_g, v_src_r, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }
    else {
        /* Пункт назначения: XRGB8888 */
        size_t max_vl = __riscv_vsetvlmax_e8m1();
        vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, max_vl);

        /* Источник: RGB888 -> XRGB8888 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_src_b, v_src_g, v_src_r;
                LV_RVV_LOAD_RGB888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_src_b, v_src_g, v_src_r, v_a, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }

    return LV_RESULT_OK;
}

/**
 * RGB888 / XRGB8888 — RGB888 / XRGB8888 с непрозрачностью
 * формула смешивания: результат = (src * opa + dst * (255 - opa)) >> 8
 */
lv_result_t lv_draw_sw_blend_riscv_v_rgb888_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc,
                                                               uint32_t dest_px_size, uint32_t src_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(src_px_size == 3 || src_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const uint8_t opa = dsc->opa;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint8_t * src_buf = dsc->src_buf;
    size_t vl;

    if(dest_px_size == 3) {
        if(src_px_size == 3) {
            /* RGB888 -> RGB888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_RGB888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, opa, v_r, v_g, v_b, vl);
                    LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
            }
        }
        else {
            /* XRGB8888 -> RGB888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, opa, v_r, v_g, v_b, vl);
                    LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
            }
        }
    }
    else {
        size_t max_vl = __riscv_vsetvlmax_e8m1();
        vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, max_vl);
        if(src_px_size == 3) {
            /* RGB888 -> XRGB8888 */
            for(int32_t y = 0; y < h; y++) {
                uint8_t * dest_row = dest_buf;
                const uint8_t * src_row = src_buf;
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_RGB888_U8M1(src_row, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(dest_row, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, opa, v_r, v_g, v_b, vl);
                    LV_RVV_STORE_XRGB8888_U8M1(dest_row, x, v_b, v_g, v_r, v_a, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
            }
        }
        else {
            /* XRGB8888 -> XRGB8888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, opa, v_r, v_g, v_b, vl);
                    LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
            }
        }
    }

    return LV_RESULT_OK;
}

/**
 * RGB888 / XRGB8888 — RGB888 / XRGB8888 с попиксельной маской
 * формула смешивания: результат = (src * маска + dst * (255 - маска)) >> 8
 */
lv_result_t lv_draw_sw_blend_riscv_v_rgb888_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                uint32_t dest_px_size, uint32_t src_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(src_px_size == 3 || src_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->mask_stride;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint8_t * src_buf = dsc->src_buf;
    const uint8_t * mask_buf = dsc->mask_buf;
    size_t vl;

    if(dest_px_size == 3) {
        if(src_px_size == 3) {
            /* RGB888 -> RGB888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_RGB888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_mask,
                                                v_r, v_g, v_b,
                                                vl);
                    LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                    v_src_r, v_src_g, v_src_b,
                                                    v_dst_r, v_dst_g, v_dst_b,
                                                    v_mask, vl);

                    LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
                mask_buf += mask_stride;
            }
        }
        else {
            /* XRGB8888 -> RGB888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_mask,
                                                v_r, v_g, v_b,
                                                vl);
                    LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                    v_src_r, v_src_g, v_src_b,
                                                    v_dst_r, v_dst_g, v_dst_b,
                                                    v_mask, vl);
                    LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
                mask_buf += mask_stride;
            }
        }
    }
    else {
        size_t max_vl = __riscv_vsetvlmax_e8m1();
        vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, max_vl);
        if(src_px_size == 3) {
            /* RGB888 -> XRGB8888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_RGB888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_mask,
                                                v_r, v_g, v_b,
                                                vl);
                    LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                    v_src_r, v_src_g, v_src_b,
                                                    v_dst_r, v_dst_g, v_dst_b,
                                                    v_mask, vl);
                    LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
                mask_buf += mask_stride;
            }
        }
        else {
            /* XRGB8888 -> XRGB8888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_mask, v_r, v_g, v_b, vl);
                    LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                    v_src_r, v_src_g, v_src_b,
                                                    v_dst_r, v_dst_g, v_dst_b,
                                                    v_mask, vl);
                    LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
                mask_buf += mask_stride;
            }
        }
    }

    return LV_RESULT_OK;
}

/**
 * RGB888 / XRGB8888 — RGB888 / XRGB8888 с непрозрачностью и попиксельной маской
 * эффективная смесь = (маска * опа) >> 8
 * формула смешивания: результат = (src * mix + dst * (255 - mix)) >> 8
 */
lv_result_t lv_draw_sw_blend_riscv_v_rgb888_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                    uint32_t dest_px_size, uint32_t src_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(src_px_size == 3 || src_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t opa = dsc->opa;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint8_t * src_buf = dsc->src_buf;
    const uint8_t * mask_buf = dsc->mask_buf;
    size_t vl;

    if(dest_px_size == 3) {
        if(src_px_size == 3) {
            /* RGB888 -> RGB888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                    vuint16m2_t v_mask16 = __riscv_vzext_vf2_u16m2(v_mask, vl);
                    vuint16m2_t v_mix16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_mask16, opa, vl), 8, vl);
                    vuint8m1_t v_mix = __riscv_vnsrl_wx_u8m1(v_mix16, 0, vl);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_RGB888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_mix, v_r, v_g, v_b, vl);
                    LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                    v_src_r, v_src_g, v_src_b,
                                                    v_dst_r, v_dst_g, v_dst_b,
                                                    v_mix, vl);
                    LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
                mask_buf += mask_stride;
            }
        }
        else {
            /* XRGB8888 -> RGB888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                    vuint16m2_t v_mask16 = __riscv_vzext_vf2_u16m2(v_mask, vl);
                    vuint16m2_t v_mix16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_mask16, opa, vl), 8, vl);
                    vuint8m1_t v_mix = __riscv_vnsrl_wx_u8m1(v_mix16, 0, vl);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, v_mix, v_r, v_g, v_b, vl);
                    LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                    v_src_r, v_src_g, v_src_b,
                                                    v_dst_r, v_dst_g, v_dst_b,
                                                    v_mix, vl);
                    LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
                mask_buf += mask_stride;
            }
        }
    }
    else {
        size_t max_vl = __riscv_vsetvlmax_e8m1();
        vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, max_vl);
        if(src_px_size == 3) {
            /* RGB888 -> XRGB8888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                    vuint16m2_t v_mask16 = __riscv_vzext_vf2_u16m2(v_mask, vl);
                    vuint16m2_t v_mix16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_mask16, opa, vl), 8, vl);
                    vuint8m1_t v_mix = __riscv_vnsrl_wx_u8m1(v_mix16, 0, vl);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_RGB888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, v_mix, v_r, v_g, v_b, vl);
                    LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                    v_src_r, v_src_g, v_src_b,
                                                    v_dst_r, v_dst_g, v_dst_b,
                                                    v_mix, vl);
                    LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
                mask_buf += mask_stride;
            }
        }
        else {
            /* XRGB8888 -> XRGB8888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                    vuint16m2_t v_mask16 = __riscv_vzext_vf2_u16m2(v_mask, vl);
                    vuint16m2_t v_mix16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_mask16, opa, vl), 8, vl);
                    vuint8m1_t v_mix = __riscv_vnsrl_wx_u8m1(v_mix16, 0, vl);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, v_mix, v_r, v_g, v_b, vl);
                    LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                    v_src_r, v_src_g, v_src_b,
                                                    v_dst_r, v_dst_g, v_dst_b,
                                                    v_mix, vl);
                    LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
                mask_buf += mask_stride;
            }
        }
    }

    return LV_RESULT_OK;
}

/**********************
 *  ARGB8888 TO RGB888/XRGB8888 BLEND FUNCTIONS
 **********************/

/**
 * Смешение ARGB8888 с RGB888/XRGB8888 с использованием исходного альфа-канала
 * формула смешивания: результат = (src * src_alpha + dst * (255 - src_alpha )) >> 8
 */
lv_result_t lv_draw_sw_blend_riscv_v_argb8888_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc,
                                                        uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint8_t * src_buf = dsc->src_buf;
    size_t vl;

    if(dest_px_size == 3) {
        /* ARGB8888 -> RGB888 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_src_b, v_src_g, v_src_r, v_src_a;
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_ARGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, v_src_a, vl);
                LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, v_src_a, v_r, v_g, v_b, vl);
                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_src_a, vl);
                LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }
    else {
        /* ARGB8888 -> XRGB8888 */
        size_t max_vl = __riscv_vsetvlmax_e8m1();
        vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, max_vl);
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_src_b, v_src_g, v_src_r, v_src_a;
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_ARGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, v_src_a, vl);
                LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, v_src_a, v_r, v_g, v_b, vl);
                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_src_a, vl);
                LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }

    return LV_RESULT_OK;
}

/**
 * От ARGB8888 до RGB888/XRGB8888 с глобальной непрозрачностью.
 * effective_alpha = (src_alpha * opa) >> 8
 * формула смешивания: результат = (src * effective_alpha + dst * (255 - effective_alpha )) >> 8
 */
lv_result_t lv_draw_sw_blend_riscv_v_argb8888_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                 uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const uint8_t opa = dsc->opa;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint8_t * src_buf = dsc->src_buf;
    size_t vl;

    if(dest_px_size == 3) {
        /* ARGB8888 -> RGB888 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_src_b, v_src_g, v_src_r, v_src_a;
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_ARGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, v_src_a, vl);
                vuint8m1_t v_eff_a;
                LV_RVV_CALC_EFF_ALPHA_OPA_U8M1(v_src_a, opa, v_eff_a, vl);
                LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, v_eff_a, v_r, v_g, v_b, vl);
                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_eff_a, vl);
                LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }
    else {
        /* ARGB8888 -> XRGB8888 */
        size_t max_vl = __riscv_vsetvlmax_e8m1();
        vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, max_vl);
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_src_b, v_src_g, v_src_r, v_src_a;
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_ARGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, v_src_a, vl);
                vuint8m1_t v_eff_a;
                LV_RVV_CALC_EFF_ALPHA_OPA_U8M1(v_src_a, opa, v_eff_a, vl);
                LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, v_eff_a, v_r, v_g, v_b, vl);
                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_eff_a, vl);
                LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }

    return LV_RESULT_OK;
}

/**
 * От ARGB8888 до RGB888 / XRGB8888 с попиксельной маской
 * effective_alpha = (src_alpha * mask) >> 8
 * формула смешивания: результат = (src * effective_alpha + dst * (255 - effective_alpha )) >> 8
 */
lv_result_t lv_draw_sw_blend_riscv_v_argb8888_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                  uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->mask_stride;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint8_t * src_buf = dsc->src_buf;
    const uint8_t * mask_buf = dsc->mask_buf;
    size_t vl;

    if(dest_px_size == 3) {
        /* ARGB8888 -> RGB888 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                vuint8m1_t v_src_b, v_src_g, v_src_r, v_src_a;
                LV_RVV_LOAD_ARGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, v_src_a, vl);
                vuint8m1_t v_eff_a;
                LV_RVV_CALC_EFF_ALPHA_MASK_U8M1(v_src_a, v_mask, v_eff_a, vl);
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                            v_dst_r, v_dst_g, v_dst_b,
                                            v_eff_a, v_r, v_g, v_b, vl);
                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_eff_a, vl);
                LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
            mask_buf += mask_stride;
        }
    }
    else {
        /* ARGB8888 -> XRGB8888 */
        size_t max_vl = __riscv_vsetvlmax_e8m1();
        vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, max_vl);
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                vuint8m1_t v_src_b, v_src_g, v_src_r, v_src_a;
                LV_RVV_LOAD_ARGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, v_src_a, vl);
                vuint8m1_t v_eff_a;
                LV_RVV_CALC_EFF_ALPHA_MASK_U8M1(v_src_a, v_mask, v_eff_a, vl);
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, v_eff_a, v_r, v_g, v_b, vl);
                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_eff_a, vl);
                LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
            mask_buf += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

/**
 * От ARGB8888 до RGB888 / XRGB8888 с непрозрачностью и попиксельной маской.
 * effective_alpha = (src_alpha * mask * opa) >> 16
 * формула смешивания: результат = (src * effective_alpha + dst * (255 - effective_alpha )) >> 8
 */
lv_result_t lv_draw_sw_blend_riscv_v_argb8888_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                      uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t opa = dsc->opa;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint8_t * src_buf = dsc->src_buf;
    const uint8_t * mask_buf = dsc->mask_buf;
    size_t vl;

    if(dest_px_size == 3) {
        /* ARGB8888 -> RGB888 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                vuint8m1_t v_src_b, v_src_g, v_src_r, v_src_a;
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_ARGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, v_src_a, vl);
                vuint8m1_t v_eff_a;
                LV_RVV_CALC_EFF_ALPHA_MASK_OPA_U8M1(v_src_a, v_mask, opa, v_eff_a, vl);
                LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                            v_dst_r, v_dst_g, v_dst_b,
                                            v_eff_a,
                                            v_r, v_g, v_b,
                                            vl);

                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_eff_a, vl);
                LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
            mask_buf += mask_stride;
        }
    }
    else {
        /* ARGB8888 -> XRGB8888 */
        size_t max_vl = __riscv_vsetvlmax_e8m1();
        vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, max_vl);
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                vuint8m1_t v_src_b, v_src_g, v_src_r, v_src_a;
                LV_RVV_LOAD_ARGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, v_src_a, vl);
                vuint8m1_t v_eff_a;
                LV_RVV_CALC_EFF_ALPHA_MASK_OPA_U8M1(v_src_a, v_mask, opa, v_eff_a, vl);
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, v_eff_a, v_r, v_g, v_b, vl);
                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_eff_a, vl);
                LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
            mask_buf += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

/**
 * ARGB8888 предварительно умножается на RGB888 / XRGB8888
 * Для предварительно умноженной альфа источник RGB уже умножен на альфу:
 *   src_premul = src * src_alpha / 255
 * формула смешивания: результат = src_premul + dst * (255 - src_alpha ) / 255
 *              = src_premul + (dst * (255 - src_alpha )) >> 8
 */
lv_result_t lv_draw_sw_blend_riscv_v_argb8888_premultiplied_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                      uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint8_t * src_buf = dsc->src_buf;
    size_t vl;

    if(dest_px_size == 3) {
        /* ARGB8888 предварительно умноженный -> RGB888 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_src_b, v_src_g, v_src_r, v_src_a;
                LV_RVV_LOAD_ARGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, v_src_a, vl);
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                vuint8m1_t v_inv_a = __riscv_vrsub_vx_u8m1(v_src_a, 255, vl);
                vuint16m2_t v_dst_r16 = __riscv_vzext_vf2_u16m2(v_dst_r, vl);
                vuint16m2_t v_dst_g16 = __riscv_vzext_vf2_u16m2(v_dst_g, vl);
                vuint16m2_t v_dst_b16 = __riscv_vzext_vf2_u16m2(v_dst_b, vl);
                vuint16m2_t v_inv_a16 = __riscv_vzext_vf2_u16m2(v_inv_a, vl);
                vuint16m2_t v_r16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vv_u16m2(v_dst_r16, v_inv_a16, vl), 8, vl);
                vuint16m2_t v_g16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vv_u16m2(v_dst_g16, v_inv_a16, vl), 8, vl);
                vuint16m2_t v_b16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vv_u16m2(v_dst_b16, v_inv_a16, vl), 8, vl);
                vuint8m1_t v_r = __riscv_vadd_vv_u8m1(v_src_r, __riscv_vnsrl_wx_u8m1(v_r16, 0, vl), vl);
                vuint8m1_t v_g = __riscv_vadd_vv_u8m1(v_src_g, __riscv_vnsrl_wx_u8m1(v_g16, 0, vl), vl);
                vuint8m1_t v_b = __riscv_vadd_vv_u8m1(v_src_b, __riscv_vnsrl_wx_u8m1(v_b16, 0, vl), vl);

                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_src_a, vl);

                LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }
    else {
        /* ARGB8888 предварительно умноженный -> XRGB8888 */
        size_t max_vl = __riscv_vsetvlmax_e8m1();
        vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, max_vl);
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_src_b, v_src_g, v_src_r, v_src_a;
                LV_RVV_LOAD_ARGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, v_src_a, vl);
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                vuint8m1_t v_inv_a = __riscv_vrsub_vx_u8m1(v_src_a, 255, vl);
                vuint16m2_t v_dst_r16 = __riscv_vzext_vf2_u16m2(v_dst_r, vl);
                vuint16m2_t v_dst_g16 = __riscv_vzext_vf2_u16m2(v_dst_g, vl);
                vuint16m2_t v_dst_b16 = __riscv_vzext_vf2_u16m2(v_dst_b, vl);
                vuint16m2_t v_inv_a16 = __riscv_vzext_vf2_u16m2(v_inv_a, vl);
                vuint16m2_t v_r16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vv_u16m2(v_dst_r16, v_inv_a16, vl), 8, vl);
                vuint16m2_t v_g16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vv_u16m2(v_dst_g16, v_inv_a16, vl), 8, vl);
                vuint16m2_t v_b16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vv_u16m2(v_dst_b16, v_inv_a16, vl), 8, vl);
                vuint8m1_t v_r = __riscv_vadd_vv_u8m1(v_src_r, __riscv_vnsrl_wx_u8m1(v_r16, 0, vl), vl);
                vuint8m1_t v_g = __riscv_vadd_vv_u8m1(v_src_g, __riscv_vnsrl_wx_u8m1(v_g16, 0, vl), vl);
                vuint8m1_t v_b = __riscv_vadd_vv_u8m1(v_src_b, __riscv_vnsrl_wx_u8m1(v_b16, 0, vl), vl);

                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_src_a, vl);

                LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }

    return LV_RESULT_OK;
}

#endif /* LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_RISCV_V */