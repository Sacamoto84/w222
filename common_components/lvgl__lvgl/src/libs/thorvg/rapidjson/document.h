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

#ifndef RAPIDJSON_DOCUMENT_H_
#define RAPIDJSON_DOCUMENT_H_

/*! \file document.h */

#include "reader.h"
#include "internal/meta.h"
#include "internal/strfunc.h"
#include "memorystream.h"
#include "encodedstream.h"
#include <new>      // размещение новое
#include <limits>
#ifdef __cpp_lib_three_way_comparison
#include <compare>
#endif

RAPIDJSON_DIAG_PUSH
#ifdef __clang__
RAPIDJSON_DIAG_OFF(padded)
RAPIDJSON_DIAG_OFF(switch-enum)
RAPIDJSON_DIAG_OFF(c++98-compat)
#elif defined(_MSC_VER)
RAPIDJSON_DIAG_OFF(4127) // условное выражение является константой
RAPIDJSON_DIAG_OFF(4244) // преобразование из kXxxFlags в 'uint16_t', возможна потеря данных
#endif

#ifdef __GNUC__
RAPIDJSON_DIAG_OFF(effc++)
#endif // __GNUC__

#ifdef GetObject
// см.  https://github.com/Tencent/rapidjson/issues/1448
// предыдущий включенныйwindows.hмог определять макрос GetObject, который влияет
// GetObject определение здесь. Это гарантирует, что макросы не будут применяться.
#pragma push_macro("GetObject")
#define RAPIDJSON_WINDOWS_GETOBJECT_WORKAROUND_APPLIED
#undef GetObject
#endif

#ifndef RAPIDJSON_NOMEMBERITERATORCLASS
#include <iterator> // std::random_access_iterator_tag
#endif

#if RAPIDJSON_USE_MEMBERSMAP
#include <map> // станд::мультимап
#endif

RAPIDJSON_NAMESPACE_BEGIN

// Форвардное заявление.
template <typename Encoding, typename Allocator>
class GenericValue;

template <typename Encoding, typename Allocator, typename StackAllocator>
class GenericDocument;

/*! \def RAPIDJSON_DEFAULT_ALLOCATOR
    \ingroup RAPIDJSON_CONFIG
    \brief Выбор управления по умолчанию.

    Пользователь может определить это для использования CrtAllocator или MemoryPoolAllocator.
*/
#ifndef RAPIDJSON_DEFAULT_ALLOCATOR
#define RAPIDJSON_DEFAULT_ALLOCATOR ::RAPIDJSON_NAMESPACE::MemoryPoolAllocator<::RAPIDJSON_NAMESPACE::CrtAllocator>
#endif

/*! \def RAPIDJSON_DEFAULT_STACK_ALLOCATOR
    \ingroup RAPIDJSON_CONFIG
    \brief Выбор блока управления по умолчанию для документа.

    Пользователь может определить это для использования CrtAllocator или MemoryPoolAllocator.
*/
#ifndef RAPIDJSON_DEFAULT_STACK_ALLOCATOR
#define RAPIDJSON_DEFAULT_STACK_ALLOCATOR ::RAPIDJSON_NAMESPACE::CrtAllocator
#endif

/*! \def RAPIDJSON_VALUE_DEFAULT_OBJECT_CAPACITY
    \ingroup RAPIDJSON_CONFIG
    \brief Определяемое пользовательское значение kDefaultObjectCapacity.

    Пользователь может определить это как любое натуральное число.
*/
#ifndef RAPIDJSON_VALUE_DEFAULT_OBJECT_CAPACITY
// количество объектов, для которых Rapidjson::Value применяется память по умолчанию
#define RAPIDJSON_VALUE_DEFAULT_OBJECT_CAPACITY 16
#endif

/*! \def RAPIDJSON_VALUE_DEFAULT_ARRAY_CAPACITY
    \ingroup RAPIDJSON_CONFIG
    \brief Определяемое пользовательское значение kDefaultArrayCapacity.

    Пользователь может определить это как любое натуральное число.
*/
#ifndef RAPIDJSON_VALUE_DEFAULT_ARRAY_CAPACITY
// количество элементов массива, для которых Rapidjson::Value применяется память по умолчанию
#define RAPIDJSON_VALUE_DEFAULT_ARRAY_CAPACITY 16
#endif

//! Пара имя-значение в значении объекта JSON.
/*!
    Этот класс был для GenericValue. Раньше это была внутренняя структура.
    Но компилятор ( IBM XL C/C++ для AIX ) сообщил о проблеме с этим, поэтому он был перемещен как структура области пространства имен.
    https://code.google.com/p/rapidjson/issues/detail?id=64
*/
template <typename Encoding, typename Allocator>
class GenericMember {
public:
    GenericValue<Encoding, Allocator> name;     //!< имя участника (должно быть строкой)
    GenericValue<Encoding, Allocator> value;    //!< значение члена.

#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
    //! Переместить конструктор в C++11
    GenericMember(GenericMember&& rhs) RAPIDJSON_NOEXCEPT
        : name(std::move(rhs.name)),
          value(std::move(rhs.value))
    {
    }

    //! Переместить назначение в C++11
    GenericMember& operator=(GenericMember&& rhs) RAPIDJSON_NOEXCEPT {
        return *this = static_cast<GenericMember&>(rhs);
    }
#endif

    //! Задание с семантикой перемещения.
    /*! \param rhs Источник задания. Его имя и значение после применения становятся нулевыми значениями.
    */
    GenericMember& operator=(GenericMember& rhs) RAPIDJSON_NOEXCEPT {
        if (RAPIDJSON_LIKELY(this != &rhs)) {
            name = rhs.name;
            value = rhs.value;
        }
        return *this;
    }

    // swap() для std::sort() и другое безопасное использование вSTL.
    friend inline void swap(GenericMember& a, GenericMember& b) RAPIDJSON_NOEXCEPT {
        a.name.Swap(b.name);
        a.value.Swap(b.value);
    }

private:
    //! Копирование конструктора не разрешено.
    GenericMember(const GenericMember& rhs);
};

///////////////////////////////////////////////////////////////////////////////
// GenericMemberIterator

#ifndef RAPIDJSON_NOMEMBERITERATORCLASS

//! (Константный) итератор-член для значения объекта JSON
/*!
    \tparam Const Is this a constant iterator?
    \tparam Кодировка Кодировка значения. (Даже нестроковые значения должны иметь одинаковую кодировку в документе)
    \tparam Тип распределителя для выделения памяти для объекта, массива и строки.

    Этот класс реализует итератор дополнительного доступа к элементам GenericMember.
    GenericValue см. в разделеISO/IEC14882:2003(E) стандарт C++, 24.1 [lib.iterator.requirements].

    \note Эта реализация итератора в основном создана для того, чтобы избежать неявного
        преобразования значений итератора в \c NULL ,
        например из GenericValue::FindMember.

    \note Определите \cRAPIDJSON_NOMEMBERITERATORCLASS, чтобы вернуться к
        реализация на основе указателей, если ваша платформа не предоставляет
        заголовок <итератор> C++.

    \см. GenericMember, GenericValue::MemberIterator, GenericValue::ConstMemberIterator
 */
template <bool Const, typename Encoding, typename Allocator>
class GenericMemberIterator {

    friend class GenericValue<Encoding,Allocator>;
    template <bool, typename, typename> friend class GenericMemberIterator;

    typedef GenericMember<Encoding,Allocator> PlainType;
    typedef typename internal::MaybeAddConst<Const,PlainType>::Type ValueType;

public:
    //! Тип итератора сам по себе
    typedef GenericMemberIterator Iterator;
    //! Тип постоянного итератора
    typedef GenericMemberIterator<true,Encoding,Allocator>  ConstIterator;
    //! Непостоянный тип итератора
    typedef GenericMemberIterator<false,Encoding,Allocator> NonConstIterator;

    /** \name стандарт: Поддержка iterator_traits */
    //@{
    typedef ValueType      value_type;
    typedef ValueType *    pointer;
    typedef ValueType &    reference;
    typedef std::ptrdiff_t difference_type;
    typedef std::random_access_iterator_tag iterator_category;
    //@}

    //! Указатель на (const) GenericMember
    typedef pointer         Pointer;
    //! Ссылка на (const) GenericMember
    typedef reference       Reference;
    //! Целочисленный тип со знаком (например, \cptrdiff_t)
    typedef difference_type DifferenceType;

    //! Конструктор по умолчанию (единственное значение)
    /*! Создает итератор, не указывающий ни на один элемент.
        \note Все операции, кроме сравнения, для подобных результатов не удалось.
     */
    GenericMemberIterator() : ptr_() {}

    //! Преобразования итераторов в более константные
    /*!
        \param it (неконстантный) итератор для копирования

        будет создан итератор из другого GenericMemberIterator.
        это «меньше константы».  В частности, создание непостоянного итератора
        из постоянного итератора отключены:
        \li const -> неконстантный (не ок)
        \li const -> const (ок)
        \li неконстантный -> константный (ок)
        \li неконстантный -> неконстантный (ок)

        \note Если параметр шаблона \c Const уже имеет значение \c false, это
            Конструктор фактически определяет обычный конструктор-копию.
            В противном случае конструктор копирования определяется неявно.
    */
    GenericMemberIterator(const NonConstIterator & it) : ptr_(it.ptr_) {}
    Iterator& operator=(const NonConstIterator & it) { ptr_ = it.ptr_; return *this; }

    //!  @nameстеппинг
    //@{
    Iterator& operator++(){ ++ptr_; return *this; }
    Iterator& operator--(){ --ptr_; return *this; }
    Iterator  operator++(int){ Iterator old(*this); ++ptr_; return old; }
    Iterator  operator--(int){ Iterator old(*this); --ptr_; return old; }
    //@}

    //!  @nameприращение/уменьшение
    //@{
    Iterator operator+(DifferenceType n) const { return Iterator(ptr_+n); }
    Iterator operator-(DifferenceType n) const { return Iterator(ptr_-n); }

    Iterator& operator+=(DifferenceType n) { ptr_+=n; return *this; }
    Iterator& operator-=(DifferenceType n) { ptr_-=n; return *this; }
    //@}

    //!  @nameотношения
    //@{
    template <bool Const_> bool operator==(const GenericMemberIterator<Const_, Encoding, Allocator>& that) const { return ptr_ == that.ptr_; }
    template <bool Const_> bool operator!=(const GenericMemberIterator<Const_, Encoding, Allocator>& that) const { return ptr_ != that.ptr_; }
    template <bool Const_> bool operator<=(const GenericMemberIterator<Const_, Encoding, Allocator>& that) const { return ptr_ <= that.ptr_; }
    template <bool Const_> bool operator>=(const GenericMemberIterator<Const_, Encoding, Allocator>& that) const { return ptr_ >= that.ptr_; }
    template <bool Const_> bool operator< (const GenericMemberIterator<Const_, Encoding, Allocator>& that) const { return ptr_ < that.ptr_; }
    template <bool Const_> bool operator> (const GenericMemberIterator<Const_, Encoding, Allocator>& that) const { return ptr_ > that.ptr_; }

#ifdef __cpp_lib_three_way_comparison
    template <bool Const_> std::strong_ordering operator<=>(const GenericMemberIterator<Const_, Encoding, Allocator>& that) const { return ptr_ <=> that.ptr_; }
#endif
    //@}

    //!  Разыменование @name
    //@{
    Reference operator*() const { return *ptr_; }
    Pointer   operator->() const { return ptr_; }
    Reference operator[](DifferenceType n) const { return ptr_[n]; }
    //@}

    //! Расстояние
    DifferenceType operator-(ConstIterator that) const { return ptr_-that.ptr_; }

private:
    //! Внутренний конструктор из простого указателя
    explicit GenericMemberIterator(Pointer p) : ptr_(p) {}

    Pointer ptr_; //!< необработанный указатель
};

#else // RAPIDJSON_NOMEMBERITERATORCLASS

// реализация итератора членов на основе класса отключена, используйте простые указатели

template <bool Const, typename Encoding, typename Allocator>
class GenericMemberIterator;

//! неконстантный GenericMemberIterator
template <typename Encoding, typename Allocator>
class GenericMemberIterator<false,Encoding,Allocator> {
public:
    //! используйте простой указатель в качестве типа итератора
    typedef GenericMember<Encoding,Allocator>* Iterator;
};
//! const GenericMemberIterator
template <typename Encoding, typename Allocator>
class GenericMemberIterator<true,Encoding,Allocator> {
public:
    //! используйте простой константный указатель в качестве типа итератора
    typedef const GenericMember<Encoding,Allocator>* Iterator;
};

#endif // RAPIDJSON_NOMEMBERITERATORCLASS

///////////////////////////////////////////////////////////////////////////////
// GenericStringRef

//! Ссылка на константную строку (без копирования)
/*!
    \tparam CharType символьный тип строки

    Этот вспомогательный класс используется для автоматического вывода постоянной строки.
    ссылки на строковые литералы, особенно из \c const \b (!)
    массивы символов.

    Основное использование — создание строковых значений JSON без копирования
    исходная строка через распределитель \ref.  Для этого необходимо, упомянутый
    строковые указатели имеют достаточное время жизни, которое превышает время жизни
    связанного GenericValue.

    \b Пример
    \code
    Значение v("фу");   // ок, не нужно копировать и вычислять размер
    const char foo[] = "foo";
    v.SetString(фу); // ок

    const char* bar = foo;
    // Значение х(бар); // не ок, нельзя полагаться на время жизни бара
    Значение x(StringRef(bar)); // время жизни гарантировано пользователю
    Значение y(StringRef(bar, 3));  // ок, явно передаем взгляд
    \endcode

    \см. StringRef, GenericValue::SetString
*/
template<typename CharType>
struct GenericStringRef {
    typedef CharType Ch; //!< тип символа строки

    //! Создать ссылку на текст из массива символов \c const
#ifndef __clang__ // -Документация
    /*!
        Этот конструктор неявно создает ссылку на константную строку из
        константный массив символов \c.  Он имеет лучшую производительность, чем
        \ref StringRef(const CharType*) посредством определения длины строки \ref
        от длины массива, а также поддерживает строки, содержащие ноль
        персонажи.

        \tparam N длина строки, определяется автоматически

        \param str Постоянный массив символов, спасибо, что время жизни больше
            чем использование строки, например. универсальное значение

        \post \ref s == ул

        \note Постоянная сложность.
        \note Существует скрытая частная перегрузка, запрещающие ссылки на
            неконстантные массивы символов, которые будут созданы с помощью этого конструктора.
            При этом, напр. Массивы области функций раньше заполнялись через
            \c snprintf исключен из рассмотрения.
            В таких случаях указанную строку следует скопировать \b в файл
            Вместо этого GenericValue.
     */
#endif
    template<SizeType N>
    GenericStringRef(const CharType (&str)[N]) RAPIDJSON_NOEXCEPT
        : s(str), length(N-1) {}

    //! Обязательно создайте ссылку на строку из символа указателя \c const
#ifndef __clang__ // -Документация
    /*!
        Этот конструктор можно использовать для явного создания ссылки на \b.
        постоянный указатель строки.

        \see StringRef(const CharType*)

        \param str Указатель постоянного символа, свидетельствующего о том, что время жизни больше
            чем использование строки, например. универсальное значение

        \post \ref s == ул

        \note Существует скрытая частная перегрузка, запрещающие ссылки на
            неконстантные массивы символов, которые будут созданы с помощью этого конструктора.
            При этом, напр. Массивы области функций раньше заполнялись через
            \c snprintf исключен из рассмотрения.
            В таких случаях указанную строку следует скопировать \b в файл
            Вместо этого GenericValue.
     */
#endif
    explicit GenericStringRef(const CharType* str)
        : s(str), length(NotNullStrLen(str)) {}

    //! Создать постоянную ссылку на строку из указателя и длины
#ifndef __clang__ // -Документация
    /*! \param str константная строка, что время жизни больше, чем использование строки, например. универсальное значение
        \param len длина строки, завершающий терминатор NULL

        \post \ref s == str && \ref length == len
        \note Постоянная сложность.
     */
#endif
    GenericStringRef(const CharType* str, SizeType len)
        : s(RAPIDJSON_LIKELY(str) ? str : emptyString), length(len) { RAPIDJSON_ASSERT(str != 0 || len == 0u); }

    GenericStringRef(const GenericStringRef& rhs) : s(rhs.s), length(rhs.length) {}

    //! неявное преобразование в простой указатель CharType
    operator const Ch *() const { return s; }

    const Ch* const s; //!< простой указатель CharType
    const SizeType length; //!< длина строки (исключая завершающий терминатор NULL)

private:
    SizeType NotNullStrLen(const CharType* str) {
        RAPIDJSON_ASSERT(str != 0);
        return internal::StrLen(str);
    }

    /// Пустая строка — используется при передаче указателя NULL.
    static const Ch emptyString[];

    //! Запретить построение из неконстантного массива
    template<SizeType N>
    GenericStringRef(CharType (&str)[N]) /* = удалить */;
    //! Копировать оператор присваивания не разрешено — неизменяемый тип
    GenericStringRef& operator=(const GenericStringRef& rhs) /* = удалить */;
};

template<typename CharType>
const CharType GenericStringRef<CharType>::emptyString[] = { CharType() };

