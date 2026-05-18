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
    LV_GLTF_BG_MODE_SOLID = 0, /** Сплошной фон. Используйте `lv_obj_set_style_bg_color`, чтобы установить цветной фонарь.*/
    LV_GLTF_BG_MODE_ENVIRONMENT = 1, /** Фон окружающей среды*/
} lv_gltf_bg_mode_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * создать объект glTF
 * @param parent указатель на родительский объект
 * @return указатель на созданный объект glTF
 */
lv_obj_t * lv_gltf_create(lv_obj_t * parent);

/**
 * Назначьте объект среды glTF для рендеринга IBL.
 * @param obj указатель на объект просмотра glTF
 * @param environment указатель на среду для использования
 * @note Среда может использоваться несколькими объектами glTF.
 * @note Если перед попыткой загрузки файла не задана среда,
 *       для вас будет создан вариант по умолчанию
 */
void lv_gltf_set_environment(lv_obj_t * obj, lv_gltf_environment_t * environment);

/**
 * Загрузите модель glTF из файла в программу просмотра.
 * @param obj указатель на объект просмотра glTF
 * @param path путь к файлу модели glTF для загрузки
 * @return указатель на загруженную модель glTF илиNULLв случае сбоя
 */
lv_gltf_model_t * lv_gltf_load_model_from_file(lv_obj_t * obj, const char * path);

/**
 * Загрузите модель glTF из массива байтов в программу просмотра.
 * @param obj указатель на объект просмотра glTF
 * @param bytes необработанные данные glTF
 * @param len Длина необработанных данных glTF в байтах
 * @return указатель на загруженную модель glTF илиNULLв случае сбоя
 */
lv_gltf_model_t * lv_gltf_load_model_from_bytes(lv_obj_t * obj, const uint8_t * bytes, size_t len);

/**
 * Получите количество моделей, загруженных в средство просмотра glTF.
 * @param obj указатель на объект просмотра glTF
 * @return общее количество моделей в просмотрщике
 */
size_t lv_gltf_get_model_count(lv_obj_t * obj);

/**
 * Получить конкретную модель по ее индексу
 * @param obj указатель на объект просмотра glTF
 * @param id индекс модели для извлечения (отсчитывается от 0)
 * @return указатель на модель по указанному индексу или NULL, если индекс недействителен.
 */
lv_gltf_model_t * lv_gltf_get_model_by_index(lv_obj_t * obj, size_t id);

/**
 * Получите основную модель из средств просмотра glTF.
 * Основная модель — это первая модель, добавленная в средство просмотра, и ее можно использовать.
 * для выбора камеры и других основных операций
 * @param obj указатель на объект просмотра glTF
 * @return указатель на основную модель или NULL, если модели не загружены
 */
lv_gltf_model_t * lv_gltf_get_primary_model(lv_obj_t * obj);

/**
 * Установите рыскание (горизонтальное вращение) камеры.
 * @param obj указатель на объект просмотра glTF
 * @param yaw угол отклонения от курса в градусах
 */
void lv_gltf_set_yaw(lv_obj_t * obj, float yaw);

/**
 * Получите отклонение (горизонтальное вращение) камеры.
 * @param obj указатель на объект просмотра glTF
 * @return угол отклонения от курса в градусах
 */
float lv_gltf_get_yaw(const lv_obj_t * obj);

/**
 * Установите шаг (вертикальное вращение) камеры.
 * @param obj указатель на объект просмотра glTF
 * @param pitch угол наклона в градусах
 */
void lv_gltf_set_pitch(lv_obj_t * obj, float pitch);

/**
 * Получите шаг (вертикальное вращение) камеры.
 * @param obj указатель на объект просмотра glTF
 * @return угол наклона в градусах
 */
float lv_gltf_get_pitch(const lv_obj_t * obj);

/**
 * Установите расстояние камеры от точки фокусировки
 * @param obj указатель на объект просмотра glTF
 * @param value значение расстояния
 */
void lv_gltf_set_distance(lv_obj_t * obj, float value);

/**
 * Получите масштабный коэффициент расстояния камеры от фокусной точки.
 * @param obj указатель на объект просмотра glTF
 * @return значение коэффициента масштабирования расстояния
 */
float lv_gltf_get_distance(const lv_obj_t * obj);

/**
 * Получите расстояние камеры от фокуса в мировых единицах измерения.
 * @param obj указатель на объект просмотра GLTF
 * @return Значение расстояния в мировой единице измерения
 */
