set(APPS_DIR ${CMAKE_CURRENT_LIST_DIR})

function(jc4880_append_app_sources out_var dir_path)
    file(GLOB_RECURSE APP_C_SRCS CONFIGURE_DEPENDS ${dir_path}/*.c)
    file(GLOB_RECURSE APP_CPP_SRCS CONFIGURE_DEPENDS ${dir_path}/*.cpp)
    set(${out_var} ${${out_var}} ${APP_C_SRCS} ${APP_CPP_SRCS} PARENT_SCOPE)
endfunction()

if(CONFIG_JC4880_APP_CALCULATOR)
    jc4880_append_app_sources(APPS_SRCS ${APPS_DIR}/calculator)
endif()

idf_component_register(
    SRCS ${APPS_SRCS}
    INCLUDE_DIRS ${APPS_DIR}
                 ${APPS_DIR}/calculator
        
    REQUIRES lvgl__lvgl 
               esp_event 
               nvs_flash 
               esp-brookesia 
               bsp_extra 
               esp32_p4_function_ev_board 
               espressif__esp_lcd_touch_gt911   
               esp_partition 
               driver  
               main)

target_compile_options(
    ${COMPONENT_LIB}
    PRIVATE
    $<$<COMPILE_LANGUAGE:C>:
        -Wno-format
        -Wno-incompatible-pointer-types
        -Wno-int-conversion
    >
    $<$<COMPILE_LANGUAGE:CXX>:
        -Wno-format
        -Wno-conversion-null
        -Wno-attributes
        -Wno-write-strings
    >
    -DLV_LVGL_H_INCLUDE_SIMPLE
)
