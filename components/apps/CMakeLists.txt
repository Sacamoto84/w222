set(APPS_DIR ${CMAKE_CURRENT_LIST_DIR})
set(APPS_SRCS
    ${APPS_DIR}/lite_launcher.cpp
)

function(jc4880_append_app_sources out_var dir_path)
    file(GLOB_RECURSE APP_C_SRCS CONFIGURE_DEPENDS ${dir_path}/*.c)
    file(GLOB_RECURSE APP_CPP_SRCS CONFIGURE_DEPENDS ${dir_path}/*.cpp)
    list(FILTER APP_C_SRCS EXCLUDE REGEX ".*/assets/.*")
    list(FILTER APP_CPP_SRCS EXCLUDE REGEX ".*/assets/.*")
    set(${out_var} ${${out_var}} ${APP_C_SRCS} ${APP_CPP_SRCS} PARENT_SCOPE)
endfunction()

if(CONFIG_JC4880_APP_CALCULATOR)
    jc4880_append_app_sources(APPS_SRCS ${APPS_DIR}/calculator)
endif()

if(CONFIG_JC4880_APP_SETTINGS)
    jc4880_append_app_sources(APPS_SRCS ${APPS_DIR}/setting)
endif()

if(CONFIG_JC4880_APP_IMAGE_VIEWER)
    jc4880_append_app_sources(APPS_SRCS ${APPS_DIR}/image_display)
endif()

if(CONFIG_JC4880_APP_MUSIC_PLAYER)
    jc4880_append_app_sources(APPS_SRCS ${APPS_DIR}/music_player)
endif()

if(CONFIG_JC4880_APP_SIGNAL_GENERATOR)
    jc4880_append_app_sources(APPS_SRCS ${APPS_DIR}/signal_generator)
endif()

idf_component_register(
    SRCS ${APPS_SRCS}
    INCLUDE_DIRS ${APPS_DIR}
                 ${APPS_DIR}/calculator
                 ${APPS_DIR}/setting
                 ${APPS_DIR}/image_display
                 ${APPS_DIR}/music_player
                 ${APPS_DIR}/signal_generator
                 ${PROJECT_DIR}/main
        
    REQUIRES lvgl__lvgl
               esp_app_format
               esp_netif
               esp_wifi
               nvs_flash 
               heap
               fatfs
               esp_driver_i2s
               chmorgan__esp-audio-player
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
