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

#ifndef RAPIDJSON_ENCODINGS_H_
#define RAPIDJSON_ENCODINGS_H_

#include "rapidjson.h"

#if defined(_MSC_VER) && !defined(__clang__)
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(4244) // преобразование из «тип1» в «тип2», возможна потеря данных
RAPIDJSON_DIAG_OFF(4702)  // недостижимый код
#elif defined(__GNUC__)
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(effc++)
RAPIDJSON_DIAG_OFF(overflow)
#endif

RAPIDJSON_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
// Кодирование

/*! \class RapidJSON::Кодирование
    \brief Концепция кодирования символов Юникода.

\code
концепция Кодировка {
    имя типа Ch;    //! Тип персонажа. «Символ» на самом деле является единицей кода в определении Юникода.

    чтение {supportUnicode = 1}; // или 0, если не происходит юникод

    //! \brief Кодирование кодовой точки Unicode в выходном потоке.
    //! \param os Выходной поток.
    //! \param codepoint Код Юникода в зависимости от0x0до 0x10FFFF.
    шаблон<имя типа OutputStream>
    static void Encode (OutputStream& os, беззнаковый код);

    //! \brief Декодирование кодовой точки Unicode из входного потока.
    //! \param — входной поток.
    //! \param codepoint Вывод кодовой точки Юникода.
    //! \return true, если из потока можно декодировать действительный код.
    шаблон <имя типа InputStream>
    static bool Decode (InputStream& is, кодовая точка без знака*);

    //! \brief Проверка одного кода Юникода из закодированного потока.
    //! \param — входной поток для получения кода.
    //! \param os Вывод для копирования кода одной точки.
    //! \ возвращаем true, если оно действительно.
    //! \note Эта функция просто копирует кодовую точку, не декодируя ее.
    шаблон <имя типа InputStream, имя типа OutputStream>
    static bool Validate(InputStream& is, OutputStream& os);

    // Следующие функции работают с потоками байтов.

    //! Возьмите символ из входного потока байтов, пропустите BOM, если он существует.
    шаблон <имя типа InputByteStream>
    статический CharType TakeBOM (InputByteStream& is);

    //! Возьмите символ из входного потока байтов.
    шаблон <имя типа InputByteStream>
    статический Ch Take(InputByteStream& is);

    //! Поместите BOM в выходной поток байтов.
    шаблон <имя типа OutputByteStream>
    статическая неактивность PutBOM (OutputByteStream& os);

    //! Поместите символ в выходной поток байтов.
    шаблон <имя типа OutputByteStream>
    статическая сила Put (OutputByteStream& os, Ch c);
};
\endcode
*/

///////////////////////////////////////////////////////////////////////////////
// UTF8

//!  Кодировка UTF -8.
/*! http://en.wikipedia.org/wiki/UTF-8
    http://tools.ietf.org/html/rfc3629
    \tparam CharType Кодовая единица для хранения 8-битных данныхUTF-8. По умолчанию — символ.
    \note представляет код реализации
*/
template<typename CharType = char>
struct UTF8 {
    typedef CharType Ch;

    enum { supportUnicode = 1 };

    template<typename OutputStream>
    static void Encode(OutputStream& os, unsigned codepoint) {
        if (codepoint <= 0x7F)
            os.Put(static_cast<Ch>(codepoint & 0xFF));
        else if (codepoint <= 0x7FF) {
            os.Put(static_cast<Ch>(0xC0 | ((codepoint >> 6) & 0xFF)));
            os.Put(static_cast<Ch>(0x80 | ((codepoint & 0x3F))));
        }
        else if (codepoint <= 0xFFFF) {
            os.Put(static_cast<Ch>(0xE0 | ((codepoint >> 12) & 0xFF)));
            os.Put(static_cast<Ch>(0x80 | ((codepoint >> 6) & 0x3F)));
            os.Put(static_cast<Ch>(0x80 | (codepoint & 0x3F)));
        }
        else {
            RAPIDJSON_ASSERT(codepoint <= 0x10FFFF);
            os.Put(static_cast<Ch>(0xF0 | ((codepoint >> 18) & 0xFF)));
            os.Put(static_cast<Ch>(0x80 | ((codepoint >> 12) & 0x3F)));
            os.Put(static_cast<Ch>(0x80 | ((codepoint >> 6) & 0x3F)));
            os.Put(static_cast<Ch>(0x80 | (codepoint & 0x3F)));
        }
    }

