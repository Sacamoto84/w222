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


#ifndef NEMA_GRAPHICS_H__
#define NEMA_GRAPHICS_H__

#include "nema_sys_defs.h"
#include "nema_hal.h"
#include "nema_matrix3x3.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _img_obj_ {
    nema_buffer_t bo;
    uint16_t w;
    uint16_t h;
    int      stride;
    uint32_t color;
    uint8_t  format;
    uint8_t  sampling_mode; // NEMA_FILTER*
} img_obj_t;

typedef img_obj_t nema_img_obj_t;

typedef enum {
    NEMA_NOTEX  = -1,    /**< No Texture */
    NEMA_TEX0   =  0,    /**< Texture 0 */
    NEMA_TEX1   =  1,    /**< Texture 1 */
    NEMA_TEX2   =  2,    /**< Texture 2 */
    NEMA_TEX3   =  3,    /**< Texture 3 */
    NEMA_TEXMAX =  4     /**< Enum max value */
} nema_tex_t;

#define NEMA_RGBX8888   0x00U  /**< RGBX8888 */
#define NEMA_RGBA8888   0x01U  /**< RGBA8888 */
#define NEMA_XRGB8888   0x02U  /**< XRGB8888 */
#define NEMA_ARGB8888   0x03U  /**< ARGB8888 */
#define NEMA_RGB565     0x04U  /**< RGBA5650 */
#define NEMA_RGBA5650   0x04U  /**< RGBA5650 */
#define NEMA_RGBA5551   0x05U  /**< RGBA5551 */
#define NEMA_RGBA4444   0x06U  /**< RGBA4444 */
#define NEMA_RGBA0800   0x07U  /**< RGBA0800 */
#define NEMA_A8         0x08U  /**< RGBA0008 */
#define NEMA_RGBA0008   0x08U  /**< RGBA0008 */
#define NEMA_L8         0x09U  /**< L8       */
#define NEMA_RGBA3320   0x38U  /**< RGBA3320 (source only) */
#define NEMA_RGB332     0x38U  /**< RGBA3320 (source only) */
#define NEMA_BW1        0x0CU  /**< A1       (source only) */
#define NEMA_A1         0x0CU  /**< A1       (source only) */
#define NEMA_L1         0x0BU  /**< L1       (source only) */
#define NEMA_UYVY       0x0DU  /**< UYVY     */
#define NEMA_ABGR8888   0x0EU  /**< ABGR8888 */
#define NEMA_XBGR8888   0x0FU  /**< XBGR8888 */
#define NEMA_BGRA8888   0x10U  /**< BGRA     */
#define NEMA_BGRX8888   0x11U  /**< BGRX     */
#define NEMA_TSC4       0x12U  /**< TSC4     */
#define NEMA_TSC6       0x16U  /**< TSC6     */
#define NEMA_TSC6A      0x17U  /**< TSC6A    */
#define NEMA_RV         0x18U  /**< RV       */
#define NEMA_GU         0x19U  /**< GU       */
#define NEMA_BY         0x1AU  /**< BY       */
#define NEMA_YUV        0x1BU  /**< YUV      */
#define NEMA_Z24_8      0x1cU  /**< Z24_8    */
#define NEMA_Z16        0x1dU  /**< Z16      */
#define NEMA_UV         0x1eU  /**< UV       */
#define NEMA_A1LE       0x27U  /**< A1LE     (source only) */
#define NEMA_A2LE       0x28U  /**< A2LE     (source only) */
#define NEMA_A4LE       0x29U  /**< A4LE     (source only) */
#define NEMA_L1LE       0x2AU  /**< L1LE     (source only) */
#define NEMA_L2LE       0x2BU  /**< L2LE     (source only) */
#define NEMA_L4LE       0x2CU  /**< L4LE     (source only) */
#define NEMA_A2         0x30U  /**< A2       (source only) */
#define NEMA_A4         0x34U  /**< A4       (source only) */
#define NEMA_L2         0x31U  /**< L2       (source only) */
#define NEMA_L4         0x35U  /**< L4       (source only) */
#define NEMA_BGR24      0x39U  /**< BGR24    */
#define NEMA_RGB24      0x3CU  /**< RGB24    */
#define NEMA_RV10       0x3DU  /**< RV-10bit */
#define NEMA_GU10       0x3EU  /**< GU-10bit */
#define NEMA_BY10       0x3FU  /**< BY-10bit */
#define NEMA_RGBA2222   0x40U  /**< RGBA2222 (Available if HW enabled - check HW manual) */
#define NEMA_ABGR2222   0x41U  /**< ABGR2222 (Available if HW enabled - check HW manual) */
#define NEMA_BGRA2222   0x42U  /**< BGRA2222 (Available if HW enabled - check HW manual) */
#define NEMA_ARGB2222   0x43U  /**< ARGB2222 (Available if HW enabled - check HW manual) */
#define NEMA_AL88       0x44U  /**< AL88     (Available if HW enabled - check HW manual) */
#define NEMA_AL44       0x45U  /**< AL44     (Available if HW enabled - check HW manual) */
#define NEMA_ARGB1555   0x46U  /**< ARGB1555 (Available if HW enabled - check HW manual) */
#define NEMA_ARGB4444   0x47U  /**< ARGB4444 (Available if HW enabled - check HW manual) */
#define NEMA_BGRA5650   0x13U  /**< BGRA5650 (Available if HW enabled - check HW manual) */
#define NEMA_BGR565     0x13U  /**< BGRA5650 (Available if HW enabled - check HW manual) */
#define NEMA_BGRA5551   0x48U  /**< BGRA5551 (Available if HW enabled - check HW manual) */
#define NEMA_ABGR1555   0x49U  /**< ABGR1555 (Available if HW enabled - check HW manual) */
#define NEMA_BGRA4444   0x4aU  /**< BGRA4444 (Available if HW enabled - check HW manual) */
#define NEMA_ABGR4444   0x4bU  /**< ABGR4444 (Available if HW enabled - check HW manual) */
#define NEMA_TSC12      0x4cU  /**< TSC12    (Available if HW enabled - check HW manual) */
#define NEMA_TSC12A     0x4dU  /**< TSC12A   (Available if HW enabled - check HW manual) */
#define NEMA_TSC6AP     0x4eU  /**< TSC6AP   (Available if HW enabled - check HW manual) */

