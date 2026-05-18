/**
 * @file lv_gltf.h
 *
 */

#ifndef LV_GLTF_H
#define LV_GLTF_H

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_GLTF

#include "../math/lv_3dmath.h"
#include "../../../misc/lv_types.h"
#include "../../../misc/lv_area.h"
#include "../gltf_data/lv_gltf_model.h"

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      DEFINES
 *********************/

#define LV_GLTF_ANIM_SPEED_TENTH 100
#define LV_GLTF_ANIM_SPEED_QUARTER 250
#define LV_GLTF_ANIM_SPEED_HALF 500
#define LV_GLTF_ANIM_SPEED_NORMAL 1000
#define LV_GLTF_ANIM_SPEED_2X 2000
#define LV_GLTF_ANIM_SPEED_3X 3000
#define LV_GLTF_ANIM_SPEED_4X 4000
#define LV_GLTF_DEFAULT_CAMERA 0

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_GLTF_AA_MODE_OFF = 0, /** Сглаживание выключено*/
    LV_GLTF_AA_MODE_ON = 1,  /** Сглаживание включено*/
    LV_GLTF_AA_MODE_DYNAMIC = 2, /** Сглаживание включается только тогда, когда кадр не движется.*/
} lv_gltf_aa_mode_t;

typedef enum {
    LV_GLTF_BG_MODE_SOLID = 0, /** Сплошной фон. Используйте `lv_obj_set_style_bg_color`, чтобы установить цвет фона.*/
    LV_GLTF_BG_MODE_ENVIRONMENT = 1, /** Фон окружающей среды*/
} lv_gltf_bg_mode_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте объект glTF
 * @param parent pointer to the parent object
 * @return pointer to the created glTF object
 */
lv_obj_t * lv_gltf_create(lv_obj_t * parent);

/**
 * Назначьте среду объекту glTF для рендеринга IBL.
 * @param obj pointer to a glTF viewer object
 * @param environment pointer to the environment to use
 * @note The environment can be shared across multiple glTF objects
 * @note If no environment is set before attempting to load a file,
 *       для вас будет создан вариант по умолчанию
 */
void lv_gltf_set_environment(lv_obj_t * obj, lv_gltf_environment_t * environment);

/**
 * Загрузите модель glTF из файла в программу просмотра.
 * @param obj pointer to a glTF viewer object
 * @param path file path to the glTF model to load
 * @return pointer to the loaded glTF model, or NULL on failure
 */
lv_gltf_model_t * lv_gltf_load_model_from_file(lv_obj_t * obj, const char * path);

/**
 * Загрузите модель glTF из массива байтов в программу просмотра.
 * @param obj pointer to a glTF viewer object
 * @param bytes glTF raw data
 * @param len glTF raw data length in bytes
 * @return pointer to the loaded glTF model, or NULL on failure
 */
lv_gltf_model_t * lv_gltf_load_model_from_bytes(lv_obj_t * obj, const uint8_t * bytes, size_t len);

/**
 * Получите количество моделей, загруженных в средство просмотра glTF.
 * @param obj pointer to a glTF viewer object
 * @return the total number of models in the viewer
 */
size_t lv_gltf_get_model_count(lv_obj_t * obj);

/**
 * Получить конкретную модель по ее индексу
 * @param obj pointer to a glTF viewer object
 * @param id index of the model to retrieve (0-based)
 * @return pointer to the model at the specified index, or NULL if index is invalid
 */
lv_gltf_model_t * lv_gltf_get_model_by_index(lv_obj_t * obj, size_t id);

/**
 * Получите основную модель из средства просмотра glTF.
 * Основная модель — это первая модель, добавленная в средство просмотра, и ее можно использовать.
 * для выбора камеры и других основных операций
 * @param obj pointer to a glTF viewer object
 * @return pointer to the primary model, or NULL if no models are loaded
 */
lv_gltf_model_t * lv_gltf_get_primary_model(lv_obj_t * obj);

/**
 * Установите рыскание (горизонтальное вращение) камеры.
 * @param obj pointer to a glTF viewer object
 * @param yaw yaw angle in degrees
 */
void lv_gltf_set_yaw(lv_obj_t * obj, float yaw);

/**
 * Получите отклонение (горизонтальное вращение) камеры.
 * @param obj pointer to a glTF viewer object
 * @return yaw angle in degrees
 */
float lv_gltf_get_yaw(const lv_obj_t * obj);

/**
 * Установите шаг (вертикальное вращение) камеры.
 * @param obj pointer to a glTF viewer object
 * @param pitch pitch angle in degrees
 */
void lv_gltf_set_pitch(lv_obj_t * obj, float pitch);

