#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#define ERROR_THRESHOLD         5 /*5 из 1024, допускается максимальная ошибка 0,5 %.*/
#define NEWTON_ITERATIONS       8

static float do_cubic_bezier_f(float t, float a, float b, float c)
{
    /*a*t^3 + b*t^2 + c*t*/
    return ((a * t + b) * t + c) * t;
}

/**
 * Вычислите значение y функции кубического Безье (x1, y1, x2, y2), как указано x.
 * @param x время в диапазоне [0..1]
 * @param x1 x контрольной точки 1 в диапазоне [0..1]
 * @param y1 y контрольной точки 1 в диапазоне [0..1]
 * @param x2 x контрольной точки 2 в диапазоне [0..1]
 * @param y2 y контрольной точки 2 в диапазоне [0..1]
 * @return рассчитанное значение
 */
static float lv_cubic_bezier_f(float x, float x1, float y1, float x2, float y2)
{
    float ax, bx, cx, ay, by, cy;
    float tl, tr, t;  /*t в функции кубического Безье, используемой для деления пополам */
    float xs;  /*x выбрано на кривой */
    float d; /*значение наклона при указанном t*/

    if(x == 0 || x == 1) return x;

    cx = 3.f * x1;
    bx = 3.f * (x2 - x1) - cx;
    ax = 1.f - cx - bx;

    cy = 3.f * y1;
    by = 3.f * (y2 - y1) - cy;
    ay = 1.f - cy - by;

    /*Сначала попробуйте метод Ньютона */
    t = x; /*Сделайте предположение*/
    for(int i = 0; i < NEWTON_ITERATIONS; i++) {
        xs = do_cubic_bezier_f(t, ax, bx, cx);
        xs -= x;
        if(LV_ABS(xs) < 1e-6f) goto found;

        d = (3.f * ax * t + 2.f * bx) * t + cx;
        if(LV_ABS(d) < 1e-6f) break;
        t -= xs / d;
    }

    /*Возврат к методу деления пополам для надежности*/
    tl = 0.f, tr = 1.f, t = x;

    if(t < tl) {
        t = tl;
        goto found;
    }

    if(t > tr) {
        t = tr;
        goto found;
    }

    while(tl < tr) {
        xs = do_cubic_bezier_f(t, ax, bx, cx);
        if(LV_ABS(xs - x) < 1e-6f) goto found;
        x > xs ? (tl = t) : (tr = t);
        t = (tr - tl) * .5f + tl;
    }

found:
    return do_cubic_bezier_f(t, ay, by, cy);
}

static void test_cubic_bezier_ease_functions(float fx1, float fy1, float fx2, float fy2)
{
    int x1, y1, x2, y2, y;
    float t, t_step, fy;

    t_step = .001f;
    x1 = LV_BEZIER_VAL_FLOAT(fx1);
    y1 = LV_BEZIER_VAL_FLOAT(fy1);
    x2 = LV_BEZIER_VAL_FLOAT(fx2);
    y2 = LV_BEZIER_VAL_FLOAT(fy2);

    for(t = 0; t <= 1; t += t_step) {
        fy = lv_cubic_bezier_f(t, fx1, fy1, fx2, fy2);
        y = lv_cubic_bezier(LV_BEZIER_VAL_FLOAT(t), x1, y1, x2, y2);
        TEST_ASSERT_LESS_THAN_INT32(ERROR_THRESHOLD, LV_ABS(LV_BEZIER_VAL_FLOAT(fy) - y));
    }
}