#define NEMA_DITHER     0x80U  /**< Nema Dithering */
#define NEMA_FORMAT_MASK 0x7FU /**< Format Mask */

typedef uint32_t nema_tex_format_t;

//-----------------------------------------------------------------------------------------------------------------------

// Параметры текстурного блока
//-----------------------------------------------------------------------------------------
    // Фильтрация - 0:0
    //----------------------
#define NEMA_FILTER_PS  0x00U /**< Point Sampling. */
#define NEMA_FILTER_BL  0x01U /**< Bilinear filtering. */

    // Режим упаковки 3:2
    //----------------------
#define NEMA_TEX_CLAMP  (0x00U)    /**< Clamp */
#define NEMA_TEX_REPEAT (0x01U<<2) /**< Repeat */
#define NEMA_TEX_BORDER (0x02U<<2) /**< Border */
#define NEMA_TEX_MIRROR (0x03U<<2) /**< Mirror */

    // Порядок текстурных координат 4:4
    //----------------------
#define NEMA_TEX_MORTON_ORDER (0x10U)

    // Формат текстурных координат 6:5
    //----------------------
#define NEMA_TEX_RANGE_0_1   (0x1U<<5)  /**< Interpolated Coordinates range: 0-1 */
#define NEMA_TEX_LEFT_HANDED (0x1U<<6)  /**< (0,0) is bottom left corner */

typedef uint8_t nema_tex_mode_t;

// Отбор треугольников
//-----------------------------------------------------------------------------------------
typedef enum {
    NEMA_CULL_NONE = 0      , /**< Disable Triangle/Quadrilateral Culling */
    NEMA_CULL_CW   = (1U<<28),   /**< Cull clockwise Triangles/Quadrilaterals */
    NEMA_CULL_CCW  = (1U<<29),  /**< Cull anti-clockwise Triangles/Quadrilaterals */
    NEMA_CULL_ALL  = NEMA_CULL_CW | NEMA_CULL_CCW   /**< Cull all */
} nema_tri_cull_t;

// Режимы вращения
//-----------------------------------------------------------------------------------------------------------------------

#define NEMA_ROT_000_CCW  (0x0U) /**< No rotation */
#define NEMA_ROT_090_CCW  (0x1U) /**< Rotate  90 degrees counter-clockwise */
#define NEMA_ROT_180_CCW  (0x2U) /**< Rotate 180 degrees counter-clockwise */
#define NEMA_ROT_270_CCW  (0x3U) /**< Rotate 270 degrees counter-clockwise */
#define NEMA_ROT_000_CW   (0x0U) /**< No rotation */
#define NEMA_ROT_270_CW   (0x1U) /**< Rotate 270 degrees clockwise */
#define NEMA_ROT_180_CW   (0x2U) /**< Rotate 180 degrees clockwise */
#define NEMA_ROT_090_CW   (0x3U) /**< Rotate  90 degrees clockwise */
#define NEMA_MIR_VERT     (0x4U) /**< Mirror Vertically */
#define NEMA_MIR_HOR      (0x8U)  /**< Mirror Horizontally */


/** \brief проверка, присутствие известных GPU
 *
 * \return -1, если известныйGPUне присутствует
 *
 */
int nema_checkGPUPresence(void);

// ------------------------------ TEXTURES -------------------------------------

/** \brief Программирование текстурного модуля
 *
 * \param texid Текстурный блок, который необходимо запрограммировать
 * \paramaddr_gpuАдрес текстуры, встречается GPU
 * \param width Ширина текстуры
 * \param height Высота текстуры
 * \param format Формат текстур
 * \param stride Шаг текстуры. Если шаг < 0, его еще предстоит вычислить.
 * \paramwrap_modeиспользуется режим переноса/повтора. При использовании «повторения» или «зеркала» размеры текстур должны быть двойными. В противном случае поведение не определено.
 *
 */
