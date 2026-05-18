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

#ifndef RAPIDJSON_ALLOCATORS_H_
#define RAPIDJSON_ALLOCATORS_H_

#include "rapidjson.h"
#include "internal/meta.h"

#include <memory>
#include <limits>

#if RAPIDJSON_HAS_CXX11
#include <type_traits>
#endif

#include "../../../misc/lv_assert.h"

RAPIDJSON_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
// Распределитель

/*! \class RapidJSON::Распределитель
    \brief Концепция выделения, изменения размера и освобождения блока памяти.

    Обратите внимание, что Malloc() и Realloc() нестатические, а Free() — статические.

    Поэтому, если распределителю необходимо поддерживать Free(), ему необходимо поместить свой указатель в
    заголовок блока памяти.

\code
концепция Распределитель {
    static const bool kNeedFree;    //!< Нужно ли этому распределителю вызывать Free() .

    // Выделить блок памяти.
    // \param размер блока памяти в байтах.
    // \возвращает указатель на блок памяти.
    void* Malloc (размер size_t);

    // Изменение размера блока памяти.
    // \param originalPtr Указатель на текущий блок памяти. Нулевой указатель разрешен.
    // \param originalSize Текущий размер в байтах. (Проблема проектирования: поскольку какой-то распределитель может не учитывать это, явный переход к нему может сэкономить память.)
    // \param newSize новый размер в байтах.
    void* Realloc(void* originalPtr, size_t originalSize, size_t newSize);

    // Освободите блок памяти.
    // \param указатель на блок памяти. Нулевой указатель разрешен.
    статическая недействительность Free (void *ptr);
};
\endcode
*/


/*! \def RAPIDJSON_ALLOCATOR_DEFAULT_CHUNK_CAPACITY
    \ingroup RAPIDJSON_CONFIG
    \brief Пользовательское определение kDefaultChunkCapacity.

    Пользователь может определить это как любой размер \c, равный степени 2.
*/

#ifndef RAPIDJSON_ALLOCATOR_DEFAULT_CHUNK_CAPACITY
#define RAPIDJSON_ALLOCATOR_DEFAULT_CHUNK_CAPACITY (64 * 1024)
#endif


///////////////////////////////////////////////////////////////////////////////
// CrtAllocator

//! Распределитель библиотек времени выполнения C.
/*! Этот класс является просто оболочкой для стандартных процедур работы с памятью библиотеки C.
    \note реализует концепцию распределителя
*/
class CrtAllocator {
public:
    static const bool kNeedFree = true;
    void* Malloc(size_t size) {
        if (size) { //  поведение malloc(0) определяется реализацией.
            void * p = RAPIDJSON_MALLOC(size);
            LV_ASSERT_MALLOC(p);
            return p;
        } else {
            return NULL; // стандартизировать возврат NULL .
        }
    }
    void* Realloc(void* originalPtr, size_t originalSize, size_t newSize) {
        (void)originalSize;
        if (newSize == 0) {
            RAPIDJSON_FREE(originalPtr);
            return NULL;
        }
        void * p = RAPIDJSON_REALLOC(originalPtr, newSize);
        LV_ASSERT_MALLOC(p);
        return p;
    }
    static void Free(void *ptr) RAPIDJSON_NOEXCEPT { RAPIDJSON_FREE(ptr); }

    bool operator==(const CrtAllocator&) const RAPIDJSON_NOEXCEPT {
        return true;
    }
    bool operator!=(const CrtAllocator&) const RAPIDJSON_NOEXCEPT {
        return false;
    }
};

///////////////////////////////////////////////////////////////////////////////
// Распределитель пула памяти

//! Распределитель памяти по умолчанию, используемый анализатором и DOM.
/*! Этот распределитель выделяет блоки памяти из заранее выделенных фрагментов памяти.

    Он не освобождает блоки памяти. А Realloc() только выделяет новую память.

    Фрагменты памяти распределяются с помощью BaseAllocator, который по умолчанию является CrtAllocator.

    Пользователь также может предоставить буфер в качестве первого фрагмента.

    Если пользовательский буфер заполнен, BaseAllocator выделяет дополнительные фрагменты.

    Пользовательский буфер не освобождается этим распределителем.

    \tparam BaseAllocator тип распределителя для выделения фрагментов памяти. По умолчанию — CrtAllocator.
    \note реализует концепцию распределителя
*/
template <typename BaseAllocator = CrtAllocator>
class MemoryPoolAllocator {
    //! Заголовок чанка для каждого чанка.
    /*! Чанки хранятся в виде односвязного списка.
    */
    struct ChunkHeader {
        size_t capacity;    //!< Емкость чанка в байтах (исключая сам заголовок).
        size_t size;        //!< Текущий размер выделенной памяти в байтах.
        ChunkHeader *next;  //!< Следующий фрагмент в связанном списке.
    };

