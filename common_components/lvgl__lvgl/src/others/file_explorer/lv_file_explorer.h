/**
 * @file lv_file_explorer.h
 *
 */

#ifndef LV_FILE_EXPLORER_H
#define LV_FILE_EXPLORER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#include "../../core/lv_obj.h"

#if LV_USE_FILE_EXPLORER != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_EXPLORER_SORT_NONE,
    LV_EXPLORER_SORT_KIND,
} lv_file_explorer_sort_t;

#if LV_FILE_EXPLORER_QUICK_ACCESS
typedef enum {
    LV_EXPLORER_HOME_DIR,
    LV_EXPLORER_MUSIC_DIR,
    LV_EXPLORER_PICTURES_DIR,
    LV_EXPLORER_VIDEO_DIR,
    LV_EXPLORER_DOCS_DIR,
    LV_EXPLORER_FS_DIR,
} lv_file_explorer_dir_t;
#endif

extern const lv_obj_class_t lv_file_explorer_class;

/***********************
 * GLOBAL VARIABLES
 ***********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t * lv_file_explorer_create(lv_obj_t * parent);

/*=====================
 * Функции установки
 *====================*/

#if LV_FILE_EXPLORER_QUICK_ACCESS
/**
 * Установить file_explorer
 * @param obj   указатель на объект метки
 * @param dir   каталог из перечисления lv_file_explorer_dir_t.
 * @param path   путь

 */
void lv_file_explorer_set_quick_access_path(lv_obj_t * obj, lv_file_explorer_dir_t dir, const char * path);
#endif

/**
 * Установить сортировку file_explorer
 * @param obj   указатель на объект проводника
 * @param sort  сортировка из перечисления «lv_file_explorer_sort_t».
 */
void lv_file_explorer_set_sort(lv_obj_t * obj, lv_file_explorer_sort_t sort);

/**
 * Настройте видимость кнопки «<Назад»
 * @param obj   указатель на объект проводника
 * @param show  bool true/false, кнопка включения или отключения
 */
void lv_file_explorer_show_back_button(lv_obj_t * obj, bool show);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получить файловый менеджер Выбранный файл
 * @param obj   указатель на объект проводника
 * @return      указатель на выбранное имя файла в проводнике
 */
const char * lv_file_explorer_get_selected_file_name(const lv_obj_t * obj);

/**
 * Получить путь к файловому проводнику
 * @param obj   указатель на объект проводника
 * @return      указатель на путь к проводнику файлов
 */
const char * lv_file_explorer_get_current_path(const lv_obj_t * obj);

/**
 * Получить список файлов в проводнике (lv_table)
 * @param obj   указатель на объект проводника
 * @return      указатель на таблицу файлов в проводнике obj (lv_table)
 */
lv_obj_t * lv_file_explorer_get_file_table(lv_obj_t * obj);

/**
 * Получить объект области головы в проводнике
 * @param obj   указатель на объект проводника
 * @return      указатель на область заголовка файлового проводника obj (lv_obj)
 */
lv_obj_t * lv_file_explorer_get_header(lv_obj_t * obj);

/**
 * Получить путь к файлу в проводнике obj (метка)
 * @param obj   указатель на объект проводника
 * @return      указатель на путь к файловому проводнику obj (lv_label)
 */
lv_obj_t * lv_file_explorer_get_path_label(lv_obj_t * obj);

#if LV_FILE_EXPLORER_QUICK_ACCESS
/**
 * Получить объект области головы в проводнике
 * @param obj   указатель на объект проводника
 * @return      указатель на объект области быстрого доступа файлового менеджера (lv_obj)
 */
lv_obj_t * lv_file_explorer_get_quick_access_area(lv_obj_t * obj);

/**
 * Получить список мест в проводнике (lv_list)
 * @param obj   указатель на объект проводника
 * @return      указатель на объект списка мест в проводнике (lv_list)
 */
lv_obj_t * lv_file_explorer_get_places_list(lv_obj_t * obj);

/**
 * Получить объект списка устройств в проводнике (lv_list)
 * @param obj   указатель на объект проводника
 * @return      указатель на объект списка устройств проводника (lv_list)
 */
lv_obj_t * lv_file_explorer_get_device_list(lv_obj_t * obj);
#endif

/**
 * Установить сортировку file_explorer
 * @param obj   указатель на объект проводника
 * @return текущий режим из 'lv_file_explorer_sort_t'
 */
lv_file_explorer_sort_t lv_file_explorer_get_sort(const lv_obj_t * obj);

/*=====================
 * Другие функции
 *====================*/

/**
 * Открыть указанный путь
 * @param obj   указатель на объект проводника
 * @param dir   указатель пути
 */
void lv_file_explorer_open_dir(lv_obj_t * obj, const char * dir);

/**********************
 *      MACROS
 **********************/

#endif  /*LV_USE_FILE_EXPLORER*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_FILE_EXPLORER_H*/