//! Пометить указатель символа как константную строку
/*! Пометьте простой указатель символа как «строковый литерал».  Эта функция
    можно использовать, чтобы избежать копирования строки символов, на которую будет ссылаться как на
    значение в объектеJSONGenericValue, если известно время жизни строки
    быть действительным достаточно долго.
    \tparam CharType Тип символьной строки
    \param str Константная строка, срок жизни которой обеспечен больше, чем использование строк, например. универсальное значение
    \return Объект ссылки на текст GenericStringRef
    \relatesalso GenericStringRef

    \see GenericValue::GenericValue(StringRefType), GenericValue::operator=(StringRefType), GenericValue::SetString(StringRefType), GenericValue::PushBack(StringRefType, Allocator&), GenericValue::AddMember
*/
template<typename CharType>
inline GenericStringRef<CharType> StringRef(const CharType* str) {
    return GenericStringRef<CharType>(str);
}

//! Пометить указатель символа как константную строку
/*! Пометьте простой указатель символа как «строковый литерал».  Эта функция
    можно использовать, чтобы избежать копирования строки символов, на которую будет ссылаться как на
    значение в объектеJSONGenericValue, если известно время жизни строки
    быть действительным достаточно долго.

    Эта версия имеет лучшую производительность при указанной длине, а также
    поддерживает строку, содержащую нулевые символы.

    \tparam CharType символьный тип строки
    \param str Константная строка, срок жизни которой обеспечен больше, чем использование строк, например. универсальное значение
    \param length Длина исходной строки.
    \return Объект ссылки на текст GenericStringRef
    \relatesalso GenericStringRef
*/
template<typename CharType>
inline GenericStringRef<CharType> StringRef(const CharType* str, size_t length) {
    return GenericStringRef<CharType>(str, SizeType(length));
}

#if RAPIDJSON_HAS_STDSTRING
//! Пометить строковый объект как константную строку
/*! Относительно строковый объект (например, \c std::string) как «строковый литерал».
    Эту функцию можно использовать, чтобы избежать копирования строки, на которую будет ссылаться как на
    значение в объектеJSONGenericValue, если известно время жизни строки
    быть действительным достаточно долго.

    \tparam CharType символьный тип строки
    \param str Константная строка, срок жизни которой обеспечен больше, чем использование строк, например. универсальное значение
    \return Объект ссылки на текст GenericStringRef
    \relatesalso GenericStringRef
    \note Требуется определение символа препроцессора \refRAPIDJSON_HAS_STDSTRING.
*/
template<typename CharType>
inline GenericStringRef<CharType> StringRef(const std::basic_string<CharType>& str) {
    return GenericStringRef<CharType>(str.data(), SizeType(str.size()));
}
#endif

///////////////////////////////////////////////////////////////////////////////
// Признаки типа GenericValue
namespace internal {

template <typename T, typename Encoding = void, typename Allocator = void>
struct IsGenericValueImpl : FalseType {};

// выбирать кандидатов в соответствии с вложенными типами кодировки и распределителя
template <typename T> struct IsGenericValueImpl<T, typename Void<typename T::EncodingType>::Type, typename Void<typename T::AllocatorType>::Type>
    : IsBaseOf<GenericValue<typename T::EncodingType, typename T::AllocatorType>, T>::Type {};

// помощник парламента произвольных экземпляров GenericValue, включая производные классы
template <typename T> struct IsGenericValue : IsGenericValueImpl<T>::Type {};

} // внутреннее пространство имен

///////////////////////////////////////////////////////////////////////////////
// ТипПомощник

namespace internal {

template <typename ValueType, typename T>
struct TypeHelper {};

template<typename ValueType>
struct TypeHelper<ValueType, bool> {
    static bool Is(const ValueType& v) { return v.IsBool(); }
    static bool Get(const ValueType& v) { return v.GetBool(); }
    static ValueType& Set(ValueType& v, bool data) { return v.SetBool(data); }
    static ValueType& Set(ValueType& v, bool data, typename ValueType::AllocatorType&) { return v.SetBool(data); }
};

template<typename ValueType>
struct TypeHelper<ValueType, int> {
    static bool Is(const ValueType& v) { return v.IsInt(); }
    static int Get(const ValueType& v) { return v.GetInt(); }
    static ValueType& Set(ValueType& v, int data) { return v.SetInt(data); }
    static ValueType& Set(ValueType& v, int data, typename ValueType::AllocatorType&) { return v.SetInt(data); }
};

template<typename ValueType>
struct TypeHelper<ValueType, unsigned> {
    static bool Is(const ValueType& v) { return v.IsUint(); }
    static unsigned Get(const ValueType& v) { return v.GetUint(); }
    static ValueType& Set(ValueType& v, unsigned data) { return v.SetUint(data); }
    static ValueType& Set(ValueType& v, unsigned data, typename ValueType::AllocatorType&) { return v.SetUint(data); }
};

#ifdef _MSC_VER
RAPIDJSON_STATIC_ASSERT(sizeof(long) == sizeof(int));
template<typename ValueType>
struct TypeHelper<ValueType, long> {
    static bool Is(const ValueType& v) { return v.IsInt(); }
    static long Get(const ValueType& v) { return v.GetInt(); }
    static ValueType& Set(ValueType& v, long data) { return v.SetInt(data); }
    static ValueType& Set(ValueType& v, long data, typename ValueType::AllocatorType&) { return v.SetInt(data); }
};

RAPIDJSON_STATIC_ASSERT(sizeof(unsigned long) == sizeof(unsigned));
template<typename ValueType>
struct TypeHelper<ValueType, unsigned long> {
    static bool Is(const ValueType& v) { return v.IsUint(); }
    static unsigned long Get(const ValueType& v) { return v.GetUint(); }
    static ValueType& Set(ValueType& v, unsigned long data) { return v.SetUint(data); }
    static ValueType& Set(ValueType& v, unsigned long data, typename ValueType::AllocatorType&) { return v.SetUint(data); }
};
#endif

template<typename ValueType>
struct TypeHelper<ValueType, int64_t> {
    static bool Is(const ValueType& v) { return v.IsInt64(); }
    static int64_t Get(const ValueType& v) { return v.GetInt64(); }
    static ValueType& Set(ValueType& v, int64_t data) { return v.SetInt64(data); }
    static ValueType& Set(ValueType& v, int64_t data, typename ValueType::AllocatorType&) { return v.SetInt64(data); }
};

template<typename ValueType>
struct TypeHelper<ValueType, uint64_t> {
    static bool Is(const ValueType& v) { return v.IsUint64(); }
    static uint64_t Get(const ValueType& v) { return v.GetUint64(); }
    static ValueType& Set(ValueType& v, uint64_t data) { return v.SetUint64(data); }
    static ValueType& Set(ValueType& v, uint64_t data, typename ValueType::AllocatorType&) { return v.SetUint64(data); }
};

template<typename ValueType>
struct TypeHelper<ValueType, double> {
    static bool Is(const ValueType& v) { return v.IsDouble(); }
    static double Get(const ValueType& v) { return v.GetDouble(); }
    static ValueType& Set(ValueType& v, double data) { return v.SetDouble(data); }
    static ValueType& Set(ValueType& v, double data, typename ValueType::AllocatorType&) { return v.SetDouble(data); }
};

template<typename ValueType>
struct TypeHelper<ValueType, float> {
    static bool Is(const ValueType& v) { return v.IsFloat(); }
    static float Get(const ValueType& v) { return v.GetFloat(); }
    static ValueType& Set(ValueType& v, float data) { return v.SetFloat(data); }
    static ValueType& Set(ValueType& v, float data, typename ValueType::AllocatorType&) { return v.SetFloat(data); }
};

template<typename ValueType>
struct TypeHelper<ValueType, const typename ValueType::Ch*> {
    typedef const typename ValueType::Ch* StringType;
    static bool Is(const ValueType& v) { return v.IsString(); }
    static StringType Get(const ValueType& v) { return v.GetString(); }
    static ValueType& Set(ValueType& v, const StringType data) { return v.SetString(typename ValueType::StringRefType(data)); }
    static ValueType& Set(ValueType& v, const StringType data, typename ValueType::AllocatorType& a) { return v.SetString(data, a); }
};

#if RAPIDJSON_HAS_STDSTRING
template<typename ValueType>
struct TypeHelper<ValueType, std::basic_string<typename ValueType::Ch> > {
    typedef std::basic_string<typename ValueType::Ch> StringType;
    static bool Is(const ValueType& v) { return v.IsString(); }
    static StringType Get(const ValueType& v) { return StringType(v.GetString(), v.GetStringLength()); }
    static ValueType& Set(ValueType& v, const StringType& data, typename ValueType::AllocatorType& a) { return v.SetString(data, a); }
};
#endif

template<typename ValueType>
struct TypeHelper<ValueType, typename ValueType::Array> {
    typedef typename ValueType::Array ArrayType;
    static bool Is(const ValueType& v) { return v.IsArray(); }
    static ArrayType Get(ValueType& v) { return v.GetArray(); }
    static ValueType& Set(ValueType& v, ArrayType data) { return v = data; }
    static ValueType& Set(ValueType& v, ArrayType data, typename ValueType::AllocatorType&) { return v = data; }
};

template<typename ValueType>
struct TypeHelper<ValueType, typename ValueType::ConstArray> {
    typedef typename ValueType::ConstArray ArrayType;
    static bool Is(const ValueType& v) { return v.IsArray(); }
    static ArrayType Get(const ValueType& v) { return v.GetArray(); }
};

template<typename ValueType>
struct TypeHelper<ValueType, typename ValueType::Object> {
    typedef typename ValueType::Object ObjectType;
    static bool Is(const ValueType& v) { return v.IsObject(); }
    static ObjectType Get(ValueType& v) { return v.GetObject(); }
    static ValueType& Set(ValueType& v, ObjectType data) { return v = data; }
    static ValueType& Set(ValueType& v, ObjectType data, typename ValueType::AllocatorType&) { return v = data; }
};

template<typename ValueType>
struct TypeHelper<ValueType, typename ValueType::ConstObject> {
    typedef typename ValueType::ConstObject ObjectType;
    static bool Is(const ValueType& v) { return v.IsObject(); }
    static ObjectType Get(const ValueType& v) { return v.GetObject(); }
};

} // внутреннее пространство имен

// Форвардные декларации
template <bool, typename> class GenericArray;
template <bool, typename> class GenericObject;

///////////////////////////////////////////////////////////////////////////////
// GenericValue

//! имеет значение JSON. Используйте Value для кодировкиUTF8и дистрибутива по умолчанию.
/*!
    Значение JSON может быть одного из 7 типов. Этот класс представляет собой вариантный тип, поддерживающий
    эти типы.

    Используйте значение if UTF8 и распределитель по умолчанию.

    \tparam Кодировка Кодировка значения. (Даже нестроковые значения должны иметь одинаковую кодировку в документе)
    \tparam Тип распределителя для выделения памяти для объекта, массива и строки.
*/
template <typename Encoding, typename Allocator = RAPIDJSON_DEFAULT_ALLOCATOR >
class GenericValue {
public:
    //! Пара имя-значение в объекте.
    typedef GenericMember<Encoding, Allocator> Member;
    typedef Encoding EncodingType;                  //!< Тип кодировки из параметра шаблона.
    typedef Allocator AllocatorType;                //!< Тип распределителя из параметра шаблона.
    typedef typename Encoding::Ch Ch;               //!< Тип символа, полученный в результате кодирования.
    typedef GenericStringRef<Ch> StringRefType;     //!< Ссылка на константную строку
    typedef typename GenericMemberIterator<false,Encoding,Allocator>::Iterator MemberIterator;  //!< Итератор-член для итерации по объекту.
    typedef typename GenericMemberIterator<true,Encoding,Allocator>::Iterator ConstMemberIterator;  //!< Итератор постоянного члена для итерации по объекту.
    typedef GenericValue* ValueIterator;            //!< Итератор значений для перебора по массиву.
    typedef const GenericValue* ConstValueIterator; //!< Итератор постоянного значения для итерации в массиве.
    typedef GenericValue<Encoding, Allocator> ValueType;    //!< Тип значения самого себя.
    typedef GenericArray<false, ValueType> Array;
    typedef GenericArray<true, ValueType> ConstArray;
    typedef GenericObject<false, ValueType> Object;
    typedef GenericObject<true, ValueType> ConstObject;

    //! @nameКонструкторы и деструкторы.
    //@{

    //! Конструктор по умолчанию создает нулевое значение.
    GenericValue() RAPIDJSON_NOEXCEPT : data_() { data_.f.flags = kNullFlag; }

#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
    //! Переместить конструктор в C++11
    GenericValue(GenericValue&& rhs) RAPIDJSON_NOEXCEPT : data_(rhs.data_) {
        rhs.data_.f.flags = kNullFlag; // отказаться от содержания
    }
#endif

private:
    //! Копирование конструктора не разрешено.
    GenericValue(const GenericValue& rhs);

#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
    //! Перемещение из GenericDocument не разрешено.
    template <typename StackAllocator>
    GenericValue(GenericDocument<Encoding,Allocator,StackAllocator>&& rhs);

    //! Перемещение назначения из GenericDocument не разрешено.
    template <typename StackAllocator>
    GenericValue& operator=(GenericDocument<Encoding,Allocator,StackAllocator>&& rhs);
#endif

public:

    //! Конструктор с типом значения JSON.
    /*! При этом создается значение указанного типа с содержимым по умолчанию.
        \param type Тип значения.
        \note Содержимое номера по умолчанию равнозначно.
    */
    explicit GenericValue(Type type) RAPIDJSON_NOEXCEPT : data_() {
        static const uint16_t defaultFlags[] = {
            kNullFlag, kFalseFlag, kTrueFlag, kObjectFlag, kArrayFlag, kShortStringFlag,
            kNumberAnyFlag
        };
        RAPIDJSON_NOEXCEPT_ASSERT(type >= kNullType && type <= kNumberType);
        data_.f.flags = defaultFlags[type];

        // Используйте ShortString для хранения пустой строки.
        if (type == kStringType)
            data_.ss.SetLength(0);
    }

    //! Явный конструктор копирования (с распределителем)
    /*! Создает копию значения, используя данный распределитель.
        \tparam SourceAllocator распределитель \c rhs
        \param rhs Значение для копирования (только для чтения)
        \param allocator Распределитель для распределения скопированных элементов и буферов. Обычно GenericDocument::GetAllocator() .
        \param copyConstStrings Принудительное копирование константных строк (например, ссылка на локальный буфер)
        \см.  CopyFrom()
    */
    template <typename SourceAllocator>
    GenericValue(const GenericValue<Encoding,SourceAllocator>& rhs, Allocator& allocator, bool copyConstStrings = false) {
        switch (rhs.GetType()) {
        case kObjectType:
            DoCopyMembers(rhs, allocator, copyConstStrings);
            break;
        case kArrayType: {
                SizeType count = rhs.data_.a.size;
                GenericValue* le = reinterpret_cast<GenericValue*>(allocator.Malloc(count * sizeof(GenericValue)));
                const GenericValue<Encoding,SourceAllocator>* re = rhs.GetElementsPointer();
                for (SizeType i = 0; i < count; i++)
                    new (&le[i]) GenericValue(re[i], allocator, copyConstStrings);
                data_.f.flags = kArrayFlag;
                data_.a.size = data_.a.capacity = count;
                SetElementsPointer(le);
            }
            break;
        case kStringType:
            if (rhs.data_.f.flags == kConstStringFlag && !copyConstStrings) {
                data_.f.flags = rhs.data_.f.flags;
                data_  = *reinterpret_cast<const Data*>(&rhs.data_);
            }
            else
                SetStringRaw(StringRef(rhs.GetString(), rhs.GetStringLength()), allocator);
            break;
        default:
            data_.f.flags = rhs.data_.f.flags;
            data_  = *reinterpret_cast<const Data*>(&rhs.data_);
            break;
        }
    }

    //! Конструктор для логического значения.
    /*! \param b Логическое значение
        \note Этот конструктор \em ограничивает реальные логические значения и отклоняет
            неявно преобразованные типы, такие как произвольные указатели.  Используйте явное приведение типов
            в \c bool, если в таких случаях вы хотите создать логическое значениеJSON.
     */
#ifndef RAPIDJSON_DOXYGEN_RUNNING // скрытьSFINAEот Doxygen
    template <typename T>
    explicit GenericValue(T b, RAPIDJSON_ENABLEIF((internal::IsSame<bool, T>))) RAPIDJSON_NOEXCEPT  // См. № 472.
#else
    explicit GenericValue(bool b) RAPIDJSON_NOEXCEPT
#endif
        : data_() {
            // защита от сбоя SFINAE
            RAPIDJSON_STATIC_ASSERT((internal::IsSame<bool,T>::Value));
            data_.f.flags = b ? kTrueFlag : kFalseFlag;
    }

    //! Конструктор значений int.
    explicit GenericValue(int i) RAPIDJSON_NOEXCEPT : data_() {
        data_.n.i64 = i;
        data_.f.flags = (i >= 0) ? (kNumberIntFlag | kUintFlag | kUint64Flag) : kNumberIntFlag;
    }

    //! Конструктор для беззнакового значения.
    explicit GenericValue(unsigned u) RAPIDJSON_NOEXCEPT : data_() {
        data_.n.u64 = u;
        data_.f.flags = (u & 0x80000000) ? kNumberUintFlag : (kNumberUintFlag | kIntFlag | kInt64Flag);
    }

    //! Конструктор значений int64_t.
    explicit GenericValue(int64_t i64) RAPIDJSON_NOEXCEPT : data_() {
        data_.n.i64 = i64;
        data_.f.flags = kNumberInt64Flag;
        if (i64 >= 0) {
            data_.f.flags |= kNumberUint64Flag;
            if (!(static_cast<uint64_t>(i64) & RAPIDJSON_UINT64_C2(0xFFFFFFFF, 0x00000000)))
                data_.f.flags |= kUintFlag;
            if (!(static_cast<uint64_t>(i64) & RAPIDJSON_UINT64_C2(0xFFFFFFFF, 0x80000000)))
                data_.f.flags |= kIntFlag;
        }
        else if (i64 >= static_cast<int64_t>(RAPIDJSON_UINT64_C2(0xFFFFFFFF, 0x80000000)))
            data_.f.flags |= kIntFlag;
    }