/**
 * Получите шаг (вертикальное вращение) камеры.
 * @param obj pointer to a glTF viewer object
 * @return pitch angle in degrees
 */
float lv_gltf_get_pitch(const lv_obj_t * obj);

/**
 * Установите расстояние камеры от точки фокусировки
 * @param obj pointer to a glTF viewer object
 * @param value distance value
 */
void lv_gltf_set_distance(lv_obj_t * obj, float value);

/**
 * Получите масштабный коэффициент расстояния камеры от фокусной точки.
 * @param obj pointer to a glTF viewer object
 * @return distance scaling factor value
 */
float lv_gltf_get_distance(const lv_obj_t * obj);

/**
 * Получите расстояние камеры от фокуса в мировых единицах измерения.
 * @param obj pointer to a GLTF viewer object
 * @return world unit distance value
 */
float lv_gltf_get_world_distance(const lv_obj_t * obj);

/**********************
 * Функции видового экрана
 **********************/

/**
 * Установите поле зрения
 * @param obj pointer to a glTF viewer object
 * @param value vertical FOV in degrees. If zero, the view will be orthographic (non-perspective)
 */
void lv_gltf_set_fov(lv_obj_t * obj, float value);

/**
 * Получить поле зрения
 * @param obj pointer to a glTF viewer object
 * @return vertical FOV in degrees
 */
float lv_gltf_get_fov(const lv_obj_t * obj);

/**********************
 * Функции координатора
 **********************/

/**
 * Установите координату X фокуса камеры.
 * @param obj pointer to a glTF viewer object
 * @param value X coordinate
 */
void lv_gltf_set_focal_x(lv_obj_t * obj, float value);

/**
 * Получите координату X фокуса камеры.
 * @param obj pointer to a glTF viewer object
 * @return X coordinate
 */
float lv_gltf_get_focal_x(const lv_obj_t * obj);

/**
 * Установите координату Y фокуса камеры.
 * @param obj pointer to a glTF viewer object
 * @param value Y coordinate
 */
void lv_gltf_set_focal_y(lv_obj_t * obj, float value);

/**
 * Получите координату Y фокуса камеры.
 * @param obj pointer to a glTF viewer object
 * @return Y coordinate
 */
float lv_gltf_get_focal_y(const lv_obj_t * obj);

/**
 * Установите координату Z фокуса камеры.
 * @param obj pointer to a glTF viewer object
 * @param value Z coordinate
 */
void lv_gltf_set_focal_z(lv_obj_t * obj, float value);

/**
 * Получите координату Z фокуса камеры.
 * @param obj pointer to a glTF viewer object
 * @return Z coordinate
 */
float lv_gltf_get_focal_z(const lv_obj_t * obj);

/**
 * Установите координаты фокуса в центральную точку объекта модели.
 * @param obj pointer to a glTF viewer object
 * @param model a model attached to this viewer or NULL for the first model
 */
void lv_gltf_recenter(lv_obj_t * obj, lv_gltf_model_t * model);

/**********************
 * Функции управления сценой
 **********************/

/**
 * Установите индекс активной камеры
 * Камера выбирается из первой модели glTF, добавленной во вьювер.
 *
 * @param obj pointer to a glTF viewer object
 * @param value camera index (0 for default camera, 1+ for scene camera index)
 * @note Values higher than the scene's camera count will be clamped to the maximum available camera index
 */
void lv_gltf_set_camera(lv_obj_t * obj, uint32_t value);

/**
 * Получить индекс активной камеры
 * @param obj pointer to a glTF viewer object
 * @return active camera index
 */
uint32_t lv_gltf_get_camera(const lv_obj_t * obj);

/**
 * Получите количество камер в первой модели glTF, добавленной в средство просмотра.
 * Это количество представляет собой допустимый диапазон для параметра индекса камеры.
 * используется с lv_gltf_set_camera ()
 *
 * Чтобы узнать количество камер других моделей, позвоните по телефону
 * lv_gltf_model_get_camera_count (модель) напрямую с конкретной моделью
 *
 * @param obj pointer to a glTF viewer object
 * @return number of available cameras
 */
uint32_t lv_gltf_get_camera_count(const lv_obj_t * obj);

/**
 * Установите соотношение скорости анимации
 *
 * Фактическое соотношение — это параметр значения / LV_GLTF_ANIM_SPEED_NORMAL
 * Значения больше LV_GLTF_ANIM_SPEED_NORMAL ускорят анимацию.
 * Значения меньше LV_GLTF_ANIM_SPEED_NORMAL замедлят анимацию.
 *
 * @param obj pointer to a glTF viewer object
 * @param value speed-up ratio of the animation
 */
void lv_gltf_set_animation_speed(lv_obj_t * obj, uint32_t value);

