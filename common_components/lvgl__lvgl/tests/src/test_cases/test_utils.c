#if LV_BUILD_TEST
#include "../lvgl.h"
#include "unity/unity.h"

#define TEST_DRAW_BUF_SAVE_PATH(path) "A:src/test_files/draw_buf_save_" path

/* Тестовая установка и демонтаж */
void setUp(void)
{
    /* Функция запускается перед каждым тестом */
}

void tearDown(void)
{
    /* Функция запускается после каждого теста */
    lv_obj_clean(lv_screen_active());
}

static int compare_int(const void * ref_p, const void * pElement)
{
    int ref = *(const int *)ref_p;
    int element = *(const int *)pElement;
    return ref - element;
}

static int compare_string(const void * ref_p, const void * pElement)
{
    const char * ref = *(const char **)ref_p;
    const char * element = *(const char **)pElement;
    return lv_strcmp(ref, element);
}

/* Проверить случай пустого массива */
void test_utils_bsearch_empty_array(void)
{
    int * array = NULL;
    int key = 5;
    int * result = (int *)lv_utils_bsearch(&key, array, 0, sizeof(int), compare_int);
    TEST_ASSERT_NULL(result);
}

/* Тестирование одноэлементного массива */
void test_utils_bsearch_single_element(void)
{
    int array[] = {5};

    /* Проверить существующий элемент */
    int key1 = 5;
    int * result1 = (int *)lv_utils_bsearch(&key1, array, 1, sizeof(int), compare_int);
    TEST_ASSERT_NOT_NULL(result1);
    TEST_ASSERT_EQUAL_INT(5, *result1);

    /* Проверить несуществующий элемент */
    int key2 = 1;
    int * result2 = (int *)lv_utils_bsearch(&key2, array, 1, sizeof(int), compare_int);
    TEST_ASSERT_NULL(result2);
}

/* Тест на поиск среднего элемента */
void test_utils_bsearch_middle_element(void)
{
    int array[] = {1, 3, 5, 7, 9};
    int key = 5;
    int * result = (int *)lv_utils_bsearch(&key, array, 5, sizeof(int), compare_int);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(5, *result);
}

/* Тестовый пример, когда ключ не найден */
void test_utils_bsearch_not_found(void)
{
    int array[] = {1, 3, 5, 7, 9};
    int key = 2;
    int * result = (int *)lv_utils_bsearch(&key, array, 5, sizeof(int), compare_int);
    TEST_ASSERT_NULL(result);
}

/* Тест на поиск первого элемента */
void test_utils_bsearch_first_element(void)
{
    int array[] = {1, 3, 5, 7, 9};
    int key = 1;
    int * result = (int *)lv_utils_bsearch(&key, array, 5, sizeof(int), compare_int);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(1, *result);
}

/* Тест на поиск последнего элемента */
void test_utils_bsearch_last_element(void)
{
    int array[] = {1, 3, 5, 7, 9};
    int key = 9;
    int * result = (int *)lv_utils_bsearch(&key, array, 5, sizeof(int), compare_int);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(9, *result);
}

/* Тестовый поиск в массиве строк */
void test_utils_bsearch_string_array(void)
{
    const char * array[] = {"apple", "banana", "cherry", "date", "elderberry"};
    const char * key = "cherry";
    const char ** result = (const char **)lv_utils_bsearch(&key, array, 5, sizeof(const char *), compare_string);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("cherry", *result);
}

/* Тестовая строка не найдена */
void test_utils_bsearch_string_not_found(void)
{
    const char * array[] = {"apple", "banana", "cherry", "date", "elderberry"};
    const char * key = "fig";
    const char ** result = (const char **)lv_utils_bsearch(&key, array, 5, sizeof(const char *), compare_string);
    TEST_ASSERT_NULL(result);
}