    //! Конструктор значений uint64_t.
    explicit GenericValue(uint64_t u64) RAPIDJSON_NOEXCEPT : data_() {
        data_.n.u64 = u64;
        data_.f.flags = kNumberUint64Flag;
        if (!(u64 & RAPIDJSON_UINT64_C2(0x80000000, 0x00000000)))
            data_.f.flags |= kInt64Flag;
        if (!(u64 & RAPIDJSON_UINT64_C2(0xFFFFFFFF, 0x00000000)))
            data_.f.flags |= kUintFlag;
        if (!(u64 & RAPIDJSON_UINT64_C2(0xFFFFFFFF, 0x80000000)))
            data_.f.flags |= kIntFlag;
    }

    //! Конструктор для двойного значения.
    explicit GenericValue(double d) RAPIDJSON_NOEXCEPT : data_() { data_.n.d = d; data_.f.flags = kNumberDoubleFlag; }

    //! Конструктор для значения с плавающей запятой.
    explicit GenericValue(float f) RAPIDJSON_NOEXCEPT : data_() { data_.n.d = static_cast<double>(f); data_.f.flags = kNumberDoubleFlag; }

    //! Конструктор для константной строки (т.е. не копировать строку)
    GenericValue(const Ch* s, SizeType length) RAPIDJSON_NOEXCEPT : data_() { SetStringRaw(StringRef(s, length)); }

    //! Конструктор для константной строки (т.е. не копировать строку)
    explicit GenericValue(StringRefType s) RAPIDJSON_NOEXCEPT : data_() { SetStringRaw(s); }

    //! Конструктор для копирования строки (т.е. сделать копию строки)
    GenericValue(const Ch* s, SizeType length, Allocator& allocator) : data_() { SetStringRaw(StringRef(s, length), allocator); }

    //! Конструктор для копирования строки (т.е. сделать копию строки)
    GenericValue(const Ch*s, Allocator& allocator) : data_() { SetStringRaw(StringRef(s), allocator); }

#if RAPIDJSON_HAS_STDSTRING
    //! Конструктор для копирования строки из строкового объекта (т.е. создания копии строки)
    /*! \note Требуется определение символа препроцессора \refRAPIDJSON_HAS_STDSTRING.
     */
    GenericValue(const std::basic_string<Ch>& s, Allocator& allocator) : data_() { SetStringRaw(StringRef(s), allocator); }
#endif

    //! Конструктор массива.
    /*!
        \param Массив, полученный с помощью \cGetArray().
        \note \c Массив всегда возникает из-за обстоятельств.
        \note исходный массив изменяется в этом значении, и исходный массив становится пустым.
    */
    GenericValue(Array a) RAPIDJSON_NOEXCEPT : data_(a.value_.data_) {
        a.value_.data_ = Data();
        a.value_.data_.f.flags = kArrayFlag;
    }

    //! Конструктор объекта.
    /*!
        \param o Объект, полученный с помощью \cGetObject().
        \note \c Объект всегда возникает по причине.
        \note исходный объект изменяется в этом значении, и исходный объект становится пустым.
    */
    GenericValue(Object o) RAPIDJSON_NOEXCEPT : data_(o.value_.data_) {
        o.value_.data_ = Data();
        o.value_.data_.f.flags = kObjectFlag;
    }

    //! Деструктор.
    /*! Необходимо уничтожить элементы массива, члены объекта или копию строки.
    */
    ~GenericValue() {
        // С RAPIDJSON_USE_MEMBERSMAP карты необходимо уничтожить, чтобы выпустить.
        // их распределитель, если он пересчитан (например, MemoryPoolAllocator).
        if (Allocator::kNeedFree || (RAPIDJSON_USE_MEMBERSMAP+0 &&
                                     internal::IsRefCounted<Allocator>::Value)) {
            switch(data_.f.flags) {
            case kArrayFlag:
                {
                    GenericValue* e = GetElementsPointer();
                    for (GenericValue* v = e; v != e + data_.a.size; ++v)
                        v->~GenericValue();
                    if (Allocator::kNeedFree) { // Ярлык по признаку Распределитель
                        Allocator::Free(e);
                    }
                }
                break;

            case kObjectFlag:
                DoFreeMembers();
                break;

            case kCopyStringFlag:
                if (Allocator::kNeedFree) { // Ярлык по признаку Распределитель
                    Allocator::Free(const_cast<Ch*>(GetStringPointer()));
                }
                break;

            default:
                break;  // Ничего не делайте для других типов.
            }
        }
    }

    //@}

    //! @nameОператоры соответствия
    //@{

    //! Задание с семантикой перемещения.
    /*! \param rhs Источник задания. После произнесения оно станет нулевым значением.
    */
    GenericValue& operator=(GenericValue& rhs) RAPIDJSON_NOEXCEPT {
        if (RAPIDJSON_LIKELY(this != &rhs)) {
            // Невозможно удалить «this» перед присвоением «rhs», иначе — «rhs».
            // можно использовать после бесплатного, если это подзначение «this»,
            // отсюда и временный танец.
            GenericValue temp;
            temp.RawAssign(rhs);
            this->~GenericValue();
            RawAssign(temp);
        }
        return *this;
    }

#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
    //! Переместить назначение в C++11
    GenericValue& operator=(GenericValue&& rhs) RAPIDJSON_NOEXCEPT {
        return *this = rhs.Move();
    }
#endif

    //! Присвоение ссылки на константную строку (без копирования)
    /*! \param str Ссылка на константный текст, который будет назначен
        \note Эта перегрузка необходима во избежание последствий с приведенной ниже общей перегрузкой примитивного типа.
        \см. GenericStringRef, оператор=(T)
    */
    GenericValue& operator=(StringRefType str) RAPIDJSON_NOEXCEPT {
        GenericValue s(str);
        return *this = s;
    }

    //! Присваивание с примитивными типами.
    /*! \tparam T Либо \ref Тип, \c int, \c unsigned, \cint64_t, \c uint64_t
        \param value Назначаемое значение.

        \note Исходный тип \c T явно запрещает все типы указателей,
            особенно (\c const) \ref Ch*.  Это помогает избежать неявного
            для ссылки на строки символов с недостаточным временем жизни используйте
            \ref SetString(const Ch*, Allocator&) (для копирования) или
            Вместо этого \refStringRef()(чтобы явно пометить указатель как константу).
            Все остальные типы указателей будут неявно преобразованы в \c bool,
            вместо этого воспользуйтесь \refSetBool().
    */
    template <typename T>
    RAPIDJSON_DISABLEIF_RETURN((internal::IsPointer<T>), (GenericValue&))
    operator=(T value) {
        GenericValue v(value);
        return *this = v;
    }

    //! Назначение глубокого копирования из Value
    /*! Назначает копию \b значения текущему объекту значения.
        \tparam SourceAllocator Тип распределения \c rhs
        \param rhs Значение для копирования (только для чтения)
        \param allocator Распределитель, источник для копирования
        \param copyConstStrings Принудительное копирование константных строк (например, ссылка на локальный буфер)
     */
    template <typename SourceAllocator>
    GenericValue& CopyFrom(const GenericValue<Encoding, SourceAllocator>& rhs, Allocator& allocator, bool copyConstStrings = false) {
        RAPIDJSON_ASSERT(static_cast<void*>(this) != static_cast<void const*>(&rhs));
        this->~GenericValue();
        new (this) GenericValue(rhs, allocator, copyConstStrings);
        return *this;
    }

    //! Обменяйте содержимое этого значения содержимым другого.
    /*!
        \param другое Другое значение.
        \note Постоянная сложность.
    */
    GenericValue& Swap(GenericValue& other) RAPIDJSON_NOEXCEPT {
        GenericValue temp;
        temp.RawAssign(*this);
        RawAssign(other);
        other.RawAssign(temp);
        return *this;
    }

    //! отдельно стоящий помощник функции подкачки
    /*!
        Вспомогательная функция для включения поддержки реализации общего шаблона подкачки на основе \c std::swap:
        \code
        void swap(MyClass& a, MyClass& b) {
            с помощью станд::своп;
            своп(a.значение, b.значение);
            // ...
        }
        \endcode
        \см.  Swap()
     */
    friend inline void swap(GenericValue& a, GenericValue& b) RAPIDJSON_NOEXCEPT { a.Swap(b); }

    //! Подготовьте значение для семантики перемещения
    /*! \return *this */
    GenericValue& Move() RAPIDJSON_NOEXCEPT { return *this; }
    //@}

    //! @nameОператоры равны и не равно
    //@{
    //! Оператор равенства
    /*!
        \note Если объект содержит повторяющийся именованный элемент, сравнение сравнения с любым объектом всегда \c ложно.
        \note Сложность квадратичности по количеству членов объекта и линейной по всем параметрам штата (количество всех результатов в поддереве и длина всех строк).
    */
    template <typename SourceAllocator>
    bool operator==(const GenericValue<Encoding, SourceAllocator>& rhs) const {
        typedef GenericValue<Encoding, SourceAllocator> RhsType;
        if (GetType() != rhs.GetType())
            return false;

        switch (GetType()) {
        case kObjectType: // Warning: O(n^2) внутренний цикл
            if (data_.o.size != rhs.data_.o.size)
                return false;
            for (ConstMemberIterator lhsMemberItr = MemberBegin(); lhsMemberItr != MemberEnd(); ++lhsMemberItr) {
                typename RhsType::ConstMemberIterator rhsMemberItr = rhs.FindMember(lhsMemberItr->name);
                if (rhsMemberItr == rhs.MemberEnd() || lhsMemberItr->value != rhsMemberItr->value)
                    return false;
            }
            return true;

        case kArrayType:
            if (data_.a.size != rhs.data_.a.size)
                return false;
            for (SizeType i = 0; i < data_.a.size; i++)
                if ((*this)[i] != rhs[i])
                    return false;
            return true;

        case kStringType:
            return StringEqual(rhs);

        case kNumberType:
            if (IsDouble() || rhs.IsDouble()) {
                double a = GetDouble();     // Может конвертировать целое число в двойное.
                double b = rhs.GetDouble(); // то же самое
                return a >= b && a <= b;    // Предотвратить -Wfloat-равный
            }
            else
                return data_.n.u64 == rhs.data_.n.u64;

        default:
            return true;
        }
    }

    //! Оператор равенства с константным указателем на строку C
    bool operator==(const Ch* rhs) const { return *this == GenericValue(StringRef(rhs)); }

#if RAPIDJSON_HAS_STDSTRING
    //! Оператор равенства со строковым объектом
    /*! \note Требуется определение символа препроцессора \refRAPIDJSON_HAS_STDSTRING.
     */
    bool operator==(const std::basic_string<Ch>& rhs) const { return *this == GenericValue(StringRef(rhs)); }
#endif

    //! Оператор равенства с примитивными типами
    /*! \tparam T Либо \ref Type, \c int, \c unsigned, \cint64_t, \cuint64_t, \c double, \c true, \c false
    */
    template <typename T> RAPIDJSON_DISABLEIF_RETURN((internal::OrExpr<internal::IsPointer<T>,internal::IsGenericValue<T> >), (bool)) operator==(const T& rhs) const { return *this == GenericValue(rhs); }

#ifndef __cpp_impl_three_way_comparison
    //! Оператор «не равно»
    /*! \return !(*this == правая сторона)
     */
    template <typename SourceAllocator>
    bool operator!=(const GenericValue<Encoding, SourceAllocator>& rhs) const { return !(*this == rhs); }

    //! Оператор «не равно» с константным указателем на строку C
    bool operator!=(const Ch* rhs) const { return !(*this == rhs); }

    //! Оператор «не равно» с произвольными типами
    /*! \return !(*this == правая сторона)
     */
    template <typename T> RAPIDJSON_DISABLEIF_RETURN((internal::IsGenericValue<T>), (bool)) operator!=(const T& rhs) const { return !(*this == rhs); }

    //! Оператор равенства с произвольными типами (симметричная версия)
    /*! \return (правая сторона == левая сторона)
     */
    template <typename T> friend RAPIDJSON_DISABLEIF_RETURN((internal::IsGenericValue<T>), (bool)) operator==(const T& lhs, const GenericValue& rhs) { return rhs == lhs; }

    //! Оператор «Не равно» с произвольными типами (симметричная версия)
    /*! \return !(правая сторона == левая сторона)
     */
    template <typename T> friend RAPIDJSON_DISABLEIF_RETURN((internal::IsGenericValue<T>), (bool)) operator!=(const T& lhs, const GenericValue& rhs) { return !(rhs == lhs); }
    //@}
#endif

    //! Тип @name
    //@{

    Type GetType()  const { return static_cast<Type>(data_.f.flags & kTypeMask); }
    bool IsNull()   const { return data_.f.flags == kNullFlag; }
    bool IsFalse()  const { return data_.f.flags == kFalseFlag; }
    bool IsTrue()   const { return data_.f.flags == kTrueFlag; }
    bool IsBool()   const { return (data_.f.flags & kBoolFlag) != 0; }
    bool IsObject() const { return data_.f.flags == kObjectFlag; }
    bool IsArray()  const { return data_.f.flags == kArrayFlag; }
    bool IsNumber() const { return (data_.f.flags & kNumberFlag) != 0; }
    bool IsInt()    const { return (data_.f.flags & kIntFlag) != 0; }
    bool IsUint()   const { return (data_.f.flags & kUintFlag) != 0; }
    bool IsInt64()  const { return (data_.f.flags & kInt64Flag) != 0; }
    bool IsUint64() const { return (data_.f.flags & kUint64Flag) != 0; }
    bool IsDouble() const { return (data_.f.flags & kDoubleFlag) != 0; }
    bool IsString() const { return (data_.f.flags & kStringFlag) != 0; }

    // Проверяет, можно ли без потерь преобразовать число в двойное.
    bool IsLosslessDouble() const {
        if (!IsNumber()) return false;
        if (IsUint64()) {
            uint64_t u = GetUint64();
            volatile double d = static_cast<double>(u);
            return (d >= 0.0)
                && (d < static_cast<double>((std::numeric_limits<uint64_t>::max)()))
                && (u == static_cast<uint64_t>(d));
        }
        if (IsInt64()) {
            int64_t i = GetInt64();
            volatile double d = static_cast<double>(i);
            return (d >= static_cast<double>((std::numeric_limits<int64_t>::min)()))
                && (d < static_cast<double>((std::numeric_limits<int64_t>::max)()))
                && (i == static_cast<int64_t>(d));
        }
        return true; // double, int, uint всегда без потерь
    }

    // Проверяет, является ли число числом с плавающей точкой (возможно, с потерями).
    bool IsFloat() const  {
        if ((data_.f.flags & kDoubleFlag) == 0)
            return false;
        double d = GetDouble();
        return d >= -3.4028234e38 && d <= 3.4028234e38;
    }
    // Проверяет, может ли число быть преобразовано без потерь в число с плавающей запятой.
    bool IsLosslessFloat() const {
        if (!IsNumber()) return false;
        double a = GetDouble();
        if (a < static_cast<double>(-(std::numeric_limits<float>::max)())
                || a > static_cast<double>((std::numeric_limits<float>::max)()))
            return false;
        double b = static_cast<double>(static_cast<float>(a));
        return a >= b && a <= b;    // Предотвратить -Wfloat-равный
    }

    //@}

    //! @nameНуль
    //@{

    GenericValue& SetNull() { this->~GenericValue(); new (this) GenericValue(); return *this; }

    //@}

    //! @nameБул
    //@{

    bool GetBool() const { RAPIDJSON_ASSERT(IsBool()); return data_.f.flags == kTrueFlag; }
    //!< Установить логическое значение
    /*! \post IsBool() == true */
    GenericValue& SetBool(bool b) { this->~GenericValue(); new (this) GenericValue(b); return *this; }

    //@}

    //! Объект @name
    //@{

    //! Установите это значение как пустой объект.
    /*! \post IsObject() == true */
    GenericValue& SetObject() { this->~GenericValue(); new (this) GenericValue(kObjectType); return *this; }

    //! Получите количество членов в объекте.
    SizeType MemberCount() const { RAPIDJSON_ASSERT(IsObject()); return data_.o.size; }

    //! Получить емкость объекта.
    SizeType MemberCapacity() const { RAPIDJSON_ASSERT(IsObject()); return data_.o.capacity; }

    //! Проверьте, пуст ли объект.
    bool ObjectEmpty() const { RAPIDJSON_ASSERT(IsObject()); return data_.o.size == 0; }

    //! Получите значение из объекта, связанного с именем.
    /*! \pre IsObject() == true
        \tparam T Либо \c Ch, либо \c const \c Ch (шаблон, эффект для затруднения неоднозначности с помощью \ref оператора[](SizeType))
        \note В версии 0.1x, если элемент не найден, эта функция возвращает нулевое значение. Это создает проблему 7.
        Начиная с версии 0.2, если имя неверное, оно будет утверждаться.
        Если пользователь не уверен, что этот элемент существует, ему следует сначала использоватьHasMember().
        Лучшим подходом является использование FindMember().
        \note Линейная временная сложность.
    */
    template <typename T>
    RAPIDJSON_DISABLEIF_RETURN((internal::NotExpr<internal::IsSame<typename internal::RemoveConst<T>::Type, Ch> >),(GenericValue&)) operator[](T* name) {
        GenericValue n(StringRef(name));
        return (*this)[n];
    }
    template <typename T>
    RAPIDJSON_DISABLEIF_RETURN((internal::NotExpr<internal::IsSame<typename internal::RemoveConst<T>::Type, Ch> >),(const GenericValue&)) operator[](T* name) const { return const_cast<GenericValue&>(*this)[name]; }