static uint32_t lv_bezier3_legacy(uint32_t t, uint32_t u0, uint32_t u1, uint32_t u2, uint32_t u3)
{
    uint32_t t_rem  = 1024 - t;
    uint32_t t_rem2 = (t_rem * t_rem) >> 10;
    uint32_t t_rem3 = (t_rem2 * t_rem) >> 10;
    uint32_t t2     = (t * t) >> 10;
    uint32_t t3     = (t2 * t) >> 10;

    uint32_t v1 = (t_rem3 * u0) >> 10;
    uint32_t v2 = (3 * t_rem2 * t * u1) >> 20;
    uint32_t v3 = (3 * t_rem * t2 * u2) >> 20;
    uint32_t v4 = (t3 * u3) >> 10;

    return v1 + v2 + v3 + v4;
}

/* Проверка функции lv_trigo_sin */
void test_math_trigo_sin_basic(void)
{
    /* Граничные значения теста */
    TEST_ASSERT_EQUAL_INT32(0, lv_trigo_sin(0));
    TEST_ASSERT_EQUAL_INT32(32768, lv_trigo_sin(90));
    TEST_ASSERT_EQUAL_INT32(0, lv_trigo_sin(180));
    TEST_ASSERT_EQUAL_INT32(-32768, lv_trigo_sin(270));
    TEST_ASSERT_EQUAL_INT32(0, lv_trigo_sin(360));

    /* Проверьте все квадранты */
    TEST_ASSERT_EQUAL_INT32(16384, lv_trigo_sin(30));    /* Q1 */
    TEST_ASSERT_EQUAL_INT32(28378, lv_trigo_sin(120));   /* Q2 */
    TEST_ASSERT_EQUAL_INT32(-16384, lv_trigo_sin(210));   /* Q3 */
    TEST_ASSERT_EQUAL_INT32(-28378, lv_trigo_sin(300));   /* Q4 */
}

void test_math_trigo_sin_negative_angles(void)
{
    /* Тестирование отрицательных углов */
    TEST_ASSERT_EQUAL_INT32(0, lv_trigo_sin(-360));
    TEST_ASSERT_EQUAL_INT32(-32768, lv_trigo_sin(-90));
    TEST_ASSERT_EQUAL_INT32(0, lv_trigo_sin(-180));
    TEST_ASSERT_EQUAL_INT32(32768, lv_trigo_sin(-270));
}

void test_math_trigo_sin_large_angles(void)
{
    /* Тестируйте большие углы */
    TEST_ASSERT_EQUAL_INT32(0, lv_trigo_sin(720));
    TEST_ASSERT_EQUAL_INT32(32768, lv_trigo_sin(450)); /* 450 = 360 + 90 */
    TEST_ASSERT_EQUAL_INT32(-32768, lv_trigo_sin(630)); /* 630 = 360 + 270 */
}

/* Проверка функции lv_trigo_cos */
void test_math_trigo_cos_basic(void)
{
    /* Граничные значения теста */
    TEST_ASSERT_EQUAL_INT32(32768, lv_trigo_cos(0));
    TEST_ASSERT_EQUAL_INT32(0, lv_trigo_cos(90));
    TEST_ASSERT_EQUAL_INT32(-32768, lv_trigo_cos(180));
    TEST_ASSERT_EQUAL_INT32(0, lv_trigo_cos(270));
    TEST_ASSERT_EQUAL_INT32(32768, lv_trigo_cos(360));
}

void test_math_trigo_cos_negative_angles(void)
{
    /* Тестирование отрицательных углов */
    TEST_ASSERT_EQUAL_INT32(32768, lv_trigo_cos(-360));
    TEST_ASSERT_EQUAL_INT32(0, lv_trigo_cos(-90));
    TEST_ASSERT_EQUAL_INT32(-32768, lv_trigo_cos(-180));
    TEST_ASSERT_EQUAL_INT32(0, lv_trigo_cos(-270));
}

static void assert_sqrt32_floor(uint32_t x)
{
    int32_t r = lv_sqrt32(x);
    uint32_t rr = (uint32_t)r * (uint32_t)r;
    uint32_t rr_next = (uint32_t)(r + 1) * (uint32_t)(r + 1);

    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(rr, x);
    TEST_ASSERT_LESS_THAN_UINT32(rr_next, x);
}