void nema_bind_tex(nema_tex_t texid, uintptr_t addr_gpu,
                   uint32_t width, uint32_t height,
                   nema_tex_format_t format, int32_t stride, nema_tex_mode_t wrap_mode);

/** \brief Установить цвет по умолчанию для наложения текстуры
 *
 * \param цвет цвета по умолчанию в 32-битном формате RGBA
 * \см. nema_rgba()
 *
 */
void nema_set_tex_color(uint32_t color);

// ------------------------------ CONSTREGS ------------------------------------

/** \brief Запись значений в регистр константы GPU
 *
 * \param reg Постоянный регистр для записей
 * Значение \param для записей
 *
 */
void nema_set_const_reg(int reg, uint32_t value);

// --------------------------------- CLIP --------------------------------------

/** \brief Установка окружает области рисования.
 *
 * \param x Координата x в верхнем нижнем углу окна, клипа
 * \param y Минимальный оконный клип y
 * \param w Ширина окна клипа
 * \param h Высота окна клипа
 *
 */
void nema_set_clip(int32_t x, int32_t y, uint32_t w, uint32_t h);

void nema_enable_gradient(int enable);

void nema_enable_depth(int enable);

/** \brief ВключаетMSAAдля каждого ребра
 *
 * \param e0 ВключитьMSAAдля ребра 0 (вершины 0–1)
 * \param e1 ВключитьMSAAдля ребра 1 (вершины 1-2)
 * \param e2 ВключитьMSAAдля ребра 2 (вершины 2–3)
 * \param e3 ВключитьMSAAдля ребра 3 (вершины 3-0)
 * \вернуть предыдущие флаги AA (можно игнорировать)
 *
 */
uint32_t nema_enable_aa(uint8_t e0, uint8_t e1, uint8_t e2, uint8_t e3);

// ------------------------------- DIRTY REGIONS ------------------------------------

/** \brief Возвращает ограничение всех изображений, которые были изменены с момента обращения.
 * Доступно только на Nema|P и Nema| Графические процессоры PVG.
 *
 * \param minx x координата верхнего левого угла грязной области
 * \param miny y координата верхнего левого угла грязной области
 * \param maxx x координата нижнего правого угла загрязненной области
 * \param maxy координата y нижнего правого угла грязной области
 *
 */
void nema_get_dirty_region(int *minx, int *miny, int *maxx, int *maxy);

/** \brief Очистить информацию о грязной области — результат через связанный список команды.
 * Доступно только на Nema|P и Nema| Графические процессоры PVG.
 *
 * \см. nema_get_dirty_region()
 * \см. nema_clear_dirty_region_imm()
 *
 */
void nema_clear_dirty_region(void);

/** \brief Немедленно ухудшается динамика о грязной области, без использования списка команд.
 * Доступно только на Nema|P и Nema| Графические процессоры PVG.
 *
 * \см. nema_get_dirty_region()
 * \см. nema_clear_dirty_region()
 *
 */
void nema_clear_dirty_region_imm(void);

// -------------------------------- UTILS --------------------------------------

/** \brief Установить режим отсечения треугольников/четырехугольников
 *
 * \param cull Режим отсеивания
 *
 */
void nema_tri_cull(nema_tri_cull_t cull);

/** \brief Возвращает размер пикселя в байтах
 *
 * \param format Формат цвета
 * \return Размер пикселя в байтах
 *
 */
int nema_format_size (nema_tex_format_t format);

/** \brief Возвращаемый шаг в байтах
 *
 * \param format Формат цвета
 * \paramwrap_modeиспользуется режим переноса/повтора. При использовании «повторения» или «зеркала» размеры текстур должны быть двойными. В противном случае поведение не определено.
 * \param width Формат цвета текстуры
 * \return Шаг в байтах
 *
 */
int nema_stride_size(nema_tex_format_t format, nema_tex_mode_t wrap_mode, int width);


/** \brief Возвращает размер текстур в байтах
 *
 * \param format Формат цвета текстуры
 * \paramwrap_modeиспользуется режим переноса/повтора. При использовании «повторения» или «зеркала» размеры текстур должны быть двойными. В противном случае поведение не определено.
 * \param width Ширина текстуры
 * \param height Высота текстуры
 * \return Размер текстур в байтах
 *
 */
int nema_texture_size(nema_tex_format_t format, nema_tex_mode_t wrap_mode, int width, int height);

/** \brief Возврат внутреннего цвета Nema RGBA
 *
 * \param R Красный компонент
 * \param G Зеленый компонент
 * \param B Синий компонент
 * \param Альфа-компонент
 * \return значение RGBA
 *
 */
uint32_t nema_rgba(unsigned char R,
                   unsigned char G,
                   unsigned char B,
                   unsigned char A);

/** \brief Предварительное умножение каналовRGBс альфа-каналом
 *
 * \param rgbaRGBAцвет
 * \return Предварительно умноженный цвет RGBA
 *
 */
