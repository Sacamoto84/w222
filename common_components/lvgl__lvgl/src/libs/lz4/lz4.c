/*
   LZ4 - Fast LZ compression algorithm
   Copyright (C) 2011-2023, Yann Collet.

   BSD Лицензия из 2 пунктов ( http://www.opensource.org/licenses/bsd-license.php)

   Распространение и использование в исходной и двоичной форме, с или без
   модификации разрешены при соблюдении следующих условий:
   встретил:

       * При повторном распространении исходного кода необходимо сохранять вышеуказанные авторские права.
   уведомление, этот список условий и следующий отказ от ответственности.
       * Распространения в двоичной форме должны воспроизводить вышеизложенное.
   уведомление об авторских правах, этот список условий и следующий отказ от ответственности
   в документации и/или других материалах, прилагаемых к
   распространение.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   Связаться с автором можно по адресу:
    - LZ4 homepage : http://www.lz4.org
    - LZ4 source repository : https://github.com/lz4/lz4
*/

#include "../../lv_conf_internal.h"
#if LV_USE_LZ4_INTERNAL

/*-************************************
*  Параметры настройки
**************************************/
/*
 * LZ4_HEAPMODE :
 * Выберите, как будет работать сжатие без сохранения состояния, например `LZ4_compress_default()`.
 * выделить память для своей хэш-таблицы,
 * в стеке памяти (0: по умолчанию, самый быстрый) или в куче памяти (1: требуется malloc() ).
 */
#ifndef LZ4_HEAPMODE
#  define LZ4_HEAPMODE 0
#endif

/*
 * LZ4_ACCELERATION_DEFAULT :
 * Выберите «ускорение» для LZ4_compress_fast (), когда значение параметра <= 0.
 */
#define LZ4_ACCELERATION_DEFAULT 1
/*
 * LZ4_ACCELERATION_MAX :
 * Любое значение «ускорения», превышающее этот порог.
 * вместо этого восприниматься как LZ4_ACCELERATION_MAX (исправление № 876)
 */
#define LZ4_ACCELERATION_MAX 65537


/*-************************************
*  CPU Обнаружение функций
**************************************/
/* LZ4_FORCE_MEMORY_ACCESS
 * По умолчанию доступ к невыровненной памяти контролируется `memcpy()`, который является безопасным и переносимым.
 * К сожалению, в некоторых комбинациях целевой системы и компилятора сгенерированная сборка оказывается неоптимальной.
 * Переключатель ниже позволяет выбрать другой метод доступа для повышения производительности.
 * Метод 0 (по умолчанию): используйте `memcpy()`. Безопасный и портативный.
 * Способ 1: оператор `__packed`. Это зависит от расширения компилятора (т. е. непереносимости).
 *            Этот метод безопасен, если ваш компилятор поддерживает его и *обычно* так же быстр или быстрее, чем `memcpy`.
 * Способ 2: прямой доступ. Этот метод переносим, но нарушает стандарт C.
 *            Он может генерировать код с ошибками на целях, генерация сборки которых зависит от выравнивания.
 *            Но в некоторых случаях это единственный известный способ добиться максимальной производительности (например, GCC + ARMv6).
 * Подробности см. в https://fastcompression.blogspot.fr/2015/08/accessing-unaligned-memory.html.
 * Предпочитайте эти методы в порядке приоритета (0 > 1 > 2)
 */
#ifndef LZ4_FORCE_MEMORY_ACCESS   /* можно определить внешне */
#  if defined(__GNUC__) && \
  ( defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) \
  || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__) || defined(__ARM_ARCH_6T2__) )
#    define LZ4_FORCE_MEMORY_ACCESS 2
#  elif (defined(__INTEL_COMPILER) && !defined(_WIN32)) || defined(__GNUC__) || defined(_MSC_VER)
#    define LZ4_FORCE_MEMORY_ACCESS 1
#  endif
#endif

/*
 * LZ4_FORCE_SW_BITCOUNT
 * Определите этот параметр, если ваша целевая система или компилятор не поддерживает аппаратный подсчет битов.
 */
#if defined(_MSC_VER) && defined(_WIN32_WCE)   /* Visual Studio для WinCE не поддерживает аппаратный подсчет разрядов */
#  undef  LZ4_FORCE_SW_BITCOUNT  /* избегать двойной защиты */
#  define LZ4_FORCE_SW_BITCOUNT
#endif



/*-************************************
*  Зависимость
**************************************/
/*
 * LZ4_SRC_INCLUDED:
 * Флаг объединения, включен ли lz4.c
 */
#ifndef LZ4_SRC_INCLUDED
#  define LZ4_SRC_INCLUDED 1
#endif

#ifndef LZ4_DISABLE_DEPRECATE_WARNINGS
#  define LZ4_DISABLE_DEPRECATE_WARNINGS /* из-за LZ4_decompress_safe_withPrefix64k */
#endif

#ifndef LZ4_STATIC_LINKING_ONLY
#  define LZ4_STATIC_LINKING_ONLY
#endif
#include "lz4.h"
/* см. также «процедуры памяти» ниже. */


/*-************************************
*  Параметры компилятора
**************************************/
#if defined(_MSC_VER) && (_MSC_VER >= 1400)  /* Визуальная Студия 2005+ */
#  include <intrin.h>               /* присутствует только в VS2005 + */
#  pragma warning(disable : 4127)   /* disable: C4127: conditional expression is constant */
#  pragma warning(disable : 6237)   /* disable: C6237: conditional expression is always 0 */
#  pragma warning(disable : 6239)   /* disable: C6239: (<non-zero constant> && <expression>) always evaluates to the result of <expression> */
#  pragma warning(disable : 6240)   /* disable: C6240: (<expression> && <non-zero constant>) always evaluates to the result of <expression> */
#  pragma warning(disable : 6326)   /* disable: C6326: Potential comparison of a constant with another constant */
#endif  /* _MSC_VER */

#ifndef LZ4_FORCE_INLINE
#  if defined (_MSC_VER) && !defined (__clang__)    /* MSVC */
#    define LZ4_FORCE_INLINE static __forceinline
#  else
#    if defined (__cplusplus) || defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L   /* C99 */
#      if defined (__GNUC__) || defined (__clang__)
#        define LZ4_FORCE_INLINE static inline __attribute__((always_inline))
#      else
#        define LZ4_FORCE_INLINE static inline
#      endif
#    else
#      define LZ4_FORCE_INLINE static
#    endif /* __STDC_VERSION__ */
#  endif  /* _MSC_VER */
#endif /* LZ4_FORCE_INLINE */

/* LZ4_FORCE_O2 и LZ4_FORCE_INLINE
 * gcc на ppc64le генерирует развернутый цикл SIMDized для LZ4_wildCopy8,
 * вместе с простым 8-байтовым циклом копирования в качестве резервного пути.
 * Однако эта оптимизация снижает скорость распаковки более чем на 30%.
 * потому что выполнение не переходит в оптимизированный цикл
 * для типичных сжимаемых данных и все проверки преамбулы
 * прежде чем перейти на запасной путь, накладные расходы становятся бесполезными.
 * Эта оптимизация происходит только с флагом - O3, а - O2 генерирует
 * простой 8-байтовый цикл копирования.
 * При использовании gcc на ppc64le все LZ4_decompress_* и LZ4_wildCopy8
 * функции аннотированы __attribute__ ((optimize(" O2 "))),
 * а также LZ4_wildCopy8 принудительно встраивается, так что атрибут O2
 * LZ4_wildCopy8 не влияет на скорость сжатия.
 */
#if defined(__PPC64__) && defined(__LITTLE_ENDIAN__) && defined(__GNUC__) && !defined(__clang__)
#  define LZ4_FORCE_O2  __attribute__((optimize("O2")))
#  undef LZ4_FORCE_INLINE
#  define LZ4_FORCE_INLINE  static __inline __attribute__((optimize("O2"),always_inline))
#else
#  define LZ4_FORCE_O2
#endif

#if (defined(__GNUC__) && (__GNUC__ >= 3)) || (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 800)) || defined(__clang__)
#  define expect(expr,value)    (__builtin_expect ((expr),(value)) )
#else
#  define expect(expr,value)    (expr)
#endif

#ifndef likely
#define likely(expr)     expect((expr) != 0, 1)
#endif
#ifndef unlikely
#define unlikely(expr)   expect((expr) != 0, 0)
#endif

/* Если проверка центровки по какой-либо причине окажется ненадежной,
 * его можно отключить, установив для LZ4_ALIGN_TEST значение 0. */
#ifndef LZ4_ALIGN_TEST  /* может быть предоставлено извне */
# define LZ4_ALIGN_TEST 1
#endif


/*-************************************
*  Процедуры памяти
**************************************/

/*! LZ4_STATIC_LINKING_ONLY_DISABLE_MEMORY_ALLOCATION :
 *  Отключите функции LZ4/HC относительно высокого уровня, использующие динамическую память.
 *  функции распределения ( malloc() , calloc() , free() ).
 *
 *  Обратите внимание, что это переключатель времени компиляции. И поскольку он отключает
 *  общедоступные/стабильные функции LZ4 v1 API, мы не рекомендуем использовать это
 *  символ для создания библиотеки для распространения.
 *
 *  Следующие общедоступные функции удаляются при определении этого символа.
 *  - lz4   : LZ4_createStream, LZ4_freeStream,
 *            LZ4_createStreamDecode , LZ4_freeStreamDecode , LZ4_create (устарело)
 *  - lz4hc : LZ4_createStreamHC, LZ4_freeStreamHC,
 *            LZ4_createHC (устарело), LZ4_freeHC (устарело)
 *  - lz4frame, lz4file : All LZ4F_* functions
 */
#if defined(LZ4_STATIC_LINKING_ONLY_DISABLE_MEMORY_ALLOCATION)
#  define ALLOC(s)          lz4_error_memory_allocation_is_disabled
#  define ALLOC_AND_ZERO(s) lz4_error_memory_allocation_is_disabled
#  define FREEMEM(p)        lz4_error_memory_allocation_is_disabled
#elif defined(LZ4_USER_MEMORY_FUNCTIONS)
/* Функции управления памятью могут быть настроены в соответствии с пользовательским проектом.
 * Нижеприведенные функции должны существовать где-то в проекте.
 * и быть доступным во время ссылки */
void* LZ4_malloc(size_t s);
void* LZ4_calloc(size_t n, size_t s);
void  LZ4_free(void* p);
# define ALLOC(s)          LZ4_malloc(s)
# define ALLOC_AND_ZERO(s) LZ4_calloc(1,s)
# define FREEMEM(p)        LZ4_free(p)
#else
# include <stdlib.h>   /* malloc, calloc, бесплатно */
# define ALLOC(s)          malloc(s)
# define ALLOC_AND_ZERO(s) calloc(1,s)
# define FREEMEM(p)        free(p)
#endif

#if ! LZ4_FREESTANDING
#  include <string.h>   /* мемсет, мемкопи */
#endif
#if !defined(LZ4_memset)
#  define LZ4_memset(p,v,s) memset((p),(v),(s))
#endif
#define MEM_INIT(p,v,s)   LZ4_memset((p),(v),(s))


/*-************************************
*  Общие константы
**************************************/
#define MINMATCH 4

#define WILDCOPYLENGTH 8
#define LASTLITERALS   5   /* см. ../doc/ lz4_Block_format.md #parsing -restrictions */
#define MFLIMIT       12   /* см. ../doc/ lz4_Block_format.md #parsing -restrictions */
#define MATCH_SAFEGUARD_DISTANCE  ((2*WILDCOPYLENGTH) - MINMATCH)   /* убедитесь, что можно записать 2 x wildcopyLength без переполнения выходного буфера */
#define FASTLOOP_SAFE_DISTANCE 64
static const int LZ4_minLength = (MFLIMIT+1);

#define KB *(1 <<10)
#define MB *(1 <<20)
#define GB *(1U<<30)

#define LZ4_DISTANCE_ABSOLUTE_MAX 65535
#if (LZ4_DISTANCE_MAX > LZ4_DISTANCE_ABSOLUTE_MAX)   /* max поддерживается форматом LZ4 */
#  error "LZ4_DISTANCE_MAX is too big : must be <= 65535"
#endif

#define ML_BITS  4
#define ML_MASK  ((1U<<ML_BITS)-1)
#define RUN_BITS (8-ML_BITS)
#define RUN_MASK ((1U<<RUN_BITS)-1)


/*-************************************
*  Обнаружение ошибок
**************************************/
#if defined(LZ4_DEBUG) && (LZ4_DEBUG>=1)
#  include <assert.h>
#else
#  ifndef assert
#    define assert(condition) ((void)0)
#  endif
#endif

#define LZ4_STATIC_ASSERT(c)   { enum { LZ4_static_assert = 1/(int)(!!(c)) }; }   /* использовать после объявления переменных */

#if defined(LZ4_DEBUG) && (LZ4_DEBUG>=2)
#  include <stdio.h>
   static int g_debuglog_enable = 1;
#  define DEBUGLOG(l, ...) {                          \
        if ((g_debuglog_enable) && (l<=LZ4_DEBUG)) {  \
            fprintf(stderr, __FILE__  " %i: ", __LINE__); \
            fprintf(stderr, __VA_ARGS__);             \
            fprintf(stderr, " \n");                   \
    }   }
#else
#  define DEBUGLOG(l, ...) {}    /* отключен */
#endif

static int LZ4_isAligned(const void* ptr, size_t alignment)
{
    return ((size_t)ptr & (alignment -1)) == 0;
}


/*-************************************
*  Типы
**************************************/
#include <limits.h>
#if defined(__cplusplus) || (defined (__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) /* C99 */)
# include <stdint.h>
  typedef  uint8_t BYTE;
  typedef uint16_t U16;
  typedef uint32_t U32;
  typedef  int32_t S32;
  typedef uint64_t U64;
  typedef uintptr_t uptrval;
#else
# if UINT_MAX != 4294967295UL
#   error "LZ4 code (when not C++ or C99) assumes that sizeof(int) == 4"
# endif
  typedef unsigned char       BYTE;
  typedef unsigned short      U16;
  typedef unsigned int        U32;
  typedef   signed int        S32;
  typedef unsigned long long  U64;
  typedef size_t              uptrval;   /* в целом верно, кроме OpenVMS-64 */
#endif

#if defined(__x86_64__)
  typedef U64    reg_t;   /* 64-бит в режиме x32 */
#else
  typedef size_t reg_t;   /* 32-бит в режиме x32 */
#endif

typedef enum {
    notLimited = 0,
    limitedOutput = 1,
    fillOutput = 2
} limitedOutput_directive;


/*-************************************
*  Чтение и запись в память
**************************************/

/**
 * LZ4 использует memcpy с встроенным встроенным размером постоянного размера. Отдельно стоящий
 * средах компилятор не может предполагать, что реализация memcpy() является
 * соответствует стандарту, поэтому он не может применять специализированную встраивание memcpy()
 * логика. Если возможно, используйте __builtin_memcpy(), чтобы указать компилятору проанализировать
 * memcpy(), как если бы он соответствовал стандарту, поэтому его можно встроить в отдельно стоящее место.
 * среды. Это необходимо, например, при распаковке ядра Linux.
 */
#if !defined(LZ4_memcpy)
#  if defined(__GNUC__) && (__GNUC__ >= 4)
#    define LZ4_memcpy(dst, src, size) __builtin_memcpy(dst, src, size)
#  else
#    define LZ4_memcpy(dst, src, size) memcpy(dst, src, size)
#  endif
#endif

#if !defined(LZ4_memmove)
#  if defined(__GNUC__) && (__GNUC__ >= 4)
#    define LZ4_memmove __builtin_memmove
#  else
#    define LZ4_memmove memmove
#  endif
#endif

static unsigned LZ4_isLittleEndian(void)
{
    const union { U32 u; BYTE c[4]; } one = { 1 };   /* не используйте статику: производительность ухудшается */
    return one.c[0];
}

