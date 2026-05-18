#ifndef LV_GLTF_MODEL_H
#define LV_GLTF_MODEL_H

#include "../../../lv_conf_internal.h"
#if LV_USE_GLTF

#include "lv_gltf_model_node.h"
#include "../../../misc/lv_types.h"
#include "../../../misc/lv_event.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Get the number of images in the glTF model
 *
 * Изображения в glTF используются в качестве источников текстур и могут храниться как внешние файлы.
 * или встроен в виде модели в кодировке Base64 в файл glTF.
 *
 * @param model Pointer to the glTF model data structure
 * @return Number of images in the model
 */
size_t lv_gltf_model_get_image_count(const lv_gltf_model_t * model);

/**
 * @brief Get the number of textures in the glTF model
 *
 * Текстуры определяют, как изображения отбираются и применяются к материалам. Ссылки на каждую текстуру
 * изображение и может указывать параметры выборки, такие как режимы фильтрации и переноса.
 *
 * @param model Pointer to the glTF model data structure
 * @return Number of textures in the model
 */
size_t lv_gltf_model_get_texture_count(const lv_gltf_model_t * model);

/**
 * @brief Get the number of materials in the glTF model
 *
 * Материалы определяют внешний вид примитивов сетки, включая такие свойства, как
 * базовый цвет, значения металлического цвета/шероховатости, карты нормалей и другие характеристики поверхности.
 *
 * @param model Pointer to the glTF model data structure
 * @return Number of materials in the model
 */
size_t lv_gltf_model_get_material_count(const lv_gltf_model_t * model);

/**
 * @brief Get the number of cameras in the glTF model
 *
 * Камеры определяют точки обзора внутри 3D-сцены и могут быть либо перспективными, либо перспективными.
 * орфографический. Обычно они прикрепляются к узлам графа сцены.
 *
 * @param model Pointer to the glTF model data structure
 * @return Number of cameras in the model
 */
size_t lv_gltf_model_get_camera_count(const lv_gltf_model_t * model);


/**
 * @brief Get the number of meshes in the glTF model
 *
 * Сетки содержат геометрическую модель трехмерных объектов, включая положения вершин, нормали,
 * текстурные координаты и индексы. Каждая сетка может иметь несколько примитивов с разными материалами.
 *
 * @param model Pointer to the glTF model data structure
 * @return Number of meshes in the model
 */
size_t lv_gltf_model_get_mesh_count(const lv_gltf_model_t * model);

/**
 * @brief Get the number of scenes in the glTF model
 *
 * Сцены определяют корневые узлы графа сцены. Файл glTF может содержать несколько сцен.
 * хотя обычно только одна из них назначается в качестве сцены по умолчанию для отображения.
 *
 * @param model Pointer to the glTF model data structure
 * @return Number of scenes in the model
 */
size_t lv_gltf_model_get_scene_count(const lv_gltf_model_t * model);

/**
 * @brief Get the number of animations in the glTF model
 *
 * Анимации определяют движение узлов сцены на основе ключевых кадров, включая преобразования.
 * такие как перевод, вращение и масштабирование с течением времени.
 *
 * @param model Pointer to the glTF model data structure
 * @return Number of animations in the model
 */
size_t lv_gltf_model_get_animation_count(const lv_gltf_model_t * model);

/**
 * @brief Select and start playing an animation
 *
 * @param model Pointer to the glTF model structure
 * @param index Animation number to start playing
 * @return LV_RESULT_OK if the animation was started else LV_RESULT_INVALID
 */
lv_result_t lv_gltf_model_play_animation(lv_gltf_model_t * model, size_t index);

/**
 * @brief Pause the current animation
 *
 * @param model Pointer to the glTF model structure
 */
void lv_gltf_model_pause_animation(lv_gltf_model_t * model);

/**
 * @brief Check if an animation is currently being played
 *
 * @param model Pointer to the glTF model structure
 */
bool lv_gltf_model_is_animation_paused(lv_gltf_model_t * model);

/**
 * @brief Get the current selected animation. To see if it's playing see `lv_gltf_model_is_animation_paused`
 *
 * @param model Pointer to the glTF model structure
 */
size_t lv_gltf_model_get_animation(lv_gltf_model_t * model);

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_USE_GLTF*/
#endif /*LV_GLTF_MODEL_H*/