    template<typename OutputStream>
    static void EncodeUnsafe(OutputStream& os, unsigned codepoint) {
        if (codepoint <= 0x7F)
            PutUnsafe(os, static_cast<Ch>(codepoint & 0xFF));
        else if (codepoint <= 0x7FF) {
            PutUnsafe(os, static_cast<Ch>(0xC0 | ((codepoint >> 6) & 0xFF)));
            PutUnsafe(os, static_cast<Ch>(0x80 | ((codepoint & 0x3F))));
        }
        else if (codepoint <= 0xFFFF) {
            PutUnsafe(os, static_cast<Ch>(0xE0 | ((codepoint >> 12) & 0xFF)));
            PutUnsafe(os, static_cast<Ch>(0x80 | ((codepoint >> 6) & 0x3F)));
            PutUnsafe(os, static_cast<Ch>(0x80 | (codepoint & 0x3F)));
        }
        else {
            RAPIDJSON_ASSERT(codepoint <= 0x10FFFF);
            PutUnsafe(os, static_cast<Ch>(0xF0 | ((codepoint >> 18) & 0xFF)));
            PutUnsafe(os, static_cast<Ch>(0x80 | ((codepoint >> 12) & 0x3F)));
            PutUnsafe(os, static_cast<Ch>(0x80 | ((codepoint >> 6) & 0x3F)));
            PutUnsafe(os, static_cast<Ch>(0x80 | (codepoint & 0x3F)));
        }
    }

    template <typename InputStream>
    static bool Decode(InputStream& is, unsigned* codepoint) {
#define RAPIDJSON_COPY() c = is.Take(); *codepoint = (*codepoint << 6) | (static_cast<unsigned char>(c) & 0x3Fu)
#define RAPIDJSON_TRANS(mask) result &= ((GetRange(static_cast<unsigned char>(c)) & mask) != 0)
#define RAPIDJSON_TAIL() RAPIDJSON_COPY(); RAPIDJSON_TRANS(0x70)
        typename InputStream::Ch c = is.Take();
        if (!(c & 0x80)) {
            *codepoint = static_cast<unsigned char>(c);
            return true;
        }

        unsigned char type = GetRange(static_cast<unsigned char>(c));
        if (type >= 32) {
            *codepoint = 0;
        } else {
            *codepoint = (0xFFu >> type) & static_cast<unsigned char>(c);
        }
        bool result = true;
        switch (type) {
        case 2: RAPIDJSON_TAIL(); return result;
        case 3: RAPIDJSON_TAIL(); RAPIDJSON_TAIL(); return result;
        case 4: RAPIDJSON_COPY(); RAPIDJSON_TRANS(0x50); RAPIDJSON_TAIL(); return result;
        case 5: RAPIDJSON_COPY(); RAPIDJSON_TRANS(0x10); RAPIDJSON_TAIL(); RAPIDJSON_TAIL(); return result;
        case 6: RAPIDJSON_TAIL(); RAPIDJSON_TAIL(); RAPIDJSON_TAIL(); return result;
        case 10: RAPIDJSON_COPY(); RAPIDJSON_TRANS(0x20); RAPIDJSON_TAIL(); return result;
        case 11: RAPIDJSON_COPY(); RAPIDJSON_TRANS(0x60); RAPIDJSON_TAIL(); RAPIDJSON_TAIL(); return result;
        default: return false;
        }
#undef RAPIDJSON_COPY
#undef RAPIDJSON_TRANS
#undef RAPIDJSON_TAIL
    }

