/**
 * @file lv_array.h
 * Массив. Элементы перемещаются модулямlv_mem.
 */

#ifndef LV_ARRAY_H
#define LV_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/

#ifndef LV_ARRAY_DEFAULT_CAPACITY
#define LV_ARRAY_DEFAULT_CAPACITY  4
#endif

#ifndef LV_ARRAY_DEFAULT_SHRINK_RATIO
#define LV_ARRAY_DEFAULT_SHRINK_RATIO 2
#endif

/**********************
 *      TYPEDEFS
 **********************/

/** Описание массива*/
struct _lv_array_t {
    uint8_t * data;
    uint32_t size;
    uint32_t capacity;
    uint32_t element_size;

    bool inner_alloc; /* true: данные выделяются массивом; false: данные выделяются пользователем */
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализировать массив.
 * @param array указатель на переменную`lv_array_t`для инициализации
 * @param capacity начальная емкость массива
 * @param element_size размер элемента в байтах
 */
void lv_array_init(lv_array_t * array, uint32_t capacity, uint32_t element_size);

/**
 * Инициализировать массив из буфера.
 * @note Буфер должен быть достаточно большим для хранения элементов `capacity`. Массив не освободит буфер и не перераспределит его.
 *       Пользователь должен убедиться, что буфер действителен в течение всего срока службы массива. И освободите буфер, когда массив больше не нужен.
 * @param array указатель на переменную`lv_array_t`для инициализации
 * @param buf указатель на буфер, который будет использоваться в качестве данных массива
 * @param capacity начальная емкость массива
 * @param element_size размер элемента в байтах
 */
void lv_array_init_from_buf(lv_array_t * array, void * buf, uint32_t capacity, uint32_t element_size);

/**
 * Измените размер массива до заданной емкости.
 * @note если новая емкость меньше текущего размера, массив будет усечен.
 * @param array указатель на переменную `lv_array_t`
 * @param new_capacity новая емкость массива
 */
bool lv_array_resize(lv_array_t * array, uint32_t new_capacity);

/**
 * Определите массив и освободите выделенную память.
 * @param array указатель на переменную`lv_array_t`для деинициализации
 */
void lv_array_deinit(lv_array_t * array);

/**
 * Возвращает количество элементов, хранящихся в массиве.
 * @param array указатель на переменную `lv_array_t`
 * @return количество элементов, хранящихся в массиве
 */
static inline uint32_t lv_array_size(const lv_array_t * array)
{
    return array->size;
}

/**
 * Возвращает емкость массива, т. е. сколько элементов можно хранить.
 * @param array указатель на переменную `lv_array_t`
 * @return емкость массива
 */
static inline uint32_t lv_array_capacity(const lv_array_t * array)
{
    return array->capacity;
}

/**
 * Возврат, если массив пуст
 * @param array указатель на переменную `lv_array_t`
 * @return true: массив пуст; false: массив не пуст
 */
static inline bool lv_array_is_empty(const lv_array_t * array)
{
    return array->size == 0;
}

/**
 * Возврат, если массив заполнен
 * @param array указатель на переменную `lv_array_t`
 * @return true: массив заполнен; ложь: массив не заполнен
 */
static inline bool lv_array_is_full(const lv_array_t * array)
{
    return array->size == array->capacity;
}

/**
 * Скопируйте массив в другой.
 * @note это создаст новый массив с той же емкостью и размером, что и исходный массив.
 * @param target указатель на переменную `lv_array_t`, в которую нужно скопировать
 * @param source указатель на переменную`lv_array_t`для копирования
 */
void lv_array_copy(lv_array_t * target, const lv_array_t * source);

/**
 * Удалить все элементы массива.
 * @param array указатель на переменную `lv_array_t`
 */
static inline void lv_array_clear(lv_array_t * array)
{
    array->size = 0;
}

/**
 * При необходимости уменьшите объем памяти массива.
 * @param array указатель на переменную `lv_array_t`
 */
void lv_array_shrink(lv_array_t * array);

/**
 * Удалить элемент в указанной позиции массива.
 *
 * Эта функция сохраняет порядок массива. Сложность равна O(n)
 *
 * @param array указатель на переменную `lv_array_t`
 * @param index индекс элемента, который нужно удалить
 * @return LV_RESULT_OK: успех, иначе: ошибка.
 */
lv_result_t lv_array_remove(lv_array_t * array, uint32_t index);

/**
 * Удалить элемент в указанной позиции массива.
 *
 * Эта функция не гарантирует порядок массива. Сложность равна O(1)
 *
 * @param array указатель на переменную `lv_array_t`
 * @param index индекс элемента, который нужно удалить
 * @return LV_RESULT_OK: успех, иначе: ошибка.
 */
lv_result_t lv_array_remove_unordered(lv_array_t * array, uint32_t index);

/**
 * Удалить из массива один элемент или диапазон элементов ([start, end)).
 * @note Это эффективно уменьшает размер контейнера на количество удаленных элементов.
 * @note Когда начало равно концу, функция не имеет никакого эффекта.
 * @param array указатель на переменную `lv_array_t`
 * @param start индекс первого элемента, который будет удален
 * @param end индекс первого элемента, который не подлежит удалению
 * @return LV_RESULT_OK: успех, иначе: ошибка.
 */
lv_result_t lv_array_erase(lv_array_t * array, uint32_t start, uint32_t end);

/**
 * Объединить два массива. Добавляет новые элементы в конец массива.
 * @note Массив назначения автоматически расширяется по мере необходимости.
 * @param array указатель на переменную `lv_array_t`
 * @param other указатель на массив для объединения
 * @return LV_RESULT_OK: успех, иначе: ошибка.
 */
lv_result_t lv_array_concat(lv_array_t * array, const lv_array_t * other);

/**
 * Отодвиньте элемент назад. Добавляет новый элемент в конец массива.
 * Если емкости массива недостаточно для нового элемента, размер массива будет изменен автоматически.
 * @note Если элементNULL, он будет добавлен как пустой элемент.
 * @param array указатель на переменную `lv_array_t`
 * @param element указатель на элемент, который нужно добавить. NULLдля отправки пустого элемента.
 * @return LV_RESULT_OK: успех, иначе: ошибка.
 */
lv_result_t lv_array_push_back(lv_array_t * array, const void * element);

/**
 * Назначает одно содержимое массиву, заменяя его текущее содержимое.
 * @param array указатель на переменную `lv_array_t`
 * @param index индекс элемента для замены
 * @param value указатель на элементы, которые нужно добавить
 * @return правда: успех; ложь: ошибка
 */
lv_result_t lv_array_assign(lv_array_t * array, uint32_t index, const void * value);

/**
 * Возвращает указатель на элемент в позиции n массива.
 * @param array указатель на переменную `lv_array_t`
 * @param index индекс возвращаемого элемента
 * @return указатель на запрошенный элемент, NULL, если`index`находится вне диапазона
 */
void * lv_array_at(const lv_array_t * array, uint32_t index);

/**
 * Возвращает указатель на первый элемент массива.
 * @param array указатель на переменную `lv_array_t`
 * @return указатель на первый элемент массива
 */
static inline void * lv_array_front(const lv_array_t * array)
{
    return lv_array_at(array, 0);
}

/**
 * Возвращает указатель на последний элемент массива.
 * @param array указатель на переменную `lv_array_t`
 */
static inline void * lv_array_back(const lv_array_t * array)
{
    return lv_array_at(array, lv_array_size(array) - 1);
}

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif
