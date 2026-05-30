set(APPS_DIR ${CMAKE_CURRENT_LIST_DIR})
set(APPS_SRCS
    ${APPS_DIR}/lite_launcher.cpp
)

function(jc4880_append_app_sources out_var dir_path)
    file(GLOB_RECURSE APP_C_SRCS CONFIGURE_DEPENDS ${dir_path}/*.c)
    file(GLOB_RECURSE APP_CPP_SRCS CONFIGURE_DEPENDS ${dir_path}/*.cpp)
    list(FILTER APP_C_SRCS EXCLUDE REGEX ".*/assets/.*")
    list(FILTER APP_CPP_SRCS EXCLUDE REGEX ".*/assets/.*")
    # Хостовые юнит-тесты (имеют свой main()) не должны попадать в прошивку.
    list(FILTER APP_CPP_SRCS EXCLUDE REGEX ".*_host\\.cpp$")
    set(${out_var} ${${out_var}} ${APP_C_SRCS} ${APP_CPP_SRCS} PARENT_SCOPE)
endfunction()

if(CONFIG_JC4880_APP_SETTINGS)
    jc4880_append_app_sources(APPS_SRCS ${APPS_DIR}/setting)
endif()

if(CONFIG_JC4880_APP_UART_TERMINAL)
    jc4880_append_app_sources(APPS_SRCS ${APPS_DIR}/terminal)
endif()

idf_component_register(
    SRCS ${APPS_SRCS}
    INCLUDE_DIRS ${APPS_DIR}
                 ${APPS_DIR}/setting
                 ${APPS_DIR}/terminal
                 ${PROJECT_DIR}/main
        
    REQUIRES lvgl__lvgl
               esp_app_format
               esp_netif
               esp_wifi
               esp_driver_gpio
               esp_driver_uart
               freertos
               nvs_flash 
               heap
               log)

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
