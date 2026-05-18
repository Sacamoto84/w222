/**
 *@file lv_theme.h
 *
 */

#ifndef LV_THEME_H
#define LV_THEME_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *    INCLUDES
 *********************/
#include "../core/lv_obj.h"

/*********************
 *    DEFINES
 *********************/

/**********************
 *    TYPEDEFS
 **********************/

typedef void (*lv_theme_apply_cb_t)(lv_theme_t *, lv_obj_t *);

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**
 * Создает новую тему
 * @return  новая тема или NULL, если выделение не удалось
 */
lv_theme_t * lv_theme_create(void);

/**
 * Скопируйте тему «src» в «dst».
 * @param dst   указатель на целевую тему
 * @param src   указатель на исходную тему
 */
void lv_theme_copy(lv_theme_t * dst, const lv_theme_t * src);

/**
 * Получить тему, назначенную для отображения объекта
 * @param obj       указатель на объект темы
 * @return          тема отображения объекта (может бытьNULL)
 */
lv_theme_t  * lv_theme_get_from_obj(lv_obj_t * obj);

/**
 * Применить активную тему к объекту
 * @param obj указатель на объект
 */
void lv_theme_apply(lv_obj_t * obj);

/**
 * Установите базовую тему для темы.
 * Стили из их базы будут добавлены перед стилями текущей темы.
 * Произвольную длинную цепочку тем можно создать, установив базовые темы.
 * @param theme указатель на тему, базу которой следует установить
 * @param parent указатель на базовую тему
 */
void lv_theme_set_parent(lv_theme_t * theme, lv_theme_t * parent);

/**
 * Установите обратный вызов для темы.
 * Обратный вызов Примените методы для добавления стилей к различным объектам.
 * @param theme указатель на тему, для которой должен быть установлен обратный вызов
 * @param apply_cb указатель на обратный вызов
 */
void lv_theme_set_apply_cb(lv_theme_t * theme, lv_theme_apply_cb_t apply_cb);

/**
 * Получите мелкий шрифт темы
 * @param obj указатель на объект
 * @return указатель на шрифт
 */
const lv_font_t * lv_theme_get_font_small(lv_obj_t * obj);
/**
 * Получить нормальный шрифт темы
 * @param obj указатель на объект
 * @return указатель на шрифт
 */
const lv_font_t * lv_theme_get_font_normal(lv_obj_t * obj);

/**
 * Получить шрифт субтитров темы
 * @param obj указатель на объект
 * @return указатель на шрифт
 */
const lv_font_t * lv_theme_get_font_large(lv_obj_t * obj);

/**
 * Получить основной цвет темы
 * @param obj указатель на объект
 * @return цвет
 */
lv_color_t lv_theme_get_color_primary(lv_obj_t * obj);

/**
 * Получить дополнительный цвет темы
 * @param obj указатель на объект
 * @return цвет
 */
lv_color_t lv_theme_get_color_secondary(lv_obj_t * obj);


/**
 * Удалить тему
 * @param   theme тема, которую нужно уничтожить
 */
void lv_theme_delete(lv_theme_t * theme);

#if LV_USE_EXT_DATA
/**
 * @brief Прикрепляет к теме внешние пользовательские данные и обратный вызов деструктора.
 *
 * Связывает пользовательские данные с темой LVGL и определяет функцию деструктора.
 * который будет автоматически вызываться при удалении темы для правильной очистки.
 * связанные ресурсы.
 *
 * @param theme      Указатель на тему, для которой следует установить обратный вызов
 * @param data       Пользовательский указатель данных для связи с темой
 * @param free_cb    Функция обратного вызова для очисткиext_dataпри удалении темы.
 *                   Получаетext_dataв качестве параметра.  NULLозначает, что очистка не требуется.
 */
void lv_theme_set_external_data(lv_theme_t * theme, void * data, void (* free_cb)(void * data));
#endif

/**********************
 *    MACROS
 **********************/

#include "default/lv_theme_default.h"
#include "mono/lv_theme_mono.h"
#include "simple/lv_theme_simple.h"

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_THEME_H*/