float lv_gltf_get_world_distance(const lv_obj_t * obj);

/**********************
 * Функции видового экрана
 **********************/

/**
 * Установите поле зрения
 * @param obj указатель на объект просмотра glTF
 * @param value вертикальныйFOVв градусах. Если ноль, вид будет ортогональным (неперспективным).
 */
void lv_gltf_set_fov(lv_obj_t * obj, float value);

/**
 * Получить поле зрения
 * @param obj указатель на объект просмотра glTF
 * @return вертикальныйFOVв градусах
 */
float lv_gltf_get_fov(const lv_obj_t * obj);

/**********************
 * Функции координатора
 **********************/

/**
 * Установите координату X фокуса камеры.
 * @param obj указатель на объект просмотра glTF
 * @param value координата X
 */
void lv_gltf_set_focal_x(lv_obj_t * obj, float value);

/**
 * Получите координату X фокуса камеры.
 * @param obj указатель на объект просмотра glTF
 * @return координата X
 */
float lv_gltf_get_focal_x(const lv_obj_t * obj);

/**
 * Установите координату Y фокуса камеры.
 * @param obj указатель на объект просмотра glTF
 * @param value координата Y
 */
void lv_gltf_set_focal_y(lv_obj_t * obj, float value);

/**
 * Получите координату Y фокуса камеры.
 * @param obj указатель на объект просмотра glTF
 * @return координата Y
 */
float lv_gltf_get_focal_y(const lv_obj_t * obj);

/**
 * Установите координату Z фокуса камеры.
 * @param obj указатель на объект просмотра glTF
 * @param value координата Z
 */
void lv_gltf_set_focal_z(lv_obj_t * obj, float value);

/**
 * Получите координату Z фокуса камеры.
 * @param obj указатель на объект просмотра glTF
 * @return координата Z
 */
float lv_gltf_get_focal_z(const lv_obj_t * obj);

/**
 * Установите координаты фокуса в центральную точку объекта модели.
 * @param obj указатель на объект просмотра glTF
 * @param model модель, прикрепленная к этому средству просмотра, илиNULLдля первой модели
 */
void lv_gltf_recenter(lv_obj_t * obj, lv_gltf_model_t * model);

/**********************
 * Функции управления сценой
 **********************/

/**
 * Установите индекс активной камеры
 * Камера выбирается из первой модели glTF, добавленной во вьювер.
 *
 * @param obj указатель на объект просмотра glTF
 * @param value индекс камеры (0 для камеры по умолчанию, 1+ для индекса камеры сцены)
 * @note Значения, превышающие количество камер в сцене, будут ограничены максимально доступным индексом камеры.
 */
void lv_gltf_set_camera(lv_obj_t * obj, uint32_t value);

/**
 * Получить индекс активной камеры
 * @param obj указатель на объект просмотра glTF
 * @return индекс активной камеры
 */
uint32_t lv_gltf_get_camera(const lv_obj_t * obj);

/**
 * Получите количество камер в первой модели glTF, добавленной в средство просмотра.
 * Это количество представляет собой допустимый диапазон для параметра индекса камеры.
 * используется сlv_gltf_set_camera()
 *
 * Чтобы узнать количество камер других моделей, позвоните по телефону
 * lv_gltf_model_get_camera_count (модель) непосредственно с конкретной моделью
 *
 * @param obj указатель на объект просмотра glTF
 * @return количество доступных камер
 */
uint32_t lv_gltf_get_camera_count(const lv_obj_t * obj);

/**
 * Установите соотношение скорости анимации
 *
 * Фактическое соотношение — это параметр значения / LV_GLTF_ANIM_SPEED_NORMAL
 * Значения больше LV_GLTF_ANIM_SPEED_NORMAL ускорят анимацию.
 * Значения меньше LV_GLTF_ANIM_SPEED_NORMAL замедлят анимацию.
 *
 * @param obj указатель на объект просмотра glTF
 * @param value коэффициент ускорения анимации
 */
void lv_gltf_set_animation_speed(lv_obj_t * obj, uint32_t value);

/**
 * Получить коэффициент скорости анимации
 *
 * Фактическое соотношение — это возвращаемое значение / LV_GLTF_ANIM_SPEED_NORMAL.
 *
 * @param obj указатель на объект просмотра glTF
 */
