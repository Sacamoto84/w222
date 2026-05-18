/**
 * @file lv_wl_backend_private.h
 *
 */

#ifndef LV_WL_BACKEND_PRIVATE_H
#define LV_WL_BACKEND_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland.h"

#if LV_USE_WAYLAND

#include <wayland-client.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * @typedef lv_wayland_backend_init_t
 * @brief Инициализируйте внутренний контекст
 *
 * Эта функция возникает один раз, когда драйвер Wayland разрабатывается для создания
 * глобальный внутренний контекст. Возвращенный указатель будет передан какbackend_ctx.
 * ко всем остальным серверным операциям.
 *
 * @return Указатель на контекстные данные, специфичные для серверной части, илиNULLв случае сбоя.
 *
 * @note Это вызывается перед созданием каких-либо дисплеев.
 * @see lv_wayland_backend_deinit_t
 */
typedef void * (*lv_wayland_backend_init_t)(void);

/**
 * @typedef lv_wayland_backend_deinit_t
 * @brief Деинициализировать внутренний контекст
 *
 * Эта функция возникает при деинициализации драйвера Wayland. Это должно быть
 * очистите все ресурсы, выделенные в функции инициализации, и освободите серверную часть
 * контекст.
 *
 * @param [in] backend_ctx Указатель внутреннего контекста, возвращаемый init
 *
 * @note Это вызывается после того, как все дисплеи были уничтожены.
 * @see lv_wayland_backend_init_t
 */
typedef void (*lv_wayland_backend_deinit_t)(void * backend_ctx);

/**
 * @typedef lv_wayland_backend_init_display_t
 * @brief Инициализировать новый дисплей
 *
 * Эта функция возникает при создании новых видеокартLVGLна Wayland. Это должно быть
 * выделить и инициализировать ресурсы для каждого дисплея, необходимые для рендеринга.
 *
 * @param [in] backend_ctx Указатель на внутренний контекст
 * Дисплей@param[in] Указатель на экранный объект LVGL
 * @param [in] ширина Начальная ширина дисплея в пикселях
 * @param [дюймы] высота Начальная высота дисплея в пикселях
 * @return Указатель на отображение конкретных данных илиNULLв случае сбоя.
 *
 * @note Возвращенный указатель можно получить позже, используя
 *       lv_wayland_get_backend_display_data ()
 * @note Ожидается, что каждый дисплей получит свою собственную структуру данных для серверной части.
 *       для поддержки нескольких дисплеев
 */
typedef void * (*lv_wayland_backend_init_display_t)(void * backend_ctx, lv_display_t * display, int32_t width,
                                                    int32_t height);

/**
 * @typedef lv_wayland_backend_resize_display_t
 * @brief Изменение размера или перенастройка дисплея
 *
 * Эта функция вызывается, когда необходимо изменить размер дисплея или когда его поворот
 * модифицирован. Серверная часть должна соответствующим образом обновить свои ресурсы рендеринга.
 *
 * @param [in] backend_ctx Указатель на внутренний контекст
 * @param [in] display Указатель на экранный объект LVGL, размер которого изменяется.
 * @return Указатель на обновленные данные, специфичные для дисплея, илиNULLв случае сбоя.
 *
 * @note Это может быть вызвано несколько раз в течение жизни дисплея.
 * @note Возвращенный указатель заменит предыдущие отображаемые данные. Это может быть
 *       получено с помощью lv_wayland_get_backend_display_data()
 * @warning Данные дисплея перезаписываются возвращаемым значением этой функции.
 */
typedef void * (*lv_wayland_backend_resize_display_t)(void * backend_ctx, lv_display_t * display);

/**
 * @typedef lv_wayland_backend_destroy_display_t
 * @brief Уничтожить дисплей
 *
 * Эта функция вызывается при уничтожении дисплея LVGL. Оно должно очиститься
 * все ресурсы для каждого дисплея и освободить данные дисплея, которые были выделены в
 * init_display.
 *
 * @param [in] backend_ctx Указатель на внутренний контекст
 * @param [in] display Указатель на уничтожаемый экранный объект LVGL
 *
 * @note Данные дисплея, связанные с этим дисплеем, должны быть освобождены.
 */
typedef void (*lv_wayland_backend_destroy_display_t)(void * backend_ctx, lv_display_t * display);

