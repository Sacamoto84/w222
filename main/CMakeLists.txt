file(GLOB_RECURSE UI_SOURCES "ui/*.c")

idf_component_register(
    SRCS main.cpp battery_monitor.c ${UI_SOURCES}
    INCLUDE_DIRS . ui
    REQUIRES
        apps
        esp_adc
        esp_common
        esp_event
        esp_driver_jpeg
        esp_hw_support
        esp_netif
        esp_wifi
        espressif__esp32_p4_function_ev_board
        log
        lwip
        lvgl__lvgl
        nvs_flash)

idf_component_get_property(LVGL_LIB lvgl__lvgl COMPONENT_LIB)
target_sources(${LVGL_LIB} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/app_jpeg_image.c)
target_include_directories(${LVGL_LIB} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${LVGL_LIB} PUBLIC idf::esp_driver_jpeg)
target_compile_definitions(
    ${LVGL_LIB} 
    PRIVATE
)
