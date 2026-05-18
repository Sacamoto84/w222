// Tencent рада поддержать сообщество открытого исходного кода, созданного доступным RapidJSON.
//
// Copyright (C) 2015 THL A29 Limited, a Tencent company, and Milo Yip.
//
// Лицензия MIT («Лицензия»); вы не можете использовать этот файл, за исключением
// в соответствии с Лицензией. Вы можете получить копию Лицензии по адресу
//
// http://opensource.org/licenses/MIT
//
// Если это не требуется действующим законодательством или не согласовано в письменной форме, распространяемое программное обеспечение
// по Лицензии распространяется на " AS IS " BASIS , WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND , явный или подразумеваемый. См. Лицензию на
// конкретный язык, регулирующий разрешения и ограничения по Лицензии.

#ifndef RAPIDJSON_RAPIDJSON_H_
#define RAPIDJSON_RAPIDJSON_H_

/*!\файл rapidjson.h
    \краткие общие определения и конфигурация

    \см. RAPIDJSON_CONFIG
 */

/*! \defgroup КонфигурацияRAPIDJSON_CONFIGRapidJSON
    \brief Макросы настройки функций библиотеки

    Некоторые функции RapidJSON можно настроить для адаптации библиотек к широкому кругу пользователей.
    разнообразие платформ, сред и сценариев использования.  Большая часть
    функции могут быть настроены с точки зрения переопределения или предопределения
    макросы препроцессора во время компиляции.

    Некоторые дополнительные настройки доступны вAPI\refRAPIDJSON_ERRORS.

    \note Эти макросы следует задавать в командной строке компилятора.
          (где это применимо), чтобы избежать противоречивых значений при составлении
          различные единицы перевода одного приложения.
 */

#include <cstdlib>  // malloc() ,realloc(),free(), size_t
#include <cstring>  // memset() ,memcpy(),memmove(), memcmp()

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_VERSION_STRING
//
// ALWAYS синхронизирует три макроса с соответствием переменным в /CMakeLists.txt.
//

//! @condRAPIDJSON_HIDDEN_FROM_DOXYGEN
// строкирование токена
#define RAPIDJSON_STRINGIFY(x) RAPIDJSON_DO_STRINGIFY(x)
#define RAPIDJSON_DO_STRINGIFY(x) #x

// конкатенация токенов
#define RAPIDJSON_JOIN(X, Y) RAPIDJSON_DO_JOIN(X, Y)
#define RAPIDJSON_DO_JOIN(X, Y) RAPIDJSON_DO_JOIN2(X, Y)
#define RAPIDJSON_DO_JOIN2(X, Y) X##Y
//!  @endcond

/*! \def RAPIDJSON_MAJOR_VERSION
    \ingroup RAPIDJSON_CONFIG
    \brief Основная версия RapidJSON в целочисленном формате.
*/
/*! \def RAPIDJSON_MINOR_VERSION
    \ingroup RAPIDJSON_CONFIG
    \brief Младшая версия RapidJSON в целочисленном формате.
*/
/*! \def RAPIDJSON_PATCH_VERSION
    \ingroup RAPIDJSON_CONFIG
    \brief Исправление версии RapidJSON в целочисленном формате.
*/
/*! \def RAPIDJSON_VERSION_STRING
    \ingroup RAPIDJSON_CONFIG
    \brief Версия RapidJSON в строковом формате "<major>.<minor>.<patch>".
*/
#define RAPIDJSON_MAJOR_VERSION 1
#define RAPIDJSON_MINOR_VERSION 1
#define RAPIDJSON_PATCH_VERSION 0
#define RAPIDJSON_VERSION_STRING \
    RAPIDJSON_STRINGIFY(RAPIDJSON_MAJOR_VERSION.RAPIDJSON_MINOR_VERSION.RAPIDJSON_PATCH_VERSION)

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_NAMESPACE_(BEGIN|END)
/*! \def RAPIDJSON_NAMESPACE
    \ingroup RAPIDJSON_CONFIG
    \brief указать собственное пространство имен RapidJSON

    Во избежание конфликтов символов и/или ошибок «одного правила определения».
    между последовательными включениями (разные варианты) RapidJSON в
    один двойной файл, пользователи могут настроить имя основного RapidJSON
    пространство имен.

    В случае одного уровня вложенности определение \c RAPIDJSON_NAMESPACE
    на собственное имя (например, \c MyRapidJSON).  если несколько
    необходимы уровни, оба \refRAPIDJSON_NAMESPACE_BEGINи \ref
    RAPIDJSON_NAMESPACE_END также необходимо определить:

    \code
    // в каком-то файле .cpp
    #RAPIDJSON_NAMESPACE определить my::rapidjson
    #определить имена пространстваRAPIDJSON_NAMESPACE_BEGINmy { имена пространства RapidJSON {
    #определить RAPIDJSON_NAMESPACE_END } }
    #включить "rapidjson/..."
    \endcode

    \см. РапидJSON
 */
