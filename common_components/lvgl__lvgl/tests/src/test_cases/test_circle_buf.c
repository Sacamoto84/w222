#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static bool test_fill_cb(void * buf, uint32_t len, int32_t idx, void * user)
{
    TEST_ASSERT_EQUAL_UINT32(4, len);
    TEST_ASSERT_NULL(user);
    *(int32_t *)buf = idx * 10;
    return true;
}

static lv_circle_buf_t * circle_buf;

#define circle_buf_CAPACITY 4

void setUp(void)
{
    circle_buf = lv_circle_buf_create(circle_buf_CAPACITY, sizeof(int32_t));

    TEST_ASSERT_EQUAL_UINT32(lv_circle_buf_capacity(circle_buf), circle_buf_CAPACITY);
    TEST_ASSERT_EQUAL_UINT32(0, lv_circle_buf_size(circle_buf));

    /**
     * Запишите значения в буфер круга. Максимальный размер буфера — circle_buf_CAPACITY.
     * Когда буфер заполнен, операция записи должна вернуть LV_RESULT_INVALID.
     */
    for(int32_t i = 0; i < circle_buf_CAPACITY * 2; i++) {
        const lv_result_t res = lv_circle_buf_write(circle_buf, &i);

        if(i < circle_buf_CAPACITY) TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        else TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);
    }

    /**
     * После записи значений в буфер размер буфера должен быть равен емкости.
     */
    TEST_ASSERT_EQUAL_UINT32(lv_circle_buf_size(circle_buf), circle_buf_CAPACITY);
}

void tearDown(void)
{
    lv_circle_buf_destroy(circle_buf);
    circle_buf = NULL;
}

void test_circle_buf_read_write_peek_values(void)
{
    /**
     * Считайте 1 значение из буфера.
     */
    {
        int32_t value;
        const lv_result_t res = lv_circle_buf_read(circle_buf, &value);

        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        TEST_ASSERT_EQUAL_INT32(0, value);
    }

    /**
     * Пиковые значения не будут перемещать курсоры чтения и записи.
     * Если индекс просмотра больше размера буфера, он будет возвращаться циклически.
     */
    for(int32_t i = 0, j = 1; i < circle_buf_CAPACITY * 10; i++, j++) {
        int32_t value;
        const lv_result_t res = lv_circle_buf_peek_at(circle_buf, i, &value);

        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        TEST_ASSERT_EQUAL_INT32(j, value);

        if(j == 3) j = 0;
    }

    /**
     * Прочтите значение круглого буфера. Максимальный размер буфера — circle_buf_CAPACITY.
     * Когда буфер пуст, операция чтения должна вернуть LV_RESULT_INVALID.
     */
    for(int32_t i = 1; i < circle_buf_CAPACITY * 2; i++) {
        int32_t value;
        const lv_result_t res = lv_circle_buf_read(circle_buf, &value);

        if(i < circle_buf_CAPACITY) {
            TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
            TEST_ASSERT_EQUAL_INT32(i, value);
        }
        else {
            TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);
        }
    }

    /**
     * После чтения значений из буфера размер буфера должен быть равен 0.
     */
    TEST_ASSERT_EQUAL_INT32(0, lv_circle_buf_size(circle_buf));
}

void test_circle_buf_skip_values(void)
{
    /**
     * Пропустить 1 значение из буфера.
     */
    {
        const lv_result_t res = lv_circle_buf_skip(circle_buf);

        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    }

    /**
     * Пропустить значения из кругового буфера. Максимальный размер буфера — circle_buf_CAPACITY.
     * Когда буфер пуст, операция пропуска должна вернуть LV_RESULT_INVALID.
     */
    for(int32_t i = 1; i < circle_buf_CAPACITY * 2; i++) {
        const lv_result_t res = lv_circle_buf_skip(circle_buf);

        if(i < circle_buf_CAPACITY) {
            TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        }
        else {
            TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);
        }
    }

    /**
     * После пропуска значений из буфера размер буфера должен быть равен 0.
     */
    TEST_ASSERT_EQUAL_INT32(0, lv_circle_buf_size(circle_buf));
}

void test_circle_buf_read_after_read_and_write(void)
{
    /**
     * Считайте 1 значение из буфера.
     */
    {
        int32_t value;
        const lv_result_t res = lv_circle_buf_read(circle_buf, &value);

        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        TEST_ASSERT_EQUAL_INT32(0, value);
    }

    /**
     * Запишите 1 значение в буфер.
     */
    {
        const int32_t value = 4;
        const lv_result_t res = lv_circle_buf_write(circle_buf, &value);

        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    }

    const int32_t expected[] = {4, 1, 2, 3};
    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, ((lv_array_t *)circle_buf)->data, 4);

    /**
     * Прочтите значение круглого буфера. Максимальный размер буфера — circle_buf_CAPACITY.
     * Когда буфер пуст, операция чтения должна вернуть LV_RESULT_INVALID.
     */
    for(int32_t i = 1; i < circle_buf_CAPACITY * 2; i++) {
        int32_t value;
        const lv_result_t res = lv_circle_buf_read(circle_buf, &value);

        if(i <= circle_buf_CAPACITY) {
            TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
            TEST_ASSERT_EQUAL_INT32(i, value);
        }
        else {
            TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);
        }
    }
}

