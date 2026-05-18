include("${CMAKE_CURRENT_LIST_DIR}/version.cmake")

file(GLOB_RECURSE SOURCES ${LVGL_ROOT_DIR}/src/*.c)
file(GLOB_RECURSE EXAMPLE_SOURCES ${LVGL_ROOT_DIR}/examples/*.c)

# С помощью micropython создайте lvgl как интерфейс интерфейса, цепочку ссылок:
# lvgl_interface [lvgl] →usermod_lvgl_bindings[lv_bindings] → usermod
# [микропитон] → прошивка [микропитон]
add_library(lvgl_interface INTERFACE)
# ${SOURCES} по какой-то причине должен быть переданNOTнепосредственноadd_library(не будет
# построен)
target_sources(lvgl_interface INTERFACE ${SOURCES})
# MicroPython собирается с -Werror; нам нужно подавить некоторые замечания, например:
#
# /home/test/build/lv_micropython/ports/rp2/build-PICO/lv_mp.c:29316:16: ошибка:
# Определение 'lv_style_transition_dsc_t_path_xcb_callback', но не используется
# [-Werror=неиспользуемая функция] 29316 |  STATICint32_t
# lv_style_transition_dsc_t_path_xcb_callback (constlv_anim_t* arg0) |
# ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
target_compile_options(lvgl_interface INTERFACE -Wno-unused-function)