/*! \def RAPIDJSON_NAMESPACE_BEGIN
    \ingroup RAPIDJSON_CONFIG
    \brief обеспечивает собственное пространство имен RapidJSON (открывающее выражение)
    \см. RAPIDJSON_NAMESPACE
*/
/*! \def RAPIDJSON_NAMESPACE_END
    \ingroup RAPIDJSON_CONFIG
    \brief обеспечивает собственное пространство имен RapidJSON (закрывающее выражение)
    \см. RAPIDJSON_NAMESPACE
*/
#ifndef RAPIDJSON_NAMESPACE
#define RAPIDJSON_NAMESPACE rapidjson
#endif
#ifndef RAPIDJSON_NAMESPACE_BEGIN
#define RAPIDJSON_NAMESPACE_BEGIN namespace RAPIDJSON_NAMESPACE {
#endif
#ifndef RAPIDJSON_NAMESPACE_END
#define RAPIDJSON_NAMESPACE_END }
#endif

///////////////////////////////////////////////////////////////////////////////
// __cplusplus макрос

//! @condRAPIDJSON_HIDDEN_FROM_DOXYGEN

#if defined(_MSC_VER)
#define RAPIDJSON_CPLUSPLUS _MSVC_LANG
#else
#define RAPIDJSON_CPLUSPLUS __cplusplus
#endif

//!  @endcond

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_HAS_STDSTRING

#ifndef RAPIDJSON_HAS_STDSTRING
#ifdef RAPIDJSON_DOXYGEN_RUNNING
#define RAPIDJSON_HAS_STDSTRING 1 // принудительное оформление документации
#else
#define RAPIDJSON_HAS_STDSTRING 0 // нет поддержки std::string по умолчанию
#endif
/*! \def RAPIDJSON_HAS_STDSTRING
    \ingroup RAPIDJSON_CONFIG
    \brief Включить поддержку RapidJSON для \c std::string

    Определив для этого символа препроцессора значение \c 1, можно получить несколько удобных функций для использования.
    \ref Rapidjson::GenericValue с включением \c std::string, особенно
    для построения и сравнения.

    \hideinitializer
*/
#endif // !определено( RAPIDJSON_HAS_STDSTRING )

#if RAPIDJSON_HAS_STDSTRING
#include <string>
#endif // RAPIDJSON_HAS_STDSTRING

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_USE_MEMBERSMAP

/*! \def RAPIDJSON_USE_MEMBERSMAP
    \ingroup RAPIDJSON_CONFIG
    \brief Включить поддержку RapidJSON для обработки мужских объектов в \c std::multimap

    Определив этот символьный препроцессор в \c 1, объект \ref Rapidjson::GenericValue
    Сохранение элементов в \c std::multimap для более быстрого поиска и удаления.
    компромисс с немного более медленным временем вставки и выделением (или) небольшого объекта
    накладные расходы на память.

    \hideinitializer
*/
#ifndef RAPIDJSON_USE_MEMBERSMAP
#define RAPIDJSON_USE_MEMBERSMAP 0 // не по умолчанию
#endif

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_NO_INT64DEFINE

/*! \def RAPIDJSON_NO_INT64DEFINE
    \ingroup RAPIDJSON_CONFIG
    \brief Используйте внешние 64-битные целочисленные типы.

    RapidJSON требует 64-битных целочисленных типов \cint64_tи \cuint64_t.
    быть доступным в глобальном масштабе.

    Если у пользователей есть собственное определение, определите RAPIDJSON_NO_INT64DEFINE, чтобы
    запретить RapidJSON определять свои типы.
*/
#ifndef RAPIDJSON_NO_INT64DEFINE
//! @condRAPIDJSON_HIDDEN_FROM_DOXYGEN
#if defined(_MSC_VER) && (_MSC_VER < 1800) // Визуальная Студия 2013
#include "msinttypes/stdint.h"
#include "msinttypes/inttypes.h"
#else
// Другие компиляторы должны иметь это.
#include <stdint.h>
#include <inttypes.h>
#endif
//!  @endcond
#ifdef RAPIDJSON_DOXYGEN_RUNNING
#define RAPIDJSON_NO_INT64DEFINE
#endif
#endif // RAPIDJSON_NO_INT64TYPEDEF

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_FORCEINLINE

