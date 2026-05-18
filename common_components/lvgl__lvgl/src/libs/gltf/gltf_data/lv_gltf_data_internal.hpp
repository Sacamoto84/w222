#ifndef LV_GLTFDATA_HPP
#define LV_GLTFDATA_HPP

#include "../../../lv_conf_internal.h"

#if LV_USE_GLTF

#include "../gltf_view/lv_gltf.h"
#include "lv_gltf_data_internal.h"

#include "../../../misc/lv_array.h"
#include "../../../drivers/opengles/lv_opengles_private.h"

#include "../../../misc/lv_types.h"
#include "../../../misc/lv_ll.h"
#include "../../../misc/lv_event.h"

#ifdef __cplusplus

#include <fastgltf/math.hpp>
#include <string>
#include <vector>
#include <map>
#include <fastgltf/types.hpp>

// Вектор int32_t
using UintVector = std::vector<uint32_t>;
// Вектор int32_t
using IntVector = std::vector<int32_t>;
// Вектор int64_t
using LongVector = std::vector<int64_t>;
// Указатель на fastgltf::Node
using NodePtr = fastgltf::Node *;
// Стандартная матрица преобразования 4x4.
using Transform = fastgltf::math::fmat4x4;
// Пара указателя узла и int32_t
using NodeIndexPair = std::pair<NodePtr, size_t>;
// Пара чисел с плавающей запятой и пара узлов/индексов
using NodeIndexDistancePair = std::pair<float, NodeIndexPair>;
// Вектор NodeIndexPair
using NodePairVector = std::vector<NodeIndexPair>;
// Вектор NodeIndexDistancePair
using NodeDistanceVector = std::vector<NodeIndexDistancePair>;
// Картаuint32_tс NodePairVector
using MaterialIndexMap = std::map<uint32_t, NodePairVector>;
// Карта указателей узлов на преобразования
using NodeTransformMap = std::map<NodePtr, Transform>;
// Карта узлов по строке (имени)
using StringNodeMap = std::map<std::string, NodePtr>;
// Карта узлов по строке (имени)
using NodeIntMap = std::map<NodePtr, uint32_t>;
// Карта узлов по строке (имени)
using NodeVector = std::vector<NodePtr>;
// Карта индекса узла с картой индекса Prim с CenterXYZ+RadiusW Vec4
using NodePrimCenterMap = std::map<uint32_t, std::map<uint32_t, fastgltf::math::fvec4> >;

#define LV_GLTF_NODE_CHANNEL_X 0
#define LV_GLTF_NODE_CHANNEL_Y 1
#define LV_GLTF_NODE_CHANNEL_Z 2
#define LV_GLTF_NODE_CHANNEL_W 4

typedef enum {
    LV_GLTF_NODE_PROP_POSITION,
    LV_GLTF_NODE_PROP_ROTATION,
    LV_GLTF_NODE_PROP_SCALE,
} lv_gltf_node_prop_t;

typedef struct {
    lv_3dpoint_t local_position;
    lv_3dpoint_t world_position;
    lv_3dpoint_t scale;
    lv_3dpoint_t rotation;
} lv_gltf_model_node_data_t;

typedef struct {
    lv_gltf_node_prop_t prop;
    uint8_t channel;
    float value;
} lv_gltf_write_op_t;

typedef struct {
    GLuint drawsBuffer;
    std::vector<lv_gltf_primitive_t> primitives;
} lv_gltf_mesh_data_t;

typedef struct  {
    lv_event_list_t event_list;
    lv_gltf_model_node_data_t node_data;
    bool read_world_position;
    bool value_changed;
} lv_gltf_model_node_attr_t;


struct _lv_gltf_model_node_t {
    lv_gltf_model_t * model;
    const char * numeric_path;
    const char * path;
    fastgltf::Node * fastgltf_node;
    lv_gltf_model_node_attr_t * read_attrs;
    lv_array_t write_ops;
};

