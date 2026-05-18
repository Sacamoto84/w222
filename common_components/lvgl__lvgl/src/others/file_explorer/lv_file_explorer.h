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
 * @param obj   pointer to a label object
 * @param dir   the dir from 'lv_file_explorer_dir_t' enum.
 * @param path   path

 */
void lv_file_explorer_set_quick_access_path(lv_obj_t * obj, lv_file_explorer_dir_t dir, const char * path);
#endif

/**
 * Установить сортировку file_explorer
 * @param obj   pointer to a file explorer object
 * @param sort  the sort from 'lv_file_explorer_sort_t' enum.
 */
void lv_file_explorer_set_sort(lv_obj_t * obj, lv_file_explorer_sort_t sort);

/**
 * Настройте видимость кнопки «<Назад»
 * @param obj   pointer to a file explorer object
 * @param show  bool true/false, enable or disable button
 */
void lv_file_explorer_show_back_button(lv_obj_t * obj, bool show);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получить файловый менеджер Выбранный файл
 * @param obj   pointer to a file explorer object
 * @return      pointer to the file explorer selected file name
 */
const char * lv_file_explorer_get_selected_file_name(const lv_obj_t * obj);

/**
 * Получить путь к файловому проводнику
 * @param obj   pointer to a file explorer object
 * @return      pointer to the file explorer cur path
 */
const char * lv_file_explorer_get_current_path(const lv_obj_t * obj);

/**
 * Получить список файлов в проводнике (lv_table)
 * @param obj   pointer to a file explorer object
 * @return      pointer to the file explorer file table obj(lv_table)
 */
lv_obj_t * lv_file_explorer_get_file_table(lv_obj_t * obj);

/**
 * Получить объект области головы в проводнике
 * @param obj   pointer to a file explorer object
 * @return      pointer to the file explorer head area obj(lv_obj)
 */
lv_obj_t * lv_file_explorer_get_header(lv_obj_t * obj);

/**
 * Получить путь к файлу в проводнике obj (метка)
 * @param obj   pointer to a file explorer object
 * @return      pointer to the file explorer path obj(lv_label)
 */
lv_obj_t * lv_file_explorer_get_path_label(lv_obj_t * obj);

#if LV_FILE_EXPLORER_QUICK_ACCESS
/**
 * Получить объект области головы в проводнике
 * @param obj   pointer to a file explorer object
 * @return      pointer to the file explorer quick access area obj(lv_obj)
 */
lv_obj_t * lv_file_explorer_get_quick_access_area(lv_obj_t * obj);

/**
 * Получить объект списка мест в проводнике (lv_list)
 * @param obj   pointer to a file explorer object
 * @return      pointer to the file explorer places list obj(lv_list)
 */
lv_obj_t * lv_file_explorer_get_places_list(lv_obj_t * obj);

/**
 * Получить объект списка устройств в проводнике (lv_list)
 * @param obj   pointer to a file explorer object
 * @return      pointer to the file explorer device list obj(lv_list)
 */
lv_obj_t * lv_file_explorer_get_device_list(lv_obj_t * obj);
#endif

/**
 * Установить сортировку file_explorer
 * @param obj   pointer to a file explorer object
 * @return the current mode from 'lv_file_explorer_sort_t'
 */
lv_file_explorer_sort_t lv_file_explorer_get_sort(const lv_obj_t * obj);

/*=====================
 * Другие функции
 *====================*/

/**
 * Открыть указанный путь
 * @param obj   pointer to a file explorer object
 * @param dir   pointer to the path
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