#ifndef RAPIDJSON_FORCEINLINE
//! @condRAPIDJSON_HIDDEN_FROM_DOXYGEN
#if defined(_MSC_VER) && defined(NDEBUG)
#define RAPIDJSON_FORCEINLINE __forceinline
#elif defined(__GNUC__) && __GNUC__ >= 4 && defined(NDEBUG)
#define RAPIDJSON_FORCEINLINE __attribute__((always_inline))
#else
#define RAPIDJSON_FORCEINLINE
#endif
//!  @endcond
#endif // RAPIDJSON_FORCEINLINE

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_ENDIAN
#define RAPIDJSON_LITTLEENDIAN  0   //!< Машина с прямым порядком байтов
#define RAPIDJSON_BIGENDIAN     1   //!< Машина с прямым порядком байтов

//! Эндианность машины.
/*!
    \def RAPIDJSON_ENDIAN
    \ingroup RAPIDJSON_CONFIG

    В GCC 4.6 предоставлен макрос для определения порядка байтов целевой машины. Но другие
    компиляторы могут этого не иметь. Пользователь может определить RAPIDJSON_ENDIAN либо
    \refRAPIDJSON_LITTLEENDIANили \refRAPIDJSON_BIGENDIAN.

    Обнаружение по умолчанию реализовано со ссылкой на
    \li https://gcc.gnu.org/onlinedocs/gcc-4.6.0/cpp/Common-Predefined-Macros.html
    \li http://www.boost.org/doc/libs/1_42_0/boost/detail/endian.hpp
*/
#ifndef RAPIDJSON_ENDIAN
// Обнаружение с помощью макроса GCC 4.6.
#  ifdef __BYTE_ORDER__
#    if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#      define RAPIDJSON_ENDIAN RAPIDJSON_LITTLEENDIAN
#    elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#      define RAPIDJSON_ENDIAN RAPIDJSON_BIGENDIAN
#    else
#      error Unknown machine endianness detected. User needs to define RAPIDJSON_ENDIAN.
#    endif // __BYTE_ORDER__
// Обнаружение с помощьюendian.hGLIBC
#  elif defined(__GLIBC__)
#    include <endian.h>
#    if (__BYTE_ORDER == __LITTLE_ENDIAN)
#      define RAPIDJSON_ENDIAN RAPIDJSON_LITTLEENDIAN
#    elif (__BYTE_ORDER == __BIG_ENDIAN)
#      define RAPIDJSON_ENDIAN RAPIDJSON_BIGENDIAN
#    else
#      error Unknown machine endianness detected. User needs to define RAPIDJSON_ENDIAN.
#   endif // __GLIBC__
// Обнаружение с помощью макроса _LITTLE_ENDIAN и _BIG_ENDIAN
#  elif defined(_LITTLE_ENDIAN) && !defined(_BIG_ENDIAN)
#    define RAPIDJSON_ENDIAN RAPIDJSON_LITTLEENDIAN
#  elif defined(_BIG_ENDIAN) && !defined(_LITTLE_ENDIAN)
#    define RAPIDJSON_ENDIAN RAPIDJSON_BIGENDIAN
// Обнаружение с помощью архитектурных макросов
#  elif defined(__sparc) || defined(__sparc__) || defined(_POWER) || defined(__powerpc__) || defined(__ppc__) || defined(__hpux) || defined(__hppa) || defined(_MIPSEB) || defined(_POWER) || defined(__s390__)
#    define RAPIDJSON_ENDIAN RAPIDJSON_BIGENDIAN
#  elif defined(__i386__) || defined(__alpha__) || defined(__ia64) || defined(__ia64__) || defined(_M_IX86) || defined(_M_IA64) || defined(_M_ALPHA) || defined(__amd64) || defined(__amd64__) || defined(_M_AMD64) || defined(__x86_64) || defined(__x86_64__) || defined(_M_X64) || defined(__bfin__)
#    define RAPIDJSON_ENDIAN RAPIDJSON_LITTLEENDIAN
#  elif defined(_MSC_VER) && (defined(_M_ARM) || defined(_M_ARM64))
#    define RAPIDJSON_ENDIAN RAPIDJSON_LITTLEENDIAN
#  elif defined(RAPIDJSON_DOXYGEN_RUNNING)
#    define RAPIDJSON_ENDIAN
#  else
#    error Unknown machine endianness detected. User needs to define RAPIDJSON_ENDIAN.
#  endif
#endif // RAPIDJSON_ENDIAN

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_64BIT

