/**
 * @file lv_draw_buf.h
 *
 */

#ifndef LV_DRAW_BUF_H
#define LV_DRAW_BUF_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_types.h"
#include "../misc/lv_area.h"
#include "../misc/lv_color.h"
#include "../stdlib/lv_string.h"
#include "lv_image_dsc.h"

/*********************
 *      DEFINES
 *********************/

/** Используйте это значение, чтобы LVGL автоматически рассчитывал шаг. */
#define LV_STRIDE_AUTO 0
LV_EXPORT_CONST_INT(LV_STRIDE_AUTO);

/**
 * Выравнивание шага для буферов прорисовки.
 * Это может варьироваться в зависимости от различных цветовых форматов и оборудования.
 * Уточните его в соответствии с вашими потребностями.
 */

#define LV_DRAW_BUF_STRIDE(w, cf) \
    LV_ROUND_UP(((w) * LV_COLOR_FORMAT_GET_BPP(cf) + 7) / 8, LV_DRAW_BUF_STRIDE_ALIGN)

/** Выделите буфер немного большего размера, чтобы мы могли настроить начальный адрес в соответствии с выравниванием. */
#define LV_DRAW_BUF_SIZE(w, h, cf) \
    (LV_DRAW_BUF_STRIDE(w, cf) * (h) + LV_DRAW_BUF_ALIGN + \
     LV_COLOR_INDEXED_PALETTE_SIZE(cf) * sizeof(lv_color32_t))

/**
 * Определите статический буфер рисования с заданной шириной, высотой и цветовым форматом.
 * Для выравнивания шага установлено значение LV_DRAW_BUF_STRIDE_ALIGN.
 *
 * Для платформы, которой требуется специальное выравнивание буфера, вызовите LV_DRAW_BUF_INIT_STATIC.
 */