#if defined(__GNUC__) || defined(__INTEL_COMPILER)
#define LZ4_PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#elif defined(_MSC_VER)
#define LZ4_PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif

#if defined(LZ4_FORCE_MEMORY_ACCESS) && (LZ4_FORCE_MEMORY_ACCESS==2)
/* солгать компилятору о выравнивании данных; используйте с осторожностью */

static U16 LZ4_read16(const void* memPtr) { return *(const U16*) memPtr; }
static U32 LZ4_read32(const void* memPtr) { return *(const U32*) memPtr; }
static reg_t LZ4_read_ARCH(const void* memPtr) { return *(const reg_t*) memPtr; }

static void LZ4_write16(void* memPtr, U16 value) { *(U16*)memPtr = value; }
static void LZ4_write32(void* memPtr, U32 value) { *(U32*)memPtr = value; }

#elif defined(LZ4_FORCE_MEMORY_ACCESS) && (LZ4_FORCE_MEMORY_ACCESS==1)

/* Инструкции __pack более безопасны, но специфичны для компилятора, поэтому потенциально проблематичны для некоторых компиляторов. */
/* в настоящее время определено только для gcc и icc */
LZ4_PACK(typedef struct { U16 u16; }) LZ4_unalign16;
LZ4_PACK(typedef struct { U32 u32; }) LZ4_unalign32;
LZ4_PACK(typedef struct { reg_t uArch; }) LZ4_unalignST;

static U16 LZ4_read16(const void* ptr) { return ((const LZ4_unalign16*)ptr)->u16; }
static U32 LZ4_read32(const void* ptr) { return ((const LZ4_unalign32*)ptr)->u32; }
static reg_t LZ4_read_ARCH(const void* ptr) { return ((const LZ4_unalignST*)ptr)->uArch; }

static void LZ4_write16(void* memPtr, U16 value) { ((LZ4_unalign16*)memPtr)->u16 = value; }
static void LZ4_write32(void* memPtr, U32 value) { ((LZ4_unalign32*)memPtr)->u32 = value; }

#else  /* безопасный и портативный доступ с помощью memcpy() */

static U16 LZ4_read16(const void* memPtr)
{
    U16 val; LZ4_memcpy(&val, memPtr, sizeof(val)); return val;
}

static U32 LZ4_read32(const void* memPtr)
{
    U32 val; LZ4_memcpy(&val, memPtr, sizeof(val)); return val;
}

static reg_t LZ4_read_ARCH(const void* memPtr)
{
    reg_t val; LZ4_memcpy(&val, memPtr, sizeof(val)); return val;
}

static void LZ4_write16(void* memPtr, U16 value)
{
    LZ4_memcpy(memPtr, &value, sizeof(value));
}

static void LZ4_write32(void* memPtr, U32 value)
{
    LZ4_memcpy(memPtr, &value, sizeof(value));
}

#endif /* LZ4_FORCE_MEMORY_ACCESS */


static U16 LZ4_readLE16(const void* memPtr)
{
    if (LZ4_isLittleEndian()) {
        return LZ4_read16(memPtr);
    } else {
        const BYTE* p = (const BYTE*)memPtr;
        return (U16)((U16)p[0] | (p[1]<<8));
    }
}

#ifdef LZ4_STATIC_LINKING_ONLY_ENDIANNESS_INDEPENDENT_OUTPUT
static U32 LZ4_readLE32(const void* memPtr)
{
    if (LZ4_isLittleEndian()) {
        return LZ4_read32(memPtr);
    } else {
        const BYTE* p = (const BYTE*)memPtr;
        return (U32)p[0] | (p[1]<<8) | (p[2]<<16) | (p[3]<<24);
    }
}
#endif

static void LZ4_writeLE16(void* memPtr, U16 value)
{
    if (LZ4_isLittleEndian()) {
        LZ4_write16(memPtr, value);
    } else {
        BYTE* p = (BYTE*)memPtr;
        p[0] = (BYTE) value;
        p[1] = (BYTE)(value>>8);
    }
}

/* индивидуальный вариант memcpy, который может перезаписывать до 8 байтов за пределами dstEnd */
LZ4_FORCE_INLINE
void LZ4_wildCopy8(void* dstPtr, const void* srcPtr, void* dstEnd)
{
    BYTE* d = (BYTE*)dstPtr;
    const BYTE* s = (const BYTE*)srcPtr;
    BYTE* const e = (BYTE*)dstEnd;

    do { LZ4_memcpy(d,s,8); d+=8; s+=8; } while (d<e);
}

static const unsigned inc32table[8] = {0, 1, 2,  1,  0,  4, 4, 4};
static const int      dec64table[8] = {0, 0, 0, -1, -4,  1, 2, 3};


#ifndef LZ4_FAST_DEC_LOOP
#  if defined __i386__ || defined _M_IX86 || defined __x86_64__ || defined _M_X64
#    define LZ4_FAST_DEC_LOOP 1
#  elif defined(__aarch64__) && defined(__APPLE__)
#    define LZ4_FAST_DEC_LOOP 1
#  elif defined(__aarch64__) && !defined(__clang__)
     /* В aarch64, отличном от Apple, мы отключаем эту оптимизацию для clang, потому что
      * на некоторых мобильных чипсетах производительность снижается из-за лязга. Для
      * дополнительную информацию см. в https://github.com/lz4/lz4/pull/707. */
#    define LZ4_FAST_DEC_LOOP 1
#  else
#    define LZ4_FAST_DEC_LOOP 0
#  endif
#endif

#if LZ4_FAST_DEC_LOOP

LZ4_FORCE_INLINE void
LZ4_memcpy_using_offset_base(BYTE* dstPtr, const BYTE* srcPtr, BYTE* dstEnd, const size_t offset)
{
    assert(srcPtr + offset == dstPtr);
    if (offset < 8) {
        LZ4_write32(dstPtr, 0);   /* отключить предупреждение msan при смещении == 0 */
        dstPtr[0] = srcPtr[0];
        dstPtr[1] = srcPtr[1];
        dstPtr[2] = srcPtr[2];
        dstPtr[3] = srcPtr[3];
        srcPtr += inc32table[offset];
        LZ4_memcpy(dstPtr+4, srcPtr, 4);
        srcPtr -= dec64table[offset];
        dstPtr += 8;
    } else {
        LZ4_memcpy(dstPtr, srcPtr, 8);
        dstPtr += 8;
        srcPtr += 8;
    }

    LZ4_wildCopy8(dstPtr, srcPtr, dstEnd);
}

/* индивидуальный вариант memcpy, который может перезаписывать до 32 байтов за пределами dstEnd
 * эта версия копирует два раза по 16 байт (вместо одного раза по 32 байта)
 * потому что он должен быть совместим со смещениями >= 16. */
LZ4_FORCE_INLINE void
LZ4_wildCopy32(void* dstPtr, const void* srcPtr, void* dstEnd)
{
    BYTE* d = (BYTE*)dstPtr;
    const BYTE* s = (const BYTE*)srcPtr;
    BYTE* const e = (BYTE*)dstEnd;

    do { LZ4_memcpy(d,s,16); LZ4_memcpy(d+16,s+16,16); d+=32; s+=32; } while (d<e);
}

/* LZ4_memcpy_using_offset () предполагает:
 * - dstEnd >= dstPtr + MINMATCH
 * - there is at least 12 bytes available to write after dstEnd */
LZ4_FORCE_INLINE void
LZ4_memcpy_using_offset(BYTE* dstPtr, const BYTE* srcPtr, BYTE* dstEnd, const size_t offset)
{
    BYTE v[8];

    assert(dstEnd >= dstPtr + MINMATCH);

    switch(offset) {
    case 1:
        MEM_INIT(v, *srcPtr, 8);
        break;
    case 2:
        LZ4_memcpy(v, srcPtr, 2);
        LZ4_memcpy(&v[2], srcPtr, 2);
#if defined(_MSC_VER) && (_MSC_VER <= 1937) /* MSVC 2022 версия 17.7 или более ранняя */
#  pragma warning(push)
#  pragma warning(disable : 6385) /* предупреждение C6385: Чтение недопустимых данных из 'v'. */
#endif
        LZ4_memcpy(&v[4], v, 4);
#if defined(_MSC_VER) && (_MSC_VER <= 1937) /* MSVC 2022 версия 17.7 или более ранняя */
#  pragma warning(pop)
#endif
        break;
    case 4:
        LZ4_memcpy(v, srcPtr, 4);
        LZ4_memcpy(&v[4], srcPtr, 4);
        break;
    default:
        LZ4_memcpy_using_offset_base(dstPtr, srcPtr, dstEnd, offset);
        return;
    }

    LZ4_memcpy(dstPtr, v, 8);
    dstPtr += 8;
    while (dstPtr < dstEnd) {
        LZ4_memcpy(dstPtr, v, 8);
        dstPtr += 8;
    }
}
#endif


/*-************************************
*  Общие функции
**************************************/
static unsigned LZ4_NbCommonBytes (reg_t val)
{
    assert(val != 0);
    if (LZ4_isLittleEndian()) {
        if (sizeof(val) == 8) {
#       if defined(_MSC_VER) && (_MSC_VER >= 1800) && (defined(_M_AMD64) && !defined(_M_ARM64EC)) && !defined(LZ4_FORCE_SW_BITCOUNT)
/*-*************************************************************************************************
* ARM64EC — это разработанный Microsoft ARM64 ABI, совместимый с приложениями AMD64 в ARM64 Windows 11.
* ARM64EC ABI не поддерживает инструкции AVX/AVX2/AVX512, а также их соответствующие встроенные функции.
* включая _tzcnt_u64. Поэтому нам необходимо нейтрализовать путь кода _tzcnt_u64 для ARM64EC.
****************************************************************************************************/
#         if defined(__clang__) && (__clang_major__ < 10)
            /* Избегайте неопределенной проблемы с внутренними функциями clang-cl.
             * Подробности см. в https://github.com/lz4/lz4/pull/1017. */
            return (unsigned)__builtin_ia32_tzcnt_u64(val) >> 3;
#         else
            /* x64 CPUS без поддержки BMI интерпретирует `TZCNT` как `REP BSF` */
            return (unsigned)_tzcnt_u64(val) >> 3;
#         endif
#       elif defined(_MSC_VER) && defined(_WIN64) && !defined(LZ4_FORCE_SW_BITCOUNT)
            unsigned long r = 0;
            _BitScanForward64(&r, (U64)val);
            return (unsigned)r >> 3;
#       elif (defined(__clang__) || (defined(__GNUC__) && ((__GNUC__ > 3) || \
                            ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))))) && \
                                        !defined(LZ4_FORCE_SW_BITCOUNT)
            return (unsigned)__builtin_ctzll((U64)val) >> 3;
#       else
            const U64 m = 0x0101010101010101ULL;
            val ^= val - 1;
            return (unsigned)(((U64)((val & (m - 1)) * m)) >> 56);
#       endif
        } else /* 32 бита */ {
#       if defined(_MSC_VER) && (_MSC_VER >= 1400) && !defined(LZ4_FORCE_SW_BITCOUNT)
            unsigned long r;
            _BitScanForward(&r, (U32)val);
            return (unsigned)r >> 3;
#       elif (defined(__clang__) || (defined(__GNUC__) && ((__GNUC__ > 3) || \
                            ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))))) && \
                        !defined(__TINYC__) && !defined(LZ4_FORCE_SW_BITCOUNT)
            return (unsigned)__builtin_ctz((U32)val) >> 3;
#       else
            const U32 m = 0x01010101;
            return (unsigned)((((val - 1) ^ val) & (m - 1)) * m) >> 24;
#       endif
        }
    } else   /* Большой порядок байтов CPU */ {
        if (sizeof(val)==8) {
#       if (defined(__clang__) || (defined(__GNUC__) && ((__GNUC__ > 3) || \
                            ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))))) && \
                        !defined(__TINYC__) && !defined(LZ4_FORCE_SW_BITCOUNT)
            return (unsigned)__builtin_clzll((U64)val) >> 3;
#       else
#if 1
            /* этот метод, вероятно, быстрее,
             * но добавляет таблицу поиска размером 128 байт */
            static const unsigned char ctz7_tab[128] = {
                7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
            };
            U64 const mask = 0x0101010101010101ULL;
            U64 const t = (((val >> 8) - mask) | val) & mask;
            return ctz7_tab[(t * 0x0080402010080402ULL) >> 57];
#else
            /* этот метод не использует пространство памяти, как предыдущий,
             * но он содержит несколько ветвей,
             * это может привести к замедлению выполнения */
            static const U32 by32 = sizeof(val)*4;  /* 32 на 64 бита (цель), 16 на 32 бита.
            Просто чтобы какой-нибудь статический анализатор не жаловался на сдвиг на 32 на 32-битной цели.
            Обратите внимание, что этот путь кода никогда не запускается в 32-битном режиме. */
            unsigned r;
            if (!(val>>by32)) { r=4; } else { r=0; val>>=by32; }
            if (!(val>>16)) { r+=2; val>>=8; } else { val>>=24; }
            r += (!val);
            return r;
#endif
#       endif
        } else /* 32 бита */ {
#       if (defined(__clang__) || (defined(__GNUC__) && ((__GNUC__ > 3) || \
                            ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))))) && \
                                        !defined(LZ4_FORCE_SW_BITCOUNT)
            return (unsigned)__builtin_clz((U32)val) >> 3;
#       else
            val >>= 8;
            val = ((((val + 0x00FFFF00) | 0x00FFFFFF) + val) |
              (val + 0x00FF0000)) >> 24;
            return (unsigned)val ^ 3;
#       endif
        }
    }
}


#define STEPSIZE sizeof(reg_t)
LZ4_FORCE_INLINE
unsigned LZ4_count(const BYTE* pIn, const BYTE* pMatch, const BYTE* pInLimit)
{
    const BYTE* const pStart = pIn;

    if (likely(pIn < pInLimit-(STEPSIZE-1))) {
        reg_t const diff = LZ4_read_ARCH(pMatch) ^ LZ4_read_ARCH(pIn);
        if (!diff) {
            pIn+=STEPSIZE; pMatch+=STEPSIZE;
        } else {
            return LZ4_NbCommonBytes(diff);
    }   }

    while (likely(pIn < pInLimit-(STEPSIZE-1))) {
        reg_t const diff = LZ4_read_ARCH(pMatch) ^ LZ4_read_ARCH(pIn);
        if (!diff) { pIn+=STEPSIZE; pMatch+=STEPSIZE; continue; }
        pIn += LZ4_NbCommonBytes(diff);
        return (unsigned)(pIn - pStart);
    }

    if ((STEPSIZE==8) && (pIn<(pInLimit-3)) && (LZ4_read32(pMatch) == LZ4_read32(pIn))) { pIn+=4; pMatch+=4; }
    if ((pIn<(pInLimit-1)) && (LZ4_read16(pMatch) == LZ4_read16(pIn))) { pIn+=2; pMatch+=2; }
    if ((pIn<pInLimit) && (*pMatch == *pIn)) pIn++;
    return (unsigned)(pIn - pStart);
}


#ifndef LZ4_COMMONDEFS_ONLY
/*-************************************
*  Локальные константы
**************************************/
static const int LZ4_64Klimit = ((64 KB) + (MFLIMIT-1));
static const U32 LZ4_skipTrigger = 6;  /* Увеличьте это значение ==> сжатие несжимаемых данных будет выполняться медленнее. */


/*-************************************
*  Локальные структуры и типы
**************************************/
typedef enum { clearedTable = 0, byPtr, byU32, byU16 } tableType_t;

