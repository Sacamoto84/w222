/**
 * @file lv_matrix.h
 *
 */

#ifndef LV_MATRIX_H
#define LV_MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lv_conf_internal.h"

#if LV_USE_MATRIX

#include "lv_types.h"
#include "lv_area.h"

/*********************
 *      DEFINES
 *********************/

#if !LV_USE_FLOAT
#error "LV_USE_FLOAT is required for lv_matrix"
#endif

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_matrix_t {
    float m[3][3];
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Установить матрицу в единичную матрицу
 * @param matrix           указатель на матрицу
 */
void lv_matrix_identity(lv_matrix_t * matrix);

/**
 * Перевести матрицу в новую позицию
 * @param matrix           указатель на матрицу
 * @param tx               величина перевода в направлении x
 * @param tx               сумма перевода в направлении Y
 */
void lv_matrix_translate(lv_matrix_t * matrix, float tx, float ty);

/**
 * Измените масштабный коэффициент матрицы
 * @param matrix           указатель на матрицу
 * @param scale_x          масштабный коэффициент для направления X
 * @param scale_y          масштабный коэффициент для направления Y
 */
void lv_matrix_scale(lv_matrix_t * matrix, float scale_x, float scale_y);

/**
 * Поворот матрицы относительно начала координат
 * @param matrix           указатель на матрицу
 * @param degree           угол для поворота
 */
void lv_matrix_rotate(lv_matrix_t * matrix, float degree);

/**
 * Измените коэффициент перекоса матрицы
 * @param matrix           указатель на матрицу
 * @param skew_x           коэффициент перекоса для направления x
 * @param skew_y           коэффициент перекоса для направления y
 */
void lv_matrix_skew(lv_matrix_t * matrix, float skew_x, float skew_y);

/**
 * Умножьте две матрицы и сохраните результат в первой
 * @param matrix           указатель на матрицу
 * @param matrix2          указатель на другую матрицу
 */
void lv_matrix_multiply(lv_matrix_t * matrix, const lv_matrix_t * mul);

/**
 * Инвертировать матрицу
 * @param matrix           указатель на матрицу
 * @param m                указатель на другую матрицу (необязательно)
 * @return true: матрица обратима, false: матрица сингулярна и не может быть инвертирована
 */
bool lv_matrix_inverse(lv_matrix_t * matrix, const lv_matrix_t * m);

/**
 * Преобразование точки с помощью матрицы
 * @param matrix           указатель на матрицу
 * @param point            указатель на точку
 * @return преобразованная точка
 */
lv_point_precise_t lv_matrix_transform_precise_point(const lv_matrix_t * matrix, const lv_point_precise_t * point);

/**
 * Преобразование области с помощью матрицы
 * @param matrix           указатель на матрицу
 * @param area             указатель на область
 * @return преобразованная территория
 */
lv_area_t lv_matrix_transform_area(const lv_matrix_t * matrix, const lv_area_t * area);

/**
 * Проверьте, является ли матрица тождественной
 * @param matrix           указатель на матрицу
 * @return true: матрица единичная, false: матрица не единичная
 */
bool lv_matrix_is_identity(const lv_matrix_t * matrix);

/**
 * Проверьте, является ли матрица тождественной или матрицей перевода.
 * @param matrix           указатель на матрицу
 * @return true: матрица является тождественной матрицей или матрицей перевода, false: матрица не является тождественной матрицей или матрицей перевода.
 */
bool lv_matrix_is_identity_or_translation(const lv_matrix_t * matrix);

/**
 * Транспонировать матрицу.
 * @param src   указатель на исходную матрицу. ЕслиNULL, функция возвращает значение.
 * @param dst   указатель на матрицу назначения. ЕслиNULL, функция возвращает значение.
 * Note: src и dst могут указывать на одну и ту же матрицу для транспонирования на месте.
 */
void lv_matrix_transpose(const lv_matrix_t * src, lv_matrix_t * dst);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_MATRIX*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_MATRIX_H*/
