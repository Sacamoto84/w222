#if LV_BUILD_TEST

#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static uint32_t MEM_SIZE = 0;

// Размер кэша в байтах
#define CACHE_SIZE_BYTES 1000
#define CACHE_EXPECTED_DATA_CNT 10

void setUp(void)
{
    /* Функция запускается перед каждым тестом */
    MEM_SIZE = lv_test_get_free_mem();
}

void tearDown(void)
{
    /* Функция запускается после каждого теста */
    TEST_ASSERT_MEM_LEAK_LESS_THAN(MEM_SIZE, 64);
}

typedef struct {
    lv_cache_slot_size_t slot;
    void * data; // распределенные данные
    int32_t key1;
    int32_t key2;
    uint32_t magic;
} test_data_t;

static lv_cache_compare_res_t compare_cb(const test_data_t * lhs,
                                         const test_data_t * rhs)
{
    if(lhs->key1 != rhs->key1) {
        return lhs->key1 > rhs->key1 ? 1 : -1;
    }
    if(lhs->key2 != rhs->key2) {
        return lhs->key2 > rhs->key2 ? 1 : -1;
    }
    return 0;
}

static void free_cb(test_data_t * node, void * user_data)
{
    LV_UNUSED(user_data);
    lv_free(node->data);
}

static lv_cache_t * create_cache(const lv_cache_class_t * cache_class,
                                 size_t max_size)
{
    lv_cache_ops_t ops = {
        .compare_cb = (lv_cache_compare_cb_t)compare_cb,
        .create_cb = NULL,
        .free_cb = (lv_cache_free_cb_t)free_cb,
    };
    return lv_cache_create(cache_class, sizeof(test_data_t), max_size, ops);
}

void test_cache_lru_rb_1(void)
{
    lv_cache_t * cache = create_cache(&lv_cache_class_lru_rb_size, CACHE_SIZE_BYTES);
    TEST_ASSERT_NOT_NULL(cache);
    void * record_data_ptr = NULL;

    // создать много узлов, если кеш не заполнен
    uint32_t curr_mem_size = 8;
    uint32_t curr_total_mem_size = 0;
    while(curr_total_mem_size < CACHE_SIZE_BYTES) {
        test_data_t search_key = { .slot.size = curr_mem_size,

                                   .key1 = (int32_t)curr_mem_size,
                                   .key2 = (int32_t)curr_mem_size + 1
                                 };

        // сначала получить кэш
        lv_cache_entry_t * entry =
            lv_cache_acquire(cache, &search_key, NULL);
        if(entry != NULL) {
            continue;
        }

        // если кеш отсутствует, добавьте кеш
        entry = lv_cache_add(cache, &search_key, NULL);
        TEST_ASSERT_NOT_NULL(entry);

        test_data_t * data = lv_cache_entry_get_data(entry);
        TEST_ASSERT_NOT_NULL(data);

        data->data = lv_malloc(data->slot.size);

        // записать данные ptr, когда {key1 = 32, key2 = 33}.
        if(search_key.key1 == 32 && search_key.key2 == 33) {
            record_data_ptr = data->data;
        }

        lv_cache_release(cache, entry, NULL);

        curr_total_mem_size += curr_mem_size;
        curr_mem_size *= 2;

        TEST_PRINTF("cache free: %d, allocated: %d",
                    lv_cache_get_free_size(cache, NULL), curr_mem_size);
    }

    /*
     * выделено = 8 + 16 + 32 + 64 + 128 + 256 - 8 - 16 + 512 = 992
     * бесплатно = 1000 - 992 = 8
     * Размер последнего узла должен быть 1024, но максимальный размер кэша равен 1000. Таким образом, новая запись будет выделена неудачно и
     * петля разорвется.
     * */
    TEST_ASSERT_EQUAL(8, lv_cache_get_free_size(cache, NULL));

    /*
     * Поисковая запись {ключ1 = 32, ключ2 = 33}
     */
    test_data_t search_key32 = { .key1 = 32, .key2 = 33 };
    lv_cache_entry_t * entry_key32 =
        lv_cache_acquire(cache, &search_key32, NULL);

    test_data_t * cached_data_key32 = lv_cache_entry_get_data(entry_key32);
    TEST_ASSERT_EQUAL(record_data_ptr, cached_data_key32->data);

    /*
     * Теперь сбросьте кэш {key1 = 32, key2 = 33}. Однако эта запись обозначается один раз без выпуска, поэтому `drop`
     * не освободит память, выделенную этой записью.
     */
    size_t mem_curr_free = lv_test_get_free_mem();
    lv_cache_drop(cache, &search_key32, NULL);
    /*
     * Хотя данные не освобождаются, запись и другая структура были освобождены.
     * lv_rb_note_t (4 пункта + 1 int32 могут быть выровнены по 8 бит на 64-битной машине) +lv_ll(2 пункта +node_size).
     * Кроме того, в куче определения есть некоторые другие выровненные атрибуты. Это также повлияет на конечный результат.
     */
    TEST_ASSERT_MEM_LEAK_LESS_THAN(
        mem_curr_free,
        sizeof(lv_rb_node_t) + sizeof(void *) +
        (sizeof(lv_ll_node_t *) + sizeof(lv_ll_node_t *)) +
        32); // последние 32 — это ошибка выделения памяти
    mem_curr_free = lv_test_get_free_mem();
    lv_cache_release(cache, entry_key32, NULL);
    TEST_ASSERT_MEM_LEAK_LESS_THAN(
        mem_curr_free, lv_cache_entry_get_size(sizeof(test_data_t)) +
        sizeof(void *) + 32 + 32);

    // Теперь размер освобожденного кэша должен составлять 8 + 32 = 40.
    TEST_ASSERT_EQUAL(40, lv_cache_get_free_size(cache, NULL));

    lv_cache_destroy(cache, NULL);
}