    //! Получите значение из объекта, связанного с именем.
    /*! \pre IsObject() == true
        \tparam SourceAllocator Распределение значений имени \c

        \note По сравнению с \refoperator[](T*) эта версия работает быстрее, поскольку ей не требуетсяStrLen().
        И он также может обрабатывать строки со встроенными нулевыми символами.

        \note Линейная временная сложность.
    */
    template <typename SourceAllocator>
    GenericValue& operator[](const GenericValue<Encoding, SourceAllocator>& name) {
        MemberIterator member = FindMember(name);
        if (member != MemberEnd())
            return member->value;
        else {
            RAPIDJSON_ASSERT(false);    // см. примечание выше

#if RAPIDJSON_HAS_CXX11
            // Используйте локальное хранилище потоков, чтобы предотвратить гонки между потоками.
            // Используйте статический буфер и новое размещение, чтобы предотвратить разрушение, с
            // alignas() , чтобы обеспечить правильное соревнование.
            alignas(GenericValue) thread_local static char buffer[sizeof(GenericValue)];
            return *new (buffer) GenericValue();
#elif defined(_MSC_VER) && _MSC_VER < 1900
            // Невозможно решить как локальность потока, так и правильное выравнивание.
            // одновременно.
            __declspec(thread) static char buffer[sizeof(GenericValue)];
            return *new (buffer) GenericValue();
#elif defined(__GNUC__) || defined(__clang__)
            // Это создаст деструкторы -Wexit-time-destructors в clang, но это
            // лучше, чем иметь недостаточное выравнивание.
            __thread static GenericValue buffer;
            return buffer;
#else
            // Не знаю, что это за компилятор, поэтому не знаю, как гарантировать
            // локальность потока.
            static GenericValue buffer;
            return buffer;
#endif
        }
    }
    template <typename SourceAllocator>
    const GenericValue& operator[](const GenericValue<Encoding, SourceAllocator>& name) const { return const_cast<GenericValue&>(*this)[name]; }

#if RAPIDJSON_HAS_STDSTRING
    //! Получите значение из объекта, связанного с именем (строковый объект).
    GenericValue& operator[](const std::basic_string<Ch>& name) { return (*this)[GenericValue(StringRef(name))]; }
    const GenericValue& operator[](const std::basic_string<Ch>& name) const { return (*this)[GenericValue(StringRef(name))]; }
#endif

    //! Итератор константного члена
    /*! \pre IsObject() == true */
    ConstMemberIterator MemberBegin() const { RAPIDJSON_ASSERT(IsObject()); return ConstMemberIterator(GetMembersPointer()); }
    //! Const \em итератор конечного члена
    /*! \pre IsObject() == true */
    ConstMemberIterator MemberEnd() const   { RAPIDJSON_ASSERT(IsObject()); return ConstMemberIterator(GetMembersPointer() + data_.o.size); }
    //! Итератор члена
    /*! \pre IsObject() == true */
    MemberIterator MemberBegin()            { RAPIDJSON_ASSERT(IsObject()); return MemberIterator(GetMembersPointer()); }
    //! \em Прошедший итератор члена
    /*! \pre IsObject() == true */
    MemberIterator MemberEnd()              { RAPIDJSON_ASSERT(IsObject()); return MemberIterator(GetMembersPointer() + data_.o.size); }

    //! Запросите у объекта достаточную емкость для хранения участников.
    /*! \param newCapacity Эмкость того, что по мере должно иметь объект.
        \param allocator Распределитель для перераспределения памяти. Он должен быть таким же, как и раньше. Обычно GenericDocument::GetAllocator() .
        \return Само значение для беглогоAPI.
        \note Линейная временная сложность.
    */
    GenericValue& MemberReserve(SizeType newCapacity, Allocator &allocator) {
        RAPIDJSON_ASSERT(IsObject());
        DoReserveMembers(newCapacity, allocator);
        return *this;
    }

    //! Проверьте, существует ли член в объекте.
    /*!
        \param name Имя элемента для поиска.
        \pre IsObject() == true
        \return Существует ли элемент с таким именем.
        \note Лучше использоватьFindMember()напрямую, если вам также необходимо получить значение.
        \note Линейная временная сложность.
    */
    bool HasMember(const Ch* name) const { return FindMember(name) != MemberEnd(); }

#if RAPIDJSON_HAS_STDSTRING
    //! Проверьте, существует ли член в объекте с помощью строкового объекта.
    /*!
        \param name Имя элемента для поиска.
        \pre IsObject() == true
        \return Существует ли элемент с таким именем.
        \note Лучше использоватьFindMember()напрямую, если вам также необходимо получить значение.
        \note Линейная временная сложность.
    */
    bool HasMember(const std::basic_string<Ch>& name) const { return FindMember(name) != MemberEnd(); }
#endif

    //! Проверьте, существует ли в объекте член с именем GenericValue.
    /*!
        Эта версия работает быстрее, поскольку ей не требуется StrLen(). Он также может обрабатывать обращение нулевым символом.
        \param name Имя элемента для поиска.
        \pre IsObject() == true
        \return Существует ли элемент с таким именем.
        \note Лучше использоватьFindMember()напрямую, если вам также необходимо получить значение.
        \note Линейная временная сложность.
    */
    template <typename SourceAllocator>
    bool HasMember(const GenericValue<Encoding, SourceAllocator>& name) const { return FindMember(name) != MemberEnd(); }

    //! Найдите участника по имени.
    /*!
        \param name Имя элемента для поиска.
        \pre IsObject() == true
        \return Итератор к члену, если он существует.
            В противном случае возвращается \ref MemberEnd().

        \note Более ранняя версия Rapidjson возвращаемые указатели \cNULLна случай, если
            запрошенный член не существует. Для согласованности, например.
            \c std::map, теперь это было изменено наMemberEnd().
        \note Линейная временная сложность.
    */
    MemberIterator FindMember(const Ch* name) {
        GenericValue n(StringRef(name));
        return FindMember(n);
    }

    ConstMemberIterator FindMember(const Ch* name) const { return const_cast<GenericValue&>(*this).FindMember(name); }

    //! Найдите участника по имени.
    /*!
        Эта версия работает быстрее, поскольку ей не требуется StrLen(). Он также может обрабатывать обращение нулевым символом.
        \param name Имя элемента для поиска.
        \pre IsObject() == true
        \return Итератор к члену, если он существует.
            В противном случае возвращается \ref MemberEnd().

        \note Более ранняя версия Rapidjson возвращаемые указатели \cNULLна случай, если
            запрошенный член не существует. Для согласованности, например.
            \c std::map, теперь это было изменено наMemberEnd().
        \note Линейная временная сложность.
    */
    template <typename SourceAllocator>
    MemberIterator FindMember(const GenericValue<Encoding, SourceAllocator>& name) {
        RAPIDJSON_ASSERT(IsObject());
        RAPIDJSON_ASSERT(name.IsString());
        return DoFindMember(name);
    }
    template <typename SourceAllocator> ConstMemberIterator FindMember(const GenericValue<Encoding, SourceAllocator>& name) const { return const_cast<GenericValue&>(*this).FindMember(name); }

#if RAPIDJSON_HAS_STDSTRING
    //! Найти члена по имени строкового объекта.
    /*!
        \param name Имя элемента для поиска.
        \pre IsObject() == true
        \return Итератор к члену, если он существует.
            В противном случае возвращается \ref MemberEnd().
    */
    MemberIterator FindMember(const std::basic_string<Ch>& name) { return FindMember(GenericValue(StringRef(name))); }
    ConstMemberIterator FindMember(const std::basic_string<Ch>& name) const { return FindMember(GenericValue(StringRef(name))); }
#endif

    //! Добавьте член (пару имя-значение) к объекту.
    /*! \param name Строковое значение в качестве имени члена.
        \param value Значение любого типа.
        \param allocator Распределитель для перераспределения памяти. Он должен быть таким же, как и раньше. Обычно GenericDocument::GetAllocator() .
        \return Само значение для беглогоAPI.
        \note В случае успеха право собственности на имя \c и значение \c будет передано этому объекту.
        \preIsObject()&& имя.  IsString()
        \имя сообщения. IsNull()&& значение.  IsNull()
        \note Амортизированная постоянная временная характеристика.
    */
    GenericValue& AddMember(GenericValue& name, GenericValue& value, Allocator& allocator) {
        RAPIDJSON_ASSERT(IsObject());
        RAPIDJSON_ASSERT(name.IsString());
        DoAddMember(name, value, allocator);
        return *this;
    }

    //! Добавьте к объекту постоянное строковое значение в качестве члена (пара имя-значение).
    /*! \param name Строковое значение в качестве имени члена.
        \param value — ссылка на константный текст как значение члена.
        \param allocator Распределитель для перераспределения памяти. Он должен быть таким же, как и раньше. Обычно GenericDocument::GetAllocator() .
        \return Само значение для беглогоAPI.
        \pre IsObject()
        \note Эта перегрузка необходима во избежание ошибок с перегрузкой универсального примитивного типа AddMember(GenericValue&,T,Allocator&), приведенного ниже.
        \note Амортизированная постоянная временная характеристика.
    */
    GenericValue& AddMember(GenericValue& name, StringRefType value, Allocator& allocator) {
        GenericValue v(value);
        return AddMember(name, v, allocator);
    }

#if RAPIDJSON_HAS_STDSTRING
    //! Добавьте к объекту строковый объект в качестве члена (пара имя-значение).
    /*! \param name Строковое значение в качестве имени члена.
        \param value — ссылка на константный текст как значение члена.
        \param allocator Распределитель для перераспределения памяти. Он должен быть таким же, как и раньше. Обычно GenericDocument::GetAllocator() .
        \return Само значение для беглогоAPI.
        \pre IsObject()
        \note Эта перегрузка необходима во избежание ошибок с перегрузкой универсального примитивного типа AddMember(GenericValue&,T,Allocator&), приведенного ниже.
        \note Амортизированная постоянная временная характеристика.
    */
    GenericValue& AddMember(GenericValue& name, std::basic_string<Ch>& value, Allocator& allocator) {
        GenericValue v(value, allocator);
        return AddMember(name, v, allocator);
    }
#endif

    //! Добавьте любое примитивное значение в качестве члена (пара имя-значение) к объекту.
    /*! \tparam T Либо \ref Тип, \c int, \c unsigned, \cint64_t, \c uint64_t
        \param name Строковое значение в качестве имени члена.
        \param value Значение примитивного типа \c T как значение члена
        \param allocator Распределитель для перераспределения памяти. Обычно GenericDocument::GetAllocator() .
        \return Само значение для беглогоAPI.
        \pre IsObject()

        \note Исходный тип \c T явно запрещает все типы указателей,
            особенно (\c const) \ref Ch*.  Это помогает избежать неявного
            для ссылки на строки символов с недостаточным временем жизни используйте
            \ref AddMember(StringRefType, GenericValue&, Allocator&) или \ref
            AddMember(StringRefType, StringRefType, Распределитель&).
            Все остальные типы указателей будут неявно преобразованы в \c bool,
            вместо этого используйте явное приведение, если необходимо.
        \note Амортизированная постоянная временная характеристика.
    */
    template <typename T>
    RAPIDJSON_DISABLEIF_RETURN((internal::OrExpr<internal::IsPointer<T>, internal::IsGenericValue<T> >), (GenericValue&))
    AddMember(GenericValue& name, T value, Allocator& allocator) {
        GenericValue v(value);
        return AddMember(name, v, allocator);
    }

#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
    GenericValue& AddMember(GenericValue&& name, GenericValue&& value, Allocator& allocator) {
        return AddMember(name, value, allocator);
    }
    GenericValue& AddMember(GenericValue&& name, GenericValue& value, Allocator& allocator) {
        return AddMember(name, value, allocator);
    }
    GenericValue& AddMember(GenericValue& name, GenericValue&& value, Allocator& allocator) {
        return AddMember(name, value, allocator);
    }
    GenericValue& AddMember(StringRefType name, GenericValue&& value, Allocator& allocator) {
        GenericValue n(name);
        return AddMember(n, value, allocator);
    }
#endif // RAPIDJSON_HAS_CXX11_RVALUE_REFS


    //! Добавьте член (пару имя-значение) к объекту.
    /*! \param name Ссылка на константную букву имени члена.
        \param value Значение любого типа.
        \param allocator Распределитель для перераспределения памяти. Он должен быть таким же, как и раньше. Обычно GenericDocument::GetAllocator() .
        \return Само значение для беглогоAPI.
        \note В случае успеха право собственности на значение \c будет передано этому объекту.
        \pre IsObject()
        \ опубликовать значение.  IsNull()
        \note Амортизированная постоянная временная характеристика.
    */
    GenericValue& AddMember(StringRefType name, GenericValue& value, Allocator& allocator) {
        GenericValue n(name);
        return AddMember(n, value, allocator);
    }

    //! Добавьте к объекту постоянное строковое значение в качестве члена (пара имя-значение).
    /*! \param name Ссылка на константную букву имени члена.
        \param value — ссылка на константный текст как значение члена.
        \param allocator Распределитель для перераспределения памяти. Он должен быть таким же, как и раньше. Обычно GenericDocument::GetAllocator() .
        \return Само значение для беглогоAPI.
        \pre IsObject()
        \note Эта перегрузка необходима во избежание ошибок при перегрузке универсального примитивного типа AddMember(StringRefType,T,Allocator&), приведенного ниже.
        \note Амортизированная постоянная временная характеристика.
    */
    GenericValue& AddMember(StringRefType name, StringRefType value, Allocator& allocator) {
        GenericValue v(value);
        return AddMember(name, v, allocator);
    }

    //! Добавьте любое примитивное значение в качестве члена (пара имя-значение) к объекту.
    /*! \tparam T Либо \ref Тип, \c int, \c unsigned, \cint64_t, \c uint64_t
        \param name Ссылка на константную букву имени члена.
        \param value Значение примитивного типа \c T как значение члена
        \param allocator Распределитель для перераспределения памяти. Обычно GenericDocument::GetAllocator() .
        \return Само значение для беглогоAPI.
        \pre IsObject()

        \note Исходный тип \c T явно запрещает все типы указателей,
            особенно (\c const) \ref Ch*.  Это помогает избежать неявного
            для ссылки на строки символов с недостаточным временем жизни используйте
            \ref AddMember(StringRefType, GenericValue&, Allocator&) или \ref
            AddMember(StringRefType, StringRefType, Распределитель&).
            Все остальные типы указателей будут неявно преобразованы в \c bool,
            вместо этого используйте явное приведение, если необходимо.
        \note Амортизированная постоянная временная характеристика.
    */
    template <typename T>
    RAPIDJSON_DISABLEIF_RETURN((internal::OrExpr<internal::IsPointer<T>, internal::IsGenericValue<T> >), (GenericValue&))
    AddMember(StringRefType name, T value, Allocator& allocator) {
        GenericValue n(name);
        return AddMember(n, value, allocator);
    }

    //! Удалить все члены объекта.
    /*! Эта функция не освобождает память в объекте, т.е. емкость не изменяется.
        \note Линейная временная сложность.
    */
    void RemoveAllMembers() {
        RAPIDJSON_ASSERT(IsObject());
        DoClearMembers();
    }

    //! Удалить член объекта по его имени.
    /*! \param name Имя удаляемого участника.
        \return Существовал ли этот член.
        \note Эта функция может изменить порядок сотрудников объекта. Используйте \ref
            EraseMember(ConstMemberIterator), если вам нужно сохранить
            относительный порядок остальных членов.
        \note Линейная временная сложность.
    */
    bool RemoveMember(const Ch* name) {
        GenericValue n(StringRef(name));
        return RemoveMember(n);
    }

#if RAPIDJSON_HAS_STDSTRING
    bool RemoveMember(const std::basic_string<Ch>& name) { return RemoveMember(GenericValue(StringRef(name))); }
#endif

    template <typename SourceAllocator>
    bool RemoveMember(const GenericValue<Encoding, SourceAllocator>& name) {
        MemberIterator m = FindMember(name);
        if (m != MemberEnd()) {
            RemoveMember(m);
            return true;
        }
        else
            return false;
    }

    //! Удалить член объекта с помощью итератора.
    /*! Итератор-член \param m (полученныйFindMember()илиMemberBegin()).
        \вернуть новый итератор после удаления.
        \note Эта функция может изменить порядок сотрудников объекта. Используйте \ref
            EraseMember(ConstMemberIterator), если вам нужно сохранить
            относительный порядок остальных членов.
        \note Постоянная временная сложность.
    */
    MemberIterator RemoveMember(MemberIterator m) {
        RAPIDJSON_ASSERT(IsObject());
        RAPIDJSON_ASSERT(data_.o.size > 0);
        RAPIDJSON_ASSERT(GetMembersPointer() != 0);
        RAPIDJSON_ASSERT(m >= MemberBegin() && m < MemberEnd());
        return DoRemoveMember(m);
    }

    //! Удалить член объекта с помощью итератора.
    /*! \param pos итератор для удаляемого элемента
        \pre IsObject() == true && \ref MemberBegin() <= \c pos < \ref MemberEnd()
        \return Итератор, следующий за удаленным элементом.
            Если итератор \c pos ссылается на последний элемент, возвращается итератор \refMemberEnd().
        \note Эта функция сохраняет соответствующий порядок на территории.
            член. Если вам это не нужно, используйте более эффективный метод \ref RemoveMember(MemberIterator).
        \note Линейная временная сложность.
    */
    MemberIterator EraseMember(ConstMemberIterator pos) {
        return EraseMember(pos, pos +1);
    }

