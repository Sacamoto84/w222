//
// Copyright (c) 2009-2013 Mikko Mononen memon@inside.org
//
// Данное программное обеспечение предоставляется «как есть», без каких-либо явных или подразумеваемых
// гарантия.  Ни при каких обстоятельствах авторы не несут ответственности за какой-либо ущерб.
// возникшие в результате использования данного программного обеспечения.
// Разрешение предоставляется каждому на использование этого программного обеспечения для любых целей,
// включая коммерческие приложения, а также изменять его и распространять
// свободно, с соблюдением следующих ограничений:
// 1. The origin of this software must not be misrepresented; you must not
//    утверждайте, что вы написали оригинальное программное обеспечение. Если вы используете это программное обеспечение
//    в продукте подтверждение в документации по продукту будет
//    приветствуется, но не является обязательным.
// 2. Altered source versions must be plainly marked as such, and must not be
//    ошибочно представлено как оригинальное программное обеспечение.
// 3. This notice may not be removed or altered from any source distribution.
//
#ifndef NANOVG_GL_UTILS_H
#define NANOVG_GL_UTILS_H

#include "../../lv_conf_internal.h"
#include "../../stdlib/lv_mem.h"
#include "../../stdlib/lv_string.h"

#if LV_USE_NANOVG

#ifdef NANOVG_GL_IMPLEMENTATION
struct NVGLUframebuffer {
    NVGcontext * ctx;
    GLuint fbo;
    GLuint rbo;
    GLuint texture;
    int image;
};
#endif

typedef struct NVGLUframebuffer NVGLUframebuffer;

// Вспомогательная функция для создания буфера кадра GL для рендеринга.
// format: see NVGtexture
void nvgluBindFramebuffer(NVGLUframebuffer * fb);
NVGLUframebuffer * nvgluCreateFramebuffer(NVGcontext * ctx, int w, int h, int imageFlags, int format);
void nvgluDeleteFramebuffer(NVGLUframebuffer * fb);

#endif // NANOVG_GL_UTILS_H

#ifdef NANOVG_GL_IMPLEMENTATION

#if defined(NANOVG_GL3) || defined(NANOVG_GLES2) || defined(NANOVG_GLES3)
    // FBO является ядром OpenGL 3>.
    #define NANOVG_FBO_VALID 1
#elif defined(NANOVG_GL2)
    // В OS X, включая glext, определяет FBO и в GL2.
    #ifdef __APPLE__
        #include <OpenGL/glext.h>
        #define NANOVG_FBO_VALID 1
    #endif
#endif

static GLint defaultFBO = -1;

NVGLUframebuffer * nvgluCreateFramebuffer(NVGcontext * ctx, int w, int h, int imageFlags, int format)
{
#ifdef NANOVG_FBO_VALID
    GLint defFBO;
    GLint defaultRBO;
    NVGLUframebuffer * fb = NULL;

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defFBO);
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &defaultRBO);

    fb = (NVGLUframebuffer *)lv_malloc(sizeof(NVGLUframebuffer));
    if(fb == NULL) goto error;
    lv_memzero(fb, sizeof(NVGLUframebuffer));

    fb->image = nvgCreateImage(ctx, w, h, imageFlags | NVG_IMAGE_FLIPY | NVG_IMAGE_PREMULTIPLIED, format, NULL);

#if defined NANOVG_GL2
    fb->texture = nvglImageHandleGL2(ctx, fb->image);
#elif defined NANOVG_GL3
    fb->texture = nvglImageHandleGL3(ctx, fb->image);
#elif defined NANOVG_GLES2
    fb->texture = nvglImageHandleGLES2(ctx, fb->image);
#elif defined NANOVG_GLES3
    fb->texture = nvglImageHandleGLES3(ctx, fb->image);
#endif

    fb->ctx = ctx;

    // объект кадрового буфера
    glGenFramebuffers(1, &fb->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);

    // рендеринг объекта буфера
    glGenRenderbuffers(1, &fb->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, fb->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, w, h);

    // объединить все
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb->rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
#ifdef GL_DEPTH24_STENCIL8
        // Если GL_STENCIL_INDEX8 не поддерживается, попробуйте GL_DEPTH24_STENCIL8 в качестве запасного варианта.
        // Некоторым видеокартам вместе с трафаретом требуется буфер глубины.
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->texture, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb->rbo);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
#endif // GL_DEPTH24_STENCIL8
            goto error;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, defFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, defaultRBO);
    return fb;
error:
    glBindFramebuffer(GL_FRAMEBUFFER, defFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, defaultRBO);
    nvgluDeleteFramebuffer(fb);
    return NULL;
#else
    NVG_NOTUSED(ctx);
    NVG_NOTUSED(w);
    NVG_NOTUSED(h);
    NVG_NOTUSED(imageFlags);
    NVG_NOTUSED(format);
    return NULL;
#endif
}

void nvgluBindFramebuffer(NVGLUframebuffer * fb)
{
#ifdef NANOVG_FBO_VALID
    if(defaultFBO == -1) glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, fb != NULL ? fb->fbo : (GLuint)defaultFBO);
#else
    NVG_NOTUSED(fb);
#endif
}

void nvgluDeleteFramebuffer(NVGLUframebuffer * fb)
{
#ifdef NANOVG_FBO_VALID
    if(fb == NULL) return;
    if(fb->fbo != 0)
        glDeleteFramebuffers(1, &fb->fbo);
    if(fb->rbo != 0)
        glDeleteRenderbuffers(1, &fb->rbo);
    if(fb->image >= 0)
        nvgDeleteImage(fb->ctx, fb->image);
    fb->ctx = NULL;
    fb->fbo = 0;
    fb->rbo = 0;
    fb->texture = 0;
    fb->image = -1;
    lv_free(fb);
#else
    NVG_NOTUSED(fb);
#endif
}

#endif // LV_USE_NANOVG

#endif // NANOVG_GL_IMPLEMENTATION