uint32_t nema_premultiply_rgba(uint32_t rgba);

/** \brief Инициализация библиотеки NemaGFX
 *
 * \вернуть отрицательное значение в случае ошибки
 *
 */
int nema_init(void);

/** \brief Повторная инициализация библиотеки NemaGFX
 *
 * \details Эта функция повторно реализует оригинальный код NemaGFX после выключенияGPU.
 * Выделение памяти для кольцевого буфера и т. д. не выполняется.
 *
 * \вернуть отрицательное значение в случае ошибки
 *
 */
int nema_reinit(void);

// ------------------------------- CONTEXT -------------------------------------

/** \brief Программный текстурный модуль с текстурой переднего плана (исходной) (NEMA_TEX1)
 *
 * \parambaseaddr_physАдрес исходной текстуры, видимый GPU
 * \param width Ширина текстуры
 * \param height Высота текстуры
 * \param format Формат текстур
 * \param stride Шаг текстуры. Если значение отрицательное, оно рассчитывается внутри компании.
 * \paramwrap_modeиспользуется режим переноса/повтора. При использовании «повторения» или «зеркала» размеры текстур должны быть двойными. В противном случае поведение не определено.
 *
 */
void nema_bind_src_tex(uintptr_t baseaddr_phys,
                       uint32_t width, uint32_t height,
                       nema_tex_format_t format, int32_t stride, nema_tex_mode_t mode);

/** \brief Программный текстурный блок с фоновой текстурой ((NEMA_TEX2)
 *
 * \parambaseaddr_physАдрес текстуры source2, появляется GPU
 * \param width Ширина текстуры
 * \param height Высота текстуры
 * \param format Формат текстур
 * \param stride Шаг текстуры. Если значение отрицательное, оно рассчитывается внутри компании.
 * \paramwrap_modeиспользуется режим переноса/повтора. При использовании «повторения» или «зеркала» размеры текстур должны быть двойными. В противном случае поведение не определено.
 *
 */
void nema_bind_src2_tex(uintptr_t baseaddr_phys,
                       uint32_t width, uint32_t height,
                       nema_tex_format_t format, int32_t stride, nema_tex_mode_t mode);

/** \brief Программный текстурный блок с вертикальной текстурой (NEMA_TEX0)
 *
 * \parambaseaddr_physАдрес встроенных текстур, появляется GPU
 * \param width Ширина текстуры
 * \param height Высота текстуры
 * \param format Формат текстур
 * \param stride Шаг текстуры. Если значение отрицательное, оно рассчитывается внутри компании.
 *
 */
void nema_bind_dst_tex(uintptr_t baseaddr_phys,
                        uint32_t width, uint32_t height,
                        nema_tex_format_t format, int32_t stride);

/** Программный текстурный блок с текстурой люта/палитры (NEMA_TEX2) и индексной текстурой (NEMA_TEX1_)
 *
 * \parambaseaddr_physАдрес индексной текстуры
 * \param width Ширина индексной текстуры
 * \param height Высота индексной текстуры
 * \param format Формат индексной текстуры
 * \param stride Индекс шага текстуры. Если значение отрицательное, оно рассчитывается внутри компании.
 * \param mode Режим выбора индексной текстуры.  При использовании режима переноски «NEMA_TEX_REPEAT» или «NEMA_TEX_MIRROR»
 *             размеры текстуры должны быть степенью двойки, иначе поведение не определено.  NEMA_FILTER_BL не поддерживается.
 * \parampalette_baseaddr_physАдрес текстуры lut/палитры
 * \parampalette_formatформат текстуры lut/палитры
 *
 */
void nema_bind_lut_tex( uintptr_t baseaddr_phys,
                        uint32_t width, uint32_t height,
                        nema_tex_format_t format, int32_t stride, nema_tex_mode_t mode,
                        uintptr_t palette_baseaddr_phys,
                        nema_tex_format_t palette_format);

/** \brief Привязка буфера глубины
 *
 * \parambaseaddr_physАдрес буфера напряжения, видимый GPU
 * \param width Ширина буфера
 * \param высота буфера
 *
 */
void nema_bind_depth_buffer(uintptr_t baseaddr_phys,
                             uint32_t width, uint32_t height);

/** \private */
// -------------------------------- DEPTH --------------------------------------
void nema_set_depth(float start, float dx, float dy);

// ------------------------------ GRADIENT -------------------------------------
/** \private */
void nema_set_gradient(float r_init, float g_init, float b_init, float a_init,
                       float r_dx, float r_dy,
                       float g_dx, float g_dy,
                       float b_dx, float b_dy,
                       float a_dx, float a_dy);

// ------------------------------- DRAWING -------------------------------------
/** \brief Четкая текстура назначения с цветом
 *
 * \param rgba8888 32-битный цвет RGBA
 * \см. nema_rgba()
 *
 */
void nema_clear(uint32_t rgba8888);

/** \brief Очистить буфер с этим значением
 *
 * \param val Очистить значение
 *
 */
void nema_clear_depth(uint32_t val);

