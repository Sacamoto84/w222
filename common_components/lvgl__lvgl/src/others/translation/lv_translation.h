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
 * @param languages     Список языков. Например.  `{"en", "de", NULL}`
 * @param tags          Теги, которые используются в файле UI. Например.  `{"dog", "cat", NULL}`
 * @param translations  Список переводов. Например.  `{"Dog", "Cat", "Hund", "Katze"}`
 * @return              Созданный пакет
 */
lv_translation_pack_t * lv_translation_add_static(const char * const languages[], const char * const tags[],
                                                  const char * const translations[]);

/**
 * Добавьте пакет, в который переводы можно добавлять динамически.
 * `pack->languages` должен быть массивом с распределенной памятью, в котором каждый язык также выделяется как элемент.
 * `pack->translation_array` хранит преобразование элементов`lv_translation_tag_dsc_t`.
 * В каждом элементе массива`tag`представляет собой распределенный массив,`translations`— это распределенный массив.
 * с массивом malloc для каждого элемента.
 * @return      созданный пакет, в который данные можно добавлять вручную.
 */
lv_translation_pack_t * lv_translation_add_dynamic(void);

/**
 * Выберите текущий язык
 * Событие `LV_EVENT_TRANSLATION_LANGUAGE_CHANGED` будет отправлено каждому виджету.
 * @param lang      строка из определенных языков. Например. «эн» или «де»
 */
void lv_translation_set_language(const char * lang);

/**
 * Получить текущий выбранный язык
 * @return текущий выбранный язык
 */
const char * lv_translation_get_language(void);

/**
 * Получите переведенную версию тега на выбранном языке
 * @param tag       тег для перевода
 * @return          перевод
 * @note            запасные правила:
 *                      - если тег найден на выбранном языке, верните его
 *                      - если тег не найден на выбранном языке, используйте первый язык
 *                      - если тег не найден на первом языке, верните тег
 */
const char * lv_translation_get(const char * tag);

/**
 * Сокращение lv_translation_set_language
 * @param tag       тег для перевода
 * @return          перевод
 */
static inline const char * lv_tr(const char * tag)
{
    return lv_translation_get(tag);
}

/**
 * Добавьте новый язык в динамический языковой пакет.
 * Все языки следует добавить перед добавлением тегов.
 * @param pack      указатель на пакет динамического перевода
 * @param lang      язык для добавления, например «эн» или «де»
 * @return          LV_RESULT_OK: успех, LV_RESULT_INVALID: неудача.
 */
lv_result_t lv_translation_add_language(lv_translation_pack_t * pack, const char * lang);

/**
 * Получите индекс языка в пакете.
 * @param pack      указатель на статический или динамический языковой пакет
 * @param lang_name название языка, который нужно найти
 * @return          индекс языка или -1, если не найден.
 */
int32_t lv_translation_get_language_index(lv_translation_pack_t * pack, const char * lang_name);

/**
 * Добавьте новый тег в динамический языковой пакет.
 * После добавления тега можно также добавить переводы для каждого языка, используя
 * `lv_translation_set_tag_translation`
 * @param pack      указатель на пакет динамического перевода
 * @param tag_name  имя тега, например. «собака» или «дом»
 * @return          указатель на выделенный дескриптор тега
 */
lv_translation_tag_dsc_t * lv_translation_add_tag(lv_translation_pack_t * pack, const char * tag_name);

/**
 * Добавьте перевод в тег в пакете динамического перевода
 * @param pack      указатель на пакет динамического перевода
 * @param tag       возвращаемое значение `lv_translation_add_tag`
 * @param lang_idx  индекс языка, для которого должен быть установлен перевод
 * @param trans     перевод на указанный язык
 * @return          LV_RESULT_OK: успех, LV_RESULT_INVALID: неудача.
 */
lv_result_t lv_translation_set_tag_translation(lv_translation_pack_t * pack, lv_translation_tag_dsc_t * tag,
                                               uint32_t lang_idx, const char * trans);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_TRANSLATION*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_TRANSLATION_H */
