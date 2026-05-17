/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_event.h"
#include "esp_system.h"
#include "driver/uart.h"
#include "esp_memory_utils.h"
//#include "esp_dsp.h"
#include "lvgl.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"
#include "bsp_board_extra.h"

#include "ui/ui.h"

#include "demos/lv_demos.h"

extern void ui_init(void);

static const char *TAG = "app";

static void show_startup_screen(void)
{
    lv_obj_t *screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x12343b), 0);

    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "ESP32-P4 display OK");
    lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_22, 0);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, -32);

    lv_obj_t *subtitle = lv_label_create(screen);
    lv_label_set_text_fmt(subtitle, "%dx%d  LVGL %d.%d.%d",
                          BSP_LCD_H_RES, BSP_LCD_V_RES,
                          lv_version_major(), lv_version_minor(), lv_version_patch());
    lv_obj_set_style_text_color(subtitle, lv_color_hex(0xb8e6e0), 0);
    lv_obj_align(subtitle, LV_ALIGN_CENTER, 0, 24);
}


void app_main(void)
{
    ESP_ERROR_CHECK(bsp_extra_codec_init());

    bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size =  BSP_LCD_H_RES * 100,
        .double_buffer = BSP_LCD_DRAW_BUFF_DOUBLE,
        // .hw_cfg = {
        //     .hdmi_resolution = BSP_HDMI_RES_NONE,
        //     .dsi_bus = {
        //         .phy_clk_src = 0,
        //         .lane_bit_rate_mbps = BSP_LCD_MIPI_DSI_LANE_BITRATE_MBPS,
        //     },
        // },
        .flags = {
            .buff_dma = false,
            .buff_spiram = true,
        }
    };
    lv_display_t *display = bsp_display_start_with_config(&cfg);
    ESP_ERROR_CHECK(display ? ESP_OK : ESP_FAIL);
    ESP_ERROR_CHECK(bsp_display_backlight_on());
    ESP_ERROR_CHECK(bsp_display_brightness_set(60));

    ESP_LOGI(TAG, "Display initialized: %dx%d", BSP_LCD_H_RES, BSP_LCD_V_RES);

    if (!bsp_display_lock(1000)) {
        ESP_LOGE(TAG, "LVGL lock timeout");
        return;
    }
    //show_startup_screen();
    
    ui_init();
    
    //lv_demo_benchmark(); 

    //lv_demo_stress(); 

    lv_refr_now(display);
    bsp_display_unlock();
}