/**
 * Получить коэффициент скорости анимации
 *
 * Фактическое соотношение — это возвращаемое значение / LV_GLTF_ANIM_SPEED_NORMAL.
 *
 * @param obj pointer to a glTF viewer object
 */
uint32_t lv_gltf_get_animation_speed(const lv_obj_t * obj);

/**********************
 * Функции визуальных настроек
 **********************/

/**
 * Установите фоновый режим
 * @param obj pointer to a glTF viewer object
 * @param value background mode
 */
void lv_gltf_set_background_mode(lv_obj_t * obj, lv_gltf_bg_mode_t value);

/**
 * Получить фоновый режим
 * @param obj pointer to a glTF viewer object
 * @return background mode
 */
lv_gltf_bg_mode_t lv_gltf_get_background_mode(const lv_obj_t * obj);

/**
 * Установите степень размытия фона
 * @param obj pointer to a glTF viewer object
 * @param value blur amount between 0 and 100
 */
void lv_gltf_set_background_blur(lv_obj_t * obj, uint32_t value);

/**
 * Получить степень размытия фона
 * @param obj pointer to a glTF viewer object
 * @return blur amount between 0 and 100
 */
uint32_t lv_gltf_get_background_blur(const lv_obj_t * obj);

/**
 * Установите яркость/мощность окружающей среды
 * @param obj pointer to a glTF viewer object
 * @param value brightness multiplier
 */
void lv_gltf_set_env_brightness(lv_obj_t * obj, uint32_t value);

/**
 * Получите окружающую яркость/мощность
 * @param obj pointer to a glTF viewer object
 * @return brightness multiplier
 */
uint32_t lv_gltf_get_env_brightness(const lv_obj_t * obj);

/**
 * Установите уровень экспозиции изображения
 * @param obj pointer to a glTF viewer object
 * @param value exposure level (1.0 is default)
 */
void lv_gltf_set_image_exposure(lv_obj_t * obj, float value);

/**
 * Получить уровень экспозиции изображения
 * @param obj pointer to a glTF viewer object
 * @return exposure level
 */
float lv_gltf_get_image_exposure(const lv_obj_t * obj);

/**********************
 * Функции рендеринга
 **********************/

/**
 * Установите режим сглаживания
 * @param obj pointer to a glTF viewer object
 * @param value anti-aliasing mode
 */
void lv_gltf_set_antialiasing_mode(lv_obj_t * obj, lv_gltf_aa_mode_t value);

/**
 * Получить режим сглаживания
 * @param obj pointer to a glTF viewer object
 * @return anti-aliasing mode
 */
lv_gltf_aa_mode_t lv_gltf_get_antialiasing_mode(const lv_obj_t * obj);

/***********************
 * Функции рейкастинга
 ***********************/

/**
 * Получить точку, в которой данный луч пересекается с указанной плоскостью, если таковая имеется.
 * @param ray the intersection test ray
 * @param screen_y the plane to test ray intersection with
 * @param collision_point output lv_3dpoint_t holder, values are only valid if true is the return value
 * @return LV_RESULT_OK if intersection, LV_RESULT_INVALID if no intersection
 */
lv_result_t lv_intersect_ray_with_plane(const lv_3dray_t * ray, const lv_3dplane_t * plane,
                                        lv_3dpoint_t * collision_point);

/**
 * Получите плоскость, обращенную к текущей камере обзора, по центру некоторых юнитов перед ней.
 * @param obj pointer to a GLTF viewer object
 * @param distance distance in front of the camera to set the plane, in world units. see lv_gltf_get_world_distance to get the auto-distance
 * @return camera facing plane
 */
lv_3dplane_t lv_gltf_get_current_view_plane(lv_obj_t * obj, float distance);

/**
 * Вычисляет луч, исходящий из камеры и проходящий через указанное положение мыши на экране.
 * @param obj pointer to a GLTF viewer object
 * @param screen_pos screen co-ordinate, in pixels
 * @return mouse point ray
 */
lv_3dray_t lv_gltf_get_ray_from_2d_coordinate(lv_obj_t * obj, const lv_point_t * screen_pos);


/**
 * Получить положение на экране трехмерной точки
 * @param obj pointer to a GLTF viewer object
 * @param world_pos world position to convert
 * @param lv_point_t the resulting point, in pixels. only valid if return value is true
 * @return LV_RESULT_OK if conversion valid, LV_RESULT_INVALID if no valid conversion
 */
lv_result_t lv_gltf_world_to_screen(lv_obj_t * obj, const lv_3dpoint_t world_pos, lv_point_t * screen_pos);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
}
#endif

#endif /*LV_USE_GLTF*/

#endif /*LV_GLTF_H*/