    template <typename InputStream, typename OutputStream>
    static bool Validate(InputStream& is, OutputStream& os) {
#define RAPIDJSON_COPY() os.Put(c = is.Take())
#define RAPIDJSON_TRANS(mask) result &= ((GetRange(static_cast<unsigned char>(c)) & mask) != 0)
#define RAPIDJSON_TAIL() RAPIDJSON_COPY(); RAPIDJSON_TRANS(0x70)
        Ch c;
        RAPIDJSON_COPY();
        if (!(c & 0x80))
            return true;

        bool result = true;
        switch (GetRange(static_cast<unsigned char>(c))) {
        case 2: RAPIDJSON_TAIL(); return result;
        case 3: RAPIDJSON_TAIL(); RAPIDJSON_TAIL(); return result;
        case 4: RAPIDJSON_COPY(); RAPIDJSON_TRANS(0x50); RAPIDJSON_TAIL(); return result;
        case 5: RAPIDJSON_COPY(); RAPIDJSON_TRANS(0x10); RAPIDJSON_TAIL(); RAPIDJSON_TAIL(); return result;
        case 6: RAPIDJSON_TAIL(); RAPIDJSON_TAIL(); RAPIDJSON_TAIL(); return result;
        case 10: RAPIDJSON_COPY(); RAPIDJSON_TRANS(0x20); RAPIDJSON_TAIL(); return result;
        case 11: RAPIDJSON_COPY(); RAPIDJSON_TRANS(0x60); RAPIDJSON_TAIL(); RAPIDJSON_TAIL(); return result;
        default: return false;
        }
#undef RAPIDJSON_COPY
#undef RAPIDJSON_TRANS
#undef RAPIDJSON_TAIL
    }

    static unsigned char GetRange(unsigned char c) {
        // Ссылаясь наDFAиз http://bjoern.hoehrmann.de/utf-8/decoder/dfa/
        // С новым сопоставлением 1 -> 0x10 , 7 -> 0x20 , 9 -> 0x40 , так что операция AND может проверять несколько типов.
        static const unsigned char type[] = {
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
            0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
            0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
            0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
            8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
            10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,
        };
        return type[c];
    }

    template <typename InputByteStream>
    static CharType TakeBOM(InputByteStream& is) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename InputByteStream::Ch) == 1);
        typename InputByteStream::Ch c = Take(is);
        if (static_cast<unsigned char>(c) != 0xEFu) return c;
        c = is.Take();
        if (static_cast<unsigned char>(c) != 0xBBu) return c;
        c = is.Take();
        if (static_cast<unsigned char>(c) != 0xBFu) return c;
        c = is.Take();
        return c;
    }

    template <typename InputByteStream>
    static Ch Take(InputByteStream& is) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename InputByteStream::Ch) == 1);
        return static_cast<Ch>(is.Take());
    }

    template <typename OutputByteStream>
    static void PutBOM(OutputByteStream& os) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename OutputByteStream::Ch) == 1);
        os.Put(static_cast<typename OutputByteStream::Ch>(0xEFu));
        os.Put(static_cast<typename OutputByteStream::Ch>(0xBBu));
        os.Put(static_cast<typename OutputByteStream::Ch>(0xBFu));
    }

    template <typename OutputByteStream>
    static void Put(OutputByteStream& os, Ch c) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename OutputByteStream::Ch) == 1);
        os.Put(static_cast<typename OutputByteStream::Ch>(c));
    }
};

///////////////////////////////////////////////////////////////////////////////
// UTF16

//!  Кодировка UTF -16.
/*! http://en.wikipedia.org/wiki/UTF-16
    http://tools.ietf.org/html/rfc2781
    \tparam CharType Тип для хранения 16-битных данныхUTF-16. По умолчанию —wchar_t. Вместо этого C++11 может использоватьchar16_t.
    \note представляет код реализации

    \note Для доступа к памяти не нужно беспокоиться о порядке байтов. Кодовые значения кода и точки представляют собой порядок байтов CPU.
    Для потоковой передачи используйте UTF16LE и UTF16BE, которые обрабатывают порядок байтов.
*/
template<typename CharType = wchar_t>
struct UTF16 {
    typedef CharType Ch;
    RAPIDJSON_STATIC_ASSERT(sizeof(Ch) >= 2);