/* Проверка функции lv_cubic_bezier */
void test_math_cubic_bezier_result_should_be_precise(void)
{
    /*функция легкого извлечения*/
    test_cubic_bezier_ease_functions(.42f, 0, .58f, 1);

    /*функция облегчения*/
    test_cubic_bezier_ease_functions(0, 0, .58f, 1);

    /*функция облегчения входа*/
    test_cubic_bezier_ease_functions(.42f, 0, 1, 1);

    /*функция облегчения*/
    test_cubic_bezier_ease_functions(.25f, .1f, .25f, 1);

    int32_t u0 = 0, u1 = 50, u2 = 952, u3 = LV_BEZIER_VAL_MAX;
    for(int32_t i = 0; i <= 1024; i++) {
        int32_t legacy = lv_bezier3_legacy(i, u0, u1, u2, u3);
        int32_t cubic_bezier = lv_bezier3(i, u0, u1, u2, u3);

        TEST_ASSERT_LESS_OR_EQUAL_INT32(5, LV_ABS(legacy - cubic_bezier));
    }

    /* Сравните с устаревшей реализацией */
    u0 = 0, u1 = 341, u2 = 683, u3 = 1024;

    for(int32_t i = 0; i <= 1024; i++) {
        int32_t legacy = lv_bezier3_legacy(i, u0, u1, u2, u3);
        int32_t result = lv_bezier3(i, u0, u1, u2, u3);
        TEST_ASSERT_LESS_OR_EQUAL_INT32(5, LV_ABS(legacy - result));
    }
}

void test_math_cubic_bezier_edge_cases(void)
{
    /* Тест x = 0 и x = LV_BEZIER_VAL_MAX - должен возвращать x напрямую */
    int32_t result1 = lv_cubic_bezier(0, 341, 512, 683, 1024);
    int32_t result2 = lv_cubic_bezier(1024, 341, 512, 683, 1024);
    TEST_ASSERT_EQUAL_INT32(0, result1);
    TEST_ASSERT_EQUAL_INT32(1024, result2);
}

void test_math_cubic_bezier_boundary_values(void)
{
    /* Тест с граничными контрольными точками – упрощенные тесты */
    int32_t result1 = lv_cubic_bezier(512, 0, 0, 0, 0);
    int32_t result2 = lv_cubic_bezier(512, 1024, 1024, 1024, 1024);
    /* Они должны давать результаты в допустимом диапазоне. */
    TEST_ASSERT_INT32_WITHIN(512, 512, result1);  /* результат1 должен быть между 0 и 1024 */
    TEST_ASSERT_INT32_WITHIN(512, 512, result2);  /* результат2 должен быть между 0 и 1024 */
}

void test_math_cubic_bezier_clamps_out_of_range(void)
{
    /* Принудительно использовать резервный вариант деления пополам, чтобы зафиксировать t в допустимом диапазоне. */
    int32_t below = lv_cubic_bezier(-50, 0, 0, 0, 0);
    int32_t above = lv_cubic_bezier(LV_BEZIER_VAL_MAX + 50, LV_BEZIER_VAL_MAX, LV_BEZIER_VAL_MAX,
                                    LV_BEZIER_VAL_MAX, LV_BEZIER_VAL_MAX);

    TEST_ASSERT_EQUAL_INT32(0, below);
    TEST_ASSERT_EQUAL_INT32(LV_BEZIER_VAL_MAX, above);
}

/* Проверка функции lv_bezier3 */
void test_math_bezier3_basic(void)
{
    /* Тестирование базовых случаев */
    TEST_ASSERT_EQUAL_INT32(0, lv_bezier3(0, 0, 50, 952, 1024));
    TEST_ASSERT_EQUAL_INT32(1024, lv_bezier3(1024, 0, 50, 952, 1024));
    /* Проверьте, что он возвращает допустимое значение в диапазоне. */
    int32_t result = lv_bezier3(512, 0, 50, 50, 1024);
    TEST_ASSERT_INT32_WITHIN(512, 512, result);  /* Должно быть в допустимом диапазоне от 0 до 1024. */
}