    struct SharedData {
        ChunkHeader *chunkHead;  //!< Глава связанного списка чанка. Только головной блок служит для распределения.
        BaseAllocator* ownBaseAllocator; //!< базовый распределитель, созданный этим объектом.
        size_t refcount;
        bool ownBuffer;
    };

    static const size_t SIZEOF_SHARED_DATA = RAPIDJSON_ALIGN(sizeof(SharedData));
    static const size_t SIZEOF_CHUNK_HEADER = RAPIDJSON_ALIGN(sizeof(ChunkHeader));

    static inline ChunkHeader *GetChunkHead(SharedData *shared)
    {
        return reinterpret_cast<ChunkHeader*>(reinterpret_cast<uint8_t*>(shared) + SIZEOF_SHARED_DATA);
    }
    static inline uint8_t *GetChunkBuffer(SharedData *shared)
    {
        return reinterpret_cast<uint8_t*>(shared->chunkHead) + SIZEOF_CHUNK_HEADER;
    }

    static const size_t kDefaultChunkCapacity = RAPIDJSON_ALLOCATOR_DEFAULT_CHUNK_CAPACITY; //!< Емкость чанка по умолчанию.

public:
    static const bool kNeedFree = false;    //!< Сообщите пользователям, что с помощью этого распределителя не нужно вызывать Free(). (концепция распределителя)
    static const bool kRefCounted = true;   //!< Сообщите пользователям, что этот распределитель учитывает ссылки при копировании.

    //! Конструктор с chunkSize.
    /*! \param chunkSize Размер фрагмента памяти. По умолчанию используется kDefaultChunkSize.
        \param baseAllocator Распределитель для выделения фрагментов памяти.
    */
    explicit
    MemoryPoolAllocator(size_t chunkSize = kDefaultChunkCapacity, BaseAllocator* baseAllocator = 0) :
        chunk_capacity_(chunkSize),
        baseAllocator_(baseAllocator ? baseAllocator : RAPIDJSON_NEW(BaseAllocator)()),
        shared_(static_cast<SharedData*>(baseAllocator_ ? baseAllocator_->Malloc(SIZEOF_SHARED_DATA + SIZEOF_CHUNK_HEADER) : 0))
    {
        RAPIDJSON_ASSERT(baseAllocator_ != 0);
        RAPIDJSON_ASSERT(shared_ != 0);
        if (baseAllocator) {
            shared_->ownBaseAllocator = 0;
        }
        else {
            shared_->ownBaseAllocator = baseAllocator_;
        }
        shared_->chunkHead = GetChunkHead(shared_);
        shared_->chunkHead->capacity = 0;
        shared_->chunkHead->size = 0;
        shared_->chunkHead->next = 0;
        shared_->ownBuffer = true;
        shared_->refcount = 1;
    }

    //! Конструктор с буфером, предоставленным пользователем.
    /*! Пользовательский буфер будет использоваться в первую очередь. Когда он заполнен, пул памяти выделяет новый фрагмент с размером фрагмента.

        Пользовательский буфер не будет освобожден при разрушении этого распределителя.

        \param buffer Буфер, предоставленный пользователем.
        \param size Размер буфера в байтах. Он должен быть как минимум больше sizeof(ChunkHeader).
        \param chunkSize Размер фрагмента памяти. По умолчанию используется kDefaultChunkSize.
        \param baseAllocator Распределитель для выделения фрагментов памяти.
    */
    MemoryPoolAllocator(void *buffer, size_t size, size_t chunkSize = kDefaultChunkCapacity, BaseAllocator* baseAllocator = 0) :
        chunk_capacity_(chunkSize),
        baseAllocator_(baseAllocator),
        shared_(static_cast<SharedData*>(AlignBuffer(buffer, size)))
    {
        RAPIDJSON_ASSERT(size >= SIZEOF_SHARED_DATA + SIZEOF_CHUNK_HEADER);
        shared_->chunkHead = GetChunkHead(shared_);
        shared_->chunkHead->capacity = size - SIZEOF_SHARED_DATA - SIZEOF_CHUNK_HEADER;
        shared_->chunkHead->size = 0;
        shared_->chunkHead->next = 0;
        shared_->ownBaseAllocator = 0;
        shared_->ownBuffer = false;
        shared_->refcount = 1;
    }

