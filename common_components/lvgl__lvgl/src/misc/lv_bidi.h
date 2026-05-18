/**
 * @file lv_bidi.h
 *
 */

#ifndef LV_BIDI_H
#define LV_BIDI_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "lv_types.h"
#include "lv_text.h"

/*********************
 *      DEFINES
 *********************/
/** Специальные непечатаемые сильные символы.
 * Их можно вставлять в тексты, чтобы влиять на направление бега. */
#define LV_BIDI_LRO  "\xE2\x80\xAD" /*U+202D*/
#define LV_BIDI_RLO  "\xE2\x80\xAE" /*U+202E*/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_BASE_DIR_LTR      = 0x00,
    LV_BASE_DIR_RTL      = 0x01,
    LV_BASE_DIR_AUTO     = 0x02,

    LV_BASE_DIR_NEUTRAL  = 0x20,
    LV_BASE_DIR_WEAK     = 0x21,
} lv_base_dir_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
#if LV_USE_BIDI

/**
 * Получите реальное выравнивание текста из выравнивания текста, базового направления и текста.
 * @param align     LV_TEXT_ALIGN_ ..., запишите сюда вычисленное выравнивание (LV_TEXT_ALIGN_LEFT/RIGHT/CENTER)
 * @param base_dir  LV_BASE_DIR_ ..., сюда запишите рассчитанную базовую директорию (LV_BASE_DIR_LTR/RTL)
 * @param txt       текст, используемый сLV_BASE_DIR_AUTOдля определения базового направления
 */
void lv_bidi_calculate_align(lv_text_align_t * align, lv_base_dir_t * base_dir, const char * txt);

/**
 * Установить пользовательскую строку нейтральных значений
 * @param neutrals  default " \t\n\r.,:;'\"`!?%/\\-=()[]{}<>@#&$|"
 */
void lv_bidi_set_custom_neutrals_static(const char * neutrals);

/**********************
 *      MACROS
 **********************/

#else /*LV_USE_BIDI*/
/**
 * Для совместимости, если LV_USE_BIDI = 0
 * Получите реальное выравнивание текста из выравнивания текста, базового направления и текста.
 * @param align     ДляLV_TEXT_ALIGN_AUTOукажите LV_TEXT_ALIGN_LEFT, иначе оставьте без изменений, запишите сюда вычисленное выравнивание.
 * @param base_dir  Неиспользованный
 * @param txt       Неиспользованный
 */
static inline void lv_bidi_calculate_align(lv_text_align_t * align, lv_base_dir_t * base_dir, const char * txt)
{
    LV_UNUSED(txt);
    LV_UNUSED(base_dir);
    if(*align == LV_TEXT_ALIGN_AUTO) * align = LV_TEXT_ALIGN_LEFT;
}
#endif /*LV_USE_BIDI*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BIDI_H*/
