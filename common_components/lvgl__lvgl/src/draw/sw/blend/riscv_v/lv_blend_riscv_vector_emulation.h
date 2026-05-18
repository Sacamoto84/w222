/**
 * @file lv_blend_riscv_vector_emulation.h
 * Программная эмуляция встроенных функцийRISC-V Vector Extension (RVV 1.0).
 *
 * Этот заголовок предоставляет чистую реализацию встроенных функций RVV на языке C, позволяющую
 * тестирование и проверка на платформах, отличных от RVV. Реализации следуют
 * спецификация RVV, как описано в:
 * https://dzaima.github.io/intrinsics-viewer/
 *
 * Использование:
 * 1. В системах без поддержкиRVVвключите этот заголовокBEFORE<riscv_vector.h>
 * 2. Или определитеRISCV_VECTOR_EMULATIONперед включением фактического <riscv_vector.h>
 * 3. Все функции __riscv_* будут эмулироваться в программном обеспечении.
 *
 * Ограничения:
 * - Никакой оптимизации производительности (это программная эмуляция)
 * - Длина вектора (vl) отслеживается, но все операции выполняются с отдельными элементами в цикле.
 * - Предикация и маскирование упрощены, но функционально корректны.
 * - LMUL < 1 (дробные множители) не поддерживаются.
 */

#ifndef LV_BLEND_RISCV_VECTOR_EMULATION_H
#define LV_BLEND_RISCV_VECTOR_EMULATION_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../lv_conf_internal.h"
#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_RISCV_V

#include <stdint.h>
#include <string.h>
#include <stddef.h>

/* ============================================================================
 * Определения векторных типов
 * ============================================================================
 *
 * Для эмуляции мы используем структуры, содержащие данные и текущую длину вектора.
 * Реальный RVV гораздо сложнее, но это позволяет нам проверять логику.
 *
 * Assumption: VLEN = 128 бит (обычная конфигурация RVV)
 * - e8m1: 128 бит/8 бит = 16 элементов
 * - e8m2: 256 бит/8 бит = 32 элемента
 * - e8m4: 512 бит/8 бит = 64 элемента
 * - e16m1: 128 бит/16 бит = 8 элементов
 * - e16m2: 256 бит/16 бит = 16 элементов
 * - e16m4: 512 бит/16 бит = 32 элемента
 * - e32m1: 128 бит/32 бита = 4 элемента
 * - e32m2: 256 бит/32 бита = 8 элементов
 * - e32m4: 512 бит/32 бита = 16 элементов
 */

/* LMUL = 1 (1 векторный регистр,VLEN=128 бит) */
typedef struct {
    uint8_t data[16];   /* 128 бит/8 бит на элемент = 16 элементов */
    size_t vl;          /* Текущая длина вектора */
} vuint8m1_t;

typedef struct {
    uint16_t data[8];   /* 128 бит/16 бит на элемент = 8 элементов */
    size_t vl;
} vuint16m1_t;

typedef struct {
    uint32_t data[4];   /* 128 бит/32 бита на элемент = 4 элемента */
    size_t vl;
} vuint32m1_t;

typedef struct {
    uint64_t data[2];   /* 128 бит/64 бита на элемент = 2 элемента */
    size_t vl;
} vuint64m1_t;

/* LMUL = 2 (2 векторных регистра, всего 256 бит) */
typedef struct {
    uint8_t data[32];   /* 256 бит/8 бит на элемент = 32 элемента */
    size_t vl;
} vuint8m2_t;

typedef struct {
    uint16_t data[16];  /* 256 бит/16 бит на элемент = 16 элементов */
    size_t vl;
} vuint16m2_t;

typedef struct {
    uint32_t data[8];   /* 256 бит/32 бита на элемент = 8 элементов */
    size_t vl;
} vuint32m2_t;

typedef struct {
    uint64_t data[4];   /* 256 бит/64 бита на элемент = 4 элемента */
    size_t vl;
} vuint64m2_t;