struct _lv_gltf_model_t {
    const char * filename;
    fastgltf::Asset asset;
    lv_array_t nodes;
    NodeVector node_by_light_index;
    NodeTransformMap node_transform_cache;
    MaterialIndexMap opaque_nodes_by_material_index;
    MaterialIndexMap blended_nodes_by_material_index;
    std::vector<size_t> validated_skins;
    std::vector<GLuint> skin_tex;
    NodePrimCenterMap local_mesh_to_center_points_by_primitive;
    lv_gltf_t * viewer;

    std::vector<lv_gltf_mesh_data_t> meshes;
    std::vector<GLuint> textures;
    lv_array_t compiled_shaders;
    std::map<fastgltf::Node *, std::vector<uint32_t> > channel_set_cache;
    fastgltf::math::fmat4x4 view_mat;
    fastgltf::math::fvec3 view_pos;
    fastgltf::math::fvec3 vertex_max;
    fastgltf::math::fvec3 vertex_min;
    fastgltf::math::fvec3 vertex_cen;

    lv_timer_t * animation_update_timer;

    size_t current_animation;
    size_t last_material_index;

    uint32_t last_camera_index;
    int32_t last_anim_num;

    float bound_radius;

    uint32_t current_animation_max_time;
    uint32_t local_timestamp;
    uint32_t last_tick;
    uint32_t camera;

    bool is_animation_enabled;
    bool last_pass_was_transmission;
    bool last_frame_was_antialiased;
    bool last_frame_no_motion;
    bool _last_frame_no_motion;
    bool write_ops_pending;
    bool write_ops_flushed;
    struct _lv_gltf_model_t * linked_view_source;
};

/**
 * @brief Получите определенную текстуру из данных модели GLTF.
 *
 * @param data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @param index Индекс текстуры, которую нужно получить.
 * @return Указатель на объект текстуры.
 */
GLuint lv_gltf_data_get_texture(lv_gltf_model_t * data, size_t index);


/**
 * @brief Получите минимальные границы (X/Y/Z) модели из данных GLTF.
 *
 * @param data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @return Указатель на трехэлементный массив с плавающей запятой, представляющий минимальные границы.
 */
fastgltf::math::fvec3 lv_gltf_data_get_bounds_min(const lv_gltf_model_t * data);

/**
 * @brief Получите максимальные границы (X/Y/Z) модели из данных GLTF.
 *
 * @param data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @return Указатель на трехэлементный массив с плавающей запятой, представляющий максимальные границы.
 */
fastgltf::math::fvec3 lv_gltf_data_get_bounds_max(const lv_gltf_model_t * data);

/**
 * @brief Получите координаты центра объекта данных GLTF.
 *
 * @param data Указатель на объект lv_gltf_data_t, из которого можно получить центр.
 * @return Указатель на массив, содержащий координаты центра (x, y, z).
 */
fastgltf::math::fvec3 lv_gltf_data_get_center(const lv_gltf_model_t * data);

/**
 * @brief Получите имя файла модели GLTF.
 *
 * @param data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @return Указатель на постоянную строку символов, представляющую имя файла.
 */
const char * lv_gltf_get_filename(const lv_gltf_model_t * data);

/**
 * @brief Проверьте, содержит ли кэш центральной точки определенную запись.
 *
 * @param data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @param index Индекс записи для проверки.
 * @param element Конкретный параметр для проверки в кеше.
 * @return True, если кеш содержит запись, в противном случае — false.
 */
bool lv_gltf_data_centerpoint_cache_contains(lv_gltf_model_t * data, size_t index, int32_t element);

/**
 * @brief Извлеките конкретный примитив из сетки.
 *
 * @param M Указатель на структуру MeshData, содержащую данные сетки.
 * @param I Индекс примитива, который требуется получить.
 * @return Указатель на примитивные данные.
 */
lv_gltf_primitive_t * lv_gltf_data_get_primitive_from_mesh(lv_gltf_mesh_data_t * M, size_t I);

/**
 * @brief Получите актив, связанный с данными модели GLTF.
 *
 * @param data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @return Указатель на данные актива.
 */
fastgltf::Asset * lv_gltf_data_get_asset(lv_gltf_model_t * data);

/**
 * @brief Получите данные сетки для определенного индекса из данных модели GLTF.
 *
 * @param data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @param index Индекс данных сетки, которые нужно получить.
 * @return Указатель на структуру MeshData, содержащую данные сетки.
 */