    MemoryPoolAllocator(const MemoryPoolAllocator& rhs) RAPIDJSON_NOEXCEPT :
        chunk_capacity_(rhs.chunk_capacity_),
        baseAllocator_(rhs.baseAllocator_),
        shared_(rhs.shared_)
    {
        RAPIDJSON_NOEXCEPT_ASSERT(shared_->refcount > 0);
        ++shared_->refcount;
    }
    MemoryPoolAllocator& operator=(const MemoryPoolAllocator& rhs) RAPIDJSON_NOEXCEPT
    {
        RAPIDJSON_NOEXCEPT_ASSERT(rhs.shared_->refcount > 0);
        ++rhs.shared_->refcount;
        this->~MemoryPoolAllocator();
        baseAllocator_ = rhs.baseAllocator_;
        chunk_capacity_ = rhs.chunk_capacity_;
        shared_ = rhs.shared_;
        return *this;
    }

#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
    MemoryPoolAllocator(MemoryPoolAllocator&& rhs) RAPIDJSON_NOEXCEPT :
        chunk_capacity_(rhs.chunk_capacity_),
        baseAllocator_(rhs.baseAllocator_),
        shared_(rhs.shared_)
    {
        RAPIDJSON_NOEXCEPT_ASSERT(rhs.shared_->refcount > 0);
        rhs.shared_ = 0;
    }
    MemoryPoolAllocator& operator=(MemoryPoolAllocator&& rhs) RAPIDJSON_NOEXCEPT
    {
        RAPIDJSON_NOEXCEPT_ASSERT(rhs.shared_->refcount > 0);
        this->~MemoryPoolAllocator();
        baseAllocator_ = rhs.baseAllocator_;
        chunk_capacity_ = rhs.chunk_capacity_;
        shared_ = rhs.shared_;
        rhs.shared_ = 0;
        return *this;
    }
#endif

    //! Деструктор.
    /*! При этом освобождаются все фрагменты памяти, за исключением предоставленного пользователем буфера.
    */
    ~MemoryPoolAllocator() RAPIDJSON_NOEXCEPT {
        if (!shared_) {
            // ничего не делать, если переехали
            return;
        }
        if (shared_->refcount > 1) {
            --shared_->refcount;
            return;
        }
        Clear();
        BaseAllocator *a = shared_->ownBaseAllocator;
        if (shared_->ownBuffer) {
            baseAllocator_->Free(shared_);
        }
        RAPIDJSON_DELETE(a);
    }

    //! Освобождает все фрагменты памяти, за исключением первого/пользовательского.
    void Clear() RAPIDJSON_NOEXCEPT {
        RAPIDJSON_NOEXCEPT_ASSERT(shared_->refcount > 0);
        for (;;) {
            ChunkHeader* c = shared_->chunkHead;
            if (!c->next) {
                break;
            }
            shared_->chunkHead = c->next;
            baseAllocator_->Free(c);
        }
        shared_->chunkHead->size = 0;
    }

    //! Вычисляет общую емкость выделенных фрагментов памяти.
    /*! \возвращает общую емкость в байтах.
    */
    size_t Capacity() const RAPIDJSON_NOEXCEPT {
        RAPIDJSON_NOEXCEPT_ASSERT(shared_->refcount > 0);
        size_t capacity = 0;
        for (ChunkHeader* c = shared_->chunkHead; c != 0; c = c->next)
            capacity += c->capacity;
        return capacity;
    }

    //! Вычисляет выделенные блоки памяти.
    /*! \return общее количество использованных байт.
    */
    size_t Size() const RAPIDJSON_NOEXCEPT {
        RAPIDJSON_NOEXCEPT_ASSERT(shared_->refcount > 0);
        size_t size = 0;
        for (ChunkHeader* c = shared_->chunkHead; c != 0; c = c->next)
            size += c->size;
        return size;
    }