/** \brief Нарисуйте цветную линию
 *
 * \param x0 координата x в начале строки
 * \param y0 координата y в начале строки
 * \param x1 координата x в конце строки
 * \param y1 координата y в конце строки
 * \param rgba8888 используемый цвет
 * \см. nema_set_blend_fill()
 * \см. nema_rgba()
 *
 */
void nema_draw_line(int x0, int y0, int x1, int y1, uint32_t rgba8888);


/** \brief Нарисуйте линию диапазона. ЗаменитеAA, если доступно.
 *
 * \param x0 координата x в начале строки
 * \param y0 координата y в начале строки
 * \param x1 координата x в конце строки
 * \param y1 координата y в конце строки
 * \param w ширина линии
 * \param rgba8888 используемый цвет
 * \см. nema_draw_line()
 * \см. nema_set_blend_fill()
 * \см. nema_rgba()
 *
 */
void
nema_draw_line_aa(float x0, float y0, float x1, float y1, float w,
                  uint32_t rgba8888);

/** \brief Нарисуйте цветной круг размером 1 пиксель.
 *
 * \param x x координата центра круга
 * \param y y координата центра круга
 * \param r радиус круга
 * \param rgba8888 используемый цвет
 * \см. nema_set_blend_fill()
 * \см. nema_rgba()
 *
 */
void nema_draw_circle(int x, int y, int r, uint32_t rgba8888);

/** \brief Нарисуйте цветной круг со сглаживанием (если это возможно) и заданной величиной.
 *
 * \param x x координата центра круга
 * \param y y координата центра круга
 * \param r радиус круга
 * \param w карандаш
 * \param rgba8888 используемый цвет
 * \см. nema_set_blend_fill()
 * \см. nema_rgba()
 *
 */
void nema_draw_circle_aa(float x, float y, float r, float w, uint32_t rgba8888);

/** \brief Нарисуйте цветной контур с закругленными краями.
 *
 * \param x0 x координата верхних левых вершин вершины
 * \param y0 Координата y на верхней левой вершине
 * \param w ширина стороны
 * \param h высота стороны
 * \param r радиус угла
 * \параметр rgba8888
 * \см. nema_set_blend_fill()
 * \см. nema_rgba()
 *
 */
void nema_draw_rounded_rect(int x0, int y0, int w, int h, int r, uint32_t rgba8888);

/** \brief Нарисуйте цветной контур
 *
 * \param x x координата верхних левых вершин показана
 * \param y y координата на верхней левой вершине
 * \param w ширина стороны
 * \param h высота стороны
 * \param rgba8888 используемый цвет
 * \см. nema_set_blend_fill()
 * \см. nema_rgba()
 *
 */
void nema_draw_rect(int x, int y, int w, int h, uint32_t rgba8888);

/** \brief Залейте круг цветом
 *
 * \param x x координата центра круга
 * \param y y координата центра круга
 * \param r радиус круга
 * \param rgba8888 используемый цвет
 * \см. nema_set_blend_fill()
 * \см. nema_rgba()
 *
 */
void nema_fill_circle(int x, int y, int r, uint32_t rgba8888);

/** \brief Залейте круг цвета, воспользуйтесь поглаживанием, если оно доступно.
 *
 * \param x x координата центра круга
 * \param y y координата центра круга
 * \param r радиус круга
 * \param rgba8888 используемый цвет
 * \см. nema_set_blend_fill()
 * \см. nema_rgba()
 *
 */
void nema_fill_circle_aa(float x, float y, float r, uint32_t rgba8888);

/** \brief Залейте треугольник цветом
 *
 * \param x0 Координата x в первой вершине треугольника
 * \param y0 Координата y в первой вершине треугольника
 * \param x1 Координата x на вершине второго треугольника
 * \param y1 Координата y во второй вершине треугольника
 * \param x2 Координата x в вершине треугольника
 * \param y2 Координата y в вершине треугольника
 * \param rgba8888 используемый цвет
 * \см. nema_set_blend_fill()
 *
 */
void nema_fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t rgba8888);

/** \brief Залейте цвет контуром с закругленными краями
 *
 * \param x0 x координата верхних левых вершин вершины
 * \param y0 Координата y на верхней левой вершине
 * \param w ширина стороны
 * \param h высота стороны
 * \param r радиус угла
 * \параметр rgba8888
 * \см. nema_set_blend_fill()
 * \см. nema_rgba()
 *
 */
void nema_fill_rounded_rect(int x0, int y0, int w, int h, int r, uint32_t rgba8888);

/** \brief Залейте буквенный цвет
 *
 * \param x x координата верхних левых вершин показана
 * \param y y координата на верхней левой вершине
 * \param w ширина стороны
 * \param h высота стороны
 * \param rgba8888 используемый цвет
 * \см. nema_set_blend_fill()
 * \см. nema_rgba()
 *
 */
void nema_fill_rect(int x, int y, int w, int h, uint32_t rgba8888);

