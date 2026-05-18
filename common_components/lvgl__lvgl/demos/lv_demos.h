/**
 * @file lv_demos.h
 *
 */

#ifndef LV_DEMOS_H
#define LV_DEMOS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl.h"

typedef struct _lv_demo_args lv_demo_args_t;

#if LV_USE_DEMO_BENCHMARK
#include "benchmark/lv_demo_benchmark.h"
#endif

#if LV_USE_DEMO_KEYPAD_AND_ENCODER
#include "keypad_encoder/lv_demo_keypad_encoder.h"
#endif

#if LV_USE_DEMO_MUSIC
#include "music/lv_demo_music.h"
#endif

#if LV_USE_DEMO_STRESS
#include "stress/lv_demo_stress.h"
#endif

#if LV_USE_DEMO_WIDGETS
#include "widgets/lv_demo_widgets.h"
#endif

#if LV_USE_DEMO_VECTOR_GRAPHIC && LV_USE_VECTOR_GRAPHIC
#include "vector_graphic/lv_demo_vector_graphic.h"
#endif

#if LV_USE_DEMO_RENDER
#include "render/lv_demo_render.h"
#endif

#if LV_USE_DEMO_GLTF
#include "gltf/lv_demo_gltf.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_demo_args {
    lv_obj_t * parent;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте структуруlv_demo_args_tзначениями по умолчанию.
 * @param args Указатель на структуру lv_demo_args_t, которую необходимо инициализировать.
 */
void lv_demo_args_init(lv_demo_args_t * args);

/**
 * Позвонитеlv_demo_xxx.
 * @param   info информация, содержащая имя и параметры демо-версии
 *               Модель lv_demo_xxx.
 * @size больше информации.
 */
bool lv_demos_create(char * info[], int size);

/**
 * Показать справку поlv_demos.
 */
void lv_demos_show_help(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMOS_H*/
