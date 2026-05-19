file(GLOB_RECURSE UI_SOURCES "ui/*.c")

idf_component_register(
    SRCS main.c ftp_server.c c6_slave_ota.c ${UI_SOURCES}
    INCLUDE_DIRS . ui
    REQUIRES
        bsp_extra
        driver
        esp_common
        esp_event
        esp_hw_support
        esp_netif
        esp_wifi
        fatfs
        espressif__esp32_p4_function_ev_board
        log
        lwip
        lvgl__lvgl
        nvs_flash
        sdmmc)

idf_component_get_property(LVGL_LIB lvgl__lvgl COMPONENT_LIB)
target_compile_definitions(
    ${LVGL_LIB} 
    PRIVATE
)
