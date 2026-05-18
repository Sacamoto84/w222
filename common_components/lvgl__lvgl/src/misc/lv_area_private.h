/**
 * @file lv_area_private.h
 *
 */

#ifndef LV_AREA_PRIVATE_H
#define LV_AREA_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_area.h"

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
 * Установите положение области (ширина и высота будут сохранены)
 * @param area_p указатель на область
 * @param x новая координата x области
 * @param y новая координата y области
 */
void lv_area_set_pos(lv_area_t * area_p, int32_t x, int32_t y);

/**
 * Получить общие части двух областей
 * @param res_p указатель на область, результат будет сохранен в ней
 * @param a1_p указатель на первую область
 * @param a2_p указатель на вторую область
 * @return false: две области имеют общие части NO,res_pнедействителен.
 */
bool lv_area_intersect(lv_area_t * res_p, const lv_area_t * a1_p, const lv_area_t * a2_p);

/**
 * Получите результирующие подобласти после удаления общих частей двух областей из первой области.
 * @param res_p указатель на массив областей со счетчиком 4, здесь будут храниться полученные области
 * @param a1_p указатель на первую область
 * @param a2_p указатель на вторую область
 * @return количество результатов (максимум 4) или -1, если нет пересечений
 */
int8_t lv_area_diff(lv_area_t res_p[], const lv_area_t * a1_p, const lv_area_t * a2_p);

/**
 * Объедините две области в третью, которая включает в себя две другие.
 * @param a_res_p указатель на область, здесь будет сохранен результат
 * @param a1_p указатель на первую область
 * @param a2_p указатель на вторую область
 */
void lv_area_join(lv_area_t * a_res_p, const lv_area_t * a1_p, const lv_area_t * a2_p);

/**
 * Проверьте, находится ли точка на площади
 * @param a_p указатель на область
 * @param p_p указатель на точку
 * @param radius радиус площади (например, для прямоугольника со скругленными углами)
 * @return false: точка находится за пределами области
 */
bool lv_area_is_point_on(const lv_area_t * a_p, const lv_point_t * p_p, int32_t radius);

/**
 * Проверьте, есть ли у двух областей общие части
 * @param a1_p указатель на область.
 * @param a2_p указатель на другую область
 * @return false:a1_pиa2_pне имеют общих частей.
 */
bool lv_area_is_on(const lv_area_t * a1_p, const lv_area_t * a2_p);

/**
 * Проверьте, полностью ли область находится на другой
 * @param ain_p указатель на область, которая может находиться в 'aholder_p'
 * @param aholder_p указатель на область, которая может включать «ain_p»
 * @param radius радиус`aholder_p`(например, для закругленного прямоугольника)
 * @return правда:`ain_p`полностью находится внутри `aholder_p`
 */
bool lv_area_is_in(const lv_area_t * ain_p, const lv_area_t * aholder_p, int32_t radius);

/**
 * Проверьте, полностью ли одна область выходит за пределы другой
 * @param aout_p указатель на область, которая может находиться в 'aholder_p'
 * @param aholder_p указатель на область, которая может включать «ain_p»
 * @param radius радиус`aholder_p`(например, для закругленного прямоугольника)
 * @return true:`aout_p`полностью находится за пределами `aholder_p`
 */
bool lv_area_is_out(const lv_area_t * aout_p, const lv_area_t * aholder_p, int32_t radius);

/**
 * Проверьте, совпадают ли 2 области
 * @param a указатель на область
 * @param b указатель на другую область
 */
bool lv_area_is_equal(const lv_area_t * a, const lv_area_t * b);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_AREA_PRIVATE_H*/
