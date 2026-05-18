// Tencent рада поддержать сообщество открытого исходного кода, сделав доступным RapidJSON.
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

#ifndef RAPIDJSON_READER_H_
#define RAPIDJSON_READER_H_

/*! \file reader.h */

#include "allocators.h"
#include "stream.h"
#include "encodedstream.h"
#include "internal/clzll.h"
#include "internal/meta.h"
#include "internal/stack.h"
#include "internal/strtod.h"
#include <limits>

#if defined(RAPIDJSON_SIMD) && defined(_MSC_VER)
#include <intrin.h>
#pragma intrinsic(_BitScanForward)
#endif
#ifdef RAPIDJSON_SSE42
#include <nmmintrin.h>
#elif defined(RAPIDJSON_SSE2)
#include <emmintrin.h>
#elif defined(RAPIDJSON_NEON)
#include <arm_neon.h>
#endif

#ifdef __clang__
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(old-style-cast)
RAPIDJSON_DIAG_OFF(padded)
RAPIDJSON_DIAG_OFF(switch-enum)
#elif defined(_MSC_VER)
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(4127)  // условное выражение является константой
RAPIDJSON_DIAG_OFF(4702)  // недостижимый код
#endif

#ifdef __GNUC__
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(effc++)
#endif

//! @cond RAPIDJSON_HIDDEN_FROM_DOXYGEN
#define RAPIDJSON_NOTHING /* намеренно пустой */
#ifndef RAPIDJSON_PARSE_ERROR_EARLY_RETURN
#define RAPIDJSON_PARSE_ERROR_EARLY_RETURN(value) \
    RAPIDJSON_MULTILINEMACRO_BEGIN \
    if (RAPIDJSON_UNLIKELY(HasParseError())) { return value; } \
    RAPIDJSON_MULTILINEMACRO_END
#endif
#define RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID \
    RAPIDJSON_PARSE_ERROR_EARLY_RETURN(RAPIDJSON_NOTHING)
//! @endcond

/*! \def RAPIDJSON_PARSE_ERROR_NORETURN
    \ingroup RAPIDJSON_ERRORS
    \brief Макрос, указывающий на ошибку синтаксического анализа.
    \param parseErrorCode \ref Rapidjson::ParseErrorCode ошибки
    \param смещение позиции ошибки во вводе JSON (\c size_t )

    Этот макрос можно использовать в качестве точки настройки внутреннего
    механизм обработки ошибок RapidJSON.

    Распространенной моделью использования является создание исключения вместо требования
    вызывающий объект явно проверяет \ref Rapidjson::GenericReader::Parse
    возвращаемое значение:

    \code
    #определить RAPIDJSON_PARSE_ERROR_NORETURN (parseErrorCode, смещение) \
       бросать ParseException(parseErrorCode, #parseErrorCode, смещение)

    #включить <stdException> // std:: runtime_error
    #включить "rapidjson/error/error.h" // Rapidjson::ParseResult

    struct ParseException : std:: runtime_error , Rapidjson::ParseResult {
      ParseException (код Rapidjson::ParseErrorCode, const char* msg, смещение size_t)
        : std:: runtime_error (msg), ParseResult(код, смещение) {}
    };

    #включить "rapidjson/reader.h"
    \endcode

    \see RAPIDJSON_PARSE_ERROR , Rapidjson::GenericReader::Parse
 */
#ifndef RAPIDJSON_PARSE_ERROR_NORETURN
#define RAPIDJSON_PARSE_ERROR_NORETURN(parseErrorCode, offset) \
    RAPIDJSON_MULTILINEMACRO_BEGIN \
    RAPIDJSON_ASSERT(!HasParseError()); /* Ошибка может быть назначена только один раз */ \
    SetParseError(parseErrorCode, offset); \
    RAPIDJSON_MULTILINEMACRO_END
#endif

/*! \def RAPIDJSON_PARSE_ERROR
    \ingroup RAPIDJSON_ERRORS
    Макрос \brief (внутренний) для указания и обработки ошибки синтаксического анализа.
    \param parseErrorCode \ref Rapidjson::ParseErrorCode ошибки
    \param смещение позиции ошибки во вводе JSON (\c size_t )

    Вызывает RAPIDJSON_PARSE_ERROR_NORETURN и останавливает анализ.

    \см. RAPIDJSON_PARSE_ERROR_NORETURN
    \hideinitializer
 */
#ifndef RAPIDJSON_PARSE_ERROR
#define RAPIDJSON_PARSE_ERROR(parseErrorCode, offset) \
    RAPIDJSON_MULTILINEMACRO_BEGIN \
    RAPIDJSON_PARSE_ERROR_NORETURN(parseErrorCode, offset); \
    RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID; \
    RAPIDJSON_MULTILINEMACRO_END
#endif

#include "error/error.h" // ParseErrorCode, ParseResult

RAPIDJSON_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
// ParseFlag

/*! \def RAPIDJSON_PARSE_DEFAULT_FLAGS
    \ingroup RAPIDJSON_CONFIG
    \brief Определяемое пользователем определение kParseDefaultFlags.

    Пользователь может определить это как любую комбинацию \c ParseFlag.
*/
#ifndef RAPIDJSON_PARSE_DEFAULT_FLAGS
#define RAPIDJSON_PARSE_DEFAULT_FLAGS kParseNoFlags
#endif

//! Комбинация parseFlags
/*! \see Reader::Parse, Document::Parse, Document::ParseInsitu, Document::ParseStream
 */
enum ParseFlag {
    kParseNoFlags = 0,              //!< Флаги не установлены.
    kParseInsituFlag = 1,           //!< Разрушительный анализ на месте.
    kParseValidateEncodingFlag = 2, //!< Проверка кодировки строк JSON.
    kParseIterativeFlag = 4,        //!< Итеративный (постоянная сложность с точки зрения размера стека вызовов функций) синтаксический анализ.
    kParseStopWhenDoneFlag = 8,     //!< После анализа полного корня JSON из потока прекратите дальнейшую обработку остальной части потока. При использовании этого флага синтаксический анализатор не будет генерировать ошибку kParseErrorDocumentRootNotSingular.
    kParseFullPrecisionFlag = 16,   //!< Число анализа с полной точностью (но медленнее).
    kParseCommentsFlag = 32,        //!< Разрешить однострочные (//) и многострочные (/**/) комментарии.
    kParseNumbersAsStringsFlag = 64,    //!< Анализировать все числа (целые/двойные) как строки.
    kParseTrailingCommasFlag = 128, //!< Разрешить конечные запятые в конце объектов и массивов.
    kParseNanAndInfFlag = 256,      //!< Разрешить анализ NaN, Inf, Infinity, -Inf и -Infinity как двойных значений.
    kParseEscapedApostropheFlag = 512,  //!< Разрешить экранированный апостроф в строках.
    kParseDefaultFlags = RAPIDJSON_PARSE_DEFAULT_FLAGS  //!< Флаги анализа по умолчанию. Можно настроить, определив RAPIDJSON_PARSE_DEFAULT_FLAGS.
};

///////////////////////////////////////////////////////////////////////////////
// Обработчик

/*! \класс RapidJSON::Handler
    \brief Концепция получения событий от GenericReader при парсинге.
    Функции возвращают true, если ошибок не происходит. Если они вернут ложь,
    издатель события должен прекратить процесс.
\code
концепция Обработчик {
    имя типа Ch;

    бул Null() ;
    bool Bool(bool b);
    Bool Int (интервал я);
    bool Uint (беззнаковый i);
    bool Int64( int64_t я);
    bool Uint64( uint64_t я);
    bool Double (двойной d);
    /// включено через kParseNumbersAsStringsFlag, строка не завершается нулем (используйте длину)
    bool RawNumber(const Ch* str, длина SizeType, bool copy);
    bool String(const Ch* str, длина SizeType, bool copy);
    бул StartObject() ;
    bool Key(const Ch* str, длина SizeType, bool copy);
    bool EndObject (SizeTypememberCount);
    бул StartArray() ;
    bool EndArray (SizeType elementCount);
};
\endcode
*/
///////////////////////////////////////////////////////////////////////////////
// БазовыйReaderHandler

//! Реализация Handler по умолчанию.
/*! Его можно использовать в качестве базового класса любого обработчика чтения.
    \note реализует концепцию Handler
*/
template<typename Encoding = UTF8<>, typename Derived = void>
struct BaseReaderHandler {
    typedef typename Encoding::Ch Ch;

    typedef typename internal::SelectIf<internal::IsSame<Derived, void>, BaseReaderHandler, Derived>::Type Override;

    bool Default() { return true; }
    bool Null() { return static_cast<Override&>(*this).Default(); }
    bool Bool(bool) { return static_cast<Override&>(*this).Default(); }
    bool Int(int) { return static_cast<Override&>(*this).Default(); }
    bool Uint(unsigned) { return static_cast<Override&>(*this).Default(); }
    bool Int64(int64_t) { return static_cast<Override&>(*this).Default(); }
    bool Uint64(uint64_t) { return static_cast<Override&>(*this).Default(); }
    bool Double(double) { return static_cast<Override&>(*this).Default(); }
    /// включено через kParseNumbersAsStringsFlag, строка не завершается нулем (используйте длину)
    bool RawNumber(const Ch* str, SizeType len, bool copy) { return static_cast<Override&>(*this).String(str, len, copy); }
    bool String(const Ch*, SizeType, bool) { return static_cast<Override&>(*this).Default(); }
    bool StartObject() { return static_cast<Override&>(*this).Default(); }
    bool Key(const Ch* str, SizeType len, bool copy) { return static_cast<Override&>(*this).String(str, len, copy); }
    bool EndObject(SizeType) { return static_cast<Override&>(*this).Default(); }
    bool StartArray() { return static_cast<Override&>(*this).Default(); }
    bool EndArray(SizeType) { return static_cast<Override&>(*this).Default(); }
};

///////////////////////////////////////////////////////////////////////////////
// StreamLocalCopy

namespace internal {

template<typename Stream, int = StreamTraits<Stream>::copyOptimization>
class StreamLocalCopy;

//! Сделайте оптимизацию копирования.
template<typename Stream>
class StreamLocalCopy<Stream, 1> {
public:
    StreamLocalCopy(Stream& original) : s(original), original_(original) {}
    ~StreamLocalCopy() { original_ = s; }

    Stream s;

private:
    StreamLocalCopy& operator=(const StreamLocalCopy&) /* = удалить */;

    Stream& original_;
};

//! Держите ссылку.
template<typename Stream>
class StreamLocalCopy<Stream, 0> {
public:
    StreamLocalCopy(Stream& original) : s(original) {}

    Stream& s;

private:
    StreamLocalCopy& operator=(const StreamLocalCopy&) /* = удалить */;
};

} // внутреннее пространство имен

///////////////////////////////////////////////////////////////////////////////
// ПропуститьПробелы

//! Пропускайте пробелы JSON в потоке.
/*! \param — входной поток для пропуска пробелов.
    \note Эта функция имеет специализацию SSE2/SSE4 .2.
*/
template<typename InputStream>
void SkipWhitespace(InputStream& is) {
    internal::StreamLocalCopy<InputStream> copy(is);
    InputStream& s(copy.s);

    typename InputStream::Ch c;
    while ((c = s.Peek()) == ' ' || c == '\n' || c == '\r' || c == '\t')
        s.Take();
}

