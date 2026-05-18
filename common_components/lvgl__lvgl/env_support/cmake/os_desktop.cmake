include("${CMAKE_CURRENT_LIST_DIR}/version.cmake")

############################## OPTIONS ######################################

set(LV_BUILD_CONF_PATH "" CACHE PATH
    "Use this to specify the location of and/or filename of lv_conf.h")

set(LV_BUILD_CONF_DIR "" CACHE PATH
    "Can be used to specify the include dir containing lv_conf.h, to be used in conjunction with LV_CONF_INCLUDE_SIMPLE")

option(LV_BUILD_USE_KCONFIG "Use Kconfig" OFF)
set(LV_BUILD_DEFCONFIG_PATH "" CACHE PATH
    "Supply the default Kconfig configuration - used with Kconfig")

option(LV_BUILD_SET_CONFIG_OPTS
    "Create variables from the definitions in lv_conf_internal.h"  OFF)

option(LV_BUILD_LVGL_H_SIMPLE_INCLUDE
    "Disable if the lvgl directory is located at the top-level of your project" ON)

option(LV_BUILD_LVGL_H_SYSTEM_INCLUDE
    "Enable if LVGL will be installed on the system or the build system of your application uses a sysroot" OFF)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

### В опциях конфигурации LVGL всегда используется префикс CONFIG_.
### Их можно установить с помощью аргумента -D или cmake-gui(1) и сохранить в кеше.
### Имена опций такие же, как вlv_conf.h, поэтому проверьте их описание.

### INFO: когда LV_BUILD_SET_CONFIG_OPTS включен — эти параметры устанавливаются автоматически.
### на основеlv_conf.hили Kconfig

option(CONFIG_LV_BUILD_DEMOS "Build demos" ON)
option(CONFIG_LV_BUILD_EXAMPLES "Build examples" ON)
option(CONFIG_LV_USE_THORVG_INTERNAL "Use the internal version of ThorVG" ON)
option(CONFIG_LV_USE_PRIVATE_API "If set - install the private headers" OFF)

############################## BUILD ######################################

if (LV_BUILD_CONF_PATH AND LV_BUILD_CONF_DIR)
    message(FATAL_ERROR "can not use LV_BUILD_CONF_DIR and LV_BUILD_CONF_PATH at the same time")
endif()

if (LV_BUILD_USE_KCONFIG)
    # Если используется Kconfig, тоPCPP.
    set(LV_BUILD_SET_CONFIG_OPTS ON)
endif()

if (LV_BUILD_SET_CONFIG_OPTS)
    # Python требуется, когдаLV_BUILD_SET_CONFIG_OPTSвключен.
    # PCPP используется для предварительной обработкиlv_conf_internal.hи позволяет избежать установки
    # Опции CONFIG_LV_USE или CONFIG_LV_BUILD вручную.
    find_package(Python REQUIRED)
endif()

# Определите, собран ли LVGL из каталога верхнего уровня.
get_directory_property(HAS_PARENT_SCOPE PARENT_DIRECTORY)