lv_gltf_mesh_data_t * lv_gltf_data_get_mesh(lv_gltf_model_t * data, size_t index);

/**
 * @brief Получите индекс текстуры кожи для определенной записи в данных модели GLTF.
 *
 * @param data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @param index Индекс записи, для которой нужно получить индекс текстуры кожи.
 * @return Индекс текстуры кожи.
 */
GLuint lv_gltf_data_get_skin_texture_at(lv_gltf_model_t * data, size_t index);

/**
 * @brief Проверьте, содержат ли проверенные скины определенную запись.
 *
 * @param data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @param index Индекс скина для проверки.
 * @return True, если проверенные темы оформления содержат запись, в противном случае — false.
 */
bool lv_gltf_data_validated_skins_contains(lv_gltf_model_t * data, size_t index);

/**
 * @brief Проверьте конкретный скин в данных модели GLTF.
 *
 * @param data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @param index Индекс скина для проверки.
 */
void lv_gltf_data_validate_skin(lv_gltf_model_t * data, size_t index);

/**
 * @brief Добавьте непрозрачный примитив узла к данным модели GLTF.
 *
 * @param data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @param index Индекс добавляемого примитива.
 * @param node Указатель на NodePtr, представляющий добавляемый узел.
 * @param primitive_index Конкретный параметр, связанный с примитивом.
 */
void lv_gltf_data_add_opaque_node_primitive(lv_gltf_model_t * data, size_t index, fastgltf::Node * node,
                                            size_t primitive_index);

/**
 * @brief Добавьте примитив смешанного узла к данным модели GLTF.
 *
 * @param data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @param mesh_index Индекс добавляемого примитива.
 * @param node Указатель на NodePtr, представляющий добавляемый узел.
 * @param primitive_index Конкретный параметр, связанный с примитивом.
 */
void lv_gltf_data_add_blended_node_primitive(lv_gltf_model_t * data, size_t mesh_index, fastgltf::Node * node,
                                             size_t primitive_index);

/**
 * @brief Установите кэшированную матрицу преобразования для определенного узла в данных модели GLTF.
 *
 * @param data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @param node Указатель на NodePtr, представляющий узел, для которого необходимо задать преобразование.
 * @param M Матрица преобразования для кэширования.
 */
void lv_gltf_data_set_cached_transform(lv_gltf_model_t * data, fastgltf::Node * node, fastgltf::math::fmat4x4 M);

/**
 * @brief Очистите кэш преобразования для данных модели GLTF.
 *
 * @param data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 */
void lv_gltf_data_clear_transform_cache(lv_gltf_model_t * data);

/**
 * @brief Получите кэшированную матрицу преобразования для определенного узла в данных модели GLTF.
 *
 * @param data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @param node Указатель на NodePtr, представляющий узел, для которого требуется получить преобразование.
 * @return Кэшированная матрица преобразования.
 */
fastgltf::math::fmat4x4 lv_gltf_data_get_cached_transform(lv_gltf_model_t * data, fastgltf::Node * node);

/**
 * @brief Проверьте, существует ли кэшированная матрица преобразования для данного узла.
 *
 * @param data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @param node Указатель на NodePtr, представляющий узел, для которого требуется получить преобразование.
 * @return true, если элемент кэша существует, в противном случае — false
 int32_t*/
bool lv_gltf_data_has_cached_transform(lv_gltf_model_t * data, fastgltf::Node * node);

/**
 * @brief Проверьте, пуст ли кэш преобразований.
 *
 * @param data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @return True, если кэш преобразования пуст, в противном случае — false.
 */
bool lv_gltf_data_transform_cache_is_empty(lv_gltf_model_t * data);

/**
 * @brief Получите размер скинов в данных модели GLTF.
 *
 * @param data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @return Размер шкур.
 */
size_t lv_gltf_data_get_skins_size(lv_gltf_model_t * data);

/**
 * @brief Получите конкретный скин из данных модели GLTF.
 *
 * @param data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @param index Индекс скина для получения.
 * @return Кожный индекс.
 */