    enum { supportUnicode = 1 };

    template<typename OutputStream>
    static void Encode(OutputStream& os, unsigned codepoint) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename OutputStream::Ch) >= 2);
        if (codepoint <= 0xFFFF) {
            RAPIDJSON_ASSERT(codepoint < 0xD800 || codepoint > 0xDFFF); // Сама кодовая точка не может быть суррогатной парой.
            os.Put(static_cast<typename OutputStream::Ch>(codepoint));
        }
        else {
            RAPIDJSON_ASSERT(codepoint <= 0x10FFFF);
            unsigned v = codepoint - 0x10000;
            os.Put(static_cast<typename OutputStream::Ch>((v >> 10) | 0xD800));
            os.Put(static_cast<typename OutputStream::Ch>((v & 0x3FF) | 0xDC00));
        }
    }


    template<typename OutputStream>
    static void EncodeUnsafe(OutputStream& os, unsigned codepoint) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename OutputStream::Ch) >= 2);
        if (codepoint <= 0xFFFF) {
            RAPIDJSON_ASSERT(codepoint < 0xD800 || codepoint > 0xDFFF); // Сама кодовая точка не может быть суррогатной парой.
            PutUnsafe(os, static_cast<typename OutputStream::Ch>(codepoint));
        }
        else {
            RAPIDJSON_ASSERT(codepoint <= 0x10FFFF);
            unsigned v = codepoint - 0x10000;
            PutUnsafe(os, static_cast<typename OutputStream::Ch>((v >> 10) | 0xD800));
            PutUnsafe(os, static_cast<typename OutputStream::Ch>((v & 0x3FF) | 0xDC00));
        }
    }

    template <typename InputStream>
    static bool Decode(InputStream& is, unsigned* codepoint) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename InputStream::Ch) >= 2);
        typename InputStream::Ch c = is.Take();
        if (c < 0xD800 || c > 0xDFFF) {
            *codepoint = static_cast<unsigned>(c);
            return true;
        }
        else if (c <= 0xDBFF) {
            *codepoint = (static_cast<unsigned>(c) & 0x3FF) << 10;
            c = is.Take();
            *codepoint |= (static_cast<unsigned>(c) & 0x3FF);
            *codepoint += 0x10000;
            return c >= 0xDC00 && c <= 0xDFFF;
        }
        return false;
    }

    template <typename InputStream, typename OutputStream>
    static bool Validate(InputStream& is, OutputStream& os) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename InputStream::Ch) >= 2);
        RAPIDJSON_STATIC_ASSERT(sizeof(typename OutputStream::Ch) >= 2);
        typename InputStream::Ch c;
        os.Put(static_cast<typename OutputStream::Ch>(c = is.Take()));
        if (c < 0xD800 || c > 0xDFFF)
            return true;
        else if (c <= 0xDBFF) {
            os.Put(c = is.Take());
            return c >= 0xDC00 && c <= 0xDFFF;
        }
        return false;
    }
};