# Установить источники, используемые для компонентов LVGL
file(GLOB_RECURSE SOURCES ${LVGL_ROOT_DIR}/src/*.c
                          ${LVGL_ROOT_DIR}/src/*.cpp
                          ${LVGL_ROOT_DIR}/src/*.S)
file(GLOB_RECURSE EXAMPLE_SOURCES ${LVGL_ROOT_DIR}/examples/*.c)
file(GLOB_RECURSE DEMO_SOURCES ${LVGL_ROOT_DIR}/demos/*.c)
file(GLOB_RECURSE THORVG_SOURCES ${LVGL_ROOT_DIR}/src/libs/thorvg/*.cpp
                                 ${LVGL_ROOT_DIR}/src/others/vg_lite_tvg/*.cpp)

# Проверьте случай, когда LVGL распространяется только с исходным кодом.
if(NOT EXAMPLE_SOURCES AND CONFIG_LV_BUILD_EXAMPLES)
    message(STATUS "No Examples found. Disabling Examples build")
    set(CONFIG_LV_BUILD_EXAMPLES OFF)
endif()

if(NOT DEMO_SOURCES AND CONFIG_LV_BUILD_DEMOS)
    message(STATUS "No Demos found. Disabling Demos build")
    set(CONFIG_LV_BUILD_DEMOS OFF)
endif()

# Сборка библиотеки LVGL.
add_library(lvgl ${SOURCES})
add_library(lvgl::lvgl ALIAS lvgl)

if (NOT LV_BUILD_USE_KCONFIG)

    # По умолчанию — воспользуйтесь файлом конфигурацииlv_conf.h.
    target_compile_definitions(lvgl PUBLIC LV_KCONFIG_IGNORE)

    message(STATUS ${LV_BUILD_CONF_PATH})

    if (LV_BUILD_CONF_PATH)

        set(CONF_PATH ${LV_BUILD_CONF_PATH})
        message(STATUS "Using configuration: ${CONF_PATH}")

        if (NOT IS_ABSOLUTE ${CONF_PATH})
            file(REAL_PATH ${CONF_PATH} CONF_PATH BASE_DIRECTORY ${CMAKE_SOURCE_DIR})
            message(STATUS "Converted to absolute path: ${CONF_PATH}")
        endif()

        get_filename_component(CONF_INC_DIR ${CONF_PATH} DIRECTORY)

        target_compile_definitions(lvgl PUBLIC LV_CONF_PATH="${CONF_PATH}")

    elseif(LV_BUILD_CONF_DIR)

        message(STATUS "Using lv_conf.h from the user-supplied configuration directory: ${LV_BUILD_CONF_DIR}")

        set(CONF_INC_DIR ${LV_BUILD_CONF_DIR})

        list(APPEND LVGL_PUBLIC_HEADERS ${CONF_INC_DIR}/lv_conf.h)

        # Избегайте ошибок компиляции из-за относительного пути к каталогу включения.
        if (NOT IS_ABSOLUTE ${CONF_INC_DIR})
            file(REAL_PATH ${CONF_INC_DIR} CONF_INC_DIR BASE_DIRECTORY ${CMAKE_SOURCE_DIR})
            message(STATUS "Converted to absolute path: ${CONF_INC_DIR}")
        endif()

        set(CONF_PATH ${CONF_INC_DIR}/lv_conf.h)

        target_compile_definitions(lvgl PUBLIC LV_CONF_INCLUDE_SIMPLE)

    else()

        message(STATUS "Using lv_conf.h from the top-level project directory")

        list(APPEND LVGL_PUBLIC_HEADERS ${CMAKE_SOURCE_DIR}/lv_conf.h)

        set(CONF_INC_DIR ${CMAKE_SOURCE_DIR})
        set(CONF_PATH ${CONF_INC_DIR}/lv_conf.h)

        target_compile_definitions(lvgl PUBLIC LV_CONF_INCLUDE_SIMPLE)

    endif()

    # Проверьте, существует лиlv_conf.hв настроенном месте.
    if (NOT EXISTS ${CONF_PATH})
        message(FATAL_ERROR "Configuration file: ${CONF_PATH} - not found")
    endif()

else()

    # Используйте кконфиг
    # kconfig.cmake сгенерирует .config
    # и autoconf.h, который будет использоваться lv_conf_kconfig.h.
    include(${CMAKE_CURRENT_LIST_DIR}/kconfig.cmake)

    # Установите флаг, указывающий, что мы используем kconfig, и принимаем решение
    # Скриптgenerate_cmake_variables.py.
    set(GEN_VARS_KCONFIG_MODE_FLAG --kconfig)

    # При использовании Kconfig нам необходимо определить дополнительное определение.
    target_compile_definitions(lvgl PUBLIC
        "LV_CONF_SKIP"
        "LV_CONF_KCONFIG_EXTERNAL_INCLUDE=\"${KCONFIG_EXTERNAL_INCLUDE}\"")

endif()

if (LV_BUILD_LVGL_H_SYSTEM_INCLUDE)
    target_compile_definitions(lvgl PUBLIC LV_LVGL_H_INCLUDE_SYSTEM)
elseif(LV_BUILD_LVGL_H_SIMPLE_INCLUDE)
    target_compile_definitions(lvgl PUBLIC LV_LVGL_H_INCLUDE_SIMPLE)
endif()


if (LV_BUILD_SET_CONFIG_OPTS)
    # Используйте портативный компьютер для предварительной обработки lv_conf_internal.h.

    get_target_property(CONF_DEFINES lvgl COMPILE_DEFINITIONS)

    execute_process(
        COMMAND ${Python_EXECUTABLE} ${LVGL_ROOT_DIR}/scripts/preprocess_lv_conf_internal.py
        --input ${LVGL_ROOT_DIR}/src/lv_conf_internal.h
        --tmp_file ${CMAKE_CURRENT_BINARY_DIR}/tmp.h
        --output ${CMAKE_CURRENT_BINARY_DIR}/lv_conf_expanded.h
        --workfolder ${CMAKE_CURRENT_BINARY_DIR}
        --defs ${CONF_DEFINES}
        --include ${LVGL_ROOT_DIR} ${CMAKE_SOURCE_DIR} ${LVGL_ROOT_DIR}/src ${CONF_INC_DIR}
        RESULT_VARIABLE ret
    )
    if(NOT "${ret}" STREQUAL "0")
        message(FATAL_ERROR "preprocess_lv_conf_internal.py failed with return code: ${ret}")
    endif()

    if(HAS_PARENT_SCOPE)
        # Также установите конфигурацию CONFIG_LV_USE_* и CONFIG_LV_BUILD_*.
        # в родительской области
        set(PARENT_SCOPE_ARG "--parentscope")
    else()
        set(PARENT_SCOPE_ARG "")
    endif()

    # Преобразуйте расширенныйlv_conf_expanded.hв переменные cmake.
    execute_process(
        COMMAND ${Python_EXECUTABLE}
        ${LVGL_ROOT_DIR}/scripts/generate_cmake_variables.py
        --input ${CMAKE_CURRENT_BINARY_DIR}/lv_conf_expanded.h
        --output ${CMAKE_CURRENT_BINARY_DIR}/lv_conf.cmake
        ${PARENT_SCOPE_ARG}
        ${GEN_VARS_KCONFIG_MODE_FLAG}
        RESULT_VARIABLE ret
    )

    if(NOT "${ret}" STREQUAL "0")
        message(FATAL_ERROR "generate_cmake_variables.py command failed with return code: ${ret}")
    endif()

    # Это устанавливает все переменныеCONFIG_LV_USE_* илиCONFIG_LV_BUILD_* в cmake.
    include(${CMAKE_CURRENT_BINARY_DIR}/lv_conf.cmake)
endif()

# Установите конфигурацию каталога для всех целей, созданных в этомCMakeLists.txt.
# CMAKE_CURRENT_SOURCE_DIR необходим, потому что в активах есть lvgl/lvgl.h...
include_directories(${CONF_INC_DIR} ${LVGL_ROOT_DIR})

target_include_directories(lvgl SYSTEM PUBLIC ${LVGL_ROOT_DIR} ${CONF_INC_DIR} ${CMAKE_CURRENT_BINARY_DIR})

# Распространить определения компилятора, установленные в LVGL, на остальные цели.
# упомянуто в этом файле
get_target_property(COMP_DEF lvgl COMPILE_DEFINITIONS)

if(CONFIG_LV_USE_THORVG_INTERNAL)
    message(STATUS "Enabling the building of ThorVG internal")

    add_library(lvgl_thorvg ${THORVG_SOURCES})
    add_library(lvgl::thorvg ALIAS lvgl_thorvg)
    target_include_directories(lvgl_thorvg PRIVATE ${LVGL_ROOT_DIR}/src/libs/thorvg)
    set_target_properties(lvgl_thorvg PROPERTIES COMPILE_DEFINITIONS "${COMP_DEF}")

    # Это говорит cmake связать lvgl сlvgl_thorvg.
    # Компоновщик разрешит все зависимости при динамическом связывании. 
    target_link_libraries(lvgl PRIVATE lvgl_thorvg)
    # Во время статического соединения нам необходимо создать циклическую зависимость, поскольку thorvg также нуждается в уровне lvgl.
    if (NOT BUILD_SHARED_LIBS)
        target_link_libraries(lvgl_thorvg PRIVATE lvgl)
    endif()
endif()

if(NOT (CMAKE_C_COMPILER_ID STREQUAL "MSVC"))
    set_source_files_properties(${LVGL_ROOT_DIR}/src/others/vg_lite_tvg/vg_lite_tvg.cpp
        PROPERTIES COMPILE_FLAGS -Wunused-parameter)
endif()

# Создайте библиотеку примеров LVGL.
if(CONFIG_LV_BUILD_EXAMPLES)

    message(STATUS "Enabling the building of examples")

    add_library(lvgl_examples ${EXAMPLE_SOURCES})
    add_library(lvgl::examples ALIAS lvgl_examples)
    target_include_directories(lvgl_examples SYSTEM PUBLIC ${LVGL_ROOT_DIR}/examples)
    set_target_properties(lvgl_examples PROPERTIES COMPILE_DEFINITIONS "${COMP_DEF}")

    # Это говорит cmake связать lvgl сlvgl_examples.
    # PUBLIC позволяет связывать код с LVGL, чтобы также использовать библиотеку.
    # Компоновщик разрешит все зависимости при динамическом связывании. 
    target_link_libraries(lvgl PUBLIC lvgl_examples)

    # Во время статического соединения нам необходимо создать циклическую зависимость, поскольку в примерах также требуется lvgl.
    if (NOT BUILD_SHARED_LIBS)
        target_link_libraries(lvgl_examples PRIVATE lvgl)
    endif()

endif()

# Создайте демонстрационную библиотеку LVGL.
if(CONFIG_LV_BUILD_DEMOS)

    message(STATUS "Enabling the building of demos")

    add_library(lvgl_demos ${DEMO_SOURCES})
    add_library(lvgl::demos ALIAS lvgl_demos)
    target_include_directories(lvgl_demos SYSTEM PUBLIC ${LVGL_ROOT_DIR}/demos)
    set_target_properties(lvgl_demos PROPERTIES COMPILE_DEFINITIONS "${COMP_DEF}")

    # Это говорит cmake связать lvgl сlvgl_examples.
    # PUBLIC позволяет связывать код с LVGL, чтобы также использовать библиотеку.
    # Компоновщик разрешит все зависимости при динамическом связывании. 
    target_link_libraries(lvgl PUBLIC lvgl_demos)

    # Во время статического соединения нам необходимо создать циклическую зависимость, поскольку для демо-версии также требуется lvgl.
    if (NOT BUILD_SHARED_LIBS)
        # Если статическая привязка - демки изменения от шрифтов, определенные в lvgl
        # Во время динамического связывания компоновщик может разрешить все
        target_link_libraries(lvgl_demos PRIVATE lvgl)
    endif()

endif()

############################## INSTALLATION ######################################

# Библиотеку и заголовки можно установить в систему с помощью команды make install.
file(GLOB LVGL_PUBLIC_HEADERS
    "${LVGL_ROOT_DIR}/lvgl.h"
    "${CONF_INC_DIR}/*.h"
    "${LVGL_ROOT_DIR}/lv_version.h")

if (CONFIG_LV_USE_PRIVATE_API)
    list(APPEND LVGL_PUBLIC_HEADERS "${LVGL_ROOT_DIR}/lvgl_private.h")
endif()

if("${LIB_INSTALL_DIR}" STREQUAL "")
    set(LIB_INSTALL_DIR "lib")
endif()
if("${RUNTIME_INSTALL_DIR}" STREQUAL "")
    set(RUNTIME_INSTALL_DIR "bin")
endif()
if("${INC_INSTALL_DIR}" STREQUAL "")
    set(INC_INSTALL_DIR "include/lvgl")
endif()

#Установить общедоступные заголовки
install(
    DIRECTORY "${LVGL_ROOT_DIR}/src"
    DESTINATION "${INC_INSTALL_DIR}"
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*_private.h" EXCLUDE)

if (CONFIG_LV_USE_PRIVATE_API)
    # Установите приватные заголовки — только при необходимости
    install(
        DIRECTORY "${LVGL_ROOT_DIR}/src"
        DESTINATION "${INC_INSTALL_DIR}"
        FILES_MATCHING
        PATTERN "*_private.h")
endif()


if (LV_BUILD_USE_KCONFIG)
    # При использовании KConfig скопируйте расширенный заголовок конфигурации.
    # и переименуйте его в lv_conf.h
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lv_conf_expanded.h
            DESTINATION ${INC_INSTALL_DIR}
            RENAME lv_conf.h)
endif()

install(
    FILES ${LVGL_PUBLIC_HEADERS}
    DESTINATION "${INC_INSTALL_DIR}/")

# Установить библиотеку
set_target_properties(
    lvgl
    PROPERTIES OUTPUT_NAME lvgl
    VERSION ${LVGL_VERSION}
    SOVERSION ${LVGL_SOVERSION}
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
    PUBLIC_HEADER "${LVGL_PUBLIC_HEADERS}")

install(
    TARGETS lvgl
    ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
    LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
    RUNTIME DESTINATION "${RUNTIME_INSTALL_DIR}"
    PUBLIC_HEADER DESTINATION "${INC_INSTALL_DIR}")

# TODO: еслиLVGLсвязан с чем-то другим, например с Freetype, файл Pkgconfig должен содержать -lfreetype2
configure_file("${LVGL_ROOT_DIR}/lvgl.pc.in" ${CMAKE_CURRENT_BINARY_DIR}/lvgl.pc @ONLY)
configure_file("${LVGL_ROOT_DIR}/lv_version.h.in" ${CMAKE_CURRENT_BINARY_DIR}/lv_version.h @ONLY)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/lvgl.pc"
    DESTINATION "share/pkgconfig/")

# Установить контент
if(CONFIG_LV_USE_THORVG_INTERNAL)

    set_target_properties(
        lvgl_thorvg
        PROPERTIES OUTPUT_NAME lvgl_thorvg
        VERSION ${LVGL_VERSION}
        SOVERSION ${LVGL_SOVERSION}
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
        PUBLIC_HEADER "${LVGL_PUBLIC_HEADERS}")

    install(
        TARGETS lvgl_thorvg
        ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
        LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
        RUNTIME DESTINATION "${RUNTIME_INSTALL_DIR}"
        PUBLIC_HEADER DESTINATION "${INC_INSTALL_DIR}")

endif()

if(CONFIG_LV_BUILD_DEMOS)

    install(
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/demos"
        DESTINATION "${INC_INSTALL_DIR}"
        FILES_MATCHING
        PATTERN "*.h")

    set_target_properties(
        lvgl_demos
        PROPERTIES OUTPUT_NAME lvgl_demos
        VERSION ${LVGL_VERSION}
        SOVERSION ${LVGL_SOVERSION}
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
        PUBLIC_HEADER "${LVGL_PUBLIC_HEADERS}")

    install(
        TARGETS lvgl_demos
        ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
        LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
        RUNTIME DESTINATION "${RUNTIME_INSTALL_DIR}"
        PUBLIC_HEADER DESTINATION "${INC_INSTALL_DIR}")

endif()

if(CONFIG_LV_BUILD_EXAMPLES)

    install(
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
        DESTINATION "${INC_INSTALL_DIR}"
        FILES_MATCHING
        PATTERN "*.h")

    set_target_properties(
        lvgl_examples
        PROPERTIES OUTPUT_NAME lvgl_examples
        VERSION ${LVGL_VERSION}
        SOVERSION ${LVGL_SOVERSION}
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
        PUBLIC_HEADER "${LVGL_PUBLIC_HEADERS}")

    install(
        TARGETS lvgl_examples
        ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
        LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
        RUNTIME DESTINATION "${RUNTIME_INSTALL_DIR}"
        PUBLIC_HEADER DESTINATION "${INC_INSTALL_DIR}")

endif()

############################## SET GLOBAL VARIABLES ######################################

if (HAS_PARENT_SCOPE)
    # Установите каталог включения конфигурации в проекте верхнего уровня.
    # Полезно для того, чтобы его не нужно было определять второй раз в самом проекте.
    set(LVGL_CONF_INC_DIR ${CONF_INC_DIR} PARENT_SCOPE)
    set(LVGL_CONF_PATH ${CONF_PATH} PARENT_SCOPE)
    set(LVGL_COMPILER_DEFINES ${COMP_DEF} PARENT_SCOPE)

else()
    # Файл был включен в тесты/CMakeLists.txt-
    # или где-то еще, установите переменную, содержащую нормализованный путь к включаемому каталогу
    # содержащий файл конфигурации
    set(LVGL_CONF_INC_DIR ${CONF_INC_DIR})
    set(LVGL_CONF_PATH ${CONF_PATH})
    set(LVGL_COMPILER_DEFINES ${COMP_DEF})
endif()