//! Используете ли 64-битную архитектуру
#ifndef RAPIDJSON_64BIT
#if defined(__LP64__) || (defined(__x86_64__) && defined(__ILP32__)) || defined(_WIN64) || defined(__EMSCRIPTEN__)
#define RAPIDJSON_64BIT 1
#else
#define RAPIDJSON_64BIT 0
#endif
#endif // RAPIDJSON_64BIT

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_ALIGN

//! Согласование данных машины.
/*! \ingroup RAPIDJSON_CONFIG
    \param x указатель для спорта

    Некоторые машины требуют строгого выравнивания данных. По умолчанию — 8 байт.
    Пользователь может выполнить настройку, определив функциональный макрос RAPIDJSON_ALIGN.
*/
#ifndef RAPIDJSON_ALIGN
#define RAPIDJSON_ALIGN(x) (((x) + static_cast<size_t>(7u)) & ~static_cast<size_t>(7u))
#endif

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_UINT64_C2

//! Создайте 64-битный литерал по паре 32-битных целых чисел.
/*!
    64-битный литерал с суффиксом ULL или без него подвержен предупреждениям компилятора.
    UINT64_C () — макрос C, вызывающий проблемы с компиляцией.
    Используйте этот макрос, чтобы определить 64-битные константы парой 32-битных целых чисел.
*/
#ifndef RAPIDJSON_UINT64_C2
#define RAPIDJSON_UINT64_C2(high32, low32) ((static_cast<uint64_t>(high32) << 32) | static_cast<uint64_t>(low32))
#endif

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_48BITPOINTER_OPTIMIZATION

//! Для некоторых указателей используйте только младший 48-битный адрес.
/*!
    \ingroup RAPIDJSON_CONFIG

    Эта оптимизация использует тот факт, что текущая архитектура X86 -64 реализует только младший 48-битный виртуальный адрес.
    Старшие 16 бит можно использовать для хранения других данных.
    \c GenericValue использует эту оптимизацию для размера от 24 до 16 байт в 64-битной архитектуре.
*/
#ifndef RAPIDJSON_48BITPOINTER_OPTIMIZATION
#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
#define RAPIDJSON_48BITPOINTER_OPTIMIZATION 1
#else
#define RAPIDJSON_48BITPOINTER_OPTIMIZATION 0
#endif
#endif // RAPIDJSON_48BITPOINTER_OPTIMIZATION

#if RAPIDJSON_48BITPOINTER_OPTIMIZATION == 1
#if RAPIDJSON_64BIT != 1
#error RAPIDJSON_48BITPOINTER_OPTIMIZATION can only be set to 1 when RAPIDJSON_64BIT=1
#endif
#define RAPIDJSON_SETPOINTER(type, p, x) (p = reinterpret_cast<type *>((reinterpret_cast<uintptr_t>(p) & static_cast<uintptr_t>(RAPIDJSON_UINT64_C2(0xFFFF0000, 0x00000000))) | reinterpret_cast<uintptr_t>(reinterpret_cast<const void*>(x))))
#define RAPIDJSON_GETPOINTER(type, p) (reinterpret_cast<type *>(reinterpret_cast<uintptr_t>(p) & static_cast<uintptr_t>(RAPIDJSON_UINT64_C2(0x0000FFFF, 0xFFFFFFFF))))
#else
#define RAPIDJSON_SETPOINTER(type, p, x) (p = (x))
#define RAPIDJSON_GETPOINTER(type, p) (p)
#endif

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_SSE2/RAPIDJSON_SSE42/RAPIDJSON_NEON/RAPIDJSON_SIMD