#define LV_DRAW_BUF_DEFINE_STATIC(name, _w, _h, _cf) \
    static LV_ATTRIBUTE_MEM_ALIGN uint8_t buf_##name[LV_DRAW_BUF_SIZE(_w, _h, _cf)]; \
    static lv_draw_buf_t name = { \
                                  .header = { \
                                              .magic = LV_IMAGE_HEADER_MAGIC, \
                                              .cf = (_cf), \
                                              .flags = LV_IMAGE_FLAGS_MODIFIABLE, \
                                              .w = (_w), \
                                              .h = (_h), \
                                              .stride = LV_DRAW_BUF_STRIDE(_w, _cf), \
                                              .reserved_2 = 0, \
                                            }, \
                                  .data_size = sizeof(buf_##name), \
                                  .data = buf_##name, \
                                  .unaligned_data = buf_##name, \
                                }

#define LV_DRAW_BUF_INIT_STATIC(name) \
    do { \
        lv_image_header_t * header = &name.header; \
        lv_draw_buf_init(&name, header->w, header->h, (lv_color_format_t)header->cf, header->stride, buf_##name, sizeof(buf_##name)); \
        lv_draw_buf_set_flag(&name, LV_IMAGE_FLAGS_MODIFIABLE); \
    } while(0)

/**********************
 *      TYPEDEFS
 **********************/

typedef void * (*lv_draw_buf_malloc_cb_t)(size_t size, lv_color_format_t color_format);

typedef void (*lv_draw_buf_free_cb_t)(void * draw_buf);

typedef void (*lv_draw_buf_copy_cb_t)(lv_draw_buf_t * dest, const lv_area_t * dest_area,
                                      const lv_draw_buf_t * src, const lv_area_t * src_area);

typedef void * (*lv_draw_buf_align_cb_t)(void * buf, lv_color_format_t color_format);

typedef void (*lv_draw_buf_cache_operation_cb_t)(const lv_draw_buf_t * draw_buf, const lv_area_t * area);

typedef uint32_t (*lv_draw_buf_width_to_stride_cb_t)(uint32_t w, lv_color_format_t color_format);

struct _lv_draw_buf_t {
    lv_image_header_t header;
    uint32_t data_size;       /**< Total buf size in bytes */
    uint8_t * data;
    void * unaligned_data;    /**< Unaligned address of `data`, used internally by lvgl */
    const lv_draw_buf_handlers_t * handlers; /**< draw buffer alloc/free ops. */
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте буфер отрисовки с помощью обработчиков по умолчанию.
 *
 * @param handlers  обработчики буфера прорисовки, которые нужно установить
 */
void lv_draw_buf_init_with_default_handlers(lv_draw_buf_handlers_t * handlers);

/**
 * Инициализируйте буфер отрисовки с заданными обработчиками.
 *
 * @param handlers             обработчики буфера прорисовки, которые нужно установить
 * @param buf_malloc_cb        обратный вызов для выделения памяти для буфера
 * @param buf_free_cb          обратный вызов для освобождения памяти буфера
 * @param buf_copy_cb          обратный вызов для копирования буфера рисования в другой
 * @param align_pointer_cb     обратный вызов для выравнивания буфера
 * @param invalidate_cache_cb  обратный вызов для аннулирования кеша буфера
 * @param flush_cache_cb       обратный вызов для очистки буфера
 * @param width_to_stride_cb   обратный вызов для расчета шага на основе ширины и формата цвета
 */
void lv_draw_buf_handlers_init(lv_draw_buf_handlers_t * handlers,
                               lv_draw_buf_malloc_cb_t buf_malloc_cb,
                               lv_draw_buf_free_cb_t buf_free_cb,
                               lv_draw_buf_copy_cb_t buf_copy_cb,
                               lv_draw_buf_align_cb_t align_pointer_cb,
                               lv_draw_buf_cache_operation_cb_t invalidate_cache_cb,
                               lv_draw_buf_cache_operation_cb_t flush_cache_cb,
                               lv_draw_buf_width_to_stride_cb_t width_to_stride_cb);

/**
 * Получите структуру, содержащую обратные вызовы для управления буфером отрисовки.
 * Пользовательский обратный вызов может быть установлен для возвращаемого значения.
 * @return                  указатель на структуру обработчиков
 */
lv_draw_buf_handlers_t * lv_draw_buf_get_handlers(void);
lv_draw_buf_handlers_t * lv_draw_buf_get_font_handlers(void);
lv_draw_buf_handlers_t * lv_draw_buf_get_image_handlers(void);


/**
 * Выровняйте адрес буфера. Буфер должен быть достаточно большим для реальных данных после выравнивания.
 * @param buf           данные для согласования
 * @param color_format  цветовой формат буфера
 * @return              выровненный буфер
 */
void * lv_draw_buf_align(void * buf, lv_color_format_t color_format);

/**
 * Согласуйте адрес буфера с пользовательскими обработчиками буфера отрисовки.
 * Буфер должен быть достаточно большим для реальных данных после выравнивания.
 * @param handlers      обработчики буфера отрисовки
 * @param buf           данные для согласования
 * @param color_format  цветовой формат буфера
 * @return              выровненный буфер
 */
void * lv_draw_buf_align_ex(const lv_draw_buf_handlers_t * handlers, void * buf, lv_color_format_t color_format);

/**
 * Инвалидировать кеш буфера
 * @param draw_buf     буфер отрисовки должен быть признан недействительным
 * @param area         область, которую нужно сделать недействительной в буфере,
 *                     используйте NULL, чтобы сделать недействительным весь диапазон адресов буфера прорисовки
 */
void lv_draw_buf_invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area);

/**
 * Очистить кеш буфера
 * @param draw_buf     буфер отрисовки необходимо очистить
 * @param area         область для очистки в буфере,
 *                     используйте NULL, чтобы очистить весь диапазон адресов буфера прорисовки
 */
void lv_draw_buf_flush_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area);

/**
 * Рассчитайте шаг в байтах на основе ширины и формата цвета.
 * @param w                 ширина в пикселях
 * @param color_format      цветовой формат
 * @return                  шаг в байтах
 */
uint32_t lv_draw_buf_width_to_stride(uint32_t w, lv_color_format_t color_format);

/**
 * Рассчитайте шаг в байтах на основе ширины и формата цвета.
 * @param handlers          обработчики буфера отрисовки
 * @param w                 ширина в пикселях
 * @param color_format      цветовой формат
 * @return                  шаг в байтах
 */
uint32_t lv_draw_buf_width_to_stride_ex(const lv_draw_buf_handlers_t * handlers, uint32_t w,
                                        lv_color_format_t color_format);

/**
 * Очистить область в буфере
 * @param draw_buf          указатель для рисования буфера
 * @param a                 область для очистки илиNULLдля очистки всего буфера
 */
void lv_draw_buf_clear(lv_draw_buf_t * draw_buf, const lv_area_t * a);


/**
 * Note: Со временемlv_draw_buf_malloc/free останется конфиденциальным.
 *       На данный момент мы используем `create`, чтобы различать malloc.
 *
 * Сделайте отрисовку буфера, выделите узор для`lv_draw_buf_t`и выделите для нее буфер.
 * который соответствует заданным требованиям.
 *
 * @param w         ширина буфера в пикселях
 * @param h         высота буфера в пикселях
 * @param cf        цветовой формат изображения
 * @param stride    шаг в байтах для изображения. Используйте 0 для автоматического расчета на основе
 *                  w, cf и глобальная конфигурация выравнивания шага.
 */
lv_draw_buf_t * lv_draw_buf_create(uint32_t w, uint32_t h, lv_color_format_t cf, uint32_t stride);

/**
 * Note: Со временемlv_draw_buf_malloc/free останется конфиденциальным.
 *       На данный момент мы используем `create`, чтобы различать malloc.
 *
 * Сделайте отрисовку буфера, выделите узор для`lv_draw_buf_t`и выделите для нее буфер.
 * который соответствует заданным требованиям.
 *
 * @param handlers  обработчики буфера отрисовки
 * @param w         ширина буфера в пикселях
 * @param h         высота буфера в пикселях
 * @param cf        цветовой формат изображения
 * @param stride    шаг в байтах для изображения. Используйте 0 для автоматического расчета на основе
 *                  w, cf и глобальная конфигурация выравнивания шага.
 */
lv_draw_buf_t * lv_draw_buf_create_ex(const lv_draw_buf_handlers_t * handlers, uint32_t w, uint32_t h,
                                      lv_color_format_t cf, uint32_t stride);

/**
 * Дублируйте рисунок с тем же размером изображения, шагом и цветовым форматом. Скопируйте также данные изображения.
 * @param draw_buf  буф для дублирования
 * @return          дублированный буф отрисовки в случае успеха,NULLв случае неудачи
 */
lv_draw_buf_t * lv_draw_buf_dup(const lv_draw_buf_t * draw_buf);

/**
 * Дублируйте рисунок с тем же размером изображения, шагом и цветовым форматом. Скопируйте также данные изображения.
 * @param handlers  обработчики буфера отрисовки
 * @param draw_buf  буф для дублирования
 * @return          дублированный буф отрисовки в случае успеха,NULLв случае неудачи
 */
lv_draw_buf_t * lv_draw_buf_dup_ex(const lv_draw_buf_handlers_t * handlers, const lv_draw_buf_t * draw_buf);

/**
 * Инициализируйте буфер отрисовки с заданным буфером и параметрами. Очистить флаг буфера прорисовки до нуля.
 * @param draw_buf  буф рисования для инициализации
 * @param w         ширина буфера в пикселях
 * @param h         высота буфера в пикселях
 * @param cf        цветовой формат
 * @param stride    шаг в байтах. Используйте 0 для автоматического расчета.
 * @param data      буфер, используемый для рисования. Невыровненный`data`будет выровнен внутри.
 * @param data_size размер буфера в байтах
 * @return          вернутьLV_RESULT_OKв случае успеха,LV_RESULT_INVALIDв противном случае
 */
lv_result_t lv_draw_buf_init(lv_draw_buf_t * draw_buf, uint32_t w, uint32_t h, lv_color_format_t cf, uint32_t stride,
                             void * data, uint32_t data_size);

/**
 * Продолжайте использовать существующую память, измените форму буфера прорисовки до заданной ширины и высоты.
 * ВозвращайтеNULL, еслиdata_sizeменьше требуемого размера.
 * @param draw_buf  указатель на буфер рисования
 * @param cf        новый формат цвета, используйте 0 или LV_COLOR_FORMAT_UNKNOWN, чтобы продолжать использовать исходный формат цвета.
 * @param w         новая ширина в пикселях
 * @param h         новая высота в пикселях
 * @param stride    шаг в байтах для изображения. Используйте 0 для автоматического расчета.
 */
lv_draw_buf_t * lv_draw_buf_reshape(lv_draw_buf_t * draw_buf, lv_color_format_t cf, uint32_t w, uint32_t h,
                                    uint32_t stride);

/**
 * Уничтожьте буфер отрисовки, освободив фактический буфер, если он помечен в заголовке как LV_IMAGE_FLAGS_ALLOCATED.
 * Затем освободите структуру lv_draw_buf_t.
 *
 * @param draw_buf  буфер отрисовки для уничтожения
 */
void lv_draw_buf_destroy(lv_draw_buf_t * draw_buf);

/**
 * Копирование области из буфера в другой
 * @param dest      указатель на целевой буфер отрисовки
 * @param dest_area область для копирования из целевого буфера. Если NULL, используйте весь буфер.
 * @param src       указатель на исходный буфер отрисовки
 * @param src_area  область для копирования из целевого буфера. Если NULL, используйте весь буфер.
 * @note `dest_area` и`src_area`должны иметь одинаковую ширину и высоту.
 * @note  Функция копирования по умолчанию требовала, чтобы`dest`и`src`имели одинаковый цветовой формат.
 * Перезапись dest->handlers->buf_copy_cbможет устранить это ограничение.
 */
void lv_draw_buf_copy(lv_draw_buf_t * dest, const lv_area_t * dest_area,
                      const lv_draw_buf_t * src, const lv_area_t * src_area);

/**
 * Вернуть указатель на буфер по заданным координатам
 */
void * lv_draw_buf_goto_xy(const lv_draw_buf_t * buf, uint32_t x, uint32_t y);

/**
 * Отрегулируйте шаг буфа на месте.
 * @param src       указатель на буфер рисования
 * @param stride    новый шаг в байтах для изображения. ИспользуйтеLV_STRIDE_AUTOдля автоматического расчета.
 * @return          LV_RESULT_OK: успех или LV_RESULT_INVALID: неудача.
 */
lv_result_t lv_draw_buf_adjust_stride(lv_draw_buf_t * src, uint32_t stride);

/**
 * Предварительно умножить цвет буфера рисования с альфа-каналом.
 * Если оно уже предварительно умножено, вернитесь напрямую.
 * Будут обрабатываться только цветовые форматы с альфа-каналом.
 *
 * @return LV_RESULT_OK : успех предварительного умножения
 */
lv_result_t lv_draw_buf_premultiply(lv_draw_buf_t * draw_buf);


/**
 * Проверьте, имеет ли буфер отрисовки заданный флаг.
 * @param draw_buf  указатель на буфер рисования
 * @param flag      флаг для проверки
 * @return true: флаг установлен, false: флаг не установлен
 */
static inline bool lv_draw_buf_has_flag(const lv_draw_buf_t * draw_buf, lv_image_flags_t flag)
{
    return draw_buf->header.flags & flag;
}

/**
 * Установите флаг для буфера рисования.
 * @param draw_buf  указатель на буфер рисования
 * @param flag      флаг, который нужно установить
 */
static inline void lv_draw_buf_set_flag(lv_draw_buf_t * draw_buf, lv_image_flags_t flag)
{
    draw_buf->header.flags |= flag;
}

/**
 * Очистить флаг из буфера рисования.
 * @param draw_buf  указатель на буфер рисования
 * @param flag      флаг, который нужно очистить
 */
static inline void lv_draw_buf_clear_flag(lv_draw_buf_t * draw_buf, lv_image_flags_t flag)
{
    draw_buf->header.flags &= ~flag;
}

/**
 * На данный момент draw buf имеет то же определение, что и`lv_image_dsc_t`.
 * И взаимозаменяем с`lv_image_dsc_t`.
 */

lv_result_t lv_draw_buf_from_image(lv_draw_buf_t * buf, const lv_image_dsc_t * img);

void lv_draw_buf_to_image(const lv_draw_buf_t * buf, lv_image_dsc_t * img);

/**
 * Установите цвет палитры индексированного изображения. Действительно только для `LV_COLOR_FORMAT_I1/2/4/8`
 * @param draw_buf указатель на дескриптор изображения
 * @param index цвет палитры для установки:
 *   - для `LV_COLOR_FORMAT_I1`: 0..1
 *   - для `LV_COLOR_FORMAT_I2`: 0..3
 *   - для `LV_COLOR_FORMAT_I4`: 0..15
 *   - для `LV_COLOR_FORMAT_I8`: 0..255
 * @param color цвет для установки в формате lv_color32_t
 */
void lv_draw_buf_set_palette(lv_draw_buf_t * draw_buf, uint8_t index, lv_color32_t color);

/**
 * @deprecated Вместо этого используйте lv_draw_buf_set_palette.
 */
void lv_image_buf_set_palette(lv_image_dsc_t * dsc, uint8_t id, lv_color32_t c);

/**
 * @deprecated Вместо этого используйтеlv_draw_buffer_create/destroy.
 * Освободите указатель данных и создайте структуру изображения dsc.
 */
void lv_image_buf_free(lv_image_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_BUF_H*/
