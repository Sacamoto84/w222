/**
 * @file lv_display.h
 *
 */

#ifndef LV_DISPLAY_H
#define LV_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_types.h"
#include "../misc/lv_timer.h"
#include "../misc/lv_event.h"
#include "../misc/lv_color.h"
#include "../misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/

#ifndef LV_ATTRIBUTE_FLUSH_READY
#define LV_ATTRIBUTE_FLUSH_READY
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_DISPLAY_ROTATION_0 = 0,
    LV_DISPLAY_ROTATION_90,
    LV_DISPLAY_ROTATION_180,
    LV_DISPLAY_ROTATION_270
} lv_display_rotation_t;

typedef enum {
    /**
     * Используйте буфер(ы) для рендеринга меньших частей экрана.
     * Таким образом, буферы могут быть меньше, чем дисплей, чтобы сохранить RAM. Рекомендуется использовать буфер(ы) размером не менее 1/10 размера экрана.
     */
    LV_DISPLAY_RENDER_MODE_PARTIAL,

    /**
     * Буфер(ы) должен быть размером с экран, и LVGL будет отображаться в правильном месте буфера.
     * Таким образом, буфер всегда будет содержать все изображение. Будут обновлены только измененные области.
     * При наличии двух буферов буферы буферов синхронизируются автоматически, а вflush_cbтребуется только изменение адреса.
     */
    LV_DISPLAY_RENDER_MODE_DIRECT,

    /**
     * Всегда перерисовывайте весь экран, даже если был изменен только один пиксель.
     * При наличии двух буферов вflush_cbтребуется только изменение адреса.
     */
    LV_DISPLAY_RENDER_MODE_FULL,
} lv_display_render_mode_t;

typedef enum {
    LV_SCREEN_LOAD_ANIM_NONE,
    LV_SCREEN_LOAD_ANIM_OVER_LEFT,
    LV_SCREEN_LOAD_ANIM_OVER_RIGHT,
    LV_SCREEN_LOAD_ANIM_OVER_TOP,
    LV_SCREEN_LOAD_ANIM_OVER_BOTTOM,
    LV_SCREEN_LOAD_ANIM_MOVE_LEFT,
    LV_SCREEN_LOAD_ANIM_MOVE_RIGHT,
    LV_SCREEN_LOAD_ANIM_MOVE_TOP,
    LV_SCREEN_LOAD_ANIM_MOVE_BOTTOM,
    LV_SCREEN_LOAD_ANIM_FADE_IN,
    LV_SCREEN_LOAD_ANIM_FADE_ON = LV_SCREEN_LOAD_ANIM_FADE_IN, /*Для обратной совместимости*/
    LV_SCREEN_LOAD_ANIM_FADE_OUT,
    LV_SCREEN_LOAD_ANIM_OUT_LEFT,
    LV_SCREEN_LOAD_ANIM_OUT_RIGHT,
    LV_SCREEN_LOAD_ANIM_OUT_TOP,
    LV_SCREEN_LOAD_ANIM_OUT_BOTTOM,
} lv_screen_load_anim_t;

