#ifndef LV_GLTFDATA_PRIVATE_H
#define LV_GLTFDATA_PRIVATE_H

#include "../../../lv_conf_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LV_USE_GLTF
#include "../../../drivers/opengles/opengl_shader/lv_opengl_shader_internal.h"
#include "../../../draw/lv_image_dsc.h"
#include "../../../misc/lv_types.h"
#include "../../../misc/lv_array.h"


typedef struct {
    GLuint count;
    GLuint instanceCount;
    GLuint firstIndex;
    GLint baseVertex;
    GLuint baseInstance;
} IndirectDrawCommand;

typedef struct {
    IndirectDrawCommand draw;
    GLenum primitiveType;
    GLenum indexType;
    GLuint vertexArray;

    GLuint vertexBuffer;
    GLuint indexBuffer;

    GLuint materialUniformsIndex;
    GLuint albedoTexture;
    GLuint emissiveTexture;
    GLuint metalRoughTexture;
    GLuint occlusionTexture;
    GLuint normalTexture;
    GLuint diffuseTransmissionTexture;
    GLuint diffuseTransmissionColorTexture;
    GLuint transmissionTexture;
    GLuint transmissionTexcoordIndex;

    GLint baseColorTexcoordIndex;
    GLint emissiveTexcoordIndex;

    GLint metallicRoughnessTexcoordIndex;
    GLint occlusionTexcoordIndex;
    GLint normalTexcoordIndex;
    GLint diffuseTransmissionTexcoordIndex;
    GLint diffuseTransmissionColorTexcoordIndex;

    GLint clearcoatTexture;
    GLint clearcoatRoughnessTexture;
    GLint clearcoatNormalTexture;
    GLint clearcoatTexcoordIndex;
    GLint clearcoatRoughnessTexcoordIndex;
    GLint clearcoatNormalTexcoordIndex;

    GLuint thicknessTexture;
    GLint thicknessTexcoordIndex;

    GLuint diffuseTexture;
    GLint diffuseTexcoordIndex;

    GLuint specularGlossinessTexture;
    GLint specularGlossinessTexcoordIndex;

} lv_gltf_primitive_t;

typedef struct {
    GLint camera;
    GLint view_projection_matrix;
    GLint model_matrix;
    GLint view_matrix;
    GLint projection_matrix;

    GLint env_intensity;
    GLint env_diffuse_sampler;
    GLint env_specular_sampler;
    GLint env_sheen_sampler;
    GLint env_ggx_lut_sampler;
    GLint env_charlie_lut_sampler;
    GLint env_mip_count;

    GLint exposure;
    GLint roughness_factor;

    GLint base_color_factor;
    GLint base_color_sampler;
    GLint base_color_uv_set;
    GLint base_color_uv_transform;

    GLint emissive_factor;
    GLint emissive_sampler;
    GLint emissive_uv_set;
    GLint emissive_uv_transform;
    GLint emissive_strength;

    GLint metallic_factor;
    GLint metallic_roughness_sampler;
    GLint metallic_roughness_uv_set;
    GLint metallic_roughness_uv_transform;

    GLint occlusion_strength;
    GLint occlusion_sampler;
    GLint occlusion_uv_set;
    GLint occlusion_uv_transform;

    GLint normal_scale;
    GLint normal_sampler;
    GLint normal_uv_set;
    GLint normal_uv_transform;

    GLint clearcoat_factor;
    GLint clearcoat_roughness_factor;
    GLint clearcoat_sampler;
    GLint clearcoat_uv_set;
    GLint clearcoat_uv_transform;
    GLint clearcoat_roughness_sampler;
    GLint clearcoat_roughness_uv_set;
    GLint clearcoat_roughness_uv_transform;
    GLint clearcoat_normal_scale;
    GLint clearcoat_normal_sampler;
    GLint clearcoat_normal_uv_set;
    GLint clearcoat_normal_uv_transform;

    GLint thickness;
    GLint thickness_sampler;
    GLint thickness_uv_set;
    GLint thickness_uv_transform;

    GLint diffuse_transmission_sampler;
    GLint diffuse_transmission_uv_set;
    GLint diffuse_transmission_uv_transform;

    GLint diffuse_transmission_color_sampler;
    GLint diffuse_transmission_color_uv_set;
    GLint diffuse_transmission_color_uv_transform;

    GLint sheen_color_factor;
    GLint sheen_roughness_factor;

    GLint specular_color_factor;
    GLint specular_factor;

    GLint diffuse_transmission_color_factor;
    GLint diffuse_transmission_factor;

    GLint ior;
    GLint alpha_cutoff;

    GLint dispersion;
    GLint screen_size;
    GLint transmission_factor;
    GLint transmission_sampler;
    GLint transmission_uv_set;
    GLint transmission_uv_transform;
    GLint transmission_framebuffer_sampler;
    GLint transmission_framebuffer_size;

    GLint attenuation_distance;
    GLint attenuation_color;

    GLint joints_sampler;

    GLint diffuse_factor;
    GLint glossiness_factor;

    GLint diffuse_sampler;
    GLint diffuse_uv_set;
    GLint diffuse_uv_transform;
    GLint specular_glossiness_sampler;
    GLint specular_glossiness_uv_set;
    GLint specular_glossiness_uv_transform;

} lv_gltf_uniform_locations_t;

