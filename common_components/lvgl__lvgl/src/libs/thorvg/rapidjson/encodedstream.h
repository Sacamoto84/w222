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

#ifndef RAPIDJSON_ENCODEDSTREAM_H_
#define RAPIDJSON_ENCODEDSTREAM_H_

#include "stream.h"
#include "memorystream.h"

#ifdef __GNUC__
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(effc++)
#endif

#ifdef __clang__
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(padded)
#endif

RAPIDJSON_NAMESPACE_BEGIN

//! Оболочка входного потока байтов со статически связанной кодировкой.
/*!
    \tparam Кодирование Интерпретация кодирования потока. Либо UTF8, UTF16LE, UTF16BE, UTF32LE, UTF32BE.
    \tparam InputByteStream Тип входного потока байтов. Например, FileReadStream.
*/
template <typename Encoding, typename InputByteStream>
class EncodedInputStream {
    RAPIDJSON_STATIC_ASSERT(sizeof(typename InputByteStream::Ch) == 1);
public:
    typedef typename Encoding::Ch Ch;

    EncodedInputStream(InputByteStream& is) : is_(is) {
        current_ = Encoding::TakeBOM(is_);
    }

    Ch Peek() const { return current_; }
    Ch Take() { Ch c = current_; current_ = Encoding::Take(is_); return c; }
    size_t Tell() const { return is_.Tell(); }

    // Не реализовано
    void Put(Ch) { RAPIDJSON_ASSERT(false); }
    void Flush() { RAPIDJSON_ASSERT(false); }
    Ch* PutBegin() { RAPIDJSON_ASSERT(false); return 0; }
    size_t PutEnd(Ch*) { RAPIDJSON_ASSERT(false); return 0; }

private:
    EncodedInputStream(const EncodedInputStream&);
    EncodedInputStream& operator=(const EncodedInputStream&);

    InputByteStream& is_;
    Ch current_;
};

//! Специально для UTF8 MemoryStream.
template <>
class EncodedInputStream<UTF8<>, MemoryStream> {
public:
    typedef UTF8<>::Ch Ch;

    EncodedInputStream(MemoryStream& is) : is_(is) {
        if (static_cast<unsigned char>(is_.Peek()) == 0xEFu) is_.Take();
        if (static_cast<unsigned char>(is_.Peek()) == 0xBBu) is_.Take();
        if (static_cast<unsigned char>(is_.Peek()) == 0xBFu) is_.Take();
    }
    Ch Peek() const { return is_.Peek(); }
    Ch Take() { return is_.Take(); }
    size_t Tell() const { return is_.Tell(); }

    // Не реализовано
    void Put(Ch) {}
    void Flush() {}
    Ch* PutBegin() { return 0; }
    size_t PutEnd(Ch*) { return 0; }

    MemoryStream& is_;

private:
    EncodedInputStream(const EncodedInputStream&);
    EncodedInputStream& operator=(const EncodedInputStream&);
};

//! Оболочка выходного потока байтов со статически связанной кодировкой.
/*!
    \tparam Кодирование Интерпретация кодирования потока. Либо UTF8, UTF16LE, UTF16BE, UTF32LE, UTF32BE.
    \tparam OutputByteStream Тип входного потока байтов. Например, FileWriteStream.
*/
template <typename Encoding, typename OutputByteStream>
class EncodedOutputStream {
    RAPIDJSON_STATIC_ASSERT(sizeof(typename OutputByteStream::Ch) == 1);
public:
    typedef typename Encoding::Ch Ch;

    EncodedOutputStream(OutputByteStream& os, bool putBOM = true) : os_(os) {
        if (putBOM)
            Encoding::PutBOM(os_);
    }

    void Put(Ch c) { Encoding::Put(os_, c);  }
    void Flush() { os_.Flush(); }

    // Не реализовано
    Ch Peek() const { RAPIDJSON_ASSERT(false); return 0;}
    Ch Take() { RAPIDJSON_ASSERT(false); return 0;}
    size_t Tell() const { RAPIDJSON_ASSERT(false);  return 0; }
    Ch* PutBegin() { RAPIDJSON_ASSERT(false); return 0; }
    size_t PutEnd(Ch*) { RAPIDJSON_ASSERT(false); return 0; }

private:
    EncodedOutputStream(const EncodedOutputStream&);
    EncodedOutputStream& operator=(const EncodedOutputStream&);