//!  UTF -16 кодировка с прямым порядком байтов.
template<typename CharType = wchar_t>
struct UTF16LE : UTF16<CharType> {
    template <typename InputByteStream>
    static CharType TakeBOM(InputByteStream& is) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename InputByteStream::Ch) == 1);
        CharType c = Take(is);
        return static_cast<uint16_t>(c) == 0xFEFFu ? Take(is) : c;
    }

    template <typename InputByteStream>
    static CharType Take(InputByteStream& is) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename InputByteStream::Ch) == 1);
        unsigned c = static_cast<uint8_t>(is.Take());
        c |= static_cast<unsigned>(static_cast<uint8_t>(is.Take())) << 8;
        return static_cast<CharType>(c);
    }

    template <typename OutputByteStream>
    static void PutBOM(OutputByteStream& os) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename OutputByteStream::Ch) == 1);
        os.Put(static_cast<typename OutputByteStream::Ch>(0xFFu));
        os.Put(static_cast<typename OutputByteStream::Ch>(0xFEu));
    }

    template <typename OutputByteStream>
    static void Put(OutputByteStream& os, CharType c) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename OutputByteStream::Ch) == 1);
        os.Put(static_cast<typename OutputByteStream::Ch>(static_cast<unsigned>(c) & 0xFFu));
        os.Put(static_cast<typename OutputByteStream::Ch>((static_cast<unsigned>(c) >> 8) & 0xFFu));
    }
};

//!  UTF -16 кодировка с прямым порядком байтов.
template<typename CharType = wchar_t>
struct UTF16BE : UTF16<CharType> {
    template <typename InputByteStream>
    static CharType TakeBOM(InputByteStream& is) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename InputByteStream::Ch) == 1);
        CharType c = Take(is);
        return static_cast<uint16_t>(c) == 0xFEFFu ? Take(is) : c;
    }

    template <typename InputByteStream>
    static CharType Take(InputByteStream& is) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename InputByteStream::Ch) == 1);
        unsigned c = static_cast<unsigned>(static_cast<uint8_t>(is.Take())) << 8;
        c |= static_cast<unsigned>(static_cast<uint8_t>(is.Take()));
        return static_cast<CharType>(c);
    }

    template <typename OutputByteStream>
    static void PutBOM(OutputByteStream& os) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename OutputByteStream::Ch) == 1);
        os.Put(static_cast<typename OutputByteStream::Ch>(0xFEu));
        os.Put(static_cast<typename OutputByteStream::Ch>(0xFFu));
    }

    template <typename OutputByteStream>
    static void Put(OutputByteStream& os, CharType c) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename OutputByteStream::Ch) == 1);
        os.Put(static_cast<typename OutputByteStream::Ch>((static_cast<unsigned>(c) >> 8) & 0xFFu));
        os.Put(static_cast<typename OutputByteStream::Ch>(static_cast<unsigned>(c) & 0xFFu));
    }
};

///////////////////////////////////////////////////////////////////////////////
// UTF32

//!  Кодировка UTF -32.
/*! http://en.wikipedia.org/wiki/UTF-32
    \tparam CharType Тип для хранения 32-битных данныхUTF-32. По умолчанию без знака. Вместо этого C++11 может использоватьchar32_t.
    \note представляет код реализации

    \note Для доступа к памяти не нужно беспокоиться о порядке байтов. Кодовые значения кода и точки представляют собой порядок байтов CPU.
    Для потоковой передачи используйте UTF32LE и UTF32BE, которые обрабатывают порядок байтов.
*/
template<typename CharType = unsigned>
struct UTF32 {
    typedef CharType Ch;
    RAPIDJSON_STATIC_ASSERT(sizeof(Ch) >= 4);

    enum { supportUnicode = 1 };

    template<typename OutputStream>
    static void Encode(OutputStream& os, unsigned codepoint) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename OutputStream::Ch) >= 4);
        RAPIDJSON_ASSERT(codepoint <= 0x10FFFF);
        os.Put(codepoint);
    }

    template<typename OutputStream>
    static void EncodeUnsafe(OutputStream& os, unsigned codepoint) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename OutputStream::Ch) >= 4);
        RAPIDJSON_ASSERT(codepoint <= 0x10FFFF);
        PutUnsafe(os, codepoint);
    }

    template <typename InputStream>
    static bool Decode(InputStream& is, unsigned* codepoint) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename InputStream::Ch) >= 4);
        Ch c = is.Take();
        *codepoint = c;
        return c <= 0x10FFFF;
    }

    template <typename InputStream, typename OutputStream>
    static bool Validate(InputStream& is, OutputStream& os) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename InputStream::Ch) >= 4);
        Ch c;
        os.Put(c = is.Take());
        return c <= 0x10FFFF;
    }
};

