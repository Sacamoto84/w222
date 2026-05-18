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
 * @brief Получите количество изображений в модели glTF.
 *
 * Изображения в glTF используются в качестве источников текстур и могут храниться как внешние файлы.
 * или встроить в видео модели в кодировке Base64 в файл glTF.
 *
 * @param model Указатель на структуру данных модели glTF
 * @return Количество изображений в модели
 */
size_t lv_gltf_model_get_image_count(const lv_gltf_model_t * model);

/**
 * @brief Получить количество текстур в модели glTF.
 *
 * Текстуры определяют, как изображения отбираются и применяются к материалам. Ссылки на каждую текстуру
 * изображение и может указывать параметры выборки, такие как режимы фильтрации и переноса.
 *
 * @param model Указатель на структуру данных модели glTF
 * @return Количество текстур в модели
 */
size_t lv_gltf_model_get_texture_count(const lv_gltf_model_t * model);

/**
 * @brief Получить количество материалов в модели glTF.
 *
 * Материалы определяют внешний вид примитивов сетки, включая такие свойства, как
 * базовый цвет, значения металлического цвета/шероховатости, карты нормалей и другие характеристики поверхности.
 *
 * @param model Указатель на структуру данных модели glTF
 * @return Количество материалов в модели
 */
size_t lv_gltf_model_get_material_count(const lv_gltf_model_t * model);

/**
 * @brief Получите количество камер в модели glTF.
 *
 * Камеры определяют точки обзора внутри 3D-сцены и могут быть либо перспективными, либо перспективными.
 * орфографический. Обычно они прикрепляются к узлам графа сцены.
 *
 * @param model Указатель на структуру данных модели glTF
 * @return Количество камер в модели
 */
size_t lv_gltf_model_get_camera_count(const lv_gltf_model_t * model);


/**
 * @brief Получите количество сеток в модели glTF.
 *
 * Сетки содержат геометрическую модель трехмерных объектов, включая положения вершин, нормали,
 * текстурные координаты и индексы. Каждая сетка может иметь несколько примитивов с разными материалами.
 *
 * @param model Указатель на структуру данных модели glTF
 * @return Количество сеток в модели
 */
size_t lv_gltf_model_get_mesh_count(const lv_gltf_model_t * model);

/**
 * @brief Получите количество сцен в модели glTF.
 *
 * Сцены определяют корневые узлы графа сцены. Файл glTF может сохранять несколько сцен.
 * хотя обычно только одна из них назначается в качестве сцены по умолчанию для отображения.
 *
 * @param model Указатель на структуру данных модели glTF
 * @return Количество сцен в модели
 */
size_t lv_gltf_model_get_scene_count(const lv_gltf_model_t * model);

/**
 * @brief Получить количество анимаций в модели glTF.
 *
 * Анимации определяют движение узлов сцены на основе ключевых кадров, включая преобразования.
 * такие как перевод, вращение и масштабирование с течением времени.
 *
 * @param model Указатель на структуру данных модели glTF
 * @return Количество анимаций в модели
 */
size_t lv_gltf_model_get_animation_count(const lv_gltf_model_t * model);

/**
 * @brief Выберите и начните воспроизведение анимации
 *
 * @param model Указатель на структуру модели glTF
 * @param index Номер анимации, чтобы начать играть
 * @return LV_RESULT_OK, если анимация была запущена, иначе LV_RESULT_INVALID
 */
lv_result_t lv_gltf_model_play_animation(lv_gltf_model_t * model, size_t index);

/**
 * @brief Приостановить текущую анимацию
 *
 * @param model Указатель на структуру модели glTF
 */
void lv_gltf_model_pause_animation(lv_gltf_model_t * model);

/**
 * @brief Проверьте, воспроизводится ли в данный момент анимация
 *
 * @param model Указатель на структуру модели glTF
 */
bool lv_gltf_model_is_animation_paused(lv_gltf_model_t * model);

/**
 * @brief Получить текущую выбранную анимацию. Чтобы узнать, играет ли он, см. `lv_gltf_model_is_animation_paused`.
 *
 * @param model Указатель на структуру модели glTF
 */
size_t lv_gltf_model_get_animation(lv_gltf_model_t * model);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_USE_GLTF*/
#endif /*LV_GLTF_MODEL_H*/