inline const char* SkipWhitespace(const char* p, const char* end) {
    while (p != end && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t'))
        ++p;
    return p;
}

#ifdef RAPIDJSON_SSE42
//! Пропускайте пробелы с помощью инструкции SSE 4.2 pcmpistrm, проверяя одновременно 16 8-байтовых символов.
inline const char *SkipWhitespace_SIMD(const char* p) {
    // Быстрый возврат для одного непробельного пробела
    if (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')
        ++p;
    else
        return p;

    // 16-байтовое выравнивание по следующей границе
    const char* nextAligned = reinterpret_cast<const char*>((reinterpret_cast<size_t>(p) + 15) & static_cast<size_t>(~15));
    while (p != nextAligned)
        if (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')
            ++p;
        else
            return p;

    // Остальная часть строки с использованием SIMD
    static const char whitespace[16] = " \n\r\t";
    const __m128i w = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespace[0]));

    for (;; p += 16) {
        const __m128i s = _mm_load_si128(reinterpret_cast<const __m128i *>(p));
        const int r = _mm_cmpistri(w, s, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_LEAST_SIGNIFICANT | _SIDD_NEGATIVE_POLARITY);
        if (r != 16)    // некоторые символы не являются пробелами
            return p + r;
    }
}

inline const char *SkipWhitespace_SIMD(const char* p, const char* end) {
    // Быстрый возврат для одного непробельного пробела
    if (p != end && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t'))
        ++p;
    else
        return p;

    // Середина строки с использованием SIMD
    static const char whitespace[16] = " \n\r\t";
    const __m128i w = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespace[0]));

    for (; p <= end - 16; p += 16) {
        const __m128i s = _mm_loadu_si128(reinterpret_cast<const __m128i *>(p));
        const int r = _mm_cmpistri(w, s, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_LEAST_SIGNIFICANT | _SIDD_NEGATIVE_POLARITY);
        if (r != 16)    // некоторые символы не являются пробелами
            return p + r;
    }

    return SkipWhitespace(p, end);
}

#elif defined(RAPIDJSON_SSE2)

//! Пропускайте пробелы с помощью инструкций SSE2, проверяя одновременно 16 8-байтовых символов.
inline const char *SkipWhitespace_SIMD(const char* p) {
    // Быстрый возврат для одного непробельного пробела
    if (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')
        ++p;
    else
        return p;

    // 16-байтовое выравнивание по следующей границе
    const char* nextAligned = reinterpret_cast<const char*>((reinterpret_cast<size_t>(p) + 15) & static_cast<size_t>(~15));
    while (p != nextAligned)
        if (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')
            ++p;
        else
            return p;

    // Остальная часть строки
    #define C16(c) { c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c }
    static const char whitespaces[4][16] = { C16(' '), C16('\n'), C16('\r'), C16('\t') };
    #undef C16

    const __m128i w0 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespaces[0][0]));
    const __m128i w1 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespaces[1][0]));
    const __m128i w2 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespaces[2][0]));
    const __m128i w3 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespaces[3][0]));

    for (;; p += 16) {
        const __m128i s = _mm_load_si128(reinterpret_cast<const __m128i *>(p));
        __m128i x = _mm_cmpeq_epi8(s, w0);
        x = _mm_or_si128(x, _mm_cmpeq_epi8(s, w1));
        x = _mm_or_si128(x, _mm_cmpeq_epi8(s, w2));
        x = _mm_or_si128(x, _mm_cmpeq_epi8(s, w3));
        unsigned short r = static_cast<unsigned short>(~_mm_movemask_epi8(x));
        if (r != 0) {   // некоторые символы могут быть не пробелами
#ifdef _MSC_VER         // Найдите индекс первого непробельного пробела
            unsigned long offset;
            _BitScanForward(&offset, r);
            return p + offset;
#else
            return p + __builtin_ffs(r) - 1;
#endif
        }
    }
}

inline const char *SkipWhitespace_SIMD(const char* p, const char* end) {
    // Быстрый возврат для одного непробельного пробела
    if (p != end && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t'))
        ++p;
    else
        return p;

    // Остальная часть строки
    #define C16(c) { c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c }
    static const char whitespaces[4][16] = { C16(' '), C16('\n'), C16('\r'), C16('\t') };
    #undef C16

    const __m128i w0 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespaces[0][0]));
    const __m128i w1 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespaces[1][0]));
    const __m128i w2 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespaces[2][0]));
    const __m128i w3 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespaces[3][0]));

    for (; p <= end - 16; p += 16) {
        const __m128i s = _mm_loadu_si128(reinterpret_cast<const __m128i *>(p));
        __m128i x = _mm_cmpeq_epi8(s, w0);
        x = _mm_or_si128(x, _mm_cmpeq_epi8(s, w1));
        x = _mm_or_si128(x, _mm_cmpeq_epi8(s, w2));
        x = _mm_or_si128(x, _mm_cmpeq_epi8(s, w3));
        unsigned short r = static_cast<unsigned short>(~_mm_movemask_epi8(x));
        if (r != 0) {   // некоторые символы могут быть не пробелами
#ifdef _MSC_VER         // Найдите индекс первого непробельного пробела
            unsigned long offset;
            _BitScanForward(&offset, r);
            return p + offset;
#else
            return p + __builtin_ffs(r) - 1;
#endif
        }
    }

    return SkipWhitespace(p, end);
}

#elif defined(RAPIDJSON_NEON)