    //! Является ли распределитель общим.
    /*! \вернуть истину или ложь.
    */
    bool Shared() const RAPIDJSON_NOEXCEPT {
        RAPIDJSON_NOEXCEPT_ASSERT(shared_->refcount > 0);
        return shared_->refcount > 1;
    }

    //! Выделяет блок памяти. (концепция распределителя)
    void* Malloc(size_t size) {
        RAPIDJSON_NOEXCEPT_ASSERT(shared_->refcount > 0);
        if (!size)
            return NULL;

        size = RAPIDJSON_ALIGN(size);
        if (RAPIDJSON_UNLIKELY(shared_->chunkHead->size + size > shared_->chunkHead->capacity))
            if (!AddChunk(chunk_capacity_ > size ? chunk_capacity_ : size))
                return NULL;

        void *buffer = GetChunkBuffer(shared_) + shared_->chunkHead->size;
        shared_->chunkHead->size += size;
        return buffer;
    }

    //! Изменяет размер блока памяти (концепция Распределитель)
    void* Realloc(void* originalPtr, size_t originalSize, size_t newSize) {
        if (originalPtr == 0)
            return Malloc(newSize);

        RAPIDJSON_NOEXCEPT_ASSERT(shared_->refcount > 0);
        if (newSize == 0)
            return NULL;

        originalSize = RAPIDJSON_ALIGN(originalSize);
        newSize = RAPIDJSON_ALIGN(newSize);

        // Не уменьшайте размер, если новый размер меньше исходного.
        if (originalSize >= newSize)
            return originalPtr;

        // Просто разверните его, если это последнее выделение и достаточно места.
        if (originalPtr == GetChunkBuffer(shared_) + shared_->chunkHead->size - originalSize) {
            size_t increment = static_cast<size_t>(newSize - originalSize);
            if (shared_->chunkHead->size + increment <= shared_->chunkHead->capacity) {
                shared_->chunkHead->size += increment;
                return originalPtr;
            }
        }

        // Процесс Realloc: выделять и копировать память, не освобождать исходный буфер.
        if (void* newBuffer = Malloc(newSize)) {
            if (originalSize)
                std::memcpy(newBuffer, originalPtr, originalSize);
            return newBuffer;
        }
        else
            return NULL;
    }

    //! Освобождает блок памяти (концепция Распределитель)
    static void Free(void *ptr) RAPIDJSON_NOEXCEPT { (void)ptr; } // ничего не делать

    //! Сравнить (равенство) с другим MemoryPoolAllocator
    bool operator==(const MemoryPoolAllocator& rhs) const RAPIDJSON_NOEXCEPT {
        RAPIDJSON_NOEXCEPT_ASSERT(shared_->refcount > 0);
        RAPIDJSON_NOEXCEPT_ASSERT(rhs.shared_->refcount > 0);
        return shared_ == rhs.shared_;
    }
    //! Сравнить (неравенство) с другим MemoryPoolAllocator
    bool operator!=(const MemoryPoolAllocator& rhs) const RAPIDJSON_NOEXCEPT {
        return !operator==(rhs);
    }

private:
    //! Создает новый чанк.
    /*! \param емкость Емкость чанка в байтах.
        \Верните true в случае успеха.
    */
    bool AddChunk(size_t capacity) {
        if (!baseAllocator_)
            shared_->ownBaseAllocator = baseAllocator_ = RAPIDJSON_NEW(BaseAllocator)();
        if (ChunkHeader* chunk = static_cast<ChunkHeader*>(baseAllocator_->Malloc(SIZEOF_CHUNK_HEADER + capacity))) {
            chunk->capacity = capacity;
            chunk->size = 0;
            chunk->next = shared_->chunkHead;
            shared_->chunkHead = chunk;
            return true;
        }
        else
            return false;
    }

    static inline void* AlignBuffer(void* buf, size_t &size)
    {
        RAPIDJSON_NOEXCEPT_ASSERT(buf != 0);
        const uintptr_t mask = sizeof(void*) - 1;
        const uintptr_t ubuf = reinterpret_cast<uintptr_t>(buf);
        if (RAPIDJSON_UNLIKELY(ubuf & mask)) {
            const uintptr_t abuf = (ubuf + mask) & ~mask;
            RAPIDJSON_ASSERT(size >= abuf - ubuf);
            buf = reinterpret_cast<void*>(abuf);
            size -= abuf - ubuf;
        }
        return buf;
    }

