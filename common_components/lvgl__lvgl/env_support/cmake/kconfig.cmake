set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(KCONFIG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Kconfig)
set(AUTOCONF_H ${CMAKE_CURRENT_BINARY_DIR}/autoconf.h)
set(OUTPUT_DOTCONFIG ${CMAKE_CURRENT_SOURCE_DIR}/.config)
set(KCONFIG_LIST_OUT ${CMAKE_CURRENT_BINARY_DIR}/kconfig_list)
set(AUTO_CONF_DIR ${CMAKE_CURRENT_BINARY_DIR})

# Проверьте, хочет ли пользователь использовать defconfig, используя опцию -DLV_BUILD_DEFCONFIG_PATH.
if(LV_BUILD_DEFCONFIG_PATH)
    # Указанный путь может быть относительным — нормализуйте его до абсолютного.
    message(STATUS "Using defconfig: ${LV_BUILD_DEFCONFIG_PATH}")

    if (NOT IS_ABSOLUTE ${CONF_PATH})
        file(REAL_PATH ${LV_BUILD_DEFCONFIG_PATH}
            DOTCONFIG BASE_DIRECTORY ${CMAKE_SOURCE_DIR})
        message(STATUS "Converted to absolute path: ${DOTCONFIG}")

    else()
        set(DOTCONFIG ${LV_BUILD_DEFCONFIG_PATH})
    endif()

else()
    # Резервный вариант — будет предпринята попытка использовать файл .config внутри каталога LVGL.
    set(DOTCONFIG ${CMAKE_CURRENT_SOURCE_DIR}/.config)

endif()

if (NOT EXISTS ${DOTCONFIG})
    message(FATAL_ERROR "defconfig: ${DOTCONFIG} - does not exist")
endif()

execute_process(
    COMMAND ${Python_EXECUTABLE}
    ${LVGL_ROOT_DIR}/scripts/kconfig.py
    ${LVGL_ROOT_DIR}/Kconfig
    ${OUTPUT_DOTCONFIG}
    ${AUTOCONF_H}
    ${KCONFIG_LIST_OUT}
    ${DOTCONFIG}
    WORKING_DIRECTORY ${LVGL_ROOT_DIR}
    # Рабочий каталог устанавливается в каталог приложения, чтобы пользователь
    # можно использовать соответствующие пути в CONF_FILE, например.  CONF_FILE=nrf5.conf
    RESULT_VARIABLE ret
    )
if(NOT "${ret}" STREQUAL "0")
    message(FATAL_ERROR "command failed with return code: ${ret}")
endif()

# Перенастроить (перевыполнить весь кодCMakeLists.txt) при поддержкеautoconf.h.
set_target_properties(lvgl PROPERTIES CMAKE_CONFIGURE_DEPENDS ${AUTOCONF_H})

# Установите переменную, которая может использоватьсяCMakeLists.txt, включая этот файл.
set(KCONFIG_EXTERNAL_INCLUDE ${AUTOCONF_H})

# Убедитесь, что LV_BUILD_DEFCONFIG_PATH не установлен в пути, чтобы можно было вызвать его без
# - DLV_BUILD_DEFCONFIG_PATH после первой настройки и для работы с .config
unset(LV_BUILD_DEFCONFIG_PATH CACHE)
