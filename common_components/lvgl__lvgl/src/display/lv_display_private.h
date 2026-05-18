/**
 * @file lv_display_private.h
 *
 */

#ifndef LV_DISPLAY_PRIVATE_H
#define LV_DISPLAY_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_ext_data.h"
#include "../misc/lv_types.h"
#include "../core/lv_obj.h"
#include "../draw/lv_draw.h"
#include "lv_display.h"

#if LV_USE_SYSMON
#include "../debugging/sysmon/lv_sysmon_private.h"
#endif

/*********************
 *      DEFINES
 *********************/
#ifndef LV_INV_BUF_SIZE
#define LV_INV_BUF_SIZE 32 /**< Buffer size for invalid areas */
#endif

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_display_t {
#if LV_USE_EXT_DATA
    lv_ext_data_t ext_data;
#endif
    /*---------------------
     * Разрешение
     *--------------------*/

    /** Горизонтальное разрешение.*/
    int32_t hor_res;

    /** Вертикальное разрешение.*/
    int32_t ver_res;

    /** Горизонтальное разрешение полного/физического дисплея. Установите значение -1 для полноэкранного режима.*/
    int32_t physical_hor_res;

    /** Вертикальное разрешение полного/физического дисплея. Установите значение -1 для полноэкранного режима.*/
    int32_t physical_ver_res;

    /** Горизонтальное смещение от полного/физического дисплея. Установите значение 0 для полноэкранного режима.*/
    int32_t offset_x;

    /** Вертикальное смещение от полного/физического дисплея. Установите значение 0 для полноэкранного режима.*/
    int32_t offset_y;

    /** DPI (точек на дюйм) дисплея. Значение по умолчанию — `LV_DPI_DEF`.*/
    uint32_t dpi;

    /*---------------------
     * Буферизация
     *--------------------*/
    lv_draw_buf_t * buf_1;
    lv_draw_buf_t * buf_2;
    lv_draw_buf_t * buf_3;

    /** Внутренний, используется библиотекой*/
    lv_draw_buf_t * buf_act;

    /** MANDATORY: Запишите внутренний буфер (draw_buf) на дисплей. 'lv_display_flush_ready()' должно быть
     * позвонил, когда закончил*/
    lv_display_flush_cb_t flush_cb;

    /**
     * Ожидание готовности промывки.
     * Он может выполнять любую сложную логику ожидания, включая семафоры, мьютексы, флаги опроса и т. д.
     * Если не установлен, используется флаг `flushing`, который можно сохранить с помощью `lv_display_flush_ready()`. */
    lv_display_flush_wait_cb_t flush_wait_cb;

    /** 1: идет промывка. (Это не может быть битовое поле, потому что при его очистке из IRQ
     * Может возникнуть проблема чтения-изменения-записи) */
    volatile int flushing;

    /** 1: Это был последний кусок, который нужно было смыть. (Это не может быть битовое поле, потому что, когда оно очищено
     * из IRQ может возникнуть проблема чтения-изменения-записи) */
    volatile int flushing_last;
    volatile uint32_t last_area         : 1; /**< 1: last area is being rendered */
    volatile uint32_t last_part         : 1; /**< 1: last part of the current area is being rendered */

    lv_display_render_mode_t render_mode;
    uint32_t antialiasing : 1;       /**< 1: anti-aliasing is enabled on this display.*/
    uint32_t tile_cnt     : 8;       /**< Divide the display buffer into these number of tiles */
    uint32_t stride_is_auto : 1;     /**< 1: The stride of the buffers was not set explicitly. */


    /** 1: выполняется текущий рендеринг экрана.*/
    uint32_t rendering_in_progress : 1;

    lv_color_format_t   color_format;

    /** Недействительные (отмеченные для перерисовки) области*/
    lv_area_t inv_areas[LV_INV_BUF_SIZE];
    uint8_t inv_area_joined[LV_INV_BUF_SIZE];
    uint32_t inv_p;
    int32_t inv_en_cnt;

    /** Области синхронизации с двойным буфером (перерисовываются во время последнего обновления) */
    lv_ll_t sync_areas;

    lv_draw_buf_t _static_buf1; /**< Used when user pass in a raw buffer as display draw buffer */
    lv_draw_buf_t _static_buf2;
    /*---------------------
     * Слой
     *--------------------*/
    lv_layer_t * layer_head;
    void (*layer_init)(lv_display_t * disp, lv_layer_t * layer);
    void (*layer_deinit)(lv_display_t * disp, lv_layer_t * layer);

    /*---------------------
     * Экраны
     *--------------------*/

    /** Экраны дисплея*/
    lv_obj_t ** screens;    /**< Array of screen objects.*/
    lv_obj_t * sys_layer;   /**< @see lv_display_get_layer_sys*/
    lv_obj_t * top_layer;   /**< @see lv_display_get_layer_top*/
    lv_obj_t * act_scr;     /**< Currently active screen on this display*/
    lv_obj_t * bottom_layer;/**< @see lv_display_get_layer_bottom*/
    lv_obj_t * prev_scr;    /**< Previous screen. Used during screen animations*/
    lv_obj_t * scr_to_load; /**< The screen prepared to load in lv_screen_load_anim*/
    uint32_t screen_cnt;
    uint8_t draw_prev_over_act  : 1;/** 1: Нарисовать предыдущий экран поверх активного экрана.*/
    uint8_t del_prev  : 1;  /** 1: Автоматически удалять предыдущий экран, когда анимация загрузки экрана готова.*/

    /*---------------------
     * Другие
     *--------------------*/

    void * driver_data; /**< Custom user data*/

    void * user_data; /**< Custom user data*/

    lv_event_list_t event_list;

    uint32_t rotation  : 3; /**< Element of  lv_display_rotation_t*/

    uint32_t matrix_rotation : 1; /**< 1: Use matrix for display rotation*/

    lv_theme_t * theme;     /**< The theme assigned to the screen*/

    /** Таймер, который периодически проверяет загрязненные участки и обновляет их.*/
    lv_timer_t * refr_timer;

    /*Разные данные*/
    uint32_t last_activity_time;        /**< Last time when there was activity on this display*/

    /** Обновляемая область*/
    lv_area_t refreshed_area;
    uint32_t vsync_count;

#if LV_USE_PERF_MONITOR
    lv_obj_t * perf_label;
    lv_sysmon_backend_data_t perf_sysmon_backend;
    lv_sysmon_perf_info_t perf_sysmon_info;
#endif

#if LV_USE_MEM_MONITOR
    lv_obj_t * mem_label;
#endif

};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_DISPLAY_PRIVATE_H*/
