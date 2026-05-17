file(GLOB_RECURSE UI_SOURCES "ui/*.c")

idf_component_register(
    SRCS main.c ${UI_SOURCES}
    INCLUDE_DIRS . ui
    REQUIRES lvgl__lvgl)

idf_component_get_property(LVGL_LIB lvgl__lvgl COMPONENT_LIB)
target_compile_definitions(
    ${LVGL_LIB} 
    PRIVATE
)