    //! Удалить элементы в диапазоне [первый, последний) из объекта.
    /*! \param первый итератор для первого удаленного члена
        \param последний итератор, следующий за последним удаляемым элементом
        \preIsObject()== true && \refMemberBegin()<= \c первый <= \c последний <= \ref MemberEnd()
        \return Итератор, следующий за последним удаленным элементом.
        \note Эта функция сохраняет соответствующий порядок на территории.
            члены.
        \note Линейная временная сложность.
    */
    MemberIterator EraseMember(ConstMemberIterator first, ConstMemberIterator last) {
        RAPIDJSON_ASSERT(IsObject());
        RAPIDJSON_ASSERT(data_.o.size > 0);
        RAPIDJSON_ASSERT(GetMembersPointer() != 0);
        RAPIDJSON_ASSERT(first >= MemberBegin());
        RAPIDJSON_ASSERT(first <= last);
        RAPIDJSON_ASSERT(last <= MemberEnd());
        return DoEraseMembers(first, last);
    }

    //! Удалить член объекта по его имени.
    /*! \param name Имя удаляемого участника.
        \return Существовал ли этот член.
        \note Линейная временная сложность.
    */
    bool EraseMember(const Ch* name) {
        GenericValue n(StringRef(name));
        return EraseMember(n);
    }

#if RAPIDJSON_HAS_STDSTRING
    bool EraseMember(const std::basic_string<Ch>& name) { return EraseMember(GenericValue(StringRef(name))); }
#endif

    template <typename SourceAllocator>
    bool EraseMember(const GenericValue<Encoding, SourceAllocator>& name) {
        MemberIterator m = FindMember(name);
        if (m != MemberEnd()) {
            EraseMember(m);
            return true;
        }
        else
            return false;
    }

    Object GetObject() { RAPIDJSON_ASSERT(IsObject()); return Object(*this); }
    Object GetObj() { RAPIDJSON_ASSERT(IsObject()); return Object(*this); }
    ConstObject GetObject() const { RAPIDJSON_ASSERT(IsObject()); return ConstObject(*this); }
    ConstObject GetObj() const { RAPIDJSON_ASSERT(IsObject()); return ConstObject(*this); }

    //@}

    //! @nameМассив
    //@{

    //! Установите это значение как пустой массив.
    /*! \post IsArray == true */
    GenericValue& SetArray() { this->~GenericValue(); new (this) GenericValue(kArrayType); return *this; }

    //! Получить количество элементов в массиве.
    SizeType Size() const { RAPIDJSON_ASSERT(IsArray()); return data_.a.size; }

    //! Получить емкость массива.
    SizeType Capacity() const { RAPIDJSON_ASSERT(IsArray()); return data_.a.capacity; }

    //! Проверьте, пуст ли массив.
    bool Empty() const { RAPIDJSON_ASSERT(IsArray()); return data_.a.size == 0; }

    //! Удалить все элементы массива.
    /*! Эта функция не освобождает память в массиве, т.е. емкость не изменяется.
        \note Линейная временная сложность.
    */
    void Clear() {
        RAPIDJSON_ASSERT(IsArray());
        GenericValue* e = GetElementsPointer();
        for (GenericValue* v = e; v != e + data_.a.size; ++v)
            v->~GenericValue();
        data_.a.size = 0;
    }

    //! Получить элемент из массива по индексу.
    /*! \pre IsArray() == true
        \param index Индекс элемента, отсчитываемый от нуля.
        \см. оператор[](T*)
    */
    GenericValue& operator[](SizeType index) {
        RAPIDJSON_ASSERT(IsArray());
        RAPIDJSON_ASSERT(index < data_.a.size);
        return GetElementsPointer()[index];
    }
    const GenericValue& operator[](SizeType index) const { return const_cast<GenericValue&>(*this)[index]; }

    //! Итератор элемента
    /*! \pre IsArray() == true */
    ValueIterator Begin() { RAPIDJSON_ASSERT(IsArray()); return GetElementsPointer(); }
    //! \em Итератор элемента прошедшего конца
    /*! \pre IsArray() == true */
    ValueIterator End() { RAPIDJSON_ASSERT(IsArray()); return GetElementsPointer() + data_.a.size; }
    //! Итератор постоянного элемента
    /*! \pre IsArray() == true */
    ConstValueIterator Begin() const { return const_cast<GenericValue&>(*this).Begin(); }
    //! Константный итератор элемента \em после конца
    /*! \pre IsArray() == true */
    ConstValueIterator End() const { return const_cast<GenericValue&>(*this).End(); }

    //! Запросите у массива достаточную емкость для хранения элементов.
    /*! \param newCapacity Емкость, которая по мере мере должна иметь массив.
        \param allocator Распределитель для перераспределения памяти. Он должен быть таким же, как и раньше. Обычно GenericDocument::GetAllocator() .
        \return Само значение для беглогоAPI.
        \note Линейная временная сложность.
    */
    GenericValue& Reserve(SizeType newCapacity, Allocator &allocator) {
        RAPIDJSON_ASSERT(IsArray());
        if (newCapacity > data_.a.capacity) {
            SetElementsPointer(reinterpret_cast<GenericValue*>(allocator.Realloc(GetElementsPointer(), data_.a.capacity * sizeof(GenericValue), newCapacity * sizeof(GenericValue))));
            data_.a.capacity = newCapacity;
        }
        return *this;
    }

    //! Добавьте GenericValue в конец массива.
    /*! \param value Добавляемое значение.
        \param allocator Распределитель для перераспределения памяти. Он должен быть таким же, как и раньше. Обычно GenericDocument::GetAllocator() .
        \pre IsArray() == true
        \ опубликовать значение. IsNull()== правда
        \return Само значение для беглогоAPI.
        \note В случае успеха право собственности на значение \c будет передано этому массиву.
        \note Если известно количество добавленных элементов, вызовReserve()один раз может оказаться более эффективным.
        \note Амортизированная постоянная временная характеристика.
    */
    GenericValue& PushBack(GenericValue& value, Allocator& allocator) {
        RAPIDJSON_ASSERT(IsArray());
        if (data_.a.size >= data_.a.capacity)
            Reserve(data_.a.capacity == 0 ? kDefaultArrayCapacity : (data_.a.capacity + (data_.a.capacity + 1) / 2), allocator);
        GetElementsPointer()[data_.a.size++].RawAssign(value);
        return *this;
    }

#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
    GenericValue& PushBack(GenericValue&& value, Allocator& allocator) {
        return PushBack(value, allocator);
    }
#endif // RAPIDJSON_HAS_CXX11_RVALUE_REFS

    //! Добавьте ссылку на константную строку в конец массива.
    /*! \param value Ссылка на константный текст, который будет добавлен.
        \param allocator Распределитель для перераспределения памяти. Это должен быть тот самый вариант, который использовался ранее. Обычно GenericDocument::GetAllocator() .
        \pre IsArray() == true
        \return Само значение для беглогоAPI.
        \note Если известно количество добавленных элементов, вызовReserve()один раз может оказаться более эффективным.
        \note Амортизированная постоянная временная характеристика.
        \см. GenericStringRef
    */
    GenericValue& PushBack(StringRefType value, Allocator& allocator) {
        return (*this).template PushBack<StringRefType>(value, allocator);
    }

    //! Добавьте примитивное значение в конец массива.
    /*! \tparam T Либо \ref Тип, \c int, \c unsigned, \cint64_t, \c uint64_t
        \param value Добавляемое значение примитивного типа T.
        \param allocator Распределитель для перераспределения памяти. Он должен быть таким же, как и раньше. Обычно GenericDocument::GetAllocator() .
        \pre IsArray() == true
        \return Само значение для беглогоAPI.
        \note Если известно количество добавленных элементов, вызовReserve()один раз может оказаться более эффективным.

        \note Исходный тип \c T явно запрещает все типы указателей,
            особенно (\c const) \ref Ch*.  Это помогает избежать неявного
            для ссылки на строки символов с недостаточным временем жизни используйте
            \ref PushBack(GenericValue&, Allocator&) или \ref
            PushBack(StringRefType, Распределитель&).
            Все остальные типы указателей будут неявно преобразованы в \c bool,
            вместо этого используйте явное приведение, если необходимо.
        \note Амортизированная постоянная временная характеристика.
    */
    template <typename T>
    RAPIDJSON_DISABLEIF_RETURN((internal::OrExpr<internal::IsPointer<T>, internal::IsGenericValue<T> >), (GenericValue&))
    PushBack(T value, Allocator& allocator) {
        GenericValue v(value);
        return PushBack(v, allocator);
    }

    //! Удалить последний элемент массива.
    /*!
        \note Постоянная временная сложность.
    */
    GenericValue& PopBack() {
        RAPIDJSON_ASSERT(IsArray());
        RAPIDJSON_ASSERT(!Empty());
        GetElementsPointer()[--data_.a.size].~GenericValue();
        return *this;
    }

    //! Удалить элемент массива с помощью итератора.
    /*!
        \param pos итератор элемента, который нужно удалить
        \pre IsArray() == true && \ref Begin() <= \c pos < \ref End()
        \return Итератор, следующий за удаленным элементом. Если итератор pos ссылается на последний элемент, возвращается итераторEnd().
        \note Линейная временная сложность.
    */
    ValueIterator Erase(ConstValueIterator pos) {
        return Erase(pos, pos + 1);
    }

    //! Удалить элементы в диапазоне [первый, последний) массива.
    /*!
        \param первый итератор первого удаляемого элемента
        \param последний итератор, следующий за последним удаляемым элементом
        \preIsArray()== true && \refBegin()<= \c первый <= \c последний <= \ref End()
        \return Итератор, следующий за последним удаленным элементом.
        \note Линейная временная сложность.
    */
    ValueIterator Erase(ConstValueIterator first, ConstValueIterator last) {
        RAPIDJSON_ASSERT(IsArray());
        RAPIDJSON_ASSERT(data_.a.size > 0);
        RAPIDJSON_ASSERT(GetElementsPointer() != 0);
        RAPIDJSON_ASSERT(first >= Begin());
        RAPIDJSON_ASSERT(first <= last);
        RAPIDJSON_ASSERT(last <= End());
        ValueIterator pos = Begin() + (first - Begin());
        for (ValueIterator itr = pos; itr != last; ++itr)
            itr->~GenericValue();
        std::memmove(static_cast<void*>(pos), last, static_cast<size_t>(End() - last) * sizeof(GenericValue));
        data_.a.size -= static_cast<SizeType>(last - first);
        return pos;
    }

    Array GetArray() { RAPIDJSON_ASSERT(IsArray()); return Array(*this); }
    ConstArray GetArray() const { RAPIDJSON_ASSERT(IsArray()); return ConstArray(*this); }

    //@}

    //! @nameНомер
    //@{

    int GetInt() const          { RAPIDJSON_ASSERT(data_.f.flags & kIntFlag);   return data_.n.i.i;   }
    unsigned GetUint() const    { RAPIDJSON_ASSERT(data_.f.flags & kUintFlag);  return data_.n.u.u;   }
    int64_t GetInt64() const    { RAPIDJSON_ASSERT(data_.f.flags & kInt64Flag); return data_.n.i64; }
    uint64_t GetUint64() const  { RAPIDJSON_ASSERT(data_.f.flags & kUint64Flag); return data_.n.u64; }

    //! Получите значение как двойной тип.
    /*! \note Если значение представляет собой 64-битное число, оно может потерять точность. Используйте \cIsLosslessDouble(), чтобы проверить, происходит ли преобразование без потерь.
    */
    double GetDouble() const {
        RAPIDJSON_ASSERT(IsNumber());
        if ((data_.f.flags & kDoubleFlag) != 0)                return data_.n.d;   // точный тип, без преобразования.
        if ((data_.f.flags & kIntFlag) != 0)                   return data_.n.i.i; // интервал -> двойной
        if ((data_.f.flags & kUintFlag) != 0)                  return data_.n.u.u; // без знака -> двойной
        if ((data_.f.flags & kInt64Flag) != 0)                 return static_cast<double>(data_.n.i64); // int64_t -> двойной (может потерять точность)
        RAPIDJSON_ASSERT((data_.f.flags & kUint64Flag) != 0);  return static_cast<double>(data_.n.u64); // uint64_t -> двойной (может потерять точность)
    }

    //! Получите значение типа float.
    /*! \note Если значение представляет собой 64-битное число, оно может потерять точность. Используйте \cIsLosslessFloat(), чтобы проверить, происходит ли преобразование без потерь.
    */
    float GetFloat() const {
        return static_cast<float>(GetDouble());
    }

    GenericValue& SetInt(int i)             { this->~GenericValue(); new (this) GenericValue(i);    return *this; }
    GenericValue& SetUint(unsigned u)       { this->~GenericValue(); new (this) GenericValue(u);    return *this; }
    GenericValue& SetInt64(int64_t i64)     { this->~GenericValue(); new (this) GenericValue(i64);  return *this; }
    GenericValue& SetUint64(uint64_t u64)   { this->~GenericValue(); new (this) GenericValue(u64);  return *this; }
    GenericValue& SetDouble(double d)       { this->~GenericValue(); new (this) GenericValue(d);    return *this; }
    GenericValue& SetFloat(float f)         { this->~GenericValue(); new (this) GenericValue(static_cast<double>(f)); return *this; }

    //@}

    //! @nameСтрока
    //@{

    const Ch* GetString() const { RAPIDJSON_ASSERT(IsString()); return DataString(data_); }

    //! Получить длину строки.
    /*! поскольку Rapidjson допускает использование "\\u0000" в строке json, strlen(v.GetString()) может не равняться v.GetStringLength().
    */
    SizeType GetStringLength() const { RAPIDJSON_ASSERT(IsString()); return DataStringLength(data_); }

    //! Установите это значение как строку, не копируя исходную строку.
    /*! Эта версия имеет лучшую производительность с указанной длиной, а также поддерживает строку, содержащую нулевой символ.
        \param указатель исходной строки.
        \param length Длина исходной строки, завершающая нулевой признак.
        \return Само значение для беглогоAPI.
        \postIsString()== true &&GetString()== s &&GetStringLength()== длина
        \см. SetString(StringRefType)
    */
    GenericValue& SetString(const Ch* s, SizeType length) { return SetString(StringRef(s, length)); }

    //! Установите это значение как строку, не копируя исходную строку.
    /*! Ссылка на исходный текст \param s
        \return Само значение для беглогоAPI.
        \post IsString() == true && GetString() == s && GetStringLength() == s.length
    */
    GenericValue& SetString(StringRefType s) { this->~GenericValue(); SetStringRaw(s); return *this; }

    //! Установите это значение как строку, скопировав исходную строку.
    /*! Эта версия имеет лучшую производительность с указанной длиной, а также поддерживает строку, содержащую нулевой символ.
        Исходная строка \param s.
        \param length Длина исходной строки, завершающая нулевой признак.
        \param allocator Распределитель для выделения скопированного буфера. Обычно GenericDocument::GetAllocator() .
        \return Само значение для беглогоAPI.
        \postIsString()== true &&GetString()!= s && strcmp(GetString(),s) == 0 &&GetStringLength()== длина
    */
    GenericValue& SetString(const Ch* s, SizeType length, Allocator& allocator) { return SetString(StringRef(s, length), allocator); }

    //! Установите это значение как строку, скопировав исходную строку.
    /*! Исходная строка \param s.
        \param allocator Распределитель для выделения скопированного буфера. Обычно GenericDocument::GetAllocator() .
        \return Само значение для беглогоAPI.
        \postIsString()== true &&GetString()!= s && strcmp(GetString(),s) == 0 &&GetStringLength()== длина
    */
    GenericValue& SetString(const Ch* s, Allocator& allocator) { return SetString(StringRef(s), allocator); }

    //! Установите это значение как строку, скопировав исходную строку.
    /*! Ссылка на исходный текст \param s
        \param allocator Распределитель для выделения скопированного буфера. Обычно GenericDocument::GetAllocator() .
        \return Само значение для беглогоAPI.
        \postIsString()== true &&GetString()!= s.s && strcmp(GetString(),s) == 0 &&GetStringLength()== длина
    */
    GenericValue& SetString(StringRefType s, Allocator& allocator) { this->~GenericValue(); SetStringRaw(s, allocator); return *this; }

#if RAPIDJSON_HAS_STDSTRING
    //! Установите это значение как строку, скопировав исходную строку.
    /*! Исходная строка \param s.
        \param allocator Распределитель для выделения скопированного буфера. Обычно GenericDocument::GetAllocator() .
        \return Само значение для беглогоAPI.
        \postIsString()== true &&GetString()!= s. data()&& strcmp(GetString(),s.data()== 0 &&GetStringLength()== s. size()
        \note Требуется определение символа препроцессора \refRAPIDJSON_HAS_STDSTRING.
    */
    GenericValue& SetString(const std::basic_string<Ch>& s, Allocator& allocator) { return SetString(StringRef(s), allocator); }
#endif

    //@}

    //! @nameМассив
    //@{

    //! Шаблонная версия для проверки того, является ли это значение типом T.
    /*!
        \tparam T Либо \c bool, \c int, \c unsigned, \cint64_t, \cuint64_t, \c double, \c float, \c const \c char*, \c std::basic_string<Ch>
    */
    template <typename T>
    bool Is() const { return internal::TypeHelper<ValueType, T>::Is(*this); }