/**
 * @typedef lv_wayland_backend_global_handler_t
 * @brief Обработка глобальных объектов Wayland
 *
 * Эта функция доступна для каждой глобальной точки, рекламируемого Wayland.
 * наборщик. Серверная часть может использовать ее для привязки к протоколам Wayland, которые ему необходимы.
 * (например, расширения wl_shm, EGL, протоколыDMA-BUFи т. д.).
 *
 * @param [in] backend_ctx Указатель на внутренний контекст
 * @param [in] реестр объектов реестра Wayland
 * @param [in] name Числовое имя глобального объекта.
 * @param [in] интерфейс Строковое имя интерфейса (например, "wl_shm")
 * @param [in] версия Номер версии интерфейса
 *
 * @note Это вызывается во время настройки соединения Wayland.
 * @note Бэкэнд должен использовать wl_registry_bind() для привязки к необходимым протоколам.
 */
typedef void (*lv_wayland_backend_global_handler_t)(void * backend_ctx, struct wl_registry * registry, uint32_t name,
                                                    const char * interface, uint32_t version);

/**
 * @struct lv_wayland_backend_ops_t
 * @brief Структура серверных операций Wayland
 *
 * Эта структура определяет полный набор операций, которые выполняют серверную часть Wayland.
 * должен реализовать. Все указатели на функции не должны быть NULL.
 *
 * @par Порядок жизненного цикла:
 * 1. init() — инициализировать внутренний контекст.
 * 2. global_handler () — вызывается для каждого глобального Wayland (может вызываться несколько раз)
 * 3. init_display () — Создать дисплей (может вызываться несколько раз для нескольких дисплеев)
 * 4. resize_display() — Изменение размера дисплея (вызывается по необходимости)
 * 5. deinit_display() — Уничтожить дисплей (вызывается один раз для каждого дисплея)
 * 6. deinit() — Очистка внутреннего контекста.
 */
typedef struct {
    lv_wayland_backend_init_t init;                         /**< Initialize backend context */
    lv_wayland_backend_global_handler_t global_handler;     /**< Handle Wayland global objects */
    lv_wayland_backend_init_display_t init_display;         /**< Initialize a new display */
    lv_wayland_backend_resize_display_t resize_display;     /**< Resize or reconfigure display */
    lv_wayland_backend_destroy_display_t deinit_display;    /**< Destroy a display */
    lv_wayland_backend_deinit_t deinit;                     /**< Deinitialize backend context */
} lv_wayland_backend_ops_t;

extern const lv_wayland_backend_ops_t wl_backend_ops;

/** @brief Получите данные отображения, специфичные для серверной части
 *
 * Получает указатель данных для каждого дисплея, который был возвращен серверной частью.
 * Функции init_display/resize_display. Это позволяет серверной части получить доступ к своей собственной
 * состояние и ресурсы, специфичные для дисплея.
 *
 * Дисплей@param[in] Указатель на экранный объект LVGL
 * @return Указатель на данные отображения, специфичные для серверной части
 *
 * @note Это возвращает значение, возвращенное lv_wayland_backend_init_display_t.
 *       или lv_wayland_backend_resize_display_t
 * @see lv_wayland_backend_init_display_t
 */
void * lv_wayland_get_backend_display_data(lv_display_t * display);

/** @brief Установите данные отображения, специфичные для серверной части
 *
 * Для данных отображения серверной части автоматически устанавливается тип возвращаемого значения
 * init_display /resize_display, эту функцию следует использовать только в особых случаях.
 * случаи, когда его необходимо временно отменить
 *
 * Дисплей@param[in] Указатель на экранный объект LVGL
 * @param [in] data Указатель на отображаемые данные, специфичные для серверной части
 */
void lv_wayland_set_backend_display_data(lv_display_t * display, void * data);

/**
 * @brief Получите поверхность Wayland для рендеринга
 *
 * Извлекает объектwl_surface, связанный с оконным отображением. Это
 * поверхность, которую серверная часть должна использовать для всех операций рендеринга (присоединение буферов,
 * фиксация кадров и т.д.).
 *
 * Дисплей@param[in] Указатель на экранный объект LVGL
 * @return Указатель на поверхность Wayland для рендеринга или NULL, если недоступен.
 *
 * @note Эта поверхность управляется драйвером Wayland и не должна быть разрушена.
 *       через серверную часть
 * @note Все выходные данные рендеринга должны быть прикреплены к этой поверхности.
 */
struct wl_surface * lv_wayland_get_window_surface(lv_display_t * display);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_WAYLAND*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_WL_BACKEND_PRIVATE_H*/
