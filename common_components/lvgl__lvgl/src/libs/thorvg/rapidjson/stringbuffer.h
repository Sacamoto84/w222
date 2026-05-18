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

#ifndef RAPIDJSON_STRINGBUFFER_H_
#define RAPIDJSON_STRINGBUFFER_H_

#include "stream.h"
#include "internal/stack.h"

#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
#include <utility> // станд::переместить
#endif

#include "internal/stack.h"

#if defined(__clang__)
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(c++98-compat)
#endif

RAPIDJSON_NAMESPACE_BEGIN

//! Представляет поток вывода в памяти.
/*!
    \tparam Кодирование Кодирование потока.
    \tparam Тип распределителя для выделения буфера памяти.
    \note реализации соглашения Stream
*/
template <typename Encoding, typename Allocator = CrtAllocator>
class GenericStringBuffer {
public:
    typedef typename Encoding::Ch Ch;

    GenericStringBuffer(Allocator* allocator = 0, size_t capacity = kDefaultCapacity) : stack_(allocator, capacity) {}

#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
    GenericStringBuffer(GenericStringBuffer&& rhs) : stack_(std::move(rhs.stack_)) {}
    GenericStringBuffer& operator=(GenericStringBuffer&& rhs) {
        if (&rhs != this)
            stack_ = std::move(rhs.stack_);
        return *this;
    }
#endif

    void Put(Ch c) { *stack_.template Push<Ch>() = c; }
    void PutUnsafe(Ch c) { *stack_.template PushUnsafe<Ch>() = c; }
    void Flush() {}

    void Clear() { stack_.Clear(); }
    void ShrinkToFit() {
        // Нажмите и вытолкните нулевой терминатор. Это безопасно.
        *stack_.template Push<Ch>() = '\0';
        stack_.ShrinkToFit();
        stack_.template Pop<Ch>(1);
    }

    void Reserve(size_t count) { stack_.template Reserve<Ch>(count); }
    Ch* Push(size_t count) { return stack_.template Push<Ch>(count); }
    Ch* PushUnsafe(size_t count) { return stack_.template PushUnsafe<Ch>(count); }
    void Pop(size_t count) { stack_.template Pop<Ch>(count); }

    const Ch* GetString() const {
        // Нажмите и вытолкните нулевой терминатор. Это безопасно.
        *stack_.template Push<Ch>() = '\0';
        stack_.template Pop<Ch>(1);

        return stack_.template Bottom<Ch>();
    }

    //! Получите размер строки в байтах в строковом буфере.
    size_t GetSize() const { return stack_.GetSize(); }

    //! Получите длину строки в Ch в строковом буфере.
    size_t GetLength() const { return stack_.GetSize() / sizeof(Ch); }

    static const size_t kDefaultCapacity = 256;
    mutable internal::Stack<Allocator> stack_;

private:
    // Запретить конструктор копирования и оператор присваивания.
    GenericStringBuffer(const GenericStringBuffer&);
    GenericStringBuffer& operator=(const GenericStringBuffer&);
};

//! Строковый буфер с кодировкой UTF8.
typedef GenericStringBuffer<UTF8<> > StringBuffer;

template<typename Encoding, typename Allocator>
inline void PutReserve(GenericStringBuffer<Encoding, Allocator>& stream, size_t count) {
    stream.Reserve(count);
}

template<typename Encoding, typename Allocator>
inline void PutUnsafe(GenericStringBuffer<Encoding, Allocator>& stream, typename Encoding::Ch c) {
    stream.PutUnsafe(c);
}

//! Внедрите специализированную версиюPutN()сmemset()для повышения производительности.
template<>
inline void PutN(GenericStringBuffer<UTF8<> >& stream, char c, size_t n) {
    std::memset(stream.stack_.Push<char>(n), c, n * sizeof(c));
}

RAPIDJSON_NAMESPACE_END

#if defined(__clang__)
RAPIDJSON_DIAG_POP
#endif

#endif // RAPIDJSON_STRINGBUFFER_H_