    template <typename T>
    T Get() const { return internal::TypeHelper<ValueType, T>::Get(*this); }

    template <typename T>
    T Get() { return internal::TypeHelper<ValueType, T>::Get(*this); }

    template<typename T>
    ValueType& Set(const T& data) { return internal::TypeHelper<ValueType, T>::Set(*this, data); }

    template<typename T>
    ValueType& Set(const T& data, AllocatorType& allocator) { return internal::TypeHelper<ValueType, T>::Set(*this, data, allocator); }

    //@}

    //! Генерируйте события этого значения для обработчика.
    /*! Эта функция использует шаблон посетителя GoF.
        Типичное использование — выведите это значениеJSONв виде текстаJSONчерез Writer, который является обработчиком.
        Его также можно использовать для глубокого клонирования этого значения через GenericDocument, который также является обработчиком.
        \tparam Handler Тип обработчика.
        \param handler Объект, реализующий создание Handler.
    */
    template <typename Handler>
    bool Accept(Handler& handler) const {
        switch(GetType()) {
        case kNullType:     return handler.Null();
        case kFalseType:    return handler.Bool(false);
        case kTrueType:     return handler.Bool(true);

        case kObjectType:
            if (RAPIDJSON_UNLIKELY(!handler.StartObject()))
                return false;
            for (ConstMemberIterator m = MemberBegin(); m != MemberEnd(); ++m) {
                RAPIDJSON_ASSERT(m->name.IsString()); // Пользователь может изменить тип имени с помощью MemberIterator.
                if (RAPIDJSON_UNLIKELY(!handler.Key(m->name.GetString(), m->name.GetStringLength(), (m->name.data_.f.flags & kCopyFlag) != 0)))
                    return false;
                if (RAPIDJSON_UNLIKELY(!m->value.Accept(handler)))
                    return false;
            }
            return handler.EndObject(data_.o.size);

        case kArrayType:
            if (RAPIDJSON_UNLIKELY(!handler.StartArray()))
                return false;
            for (ConstValueIterator v = Begin(); v != End(); ++v)
                if (RAPIDJSON_UNLIKELY(!v->Accept(handler)))
                    return false;
            return handler.EndArray(data_.a.size);

        case kStringType:
            return handler.String(GetString(), GetStringLength(), (data_.f.flags & kCopyFlag) != 0);

        default:
            RAPIDJSON_ASSERT(GetType() == kNumberType);
            if (IsDouble())         return handler.Double(data_.n.d);
            else if (IsInt())       return handler.Int(data_.n.i.i);
            else if (IsUint())      return handler.Uint(data_.n.u.u);
            else if (IsInt64())     return handler.Int64(data_.n.i64);
            else                    return handler.Uint64(data_.n.u64);
        }
    }

private:
    template <typename, typename> friend class GenericValue;
    template <typename, typename, typename> friend class GenericDocument;

    enum {
        kBoolFlag       = 0x0008,
        kNumberFlag     = 0x0010,
        kIntFlag        = 0x0020,
        kUintFlag       = 0x0040,
        kInt64Flag      = 0x0080,
        kUint64Flag     = 0x0100,
        kDoubleFlag     = 0x0200,
        kStringFlag     = 0x0400,
        kCopyFlag       = 0x0800,
        kInlineStrFlag  = 0x1000,

        // Начальные флаги разных типов.
        kNullFlag = kNullType,
        // Эти приведения добавлены для подавления предупреждений MSVC о побитовых операциях между перечислениями разных типов.
        kTrueFlag = static_cast<int>(kTrueType) | static_cast<int>(kBoolFlag),
        kFalseFlag = static_cast<int>(kFalseType) | static_cast<int>(kBoolFlag),
        kNumberIntFlag = static_cast<int>(kNumberType) | static_cast<int>(kNumberFlag | kIntFlag | kInt64Flag),
        kNumberUintFlag = static_cast<int>(kNumberType) | static_cast<int>(kNumberFlag | kUintFlag | kUint64Flag | kInt64Flag),
        kNumberInt64Flag = static_cast<int>(kNumberType) | static_cast<int>(kNumberFlag | kInt64Flag),
        kNumberUint64Flag = static_cast<int>(kNumberType) | static_cast<int>(kNumberFlag | kUint64Flag),
        kNumberDoubleFlag = static_cast<int>(kNumberType) | static_cast<int>(kNumberFlag | kDoubleFlag),
        kNumberAnyFlag = static_cast<int>(kNumberType) | static_cast<int>(kNumberFlag | kIntFlag | kInt64Flag | kUintFlag | kUint64Flag | kDoubleFlag),
        kConstStringFlag = static_cast<int>(kStringType) | static_cast<int>(kStringFlag),
        kCopyStringFlag = static_cast<int>(kStringType) | static_cast<int>(kStringFlag | kCopyFlag),
        kShortStringFlag = static_cast<int>(kStringType) | static_cast<int>(kStringFlag | kCopyFlag | kInlineStrFlag),
        kObjectFlag = kObjectType,
        kArrayFlag = kArrayType,

        kTypeMask = 0x07
    };

    static const SizeType kDefaultArrayCapacity = RAPIDJSON_VALUE_DEFAULT_ARRAY_CAPACITY;
    static const SizeType kDefaultObjectCapacity = RAPIDJSON_VALUE_DEFAULT_OBJECT_CAPACITY;

    struct Flag {
#if RAPIDJSON_48BITPOINTER_OPTIMIZATION
        char payload[sizeof(SizeType) * 2 + 6];     // 2 x SizeType + нижний 48-битный указатель
#elif RAPIDJSON_64BIT
        char payload[sizeof(SizeType) * 2 + sizeof(void*) + 6]; // 6 байтов заполнения
#else
        char payload[sizeof(SizeType) * 2 + sizeof(void*) + 2]; // 2 байта заполнения
#endif
        uint16_t flags;
    };

    struct String {
        SizeType length;
        SizeType hashcode;  //!< зарезервировано
        const Ch* str;
    };  // 12 байт в 32-битном режиме, 16 байт в 64-битном режиме.

    // Деталь реализации: ShortString может возглавить строку с нулевым завершением до символов MaxSize.
    // (исключая завершающий ноль) и сохраните значение, чтобы определить длину содержащегося
    // В последнем символе str[LenPos] сохранился текст «MaxSize — длина». Если строка
    // для хранения имеет высоту MaxSize, тогда str[LenPos] будет равно 0 и, следовательно, будет действовать как
    // терминатор строки, а также. Чтобы получить длину строки из этого значения, просто используйте
    // «MaxSize — str[ЛенПос]».
    // Это позволяет хранить строки длиной 13 символов в 32-битном режиме, строки из 21 символа в 64-битном режиме.
    // Строки из 13 символов для встроенного RAPIDJSON_48BITPOINTER_OPTIMIZATION =1 (для строк в кодировке `UTF8`).
    struct ShortString {
        enum { MaxChars = sizeof(static_cast<Flag*>(0)->payload) / sizeof(Ch), MaxSize = MaxChars - 1, LenPos = MaxSize };
        Ch str[MaxChars];

        inline static bool Usable(SizeType len) { return                       (MaxSize >= len); }
        inline void     SetLength(SizeType len) { str[LenPos] = static_cast<Ch>(MaxSize -  len); }
        inline SizeType GetLength() const       { return  static_cast<SizeType>(MaxSize -  str[LenPos]); }
    };  // максимум столько же байтов, сколько «Строка» выше => 12 байтов в 32-битном режиме, 16 байтов в 64-битном режиме

    // При использовании правильной двоичной компоновки получение различных целочисленных типов не требует преобразований.
    union Number {
#if RAPIDJSON_ENDIAN == RAPIDJSON_LITTLEENDIAN
        struct I {
            int i;
            char padding[4];
        }i;
        struct U {
            unsigned u;
            char padding2[4];
        }u;
#else
        struct I {
            char padding[4];
            int i;
        }i;
        struct U {
            char padding2[4];
            unsigned u;
        }u;
#endif
        int64_t i64;
        uint64_t u64;
        double d;
    };  // 8 байт

    struct ObjectData {
        SizeType size;
        SizeType capacity;
        Member* members;
    };  // 12 байт в 32-битном режиме, 16 байт в 64-битном режиме.

    struct ArrayData {
        SizeType size;
        SizeType capacity;
        GenericValue* elements;
    };  // 12 байт в 32-битном режиме, 16 байт в 64-битном режиме.

    union Data {
        String s;
        ShortString ss;
        Number n;
        ObjectData o;
        ArrayData a;
        Flag f;
    };  // 16 байт в 32-битном режиме, 24 байта в 64-битном режиме, 16 байт в 64-битном режиме с RAPIDJSON_48BITPOINTER_OPTIMIZATION

    static RAPIDJSON_FORCEINLINE const Ch* DataString(const Data& data) {
        return (data.f.flags & kInlineStrFlag) ? data.ss.str : RAPIDJSON_GETPOINTER(Ch, data.s.str);
    }
    static RAPIDJSON_FORCEINLINE SizeType DataStringLength(const Data& data) {
        return (data.f.flags & kInlineStrFlag) ? data.ss.GetLength() : data.s.length;
    }

    RAPIDJSON_FORCEINLINE const Ch* GetStringPointer() const { return RAPIDJSON_GETPOINTER(Ch, data_.s.str); }
    RAPIDJSON_FORCEINLINE const Ch* SetStringPointer(const Ch* str) { return RAPIDJSON_SETPOINTER(Ch, data_.s.str, str); }
    RAPIDJSON_FORCEINLINE GenericValue* GetElementsPointer() const { return RAPIDJSON_GETPOINTER(GenericValue, data_.a.elements); }
    RAPIDJSON_FORCEINLINE GenericValue* SetElementsPointer(GenericValue* elements) { return RAPIDJSON_SETPOINTER(GenericValue, data_.a.elements, elements); }
    RAPIDJSON_FORCEINLINE Member* GetMembersPointer() const { return RAPIDJSON_GETPOINTER(Member, data_.o.members); }
    RAPIDJSON_FORCEINLINE Member* SetMembersPointer(Member* members) { return RAPIDJSON_SETPOINTER(Member, data_.o.members, members); }

#if RAPIDJSON_USE_MEMBERSMAP

    struct MapTraits {
        struct Less {
            bool operator()(const Data& s1, const Data& s2) const {
                SizeType n1 = DataStringLength(s1), n2 = DataStringLength(s2);
                int cmp = std::memcmp(DataString(s1), DataString(s2), sizeof(Ch) * (n1 < n2 ? n1 : n2));
                return cmp < 0 || (cmp == 0 && n1 < n2);
            }
        };
        typedef std::pair<const Data, SizeType> Pair;
        typedef std::multimap<Data, SizeType, Less, StdAllocator<Pair, Allocator> > Map;
        typedef typename Map::iterator Iterator;
    };
    typedef typename MapTraits::Map         Map;
    typedef typename MapTraits::Less        MapLess;
    typedef typename MapTraits::Pair        MapPair;
    typedef typename MapTraits::Iterator    MapIterator;

    //
    // Макет карты/массива участников, перемещаемый в соответствии с необходимой емкостью:
    //
    //    {Map*}<>{capacity}<>{Member[capacity]}<>{MapIterator[capacity]}
    //
    // (где <> означаетRAPIDJSON_ALIGN-ment, если необходимо)
    //

    static RAPIDJSON_FORCEINLINE size_t GetMapLayoutSize(SizeType capacity) {
        return RAPIDJSON_ALIGN(sizeof(Map*)) +
               RAPIDJSON_ALIGN(sizeof(SizeType)) +
               RAPIDJSON_ALIGN(capacity * sizeof(Member)) +
               capacity * sizeof(MapIterator);
    }

    static RAPIDJSON_FORCEINLINE SizeType &GetMapCapacity(Map* &map) {
        return *reinterpret_cast<SizeType*>(reinterpret_cast<uintptr_t>(&map) +
                                            RAPIDJSON_ALIGN(sizeof(Map*)));
    }

    static RAPIDJSON_FORCEINLINE Member* GetMapMembers(Map* &map) {
        return reinterpret_cast<Member*>(reinterpret_cast<uintptr_t>(&map) +
                                         RAPIDJSON_ALIGN(sizeof(Map*)) +
                                         RAPIDJSON_ALIGN(sizeof(SizeType)));
    }

    static RAPIDJSON_FORCEINLINE MapIterator* GetMapIterators(Map* &map) {
        return reinterpret_cast<MapIterator*>(reinterpret_cast<uintptr_t>(&map) +
                                              RAPIDJSON_ALIGN(sizeof(Map*)) +
                                              RAPIDJSON_ALIGN(sizeof(SizeType)) +
                                              RAPIDJSON_ALIGN(GetMapCapacity(map) * sizeof(Member)));
    }

    static RAPIDJSON_FORCEINLINE Map* &GetMap(Member* members) {
        RAPIDJSON_ASSERT(members != 0);
        return *reinterpret_cast<Map**>(reinterpret_cast<uintptr_t>(members) -
                                        RAPIDJSON_ALIGN(sizeof(SizeType)) -
                                        RAPIDJSON_ALIGN(sizeof(Map*)));
    }

    // Механизмы отладки некоторых компиляторов требуют, чтобы все итераторы были уничтожены для их учета.
    RAPIDJSON_FORCEINLINE MapIterator DropMapIterator(MapIterator& rhs) {
#if RAPIDJSON_HAS_CXX11
        MapIterator ret = std::move(rhs);
#else
        MapIterator ret = rhs;
#endif
        rhs.~MapIterator();
        return ret;
    }

    Map* &DoReallocMap(Map** oldMap, SizeType newCapacity, Allocator& allocator) {
        Map **newMap = static_cast<Map**>(allocator.Malloc(GetMapLayoutSize(newCapacity)));
        GetMapCapacity(*newMap) = newCapacity;
        if (!oldMap) {
            *newMap = new (allocator.Malloc(sizeof(Map))) Map(MapLess(), allocator);
        }
        else {
            *newMap = *oldMap;
            size_t count = (*oldMap)->size();
            std::memcpy(static_cast<void*>(GetMapMembers(*newMap)),
                        static_cast<void*>(GetMapMembers(*oldMap)),
                        count * sizeof(Member));
            MapIterator *oldIt = GetMapIterators(*oldMap),
                        *newIt = GetMapIterators(*newMap);
            while (count--) {
                new (&newIt[count]) MapIterator(DropMapIterator(oldIt[count]));
            }
            Allocator::Free(oldMap);
        }
        return *newMap;
    }

    RAPIDJSON_FORCEINLINE Member* DoAllocMembers(SizeType capacity, Allocator& allocator) {
        return GetMapMembers(DoReallocMap(0, capacity, allocator));
    }

    void DoReserveMembers(SizeType newCapacity, Allocator& allocator) {
        ObjectData& o = data_.o;
        if (newCapacity > o.capacity) {
            Member* oldMembers = GetMembersPointer();
            Map **oldMap = oldMembers ? &GetMap(oldMembers) : 0,
                *&newMap = DoReallocMap(oldMap, newCapacity, allocator);
            RAPIDJSON_SETPOINTER(Member, o.members, GetMapMembers(newMap));
            o.capacity = newCapacity;
        }
    }

    template <typename SourceAllocator>
    MemberIterator DoFindMember(const GenericValue<Encoding, SourceAllocator>& name) {
        if (Member* members = GetMembersPointer()) {
            Map* &map = GetMap(members);
            MapIterator mit = map->find(reinterpret_cast<const Data&>(name.data_));
            if (mit != map->end()) {
                return MemberIterator(&members[mit->second]);
            }
        }
        return MemberEnd();
    }

    void DoClearMembers() {
        if (Member* members = GetMembersPointer()) {
            Map* &map = GetMap(members);
            MapIterator* mit = GetMapIterators(map);
            for (SizeType i = 0; i < data_.o.size; i++) {
                map->erase(DropMapIterator(mit[i]));
                members[i].~Member();
            }
            data_.o.size = 0;
        }
    }

    void DoFreeMembers() {
        if (Member* members = GetMembersPointer()) {
            GetMap(members)->~Map();
            for (SizeType i = 0; i < data_.o.size; i++) {
                members[i].~Member();
            }
            if (Allocator::kNeedFree) { // Ярлык по признаку Распределитель
                Map** map = &GetMap(members);
                Allocator::Free(*map);
                Allocator::Free(map);
            }
        }
    }

#else // !RAPIDJSON_USE_MEMBERSMAP

    RAPIDJSON_FORCEINLINE Member* DoAllocMembers(SizeType capacity, Allocator& allocator) {
        return Malloc<Member>(allocator, capacity);
    }

    void DoReserveMembers(SizeType newCapacity, Allocator& allocator) {
        ObjectData& o = data_.o;
        if (newCapacity > o.capacity) {
            Member* newMembers = Realloc<Member>(allocator, GetMembersPointer(), o.capacity, newCapacity);
            RAPIDJSON_SETPOINTER(Member, o.members, newMembers);
            o.capacity = newCapacity;
        }
    }

    template <typename SourceAllocator>
    MemberIterator DoFindMember(const GenericValue<Encoding, SourceAllocator>& name) {
        MemberIterator member = MemberBegin();
        for ( ; member != MemberEnd(); ++member)
            if (name.StringEqual(member->name))
                break;
        return member;
    }

    void DoClearMembers() {
        for (MemberIterator m = MemberBegin(); m != MemberEnd(); ++m)
            m->~Member();
        data_.o.size = 0;
    }

    void DoFreeMembers() {
        for (MemberIterator m = MemberBegin(); m != MemberEnd(); ++m)
            m->~Member();
        Allocator::Free(GetMembersPointer());
    }

#endif // !RAPIDJSON_USE_MEMBERSMAP

