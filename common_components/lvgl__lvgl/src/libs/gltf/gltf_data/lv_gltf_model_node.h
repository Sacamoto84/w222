/**
 * @file lv_gltf_model_node.h
 *
 */

#ifndef LV_GLTF_MODEL_NODE_H
#define LV_GLTF_MODEL_NODE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_GLTF

#include "../../../misc/lv_types.h"
#include "../math/lv_3dmath.h"
#include "../../../misc/lv_event.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Получить узел модели glTF по его индексу
 *
 * @param data Указатель на структуру модели glTF
 * @param index Индекс узла для получения
 * @return Указатель на узел модели glTF или NULL, если не найден.
 */
lv_gltf_model_node_t * lv_gltf_model_node_get_by_index(lv_gltf_model_t * data, size_t index);

/**
 * @brief Получите узел модели glTF по его числовому пути.
 *
 * @param data Указатель на структуру модели glTF
 * @param num_path Числовая строка пути к извлекаемому узлу (например, ".0").
 * @return Указатель на узел модели glTF или NULL, если не найден.
 */
lv_gltf_model_node_t * lv_gltf_model_node_get_by_numeric_path(lv_gltf_model_t * data, const char * num_path);

/**
 * @brief Получить узел модели glTF по его пути
 *
 * @param data Указатель на структуру модели glTF
 * @param path Строка пути к узлу, который требуется получить.
 * @return Указатель на узел модели glTF или NULL, если не найден.
 */
lv_gltf_model_node_t * lv_gltf_model_node_get_by_path(lv_gltf_model_t * data, const char * path);

/**
 * @brief Получить путь к узлу модели glTF
 *
 * @param node Указатель на структуру узла модели glTF
 * @return Строка пути к узлу или NULL, если узел недействителен.
 */
const char * lv_gltf_model_node_get_path(lv_gltf_model_node_t * node);

/**
 * @brief ПолучитеIP(внутренний указатель/идентификатор) узла модели glTF.
 *
 * @param node Указатель на структуру узла модели glTF
 * @return СтрокаIPузла или NULL, если узел недействителен.
 */
const char * lv_gltf_model_node_get_ip(lv_gltf_model_node_t * node);

/**
 * @brief Добавьте обратный вызов события в узел модели glTF.
 *
 * @param node Указатель на структуру узла модели glTF
 * @param cb Добавляемая функция обратного вызова события. Используйте lv_event_get_param() для полученияlv_gltf_node_data_tс данными узла.
 * @param filter_list Фильтр кода события для обратного вызова
 * @param user_data Пользовательские данные для передачи в обратный вызов
 * @return Указатель на дескриптор события или NULL, если выделение не удалось.
 */
lv_event_dsc_t * lv_gltf_model_node_add_event_cb(lv_gltf_model_node_t * node, lv_event_cb_t cb,
                                                 lv_event_code_t filter_list,
                                                 void * user_data);

/**
 * @brief Добавьте обратный вызов события в узел модели glTF с включенным вычислением мирового положения. Используйте это только тогда, когда необходимо положение в мире, поскольку его вычисление — дорогостоящая операция.
 *
 * @param node Указатель на структуру узла модели glTF
 * @param cb Добавляемая функция обратного вызова события. Используйте lv_event_get_param() для полученияlv_gltf_node_data_tс данными узла.
 * @param filter_list Фильтр кода события для обратного вызова
 * @param user_data Пользовательские данные для передачи в обратный вызов
 * @return Указатель на дескриптор события или NULL, если выделение не удалось.
 */
lv_event_dsc_t * lv_gltf_model_node_add_event_cb_with_world_position(lv_gltf_model_node_t * node, lv_event_cb_t cb,
                                                                     lv_event_code_t filter_list,
                                                                     void * user_data);
/**
 * @brief Получите количество узлов в модели glTF.
 *
 * Узлы образуют иерархию графа сцены и могут содержать преобразования, сетки, камеры,
 * или другие узлы как дочерние. Они определяют пространственные отношения между объектами сцены.
 *
 * @param model Указатель на структуру данных модели glTF
 * @return Количество узлов в модели
 */
size_t lv_gltf_model_get_node_count(const lv_gltf_model_t * model);