//! Пропускайте пробелы с помощью инструкций ARM Neon, проверяя одновременно 16 8-байтовых символов.
inline const char *SkipWhitespace_SIMD(const char* p) {
    // Быстрый возврат для одного непробельного пробела
    if (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')
        ++p;
    else
        return p;

    // 16-байтовое выравнивание по следующей границе
    const char* nextAligned = reinterpret_cast<const char*>((reinterpret_cast<size_t>(p) + 15) & static_cast<size_t>(~15));
    while (p != nextAligned)
        if (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')
            ++p;
        else
            return p;

    const uint8x16_t w0 = vmovq_n_u8(' ');
    const uint8x16_t w1 = vmovq_n_u8('\n');
    const uint8x16_t w2 = vmovq_n_u8('\r');
    const uint8x16_t w3 = vmovq_n_u8('\t');

    for (;; p += 16) {
        const uint8x16_t s = vld1q_u8(reinterpret_cast<const uint8_t *>(p));
        uint8x16_t x = vceqq_u8(s, w0);
        x = vorrq_u8(x, vceqq_u8(s, w1));
        x = vorrq_u8(x, vceqq_u8(s, w2));
        x = vorrq_u8(x, vceqq_u8(s, w3));

        x = vmvnq_u8(x);                       // Отрицать
        x = vrev64q_u8(x);                     // Оборот в 64
        uint64_t low = vgetq_lane_u64(vreinterpretq_u64_u8(x), 0);   // извлекать
        uint64_t high = vgetq_lane_u64(vreinterpretq_u64_u8(x), 1);  // извлекать

        if (low == 0) {
            if (high != 0) {
                uint32_t lz = internal::clzll(high);
                return p + 8 + (lz >> 3);
            }
        } else {
            uint32_t lz = internal::clzll(low);
            return p + (lz >> 3);
        }
    }
}

inline const char *SkipWhitespace_SIMD(const char* p, const char* end) {
    // Быстрый возврат для одного непробельного пробела
    if (p != end && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t'))
        ++p;
    else
        return p;

    const uint8x16_t w0 = vmovq_n_u8(' ');
    const uint8x16_t w1 = vmovq_n_u8('\n');
    const uint8x16_t w2 = vmovq_n_u8('\r');
    const uint8x16_t w3 = vmovq_n_u8('\t');

    for (; p <= end - 16; p += 16) {
        const uint8x16_t s = vld1q_u8(reinterpret_cast<const uint8_t *>(p));
        uint8x16_t x = vceqq_u8(s, w0);
        x = vorrq_u8(x, vceqq_u8(s, w1));
        x = vorrq_u8(x, vceqq_u8(s, w2));
        x = vorrq_u8(x, vceqq_u8(s, w3));

        x = vmvnq_u8(x);                       // Отрицать
        x = vrev64q_u8(x);                     // Оборот в 64
        uint64_t low = vgetq_lane_u64(vreinterpretq_u64_u8(x), 0);   // извлекать
        uint64_t high = vgetq_lane_u64(vreinterpretq_u64_u8(x), 1);  // извлекать

        if (low == 0) {
            if (high != 0) {
                uint32_t lz = internal::clzll(high);
                return p + 8 + (lz >> 3);
            }
        } else {
            uint32_t lz = internal::clzll(low);
            return p + (lz >> 3);
        }
    }

    return SkipWhitespace(p, end);
}

#endif // RAPIDJSON_NEON

#ifdef RAPIDJSON_SIMD
//! Специализация функции шаблона для InsituStringStream
template<> inline void SkipWhitespace(InsituStringStream& is) {
    is.src_ = const_cast<char*>(SkipWhitespace_SIMD(is.src_));
}

//! Специализация функции шаблона для StringStream
template<> inline void SkipWhitespace(StringStream& is) {
    is.src_ = SkipWhitespace_SIMD(is.src_);
}

template<> inline void SkipWhitespace(EncodedInputStream<UTF8<>, MemoryStream>& is) {
    is.is_.src_ = SkipWhitespace_SIMD(is.is_.src_, is.is_.end_);
}
#endif // RAPIDJSON_SIMD

///////////////////////////////////////////////////////////////////////////////
// GenericReader

//!  SAX - парсер JSON в стиле. Используйте \ref Reader для кодировки UTF8 и распределителя по умолчанию.
/*! GenericReader анализирует текст JSON из потока и синхронно отправляет события в
    объект, реализующий концепцию Handler.

    Ему необходимо выделить стек для хранения одной декодированной строки во время
    неразрушающий разбор.

    При разборе на месте декодированная строка напрямую записывается в источник.
    текстовая строка, временный буфер не требуется.

    Объект GenericReader можно повторно использовать для анализа нескольких текстов JSON.

    \tparam SourceEncoding Кодирование входного потока.
    \tparam TargetEncoding Кодирование результатов анализа.
    \tparam StackAllocator Тип распределителя для стека.
*/
template <typename SourceEncoding, typename TargetEncoding, typename StackAllocator = CrtAllocator>
class GenericReader {
public:
    typedef typename SourceEncoding::Ch Ch; //!< Тип символа SourceEncoding

    //! Конструктор.
    /*! \param stackAllocator Необязательный распределитель для выделения памяти стека. (Использовать только для неразрушающего анализа)
        \param stackCapacity Емкость стека в байтах для хранения одной декодированной строки.  (Использовать только для неразрушающего анализа)
    */
    GenericReader(StackAllocator* stackAllocator = 0, size_t stackCapacity = kDefaultStackCapacity) :
        stack_(stackAllocator, stackCapacity), parseResult_(), state_(IterativeParsingStartState) {}

    //! Разобрать текст JSON.
    /*! \tparam parseFlags Комбинация \ref ParseFlag.
        \tparam InputStream Тип входного потока, реализующий концепцию потока.
        \tparam Handler Тип обработчика, реализующий концепцию обработчика.
        \param — входной поток для анализа.
        \param handler Обработчик для получения событий.
        \return Успешно ли выполнен синтаксический анализ.
    */
    template <unsigned parseFlags, typename InputStream, typename Handler>
    ParseResult Parse(InputStream& is, Handler& handler) {
        if (parseFlags & kParseIterativeFlag)
            return IterativeParse<parseFlags>(is, handler);

        parseResult_.Clear();

        ClearStackOnExit scope(*this);

        SkipWhitespaceAndComments<parseFlags>(is);
        RAPIDJSON_PARSE_ERROR_EARLY_RETURN(parseResult_);

        if (RAPIDJSON_UNLIKELY(is.Peek() == '\0')) {
            RAPIDJSON_PARSE_ERROR_NORETURN(kParseErrorDocumentEmpty, is.Tell());
            RAPIDJSON_PARSE_ERROR_EARLY_RETURN(parseResult_);
        }
        else {
            ParseValue<parseFlags>(is, handler);
            RAPIDJSON_PARSE_ERROR_EARLY_RETURN(parseResult_);

            if (!(parseFlags & kParseStopWhenDoneFlag)) {
                SkipWhitespaceAndComments<parseFlags>(is);
                RAPIDJSON_PARSE_ERROR_EARLY_RETURN(parseResult_);

                if (RAPIDJSON_UNLIKELY(is.Peek() != '\0')) {
                    RAPIDJSON_PARSE_ERROR_NORETURN(kParseErrorDocumentRootNotSingular, is.Tell());
                    RAPIDJSON_PARSE_ERROR_EARLY_RETURN(parseResult_);
                }
            }
        }

        return parseResult_;
    }

    //! Разобрать текст JSON (с помощью \ref kParseDefaultFlags)
    /*! \tparam InputStream Тип входного потока, реализующий концепцию потока
        \tparam Handler Тип обработчика, реализующий концепцию обработчика.
        \param — входной поток для анализа.
        \param handler Обработчик для получения событий.
        \return Успешно ли выполнен синтаксический анализ.
    */
    template <typename InputStream, typename Handler>
    ParseResult Parse(InputStream& is, Handler& handler) {
        return Parse<kParseDefaultFlags>(is, handler);
    }

    //! Инициализировать поточный анализ текстового токена JSON
    /*!
     */
    void IterativeParseInit() {
        parseResult_.Clear();
        state_ = IterativeParsingStartState;
    }

    //! Разобрать один токен из текста JSON
    /*! \tparam InputStream Тип входного потока, реализующий концепцию потока
        \tparam Handler Тип обработчика, реализующий концепцию обработчика.
        \param — входной поток для анализа.
        \param handler Обработчик для получения событий.
        \return Успешно ли выполнен синтаксический анализ.
     */
    template <unsigned parseFlags, typename InputStream, typename Handler>
    bool IterativeParseNext(InputStream& is, Handler& handler) {
        while (RAPIDJSON_LIKELY(is.Peek() != '\0')) {
            SkipWhitespaceAndComments<parseFlags>(is);

            Token t = Tokenize(is.Peek());
            IterativeParsingState n = Predict(state_, t);
            IterativeParsingState d = Transit<parseFlags>(state_, t, n, is, handler);

            // Если мы закончили или столкнулись с ошибкой...
            if (RAPIDJSON_UNLIKELY(IsIterativeParsingCompleteState(d))) {
                // Сообщайте об ошибках.
                if (d == IterativeParsingErrorState) {
                    HandleError(state_, is);
                    return false;
                }

                // Переход в финишное состояние.
                RAPIDJSON_ASSERT(d == IterativeParsingFinishState);
                state_ = d;

                // Если StopWhenDone не установлен...
                if (!(parseFlags & kParseStopWhenDoneFlag)) {
                    // ... и обнаружены дополнительные данные без пробелов...
                    SkipWhitespaceAndComments<parseFlags>(is);
                    if (is.Peek() != '\0') {
                        // ... это считается ошибкой.
                        HandleError(state_, is);
                        return false;
                    }
                }

                // Успех! Мы закончили!
                return true;
            }

            // Переход в новое состояние.
            state_ = d;

            // Если мы проанализировали что-то кроме разделителя, мы вызвали обработчик, поэтому теперь мы можем вернуть true.
            if (!IsIterativeParsingDelimiterState(n))
                return true;
        }

        // Мы достигли конца файла.
        stack_.Clear();

        if (state_ != IterativeParsingFinishState) {
            HandleError(state_, is);
            return false;
        }

        return true;
    }

    //! Проверьте, завершен ли потокенный анализ текста JSON.
    /*! \return Был ли JSON полностью декодирован.
     */
    RAPIDJSON_FORCEINLINE bool IterativeParseComplete() const {
        return IsIterativeParsingCompleteState(state_);
    }

    //! Произошла ли ошибка синтаксического анализа при последнем синтаксическом анализе.
    bool HasParseError() const { return parseResult_.IsError(); }

    //! Получите \ref ParseErrorCode последнего синтаксического анализа.
    ParseErrorCode GetParseErrorCode() const { return parseResult_.Code(); }

    //! Получите позицию последней ошибки синтаксического анализа во входных данных, в противном случае — 0.
    size_t GetErrorOffset() const { return parseResult_.Offset(); }

protected:
    void SetParseError(ParseErrorCode code, size_t offset) { parseResult_.Set(code, offset); }

private:
    // Запретить конструктор копирования и оператор присваивания.
    GenericReader(const GenericReader&);
    GenericReader& operator=(const GenericReader&);

    void ClearStack() { stack_.Clear(); }

    // очистить стек при любом выходе из ParseStream, например. из-за исключения
    struct ClearStackOnExit {
        explicit ClearStackOnExit(GenericReader& r) : r_(r) {}
        ~ClearStackOnExit() { r_.ClearStack(); }
    private:
        GenericReader& r_;
        ClearStackOnExit(const ClearStackOnExit&);
        ClearStackOnExit& operator=(const ClearStackOnExit&);
    };

    template<unsigned parseFlags, typename InputStream>
    void SkipWhitespaceAndComments(InputStream& is) {
        SkipWhitespace(is);

        if (parseFlags & kParseCommentsFlag) {
            while (RAPIDJSON_UNLIKELY(Consume(is, '/'))) {
                if (Consume(is, '*')) {
                    while (true) {
                        if (RAPIDJSON_UNLIKELY(is.Peek() == '\0'))
                            RAPIDJSON_PARSE_ERROR(kParseErrorUnspecificSyntaxError, is.Tell());
                        else if (Consume(is, '*')) {
                            if (Consume(is, '/'))
                                break;
                        }
                        else
                            is.Take();
                    }
                }
                else if (RAPIDJSON_LIKELY(Consume(is, '/')))
                    while (is.Peek() != '\0' && is.Take() != '\n') {}
                else
                    RAPIDJSON_PARSE_ERROR(kParseErrorUnspecificSyntaxError, is.Tell());

                SkipWhitespace(is);
            }
        }
    }

    // Разобрать объект: {строка: значение, ... }
    template<unsigned parseFlags, typename InputStream, typename Handler>
    void ParseObject(InputStream& is, Handler& handler) {
        RAPIDJSON_ASSERT(is.Peek() == '{');
        is.Take();  // Пропустить '{'

        if (RAPIDJSON_UNLIKELY(!handler.StartObject()))
            RAPIDJSON_PARSE_ERROR(kParseErrorTermination, is.Tell());

        SkipWhitespaceAndComments<parseFlags>(is);
        RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;

        if (Consume(is, '}')) {
            if (RAPIDJSON_UNLIKELY(!handler.EndObject(0)))  // пустой объект
                RAPIDJSON_PARSE_ERROR(kParseErrorTermination, is.Tell());
            return;
        }

        for (SizeType memberCount = 0;;) {
            if (RAPIDJSON_UNLIKELY(is.Peek() != '"'))
                RAPIDJSON_PARSE_ERROR(kParseErrorObjectMissName, is.Tell());

            ParseString<parseFlags>(is, handler, true);
            RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;

            SkipWhitespaceAndComments<parseFlags>(is);
            RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;

            if (RAPIDJSON_UNLIKELY(!Consume(is, ':')))
                RAPIDJSON_PARSE_ERROR(kParseErrorObjectMissColon, is.Tell());

            SkipWhitespaceAndComments<parseFlags>(is);
            RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;

            ParseValue<parseFlags>(is, handler);
            RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;

            SkipWhitespaceAndComments<parseFlags>(is);
            RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;

            ++memberCount;

            switch (is.Peek()) {
                case ',':
                    is.Take();
                    SkipWhitespaceAndComments<parseFlags>(is);
                    RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;
                    break;
                case '}':
                    is.Take();
                    if (RAPIDJSON_UNLIKELY(!handler.EndObject(memberCount)))
                        RAPIDJSON_PARSE_ERROR(kParseErrorTermination, is.Tell());
                    return;
                default:
                    RAPIDJSON_PARSE_ERROR(kParseErrorObjectMissCommaOrCurlyBracket, is.Tell()); break; // Этот бесполезный перерыв предназначен только для того, чтобы сделать предупреждение и освещение радостными.
            }

            if (parseFlags & kParseTrailingCommasFlag) {
                if (is.Peek() == '}') {
                    if (RAPIDJSON_UNLIKELY(!handler.EndObject(memberCount)))
                        RAPIDJSON_PARSE_ERROR(kParseErrorTermination, is.Tell());
                    is.Take();
                    return;
                }
            }
        }
    }

    // Разобрать массив: [значение, ...]
    template<unsigned parseFlags, typename InputStream, typename Handler>
    void ParseArray(InputStream& is, Handler& handler) {
        RAPIDJSON_ASSERT(is.Peek() == '[');
        is.Take();  // Пропустить '['

        if (RAPIDJSON_UNLIKELY(!handler.StartArray()))
            RAPIDJSON_PARSE_ERROR(kParseErrorTermination, is.Tell());

        SkipWhitespaceAndComments<parseFlags>(is);
        RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;

        if (Consume(is, ']')) {
            if (RAPIDJSON_UNLIKELY(!handler.EndArray(0))) // пустой массив
                RAPIDJSON_PARSE_ERROR(kParseErrorTermination, is.Tell());
            return;
        }

        for (SizeType elementCount = 0;;) {
            ParseValue<parseFlags>(is, handler);
            RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;

            ++elementCount;
            SkipWhitespaceAndComments<parseFlags>(is);
            RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;

            if (Consume(is, ',')) {
                SkipWhitespaceAndComments<parseFlags>(is);
                RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;
            }
            else if (Consume(is, ']')) {
                if (RAPIDJSON_UNLIKELY(!handler.EndArray(elementCount)))
                    RAPIDJSON_PARSE_ERROR(kParseErrorTermination, is.Tell());
                return;
            }
            else
                RAPIDJSON_PARSE_ERROR(kParseErrorArrayMissCommaOrSquareBracket, is.Tell());

            if (parseFlags & kParseTrailingCommasFlag) {
                if (is.Peek() == ']') {
                    if (RAPIDJSON_UNLIKELY(!handler.EndArray(elementCount)))
                        RAPIDJSON_PARSE_ERROR(kParseErrorTermination, is.Tell());
                    is.Take();
                    return;
                }
            }
        }
    }

    template<unsigned parseFlags, typename InputStream, typename Handler>
    void ParseNull(InputStream& is, Handler& handler) {
        RAPIDJSON_ASSERT(is.Peek() == 'n');
        is.Take();

        if (RAPIDJSON_LIKELY(Consume(is, 'u') && Consume(is, 'l') && Consume(is, 'l'))) {
            if (RAPIDJSON_UNLIKELY(!handler.Null()))
                RAPIDJSON_PARSE_ERROR(kParseErrorTermination, is.Tell());
        }
        else
            RAPIDJSON_PARSE_ERROR(kParseErrorValueInvalid, is.Tell());
    }

    template<unsigned parseFlags, typename InputStream, typename Handler>
    void ParseTrue(InputStream& is, Handler& handler) {
        RAPIDJSON_ASSERT(is.Peek() == 't');
        is.Take();

        if (RAPIDJSON_LIKELY(Consume(is, 'r') && Consume(is, 'u') && Consume(is, 'e'))) {
            if (RAPIDJSON_UNLIKELY(!handler.Bool(true)))
                RAPIDJSON_PARSE_ERROR(kParseErrorTermination, is.Tell());
        }
        else
            RAPIDJSON_PARSE_ERROR(kParseErrorValueInvalid, is.Tell());
    }

    template<unsigned parseFlags, typename InputStream, typename Handler>
    void ParseFalse(InputStream& is, Handler& handler) {
        RAPIDJSON_ASSERT(is.Peek() == 'f');
        is.Take();

        if (RAPIDJSON_LIKELY(Consume(is, 'a') && Consume(is, 'l') && Consume(is, 's') && Consume(is, 'e'))) {
            if (RAPIDJSON_UNLIKELY(!handler.Bool(false)))
                RAPIDJSON_PARSE_ERROR(kParseErrorTermination, is.Tell());
        }
        else
            RAPIDJSON_PARSE_ERROR(kParseErrorValueInvalid, is.Tell());
    }

    template<typename InputStream>
    RAPIDJSON_FORCEINLINE static bool Consume(InputStream& is, typename InputStream::Ch expect) {
        if (RAPIDJSON_LIKELY(is.Peek() == expect)) {
            is.Take();
            return true;
        }
        else
            return false;
    }

    // Вспомогательная функция для анализа четырех шестнадцатеричных цифр в \uXXXX в ParseString().
    template<typename InputStream>
    unsigned ParseHex4(InputStream& is, size_t escapeOffset) {
        unsigned codepoint = 0;
        for (int i = 0; i < 4; i++) {
            Ch c = is.Peek();
            codepoint <<= 4;
            codepoint += static_cast<unsigned>(c);
            if (c >= '0' && c <= '9')
                codepoint -= '0';
            else if (c >= 'A' && c <= 'F')
                codepoint -= 'A' - 10;
            else if (c >= 'a' && c <= 'f')
                codepoint -= 'a' - 10;
            else {
                RAPIDJSON_PARSE_ERROR_NORETURN(kParseErrorStringUnicodeEscapeInvalidHex, escapeOffset);
                RAPIDJSON_PARSE_ERROR_EARLY_RETURN(0);
            }
            is.Take();
        }
        return codepoint;
    }

    template <typename CharType>
    class StackStream {
    public:
        typedef CharType Ch;

        StackStream(internal::Stack<StackAllocator>& stack) : stack_(stack), length_(0) {}
        RAPIDJSON_FORCEINLINE void Put(Ch c) {
            *stack_.template Push<Ch>() = c;
            ++length_;
        }

        RAPIDJSON_FORCEINLINE void* Push(SizeType count) {
            length_ += count;
            return stack_.template Push<Ch>(count);
        }

        size_t Length() const { return length_; }

        Ch* Pop() {
            return stack_.template Pop<Ch>(length_);
        }

    private:
        StackStream(const StackStream&);
        StackStream& operator=(const StackStream&);

        internal::Stack<StackAllocator>& stack_;
        SizeType length_;
    };

    // Разобрать строку и сгенерировать событие String. Различные пути кода для kParseInsituFlag.
    template<unsigned parseFlags, typename InputStream, typename Handler>
    void ParseString(InputStream& is, Handler& handler, bool isKey = false) {
        internal::StreamLocalCopy<InputStream> copy(is);
        InputStream& s(copy.s);

        RAPIDJSON_ASSERT(s.Peek() == '\"');
        s.Take();  // Пропустить '\"'

        bool success = false;
        if (parseFlags & kParseInsituFlag) {
            typename InputStream::Ch *head = s.PutBegin();
            ParseStringToStream<parseFlags, SourceEncoding, SourceEncoding>(s, s);
            RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;
            size_t length = s.PutEnd(head) - 1;
            RAPIDJSON_ASSERT(length <= 0xFFFFFFFF);
            const typename TargetEncoding::Ch* const str = reinterpret_cast<typename TargetEncoding::Ch*>(head);
            success = (isKey ? handler.Key(str, SizeType(length), false) : handler.String(str, SizeType(length), false));
        }
        else {
            StackStream<typename TargetEncoding::Ch> stackStream(stack_);
            ParseStringToStream<parseFlags, SourceEncoding, TargetEncoding>(s, stackStream);
            RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;
            SizeType length = static_cast<SizeType>(stackStream.Length()) - 1;
            const typename TargetEncoding::Ch* const str = stackStream.Pop();
            success = (isKey ? handler.Key(str, length, true) : handler.String(str, length, true));
        }
        if (RAPIDJSON_UNLIKELY(!success))
            RAPIDJSON_PARSE_ERROR(kParseErrorTermination, s.Tell());
    }

    // Строка синтаксического анализа для вывода
    // Эта функция обрабатывает двойные кавычки префикса/суффикса, экранирование и дополнительную проверку кодировки.
    template<unsigned parseFlags, typename SEncoding, typename TEncoding, typename InputStream, typename OutputStream>
    RAPIDJSON_FORCEINLINE void ParseStringToStream(InputStream& is, OutputStream& os) {
//! @cond RAPIDJSON_HIDDEN_FROM_DOXYGEN
#define Z16 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        static const char escape[256] = {
            Z16, Z16, 0, 0,'\"', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '/',
            Z16, Z16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,'\\', 0, 0, 0,
            0, 0,'\b', 0, 0, 0,'\f', 0, 0, 0, 0, 0, 0, 0,'\n', 0,
            0, 0,'\r', 0,'\t', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            Z16, Z16, Z16, Z16, Z16, Z16, Z16, Z16
        };
#undef Z16
//! @endcond

        for (;;) {
            // Сканируйте и копируйте строку перед "\\\"" или < 0x20. Это необязательная оптимизация.
            if (!(parseFlags & kParseValidateEncodingFlag))
                ScanCopyUnescapedString(is, os);

            Ch c = is.Peek();
            if (RAPIDJSON_UNLIKELY(c == '\\')) {    // Побег
                size_t escapeOffset = is.Tell();    // В случае недопустимого экранирования сообщите начальный '\\' как смещение ошибки.
                is.Take();
                Ch e = is.Peek();
                if ((sizeof(Ch) == 1 || unsigned(e) < 256) && RAPIDJSON_LIKELY(escape[static_cast<unsigned char>(e)])) {
                    is.Take();
                    os.Put(static_cast<typename TEncoding::Ch>(escape[static_cast<unsigned char>(e)]));
                }
                else if ((parseFlags & kParseEscapedApostropheFlag) && RAPIDJSON_LIKELY(e == '\'')) { // Разрешить экранированный апостроф
                    is.Take();
                    os.Put('\'');
                }
                else if (RAPIDJSON_LIKELY(e == 'u')) {    // Юникод
                    is.Take();
                    unsigned codepoint = ParseHex4(is, escapeOffset);
                    RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;
                    if (RAPIDJSON_UNLIKELY(codepoint >= 0xD800 && codepoint <= 0xDFFF)) {
                        // старший суррогат, проверьте, следует ли за ним действительный младший суррогат
                        if (RAPIDJSON_LIKELY(codepoint <= 0xDBFF)) {
                            // Ручка UTF -16 суррогатная пара
                            if (RAPIDJSON_UNLIKELY(!Consume(is, '\\') || !Consume(is, 'u')))
                                RAPIDJSON_PARSE_ERROR(kParseErrorStringUnicodeSurrogateInvalid, escapeOffset);
                            unsigned codepoint2 = ParseHex4(is, escapeOffset);
                            RAPIDJSON_PARSE_ERROR_EARLY_RETURN_VOID;
                            if (RAPIDJSON_UNLIKELY(codepoint2 < 0xDC00 || codepoint2 > 0xDFFF))
                                RAPIDJSON_PARSE_ERROR(kParseErrorStringUnicodeSurrogateInvalid, escapeOffset);
                            codepoint = (((codepoint - 0xD800) << 10) | (codepoint2 - 0xDC00)) + 0x10000;
                        }
                        // одиночный низкий суррогатный
                        else
                        {
                            RAPIDJSON_PARSE_ERROR(kParseErrorStringUnicodeSurrogateInvalid, escapeOffset);
                        }
                    }
                    TEncoding::Encode(os, codepoint);
                }
                else
                    RAPIDJSON_PARSE_ERROR(kParseErrorStringEscapeInvalid, escapeOffset);
            }
            else if (RAPIDJSON_UNLIKELY(c == '"')) {    // Закрытие двойной кавычки
                is.Take();
                os.Put('\0');   // завершить строку нулем
                return;
            }
            else if (RAPIDJSON_UNLIKELY(static_cast<unsigned>(c) < 0x20)) { // RFC 4627: неэкранированный = %x20-21 / %x23-5B / %x5D-10FFFF
                if (c == '\0')
                    RAPIDJSON_PARSE_ERROR(kParseErrorStringMissQuotationMark, is.Tell());
                else
                    RAPIDJSON_PARSE_ERROR(kParseErrorStringInvalidEncoding, is.Tell());
            }
            else {
                size_t offset = is.Tell();
                if (RAPIDJSON_UNLIKELY((parseFlags & kParseValidateEncodingFlag ?
                    !Transcoder<SEncoding, TEncoding>::Validate(is, os) :
                    !Transcoder<SEncoding, TEncoding>::Transcode(is, os))))
                    RAPIDJSON_PARSE_ERROR(kParseErrorStringInvalidEncoding, offset);
            }
        }
    }

    template<typename InputStream, typename OutputStream>
    static RAPIDJSON_FORCEINLINE void ScanCopyUnescapedString(InputStream&, OutputStream&) {
            // Ничего не делать для общей версии
    }

#if defined(RAPIDJSON_SSE2) || defined(RAPIDJSON_SSE42)
    // StringStream → StackStream<char>
    static RAPIDJSON_FORCEINLINE void ScanCopyUnescapedString(StringStream& is, StackStream<char>& os) {
        const char* p = is.src_;

        // Сканируйте по одному до выравнивания (невыровненная загрузка может пересечь границу страницы и привести к сбою)
        const char* nextAligned = reinterpret_cast<const char*>((reinterpret_cast<size_t>(p) + 15) & static_cast<size_t>(~15));
        while (p != nextAligned)
            if (RAPIDJSON_UNLIKELY(*p == '\"') || RAPIDJSON_UNLIKELY(*p == '\\') || RAPIDJSON_UNLIKELY(static_cast<unsigned>(*p) < 0x20)) {
                is.src_ = p;
                return;
            }
            else
                os.Put(*p++);

        // Остальная часть строки с использованием SIMD
        static const char dquote[16] = { '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"' };
        static const char bslash[16] = { '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\' };
        static const char space[16]  = { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F };
        const __m128i dq = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&dquote[0]));
        const __m128i bs = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&bslash[0]));
        const __m128i sp = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&space[0]));

        for (;; p += 16) {
            const __m128i s = _mm_load_si128(reinterpret_cast<const __m128i *>(p));
            const __m128i t1 = _mm_cmpeq_epi8(s, dq);
            const __m128i t2 = _mm_cmpeq_epi8(s, bs);
            const __m128i t3 = _mm_cmpeq_epi8(_mm_max_epu8(s, sp), sp); // s < 0x20 <=> max(s, 0x1F ) == 0x1F
            const __m128i x = _mm_or_si128(_mm_or_si128(t1, t2), t3);
            unsigned short r = static_cast<unsigned short>(_mm_movemask_epi8(x));
            if (RAPIDJSON_UNLIKELY(r != 0)) {   // некоторые символы экранированы
                SizeType length;
    #ifdef _MSC_VER         // Найдите индекс первого сбежавшего
                unsigned long offset;
                _BitScanForward(&offset, r);
                length = offset;
    #else
                length = static_cast<SizeType>(__builtin_ffs(r) - 1);
    #endif
                if (length != 0) {
                    char* q = reinterpret_cast<char*>(os.Push(length));
                    for (size_t i = 0; i < length; i++)
                        q[i] = p[i];

                    p += length;
                }
                break;
            }
            _mm_storeu_si128(reinterpret_cast<__m128i *>(os.Push(16)), s);
        }

        is.src_ = p;
    }

    // InsituStringStream → InsituStringStream
    static RAPIDJSON_FORCEINLINE void ScanCopyUnescapedString(InsituStringStream& is, InsituStringStream& os) {
        RAPIDJSON_ASSERT(&is == &os);
        (void)os;

        if (is.src_ == is.dst_) {
            SkipUnescapedString(is);
            return;
        }

        char* p = is.src_;
        char *q = is.dst_;

        // Сканируйте по одному до выравнивания (невыровненная загрузка может пересечь границу страницы и привести к сбою)
        const char* nextAligned = reinterpret_cast<const char*>((reinterpret_cast<size_t>(p) + 15) & static_cast<size_t>(~15));
        while (p != nextAligned)
            if (RAPIDJSON_UNLIKELY(*p == '\"') || RAPIDJSON_UNLIKELY(*p == '\\') || RAPIDJSON_UNLIKELY(static_cast<unsigned>(*p) < 0x20)) {
                is.src_ = p;
                is.dst_ = q;
                return;
            }
            else
                *q++ = *p++;

        // Остальная часть строки с использованием SIMD
        static const char dquote[16] = { '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"' };
        static const char bslash[16] = { '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\' };
        static const char space[16] = { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F };
        const __m128i dq = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&dquote[0]));
        const __m128i bs = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&bslash[0]));
        const __m128i sp = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&space[0]));

        for (;; p += 16, q += 16) {
            const __m128i s = _mm_load_si128(reinterpret_cast<const __m128i *>(p));
            const __m128i t1 = _mm_cmpeq_epi8(s, dq);
            const __m128i t2 = _mm_cmpeq_epi8(s, bs);
            const __m128i t3 = _mm_cmpeq_epi8(_mm_max_epu8(s, sp), sp); // s < 0x20 <=> max(s, 0x1F ) == 0x1F
            const __m128i x = _mm_or_si128(_mm_or_si128(t1, t2), t3);
            unsigned short r = static_cast<unsigned short>(_mm_movemask_epi8(x));
            if (RAPIDJSON_UNLIKELY(r != 0)) {   // некоторые символы экранированы
                size_t length;
#ifdef _MSC_VER         // Найдите индекс первого сбежавшего
                unsigned long offset;
                _BitScanForward(&offset, r);
                length = offset;
#else
                length = static_cast<size_t>(__builtin_ffs(r) - 1);
#endif
                for (const char* pend = p + length; p != pend; )
                    *q++ = *p++;
                break;
            }
            _mm_storeu_si128(reinterpret_cast<__m128i *>(q), s);
        }

        is.src_ = p;
        is.dst_ = q;
    }

    // Если указатели чтения/записи одинаковы для потока insitu, просто пропустите неэкранированные символы.
    static RAPIDJSON_FORCEINLINE void SkipUnescapedString(InsituStringStream& is) {
        RAPIDJSON_ASSERT(is.src_ == is.dst_);
        char* p = is.src_;

        // Сканируйте по одному до выравнивания (невыровненная загрузка может пересечь границу страницы и привести к сбою)
        const char* nextAligned = reinterpret_cast<const char*>((reinterpret_cast<size_t>(p) + 15) & static_cast<size_t>(~15));
        for (; p != nextAligned; p++)
            if (RAPIDJSON_UNLIKELY(*p == '\"') || RAPIDJSON_UNLIKELY(*p == '\\') || RAPIDJSON_UNLIKELY(static_cast<unsigned>(*p) < 0x20)) {
                is.src_ = is.dst_ = p;
                return;
            }

        // Остальная часть строки с использованием SIMD
        static const char dquote[16] = { '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"' };
        static const char bslash[16] = { '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\' };
        static const char space[16] = { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F };
        const __m128i dq = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&dquote[0]));
        const __m128i bs = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&bslash[0]));
        const __m128i sp = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&space[0]));

        for (;; p += 16) {
            const __m128i s = _mm_load_si128(reinterpret_cast<const __m128i *>(p));
            const __m128i t1 = _mm_cmpeq_epi8(s, dq);
            const __m128i t2 = _mm_cmpeq_epi8(s, bs);
            const __m128i t3 = _mm_cmpeq_epi8(_mm_max_epu8(s, sp), sp); // s < 0x20 <=> max(s, 0x1F ) == 0x1F
            const __m128i x = _mm_or_si128(_mm_or_si128(t1, t2), t3);
            unsigned short r = static_cast<unsigned short>(_mm_movemask_epi8(x));
            if (RAPIDJSON_UNLIKELY(r != 0)) {   // некоторые символы экранированы
                size_t length;
#ifdef _MSC_VER         // Найдите индекс первого сбежавшего
                unsigned long offset;
                _BitScanForward(&offset, r);
                length = offset;
#else
                length = static_cast<size_t>(__builtin_ffs(r) - 1);
#endif
                p += length;
                break;
            }
        }

        is.src_ = is.dst_ = p;
    }
