/**
 * @file lv_evdev.h
 *
 */

#ifndef LV_EVDEV_H
#define LV_EVDEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../indev/lv_indev.h"

#if LV_USE_EVDEV

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_EVDEV_TYPE_REL,   /**< mice */
    LV_EVDEV_TYPE_ABS,   /**< touch screens, mousepads */
    LV_EVDEV_TYPE_KEY    /**< keyboards, keypads, buttons */
} lv_evdev_type_t;

/**
 * @param indev      the indev created for the newly discovered evdev
 * @param type       the type of the evdev
 * @param user_data  a custom parameter
 */
typedef void (*lv_evdev_discovery_cb_t)(lv_indev_t * indev, lv_evdev_type_t type, void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создать устройство ввода evdev по заданному пути.
 * @param type LV_INDEV_TYPE_POINTER or LV_INDEV_TYPE_KEYPAD
 * @param dev_path device path, e.g., /dev/input/event0
 * @return pointer to input device or NULL if opening failed
 */
lv_indev_t * lv_evdev_create(lv_indev_type_t indev_type, const char * dev_path);

/**
 * Создайте устройство ввода evdev, взяв на себя владение данным дескриптором файла.
 * @param type LV_INDEV_TYPE_POINTER or LV_INDEV_TYPE_KEYPAD
 * @param fd file descriptor of the evdev device
 * @return pointer to input device or NULL if opening failed
 */
lv_indev_t * lv_evdev_create_fd(lv_indev_type_t indev_type, int fd);

/**
 * Начните автоматически создавать файлы evdev для всех новых и существующих
 * Устройства evdev найдены в /dev/input/
 * @param cb         function to call when a new evdev indev is discovered, or `NULL`
 * @param user_data  parameter to pass to the callback
 * @return           the success or failure status. It will fail if it's
 *                   уже запущен или ресурсы не удалось инициализировать.
 */
lv_result_t lv_evdev_discovery_start(lv_evdev_discovery_cb_t cb, void * user_data);

/**
 * Прекратите автоматическое создание evdev indev. Безопасно звонить из
 * обратный вызов обнаружения.
 * @return  the success or failure status. It will fail if it's already running.
 */
lv_result_t lv_evdev_discovery_stop(void);

/**
 * Установите, следует ли менять местами координаты указателя. По умолчанию
 * ложь.
 * @param indev evdev input device
 * @param swap_axes whether to swap x and y axes
 */
void lv_evdev_set_swap_axes(lv_indev_t * indev, bool swap_axes);

/**
 * Настройте преобразование координат для указателей. Применяется после
 * замена осей, если таковая имеется. По умолчанию трансформация не применяется.
 * @param indev evdev input device
 * @param min_x pointer coordinate mapped to min x of display
 * @param min_y pointer coordinate mapped to min y of display
 * @param max_x pointer coordinate mapped to max x of display
 * @param max_y pointer coordinate mapped to max y of display
 */
void lv_evdev_set_calibration(lv_indev_t * indev, int min_x, int min_y, int max_x, int max_y);

/**
 * Удалите устройство ввода evdev.
 * @param indev evdev input device to close and free
 */
void lv_evdev_delete(lv_indev_t * indev);

#endif /*LV_USE_EVDEV*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_EVDEV_H*/