/* LMUL = 4 (4 векторных регистра, всего 512 бит) */
typedef struct {
    uint32_t data[16];  /* 512 бит/32 бита на элемент = 16 элементов */
    size_t vl;
} vuint32m4_t;

typedef struct {
    uint16_t data[32];  /* 512 бит/16 бит на элемент = 32 элемента */
    size_t vl;
} vuint16m4_t;

/* LMUL = 8 (8 векторных регистров, всего 1024 бита) */
typedef struct {
    uint8_t data[128];  /* 1024 бита/8 бит на элемент = 128 элементов */
    size_t vl;
} vuint8m8_t;

/* Логические типы/типы масок (vbool4 означаетSEW/LMUL=4, для e8m2 -> 8/2=4) */
typedef struct {
    uint8_t data[32];   /* Тот же размер, что и вектор, который он маскирует (e8m2 = 32 элемента) */
    size_t vl;
} vbool4_t;

typedef struct {
    uint8_t data[16];   /* Маска для e8m1 (16 элементов) */
    size_t vl;
} vbool8_t;

/* ============================================================================
 * Управление длиной вектора
 * ============================================================================
 *
 * Операции:
 * - __riscv_vsetvl_ * : Установить длину вектора для данного типа элемента и LMUL.
 * - __riscv_vsetvlmax_ *: получить максимальную длину вектора.
 */

/**
 * Получите максимальную длину вектора для данного типа элемента и LMUL.
 * На основе VLEN =128 бит.
 */
static inline size_t __riscv_vsetvlmax_e8m1(void)
{
    return 16;    /* 128/8 = 16 */
}
static inline size_t __riscv_vsetvlmax_e8m2(void)
{
    return 32;    /* 256/8 = 32 */
}
static inline size_t __riscv_vsetvlmax_e8m4(void)
{
    return 64;    /* 512/8 = 64 */
}

static inline size_t __riscv_vsetvlmax_e16m1(void)
{
    return 8;    /* 128/16 = 8 */
}
static inline size_t __riscv_vsetvlmax_e16m2(void)
{
    return 16;    /* 256/16 = 16 */
}
static inline size_t __riscv_vsetvlmax_e16m4(void)
{
    return 32;    /* 512/16 = 32 */
}

static inline size_t __riscv_vsetvlmax_e32m1(void)
{
    return 4;    /* 128/32 = 4 */
}
static inline size_t __riscv_vsetvlmax_e32m2(void)
{
    return 8;    /* 256/32 = 8 */
}
static inline size_t __riscv_vsetvlmax_e32m4(void)
{
    return 16;    /* 512/32 = 16 */
}

static inline size_t __riscv_vsetvlmax_e8m8(void)
{
    return 128;    /* 1024/8 = 128 */
}

static inline size_t __riscv_vsetvlmax_e64m1(void)
{
    return 2;    /* 128/64 = 2 */
}
static inline size_t __riscv_vsetvlmax_e64m2(void)
{
    return 4;    /* 256/64 = 4 */
}
static inline size_t __riscv_vsetvlmax_e64m4(void)
{
    return 8;    /* 512/64 = 8 */
}

/**
 * Установите длину вектора на запрошенное значение (или максимальную, если требуется > макс.)
 */
static inline size_t __riscv_vsetvl_e8m1(size_t avl)
{
    return avl > 16 ? 16 : avl;
}

static inline size_t __riscv_vsetvl_e8m2(size_t avl)
{
    return avl > 32 ? 32 : avl;
}

static inline size_t __riscv_vsetvl_e8m4(size_t avl)
{
    return avl > 64 ? 64 : avl;
}

static inline size_t __riscv_vsetvl_e16m1(size_t avl)
{
    return avl > 8 ? 8 : avl;
}

static inline size_t __riscv_vsetvl_e16m2(size_t avl)
{
    return avl > 16 ? 16 : avl;
}

static inline size_t __riscv_vsetvl_e16m4(size_t avl)
{
    return avl > 32 ? 32 : avl;
}

static inline size_t __riscv_vsetvl_e32m1(size_t avl)
{
    return avl > 4 ? 4 : avl;
}