#elif defined(RAPIDJSON_NEON)
    // StringStream → StackStream<char>
    static RAPIDJSON_FORCEINLINE void ScanCopyUnescapedString(StringStream& is, StackStream<char>& os) {
        const char* p = is.src_;

        // Сканируйте по одному до выравнивания (невыровненная загрузка может пересечь границу страницы и привести к сбою)
        const char* nextAligned = reinterpret_cast<const char*>((reinterpret_cast<size_t>(p) + 15) & static_cast<size_t>(~15));
        while (p != nextAligned)
            if (RAPIDJSON_UNLIKELY(*p == '\"') || RAPIDJSON_UNLIKELY(*p == '\\') || RAPIDJSON_UNLIKELY(static_cast<unsigned>(*p) < 0x20)) {
                is.src_ = p;
                return;
            }
            else
                os.Put(*p++);

        // Остальная часть строки с использованием SIMD
        const uint8x16_t s0 = vmovq_n_u8('"');
        const uint8x16_t s1 = vmovq_n_u8('\\');
        const uint8x16_t s2 = vmovq_n_u8('\b');
        const uint8x16_t s3 = vmovq_n_u8(32);

        for (;; p += 16) {
            const uint8x16_t s = vld1q_u8(reinterpret_cast<const uint8_t *>(p));
            uint8x16_t x = vceqq_u8(s, s0);
            x = vorrq_u8(x, vceqq_u8(s, s1));
            x = vorrq_u8(x, vceqq_u8(s, s2));
            x = vorrq_u8(x, vcltq_u8(s, s3));

            x = vrev64q_u8(x);                     // Оборот в 64
            uint64_t low = vgetq_lane_u64(vreinterpretq_u64_u8(x), 0);   // извлекать
            uint64_t high = vgetq_lane_u64(vreinterpretq_u64_u8(x), 1);  // извлекать

            SizeType length = 0;
            bool escaped = false;
            if (low == 0) {
                if (high != 0) {
                    uint32_t lz = internal::clzll(high);
                    length = 8 + (lz >> 3);
                    escaped = true;
                }
            } else {
                uint32_t lz = internal::clzll(low);
                length = lz >> 3;
                escaped = true;
            }
            if (RAPIDJSON_UNLIKELY(escaped)) {   // некоторые символы экранированы
                if (length != 0) {
                    char* q = reinterpret_cast<char*>(os.Push(length));
                    for (size_t i = 0; i < length; i++)
                        q[i] = p[i];

                    p += length;
                }
                break;
            }
            vst1q_u8(reinterpret_cast<uint8_t *>(os.Push(16)), s);
        }

        is.src_ = p;
    }

    // InsituStringStream → InsituStringStream
    static RAPIDJSON_FORCEINLINE void ScanCopyUnescapedString(InsituStringStream& is, InsituStringStream& os) {
        RAPIDJSON_ASSERT(&is == &os);
        (void)os;

        if (is.src_ == is.dst_) {
            SkipUnescapedString(is);
            return;
        }

        char* p = is.src_;
        char *q = is.dst_;

        // Сканируйте по одному до выравнивания (невыровненная загрузка может пересечь границу страницы и привести к сбою)
        const char* nextAligned = reinterpret_cast<const char*>((reinterpret_cast<size_t>(p) + 15) & static_cast<size_t>(~15));
        while (p != nextAligned)
            if (RAPIDJSON_UNLIKELY(*p == '\"') || RAPIDJSON_UNLIKELY(*p == '\\') || RAPIDJSON_UNLIKELY(static_cast<unsigned>(*p) < 0x20)) {
                is.src_ = p;
                is.dst_ = q;
                return;
            }
            else
                *q++ = *p++;

        // Остальная часть строки с использованием SIMD
        const uint8x16_t s0 = vmovq_n_u8('"');
        const uint8x16_t s1 = vmovq_n_u8('\\');
        const uint8x16_t s2 = vmovq_n_u8('\b');
        const uint8x16_t s3 = vmovq_n_u8(32);

        for (;; p += 16, q += 16) {
            const uint8x16_t s = vld1q_u8(reinterpret_cast<uint8_t *>(p));
            uint8x16_t x = vceqq_u8(s, s0);
            x = vorrq_u8(x, vceqq_u8(s, s1));
            x = vorrq_u8(x, vceqq_u8(s, s2));
            x = vorrq_u8(x, vcltq_u8(s, s3));

            x = vrev64q_u8(x);                     // Оборот в 64
            uint64_t low = vgetq_lane_u64(vreinterpretq_u64_u8(x), 0);   // извлекать
            uint64_t high = vgetq_lane_u64(vreinterpretq_u64_u8(x), 1);  // извлекать

            SizeType length = 0;
            bool escaped = false;
            if (low == 0) {
                if (high != 0) {
                    uint32_t lz = internal::clzll(high);
                    length = 8 + (lz >> 3);
                    escaped = true;
                }
            } else {
                uint32_t lz = internal::clzll(low);
                length = lz >> 3;
                escaped = true;
            }
            if (RAPIDJSON_UNLIKELY(escaped)) {   // некоторые символы экранированы
                for (const char* pend = p + length; p != pend; ) {
                    *q++ = *p++;
                }
                break;
            }
            vst1q_u8(reinterpret_cast<uint8_t *>(q), s);
        }

        is.src_ = p;
        is.dst_ = q;
    }

    // Если указатели чтения/записи одинаковы для потока insitu, просто пропустите неэкранированные символы.
    static RAPIDJSON_FORCEINLINE void SkipUnescapedString(InsituStringStream& is) {
        RAPIDJSON_ASSERT(is.src_ == is.dst_);
        char* p = is.src_;

        // Сканируйте по одному до выравнивания (невыровненная загрузка может пересечь границу страницы и привести к сбою)
        const char* nextAligned = reinterpret_cast<const char*>((reinterpret_cast<size_t>(p) + 15) & static_cast<size_t>(~15));
        for (; p != nextAligned; p++)
            if (RAPIDJSON_UNLIKELY(*p == '\"') || RAPIDJSON_UNLIKELY(*p == '\\') || RAPIDJSON_UNLIKELY(static_cast<unsigned>(*p) < 0x20)) {
                is.src_ = is.dst_ = p;
                return;
            }

        // Остальная часть строки с использованием SIMD
        const uint8x16_t s0 = vmovq_n_u8('"');
        const uint8x16_t s1 = vmovq_n_u8('\\');
        const uint8x16_t s2 = vmovq_n_u8('\b');
        const uint8x16_t s3 = vmovq_n_u8(32);

        for (;; p += 16) {
            const uint8x16_t s = vld1q_u8(reinterpret_cast<uint8_t *>(p));
            uint8x16_t x = vceqq_u8(s, s0);
            x = vorrq_u8(x, vceqq_u8(s, s1));
            x = vorrq_u8(x, vceqq_u8(s, s2));
            x = vorrq_u8(x, vcltq_u8(s, s3));

            x = vrev64q_u8(x);                     // Оборот в 64
            uint64_t low = vgetq_lane_u64(vreinterpretq_u64_u8(x), 0);   // извлекать
            uint64_t high = vgetq_lane_u64(vreinterpretq_u64_u8(x), 1);  // извлекать

            if (low == 0) {
                if (high != 0) {
                    uint32_t lz = internal::clzll(high);
                    p += 8 + (lz >> 3);
                    break;
                }
            } else {
                uint32_t lz = internal::clzll(low);
                p += lz >> 3;
                break;
            }
        }

        is.src_ = is.dst_ = p;
    }
