/**
 * @file lv_font_manager.h
 *
 */
#ifndef LV_FONT_MANAGER_H
#define LV_FONT_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../font/lv_font.h"

#if LV_USE_FONT_MANAGER

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте главный менеджер шрифтов.
 * @param recycle_cache_size количество шрифтов, которые были недавно удалены из кэша.
 * @return указатель на главный менеджер шрифтов.
 */
lv_font_manager_t * lv_font_manager_create(uint32_t recycle_cache_size);

/**
 * Удалить основной менеджер шрифтов.
 * @param manager указатель на главный менеджер шрифтов.
 * @return верните true, если удаление прошло успешно.
 */
bool lv_font_manager_delete(lv_font_manager_t * manager);

/**
 * Добавьте ресурс шрифта.
 * @param manager указатель на главный менеджер шрифтов.
 * @param name название шрифта.
 * @param src источник шрифта. Необходимо строго соответствовать классу шрифта.
 * @param class_p класс шрифта. например. lv_freetype_font_class,lv_builtin_font_class.
 * @return верните true, если добавление прошло успешно.
 */
bool lv_font_manager_add_src(lv_font_manager_t * manager,
                             const char * name,
                             const void * src,
                             const lv_font_class_t * class_p);

/**
 * Добавьте ресурс шрифта со статической памятью.
 * @param manager указатель на главный менеджер шрифтов.
 * @param name название шрифта. Это не может быть локальная переменная.
 * @param src источник шрифта. Необходимо строго соответствовать классу шрифта. И это не может быть локальная переменная.
 * @param class_p класс шрифта. Например. lv_freetype_font_class,lv_builtin_font_class.
 * @return верните true, если добавление прошло успешно.
 */
bool lv_font_manager_add_src_static(lv_font_manager_t * manager,
                                    const char * name,
                                    const void * src,
                                    const lv_font_class_t * class_p);

/**
 * Удалить ресурс шрифта.
 * @param manager указатель на главный менеджер шрифтов.
 * @param name название шрифта.
 * @return верните true, если удаление прошло успешно.
 */
bool lv_font_manager_remove_src(lv_font_manager_t * manager, const char * name);

/**
 * Создать шрифт.
 * @param manager указатель на главный менеджер шрифтов.
 * @param font_family название семейства шрифтов. Соответствует имени ресурса шрифта, используя запятые для разделения разных имен. Например. «my_font_1, my_font_2».
 * @param render_mode режим рендеринга шрифта. см. `lv_freetype_font_render_mode_t`.
 * @param size размер шрифта в пикселях.
 * @param style стиль шрифта. см. `lv_freetype_font_style_t`.
 * @param kerning режим кернинга. см. `lv_font_kerning_t`.
 * @return укажите на созданный шрифт.
 */
lv_font_t * lv_font_manager_create_font(lv_font_manager_t * manager,
                                        const char * font_family,
                                        uint32_t render_mode,
                                        uint32_t size,
                                        uint32_t style,
                                        lv_font_kerning_t kerning);

/**
 * Удалить шрифт.
 * @param manager указатель на главный менеджер шрифтов.
 * @param font укажите на шрифт.
 * @return верните true, если удаление прошло успешно.
 */
void lv_font_manager_delete_font(lv_font_manager_t * manager, lv_font_t * font);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_FONT_MANAGER */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_FONT_MANAGER_H */