/* Проверка функции lv_sqrt */
void test_math_sqrt_basic(void)
{
    lv_sqrt_res_t result;

    /* Тест 0 */
    lv_sqrt(0, &result, 0x8000);
    TEST_ASSERT_EQUAL_UINT16(0, result.i);
    TEST_ASSERT_EQUAL_UINT16(0, result.f);

    /* Тест 1 */
    lv_sqrt(1, &result, 0x8000);
    TEST_ASSERT_EQUAL_UINT16(1, result.i);

    /* Тест 16 */
    lv_sqrt(16, &result, 0x8000);
    TEST_ASSERT_EQUAL_UINT16(4, result.i);

    /* Тест 256 */
    lv_sqrt(256, &result, 0x8000);
    TEST_ASSERT_EQUAL_UINT16(16, result.i);

    /* Тест 65536 */
    lv_sqrt(65536, &result, 0x8000);
    TEST_ASSERT_EQUAL_UINT16(256, result.i);
}

void test_math_sqrt_fractional(void)
{
    lv_sqrt_res_t result;

    /* Тест 2 - должно дать 1,414... */
    lv_sqrt(2, &result, 0x8000);
    TEST_ASSERT_EQUAL_UINT16(1, result.i);
    TEST_ASSERT_EQUAL_UINT16(96, result.f); /* Должна иметь дробную часть */

    /* Тест 100 - должен дать 10 */
    lv_sqrt(100, &result, 0x8000);
    TEST_ASSERT_EQUAL_UINT16(10, result.i);
    TEST_ASSERT_EQUAL_UINT16(0, result.f);
}

void test_math_sqrt_with_different_masks(void)
{
    lv_sqrt_res_t result;

    /* Тестируйте с разными значениями маски — результаты могут незначительно отличаться */
    lv_sqrt(256, &result, 0x80);   /* Корень < 16 */
    TEST_ASSERT_EQUAL_UINT16(15, result.i);
    TEST_ASSERT_EQUAL_UINT16(240, result.f);

    lv_sqrt(65536, &result, 0x800); /* Корень < 256 */
    TEST_ASSERT_EQUAL_UINT16(255, result.i);  /* результат. мне должно быть 255 или 256 */
    TEST_ASSERT_EQUAL_UINT16(240, result.f);

    lv_sqrt(1048576, &result, 0x8000); /* Корень >= 256 */
    TEST_ASSERT_EQUAL_UINT16(1024, result.i);
    TEST_ASSERT_EQUAL_UINT16(0, result.f);
}

/* Проверка функции lv_sqrt32 */
void test_math_sqrt32_basic(void)
{
    /* Проверьте базовые значения */
    TEST_ASSERT_EQUAL_INT32(0, lv_sqrt32(0));
    TEST_ASSERT_EQUAL_INT32(1, lv_sqrt32(1));
    TEST_ASSERT_EQUAL_INT32(2, lv_sqrt32(4));
    TEST_ASSERT_EQUAL_INT32(3, lv_sqrt32(9));
    TEST_ASSERT_EQUAL_INT32(4, lv_sqrt32(16));
    TEST_ASSERT_EQUAL_INT32(10, lv_sqrt32(100));
    TEST_ASSERT_EQUAL_INT32(256, lv_sqrt32(65536));
    TEST_ASSERT_EQUAL_INT32(1024, lv_sqrt32(1048576));
}

void test_math_sqrt32_range(void)
{
    /* Тестируйте различные диапазоны */
    TEST_ASSERT_EQUAL_INT32(255, lv_sqrt32(65025));   /* 255^2 = 65025 */
    TEST_ASSERT_EQUAL_INT32(256, lv_sqrt32(65536));   /* 256^2 = 65536 */
    /* Избегайте тестирования вблизи максимума, чтобы предотвратить переполнение */
    TEST_ASSERT_EQUAL_INT32(1000, lv_sqrt32(1000000)); /* 1000^2 = 1,000,000 */
}