//!  UTF -32 кодировка с прямым порядком байтов.
template<typename CharType = unsigned>
struct UTF32LE : UTF32<CharType> {
    template <typename InputByteStream>
    static CharType TakeBOM(InputByteStream& is) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename InputByteStream::Ch) == 1);
        CharType c = Take(is);
        return static_cast<uint32_t>(c) == 0x0000FEFFu ? Take(is) : c;
    }

    template <typename InputByteStream>
    static CharType Take(InputByteStream& is) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename InputByteStream::Ch) == 1);
        unsigned c = static_cast<uint8_t>(is.Take());
        c |= static_cast<unsigned>(static_cast<uint8_t>(is.Take())) << 8;
        c |= static_cast<unsigned>(static_cast<uint8_t>(is.Take())) << 16;
        c |= static_cast<unsigned>(static_cast<uint8_t>(is.Take())) << 24;
        return static_cast<CharType>(c);
    }

    template <typename OutputByteStream>
    static void PutBOM(OutputByteStream& os) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename OutputByteStream::Ch) == 1);
        os.Put(static_cast<typename OutputByteStream::Ch>(0xFFu));
        os.Put(static_cast<typename OutputByteStream::Ch>(0xFEu));
        os.Put(static_cast<typename OutputByteStream::Ch>(0x00u));
        os.Put(static_cast<typename OutputByteStream::Ch>(0x00u));
    }

    template <typename OutputByteStream>
    static void Put(OutputByteStream& os, CharType c) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename OutputByteStream::Ch) == 1);
        os.Put(static_cast<typename OutputByteStream::Ch>(c & 0xFFu));
        os.Put(static_cast<typename OutputByteStream::Ch>((c >> 8) & 0xFFu));
        os.Put(static_cast<typename OutputByteStream::Ch>((c >> 16) & 0xFFu));
        os.Put(static_cast<typename OutputByteStream::Ch>((c >> 24) & 0xFFu));
    }
};

//!  UTF -32 кодировка с прямым порядком байтов.
template<typename CharType = unsigned>
struct UTF32BE : UTF32<CharType> {
    template <typename InputByteStream>
    static CharType TakeBOM(InputByteStream& is) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename InputByteStream::Ch) == 1);
        CharType c = Take(is);
        return static_cast<uint32_t>(c) == 0x0000FEFFu ? Take(is) : c;
    }

    template <typename InputByteStream>
    static CharType Take(InputByteStream& is) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename InputByteStream::Ch) == 1);
        unsigned c = static_cast<unsigned>(static_cast<uint8_t>(is.Take())) << 24;
        c |= static_cast<unsigned>(static_cast<uint8_t>(is.Take())) << 16;
        c |= static_cast<unsigned>(static_cast<uint8_t>(is.Take())) << 8;
        c |= static_cast<unsigned>(static_cast<uint8_t>(is.Take()));
        return static_cast<CharType>(c);
    }

    template <typename OutputByteStream>
    static void PutBOM(OutputByteStream& os) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename OutputByteStream::Ch) == 1);
        os.Put(static_cast<typename OutputByteStream::Ch>(0x00u));
        os.Put(static_cast<typename OutputByteStream::Ch>(0x00u));
        os.Put(static_cast<typename OutputByteStream::Ch>(0xFEu));
        os.Put(static_cast<typename OutputByteStream::Ch>(0xFFu));
    }

    template <typename OutputByteStream>
    static void Put(OutputByteStream& os, CharType c) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename OutputByteStream::Ch) == 1);
        os.Put(static_cast<typename OutputByteStream::Ch>((c >> 24) & 0xFFu));
        os.Put(static_cast<typename OutputByteStream::Ch>((c >> 16) & 0xFFu));
        os.Put(static_cast<typename OutputByteStream::Ch>((c >> 8) & 0xFFu));
        os.Put(static_cast<typename OutputByteStream::Ch>(c & 0xFFu));
    }
};

