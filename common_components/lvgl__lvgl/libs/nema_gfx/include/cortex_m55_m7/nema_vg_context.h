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
 * @brief Контекстный интерфейс NemaVG.
 *
 * Содержит коды ошибок NemaVG, правила определения, определения качества рендеринга и функции для обновления различных параметров рендеринга.
 * Определенные здесь функции можно использовать для доступа к параметрам контекста. Контекст — это внутренняя (непрозрачная) структура NemaVG.
 */

#ifndef __NEMA_VG_CONTEXT_H__
#define __NEMA_VG_CONTEXT_H__

#include "nema_graphics.h"
#include "nema_matrix3x3.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifndef NEMA_VG_HANDLE
#define NEMA_VG_HANDLE void* /**< NemaVG handle object (void pointer)*/
#endif

#define NEMA_VG_PATH_HANDLE NEMA_VG_HANDLE  /**< NemaVG path handle (pointer to path object)*/
#define NEMA_VG_PAINT_HANDLE NEMA_VG_HANDLE /**< NemaVG paint handle (pointer to paint object)*/
#define NEMA_VG_GRAD_HANDLE NEMA_VG_HANDLE  /**< NemaVG gradient handle (pointer to gradient object)*/

typedef float nema_vg_float_t; /**< Floating point data type (default is 'float') */

#define NEMA_VG_ERR_NO_ERROR                (0x00000000U)  /**< No Error */
#define NEMA_VG_ERR_BAD_HANDLE              (0x00000001U)  /**< Bad handle */
#define NEMA_VG_ERR_BAD_BUFFER              (0x00000002U)  /**< Bad buffer */
#define NEMA_VG_ERR_INVALID_FILL_RULE       (0x00000004U)  /**< Invalid fill rule*/
#define NEMA_VG_ERR_INVALID_PAINT_TYPE      (0x00000008U)  /**< Invalid paint type*/
#define NEMA_VG_ERR_INVALID_VERTEX_DATA     (0x00000010U)  /**< Invalid vertex data*/
#define NEMA_VG_ERR_NO_RADIAL_ENABLED       (0x00000020U)  /**< Radial not present in HW*/
#define NEMA_VG_ERR_NO_BOUND_CL             (0x00000040U)  /**< No bound CL*/
#define NEMA_VG_ERR_INVALID_ARGUMENTS       (0x00000080U)  /**< Invalid arguments*/
#define NEMA_VG_ERR_INVALID_ARC_DATA        (0x00000100U)  /**< reserved */
#define NEMA_VG_ERR_CL_FULL                 (0x00000200U)  /**< reserved */
#define NEMA_VG_ERR_DRAW_OUT_OF_BOUNDS      (0x00000400U)  /**< Path is out of the drawing area */
#define NEMA_VG_ERR_INVALID_MASKING_OBJ     (0x00000800U)  /**< Masking object was not set */
#define NEMA_VG_ERR_INVALID_MASKING_FORMAT  (0x00001000U)  /**< Invalid Masking object Format */
#define NEMA_VG_ERR_INVALID_LUT_IDX_FORMAT  (0x00002000U)  /**< Invalid LUT indices object Format */
#define NEMA_VG_ERR_COORDS_OUT_OF_RANGE     (0x00004000U)  /**< Path coordinates out of supported range */
#define NEMA_VG_ERR_EMPTY_TSVG              (0x00008000U)  /**< Tsvg has no geometries */
#define NEMA_VG_ERR_NO_BOUND_FONT           (0x00010000U)  /**< There is no bound font */
#define NEMA_VG_ERR_UNSUPPORTED_FONT        (0x00020000U)  /**< The font is not supported (eg. older version) by NemaVG API */
#define NEMA_VG_ERR_NON_INVERTIBLE_MATRIX   (0x00040000U)  /**< A matrix that needs to be inverted, is not invertible */
#define NEMA_VG_ERR_INVALID_GRAD_STOPS      (0x00080000U)  /**< Gradient stops exceed maximum available stops */
#define NEMA_VG_ERR_NO_INIT                 (0x00100000U)  /**< VG uninitialized */
#define NEMA_VG_ERR_INVALID_STROKE_WIDTH    (0x00200000U)  /**< Invalid stroke width */
#define NEMA_VG_ERR_INVALID_OPACITY         (0x00400000U)  /**< Invalid opacity */
#define NEMA_VG_ERR_INVALID_CAP_STYLE       (0x00800000U)  /**< Invalid cap style */
#define NEMA_VG_ERR_INVALID_JOIN_STYLE      (0x01000000U)  /**< Invalid join style */
#define NEMA_VG_ERR_INVALID_STENCIL_SIZE    (0x02000000U)  /**< Invalid stencil buffer size */
#define NEMA_VG_ERR_DEPRECATED_FONT         (0x04000000U)  /**< The font version is an old one, but it is still compatible with NemaVG API*/