uint32_t lv_gltf_get_animation_speed(const lv_obj_t * obj);

/**********************
 * Функции визуальных настроек
 **********************/

/**
 * Установите фоновый режим
 * @param obj указатель на объект просмотра glTF
 * @param value фоновый режим
 */
void lv_gltf_set_background_mode(lv_obj_t * obj, lv_gltf_bg_mode_t value);

/**
 * Получить фоновый режим
 * @param obj указатель на объект просмотра glTF
 * @return фоновый режим
 */
lv_gltf_bg_mode_t lv_gltf_get_background_mode(const lv_obj_t * obj);

/**
 * Установите степень размытия фона
 * @param obj указатель на объект просмотра glTF
 * @param value степень размытия от 0 до 100
 */
void lv_gltf_set_background_blur(lv_obj_t * obj, uint32_t value);

/**
 * Получить степень размытия фона
 * @param obj указатель на объект просмотра glTF
 * @return степень размытия от 0 до 100
 */
uint32_t lv_gltf_get_background_blur(const lv_obj_t * obj);

/**
 * Установите яркость/мощность окружающей среды
 * @param obj указатель на объект просмотра glTF
 * @param value множитель яркости
 */
void lv_gltf_set_env_brightness(lv_obj_t * obj, uint32_t value);

/**
 * Получите окружающую яркость/мощность
 * @param obj указатель на объект просмотра glTF
 * @return множитель яркости
 */
uint32_t lv_gltf_get_env_brightness(const lv_obj_t * obj);

/**
 * Установите уровень экспозиции изображения
 * @param obj указатель на объект просмотра glTF
 * @param value уровень воздействия (по умолчанию 1,0)
 */
void lv_gltf_set_image_exposure(lv_obj_t * obj, float value);

/**
 * Получить уровень экспозиции изображения
 * @param obj указатель на объект просмотра glTF
 * @return уровень воздействия
 */
float lv_gltf_get_image_exposure(const lv_obj_t * obj);

/**********************
 * Функции рендеринга
 **********************/

/**
 * Установите режим сглаживания
 * @param obj указатель на объект просмотра glTF
 * @param value режим сглаживания
 */
void lv_gltf_set_antialiasing_mode(lv_obj_t * obj, lv_gltf_aa_mode_t value);

/**
 * Получить режим сглаживания
 * @param obj указатель на объект просмотра glTF
 * @return режим сглаживания
 */
lv_gltf_aa_mode_t lv_gltf_get_antialiasing_mode(const lv_obj_t * obj);

/***********************
 * Функции рейкастинга
 ***********************/

/**
 * Получить точку, в которой данный луч пересекается с указанной плоскостью, если таковая имеется.
 * @param ray тестовый луч пересечения
 * @param plane плоскость для проверки пересечения лучей с
 * @param collision_point выходной держатель lv_3dpoint_t, значения действительны только в том случае, если возвращаемое значение равно true
 * @return LV_RESULT_OK, если пересечение, LV_RESULT_INVALID, если пересечения нет.
 */
lv_result_t lv_intersect_ray_with_plane(const lv_3dray_t * ray, const lv_3dplane_t * plane,
                                        lv_3dpoint_t * collision_point);

/**
 * Получите плоскость, обращенную к текущей камере обзора, по центру некоторых юнитов перед ней.
 * @param obj указатель на объект просмотра GLTF
 * @param distance расстояние перед камерой для установки самолета, в мировых единицах. см. lv_gltf_get_world_distance, чтобы получить автоматическое расстояние
 * @return камера обращена к плоскости
 */
lv_3dplane_t lv_gltf_get_current_view_plane(lv_obj_t * obj, float distance);

/**
 * Вычисляет луч, исходящий из камеры и проходящий через указанное положение мыши на экране.
 * @param obj указатель на объект просмотра GLTF
 * @param screen_pos координаты экрана в пикселях
 * @return луч мыши
 */
lv_3dray_t lv_gltf_get_ray_from_2d_coordinate(lv_obj_t * obj, const lv_point_t * screen_pos);


/**
 * Получить положение на экране трехмерной точки
 * @param obj указатель на объект просмотра GLTF
 * @param world_pos мировое положение для конвертации
 * @param screen_pos результирующая точка в пикселях. допустимо только в том случае, если возвращаемое значение истинно
 * @return LV_RESULT_OK, если преобразование действительно, LV_RESULT_INVALID, если преобразование недействительно.
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
