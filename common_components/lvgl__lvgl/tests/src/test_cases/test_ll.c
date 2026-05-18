
#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../src/misc/lv_ll.h"
#include "unity/unity.h"
/* Тестовое приспособление */
static lv_ll_t test_ll;

#define NODE_SIZE sizeof(int32_t)

void setUp(void)
{
    /* Инициализируйте связанный список перед каждым тестом */
    lv_ll_init(&test_ll, NODE_SIZE);
}

void tearDown(void)
{
    /* Очищайте связанный список после каждого теста */
    lv_ll_clear(&test_ll);
}

/* Тестовые случаи */
void test_ll_init(void)
{
    uint32_t node_size = NODE_SIZE;
#ifdef LV_ARCH_64
    /*Округляем размер до 8.*/
    node_size = (node_size + 7) & (~0x7);
#else
    /*Округляем размер до 4.*/
    node_size = (node_size + 3) & (~0x3);
#endif
    TEST_ASSERT_NULL(test_ll.head);
    TEST_ASSERT_NULL(test_ll.tail);
    TEST_ASSERT_EQUAL_UINT32(node_size, test_ll.n_size);
}

void test_ll_ins_head(void)
{
    int32_t * node = lv_ll_ins_head(&test_ll);
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_EQUAL_PTR(node, test_ll.head);
    TEST_ASSERT_EQUAL_PTR(node, test_ll.tail);

    /* Вставить второй узел */
    int32_t * node2 = lv_ll_ins_head(&test_ll);
    TEST_ASSERT_EQUAL_PTR(node2, test_ll.head);
    TEST_ASSERT_EQUAL_PTR(node, test_ll.tail);
}

void test_ll_ins_tail(void)
{
    int32_t * node = lv_ll_ins_tail(&test_ll);
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_EQUAL_PTR(node, test_ll.head);
    TEST_ASSERT_EQUAL_PTR(node, test_ll.tail);

    /* Вставить второй узел */
    int32_t * node2 = lv_ll_ins_tail(&test_ll);
    TEST_ASSERT_EQUAL_PTR(node, test_ll.head);
    TEST_ASSERT_EQUAL_PTR(node2, test_ll.tail);
}

void test_ll_ins_prev(void)
{
    int32_t * head = lv_ll_ins_head(&test_ll);
    int32_t * tail = lv_ll_ins_tail(&test_ll);

    /* Вставить перед хвостом */
    int32_t * middle = lv_ll_ins_prev(&test_ll, tail);
    TEST_ASSERT_EQUAL_PTR(head, test_ll.head);
    TEST_ASSERT_EQUAL_PTR(tail, test_ll.tail);
    TEST_ASSERT_EQUAL_PTR(middle, lv_ll_get_next(&test_ll, head));
    TEST_ASSERT_EQUAL_PTR(middle, lv_ll_get_prev(&test_ll, tail));

    int32_t * new_head = lv_ll_ins_prev(&test_ll, head);
    TEST_ASSERT_EQUAL_PTR(new_head, test_ll.head);
}

void test_ll_remove_head(void)
{
    int32_t * head = lv_ll_ins_head(&test_ll);
    int32_t * tail = lv_ll_ins_tail(&test_ll);

    lv_ll_remove(&test_ll, head);
    lv_free(head);
    TEST_ASSERT_EQUAL_PTR(tail, test_ll.head);
    TEST_ASSERT_EQUAL_PTR(tail, test_ll.tail);
    TEST_ASSERT_NULL(lv_ll_get_prev(&test_ll, tail));
}

void test_ll_remove_tail(void)
{
    int32_t * head = lv_ll_ins_head(&test_ll);
    int32_t * tail = lv_ll_ins_tail(&test_ll);

    lv_ll_remove(&test_ll, tail);
    lv_free(tail);
    TEST_ASSERT_EQUAL_PTR(head, test_ll.head);
    TEST_ASSERT_EQUAL_PTR(head, test_ll.tail);
    TEST_ASSERT_NULL(lv_ll_get_next(&test_ll, head));
}

void test_ll_remove_middle(void)
{
    int32_t * head = lv_ll_ins_head(&test_ll);
    int32_t * middle = lv_ll_ins_tail(&test_ll);
    int32_t * tail = lv_ll_ins_tail(&test_ll);

    lv_ll_remove(&test_ll, middle);
    lv_free(middle);
    TEST_ASSERT_EQUAL_PTR(head, test_ll.head);
    TEST_ASSERT_EQUAL_PTR(tail, test_ll.tail);
    TEST_ASSERT_EQUAL_PTR(tail, lv_ll_get_next(&test_ll, head));
    TEST_ASSERT_EQUAL_PTR(head, lv_ll_get_prev(&test_ll, tail));
}