void test_circle_buf_reset(void)
{
    /**
     * Сбросьте буфер круга, теперь буфер должен быть пустым, как иlv_circle_buf_create.
     */
    lv_circle_buf_reset(circle_buf);

    TEST_ASSERT_EQUAL_UINT32(lv_circle_buf_capacity(circle_buf), circle_buf_CAPACITY);
    TEST_ASSERT_EQUAL_UINT32(0, lv_circle_buf_size(circle_buf));

    /**
     * Запишите значения в буфер круга. Максимальный размер буфера — circle_buf_CAPACITY.
     * Когда буфер заполнен, операция записи должна вернуть LV_RESULT_INVALID.
     */
    for(int32_t i = 0; i < circle_buf_CAPACITY * 2; i++) {
        const lv_result_t res = lv_circle_buf_write(circle_buf, &i);

        if(i < circle_buf_CAPACITY) TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        else TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);
    }

    /**
     * После записи значений в буфер размер буфера должен быть равен емкости.
     */
    TEST_ASSERT_EQUAL_UINT32(lv_circle_buf_size(circle_buf), circle_buf_CAPACITY);

    /**
     * Прочтите значение круглого буфера. Максимальный размер буфера — circle_buf_CAPACITY.
     * Когда буфер пуст, операция чтения должна вернуть LV_RESULT_INVALID.
     */
    for(int32_t i = 0; i < circle_buf_CAPACITY * 2; i++) {
        int32_t value;
        const lv_result_t res = lv_circle_buf_read(circle_buf, &value);

        if(i < circle_buf_CAPACITY) {
            TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
            TEST_ASSERT_EQUAL_INT32(i, value);
        }
        else {
            TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);
        }
    }
}

void test_circle_buf_create_methods(void)
{
    /* Тестовое создание из внешнего буфера */
    int32_t ext_buf[4];
    lv_circle_buf_t * ext_cb = lv_circle_buf_create_from_buf(ext_buf, 4, sizeof(int32_t));
    TEST_ASSERT_NOT_NULL(ext_cb);

    /* Проверьте возможность записи во внешний буфер */
    int32_t test_val = 5;
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_circle_buf_write(ext_cb, &test_val));
    lv_circle_buf_destroy(ext_cb);

    /* Тестовое создание из массива */
    lv_array_t arr;
    lv_array_init(&arr, 4, sizeof(int32_t));
    lv_circle_buf_t * arr_cb = lv_circle_buf_create_from_array(&arr);
    TEST_ASSERT_NOT_NULL(arr_cb);

    /* Проверка возможности записи в буфер на основе массива */
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_circle_buf_write(arr_cb, &test_val));
    lv_circle_buf_destroy(arr_cb);
}

void test_circle_buf_fill(void)
{
    lv_circle_buf_reset(circle_buf);

    /* Обратный вызов для тестового заполнения */
    uint32_t filled = lv_circle_buf_fill(circle_buf, 4, test_fill_cb, NULL);

    TEST_ASSERT_EQUAL_UINT32(4, filled);
    TEST_ASSERT_TRUE(lv_circle_buf_is_full(circle_buf));

    /* Проверьте заполненные значения */
    for(int i = 0; i < 4; i++) {
        int32_t val = -1;
        TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_circle_buf_peek_at(circle_buf, i, &val));
        TEST_ASSERT_EQUAL_INT32(i * 10, val);
    }
}

void test_circle_buf_edge_cases(void)
{
    lv_circle_buf_reset(circle_buf);
    /* Тестовое чтение, когда оно пусто */
    int32_t val;
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_circle_buf_read(circle_buf, &val));

    /* Пропуск теста, когда он пуст */
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_circle_buf_skip(circle_buf));
}

void test_circle_buf_resize(void)
{
    /* Тестирование изменения размера до большей емкости */
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_circle_buf_resize(circle_buf, 8));
    TEST_ASSERT_EQUAL_UINT32(8, lv_circle_buf_capacity(circle_buf));
    TEST_ASSERT_EQUAL_UINT32(0, lv_circle_buf_size(circle_buf));

    /* Тестирование изменения размера до меньшей емкости */
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_circle_buf_resize(circle_buf, 2));
    TEST_ASSERT_EQUAL_UINT32(2, lv_circle_buf_capacity(circle_buf));
    TEST_ASSERT_EQUAL_UINT32(0, lv_circle_buf_size(circle_buf));

    /* Тестовое изменение размера до нулевой емкости */
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_circle_buf_resize(circle_buf, 0));
    TEST_ASSERT_EQUAL_UINT32(0, lv_circle_buf_capacity(circle_buf));
    TEST_ASSERT_EQUAL_UINT32(0, lv_circle_buf_size(circle_buf));
}

#endif