    OutputByteStream& os_;
};

#define RAPIDJSON_ENCODINGS_FUNC(x) UTF8<Ch>::x, UTF16LE<Ch>::x, UTF16BE<Ch>::x, UTF32LE<Ch>::x, UTF32BE<Ch>::x

//! Оболочка входного потока с динамически привязанной кодировкой и автоматическим определением кодировки.
/*!
    \tparam CharType Тип символа для чтения.
    \tparam InputByteStream тип входного потока байтов, который необходимо обернуть.
*/
template <typename CharType, typename InputByteStream>
class AutoUTFInputStream {
    RAPIDJSON_STATIC_ASSERT(sizeof(typename InputByteStream::Ch) == 1);
public:
    typedef CharType Ch;

    //! Конструктор.
    /*!
        \param — входной поток, который нужно обернуть.
        \param type Тип кодировки UTF, если он не обнаружен в потоке.
    */
    AutoUTFInputStream(InputByteStream& is, UTFType type = kUTF8) : is_(&is), type_(type), hasBOM_(false) {
        RAPIDJSON_ASSERT(type >= kUTF8 && type <= kUTF32BE);
        DetectType();
        static const TakeFunc f[] = { RAPIDJSON_ENCODINGS_FUNC(Take) };
        takeFunc_ = f[type_];
        current_ = takeFunc_(*is_);
    }

    UTFType GetType() const { return type_; }
    bool HasBOM() const { return hasBOM_; }

    Ch Peek() const { return current_; }
    Ch Take() { Ch c = current_; current_ = takeFunc_(*is_); return c; }
    size_t Tell() const { return is_->Tell(); }

    // Не реализовано
    void Put(Ch) { RAPIDJSON_ASSERT(false); }
    void Flush() { RAPIDJSON_ASSERT(false); }
    Ch* PutBegin() { RAPIDJSON_ASSERT(false); return 0; }
    size_t PutEnd(Ch*) { RAPIDJSON_ASSERT(false); return 0; }

private:
    AutoUTFInputStream(const AutoUTFInputStream&);
    AutoUTFInputStream& operator=(const AutoUTFInputStream&);

    // Определите тип кодировки с помощью BOM или RFC 4627.
    void DetectType() {
        // BOM (метка порядка байтов):
        // 00 00 FE FF  UTF-32BE
        // FF FE 00 00  UTF-32LE
        // FE FF        UTF-16BE
        // FF FE        UTF-16LE
        // EF BB BF     UTF-8

        const unsigned char* c = reinterpret_cast<const unsigned char *>(is_->Peek4());
        if (!c)
            return;

        unsigned bom = static_cast<unsigned>(c[0] | (c[1] << 8) | (c[2] << 16) | (c[3] << 24));
        hasBOM_ = false;
        if (bom == 0xFFFE0000)                  { type_ = kUTF32BE; hasBOM_ = true; is_->Take(); is_->Take(); is_->Take(); is_->Take(); }
        else if (bom == 0x0000FEFF)             { type_ = kUTF32LE; hasBOM_ = true; is_->Take(); is_->Take(); is_->Take(); is_->Take(); }
        else if ((bom & 0xFFFF) == 0xFFFE)      { type_ = kUTF16BE; hasBOM_ = true; is_->Take(); is_->Take();                           }
        else if ((bom & 0xFFFF) == 0xFEFF)      { type_ = kUTF16LE; hasBOM_ = true; is_->Take(); is_->Take();                           }
        else if ((bom & 0xFFFFFF) == 0xBFBBEF)  { type_ = kUTF8;    hasBOM_ = true; is_->Take(); is_->Take(); is_->Take();              }

        // RFC 4627: Раздел 3
        // «Поскольку первые два символа текста JSON всегда будут ASCII
        // символов [ RFC0020 ], можно определить, является ли октет
        // поток: UTF -8, UTF -16 ( BE или LE ) или UTF -32 ( BE или LE ), посмотрев
        // по шаблону нулей в первых четырех октетах».
        // 00 00 00 xx  UTF-32BE
        // 00 xx 00 xx  UTF-16BE
        // xx 00 00 00  UTF-32LE
        // xx 00 xx 00  UTF-16LE
        // xx xx xx xx  UTF-8

        if (!hasBOM_) {
            int pattern = (c[0] ? 1 : 0) | (c[1] ? 2 : 0) | (c[2] ? 4 : 0) | (c[3] ? 8 : 0);
            switch (pattern) {
            case 0x08: type_ = kUTF32BE; break;
            case 0x0A: type_ = kUTF16BE; break;
            case 0x01: type_ = kUTF32LE; break;
            case 0x05: type_ = kUTF16LE; break;
            case 0x0F: type_ = kUTF8;    break;
            default: break; // Тип использования, определенный пользователем.
            }
        }

        // Во время выполнения проверьте, достаточен ли размер символьного типа. Он выполняет только проверки с утверждением.
        if (type_ == kUTF16LE || type_ == kUTF16BE) RAPIDJSON_ASSERT(sizeof(Ch) >= 2);
        if (type_ == kUTF32LE || type_ == kUTF32BE) RAPIDJSON_ASSERT(sizeof(Ch) >= 4);
    }

