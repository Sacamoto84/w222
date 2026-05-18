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

#ifndef RAPIDJSON_INTERNAL_STACK_H_
#define RAPIDJSON_INTERNAL_STACK_H_

#include "../allocators.h"
#include "swap.h"
#include <cstddef>

#if defined(__clang__)
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(c++98-compat)
#endif

RAPIDJSON_NAMESPACE_BEGIN
namespace internal {

///////////////////////////////////////////////////////////////////////////////
// Стек

//! Небезопасный по типам стек для хранения различных типов данных.
/*! \tparam Распределитель Распределитель для выделения памяти стека.
*/
template <typename Allocator>
class Stack {
public:
    // Примечание по оптимизации: не выделяйте память для stack_ в конструкторе.
    // Делайте это лениво, когда сначала Push() -> Expand() -> Resize() .
    Stack(Allocator* allocator, size_t stackCapacity) : allocator_(allocator), ownAllocator_(0), stack_(0), stackTop_(0), stackEnd_(0), initialCapacity_(stackCapacity) {
    }

#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
    Stack(Stack&& rhs)
        : allocator_(rhs.allocator_),
          ownAllocator_(rhs.ownAllocator_),
          stack_(rhs.stack_),
          stackTop_(rhs.stackTop_),
          stackEnd_(rhs.stackEnd_),
          initialCapacity_(rhs.initialCapacity_)
    {
        rhs.allocator_ = 0;
        rhs.ownAllocator_ = 0;
        rhs.stack_ = 0;
        rhs.stackTop_ = 0;
        rhs.stackEnd_ = 0;
        rhs.initialCapacity_ = 0;
    }
#endif

    ~Stack() {
        Destroy();
    }

#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
    Stack& operator=(Stack&& rhs) {
        if (&rhs != this)
        {
            Destroy();

            allocator_ = rhs.allocator_;
            ownAllocator_ = rhs.ownAllocator_;
            stack_ = rhs.stack_;
            stackTop_ = rhs.stackTop_;
            stackEnd_ = rhs.stackEnd_;
            initialCapacity_ = rhs.initialCapacity_;

            rhs.allocator_ = 0;
            rhs.ownAllocator_ = 0;
            rhs.stack_ = 0;
            rhs.stackTop_ = 0;
            rhs.stackEnd_ = 0;
            rhs.initialCapacity_ = 0;
        }
        return *this;
    }
#endif

    void Swap(Stack& rhs) RAPIDJSON_NOEXCEPT {
        internal::Swap(allocator_, rhs.allocator_);
        internal::Swap(ownAllocator_, rhs.ownAllocator_);
        internal::Swap(stack_, rhs.stack_);
        internal::Swap(stackTop_, rhs.stackTop_);
        internal::Swap(stackEnd_, rhs.stackEnd_);
        internal::Swap(initialCapacity_, rhs.initialCapacity_);
    }

    void Clear() { stackTop_ = stack_; }

    void ShrinkToFit() {
        if (Empty()) {
            // Если стек пуст, полностью освободите память.
            Allocator::Free(stack_); // NOLINT (+clang-analyzer-unix.Malloc)
            stack_ = 0;
            stackTop_ = 0;
            stackEnd_ = 0;
        }
        else
            Resize(GetSize());
    }

    // Примечание по оптимизации: постарайтесь минимизировать размер этой функции для принудительного встроенного выполнения.
    // Расширение выполняется очень редко, поэтому оно переносится в другую (вероятно, не встроенную) функцию.
    template<typename T>
    RAPIDJSON_FORCEINLINE void Reserve(size_t count = 1) {
         // Расширьте стек при необходимости
        if (RAPIDJSON_UNLIKELY(static_cast<std::ptrdiff_t>(sizeof(T) * count) > (stackEnd_ - stackTop_)))
            Expand<T>(count);
    }

    template<typename T>
    RAPIDJSON_FORCEINLINE T* Push(size_t count = 1) {
        Reserve<T>(count);
        return PushUnsafe<T>(count);
    }

    template<typename T>
    RAPIDJSON_FORCEINLINE T* PushUnsafe(size_t count = 1) {
        RAPIDJSON_ASSERT(stackTop_);
        RAPIDJSON_ASSERT(static_cast<std::ptrdiff_t>(sizeof(T) * count) <= (stackEnd_ - stackTop_));
        T* ret = reinterpret_cast<T*>(stackTop_);
        stackTop_ += sizeof(T) * count;
        return ret;
    }

    template<typename T>
    T* Pop(size_t count) {
        RAPIDJSON_ASSERT(GetSize() >= count * sizeof(T));
        stackTop_ -= count * sizeof(T);
        return reinterpret_cast<T*>(stackTop_);
    }

    template<typename T>
    T* Top() {
        RAPIDJSON_ASSERT(GetSize() >= sizeof(T));
        return reinterpret_cast<T*>(stackTop_ - sizeof(T));
    }

    template<typename T>
    const T* Top() const {
        RAPIDJSON_ASSERT(GetSize() >= sizeof(T));
        return reinterpret_cast<T*>(stackTop_ - sizeof(T));
    }

    template<typename T>
    T* End() { return reinterpret_cast<T*>(stackTop_); }

    template<typename T>
    const T* End() const { return reinterpret_cast<T*>(stackTop_); }

    template<typename T>
    T* Bottom() { return reinterpret_cast<T*>(stack_); }

    template<typename T>
    const T* Bottom() const { return reinterpret_cast<T*>(stack_); }

    bool HasAllocator() const {
        return allocator_ != 0;
    }

    Allocator& GetAllocator() {
        RAPIDJSON_ASSERT(allocator_);
        return *allocator_;
    }

    bool Empty() const { return stackTop_ == stack_; }
    size_t GetSize() const { return static_cast<size_t>(stackTop_ - stack_); }
    size_t GetCapacity() const { return static_cast<size_t>(stackEnd_ - stack_); }

private:
    template<typename T>
    void Expand(size_t count) {
        // Расширяйте емкость только в том случае, если текущий стек существует. В противном случае просто создайте стек с начальной емкостью.
        size_t newCapacity;
        if (stack_ == 0) {
            if (!allocator_)
                ownAllocator_ = allocator_ = RAPIDJSON_NEW(Allocator)();
            newCapacity = initialCapacity_;
        } else {
            newCapacity = GetCapacity();
            newCapacity += (newCapacity + 1) / 2;
        }
        size_t newSize = GetSize() + sizeof(T) * count;
        if (newCapacity < newSize)
            newCapacity = newSize;

        Resize(newCapacity);
    }

    void Resize(size_t newCapacity) {
        const size_t size = GetSize();  // Резервная копия текущего размера
        stack_ = static_cast<char*>(allocator_->Realloc(stack_, GetCapacity(), newCapacity));
        stackTop_ = stack_ + size;
        stackEnd_ = stack_ + newCapacity;
    }

    void Destroy() {
        Allocator::Free(stack_);
        RAPIDJSON_DELETE(ownAllocator_); // Удалить только в том случае, если он принадлежит стеку
    }

    // Запретить конструктор копирования и оператор присваивания.
    Stack(const Stack&);
    Stack& operator=(const Stack&);

    Allocator* allocator_;
    Allocator* ownAllocator_;
    char *stack_;
    char *stackTop_;
    char *stackEnd_;
    size_t initialCapacity_;
};

} // внутреннее пространство имен
RAPIDJSON_NAMESPACE_END

#if defined(__clang__)
RAPIDJSON_DIAG_POP
#endif

#endif // RAPIDJSON_STACK_H_
