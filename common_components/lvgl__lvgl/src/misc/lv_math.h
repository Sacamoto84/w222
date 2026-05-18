/**
 * @file lv_math.h
 *
 */

#ifndef LV_MATH_H
#define LV_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/
#define LV_TRIGO_SIN_MAX 32768
#define LV_TRIGO_SHIFT 15 /**<  >> LV_TRIGO_SHIFT to normalize*/

#define LV_BEZIER_VAL_SHIFT 10 /**< log2(LV_BEZIER_VAL_MAX): used to normalize up scaled values*/
#define LV_BEZIER_VAL_MAX (1L << LV_BEZIER_VAL_SHIFT) /**< Max time in Bezier functions (not [0..1] to use integers)*/
#define LV_BEZIER_VAL_FLOAT(f) ((int32_t)((f) * LV_BEZIER_VAL_MAX)) /**< Convert const float number cubic-bezier values to fix-point value*/

/** Выровнять значение x для выравнивания, выравнивание должно быть степенью двойки */
#define LV_ALIGN_UP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))

/** Округлите значение x до округления, округление может быть любым целым числом. */
#define LV_ROUND_UP(x, round) ((((x) + ((round) - 1)) / (round)) * (round))

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint16_t i;
    uint16_t f;
} lv_sqrt_res_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Возврат с синусом угла
 * Угол @param
 * @return синус «угла». грех(-90) = -32767, грех(90) = 32767
 */
int32_t /* LV_ATTRIBUTE_FAST_MEM */ lv_trigo_sin(int16_t angle);

int32_t LV_ATTRIBUTE_FAST_MEM lv_trigo_cos(int16_t angle);

/**
 * Вычислите значение y функции кубического Безье (x1, y1, x2, y2), как указано x.
 * @param x время в диапазоне [0..LV_BEZIER_VAL_MAX]
 * @param x1 x контрольной точки 1 в диапазоне [0..LV_BEZIER_VAL_MAX]
 * @param y1 y контрольной точки 1 в диапазоне [0..LV_BEZIER_VAL_MAX]
 * @param x2 x контрольной точки 2 в диапазоне [0..LV_BEZIER_VAL_MAX]
 * @param y2 y контрольной точки 2 в диапазоне [0..LV_BEZIER_VAL_MAX]
 * @return рассчитанное значение
 */
int32_t lv_cubic_bezier(int32_t x, int32_t x1, int32_t y1, int32_t x2, int32_t y2);

/**
 * Вычислите значение кубической функции Безье.
 * @param t время в диапазоне [0..LV_BEZIER_VAL_MAX]
 * @param u0 должно быть 0
 * @param u1 Значения управляющего значения 1 в диапазоне [0..LV_BEZIER_VAL_MAX]
 * @param u2 управляющее значение 2 в диапазоне [0..LV_BEZIER_VAL_MAX]
 * @param u3 должно быть LV_BEZIER_VAL_MAX
 * @return значение, рассчитанное на основе заданных параметров в диапазоне [0..LV_BEZIER_VAL_MAX]
 */
int32_t lv_bezier3(int32_t t, int32_t u0, uint32_t u1, int32_t u2, int32_t u3);


/**
 * Вычислите вектор atan2.
 * @param x
 * @param y
 * @return угол в градусах, рассчитанный по заданным параметрам в диапазоне [0..360]
 */
uint16_t lv_atan2(int x, int y);

/**
 * Получить квадратный корень числа
 * @param x целое число, квадратный корень которого следует вычислить
 * @param q сохраните результат здесь. q->i: целая часть, q->f: дробная часть в единице 1/256
 * @param mask необязательно пропускать некоторые итерации, если известна величина корня.
 * По умолчанию установлено значение 0x8000.
 * Если корень < 16: маска = 0x80
 * Если root < 256: маска = 0x800
 * Else: маска = 0x8000
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_sqrt(uint32_t x, lv_sqrt_res_t * q, uint32_t mask);

/**
 * Альтернативная (быстрая, приблизительная) реализация извлечения квадратного корня из целого числа.
 * @param x целое число, квадратный корень которого следует вычислить
 */
int32_t /* LV_ATTRIBUTE_FAST_MEM */ lv_sqrt32(uint32_t x);

/**
 * Вычислить квадрат целого числа (диапазон ввода: 0..32767).
 * @param x ввод
 * @return квадрат
 */
static inline int32_t lv_sqr(int32_t x)
{
    return x * x;
}

/**
 * Вычислите целочисленные показатели.
 * База @param
 * @param base
 * @return основание возведено в степень
 */
int64_t lv_pow(int64_t base, int8_t exp);

/**
 * Получить отображение числа с заданным диапазоном ввода и вывода.
 * @param x целое число, отображаемое значение которого следует вычислить
 * @param min_in минимальный диапазон ввода
 * @param max_in максимальный диапазон ввода
 * @param min_out максимальный выходной диапазон
 * @param max_out максимальный выходной диапазон
 * @return сопоставленный номер
 */
int32_t lv_map(int32_t x, int32_t min_in, int32_t max_in, int32_t min_out, int32_t max_out);

/**
 * Установите начальное значение генератора псевдослучайных чисел
 * @param seed число для инициализации генератора случайных чисел
 */
void lv_rand_set_seed(uint32_t seed);

/**
 * Получить псевдослучайное число в заданном диапазоне
 * @param min   минимальное значение
 * @param max   максимальное значение
 * @return вернуть случайное число. мин <=return_value<= макс
 */
uint32_t lv_rand(uint32_t min, uint32_t max);

/**********************
 *      MACROS
 **********************/
#define LV_MIN(a, b) ((a) < (b) ? (a) : (b))
#define LV_MIN3(a, b, c) (LV_MIN(LV_MIN(a,b), c))
#define LV_MIN4(a, b, c, d) (LV_MIN(LV_MIN(a,b), LV_MIN(c,d)))

#define LV_MAX(a, b) ((a) > (b) ? (a) : (b))
#define LV_MAX3(a, b, c) (LV_MAX(LV_MAX(a,b), c))
#define LV_MAX4(a, b, c, d) (LV_MAX(LV_MAX(a,b), LV_MAX(c,d)))

#define LV_CLAMP(min, val, max) (LV_MAX(min, (LV_MIN(val, max))))

#define LV_ABS(x) ((x) > 0 ? (x) : (-(x)))
#define LV_UDIV255(x) (((x) * 0x8081U) >> 0x17)

#define LV_IS_SIGNED(t) (((t)(-1)) < ((t)0))
#define LV_UMAX_OF(t) (((0x1ULL << ((sizeof(t) * 8ULL) - 1ULL)) - 1ULL) | (0xFULL << ((sizeof(t) * 8ULL) - 4ULL)))
#define LV_SMAX_OF(t) (((0x1ULL << ((sizeof(t) * 8ULL) - 1ULL)) - 1ULL) | (0x7ULL << ((sizeof(t) * 8ULL) - 4ULL)))
#define LV_MAX_OF(t) ((unsigned long)(LV_IS_SIGNED(t) ? LV_SMAX_OF(t) : LV_UMAX_OF(t)))

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif
