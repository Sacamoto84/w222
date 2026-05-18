#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#define PCT_MAX_VALUE 268435455

void setUp(void)
{
    /* Функция запускается перед каждым тестом */
}

void tearDown(void)
{
    /* Функция запускается после каждого теста */
    lv_obj_clean(lv_screen_active());
}

void test_pct(void)
{
    int32_t pct_val;
    int32_t pct_coord;

    pct_val = 0;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(pct_val, LV_COORD_GET_PCT(pct_coord));

    pct_val = 1;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(pct_val, LV_COORD_GET_PCT(pct_coord));

    pct_val = 100;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(pct_val, LV_COORD_GET_PCT(pct_coord));

    pct_val = 111111111;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(pct_val, LV_COORD_GET_PCT(pct_coord));

    pct_val = PCT_MAX_VALUE;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(pct_val, LV_COORD_GET_PCT(pct_coord));

    pct_val = -1;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(pct_val, LV_COORD_GET_PCT(pct_coord));

    pct_val = -100;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(pct_val, LV_COORD_GET_PCT(pct_coord));

    pct_val = -111111111;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(pct_val, LV_COORD_GET_PCT(pct_coord));

    pct_val = -PCT_MAX_VALUE;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(pct_val, LV_COORD_GET_PCT(pct_coord));

    /**
     * Поведение, выходящее за рамки.
     * Значение pct будет ограничено значением макс/мин, если оно выходит за пределы.
    */

    pct_val = PCT_MAX_VALUE + 1;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(PCT_MAX_VALUE, LV_COORD_GET_PCT(pct_coord));

    pct_val = PCT_MAX_VALUE + 100;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(PCT_MAX_VALUE, LV_COORD_GET_PCT(pct_coord));

    pct_val = -PCT_MAX_VALUE - 1;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(-PCT_MAX_VALUE, LV_COORD_GET_PCT(pct_coord));

    pct_val = -PCT_MAX_VALUE - 100;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(-PCT_MAX_VALUE, LV_COORD_GET_PCT(pct_coord));
}

void test_area_diff(void)
{
    lv_area_t expected[4] = {
        {0,  0,  100, 29},  /* широкий верхний прямоугольник */
        {0,  91, 100, 100}, /* широкий нижний прямоугольник */
        {0,  30, 39,  90},  /* левый прямоугольник */
        {81, 30, 100, 90}   /* правый прямоугольник */
    };
    lv_area_t outer = {0, 0, 100, 100};
    lv_area_t remove = {40, 30, 80, 90};
    lv_area_t actual[4];
    lv_memset(actual, 0, sizeof(actual));

    int8_t area_count = lv_area_diff(actual, &outer, &remove);
    TEST_ASSERT_EQUAL_INT8(area_count, 4);

    TEST_PRINTF("%d %d %d %d", actual[0].x1, actual[0].y1, actual[0].x2, actual[0].y2);
    TEST_ASSERT_TRUE(lv_area_is_equal(&expected[0], &actual[0]));
    TEST_PRINTF("%d %d %d %d", actual[1].x1, actual[1].y1, actual[1].x2, actual[1].y2);
    TEST_ASSERT_TRUE(lv_area_is_equal(&expected[1], &actual[1]));
    TEST_PRINTF("%d %d %d %d", actual[2].x1, actual[2].y1, actual[2].x2, actual[2].y2);
    TEST_ASSERT_TRUE(lv_area_is_equal(&expected[2], &actual[2]));
    TEST_PRINTF("%d %d %d %d", actual[3].x1, actual[3].y1, actual[3].x2, actual[3].y2);
    TEST_ASSERT_TRUE(lv_area_is_equal(&expected[3], &actual[3]));


    /* нет левого края */
    lv_area_set(&remove, 0, 10, 10, 20);
    area_count = lv_area_diff(actual, &outer, &remove);
    TEST_ASSERT_EQUAL_INT8(area_count, 3);

    lv_area_set(&remove, 1, 10, 10, 20);
    area_count = lv_area_diff(actual, &outer, &remove);
    TEST_ASSERT_EQUAL_INT8(area_count, 4);

    /* нет правого края */
    lv_area_set(&remove, 90, 10, 100, 20);
    area_count = lv_area_diff(actual, &outer, &remove);
    TEST_ASSERT_EQUAL_INT8(area_count, 3);

    lv_area_set(&remove, 90, 10, 99, 20);
    area_count = lv_area_diff(actual, &outer, &remove);
    TEST_ASSERT_EQUAL_INT8(area_count, 4);

    /* нет верхнего края */
    lv_area_set(&remove, 10, 0, 20, 20);
    area_count = lv_area_diff(actual, &outer, &remove);
    TEST_ASSERT_EQUAL_INT8(area_count, 3);

    lv_area_set(&remove, 10, 1, 20, 20);
    area_count = lv_area_diff(actual, &outer, &remove);
    TEST_ASSERT_EQUAL_INT8(area_count, 4);

    /* нет нижнего края */
    lv_area_set(&remove, 10, 90, 20, 100);
    area_count = lv_area_diff(actual, &outer, &remove);
    TEST_ASSERT_EQUAL_INT8(area_count, 3);

    lv_area_set(&remove, 10, 90, 20, 99);
    area_count = lv_area_diff(actual, &outer, &remove);
    TEST_ASSERT_EQUAL_INT8(area_count, 4);


    /* отсутствуют верхний и левый края */
    lv_area_set(&remove, -50, -50, 50, 50);
    area_count = lv_area_diff(actual, &outer, &remove);
    TEST_ASSERT_EQUAL_INT8(area_count, 2);
}

#define GRID_WIDTH 5
#define GRID_HEIGHT 5

static void grid_assert_equal(char grid1[GRID_HEIGHT][GRID_WIDTH], char grid2[GRID_HEIGHT][GRID_WIDTH])
{
    for(int y = 0; y < GRID_HEIGHT; y++) {
        for(int x = 0; x < GRID_WIDTH; x++) {
            TEST_ASSERT_EQUAL_CHAR(grid1[y][x], grid2[y][x]);
        }
    }
}

static void grid_mark(char grid[GRID_HEIGHT][GRID_WIDTH], const lv_area_t * area, char c, bool assert_change)
{
    for(int y = area->y1; y <= area->y2; y++) {
        for(int x = area->x1; x <= area->x2; x++) {
            if(assert_change) TEST_ASSERT_NOT_EQUAL_CHAR(grid[y][x], c);
            grid[y][x] = c;
        }
    }
}

static void grid_mark_area_diff_simple(char grid[GRID_HEIGHT][GRID_WIDTH], const lv_area_t * outer,
                                       const lv_area_t * remove)
{
    grid_mark(grid, outer, 'x', true);

    lv_area_t intersection;
    if(lv_area_intersect(&intersection, outer, remove)) {
        grid_mark(grid, &intersection, ' ', true);
    }
}

static void grid_mark_area_diff(char grid[GRID_HEIGHT][GRID_WIDTH], const lv_area_t * outer, const lv_area_t * remove)
{
    lv_area_t parts[4];
    int8_t num_parts = lv_area_diff(parts, outer, remove);

    if(num_parts < 0) {
        grid_mark(grid, outer, 'x', true);
    }
    for(int i = 0; i < num_parts; i++) {
        grid_mark(grid, &parts[i], 'x', true);
    }
}

void test_area_diff_property(void)
{
    lv_rand_set_seed(7875);

    char grid_simple[GRID_HEIGHT][GRID_WIDTH];
    char grid[GRID_HEIGHT][GRID_WIDTH];

    lv_area_t everything;
    everything.x1 = 0;
    everything.y1 = 0;
    everything.x2 = GRID_WIDTH - 1;
    everything.y2 = GRID_HEIGHT - 1;

    for(int test = 0; test < 200; test++) {
        lv_area_t outer;
        outer.x1 = lv_rand(0, GRID_WIDTH - 1);
        outer.y1 = lv_rand(0, GRID_HEIGHT - 1);
        outer.x2 = lv_rand(outer.x1, GRID_WIDTH - 1);
        outer.y2 = lv_rand(outer.y1, GRID_HEIGHT - 1);

        lv_area_t remove;
        remove.x1 = lv_rand(0, GRID_WIDTH - 1);
        remove.y1 = lv_rand(0, GRID_HEIGHT - 1);
        remove.x2 = lv_rand(remove.x1, GRID_WIDTH - 1);
        remove.y2 = lv_rand(remove.y1, GRID_HEIGHT - 1);

        TEST_PRINTF("%d %d %d %d / %d %d %d %d", outer.x1, outer.y1, outer.x2, outer.y2, remove.x1, remove.y1, remove.x2,
                    remove.y2);

        /*Отметьте оставшуюся площадь, используя простой алгоритм*/
        grid_mark(grid_simple, &everything, ' ', false);
        grid_mark_area_diff_simple(grid_simple, &outer, &remove);

        /*Учитывая оставшуюся область, используя более эффективныйlv_area_diff()*/
        grid_mark(grid, &everything, ' ', false);
        grid_mark_area_diff(grid, &outer, &remove);

        grid_assert_equal(grid_simple, grid);
    }
}

/* Тестовое пересечение прямоугольника */
void test_area_intersect(void)
{
    /* Нормальный перекрёсток */
    lv_area_t a1 = {10, 20, 100, 200};
    lv_area_t a2 = {50, 50, 150, 250};
    lv_area_t res;
    bool intersect = lv_area_intersect(&res, &a1, &a2);
    TEST_ASSERT_TRUE(intersect);
    TEST_ASSERT_EQUAL_INT32(50, res.x1);
    TEST_ASSERT_EQUAL_INT32(50, res.y1);
    TEST_ASSERT_EQUAL_INT32(100, res.x2);
    TEST_ASSERT_EQUAL_INT32(200, res.y2);

    /* Нет пересечения */
    lv_area_t a3 = {200, 300, 300, 400};
    intersect = lv_area_intersect(&res, &a1, &a3);
    TEST_ASSERT_FALSE(intersect);
}

/* Тестовое объединение прямоугольников */
void test_area_join(void)
{
    lv_area_t a1 = {10, 20, 100, 200};
    lv_area_t a2 = {50, 50, 150, 250};
    lv_area_t res;

    lv_area_join(&res, &a1, &a2);
    TEST_ASSERT_EQUAL_INT32(10, res.x1);
    TEST_ASSERT_EQUAL_INT32(20, res.y1);
    TEST_ASSERT_EQUAL_INT32(150, res.x2);
    TEST_ASSERT_EQUAL_INT32(250, res.y2);

    /* Одна область внутри другой */
    lv_area_t a3 = {20, 30, 80, 180};
    lv_area_join(&res, &a1, &a3);
    TEST_ASSERT_EQUAL_INT32(10, res.x1);
    TEST_ASSERT_EQUAL_INT32(20, res.y1);
    TEST_ASSERT_EQUAL_INT32(100, res.x2);
    TEST_ASSERT_EQUAL_INT32(200, res.y2);
}

/* Тестовая точка на прямоугольнике */
void test_area_is_point_on(void)
{
    lv_area_t a = {10, 20, 100, 200};
    lv_point_t p1 = {50, 50};
    TEST_ASSERT_TRUE(lv_area_is_point_on(&a, &p1, 0));

    lv_point_t p2 = {5, 5};
    TEST_ASSERT_FALSE(lv_area_is_point_on(&a, &p2, 0));

    /* Тест с радиусом */
    lv_point_t p3 = {15, 25};
    TEST_ASSERT_TRUE(lv_area_is_point_on(&a, &p3, 4));

    /* Контрольная точка на краю без радиуса */
    lv_point_t p4 = {10, 100};  /* Левый край */
    TEST_ASSERT_TRUE(lv_area_is_point_on(&a, &p4, 0));

    lv_point_t p5 = {100, 20};  /* Верхний край */
    TEST_ASSERT_TRUE(lv_area_is_point_on(&a, &p5, 0));

    /* Тестовая точка на углу */
    lv_point_t p6 = {100, 200};  /* Нижний правый угол */
    TEST_ASSERT_TRUE(lv_area_is_point_on(&a, &p6, 0));

}

/* Проверка отношений прямоугольника */
void test_area_relations(void)
{
    lv_area_t a1 = {10, 20, 100, 200};
    lv_area_t a2 = {50, 50, 80, 180};
    lv_area_t a3 = {200, 300, 300, 400};

    /* Тест is_on (перекрытие) */
    TEST_ASSERT_TRUE(lv_area_is_on(&a1, &a2));
    TEST_ASSERT_FALSE(lv_area_is_on(&a1, &a3));

    /* Тест is_in (содержит) */
    TEST_ASSERT_TRUE(lv_area_is_in(&a2, &a1, 0));
    TEST_ASSERT_FALSE(lv_area_is_in(&a1, &a2, 0));

    /* Тест is_out */
    TEST_ASSERT_TRUE(lv_area_is_out(&a3, &a1, 0));
    TEST_ASSERT_FALSE(lv_area_is_out(&a2, &a1, 0));

    /* Тест is_equal */
    TEST_ASSERT_TRUE(lv_area_is_equal(&a1, &a1));
    TEST_ASSERT_FALSE(lv_area_is_equal(&a1, &a2));
}

/* Выравнивание тестовой зоны */
void test_area_align(void)
{
    lv_area_t base = {0, 0, 200, 200};
    lv_area_t to_align = {0, 0, 50, 50};

    /* Выравнивание испытательного центра */
    lv_area_align(&base, &to_align, LV_ALIGN_CENTER, 0, 0);
    TEST_ASSERT_EQUAL_INT32(75, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(75, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(125, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(125, to_align.y2);

    /* Проверка выравнивания верхнего левого угла со смещением */
    lv_area_align(&base, &to_align, LV_ALIGN_TOP_LEFT, 10, 20);
    TEST_ASSERT_EQUAL_INT32(10, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(20, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(60, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(70, to_align.y2);

    /* Проверьте выравнивание верхней середины */
    lv_area_align(&base, &to_align, LV_ALIGN_TOP_MID, 0, 0);
    TEST_ASSERT_EQUAL_INT32(75, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(0, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(125, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(50, to_align.y2);

    /* Проверка выравнивания верхнего правого угла со смещением */
    lv_area_align(&base, &to_align, LV_ALIGN_TOP_RIGHT, -10, 5);
    TEST_ASSERT_EQUAL_INT32(140, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(5, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(190, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(55, to_align.y2);

    /* Проверьте выравнивание левой середины */
    lv_area_align(&base, &to_align, LV_ALIGN_LEFT_MID, 0, 0);
    TEST_ASSERT_EQUAL_INT32(0, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(75, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(50, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(125, to_align.y2);

    /* Проверьте выравнивание по середине справа со смещением */
    lv_area_align(&base, &to_align, LV_ALIGN_RIGHT_MID, 5, -5);
    TEST_ASSERT_EQUAL_INT32(155, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(70, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(205, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(120, to_align.y2);

    /* Проверьте выравнивание по левому нижнему краю */
    lv_area_align(&base, &to_align, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    TEST_ASSERT_EQUAL_INT32(0, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(150, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(50, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(200, to_align.y2);

    /* Проверьте выравнивание нижней части посередине со смещением */
    lv_area_align(&base, &to_align, LV_ALIGN_BOTTOM_MID, 0, -10);
    TEST_ASSERT_EQUAL_INT32(75, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(140, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(125, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(190, to_align.y2);

    /* Проверьте выравнивание по правому нижнему краю */
    lv_area_align(&base, &to_align, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    TEST_ASSERT_EQUAL_INT32(150, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(150, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(200, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(200, to_align.y2);

    /* Проверка внешнего выравнивания */
    lv_area_align(&base, &to_align, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
    TEST_ASSERT_EQUAL_INT32(0, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(-51, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(50, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(-1, to_align.y2);

    lv_area_align(&base, &to_align, LV_ALIGN_OUT_TOP_MID, 0, 0);
    TEST_ASSERT_EQUAL_INT32(75, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(-51, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(125, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(-1, to_align.y2);

    lv_area_align(&base, &to_align, LV_ALIGN_OUT_TOP_RIGHT, 0, 0);
    TEST_ASSERT_EQUAL_INT32(150, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(-51, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(200, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(-1, to_align.y2);

    lv_area_align(&base, &to_align, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    TEST_ASSERT_EQUAL_INT32(0, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(201, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(50, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(251, to_align.y2);

    lv_area_align(&base, &to_align, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    TEST_ASSERT_EQUAL_INT32(75, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(201, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(125, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(251, to_align.y2);

    lv_area_align(&base, &to_align, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
    TEST_ASSERT_EQUAL_INT32(150, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(201, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(200, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(251, to_align.y2);

    lv_area_align(&base, &to_align, LV_ALIGN_OUT_LEFT_TOP, 0, 0);
    TEST_ASSERT_EQUAL_INT32(-51, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(0, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(-1, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(50, to_align.y2);

    lv_area_align(&base, &to_align, LV_ALIGN_OUT_LEFT_MID, 0, 0);
    TEST_ASSERT_EQUAL_INT32(-51, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(75, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(-1, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(125, to_align.y2);

    lv_area_align(&base, &to_align, LV_ALIGN_OUT_LEFT_BOTTOM, 0, 0);
    TEST_ASSERT_EQUAL_INT32(-51, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(150, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(-1, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(200, to_align.y2);

    lv_area_align(&base, &to_align, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);
    TEST_ASSERT_EQUAL_INT32(201, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(0, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(251, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(50, to_align.y2);

    lv_area_align(&base, &to_align, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    TEST_ASSERT_EQUAL_INT32(201, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(75, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(251, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(125, to_align.y2);

    lv_area_align(&base, &to_align, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, 0);
    TEST_ASSERT_EQUAL_INT32(201, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(150, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(251, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(200, to_align.y2);

    /* Тест со смещениями */
    lv_area_align(&base, &to_align, LV_ALIGN_OUT_TOP_MID, 10, 20);
    TEST_ASSERT_EQUAL_INT32(85, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(-31, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(135, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(19, to_align.y2);

    lv_area_align(&base, &to_align, LV_ALIGN_OUT_LEFT_MID, -5, 10);
    TEST_ASSERT_EQUAL_INT32(-56, to_align.x1);
    TEST_ASSERT_EQUAL_INT32(85, to_align.y1);
    TEST_ASSERT_EQUAL_INT32(-6, to_align.x2);
    TEST_ASSERT_EQUAL_INT32(135, to_align.y2);

    /* Тестовый случай по умолчанию с неверным выравниванием */
    lv_area_align(&base, &to_align, (lv_align_t)100, 10, 20);
    TEST_ASSERT_EQUAL_INT32(10, to_align.x1);  /* база.x1 +ofs_x= 0 + 10 */
    TEST_ASSERT_EQUAL_INT32(20, to_align.y1);  /* base.y1 + ofs_y = 0 + 20 */
    TEST_ASSERT_EQUAL_INT32(60, to_align.x2);  /* х1 + ширина – 1 = 10 + 51 – 1 */
    TEST_ASSERT_EQUAL_INT32(70, to_align.y2);  /* y1 + высота - 1 = 20 + 51 - 1 */
}

void test_area_set_pos(void)
{
    lv_area_t area = {10, 20, 100, 200};  /* Исходная область */
    int32_t original_width = lv_area_get_width(&area);
    int32_t original_height = lv_area_get_height(&area);

    /* Проверка базовой настройки положения */
    lv_area_set_pos(&area, 50, 60);
    TEST_ASSERT_EQUAL_INT32(50, area.x1);
    TEST_ASSERT_EQUAL_INT32(60, area.y1);
    TEST_ASSERT_EQUAL_INT32(50 + original_width - 1, area.x2);
    TEST_ASSERT_EQUAL_INT32(60 + original_height - 1, area.y2);

    /* Тестовая настройка на минимальные координаты */
    lv_area_set_pos(&area, 0, 0);
    TEST_ASSERT_EQUAL_INT32(0, area.x1);
    TEST_ASSERT_EQUAL_INT32(0, area.y1);
    TEST_ASSERT_EQUAL_INT32(original_width - 1, area.x2);
    TEST_ASSERT_EQUAL_INT32(original_height - 1, area.y2);

    /* Тестовая настройка на отрицательные координаты */
    lv_area_set_pos(&area, -10, -20);
    TEST_ASSERT_EQUAL_INT32(-10, area.x1);
    TEST_ASSERT_EQUAL_INT32(-20, area.y1);
    TEST_ASSERT_EQUAL_INT32(-10 + original_width - 1, area.x2);
    TEST_ASSERT_EQUAL_INT32(-20 + original_height - 1, area.y2);

    /* Убедитесь, что ширина и высота остались неизменными. */
    TEST_ASSERT_EQUAL_INT32(original_width, lv_area_get_width(&area));
    TEST_ASSERT_EQUAL_INT32(original_height, lv_area_get_height(&area));
}

/* Функция точной замены контрольной точки */
void test_point_precise_swap(void)
{
    lv_point_precise_t p1 = {10.5f, 20.7f};
    lv_point_precise_t p2 = {30.2f, 40.9f};

    /* Сохраните исходные значения для проверки */
    lv_point_precise_t original_p1 = p1;
    lv_point_precise_t original_p2 = p2;

    /* Обменяйтесь баллами */
    lv_point_precise_swap(&p1, &p2);

    /* Убедитесь, что точки поменялись местами правильно. */
    TEST_ASSERT_EQUAL_FLOAT(original_p2.x, p1.x);
    TEST_ASSERT_EQUAL_FLOAT(original_p2.y, p1.y);
    TEST_ASSERT_EQUAL_FLOAT(original_p1.x, p2.x);
    TEST_ASSERT_EQUAL_FLOAT(original_p1.y, p2.y);

    /* Тест с отрицательными значениями */
    lv_point_precise_t p3 = {-15.3f, -25.1f};
    lv_point_precise_t p4 = {35.7f, -45.8f};

    lv_point_precise_t original_p3 = p3;
    lv_point_precise_t original_p4 = p4;

    lv_point_precise_swap(&p3, &p4);

    TEST_ASSERT_EQUAL_FLOAT(original_p4.x, p3.x);
    TEST_ASSERT_EQUAL_FLOAT(original_p4.y, p3.y);
    TEST_ASSERT_EQUAL_FLOAT(original_p3.x, p4.x);
    TEST_ASSERT_EQUAL_FLOAT(original_p3.y, p4.y);

    /* Тест с нулевыми значениями */
    lv_point_precise_t p5 = {0.0f, 0.0f};
    lv_point_precise_t p6 = {100.0f, 200.0f};

    lv_point_precise_t original_p5 = p5;
    lv_point_precise_t original_p6 = p6;

    lv_point_precise_swap(&p5, &p6);

    TEST_ASSERT_EQUAL_FLOAT(original_p6.x, p5.x);
    TEST_ASSERT_EQUAL_FLOAT(original_p6.y, p5.y);
    TEST_ASSERT_EQUAL_FLOAT(original_p5.x, p6.x);
    TEST_ASSERT_EQUAL_FLOAT(original_p5.y, p6.y);
}

#endif