/** \brief Залейте четырехцветный цвет
 *
 * \param x0 Координата x в первой вершине четырехугольника
 * \param y0 Координата y в первой вершине четырёхугольника
 * \param x1 Координата x на вершине второго четырехугольника
 * \param y1 Координата y на второй вершине четырёхугольника
 * \param x2 Координата x в вершине четырехугольника
 * \param y2 Координата y на вершине четырехугольника
 * \param x3 Координата x в четвертой вершине четырехугольника
 * \param y3 Координата y на четвертой вершине четырёхугольника
 * \param rgba8888 используемый цвет
 * \см. nema_set_blend_fill()
 * \см. nema_rgba()
 *
 */
void nema_fill_quad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t rgba8888);


/** \brief Залейте буквенный цвет (плавающие координаты)
 *
 * \param x x координата верхних левых вершин показана
 * \param y y координата на верхней левой вершине
 * \param w ширина стороны
 * \param h высота стороны
 * \param rgba8888 используемый цвет
 * \см. nema_set_blend_fill()
 * \см. nema_rgba()
 *
 */
void nema_fill_rect_f(float x, float y, float w, float h, uint32_t rgba8888);


/** \brief Заливка четырёхугольника цвета (плавающие координаты)
 *
 * \param x0 Координата x в первой вершине четырехугольника
 * \param y0 Координата y в первой вершине четырёхугольника
 * \param x1 Координата x на вершине второго четырехугольника
 * \param y1 Координата y на второй вершине четырёхугольника
 * \param x2 Координата x в вершине четырехугольника
 * \param y2 Координата y на вершине четырехугольника
 * \param x3 Координата x в четвертой вершине четырехугольника
 * \param y3 Координата y на четвертой вершине четырёхугольника
 * \param rgba8888 используемый цвет
 * \см. nema_set_blend_fill()
 * \см. nema_rgba()
 *
 */
void nema_fill_quad_f(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, uint32_t rgba8888);


/** \brief Залейте треугольник цветом (плавающие координаты)
 *
 * \param x0 Координата x в первой вершине треугольника
 * \param y0 Координата y в первой вершине треугольника
 * \param x1 Координата x на вершине второго треугольника
 * \param y1 Координата y во второй вершине треугольника
 * \param x2 Координата x в вершине треугольника
 * \param y2 Координата y в вершине треугольника
 * \param rgba8888 используемый цвет
 * \см. nema_set_blend_fill()
 *
 */
void nema_fill_triangle_f(float x0, float y0, float x1, float y1, float x2, float y2, uint32_t rgba8888);


// ------------------------------- BLITTING ------------------------------------

/** \brief Перенос исходной текстуры в назначении текстуры
 *
 * \param x координаты места назначения x
 * \param y пункт назначения y координата
 * \см. nema_set_blend_fill()
 *
 */
void nema_blit (int x, int y);


/** \brief Перенос исходной текстуры в текстуре со скругленными углами
 *
 * \param x координаты места назначения x
 * \param y пункт назначения y координата
 * \param r радиус угла отклонения
 * \см. nema_set_blend_fill()
 *
 */
void nema_blit_rounded (int x, int y, int r);

/** \brief Преобразование исходной текстуры в указанный контур (обрезка или перенос при необходимости)
 *
 * \param x координаты места назначения x
 * \param y пункт назначения y координата
 * \param w ширина назначения
 * \param h высота назначения
 * \см. nema_set_blend_blit()
 *
 */
void nema_blit_rect (int x, int y, int w, int h);

/** \brief Переместите часть исходной текстуры в указанный контур (скройте или перенесите при необходимости)
 *
 * \param x координаты места назначения x
 * \param y пункт назначения y координата
 * \param w ширина назначения
 * \param h высота назначения
 * \param x источник x координата
 * \param y источник y координата
 * \см. nema_blit_subrect()
 * \см. nema_set_blend_blit()
 *
 */
void nema_blit_subrect(int dst_x, int dst_y, int w, int h, int src_x, int src_y);

/** \brief Перенесите исходную текстуру в место назначения. Подогнать (масштабировать) текстуру к указанному контуру.
 *
 * \param x координаты места назначения x
 * \param y пункт назначения y координата
 * \param w ширина назначения
 * \param h высота назначения
 * \см. nema_set_blend_blit()
 *
 */
void nema_blit_rect_fit(int x, int y, int w, int h);


/** \brief Перенесите часть исходной текстуры в место назначения. Подогнать (масштабировать) текстуру к указанному контуру.
 *
 * \param x координаты места назначения x
 * \param y пункт назначения y координата
 * \param w ширина назначения
 * \param h высота назначения
 * \param x источник x координата
 * \param y источник y координата
 * \param w исходная ширина
 * \param h высота источника
 * \см. nema_blit_rect_fit()
 * \см. nema_set_blend_blit()
 *
 */
void nema_blit_subrect_fit( int dst_x, int dst_y, int dst_w, int dst_h,
                            int src_x, int src_y, int src_w, int src_h);