static void cache_add_acquire_test(lv_cache_t * cache, test_data_t * expected_data, size_t expected_data_cnt)
{
    TEST_ASSERT_NOT_NULL(cache);
    lv_memzero(expected_data, sizeof(test_data_t) * expected_data_cnt);

    for(size_t i = 0; i < expected_data_cnt; ++i) {
        expected_data[i].key1 = i;
        expected_data[i].key2 = i + 1;
        lv_cache_entry_t * entry =
            lv_cache_add(cache, &expected_data[i], NULL);
        TEST_ASSERT_NOT_NULL(entry);
        lv_cache_release(cache, entry, NULL);
    }

    for(size_t i = 0; i < expected_data_cnt; ++i) {
        lv_cache_entry_t * entry =
            lv_cache_acquire(cache, &expected_data[i], NULL);
        TEST_ASSERT_NOT_NULL(entry);
        test_data_t * actual_data = lv_cache_entry_get_data(entry);
        TEST_ASSERT_EQUAL(expected_data[i].key1, actual_data->key1);
        TEST_ASSERT_EQUAL(expected_data[i].key2, actual_data->key2);
        lv_cache_release(cache, entry, NULL);
    }
}

static void cache_eviction_test(lv_cache_t * cache, test_data_t * expected_data, size_t expected_data_cnt)
{
    TEST_ASSERT_NOT_NULL(cache);
    lv_memzero(expected_data, sizeof(test_data_t) * expected_data_cnt);

    for(size_t i = 0; i < expected_data_cnt; ++i) {
        expected_data[i].key1 = i;
        expected_data[i].key2 = i + 1;
        lv_cache_entry_t * entry =
            lv_cache_add(cache, &expected_data[i], NULL);
        TEST_ASSERT_NOT_NULL(entry);
        lv_cache_release(cache, entry, NULL);
    }

    for(size_t i = 0; i < expected_data_cnt; ++i) {
        lv_cache_entry_t * entry =
            lv_cache_acquire(cache, &expected_data[i], NULL);
        TEST_ASSERT_NOT_NULL(entry);
        test_data_t * actual_data = lv_cache_entry_get_data(entry);
        TEST_ASSERT_EQUAL(expected_data[i].key1, actual_data->key1);
        TEST_ASSERT_EQUAL(expected_data[i].key2, actual_data->key2);
        lv_cache_release(cache, entry, NULL);
    }
    test_data_t new_expected_entry = {
        .key1 = 1000,
        .key2 = 2000,
    };
    lv_cache_entry_t * new_entry =
        lv_cache_add(cache, &new_expected_entry, NULL);
    TEST_ASSERT_NOT_NULL(new_entry);
    test_data_t * new_entry_data = lv_cache_entry_get_data(new_entry);
    TEST_ASSERT_EQUAL(new_expected_entry.key1, new_entry_data->key1);
    TEST_ASSERT_EQUAL(new_expected_entry.key2, new_entry_data->key2);

    /* Убедитесь, что мы удалили предыдущую запись */
    size_t original_entries_found_cnt = 0;
    for(size_t i = 0; i < expected_data_cnt; ++i) {
        lv_cache_entry_t * entry =
            lv_cache_acquire(cache, &expected_data[i], NULL);
        if(entry != NULL) {
            original_entries_found_cnt++;
            lv_cache_release(cache, entry, NULL);
        }
    }
    TEST_ASSERT_EQUAL(9, original_entries_found_cnt);
}

