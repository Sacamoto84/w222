/**
 * @file lv_log.h
 *
 */

#ifndef LV_LOG_H
#define LV_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/

/*Возможный уровень журнала. Для совместимости объявите его независимо от `LV_USE_LOG`.*/

#define LV_LOG_LEVEL_TRACE 0 /**< Log detailed information. */
#define LV_LOG_LEVEL_INFO  1 /**< Log important events. */
#define LV_LOG_LEVEL_WARN  2 /**< Log if something unwanted happened but didn't caused problem. */
#define LV_LOG_LEVEL_ERROR 3 /**< Log only critical issues, when system may fail. */
#define LV_LOG_LEVEL_USER  4 /**< Log only custom log messages added by the user. */
#define LV_LOG_LEVEL_NONE  5 /**< Do not log anything. */
#define LV_LOG_LEVEL_NUM   5 /**< Number of log levels */

LV_EXPORT_CONST_INT(LV_LOG_LEVEL_TRACE);
LV_EXPORT_CONST_INT(LV_LOG_LEVEL_INFO);
LV_EXPORT_CONST_INT(LV_LOG_LEVEL_WARN);
LV_EXPORT_CONST_INT(LV_LOG_LEVEL_ERROR);
LV_EXPORT_CONST_INT(LV_LOG_LEVEL_USER);
LV_EXPORT_CONST_INT(LV_LOG_LEVEL_NONE);

typedef int8_t lv_log_level_t;

#if LV_USE_LOG

#if LV_LOG_USE_FILE_LINE
#define LV_LOG_FILE __FILE__
#define LV_LOG_LINE __LINE__
#else
#define LV_LOG_FILE NULL
#define LV_LOG_LINE 0
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Функция печати журнала. Получает строковый буфер для печати".
 */
typedef void (*lv_log_print_g_cb_t)(lv_log_level_t level, const char * buf);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Зарегистрируйте пользовательскую функцию печати/записи для вызова при добавлении журнала.
 * Он может форматировать «Путь к файлу», «Номер строки» и «Описание» по мере необходимости.
 * и отправьте отформатированное сообщение журнала на консоль или последовательный порт.
 * @param           print_cb указатель функции для печати журнала
 */
void lv_log_register_print_cb(lv_log_print_g_cb_t print_cb);

/**
 * Распечатать сообщение журнала через `printf`, если это разрешено с помощью`LV_LOG_PRINTF`в `lv_conf.h`.
 * и/или обратный вызов печати, если он зарегистрирован в `lv_log_register_print_cb`
 * @param format    строка формата в формате printf
 * @param ... параметры для `format`
 */
void lv_log(const char * format, ...) LV_FORMAT_ATTRIBUTE(1, 2);

/**
 * Добавить журнал
 * @param level     уровень журнала. (Из перечисления `lv_log_level_t`)
 * @param file      имя файла при добавлении журнала
 * @param line      номер строки в исходном коде, куда добавлен журнал
 * @param func      имя функции при добавлении журнала
 * @param format    строка формата в формате printf
 * @param ... параметры для `format`
 */
void lv_log_add(lv_log_level_t level, const char * file, int line,
                const char * func, const char * format, ...) LV_FORMAT_ATTRIBUTE(5, 6);

/**********************
 *      MACROS
 **********************/
#ifndef LV_LOG_TRACE
#  if LV_LOG_LEVEL <= LV_LOG_LEVEL_TRACE
#    define LV_LOG_TRACE(...) lv_log_add(LV_LOG_LEVEL_TRACE, LV_LOG_FILE, LV_LOG_LINE, __func__, __VA_ARGS__)
#  else
#    define LV_LOG_TRACE(...) do {}while(0)
#  endif
#endif

#ifndef LV_LOG_INFO
#  if LV_LOG_LEVEL <= LV_LOG_LEVEL_INFO
#    define LV_LOG_INFO(...) lv_log_add(LV_LOG_LEVEL_INFO, LV_LOG_FILE, LV_LOG_LINE, __func__, __VA_ARGS__)
#  else
#    define LV_LOG_INFO(...) do {}while(0)
#  endif
#endif

#ifndef LV_LOG_WARN
#  if LV_LOG_LEVEL <= LV_LOG_LEVEL_WARN
#    define LV_LOG_WARN(...) lv_log_add(LV_LOG_LEVEL_WARN, LV_LOG_FILE, LV_LOG_LINE, __func__, __VA_ARGS__)
#  else
#    define LV_LOG_WARN(...) do {}while(0)
#  endif
#endif

#ifndef LV_LOG_ERROR
#  if LV_LOG_LEVEL <= LV_LOG_LEVEL_ERROR
#    define LV_LOG_ERROR(...) lv_log_add(LV_LOG_LEVEL_ERROR, LV_LOG_FILE, LV_LOG_LINE, __func__, __VA_ARGS__)
#  else
#    define LV_LOG_ERROR(...) do {}while(0)
#  endif
#endif

#ifndef LV_LOG_USER
#  if LV_LOG_LEVEL <= LV_LOG_LEVEL_USER
#    define LV_LOG_USER(...) lv_log_add(LV_LOG_LEVEL_USER, LV_LOG_FILE, LV_LOG_LINE, __func__, __VA_ARGS__)
#  else
#    define LV_LOG_USER(...) do {}while(0)
#  endif
#endif

#ifndef LV_LOG
#  if LV_LOG_LEVEL < LV_LOG_LEVEL_NONE
#    define LV_LOG(...) lv_log(__VA_ARGS__)
#  else
#    define LV_LOG(...) do {} while(0)
#  endif
#endif

#else /*LV_USE_LOG*/

/*Ничего не делайте, если `LV_USE_LOG 0`*/
#define lv_log_add(level, file, line, ...)
#define LV_LOG_TRACE(...) do {}while(0)
#define LV_LOG_INFO(...) do {}while(0)
#define LV_LOG_WARN(...) do {}while(0)
#define LV_LOG_ERROR(...) do {}while(0)
#define LV_LOG_USER(...) do {}while(0)
#define LV_LOG(...) do {}while(0)

#endif /*LV_USE_LOG*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LOG_H*/
