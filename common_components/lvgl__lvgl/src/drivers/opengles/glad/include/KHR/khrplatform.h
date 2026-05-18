#ifndef __khrplatform_h_
#define __khrplatform_h_

#include "../../../../../lv_conf_internal.h"
#if LV_USE_OPENGLES
/*
** Copyright (c) 2008-2018 The Khronos Group Inc.
**
** Разрешение настоящим предоставляется бесплатно любому лицу, получившему
** копию этого программного обеспечения и/или связанных с ним файлов документации (файл
** «Материалы»), распоряжаться Материалами без ограничений, в том числе
** без ограничений права на использование, копирование, изменение, объединение, публикацию,
** распространять, сублицензировать и/или продавать копии Материалов, а также
** разрешить лицам, которым предоставляются Материалы, сделать это при условии, что
** следующие условия:
**
** Вышеупомянутое уведомление об авторских правах и настоящее уведомление о разрешении должны быть включены.
** во всех копиях или существенных частях Материалов.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/

/* Типы и определения Khronos, специфичные для платформы.
 *
 * Мастер-копияkhrplatform.hхранится в Khronos EGL.
 * Репозиторий реестра по адресу https://github.com/KhronosGroup/EGL-Registry.
 * Последняя семантическая модификацияkhrplatform.hбыла сделана при исправлении ID:
 *      67a3e0864c2d75ea5287b9f3d2eb74a745936692
 *
 * Усыновители могут изменить этот файл в соответствии со своей платформой. Усыновители
 * Рекомендуется отправить в Хронос изменения, специфичные для платформы.
 * группу, чтобы их можно было включить в будущие версии этого файла.
 * Пожалуйста, отправьте изменения, отправив запросы на включение или проблемы на
 * репозиторий реестра EGL, указанный выше.
 *
 *
 * Информацию о том, где находится этот файл, см. в Руководстве по внедрению.
 * должен находиться в вашей системе, а для получения более подробной информации о его использовании:
 *    http://www.khronos.org/registry/implementers_guide.pdf
 *
 * Этот файл должен быть включен как
 *        #include <KHR/khrplatform.h>
 * заголовочные файлыAPIклиента Khronos, которые используют его типы и определения.
 *
 * Типы вkhrplatform.hследует использовать только для определения типов, специфичных для API.
 *
 * Типы настроек в khrplatform.h:
 *    khronos_int8_t подписанный 8 бит
 *    khronos_uint8_t 8 без бита
 *    khronos_int16_t подписанный 16 бит
 *    khronos_uint16_t беззнаковый 16 бит
 *    khronos_int32_t подписанный 32 бит
 *    khronos_uint32_t беззнаковый 32-битный
 *    khronos_int64_t подписанный 64 бит
 *    khronos_uint64_t беззнаковый 64-битный
 *    khronos_intptr_t соответствует тому же количеству битов, что и указатель.
 *    khronos_uintptr_t беззнаковое то же количество бит, что и указатель
 *    khronos_ssize_t подписанный размер
 *    khronos_usize_t беззнаковый размер
 *    khronos_float_t 32-битная цифра с плавающей запятой со знакомым
 *    khronos_time_ns_t беззнаковое 64-битное время в наносекундах
 *    khronos_utime_nanoseconds_t беззнаковый интервал времени или абсолютное время в
 *                                         наносекунды
 *    khronos_stime_nanoseconds_t обозначает интервал времени в наносекундах
 *    khronos_boolean_enum_t перечисляемый логический тип. Это должно быть
 *      использоваться в качестве базового типа только в том случае, если логический тип клиента API равен
 *      перечисление. Клиентские API, которые используют целое число или другой тип для
 *      логические значения не могут использовать это значение в качестве базового типа для своих логических значений.
 *
 * Токены, настройки вkhrplatform.h:
 *
 *    KHRONOS_FALSE ,KHRONOS_TRUEПеречисляемые логические значения false/true.
 *
 *    KHRONOS_SUPPORT_INT64 равен 1, если поддерживаются 64-битные целые числа; иначе 0.
 *    KHRONOS_SUPPORT_FLOAT равен 1, если поддерживаются числа с плавающей запятой; иначе 0.
 *
 * Макросы соглашения о вызовах, определенные в этом файле:
 *    KHRONOS_APICALL
 *    KHRONOS_APIENTRY
 *    KHRONOS_APIATTRIBUTES
 *
 * Их можно использовать в прототипах функций как:
 *
 *      KHRONOS_APICALL voidKHRONOS_APIENTRYимя_функции(
 *                                  интервал аргумент1,
 *                                  int arg2)KHRONOS_APIATTRIBUTES;
 */

