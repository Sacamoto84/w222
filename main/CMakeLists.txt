idf_component_register(
    SRCS main.c
    INCLUDE_DIRS .
    REQUIRES lvgl__lvgl)

idf_component_get_property(LVGL_LIB lvgl__lvgl COMPONENT_LIB)
target_compile_definitions(
    ${LVGL_LIB} 
    PRIVATE
        LV_LVGL_H_INCLUDE_SIMPLE
        LV_USE_DEMO_MUSIC
)