size_t lv_gltf_data_get_skin(lv_gltf_model_t * data, size_t index);

/**
 * @brief Прием и обнаружение определяют для конкретного узла и примитива в данных модели GLTF.
 *
 * @param data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @param node Указатель на узел, для которого необходимо принять определения.
 * @param prim Указатель на примитив, который определяет для приема.
 */
void lv_gltf_data_injest_discover_defines(lv_gltf_model_t * data, fastgltf::Node * node, fastgltf::Primitive * prim);

/**
 * @brief Получите центральную точку определенного элемента сетки из данных модели GLTF.
 *
 * @param gltf_data Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @param matrix Матрица преобразования, применяемая при вычислении центральной точки.
 * @param mesh_index Индекс сетки, из которой извлекается центральная точка.
 * @param elem Конкретный индекс элемента внутри сетки.
 * @return Центральная точка как структура fastgltf::math::fvec3.
 */
fastgltf::math::fvec3 lv_gltf_data_get_centerpoint(lv_gltf_model_t * gltf_data, fastgltf::math::fmat4x4 matrix,
                                                   size_t mesh_index,
                                                   int32_t elem);


lv_gltf_mesh_data_t * lv_gltf_get_new_meshdata(lv_gltf_model_t * _data);

lv_gltf_model_t * lv_gltf_data_create_internal(const char * gltf_path, fastgltf::Asset);

lv_gltf_model_t * lv_gltf_data_load_internal(const void * data_source, size_t data_size,
                                             lv_opengl_shader_manager_t * shaders);

fastgltf::math::fvec4 lv_gltf_get_primitive_centerpoint(lv_gltf_model_t * data, fastgltf::Mesh & mesh,
                                                        uint32_t prim_num);

fastgltf::math::fvec3 get_cached_centerpoint(lv_gltf_model_t * data, size_t index, int32_t element,
                                             fastgltf::math::fmat4x4 matrix);

void lv_gltf_data_delete_textures(lv_gltf_model_t * data);
GLuint lv_gltf_data_create_texture(lv_gltf_model_t * data);
void lv_gltf_model_node_init(lv_gltf_model_t * model, lv_gltf_model_node_t * node, fastgltf::Node * fastgltf_node,
                             const char * path,
                             const char * num_path);

void lv_gltf_model_node_deinit(lv_gltf_model_node_t * node);

/**
 * @brief Получите данные пикселей для определенной текстуры в модели GLTF.
 *
 * @param pixels Указатель на память, в которой будут храниться данные пикселей.
 * @param data_obj Указатель на объект lv_gltf_data_t, содержащий данные модели.
 * @param model_texture_index Индекс текстуры в модели.
 * @param mipmapnum Уровень MIP-карты, для которого нужно получить пиксельные данные.
 * @param width Ширина текстуры.
 * @param height Высота текстуры.
 * @param has_alpha Флаг, указывающий, включает ли текстура альфа-канал.
 * @return True, если данные пикселей были успешно получены, в противном случае — false.
 */
bool lv_gltf_data_get_texture_pixels(void * pixels, lv_gltf_model_t * data_obj, uint32_t model_texture_index,
                                     uint32_t mipmapnum,
                                     uint32_t width, uint32_t height, bool has_alpha);

uint32_t lv_gltf_data_get_animation_total_time(lv_gltf_model_t * data, uint32_t index);
std::vector<uint32_t> * lv_gltf_data_animation_get_channel_set(std::size_t anim_num, lv_gltf_model_t * data,
                                                               fastgltf::Node * node);
void lv_gltf_data_animation_matrix_apply(float timestamp, std::size_t anim_num, lv_gltf_model_t * gltf_data,
                                         fastgltf::Node * node,
                                         fastgltf::math::fmat4x4 & matrix);

lv_gltf_model_node_t * lv_gltf_model_node_get_by_internal_node(lv_gltf_model_t * model,
                                                               const fastgltf::Node * fastgltf_node);

void lv_gltf_model_send_new_values(lv_gltf_model_t * model);

#endif


#endif /*LV_USE_GLTF*/
#endif /*LV_GLTFVIEW_H*/