/**
 * Это перечисление различает несколько различных режимов доступа к предыдущим
 * контент в потоке.
 *
 * - noDict        : There is no preceding content.
 * - withPrefix64k : Table entries up to ctx->dictSize before the current blob
 *                   сжимаемый большой двоичный объект действительны и относятся к предыдущему
 *                   контент (длиной ctx->dictSize), который доступен
 *                   непрерывно предшествующий в памяти контент в данный момент
 *                   сжимается.
 * - usingExtDict  : Like withPrefix64k, but the preceding content is somewhere
 *                   else в памяти, начиная с ctx->словарь с длиной
 *                   ctx->dictSize.
 * - usingDictCtx  : Everything concerning the preceding content is
 *                   в отдельном контексте, на который указывает ctx->dictCtx.
 *                   ctx->словарь, ctx->dictSize и записи таблицы
 *                   в текущем контексте, которые относятся к позициям
 *                   предшествующие началу текущего сжатия
 *                   игнорируется. Вместо этого ctx->dictCtx->словарь и ctx->dictCtx.
 *                   ->dictSize описывает расположение и размер предыдущего
 *                   содержимое, а совпадения можно найти, просмотрев файл ctx
 *                   ->dictCtx->hashTable.
 */
typedef enum { noDict = 0, withPrefix64k, usingExtDict, usingDictCtx } dict_directive;
typedef enum { noDictIssue = 0, dictSmall } dictIssue_directive;


/*-************************************
*  Локальные утилиты
**************************************/
int LZ4_versionNumber (void) { return LZ4_VERSION_NUMBER; }
const char* LZ4_versionString(void) { return LZ4_VERSION_STRING; }
int LZ4_compressBound(int isize)  { return LZ4_COMPRESSBOUND(isize); }
int LZ4_sizeofState(void) { return sizeof(LZ4_stream_t); }


/*-****************************************
*  Внутренние определения, используемые только в тестах.
*******************************************/
#if defined (__cplusplus)
extern "C" {
#endif

int LZ4_compress_forceExtDict (LZ4_stream_t* LZ4_dict, const char* source, char* dest, int srcSize);

int LZ4_decompress_safe_forceExtDict(const char* source, char* dest,
                                     int compressedSize, int maxOutputSize,
                                     const void* dictStart, size_t dictSize);
int LZ4_decompress_safe_partial_forceExtDict(const char* source, char* dest,
                                     int compressedSize, int targetOutputSize, int dstCapacity,
                                     const void* dictStart, size_t dictSize);
#if defined (__cplusplus)
}
#endif

/*-******************************
*  Функции сжатия
********************************/
LZ4_FORCE_INLINE U32 LZ4_hash4(U32 sequence, tableType_t const tableType)
{
    if (tableType == byU16)
        return ((sequence * 2654435761U) >> ((MINMATCH*8)-(LZ4_HASHLOG+1)));
    else
        return ((sequence * 2654435761U) >> ((MINMATCH*8)-LZ4_HASHLOG));
}

LZ4_FORCE_INLINE U32 LZ4_hash5(U64 sequence, tableType_t const tableType)
{
    const U32 hashLog = (tableType == byU16) ? LZ4_HASHLOG+1 : LZ4_HASHLOG;
    if (LZ4_isLittleEndian()) {
        const U64 prime5bytes = 889523592379ULL;
        return (U32)(((sequence << 24) * prime5bytes) >> (64 - hashLog));
    } else {
        const U64 prime8bytes = 11400714785074694791ULL;
        return (U32)(((sequence >> 24) * prime8bytes) >> (64 - hashLog));
    }
}

LZ4_FORCE_INLINE U32 LZ4_hashPosition(const void* const p, tableType_t const tableType)
{
    if ((sizeof(reg_t)==8) && (tableType != byU16)) return LZ4_hash5(LZ4_read_ARCH(p), tableType);

#ifdef LZ4_STATIC_LINKING_ONLY_ENDIANNESS_INDEPENDENT_OUTPUT
    return LZ4_hash4(LZ4_readLE32(p), tableType);
#else
    return LZ4_hash4(LZ4_read32(p), tableType);
#endif
}

LZ4_FORCE_INLINE void LZ4_clearHash(U32 h, void* tableBase, tableType_t const tableType)
{
    switch (tableType)
    {
    default: /* провал */
    case clearedTable: { /* незаконно! */ assert(0); return; }
    case byPtr: { const BYTE** hashTable = (const BYTE**)tableBase; hashTable[h] = NULL; return; }
    case byU32: { U32* hashTable = (U32*) tableBase; hashTable[h] = 0; return; }
    case byU16: { U16* hashTable = (U16*) tableBase; hashTable[h] = 0; return; }
    }
}

LZ4_FORCE_INLINE void LZ4_putIndexOnHash(U32 idx, U32 h, void* tableBase, tableType_t const tableType)
{
    switch (tableType)
    {
    default: /* провал */
    case clearedTable: /* провал */
    case byPtr: { /* незаконно! */ assert(0); return; }
    case byU32: { U32* hashTable = (U32*) tableBase; hashTable[h] = idx; return; }
    case byU16: { U16* hashTable = (U16*) tableBase; assert(idx < 65536); hashTable[h] = (U16)idx; return; }
    }
}

/* LZ4_putPosition *() : используется только в режиме byPtr. */
LZ4_FORCE_INLINE void LZ4_putPositionOnHash(const BYTE* p, U32 h,
                                  void* tableBase, tableType_t const tableType)
{
    const BYTE** const hashTable = (const BYTE**)tableBase;
    assert(tableType == byPtr); (void)tableType;
    hashTable[h] = p;
}

LZ4_FORCE_INLINE void LZ4_putPosition(const BYTE* p, void* tableBase, tableType_t tableType)
{
    U32 const h = LZ4_hashPosition(p, tableType);
    LZ4_putPositionOnHash(p, h, tableBase, tableType);
}

/* LZ4_getIndexOnHash () :
 * Индекс позиции совпадения, зарегистрированный в хеш-таблице.
 * хэш-позиция должна рассчитываться с использованием base+index или dictBase+index.
 * Предположение 1: действительно только в том случае, если tableType == byU32 или byU16.
 * Допущение 2: h считается действительным (в пределах хеш-таблицы).
 */
LZ4_FORCE_INLINE U32 LZ4_getIndexOnHash(U32 h, const void* tableBase, tableType_t tableType)
{
    LZ4_STATIC_ASSERT(LZ4_MEMORY_USAGE > 2);
    if (tableType == byU32) {
        const U32* const hashTable = (const U32*) tableBase;
        assert(h < (1U << (LZ4_MEMORY_USAGE-2)));
        return hashTable[h];
    }
    if (tableType == byU16) {
        const U16* const hashTable = (const U16*) tableBase;
        assert(h < (1U << (LZ4_MEMORY_USAGE-1)));
        return hashTable[h];
    }
    assert(0); return 0;  /* запрещенное дело */
}

static const BYTE* LZ4_getPositionOnHash(U32 h, const void* tableBase, tableType_t tableType)
{
    assert(tableType == byPtr); (void)tableType;
    { const BYTE* const* hashTable = (const BYTE* const*) tableBase; return hashTable[h]; }
}

LZ4_FORCE_INLINE const BYTE*
LZ4_getPosition(const BYTE* p,
                const void* tableBase, tableType_t tableType)
{
    U32 const h = LZ4_hashPosition(p, tableType);
    return LZ4_getPositionOnHash(h, tableBase, tableType);
}

LZ4_FORCE_INLINE void
LZ4_prepareTable(LZ4_stream_t_internal* const cctx,
           const int inputSize,
           const tableType_t tableType) {
    /* Если таблица не использовалась, она гарантированно обнуляется и
     * поэтому безопасно использовать независимо от того, в каком режиме мы находимся. В противном случае мы полагаем,
     * выясните, можно ли оставить все как есть или необходимо сбросить настройки.
     */
    if ((tableType_t)cctx->tableType != clearedTable) {
        assert(inputSize >= 0);
        if ((tableType_t)cctx->tableType != tableType
          || ((tableType == byU16) && cctx->currentOffset + (unsigned)inputSize >= 0xFFFFU)
          || ((tableType == byU32) && cctx->currentOffset > 1 GB)
          || tableType == byPtr
          || inputSize >= 4 KB)
        {
            DEBUGLOG(4, "LZ4_prepareTable: Resetting table in %p", cctx);
            MEM_INIT(cctx->hashTable, 0, LZ4_HASHTABLESIZE);
            cctx->currentOffset = 0;
            cctx->tableType = (U32)clearedTable;
        } else {
            DEBUGLOG(4, "LZ4_prepareTable: Re-use hash table (no reset)");
        }
    }

    /* Добавляем пробел, чтобы все предыдущие записи были > LZ4_DISTANCE_MAX назад,
     * быстрее, чем сжатие без зазора.
     * Однако сжатие с currentOffset == 0 еще быстрее.
     * поэтому мы сохраняем этот случай.
     */
    if (cctx->currentOffset != 0 && tableType == byU32) {
        DEBUGLOG(5, "LZ4_prepareTable: adding 64KB to currentOffset");
        cctx->currentOffset += 64 KB;
    }

    /* Наконец-то чистая история */
    cctx->dictCtx = NULL;
    cctx->dictionary = NULL;
    cctx->dictSize = 0;
}

/** LZ4_compress_generic_validated () :
 *  встроен, чтобы обеспечить выбор ветвей во время компиляции.
 *  Предполагается, что следующие условия уже подтверждены:
 *  - source != NULL
 *  - inputSize > 0
 */
