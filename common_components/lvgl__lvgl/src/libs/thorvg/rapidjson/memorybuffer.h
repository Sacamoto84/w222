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

#ifndef RAPIDJSON_MEMORYBUFFER_H_
#define RAPIDJSON_MEMORYBUFFER_H_

#include "stream.h"
#include "internal/stack.h"

RAPIDJSON_NAMESPACE_BEGIN

//! Представляет поток выходных байтов в памяти.
/*!
    Этот класс предназначен для обработки EncodedOutputStream или AutoUTFOutputStream.

    Это похоже на FileWriteBuffer, но определением является буфер в памяти, а не файл.

    Получения между MemoryBuffer и StringBuffer:
    1. StringBuffer имеет кодировку, но MemoryBuffer — это всего лишь байтовый буфер.
    2. StringBuffer::GetString() возвращает строку, завершающуюся нулем. MemoryBuffer::GetBuffer() возвращает буфер без терминатора.

    \tparam Тип распределителя для выделения буфера памяти.
    \note реализации соглашения Stream
*/
template <typename Allocator = CrtAllocator>
struct GenericMemoryBuffer {
    typedef char Ch; // байт

    GenericMemoryBuffer(Allocator* allocator = 0, size_t capacity = kDefaultCapacity) : stack_(allocator, capacity) {}

    void Put(Ch c) { *stack_.template Push<Ch>() = c; }
    void Flush() {}

    void Clear() { stack_.Clear(); }
    void ShrinkToFit() { stack_.ShrinkToFit(); }
    Ch* Push(size_t count) { return stack_.template Push<Ch>(count); }
    void Pop(size_t count) { stack_.template Pop<Ch>(count); }

    const Ch* GetBuffer() const {
        return stack_.template Bottom<Ch>();
    }

    size_t GetSize() const { return stack_.GetSize(); }

    static const size_t kDefaultCapacity = 256;
    mutable internal::Stack<Allocator> stack_;
};

typedef GenericMemoryBuffer<> MemoryBuffer;

//! Внедрите специализированную версиюPutN()сmemset()для повышения производительности.
template<>
inline void PutN(MemoryBuffer& memoryBuffer, char c, size_t n) {
    std::memset(memoryBuffer.stack_.Push<char>(n), c, n * sizeof(c));
}

RAPIDJSON_NAMESPACE_END

#endif // RAPIDJSON_MEMORYBUFFER_H_