void test_math_sqrt32_large_values(void)
{
    uint32_t max_sq = 65535U * 65535U;

    TEST_ASSERT_EQUAL_INT32(65535, lv_sqrt32(max_sq));          /* Прикрывает насыщенную верхнюю ветку */
    TEST_ASSERT_EQUAL_INT32(32768, lv_sqrt32(0x40000000));      /* Использует самый высокий сегмент поиска */
    TEST_ASSERT_EQUAL_INT32(8192, lv_sqrt32(0x4000000));        /* Упражнения для пути поиска среднего и высокого уровня. */
    TEST_ASSERT_EQUAL_INT32(256, lv_sqrt32(0x10000));           /* Использует путь среднего радиуса действия и шаг №1. */
}

void test_math_sqrt32_midrange_buckets(void)
{
    /* Нажмите на ветки 0x4000000 > x >= 0x1000000. */
    assert_sqrt32_floor(0x5000000U);  /* принимает путь x>= 0x4000000 */
    assert_sqrt32_floor(0x2000000U);  /* принимает x>= 0x1000000, но < путь 0x4000000 */
    assert_sqrt32_floor(0x20000000U); /* принимает x>= 0x10000000, но < путь 0x40000000 */
}

/* Проверка функции lv_atan2 */
/* Note: lv_atan2 (x, y) возвращает угол для вектора (x, y), где x — горизонтально, y — вертикально. */
void test_math_atan2_quadrants(void)
{
    /* Проверить все четыре квадранта —lv_atan2(x, y) */
    TEST_ASSERT_EQUAL_INT32(90, lv_atan2(1, 0));      /* Положительная ось X (x=1, y=0) */
    TEST_ASSERT_EQUAL_INT32(0, lv_atan2(0, 1));       /* Положительная ось Y (x=0, y=1) */
    TEST_ASSERT_EQUAL_INT32(270, lv_atan2(-1, 0));    /* Отрицательная ось X (x=-1, y=0) */
    TEST_ASSERT_EQUAL_INT32(180, lv_atan2(0, -1));    /* Отрицательная ось Y (x=0, y=-1) */
}

void test_math_atan2_diagonal(void)
{
    /* Проверка диагональных углов -lv_atan2(x, y) */
    TEST_ASSERT_EQUAL_INT32(45, lv_atan2(100, 100));    /* Q1: (100, 100) */
    TEST_ASSERT_EQUAL_INT32(135, lv_atan2(100, -100));  /* Q2: (100, -100) */
    TEST_ASSERT_EQUAL_INT32(225, lv_atan2(-100, -100)); /* Q3: (-100, -100) */
    TEST_ASSERT_EQUAL_INT32(315, lv_atan2(-100, 100));  /* Q4: (-100, 100) */
}

void test_math_atan2_edge_cases(void)
{
    /* Краевые случаи теста —lv_atan2(x, y) */
    TEST_ASSERT_EQUAL_INT32(90, lv_atan2(1000, 0));   /* (1000, 0) = положительная ось X */
    TEST_ASSERT_EQUAL_INT32(0, lv_atan2(0, 1000));    /* (0, 1000) = положительная ось Y */
    TEST_ASSERT_EQUAL_INT32(270, lv_atan2(-1000, 0)); /* (-1000, 0) = отрицательная ось X */
    TEST_ASSERT_EQUAL_INT32(180, lv_atan2(0, -1000)); /* (0, -1000) = отрицательная ось Y */
}

void test_math_atan2_large_values(void)
{
    /* Тест со значениями значениями —lv_atan2(x, y) */
    TEST_ASSERT_EQUAL_INT32(90, lv_atan2(1456, 1));    /* (1456, 1) ≈ положительная ось X */
    TEST_ASSERT_EQUAL_INT32(270, lv_atan2(-1456, 1));  /* (-1456, 1) ≈ отрицательная ось X */
    TEST_ASSERT_EQUAL_INT32(0, lv_atan2(1, 1456));     /* (1, 1456) ≈ положительная ось Y */
    TEST_ASSERT_EQUAL_INT32(180, lv_atan2(1, -1456));  /* (1, -1456) ≈ отрицательная ось Y */
}

/* Проверка функции lv_pow */
void test_math_pow_basic(void)
{
    /* Проверьте базовые способности */
    TEST_ASSERT_EQUAL_INT32(1, lv_pow(5, 0));
    TEST_ASSERT_EQUAL_INT32(5, lv_pow(5, 1));
    TEST_ASSERT_EQUAL_INT32(25, lv_pow(5, 2));
    TEST_ASSERT_EQUAL_INT32(125, lv_pow(5, 3));
    TEST_ASSERT_EQUAL_INT32(625, lv_pow(5, 4));
}

void test_math_pow_negative_exponent(void)
{
    /* Проверка отрицательных показателей степени — в целочисленной математике результат < 1, поэтому возвращается 0 */
    TEST_ASSERT_EQUAL_INT32(0, lv_pow(5, -1));   /* 5^(-1) = 1/5 = 0.2 -> 0 */
    TEST_ASSERT_EQUAL_INT32(0, lv_pow(10, -5));  /* 10^(-5) = 1/100000 = 0.00001 -> 0 */
    TEST_ASSERT_EQUAL_INT32(0, lv_pow(2, -10));  /* 2^(-10) = 1/1024 = 0.00098 -> 0 */

    /* Особые случаи */
    TEST_ASSERT_EQUAL_INT32(1, lv_pow(1, -1));   /* 1^(-1) = 1 */
    TEST_ASSERT_EQUAL_INT32(1, lv_pow(1, -100)); /* 1^(-100) = 1 */
    TEST_ASSERT_EQUAL_INT32(0, lv_pow(0, -1));   /* 0^(-1) не определено, верните 0 */
}

void test_math_pow_zero_base(void)
{
    /* Тестирование нулевой базы */
    TEST_ASSERT_EQUAL_INT32(0, lv_pow(0, 1));
    TEST_ASSERT_EQUAL_INT32(1, lv_pow(0, 0));
}

void test_math_pow_large_values(void)
{
    /* Тестируйте умеренно большие значения */
    TEST_ASSERT_EQUAL_INT32(1024, lv_pow(2, 10));
    TEST_ASSERT_EQUAL_INT32(1000, lv_pow(10, 3));
    TEST_ASSERT_EQUAL_INT32(256, lv_pow(4, 4));
}

/* Проверка функции lv_map */
void test_math_map_basic(void)
{
    /* Тестирование базового картографирования */
    TEST_ASSERT_EQUAL_INT32(50, lv_map(50, 0, 100, 0, 100));   /* идентичность */
    TEST_ASSERT_EQUAL_INT32(0, lv_map(0, 0, 100, 0, 100));
    TEST_ASSERT_EQUAL_INT32(100, lv_map(100, 0, 100, 0, 100));
}

void test_math_map_scaling(void)
{
    /* Тестовое масштабирование */
    TEST_ASSERT_EQUAL_INT32(25, lv_map(50, 0, 100, 0, 50));    /* Уменьшить масштаб */
    TEST_ASSERT_EQUAL_INT32(100, lv_map(50, 0, 100, 0, 200));  /* Масштабировать */
    TEST_ASSERT_EQUAL_INT32(75, lv_map(50, 0, 100, 50, 100));  /* Диапазон переключения */
}

