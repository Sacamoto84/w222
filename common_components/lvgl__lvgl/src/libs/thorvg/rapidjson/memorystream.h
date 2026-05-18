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

#ifndef RAPIDJSON_MEMORYSTREAM_H_
#define RAPIDJSON_MEMORYSTREAM_H_

#include "stream.h"

#ifdef __clang__
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(unreachable-code)
RAPIDJSON_DIAG_OFF(missing-noreturn)
#endif

RAPIDJSON_NAMESPACE_BEGIN

//! Представляет поток входных байтов в памяти.
/*!
    Этот класс предназначен для обработки EncodedInputStream или AutoUTFInputStream.

    Это похоже на FileReadBuffer, но это буфер в памяти, а не файл.

    Получение между MemoryStream и StringStream:
    1. StringStream имеет кодировку, но MemoryStream представляет собой поток байтов.
    2. MemoryStream требуется размер исходного буфера, и буфер не обязательно должен иметь нулевое завершение. StringStream принимает в качестве источника строку, завершающуюся нулем.
    3. MemoryStream поддерживаетPeek4()для обнаружения кодировки. StringStream указан с кодировкой, поэтому он не должен иметь Peek4().
    \note реализации соглашения Stream
*/
struct MemoryStream {
    typedef char Ch; // байт

    MemoryStream(const Ch *src, size_t size) : src_(src), begin_(src), end_(src + size), size_(size) {}

    Ch Peek() const { return RAPIDJSON_UNLIKELY(src_ == end_) ? '\0' : *src_; }
    Ch Take() { return RAPIDJSON_UNLIKELY(src_ == end_) ? '\0' : *src_++; }
    size_t Tell() const { return static_cast<size_t>(src_ - begin_); }

    Ch* PutBegin() { RAPIDJSON_ASSERT(false); return 0; }
    void Put(Ch) { RAPIDJSON_ASSERT(false); }
    void Flush() { RAPIDJSON_ASSERT(false); }
    size_t PutEnd(Ch*) { RAPIDJSON_ASSERT(false); return 0; }

    // Только для обнаружения кодировки.
    const Ch* Peek4() const {
        return Tell() + 4 <= size_ ? src_ : 0;
    }

    const Ch* src_;     //!< Текущая позиция чтения.
    const Ch* begin_;   //!< Исходный заголовок строки.
    const Ch* end_;     //!< Конец потока.
    size_t size_;       //!< Размер потока.
};

RAPIDJSON_NAMESPACE_END

#ifdef __clang__
RAPIDJSON_DIAG_POP
#endif

#endif // RAPIDJSON_MEMORYBUFFER_H_