///////////////////////////////////////////////////////////////////////////////
// ASCII

//!  Кодировка ASCII.
/*! http://en.wikipedia.org/wiki/ASCII
    \tparam CharType Кодовая единица для хранения 7-битных данныхASCII. По умолчанию — символ.
    \note представляет код реализации
*/
template<typename CharType = char>
struct ASCII {
    typedef CharType Ch;

    enum { supportUnicode = 0 };

    template<typename OutputStream>
    static void Encode(OutputStream& os, unsigned codepoint) {
        RAPIDJSON_ASSERT(codepoint <= 0x7F);
        os.Put(static_cast<Ch>(codepoint & 0xFF));
    }

    template<typename OutputStream>
    static void EncodeUnsafe(OutputStream& os, unsigned codepoint) {
        RAPIDJSON_ASSERT(codepoint <= 0x7F);
        PutUnsafe(os, static_cast<Ch>(codepoint & 0xFF));
    }

    template <typename InputStream>
    static bool Decode(InputStream& is, unsigned* codepoint) {
        uint8_t c = static_cast<uint8_t>(is.Take());
        *codepoint = c;
        return c <= 0X7F;
    }

    template <typename InputStream, typename OutputStream>
    static bool Validate(InputStream& is, OutputStream& os) {
        uint8_t c = static_cast<uint8_t>(is.Take());
        os.Put(static_cast<typename OutputStream::Ch>(c));
        return c <= 0x7F;
    }

    template <typename InputByteStream>
    static CharType TakeBOM(InputByteStream& is) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename InputByteStream::Ch) == 1);
        uint8_t c = static_cast<uint8_t>(Take(is));
        return static_cast<Ch>(c);
    }

    template <typename InputByteStream>
    static Ch Take(InputByteStream& is) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename InputByteStream::Ch) == 1);
        return static_cast<Ch>(is.Take());
    }

    template <typename OutputByteStream>
    static void PutBOM(OutputByteStream& os) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename OutputByteStream::Ch) == 1);
        (void)os;
    }

    template <typename OutputByteStream>
    static void Put(OutputByteStream& os, Ch c) {
        RAPIDJSON_STATIC_ASSERT(sizeof(typename OutputByteStream::Ch) == 1);
        os.Put(static_cast<typename OutputByteStream::Ch>(c));
    }
};

///////////////////////////////////////////////////////////////////////////////
// АвтоUTF

//! Тип кодировки потока UTF, указанный во время выполнения.
enum UTFType {
    kUTF8 = 0,      //!< UTF-8.
    kUTF16LE = 1,   //!< UTF -16 с прямым порядком байтов.
    kUTF16BE = 2,   //!< UTF -16 с прямым порядком байтов.
    kUTF32LE = 3,   //!< UTF -32 с прямым порядком байтов.
    kUTF32BE = 4    //!< UTF -32 с прямым порядком байтов.
};

//! Динамический выбор кодировки в соответствии с типом кодировки UTF, указанным во время выполнения потока.
/*! \note Этот класс можно использовать с AutoUTFInputStream и AutoUTFOutputStream, которые предоставляютGetType().
*/
template<typename CharType>
struct AutoUTF {
    typedef CharType Ch;

    enum { supportUnicode = 1 };

#define RAPIDJSON_ENCODINGS_FUNC(x) UTF8<Ch>::x, UTF16LE<Ch>::x, UTF16BE<Ch>::x, UTF32LE<Ch>::x, UTF32BE<Ch>::x

    template<typename OutputStream>
    static RAPIDJSON_FORCEINLINE void Encode(OutputStream& os, unsigned codepoint) {
        typedef void (*EncodeFunc)(OutputStream&, unsigned);
        static const EncodeFunc f[] = { RAPIDJSON_ENCODINGS_FUNC(Encode) };
        (*f[os.GetType()])(os, codepoint);
    }

