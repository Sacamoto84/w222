file(GLOB_RECURSE UI_SOURCES "ui/*.c")

idf_component_register(
    SRCS main.c ${UI_SOURCES}
    INCLUDE_DIRS . ui
    REQUIRES
        bsp_extra
        driver
        esp_common
        esp_event
        esp_hw_support
        espressif__esp32_p4_function_ev_board
        log
        lvgl__lvgl
        nvs_flash)

idf_component_get_property(LVGL_LIB lvgl__lvgl COMPONENT_LIB)
target_compile_definitions(
    ${LVGL_LIB} 
    PRIVATE
)
