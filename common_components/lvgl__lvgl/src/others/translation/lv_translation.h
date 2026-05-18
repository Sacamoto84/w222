/**
 * @file lv_translation.h
 *
 */

#ifndef LV_TRANSLATION_H
#define LV_TRANSLATION_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_TRANSLATION

#include LV_STDINT_INCLUDE
#include "../../misc/lv_array.h"

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
 * Инициализируйте модуль перевода
 */
void lv_translation_init(void);

/**
 * Деинициализируйте модуль перевода и освободите все выделенные переводы.
 */
void lv_translation_deinit(void);

/**
 * Зарегистрируйте пакет перевода из статических массивов.
 * Все указатели должны быть статическими, то есть жить, пока они используются.
 * @param languages     List of languages. E.g. `{"en", "de", NULL}`
 * @param tags          Tags that are using in the UI. E.g. `{"dog", "cat", NULL}`
 * @param translations  List of translations. E.g. `{"Dog", "Cat", "Hund", "Katze"}`
 * @return              The created pack
 */
lv_translation_pack_t * lv_translation_add_static(const char * const languages[], const char * const tags[],
                                                  const char * const translations[]);

/**
 * Добавьте пакет, в который переводы можно добавлять динамически.
 * `pack->languages` должен быть массивом с распределенной памятью, в котором каждый язык также выделен как элемент.
 * `pack->translation_array` хранит перевод, содержащий элементы `lv_translation_tag_dsc_t`.
 * В каждом элементе массива `tag` представляет собой распределенную строку, `translations` — это распределенный массив.
 * с массивом malloc для каждого элемента.
 * @return      the created pack to which data can be added manually.
 */
lv_translation_pack_t * lv_translation_add_dynamic(void);

/**
 * Выберите текущий язык
 * Событие `LV_EVENT_TRANSLATION_LANGUAGE_CHANGED` будет отправлено каждому виджету.
 * @param lang      a string from the defined languages. E.g. "en" or "de"
 */
void lv_translation_set_language(const char * lang);

/**
 * Получить текущий выбранный язык
 * @return the current selected language
 */
const char * lv_translation_get_language(void);

/**
 * Получите переведенную версию тега на выбранном языке
 * @param tag       the tag to translate
 * @return          the translation
 * @note            fallback rules:
 *                      - if the tag is found on the selected language return it
 *                      - if the tag is not found on the selected language, use the fist language
 *                      - if the tag is not found on the first language, return the tag
 */
const char * lv_translation_get(const char * tag);

/**
 * Сокращение lv_translation_set_language
 * @param tag       the tag to translate
 * @return          the translation
 */
static inline const char * lv_tr(const char * tag)
{
    return lv_translation_get(tag);
}

/**
 * Добавьте новый язык в динамический языковой пакет.
 * Все языки следует добавить перед добавлением тегов.
 * @param pack      pointer to a dynamic translation pack
 * @param lang      language to add, e.g. "en", or "de"
 * @return          LV_RESULT_OK: success, LV_RESULT_INVALID: failed
 */
lv_result_t lv_translation_add_language(lv_translation_pack_t * pack, const char * lang);

/**
 * Получите индекс языка в пакете.
 * @param pack      pointer to a static or dynamic language pack
 * @param lang_name name of the language to find
 * @return          index of the language or -1 if not found.
 */
int32_t lv_translation_get_language_index(lv_translation_pack_t * pack, const char * lang_name);

/**
 * Добавьте новый тег в динамический языковой пакет.
 * После добавления тега можно также добавить переводы для каждого языка, используя
 * `lv_translation_set_tag_translation`
 * @param pack      pointer to a dynamic translation pack
 * @param tag_name  name of the tag, e.g. "dog", or "house"
 * @return          pointer to the allocated tag descriptor
 */
lv_translation_tag_dsc_t * lv_translation_add_tag(lv_translation_pack_t * pack, const char * tag_name);

/**
 * Добавьте перевод в тег в пакете динамического перевода
 * @param pack      pointer to a dynamic translation pack
 * @param tag       return value of `lv_translation_add_tag`
 * @param lang_idx  index of the language for which translation should be set
 * @param trans     the translation on the given language
 * @return          LV_RESULT_OK: success, LV_RESULT_INVALID: failed
 */
lv_result_t lv_translation_set_tag_translation(lv_translation_pack_t * pack, lv_translation_tag_dsc_t * tag,
                                               uint32_t lang_idx, const char * trans);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_TRANSLATION*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /* LV_TRANSLATION_H */
