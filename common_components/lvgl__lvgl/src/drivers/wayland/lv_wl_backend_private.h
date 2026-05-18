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
 * @brief Initialize the backend context
 *
 * Эта функция вызывается один раз, когда драйвер Wayland инициализируется для создания
 * глобальный внутренний контекст. Возвращенный указатель будет передан как backend_ctx.
 * ко всем остальным серверным операциям.
 *
 * @return Pointer to backend-specific context data, or NULL on failure
 *
 * @note This is called before any displays are created
 * @see lv_wayland_backend_deinit_t
 */
typedef void * (*lv_wayland_backend_init_t)(void);

/**
 * @typedef lv_wayland_backend_deinit_t
 * @brief Deinitialize the backend context
 *
 * Эта функция вызывается при деинициализации драйвера Wayland. Это должно
 * очистите все ресурсы, выделенные в функции инициализации, и освободите серверную часть
 * контекст.
 *
 * @param [in] backend_ctx Указатель на внутренний контекст, возвращаемый init
 *
 * @note This is called after all displays have been destroyed
 * @see lv_wayland_backend_init_t
 */
typedef void (*lv_wayland_backend_deinit_t)(void * backend_ctx);

/**
 * @typedef lv_wayland_backend_init_display_t
 * @brief Initialize a new display
 *
 * Эта функция вызывается при создании нового дисплея LVGL на Wayland. Это должно
 * выделить и инициализировать ресурсы для каждого дисплея, необходимые для рендеринга.
 *
 * @param [in] backend_ctx Указатель на внутренний контекст
 * Дисплей @param [in] Указатель на экранный объект LVGL
 * @param [in] ширина Начальная ширина дисплея в пикселях
 * @param [дюймы] высота Начальная высота дисплея в пикселях
 * @return Pointer to display-specific data, or NULL on failure
 *
 * @note The returned pointer can be retrieved later using
 *       lv_wayland_get_backend_display_data ()
 * @note It is expected that each display gets its own data structure in order for a backend
 *       для поддержки нескольких дисплеев
 */
typedef void * (*lv_wayland_backend_init_display_t)(void * backend_ctx, lv_display_t * display, int32_t width,
                                                    int32_t height);

/**
 * @typedef lv_wayland_backend_resize_display_t
 * @brief Resize or reconfigure a display
 *
 * Эта функция вызывается, когда необходимо изменить размер дисплея или когда его поворот
 * модифицирован. Серверная часть должна соответствующим образом обновить свои ресурсы рендеринга.
 *
 * @param [in] backend_ctx Указатель на внутренний контекст
 * @param [in] display Указатель на экранный объект LVGL, размер которого изменяется.
 * @return Pointer to updated display-specific data, or NULL on failure
 *
 * @note This may be called multiple times during a display's lifetime
 * @note The returned pointer will replace the previous display data. It can be
 *       получено с помощью lv_wayland_get_backend_display_data ()
 * @warning The display data is overwritten with the return value of this function
 */
typedef void * (*lv_wayland_backend_resize_display_t)(void * backend_ctx, lv_display_t * display);

/**
 * @typedef lv_wayland_backend_destroy_display_t
 * @brief Destroy a display
 *
 * Эта функция вызывается при уничтожении дисплея LVGL. Оно должно очиститься
 * все ресурсы для каждого дисплея и освободить данные дисплея, которые были выделены в
 * init_display.
 *
 * @param [in] backend_ctx Указатель на внутренний контекст
 * @param [in] display Указатель на уничтожаемый экранный объект LVGL
 *
 * @note The display data associated with this display must be freed
 */
typedef void (*lv_wayland_backend_destroy_display_t)(void * backend_ctx, lv_display_t * display);

/**
 * @typedef lv_wayland_backend_global_handler_t
 * @brief Handle Wayland global objects
 *
 * Эта функция вызывается для каждого глобального объекта, рекламируемого Wayland.
 * наборщик. Серверная часть может использовать это для привязки к протоколам Wayland, которые ему необходимы.
 * (например, расширения wl_shm , EGL, протоколы DMA - BUF и т. д.).
 *
 * @param [in] backend_ctx Указатель на внутренний контекст
 * @param [in] реестр Объект реестра Wayland
 * @param [in] name Числовое имя глобального объекта.
 * @param [in] интерфейс Строковое имя интерфейса (например, " wl_shm ")
 * @param [in] версия Номер версии интерфейса
 *
 * @note This is called during Wayland connection setup
 * @note The backend should use wl_registry_bind() to bind to needed protocols
 */
typedef void (*lv_wayland_backend_global_handler_t)(void * backend_ctx, struct wl_registry * registry, uint32_t name,
                                                    const char * interface, uint32_t version);

/**
 * @struct lv_wayland_backend_ops_t
 * @brief Wayland backend operations structure
 *
 * Эта структура определяет полный набор операций, которые выполняет серверная часть Wayland.
 * должен реализовать. Все указатели на функции не должны быть NULL.
 *
 * @par Порядок жизненного цикла:
 * 1. init() - Initialize backend context
 * 2. global_handler() - Called for each Wayland global (may be called multiple times)
 * 3. init_display() - Create display (may be called multiple times for multiple displays)
 * 4. resize_display() - Resize display (called as needed)
 * 5. deinit_display() - Destroy display (called once per display)
 * 6. deinit() - Clean up backend context
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

/** @brief Get the backend-specific display data
 *
 * Получает указатель данных для каждого дисплея, который был возвращен серверной частью.
 * Функции init_display/resize_display. Это позволяет серверной части получить доступ к своим собственным
 * состояние и ресурсы, специфичные для дисплея.
 *
 * Дисплей @param [in] Указатель на экранный объект LVGL
 * @return Pointer to backend-specific display data
 *
 * @note This returns the value that was returned by lv_wayland_backend_init_display_t
 *       или lv_wayland_backend_resize_display_t
 * @see lv_wayland_backend_init_display_t
 */
void * lv_wayland_get_backend_display_data(lv_display_t * display);

/** @brief Set the backend-specific display data
 *
 * Для данных отображения серверной части автоматически устанавливается тип возвращаемого значения
 * init_display/resize_display, эту функцию следует использовать только в особых случаях.
 * случаи, когда его необходимо временно отменить
 *
 * Дисплей @param [in] Указатель на экранный объект LVGL
 * @param [in] data Указатель на отображаемые данные, специфичные для серверной части
 */
void lv_wayland_set_backend_display_data(lv_display_t * display, void * data);

/**
 * @brief Get the Wayland surface for rendering
 *
 * Извлекает объект wl_surface, связанный с окном отображения. Это
 * поверхность, которую серверная часть должна использовать для всех операций рендеринга (присоединение буферов,
 * фиксация кадров и т.д.).
 *
 * Дисплей @param [in] Указатель на экранный объект LVGL
 * @return Pointer to the Wayland surface for rendering, or NULL if not available
 *
 * @note This surface is managed by the Wayland driver and must not be destroyed
 *       через серверную часть
 * @note All rendering output should be attached to this surface
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