static inline size_t __riscv_vsetvl_e32m2(size_t avl)
{
    return avl > 8 ? 8 : avl;
}

static inline size_t __riscv_vsetvl_e32m4(size_t avl)
{
    return avl > 16 ? 16 : avl;
}

static inline size_t __riscv_vsetvl_e8m8(size_t avl)
{
    return avl > 128 ? 128 : avl;
}

/* ============================================================================
 * Операции векторной инициализации (vmv.v.x — широковещательная связь)
 * ============================================================================
 */

/**
 * vmv.v.x: широковещательная передача скаляра всем векторным элементам.
 */
static inline vuint8m1_t __riscv_vmv_v_x_u8m1(uint8_t src, size_t vl)
{
    vuint8m1_t res;
    res.vl = vl > 16 ? 16 : vl;
    for(size_t i = 0; i < res.vl; i++) {
        res.data[i] = src;
    }
    return res;
}

static inline vuint8m2_t __riscv_vmv_v_x_u8m2(uint8_t src, size_t vl)
{
    vuint8m2_t res;
    res.vl = vl > 32 ? 32 : vl;
    for(size_t i = 0; i < res.vl; i++) {
        res.data[i] = src;
    }
    return res;
}

static inline vuint16m2_t __riscv_vmv_v_x_u16m2(uint16_t src, size_t vl)
{
    vuint16m2_t res;
    res.vl = vl > 16 ? 16 : vl;
    for(size_t i = 0; i < res.vl; i++) {
        res.data[i] = src;
    }
    return res;
}

static inline vuint16m4_t __riscv_vmv_v_x_u16m4(uint16_t src, size_t vl)
{
    vuint16m4_t res;
    res.vl = vl > 32 ? 32 : vl;
    for(size_t i = 0; i < res.vl; i++) {
        res.data[i] = src;
    }
    return res;
}

static inline vuint32m4_t __riscv_vmv_v_x_u32m4(uint32_t src, size_t vl)
{
    vuint32m4_t res;
    res.vl = vl > 16 ? 16 : vl;
    for(size_t i = 0; i < res.vl; i++) {
        res.data[i] = src;
    }
    return res;
}

/* ============================================================================
 * Операции векторной загрузки (vle, vlse)
 * ============================================================================
 */

/**
 * vle8: Вектор загрузки 8-битных элементов с единичным шагом
 */
static inline vuint8m1_t __riscv_vle8_v_u8m1(const uint8_t * base, size_t vl)
{
    vuint8m1_t res;
    res.vl = vl > 16 ? 16 : vl;
    for(size_t i = 0; i < res.vl; i++) {
        res.data[i] = base[i];
    }
    return res;
}

static inline vuint8m2_t __riscv_vle8_v_u8m2(const uint8_t * base, size_t vl)
{
    vuint8m2_t res;
    res.vl = vl > 32 ? 32 : vl;
    for(size_t i = 0; i < res.vl; i++) {
        res.data[i] = base[i];
    }
    return res;
}

static inline vuint8m8_t __riscv_vle8_v_u8m8(const uint8_t * base, size_t vl)
{
    vuint8m8_t res;
    res.vl = vl > 128 ? 128 : vl;
    for(size_t i = 0; i < res.vl; i++) {
        res.data[i] = base[i];
    }
    return res;
}

/**
 * vlse8: Вектор нагрузки с шагом
 * Загрузка из базы адресов + i * шаг для каждого элемента i
 */
static inline vuint8m2_t __riscv_vlse8_v_u8m2(const uint8_t * base, ptrdiff_t stride, size_t vl)
{
    vuint8m2_t res;
    res.vl = vl > 32 ? 32 : vl;
    for(size_t i = 0; i < res.vl; i++) {
        res.data[i] = *(const uint8_t *)((const char *)base + i * stride);
    }
    return res;
}

static inline vuint8m1_t __riscv_vlse8_v_u8m1(const uint8_t * base, ptrdiff_t stride, size_t vl)
{
    vuint8m1_t res;
    res.vl = vl > 16 ? 16 : vl;
    for(size_t i = 0; i < res.vl; i++) {
        res.data[i] = *(const uint8_t *)((const char *)base + i * stride);
    }
    return res;
}

