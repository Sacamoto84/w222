/**
 * @file nuttx_mm.h
 *
 */

#ifndef NUTTX_MM_H
#define NUTTX_MM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <stddef.h>
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/

/* Специальный PID для запроса информации о распределении, свободном доступе и мемпуле. */

#define PID_MM_ORPHAN  (-6)
#define PID_MM_BIGGEST (-5)
#define PID_MM_FREE    (-4)
#define PID_MM_ALLOC   (-3)
#define PID_MM_LEAK    (-2)
#define PID_MM_MEMPOOL (-1)

#ifndef ULONG_MAX
#define ULONG_MAX   4294967295UL
#endif

#define CONFIG_MM_BACKTRACE 1

/**********************
 *      TYPEDEFS
 **********************/

struct mm_heap_s {
    /* Это размер кучи, предоставленный в мм. */

    size_t mm_heapsize;

    /* Это максимальный размер используемой памяти кучи. */

    size_t mm_maxused;

    /* Это текущий используемый размер кучи. */

    size_t mm_curused;

    /* Касан отключен или включен для этого кучи */

    bool mm_nokasan;
};

struct malltask {
    /* Отрицательный pid означает другое. См. включение/ malloc.h */

    int pid; /* Идентификатор процесса */
    unsigned long seqmin; /* Минимальная последовательность */
    unsigned long seqmax; /* Максимальная последовательность */
};

#define mm_memdump_s malltask

struct mallinfo {
    int arena;    /* неотображенное пространство, выделенное из системы */
    int ordblks;  /* количество свободных кусков */
    int smblks;   /* количество блоков в Fastbin */
    int hblks;    /* количество отображаемых регионов */
    int hblkhd;   /* пространство в mmapped регионах */
    int usmblks;  /* всегда 0, сохраняется для обратной совместимости */
    int fsmblks;  /* доступное место в освобожденных блоках fastbin */
    int uordblks; /* общее выделенное пространство */
    int fordblks; /* общее свободное пространство */
    int keepcost; /* самое верхнее, освобождаемое (черезmalloc_trim) пространство */
    int aordblks; /* Это количество выделенных (используемых) фрагментов для задачи. */
    int mxordblk; /* размер самого большого свободного фрагмента */
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

static inline struct mm_heap_s * mm_initialize(const char * name, void * mem, size_t size)
{
    (void)name;
    (void)mem;
    (void)size;
    return NULL;
}

static inline void mm_uninitialize(struct mm_heap_s * heap)
{
    (void)heap;
}

static inline void * mm_malloc(struct mm_heap_s * heap, size_t size)
{
    (void)heap;
    (void)size;
    return NULL;
}

static inline void mm_free(struct mm_heap_s * heap, void * ptr)
{
    (void)heap;
    (void)ptr;
}

static inline void mm_memdump(struct mm_heap_s * heap, const struct mm_memdump_s * dump)
{
    (void)heap;
    (void)dump;
}

static inline struct mallinfo mm_mallinfo(struct mm_heap_s * heap)
{
    (void)heap;
    struct mallinfo info = {0};
    return info;
}

static inline int gettid(void)
{
    return -1;
}

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*NUTTX_MM_H*/
