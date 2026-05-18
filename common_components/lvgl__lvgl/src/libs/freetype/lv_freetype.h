/**
 * @file lv_freetype.h
 *
 */
#ifndef LV_FREETYPE_H
#define LV_FREETYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_FREETYPE

#include "../../misc/lv_types.h"
#include "../../misc/lv_event.h"
#include "../../misc/lv_color.h"

#include LV_STDBOOL_INCLUDE

/*********************
*      DEFINES
*********************/

#define LV_FREETYPE_F26DOT6_TO_INT(x)   ((x) >> 6)
#define LV_FREETYPE_F26DOT6_TO_FLOAT(x) ((float)(x) / 64)

#define FT_FONT_STYLE_NORMAL LV_FREETYPE_FONT_STYLE_NORMAL
#define FT_FONT_STYLE_ITALIC LV_FREETYPE_FONT_STYLE_ITALIC
#define FT_FONT_STYLE_BOLD   LV_FREETYPE_FONT_STYLE_BOLD

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_FREETYPE_FONT_STYLE_NORMAL = 0,
    LV_FREETYPE_FONT_STYLE_ITALIC = 1 << 0,
    LV_FREETYPE_FONT_STYLE_BOLD = 1 << 1,
} lv_freetype_font_style_t;

typedef lv_freetype_font_style_t LV_FT_FONT_STYLE;

typedef enum {
    LV_FREETYPE_FONT_RENDER_MODE_BITMAP = 0,
    LV_FREETYPE_FONT_RENDER_MODE_OUTLINE = 1,
} lv_freetype_font_render_mode_t;

typedef void * lv_freetype_outline_t;

typedef enum {
    LV_FREETYPE_OUTLINE_END,
    LV_FREETYPE_OUTLINE_MOVE_TO,
    LV_FREETYPE_OUTLINE_LINE_TO,
    LV_FREETYPE_OUTLINE_CUBIC_TO,
    LV_FREETYPE_OUTLINE_CONIC_TO,
    LV_FREETYPE_OUTLINE_BORDER_START,     /* Если ширина линии > 0, глиф границы рисуется после обычного глифа. */
} lv_freetype_outline_type_t;

/* Требуется только строка пути */
typedef const char lv_freetype_font_src_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_font_class_t lv_freetype_font_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте библиотеку свободных типов.
 * @return LV_RESULT_OK в случае успеха, в противном случаеLV_RESULT_INVALID.
 */
lv_result_t lv_freetype_init(uint32_t max_glyph_cnt);

/**
 * Деинициализировать библиотеку свободных типов
 */
void lv_freetype_uninit(void);

/**
 * Инициализируйте информационную структуру шрифта.
 * @param font_info Структура информации о шрифте, которую необходимо инициализировать.
 */
void lv_freetype_init_font_info(lv_font_info_t * font_info);

/**
 * Создайте шрифт произвольного типа со структурой информации о шрифте.
 * @param font_info информационная структура шрифта.
 * @return Созданный шрифт илиNULLв случае сбоя.
 */
lv_font_t * lv_freetype_font_create_with_info(const lv_font_info_t * font_info);

/**
 * Создайте произвольный шрифт.
 * @param pathname путь к файлу шрифта.
 * @param render_mode режим рендеринга шрифтов (подробности см. в @lv_freetype_font_render_mode_t).
 * @param size размер шрифта.
 * @param style стиль шрифта (подробности см. в lv_freetype_font_style_t).
 * @return Созданный шрифт илиNULLв случае сбоя.
 */
lv_font_t * lv_freetype_font_create(const char * pathname, lv_freetype_font_render_mode_t render_mode, uint32_t size,
                                    lv_freetype_font_style_t style);

/**
 * Удалить произвольный шрифт.
 * @param font произвольный шрифт, который нужно удалить.
 */
void lv_freetype_font_delete(lv_font_t * font);

/**
 * Зарегистрируйте функцию обратного вызова для создания контуров для шрифтов FreeType.
 *
 * @param cb Функция обратного вызова, которую необходимо зарегистрировать.
 * @param user_data Пользовательские данные, которые будут переданы в функцию обратного вызова.
 * @return ID зарегистрированной функции обратного вызова или отрицательное значение в случае сбоя.
 */
void lv_freetype_outline_add_event(lv_event_cb_t event_cb, lv_event_code_t filter, void * user_data);

/**
 * Получите масштабный шрифт FreeType.
 *
 * @param font Шрифт FreeType, масштаб которого требуется получить.
 * @return Масштаб шрифта FreeType.
 */
uint32_t lv_freetype_outline_get_scale(const lv_font_t * font);

/**
 * Проверьте, является ли шрифт контурным.
 *
 * @param font Шрифт FreeType.
 * @return Контурный шрифт в случае успеха, в противном случае — значение false.
 */
bool lv_freetype_is_outline_font(const lv_font_t * font);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_FREETYPE*/

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /* LV_FREETYPE_H */