/**
 * vle16: Загрузить 16-битный вектор
 */
static inline vuint16m2_t __riscv_vle16_v_u16m2(const uint16_t * base, size_t vl)
{
    vuint16m2_t res;
    res.vl = vl > 16 ? 16 : vl;
    for(size_t i = 0; i < res.vl; i++) {
        res.data[i] = base[i];
    }
    return res;
}

/**
 * vlse16: Загрузите 16-битный вектор с шагом
 */
static inline vuint16m2_t __riscv_vlse16_v_u16m2(const uint16_t * base, ptrdiff_t stride, size_t vl)
{
    vuint16m2_t res;
    res.vl = vl > 16 ? 16 : vl;
    for(size_t i = 0; i < res.vl; i++) {
        res.data[i] = *(const uint16_t *)((const char *)base + i * stride);
    }
    return res;
}

/* ============================================================================
 * Операции векторного хранилища (все, все)
 * ============================================================================
 */

/**
 * vse8: Сохраните вектор 8-битных элементов с единичным шагом
 */
static inline void __riscv_vse8_v_u8m1(uint8_t * base, vuint8m1_t v, size_t vl)
{
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        base[i] = v.data[i];
    }
}

static inline void __riscv_vse8_v_u8m2(uint8_t * base, vuint8m2_t v, size_t vl)
{
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        base[i] = v.data[i];
    }
}

static inline void __riscv_vse8_v_u8m8(uint8_t * base, vuint8m8_t v, size_t vl)
{
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        base[i] = v.data[i];
    }
}

/**
 * vsse8: Сохраняйте вектор с легкостью
 * Сохранение по адресу base +i *шаг для каждого элемента i
 */
static inline void __riscv_vsse8_v_u8m2(uint8_t * base, ptrdiff_t stride, vuint8m2_t v, size_t vl)
{
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        *(uint8_t *)((char *)base + i * stride) = v.data[i];
    }
}

static inline void __riscv_vsse8_v_u8m1(uint8_t * base, ptrdiff_t stride, vuint8m1_t v, size_t vl)
{
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        *(uint8_t *)((char *)base + i * stride) = v.data[i];
    }
}

/**
 * vse16: Сохраните 16-битный вектор
 */
static inline void __riscv_vse16_v_u16m2(uint16_t * base, vuint16m2_t v, size_t vl)
{
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        base[i] = v.data[i];
    }
}

static inline void __riscv_vse16_v_u16m4(uint16_t * base, vuint16m4_t v, size_t vl)
{
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        base[i] = v.data[i];
    }
}

/**
 * vsse16: Храните 16-битный вектор с легкостью
 */
static inline void __riscv_vsse16_v_u16m2(uint16_t * base, ptrdiff_t stride, vuint16m2_t v, size_t vl)
{
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        *(uint16_t *)((char *)base + i * stride) = v.data[i];
    }
}

/**
 * vse32: Хранить 32-битный вектор
 */
static inline void __riscv_vse32_v_u32m4(uint32_t * base, vuint32m4_t v, size_t vl)
{
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        base[i] = v.data[i];
    }
}

/* ============================================================================
 * Векторные арифметические операции
 * ============================================================================
 */

/**
 * vmul: Векторное умножение (скаляр * вектор)
 */
static inline vuint16m2_t __riscv_vmul_vx_u16m2(vuint16m2_t v, uint16_t x, size_t vl)
{
    vuint16m2_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = v.data[i] * x;
    }
    return res;
}

static inline vuint16m2_t __riscv_vmul_vv_u16m2(vuint16m2_t v1, vuint16m2_t v2, size_t vl)
{
    vuint16m2_t res;
    res.vl = v1.vl;
    for(size_t i = 0; i < v1.vl && i < vl; i++) {
        res.data[i] = v1.data[i] * v2.data[i];
    }
    return res;
}