    size_t chunk_capacity_;     //!< Минимальная емкость чанка при его выделении.
    BaseAllocator* baseAllocator_;  //!< базовый распределитель для выделения фрагментов памяти.
    SharedData *shared_;        //!< Общие данные распределителя
};

namespace internal {
    template<typename, typename = void>
    struct IsRefCounted :
        public FalseType
    { };
    template<typename T>
    struct IsRefCounted<T, typename internal::EnableIfCond<T::kRefCounted>::Type> :
        public TrueType
    { };
}

template<typename T, typename A>
inline T* Realloc(A& a, T* old_p, size_t old_n, size_t new_n)
{
    RAPIDJSON_NOEXCEPT_ASSERT(old_n <= (std::numeric_limits<size_t>::max)() / sizeof(T) && new_n <= (std::numeric_limits<size_t>::max)() / sizeof(T));
    return static_cast<T*>(a.Realloc(old_p, old_n * sizeof(T), new_n * sizeof(T)));
}

template<typename T, typename A>
inline T *Malloc(A& a, size_t n = 1)
{
    return Realloc<T, A>(a, NULL, 0, n);
}

template<typename T, typename A>
inline void Free(A& a, T *p, size_t n = 1)
{
    static_cast<void>(Realloc<T, A>(a, p, n, 0));
}

#ifdef __GNUC__
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(effc++) // std::allocator можно безопасно унаследовать
#endif

template <typename T, typename BaseAllocator = CrtAllocator>
class StdAllocator :
    public std::allocator<T>
{
    typedef std::allocator<T> allocator_type;
#if RAPIDJSON_HAS_CXX11
    typedef std::allocator_traits<allocator_type> traits_type;
#else
    typedef allocator_type traits_type;
#endif

public:
    typedef BaseAllocator BaseAllocatorType;

    StdAllocator() RAPIDJSON_NOEXCEPT :
        allocator_type(),
        baseAllocator_()
    { }

    StdAllocator(const StdAllocator& rhs) RAPIDJSON_NOEXCEPT :
        allocator_type(rhs),
        baseAllocator_(rhs.baseAllocator_)
    { }

    template<typename U>
    StdAllocator(const StdAllocator<U, BaseAllocator>& rhs) RAPIDJSON_NOEXCEPT :
        allocator_type(rhs),
        baseAllocator_(rhs.baseAllocator_)
    { }

#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
    StdAllocator(StdAllocator&& rhs) RAPIDJSON_NOEXCEPT :
        allocator_type(std::move(rhs)),
        baseAllocator_(std::move(rhs.baseAllocator_))
    { }
#endif
#if RAPIDJSON_HAS_CXX11
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;
#endif

    /* неявный */
    StdAllocator(const BaseAllocator& allocator) RAPIDJSON_NOEXCEPT :
        allocator_type(),
        baseAllocator_(allocator)
    { }

    ~StdAllocator() RAPIDJSON_NOEXCEPT
    { }

    template<typename U>
    struct rebind {
        typedef StdAllocator<U, BaseAllocator> other;
    };

    typedef typename traits_type::size_type         size_type;
    typedef typename traits_type::difference_type   difference_type;

    typedef typename traits_type::value_type        value_type;
    typedef typename traits_type::pointer           pointer;
    typedef typename traits_type::const_pointer     const_pointer;

#if RAPIDJSON_HAS_CXX11

    typedef typename std::add_lvalue_reference<value_type>::type &reference;
    typedef typename std::add_lvalue_reference<typename std::add_const<value_type>::type>::type &const_reference;

    pointer address(reference r) const RAPIDJSON_NOEXCEPT
    {
        return std::addressof(r);
    }
    const_pointer address(const_reference r) const RAPIDJSON_NOEXCEPT
    {
        return std::addressof(r);
    }

    size_type max_size() const RAPIDJSON_NOEXCEPT
    {
        return traits_type::max_size(*this);
    }

    template <typename ...Args>
    void construct(pointer p, Args&&... args)
    {
        traits_type::construct(*this, p, std::forward<Args>(args)...);
    }
    void destroy(pointer p)
    {
        traits_type::destroy(*this, p);
    }

#else // !RAPIDJSON_HAS_CXX11

    typedef typename allocator_type::reference       reference;
    typedef typename allocator_type::const_reference const_reference;

    pointer address(reference r) const RAPIDJSON_NOEXCEPT
    {
        return allocator_type::address(r);
    }
    const_pointer address(const_reference r) const RAPIDJSON_NOEXCEPT
    {
        return allocator_type::address(r);
    }

    size_type max_size() const RAPIDJSON_NOEXCEPT
    {
        return allocator_type::max_size();
    }

    void construct(pointer p, const_reference r)
    {
        allocator_type::construct(p, r);
    }
    void destroy(pointer p)
    {
        allocator_type::destroy(p);
    }

#endif // !RAPIDJSON_HAS_CXX11

    template <typename U>
    U* allocate(size_type n = 1, const void* = 0)
    {
        return RAPIDJSON_NAMESPACE::Malloc<U>(baseAllocator_, n);
    }
    template <typename U>
    void deallocate(U* p, size_type n = 1)
    {
        RAPIDJSON_NAMESPACE::Free<U>(baseAllocator_, p, n);
    }

    pointer allocate(size_type n = 1, const void* = 0)
    {
        return allocate<value_type>(n);
    }
    void deallocate(pointer p, size_type n = 1)
    {
        deallocate<value_type>(p, n);
    }

#if RAPIDJSON_HAS_CXX11
    using is_always_equal = std::is_empty<BaseAllocator>;
#endif

    template<typename U>
    bool operator==(const StdAllocator<U, BaseAllocator>& rhs) const RAPIDJSON_NOEXCEPT
    {
        return baseAllocator_ == rhs.baseAllocator_;
    }
    template<typename U>
    bool operator!=(const StdAllocator<U, BaseAllocator>& rhs) const RAPIDJSON_NOEXCEPT
    {
        return !operator==(rhs);
    }

    //! Концепция распределителя RapidJSON
    static const bool kNeedFree = BaseAllocator::kNeedFree;
    static const bool kRefCounted = internal::IsRefCounted<BaseAllocator>::Value;
    void* Malloc(size_t size)
    {
        return baseAllocator_.Malloc(size);
    }
    void* Realloc(void* originalPtr, size_t originalSize, size_t newSize)
    {
        return baseAllocator_.Realloc(originalPtr, originalSize, newSize);
    }
    static void Free(void *ptr) RAPIDJSON_NOEXCEPT
    {
        BaseAllocator::Free(ptr);
    }

private:
    template <typename, typename>
    friend class StdAllocator; // доступ к StdAllocator<!T>.*

    BaseAllocator baseAllocator_;
};