    template<typename OutputStream>
    static RAPIDJSON_FORCEINLINE void EncodeUnsafe(OutputStream& os, unsigned codepoint) {
        typedef void (*EncodeFunc)(OutputStream&, unsigned);
        static const EncodeFunc f[] = { RAPIDJSON_ENCODINGS_FUNC(EncodeUnsafe) };
        (*f[os.GetType()])(os, codepoint);
    }

    template <typename InputStream>
    static RAPIDJSON_FORCEINLINE bool Decode(InputStream& is, unsigned* codepoint) {
        typedef bool (*DecodeFunc)(InputStream&, unsigned*);
        static const DecodeFunc f[] = { RAPIDJSON_ENCODINGS_FUNC(Decode) };
        return (*f[is.GetType()])(is, codepoint);
    }

    template <typename InputStream, typename OutputStream>
    static RAPIDJSON_FORCEINLINE bool Validate(InputStream& is, OutputStream& os) {
        typedef bool (*ValidateFunc)(InputStream&, OutputStream&);
        static const ValidateFunc f[] = { RAPIDJSON_ENCODINGS_FUNC(Validate) };
        return (*f[is.GetType()])(is, os);
    }

#undef RAPIDJSON_ENCODINGS_FUNC
};

///////////////////////////////////////////////////////////////////////////////
// Транскодер

//! Преобразование кодировки.
template<typename SourceEncoding, typename TargetEncoding>
struct Transcoder {
    //! Возьмите кодовую точку Unicode из исходной кодировки, преобразуйте ее в целевую кодировку и поместите в выходной поток.
    template<typename InputStream, typename OutputStream>
    static RAPIDJSON_FORCEINLINE bool Transcode(InputStream& is, OutputStream& os) {
        unsigned codepoint;
        if (!SourceEncoding::Decode(is, &codepoint))
            return false;
        TargetEncoding::Encode(os, codepoint);
        return true;
    }

    template<typename InputStream, typename OutputStream>
    static RAPIDJSON_FORCEINLINE bool TranscodeUnsafe(InputStream& is, OutputStream& os) {
        unsigned codepoint;
        if (!SourceEncoding::Decode(is, &codepoint))
            return false;
        TargetEncoding::EncodeUnsafe(os, codepoint);
        return true;
    }

    //! Проверьте один код Unicode из закодированного потока.
    template<typename InputStream, typename OutputStream>
    static RAPIDJSON_FORCEINLINE bool Validate(InputStream& is, OutputStream& os) {
        return Transcode(is, os);   // Поскольку исходная/целевая кодировка различается, необходимо перекодировать.
    }
};

// Форвардное заявление.
template<typename Stream>
inline void PutUnsafe(Stream& stream, typename Stream::Ch c);

//! Специализация транскодера с одинаковой исходной и целевой кодировкой.
template<typename Encoding>
struct Transcoder<Encoding, Encoding> {
    template<typename InputStream, typename OutputStream>
    static RAPIDJSON_FORCEINLINE bool Transcode(InputStream& is, OutputStream& os) {
        os.Put(is.Take());  // Просто скопируйте одну единицу кода. Эта семантика отличается от основного класса шаблона.
        return true;
    }

    template<typename InputStream, typename OutputStream>
    static RAPIDJSON_FORCEINLINE bool TranscodeUnsafe(InputStream& is, OutputStream& os) {
        PutUnsafe(os, is.Take());  // Просто скопируйте одну единицу кода. Эта семантика отличается от основного класса шаблона.
        return true;
    }

    template<typename InputStream, typename OutputStream>
    static RAPIDJSON_FORCEINLINE bool Validate(InputStream& is, OutputStream& os) {
        return Encoding::Validate(is, os);  // исходная/целевая кодировка одинаковы
    }
};

RAPIDJSON_NAMESPACE_END

#if defined(__GNUC__) || (defined(_MSC_VER) && !defined(__clang__))
RAPIDJSON_DIAG_POP
#endif

#endif // RAPIDJSON_ENCODINGS_H_