/*! \def RAPIDJSON_SIMD
    \ingroup RAPIDJSON_CONFIG
    \brief Включите оптимизациюSSE2/SSE4.2/Neon.

    RapidJSON supports optimized implementations for some parsing operations
    на основе расширенийSSE2,SSE4.2 или NEonSIMDна современном Intel
    или процессоры, совместимые с ARM.

    To enable these optimizations, three different symbols can be defined;
    \code
    // Включите оптимизацию SSE2.
    #определить RAPIDJSON_SSE2

    // Включите оптимизацию SSE4 .2.
    #определить RAPIDJSON_SSE42
    \endcode

    // Включите оптимизациюARMNeon.
    #определить RAPIDJSON_NEON
    \endcode

    \c RAPIDJSON_SSE42 имеет приоритет над SSE2 , если оба определены.

    Если какой-либо из этих символов русского языка, RapidJSON определяет макрос
    \c RAPIDJSON_SIMD, чтобы указать наличие оптимизированного кода.
*/
#if defined(RAPIDJSON_SSE2) || defined(RAPIDJSON_SSE42) \
    || defined(RAPIDJSON_NEON) || defined(RAPIDJSON_DOXYGEN_RUNNING)
#define RAPIDJSON_SIMD
#endif

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_NO_SIZETYPEDEFINE

#ifndef RAPIDJSON_NO_SIZETYPEDEFINE
/*! \def RAPIDJSON_NO_SIZETYPEDEFINE
    \ingroup RAPIDJSON_CONFIG
    \brief Предоставленное пользователем \c определение SizeType.

    Чтобы избежать использования 32-битных типов размера для индексации строк и массивов,
    определите этот символьный препроцессор и укажите тип Rapidjson::SizeType
    перед включением RapidJSON:
    \code
    #определить RAPIDJSON_NO_SIZETYPEDEFINE
    пространство имен RapidJSON { typedef ::std::size_tSizeType; }
    #включить "rapidjson/..."
    \endcode

    \см. Рапидджсон::Сизетипе
*/
#ifdef RAPIDJSON_DOXYGEN_RUNNING
#define RAPIDJSON_NO_SIZETYPEDEFINE
#endif
RAPIDJSON_NAMESPACE_BEGIN
//! Тип размера (для длины строки, размера массива и т. д.)
/*! RapidJSON использует 32-битные индексы массива/строки даже на 64-битных платформах.
    вместо использования \csize_t. Пользователи могут переопределить SizeType, определить
    \refRAPIDJSON_NO_SIZETYPEDEFINE.
*/
typedef unsigned SizeType;
RAPIDJSON_NAMESPACE_END
#endif

// всегда импортируйте std::size_t в пространство имен RapidJSON
RAPIDJSON_NAMESPACE_BEGIN
using std::size_t;
RAPIDJSON_NAMESPACE_END

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_ASSERT

//! Утверждение.
/*! \ingroup RAPIDJSON_CONFIG
    По умолчанию Rapidjson использует C\cassert()для внутренних утверждений.
    Пользователь может переопределить его, указав макрос RAPIDJSON_ASSERT (x).

    \note Ошибки синтаксического анализа обрабатываются и могут быть настроены с помощью
          \refAPIRAPIDJSON_ERRORS .
*/
#ifndef RAPIDJSON_ASSERT
#include <cassert>
#define RAPIDJSON_ASSERT(x) assert(x)
#endif // RAPIDJSON_ASSERT

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_STATIC_ASSERT

// Предпочитаю C++11static_assert, если имеется.
#ifndef RAPIDJSON_STATIC_ASSERT
#if RAPIDJSON_CPLUSPLUS >= 201103L || ( defined(_MSC_VER) && _MSC_VER >= 1800 )
#define RAPIDJSON_STATIC_ASSERT(x) \
   static_assert(x, RAPIDJSON_STRINGIFY(x))
#endif // C++11
#endif // RAPIDJSON_STATIC_ASSERT

// Принять внедрение C++03 из boost
#ifndef RAPIDJSON_STATIC_ASSERT
#ifndef __clang__
//! @condRAPIDJSON_HIDDEN_FROM_DOXYGEN
#endif
RAPIDJSON_NAMESPACE_BEGIN
template <bool x> struct STATIC_ASSERTION_FAILURE;
template <> struct STATIC_ASSERTION_FAILURE<true> { enum { value = 1 }; };
template <size_t x> struct StaticAssertTest {};
RAPIDJSON_NAMESPACE_END

#if defined(__GNUC__) || defined(__clang__)
#define RAPIDJSON_STATIC_ASSERT_UNUSED_ATTRIBUTE __attribute__((unused))
#else
#define RAPIDJSON_STATIC_ASSERT_UNUSED_ATTRIBUTE
#endif
#ifndef __clang__
//!  @endcond
#endif