/** \brief Вращение вокруг точек поворота и исходной текстуры Blit.
 *
 * \param cx координата центра пункта назначения x
 * \param cy координата центра пункта назначения y
 * \param px исходная точка поворота x координата
 * \param py исходная точка поворота координаты y
 * \paramdegrees_cwградусы включения по часовой стрелке в отдельности [0, 360]
 * \см. nema_set_blend_blit()
 *
 */
void nema_blit_rotate_pivot( float cx, float cy,
                             float px, float py, float degrees_cw );

/** \brief Верните и поместите исходную текстуру в место назначения.
 *
 * \param x координаты места назначения x
 * \param y пункт назначения y координата
 * \param Ротация требует восстановления поворота
 * \см. nema_set_blend_blit()
 *
 */
void nema_blit_rotate(int x, int y, uint32_t rotation);

/** \brief Поворот и блит частичной исходной текстуры в месте назначения.
 *
 * \param sx source верхняя левая координата x
 * \param sy source верхняя левая координата y
 * \param sw исходная ширина частичной области
 * \param sh исходная высота частичной области
 * \param x координаты места назначения x
 * \param y пункт назначения y координата
 * \param Ротация требует восстановления поворота
 * \см. nema_set_blend_blit()
 *
 */
void nema_blit_rotate_partial(int sx, int sy,
                              int sw, int sh,
                              int x,  int y,
                              uint32_t rotation);


/** \brief Перенесите исходную текстуру в место назначения. Подогнать текстуру указанному треугольнику.
 *
 * \param dx0 Координата x первой вершины треугольника
 * \param dy0 координата y первой вершины треугольника
 * \param v0 в [0, 3] обозначает угол текстуры, соответствующую вершине треугольника.
 *                0 _ _ 1
 *                 |_ _|
 *                3     2
 * \param dx1 Координата x на вершине второго треугольника
 * \param dy1 Координата у второй вершины треугольника
 * \param v1 в [0, 3] обозначает угол текстуры, соответствующий второй вершине треугольника.
 * \param dx2 Координата x в вершине треугольника
 * \param dy2 Координата и вершина треугольника
 * \param v2 в [0, 3] обозначает угол текстуры, соответствующие вершине треугольника.
 * \см. nema_set_blend_blit()
 *
 */
void nema_blit_tri_fit (float dx0, float dy0, int v0,
                        float dx1, float dy1, int v1,
                        float dx2, float dy2, int v2);

/** \brief Преобразовать треугольную часть исходной текстуры в треугольную область назначения.
 *
 * \param dx0 Координата x первой в вершине конечного треугольника
 * \param dy0 координата y первой вершины треугольника назначения
 * \param dw0 w координата в первой вершине конечного треугольника
 * \param dx1 Координата x на вершине второго целевого треугольника
 * \param dy1 Координата и во второй вершине треугольника назначения
 * \param dw1 координата w на вершине второго треугольника назначения
 * \param dx2 Координата x в вершине треугольника назначения
 * \param dy2 Координата и вершина треугольника назначения
 * \param dw2 координата w в конце вершины треугольника назначения
 * \param sx0 Координата x первой вершины исходного треугольника
 * \param sy0 Координата и вершина исходного треугольника
 * \param sx1 Координата x во второй вершине исходного треугольника
 * \param sy1 Координата у второй вершины исходного треугольника
 * \param sx2 Координата x в вершине исходного треугольника
 * \param sy2 Координата и вершина исходного треугольника
 * \см. nema_set_blend_blit()
 *
 */
void nema_blit_tri_uv  (float dx0, float dy0, float dw0,
                        float dx1, float dy1, float dw1,
                        float dx2, float dy2, float dw2,
                        float sx0, float sy0,
                        float sx1, float sy1,
                        float sx2, float sy2
                        );

/** \brief Перенесите исходную текстуру в место назначения. Подогнать текстуру указанному к четырехугольнику.
 *
 * \param dx0 Координата x первой в вершине четырехугольника
 * \param dy0 координата y первой вершины четырёхугольника
 * \param dx1 Координата x на вершине второго четырехугольника
 * \param dy1 координата y на вершине второго четырёхугольника
 * \param dx2 Координата x в вершине четырехугольника
 * \param dy2 Координата и вершина четырехугольника
 * \param dx3 Координата x на четвертой вершине четырехугольника
 * \param dy3 Координата и на четвертой вершине четырёхугольника
 * \см. nema_set_blend_blit()
 * \см. nema_blit_subrect_quad_fit()
 *
 */
void nema_blit_quad_fit (float dx0, float dy0,
                         float dx1, float dy1,
                         float dx2, float dy2,
                         float dx3, float dy3);

