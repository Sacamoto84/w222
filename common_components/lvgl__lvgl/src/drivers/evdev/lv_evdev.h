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
 * @param indev      indev, созданный для недавно обнаруженного evdev
 * @param type       тип evdev
 * @param user_data  специальный параметр
 */
typedef void (*lv_evdev_discovery_cb_t)(lv_indev_t * indev, lv_evdev_type_t type, void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создать устройство ввода evdev по заданному пути.
 * @param типа LV_INDEV_TYPE_POINTER или LV_INDEV_TYPE_KEYPAD
 * @param dev_path путь к устройству, например, /dev/input/event0
 * @return указатель на устройство ввода или NULL, если открытие не удалось
 */
lv_indev_t * lv_evdev_create(lv_indev_type_t indev_type, const char * dev_path);

/**
 * Создайте устройство ввода evdev, взяв на себя требования к данным дескриптором файла.
 * @param типа LV_INDEV_TYPE_POINTER или LV_INDEV_TYPE_KEYPAD
 * @param fd файловый дескриптор устройства evdev
 * @return указатель на устройство ввода или NULL, если открытие не удалось
 */
lv_indev_t * lv_evdev_create_fd(lv_indev_type_t indev_type, int fd);

/**
 * включить автоматическое создание файлов evdev для всех новых и существующих
 * Устройства evdev найдены в /dev/input/
 * @param cb         функция для вызова при обнаружении нового evdev indev или `NULL`
 * @param user_data  параметр для передачи в обратный вызов
 * @return           статус успеха или неудачи. Это потерпит неудачу, если это
 *                   уже запущен или ресурсы не удалось инициализировать.
 */
lv_result_t lv_evdev_discovery_start(lv_evdev_discovery_cb_t cb, void * user_data);

/**
 * Прекратите создание evdev indev. Безопасно звонить из
 * обратный вызов обнаружения.
 * @return  статус успеха или неудачи. Он потерпит неудачу, если он уже запущен.
 */
lv_result_t lv_evdev_discovery_stop(void);

/**
 * Установите, следует ли менять местами координаты указателя. По умолчанию
 * ложь.
 * @param indev устройство ввода evdev
 * @param swap_axes стоит ли менять местами оси x и y
 */
void lv_evdev_set_swap_axes(lv_indev_t * indev, bool swap_axes);

/**
 * Настройте преобразование координат для указателей. Применяется после
 * замена осей, если таковая имеется. По умолчанию трансформация не применяется.
 * @param indev устройство ввода evdev
 * @param min_x Координата указателя сопоставлена с min x дисплея
 * @param min_y координата указателя, сопоставленная с min y дисплея
 * @param max_x Координата указателя сопоставлена с максимальным x дисплея
 * @param max_y координата указателя сопоставлена с максимальным значением y дисплея
 */
void lv_evdev_set_calibration(lv_indev_t * indev, int min_x, int min_y, int max_x, int max_y);

/**
 * Удалите устройство ввода evdev.
 * @param indev устройство ввода evdev, чтобы закрыть и освободить
 */
void lv_evdev_delete(lv_indev_t * indev);

#endif /*LV_USE_EVDEV*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_EVDEV_H*/
