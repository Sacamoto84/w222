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


#ifndef NEMA_BLENDER_H__
#define NEMA_BLENDER_H__

#include "nema_sys_defs.h"
#include "nema_graphics.h"

#ifdef __cplusplus
extern "C" {
#endif

// Выбор коэффициента смешивания
//-----------------------------------------------------------------------------------------------------------------------
#define NEMA_BF_ZERO         (0x0U) /**< 0 */
#define NEMA_BF_ONE          (0x1U) /**< 1 */
#define NEMA_BF_SRCCOLOR     (0x2U) /**< Sc */
#define NEMA_BF_INVSRCCOLOR  (0x3U) /**< (1-Sc) */
#define NEMA_BF_SRCALPHA     (0x4U) /**< Sa */
#define NEMA_BF_INVSRCALPHA  (0x5U) /**< (1-Sa) */
#define NEMA_BF_DESTALPHA    (0x6U) /**< Da */
#define NEMA_BF_INVDESTALPHA (0x7U) /**< (1-Da) */
#define NEMA_BF_DESTCOLOR    (0x8U) /**< Dc */
#define NEMA_BF_INVDESTCOLOR (0x9U) /**< (1-Dc) */
#define NEMA_BF_CONSTCOLOR   (0xaU) /**< Cc */
#define NEMA_BF_CONSTALPHA   (0xbU) /**< Ca */

    /*                  Фактор источника Фактор назначения */
#define NEMA_BL_SIMPLE     (  (uint32_t)NEMA_BF_SRCALPHA      |   ((uint32_t)NEMA_BF_INVSRCALPHA  <<8)  )   /**< Sa * Sa + Da * (1 - Sa) */
#define NEMA_BL_CLEAR      (  (uint32_t)NEMA_BF_ZERO        /*|   ((uint32_t)NEMA_BF_ZERO<<8)*/)   /**< 0 */
#define NEMA_BL_SRC        (  (uint32_t)NEMA_BF_ONE         /*|   ((uint32_t)NEMA_BF_ZERO<<8)*/)   /**< Sa */
#define NEMA_BL_SRC_OVER   (  (uint32_t)NEMA_BF_ONE           |   ((uint32_t)NEMA_BF_INVSRCALPHA  <<8)  )   /**< Sa + Da * (1 - Sa) */
#define NEMA_BL_DST_OVER   (  (uint32_t)NEMA_BF_INVDESTALPHA  |   ((uint32_t)NEMA_BF_ONE          <<8)  )   /**< Sa * (1 - Da) + Da */
#define NEMA_BL_SRC_IN     (  (uint32_t)NEMA_BF_DESTALPHA   /*|   ((uint32_t)NEMA_BF_ZERO<<8)*/)   /**< Sa * Da */
#define NEMA_BL_DST_IN     (/*(uint32_t)NEMA_BF_ZERO|*/ ((uint32_t)NEMA_BF_SRCALPHA     <<8)  )   /**< Da * Sa */
#define NEMA_BL_SRC_OUT    (  (uint32_t)NEMA_BF_INVDESTALPHA/*|   ((uint32_t)NEMA_BF_ZERO<<8)*/ )   /**< Sa * (1 - Da) */
#define NEMA_BL_DST_OUT    (/*(uint32_t)NEMA_BF_ZERO|*/ ((uint32_t)NEMA_BF_INVSRCALPHA  <<8)  )   /**< Da * (1 - Sa) */
#define NEMA_BL_SRC_ATOP   (  (uint32_t)NEMA_BF_DESTALPHA     |   ((uint32_t)NEMA_BF_INVSRCALPHA  <<8)  )   /**< Sa * Da + Da * (1 - Sa) */
#define NEMA_BL_DST_ATOP   (  (uint32_t)NEMA_BF_INVDESTALPHA  |   ((uint32_t)NEMA_BF_SRCALPHA     <<8)  )   /**< Sa * (1 - Da) + Da * Sa */
#define NEMA_BL_ADD        (  (uint32_t)NEMA_BF_ONE           |   ((uint32_t)NEMA_BF_ONE          <<8)  )   /**< Sa + Da */
#define NEMA_BL_XOR        (  (uint32_t)NEMA_BF_INVDESTALPHA  |   ((uint32_t)NEMA_BF_INVSRCALPHA  <<8)  )   /**< Sa * (1 - Da) + Da * (1 - Sa) */


#define NEMA_BLOP_NONE         (0U)           /**< No extra blending operation */
#define NEMA_BLOP_RECOLOR      (0x00100000U)  /**< Cconst*Aconst + Csrc*(1-Aconst). Overrides MODULATE_RGB. On NemaP GPU, recolor is available only when HW Rop Blender is enabled */
#define NEMA_BLOP_LUT          (0x00200000U)  /**< src_tex as index, src2_tex as palette */
#define NEMA_BLOP_STENCIL_XY   (0x00400000U)  /**< Use TEX3 as mask/stencil */
#define NEMA_BLOP_STENCIL_TXTY (0x00800000U)  /**< Use TEX3 as mask/stencil */
#define NEMA_BLOP_NO_USE_ROPBL (0x01000000U)  /**< Don't use Rop Blender even if present */
#define NEMA_BLOP_DST_CKEY_NEG (0x02000000U)  /**< Apply Inverse Destination Color Keying - draw only when dst color doesn't match colorkey*/
#define NEMA_BLOP_SRC_PREMULT  (0x04000000U)  /**< Premultiply Source Color with Source Alpha (cannot be used with NEMA_BLOP_MODULATE_RGB) */
#define NEMA_BLOP_MODULATE_A   (0x08000000U)  /**< Modulate by Constant Alpha value*/
#define NEMA_BLOP_FORCE_A      (0x10000000U)  /**< Force Constant Alpha value */
#define NEMA_BLOP_MODULATE_RGB (0x20000000U)  /**< Modulate by Constant Color (RGB) values */
#define NEMA_BLOP_SRC_CKEY     (0x40000000U)  /**< Apply Source Color Keying - draw only when src color doesn't match colorkey */
#define NEMA_BLOP_DST_CKEY     (0x80000000U)  /**< Apply Destination Color Keying - draw only when dst color matches colorkey */
#define NEMA_BLOP_MASK         (0xfff00000U)

/** \brief Возвращаемый режим колебаний с учетом коэффициентов изменения источника и назначения и дополнительных операций переключения
 *
 * \param src Коэффициент переменного источника
 * \param dst Коэффициент изменения назначения
 * \param ops Дополнительные операции переключения
 * \return Окончательный режим наложений
 *
 */
static inline uint32_t nema_blending_mode(uint32_t src_bf, uint32_t dst_bf, uint32_t blops) {
    return ( (src_bf) | (dst_bf << 8) | (blops&NEMA_BLOP_MASK) );
}

/** \brief Включить режим наложения
 *
 * \paramblending_modeРежим приложения, который необходимо установить
 * \paramdst_texТекстура назначения
 * \paramfg_texПередний план (исходник) Текстура
 * \parambg_texФон (источник2) Текстура
 *
 */
void nema_set_blend(uint32_t blending_mode, nema_tex_t dst_tex, nema_tex_t fg_tex, nema_tex_t bg_tex);

/** \brief Включить режим наложения для заливки
 *
 * \paramblending_modeРежим приложения, который необходимо установить
 *
 */
static inline void nema_set_blend_fill(uint32_t blending_mode) {
    nema_set_blend(blending_mode, NEMA_TEX0, NEMA_NOTEX, NEMA_NOTEX);
}

/** \brief Установить режим наложения для заливки композиции
 *
 * \paramblending_modeРежим приложения, который необходимо установить
 *
 */
static inline void nema_set_blend_fill_compose(uint32_t blending_mode) {
    nema_set_blend(blending_mode, NEMA_TEX0, NEMA_NOTEX, NEMA_TEX2);
}

/** \brief Установка режима наложения для блитинга
 *
 * \paramblending_modeРежим приложения, который необходимо установить
 *
 */
static inline void nema_set_blend_blit(uint32_t blending_mode) {
    nema_set_blend(blending_mode, NEMA_TEX0, NEMA_TEX1, NEMA_NOTEX);
}

/** \brief Установите режим приложения для копирования с компоновкой
 *
 * \paramblending_modeРежим приложения, который необходимо установить
 *
 */
static inline void nema_set_blend_blit_compose(uint32_t blending_mode) {
    nema_set_blend(blending_mode, NEMA_TEX0, NEMA_TEX1, NEMA_TEX2);
}

/** \brief Поддержание постоянного цвета
 *
 * \param rgbaRGBAцвет
 * \см. nema_rgba()
 *
 */
void nema_set_const_color(uint32_t rgba);


/** \brief Установить цвет перекрашивания. Переопределяет постоянный цвет
 *
 * \param rgbaRGBAцвет
 * \см. nema_rgba(),nema_set_const_color()
 *
 */
void nema_set_recolor_color(uint32_t rgba);

/** \brief Возврат исходного цветочного ключа
 *
 * \param цветной ключ rgba RGBA
 * \см. nema_rgba()
 *
 */
void nema_set_src_color_key(uint32_t rgba);

/** \brief Установка цветов для назначения
 *
 * \param цветной ключ rgba RGBA
 * \см. nema_rgba()
 *
 */
void nema_set_dst_color_key(uint32_t rgba);


/** \brief Включение/выключение отладки overdraw. Отключает градиент и текстуру, устанавливает режим наложенияNEMA_BL_ADD.
 *
 * \param Enable Включает отладку с перерисовкой, если ненулевое значение.
 *
 */
void nema_debug_overdraws(uint32_t enable);

#ifdef __cplusplus
}
#endif

#endif // NEMA_BLENDER_H__