/**
 * @brief Установите положение X узла модели glTF. Операция ставится в очередь и применяется на следующем этапе рендеринга.
 *
 * @param node Указатель на структуру узла модели glTF
 * @param x Значение позиции X
 * @return LV_RESULT_OK, если операция успешно поставлена в очередь, LV_RESULT_INVALID, если узел равен нулю или больше нет памяти для постановки операции в очередь.
 */
lv_result_t lv_gltf_model_node_set_position_x(lv_gltf_model_node_t * node, float x);

/**
 * @brief Установите положение Y узла модели glTF. Операция ставится в очередь и применяется на следующем этапе рендеринга.
 *
 * @param node Указатель на структуру узла модели glTF
 * @param y Значение позиции Y
 * @return LV_RESULT_OK, если операция успешно поставлена в очередь, LV_RESULT_INVALID, если узел равен нулю или больше нет памяти для постановки операции в очередь.
 */
lv_result_t lv_gltf_model_node_set_position_y(lv_gltf_model_node_t * node, float y);

/**
 * @brief Установите положение Z узла модели glTF. Операция ставится в очередь и применяется на следующем этапе рендеринга.
 *
 * @param node Указатель на структуру узла модели glTF
 * @param z Значение позиции Z
 * @return LV_RESULT_OK, если операция успешно поставлена в очередь, LV_RESULT_INVALID, если узел равен нулю или больше нет памяти для постановки операции в очередь.
 */
lv_result_t lv_gltf_model_node_set_position_z(lv_gltf_model_node_t * node, float z);

/**
 * @brief Установите компонент X кватерниона вращения узла модели glTF. Операция ставится в очередь и применяется на следующем этапе рендеринга.
 *
 * @param node Указатель на структуру узла модели glTF
 * @param x Значение компонента вращения X
 * @return LV_RESULT_OK, если операция успешно поставлена в очередь, LV_RESULT_INVALID, если узел равен нулю или больше нет памяти для постановки операции в очередь.
 */
lv_result_t lv_gltf_model_node_set_rotation_x(lv_gltf_model_node_t * node, float x);

/**
 * @brief Установите компонент Y кватерниона вращения узла модели glTF. Операция ставится в очередь и применяется на следующем этапе рендеринга.
 *
 * @param node Указатель на структуру узла модели glTF
 * @param y Значение компонента вращения по оси Y
 * @return LV_RESULT_OK, если операция успешно поставлена в очередь, LV_RESULT_INVALID, если узел равен нулю или больше нет памяти для постановки операции в очередь.
 */
lv_result_t lv_gltf_model_node_set_rotation_y(lv_gltf_model_node_t * node, float y);

/**
 * @brief Установите компонент Z кватерниона вращения узла модели glTF. Операция ставится в очередь и применяется на следующем этапе рендеринга.
 *
 * @param node Указатель на структуру узла модели glTF
 * @param z Значение компонента вращения Z
 * @return LV_RESULT_OK, если операция успешно поставлена в очередь, LV_RESULT_INVALID, если узел равен нулю или больше нет памяти для постановки операции в очередь.
 */
lv_result_t lv_gltf_model_node_set_rotation_z(lv_gltf_model_node_t * node, float z);

/**
 * @brief Установите масштаб X узла модели glTF. Операция ставится в очередь и применяется на следующем этапе рендеринга.
 *
 * @param node Указатель на структуру узла модели glTF
 * @param x Значение шкалы X
 * @return LV_RESULT_OK, если операция успешно поставлена в очередь, LV_RESULT_INVALID, если узел равен нулю или больше нет памяти для постановки операции в очередь.
 */
lv_result_t lv_gltf_model_node_set_scale_x(lv_gltf_model_node_t * node, float x);

/**
 * @brief Установите масштаб Y узла модели glTF. Операция ставится в очередь и применяется на следующем этапе рендеринга.
 *
 * @param node Указатель на структуру узла модели glTF
 * @param y Значение шкалы Y
 * @return LV_RESULT_OK, если операция успешно поставлена в очередь, LV_RESULT_INVALID, если узел равен нулю или больше нет памяти для постановки операции в очередь.
 */
lv_result_t lv_gltf_model_node_set_scale_y(lv_gltf_model_node_t * node, float y);