#define NEMA_VG_FILL_DRAW                   (0x00U) /**< DEPRECATED Stroke fill rule */
#define NEMA_VG_STROKE                      (0x00U) /**< Stroke fill rule */
#define NEMA_VG_FILL_EVEN_ODD               (0x01U) /**< Evenodd fill rule */
#define NEMA_VG_FILL_NON_ZERO               (0x02U) /**< Non zero fill rule */

#define NEMA_VG_QUALITY_BETTER              (0x00U) /**< Better rendering quality (default option, balances rendering quality and performance)*/
#define NEMA_VG_QUALITY_FASTER              (0x01U) /**< Faster rendering quality (favors performance over rendering quality)*/
#define NEMA_VG_QUALITY_MAXIMUM             (0x02U) /**< Maximum rendering quality (favors rendering quality over performance)*/
#define NEMA_VG_QUALITY_NON_AA              (0x10U) /**< Rendering quality without AA*/

#define NEMA_VG_CAP_BUTT                    (0x00U) /**< Butt cap*/
#define NEMA_VG_CAP_ROUND                   (0x01U) /**< Round cap*/
#define NEMA_VG_CAP_SQUARE                  (0x02U) /**< Square cap*/
#define NEMA_VG_CAP_MAX                     (0x03U) /**< Max value for cap*/

#define NEMA_VG_JOIN_BEVEL                  (0x00U) /**< Bevel join*/
#define NEMA_VG_JOIN_MITER                  (0x01U) /**< Mitter join*/
#define NEMA_VG_JOIN_ROUND                  (0x02U) /**< Round join*/
#define NEMA_VG_JOIN_MAX                    (0x03U) /**< Max for join*/

#define NEMA_VG_TSVG_DISABLE_NONE          (0x00000000U) /**< Disable none*/
#define NEMA_VG_TSVG_DISABLE_CAPS          (0x00000001U) /**< Disable caps*/
#define NEMA_VG_TSVG_DISABLE_JOINS         (0x00000002U) /**< Disable joins*/
#define NEMA_VG_TSVG_DISABLE_DASHING       (0x00000003U) /**< Disable dashing*/

/** \brief Установите глобальную матрицу преобразования. Глобальная матрица будет применяться во всех операциях рендеринга NemaVG.
 *
 * \param m матрица конвертировать
 *
 * \return Код ошибки
 */
uint32_t nema_vg_set_global_matrix(nema_matrix3x3_t m);


/** \brief Отключить глобальную матрицу преобразования.
 *
 */
void nema_vg_reset_global_matrix(void);

/** \brief Установите правило заливки, которое будет применяться при отрисовке пути.
 *
 * \param Правило заполненияfill_rule(NEMA_VG_STROKE,NEMA_VG_FILL_EVEN_ODD,NEMA_VG_FILL_NON_ZERO)
 *
 */
void nema_vg_set_fill_rule(uint8_t fill_rule);

/** \brief Установите необходимые обводки, которые будут применяться к пути обводки.
 *
 * \param width Устанавливаемая ширина обводки
 *
 */
void nema_vg_stroke_set_width(float width);

 /** \brief Установить стиль обводки
 *
 * \paramcap_styleСтиль шапки (NEMA_VG_CAP_BUTT|NEMA_VG_CAP_SQUARE|NEMA_VG_CAP_ROUND)
 *
 */
void nema_vg_stroke_set_cap_style(uint8_t start_cap_style, uint8_t end_cap_style);

 /** \краткий стиль Установить соединения штрихов
 *
 * \paramjoin_styleСтиль соединения (NEMA_VG_JOIN_BEVEL|NEMA_VG_JOIN_MITER|NEMA_VG_JOIN_ROUND)
 *
 */
void nema_vg_stroke_set_join_style(uint8_t join_style);

 /** \brief Установка ограничений по углу поворота
 * Если выбрано соединение под углом и длина угла больше изделия
 * вместо ограничения угла скоса и ширины штриха будет добавлено соединение фасок
 *
 * \parammiter_limitпредел соединения под углом, который необходимо установить
 *
 */
void nema_vg_stroke_set_miter_limit(float miter_limit);

/** \brief Установите образец шины.
 * Штрих включен, когда размер массива штрихов больше нуля.
 * Шаблон штриха состоит из массива плавающих элементов, которые чередуют фазы включения и выключения сегментов штриха.
 * Первое значение указывает на первый включенный сегмент, второе значение указывает на первый выключенный сегмент и каждый
 * последующая пара указывает следующую пару включения-выключения. Размер массива тире всегда должен быть четным числом.
 * Если указан нечетный размер массива тире, последний элемент игнорируется библиотекой.
 * Допускаются сегменты нулевой длины, и на этих сегментах будут нарисованы только заглушки, если они
 * либо NEMA_VG_CAP_ROUND, либо NEMA_VG_CAP_SQUARE. Если выбран колпачок NEMA_VG_CAP_BUTT, ничего не будет нарисовано.
 *
 * \paramdash_patternуказатель на массив штриховых шаблонов
 * \paramdash_pattern_sizeРазмер фазовой решетки штриха
 *
 */