/** \brief Перенесите исходную текстуру в место назначения. Подогнать прямоугольную область текстуры к указанному четырёхугольнику.
 *
 * \param dx0 Координата x первой в вершине четырехугольника
 * \param dy0 координата y первой вершины четырёхугольника
 * \param dx1 Координата x на вершине второго четырехугольника
 * \param dy1 координата y на вершине второго четырёхугольника
 * \param dx2 Координата x в вершине четырехугольника
 * \param dy2 Координата и вершина четырехугольника
 * \param dx3 Координата x на четвертой вершине четырехугольника
 * \param dy3 Координата и на четвертой вершине четырёхугольника
 * \param sx x координата верхнего левого угла прямоугольной области текстуры, подлежащей копированию
 * \param sy y координата верхнего левого угла прямоугольной области текстуры, подлежащей копированию
 * \param sw ширина прямоугольной области текстуры, подлежащей копированию
 * \param sh высота прямоугольной области текстуры, подлежащей копированию
 * \см. nema_set_blend_blit()
 * \см. nema_blit_quad_fit()
 *
 */
void nema_blit_subrect_quad_fit(float dx0, float dy0,
                                float dx1, float dy1,
                                float dx2, float dy2,
                                float dx3, float dy3,
                                int sx, int sy,
                                int sw, int sh);

/** \brief Перенесите исходную текстуру в место назначения. Используйте матрицу, предоставленную пользователю.
 *
 * \param dx0 Координата x первой в вершине четырехугольника
 * \param dy0 координата y первой вершины четырёхугольника
 * \param dx1 Координата x на вершине второго четырехугольника
 * \param dy1 координата y на вершине второго четырёхугольника
 * \param dx2 Координата x в вершине четырехугольника
 * \param dy2 Координата и вершина четырехугольника
 * \param dx3 Координата x на четвертой вершине четырехугольника
 * \param dy3 Координата и на четвертой вершине четырёхугольника
 * \param m матрица 3x3 (координаты экрана в координатах текстуры)
 * \см. nema_set_blend_blit()
 *
 */
void nema_blit_quad_m(float dx0, float dy0,
                      float dx1, float dy1,
                      float dx2, float dy2,
                      float dx3, float dy3, nema_matrix3x3_t m);




/** \brief Включение точек остановки
 *
 * \см. nema_brk_disable()
 *
 */
void nema_brk_enable(void);

/** \brief Отключить точки остановки
 *
 * \см. nema_brk_enable()
 *
 */
void nema_brk_disable(void);


/** \brief Добавить точку остановки в настоящий список команда
 *
 * \return Точка остановки ID
 *
 */
int  nema_brk_add(void);

/** \brief Добавить точку остановки в настоящий список команда
 *
 * \param brk_id, которую Точка остановкиID, нужно дождаться. Если ноль (0), дождитесь этой точки остановки.
 * \returnIDдостигнутой точки остановки
 *
 */
int  nema_brk_wait(int brk_id);

/** \brief УказываемGPUвозобновить выполнение
 *
 *
 */
void nema_brk_continue(void);

/** \brief Включить внешние сигналы удержания
 *
 * \paramhold_idСигналы удержания должны быть включены [0-3]
 * \см. nema_ext_hold_disable()
 *
 */
void nema_ext_hold_enable(uint32_t hold_id);

/** \brief Отключить внешние сигналы удержания
 *
 * \paramhold_idСигналы удержания должны быть отключены [0-3]
 * \см. nema_ext_hold_enable()
 *
 */
void nema_ext_hold_disable(uint32_t hold_id);

/** \brief Включите блокировку отключения, когдаGPUдостигает точки удержания
 *
 * \paramhold_idУдержание сигналовIRQдолжно быть включено [0-3]
 * \см. nema_ext_hold_disable()
 *
 */
void nema_ext_hold_irq_enable(uint32_t hold_id);

/** \brief Отключить внешние сигналы удержания
 *
 * \paramhold_idУдержание сигналовIRQдля отключения [0-3]
 * \см. nema_ext_hold_enable()
 *
 */
void nema_ext_hold_irq_disable(uint32_t hold_id);


/** \brief Внутреннее подтверждение сигналов удержания через команду списка
 *
 * \paramhold_idДолжен быть установлен сигнал удержания
 * \param stop Если не ноль, лучший процессор таблицы команда ожидания подтверждения подтверждения FLAG
 * \см. nema_ext_hold_deassert()
 *
 */
void nema_ext_hold_assert(uint32_t hold_id, int stop);

/** \brief Десерт удерживает внутренние сигналы через список команд
 *
 * \paramhold_idСигнал удержания должен быть отключен
 * \см. nema_ext_hold_assert()
 *
 */
void nema_ext_hold_deassert(uint32_t hold_id);

/** \brief Подтвердить сигналы удержания отCPU(без списка команд)
 *
 * \paramhold_idДолжен быть установлен сигнал удержания
 * \см. nema_ext_hold_deassert()
 *
 */
void nema_ext_hold_assert_imm(uint32_t hold_id);

/** \brief Сигналы удержания десерта отCPU(без списка команд)
 *
 * \paramhold_idСигнал удержания должен быть отключен
 * \см. nema_ext_hold_assert()
 *
 */
void nema_ext_hold_deassert_imm(uint32_t hold_id);

/** \brief проверка, для какой конструкции скомпилирована библиотека
 *
 * \return Возвращает текст с названием конструкции.
 *
 */
const char* nema_get_sw_device_name(void);


#ifdef __cplusplus
}
#endif

#endif