void test_cache_lru_rb_count_add_acquire(void)
{
    lv_cache_t * cache = create_cache(&lv_cache_class_lru_rb_count, CACHE_EXPECTED_DATA_CNT);
    test_data_t expected_data[CACHE_EXPECTED_DATA_CNT];
    cache_add_acquire_test(cache, expected_data, CACHE_EXPECTED_DATA_CNT);
    lv_cache_destroy(cache, NULL);
}

void test_cache_lru_ll_count_add_acquire(void)
{
    lv_cache_t * cache = create_cache(&lv_cache_class_lru_ll_count, CACHE_EXPECTED_DATA_CNT);
    test_data_t expected_data[CACHE_EXPECTED_DATA_CNT];
    cache_add_acquire_test(cache, expected_data, CACHE_EXPECTED_DATA_CNT);
    lv_cache_destroy(cache, NULL);
}

void test_cache_sc_da_add_acquire(void)
{
    lv_cache_t * cache = create_cache(&lv_cache_class_sc_da, CACHE_EXPECTED_DATA_CNT);
    test_data_t expected_data[CACHE_EXPECTED_DATA_CNT];
    cache_add_acquire_test(cache, expected_data, CACHE_EXPECTED_DATA_CNT);
    lv_cache_destroy(cache, NULL);
}

void test_cache_sc_da_eviction(void)
{
    lv_cache_t * cache = create_cache(&lv_cache_class_sc_da, CACHE_EXPECTED_DATA_CNT);
    test_data_t expected_data[CACHE_EXPECTED_DATA_CNT];
    cache_eviction_test(cache, expected_data, CACHE_EXPECTED_DATA_CNT);
    lv_cache_destroy(cache, NULL);
}

void test_cache_lru_rb_count_eviction(void)
{
    lv_cache_t * cache = create_cache(&lv_cache_class_lru_rb_count, CACHE_EXPECTED_DATA_CNT);
    test_data_t expected_data[CACHE_EXPECTED_DATA_CNT];
    cache_eviction_test(cache, expected_data, CACHE_EXPECTED_DATA_CNT);
    lv_cache_destroy(cache, NULL);
}

void test_cache_lru_ll_count_eviction(void)
{
    lv_cache_t * cache = create_cache(&lv_cache_class_lru_ll_count, CACHE_EXPECTED_DATA_CNT);
    test_data_t expected_data[CACHE_EXPECTED_DATA_CNT];
    cache_eviction_test(cache, expected_data, CACHE_EXPECTED_DATA_CNT);
    lv_cache_destroy(cache, NULL);
}

void test_cache_sc_da_eviction_second_chance_spares_referenced_entries(void)
{
    lv_cache_t * cache = create_cache(&lv_cache_class_sc_da, CACHE_EXPECTED_DATA_CNT);
    TEST_ASSERT_NOT_NULL(cache);
    test_data_t expected_data[CACHE_EXPECTED_DATA_CNT];

    for(size_t i = 0; i < CACHE_EXPECTED_DATA_CNT; ++i) {
        expected_data[i].key1 = i;
        expected_data[i].key2 = i + 1;
        lv_cache_add(cache, &expected_data[i], NULL);
    }

    for(size_t i = 0; i < CACHE_EXPECTED_DATA_CNT; ++i) {
        lv_cache_entry_t * entry =
            lv_cache_acquire(cache, &expected_data[i], NULL);
        TEST_ASSERT_NOT_NULL(entry);
        test_data_t * actual_data = lv_cache_entry_get_data(entry);
        TEST_ASSERT_EQUAL(expected_data[i].key1, actual_data->key1);
        TEST_ASSERT_EQUAL(expected_data[i].key2, actual_data->key2);
        /* Не освобождайте запись, чтобы на нее оставалась ссылка*/
    }

    test_data_t new_expected_entry = {
        .key1 = 1000,
        .key2 = 2000,
    };
    lv_cache_entry_t * new_entry =
        lv_cache_add(cache, &new_expected_entry, NULL);
    TEST_ASSERT_NULL(new_entry);
    lv_cache_destroy(cache, NULL);
}

void test_cache_entry_alloc(void)
{
    lv_cache_t * cache = create_cache(&lv_cache_class_lru_rb_size, CACHE_SIZE_BYTES);
    TEST_ASSERT_NOT_NULL(cache);

    uint32_t node_size = 4096;
    lv_cache_entry_t * entry = lv_cache_entry_alloc(node_size, cache);
    TEST_ASSERT_NOT_NULL(entry);

    TEST_ASSERT_EQUAL(node_size, lv_cache_entry_get_node_size(entry));

    TEST_ASSERT_EQUAL(cache, lv_cache_entry_get_cache(entry));

    lv_cache_entry_delete(entry);
    lv_cache_destroy(cache, NULL);
}

#endif
