/**
 * @file lv_opengl_shader_program.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_opengl_shader_internal.h"

#if LV_USE_OPENGLES

#include "../lv_opengles_private.h"
#include "../lv_opengles_debug.h"
#include "../../../misc/lv_assert.h"
#include "../../../stdlib/lv_mem.h"
/*********************
 *      DEFINES
 *********************/

#define INVALID_LOCATION 0xFFFFFFFFu

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void update_uniform_1i(lv_opengl_shader_program_t * program, const char * prop,
                              int value);

static void update_uniform_1f(lv_opengl_shader_program_t * program, const char * prop,
                              float value);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_opengl_shader_program_t * lv_opengl_shader_program_create(unsigned int _program)
{
    lv_opengl_shader_program_t * program = lv_malloc(sizeof(*program));
    LV_ASSERT_MALLOC(program);
    if(!program) {
        return NULL;
    }
    program->update_uniform_1i = &update_uniform_1i;
    program->update_uniform_1f = &update_uniform_1f;
    program->id = _program;

    return program;
}

void lv_opengl_shader_program_destroy(lv_opengl_shader_program_t * program)
{
#ifndef __EMSCRIPTEN__
    GLint shader_num = 0;
    GL_CALL(glGetProgramiv(program->id, GL_ATTACHED_SHADERS, &shader_num));

    if(shader_num > 0) {
        GLuint * shader_names = lv_malloc(shader_num * sizeof(GLuint));

        if(shader_names) {
            GLsizei shader_count;
            GL_CALL(glGetAttachedShaders(program->id, shader_num, &shader_count,
                                         shader_names));

            // Отсоедините и удалите каждый шейдер
            for(GLsizei i = 0; i < shader_count && i < shader_num; ++i) {
                if(shader_names[i] != 0)
                    GL_CALL(glDetachShader(program->id, shader_names[i]));
            }
            lv_free(shader_names);
        }
    }
#endif

    /* На этом этапе мы сможем без проблем вызвать функцию, указанную ниже.
     * но из-за тонких проблем, связанных с ленивым обновлением ресурсов шейдеров.
     * это вызывает значительную паузу на некоторых платформах.  Поскольку шейдеры
     * уже отсоединены, мы можем смело пропустить эту функцию и оставить
     * пустые программы в кеше OpenGL, пока приложение не закроется, это
     * очень маленький объем памяти.
     *
     * Задача: рассмотрите возможность установки флага на этом этапе и проверьте, верен ли этот флаг.
     * когда приложение наконец закроется, выполните вызовы glDeleteProgram
     * если необходимо.  В этом нет необходимости, OpenGL все равно это сделает.
     * когда он выключится. */

    /* GL_CALL (glDeleteProgram(программа->id)); */
    lv_free(program);
}

GLuint lv_opengl_shader_program_get_id(lv_opengl_shader_program_t * program)
{
    LV_ASSERT_NULL(program);
    return program->id;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


static void update_uniform_1i(lv_opengl_shader_program_t * program, const char * prop,
                              int value)
{
    GLuint location = glGetUniformLocation(program->id, prop);
    LV_ASSERT_FORMAT_MSG(location != INVALID_LOCATION,
                         "Uniform '%s' not found in program %d",
                         prop, program->id);
    if(location == INVALID_LOCATION) {
        LV_LOG_ERROR("Uniform '%s' not found in program %d", prop,
                     program->id);
        return;
    }
    GL_CALL(glUniform1i(location, value));
}

static void update_uniform_1f(lv_opengl_shader_program_t * program, const char * prop,
                              float value)
{
    GLuint location = glGetUniformLocation(program->id, prop);

    LV_ASSERT_FORMAT_MSG(location != INVALID_LOCATION,
                         "Uniform '%s' not found in program %d",
                         prop, program->id);
    if(location == INVALID_LOCATION) {
        LV_LOG_ERROR("Uniform '%s' not found in program %d", prop,
                     program->id);
        return;
    }
    GL_CALL(glUniform1f(location, value));
}

#endif /*LV_USE_OPENGLES*/