void nema_vg_stroke_set_dash_pattern(const float* dash_pattern, size_t dash_pattern_size);

/** \brief Установите фазу шины.
 * Указывает длину отбрасываемого штрихового образца.
 * при первой обработке подпути. После достижения фазы рывка следующая
 * путь обведен штрихом. Отрицательная фаза тире эквивалентна положительной
 * Фаза штриха рассчитывается путем добавления подходящего кратного длины штриха.
 *
 * \paramdash_phaseшина фазы
 *
 */
void nema_vg_stroke_set_dash_phase(float dash_phase);

/** \brief Включить/отключить фазу сброса шины
 *
 * \param Включить логический параметр, можно включить/отключить фазу шины. Когда
 * эта функция включена, указанная фаза тире используется в начале каждого подпути
 * исходный путь, в противном случае то, что осталось от фазы тире в конце подпути, используется для
 * следующий подпуть. Значение по умолчанию — ложь.
 *
 */
void nema_vg_stroke_reset_dash_phase(uint8_t enable);

/** \brief Включение/отключение маскировки.
 *
 * \param маскирование: 1 — включить, 0 — выключить.
 *
 */
void nema_vg_masking(uint8_t masking);

/** \brief Установка объекта маски (текстуры)
 *
 * \parammask_objТекстура, которая будет использоваться в качестве маски. Его формат должен бытьNEMA_A1,NEMA_A2,NEMA_A4илиNema_A8, иначе он вернет ошибку.
 * \return Код ошибки. Если ошибки не возникают,NEMA_VG_ERR_NO_ERROR, иначеNEMA_VG_ERR_INVALID_MASKING_FORMAT.
 *
 */
uint32_t nema_vg_set_mask(nema_img_obj_t *mask_obj);

/** \brief Переместите объект маски (текстуру) относительно исходной точки (0, 0). Устанавливает положение маски.
 *
 * \param x Горизонтальное положение для размещения зон видимости
 * \param y Горизонтальное положение для размещения зон видимости
 *
 */
void nema_vg_set_mask_translation(float x, float y);

/** \brief Настройка качества рендеринга
 *
 * уровень качества \param (NEMA_VG_QUALITY_BETTER,NEMA_VG_QUALITY_FASTER,NEMA_VG_QUALITY_MAXIMUM,NEMA_VG_QUALITY_NON_AA)
 *
 */
void nema_vg_set_quality(uint8_t quality);

/** \brief Настройка режима переключения для операцийVG(см. документациюnema_blender.hв руководстве NemaGFXAPI)
 *  Дополнительные операции смешивания: поддерживается только NEMA_BLOP_SRC_PREMULT.
 *
 * \param blend Режим наложения
 * \см. nema_blending_mode()
 *
 */
void nema_vg_set_blend(uint32_t blend);

/** \brief Получен настоящий код ошибки. Сбрасывает ошибку потом.
 *
 * \return Код ошибки. См. NEMA_VG_ERR_*, определяющий все возможные коды ошибок.
 */
uint32_t nema_vg_get_error(void);

/** \brief Включить/отключить обработку координат в большом рендерингеTSVG, пути или предопределенной формы.
 *
 * \param Включить 0 — отключить, 1 — включить
 * \paramallow_internal_alloc0, чтобы запретить получение знаний, 1, чтобы разрешить
 *
 */
void nema_vg_handle_large_coords(uint8_t enable, uint8_t allow_internal_alloc);

/** \brief Свяжите сегменты и буферы данных, которые будут использоваться для обработки больших координат.
 *
 * \param segs Указатель на сегменты массива (uint8_t) для больших координат
 * \paramsegs_sizeРазмер сегментов массива
 * \param data Указатель на массив данных (с плавающей запятой) для большой координаты
 * \paramdata_sizeРазмер массива данных
 *
 */
uint32_t nema_vg_bind_clip_coords_buffer(void *segs, uint32_t segs_size, void *data, uint32_t data_size);

/** \private */
uint32_t nema_vg_bind_clip_coords_buf(void *segs, uint32_t segs_size_bytes, void *data, uint32_t data_size_bytes) __attribute__ ((deprecated("This function is deprecated use nema_vg_bind_clip_coords_buffer instead.")));


/** \brief Отмените привязку сегмента и буферов данных, которые будут использоваться для обработки больших координат.
 *
 *
 */
void nema_vg_unbind_clip_coords_buf(void);

#ifdef __cplusplus
}
#endif

#endif //__NEMA_VG_CONTEXT_H__