/* Тестовый поиск по повторяющимся ключам */
void test_utils_bsearch_repeated_key(void)
{
    const char * array[] = {"apple", "banana", "apple", "date", "elderberry"};
    const char * key = "apple";
    const char ** result = (const char **)lv_utils_bsearch(&key, array, 5, sizeof(const char *), compare_string);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("apple", *result);
}

/* Тест большого массива (1000 элементов) */
void test_utils_bsearch_large_array(void)
{
#define LARGE_ARRAY_SIZE 1000
    int large_array[LARGE_ARRAY_SIZE];
    for(int i = 0; i < LARGE_ARRAY_SIZE; i++) {
        large_array[i] = i * 2;
    }

    /* Тестирование поиска существующего элемента */
    int key = 500;
    int * result = (int *)lv_utils_bsearch(&key, large_array, LARGE_ARRAY_SIZE, sizeof(int), compare_int);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(500, *result);

    /* Проверить несуществующий элемент */
    key = 501;
    result = (int *)lv_utils_bsearch(&key, large_array, LARGE_ARRAY_SIZE, sizeof(int), compare_int);
    TEST_ASSERT_NULL(result);
}

/* Тестовый граничный случай n=2 - важен для алгоритма двоичного поиска */
void test_utils_bsearch_two_element_array(void)
{
    int array[] = {1, 3};

    /* Тест на поиск первого элемента */
    int key1 = 1;
    int * result1 = (int *)lv_utils_bsearch(&key1, array, 2, sizeof(int), compare_int);
    TEST_ASSERT_NOT_NULL(result1);
    TEST_ASSERT_EQUAL_INT(1, *result1);

    /* Тест на поиск второго элемента */
    int key2 = 3;
    int * result2 = (int *)lv_utils_bsearch(&key2, array, 2, sizeof(int), compare_int);
    TEST_ASSERT_NOT_NULL(result2);
    TEST_ASSERT_EQUAL_INT(3, *result2);

    /* Тестовый элемент меньше первого */
    int key3 = 0;
    int * result3 = (int *)lv_utils_bsearch(&key3, array, 2, sizeof(int), compare_int);
    TEST_ASSERT_NULL(result3);

    /* Тестовый элемент между первым и вторым */
    int key4 = 2;
    int * result4 = (int *)lv_utils_bsearch(&key4, array, 2, sizeof(int), compare_int);
    TEST_ASSERT_NULL(result4);

    /* Тестовый элемент больше второго */
    int key5 = 4;
    int * result5 = (int *)lv_utils_bsearch(&key5, array, 2, sizeof(int), compare_int);
    TEST_ASSERT_NULL(result5);
}

/* Тестовый граничный случай n=3 – нечетное количество элементов */
void test_utils_bsearch_three_element_array(void)
{
    int array[] = {1, 2, 3};

    /* Тест на поиск первого элемента */
    int key1 = 1;
    int * result1 = (int *)lv_utils_bsearch(&key1, array, 3, sizeof(int), compare_int);
    TEST_ASSERT_NOT_NULL(result1);
    TEST_ASSERT_EQUAL_INT(1, *result1);

    /* Тест на поиск второго элемента */
    int key2 = 2;
    int * result2 = (int *)lv_utils_bsearch(&key2, array, 3, sizeof(int), compare_int);
    TEST_ASSERT_NOT_NULL(result2);
    TEST_ASSERT_EQUAL_INT(2, *result2);

    /* Тест на поиск третьего элемента */
    int key3 = 3;
    int * result3 = (int *)lv_utils_bsearch(&key3, array, 3, sizeof(int), compare_int);
    TEST_ASSERT_NOT_NULL(result3);
    TEST_ASSERT_EQUAL_INT(3, *result3);

    /* Тестовый элемент не найден */
    int key4 = 4;
    int * result4 = (int *)lv_utils_bsearch(&key4, array, 3, sizeof(int), compare_int);
    TEST_ASSERT_NULL(result4);
}