/*! \def RAPIDJSON_STATIC_ASSERT
    \краткий (внутренний) макрос для проверки условий во время компиляции
    \param x условие времени компиляции
    \hideinitializer
 */
#define RAPIDJSON_STATIC_ASSERT(x) \
    typedef ::RAPIDJSON_NAMESPACE::StaticAssertTest< \
      sizeof(::RAPIDJSON_NAMESPACE::STATIC_ASSERTION_FAILURE<bool(x) >)> \
    RAPIDJSON_JOIN(StaticAssertTypedef, __LINE__) RAPIDJSON_STATIC_ASSERT_UNUSED_ATTRIBUTE
#endif // RAPIDJSON_STATIC_ASSERT

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_LIKELY, RAPIDJSON_UNLIKELY

//! Подсказка ветвления компилятора для выражения с высокой вероятностью истинности.
/*!
    \ingroup RAPIDJSON_CONFIG
    \param x Логическое выражение, скорее всего, истинно.
*/
#ifndef RAPIDJSON_LIKELY
#if defined(__GNUC__) || defined(__clang__)
#define RAPIDJSON_LIKELY(x) __builtin_expect(!!(x), 1)
#else
#define RAPIDJSON_LIKELY(x) (x)
#endif
#endif

//! Подсказка компилятора ветвления для выражения с низкой вероятностью быть истинным.
/*!
    \ingroup RAPIDJSON_CONFIG
    \param x Логическое выражение вряд ли будет истинным.
*/
#ifndef RAPIDJSON_UNLIKELY
#if defined(__GNUC__) || defined(__clang__)
#define RAPIDJSON_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define RAPIDJSON_UNLIKELY(x) (x)
#endif
#endif

///////////////////////////////////////////////////////////////////////////////
// Помощники

//! @condRAPIDJSON_HIDDEN_FROM_DOXYGEN

#define RAPIDJSON_MULTILINEMACRO_BEGIN do {
#define RAPIDJSON_MULTILINEMACRO_END \
} while((void)0, 0)

// заимствовано из Boost
#define RAPIDJSON_VERSION_CODE(x,y,z) \
  (((x)*100000) + ((y)*100) + (z))

#if defined(__has_builtin)
#define RAPIDJSON_HAS_BUILTIN(x) __has_builtin(x)
#else
#define RAPIDJSON_HAS_BUILTIN(x) 0
#endif

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_DIAG_PUSH/POP, RAPIDJSON_DIAG_OFF

#if defined(__GNUC__)
#define RAPIDJSON_GNUC \
    RAPIDJSON_VERSION_CODE(__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__)
#endif

#if defined(__clang__) || (defined(RAPIDJSON_GNUC) && RAPIDJSON_GNUC >= RAPIDJSON_VERSION_CODE(4,2,0))

#define RAPIDJSON_PRAGMA(x) _Pragma(RAPIDJSON_STRINGIFY(x))
#define RAPIDJSON_DIAG_PRAGMA(x) RAPIDJSON_PRAGMA(GCC diagnostic x)
#define RAPIDJSON_DIAG_OFF(x) \
    RAPIDJSON_DIAG_PRAGMA(ignored RAPIDJSON_STRINGIFY(RAPIDJSON_JOIN(-W,x)))

// Поддержка push/pop в Clang иGCC>=4.6
#if defined(__clang__) || (defined(RAPIDJSON_GNUC) && RAPIDJSON_GNUC >= RAPIDJSON_VERSION_CODE(4,6,0))
#define RAPIDJSON_DIAG_PUSH RAPIDJSON_DIAG_PRAGMA(push)
#define RAPIDJSON_DIAG_POP  RAPIDJSON_DIAG_PRAGMA(pop)
#else // GCC >= 4.2, < 4.6
#define RAPIDJSON_DIAG_PUSH /* игнорируется */
#define RAPIDJSON_DIAG_POP /* игнорируется */
#endif

#elif defined(_MSC_VER)

// прагма (специфическая для MSVC)
#define RAPIDJSON_PRAGMA(x) __pragma(x)
#define RAPIDJSON_DIAG_PRAGMA(x) RAPIDJSON_PRAGMA(warning(x))