LZ4_FORCE_INLINE int LZ4_compress_generic_validated(
                 LZ4_stream_t_internal* const cctx,
                 const char* const source,
                 char* const dest,
                 const int inputSize,
                 int*  inputConsumed, /* пишется только тогда, когда outputDirective == fillOutput */
                 const int maxOutputSize,
                 const limitedOutput_directive outputDirective,
                 const tableType_t tableType,
                 const dict_directive dictDirective,
                 const dictIssue_directive dictIssue,
                 const int acceleration)
{
    int result;
    const BYTE* ip = (const BYTE*)source;

    U32 const startIndex = cctx->currentOffset;
    const BYTE* base = (const BYTE*)source - startIndex;
    const BYTE* lowLimit;

    const LZ4_stream_t_internal* dictCtx = (const LZ4_stream_t_internal*) cctx->dictCtx;
    const BYTE* const dictionary =
        dictDirective == usingDictCtx ? dictCtx->dictionary : cctx->dictionary;
    const U32 dictSize =
        dictDirective == usingDictCtx ? dictCtx->dictSize : cctx->dictSize;
    const U32 dictDelta =
        (dictDirective == usingDictCtx) ? startIndex - dictCtx->currentOffset : 0;   /* сделать индексы в dictCtx сопоставимыми с индексами в текущем контексте */

    int const maybe_extMem = (dictDirective == usingExtDict) || (dictDirective == usingDictCtx);
    U32 const prefixIdxLimit = startIndex - dictSize;   /* используется, когда dictDirective == dictSmall */
    const BYTE* const dictEnd = dictionary ? dictionary + dictSize : dictionary;
    const BYTE* anchor = (const BYTE*) source;
    const BYTE* const iend = ip + inputSize;
    const BYTE* const mflimitPlusOne = iend - MFLIMIT + 1;
    const BYTE* const matchlimit = iend - LASTLITERALS;

    /* dictCtx currentOffset индексируется в начале словаря,
     * в то время как словарь в текущем контексте предшествует currentOffset */
    const BYTE* dictBase = (dictionary == NULL) ? NULL :
                           (dictDirective == usingDictCtx) ?
                            dictionary + dictSize - dictCtx->currentOffset :
                            dictionary + dictSize - startIndex;

    BYTE* op = (BYTE*) dest;
    BYTE* const olimit = op + maxOutputSize;

    U32 offset = 0;
    U32 forwardH;

    DEBUGLOG(5, "LZ4_compress_generic_validated: srcSize=%i, tableType=%u", inputSize, tableType);
    assert(ip != NULL);
    if (tableType == byU16) assert(inputSize<LZ4_64Klimit);  /* Размер слишком велик (не в пределах 64 КБ) */
    if (tableType == byPtr) assert(dictDirective==noDict);   /* поддерживается только вариант использования с byPtr */
    /* Если условия инициализации не выполнены, нам не нужно отмечать поток.
     * как имеющий грязный контекст, поскольку никаких действий еще не было предпринято */
    if (outputDirective == fillOutput && maxOutputSize < 1) { return 0; } /* невозможно ничего хранить */
    assert(acceleration >= 1);

    lowLimit = (const BYTE*)source - (dictDirective == withPrefix64k ? dictSize : 0);

    /* Обновить состояние контекста */
    if (dictDirective == usingDictCtx) {
        /* Последующие связанные блоки не могут использовать словарь. */
        /* Вместо этого они используют блок, который мы только что сжали. */
        cctx->dictCtx = NULL;
        cctx->dictSize = (U32)inputSize;
    } else {
        cctx->dictSize += (U32)inputSize;
    }
    cctx->currentOffset += (U32)inputSize;
    cctx->tableType = (U32)tableType;

    if (inputSize<LZ4_minLength) goto _last_literals;        /* Ввод слишком мал, нет сжатия (все литералы) */

    /* Первый байт */
    {   U32 const h = LZ4_hashPosition(ip, tableType);
        if (tableType == byPtr) {
            LZ4_putPositionOnHash(ip, h, cctx->hashTable, byPtr);
        } else {
            LZ4_putIndexOnHash(startIndex, h, cctx->hashTable, tableType);
    }   }
    ip++; forwardH = LZ4_hashPosition(ip, tableType);

    /* Основной цикл */
    for ( ; ; ) {
        const BYTE* match;
        BYTE* token;
        const BYTE* filledIp;

        /* Найти совпадение */
        if (tableType == byPtr) {
            const BYTE* forwardIp = ip;
            int step = 1;
            int searchMatchNb = acceleration << LZ4_skipTrigger;
            do {
                U32 const h = forwardH;
                ip = forwardIp;
                forwardIp += step;
                step = (searchMatchNb++ >> LZ4_skipTrigger);

                if (unlikely(forwardIp > mflimitPlusOne)) goto _last_literals;
                assert(ip < mflimitPlusOne);

                match = LZ4_getPositionOnHash(h, cctx->hashTable, tableType);
                forwardH = LZ4_hashPosition(forwardIp, tableType);
                LZ4_putPositionOnHash(ip, h, cctx->hashTable, tableType);

            } while ( (match+LZ4_DISTANCE_MAX < ip)
                   || (LZ4_read32(match) != LZ4_read32(ip)) );

        } else {   /* авторU32, авторU16 */

            const BYTE* forwardIp = ip;
            int step = 1;
            int searchMatchNb = acceleration << LZ4_skipTrigger;
            do {
                U32 const h = forwardH;
                U32 const current = (U32)(forwardIp - base);
                U32 matchIndex = LZ4_getIndexOnHash(h, cctx->hashTable, tableType);
                assert(matchIndex <= current);
                assert(forwardIp - base < (ptrdiff_t)(2 GB - 1));
                ip = forwardIp;
                forwardIp += step;
                step = (searchMatchNb++ >> LZ4_skipTrigger);

                if (unlikely(forwardIp > mflimitPlusOne)) goto _last_literals;
                assert(ip < mflimitPlusOne);

                if (dictDirective == usingDictCtx) {
                    if (matchIndex < startIndex) {
                        /* совпадений не было, попробуй словарь */
                        assert(tableType == byU32);
                        matchIndex = LZ4_getIndexOnHash(h, dictCtx->hashTable, byU32);
                        match = dictBase + matchIndex;
                        matchIndex += dictDelta;   /* сделать индекс dictCtx сопоставимым с текущим контекстом */
                        lowLimit = dictionary;
                    } else {
                        match = base + matchIndex;
                        lowLimit = (const BYTE*)source;
                    }
                } else if (dictDirective == usingExtDict) {
                    if (matchIndex < startIndex) {
                        DEBUGLOG(7, "extDict candidate: matchIndex=%5u  <  startIndex=%5u", matchIndex, startIndex);
                        assert(startIndex - matchIndex >= MINMATCH);
                        assert(dictBase);
                        match = dictBase + matchIndex;
                        lowLimit = dictionary;
                    } else {
                        match = base + matchIndex;
                        lowLimit = (const BYTE*)source;
                    }
                } else {   /* один непрерывный сегмент памяти */
                    match = base + matchIndex;
                }
                forwardH = LZ4_hashPosition(forwardIp, tableType);
                LZ4_putIndexOnHash(current, h, cctx->hashTable, tableType);

                DEBUGLOG(7, "candidate at pos=%u  (offset=%u \n", matchIndex, current - matchIndex);
                if ((dictIssue == dictSmall) && (matchIndex < prefixIdxLimit)) { continue; }    /* совпадение за пределами допустимой области */
                assert(matchIndex < current);
                if ( ((tableType != byU16) || (LZ4_DISTANCE_MAX < LZ4_DISTANCE_ABSOLUTE_MAX))
                  && (matchIndex+LZ4_DISTANCE_MAX < current)) {
                    continue;
                } /* слишком далеко */
                assert((current - matchIndex) <= LZ4_DISTANCE_MAX);  /* матч теперь ожидается на расстоянии */

                if (LZ4_read32(match) == LZ4_read32(ip)) {
                    if (maybe_extMem) offset = current - matchIndex;
                    break;   /* совпадение найдено */
                }

            } while(1);
        }

        /* Догонять */
        filledIp = ip;
        assert(ip > anchor); /* это всегда так, поскольку ip был расширен перед входом в основной цикл */
        if ((match > lowLimit) && unlikely(ip[-1] == match[-1])) {
            do { ip--; match--; } while (((ip > anchor) & (match > lowLimit)) && (unlikely(ip[-1] == match[-1])));
        }

        /* Кодирование литералов */
        {   unsigned const litLength = (unsigned)(ip - anchor);
            token = op++;
            if ((outputDirective == limitedOutput) &&  /* Проверьте переполнение выходного буфера */
                (unlikely(op + litLength + (2 + 1 + LASTLITERALS) + (litLength/255) > olimit)) ) {
                return 0;   /* невозможно сжать в рамках бюджета `dst`. Сохраненные индексы в хеш-таблице, тем не менее, в порядке. */
            }
            if ((outputDirective == fillOutput) &&
                (unlikely(op + (litLength+240)/255 /* маленький */ + litLength /* литералы */ + 2 /* компенсация */ + 1 /* жетон */ + MFLIMIT - MINMATCH /* минимум последних литералов, поэтому последнее совпадение <= end - MFLIMIT */ > olimit))) {
                op--;
                goto _last_literals;
            }
            if (litLength >= RUN_MASK) {
                unsigned len = litLength - RUN_MASK;
                *token = (RUN_MASK<<ML_BITS);
                for(; len >= 255 ; len-=255) *op++ = 255;
                *op++ = (BYTE)len;
            }
            else *token = (BYTE)(litLength<<ML_BITS);

            /* Копировать литералы */
            LZ4_wildCopy8(op, anchor, op+litLength);
            op+=litLength;
            DEBUGLOG(6, "seq.start:%i, literals=%u, match.start:%i",
                        (int)(anchor-(const BYTE*)source), litLength, (int)(ip-(const BYTE*)source));
        }

_next_match:
        /* на этом этапе должны быть правильно установлены следующие переменные:
         * - ip : at start of LZ operation
         * - match : at start of previous pattern occurrence; can be within current prefix, or within extDict
         * - offset : if maybe_ext_memSegment==1 (constant)
         * - lowLimit : must be == dictionary to mean "match is within extDict"; must be == source otherwise
         * - token and *token : position to write 4-bits for match length; higher 4-bits for literal length supposed already written
         */

        if ((outputDirective == fillOutput) &&
            (op + 2 /* компенсация */ + 1 /* жетон */ + MFLIMIT - MINMATCH /* минимум последних литералов, поэтому последнее совпадение <= end - MFLIMIT */ > olimit)) {
            /* матч был слишком близок к концу, перемотайте назад и перейдите к последним литералам */
            op = token;
            goto _last_literals;
        }

        /* Смещение кодирования */
        if (maybe_extMem) {   /* статический тест */
            DEBUGLOG(6, "             with offset=%u  (ext if > %i)", offset, (int)(ip - (const BYTE*)source));
            assert(offset <= LZ4_DISTANCE_MAX && offset > 0);
            LZ4_writeLE16(op, (U16)offset); op+=2;
        } else  {
            DEBUGLOG(6, "             with offset=%u  (same segment)", (U32)(ip - match));
            assert(ip-match <= LZ4_DISTANCE_MAX);
            LZ4_writeLE16(op, (U16)(ip - match)); op+=2;
        }

        /* Кодирование длины совпадения */
        {   unsigned matchCode;

            if ( (dictDirective==usingExtDict || dictDirective==usingDictCtx)
              && (lowLimit==dictionary) /* совпадение внутри extDict */ ) {
                const BYTE* limit = ip + (dictEnd-match);
                assert(dictEnd > match);
                if (limit > matchlimit) limit = matchlimit;
                matchCode = LZ4_count(ip+MINMATCH, match+MINMATCH, limit);
                ip += (size_t)matchCode + MINMATCH;
                if (ip==limit) {
                    unsigned const more = LZ4_count(limit, (const BYTE*)source, matchlimit);
                    matchCode += more;
                    ip += more;
                }
                DEBUGLOG(6, "             with matchLength=%u starting in extDict", matchCode+MINMATCH);
            } else {
                matchCode = LZ4_count(ip+MINMATCH, match+MINMATCH, matchlimit);
                ip += (size_t)matchCode + MINMATCH;
                DEBUGLOG(6, "             with matchLength=%u", matchCode+MINMATCH);
            }

            if ((outputDirective) &&    /* Проверьте переполнение выходного буфера */
                (unlikely(op + (1 + LASTLITERALS) + (matchCode+240)/255 > olimit)) ) {
                if (outputDirective == fillOutput) {
                    /* Описание матча слишком длинное: сократите его. */
                    U32 newMatchCode = 15 /* в жетоне */ - 1 /* чтобы избежать необходимости нулевого байта */ + ((U32)(olimit - op) - 1 - LASTLITERALS) * 255;
                    ip -= matchCode - newMatchCode;
                    assert(newMatchCode < matchCode);
                    matchCode = newMatchCode;
                    if (unlikely(ip <= filledIp)) {
                        /* Мы уже заполнили fillIp, поэтому, если ip окажется меньше fillIp
                         * у нас есть позиции в хеш-таблице, выходящие за пределы текущей позиции. Это
                         * проблема, если мы повторно используем хеш-таблицу. Поэтому нам придется удалить эти позиции.
                         * из хеш-таблицы.
                         */
                        const BYTE* ptr;
                        DEBUGLOG(5, "Clearing %u positions", (U32)(filledIp - ip));
                        for (ptr = ip; ptr <= filledIp; ++ptr) {
                            U32 const h = LZ4_hashPosition(ptr, tableType);
                            LZ4_clearHash(h, cctx->hashTable, tableType);
                        }
                    }
                } else {
                    assert(outputDirective == limitedOutput);
                    return 0;   /* невозможно сжать в рамках бюджета `dst`. Сохраненные индексы в хеш-таблице, тем не менее, в порядке. */
                }
            }
            if (matchCode >= ML_MASK) {
                *token += ML_MASK;
                matchCode -= ML_MASK;
                LZ4_write32(op, 0xFFFFFFFF);
                while (matchCode >= 4*255) {
                    op+=4;
                    LZ4_write32(op, 0xFFFFFFFF);
                    matchCode -= 4*255;
                }
                op += matchCode / 255;
                *op++ = (BYTE)(matchCode % 255);
            } else
                *token += (BYTE)(matchCode);
        }
        /* Убедитесь, что у нас достаточно места для последних литералов. */
        assert(!(outputDirective == fillOutput && op + 1 + LASTLITERALS > olimit));

        anchor = ip;

        /* Тестовый конец чанка */
        if (ip >= mflimitPlusOne) break;

        /* Заполнить таблицу */
        {   U32 const h = LZ4_hashPosition(ip-2, tableType);
            if (tableType == byPtr) {
                LZ4_putPositionOnHash(ip-2, h, cctx->hashTable, byPtr);
            } else {
                U32 const idx = (U32)((ip-2) - base);
                LZ4_putIndexOnHash(idx, h, cctx->hashTable, tableType);
        }   }

        /* Проверить следующую позицию */
        if (tableType == byPtr) {

            match = LZ4_getPosition(ip, cctx->hashTable, tableType);
            LZ4_putPosition(ip, cctx->hashTable, tableType);
            if ( (match+LZ4_DISTANCE_MAX >= ip)
              && (LZ4_read32(match) == LZ4_read32(ip)) )
            { token=op++; *token=0; goto _next_match; }

        } else {   /* авторU32, авторU16 */

            U32 const h = LZ4_hashPosition(ip, tableType);
            U32 const current = (U32)(ip-base);
            U32 matchIndex = LZ4_getIndexOnHash(h, cctx->hashTable, tableType);
            assert(matchIndex < current);
            if (dictDirective == usingDictCtx) {
                if (matchIndex < startIndex) {
                    /* совпадений не было, попробуй словарь */
                    assert(tableType == byU32);
                    matchIndex = LZ4_getIndexOnHash(h, dictCtx->hashTable, byU32);
                    match = dictBase + matchIndex;
                    lowLimit = dictionary;   /* требуется для счетчика длины совпадения */
                    matchIndex += dictDelta;
                } else {
                    match = base + matchIndex;
                    lowLimit = (const BYTE*)source;  /* требуется для счетчика длины совпадения */
                }
            } else if (dictDirective==usingExtDict) {
                if (matchIndex < startIndex) {
                    assert(dictBase);
                    match = dictBase + matchIndex;
                    lowLimit = dictionary;   /* требуется для счетчика длины совпадения */
                } else {
                    match = base + matchIndex;
                    lowLimit = (const BYTE*)source;   /* требуется для счетчика длины совпадения */
                }
            } else {   /* один сегмент памяти */
                match = base + matchIndex;
            }
            LZ4_putIndexOnHash(current, h, cctx->hashTable, tableType);
            assert(matchIndex < current);
            if ( ((dictIssue==dictSmall) ? (matchIndex >= prefixIdxLimit) : 1)
              && (((tableType==byU16) && (LZ4_DISTANCE_MAX == LZ4_DISTANCE_ABSOLUTE_MAX)) ? 1 : (matchIndex+LZ4_DISTANCE_MAX >= current))
              && (LZ4_read32(match) == LZ4_read32(ip)) ) {
                token=op++;
                *token=0;
                if (maybe_extMem) offset = current - matchIndex;
                DEBUGLOG(6, "seq.start:%i, literals=%u, match.start:%i",
                            (int)(anchor-(const BYTE*)source), 0, (int)(ip-(const BYTE*)source));
                goto _next_match;
            }
        }

        /* Подготовьте следующий цикл */
        forwardH = LZ4_hashPosition(++ip, tableType);

    }

_last_literals:
    /* Кодировать последние литералы */
    {   size_t lastRun = (size_t)(iend - anchor);
        if ( (outputDirective) &&  /* Проверьте переполнение выходного буфера */
            (op + lastRun + 1 + ((lastRun+255-RUN_MASK)/255) > olimit)) {
            if (outputDirective == fillOutput) {
                /* адаптировать LastRun для заполнения «dst» */
                assert(olimit >= op);
                lastRun  = (size_t)(olimit-op) - 1/*жетон*/;
                lastRun -= (lastRun + 256 - RUN_MASK) / 256;  /*дополнительные жетоны длины*/
            } else {
                assert(outputDirective == limitedOutput);
                return 0;   /* невозможно сжать в рамках бюджета `dst`. Сохраненные индексы в хеш-таблице, тем не менее, в порядке. */
            }
        }
        DEBUGLOG(6, "Final literal run : %i literals", (int)lastRun);
        if (lastRun >= RUN_MASK) {
            size_t accumulator = lastRun - RUN_MASK;
            *op++ = RUN_MASK << ML_BITS;
            for(; accumulator >= 255 ; accumulator-=255) *op++ = 255;
            *op++ = (BYTE) accumulator;
        } else {
            *op++ = (BYTE)(lastRun<<ML_BITS);
        }
        LZ4_memcpy(op, anchor, lastRun);
        ip = anchor + lastRun;
        op += lastRun;
    }

    if (outputDirective == fillOutput) {
        *inputConsumed = (int) (((const char*)ip)-source);
    }
    result = (int)(((char*)op) - dest);
    assert(result > 0);
    DEBUGLOG(5, "LZ4_compress_generic: compressed %i bytes into %i bytes", inputSize, result);
    return result;
}

/** LZ4_compress_generic () :
 *  встроенный, чтобы обеспечить выбор ветвей во время компиляции;
 *  заботится о src == ( NULL , 0)
 *  и пересылаем остальное на LZ4_compress_generic_validated */
LZ4_FORCE_INLINE int LZ4_compress_generic(
                 LZ4_stream_t_internal* const cctx,
                 const char* const src,
                 char* const dst,
                 const int srcSize,
                 int *inputConsumed, /* пишется только тогда, когда outputDirective == fillOutput */
                 const int dstCapacity,
                 const limitedOutput_directive outputDirective,
                 const tableType_t tableType,
                 const dict_directive dictDirective,
                 const dictIssue_directive dictIssue,
                 const int acceleration)
{
    DEBUGLOG(5, "LZ4_compress_generic: srcSize=%i, dstCapacity=%i",
                srcSize, dstCapacity);

    if ((U32)srcSize > (U32)LZ4_MAX_INPUT_SIZE) { return 0; }  /* Неподдерживаемый srcSize, слишком большой (или отрицательный). */
    if (srcSize == 0) {   /* src == NULL поддерживается, если srcSize == 0 */
        if (outputDirective != notLimited && dstCapacity <= 0) return 0;  /* нет вывода, ничего не могу написать */
        DEBUGLOG(5, "Generating an empty block");
        assert(outputDirective == notLimited || dstCapacity >= 1);
        assert(dst != NULL);
        dst[0] = 0;
        if (outputDirective == fillOutput) {
            assert (inputConsumed != NULL);
            *inputConsumed = 0;
        }
        return 1;
    }
    assert(src != NULL);

    return LZ4_compress_generic_validated(cctx, src, dst, srcSize,
                inputConsumed, /* записывается только в том случае, если outputDirective == fillOutput */
                dstCapacity, outputDirective,
                tableType, dictDirective, dictIssue, acceleration);
}