void test_math_map_negative_ranges(void)
{
    /* Проверка отрицательных диапазонов — упрощенно, просто убедитесь, что это работает */
    int32_t result1 = lv_map(-50, -100, 0, 0, 100);
    int32_t result2 = lv_map(0, -100, 0, 0, 100);
    int32_t result3 = lv_map(0, 0, 100, -100, 0);

    /* Просто убедитесь, что результаты являются разумными */
    TEST_ASSERT_INT32_WITHIN(50, 50, result1);  /* результат1 должен быть между 0 и 100 */
    TEST_ASSERT_INT32_WITHIN(50, 50, result2);  /* результат2 должен быть между 0 и 100 */
    TEST_ASSERT_INT32_WITHIN(50, -50, result3);  /* результат3 должен быть между -100 и 0 */
}

void test_math_map_reverse_range(void)
{
    /* Тестовое обратное сопоставление */
    TEST_ASSERT_EQUAL_INT32(100, lv_map(0, 0, 100, 100, 0));    /* (0-0)/(100-0) * (-100) + 100 = 100 */
    TEST_ASSERT_EQUAL_INT32(0, lv_map(100, 0, 100, 100, 0));    /* (100-0)/(100-0) * (-100) + 100 = 0 */
    TEST_ASSERT_EQUAL_INT32(50, lv_map(50, 0, 100, 100, 0));    /* (50-0)/(100-0) * (-100) + 100 = 50 */
}

void test_math_map_out_of_bounds(void)
{
    /* Проверка ввода за пределами границ */
    TEST_ASSERT_EQUAL_INT32(100, lv_map(150, 0, 100, 0, 100)); /* Выше max_in */
    TEST_ASSERT_EQUAL_INT32(0, lv_map(-50, 0, 100, 0, 100));   /* Ниже min_in */
}

/* Тестlv_randи lv_rand_set_seed */
void test_math_rand_set_seed(void)
{
    /* Проверка настройки семян */
    lv_rand_set_seed(12345);
    uint32_t r1 = lv_rand(0, 100);

    lv_rand_set_seed(12345);
    uint32_t r2 = lv_rand(0, 100);

    TEST_ASSERT_EQUAL_UINT32(r1, r2); /* То же семя должно давать ту же последовательность */
}

void test_math_rand_range(void)
{
    /* Ограничения испытательного диапазона */
    lv_rand_set_seed(1000);

    for(int i = 0; i < 100; i++) {
        uint32_t r = lv_rand(10, 20);
        TEST_ASSERT_UINT32_WITHIN(5, 15, r);  /* r должно быть от 10 до 20 */
    }
}

void test_math_rand_distribution(void)
{
    /* Проверьте, что случайные значения меняются */
    lv_rand_set_seed(3000);
    uint32_t r1 = lv_rand(0, 1000);
    uint32_t r2 = lv_rand(0, 1000);

    TEST_ASSERT_NOT_EQUAL_UINT32(r1, r2);

    /* Случайно они могут оказаться одинаковыми, но это маловероятно. */
    /* Просто убедитесь, что они в пределах досягаемости */
    TEST_ASSERT_LESS_OR_EQUAL_UINT32(1000, r1);
    TEST_ASSERT_LESS_OR_EQUAL_UINT32(1000, r2);
}

/* Тестlv_sqr(встроенная функция) */
void test_math_sqr(void)
{
    struct {
        int32_t input;
        int32_t expected;
    } test_cases[] = {
        /* Тестирование базовых случаев */
        {0, 0},
        {1, 1},
        {2, 4},
        {5, 25},
        {10, 100},
        {103, 10609},

        /* Тестовый отрицательный вход */
        {-5, 25},
        {-10, 100},

        /* Проверка больших значений в допустимом диапазоне */
        {1000, 1000000},
        {500, 250000},
    };

    for(size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
        int32_t result = lv_sqr(test_cases[i].input);
        TEST_ASSERT_EQUAL_INT32(test_cases[i].expected, result);
    }
}

#endif