#endif // RAPIDJSON_NEON

    template<typename InputStream, typename StackCharacter, bool backup, bool pushOnTake>
    class NumberStream;

    template<typename InputStream, typename StackCharacter>
    class NumberStream<InputStream, StackCharacter, false, false> {
    public:
        typedef typename InputStream::Ch Ch;

        NumberStream(GenericReader& reader, InputStream& s) : is(s) { (void)reader;  }

        RAPIDJSON_FORCEINLINE Ch Peek() const { return is.Peek(); }
        RAPIDJSON_FORCEINLINE Ch TakePush() { return is.Take(); }
        RAPIDJSON_FORCEINLINE Ch Take() { return is.Take(); }
        RAPIDJSON_FORCEINLINE void Push(char) {}

        size_t Tell() { return is.Tell(); }
        size_t Length() { return 0; }
        const StackCharacter* Pop() { return 0; }

    protected:
        NumberStream& operator=(const NumberStream&);

        InputStream& is;
    };

    template<typename InputStream, typename StackCharacter>
    class NumberStream<InputStream, StackCharacter, true, false> : public NumberStream<InputStream, StackCharacter, false, false> {
        typedef NumberStream<InputStream, StackCharacter, false, false> Base;
    public:
        NumberStream(GenericReader& reader, InputStream& s) : Base(reader, s), stackStream(reader.stack_) {}

        RAPIDJSON_FORCEINLINE Ch TakePush() {
            stackStream.Put(static_cast<StackCharacter>(Base::is.Peek()));
            return Base::is.Take();
        }

        RAPIDJSON_FORCEINLINE void Push(StackCharacter c) {
            stackStream.Put(c);
        }

        size_t Length() { return stackStream.Length(); }

        const StackCharacter* Pop() {
            stackStream.Put('\0');
            return stackStream.Pop();
        }

    private:
        StackStream<StackCharacter> stackStream;
    };

    template<typename InputStream, typename StackCharacter>
    class NumberStream<InputStream, StackCharacter, true, true> : public NumberStream<InputStream, StackCharacter, true, false> {
        typedef NumberStream<InputStream, StackCharacter, true, false> Base;
    public:
        NumberStream(GenericReader& reader, InputStream& s) : Base(reader, s) {}

        RAPIDJSON_FORCEINLINE Ch Take() { return Base::TakePush(); }
    };

    template<unsigned parseFlags, typename InputStream, typename Handler>
    void ParseNumber(InputStream& is, Handler& handler) {
        typedef typename internal::SelectIf<internal::BoolType<(parseFlags & kParseNumbersAsStringsFlag) != 0>, typename TargetEncoding::Ch, char>::Type NumberCharacter;

        internal::StreamLocalCopy<InputStream> copy(is);
        NumberStream<InputStream, NumberCharacter,
            ((parseFlags & kParseNumbersAsStringsFlag) != 0) ?
                ((parseFlags & kParseInsituFlag) == 0) :
                ((parseFlags & kParseFullPrecisionFlag) != 0),
            (parseFlags & kParseNumbersAsStringsFlag) != 0 &&
                (parseFlags & kParseInsituFlag) == 0> s(*this, copy.s);

        size_t startOffset = s.Tell();
        double d = 0.0;
        bool useNanOrInf = false;

        // Разобрать минус
        bool minus = Consume(s, '-');

        // Разобрать int: ноль / (цифры 1-9 * DIGIT)
        unsigned i = 0;
        uint64_t i64 = 0;
        bool use64bit = false;
        int significandDigit = 0;
        if (RAPIDJSON_UNLIKELY(s.Peek() == '0')) {
            i = 0;
            s.TakePush();
        }
        else if (RAPIDJSON_LIKELY(s.Peek() >= '1' && s.Peek() <= '9')) {
            i = static_cast<unsigned>(s.TakePush() - '0');

            if (minus)
                while (RAPIDJSON_LIKELY(s.Peek() >= '0' && s.Peek() <= '9')) {
                    if (RAPIDJSON_UNLIKELY(i >= 214748364)) { // 2^31 = 2147483648
                        if (RAPIDJSON_LIKELY(i != 214748364 || s.Peek() > '8')) {
                            i64 = i;
                            use64bit = true;
                            break;
                        }
                    }
                    i = i * 10 + static_cast<unsigned>(s.TakePush() - '0');
                    significandDigit++;
                }
            else
                while (RAPIDJSON_LIKELY(s.Peek() >= '0' && s.Peek() <= '9')) {
                    if (RAPIDJSON_UNLIKELY(i >= 429496729)) { // 2^32 - 1 = 4294967295
                        if (RAPIDJSON_LIKELY(i != 429496729 || s.Peek() > '5')) {
                            i64 = i;
                            use64bit = true;
                            break;
                        }
                    }
                    i = i * 10 + static_cast<unsigned>(s.TakePush() - '0');
                    significandDigit++;
                }
        }
        // Разберите NaN или бесконечность здесь
        else if ((parseFlags & kParseNanAndInfFlag) && RAPIDJSON_LIKELY((s.Peek() == 'I' || s.Peek() == 'N'))) {
            if (Consume(s, 'N')) {
                if (Consume(s, 'a') && Consume(s, 'N')) {
                    d = std::numeric_limits<double>::quiet_NaN();
                    useNanOrInf = true;
                }
            }
            else if (RAPIDJSON_LIKELY(Consume(s, 'I'))) {
                if (Consume(s, 'n') && Consume(s, 'f')) {
                    d = (minus ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity());
                    useNanOrInf = true;

                    if (RAPIDJSON_UNLIKELY(s.Peek() == 'i' && !(Consume(s, 'i') && Consume(s, 'n')
                                                                && Consume(s, 'i') && Consume(s, 't') && Consume(s, 'y')))) {
                        RAPIDJSON_PARSE_ERROR(kParseErrorValueInvalid, s.Tell());
                    }
                }
            }

            if (RAPIDJSON_UNLIKELY(!useNanOrInf)) {
                RAPIDJSON_PARSE_ERROR(kParseErrorValueInvalid, s.Tell());
            }
        }
        else
            RAPIDJSON_PARSE_ERROR(kParseErrorValueInvalid, s.Tell());

        // Разобрать 64-битное целое число
        bool useDouble = false;
        if (use64bit) {
            if (minus)
                while (RAPIDJSON_LIKELY(s.Peek() >= '0' && s.Peek() <= '9')) {
                     if (RAPIDJSON_UNLIKELY(i64 >= RAPIDJSON_UINT64_C2(0x0CCCCCCC, 0xCCCCCCCC))) // 2^63 = 9223372036854775808
                        if (RAPIDJSON_LIKELY(i64 != RAPIDJSON_UINT64_C2(0x0CCCCCCC, 0xCCCCCCCC) || s.Peek() > '8')) {
                            d = static_cast<double>(i64);
                            useDouble = true;
                            break;
                        }
                    i64 = i64 * 10 + static_cast<unsigned>(s.TakePush() - '0');
                    significandDigit++;
                }
            else
                while (RAPIDJSON_LIKELY(s.Peek() >= '0' && s.Peek() <= '9')) {
                    if (RAPIDJSON_UNLIKELY(i64 >= RAPIDJSON_UINT64_C2(0x19999999, 0x99999999))) // 2^64 - 1 = 18446744073709551615
                        if (RAPIDJSON_LIKELY(i64 != RAPIDJSON_UINT64_C2(0x19999999, 0x99999999) || s.Peek() > '5')) {
                            d = static_cast<double>(i64);
                            useDouble = true;
                            break;
                        }
                    i64 = i64 * 10 + static_cast<unsigned>(s.TakePush() - '0');
                    significandDigit++;
                }
        }

        // Принудительно удвоить большое целое число
        if (useDouble) {
            while (RAPIDJSON_LIKELY(s.Peek() >= '0' && s.Peek() <= '9')) {
                d = d * 10 + (s.TakePush() - '0');
            }
        }

        // Разбор frac = десятичная точка 1* DIGIT
        int expFrac = 0;
        size_t decimalPosition;
        if (Consume(s, '.')) {
            decimalPosition = s.Length();

            if (RAPIDJSON_UNLIKELY(!(s.Peek() >= '0' && s.Peek() <= '9')))
                RAPIDJSON_PARSE_ERROR(kParseErrorNumberMissFraction, s.Tell());

            if (!useDouble) {
#if RAPIDJSON_64BIT
                // Используйте i64 для хранения мантиссы в 64-битной архитектуре
                if (!use64bit)
                    i64 = i;

                while (RAPIDJSON_LIKELY(s.Peek() >= '0' && s.Peek() <= '9')) {
                    if (i64 > RAPIDJSON_UINT64_C2(0x1FFFFF, 0xFFFFFFFF)) // 2^53 - 1 для быстрого пути
                        break;
                    else {
                        i64 = i64 * 10 + static_cast<unsigned>(s.TakePush() - '0');
                        --expFrac;
                        if (i64 != 0)
                            significandDigit++;
                    }
                }

                d = static_cast<double>(i64);
#else
                // Используйте double для хранения мантиссы в 32-битной архитектуре
                d = static_cast<double>(use64bit ? i64 : i);
#endif
                useDouble = true;
            }

            while (RAPIDJSON_LIKELY(s.Peek() >= '0' && s.Peek() <= '9')) {
                if (significandDigit < 17) {
                    d = d * 10.0 + (s.TakePush() - '0');
                    --expFrac;
                    if (RAPIDJSON_LIKELY(d > 0.0))
                        significandDigit++;
                }
                else
                    s.TakePush();
            }
        }
        else
            decimalPosition = s.Length(); // десятичная позиция в конце целого числа.

        // Разобрать exp = e [минус/плюс] 1* DIGIT
        int exp = 0;
        if (Consume(s, 'e') || Consume(s, 'E')) {
            if (!useDouble) {
                d = static_cast<double>(use64bit ? i64 : i);
                useDouble = true;
            }

            bool expMinus = false;
            if (Consume(s, '+'))
                ;
            else if (Consume(s, '-'))
                expMinus = true;

            if (RAPIDJSON_LIKELY(s.Peek() >= '0' && s.Peek() <= '9')) {
                exp = static_cast<int>(s.Take() - '0');
                if (expMinus) {
                    // (exp + expFrac) не должно переполнять int => мы определяем, когда -exp получает
                    // опасно близко к INT_MIN (пессимистичная следующая цифра 9 подтолкнет ее к
                    // подтопленная территория):
                    //
                    //        -(exp * 10 + 9) + expFrac >= INT_MIN
                    //   <=> exp <= (expFrac - INT_MIN - 9) / 10
                    RAPIDJSON_ASSERT(expFrac <= 0);
                    int maxExp = (expFrac + 2147483639) / 10;

                    while (RAPIDJSON_LIKELY(s.Peek() >= '0' && s.Peek() <= '9')) {
                        exp = exp * 10 + static_cast<int>(s.Take() - '0');
                        if (RAPIDJSON_UNLIKELY(exp > maxExp)) {
                            while (RAPIDJSON_UNLIKELY(s.Peek() >= '0' && s.Peek() <= '9'))  // Поглотить остальную часть экспоненты
                                s.Take();
                        }
                    }
                }
                else {  // положительный опыт
                    int maxExp = 308 - expFrac;
                    while (RAPIDJSON_LIKELY(s.Peek() >= '0' && s.Peek() <= '9')) {
                        exp = exp * 10 + static_cast<int>(s.Take() - '0');
                        if (RAPIDJSON_UNLIKELY(exp > maxExp))
                            RAPIDJSON_PARSE_ERROR(kParseErrorNumberTooBig, startOffset);
                    }
                }
            }
            else
                RAPIDJSON_PARSE_ERROR(kParseErrorNumberMissExponent, s.Tell());

            if (expMinus)
                exp = -exp;
        }

        // Завершите разбор, вызовите событие по типу номера.
        bool cont = true;

        if (parseFlags & kParseNumbersAsStringsFlag) {
            if (parseFlags & kParseInsituFlag) {
                s.Pop();  // Вытаскивайте стек независимо от того, будет он использоваться или нет.
                typename InputStream::Ch* head = is.PutBegin();
                const size_t length = s.Tell() - startOffset;
                RAPIDJSON_ASSERT(length <= 0xFFFFFFFF);
                // невозможно вставить сюда символ \0, запятая после этого числа будет удалена
                const typename TargetEncoding::Ch* const str = reinterpret_cast<typename TargetEncoding::Ch*>(head);
                cont = handler.RawNumber(str, SizeType(length), false);
            }
            else {
                SizeType numCharsToCopy = static_cast<SizeType>(s.Length());
                GenericStringStream<UTF8<NumberCharacter> > srcStream(s.Pop());
                StackStream<typename TargetEncoding::Ch> dstStream(stack_);
                while (numCharsToCopy--) {
                    Transcoder<UTF8<typename TargetEncoding::Ch>, TargetEncoding>::Transcode(srcStream, dstStream);
                }
                dstStream.Put('\0');
                const typename TargetEncoding::Ch* str = dstStream.Pop();
                const SizeType length = static_cast<SizeType>(dstStream.Length()) - 1;
                cont = handler.RawNumber(str, SizeType(length), true);
            }
        }
        else {
           size_t length = s.Length();
           const NumberCharacter* decimal = s.Pop();  // Вытаскивайте стек независимо от того, будет он использоваться или нет.

           if (useDouble) {
               int p = exp + expFrac;
               if (parseFlags & kParseFullPrecisionFlag)
                   d = internal::StrtodFullPrecision(d, p, decimal, length, decimalPosition, exp);
               else
                   d = internal::StrtodNormalPrecision(d, p);

               // Используйте > max вместо == inf, чтобы исправить ложное предупреждение -Wfloat-equal
               if (d > (std::numeric_limits<double>::max)()) {
                   // Переполнение
                   // TODO: internal::StrtodX should report overflow (or underflow)
                   RAPIDJSON_PARSE_ERROR(kParseErrorNumberTooBig, startOffset);
               }

               cont = handler.Double(minus ? -d : d);
           }
           else if (useNanOrInf) {
               cont = handler.Double(d);
           }
           else {
               if (use64bit) {
                   if (minus)
                       cont = handler.Int64(static_cast<int64_t>(~i64 + 1));
                   else
                       cont = handler.Uint64(i64);
               }
               else {
                   if (minus)
                       cont = handler.Int(static_cast<int32_t>(~i + 1));
                   else
                       cont = handler.Uint(i);
               }
           }
        }
        if (RAPIDJSON_UNLIKELY(!cont))
            RAPIDJSON_PARSE_ERROR(kParseErrorTermination, startOffset);
    }

    // Разобрать любое значение JSON
    template<unsigned parseFlags, typename InputStream, typename Handler>
    void ParseValue(InputStream& is, Handler& handler) {
        switch (is.Peek()) {
            case 'n': ParseNull  <parseFlags>(is, handler); break;
            case 't': ParseTrue  <parseFlags>(is, handler); break;
            case 'f': ParseFalse <parseFlags>(is, handler); break;
            case '"': ParseString<parseFlags>(is, handler); break;
            case '{': ParseObject<parseFlags>(is, handler); break;
            case '[': ParseArray <parseFlags>(is, handler); break;
            default :
                      ParseNumber<parseFlags>(is, handler);
                      break;

        }
    }

    // Итеративный анализ

    // Штаты
    enum IterativeParsingState {
        IterativeParsingFinishState = 0, // состояния стока вверху
        IterativeParsingErrorState,      // состояния стока вверху
        IterativeParsingStartState,

        // Состояния объекта
        IterativeParsingObjectInitialState,
        IterativeParsingMemberKeyState,
        IterativeParsingMemberValueState,
        IterativeParsingObjectFinishState,

        // Состояния массива
        IterativeParsingArrayInitialState,
        IterativeParsingElementState,
        IterativeParsingArrayFinishState,

        // Состояние одного значения
        IterativeParsingValueState,

        // Состояния разделителей (внизу)
        IterativeParsingElementDelimiterState,
        IterativeParsingMemberDelimiterState,
        IterativeParsingKeyValueDelimiterState,

        cIterativeParsingStateCount
    };

    // Токены
    enum Token {
        LeftBracketToken = 0,
        RightBracketToken,

        LeftCurlyBracketToken,
        RightCurlyBracketToken,

        CommaToken,
        ColonToken,

        StringToken,
        FalseToken,
        TrueToken,
        NullToken,
        NumberToken,

        kTokenCount
    };

    RAPIDJSON_FORCEINLINE Token Tokenize(Ch c) const {

//! @cond RAPIDJSON_HIDDEN_FROM_DOXYGEN
#define N NumberToken
#define N16 N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N
        // Карты от ASCII до Token
        static const unsigned char tokenMap[256] = {
            N16, // 00~0F
            N16, // 10~1F
            N, N, StringToken, N, N, N, N, N, N, N, N, N, CommaToken, N, N, N, // 20~2F
            N, N, N, N, N, N, N, N, N, N, ColonToken, N, N, N, N, N, // 30~3F
            N16, // 40~4F
            N, N, N, N, N, N, N, N, N, N, N, LeftBracketToken, N, RightBracketToken, N, N, // 50~5F
            N, N, N, N, N, N, FalseToken, N, N, N, N, N, N, N, NullToken, N, // 60~6F
            N, N, N, N, TrueToken, N, N, N, N, N, N, LeftCurlyBracketToken, N, RightCurlyBracketToken, N, N, // 70~7F
            N16, N16, N16, N16, N16, N16, N16, N16 // 80~FF
        };
#undef N
#undef N16
//! @endcond

        if (sizeof(Ch) == 1 || static_cast<unsigned>(c) < 256)
            return static_cast<Token>(tokenMap[static_cast<unsigned char>(c)]);
        else
            return NumberToken;
    }

    RAPIDJSON_FORCEINLINE IterativeParsingState Predict(IterativeParsingState state, Token token) const {
        // текущее состояние x один токен просмотра -> новое состояние
        static const char G[cIterativeParsingStateCount][kTokenCount] = {
            // Окончание (состояние раковины)
            {
                IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState,
                IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState,
                IterativeParsingErrorState
            },
            // Ошибка (состояние приемника)
            {
                IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState,
                IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState,
                IterativeParsingErrorState
            },
            // Старт
            {
                IterativeParsingArrayInitialState,  // Левая скобка
                IterativeParsingErrorState,         // Правая скобка
                IterativeParsingObjectInitialState, // Левая фигурная скобка
                IterativeParsingErrorState,         // Правая фигурная скобка
                IterativeParsingErrorState,         // Запятая
                IterativeParsingErrorState,         // Двоеточие
                IterativeParsingValueState,         // Строка
                IterativeParsingValueState,         // Ложь
                IterativeParsingValueState,         // Правда
                IterativeParsingValueState,         // Ноль
                IterativeParsingValueState          // Номер
            },
            // Объектиниал
            {
                IterativeParsingErrorState,         // Левая скобка
                IterativeParsingErrorState,         // Правая скобка
                IterativeParsingErrorState,         // Левая фигурная скобка
                IterativeParsingObjectFinishState,  // Правая фигурная скобка
                IterativeParsingErrorState,         // Запятая
                IterativeParsingErrorState,         // Двоеточие
                IterativeParsingMemberKeyState,     // Строка
                IterativeParsingErrorState,         // Ложь
                IterativeParsingErrorState,         // Правда
                IterativeParsingErrorState,         // Ноль
                IterativeParsingErrorState          // Номер
            },
            // ЧленКей
            {
                IterativeParsingErrorState,             // Левая скобка
                IterativeParsingErrorState,             // Правая скобка
                IterativeParsingErrorState,             // Левая фигурная скобка
                IterativeParsingErrorState,             // Правая фигурная скобка
                IterativeParsingErrorState,             // Запятая
                IterativeParsingKeyValueDelimiterState, // Двоеточие
                IterativeParsingErrorState,             // Строка
                IterativeParsingErrorState,             // Ложь
                IterativeParsingErrorState,             // Правда
                IterativeParsingErrorState,             // Ноль
                IterativeParsingErrorState              // Номер
            },
            // ЧленВалуэ
            {
                IterativeParsingErrorState,             // Левая скобка
                IterativeParsingErrorState,             // Правая скобка
                IterativeParsingErrorState,             // Левая фигурная скобка
                IterativeParsingObjectFinishState,      // Правая фигурная скобка
                IterativeParsingMemberDelimiterState,   // Запятая
                IterativeParsingErrorState,             // Двоеточие
                IterativeParsingErrorState,             // Строка
                IterativeParsingErrorState,             // Ложь
                IterativeParsingErrorState,             // Правда
                IterativeParsingErrorState,             // Ноль
                IterativeParsingErrorState              // Номер
            },
            // ObjectFinish (состояние приемника)
            {
                IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState,
                IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState,
                IterativeParsingErrorState
            },
            // МассивНачальный
            {
                IterativeParsingArrayInitialState,      // Левая скобка (нажмите состояние элемента)
                IterativeParsingArrayFinishState,       // Правая скобка
                IterativeParsingObjectInitialState,     // Левая фигурная скобка (нажмите состояние элемента)
                IterativeParsingErrorState,             // Правая фигурная скобка
                IterativeParsingErrorState,             // Запятая
                IterativeParsingErrorState,             // Двоеточие
                IterativeParsingElementState,           // Строка
                IterativeParsingElementState,           // Ложь
                IterativeParsingElementState,           // Правда
                IterativeParsingElementState,           // Ноль
                IterativeParsingElementState            // Номер
            },
            // Элемент
            {
                IterativeParsingErrorState,             // Левая скобка
                IterativeParsingArrayFinishState,       // Правая скобка
                IterativeParsingErrorState,             // Левая фигурная скобка
                IterativeParsingErrorState,             // Правая фигурная скобка
                IterativeParsingElementDelimiterState,  // Запятая
                IterativeParsingErrorState,             // Двоеточие
                IterativeParsingErrorState,             // Строка
                IterativeParsingErrorState,             // Ложь
                IterativeParsingErrorState,             // Правда
                IterativeParsingErrorState,             // Ноль
                IterativeParsingErrorState              // Номер
            },
            // ArrayFinish (состояние приемника)
            {
                IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState,
                IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState,
                IterativeParsingErrorState
            },
            // Одно значение (состояние приемника)
            {
                IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState,
                IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState, IterativeParsingErrorState,
                IterativeParsingErrorState
            },
            // ЭлементРазделитель
            {
                IterativeParsingArrayInitialState,      // Левая скобка (нажмите состояние элемента)
                IterativeParsingArrayFinishState,       // Правая скобка
                IterativeParsingObjectInitialState,     // Левая фигурная скобка (нажмите состояние элемента)
                IterativeParsingErrorState,             // Правая фигурная скобка
                IterativeParsingErrorState,             // Запятая
                IterativeParsingErrorState,             // Двоеточие
                IterativeParsingElementState,           // Строка
                IterativeParsingElementState,           // Ложь
                IterativeParsingElementState,           // Правда
                IterativeParsingElementState,           // Ноль
                IterativeParsingElementState            // Номер
            },
            // Разделитель члена
            {
                IterativeParsingErrorState,         // Левая скобка
                IterativeParsingErrorState,         // Правая скобка
                IterativeParsingErrorState,         // Левая фигурная скобка
                IterativeParsingObjectFinishState,  // Правая фигурная скобка
                IterativeParsingErrorState,         // Запятая
                IterativeParsingErrorState,         // Двоеточие
                IterativeParsingMemberKeyState,     // Строка
                IterativeParsingErrorState,         // Ложь
                IterativeParsingErrorState,         // Правда
                IterativeParsingErrorState,         // Ноль
                IterativeParsingErrorState          // Номер
            },
            // КлючЗначениеРазделитель
            {
                IterativeParsingArrayInitialState,      // Левая скобка (нажмите состояние MemberValue)
                IterativeParsingErrorState,             // Правая скобка
                IterativeParsingObjectInitialState,     // Левая фигурная скобка (нажмите состояние MemberValue)
                IterativeParsingErrorState,             // Правая фигурная скобка
                IterativeParsingErrorState,             // Запятая
                IterativeParsingErrorState,             // Двоеточие
                IterativeParsingMemberValueState,       // Строка
                IterativeParsingMemberValueState,       // Ложь
                IterativeParsingMemberValueState,       // Правда
                IterativeParsingMemberValueState,       // Ноль
                IterativeParsingMemberValueState        // Номер
            },
        }; // Конец G

        return static_cast<IterativeParsingState>(G[state][token]);
    }

    // Сделайте шаг вперед в потоке токенов и состоянии на основе состояния назначения-кандидата, которое было возвращено Transit() .
    // Может вернуть новое состояние населению штата.
    template <unsigned parseFlags, typename InputStream, typename Handler>
    RAPIDJSON_FORCEINLINE IterativeParsingState Transit(IterativeParsingState src, Token token, IterativeParsingState dst, InputStream& is, Handler& handler) {
        (void)token;

        switch (dst) {
        case IterativeParsingErrorState:
            return dst;

        case IterativeParsingObjectInitialState:
        case IterativeParsingArrayInitialState:
        {
            // Нажмите состояние (Element или MemberValue), если мы вложены в другой массив или значение члена.
            // Таким образом, мы можем получить правильное состояние ObjectFinish или ArrayFinish путем всплывающего кадра.
            IterativeParsingState n = src;
            if (src == IterativeParsingArrayInitialState || src == IterativeParsingElementDelimiterState)
                n = IterativeParsingElementState;
            else if (src == IterativeParsingKeyValueDelimiterState)
                n = IterativeParsingMemberValueState;
            // Нажмите текущее состояние.
            *stack_.template Push<SizeType>(1) = n;
            // Инициализируйте и нажмите количество членов/элементов.
            *stack_.template Push<SizeType>(1) = 0;
            // Обработчик вызовов
            bool hr = (dst == IterativeParsingObjectInitialState) ? handler.StartObject() : handler.StartArray();
            // Обработчик прерывает синтаксический анализ.
            if (!hr) {
                RAPIDJSON_PARSE_ERROR_NORETURN(kParseErrorTermination, is.Tell());
                return IterativeParsingErrorState;
            }
            else {
                is.Take();
                return dst;
            }
        }

        case IterativeParsingMemberKeyState:
            ParseString<parseFlags>(is, handler, true);
            if (HasParseError())
                return IterativeParsingErrorState;
            else
                return dst;

        case IterativeParsingKeyValueDelimiterState:
            RAPIDJSON_ASSERT(token == ColonToken);
            is.Take();
            return dst;

        case IterativeParsingMemberValueState:
            // Должно быть несоставное значение. Или это будет состояние ObjectInitial или ArrayInitial.
            ParseValue<parseFlags>(is, handler);
            if (HasParseError()) {
                return IterativeParsingErrorState;
            }
            return dst;

        case IterativeParsingElementState:
            // Должно быть несоставное значение. Или это будет состояние ObjectInitial или ArrayInitial.
            ParseValue<parseFlags>(is, handler);
            if (HasParseError()) {
                return IterativeParsingErrorState;
            }
            return dst;

        case IterativeParsingMemberDelimiterState:
        case IterativeParsingElementDelimiterState:
            is.Take();
            // Обновить количество участников/элементов.
            *stack_.template Top<SizeType>() = *stack_.template Top<SizeType>() + 1;
            return dst;

        case IterativeParsingObjectFinishState:
        {
            // Переход от разделителя разрешен только в том случае, если включены конечные запятые.
            if (!(parseFlags & kParseTrailingCommasFlag) && src == IterativeParsingMemberDelimiterState) {
                RAPIDJSON_PARSE_ERROR_NORETURN(kParseErrorObjectMissName, is.Tell());
                return IterativeParsingErrorState;
            }
            // Получите количество участников.
            SizeType c = *stack_.template Pop<SizeType>(1);
            // Если объект не пуст, подсчитайте последний член.
            if (src == IterativeParsingMemberValueState)
                ++c;
            // Восстановить штат.
            IterativeParsingState n = static_cast<IterativeParsingState>(*stack_.template Pop<SizeType>(1));
            // Переход в состояние завершения, если это самая верхняя область.
            if (n == IterativeParsingStartState)
                n = IterativeParsingFinishState;
            // Обработчик вызовов
            bool hr = handler.EndObject(c);
            // Обработчик прерывает синтаксический анализ.
            if (!hr) {
                RAPIDJSON_PARSE_ERROR_NORETURN(kParseErrorTermination, is.Tell());
                return IterativeParsingErrorState;
            }
            else {
                is.Take();
                return n;
            }
        }

        case IterativeParsingArrayFinishState:
        {
            // Переход от разделителя разрешен только в том случае, если включены конечные запятые.
            if (!(parseFlags & kParseTrailingCommasFlag) && src == IterativeParsingElementDelimiterState) {
                RAPIDJSON_PARSE_ERROR_NORETURN(kParseErrorValueInvalid, is.Tell());
                return IterativeParsingErrorState;
            }
            // Получить количество элементов.
            SizeType c = *stack_.template Pop<SizeType>(1);
            // Если массив не пуст, подсчитайте последний элемент.
            if (src == IterativeParsingElementState)
                ++c;
            // Восстановить штат.
            IterativeParsingState n = static_cast<IterativeParsingState>(*stack_.template Pop<SizeType>(1));
            // Переход в состояние завершения, если это самая верхняя область.
            if (n == IterativeParsingStartState)
                n = IterativeParsingFinishState;
            // Обработчик вызовов
            bool hr = handler.EndArray(c);
            // Обработчик прерывает синтаксический анализ.
            if (!hr) {
                RAPIDJSON_PARSE_ERROR_NORETURN(kParseErrorTermination, is.Tell());
                return IterativeParsingErrorState;
            }
            else {
                is.Take();
                return n;
            }
        }

        default:
            // На самом деле эта ветка предназначена для IterativeParsingValueState.
            // Используйте `default:` вместо
            // `case IterativeParsingValueState:` предназначен для покрытия кода.

            // IterativeParsingStartState не перечисляется в этом случае переключения.
            // Для такого случая это невозможно. И это можно уловить следующим утверждением.

            // В этом случае переключения IterativeParsingFinishState также не перечисляется.
            // Это «производное» состояние, которое не может быть вызвано напрямую из Predict().
            // Поэтому здесь этого произойти не может. И это можно уловить следующим утверждением.
            RAPIDJSON_ASSERT(dst == IterativeParsingValueState);

            // Должно быть несоставное значение. Или это будет состояние ObjectInitial или ArrayInitial.
            ParseValue<parseFlags>(is, handler);
            if (HasParseError()) {
                return IterativeParsingErrorState;
            }
            return IterativeParsingFinishState;
        }
    }

    template <typename InputStream>
    void HandleError(IterativeParsingState src, InputStream& is) {
        if (HasParseError()) {
            // Установлен флаг ошибки.
            return;
        }

        switch (src) {
        case IterativeParsingStartState:            RAPIDJSON_PARSE_ERROR(kParseErrorDocumentEmpty, is.Tell()); return;
        case IterativeParsingFinishState:           RAPIDJSON_PARSE_ERROR(kParseErrorDocumentRootNotSingular, is.Tell()); return;
        case IterativeParsingObjectInitialState:
        case IterativeParsingMemberDelimiterState:  RAPIDJSON_PARSE_ERROR(kParseErrorObjectMissName, is.Tell()); return;
        case IterativeParsingMemberKeyState:        RAPIDJSON_PARSE_ERROR(kParseErrorObjectMissColon, is.Tell()); return;
        case IterativeParsingMemberValueState:      RAPIDJSON_PARSE_ERROR(kParseErrorObjectMissCommaOrCurlyBracket, is.Tell()); return;
        case IterativeParsingKeyValueDelimiterState:
        case IterativeParsingArrayInitialState:
        case IterativeParsingElementDelimiterState: RAPIDJSON_PARSE_ERROR(kParseErrorValueInvalid, is.Tell()); return;
        default: RAPIDJSON_ASSERT(src == IterativeParsingElementState); RAPIDJSON_PARSE_ERROR(kParseErrorArrayMissCommaOrSquareBracket, is.Tell()); return;
        }
    }

    RAPIDJSON_FORCEINLINE bool IsIterativeParsingDelimiterState(IterativeParsingState s) const {
        return s >= IterativeParsingElementDelimiterState;
    }

    RAPIDJSON_FORCEINLINE bool IsIterativeParsingCompleteState(IterativeParsingState s) const {
        return s <= IterativeParsingErrorState;
    }

    template <unsigned parseFlags, typename InputStream, typename Handler>
    ParseResult IterativeParse(InputStream& is, Handler& handler) {
        parseResult_.Clear();
        ClearStackOnExit scope(*this);
        IterativeParsingState state = IterativeParsingStartState;

        SkipWhitespaceAndComments<parseFlags>(is);
        RAPIDJSON_PARSE_ERROR_EARLY_RETURN(parseResult_);
        while (is.Peek() != '\0') {
            Token t = Tokenize(is.Peek());
            IterativeParsingState n = Predict(state, t);
            IterativeParsingState d = Transit<parseFlags>(state, t, n, is, handler);

            if (d == IterativeParsingErrorState) {
                HandleError(state, is);
                break;
            }

            state = d;

            // Не используйте потоки дальше, если был проанализирован корневой JSON.
            if ((parseFlags & kParseStopWhenDoneFlag) && state == IterativeParsingFinishState)
                break;

            SkipWhitespaceAndComments<parseFlags>(is);
            RAPIDJSON_PARSE_ERROR_EARLY_RETURN(parseResult_);
        }

        // Обработать конец файла.
        if (state != IterativeParsingFinishState)
            HandleError(state, is);

        return parseResult_;
    }

    static const size_t kDefaultStackCapacity = 256;    //!< Емкость стека по умолчанию в байтах для хранения одной декодированной строки.
    internal::Stack<StackAllocator> stack_;  //!< Стек для временного хранения декодированной строки во время неразрушающего анализа.
    ParseResult parseResult_;
    IterativeParsingState state_;
}; // класс GenericReader

//! Читатель с кодировкой UTF8 и распределителем по умолчанию.
typedef GenericReader<UTF8<>, UTF8<> > Reader;

RAPIDJSON_NAMESPACE_END

#if defined(__clang__) || defined(_MSC_VER)
RAPIDJSON_DIAG_POP
#endif


#ifdef __GNUC__
RAPIDJSON_DIAG_POP
#endif

#endif // RAPIDJSON_READER_H_