    void DoAddMember(GenericValue& name, GenericValue& value, Allocator& allocator) {
        ObjectData& o = data_.o;
        if (o.size >= o.capacity)
            DoReserveMembers(o.capacity ? (o.capacity + (o.capacity + 1) / 2) : kDefaultObjectCapacity, allocator);
        Member* members = GetMembersPointer();
        Member* m = members + o.size;
        m->name.RawAssign(name);
        m->value.RawAssign(value);
#if RAPIDJSON_USE_MEMBERSMAP
        Map* &map = GetMap(members);
        MapIterator* mit = GetMapIterators(map);
        new (&mit[o.size]) MapIterator(map->insert(MapPair(m->name.data_, o.size)));
#endif
        ++o.size;
    }

    MemberIterator DoRemoveMember(MemberIterator m) {
        ObjectData& o = data_.o;
        Member* members = GetMembersPointer();
#if RAPIDJSON_USE_MEMBERSMAP
        Map* &map = GetMap(members);
        MapIterator* mit = GetMapIterators(map);
        SizeType mpos = static_cast<SizeType>(&*m - members);
        map->erase(DropMapIterator(mit[mpos]));
#endif
        MemberIterator last(members + (o.size - 1));
        if (o.size > 1 && m != last) {
#if RAPIDJSON_USE_MEMBERSMAP
            new (&mit[mpos]) MapIterator(DropMapIterator(mit[&*last - members]));
            mit[mpos]->second = mpos;
#endif
            *m = *last; // Переместите последний на это место
        }
        else {
            m->~Member(); // Остался только один, просто уничтожь
        }
        --o.size;
        return m;
    }

    MemberIterator DoEraseMembers(ConstMemberIterator first, ConstMemberIterator last) {
        ObjectData& o = data_.o;
        MemberIterator beg = MemberBegin(),
                       pos = beg + (first - beg),
                       end = MemberEnd();
#if RAPIDJSON_USE_MEMBERSMAP
        Map* &map = GetMap(GetMembersPointer());
        MapIterator* mit = GetMapIterators(map);
#endif
        for (MemberIterator itr = pos; itr != last; ++itr) {
#if RAPIDJSON_USE_MEMBERSMAP
            map->erase(DropMapIterator(mit[itr - beg]));
#endif
            itr->~Member();
        }
#if RAPIDJSON_USE_MEMBERSMAP
        if (first != last) {
            // Переместить оставшиеся члены/итераторы
            MemberIterator next = pos + (last - first);
            for (MemberIterator itr = pos; next != end; ++itr, ++next) {
                std::memcpy(static_cast<void*>(&*itr), &*next, sizeof(Member));
                SizeType mpos = static_cast<SizeType>(itr - beg);
                new (&mit[mpos]) MapIterator(DropMapIterator(mit[next - beg]));
                mit[mpos]->second = mpos;
            }
        }
#else
        std::memmove(static_cast<void*>(&*pos), &*last,
                     static_cast<size_t>(end - last) * sizeof(Member));
#endif
        o.size -= static_cast<SizeType>(last - first);
        return pos;
    }

    template <typename SourceAllocator>
    void DoCopyMembers(const GenericValue<Encoding,SourceAllocator>& rhs, Allocator& allocator, bool copyConstStrings) {
        RAPIDJSON_ASSERT(rhs.GetType() == kObjectType);

        data_.f.flags = kObjectFlag;
        SizeType count = rhs.data_.o.size;
        Member* lm = DoAllocMembers(count, allocator);
        const typename GenericValue<Encoding,SourceAllocator>::Member* rm = rhs.GetMembersPointer();
#if RAPIDJSON_USE_MEMBERSMAP
        Map* &map = GetMap(lm);
        MapIterator* mit = GetMapIterators(map);
#endif
        for (SizeType i = 0; i < count; i++) {
            new (&lm[i].name) GenericValue(rm[i].name, allocator, copyConstStrings);
            new (&lm[i].value) GenericValue(rm[i].value, allocator, copyConstStrings);
#if RAPIDJSON_USE_MEMBERSMAP
            new (&mit[i]) MapIterator(map->insert(MapPair(lm[i].name.data_, i)));
#endif
        }
        data_.o.size = data_.o.capacity = count;
        SetMembersPointer(lm);
    }

    // Инициализируйте это значение как массив с начальными данными, не вызывая деструктор.
    void SetArrayRaw(GenericValue* values, SizeType count, Allocator& allocator) {
        data_.f.flags = kArrayFlag;
        if (count) {
            GenericValue* e = static_cast<GenericValue*>(allocator.Malloc(count * sizeof(GenericValue)));
            SetElementsPointer(e);
            std::memcpy(static_cast<void*>(e), values, count * sizeof(GenericValue));
        }
        else
            SetElementsPointer(0);
        data_.a.size = data_.a.capacity = count;
    }

    //! Инициализируйте это значение как объект с начальными данными, не вызывая деструктор.
    void SetObjectRaw(Member* members, SizeType count, Allocator& allocator) {
        data_.f.flags = kObjectFlag;
        if (count) {
            Member* m = DoAllocMembers(count, allocator);
            SetMembersPointer(m);
            std::memcpy(static_cast<void*>(m), members, count * sizeof(Member));
#if RAPIDJSON_USE_MEMBERSMAP
            Map* &map = GetMap(m);
            MapIterator* mit = GetMapIterators(map);
            for (SizeType i = 0; i < count; i++) {
                new (&mit[i]) MapIterator(map->insert(MapPair(m[i].name.data_, i)));
            }
#endif
        }
        else
            SetMembersPointer(0);
        data_.o.size = data_.o.capacity = count;
    }

    //! Инициализируйте это значение как константную строку без вызова деструктора.
    void SetStringRaw(StringRefType s) RAPIDJSON_NOEXCEPT {
        data_.f.flags = kConstStringFlag;
        SetStringPointer(s);
        data_.s.length = s.length;
    }

    //! Инициализируйте это значение как строку копирования с начальными данными, не вызывая деструктор.
    void SetStringRaw(StringRefType s, Allocator& allocator) {
        Ch* str = 0;
        if (ShortString::Usable(s.length)) {
            data_.f.flags = kShortStringFlag;
            data_.ss.SetLength(s.length);
            str = data_.ss.str;
        } else {
            data_.f.flags = kCopyStringFlag;
            data_.s.length = s.length;
            str = static_cast<Ch *>(allocator.Malloc((s.length + 1) * sizeof(Ch)));
            SetStringPointer(str);
        }
        std::memcpy(str, s, s.length * sizeof(Ch));
        str[s.length] = '\0';
    }

    //! Присвоение без вызова деструктора
    void RawAssign(GenericValue& rhs) RAPIDJSON_NOEXCEPT {
        data_ = rhs.data_;
        // data_ .f.flags = rhs. data_.f.флаги;
        rhs.data_.f.flags = kNullFlag;
    }

    template <typename SourceAllocator>
    bool StringEqual(const GenericValue<Encoding, SourceAllocator>& rhs) const {
        RAPIDJSON_ASSERT(IsString());
        RAPIDJSON_ASSERT(rhs.IsString());

        const SizeType len1 = GetStringLength();
        const SizeType len2 = rhs.GetStringLength();
        if(len1 != len2) { return false; }

        const Ch* const str1 = GetString();
        const Ch* const str2 = rhs.GetString();
        if(str1 == str2) { return true; } // быстрый путь для постоянной строки

        return (std::memcmp(str1, str2, sizeof(Ch) * len1) == 0);
    }

    Data data_;
};

//! GenericValue с кодировкой UTF8
typedef GenericValue<UTF8<> > Value;

///////////////////////////////////////////////////////////////////////////////
// Общий документ

//! Документ для анализа текста JSON как DOM.
/*!
    \note реализация реализации Handler
    \tparam Кодировка Кодировка для синтаксического анализа и хранения строк.
    \tparam Распределитель Распределитель для выделения памяти для DOM
    \tparam StackAllocator Распределитель выделения памяти для стека во время синтаксического анализа.
    \warning Хотя GenericDocument наследует GenericValue,API\b не обеспечивает никаких виртуальных функций, особенно виртуального деструктора.  Чтобы избежать утечек памяти, не \c удаляйте объект GenericDocument с помощью указателя на GenericValue.
*/
template <typename Encoding, typename Allocator = RAPIDJSON_DEFAULT_ALLOCATOR, typename StackAllocator = RAPIDJSON_DEFAULT_STACK_ALLOCATOR >
class GenericDocument : public GenericValue<Encoding, Allocator> {
public:
    typedef typename Encoding::Ch Ch;                       //!< Тип символа, полученный в результате кодирования.
    typedef GenericValue<Encoding, Allocator> ValueType;    //!< Тип значения документа.
    typedef Allocator AllocatorType;                        //!< Тип распределителя из параметра шаблона.
    typedef StackAllocator StackAllocatorType;              //!< Тип StackAllocator из параметра шаблона.

    //! Конструктор
    /*! Создает пустой документ указанного типа.
        \param type Обязательный тип создаваемого объекта.
        \param allocator Необязательный распределитель для выделения памяти.
        \param stackCapacity Необязательная начальная емкость стека в байтах.
        \param stackAllocator Необязательный распределитель для выделения памяти для стека.
    */
    explicit GenericDocument(Type type, Allocator* allocator = 0, size_t stackCapacity = kDefaultStackCapacity, StackAllocator* stackAllocator = 0) :
        GenericValue<Encoding, Allocator>(type),  allocator_(allocator), ownAllocator_(0), stack_(stackAllocator, stackCapacity), parseResult_()
    {
        if (!allocator_)
            ownAllocator_ = allocator_ = RAPIDJSON_NEW(Allocator)();
    }

    //! Конструктор
    /*! Создает пустой документ с типом Null.
        \param allocator Необязательный распределитель для выделения памяти.
        \param stackCapacity Необязательная начальная емкость стека в байтах.
        \param stackAllocator Необязательный распределитель для выделения памяти для стека.
    */
    GenericDocument(Allocator* allocator = 0, size_t stackCapacity = kDefaultStackCapacity, StackAllocator* stackAllocator = 0) :
        allocator_(allocator), ownAllocator_(0), stack_(stackAllocator, stackCapacity), parseResult_()
    {
        if (!allocator_)
            ownAllocator_ = allocator_ = RAPIDJSON_NEW(Allocator)();
    }

#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
    //! Переместить конструктор в C++11
    GenericDocument(GenericDocument&& rhs) RAPIDJSON_NOEXCEPT
        : ValueType(std::forward<ValueType>(rhs)), // явное приведение, чтобы избежать запрещенного перемещения из документа
          allocator_(rhs.allocator_),
          ownAllocator_(rhs.ownAllocator_),
          stack_(std::move(rhs.stack_)),
          parseResult_(rhs.parseResult_)
    {
        rhs.allocator_ = 0;
        rhs.ownAllocator_ = 0;
        rhs.parseResult_ = ParseResult();
    }
#endif

    ~GenericDocument() {
        // Очистите ::ValueType перед использованием ownAllocator, ~ ValueType()
        // запускается последним и может получить доступ к своим элементам или членам, которые будут освобождены
        // с распределителем, например MemoryPoolAllocator (CrtAllocator не
        // освобождает свои данные при уничтожении, но это делает MemoryPoolAllocator).
        if (ownAllocator_) {
            ValueType::SetNull();
        }
        Destroy();
    }

#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
    //! Переместить назначение в C++11
    GenericDocument& operator=(GenericDocument&& rhs) RAPIDJSON_NOEXCEPT
    {
        // Здесь необходимо приведение к ValueType, на случай неудачного случая.
        // Попытка вызвать шаблонный оператор использования GenericValue.
        ValueType::operator=(std::forward<ValueType>(rhs));

        // Вызов деструктора здесь приведет к прежнему вызову деструктора stack_.
        Destroy();

        allocator_ = rhs.allocator_;
        ownAllocator_ = rhs.ownAllocator_;
        stack_ = std::move(rhs.stack_);
        parseResult_ = rhs.parseResult_;

        rhs.allocator_ = 0;
        rhs.ownAllocator_ = 0;
        rhs.parseResult_ = ParseResult();

        return *this;
    }
#endif

    //! Обменяйтесь содержимым этого документа с содержимым другого.
    /*!
        \param rhs Другой документ.
        \note Постоянная сложность.
        \см. GenericValue::Swap
    */
    GenericDocument& Swap(GenericDocument& rhs) RAPIDJSON_NOEXCEPT {
        ValueType::Swap(rhs);
        stack_.Swap(rhs.stack_);
        internal::Swap(allocator_, rhs.allocator_);
        internal::Swap(ownAllocator_, rhs.ownAllocator_);
        internal::Swap(parseResult_, rhs.parseResult_);
        return *this;
    }

    // Разрешить обмен на ValueType.
    // См. «Эффективное C++, 3-е издание», пункт 33: Не скрывайте унаследованные имена.
    using ValueType::Swap;

    //! отдельно стоящий помощник функции подкачки
    /*!
        Вспомогательная функция для включения поддержки реализации общего шаблона подкачки на основе \c std::swap:
        \code
        void swap(MyClass& a, MyClass& b) {
            с помощью станд::своп;
            своп(а.док, б.док);
            // ...
        }
        \endcode
        \см.  Swap()
     */
    friend inline void swap(GenericDocument& a, GenericDocument& b) RAPIDJSON_NOEXCEPT { a.Swap(b); }

    //! Заполните этот документ генератором, который генерирует события SAX.
    /*! \tparam Генератор функтор с прототипом <tt>bool f(Handler)</tt>.
        \param g Функтор-генератор, который отправляет событияSAXв параметр.
        \return Сам документ для беглогоAPI.
    */
    template <typename Generator>
    GenericDocument& Populate(Generator& g) {
        ClearStackOnExit scope(*this);
        if (g(*this)) {
            RAPIDJSON_ASSERT(stack_.GetSize() == sizeof(ValueType)); // Получил один и только один корневой объект
            ValueType::operator=(*stack_.template Pop<ValueType>(1));// Переместить значение из стека в документ
        }
        return *this;
    }

    //! @nameАнализ потока
    //!@{

    //! Анализ текста JSON из входного потока (с преобразованием кодировки)
    /*! \tparam parseFlags Комбинация \ref ParseFlag.
        \tparam SourceEncoding Кодирование входного потока
        \tparam InputStream Тип входного потока, реализующий образующий поток
        \param — входной поток для анализа.
        \return Сам документ для беглогоAPI.
    */
    template <unsigned parseFlags, typename SourceEncoding, typename InputStream>
    GenericDocument& ParseStream(InputStream& is) {
        GenericReader<SourceEncoding, Encoding, StackAllocator> reader(
            stack_.HasAllocator() ? &stack_.GetAllocator() : 0);
        ClearStackOnExit scope(*this);
        parseResult_ = reader.template Parse<parseFlags>(is, *this);
        if (parseResult_) {
            RAPIDJSON_ASSERT(stack_.GetSize() == sizeof(ValueType)); // Получил один и только один корневой объект
            ValueType::operator=(*stack_.template Pop<ValueType>(1));// Переместить значение из стека в документ
        }
        return *this;
    }

    //! Разобрать текст JSON из входного потока
    /*! \tparam parseFlags Комбинация \ref ParseFlag.
        \tparam InputStream Тип входного потока, реализующий образующий поток
        \param — входной поток для анализа.
        \return Сам документ для беглогоAPI.
    */
    template <unsigned parseFlags, typename InputStream>
    GenericDocument& ParseStream(InputStream& is) {
        return ParseStream<parseFlags, Encoding, InputStream>(is);
    }

    //! Анализировать текстJSONиз входного потока (с помощью \ref kParseDefaultFlags)
    /*! \tparam InputStream Тип входного потока, реализующий образующий поток
        \param — входной поток для анализа.
        \return Сам документ для беглогоAPI.
    */
    template <typename InputStream>
    GenericDocument& ParseStream(InputStream& is) {
        return ParseStream<kParseDefaultFlags, Encoding, InputStream>(is);
    }
    //!@}

    //! @nameАнализ изменяемой строки на месте
    //!@{

    //! Анализировать текст JSON из изменяемой строки
    /*! \tparam parseFlags Комбинация \ref ParseFlag.
        \param str Изменяемая строка с нулевым завершением, подлежащая анализу.
        \return Сам документ для беглогоAPI.
    */
    template <unsigned parseFlags>
    GenericDocument& ParseInsitu(Ch* str) {
        GenericInsituStringStream<Encoding> s(str);
        return ParseStream<parseFlags | kParseInsituFlag>(s);
    }

    //! Анализировать текстJSONиз изменяемой строки (с помощью \ref kParseDefaultFlags)
    /*! \param str Изменяемая строка с нулевым завершением, подлежащая анализу.
        \return Сам документ для беглогоAPI.
    */
    GenericDocument& ParseInsitu(Ch* str) {
        return ParseInsitu<kParseDefaultFlags>(str);
    }
    //!@}

    //! @nameАнализ строк, доступен только для чтения.
    //!@{

    //! Анализ текста JSON из строки, доступной только для чтения (с преобразованием кодировки)
    /*! \tparam parseFlags Комбинация \ref ParseFlag (не следует поддерживать \ref kParseInsituFlag).
        \tparam SourceEncoding Транскодирование из входной кодировки
        \param str Строка с нулевым завершением, доступна только для чтения и подлежащая синтаксическому анализу.
    */
    template <unsigned parseFlags, typename SourceEncoding>
    GenericDocument& Parse(const typename SourceEncoding::Ch* str) {
        RAPIDJSON_ASSERT(!(parseFlags & kParseInsituFlag));
        GenericStringStream<SourceEncoding> s(str);
        return ParseStream<parseFlags, SourceEncoding>(s);
    }