#if !RAPIDJSON_HAS_CXX17 // std::allocator<void> устарел в C++17
template <typename BaseAllocator>
class StdAllocator<void, BaseAllocator> :
    public std::allocator<void>
{
    typedef std::allocator<void> allocator_type;

public:
    typedef BaseAllocator BaseAllocatorType;

    StdAllocator() RAPIDJSON_NOEXCEPT :
        allocator_type(),
        baseAllocator_()
    { }

    StdAllocator(const StdAllocator& rhs) RAPIDJSON_NOEXCEPT :
        allocator_type(rhs),
        baseAllocator_(rhs.baseAllocator_)
    { }

    template<typename U>
    StdAllocator(const StdAllocator<U, BaseAllocator>& rhs) RAPIDJSON_NOEXCEPT :
        allocator_type(rhs),
        baseAllocator_(rhs.baseAllocator_)
    { }

    /* неявный */
    StdAllocator(const BaseAllocator& baseAllocator) RAPIDJSON_NOEXCEPT :
        allocator_type(),
        baseAllocator_(baseAllocator)
    { }

    ~StdAllocator() RAPIDJSON_NOEXCEPT
    { }

    template<typename U>
    struct rebind {
        typedef StdAllocator<U, BaseAllocator> other;
    };

    typedef typename allocator_type::value_type value_type;

private:
    template <typename, typename>
    friend class StdAllocator; // доступ к StdAllocator<!T>.*

    BaseAllocator baseAllocator_;
};
#endif

#ifdef __GNUC__
RAPIDJSON_DIAG_POP
#endif

RAPIDJSON_NAMESPACE_END

#endif // RAPIDJSON_ENCODINGS_H_