/**
 * @brief Установите масштаб Z узла модели glTF. Операция ставится в очередь и применяется на следующем этапе рендеринга.
 *
 * @param node Указатель на структуру узла модели glTF
 * @param z Значение шкалы Z
 * @return LV_RESULT_OK, если операция успешно поставлена в очередь, LV_RESULT_INVALID, если узел равен нулю или больше нет памяти для постановки операции в очередь.
 */
lv_result_t lv_gltf_model_node_set_scale_z(lv_gltf_model_node_t * node, float z);

/**
 * @brief Получите локальную позицию узла модели glTF. Должен вызываться из обратного вызова LV_EVENT_VALUE_CHANGED.
 *
 * Локальная позиция определяется относительно родительского узла.
 *
 * Эта функция действительна только при вызове из зарегистрированного обратного вызова события.
 * См. `lv_gltf_model_node_add_event_cb()`и`lv_gltf_model_node_add_event_cb_with_world_position()`.
 *
 * @param e Указатель на структуру события из обратного вызова
 * @param result Указатель на структуруlv_3dpoint_tдля хранения позиции (x, y, z)
 * @return LV_RESULT_OK в случае успеха, LV_RESULT_INVALID, если вызывается обратный вызов внешнего события или если параметры равны нулю.
 */
lv_result_t lv_gltf_model_node_get_local_position(lv_event_t * e, lv_3dpoint_t * result);

/**
 * @brief Получите мировую позицию узла модели glTF. Должен вызываться из обратного вызова LV_EVENT_VALUE_CHANGED.
 * зарегистрирован с включенной мировой позицией.
 *
 * Мировая позиция — это абсолютная позиция в глобальных координатах сцены.
 *
 * Эта функция требует, чтобы события обратного вызова регистрировались с помощью lv_gltf_model_node_add_event_cb_with_world_position().
 * поскольку он включает в себя сложные матричные вычисления, которые вычисляются по требованию.
 *
 * @param e Указатель на структуру события из обратного вызова
 * @param result Указатель на структуруlv_3dpoint_tдля хранения позиции (x, y, z)
 * @return LV_RESULT_OK в случае успеха, LV_RESULT_INVALID, если вызывается обратный вызов внешнего события, мировое положение не включено или если параметры равны нулю.
 */
lv_result_t lv_gltf_model_node_get_world_position(lv_event_t * e, lv_3dpoint_t * result);

/**
 * @brief Получите масштаб узла модели glTF. Должен вызываться из обратного вызова LV_EVENT_VALUE_CHANGED.
 *
 * Возвращает масштабные коэффициенты для каждой оси.
 *
 * Эта функция действительна только при вызове из зарегистрированного обратного вызова события.
 * См. `lv_gltf_model_node_add_event_cb()`и`lv_gltf_model_node_add_event_cb_with_world_position()`.
 *
 * @param e Указатель на структуру события из обратного вызова
 * @param result Указатель на структуруlv_3dpoint_tдля хранения масштаба (x, y, z)
 * @return LV_RESULT_OK в случае успеха, LV_RESULT_INVALID, если вызывается обратный вызов внешнего события или если параметры равны нулю.
 */
lv_result_t lv_gltf_model_node_get_scale(lv_event_t * e, lv_3dpoint_t * result);

/**
 * @brief Получите вращение Эйлера узла модели glTF. Должен вызываться из обратного вызова LV_EVENT_VALUE_CHANGED.
 *
 * Возвращает вращение в виде углов Эйлера в радианах (x, y, z).
 *
 * Эта функция действительна только при вызове из зарегистрированного обратного вызова события.
 * См. `lv_gltf_model_node_add_event_cb()`и`lv_gltf_model_node_add_event_cb_with_world_position()`.
 *
 * @param e Указатель на структуру события из обратного вызова
 * @param result Указатель на структуруlv_3dpoint_tдля хранения вращения в радианах (x, y, z)
 * @return LV_RESULT_OK в случае успеха, LV_RESULT_INVALID, если вызывается обратный вызов внешнего события или если параметры равны нулю.
 */
lv_result_t lv_gltf_model_node_get_euler_rotation(lv_event_t * e, lv_3dpoint_t * result);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GLTF*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GLTF_MODEL_NODE_H*/