void test_ll_move_before(void)
{
    int32_t * node1 = lv_ll_ins_head(&test_ll);
    int32_t * node2 = lv_ll_ins_tail(&test_ll);
    int32_t * node3 = lv_ll_ins_tail(&test_ll);

    /* Переместить узел 3 перед узлом 2. */
    lv_ll_move_before(&test_ll, node3, node2);
    TEST_ASSERT_EQUAL_PTR(node1, test_ll.head);
    TEST_ASSERT_EQUAL_PTR(node2, test_ll.tail);
    TEST_ASSERT_EQUAL_PTR(node3, lv_ll_get_next(&test_ll, node1));
    TEST_ASSERT_EQUAL_PTR(node2, lv_ll_get_next(&test_ll, node3));
}

void test_ll_chg_list(void)
{
    lv_ll_t dest_ll;
    lv_ll_init(&dest_ll, NODE_SIZE);

    int32_t * node = lv_ll_ins_head(&test_ll);
    lv_ll_chg_list(&test_ll, &dest_ll, node, true);

    TEST_ASSERT_NULL(test_ll.head);
    TEST_ASSERT_NULL(test_ll.tail);
    TEST_ASSERT_EQUAL_PTR(node, dest_ll.head);
    TEST_ASSERT_EQUAL_PTR(node, dest_ll.tail);

    lv_ll_clear(&dest_ll);
}

void test_ll_get_len(void)
{
    TEST_ASSERT_EQUAL(0, lv_ll_get_len(&test_ll));

    lv_ll_ins_head(&test_ll);
    TEST_ASSERT_EQUAL(1, lv_ll_get_len(&test_ll));

    lv_ll_ins_tail(&test_ll);
    TEST_ASSERT_EQUAL(2, lv_ll_get_len(&test_ll));
}

void test_ll_is_empty(void)
{
    TEST_ASSERT_TRUE(lv_ll_is_empty(NULL));
    TEST_ASSERT_TRUE(lv_ll_is_empty(&test_ll));

    lv_ll_ins_head(&test_ll);
    TEST_ASSERT_FALSE(lv_ll_is_empty(&test_ll));

    lv_ll_clear(&test_ll);
    TEST_ASSERT_TRUE(lv_ll_is_empty(&test_ll));
}

void test_ll_node_alignment(void)
{
    /* Тестирование с узлами разных размеров */
    lv_ll_t small_list;
    lv_ll_init(&small_list, 1); /* 1-байтовый узел */

#ifdef LV_ARCH_64
    TEST_ASSERT_EQUAL(8, small_list.n_size);
#else
    TEST_ASSERT_EQUAL(4, small_list.n_size);
#endif

    lv_ll_t large_list;
    lv_ll_init(&large_list, 100); /* 100-байтовый узел */

#ifdef LV_ARCH_64
    TEST_ASSERT_EQUAL(104, large_list.n_size);
#else
    TEST_ASSERT_EQUAL(100, large_list.n_size);
#endif
}

void test_ll_node_pointer_operations(void)
{
    /* Операции установки/получения указателя тестового узла */
    int32_t * node1 = lv_ll_ins_head(&test_ll);
    int32_t * node2 = lv_ll_ins_tail(&test_ll);
    int32_t * node3 = lv_ll_ins_tail(&test_ll);

    /* Проверьте первоначальные ссылки */
    TEST_ASSERT_EQUAL_PTR(node2, lv_ll_get_next(&test_ll, node1));
    TEST_ASSERT_EQUAL_PTR(node3, lv_ll_get_next(&test_ll, node2));
    TEST_ASSERT_EQUAL_PTR(node1, lv_ll_get_prev(&test_ll, node2));
    TEST_ASSERT_EQUAL_PTR(node2, lv_ll_get_prev(&test_ll, node3));
}

void test_ll_move_before_edge_cases(void)
{
    /* Тестирование движущихся узлов в различных положениях */
    int32_t * node1 = lv_ll_ins_head(&test_ll);
    int32_t * node2 = lv_ll_ins_tail(&test_ll);
    int32_t * node3 = lv_ll_ins_tail(&test_ll);

    /* Переместить средний узел перед головой */
    lv_ll_move_before(&test_ll, node2, node1);
    TEST_ASSERT_EQUAL_PTR(node2, test_ll.head);
    TEST_ASSERT_EQUAL_PTR(node1, lv_ll_get_next(&test_ll, node2));

    /* Переместить хвост перед NULL (должен стать новым хвостом) */
    lv_ll_move_before(&test_ll, node3, NULL);
    TEST_ASSERT_EQUAL_PTR(node3, test_ll.tail);
}