lv_gltf_uniform_locations_t lv_gltf_uniform_locations_create(GLuint program);

typedef struct {
    lv_gltf_uniform_locations_t uniforms;
    GLuint program;
} lv_gltf_compiled_shader_t;

void lv_gltf_store_compiled_shader(lv_gltf_model_t * data, size_t identifier, lv_gltf_compiled_shader_t * shader);
lv_gltf_compiled_shader_t * lv_gltf_get_compiled_shader(lv_gltf_model_t * data, size_t identifier);

/**
 * @brief Загрузите файл gltf по указанному пути.
 *
 * @param file_path Имя файла gltf
 * @param shader_manager Указатель на контейнер данных, который будет заполнен.
 * @param shaders Указатель на объект кэша шейдера, который использует этот файл.
 */
lv_gltf_model_t *
lv_gltf_data_load_from_file(const char * file_path,
                            lv_opengl_shader_manager_t * shader_manager);

/**
 * @brief Загрузите файл gltf, закодированный в предоставленном массиве байтов.
 *
 * @param data Имя файла gltf
 * @param data_size еслиgltf_pathпредставляет собой массив байтов, передайте размер этого массива через эту переменную (или 0, если это путь к файлу).
 * @param shader_manager Указатель на контейнер данных, который будет заполнен.
 * @param shaders Указатель на объект кэша шейдера, который использует этот файл.
 */

lv_gltf_model_t *
lv_gltf_data_load_from_bytes(const uint8_t * data, size_t data_size,
                             lv_opengl_shader_manager_t * shader_manager);


/**
 * @brief Получите радиус объекта данных GLTF.
 *
 * @param model Указатель на объект lv_gltf_data_t, из которого можно получить радиус.
 * @return Радиус объекта данных GLTF.
 */
double lv_gltf_data_get_radius(const lv_gltf_model_t * model);


/**
 * @brief Уничтожьте объект данныхGLTFи освободите связанные ресурсы.
 *
 * @param _data Указатель на объект lv_gltf_data_t, который необходимо уничтожить.
 */
void lv_gltf_data_delete(lv_gltf_model_t * _data);

/**
 * @brief Скопируйте информацию о границах из одного объекта данныхGLTFв другой.
 *
 * @param to Указатель на целевой объект lv_gltf_data_t.
 * @param from Указатель на исходный объект lv_gltf_data_t.
 */
void lv_gltf_data_copy_bounds_info(lv_gltf_model_t * to, lv_gltf_model_t * from);

/**
 * @brief Поменяйте местами красный и синий каналы в буфере пикселей.
 *
 * @param pixel_buffer Указатель на буфер пикселей, содержащий данные изображения.
 * @param byte_total_count Общее количество байтов в буфере пикселей.
 * @param has_alpha Флаг, указывающий, содержит ли буфер пикселей альфа-канал.
 */
void lv_gltf_data_rgb_to_bgr(uint8_t * pixel_buffer,
                             size_t byte_total_count,
                             bool has_alpha);

#endif /*LV_USE_GLTF*/

#ifdef __cplusplus
}
#endif

#endif /* LV_GLTFDATA_PRIVATE_H */