int LZ4_compress_fast_extState(void* state, const char* source, char* dest, int inputSize, int maxOutputSize, int acceleration)
{
    LZ4_stream_t_internal* const ctx = & LZ4_initStream(state, sizeof(LZ4_stream_t)) -> internal_donotuse;
    assert(ctx != NULL);
    if (acceleration < 1) acceleration = LZ4_ACCELERATION_DEFAULT;
    if (acceleration > LZ4_ACCELERATION_MAX) acceleration = LZ4_ACCELERATION_MAX;
    if (maxOutputSize >= LZ4_compressBound(inputSize)) {
        if (inputSize < LZ4_64Klimit) {
            return LZ4_compress_generic(ctx, source, dest, inputSize, NULL, 0, notLimited, byU16, noDict, noDictIssue, acceleration);
        } else {
            const tableType_t tableType = ((sizeof(void*)==4) && ((uptrval)source > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
            return LZ4_compress_generic(ctx, source, dest, inputSize, NULL, 0, notLimited, tableType, noDict, noDictIssue, acceleration);
        }
    } else {
        if (inputSize < LZ4_64Klimit) {
            return LZ4_compress_generic(ctx, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, byU16, noDict, noDictIssue, acceleration);
        } else {
            const tableType_t tableType = ((sizeof(void*)==4) && ((uptrval)source > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
            return LZ4_compress_generic(ctx, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, noDict, noDictIssue, acceleration);
        }
    }
}

/**
 * LZ4_compress_fast_extState_fastReset () :
 * Вариант LZ4_compress_fast_extState().
 *
 * Использование этого варианта позволяет избежать дорогостоящего этапа инициализации. Это только безопасно
 * для вызова, если известно, что буфер состояния уже правильно инициализирован
 * (см. комментарий в lz4.h к LZ4_resetStream_fast () для определения
 * «правильно инициализирован»).
 */
int LZ4_compress_fast_extState_fastReset(void* state, const char* src, char* dst, int srcSize, int dstCapacity, int acceleration)
{
    LZ4_stream_t_internal* const ctx = &((LZ4_stream_t*)state)->internal_donotuse;
    if (acceleration < 1) acceleration = LZ4_ACCELERATION_DEFAULT;
    if (acceleration > LZ4_ACCELERATION_MAX) acceleration = LZ4_ACCELERATION_MAX;
    assert(ctx != NULL);

    if (dstCapacity >= LZ4_compressBound(srcSize)) {
        if (srcSize < LZ4_64Klimit) {
            const tableType_t tableType = byU16;
            LZ4_prepareTable(ctx, srcSize, tableType);
            if (ctx->currentOffset) {
                return LZ4_compress_generic(ctx, src, dst, srcSize, NULL, 0, notLimited, tableType, noDict, dictSmall, acceleration);
            } else {
                return LZ4_compress_generic(ctx, src, dst, srcSize, NULL, 0, notLimited, tableType, noDict, noDictIssue, acceleration);
            }
        } else {
            const tableType_t tableType = ((sizeof(void*)==4) && ((uptrval)src > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
            LZ4_prepareTable(ctx, srcSize, tableType);
            return LZ4_compress_generic(ctx, src, dst, srcSize, NULL, 0, notLimited, tableType, noDict, noDictIssue, acceleration);
        }
    } else {
        if (srcSize < LZ4_64Klimit) {
            const tableType_t tableType = byU16;
            LZ4_prepareTable(ctx, srcSize, tableType);
            if (ctx->currentOffset) {
                return LZ4_compress_generic(ctx, src, dst, srcSize, NULL, dstCapacity, limitedOutput, tableType, noDict, dictSmall, acceleration);
            } else {
                return LZ4_compress_generic(ctx, src, dst, srcSize, NULL, dstCapacity, limitedOutput, tableType, noDict, noDictIssue, acceleration);
            }
        } else {
            const tableType_t tableType = ((sizeof(void*)==4) && ((uptrval)src > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
            LZ4_prepareTable(ctx, srcSize, tableType);
            return LZ4_compress_generic(ctx, src, dst, srcSize, NULL, dstCapacity, limitedOutput, tableType, noDict, noDictIssue, acceleration);
        }
    }
}


int LZ4_compress_fast(const char* src, char* dest, int srcSize, int dstCapacity, int acceleration)
{
    int result;
#if (LZ4_HEAPMODE)
    LZ4_stream_t* const ctxPtr = (LZ4_stream_t*)ALLOC(sizeof(LZ4_stream_t));   /* malloc-calloc всегда правильно выровнен */
    if (ctxPtr == NULL) return 0;
#else
    LZ4_stream_t ctx;
    LZ4_stream_t* const ctxPtr = &ctx;
#endif
    result = LZ4_compress_fast_extState(ctxPtr, src, dest, srcSize, dstCapacity, acceleration);

#if (LZ4_HEAPMODE)
    FREEMEM(ctxPtr);
#endif
    return result;
}


int LZ4_compress_default(const char* src, char* dst, int srcSize, int dstCapacity)
{
    return LZ4_compress_fast(src, dst, srcSize, dstCapacity, 1);
}


/* Примечание! Эта функция оставляет поток в нечистом/испорченном состоянии!
 * Небезопасно впоследствии использовать то же состояние с _fastReset() или
 * _continue() вызов без его сброса. */
static int LZ4_compress_destSize_extState_internal(LZ4_stream_t* state, const char* src, char* dst, int* srcSizePtr, int targetDstSize, int acceleration)
{
    void* const s = LZ4_initStream(state, sizeof (*state));
    assert(s != NULL); (void)s;

    if (targetDstSize >= LZ4_compressBound(*srcSizePtr)) {  /* успех сжатия гарантирован */
        return LZ4_compress_fast_extState(state, src, dst, *srcSizePtr, targetDstSize, acceleration);
    } else {
        if (*srcSizePtr < LZ4_64Klimit) {
            return LZ4_compress_generic(&state->internal_donotuse, src, dst, *srcSizePtr, srcSizePtr, targetDstSize, fillOutput, byU16, noDict, noDictIssue, acceleration);
        } else {
            tableType_t const addrMode = ((sizeof(void*)==4) && ((uptrval)src > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
            return LZ4_compress_generic(&state->internal_donotuse, src, dst, *srcSizePtr, srcSizePtr, targetDstSize, fillOutput, addrMode, noDict, noDictIssue, acceleration);
    }   }
}

int LZ4_compress_destSize_extState(void* state, const char* src, char* dst, int* srcSizePtr, int targetDstSize, int acceleration)
{
    int const r = LZ4_compress_destSize_extState_internal((LZ4_stream_t*)state, src, dst, srcSizePtr, targetDstSize, acceleration);
    /* очистить состояние при выходе */
    LZ4_initStream(state, sizeof (LZ4_stream_t));
    return r;
}


int LZ4_compress_destSize(const char* src, char* dst, int* srcSizePtr, int targetDstSize)
{
#if (LZ4_HEAPMODE)
    LZ4_stream_t* const ctx = (LZ4_stream_t*)ALLOC(sizeof(LZ4_stream_t));   /* malloc-calloc всегда правильно выровнен */
    if (ctx == NULL) return 0;
#else
    LZ4_stream_t ctxBody;
    LZ4_stream_t* const ctx = &ctxBody;
#endif

    int result = LZ4_compress_destSize_extState_internal(ctx, src, dst, srcSizePtr, targetDstSize, 1);

#if (LZ4_HEAPMODE)
    FREEMEM(ctx);
#endif
    return result;
}



/*-******************************
*  Функции потоковой передачи
********************************/

#if !defined(LZ4_STATIC_LINKING_ONLY_DISABLE_MEMORY_ALLOCATION)
LZ4_stream_t* LZ4_createStream(void)
{
    LZ4_stream_t* const lz4s = (LZ4_stream_t*)ALLOC(sizeof(LZ4_stream_t));
    LZ4_STATIC_ASSERT(sizeof(LZ4_stream_t) >= sizeof(LZ4_stream_t_internal));
    DEBUGLOG(4, "LZ4_createStream %p", lz4s);
    if (lz4s == NULL) return NULL;
    LZ4_initStream(lz4s, sizeof(*lz4s));
    return lz4s;
}
#endif

static size_t LZ4_stream_t_alignment(void)
{
#if LZ4_ALIGN_TEST
    typedef struct { char c; LZ4_stream_t t; } t_a;
    return sizeof(t_a) - sizeof(LZ4_stream_t);
#else
    return 1;  /* эффективно отключен */
#endif
}

LZ4_stream_t* LZ4_initStream (void* buffer, size_t size)
{
    DEBUGLOG(5, "LZ4_initStream");
    if (buffer == NULL) { return NULL; }
    if (size < sizeof(LZ4_stream_t)) { return NULL; }
    if (!LZ4_isAligned(buffer, LZ4_stream_t_alignment())) return NULL;
    MEM_INIT(buffer, 0, sizeof(LZ4_stream_t_internal));
    return (LZ4_stream_t*)buffer;
}

/* resetStream больше не поддерживается,
 * предпочитаю initStream(), который является более общим */
void LZ4_resetStream (LZ4_stream_t* LZ4_stream)
{
    DEBUGLOG(5, "LZ4_resetStream (ctx:%p)", LZ4_stream);
    MEM_INIT(LZ4_stream, 0, sizeof(LZ4_stream_t_internal));
}

void LZ4_resetStream_fast(LZ4_stream_t* ctx) {
    LZ4_prepareTable(&(ctx->internal_donotuse), 0, byU32);
}

#if !defined(LZ4_STATIC_LINKING_ONLY_DISABLE_MEMORY_ALLOCATION)
int LZ4_freeStream (LZ4_stream_t* LZ4_stream)
{
    if (!LZ4_stream) return 0;   /* бесплатная поддержка на NULL */
    DEBUGLOG(5, "LZ4_freeStream %p", LZ4_stream);
    FREEMEM(LZ4_stream);
    return (0);
}
#endif


typedef enum { _ld_fast, _ld_slow } LoadDict_mode_e;
#define HASH_UNIT sizeof(reg_t)
static int LZ4_loadDict_internal(LZ4_stream_t* LZ4_dict,
                    const char* dictionary, int dictSize,
                    LoadDict_mode_e _ld)
{
    LZ4_stream_t_internal* const dict = &LZ4_dict->internal_donotuse;
    const tableType_t tableType = byU32;
    const BYTE* p = (const BYTE*)dictionary;
    const BYTE* const dictEnd = p + dictSize;
    U32 idx32;

    DEBUGLOG(4, "LZ4_loadDict (%i bytes from %p into %p)", dictSize, dictionary, LZ4_dict);

    /* Необходимо сбросить контекст,
     * а не просто продолжить с prepareTable()
     * чтобы избежать риска создания переполнения matchIndex
     * при сжатии с использованием этого словаря */
    LZ4_resetStream(LZ4_dict);

    /* Мы всегда увеличиваем смещение на 64 KB, поскольку, если dict длиннее,
     * мы урезаем его до последних 64 тысяч, и если оно короче, мы все равно хотим
     * вперед на всю длину окна, чтобы мы могли гарантировать, что
     * в окне есть только действительные смещения, что позволяет оптимизировать
     * в LZ4_compress_fast_continue(), где используется noDictIssue, даже если
     * словарь не заполнен на 64 КБ. */
    dict->currentOffset += 64 KB;

    if (dictSize < (int)HASH_UNIT) {
        return 0;
    }

    if ((dictEnd - p) > 64 KB) p = dictEnd - 64 KB;
    dict->dictionary = p;
    dict->dictSize = (U32)(dictEnd - p);
    dict->tableType = (U32)tableType;
    idx32 = dict->currentOffset - dict->dictSize;

    while (p <= dictEnd-HASH_UNIT) {
        U32 const h = LZ4_hashPosition(p, tableType);
        /* Note: overwriting => favors positions end of dictionary */
        LZ4_putIndexOnHash(idx32, h, dict->hashTable, tableType);
        p+=3; idx32+=3;
    }

    if (_ld == _ld_slow) {
        /* Заполните хэш-таблицу дополнительными ссылками, чтобы улучшить возможности сжатия. */
        p = dict->dictionary;
        idx32 = dict->currentOffset - dict->dictSize;
        while (p <= dictEnd-HASH_UNIT) {
            U32 const h = LZ4_hashPosition(p, tableType);
            U32 const limit = dict->currentOffset - 64 KB;
            if (LZ4_getIndexOnHash(h, dict->hashTable, tableType) <= limit) {
                /* Note: not overwriting => favors positions beginning of dictionary */
                LZ4_putIndexOnHash(idx32, h, dict->hashTable, tableType);
            }
            p++; idx32++;
        }
    }

    return (int)dict->dictSize;
}

int LZ4_loadDict(LZ4_stream_t* LZ4_dict, const char* dictionary, int dictSize)
{
    return LZ4_loadDict_internal(LZ4_dict, dictionary, dictSize, _ld_fast);
}

int LZ4_loadDictSlow(LZ4_stream_t* LZ4_dict, const char* dictionary, int dictSize)
{
    return LZ4_loadDict_internal(LZ4_dict, dictionary, dictSize, _ld_slow);
}

void LZ4_attach_dictionary(LZ4_stream_t* workingStream, const LZ4_stream_t* dictionaryStream)
{
    const LZ4_stream_t_internal* dictCtx = (dictionaryStream == NULL) ? NULL :
        &(dictionaryStream->internal_donotuse);

    DEBUGLOG(4, "LZ4_attach_dictionary (%p, %p, size %u)",
             workingStream, dictionaryStream,
             dictCtx != NULL ? dictCtx->dictSize : 0);

    if (dictCtx != NULL) {
        /* Если текущее смещение равно нулю, мы никогда не будем искать
         * контекст внешнего словаря, поскольку таблица не имеет значения
         * запись может принять это, что указывает на промах. В таком случае нам понадобится
         * чтобы увеличить смещение до чего-то отличного от нуля.
         */
        if (workingStream->internal_donotuse.currentOffset == 0) {
            workingStream->internal_donotuse.currentOffset = 64 KB;
        }

        /* На самом деле не прикрепляйте пустой словарь.
         */
        if (dictCtx->dictSize == 0) {
            dictCtx = NULL;
        }
    }
    workingStream->internal_donotuse.dictCtx = dictCtx;
}


static void LZ4_renormDictT(LZ4_stream_t_internal* LZ4_dict, int nextSize)
{
    assert(nextSize >= 0);
    if (LZ4_dict->currentOffset + (unsigned)nextSize > 0x80000000) {   /* потенциальное переполнение ptrdiff_t (32-битный режим) */
        /* масштабировать хеш-таблицу */
        U32 const delta = LZ4_dict->currentOffset - 64 KB;
        const BYTE* dictEnd = LZ4_dict->dictionary + LZ4_dict->dictSize;
        int i;
        DEBUGLOG(4, "LZ4_renormDictT");
        for (i=0; i<LZ4_HASH_SIZE_U32; i++) {
            if (LZ4_dict->hashTable[i] < delta) LZ4_dict->hashTable[i]=0;
            else LZ4_dict->hashTable[i] -= delta;
        }
        LZ4_dict->currentOffset = 64 KB;
        if (LZ4_dict->dictSize > 64 KB) LZ4_dict->dictSize = 64 KB;
        LZ4_dict->dictionary = dictEnd - LZ4_dict->dictSize;
    }
}


int LZ4_compress_fast_continue (LZ4_stream_t* LZ4_stream,
                                const char* source, char* dest,
                                int inputSize, int maxOutputSize,
                                int acceleration)
{
    const tableType_t tableType = byU32;
    LZ4_stream_t_internal* const streamPtr = &LZ4_stream->internal_donotuse;
    const char* dictEnd = streamPtr->dictSize ? (const char*)streamPtr->dictionary + streamPtr->dictSize : NULL;

    DEBUGLOG(5, "LZ4_compress_fast_continue (inputSize=%i, dictSize=%u)", inputSize, streamPtr->dictSize);

    LZ4_renormDictT(streamPtr, inputSize);   /* исправить переполнение индекса */
    if (acceleration < 1) acceleration = LZ4_ACCELERATION_DEFAULT;
    if (acceleration > LZ4_ACCELERATION_MAX) acceleration = LZ4_ACCELERATION_MAX;

    /* аннулировать крошечные словари */
    if ( (streamPtr->dictSize < 4)     /* крошечный словарь: недостаточно для хеша */
      && (dictEnd != source)           /* префиксный режим */
      && (inputSize > 0)               /* tolerance : don't lose history, in case next invocation would use prefix mode */
      && (streamPtr->dictCtx == NULL)  /* использованиеDictCtx */
      ) {
        DEBUGLOG(5, "LZ4_compress_fast_continue: dictSize(%u) at addr:%p is too small", streamPtr->dictSize, streamPtr->dictionary);
        /* удалить существование словаря из истории, чтобы использовать более быстрый режим префикса */
        streamPtr->dictSize = 0;
        streamPtr->dictionary = (const BYTE*)source;
        dictEnd = source;
    }

    /* Проверьте перекрывающееся пространство ввода/словаря */
    {   const char* const sourceEnd = source + inputSize;
        if ((sourceEnd > (const char*)streamPtr->dictionary) && (sourceEnd < dictEnd)) {
            streamPtr->dictSize = (U32)(dictEnd - sourceEnd);
            if (streamPtr->dictSize > 64 KB) streamPtr->dictSize = 64 KB;
            if (streamPtr->dictSize < 4) streamPtr->dictSize = 0;
            streamPtr->dictionary = (const BYTE*)dictEnd - streamPtr->dictSize;
        }
    }

    /* режим префикса: исходные данные следуют за словарем */
    if (dictEnd == source) {
        if ((streamPtr->dictSize < 64 KB) && (streamPtr->dictSize < streamPtr->currentOffset))
            return LZ4_compress_generic(streamPtr, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, withPrefix64k, dictSmall, acceleration);
        else
            return LZ4_compress_generic(streamPtr, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, withPrefix64k, noDictIssue, acceleration);
    }

    /* режим внешнего словаря */
    {   int result;
        if (streamPtr->dictCtx) {
            /* Здесь мы зависим от того, что dictCtx'es (произведенный
             * LZ4_loadDict ) гарантируют, что их таблицы не содержат ссылок.
             * для смещений между dictCtx->currentOffset - 64 KB и
             * dictCtx->currentOffset — dictCtx->dictSize. Это делает его безопасным
             * использовать noDictIssue, даже если dict не является полным 64 KB .
             */
            if (inputSize > 4 KB) {
                /* Для сжатия больших двоичных объектов быстрее заплатить за установку.
                 * стоимость копирования таблиц словаря в активный контекст,
                 * так что цикл сжатия просматривает только одну таблицу.
                 */
                LZ4_memcpy(streamPtr, streamPtr->dictCtx, sizeof(*streamPtr));
                result = LZ4_compress_generic(streamPtr, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, usingExtDict, noDictIssue, acceleration);
            } else {
                result = LZ4_compress_generic(streamPtr, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, usingDictCtx, noDictIssue, acceleration);
            }
        } else {  /* небольшие данные <= 4 KB */
            if ((streamPtr->dictSize < 64 KB) && (streamPtr->dictSize < streamPtr->currentOffset)) {
                result = LZ4_compress_generic(streamPtr, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, usingExtDict, dictSmall, acceleration);
            } else {
                result = LZ4_compress_generic(streamPtr, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, usingExtDict, noDictIssue, acceleration);
            }
        }
        streamPtr->dictionary = (const BYTE*)source;
        streamPtr->dictSize = (U32)inputSize;
        return result;
    }
}


/* Скрытая функция отладки для принудительного тестирования режима внешнего словаря. */
int LZ4_compress_forceExtDict (LZ4_stream_t* LZ4_dict, const char* source, char* dest, int srcSize)
{
    LZ4_stream_t_internal* const streamPtr = &LZ4_dict->internal_donotuse;
    int result;

    LZ4_renormDictT(streamPtr, srcSize);

    if ((streamPtr->dictSize < 64 KB) && (streamPtr->dictSize < streamPtr->currentOffset)) {
        result = LZ4_compress_generic(streamPtr, source, dest, srcSize, NULL, 0, notLimited, byU32, usingExtDict, dictSmall, 1);
    } else {
        result = LZ4_compress_generic(streamPtr, source, dest, srcSize, NULL, 0, notLimited, byU32, usingExtDict, noDictIssue, 1);
    }

    streamPtr->dictionary = (const BYTE*)source;
    streamPtr->dictSize = (U32)srcSize;

    return result;
}


/*! LZ4_saveDict () :
 *  Если ранее сжатый блок данных не гарантированно останется доступным в своей памяти,
 *  сохраните его в более безопасном месте (char*safeBuffer).
 *  Note : no need to call LZ4_loadDict() afterwards, dictionary is immediately usable,
 *         поэтому можно сразу после этого вызвать LZ4_compress_fast_continue().
 * @return : saved dictionary size in bytes (necessarily <= dictSize), or 0 if error.
 */
int LZ4_saveDict (LZ4_stream_t* LZ4_dict, char* safeBuffer, int dictSize)
{
    LZ4_stream_t_internal* const dict = &LZ4_dict->internal_donotuse;

    DEBUGLOG(5, "LZ4_saveDict : dictSize=%i, safeBuffer=%p", dictSize, safeBuffer);

    if ((U32)dictSize > 64 KB) { dictSize = 64 KB; } /* бесполезно определять словарь > 64 KB */
    if ((U32)dictSize > dict->dictSize) { dictSize = (int)dict->dictSize; }

    if (safeBuffer == NULL) assert(dictSize == 0);
    if (dictSize > 0) {
        const BYTE* const previousDictEnd = dict->dictionary + dict->dictSize;
        assert(dict->dictionary);
        LZ4_memmove(safeBuffer, previousDictEnd - dictSize, (size_t)dictSize);
    }

    dict->dictionary = (const BYTE*)safeBuffer;
    dict->dictSize = (U32)dictSize;

    return dictSize;
}



/*-*******************************
 *  Функции декомпрессии
 ********************************/

typedef enum { decode_full_block = 0, partial_decode = 1 } earlyEnd_directive;

#undef MIN
#define MIN(a,b)    ( (a) < (b) ? (a) : (b) )


/* вариант для decompress_unsafe ()
 * не знает окончания ввода
 * предполагает, что ввод правильно сформирован
 * note : will consume at least one byte */
static size_t read_long_length_no_check(const BYTE** pp)
{
    size_t b, l = 0;
    do { b = **pp; (*pp)++; l += b; } while (b==255);
    DEBUGLOG(6, "read_long_length_no_check: +length=%zu using %zu input bytes", l, l/255 + 1)
    return l;
}

/* вариант основного декодера для LZ4_decompress_fast *()
 * только для поддержки устаревших версий: эти точки входа устарели.
 * - Presumes input is correctly formed (no defense vs malformed inputs)
 * - Does not know input size (presume input buffer is "large enough")
 * - Decompress a full block (only)
 * @return : nb of bytes read from input.
 * Note : this variant is not optimized for speed, just for maintenance.
 *        цель состоит в том, чтобы удалить поддержку вариантов decompress_fast *() в версии 2.0.
**/
LZ4_FORCE_INLINE int
LZ4_decompress_unsafe_generic(
                 const BYTE* const istart,
                 BYTE* const ostart,
                 int decompressedSize,

                 size_t prefixSize,
                 const BYTE* const dictStart,  /* только если dict==usingExtDict */
                 const size_t dictSize         /* note: =0 if dictStart==NULL */
                 )
{
    const BYTE* ip = istart;
    BYTE* op = (BYTE*)ostart;
    BYTE* const oend = ostart + decompressedSize;
    const BYTE* const prefixStart = ostart - prefixSize;

    DEBUGLOG(5, "LZ4_decompress_unsafe_generic");
    if (dictStart == NULL) assert(dictSize == 0);

    while (1) {
        /* начать новую последовательность */
        unsigned token = *ip++;

        /* литералы */
        {   size_t ll = token >> ML_BITS;
            if (ll==15) {
                /* большая буквальная длина */
                ll += read_long_length_no_check(&ip);
            }
            if ((size_t)(oend-op) < ll) return -1; /* переполнение выходного буфера */
            LZ4_memmove(op, ip, ll); /* поддержка декомпрессии на месте */
            op += ll;
            ip += ll;
            if ((size_t)(oend-op) < MFLIMIT) {
                if (op==oend) break;  /* конец блока */
                DEBUGLOG(5, "invalid: literals end at distance %zi from end of block", oend-op);
                /* неправильный конец блока:
                 * последнее совпадение должно начинаться как минимум за MFLIMIT == 12 байт до конца выходного блока */
                return -1;
        }   }

        /* матч */
        {   size_t ml = token & 15;
            size_t const offset = LZ4_readLE16(ip);
            ip+=2;

            if (ml==15) {
                /* большая буквальная длина */
                ml += read_long_length_no_check(&ip);
            }
            ml += MINMATCH;

            if ((size_t)(oend-op) < ml) return -1; /* переполнение выходного буфера */

            {   const BYTE* match = op - offset;

                /* вне диапазона */
                if (offset > (size_t)(op - prefixStart) + dictSize) {
                    DEBUGLOG(6, "offset out of range");
                    return -1;
                }

                /* проверьте особый случай: extDict */
                if (offset > (size_t)(op - prefixStart)) {
                    /* сценарий extDict */
                    const BYTE* const dictEnd = dictStart + dictSize;
                    const BYTE* extMatch = dictEnd - (offset - (size_t)(op-prefixStart));
                    size_t const extml = (size_t)(dictEnd - extMatch);
                    if (extml > ml) {
                        /* полностью совпадать с extDict */
                        LZ4_memmove(op, extMatch, ml);
                        op += ml;
                        ml = 0;
                    } else {
                        /* совпадение разделения между extDict и префиксом */
                        LZ4_memmove(op, extMatch, extml);
                        op += extml;
                        ml -= extml;
                    }
                    match = prefixStart;
                }

                /* копия совпадения — медленный вариант, поддерживающий копирование с перекрытием */
                {   size_t u;
                    for (u=0; u<ml; u++) {
                        op[u] = match[u];
            }   }   }
            op += ml;
            if ((size_t)(oend-op) < LASTLITERALS) {
                DEBUGLOG(5, "invalid: match ends at distance %zi from end of block", oend-op);
                /* неправильный конец блока:
                 * последнее совпадение должно прекратиться как минимум за LASTLITERALS == 5 байт до конца выходного блока */
                return -1;
            }
        } /* матч */
    } /* основной цикл */
    return (int)(ip - istart);
}


/* Прочитайте литерал переменной длины или длину совпадения.
 *
 * @ip : указатель ввода
 * @ilimit : позиция, после которой, если длина не декодирована, входные данные обязательно будут повреждены.
 * @initial_check — проверьте ip >= ipmax перед началом цикла.  Если да, возвращает initial_error.
 * @error (выход) — код ошибки.  Перед вызовом необходимо установить значение 0.
**/
typedef size_t Rvl_t;
static const Rvl_t rvl_error = (Rvl_t)(-1);
LZ4_FORCE_INLINE Rvl_t
read_variable_length(const BYTE** ip, const BYTE* ilimit,
                     int initial_check)
{
    Rvl_t s, length = 0;
    assert(ip != NULL);
    assert(*ip !=  NULL);
    assert(ilimit != NULL);
    if (initial_check && unlikely((*ip) >= ilimit)) {    /* достигнут лимит чтения */
        return rvl_error;
    }
    s = **ip;
    (*ip)++;
    length += s;
    if (unlikely((*ip) > ilimit)) {    /* достигнут лимит чтения */
        return rvl_error;
    }
    /* обнаружение переполнения аккумулятора (только 32-битный режим) */
    if ((sizeof(length) < 8) && unlikely(length > ((Rvl_t)(-1)/2)) ) {
        return rvl_error;
    }
    if (likely(s != 255)) return length;
    do {
        s = **ip;
        (*ip)++;
        length += s;
        if (unlikely((*ip) > ilimit)) {    /* достигнут лимит чтения */
            return rvl_error;
        }
        /* обнаружение переполнения аккумулятора (только 32-битный режим) */
        if ((sizeof(length) < 8) && unlikely(length > ((Rvl_t)(-1)/2)) ) {
            return rvl_error;
        }
    } while (s == 255);

    return length;
}

/*! LZ4_decompress_generic () :
 *  Эта общая функция декомпрессии охватывает все варианты использования.
 *  Его экземпляр должен быть создан несколько раз с использованием разных наборов директив.
 *  Обратите внимание: для производительности важно, чтобы эта функция действительно была встроенной.
 *  чтобы удалить ненужные ветки при оптимизации компиляции.
 */
LZ4_FORCE_INLINE int
LZ4_decompress_generic(
                 const char* const src,
                 char* const dst,
                 int srcSize,
                 int outputSize,         /* Если endOnInput==endOnInputSize, это значение равно `dstCapacity`. */

                 earlyEnd_directive partialDecoding,  /* полный, частичный */
                 dict_directive dict,                 /* noDict, withPrefix64k, usingExtDict */
                 const BYTE* const lowPrefix,  /* всегда <= dst, == dst, если нет префикса */
                 const BYTE* const dictStart,  /* только если dict==usingExtDict */
                 const size_t dictSize         /* note : = 0 if noDict */
                 )
{
    if ((src == NULL) || (outputSize < 0)) { return -1; }

    {   const BYTE* ip = (const BYTE*) src;
        const BYTE* const iend = ip + srcSize;

        BYTE* op = (BYTE*) dst;
        BYTE* const oend = op + outputSize;
        BYTE* cpy;

        const BYTE* const dictEnd = (dictStart == NULL) ? NULL : dictStart + dictSize;

        const int checkOffset = (dictSize < (int)(64 KB));


        /* Настройте «конечные» указатели для ярлыка. */
        const BYTE* const shortiend = iend - 14 /*МаксЛЛ*/ - 2 /*компенсация*/;
        const BYTE* const shortoend = oend - 14 /*МаксЛЛ*/ - 18 /*максМЛ*/;

        const BYTE* match;
        size_t offset;
        unsigned token;
        size_t length;


        DEBUGLOG(5, "LZ4_decompress_generic (srcSize:%i, dstSize:%i)", srcSize, outputSize);

        /* Особые случаи */
        assert(lowPrefix <= op);
        if (unlikely(outputSize==0)) {
            /* Пустой выходной буфер */
            if (partialDecoding) return 0;
            return ((srcSize==1) && (*ip==0)) ? 0 : -1;
        }
        if (unlikely(srcSize==0)) { return -1; }

    /* LZ4_FAST_DEC_LOOP:
     * разработан для современных процессоров высокой производительности,
     * где надежное копирование 32-байт предпочтительнее непредсказуемой ветви.
     * note : fast loop may show a regression for some client arm chips. */
#if LZ4_FAST_DEC_LOOP
        if ((oend - op) < FASTLOOP_SAFE_DISTANCE) {
            DEBUGLOG(6, "move to safe decode loop");
            goto safe_decode;
        }

        /* Быстрый цикл: декодируйте последовательности, пока выход < oend- FASTLOOP_SAFE_DISTANCE */
        DEBUGLOG(6, "using fast decode loop");
        while (1) {
            /* Основное утверждение быстрого цикла: мы всегда можем скопировать FASTLOOP_SAFE_DISTANCE в произвольном порядке. */
            assert(oend - op >= FASTLOOP_SAFE_DISTANCE);
            assert(ip < iend);
            token = *ip++;
            length = token >> ML_BITS;  /* буквальная длина */
            DEBUGLOG(7, "blockPos%6u: litLength token = %u", (unsigned)(op-(BYTE*)dst), (unsigned)length);

            /* декодировать буквальную длину */
            if (length == RUN_MASK) {
                size_t const addl = read_variable_length(&ip, iend-RUN_MASK, 1);
                if (addl == rvl_error) {
                    DEBUGLOG(6, "error reading long literal length");
                    goto _output_error;
                }
                length += addl;
                if (unlikely((uptrval)(op)+length<(uptrval)(op))) { goto _output_error; } /* обнаружение переполнения */
                if (unlikely((uptrval)(ip)+length<(uptrval)(ip))) { goto _output_error; } /* обнаружение переполнения */

                /* копировать литералы */
                LZ4_STATIC_ASSERT(MFLIMIT >= WILDCOPYLENGTH);
                if ((op+length>oend-32) || (ip+length>iend-32)) { goto safe_literal_copy; }
                LZ4_wildCopy32(op, ip, op+length);
                ip += length; op += length;
            } else if (ip <= iend-(16 + 1/*максимальное освещение + смещение + nextToken*/)) {
                /* Нам не нужно проверять oend, поскольку мы проверяем его один раз для каждого цикла ниже. */
                DEBUGLOG(7, "copy %u bytes in a 16-bytes stripe", (unsigned)length);
                /* Литералы могут быть только <= 14, но мы надеемся, что компиляторы оптимизируют лучше при копировании по размеру регистра. */
                LZ4_memcpy(op, ip, 16);
                ip += length; op += length;
            } else {
                goto safe_literal_copy;
            }

            /* получить компенсацию */
            offset = LZ4_readLE16(ip); ip+=2;
            DEBUGLOG(6, "blockPos%6u: offset = %u", (unsigned)(op-(BYTE*)dst), (unsigned)offset);
            match = op - offset;
            assert(match <= op);  /* проверка переполнения */

            /* получить длину совпадения */
            length = token & ML_MASK;
            DEBUGLOG(7, "  match length token = %u (len==%u)", (unsigned)length, (unsigned)length+MINMATCH);

            if (length == ML_MASK) {
                size_t const addl = read_variable_length(&ip, iend - LASTLITERALS + 1, 0);
                if (addl == rvl_error) {
                    DEBUGLOG(5, "error reading long match length");
                    goto _output_error;
                }
                length += addl;
                length += MINMATCH;
                DEBUGLOG(7, "  long match length == %u", (unsigned)length);
                if (unlikely((uptrval)(op)+length<(uptrval)op)) { goto _output_error; } /* обнаружение переполнения */
                if (op + length >= oend - FASTLOOP_SAFE_DISTANCE) {
                    goto safe_match_copy;
                }
            } else {
                length += MINMATCH;
                if (op + length >= oend - FASTLOOP_SAFE_DISTANCE) {
                    DEBUGLOG(7, "moving to safe_match_copy (ml==%u)", (unsigned)length);
                    goto safe_match_copy;
                }

                /* Проверка Fastpath: пропустить LZ4_wildCopy32, если это правда */
                if ((dict == withPrefix64k) || (match >= lowPrefix)) {
                    if (offset >= 8) {
                        assert(match >= lowPrefix);
                        assert(match <= op);
                        assert(op + 18 <= oend);

                        LZ4_memcpy(op, match, 8);
                        LZ4_memcpy(op+8, match+8, 8);
                        LZ4_memcpy(op+16, match+16, 2);
                        op += length;
                        continue;
            }   }   }

            if ( checkOffset && (unlikely(match + dictSize < lowPrefix)) ) {
                DEBUGLOG(5, "Error : pos=%zi, offset=%zi => outside buffers", op-lowPrefix, op-match);
                goto _output_error;
            }
            /* совпадение, начиная с внешнего словаря */
            if ((dict==usingExtDict) && (match < lowPrefix)) {
                assert(dictEnd != NULL);
                if (unlikely(op+length > oend-LASTLITERALS)) {
                    if (partialDecoding) {
                        DEBUGLOG(7, "partialDecoding: dictionary match, close to dstEnd");
                        length = MIN(length, (size_t)(oend-op));
                    } else {
                        DEBUGLOG(6, "end-of-block condition violated")
                        goto _output_error;
                }   }

                if (length <= (size_t)(lowPrefix-match)) {
                    /* совпадение полностью помещается во внешний словарь: просто скопируйте */
                    LZ4_memmove(op, dictEnd - (lowPrefix-match), length);
                    op += length;
                } else {
                    /* совпадение распространяется как на внешний словарь, так и на текущий блок */
                    size_t const copySize = (size_t)(lowPrefix - match);
                    size_t const restSize = length - copySize;
                    LZ4_memcpy(op, dictEnd - copySize, copySize);
                    op += copySize;
                    if (restSize > (size_t)(op - lowPrefix)) {  /* перекрывающаяся копия */
                        BYTE* const endOfMatch = op + restSize;
                        const BYTE* copyFrom = lowPrefix;
                        while (op < endOfMatch) { *op++ = *copyFrom++; }
                    } else {
                        LZ4_memcpy(op, lowPrefix, restSize);
                        op += restSize;
                }   }
                continue;
            }

            /* копировать совпадение внутри блока */
            cpy = op + length;

            assert((op <= oend) && (oend-op >= 32));
            if (unlikely(offset<16)) {
                LZ4_memcpy_using_offset(op, match, cpy, offset);
            } else {
                LZ4_wildCopy32(op, match, cpy);
            }

            op = cpy;   /* исправление нестандартной копии */
        }
    safe_decode:
#endif

        /* Основной цикл: декодирование оставшихся последовательностей, где выходные данные < FASTLOOP_SAFE_DISTANCE */
        DEBUGLOG(6, "using safe decode loop");
        while (1) {
            assert(ip < iend);
            token = *ip++;
            length = token >> ML_BITS;  /* буквальная длина */
            DEBUGLOG(7, "blockPos%6u: litLength token = %u", (unsigned)(op-(BYTE*)dst), (unsigned)length);

            /* Двухэтапный ярлык для наиболее распространенного случая:
             * 1) If the literal length is 0..14, and there is enough space,
             * введите ярлык и скопируйте 16 байт от имени литералов
             * (в быстром режиме таким образом можно безопасно скопировать только 8 байт).
             * 2) Further if the match length is 4..18, copy 18 bytes in a similar
             * манера; но мы гарантируем, что в выводе достаточно места для
             * те 18 байтов ранее, при вводе ярлыка (другими словами,
             * для обоих этапов предусмотрена комбинированная проверка).
             */
            if ( (length != RUN_MASK)
                /* строго «меньше чем» на входе, чтобы повторно войти в цикл хотя бы с одним байтом */
              && likely((ip < shortiend) & (op <= shortoend)) ) {
                /* Скопируйте литералы */
                LZ4_memcpy(op, ip, 16);
                op += length; ip += length;

                /* Второй этап: подготовка к копированию совпадений, декодирование полной информации.
                 * Если не получится, информация не пропадет. */
                length = token & ML_MASK; /* длина совпадения */
                DEBUGLOG(7, "blockPos%6u: matchLength token = %u (len=%u)", (unsigned)(op-(BYTE*)dst), (unsigned)length, (unsigned)length + 4);
                offset = LZ4_readLE16(ip); ip += 2;
                match = op - offset;
                assert(match <= op); /* проверить переполнение */

                /* Не имейте дело с перекрывающимися совпадениями. */
                if ( (length != ML_MASK)
                  && (offset >= 8)
                  && (dict==withPrefix64k || match >= lowPrefix) ) {
                    /* Скопируйте совпадение. */
                    LZ4_memcpy(op + 0, match + 0, 8);
                    LZ4_memcpy(op + 8, match + 8, 8);
                    LZ4_memcpy(op +16, match +16, 2);
                    op += length + MINMATCH;
                    /* Оба этапа сработали, загружаем следующий токен. */
                    continue;
                }

                /* Второй этап не получился, но инфа готова.
                 * Продвиньте его прямо к моменту копирования совпадений. */
                goto _copy_match;
            }

            /* декодировать буквальную длину */
            if (length == RUN_MASK) {
                size_t const addl = read_variable_length(&ip, iend-RUN_MASK, 1);
                if (addl == rvl_error) { goto _output_error; }
                length += addl;
                if (unlikely((uptrval)(op)+length<(uptrval)(op))) { goto _output_error; } /* обнаружение переполнения */
                if (unlikely((uptrval)(ip)+length<(uptrval)(ip))) { goto _output_error; } /* обнаружение переполнения */
            }

#if LZ4_FAST_DEC_LOOP
        safe_literal_copy:
#endif
            /* копировать литералы */
            cpy = op+length;

            LZ4_STATIC_ASSERT(MFLIMIT >= WILDCOPYLENGTH);
            if ((cpy>oend-MFLIMIT) || (ip+length>iend-(2+1+LASTLITERALS))) {
                /* Мы либо достигли ограничения синтаксического анализа входных данных, либо ограничения синтаксического анализа выходных данных.
                 * В обычном сценарии декодирования полного блока это должна быть последняя последовательность,
                 * в противном случае это ошибка (неправильный ввод или размеры).
                 * В сценарии частичного декодирования необходимо убедиться в отсутствии переполнения буфера.
                 */
                if (partialDecoding) {
                    /* Поскольку мы частично декодируем, мы можем оказаться в этом блоке из-за синтаксического анализа вывода.
                     * ограничение, которое недействительно, поскольку размер выходного буфера может быть меньшим.
                     */
                    DEBUGLOG(7, "partialDecoding: copying literals, close to input or output end")
                    DEBUGLOG(7, "partialDecoding: literal length = %u", (unsigned)length);
                    DEBUGLOG(7, "partialDecoding: remaining space in dstBuffer : %i", (int)(oend - op));
                    DEBUGLOG(7, "partialDecoding: remaining space in srcBuffer : %i", (int)(iend - ip));
                    /* Завершение в середине сегмента литералов,
                     * из-за отсутствия ввода.
                     */
                    if (ip+length > iend) {
                        length = (size_t)(iend-ip);
                        cpy = op + length;
                    }
                    /* Завершение в середине сегмента литералов,
                     * из-за нехватки места для вывода.
                     */
                    if (cpy > oend) {
                        cpy = oend;
                        assert(op<=oend);
                        length = (size_t)(oend-op);
                    }
                } else {
                     /* Мы должны быть в последней последовательности (или недействительной) из-за ограничений синтаксического анализа.
                      * поэтому убедитесь, что мы точно потребляем входные данные и не переполняем выходной буфер.
                      */
                    if ((ip+length != iend) || (cpy > oend)) {
                        DEBUGLOG(5, "should have been last run of literals")
                        DEBUGLOG(5, "ip(%p) + length(%i) = %p != iend (%p)", ip, (int)length, ip+length, iend);
                        DEBUGLOG(5, "or cpy(%p) > (oend-MFLIMIT)(%p)", cpy, oend-MFLIMIT);
                        DEBUGLOG(5, "after writing %u bytes / %i bytes available", (unsigned)(op-(BYTE*)dst), outputSize);
                        goto _output_error;
                    }
                }
                LZ4_memmove(op, ip, length);  /* поддерживает перекрывающиеся области памяти для сценариев распаковки на месте */
                ip += length;
                op += length;
                /* Обязательно EOF при !partialDecoding.
                 * При частичном декодировании это EOF, если у нас есть либо
                 * заполнил выходной буфер или
                 * не могу продолжить чтение смещения для следующего совпадения.
                 */
                if (!partialDecoding || (cpy == oend) || (ip >= (iend-2))) {
                    break;
                }
            } else {
                LZ4_wildCopy8(op, ip, cpy);   /* может перезаписать до 8 байтов за пределы cpy */
                ip += length; op = cpy;
            }

            /* получить компенсацию */
            offset = LZ4_readLE16(ip); ip+=2;
            match = op - offset;

            /* получить длину совпадения */
            length = token & ML_MASK;
            DEBUGLOG(7, "blockPos%6u: matchLength token = %u", (unsigned)(op-(BYTE*)dst), (unsigned)length);

    _copy_match:
            if (length == ML_MASK) {
                size_t const addl = read_variable_length(&ip, iend - LASTLITERALS + 1, 0);
                if (addl == rvl_error) { goto _output_error; }
                length += addl;
                if (unlikely((uptrval)(op)+length<(uptrval)op)) goto _output_error;   /* обнаружение переполнения */
            }
            length += MINMATCH;

#if LZ4_FAST_DEC_LOOP
        safe_match_copy:
#endif
            if ((checkOffset) && (unlikely(match + dictSize < lowPrefix))) goto _output_error;   /* Error : offset outside buffers */
            /* совпадение, начиная с внешнего словаря */
            if ((dict==usingExtDict) && (match < lowPrefix)) {
                assert(dictEnd != NULL);
                if (unlikely(op+length > oend-LASTLITERALS)) {
                    if (partialDecoding) length = MIN(length, (size_t)(oend-op));
                    else goto _output_error;   /* не соблюдает ограничение синтаксического анализа */
                }

                if (length <= (size_t)(lowPrefix-match)) {
                    /* совпадение полностью помещается во внешний словарь: просто скопируйте */
                    LZ4_memmove(op, dictEnd - (lowPrefix-match), length);
                    op += length;
                } else {
                    /* совпадение распространяется как на внешний словарь, так и на текущий блок */
                    size_t const copySize = (size_t)(lowPrefix - match);
                    size_t const restSize = length - copySize;
                    LZ4_memcpy(op, dictEnd - copySize, copySize);
                    op += copySize;
                    if (restSize > (size_t)(op - lowPrefix)) {  /* перекрывающаяся копия */
                        BYTE* const endOfMatch = op + restSize;
                        const BYTE* copyFrom = lowPrefix;
                        while (op < endOfMatch) *op++ = *copyFrom++;
                    } else {
                        LZ4_memcpy(op, lowPrefix, restSize);
                        op += restSize;
                }   }
                continue;
            }
            assert(match >= lowPrefix);

            /* копировать совпадение внутри блока */
            cpy = op + length;

            /* partialDecoding : may end anywhere within the block */
            assert(op<=oend);
            if (partialDecoding && (cpy > oend-MATCH_SAFEGUARD_DISTANCE)) {
                size_t const mlen = MIN(length, (size_t)(oend-op));
                const BYTE* const matchEnd = match + mlen;
                BYTE* const copyEnd = op + mlen;
                if (matchEnd > op) {   /* перекрывающаяся копия */
                    while (op < copyEnd) { *op++ = *match++; }
                } else {
                    LZ4_memcpy(op, match, mlen);
                }
                op = copyEnd;
                if (op == oend) { break; }
                continue;
            }

            if (unlikely(offset<8)) {
                LZ4_write32(op, 0);   /* отключить предупреждение msan при смещении == 0 */
                op[0] = match[0];
                op[1] = match[1];
                op[2] = match[2];
                op[3] = match[3];
                match += inc32table[offset];
                LZ4_memcpy(op+4, match, 4);
                match -= dec64table[offset];
            } else {
                LZ4_memcpy(op, match, 8);
                match += 8;
            }
            op += 8;

            if (unlikely(cpy > oend-MATCH_SAFEGUARD_DISTANCE)) {
                BYTE* const oCopyLimit = oend - (WILDCOPYLENGTH-1);
                if (cpy > oend-LASTLITERALS) { goto _output_error; } /* Error : last LASTLITERALS bytes must be literals (uncompressed) */
                if (op < oCopyLimit) {
                    LZ4_wildCopy8(op, match, oCopyLimit);
                    match += oCopyLimit - op;
                    op = oCopyLimit;
                }
                while (op < cpy) { *op++ = *match++; }
            } else {
                LZ4_memcpy(op, match, 8);
                if (length > 16) { LZ4_wildCopy8(op+8, match+8, cpy); }
            }
            op = cpy;   /* исправление нестандартной копии */
        }

        /* конец декодирования */
        DEBUGLOG(5, "decoded %i bytes", (int) (((char*)op)-dst));
        return (int) (((char*)op)-dst);     /* Кол-во декодированных выходных байтов */

        /* Обнаружена ошибка переполнения */
    _output_error:
        return (int) (-(((const char*)ip)-src))-1;
    }
}


/*===== Создайте экземпляр функций декодирования API. =====*/

LZ4_FORCE_O2
int LZ4_decompress_safe(const char* source, char* dest, int compressedSize, int maxDecompressedSize)
{
    return LZ4_decompress_generic(source, dest, compressedSize, maxDecompressedSize,
                                  decode_full_block, noDict,
                                  (BYTE*)dest, NULL, 0);
}

LZ4_FORCE_O2
int LZ4_decompress_safe_partial(const char* src, char* dst, int compressedSize, int targetOutputSize, int dstCapacity)
{
    dstCapacity = MIN(targetOutputSize, dstCapacity);
    return LZ4_decompress_generic(src, dst, compressedSize, dstCapacity,
                                  partial_decode,
                                  noDict, (BYTE*)dst, NULL, 0);
}

LZ4_FORCE_O2
int LZ4_decompress_fast(const char* source, char* dest, int originalSize)
{
    DEBUGLOG(5, "LZ4_decompress_fast");
    return LZ4_decompress_unsafe_generic(
                (const BYTE*)source, (BYTE*)dest, originalSize,
                0, NULL, 0);
}

/*===== Создайте экземпляры еще нескольких случаев декодирования, используемых более одного раза. =====*/

LZ4_FORCE_O2 /* Экспортирована устаревшая функция API. */
int LZ4_decompress_safe_withPrefix64k(const char* source, char* dest, int compressedSize, int maxOutputSize)
{
    return LZ4_decompress_generic(source, dest, compressedSize, maxOutputSize,
                                  decode_full_block, withPrefix64k,
                                  (BYTE*)dest - 64 KB, NULL, 0);
}

LZ4_FORCE_O2
static int LZ4_decompress_safe_partial_withPrefix64k(const char* source, char* dest, int compressedSize, int targetOutputSize, int dstCapacity)
{
    dstCapacity = MIN(targetOutputSize, dstCapacity);
    return LZ4_decompress_generic(source, dest, compressedSize, dstCapacity,
                                  partial_decode, withPrefix64k,
                                  (BYTE*)dest - 64 KB, NULL, 0);
}

/* Еще одна устаревшая функция API, работающая в паре с предыдущей. */
int LZ4_decompress_fast_withPrefix64k(const char* source, char* dest, int originalSize)
{
    return LZ4_decompress_unsafe_generic(
                (const BYTE*)source, (BYTE*)dest, originalSize,
                64 KB, NULL, 0);
}

LZ4_FORCE_O2
static int LZ4_decompress_safe_withSmallPrefix(const char* source, char* dest, int compressedSize, int maxOutputSize,
                                               size_t prefixSize)
{
    return LZ4_decompress_generic(source, dest, compressedSize, maxOutputSize,
                                  decode_full_block, noDict,
                                  (BYTE*)dest-prefixSize, NULL, 0);
}

LZ4_FORCE_O2
static int LZ4_decompress_safe_partial_withSmallPrefix(const char* source, char* dest, int compressedSize, int targetOutputSize, int dstCapacity,
                                               size_t prefixSize)
{
    dstCapacity = MIN(targetOutputSize, dstCapacity);
    return LZ4_decompress_generic(source, dest, compressedSize, dstCapacity,
                                  partial_decode, noDict,
                                  (BYTE*)dest-prefixSize, NULL, 0);
}

LZ4_FORCE_O2
int LZ4_decompress_safe_forceExtDict(const char* source, char* dest,
                                     int compressedSize, int maxOutputSize,
                                     const void* dictStart, size_t dictSize)
{
    DEBUGLOG(5, "LZ4_decompress_safe_forceExtDict");
    return LZ4_decompress_generic(source, dest, compressedSize, maxOutputSize,
                                  decode_full_block, usingExtDict,
                                  (BYTE*)dest, (const BYTE*)dictStart, dictSize);
}

LZ4_FORCE_O2
int LZ4_decompress_safe_partial_forceExtDict(const char* source, char* dest,
                                     int compressedSize, int targetOutputSize, int dstCapacity,
                                     const void* dictStart, size_t dictSize)
{
    dstCapacity = MIN(targetOutputSize, dstCapacity);
    return LZ4_decompress_generic(source, dest, compressedSize, dstCapacity,
                                  partial_decode, usingExtDict,
                                  (BYTE*)dest, (const BYTE*)dictStart, dictSize);
}

LZ4_FORCE_O2
static int LZ4_decompress_fast_extDict(const char* source, char* dest, int originalSize,
                                       const void* dictStart, size_t dictSize)
{
    return LZ4_decompress_unsafe_generic(
                (const BYTE*)source, (BYTE*)dest, originalSize,
                0, (const BYTE*)dictStart, dictSize);
}

/* Режим «двойного словаря», например, для использования с кольцевые буферы: первая часть
 * словаря передается как префикс, а второй через dictStart + dictSize.
 * Эти процедуры используются только один раз, в LZ4_decompress_ * _continue().
 */
LZ4_FORCE_INLINE
int LZ4_decompress_safe_doubleDict(const char* source, char* dest, int compressedSize, int maxOutputSize,
                                   size_t prefixSize, const void* dictStart, size_t dictSize)
{
    return LZ4_decompress_generic(source, dest, compressedSize, maxOutputSize,
                                  decode_full_block, usingExtDict,
                                  (BYTE*)dest-prefixSize, (const BYTE*)dictStart, dictSize);
}

/*===== функции потоковой декомпрессии =====*/

#if !defined(LZ4_STATIC_LINKING_ONLY_DISABLE_MEMORY_ALLOCATION)
LZ4_streamDecode_t* LZ4_createStreamDecode(void)
{
    LZ4_STATIC_ASSERT(sizeof(LZ4_streamDecode_t) >= sizeof(LZ4_streamDecode_t_internal));
    return (LZ4_streamDecode_t*) ALLOC_AND_ZERO(sizeof(LZ4_streamDecode_t));
}

int LZ4_freeStreamDecode (LZ4_streamDecode_t* LZ4_stream)
{
    if (LZ4_stream == NULL) { return 0; }  /* бесплатная поддержка на NULL */
    FREEMEM(LZ4_stream);
    return 0;
}
#endif

/*! LZ4_setStreamDecode () :
 *  Используйте эту функцию, чтобы указать, где найти словарь.
 *  Эта функция не требуется, если предыдущие данные все еще доступны там, где они были декодированы.
 *  Разрешена загрузка размера 0 (тот же эффект, что и при отсутствии словаря).
 * @return : 1 if OK, 0 if error
 */
int LZ4_setStreamDecode (LZ4_streamDecode_t* LZ4_streamDecode, const char* dictionary, int dictSize)
{
    LZ4_streamDecode_t_internal* lz4sd = &LZ4_streamDecode->internal_donotuse;
    lz4sd->prefixSize = (size_t)dictSize;
    if (dictSize) {
        assert(dictionary != NULL);
        lz4sd->prefixEnd = (const BYTE*) dictionary + dictSize;
    } else {
        lz4sd->prefixEnd = (const BYTE*) dictionary;
    }
    lz4sd->externalDict = NULL;
    lz4sd->extDictSize  = 0;
    return 1;
}

/*! LZ4_decoderRingBufferSize () :
 *  при настройке кольцевого буфера для потоковой декомпрессии (необязательный сценарий),
 *  обеспечивает минимальный размер этого кольцевого буфера
 *  быть совместимым с любым источником, соблюдающим условие maxBlockSize.
 *  Note : in a ring buffer scenario,
 *  блоки считаются распакованными рядом друг с другом.
 *  Когда недостаточно места для следующего блока (remainingSize < maxBlockSize),
 *  декодирование возобновляется с начала кольцевого буфера.
 * @return : minimum ring buffer size,
 *           или 0, если произошла ошибка (неверный maxBlockSize).
 */
int LZ4_decoderRingBufferSize(int maxBlockSize)
{
    if (maxBlockSize < 0) return 0;
    if (maxBlockSize > LZ4_MAX_INPUT_SIZE) return 0;
    if (maxBlockSize < 16) maxBlockSize = 16;
    return LZ4_DECODER_RING_BUFFER_SIZE(maxBlockSize);
}

/*
*_continue() :
    Эти функции декодирования позволяют распаковывать несколько блоков в «потоковом» режиме.
    Ранее декодированные блоки должны оставаться доступными в той позиции памяти, где они были декодированы.
    Если это невозможно, сохраните соответствующую часть декодированных данных в безопасный буфер.
    и указать, где оно стоит, с помощью LZ4_setStreamDecode()
*/
LZ4_FORCE_O2
int LZ4_decompress_safe_continue (LZ4_streamDecode_t* LZ4_streamDecode, const char* source, char* dest, int compressedSize, int maxOutputSize)
{
    LZ4_streamDecode_t_internal* lz4sd = &LZ4_streamDecode->internal_donotuse;
    int result;

    if (lz4sd->prefixSize == 0) {
        /* Первый звонок, словаря еще нет. */
        assert(lz4sd->extDictSize == 0);
        result = LZ4_decompress_safe(source, dest, compressedSize, maxOutputSize);
        if (result <= 0) return result;
        lz4sd->prefixSize = (size_t)result;
        lz4sd->prefixEnd = (BYTE*)dest + result;
    } else if (lz4sd->prefixEnd == (BYTE*)dest) {
        /* Они катают текущий сегмент. */
        if (lz4sd->prefixSize >= 64 KB - 1)
            result = LZ4_decompress_safe_withPrefix64k(source, dest, compressedSize, maxOutputSize);
        else if (lz4sd->extDictSize == 0)
            result = LZ4_decompress_safe_withSmallPrefix(source, dest, compressedSize, maxOutputSize,
                                                         lz4sd->prefixSize);
        else
            result = LZ4_decompress_safe_doubleDict(source, dest, compressedSize, maxOutputSize,
                                                    lz4sd->prefixSize, lz4sd->externalDict, lz4sd->extDictSize);
        if (result <= 0) return result;
        lz4sd->prefixSize += (size_t)result;
        lz4sd->prefixEnd  += result;
    } else {
        /* Буфер замыкается, или они переключаются на другой буфер. */
        lz4sd->extDictSize = lz4sd->prefixSize;
        lz4sd->externalDict = lz4sd->prefixEnd - lz4sd->extDictSize;
        result = LZ4_decompress_safe_forceExtDict(source, dest, compressedSize, maxOutputSize,
                                                  lz4sd->externalDict, lz4sd->extDictSize);
        if (result <= 0) return result;
        lz4sd->prefixSize = (size_t)result;
        lz4sd->prefixEnd  = (BYTE*)dest + result;
    }

    return result;
}

LZ4_FORCE_O2 int
LZ4_decompress_fast_continue (LZ4_streamDecode_t* LZ4_streamDecode,
                        const char* source, char* dest, int originalSize)
{
    LZ4_streamDecode_t_internal* const lz4sd =
        (assert(LZ4_streamDecode!=NULL), &LZ4_streamDecode->internal_donotuse);
    int result;

    DEBUGLOG(5, "LZ4_decompress_fast_continue (toDecodeSize=%i)", originalSize);
    assert(originalSize >= 0);

    if (lz4sd->prefixSize == 0) {
        DEBUGLOG(5, "first invocation : no prefix nor extDict");
        assert(lz4sd->extDictSize == 0);
        result = LZ4_decompress_fast(source, dest, originalSize);
        if (result <= 0) return result;
        lz4sd->prefixSize = (size_t)originalSize;
        lz4sd->prefixEnd = (BYTE*)dest + originalSize;
    } else if (lz4sd->prefixEnd == (BYTE*)dest) {
        DEBUGLOG(5, "continue using existing prefix");
        result = LZ4_decompress_unsafe_generic(
                        (const BYTE*)source, (BYTE*)dest, originalSize,
                        lz4sd->prefixSize,
                        lz4sd->externalDict, lz4sd->extDictSize);
        if (result <= 0) return result;
        lz4sd->prefixSize += (size_t)originalSize;
        lz4sd->prefixEnd  += originalSize;
    } else {
        DEBUGLOG(5, "prefix becomes extDict");
        lz4sd->extDictSize = lz4sd->prefixSize;
        lz4sd->externalDict = lz4sd->prefixEnd - lz4sd->extDictSize;
        result = LZ4_decompress_fast_extDict(source, dest, originalSize,
                                             lz4sd->externalDict, lz4sd->extDictSize);
        if (result <= 0) return result;
        lz4sd->prefixSize = (size_t)originalSize;
        lz4sd->prefixEnd  = (BYTE*)dest + originalSize;
    }

    return result;
}


/*
Расширенные функции декодирования:
*_usingDict() :
    Эти функции декодирования работают так же, как и «_continue»,
    словарь должен быть явно указан в параметрах
*/

int LZ4_decompress_safe_usingDict(const char* source, char* dest, int compressedSize, int maxOutputSize, const char* dictStart, int dictSize)
{
    if (dictSize==0)
        return LZ4_decompress_safe(source, dest, compressedSize, maxOutputSize);
    if (dictStart+dictSize == dest) {
        if (dictSize >= 64 KB - 1) {
            return LZ4_decompress_safe_withPrefix64k(source, dest, compressedSize, maxOutputSize);
        }
        assert(dictSize >= 0);
        return LZ4_decompress_safe_withSmallPrefix(source, dest, compressedSize, maxOutputSize, (size_t)dictSize);
    }
    assert(dictSize >= 0);
    return LZ4_decompress_safe_forceExtDict(source, dest, compressedSize, maxOutputSize, dictStart, (size_t)dictSize);
}

int LZ4_decompress_safe_partial_usingDict(const char* source, char* dest, int compressedSize, int targetOutputSize, int dstCapacity, const char* dictStart, int dictSize)
{
    if (dictSize==0)
        return LZ4_decompress_safe_partial(source, dest, compressedSize, targetOutputSize, dstCapacity);
    if (dictStart+dictSize == dest) {
        if (dictSize >= 64 KB - 1) {
            return LZ4_decompress_safe_partial_withPrefix64k(source, dest, compressedSize, targetOutputSize, dstCapacity);
        }
        assert(dictSize >= 0);
        return LZ4_decompress_safe_partial_withSmallPrefix(source, dest, compressedSize, targetOutputSize, dstCapacity, (size_t)dictSize);
    }
    assert(dictSize >= 0);
    return LZ4_decompress_safe_partial_forceExtDict(source, dest, compressedSize, targetOutputSize, dstCapacity, dictStart, (size_t)dictSize);
}

int LZ4_decompress_fast_usingDict(const char* source, char* dest, int originalSize, const char* dictStart, int dictSize)
{
    if (dictSize==0 || dictStart+dictSize == dest)
        return LZ4_decompress_unsafe_generic(
                        (const BYTE*)source, (BYTE*)dest, originalSize,
                        (size_t)dictSize, NULL, 0);
    assert(dictSize >= 0);
    return LZ4_decompress_fast_extDict(source, dest, originalSize, dictStart, (size_t)dictSize);
}


/*=*************************************************
*  Устаревшие функции
***************************************************/
/* устаревшие функции сжатия */
int LZ4_compress_limitedOutput(const char* source, char* dest, int inputSize, int maxOutputSize)
{
    return LZ4_compress_default(source, dest, inputSize, maxOutputSize);
}
int LZ4_compress(const char* src, char* dest, int srcSize)
{
    return LZ4_compress_default(src, dest, srcSize, LZ4_compressBound(srcSize));
}
int LZ4_compress_limitedOutput_withState (void* state, const char* src, char* dst, int srcSize, int dstSize)
{
    return LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, 1);
}
int LZ4_compress_withState (void* state, const char* src, char* dst, int srcSize)
{
    return LZ4_compress_fast_extState(state, src, dst, srcSize, LZ4_compressBound(srcSize), 1);
}
int LZ4_compress_limitedOutput_continue (LZ4_stream_t* LZ4_stream, const char* src, char* dst, int srcSize, int dstCapacity)
{
    return LZ4_compress_fast_continue(LZ4_stream, src, dst, srcSize, dstCapacity, 1);
}
int LZ4_compress_continue (LZ4_stream_t* LZ4_stream, const char* source, char* dest, int inputSize)
{
    return LZ4_compress_fast_continue(LZ4_stream, source, dest, inputSize, LZ4_compressBound(inputSize), 1);
}

/*
Эти функции распаковки устарели и больше не должны использоваться.
Они представлены здесь только для совместимости со старыми пользовательскими программами.
- LZ4_uncompress is totally equivalent to LZ4_decompress_fast
- LZ4_uncompress_unknownOutputSize is totally equivalent to LZ4_decompress_safe
*/
int LZ4_uncompress (const char* source, char* dest, int outputSize)
{
    return LZ4_decompress_fast(source, dest, outputSize);
}
int LZ4_uncompress_unknownOutputSize (const char* source, char* dest, int isize, int maxOutputSize)
{
    return LZ4_decompress_safe(source, dest, isize, maxOutputSize);
}

/* Устаревшие функции потоковой передачи */

int LZ4_sizeofStreamState(void) { return sizeof(LZ4_stream_t); }

int LZ4_resetStreamState(void* state, char* inputBuffer)
{
    (void)inputBuffer;
    LZ4_resetStream((LZ4_stream_t*)state);
    return 0;
}

#if !defined(LZ4_STATIC_LINKING_ONLY_DISABLE_MEMORY_ALLOCATION)
void* LZ4_create (char* inputBuffer)
{
    (void)inputBuffer;
    return LZ4_createStream();
}
#endif

char* LZ4_slideInputBuffer (void* state)
{
    /* избегайте предупреждения о преобразовании const char * -> char * */
    return (char *)(uptrval)((LZ4_stream_t*)state)->internal_donotuse.dictionary;
}

#endif   /* LZ4_COMMONDEFS_ONLY */

#endif /* LV_USE_LZ4_INTERNAL */