#if defined(__SCITECH_SNAP__) && !defined(KHRONOS_STATIC)
#   define KHRONOS_STATIC 1
#endif

/*-------------------------------------------------------------------------
 * Определение KHRONOS_APICALL
 *-------------------------------------------------------------------------
 * Он предшествует типу возвращаемого значения функции в прототипе функции.
 */
#if defined(KHRONOS_STATIC)
    /* Если константа препроцессора KHRONOS_STATIC определена, сделайте
     * заголовок, совместимый со статическим связыванием. */
#   define KHRONOS_APICALL
#elif defined(_WIN32)
#   define KHRONOS_APICALL __declspec(dllimport)
#elif defined (__SYMBIAN32__)
#   define KHRONOS_APICALL IMPORT_C
#elif defined(__ANDROID__)
#   define KHRONOS_APICALL __attribute__((visibility("default")))
#else
#   define KHRONOS_APICALL
#endif

/*-------------------------------------------------------------------------
 * Определение KHRONOS_APIENTRY
 *-------------------------------------------------------------------------
 * Это следует за типом возвращаемого значения функции и предшествует функции.
 * имя в прототипе функции.
 */
#if defined(_WIN32) && !defined(_WIN32_WCE) && !defined(__SCITECH_SNAP__)
    /* Win32, но не WinCE */
#   define KHRONOS_APIENTRY __stdcall
#else
#   define KHRONOS_APIENTRY
#endif

/*-------------------------------------------------------------------------
 * Определение KHRONOS_APIATTRIBUTES
 *-------------------------------------------------------------------------
 * Это следует за закрывающей круглой скобкой аргументов прототипа функции.
 */
#if defined (__ARMCC_2__)
#define KHRONOS_APIATTRIBUTES __softfp
#else
#define KHRONOS_APIATTRIBUTES
#endif

/*-------------------------------------------------------------------------
 * определения основных типов
 *-----------------------------------------------------------------------*/
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || defined(__GNUC__) || defined(__SCO__) || defined(__USLC__)


/*
 * Использование <stdint.h>
 */
#include <stdint.h>
typedef int32_t                 khronos_int32_t;
typedef uint32_t                khronos_uint32_t;
typedef int64_t                 khronos_int64_t;
typedef uint64_t                khronos_uint64_t;
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1
/*
 * Для поддержки платформ, где unsigned long не может быть использован взаимозаменяемо с
 * inptr_t (например,CHERI-расширенныеISA), мы можем использоватьstdint.hintptr_t .
 * В идеале мы могли бы просто везде использовать (u)intptr_t, но это может привести к
 * ПоломкаABI, еслиkhronos_uintptr_tизменен с unsigned long на
 * unsigned long long или аналогичный (это приводит к изменению имен в C++).
 * Чтобы избежать изменений в существующей платформе, мы ограничиваем использование intptr_t.
 * платформа, где размер указателя больше по размеру.
 */
#if defined(__SIZEOF_LONG__) && defined(__SIZEOF_POINTER__)
#if __SIZEOF_POINTER__ > __SIZEOF_LONG__
#define KHRONOS_USE_INTPTR_T
#endif
#endif

#elif defined(__VMS ) || defined(__sgi)

/*
 * Использование <inttypes.h>
 */
#include <inttypes.h>
typedef int32_t                 khronos_int32_t;
typedef uint32_t                khronos_uint32_t;
typedef int64_t                 khronos_int64_t;
typedef uint64_t                khronos_uint64_t;
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1

#elif defined(_WIN32) && !defined(__SCITECH_SNAP__)

/*
 * Win32
 */