#define RAPIDJSON_DIAG_OFF(x) RAPIDJSON_DIAG_PRAGMA(disable: x)
#define RAPIDJSON_DIAG_PUSH RAPIDJSON_DIAG_PRAGMA(push)
#define RAPIDJSON_DIAG_POP  RAPIDJSON_DIAG_PRAGMA(pop)

#else

#define RAPIDJSON_DIAG_OFF(x) /* игнорируется */
#define RAPIDJSON_DIAG_PUSH   /* игнорируется */
#define RAPIDJSON_DIAG_POP    /* игнорируется */

#endif // RAPIDJSON_DIAG_*

///////////////////////////////////////////////////////////////////////////////
// Возможности С++11

#ifndef RAPIDJSON_HAS_CXX11
#define RAPIDJSON_HAS_CXX11 (RAPIDJSON_CPLUSPLUS >= 201103L)
#endif

#ifndef RAPIDJSON_HAS_CXX11_RVALUE_REFS
#if RAPIDJSON_HAS_CXX11
#define RAPIDJSON_HAS_CXX11_RVALUE_REFS 1
#elif defined(__clang__)
#if __has_feature(cxx_rvalue_references) && \
    (defined(_MSC_VER) || defined(_LIBCPP_VERSION) || defined(__GLIBCXX__) && __GLIBCXX__ >= 20080306)
#define RAPIDJSON_HAS_CXX11_RVALUE_REFS 1
#else
#define RAPIDJSON_HAS_CXX11_RVALUE_REFS 0
#endif
#elif (defined(RAPIDJSON_GNUC) && (RAPIDJSON_GNUC >= RAPIDJSON_VERSION_CODE(4,3,0)) && defined(__GXX_EXPERIMENTAL_CXX0X__)) || \
      (defined(_MSC_VER) && _MSC_VER >= 1600) || \
      (defined(__SUNPRO_CC) && __SUNPRO_CC >= 0x5140 && defined(__GXX_EXPERIMENTAL_CXX0X__))

#define RAPIDJSON_HAS_CXX11_RVALUE_REFS 1
#else
#define RAPIDJSON_HAS_CXX11_RVALUE_REFS 0
#endif
#endif // RAPIDJSON_HAS_CXX11_RVALUE_REFS

#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
#include <utility> // станд::переместить
#endif

#ifndef RAPIDJSON_HAS_CXX11_NOEXCEPT
#if RAPIDJSON_HAS_CXX11
#define RAPIDJSON_HAS_CXX11_NOEXCEPT 1
#elif defined(__clang__)
#define RAPIDJSON_HAS_CXX11_NOEXCEPT __has_feature(cxx_noexcept)
#elif (defined(RAPIDJSON_GNUC) && (RAPIDJSON_GNUC >= RAPIDJSON_VERSION_CODE(4,6,0)) && defined(__GXX_EXPERIMENTAL_CXX0X__)) || \
    (defined(_MSC_VER) && _MSC_VER >= 1900) || \
    (defined(__SUNPRO_CC) && __SUNPRO_CC >= 0x5140 && defined(__GXX_EXPERIMENTAL_CXX0X__))
#define RAPIDJSON_HAS_CXX11_NOEXCEPT 1
#else
#define RAPIDJSON_HAS_CXX11_NOEXCEPT 0
#endif
#endif
#ifndef RAPIDJSON_NOEXCEPT
#if RAPIDJSON_HAS_CXX11_NOEXCEPT
#define RAPIDJSON_NOEXCEPT noexcept
#else
#define RAPIDJSON_NOEXCEPT throw()
#endif // RAPIDJSON_HAS_CXX11_NOEXCEPT
#endif

// автоматического обнаружения пока нет
#ifndef RAPIDJSON_HAS_CXX11_TYPETRAITS
#if (defined(_MSC_VER) && _MSC_VER >= 1700)
#define RAPIDJSON_HAS_CXX11_TYPETRAITS 1
#else
#define RAPIDJSON_HAS_CXX11_TYPETRAITS 0
#endif
#endif

#ifndef RAPIDJSON_HAS_CXX11_RANGE_FOR
#if defined(__clang__)
#define RAPIDJSON_HAS_CXX11_RANGE_FOR __has_feature(cxx_range_for)
#elif (defined(RAPIDJSON_GNUC) && (RAPIDJSON_GNUC >= RAPIDJSON_VERSION_CODE(4,6,0)) && defined(__GXX_EXPERIMENTAL_CXX0X__)) || \
      (defined(_MSC_VER) && _MSC_VER >= 1700) || \
      (defined(__SUNPRO_CC) && __SUNPRO_CC >= 0x5140 && defined(__GXX_EXPERIMENTAL_CXX0X__))