    //! Анализировать текст JSON из строки, доступной только для чтения.
    /*! \tparam parseFlags Комбинация \ref ParseFlag (не следует поддерживать \ref kParseInsituFlag).
        \param str Строка с нулевым завершением, доступна только для чтения и подлежащая синтаксическому анализу.
    */
    template <unsigned parseFlags>
    GenericDocument& Parse(const Ch* str) {
        return Parse<parseFlags, Encoding>(str);
    }

    //! Анализировать текстJSONиз строк, доступный только для чтения (с помощью \ref kParseDefaultFlags)
    /*! \param str Строка с нулевым завершением, доступна только для чтения и подлежащая синтаксическому анализу.
    */
    GenericDocument& Parse(const Ch* str) {
        return Parse<kParseDefaultFlags>(str);
    }

    template <unsigned parseFlags, typename SourceEncoding>
    GenericDocument& Parse(const typename SourceEncoding::Ch* str, size_t length) {
        RAPIDJSON_ASSERT(!(parseFlags & kParseInsituFlag));
        MemoryStream ms(reinterpret_cast<const char*>(str), length * sizeof(typename SourceEncoding::Ch));
        EncodedInputStream<SourceEncoding, MemoryStream> is(ms);
        ParseStream<parseFlags, SourceEncoding>(is);
        return *this;
    }

    template <unsigned parseFlags>
    GenericDocument& Parse(const Ch* str, size_t length) {
        return Parse<parseFlags, Encoding>(str, length);
    }

    GenericDocument& Parse(const Ch* str, size_t length) {
        return Parse<kParseDefaultFlags>(str, length);
    }

#if RAPIDJSON_HAS_STDSTRING
    template <unsigned parseFlags, typename SourceEncoding>
    GenericDocument& Parse(const std::basic_string<typename SourceEncoding::Ch>& str) {
        // c_str () — постоянная форма по стандарту. Должно быть быстрее, чем Parse(const char*,size_t)
        return Parse<parseFlags, SourceEncoding>(str.c_str());
    }

    template <unsigned parseFlags>
    GenericDocument& Parse(const std::basic_string<Ch>& str) {
        return Parse<parseFlags, Encoding>(str.c_str());
    }

    GenericDocument& Parse(const std::basic_string<Ch>& str) {
        return Parse<kParseDefaultFlags>(str);
    }
#endif // RAPIDJSON_HAS_STDSTRING

    //!@}

    //! @nameОбработка ошибок синтаксического анализа
    //!@{

    //! Произошла ли ошибка синтаксического анализа при последнем синтаксическом анализе.
    bool HasParseError() const { return parseResult_.IsError(); }

    //! Получите \ref ParseErrorCode последний синтаксический анализ.
    ParseErrorCode GetParseError() const { return parseResult_.Code(); }

    //! Получите позицию последней ошибки синтаксического анализа во входных данных, в противном случае — 0.
    size_t GetErrorOffset() const { return parseResult_.Offset(); }

    //! Неявное преобразование для получения последнего результата анализа
#ifndef __clang // -Документация
    /*! \return \ref ParseResult анализа высшей операции

        \code
          Документ-документ;
          ParseResult ок = doc.Parse(json);
          if (!ok)
            printf( "Ошибка анализаJSON: %s (%u)\n",GetParseError_En(ок.Code()), ок.Offset());
        \endcode
     */
#endif
    operator ParseResult() const { return parseResult_; }
    //!@}

    //! Получите распределитель этого документа.
    Allocator& GetAllocator() {
        RAPIDJSON_ASSERT(allocator_);
        return *allocator_;
    }

    //! Получите емкость стека в байтах.
    size_t GetStackCapacity() const { return stack_.GetCapacity(); }

private:
    // стойкий стек при любом выходе из ParseStream, например. из-за исключения
    struct ClearStackOnExit {
        explicit ClearStackOnExit(GenericDocument& d) : d_(d) {}
        ~ClearStackOnExit() { d_.ClearStack(); }
    private:
        ClearStackOnExit(const ClearStackOnExit&);
        ClearStackOnExit& operator=(const ClearStackOnExit&);
        GenericDocument& d_;
    };

    // вызывающие стороны следующих частных функций-обработчиков
    // шаблон <имя типа, имя типа, имя типа> дружественный класс GenericReader; // для разбора
    template <typename, typename> friend class GenericValue; // для глубокого копирования

public:
    // Реализация обработчика
    bool Null() { new (stack_.template Push<ValueType>()) ValueType(); return true; }
    bool Bool(bool b) { new (stack_.template Push<ValueType>()) ValueType(b); return true; }
    bool Int(int i) { new (stack_.template Push<ValueType>()) ValueType(i); return true; }
    bool Uint(unsigned i) { new (stack_.template Push<ValueType>()) ValueType(i); return true; }
    bool Int64(int64_t i) { new (stack_.template Push<ValueType>()) ValueType(i); return true; }
    bool Uint64(uint64_t i) { new (stack_.template Push<ValueType>()) ValueType(i); return true; }
    bool Double(double d) { new (stack_.template Push<ValueType>()) ValueType(d); return true; }

    bool RawNumber(const Ch* str, SizeType length, bool copy) {
        if (copy)
            new (stack_.template Push<ValueType>()) ValueType(str, length, GetAllocator());
        else
            new (stack_.template Push<ValueType>()) ValueType(str, length);
        return true;
    }

    bool String(const Ch* str, SizeType length, bool copy) {
        if (copy)
            new (stack_.template Push<ValueType>()) ValueType(str, length, GetAllocator());
        else
            new (stack_.template Push<ValueType>()) ValueType(str, length);
        return true;
    }

    bool StartObject() { new (stack_.template Push<ValueType>()) ValueType(kObjectType); return true; }

    bool Key(const Ch* str, SizeType length, bool copy) { return String(str, length, copy); }

    bool EndObject(SizeType memberCount) {
        typename ValueType::Member* members = stack_.template Pop<typename ValueType::Member>(memberCount);
        stack_.template Top<ValueType>()->SetObjectRaw(members, memberCount, GetAllocator());
        return true;
    }

    bool StartArray() { new (stack_.template Push<ValueType>()) ValueType(kArrayType); return true; }

    bool EndArray(SizeType elementCount) {
        ValueType* elements = stack_.template Pop<ValueType>(elementCount);
        stack_.template Top<ValueType>()->SetArrayRaw(elements, elementCount, GetAllocator());
        return true;
    }

private:
    //! Запретить копирование
    GenericDocument(const GenericDocument&);
    //! Запретить назначение
    GenericDocument& operator=(const GenericDocument&);

    void ClearStack() {
        if (Allocator::kNeedFree)
            while (stack_.GetSize() > 0)    // Здесь предполагается, что все элементы в массиве стека имеют значение GenericValue (на самом деле элементом являются 2 объекта GenericValue).
                (stack_.template Pop<ValueType>(1))->~ValueType();
        else
            stack_.Clear();
        stack_.ShrinkToFit();
    }

    void Destroy() {
        RAPIDJSON_DELETE(ownAllocator_);
    }

    static const size_t kDefaultStackCapacity = 1024;
    Allocator* allocator_;
    Allocator* ownAllocator_;
    internal::Stack<StackAllocator> stack_;
    ParseResult parseResult_;
};

//! GenericDocument с кодировкой UTF8
typedef GenericDocument<UTF8<> > Document;


//! Вспомогательный класс для доступа к значению типа массива.
/*!
    Экземпляр этого вспомогательного класса создается с помощью \c GenericValue::GetArray() .
    В дополнении ко всемAPI-интерфейсам для типа массива он обеспечивает цикл для основы соединения, если \cRAPIDJSON_HAS_CXX11_RANGE_FOR=1.
*/
template <bool Const, typename ValueT>
class GenericArray {
public:
    typedef GenericArray<true, ValueT> ConstArray;
    typedef GenericArray<false, ValueT> Array;
    typedef ValueT PlainType;
    typedef typename internal::MaybeAddConst<Const,PlainType>::Type ValueType;
    typedef ValueType* ValueIterator;  // Это может быть константный или неконстантный итератор.
    typedef const ValueT* ConstValueIterator;
    typedef typename ValueType::AllocatorType AllocatorType;
    typedef typename ValueType::StringRefType StringRefType;

    template <typename, typename>
    friend class GenericValue;

    GenericArray(const GenericArray& rhs) : value_(rhs.value_) {}
    GenericArray& operator=(const GenericArray& rhs) { value_ = rhs.value_; return *this; }
    ~GenericArray() {}

    operator ValueType&() const { return value_; }
    SizeType Size() const { return value_.Size(); }
    SizeType Capacity() const { return value_.Capacity(); }
    bool Empty() const { return value_.Empty(); }
    void Clear() const { value_.Clear(); }
    ValueType& operator[](SizeType index) const {  return value_[index]; }
    ValueIterator Begin() const { return value_.Begin(); }
    ValueIterator End() const { return value_.End(); }
    GenericArray Reserve(SizeType newCapacity, AllocatorType &allocator) const { value_.Reserve(newCapacity, allocator); return *this; }
    GenericArray PushBack(ValueType& value, AllocatorType& allocator) const { value_.PushBack(value, allocator); return *this; }
#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
    GenericArray PushBack(ValueType&& value, AllocatorType& allocator) const { value_.PushBack(value, allocator); return *this; }
#endif // RAPIDJSON_HAS_CXX11_RVALUE_REFS
    GenericArray PushBack(StringRefType value, AllocatorType& allocator) const { value_.PushBack(value, allocator); return *this; }
    template <typename T> RAPIDJSON_DISABLEIF_RETURN((internal::OrExpr<internal::IsPointer<T>, internal::IsGenericValue<T> >), (const GenericArray&)) PushBack(T value, AllocatorType& allocator) const { value_.PushBack(value, allocator); return *this; }
    GenericArray PopBack() const { value_.PopBack(); return *this; }
    ValueIterator Erase(ConstValueIterator pos) const { return value_.Erase(pos); }
    ValueIterator Erase(ConstValueIterator first, ConstValueIterator last) const { return value_.Erase(first, last); }

#if RAPIDJSON_HAS_CXX11_RANGE_FOR
    ValueIterator begin() const { return value_.Begin(); }
    ValueIterator end() const { return value_.End(); }
#endif

private:
    GenericArray();
    GenericArray(ValueType& value) : value_(value) {}
    ValueType& value_;
};

//! Вспомогательный класс для доступа к значению типа объекта.
/*!
    Экземпляр этого вспомогательного класса создается с помощью \c GenericValue::GetObject() .
    В дополнении ко всемAPI-интерфейсам для типа массива он обеспечивает цикл для основы соединения, если \cRAPIDJSON_HAS_CXX11_RANGE_FOR=1.
*/
template <bool Const, typename ValueT>
class GenericObject {
public:
    typedef GenericObject<true, ValueT> ConstObject;
    typedef GenericObject<false, ValueT> Object;
    typedef ValueT PlainType;
    typedef typename internal::MaybeAddConst<Const,PlainType>::Type ValueType;
    typedef GenericMemberIterator<Const, typename ValueT::EncodingType, typename ValueT::AllocatorType> MemberIterator;  // Это может быть константный или неконстантный итератор.
    typedef GenericMemberIterator<true, typename ValueT::EncodingType, typename ValueT::AllocatorType> ConstMemberIterator;
    typedef typename ValueType::AllocatorType AllocatorType;
    typedef typename ValueType::StringRefType StringRefType;
    typedef typename ValueType::EncodingType EncodingType;
    typedef typename ValueType::Ch Ch;

    template <typename, typename>
    friend class GenericValue;

    GenericObject(const GenericObject& rhs) : value_(rhs.value_) {}
    GenericObject& operator=(const GenericObject& rhs) { value_ = rhs.value_; return *this; }
    ~GenericObject() {}

    operator ValueType&() const { return value_; }
    SizeType MemberCount() const { return value_.MemberCount(); }
    SizeType MemberCapacity() const { return value_.MemberCapacity(); }
    bool ObjectEmpty() const { return value_.ObjectEmpty(); }
    template <typename T> ValueType& operator[](T* name) const { return value_[name]; }
    template <typename SourceAllocator> ValueType& operator[](const GenericValue<EncodingType, SourceAllocator>& name) const { return value_[name]; }
#if RAPIDJSON_HAS_STDSTRING
    ValueType& operator[](const std::basic_string<Ch>& name) const { return value_[name]; }
#endif
    MemberIterator MemberBegin() const { return value_.MemberBegin(); }
    MemberIterator MemberEnd() const { return value_.MemberEnd(); }
    GenericObject MemberReserve(SizeType newCapacity, AllocatorType &allocator) const { value_.MemberReserve(newCapacity, allocator); return *this; }
    bool HasMember(const Ch* name) const { return value_.HasMember(name); }
#if RAPIDJSON_HAS_STDSTRING
    bool HasMember(const std::basic_string<Ch>& name) const { return value_.HasMember(name); }
#endif
    template <typename SourceAllocator> bool HasMember(const GenericValue<EncodingType, SourceAllocator>& name) const { return value_.HasMember(name); }
    MemberIterator FindMember(const Ch* name) const { return value_.FindMember(name); }
    template <typename SourceAllocator> MemberIterator FindMember(const GenericValue<EncodingType, SourceAllocator>& name) const { return value_.FindMember(name); }
#if RAPIDJSON_HAS_STDSTRING
    MemberIterator FindMember(const std::basic_string<Ch>& name) const { return value_.FindMember(name); }
#endif
    GenericObject AddMember(ValueType& name, ValueType& value, AllocatorType& allocator) const { value_.AddMember(name, value, allocator); return *this; }
    GenericObject AddMember(ValueType& name, StringRefType value, AllocatorType& allocator) const { value_.AddMember(name, value, allocator); return *this; }
#if RAPIDJSON_HAS_STDSTRING
    GenericObject AddMember(ValueType& name, std::basic_string<Ch>& value, AllocatorType& allocator) const { value_.AddMember(name, value, allocator); return *this; }
#endif
    template <typename T> RAPIDJSON_DISABLEIF_RETURN((internal::OrExpr<internal::IsPointer<T>, internal::IsGenericValue<T> >), (ValueType&)) AddMember(ValueType& name, T value, AllocatorType& allocator) const { value_.AddMember(name, value, allocator); return *this; }
#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
    GenericObject AddMember(ValueType&& name, ValueType&& value, AllocatorType& allocator) const { value_.AddMember(name, value, allocator); return *this; }
    GenericObject AddMember(ValueType&& name, ValueType& value, AllocatorType& allocator) const { value_.AddMember(name, value, allocator); return *this; }
    GenericObject AddMember(ValueType& name, ValueType&& value, AllocatorType& allocator) const { value_.AddMember(name, value, allocator); return *this; }
    GenericObject AddMember(StringRefType name, ValueType&& value, AllocatorType& allocator) const { value_.AddMember(name, value, allocator); return *this; }
#endif // RAPIDJSON_HAS_CXX11_RVALUE_REFS
    GenericObject AddMember(StringRefType name, ValueType& value, AllocatorType& allocator) const { value_.AddMember(name, value, allocator); return *this; }
    GenericObject AddMember(StringRefType name, StringRefType value, AllocatorType& allocator) const { value_.AddMember(name, value, allocator); return *this; }
    template <typename T> RAPIDJSON_DISABLEIF_RETURN((internal::OrExpr<internal::IsPointer<T>, internal::IsGenericValue<T> >), (GenericObject)) AddMember(StringRefType name, T value, AllocatorType& allocator) const { value_.AddMember(name, value, allocator); return *this; }
    void RemoveAllMembers() { value_.RemoveAllMembers(); }
    bool RemoveMember(const Ch* name) const { return value_.RemoveMember(name); }
#if RAPIDJSON_HAS_STDSTRING
    bool RemoveMember(const std::basic_string<Ch>& name) const { return value_.RemoveMember(name); }
#endif
    template <typename SourceAllocator> bool RemoveMember(const GenericValue<EncodingType, SourceAllocator>& name) const { return value_.RemoveMember(name); }
    MemberIterator RemoveMember(MemberIterator m) const { return value_.RemoveMember(m); }
    MemberIterator EraseMember(ConstMemberIterator pos) const { return value_.EraseMember(pos); }
    MemberIterator EraseMember(ConstMemberIterator first, ConstMemberIterator last) const { return value_.EraseMember(first, last); }
    bool EraseMember(const Ch* name) const { return value_.EraseMember(name); }
#if RAPIDJSON_HAS_STDSTRING
    bool EraseMember(const std::basic_string<Ch>& name) const { return EraseMember(ValueType(StringRef(name))); }
#endif
    template <typename SourceAllocator> bool EraseMember(const GenericValue<EncodingType, SourceAllocator>& name) const { return value_.EraseMember(name); }

#if RAPIDJSON_HAS_CXX11_RANGE_FOR
    MemberIterator begin() const { return value_.MemberBegin(); }
    MemberIterator end() const { return value_.MemberEnd(); }
#endif

private:
    GenericObject();
    GenericObject(ValueType& value) : value_(value) {}
    ValueType& value_;
};

RAPIDJSON_NAMESPACE_END
RAPIDJSON_DIAG_POP

#ifdef RAPIDJSON_WINDOWS_GETOBJECT_WORKAROUND_APPLIED
#pragma pop_macro("GetObject")
#undef RAPIDJSON_WINDOWS_GETOBJECT_WORKAROUND_APPLIED
#endif

#endif // RAPIDJSON_DOCUMENT_H_