    typedef Ch (*TakeFunc)(InputByteStream& is);
    InputByteStream* is_;
    UTFType type_;
    Ch current_;
    TakeFunc takeFunc_;
    bool hasBOM_;
};

//! Оболочка выходного потока с динамически привязанной кодировкой и автоматическим обнаружением кодировки.
/*!
    \tparam CharType Тип символа для записи.
    \tparam OutputByteStream тип выходного потока байтов, который необходимо обернуть.
*/
template <typename CharType, typename OutputByteStream>
class AutoUTFOutputStream {
    RAPIDJSON_STATIC_ASSERT(sizeof(typename OutputByteStream::Ch) == 1);
public:
    typedef CharType Ch;

    //! Конструктор.
    /*!
        \param os выходной поток, который нужно обернуть.
        \param type Тип кодировки UTF.
        \param putBOM Записывать ли BOM в начале потока.
    */
    AutoUTFOutputStream(OutputByteStream& os, UTFType type, bool putBOM) : os_(&os), type_(type) {
        RAPIDJSON_ASSERT(type >= kUTF8 && type <= kUTF32BE);

        // Во время выполнения проверьте, достаточен ли размер символьного типа. Он выполняет только проверки с утверждением.
        if (type_ == kUTF16LE || type_ == kUTF16BE) RAPIDJSON_ASSERT(sizeof(Ch) >= 2);
        if (type_ == kUTF32LE || type_ == kUTF32BE) RAPIDJSON_ASSERT(sizeof(Ch) >= 4);

        static const PutFunc f[] = { RAPIDJSON_ENCODINGS_FUNC(Put) };
        putFunc_ = f[type_];

        if (putBOM)
            PutBOM();
    }

    UTFType GetType() const { return type_; }

    void Put(Ch c) { putFunc_(*os_, c); }
    void Flush() { os_->Flush(); }

    // Не реализовано
    Ch Peek() const { RAPIDJSON_ASSERT(false); return 0;}
    Ch Take() { RAPIDJSON_ASSERT(false); return 0;}
    size_t Tell() const { RAPIDJSON_ASSERT(false); return 0; }
    Ch* PutBegin() { RAPIDJSON_ASSERT(false); return 0; }
    size_t PutEnd(Ch*) { RAPIDJSON_ASSERT(false); return 0; }

private:
    AutoUTFOutputStream(const AutoUTFOutputStream&);
    AutoUTFOutputStream& operator=(const AutoUTFOutputStream&);

    void PutBOM() {
        typedef void (*PutBOMFunc)(OutputByteStream&);
        static const PutBOMFunc f[] = { RAPIDJSON_ENCODINGS_FUNC(PutBOM) };
        f[type_](*os_);
    }

    typedef void (*PutFunc)(OutputByteStream&, Ch);

    OutputByteStream* os_;
    UTFType type_;
    PutFunc putFunc_;
};

#undef RAPIDJSON_ENCODINGS_FUNC

RAPIDJSON_NAMESPACE_END

#ifdef __clang__
RAPIDJSON_DIAG_POP
#endif

#ifdef __GNUC__
RAPIDJSON_DIAG_POP
#endif

#endif // RAPIDJSON_FILESTREAM_H_