typedef __int32                 khronos_int32_t;
typedef unsigned __int32        khronos_uint32_t;
typedef __int64                 khronos_int64_t;
typedef unsigned __int64        khronos_uint64_t;
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1

#elif defined(__sun__) || defined(__digital__)

/*
 * Солнце или цифровое
 */
typedef int                     khronos_int32_t;
typedef unsigned int            khronos_uint32_t;
#if defined(__arch64__) || defined(_LP64)
typedef long int                khronos_int64_t;
typedef unsigned long int       khronos_uint64_t;
#else
typedef long long int           khronos_int64_t;
typedef unsigned long long int  khronos_uint64_t;
#endif /* __arch64__ */
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1

#elif 0

/*
 * Гипотетическая платформа без поддержки float или int64.
 */
typedef int                     khronos_int32_t;
typedef unsigned int            khronos_uint32_t;
#define KHRONOS_SUPPORT_INT64   0
#define KHRONOS_SUPPORT_FLOAT   0

#else

/*
 * Общий запасной вариант
 */
#include <stdint.h>
typedef int32_t                 khronos_int32_t;
typedef uint32_t                khronos_uint32_t;
typedef int64_t                 khronos_int64_t;
typedef uint64_t                khronos_uint64_t;
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1

#endif


/*
 * Типы, которые (пока) одинаковы на всех платформах.
 */
typedef signed   char          khronos_int8_t;
typedef unsigned char          khronos_uint8_t;
typedef signed   short int     khronos_int16_t;
typedef unsigned short int     khronos_uint16_t;

/*
 * Типы, которые различаются между архитектурами LLP64 и LP64 — в LLP64,
 * Указатели имеют длину 64 бита, но длина «длинных» по-прежнему составляет 32 бита. Win64 внешний вид
 * быть единственной архитектурой LLP64, используемой в настоящее время.
 */
#ifdef KHRONOS_USE_INTPTR_T
typedef intptr_t               khronos_intptr_t;
typedef uintptr_t              khronos_uintptr_t;
#elif defined(_WIN64)
typedef signed   long long int khronos_intptr_t;
typedef unsigned long long int khronos_uintptr_t;
#else
typedef signed   long  int     khronos_intptr_t;
typedef unsigned long  int     khronos_uintptr_t;
#endif

#if defined(_WIN64)
typedef signed   long long int khronos_ssize_t;
typedef unsigned long long int khronos_usize_t;
#else
typedef signed   long  int     khronos_ssize_t;
typedef unsigned long  int     khronos_usize_t;
#endif

#if KHRONOS_SUPPORT_FLOAT
/*
 * Поплавковый тип
 */
typedef          float         khronos_float_t;
#endif

#if KHRONOS_SUPPORT_INT64
/* Типы времени
 *
 * Эти типы можно использовать для представления интервала времени в наносекундах или
 * абсолютное нескорректированное системное время.  Нескорректированное системное время — это число
 * наносекунд с момента какого-либо произвольного системного события (например, с момента последнего
 * время загрузки системы).  Нескорректированное системное время — это беззнаковый
 * 64-битное значение, которое возвращается к 0 каждые 584 года.  Временные интервалы
 * может быть как подписанным, так и неподписанным.
 */
typedef khronos_uint64_t       khronos_utime_nanoseconds_t;
typedef khronos_int64_t        khronos_stime_nanoseconds_t;
#endif

/*
 * Фиктивное значение, используемое для расширения типов перечислений до 32 бит.
 */
#ifndef KHRONOS_MAX_ENUM
#define KHRONOS_MAX_ENUM 0x7FFFFFFF
#endif

/*
 * Перечисляемый логический тип
 *
 * Значения, отличные от нуля, следует считать истинными.  Поэтому
 * сравнения не следует проводить с KHRONOS_TRUE.
 */
typedef enum {
    KHRONOS_FALSE = 0,
    KHRONOS_TRUE  = 1,
    KHRONOS_BOOLEAN_ENUM_FORCE_SIZE = KHRONOS_MAX_ENUM
} khronos_boolean_enum_t;

#endif /*LV_USE_OPENGLES*/
#endif /* __khrplatform_h_ */
