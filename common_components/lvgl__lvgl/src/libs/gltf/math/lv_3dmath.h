/**
 * @file lv_3dmath.h
 *
 */

#ifndef LV_3DMATH_H
#define LV_3DMATH_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_GLTF

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    float x;
    float y;
    float z;
} lv_3dpoint_t;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} lv_quaternion_t;

typedef struct {
    lv_3dpoint_t origin;
    lv_3dpoint_t direction;
} lv_3dplane_t;

typedef lv_3dplane_t lv_3dray_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Получите плоскость, обращенную вверх, с центром на заданной высоте.
 * @param elevation Высота наземной плоскости в мировых единицах. обычно это ноль
 * @return наземная плоскость
 */
lv_3dplane_t lv_get_ground_plane(float elevation);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GLTF*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif


#endif /*LV_3DMATH_H*/