/**
 * vwmulu: Векторное расширение умножения без знака (скаляр * вектор, 8-битный -> 16-битный)
 */
static inline vuint16m4_t __riscv_vwmulu_vx_u16m4(vuint8m2_t v, uint8_t x, size_t vl)
{
    vuint16m4_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = (uint16_t)v.data[i] * (uint16_t)x;
    }
    return res;
}

static inline vuint16m2_t __riscv_vwmulu_vx_u16m2(vuint8m1_t v, uint8_t x, size_t vl)
{
    vuint16m2_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = (uint16_t)v.data[i] * (uint16_t)x;
    }
    return res;
}

static inline vuint16m2_t __riscv_vwmulu_vv_u16m2(vuint8m1_t v1, vuint8m1_t v2, size_t vl)
{
    vuint16m2_t res;
    res.vl = v1.vl;
    for(size_t i = 0; i < v1.vl && i < vl; i++) {
        res.data[i] = (uint16_t)v1.data[i] * (uint16_t)v2.data[i];
    }
    return res;
}

static inline vuint16m4_t __riscv_vwmulu_vv_u16m4(vuint8m2_t v1, vuint8m2_t v2, size_t vl)
{
    vuint16m4_t res;
    res.vl = v1.vl;
    for(size_t i = 0; i < v1.vl && i < vl; i++) {
        res.data[i] = (uint16_t)v1.data[i] * (uint16_t)v2.data[i];
    }
    return res;
}

static inline vuint16m2_t __riscv_vwmaccu_vx_u16m2(vuint16m2_t acc, uint8_t x, vuint8m1_t v, size_t vl)
{
    vuint16m2_t res = acc;
    for(size_t i = 0; i < acc.vl && i < vl; i++) {
        res.data[i] += (uint16_t)x * (uint16_t)v.data[i];
    }
    return res;
}

static inline vuint16m4_t __riscv_vwmaccu_vx_u16m4(vuint16m4_t acc, uint8_t x, vuint8m2_t v, size_t vl)
{
    vuint16m4_t res = acc;
    for(size_t i = 0; i < acc.vl && i < vl; i++) {
        res.data[i] += (uint16_t)x * (uint16_t)v.data[i];
    }
    return res;
}

static inline vuint16m2_t __riscv_vwmaccu_vv_u16m2(vuint16m2_t acc, vuint8m1_t v1, vuint8m1_t v2, size_t vl)
{
    vuint16m2_t res = acc;
    for(size_t i = 0; i < acc.vl && i < vl; i++) {
        res.data[i] += (uint16_t)v1.data[i] * (uint16_t)v2.data[i];
    }
    return res;
}

static inline vuint16m4_t __riscv_vwmaccu_vv_u16m4(vuint16m4_t acc, vuint8m2_t v1, vuint8m2_t v2, size_t vl)
{
    vuint16m4_t res = acc;
    for(size_t i = 0; i < acc.vl && i < vl; i++) {
        res.data[i] += (uint16_t)v1.data[i] * (uint16_t)v2.data[i];
    }
    return res;
}

/* ============================================================================
 * Операции векторного сдвига
 * ============================================================================
 */

/**
 * vsrl: Векторный сдвиг вправо, логический (величина скалярного сдвига)
 */
static inline vuint16m2_t __riscv_vsrl_vx_u16m2(vuint16m2_t v, uint32_t x, size_t vl)
{
    vuint16m2_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = v.data[i] >> x;
    }
    return res;
}

static inline vuint16m4_t __riscv_vsrl_vx_u16m4(vuint16m4_t v, uint32_t x, size_t vl)
{
    vuint16m4_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = v.data[i] >> x;
    }
    return res;
}

static inline vuint8m1_t __riscv_vnsrl_wx_u8m1(vuint16m2_t v, uint32_t x, size_t vl)
{
    vuint8m1_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = (uint8_t)(v.data[i] >> x);
    }
    return res;
}

/**
 * vnsrl: Векторный узкий сдвиг вправо логический
 * Сужение от 16-битного до 8-битного со сдвигом
 */
