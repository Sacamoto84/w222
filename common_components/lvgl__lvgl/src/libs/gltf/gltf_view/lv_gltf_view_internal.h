/**
 * @file lv_gltf_view_internal.h
 *
 */

#ifndef LV_GLTF_VIEW_INTERNAL_H
#define LV_GLTF_VIEW_INTERNAL_H

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_GLTF

#include "lv_gltf.h"
#include "../../../misc/lv_types.h"
#include "../../../drivers/opengles/opengl_shader/lv_opengl_shader_internal.h"
#include "../../../widgets/3dtexture/lv_3dtexture_private.h"
#include "../gltf_data/lv_gltf_data_internal.h"


/*********************
 *      DEFINES
 *********************/

/* ::Предустановки гаммы::
 * Стандартное значение гаммы составляет 2,2.
 * Значения варьируются от 0,5 до 3,5, грубо говоря, с
 * разумные результаты между уровнями 1,5 и 2,8.
 * Значение должно быть заключено в кавычки как строковый литерал.
 */
#define LV_GLTF_GAMMA_BRIGHTEST "3.5"
#define LV_GLTF_GAMMA_BRIGHTER  "3.0"
#define LV_GLTF_GAMMA_BRIGHT    "2.6"
#define LV_GLTF_GAMMA_STANDARD  "2.2"
#define LV_GLTF_GAMMA_DARK      "1.8"
#define LV_GLTF_GAMMA_DARKER    "1.3"
#define LV_GLTF_GAMMA_DARKEST   "0.8"

#define LV_GLTF_DISTANCE_SCALE_FACTOR 2.5f
#define LV_GLTF_TRANSMISSION_PASS_SIZE 256

/* Примените значения по умолчанию, указанные ниже, если они не заданы явно. */

/* Тональное отображение не применяется, если включен линейный выход.
 * Линейный выход используется по умолчанию.
 */
#ifndef LV_GLTF_LINEAR_OUTPUT
    #define LV_GLTF_LINEAR_OUTPUT 1
#endif

/* Если применено тональное отображение, это регулирует яркость
 * и цветовой диапазон вывода. Используйте строковые значения.
 */
#ifndef LV_GLTF_TONEMAP_GAMMA
    #define LV_GLTF_TONEMAP_GAMMA LV_GLTF_GAMMA_STANDARD
#endif

/**********************
 *      TYPEDEFS
 **********************/

#ifdef __cplusplus
extern "C" {
#endif/* __cplusplus*/


typedef struct {
    uint32_t texture;
    uint32_t renderbuffer;
    unsigned framebuffer;
} lv_gltf_renwin_state_t;

typedef struct  {
    lv_gltf_renwin_state_t render_state;
    lv_gltf_renwin_state_t opaque_render_state;

    uint64_t opaque_frame_buffer_width;
    uint64_t opaque_frame_buffer_height;
    uint32_t material_variant;
    bool render_state_ready;
    bool render_opaque_buffer;
} lv_gltf_view_state_t;

typedef struct {
    float pitch;
    float yaw;
    float distance;
    float fov;                  // Вертикаль FOV в градусах.  Если это значение равно нулю, вид будет ортогональным (неперспективным).
    int32_t render_width;       // Если сглаживание не применяется к этому кадру, они такие же, как ширина/высота, если сглаживание
    int32_t render_height;      // включен, это ширина/высота * мощность повышения разрешения сглаживания (в настоящее время 2.0)
    float focal_x;
    float focal_y;
    float focal_z;
    bool frame_was_antialiased;
    int32_t animation_speed_ratio;
    lv_gltf_aa_mode_t aa_mode;
    lv_gltf_bg_mode_t bg_mode;
    float blur_bg;              /** Насколько сильно размыть фон окружающей среды, от 0,0 до 1,0. */
    float env_pow;              /** Яркость окружающей среды, 1,8 по умолчанию. */
    float exposure;             /** Уровень экспозиции изображения, по умолчанию 1,0. */
} lv_gltf_view_desc_t;

typedef struct {
    /* Состояние смешивания */
    GLboolean blend_enabled;
    GLint blend_src;
    GLint blend_dst;
    GLint blend_equation;

    /* Состояние глубины */
    GLboolean depth_test_enabled;
    GLboolean depth_mask;
    GLint depth_func;

    /* Состояние отсеивания лиц */
    GLboolean cull_face_enabled;
    GLint cull_face_mode;
    GLint front_face;

    /* Состояние трафарета */
    GLboolean stencil_test_enabled;
    GLuint stencil_mask;
    GLint stencil_func;
    GLint stencil_ref;
    GLuint stencil_value_mask;

    /* Привязки буфера */
    GLuint current_vao;
    GLuint current_vbo;
    GLuint current_ibo;
    GLuint current_program;

    /* Состояние текстуры */
    GLint active_texture;
    GLuint bound_texture_2d;

    /* Окно просмотра и ножницы */
    GLint viewport[4];
    GLboolean scissor_test_enabled;
    GLint scissor_box[4];

    /* Четкие ценности */
    GLfloat clear_depth;
    GLfloat clear_color[4];
} lv_opengl_state_t;


#ifdef __cplusplus
}


#include <fastgltf/math.hpp>
#include <fastgltf/types.hpp>
#include <map>

struct _lv_gltf_t {
    lv_3dtexture_t texture;
    lv_array_t models;
    lv_gltf_view_state_t state;
    lv_gltf_view_desc_t desc;
    lv_gltf_view_desc_t last_desc;
    lv_opengl_shader_manager_t shader_manager;
    lv_gltf_environment_t * environment;
    fastgltf::math::fmat4x4 view_matrix;
    fastgltf::math::fmat4x4 projection_matrix;
    fastgltf::math::fmat4x4 view_projection_matrix;
    fastgltf::math::fvec3 camera_pos;

    std::map<int32_t, std::map<fastgltf::Node *, fastgltf::math::fmat4x4>> ibm_by_skin_then_node;
    bool owns_environment;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

GLuint lv_gltf_view_render(lv_gltf_t * viewer);
lv_result_t lv_gltf_view_shader_injest_discover_defines(lv_array_t * result, lv_gltf_model_t * data,
                                                        fastgltf::Node * node,
                                                        fastgltf::Primitive * prim);

/**********************
 *      MACROS
 **********************/

#endif/* __cplusplus*/
#endif /*LV_USE_GLTF*/

#endif /*LV_GLTF_VIEW_INTERNAL_H*/
