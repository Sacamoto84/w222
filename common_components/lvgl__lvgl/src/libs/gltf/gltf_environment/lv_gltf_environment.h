/**
 * @file lv_gltf_environment.h
 *
 */

#ifndef LV_GLTF_ENVIRONMENT_H
#define LV_GLTF_ENVIRONMENT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_GLTF

#include "../../../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

#define LV_GLTF_DEFAULT_CUBE_MAP_RESOLUTION 128

/**********************
 *      TYPEDEFS
 **********************/


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте сэмплер IBL для обработки изображений среды.
 * @return  указатель на созданный сэмплер илиNULLв случае неудачи
 * @note    Можно безопасно удалить после создания сред.
 */
lv_gltf_ibl_sampler_t * lv_gltf_ibl_sampler_create(void);

/**
 * Установите разрешение для каждой грани кубической карты.
 * @param   sampler к сэмплеру
 * @param   resolution каждой грани карты куба в пикселях (рекомендуется: 64–512 для встроенных)
 */
void lv_gltf_ibl_sampler_set_cube_map_pixel_resolution(lv_gltf_ibl_sampler_t * sampler, uint32_t resolution);

/**
 * Удалить сэмплер IBL
 * @param   sampler указатель на сэмплер, который нужно удалить
 */
void lv_gltf_ibl_sampler_delete(lv_gltf_ibl_sampler_t * sampler);

/**
 * Создайте среду из панорамного изображения HDR или JPEG для рендеринга IBL.
 * @param   sampler Сэмплер IBL, определяющий выходное разрешение (можно удалить после этого вызова)
 * @param   file_path путь к равноугольному изображению среды или NULL, чтобы использовать встроенное изображение по умолчанию.
 * @return  указатель на созданную среду илиNULLв случае неудачи
 *
 * @note Исходное изображение будет преобразовано вtexture_sizeсемплера.
 * @note Среда может использоваться несколькими объектами glTF.
 */
lv_gltf_environment_t * lv_gltf_environment_create(lv_gltf_ibl_sampler_t * sampler, const char * file_path);

/**
 * Установите угол поворота карты окружения
 * @param   env указатель на окружающую среду
 * @param   angle угол поворота в градусах
 */
void lv_gltf_environment_set_angle(lv_gltf_environment_t * env, float angle);

/**
 * Удаление среды
 * @param environment указатель на среду, которую нужно удалить
 */
void lv_gltf_environment_delete(lv_gltf_environment_t * environment);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GLTF*/
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GLTF_ENVIRONMENT_H*/