static inline vuint8m2_t __riscv_vnsrl_wx_u8m2(vuint16m4_t v, uint32_t x, size_t vl)
{
    vuint8m2_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = (uint8_t)(v.data[i] >> x);
    }
    return res;
}

/* ============================================================================
 * Векторные побитовые операции
 * ============================================================================
 */

/**
 * vand: Вектор побитовыйAND(скаляр)
 */
static inline vuint16m2_t __riscv_vand_vx_u16m2(vuint16m2_t v, uint16_t x, size_t vl)
{
    vuint16m2_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = v.data[i] & x;
    }
    return res;
}

/**
 * vor: Вектор побитовыйOR(вектор)
 */
static inline vuint16m2_t __riscv_vor_vv_u16m2(vuint16m2_t v1, vuint16m2_t v2, size_t vl)
{
    vuint16m2_t res;
    res.vl = v1.vl;
    for(size_t i = 0; i < v1.vl && i < vl; i++) {
        res.data[i] = v1.data[i] | v2.data[i];
    }
    return res;
}

/* ============================================================================
 * Операции векторного сдвига влево
 * ============================================================================
 */

/**
 * vsll: Векторный сдвиг влево, логический (величина скалярного сдвига)
 */
static inline vuint16m2_t __riscv_vsll_vx_u16m2(vuint16m2_t v, uint32_t x, size_t vl)
{
    vuint16m2_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = v.data[i] << x;
    }
    return res;
}

/* ============================================================================
 * Операции векторного сравнения
 * ============================================================================
 */

static inline vbool8_t __riscv_vmseq_vx_u8m1_b8(vuint8m1_t v, uint8_t x, size_t vl)
{
    vbool8_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = (v.data[i] == x) ? 1 : 0;
    }
    return res;
}

/**
 * vmseq: Векторное сравнение равенства (скалярное)
 * Возвращает логическую маску (1, если равно, 0, если нет)
 */
static inline vbool4_t __riscv_vmseq_vx_u8m2_b4(vuint8m2_t v, uint8_t x, size_t vl)
{
    vbool4_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = (v.data[i] == x) ? 1 : 0;
    }
    return res;
}

/**
 * vmsgeu: Векторное сравнение больше или равно (скаляр)
 * Возвращает логическую маску
 */
static inline vbool8_t __riscv_vmsgeu_vx_u8m1_b8(vuint8m1_t v, uint8_t x, size_t vl)
{
    vbool8_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = (v.data[i] >= x) ? 1 : 0;
    }
    return res;
}

static inline vbool4_t __riscv_vmsgeu_vx_u8m2_b4(vuint8m2_t v, uint8_t x, size_t vl)
{
    vbool4_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = (v.data[i] >= x) ? 1 : 0;
    }
    return res;
}

/* ============================================================================
 * Операции векторного слияния (условно)
 * ============================================================================
 */

/**
 * vmerge: Объединить вектор под маской предиката (векторные пути)
 * Result = mask ? v2 : v1
 */
static inline vuint8m1_t __riscv_vmerge_vvm_u8m1(vuint8m1_t v1, vuint8m1_t v2, vbool8_t mask, size_t vl)
{
    vuint8m1_t res;
    res.vl = v1.vl;
    for(size_t i = 0; i < v1.vl && i < vl; i++) {
        res.data[i] = mask.data[i] ? v2.data[i] : v1.data[i];
    }
    return res;
}

static inline vuint8m2_t __riscv_vmerge_vvm_u8m2(vuint8m2_t v1, vuint8m2_t v2, vbool4_t mask, size_t vl)
{
    vuint8m2_t res;
    res.vl = v1.vl;
    for(size_t i = 0; i < v1.vl && i < vl; i++) {
        res.data[i] = mask.data[i] ? v2.data[i] : v1.data[i];
    }
    return res;
}

/**
 * vmerge: Объединить скаляр под маской предиката (скалярный путь)
 * Result = mask ? scalar : vector
 */
