#ifndef __eglplatform_h_
#define __eglplatform_h_

#include "../../../../../lv_conf_internal.h"
#if LV_USE_OPENGLES

/*
** Copyright 2007-2020 The Khronos Group Inc.
** SPDX-License-Identifier: Apache-2.0
*/

/* Типы и определения для egl.h, зависящие от платформы
 *
 * Усыновители могут изменить khrplatform.h и этот файл в соответствии со своей платформой.
 * Вам рекомендуется отправлять все изменения в группу Khronos, чтобы
 * они могут быть включены в будущие версии этого файла.  Пожалуйста, отправьте изменения
 * оформив вопрос или запрос на включение в общедоступный реестр Khronos EGL по адресу
 * https://www.github.com/KhronosGroup/EGL-Registry/
 */

#include <KHR/khrplatform.h>

/* Макросы, используемые в объявлениях прототипов функций EGL.
 *
 * Функции EGL должны быть прототипированы как:
 *
 * EGLAPI возвращаемый тип EGLAPIENTRY eglFunction(аргументы);
 * typedef тип возвращаемого значения ( EXPAPIENTRYP PFNEGLFUNCTIONPROC ) (аргументы);
 *
 * KHRONOS_APICALL и KHRONOS_APIENTRY определены в KHR/khrplatform.h.
 */

#ifndef EGLAPI
#define EGLAPI KHRONOS_APICALL
#endif

#ifndef EGLAPIENTRY
#define EGLAPIENTRY  KHRONOS_APIENTRY
#endif
#define EGLAPIENTRYP EGLAPIENTRY*

/* Типы NativeDisplayType, NativeWindowType и NativePixmapType.
 * являются псевдонимами типов, зависящих от оконной системы, таких как X Display * или
 * Контекст устройства Windows. Они должны быть определены в специфичных для платформы
 * код ниже. Версии Native*Type с префиксом EGL одинаковы.
 * типы, переименованные в EGL 1.3, поэтому все типы в API начинаются с «EGL».
 *
 * Khronos STRONGLY RECOMMENDS, что вы используете определения по умолчанию.
 * представлено ниже, поскольку эти изменения затрагивают как двоичный, так и исходный код.
 * переносимость приложений, использующих EGL, работающих на разных EGL
 * реализации.
 */

#if defined(EGL_NO_PLATFORM_SPECIFIC_TYPES)

typedef void *EGLNativeDisplayType;
typedef void *EGLNativePixmapType;
typedef void *EGLNativeWindowType;

#elif defined(_WIN32) || defined(__VC32__) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__) /* Win32 и WinCE */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>

typedef HDC     EGLNativeDisplayType;
typedef HBITMAP EGLNativePixmapType;
typedef HWND    EGLNativeWindowType;

#elif defined(__QNX__)

typedef khronos_uintptr_t      EGLNativeDisplayType;
typedef struct _screen_pixmap* EGLNativePixmapType;  /* screen_pixmap_t */
typedef struct _screen_window* EGLNativeWindowType;  /* screen_window_t */

#elif defined(__EMSCRIPTEN__)

typedef int EGLNativeDisplayType;
typedef int EGLNativePixmapType;
typedef int EGLNativeWindowType;

#elif defined(__WINSCW__) || defined(__SYMBIAN32__)  /* Симбиан */

typedef int   EGLNativeDisplayType;
typedef void *EGLNativePixmapType;
typedef void *EGLNativeWindowType;

#elif defined(WL_EGL_PLATFORM)

typedef struct wl_display     *EGLNativeDisplayType;
typedef struct wl_egl_pixmap  *EGLNativePixmapType;
typedef struct wl_egl_window  *EGLNativeWindowType;

#elif defined(__GBM__)

typedef struct gbm_device  *EGLNativeDisplayType;
typedef struct gbm_bo      *EGLNativePixmapType;
typedef void               *EGLNativeWindowType;

#elif defined(__ANDROID__) || defined(ANDROID)

struct ANativeWindow;
struct egl_native_pixmap_t;

typedef void*                           EGLNativeDisplayType;
typedef struct egl_native_pixmap_t*     EGLNativePixmapType;
typedef struct ANativeWindow*           EGLNativeWindowType;

#elif defined(USE_OZONE)

typedef intptr_t EGLNativeDisplayType;
typedef intptr_t EGLNativePixmapType;
typedef intptr_t EGLNativeWindowType;

#elif defined(USE_X11)

/* X11 (предварительно)  */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef Display *EGLNativeDisplayType;
typedef Pixmap   EGLNativePixmapType;
typedef Window   EGLNativeWindowType;

#elif defined(__unix__)

typedef void             *EGLNativeDisplayType;
typedef khronos_uintptr_t EGLNativePixmapType;
typedef khronos_uintptr_t EGLNativeWindowType;

#elif defined(__APPLE__)

typedef int   EGLNativeDisplayType;
typedef void *EGLNativePixmapType;
typedef void *EGLNativeWindowType;

#elif defined(__HAIKU__)

#include <kernel/image.h>

typedef void              *EGLNativeDisplayType;
typedef khronos_uintptr_t  EGLNativePixmapType;
typedef khronos_uintptr_t  EGLNativeWindowType;

#elif defined(__Fuchsia__)

typedef void              *EGLNativeDisplayType;
typedef khronos_uintptr_t  EGLNativePixmapType;
typedef khronos_uintptr_t  EGLNativeWindowType;

#else
#error "Platform not recognized"
#endif

/* Типы EGL 1.2, переименованные для обеспечения единообразия в EGL 1.3. */
typedef EGLNativeDisplayType NativeDisplayType;
typedef EGLNativePixmapType  NativePixmapType;
typedef EGLNativeWindowType  NativeWindowType;


/* Определите EGLint. Это должен быть целочисленный тип со знаком, достаточно большой, чтобы содержать
 * все допустимые имена и значения атрибутов, передаваемые в EGL и из него, независимо от того,
 * их тип — логический, битовая маска, перечисляемый (символическая константа), целочисленный,
 * ручка или что-то другое.  Хотя в целом 32-битного целого числа будет достаточно, если
 * дескрипторы являются 64-битными типами, тогда EGLint должен быть определен как подписанный 64-битный тип.
 * целочисленный тип.
 */
typedef khronos_int32_t EGLint;


/* Макросы приведения типов C++/C для специальных значений дескриптора EGL */
#if defined(__cplusplus)
#define EGL_CAST(type, value) (static_cast<type>(value))
#else
#define EGL_CAST(type, value) ((type) (value))
#endif

#endif /*LV_USE_OPENGLES*/
#endif /* __eglplatform_h */