#define RAPIDJSON_HAS_CXX11_RANGE_FOR 1
#else
#define RAPIDJSON_HAS_CXX11_RANGE_FOR 0
#endif
#endif // RAPIDJSON_HAS_CXX11_RANGE_FOR

///////////////////////////////////////////////////////////////////////////////
// Возможности С++17

#ifndef RAPIDJSON_HAS_CXX17
#define RAPIDJSON_HAS_CXX17 (RAPIDJSON_CPLUSPLUS >= 201703L)
#endif

#if RAPIDJSON_HAS_CXX17
# define RAPIDJSON_DELIBERATE_FALLTHROUGH [[fallthrough]]
#elif defined(__has_cpp_attribute)
# if __has_cpp_attribute(clang::fallthrough)
#  define RAPIDJSON_DELIBERATE_FALLTHROUGH [[clang::fallthrough]]
# elif __has_cpp_attribute(fallthrough)
#  define RAPIDJSON_DELIBERATE_FALLTHROUGH __attribute__((fallthrough))
# else
#  define RAPIDJSON_DELIBERATE_FALLTHROUGH
# endif
#else
# define RAPIDJSON_DELIBERATE_FALLTHROUGH
#endif

//!  @endcond

//! Утверждение (в невыдающих контекстах).
 /*! \ingroup RAPIDJSON_CONFIG
    Некоторые функции предоставляют поддержку \c noException, если компилятор поддерживает ее.
    В этих случаях макрос \refRAPIDJSON_ASSERTнельзя переопределить на
    выдать исключение.  Этот макрос добавляет отдельную точку настройки для
    такие случаи.

    По умолчанию C \cassert()(как \refRAPIDJSON_ASSERT), если \c noException
    события, а в противном случае — \refRAPIDJSON_ASSERT.
 */

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_NOEXCEPT_ASSERT

#ifndef RAPIDJSON_NOEXCEPT_ASSERT
#ifdef RAPIDJSON_ASSERT_THROWS
#include <cassert>
#define RAPIDJSON_NOEXCEPT_ASSERT(x) assert(x)
#else
#define RAPIDJSON_NOEXCEPT_ASSERT(x) RAPIDJSON_ASSERT(x)
#endif // RAPIDJSON_ASSERT_THROWS
#endif // RAPIDJSON_NOEXCEPT_ASSERT

///////////////////////////////////////////////////////////////////////////////
// malloc/realloc/free
#include "../../../stdlib/lv_mem.h"
#ifndef RAPIDJSON_MALLOC
///! точка настройки для глобальной сети \c malloc
#define RAPIDJSON_MALLOC(size) lv_malloc(size)
#endif
#ifndef RAPIDJSON_REALLOC
///! точка настройки для глобальной сети \c realloc
#define RAPIDJSON_REALLOC(ptr, new_size) lv_realloc(ptr, new_size)
#endif
#ifndef RAPIDJSON_FREE
///! точка настройки для глобального \c бесплатно
#define RAPIDJSON_FREE(ptr) lv_free(ptr)
#endif

///////////////////////////////////////////////////////////////////////////////
// new/delete

#ifndef RAPIDJSON_NEW
///! точка настройки для глобальной сети \c новая
#define RAPIDJSON_NEW(TypeName) new TypeName
#endif
#ifndef RAPIDJSON_DELETE
///! точка настройки для глобального удаления \c
#define RAPIDJSON_DELETE(x) delete x
#endif

///////////////////////////////////////////////////////////////////////////////
// Тип

/*! \пространство имен RapidJSON
    \brief пространство пространства имен RapidJSON
    \см. RAPIDJSON_NAMESPACE
*/
RAPIDJSON_NAMESPACE_BEGIN

//! Тип значения JSON
enum Type {
    kNullType = 0,      //!< ноль
    kFalseType = 1,     //!< ложь
    kTrueType = 2,      //!< правда
    kObjectType = 3,    //!< объект
    kArrayType = 4,     //!< массив
    kStringType = 5,    //!< строка
    kNumberType = 6     //!< число
};

RAPIDJSON_NAMESPACE_END

#endif // RAPIDJSON_RAPIDJSON_H_