static inline vuint8m1_t __riscv_vmerge_vxm_u8m1(vuint8m1_t v, uint8_t x, vbool8_t mask, size_t vl)
{
    vuint8m1_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = mask.data[i] ? x : v.data[i];
    }
    return res;
}

static inline vuint8m2_t __riscv_vmerge_vxm_u8m2(vuint8m2_t v, uint8_t x, vbool4_t mask, size_t vl)
{
    vuint8m2_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = mask.data[i] ? x : v.data[i];
    }
    return res;
}

/* ==========================================================================
 * Векторные операции обратного вычитания
 * ========================================================================= */

static inline vuint8m1_t __riscv_vrsub_vx_u8m1(vuint8m1_t v, uint8_t x, size_t vl)
{
    vuint8m1_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = x - v.data[i];
    }
    return res;
}

static inline vuint8m2_t __riscv_vrsub_vx_u8m2(vuint8m2_t v, uint8_t x, size_t vl)
{
    vuint8m2_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = x - v.data[i];
    }
    return res;
}

static inline vuint16m2_t __riscv_vrsub_vx_u16m2(vuint16m2_t v, uint16_t x, size_t vl)
{
    vuint16m2_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = x - v.data[i];
    }
    return res;
}

/* ==========================================================================
 * Векторные операции добавления
 * ========================================================================= */

static inline vuint8m1_t __riscv_vadd_vv_u8m1(vuint8m1_t v1, vuint8m1_t v2, size_t vl)
{
    vuint8m1_t res;
    res.vl = v1.vl;
    for(size_t i = 0; i < v1.vl && i < vl; i++) {
        res.data[i] = v1.data[i] + v2.data[i];
    }
    return res;
}

static inline vuint16m4_t __riscv_vadd_vx_u16m4(vuint16m4_t v, uint16_t x, size_t vl)
{
    vuint16m4_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = v.data[i] + x;
    }
    return res;
}

static inline vuint16m2_t __riscv_vadd_vx_u16m2(vuint16m2_t v, uint16_t x, size_t vl)
{
    vuint16m2_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = v.data[i] + x;
    }
    return res;
}

/* ==========================================================================
 * Векторные операции нулевого расширения
 * ========================================================================= */

static inline vuint16m2_t __riscv_vzext_vf2_u16m2(vuint8m1_t v, size_t vl)
{
    vuint16m2_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = (uint16_t)v.data[i];
    }
    return res;
}

/* ==========================================================================
 * Расширение умножения/накопления для 16-битного -> 32-битного (m2 -> m4)
 * ========================================================================= */
static inline vuint32m4_t __riscv_vwmulu_vx_u32m4(vuint16m2_t v, uint32_t x, size_t vl)
{
    vuint32m4_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = (uint32_t)v.data[i] * x;
    }
    return res;
}

static inline vuint32m4_t __riscv_vwmulu_vv_u32m4(vuint16m2_t v1, vuint16m2_t v2, size_t vl)
{
    vuint32m4_t res;
    res.vl = v1.vl;
    for(size_t i = 0; i < v1.vl && i < vl; i++) {
        res.data[i] = (uint32_t)v1.data[i] * (uint32_t)v2.data[i];
    }
    return res;
}

static inline vuint32m4_t __riscv_vwmaccu_vx_u32m4(vuint32m4_t acc, uint32_t x, vuint16m2_t v, size_t vl)
{
    vuint32m4_t res = acc;
    for(size_t i = 0; i < acc.vl && i < vl; i++) {
        res.data[i] += (uint32_t)x * (uint32_t)v.data[i];
    }
    return res;
}

static inline vuint16m2_t __riscv_vnsrl_wx_u16m2(vuint32m4_t v, uint32_t x, size_t vl)
{
    vuint16m2_t res;
    res.vl = v.vl;
    for(size_t i = 0; i < v.vl && i < vl; i++) {
        res.data[i] = (uint16_t)(v.data[i] >> x);
    }
    return res;
}

#endif /* LV_USE_DRAW_SW_ASM_RISCV_V */

#ifdef __cplusplus
}
#endif

#endif /* LV_BLEND_RISCV_VECTOR_EMULATION_H */