void test_ll_chg_list_middle_node(void)
{
    /* Тестовый список изменений со средним узлом */
    lv_ll_t dest_ll;
    lv_ll_init(&dest_ll, NODE_SIZE);

    int32_t * node1 = lv_ll_ins_head(&test_ll);
    int32_t * node2 = lv_ll_ins_tail(&test_ll);
    int32_t * node3 = lv_ll_ins_tail(&test_ll);

    /* Переместить средний узел в новый список */
    lv_ll_chg_list(&test_ll, &dest_ll, node2, false);

    TEST_ASSERT_EQUAL_PTR(node1, test_ll.head);
    TEST_ASSERT_EQUAL_PTR(node3, test_ll.tail);
    TEST_ASSERT_EQUAL_PTR(node2, dest_ll.head);
    TEST_ASSERT_EQUAL_PTR(node2, dest_ll.tail);

    lv_ll_clear(&dest_ll);
}

void test_ll_alignment_edge_cases(void)
{
    /* Краевые случаи выравнивания размера тестового узла */
#ifdef LV_ARCH_64
    /* Размеры тестов чуть ниже и выше 8-байтовой границы */
    lv_ll_t list7, list8, list9;
    lv_ll_init(&list7, 7);
    lv_ll_init(&list8, 8);
    lv_ll_init(&list9, 9);

    TEST_ASSERT_EQUAL(8, list7.n_size);
    TEST_ASSERT_EQUAL(8, list8.n_size);
    TEST_ASSERT_EQUAL(16, list9.n_size);
#else
    /* Размеры тестов чуть ниже и выше 4-байтовой границы */
    lv_ll_t list3, list4, list5;
    lv_ll_init(&list3, 3);
    lv_ll_init(&list4, 4);
    lv_ll_init(&list5, 5);

    TEST_ASSERT_EQUAL(4, list3.n_size);
    TEST_ASSERT_EQUAL(4, list4.n_size);
    TEST_ASSERT_EQUAL(8, list5.n_size);
#endif
}

void test_ll_clear_empty_list(void)
{
    /* Очистить пустой список */
    lv_ll_clear(&test_ll);
    TEST_ASSERT_TRUE(lv_ll_is_empty(&test_ll));

    /* Очистить настройки с пустым списком */
    lv_ll_clear_custom(&test_ll, NULL);
    TEST_ASSERT_TRUE(lv_ll_is_empty(&test_ll));
}

void test_ll_chg_list_tail_node(void)
{
    lv_ll_t dest_ll;
    lv_ll_init(&dest_ll, NODE_SIZE);

    /* Создайте список с двумя узлами */
    int32_t * node1 = lv_ll_ins_head(&test_ll);
    int32_t * node2 = lv_ll_ins_tail(&test_ll);

    /* Переместить хвостовой узел в новый список */
    lv_ll_chg_list(&test_ll, &dest_ll, node2, false);

    TEST_ASSERT_EQUAL_PTR(node1, test_ll.head);
    TEST_ASSERT_EQUAL_PTR(node1, test_ll.tail);
    TEST_ASSERT_EQUAL_PTR(node2, dest_ll.head);
    TEST_ASSERT_EQUAL_PTR(node2, dest_ll.tail);

    lv_ll_clear(&dest_ll);

    lv_ll_clear(&test_ll);
    node1 = lv_ll_ins_head(&test_ll);

    int32_t * head = lv_ll_ins_head(&dest_ll);
    TEST_ASSERT_EQUAL_PTR(head, dest_ll.head);
    lv_ll_chg_list(&test_ll, &dest_ll, node1, true);
    TEST_ASSERT_EQUAL_PTR(node1, dest_ll.head);

    lv_ll_clear(&dest_ll);
}

void test_ll_get_len_empty_list(void)
{
    /* Длина теста пустого списка */
    TEST_ASSERT_EQUAL(0, lv_ll_get_len(&test_ll));
}

void test_ll_nullptr_handling(void)
{
    TEST_ASSERT_NULL(lv_ll_ins_prev(NULL, NULL));
    TEST_ASSERT_NULL(lv_ll_ins_prev(&test_ll, NULL));

    lv_ll_remove(NULL, NULL);

    TEST_ASSERT_NULL(lv_ll_get_head(NULL));
    TEST_ASSERT_NULL(lv_ll_get_tail(NULL));
}

#endif