typedef void (*lv_display_flush_cb_t)(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
typedef void (*lv_display_flush_wait_cb_t)(lv_display_t * disp);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создать новый дисплей с заданным разрешением.
 * @param hor_res   горизонтальное разрешение в пикселях
 * @param ver_res   вертикальное разрешение в пикселях
 * @return          указатель на экранный объект или`NULL`в случае ошибки
 */
lv_display_t * lv_display_create(int32_t hor_res, int32_t ver_res);

/**
 * Удаление дисплея
 * @param disp      указатель для отображения
 */
void lv_display_delete(lv_display_t * disp);

/**
 * Установите дисплей по умолчанию. Новые экраны будут создаваться на нем по умолчанию.
 * @param disp      указатель на дисплей
 */
void lv_display_set_default(lv_display_t * disp);

/**
 * Получить отображение по умолчанию
 * @return          указатель на дисплей по умолчанию
 */
lv_display_t * lv_display_get_default(void);

/**
 * Получите следующий дисплей.
 * @param disp      указатель на текущий дисплей. NULLдля инициализации.
 * @return          следующий дисплей или NULL, если не более. Дает первое отображение, когда параметр равен NULL.
 */
lv_display_t * lv_display_get_next(lv_display_t * disp);

/*---------------------
 * RESOLUTION
 *--------------------*/

/**
 * Устанавливает разрешение дисплея.  Событие `LV_EVENT_RESOLUTION_CHANGED` будет отправлено.
 * Здесь должно быть установлено родное разрешение устройства. Если позже дисплей будет повернут с помощью
 * `lv_display_set_rotation`LVGLпоменяет хор. и вер. разрешение автоматически.
 * @param disp      указатель на дисплей
 * @param hor_res   новое горизонтальное разрешение
 * @param ver_res   новое вертикальное разрешение
 */
void lv_display_set_resolution(lv_display_t * disp, int32_t hor_res, int32_t ver_res);

/**
 * Для LVGL не обязательно использовать весь дисплей, однако в некоторых случаях физическое разрешение важно.
 * Например, тачпад по-прежнему видит полное разрешение, и значения необходимо преобразовать.
 * в активную область отображения LVGL.
 * @param disp      указатель на дисплей
 * @param hor_res   новое физическое горизонтальное разрешение или -1, чтобы предположить, что оно такое же, как обычное горизонтальное разрешение. рез.
 * @param ver_res   новое физическое вертикальное разрешение или -1, чтобы предположить, что оно такое же, как обычное горизонтальное. рез.
 */
void lv_display_set_physical_resolution(lv_display_t * disp, int32_t hor_res, int32_t ver_res);

/**
 * Если физическое разрешение не совпадает с нормальным разрешением
 * Здесь можно установить смещение активной области отображения.
 * @param disp      указатель на дисплей
 * @param x         Смещение по оси X
 * @param y         Смещение по оси Y
 */
void lv_display_set_offset(lv_display_t * disp, int32_t x, int32_t y);

/**
 * Установите поворот этого дисплея.  LVGL внутренне поменяет местами горизонтальное и вертикальное разрешения.
 * @param disp      указатель на дисплей (NULLдля использования дисплея по умолчанию)
 * @param rotation `LV_DISPLAY_ROTATION_0/90/180/270`
 */
void lv_display_set_rotation(lv_display_t * disp, lv_display_rotation_t rotation);

/**
 * Используйте вращение матрицы для дисплея. Эта функция зависит от `LV_DRAW_TRANSFORM_USE_MATRIX`
 * @param disp      указатель на дисплей (NULLдля использования дисплея по умолчанию)
 * @param enable    true: включить вращение матрицы, false: отключить
 */
void lv_display_set_matrix_rotation(lv_display_t * disp, bool enable);

/**
 * Установите DPI (точек на дюйм) дисплея.
 * dpi = sqrt(hor_res^2 +ver_res^2) / диагональ"
 * @param disp      указатель на дисплей
 * @param dpi       новый DPI
 */
void lv_display_set_dpi(lv_display_t * disp, int32_t dpi);

/**
 * Получите горизонтальное разрешение дисплея.
 * @param disp      указатель на дисплей (NULLдля использования дисплея по умолчанию)
 * @return          горизонтальное разрешение дисплея.
 */
int32_t lv_display_get_horizontal_resolution(const lv_display_t * disp);

/**
 * Получить вертикальное разрешение дисплея
 * @param disp      указатель на дисплей (NULLдля использования дисплея по умолчанию)
 * @return          вертикальное разрешение дисплея
 */
int32_t lv_display_get_vertical_resolution(const lv_display_t * disp);

/**
 * Получите исходное горизонтальное разрешение дисплея без учета вращения.
 * @param disp      указатель на дисплей (NULLдля использования дисплея по умолчанию)
 * @return          горизонтальное разрешение дисплея.
 */
int32_t lv_display_get_original_horizontal_resolution(const lv_display_t * disp);

/**
 * Получите исходное вертикальное разрешение дисплея без учета вращения.
 * @param disp      указатель на дисплей (NULLдля использования дисплея по умолчанию)
 * @return          вертикальное разрешение дисплея
 */
int32_t lv_display_get_original_vertical_resolution(const lv_display_t * disp);

/**
 * Получить физическое горизонтальное разрешение дисплея
 * @param disp      указатель на дисплей (NULLдля использования дисплея по умолчанию)
 * @return физическое горизонтальное разрешение дисплея
 */
int32_t lv_display_get_physical_horizontal_resolution(const lv_display_t * disp);

/**
 * Получить физическое вертикальное разрешение дисплея
 * @param disp      указатель на дисплей (NULLдля использования дисплея по умолчанию)
 * @return          физическое вертикальное разрешение дисплея
 */
int32_t lv_display_get_physical_vertical_resolution(const lv_display_t * disp);

/**
 * Получите горизонтальное смещение от полного/физического дисплея.
 * @param disp      указатель на дисплей (NULLдля использования дисплея по умолчанию)
 * @return          горизонтальное смещение от физического дисплея
 */
int32_t lv_display_get_offset_x(const lv_display_t * disp);

/**
 * Получите вертикальное смещение от полного/физического дисплея.
 * @param disp      указатель на дисплей (NULLдля использования дисплея по умолчанию)
 * @return          горизонтальное смещение от физического дисплея
 */
int32_t lv_display_get_offset_y(const lv_display_t * disp);

/**
 * Получите текущий поворот этого дисплея.
 * @param disp      указатель на дисплей (NULLдля использования дисплея по умолчанию)
 * @return          текущая ротация
 */
lv_display_rotation_t lv_display_get_rotation(lv_display_t * disp);

/**
 * Узнайте, включено ли вращение матрицы для дисплея или нет.
 * @param disp      указатель на дисплей (NULLдля использования дисплея по умолчанию)
 * @return          true: вращение матрицы включено; ложь: отключено
 */
bool lv_display_get_matrix_rotation(lv_display_t * disp);

/**
 * Получите DPI дисплея.
 * @param disp      указатель на дисплей (NULLдля использования дисплея по умолчанию)
 * @return          dpi дисплея
 */
int32_t lv_display_get_dpi(const lv_display_t * disp);

/*---------------------
 * BUFFERING
 *--------------------*/

/**
 * Установите буферы для отображения символов`lv_display_set_draw_buffers`, но примите необработанные указатели буфера.
 * Для режимов рендеринга DIRECT/FULL размер буфера должен быть не менее
 * `hor_res * ver_res * lv_color_format_get_size(lv_display_get_color_format(disp))`
 * @param disp              указатель на дисплей
 * @param buf1              первый буфер
 * @param buf2              второй буфер (может быть`NULL`)
 * @param buf_size          размер буфера в байтах
 * @param render_mode LV_DISPLAY_RENDER_MODE_PARTIAL / DIRECT / FULL
 */
void lv_display_set_buffers(lv_display_t * disp, void * buf1, void * buf2, uint32_t buf_size,
                            lv_display_render_mode_t render_mode);

/**
 * Установите буферы кадров для дисплея `lv_display_set_buffers`, но это не проблема.
 * для индивидуального шага в соответствии с требованиями контроллера дисплея.
 * Это позволяет буферам кадров иметь выравнивание шага, отличное от остальных
 * буферы`
 * @param disp              указатель на дисплей
 * @param buf1              первый буфер
 * @param buf2              второй буфер (может быть`NULL`)
 * @param buf_size          размер буфера в байтах
 * @param stride            шаг буфера в байтах
 * @param render_mode LV_DISPLAY_RENDER_MODE_PARTIAL / DIRECT / FULL
 */
void lv_display_set_buffers_with_stride(lv_display_t * disp, void * buf1, void * buf2, uint32_t buf_size,
                                        uint32_t stride, lv_display_render_mode_t render_mode);

/**
 * Установите буферы для отображения, примите указатель на буфер отрисовки.
 * Обычно для большинства случаев достаточно использовать `lv_display_set_buffers`.
 * Используйте эту функцию, если доступен существующийlv_draw_buf_t.
 * @param disp              указатель на дисплей
 * @param buf1              первый буфер
 * @param buf2              второй буфер (может быть`NULL`)
 */
void lv_display_set_draw_buffers(lv_display_t * disp, lv_draw_buf_t * buf1, lv_draw_buf_t * buf2);

/**
 * Установите третий буфер прорисовки для дисплея.
 * @param disp              указатель на дисплей
 * @param buf3              третий буфер
 */
void lv_display_set_3rd_draw_buffer(lv_display_t * disp, lv_draw_buf_t * buf3);

/**
 * Установить режим рендеринга дисплея
 * @param disp              указатель на дисплей
 * @param render_mode LV_DISPLAY_RENDER_MODE_PARTIAL / DIRECT / FULL
 */
void lv_display_set_render_mode(lv_display_t * disp, lv_display_render_mode_t render_mode);

/**
 * Установите обратный вызов сброса, который будет вызываться для копирования визуализированного изображения на дисплей.
 * @param disp      указатель на дисплей
 * @param flush_cb  обратный вызов сброса (`px_map` содержит визуализированное изображение в виде необработанной карты пикселей, и его следует скопировать в`area`на дисплее)
 */
void lv_display_set_flush_cb(lv_display_t * disp, lv_display_flush_cb_t flush_cb);

/**
 * Установите обратный вызов, который будет использоваться, пока LVGL ожидает завершения очистки.
 * Он может выполнять любую сложную логику ожидания, включая семафоры, мьютексы, флаги опроса и т. д.
 * Если он не установлен, используется флаг `disp->flushing`, который можно сохранить с помощью `lv_display_flush_ready()`.
 * @param disp      указатель на дисплей
 * @param wait_cb   обратный вызов для вызова, покаLVGLожидает готовности к сбросу.
 *                  ЕслиNULL`lv_display_flush_ready()` можно использовать для сигнализации или помощи промывки.
 */
void lv_display_set_flush_wait_cb(lv_display_t * disp, lv_display_flush_wait_cb_t wait_cb);

/**
 * Установите цветовой формат дисплея.
 * @param disp              указатель на дисплей
 * @param color_format      Возможные значения:
 *                          - LV_COLOR_FORMAT_RGB565
 *                          - LV_COLOR_FORMAT_RGB888
 *                          - LV_COLOR_FORMAT_XRGB888
 *                          - LV_COLOR_FORMAT_ARGB888
 *@note Чтобы изменить порядок байтов визуализированного изображения в случае формата RGB565
 *      (т.е. поменяйте местами 2 байта) вызовите`lv_draw_sw_rgb565_swap`в flush_cb
 */
void lv_display_set_color_format(lv_display_t * disp, lv_color_format_t color_format);

/**
 * Получить цветовой формат дисплея
 * @param disp              указатель на дисплей
 * @return                  цветовой формат
 */
lv_color_format_t lv_display_get_color_format(lv_display_t * disp);

/**
 * Установите количество тайлов для параллельного рендеринга.
 * @param disp              указатель на дисплей
 * @param tile_cnt          количество плиток (1 =<tile_cnt< 256)
 */
void lv_display_set_tile_cnt(lv_display_t * disp, uint32_t tile_cnt);

/**
 * Получить количество тайлов, используемых для параллельного рендеринга.
 * @param disp              указатель на дисплей
 * @return                  количество плиток
 */
uint32_t lv_display_get_tile_cnt(lv_display_t * disp);

/**
 * Отключение сглаживания не поддерживается начиная с версии v9. Эта функция будет удалена.
 * Включить сглаживание для механизма рендеринга
 * @param disp      указатель на дисплей
 * @param en правда/ложь
 */
void lv_display_set_antialiasing(lv_display_t * disp, bool en);

/**
 * Узнайте, включено ли сглаживание для дисплея или нет.
 * @param disp      указатель на дисплей (NULLдля использования дисплея по умолчанию)
 * @return истина/ложь
 */
bool lv_display_get_antialiasing(lv_display_t * disp);

/**
 * Вызов драйвера дисплея после завершения очистки
 * @param disp      указатель для отображения того, чей`flush_cb`был вызван
 */
LV_ATTRIBUTE_FLUSH_READY void lv_display_flush_ready(lv_display_t * disp);

/**
 * Скажите, является ли это последней областью процесса обновления.
 * Может быть вызван из`flush_cb`для выполнения специального обновления дисплея, если это необходимо, когда все области очищены.
 * @param disp      указатель для отображения
 * @return          true: это последняя область, которую нужно очистить;
 *                  false: есть и другие области, которые скоро будут обновлены
 */
LV_ATTRIBUTE_FLUSH_READY bool lv_display_flush_is_last(lv_display_t * disp);

bool lv_display_is_double_buffered(lv_display_t * disp);

/**
 * Получить режим рендеринга дисплея
 * @param disp      указатель на дисплей
 * @return          режим рендеринга дисплея (LV_DISPLAY_RENDER_MODE_PARTIAL/DIRECT/FULL)
 */
lv_display_render_mode_t lv_display_get_render_mode(lv_display_t * disp);

/*---------------------
 * SCREENS
 *--------------------*/

/**
 * Возврат указателя на активный экран дисплея
 * @param disp      указатель для отображения активного экрана, который следует получить.
 *                  ( NULL для использования экрана по умолчанию)
 * @return          указатель на активный объект экрана (загружается с помощью 'lv_screen_load()')
 */
lv_obj_t * lv_display_get_screen_active(lv_display_t * disp);

/**
 * Вернитесь с помощью указателя на предыдущий экран. Используется только во время перехода между экранами.
 * @param disp      указатель для отображения предыдущего экрана, который следует открыть.
 *                  ( NULL для использования экрана по умолчанию)
 * @return          указатель на предыдущий объект экрана или NULL, если сейчас не используется
 */
lv_obj_t * lv_display_get_screen_prev(lv_display_t * disp);

/**
 * Вернуть экран, который в данный момент загружается дисплеем
 * @param disp      указатель на объект отображения (NULLдля использования экрана по умолчанию)
 * @return          указатель на загружаемый экран или NULL, если в данный момент экран не загружается
 */
lv_obj_t * lv_display_get_screen_loading(lv_display_t * disp);

/**
 * Верните верхний слой. Верхний слой одинаков на всех экранах и находится над обычным слоем экрана.
 * @param disp      указатель для отображения верхнего слоя, который необходимо получить. (NULLдля использования экрана по умолчанию)
 * @return          указатель на объект верхнего слоя
 */
lv_obj_t * lv_display_get_layer_top(lv_display_t * disp);

/**
 * Верните сис. слой. Системный уровень одинаков на всех экранах и находится над обычным экраном и верхним слоем.
 * @param disp      указатель для отображения того, какой sys. слой должен быть восстановлен. (NULLдля использования экрана по умолчанию)
 * @return          указатель на объект системного слоя
 */
lv_obj_t * lv_display_get_layer_sys(lv_display_t * disp);

/**
 * Верните нижний слой. Нижний слой одинаков на всех экранах и находится под обычным слоем экрана.
 * Это видно только в том случае, если экран прозрачный.
 * @param disp      указатель для отображения (NULLдля использования экрана по умолчанию)
 * @return          указатель на объект нижнего слоя
 */
lv_obj_t * lv_display_get_layer_bottom(lv_display_t * disp);


#if LV_USE_OBJ_NAME

/**
 * Получить экран по его имени на дисплее. Имя должно быть установлено
 * `lv_obj_set_name()` или`lv_obj_set_name_static()`.
 * @param disp          указатель на дисплей или NULL, чтобы использовать дисплей по умолчанию
 * @param screen_name   название экрана, на котором нужно получить
 * @return              указатель на экран или NULL, если не найден.
 */
lv_obj_t * lv_display_get_screen_by_name(const lv_display_t * disp, const char * screen_name);

#endif /*LV_USE_OBJ_NAME*/

/**
 * Загрузить экран на дисплее по умолчанию
 * @param scr       указатель на экран
 */
void lv_screen_load(struct _lv_obj_t * scr);

/**
 * Переключение экрана с анимацией
 * @param scr       указатель на новый экран для загрузки
 * @param anim_type тип анимации из`lv_screen_load_anim_t`, например.  `LV_SCREEN_LOAD_ANIM_MOVE_LEFT`
 * @param time      время анимации
 * @param delay     задержка перед переходом
 * @param auto_del  true: автоматически удалить старый экран
 */
void lv_screen_load_anim(lv_obj_t * scr, lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay,
                         bool auto_del);

/**
 * Получить активный экран дисплея по умолчанию
 * @return          указатель на активный экран
 */
lv_obj_t * lv_screen_active(void);

/**
 * Получить верхний слой дисплея по умолчанию
 * @return          указатель на верхний слой
 */
lv_obj_t * lv_layer_top(void);

/**
 * Получить системный уровень дисплея по умолчанию
 * @return          указатель на системный слой
 */
lv_obj_t * lv_layer_sys(void);

/**
 * Получить нижний слой дисплея по умолчанию
 * @return          указатель на нижний слой
 */
lv_obj_t * lv_layer_bottom(void);

/*---------------------
 * OTHERS
 *--------------------*/

/**
 * Добавьте обработчик событий на дисплей
 * @param disp          указатель на дисплей
 * @param event_cb      обратный вызов события
 * @param filter        код события для реакции или `LV_EVENT_ALL`
 * @param user_data     опционально user_data
 */
void lv_display_add_event_cb(lv_display_t * disp, lv_event_cb_t event_cb, lv_event_code_t filter, void * user_data);

/**
 * Получить количество событий, прикрепленных к дисплею
 * @param disp          указатель на дисплей
 * @return              количество событий
 */
uint32_t lv_display_get_event_count(lv_display_t * disp);

/**
 * Получить дескриптор события для события
 * @param disp          указатель на дисплей
 * @param index         индекс события
 * @return              дескриптор события
 */
lv_event_dsc_t * lv_display_get_event_dsc(lv_display_t * disp, uint32_t index);

/**
 * Удалить событие
 * @param disp          указатель на дисплей
 * @param index         индекс события, которое нужно удалить
 * @return              true: и событие было удалено; false: ни одно событие не было удалено
 */
bool lv_display_delete_event(lv_display_t * disp, uint32_t index);

/**
 * Удалитеevent_cbс помощью user_data.
 * @param disp          указатель на дисплей
 * @param event_cb      event_cb события, которое нужно удалить
 * @param user_data user_data
 * @return              количество событий удалено
 */
uint32_t lv_display_remove_event_cb_with_user_data(lv_display_t * disp, lv_event_cb_t event_cb, void * user_data);

/**
 * Отправить событие на дисплей
 * @param disp          указатель на дисплей
 * @param code          код события.  LV_EVENT_...
 * @param param         необязательный параметр
 * @return              LV_RESULT_OK: дисп не был удален в этом событии.
 */
lv_result_t lv_display_send_event(lv_display_t * disp, lv_event_code_t code, void * param);

/**
 * Получите область, которая будет признана недействительной. Может использоваться в `LV_EVENT_INVALIDATE_AREA`.
 * @param e     указатель на событие
 * @return      область, которую следует признать недействительной (может быть изменена по мере необходимости)
 */
lv_area_t * lv_event_get_invalidated_area(lv_event_t * e);

/**
 * Установите тему дисплея. Если созданных пользователем виджетов еще нет, тема экранов будет обновлена.
 * @param disp      указатель на дисплей
 * @param th        указатель на тему
 */
void lv_display_set_theme(lv_display_t * disp, lv_theme_t * th);

/**
 * Получить тему дисплея
 * @param disp      указатель на дисплей
 * @return          тема дисплея (может бытьNULL)
 */
lv_theme_t * lv_display_get_theme(lv_display_t * disp);

/**
 * Получить время, прошедшее с момента последней активности пользователя на дисплее (например, щелчка мышью).
 * @param disp      указатель на дисплей (NULL, чтобы получить общее минимальное значение бездействия)
 * @return          прошедшие тики (миллисекунды) с момента последней активности
 */
uint32_t lv_display_get_inactive_time(const lv_display_t * disp);

/**
 * Вручную запускать действие на дисплее
 * @param disp      указатель на дисплей (NULLдля использования дисплея по умолчанию)
 */
void lv_display_trigger_activity(lv_display_t * disp);

/**
 * Временно включите и отключите инвалидацию дисплея.
 * @param disp      указатель на дисплей (NULLдля использования дисплея по умолчанию)
 * @param en        true: включить аннулирование; ложь: аннулирование
 */
void lv_display_enable_invalidation(lv_display_t * disp, bool en);

/**
 * Инвалидация отображения включена.
 * @param disp      указатель на дисплей (NULLдля использования дисплея по умолчанию)
 * @return вернуть true, если инвалидация включена
 */
bool lv_display_is_invalidation_enabled(lv_display_t * disp);

/**
 * Получите указатель на таймер обновления экрана, чтобы
 * измените его параметры с помощью функции `lv_timer_...`.
 * @param disp      указатель на дисплей
 * @return          указатель на таймер обновления дисплея. (NULLпри ошибке)
 */
lv_timer_t * lv_display_get_refr_timer(lv_display_t * disp);

/**
 * Удалить таймер обновления экрана
 * @param disp      указатель на дисплей
 */
void lv_display_delete_refr_timer(lv_display_t * disp);

/**
 * Зарегистрируйте событие vsync видеокарты.  Событие`LV_EVENT_VSYNC`будет периодически отправляться.
 * Пожалуйста, не используйте его в прослушивателях событий отображения, так как это может вызвать утечки памяти и проблемы несанкционированного доступа.
 *
 * @param disp      указатель на дисплей
 * @param event_cb      обратный вызов события
 * @param user_data     опционально user_data
 */
bool lv_display_register_vsync_event(lv_display_t * disp, lv_event_cb_t event_cb, void * user_data);

/**
 * Отменить регистрацию событий vsync дисплея.  Событие`LV_EVENT_VSYNC`не будет периодически отправляться.
 * Пожалуйста, не используйте его в прослушивателях событий отображения, так как это может вызвать утечки памяти и проблемы несанкционированного доступа.
 * @param disp      указатель на дисплей
 * @param event_cb      обратный вызов события
 * @param user_data     опционально user_data
 */
bool lv_display_unregister_vsync_event(lv_display_t * disp, lv_event_cb_t event_cb, void * user_data);

/**
 * Отправить событие vsync на дисплей
 * @param disp          указатель на дисплей
 * @param param         необязательный параметр
 * @return              LV_RESULT_OK: дисп не был удален в этом событии.
 */
lv_result_t lv_display_send_vsync_event(lv_display_t * disp, void * param);

void lv_display_set_user_data(lv_display_t * disp, void * user_data);
void lv_display_set_driver_data(lv_display_t * disp, void * driver_data);
void * lv_display_get_user_data(lv_display_t * disp);
void * lv_display_get_driver_data(lv_display_t * disp);
lv_draw_buf_t * lv_display_get_buf_active(lv_display_t * disp);

/**
 * Поворот области на месте в соответствии с поворотом дисплея
 * @param disp      указатель на дисплей
 * @param area      указатель на область для вращения
 */
void lv_display_rotate_area(lv_display_t * disp, lv_area_t * area);

/**
 * Поворот точки на месте в соответствии с поворотом дисплея
 * @param disp      указатель на дисплей
 * @param point     указатель на точку для вращения
 */
void lv_display_rotate_point(lv_display_t * disp, lv_point_t * point);

/**
 * Получить размер буферов отрисовки
 * @param disp      указатель на дисплей
 * @return          размер буфера отрисовки в байтах для корректного отображения, 0 в противном случае
 */
uint32_t lv_display_get_draw_buf_size(lv_display_t * disp);

/**
 * Получите размер недействительного буфера отрисовки. Может использоваться в обратном вызове сброса
 * чтобы получить количество байтов, используемых в текущем буфере рендеринга.
 * @param disp      указатель на дисплей
 * @param width     ширина недействительной области
 * @param height    высота недействительной области
 * @return          размер недействительного буфера отрисовки в байтах, без учета
 *                  любая предыдущая информация о палитре для корректного отображения, в противном случае 0
 */
uint32_t lv_display_get_invalidated_draw_buf_size(lv_display_t * disp, uint32_t width, uint32_t height);

/**********************
 *      MACROS
 **********************/

/*------------------------------------------------
 * Для улучшения обратной совместимости
 * Рекомендуется, только если у вас один дисплей
 *------------------------------------------------*/

#ifndef LV_HOR_RES
/**
 * Горизонтальное разрешение текущего активного дисплея.
 */
#define LV_HOR_RES lv_display_get_horizontal_resolution(lv_display_get_default())
#endif

#ifndef LV_VER_RES
/**
 * Вертикальное разрешение текущего активного дисплея.
 */
#define LV_VER_RES lv_display_get_vertical_resolution(lv_display_get_default())
#endif

/**
 * См. `lv_dpx()`и`lv_display_dpx()`.
 * То же самое иDIPдля Android. (Выбрано другое имя, чтобы избежать ошибок при вводе междуLV_DPIиLV_DIP)
 *
 * - Угол наклона 40 — это 40 пикселей на экранеDPI160 (расстояние = 1/4 дюйма).
 * - Угол наклона 40 – это 80 пикселей на экране 320DPI(расстояние по-прежнему = 1/4 дюйма).
 *
 * @sa https://stackoverflow.com/questions/2025282/what-is-the-difference-between-px-dip-dp-and-sp
 */
#define LV_DPX_CALC(dpi, n)   ((n) == 0 ? 0 :LV_MAX((( (dpi) * (n) + 80) / 160), 1)) /*+80 за округление*/
#define LV_DPX(n)   LV_DPX_CALC(lv_display_get_dpi(NULL), n)

/**
 * Для отображения по умолчанию вычисляет количество пикселей (расстояние или размер), как если бы
 * на дисплее было 160 DPI.  Это позволяет вам указать 1/160-ю долю дюйма для
 * получить реальное расстояние на дисплее, которое будет постоянным независимо от его текущего значения
 * DPI .  Например, это гарантирует, что`lv_dpx(100)`будет иметь одинаковый физический размер.
 * независимо от DPI дисплея.
 * @param n     количество единиц размером 1/160 дюйма для вычислений
 * @return      количество пикселей, которые нужно использовать, чтобы преодолеть это расстояние
 */
int32_t lv_dpx(int32_t n);

/**
 * Для указанного дисплея вычисляет количество пикселей (расстояние или размер), как если бы
 * на дисплее было 160 DPI.  Это позволяет вам указать 1/160-ю долю дюйма для
 * получить реальное расстояние на дисплее, которое будет постоянным независимо от его текущего значения
 * DPI .  Например, это гарантирует, что`lv_dpx(100)`будет иметь одинаковый физический размер.
 * независимо от DPI дисплея.
 * @param disp  указатель для отображения того, чье разрешение следует учитывать
 * @param n     количество единиц размером 1/160 дюйма для вычислений
 * @return      количество пикселей, которые нужно использовать, чтобы преодолеть это расстояние
 */
int32_t lv_display_dpx(const lv_display_t * disp, int32_t n);

#if LV_USE_EXT_DATA
/**
 * @brief Прикрепляет внешние пользовательские данные и обратный вызов деструктора к отображению.
 *
 * Связывает пользовательские данные с отображением LVGL и определяет функцию деструктора.
 * который будет автоматически вызываться при удалении дисплея для правильной очистки
 * связанные ресурсы.
 *
 * @param disp       Указатель на дисплей
 * @param data       Определяемый пользователем указатель данных для связи с дисплеем
 * @param free_cb    Функция обратного вызова для очистки данных при удалении дисплея.
 *                   Получает данные в качестве параметра.  NULL означает, что очистка не требуется.
 */
void lv_display_set_external_data(lv_display_t * disp, void * data, void (* free_cb)(void * data));
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DISPLAY_H*/