void test_draw_buf_save_to_file_open_fail(void)
{
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(draw_buf);

    /* Используйте неверный путь для проверки ошибки открытия */
    lv_result_t res = lv_draw_buf_save_to_file(draw_buf, "Z:invalid/path/test.bin");
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);

    lv_draw_buf_destroy(draw_buf);
}

/* Тестовое сохранение формата RGB565 */
void test_draw_buf_save_to_file_rgb565_format(void)
{
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(100, 100, LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(draw_buf);

    lv_result_t res = lv_draw_buf_save_to_file(draw_buf, TEST_DRAW_BUF_SAVE_PATH("rgb565_output.bin"));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);

    lv_draw_buf_destroy(draw_buf);
}

/* Тестирование сохранения буфера минимального размера */
void test_draw_buf_save_to_file_minimal_size(void)
{
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(1, 1, LV_COLOR_FORMAT_A8, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(draw_buf);

    lv_result_t res = lv_draw_buf_save_to_file(draw_buf, TEST_DRAW_BUF_SAVE_PATH("minimal_output.bin"));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);

    lv_draw_buf_destroy(draw_buf);
}

/* Проверьте различные расширения файлов */
void test_draw_buf_save_to_file_different_extensions(void)
{
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO);

    /* Тестируйте файлы с различными расширениями, используя файловую систему по умолчанию. */
    lv_result_t res1 = lv_draw_buf_save_to_file(draw_buf, TEST_DRAW_BUF_SAVE_PATH("test.bin"));
    lv_result_t res2 = lv_draw_buf_save_to_file(draw_buf, TEST_DRAW_BUF_SAVE_PATH("test.raw"));
    lv_result_t res3 = lv_draw_buf_save_to_file(draw_buf, TEST_DRAW_BUF_SAVE_PATH("test.dat"));

    TEST_ASSERT_EQUAL(LV_RESULT_OK, res1);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res2);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res3);

    lv_draw_buf_destroy(draw_buf);
}

/* Проверка нулевого размера данных */
void test_draw_buf_save_to_file_zero_data_size(void)
{
    lv_draw_buf_t draw_buf;
    lv_result_t res1 = lv_draw_buf_init(&draw_buf, 0, 0, LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO,
                                        NULL, 0);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res1);

    lv_result_t res = lv_draw_buf_save_to_file(&draw_buf, TEST_DRAW_BUF_SAVE_PATH("zero_size.bin"));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
}

/* Тестовый индексированный цветовой формат */
void test_draw_buf_save_to_file_indexed_format(void)
{
    lv_draw_buf_t draw_buf;
    lv_result_t res1 = lv_draw_buf_init(&draw_buf, 0, 0, LV_COLOR_FORMAT_RGB888, LV_STRIDE_AUTO,
                                        NULL, 0);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res1);

    lv_result_t res = lv_draw_buf_save_to_file(&draw_buf, TEST_DRAW_BUF_SAVE_PATH("indexed.bin"));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
}

void test_draw_buf_save_to_file_and_read_decode(void)
{
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_XRGB8888, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(draw_buf);

    for(int32_t y = 0; y < 10; y++) {
        for(int32_t x = 0; x < 10; x++) {
            lv_color32_t * data = lv_draw_buf_goto_xy(draw_buf, x, y);
            data->blue = 0x00;
            data->red = 0xff;
            data->green = 0x00;
            data->alpha = 0xff;
        }
    }

    lv_result_t res = lv_draw_buf_save_to_file(draw_buf, TEST_DRAW_BUF_SAVE_PATH("decode_XRGB8888.bin"));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);

    lv_obj_t * image = lv_image_create(lv_screen_active());

    lv_image_set_src(image, TEST_DRAW_BUF_SAVE_PATH("decode_XRGB8888.bin"));

    lv_obj_center(image);

    TEST_ASSERT_EQUAL_SCREENSHOT("draw_buf_save_to_file_decode_XRGB8888.png");

    lv_draw_buf_destroy(draw_buf);

}

#endif
