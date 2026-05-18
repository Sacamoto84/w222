/**
 * @file lv_libinput.h
 *
 */

#ifndef LV_LIBINPUT_H
#define LV_LIBINPUT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../indev/lv_indev.h"

#if LV_USE_LIBINPUT

#include <poll.h>
#include <pthread.h>

#if LV_LIBINPUT_XKB
#include "lv_xkb.h"
#endif /* LV_LIBINPUT_XKB */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_LIBINPUT_CAPABILITY_NONE     = 0,
    LV_LIBINPUT_CAPABILITY_KEYBOARD = 1U << 0,
    LV_LIBINPUT_CAPABILITY_POINTER  = 1U << 1,
    LV_LIBINPUT_CAPABILITY_TOUCH    = 1U << 2
} lv_libinput_capability;

struct libinput_device;

#define LV_LIBINPUT_MAX_EVENTS 32

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Определите возможности ввода библиотеки устройства.
 * @param device устройство libinput для запроса
 * @return поддерживаемые возможности ввода
 */
lv_libinput_capability lv_libinput_query_capability(struct libinput_device * device);

/**
 * Найдите подключенное устройство ввода с конкретными возможностями
 * @param capabilities необходимые возможности устройства
 * @param force_rescan сотрите кэш устройства (если есть) и повторно просканируйте файловую систему на наличие доступных устройств.
 * @return путь узла устройства (например, /dev/input/event0) для первого соответствующего устройства или NULL, если устройство не найдено.
 *         Указатель можно безопасно использовать до следующего принудительного поиска устройства.
 */
char * lv_libinput_find_dev(lv_libinput_capability capabilities, bool force_rescan);

/**
 * Найдите подключенные устройства ввода с конкретными возможностями
 * @param capabilities необходимые возможности устройства
 * @param found предварительно выделенный массив для хранения путей к найденным узлам устройств (например, /dev/input/event0). Указатели
 *                безопасно использовать до следующего принудительного поиска устройства.
 * @param count максимальное количество устройств для поиска (массив устройств должен быть как минимум такой длины)
 * @param force_rescan сотрите кэш устройства (если есть) и повторно просканируйте файловую систему на наличие доступных устройств.
 * @return количество найденных устройств
 */
size_t lv_libinput_find_devs(lv_libinput_capability capabilities, char ** found, size_t count, bool force_rescan);

/**
 * Создайте новое устройство ввода libinput.
 * @param indev_type LV_INDEV_TYPE_POINTER или LV_INDEV_TYPE_KEYPAD
 * @param dev_path путь к устройству, например. /dev/input/event0
 * @return указатель на устройство ввода или NULL, если открытие не удалось
 */
lv_indev_t * lv_libinput_create(lv_indev_type_t indev_type, const char * dev_path);

/**
 * Удаление устройства ввода libinput
 * @param indev указатель на устройство ввода
 */
void lv_libinput_delete(lv_indev_t * indev);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_LIBINPUT */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_LIBINPUT_H */
