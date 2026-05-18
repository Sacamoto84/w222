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

#ifndef RAPIDJSON_POINTER_H_
#define RAPIDJSON_POINTER_H_

#include "document.h"
#include "uri.h"
#include "internal/itoa.h"
#include "error/error.h" // Поинтерпарсеерркоде

#ifdef __clang__
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(switch-enum)
#elif defined(_MSC_VER)
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(4512) // не удалось сгенерировать оператор присваивания
#endif

RAPIDJSON_NAMESPACE_BEGIN

static const SizeType kPointerInvalidIndex = ~SizeType(0);  //!< содержит недопустимый индекс в GenericPointer::Token.

///////////////////////////////////////////////////////////////////////////////
// Общий указатель

//! Представляет указатель JSON. Используйте указатель для кодировки UTF8 и распределителя по умолчанию.
/*!
    Этот класс реализуетRFC6901 «Указатель нотации объекта JavaScript (JSON)»
    ( https://tools.ietf.org/html/rfc6901).

    Указатель JSON предназначен для идентификации определенного значения в документе JSON.
    (Общий документ). Это может упростить кодирование манипуляций с деревом DOM, поскольку
    может получить доступ к многоуровневой глубине дерева DOM с помощью одного вызова API.

    После анализа строкового представления (например, фрагмента "/foo/0" или URI
    представление (например, «#/foo/0») в области представления (токены),
    его можно использовать для разрешения определенного значения в нескольких документах или поддереве
    документов.

    В отличие от GenericValue, Pointer может быть скопирован и назначен.
    Помимо присвоения, указатель не может быть изменен после создания.

    Хотя Pointer очень удобен, помните, что создание Pointer
    включает в себя синтаксический анализ и динамическое выделение памяти. Специальный конструктор с пользовательским
    предоставленные токены устраняют их.

    GenericPointer зависит от GenericDocument и GenericValue.

    \tparam ValueType Тип значения дереваDOM. например GenericValue<UTF8<> >
    \tparam Распределитель Тип распределителя для выделения памяти для внутреннего представления.

    \note GenericPointer использует ту же кодировку, что и ValueType.
    Однако распределение GenericPointer не зависит от распределения результатов.
*/
template <typename ValueType, typename Allocator = CrtAllocator>
class GenericPointer {
public:
    typedef typename ValueType::EncodingType EncodingType;  //!< Тип кодировки из значения
    typedef typename ValueType::Ch Ch;                      //!< Тип символа из значения
    typedef GenericUri<ValueType, Allocator> UriType;


  //! Токен — это базовая единица внутреннего представления.
    /*!
        Строковое представление указателяJSON"/foo/123" анализируется до двух токенов:
        «foo» и 123. 123 будет представлено как в числовой, так и в строковой форме.
        Они разрешаются в соответствии с фактическим типом значения (объект или массив).

        Для токенов, которые не являются числами, или числовое значение выходит за пределы
        (больше, чем размер SizeType), они рассматривают только как строковую форму.
        (т.е. токена индекса будет равна kPointerInvalidIndex).

        Эта структура является общедоступной, поэтому пользователь может создать указатель без синтаксического анализа и
        выделение с помощью специального конструктора.
    */
    struct Token {
        const Ch* name;             //!< Имя токена. В конце он имеет нулевой символ, но может содержать нулевой символ.
        SizeType length;            //!< Длина имени.
        SizeType index;             //!< Действительный индекс массива, если он не равен kPointerInvalidIndex.
    };

    //! @nameКонструкторы и деструкторы.
    //@{

    //! Конструктор по умолчанию.
    GenericPointer(Allocator* allocator = 0) : allocator_(allocator), ownAllocator_(), nameBuffer_(), tokens_(), tokenCount_(), parseErrorOffset_(), parseErrorCode_(kPointerParseErrorNone) {}

    //! Конструктор, который анализирует строку или представление фрагмента URI.
    /*!
        \param source Строковое представление указателяJSONс нулевым завершением фрагмента или представлениеURI.
        \param allocator Пользовательский распределитель для этого указателя. Если распределитель не указан, он производит большие размеры.
    */
    explicit GenericPointer(const Ch* source, Allocator* allocator = 0) : allocator_(allocator), ownAllocator_(), nameBuffer_(), tokens_(), tokenCount_(), parseErrorOffset_(), parseErrorCode_(kPointerParseErrorNone) {
        Parse(source, internal::StrLen(source));
    }

#if RAPIDJSON_HAS_STDSTRING
    //! Конструктор, который анализирует строку или представление фрагмента URI.
    /*!
        \param source Строковое представление или фрагментURIуказателяJSON.
        \param allocator Пользовательский распределитель для этого указателя. Если распределитель не указан, он производит большие размеры.
        \note Требуется определение символа препроцессора \refRAPIDJSON_HAS_STDSTRING.
    */
    explicit GenericPointer(const std::basic_string<Ch>& source, Allocator* allocator = 0) : allocator_(allocator), ownAllocator_(), nameBuffer_(), tokens_(), tokenCount_(), parseErrorOffset_(), parseErrorCode_(kPointerParseErrorNone) {
        Parse(source.c_str(), source.size());
    }
#endif

    //! Конструктор, который анализирует строку или представление фрагмента URI с длиной исходной строки.
    /*!
        \param source Строковое представление или фрагментURIуказателяJSON.
        \param length Длина источника.
        \param allocator Пользовательский распределитель для этого указателя. Если распределитель не указан, он производит большие размеры.
        \note Немного быстрее, чем перегрузка без длины.
    */
    GenericPointer(const Ch* source, size_t length, Allocator* allocator = 0) : allocator_(allocator), ownAllocator_(), nameBuffer_(), tokens_(), tokenCount_(), parseErrorOffset_(), parseErrorCode_(kPointerParseErrorNone) {
        Parse(source, length);
    }

    //! Конструктор с токенами, предоставленными пользователем.
    /*!
        Этот конструктор позволяет пользователю предоставлять константный массив токенов.
        Это предотвращает процесс синтаксического анализа и исключает выделение.
        Это предпочтительно для сред с ограниченной памятью.

        \param tokens Постоянный массив токенов, представляющий указательJSON.
        \param tokenCount Количество токенов.

        \b Пример
        \code
        #defineNAME(s) { s, sizeof(s) / sizeof(s[0]) - 1, kPointerInvalidIndex }
        #INDEX (i) {#i, sizeof(#i) - 1, i }

        static const Pointer::Token kTokens[] = {NAME("foo"),INDEX(123) };
        static const Pointer p(kTokens, sizeof(kTokens) / sizeof(kTokens[0]));
        // Эквивалентно static const Pointer p("/foo/123");

        #undef NAME
        #undef INDEX
        \endcode
    */
    GenericPointer(const Token* tokens, size_t tokenCount) : allocator_(), ownAllocator_(), nameBuffer_(), tokens_(const_cast<Token*>(tokens)), tokenCount_(tokenCount), parseErrorOffset_(), parseErrorCode_(kPointerParseErrorNone) {}

    //! Копировать конструктор.
    GenericPointer(const GenericPointer& rhs) : allocator_(), ownAllocator_(), nameBuffer_(), tokens_(), tokenCount_(), parseErrorOffset_(), parseErrorCode_(kPointerParseErrorNone) {
        *this = rhs;
    }

    //! Копировать конструктор.
    GenericPointer(const GenericPointer& rhs, Allocator* allocator) : allocator_(allocator), ownAllocator_(), nameBuffer_(), tokens_(), tokenCount_(), parseErrorOffset_(), parseErrorCode_(kPointerParseErrorNone) {
        *this = rhs;
    }

    //! Деструктор.
    ~GenericPointer() {
        if (nameBuffer_)    // Если используется конструктор токенов, предоставленный пользователем,nameBuffer_имеет значение nullptr, аtokens_не освобождается.
            Allocator::Free(tokens_);
        RAPIDJSON_DELETE(ownAllocator_);
    }

    //! Оператор присваивания.
    GenericPointer& operator=(const GenericPointer& rhs) {
        if (this != &rhs) {
            // Не удалять ownAllocator
            if (nameBuffer_)
                Allocator::Free(tokens_);

            tokenCount_ = rhs.tokenCount_;
            parseErrorOffset_ = rhs.parseErrorOffset_;
            parseErrorCode_ = rhs.parseErrorCode_;

            if (rhs.nameBuffer_)
                CopyFromRaw(rhs); // Обычно анализируются токены.
            else {
                tokens_ = rhs.tokens_; // Предоставленные пользователем константные токены.
                nameBuffer_ = 0;
            }
        }
        return *this;
    }

    //! Поменяйте местами содержимое этого указателя с другим.
    /*!
        \paramother Указатель для обмена.
        \note Постоянная сложность.
    */
    GenericPointer& Swap(GenericPointer& other) RAPIDJSON_NOEXCEPT {
        internal::Swap(allocator_, other.allocator_);
        internal::Swap(ownAllocator_, other.ownAllocator_);
        internal::Swap(nameBuffer_, other.nameBuffer_);
        internal::Swap(tokens_, other.tokens_);
        internal::Swap(tokenCount_, other.tokenCount_);
        internal::Swap(parseErrorOffset_, other.parseErrorOffset_);
        internal::Swap(parseErrorCode_, other.parseErrorCode_);
        return *this;
    }

    //! отдельно стоящий помощник функции подкачки
    /*!
        Вспомогательная функция для включения поддержки реализации общего шаблона подкачки на основе \c std::swap:
        \code
        void swap(MyClass& a, MyClass& b) {
            с помощью станд::своп;
            swap(a.pointer, b.pointer);
            // ...
        }
        \endcode
        \см.  Swap()
     */
    friend inline void swap(GenericPointer& a, GenericPointer& b) RAPIDJSON_NOEXCEPT { a.Swap(b); }

    //@}

    //! @nameДобавить токен
    //@{

    //! Добавить токен и вернуть новый указатель
    /*!
        \param token Добавляемый токен.
        \param allocator Распределитель вновь возвращающегося указателя.
        \return Новый указатель с добавленным токеном.
    */
    GenericPointer Append(const Token& token, Allocator* allocator = 0) const {
        GenericPointer r;
        r.allocator_ = allocator;
        Ch *p = r.CopyFromRaw(*this, 1, token.length + 1);
        std::memcpy(p, token.name, (token.length + 1) * sizeof(Ch));
        r.tokens_[tokenCount_].name = p;
        r.tokens_[tokenCount_].length = token.length;
        r.tokens_[tokenCount_].index = token.index;
        return r;
    }

    //! Добавьте токен имени с длиной и верните новый указатель.
    /*!
        \param name Добавляемое имя.
        \param length Длина имени.
        \param allocator Распределитель вновь возвращающегося указателя.
        \return Новый указатель с добавленным токеном.
    */
    GenericPointer Append(const Ch* name, SizeType length, Allocator* allocator = 0) const {
        Token token = { name, length, kPointerInvalidIndex };
        return Append(token, allocator);
    }

    //! Добавьте токен имени без длины и верните новый указатель.
    /*!
        \param name Имя (const Ch*), которое нужно добавить.
        \param allocator Распределитель вновь возвращающегося указателя.
        \return Новый указатель с добавленным токеном.
    */
    template <typename T>
    RAPIDJSON_DISABLEIF_RETURN((internal::NotExpr<internal::IsSame<typename internal::RemoveConst<T>::Type, Ch> >), (GenericPointer))
    Append(T* name, Allocator* allocator = 0) const {
        return Append(name, internal::StrLen(name), allocator);
    }

#if RAPIDJSON_HAS_STDSTRING
    //! Добавьте токен имени и верните новый указатель.
    /*!
        \param name Добавляемое имя.
        \param allocator Распределитель вновь возвращающегося указателя.
        \return Новый указатель с добавленным токеном.
    */
    GenericPointer Append(const std::basic_string<Ch>& name, Allocator* allocator = 0) const {
        return Append(name.c_str(), static_cast<SizeType>(name.size()), allocator);
    }
#endif

    //! Добавьте токен индекса и верните новый указатель.
    /*!
        \param index Добавляемый индекс.
        \param allocator Распределитель вновь возвращающегося указателя.
        \return Новый указатель с добавленным токеном.
    */
    GenericPointer Append(SizeType index, Allocator* allocator = 0) const {
        char buffer[21];
        char* end = sizeof(SizeType) == 4 ? internal::u32toa(index, buffer) : internal::u64toa(index, buffer);
        SizeType length = static_cast<SizeType>(end - buffer);
        buffer[length] = '\0';

        if (sizeof(Ch) == 1) {
            Token token = { reinterpret_cast<Ch*>(buffer), length, index };
            return Append(token, allocator);
        }
        else {
            Ch name[21];
            for (size_t i = 0; i <= length; i++)
                name[i] = static_cast<Ch>(buffer[i]);
            Token token = { name, length, index };
            return Append(token, allocator);
        }
    }

    //! Добавить токен по значению и вернуть новый указатель
    /*!
        \param token Токен, который нужно добавить.
        \param allocator Распределитель вновь возвращающегося указателя.
        \return Новый указатель с добавленным токеном.
    */
    GenericPointer Append(const ValueType& token, Allocator* allocator = 0) const {
        if (token.IsString())
            return Append(token.GetString(), token.GetStringLength(), allocator);
        else {
            RAPIDJSON_ASSERT(token.IsUint64());
            RAPIDJSON_ASSERT(token.GetUint64() <= SizeType(~0));
            return Append(static_cast<SizeType>(token.GetUint64()), allocator);
        }
    }

    //! @nameОбработка ошибок синтаксического анализа
    //@{

    //! Проверьте, является ли это действительным указателем.
    bool IsValid() const { return parseErrorCode_ == kPointerParseErrorNone; }

    //! Получите смещение ошибки синтаксического анализа в единице кода.
    size_t GetParseErrorOffset() const { return parseErrorOffset_; }

    //! Получите код ошибки синтаксического анализа.
    PointerParseErrorCode GetParseErrorCode() const { return parseErrorCode_; }

    //@}

    //! Получите распределитель этого указателя.
    Allocator& GetAllocator() { return *allocator_; }

    //! Токены @name
    //@{

    //! Получите массив токенов (только константная версия).
    const Token* GetTokens() const { return tokens_; }

    //! Получите количество жетонов.
    size_t GetTokenCount() const { return tokenCount_; }

    //@}

    //! @nameОператоры равенства/неравенства
    //@{

    //! Оператор равенства.
    /*!
        \note Если какие-либо указатели недействительны, всегда возвращается false.
    */
    bool operator==(const GenericPointer& rhs) const {
        if (!IsValid() || !rhs.IsValid() || tokenCount_ != rhs.tokenCount_)
            return false;

        for (size_t i = 0; i < tokenCount_; i++) {
            if (tokens_[i].index != rhs.tokens_[i].index ||
                tokens_[i].length != rhs.tokens_[i].length ||
                (tokens_[i].length != 0 && std::memcmp(tokens_[i].name, rhs.tokens_[i].name, sizeof(Ch)* tokens_[i].length) != 0))
            {
                return false;
            }
        }

        return true;
    }

    //! Оператор неравенства.
    /*!
        \note Если какие-либо указатели недействительны, всегда возвращается true.
    */
    bool operator!=(const GenericPointer& rhs) const { return !(*this == rhs); }

    //! Меньше, чем оператор.
    /*!
        \note Неверные указатели всегда более существенны.
    */
    bool operator<(const GenericPointer& rhs) const {
        if (!IsValid())
            return false;
        if (!rhs.IsValid())
            return true;

        if (tokenCount_ != rhs.tokenCount_)
            return tokenCount_ < rhs.tokenCount_;

        for (size_t i = 0; i < tokenCount_; i++) {
            if (tokens_[i].index != rhs.tokens_[i].index)
                return tokens_[i].index < rhs.tokens_[i].index;

            if (tokens_[i].length != rhs.tokens_[i].length)
                return tokens_[i].length < rhs.tokens_[i].length;

            if (int cmp = std::memcmp(tokens_[i].name, rhs.tokens_[i].name, sizeof(Ch) * tokens_[i].length))
                return cmp < 0;
        }

        return false;
    }

    //@}

    //! @nameStringify
    //@{

    //! Преобразование указателя в строковое представление.
    /*!
        \tparam OutputStream Тип выходного потока.
        \param os Выходной поток.
    */
    template<typename OutputStream>
    bool Stringify(OutputStream& os) const {
        return Stringify<false, OutputStream>(os);
    }

    //! Преобразуйте указатель в строку представления фрагмента URI.
    /*!
        \tparam OutputStream Тип выходного потока.
        \param os Выходной поток.
    */
    template<typename OutputStream>
    bool StringifyUriFragment(OutputStream& os) const {
        return Stringify<true, OutputStream>(os);
    }

    //@}

    //! @nameСоздать стоимость
    //@{

    //! Создайте значение в поддереве.
    /*!
        Если значение не существует, создаются все родительские значения и нулевое значение JSON.
        Таким образом, он всегда добивается успеха и возвращает вновь созданную или существующую ценность.

        Напоминаем, что он может менять типы родителей в зависимости от токенов, поэтому он
        потенциально удаляет ранее сохраненные значения. Например, если документ
        был массивом, а "/foo" используется для создания значений, тогда документ
        будет изменен на объект, и все существующие элементы массива будут потеряны.

        \param root Корневое значение поддереваDOM, которое необходимо разрешить. Это может быть любое значение, кроме основного документа.
        \param allocator Распределитель для создания отсчетов, если указанное значение или его родительские элементы не существуют.
        \param ужеExist Если значение не равнозначно, сохраняется информация о том, что существует уже разрешенное значение.
        \return Разрешенное вновь созданное значение (нулевое значениеJSON) или уже существующее значение.
    */
    ValueType& Create(ValueType& root, typename ValueType::AllocatorType& allocator, bool* alreadyExist = 0) const {
        RAPIDJSON_ASSERT(IsValid());
        ValueType* v = &root;
        bool exist = true;
        for (const Token *t = tokens_; t != tokens_ + tokenCount_; ++t) {
            if (v->IsArray() && t->name[0] == '-' && t->length == 1) {
                v->PushBack(ValueType().Move(), allocator);
                v = &((*v)[v->Size() - 1]);
                exist = false;
            }
            else {
                if (t->index == kPointerInvalidIndex) { // должно быть имя объекта
                    if (!v->IsObject())
                        v->SetObject(); // Перейти к объекту
                }
                else { // имя объекта или индекс массива
                    if (!v->IsArray() && !v->IsObject())
                        v->SetArray(); // Перейти к массиву
                }

                if (v->IsArray()) {
                    if (t->index >= v->Size()) {
                        v->Reserve(t->index + 1, allocator);
                        while (t->index >= v->Size())
                            v->PushBack(ValueType().Move(), allocator);
                        exist = false;
                    }
                    v = &((*v)[t->index]);
                }
                else {
                    typename ValueType::MemberIterator m = v->FindMember(GenericValue<EncodingType>(GenericStringRef<Ch>(t->name, t->length)));
                    if (m == v->MemberEnd()) {
                        v->AddMember(ValueType(t->name, t->length, allocator).Move(), ValueType().Move(), allocator);
                        m = v->MemberEnd();
                        v = &(--m)->value; // Предполагается, чтоAddMember()будет добавлен в конце.
                        exist = false;
                    }
                    else
                        v = &m->value;
                }
            }
        }

        if (alreadyExist)
            *alreadyExist = exist;

        return *v;
    }

    //! Создает значение в документе.
    /*!
        \param document Документ, который необходимо разрешить.
        \param ужеExist Если значение не равнозначно, сохраняется информация о том, что существует уже разрешенное значение.
        \return Разрешенное вновь созданное или уже существующее значение.
    */
    template <typename stackAllocator>
    ValueType& Create(GenericDocument<EncodingType, typename ValueType::AllocatorType, stackAllocator>& document, bool* alreadyExist = 0) const {
        return Create(document, document.GetAllocator(), alreadyExist);
    }

    //@}

    //! @nameВычислить URI
    //@{

    //! Вычислите URI в области видимости для поддерева.
    //  Для использования с указателями JSON в документах схемы JSON.
    /*!
        \param root Корневое значение поддереваDOM, которое необходимо разрешить. Это может быть любое значение, кроме основного документа.
        \param rootUri Корень URI
        \param unsolvedTokenIndex Если указатель не может разрешить токен в указателе, этот параметр может получить индекс неразрешенного токена.
        \param allocator Распределитель для Uris
        \return Ури, если это можно решить. В противном случае ноль.

        \note
        Есть только 3 ситуации, когда проблема URI не может быть решена:
        1. Значение в пути не является ни массивом, ни объектом.
        2. Значение объекта не содержит токена.
        3. Токен находится вне диапазона значения массива.

        Используйте unsolvedTokenIndex для получения индекса токена.
    */
    UriType GetUri(ValueType& root, const UriType& rootUri, size_t* unresolvedTokenIndex = 0, Allocator* allocator = 0) const {
        static const Ch kIdString[] = { 'i', 'd', '\0' };
        static const ValueType kIdValue(kIdString, 2);
        UriType base = UriType(rootUri, allocator);
        RAPIDJSON_ASSERT(IsValid());
        ValueType* v = &root;
        for (const Token *t = tokens_; t != tokens_ + tokenCount_; ++t) {
            switch (v->GetType()) {
                case kObjectType:
                {
                    // Посмотрите, есть ли у нас идентификатор, и если да, разрешите его с текущей базой.
                    typename ValueType::MemberIterator m = v->FindMember(kIdValue);
                    if (m != v->MemberEnd() && (m->value).IsString()) {
                        UriType here = UriType(m->value, allocator).Resolve(base, allocator);
                        base = here;
                    }
                    m = v->FindMember(GenericValue<EncodingType>(GenericStringRef<Ch>(t->name, t->length)));
                    if (m == v->MemberEnd())
                        break;
                    v = &m->value;
                }
                  continue;
                case kArrayType:
                    if (t->index == kPointerInvalidIndex || t->index >= v->Size())
                        break;
                    v = &((*v)[t->index]);
                    continue;
                default:
                    break;
            }

            // Error: неразрешенный токен
            if (unresolvedTokenIndex)
                *unresolvedTokenIndex = static_cast<size_t>(t - tokens_);
            return UriType(allocator);
        }
        return base;
    }

    UriType GetUri(const ValueType& root, const UriType& rootUri, size_t* unresolvedTokenIndex = 0, Allocator* allocator = 0) const {
      return GetUri(const_cast<ValueType&>(root), rootUri, unresolvedTokenIndex, allocator);
    }


    //! @nameЗначение запроса
    //@{

    //! Запросить значение в поддереве.
    /*!
        \param root Корневое значение поддереваDOM, которое необходимо разрешить. Это может быть любое значение, кроме основного документа.
        \param unsolvedTokenIndex Если указатель не может разрешить токен в указателе, этот параметр может получить индекс неразрешенного токена.
        \return Указатель значения, если его можно разрешить. В противном случае ноль.

        \note
        Есть только 3 ситуации, когда значение не может быть разрешено:
        1. Значение в пути не является ни массивом, ни объектом.
        2. Значение объекта не содержит токена.
        3. Токен находится вне диапазона значения массива.

        Используйте unsolvedTokenIndex для получения индекса токена.
    */
    ValueType* Get(ValueType& root, size_t* unresolvedTokenIndex = 0) const {
        RAPIDJSON_ASSERT(IsValid());
        ValueType* v = &root;
        for (const Token *t = tokens_; t != tokens_ + tokenCount_; ++t) {
            switch (v->GetType()) {
            case kObjectType:
                {
                    typename ValueType::MemberIterator m = v->FindMember(GenericValue<EncodingType>(GenericStringRef<Ch>(t->name, t->length)));
                    if (m == v->MemberEnd())
                        break;
                    v = &m->value;
                }
                continue;
            case kArrayType:
                if (t->index == kPointerInvalidIndex || t->index >= v->Size())
                    break;
                v = &((*v)[t->index]);
                continue;
            default:
                break;
            }

            // Error: неразрешенный токен
            if (unresolvedTokenIndex)
                *unresolvedTokenIndex = static_cast<size_t>(t - tokens_);
            return 0;
        }
        return v;
    }

    //! Запросить константное значение в константном поддереве.
    /*!
        \param root Корневое значение поддереваDOM, которое необходимо разрешить. Это может быть любое значение, кроме основного документа.
        \return Указатель значения, если его можно разрешить. В противном случае ноль.
    */
    const ValueType* Get(const ValueType& root, size_t* unresolvedTokenIndex = 0) const {
        return Get(const_cast<ValueType&>(root), unresolvedTokenIndex);
    }

    //@}

    //! @nameЗапросить значение по умолчанию
    //@{

    //! Запросить значение в поддереве со значением по умолчанию.
    /*!
        АналогичноGet(), но если указанное значение не существует, определение всех родительских элементов и клонируется значением по умолчанию.
        Так что эта функция всегда выполняется.

        \param root Корневое значение поддереваDOM, которое необходимо разрешить. Это может быть любое значение, кроме основного документа.
        \param defaultValue Значение по умолчанию, которое будет клонировано, если значение не существует.
        \param allocator Распределитель для создания отсчетов, если указанное значение или его родительские элементы не существуют.
        \см.  Create()
    */
    ValueType& GetWithDefault(ValueType& root, const ValueType& defaultValue, typename ValueType::AllocatorType& allocator) const {
        bool alreadyExist;
        ValueType& v = Create(root, allocator, &alreadyExist);
        return alreadyExist ? v : v.CopyFrom(defaultValue, allocator);
    }

    //! Запросите значение в поддереве со строкой по умолчанию, завершающейся нулем.
    ValueType& GetWithDefault(ValueType& root, const Ch* defaultValue, typename ValueType::AllocatorType& allocator) const {
        bool alreadyExist;
        ValueType& v = Create(root, allocator, &alreadyExist);
        return alreadyExist ? v : v.SetString(defaultValue, allocator);
    }

#if RAPIDJSON_HAS_STDSTRING
    //! Запросите значение в поддереве с помощью std::basic_string по умолчанию.
    ValueType& GetWithDefault(ValueType& root, const std::basic_string<Ch>& defaultValue, typename ValueType::AllocatorType& allocator) const {
        bool alreadyExist;
        ValueType& v = Create(root, allocator, &alreadyExist);
        return alreadyExist ? v : v.SetString(defaultValue, allocator);
    }
#endif

    //! Запросите значение в поддереве с примитивным значением по умолчанию.
    /*!
        \tparam T Либо \ref Type, \c int, \c unsigned, \cint64_t, \cuint64_t, \c bool
    */
    template <typename T>
    RAPIDJSON_DISABLEIF_RETURN((internal::OrExpr<internal::IsPointer<T>, internal::IsGenericValue<T> >), (ValueType&))
    GetWithDefault(ValueType& root, T defaultValue, typename ValueType::AllocatorType& allocator) const {
        return GetWithDefault(root, ValueType(defaultValue).Move(), allocator);
    }

    //! Запросить значение в документе со значением по умолчанию.
    template <typename stackAllocator>
    ValueType& GetWithDefault(GenericDocument<EncodingType, typename ValueType::AllocatorType, stackAllocator>& document, const ValueType& defaultValue) const {
        return GetWithDefault(document, defaultValue, document.GetAllocator());
    }

    //! Запросите значение в документе со строкой по умолчанию, завершающейся нулем.
    template <typename stackAllocator>
    ValueType& GetWithDefault(GenericDocument<EncodingType, typename ValueType::AllocatorType, stackAllocator>& document, const Ch* defaultValue) const {
        return GetWithDefault(document, defaultValue, document.GetAllocator());
    }

#if RAPIDJSON_HAS_STDSTRING
    //! Запросите значение в документе с помощью std::basic_string по умолчанию.
    template <typename stackAllocator>
    ValueType& GetWithDefault(GenericDocument<EncodingType, typename ValueType::AllocatorType, stackAllocator>& document, const std::basic_string<Ch>& defaultValue) const {
        return GetWithDefault(document, defaultValue, document.GetAllocator());
    }
#endif

    //! Запросите значение в документе с примитивным значением по умолчанию.
    /*!
        \tparam T Либо \ref Type, \c int, \c unsigned, \cint64_t, \cuint64_t, \c bool
    */
    template <typename T, typename stackAllocator>
    RAPIDJSON_DISABLEIF_RETURN((internal::OrExpr<internal::IsPointer<T>, internal::IsGenericValue<T> >), (ValueType&))
    GetWithDefault(GenericDocument<EncodingType, typename ValueType::AllocatorType, stackAllocator>& document, T defaultValue) const {
        return GetWithDefault(document, defaultValue, document.GetAllocator());
    }

    //@}

    //! @nameУстановить значение
    //@{

    //! Установите значение в поддереве с семантикой перемещения.
    /*!
        Он создает всех родителей, если они не существуют или типы отличаются от токенов.
        Таким образом, эта функция всегда завершается успешно, но потенциально удаляет существующие значения.

        \param root Корневое значение поддереваDOM, которое необходимо разрешить. Это может быть любое значение, кроме основного документа.
        \param value Устанавливаемое значение.
        \param allocator Распределитель для создания отсчетов, если указанное значение или его родительские элементы не существуют.
        \см.  Create()
    */
    ValueType& Set(ValueType& root, ValueType& value, typename ValueType::AllocatorType& allocator) const {
        return Create(root, allocator) = value;
    }

    //! Установите значение в поддереве с семантикой копирования.
    ValueType& Set(ValueType& root, const ValueType& value, typename ValueType::AllocatorType& allocator) const {
        return Create(root, allocator).CopyFrom(value, allocator);
    }

    //! Установите строку с нулевым завершением в поддереве.
    ValueType& Set(ValueType& root, const Ch* value, typename ValueType::AllocatorType& allocator) const {
        return Create(root, allocator) = ValueType(value, allocator).Move();
    }

#if RAPIDJSON_HAS_STDSTRING
    //! Установите std::basic_string в поддереве.
    ValueType& Set(ValueType& root, const std::basic_string<Ch>& value, typename ValueType::AllocatorType& allocator) const {
        return Create(root, allocator) = ValueType(value, allocator).Move();
    }
#endif

    //! Установите примитивное значение в поддереве.
    /*!
        \tparam T Либо \ref Type, \c int, \c unsigned, \cint64_t, \cuint64_t, \c bool
    */
    template <typename T>
    RAPIDJSON_DISABLEIF_RETURN((internal::OrExpr<internal::IsPointer<T>, internal::IsGenericValue<T> >), (ValueType&))
    Set(ValueType& root, T value, typename ValueType::AllocatorType& allocator) const {
        return Create(root, allocator) = ValueType(value).Move();
    }

    //! Установите значение в документе с семантикой перемещения.
    template <typename stackAllocator>
    ValueType& Set(GenericDocument<EncodingType, typename ValueType::AllocatorType, stackAllocator>& document, ValueType& value) const {
        return Create(document) = value;
    }

    //! Установите значение в документе с семантикой копирования.
    template <typename stackAllocator>
    ValueType& Set(GenericDocument<EncodingType, typename ValueType::AllocatorType, stackAllocator>& document, const ValueType& value) const {
        return Create(document).CopyFrom(value, document.GetAllocator());
    }

    //! Установите в документе строку с нулевым завершением.
    template <typename stackAllocator>
    ValueType& Set(GenericDocument<EncodingType, typename ValueType::AllocatorType, stackAllocator>& document, const Ch* value) const {
        return Create(document) = ValueType(value, document.GetAllocator()).Move();
    }

#if RAPIDJSON_HAS_STDSTRING
    //! Устанавливает std::basic_string в документ.
    template <typename stackAllocator>
    ValueType& Set(GenericDocument<EncodingType, typename ValueType::AllocatorType, stackAllocator>& document, const std::basic_string<Ch>& value) const {
        return Create(document) = ValueType(value, document.GetAllocator()).Move();
    }
#endif

    //! Установите примитивное значение в документе.
    /*!
    \tparam T Либо \ref Type, \c int, \c unsigned, \cint64_t, \cuint64_t, \c bool
    */
    template <typename T, typename stackAllocator>
    RAPIDJSON_DISABLEIF_RETURN((internal::OrExpr<internal::IsPointer<T>, internal::IsGenericValue<T> >), (ValueType&))
        Set(GenericDocument<EncodingType, typename ValueType::AllocatorType, stackAllocator>& document, T value) const {
            return Create(document) = value;
    }

    //@}

    //! @nameПоменять значение
    //@{

    //! Поменяйте местами значение на значение в поддереве.
    /*!
        Он создает всех родителей, если они не существуют или типы отличаются от токенов.
        Таким образом, эта функция всегда завершается успешно, но потенциально удаляет существующие значения.

        \param root Корневое значение поддереваDOM, которое необходимо разрешить. Это может быть любое значение, кроме основного документа.
        Значение \param для замены.
        \param allocator Распределитель для создания отсчетов, если указанное значение или его родительские элементы не существуют.
        \см.  Create()
    */
    ValueType& Swap(ValueType& root, ValueType& value, typename ValueType::AllocatorType& allocator) const {
        return Create(root, allocator).Swap(value);
    }

    //! Замените значение на значение в документе.
    template <typename stackAllocator>
    ValueType& Swap(GenericDocument<EncodingType, typename ValueType::AllocatorType, stackAllocator>& document, ValueType& value) const {
        return Create(document).Swap(value);
    }

    //@}

    //! Удалить значение в поддереве.
    /*!
        \param root Корневое значение поддереваDOM, которое необходимо разрешить. Это может быть любое значение, кроме основного документа.
        \return Будет ли найдено и удалено разрешенное значение.

        \note Стирание с пустым указателем \c Pointer(""), т.е. в корне всегда завершается неудачно и возвращает ложь.
    */
    bool Erase(ValueType& root) const {
        RAPIDJSON_ASSERT(IsValid());
        if (tokenCount_ == 0) // Не могу стереть рут
            return false;

        ValueType* v = &root;
        const Token* last = tokens_ + (tokenCount_ - 1);
        for (const Token *t = tokens_; t != last; ++t) {
            switch (v->GetType()) {
            case kObjectType:
                {
                    typename ValueType::MemberIterator m = v->FindMember(GenericValue<EncodingType>(GenericStringRef<Ch>(t->name, t->length)));
                    if (m == v->MemberEnd())
                        return false;
                    v = &m->value;
                }
                break;
            case kArrayType:
                if (t->index == kPointerInvalidIndex || t->index >= v->Size())
                    return false;
                v = &((*v)[t->index]);
                break;
            default:
                return false;
            }
        }

        switch (v->GetType()) {
        case kObjectType:
            return v->EraseMember(GenericStringRef<Ch>(last->name, last->length));
        case kArrayType:
            if (last->index == kPointerInvalidIndex || last->index >= v->Size())
                return false;
            v->Erase(v->Begin() + last->index);
            return true;
        default:
            return false;
        }
    }

private:
    //! Клонируйте сюда критерии Rhs.
    /*!
        \param rhs Указатель источника.
        \param extraToken Дополнительные токены, которые необходимо выделить.
        \param extraNameBufferSize Размер дополнительного буфера имен (во многих каналах), который будет выделен.
        \return Начало незанятого буфера имен для хранения дополнительных имен.
    */
    Ch* CopyFromRaw(const GenericPointer& rhs, size_t extraToken = 0, size_t extraNameBufferSize = 0) {
        if (!allocator_) // распределитель находится в независимой собственности.
            ownAllocator_ = allocator_ = RAPIDJSON_NEW(Allocator)();

        size_t nameBufferSize = rhs.tokenCount_; // нулевые терминаторы для токенов
        for (Token *t = rhs.tokens_; t != rhs.tokens_ + rhs.tokenCount_; ++t)
            nameBufferSize += t->length;

        tokenCount_ = rhs.tokenCount_ + extraToken;
        tokens_ = static_cast<Token *>(allocator_->Malloc(tokenCount_ * sizeof(Token) + (nameBufferSize + extraNameBufferSize) * sizeof(Ch)));
        nameBuffer_ = reinterpret_cast<Ch *>(tokens_ + tokenCount_);
        if (rhs.tokenCount_ > 0) {
            std::memcpy(tokens_, rhs.tokens_, rhs.tokenCount_ * sizeof(Token));
        }
        if (nameBufferSize > 0) {
            std::memcpy(nameBuffer_, rhs.nameBuffer_, nameBufferSize * sizeof(Ch));
        }

        // Настройте указатели на буфер имен
        std::ptrdiff_t diff = nameBuffer_ - rhs.nameBuffer_;
        for (Token *t = tokens_; t != tokens_ + rhs.tokenCount_; ++t)
            t->name += diff;

        return nameBuffer_ + nameBufferSize;
    }

    //! Проверьте, должен ли символ быть закодирован в процентах.
    /*!
        Согласно RFC 3986 2.3 Незарезервированные символы.
        \param c Символ (кодовая единица), который необходимо протестировать.
    */
    bool NeedPercentEncode(Ch c) const {
        return !((c >= '0' && c <= '9') || (c >= 'A' && c <='Z') || (c >= 'a' && c <= 'z') || c == '-' || c == '.' || c == '_' || c =='~');
    }

    //! Разберите строку JSON или ее представление фрагмента URI на токены.
#ifndef __clang__ // -Документация
    /*!
        \param source Либо строка указателяJSON, либо ее фрагментное представлениеURI. Не обязательно иметь нулевое завершение.
        \param length Длина исходной строки.
        \note Источник не может быть строковым представлениемJSONуказателяJSON, например. В "/\u0000" \u0000 не будет неэкранированным.
    */
#endif
    void Parse(const Ch* source, size_t length) {
        RAPIDJSON_ASSERT(source != NULL);
        RAPIDJSON_ASSERT(nameBuffer_ == 0);
        RAPIDJSON_ASSERT(tokens_ == 0);

        // Создайте собственный распределитель, если пользователь не предоставил его.
        if (!allocator_)
            ownAllocator_ = allocator_ = RAPIDJSON_NEW(Allocator)();

        // Подпишите количество '/' как tokenCount
        tokenCount_ = 0;
        for (const Ch* s = source; s != source + length; s++)
            if (*s == '/')
                tokenCount_++;

        Token* token = tokens_ = static_cast<Token *>(allocator_->Malloc(tokenCount_ * sizeof(Token) + length * sizeof(Ch)));
        Ch* name = nameBuffer_ = reinterpret_cast<Ch *>(tokens_ + tokenCount_);
        size_t i = 0;

        // Определить, является ли это фрагментом URI
        bool uriFragment = false;
        if (source[i] == '#') {
            uriFragment = true;
            i++;
        }

        if (i != length && source[i] != '/') {
            parseErrorCode_ = kPointerParseErrorTokenMustBeginWithSolidus;
            goto error;
        }

        while (i < length) {
            RAPIDJSON_ASSERT(source[i] == '/');
            i++; // потребляет '/'

            token->name = name;
            bool isNumber = true;

            while (i < length && source[i] != '/') {
                Ch c = source[i];
                if (uriFragment) {
                    // Декодирование процентной кодировки фрагмента URI
                    if (c == '%') {
                        PercentDecodeStream is(&source[i], source + length);
                        GenericInsituStringStream<EncodingType> os(name);
                        Ch* begin = os.PutBegin();
                        if (!Transcoder<UTF8<>, EncodingType>().Validate(is, os) || !is.IsValid()) {
                            parseErrorCode_ = kPointerParseErrorInvalidPercentEncoding;
                            goto error;
                        }
                        size_t len = os.PutEnd(begin);
                        i += is.Tell() - 1;
                        if (len == 1)
                            c = *name;
                        else {
                            name += len;
                            isNumber = false;
                            i++;
                            continue;
                        }
                    }
                    else if (NeedPercentEncode(c)) {
                        parseErrorCode_ = kPointerParseErrorCharacterMustPercentEncode;
                        goto error;
                    }
                }

                i++;

                // Экранирование "~0" -> '~', "~1" -> '/'
                if (c == '~') {
                    if (i < length) {
                        c = source[i];
                        if (c == '0')       c = '~';
                        else if (c == '1')  c = '/';
                        else {
                            parseErrorCode_ = kPointerParseErrorInvalidEscape;
                            goto error;
                        }
                        i++;
                    }
                    else {
                        parseErrorCode_ = kPointerParseErrorInvalidEscape;
                        goto error;
                    }
                }

                // Сначала проверьте индекс: все символы — цифры.
                if (c < '0' || c > '9')
                    isNumber = false;

                *name++ = c;
            }
            token->length = static_cast<SizeType>(name - token->name);
            if (token->length == 0)
                isNumber = false;
            *name++ = '\0'; // Нулевой терминатор

            // Вторая проверка индекса: более одной цифры не могут иметь ведущий нуль.
            if (isNumber && token->length > 1 && token->name[0] == '0')
                isNumber = false;

            // Преобразование строк в SizeType
            SizeType n = 0;
            if (isNumber) {
                for (size_t j = 0; j < token->length; j++) {
                    SizeType m = n * 10 + static_cast<SizeType>(token->name[j] - '0');
                    if (m < n) {   // обнаружение переполнения
                        isNumber = false;
                        break;
                    }
                    n = m;
                }
            }

            token->index = isNumber ? n : kPointerInvalidIndex;
            token++;
        }

        RAPIDJSON_ASSERT(name <= nameBuffer_ + length); // Не должен переполнять буфер
        parseErrorCode_ = kPointerParseErrorNone;
        return;

    error:
        Allocator::Free(tokens_);
        nameBuffer_ = 0;
        tokens_ = 0;
        tokenCount_ = 0;
        parseErrorOffset_ = i;
        return;
    }

    //! Преобразование в строку или представление фрагмента URI.
    /*!
        \tparam uriFragment True для преобразования в код представления фрагментаURI. Ложь для строкового представления.
        \tparam OutputStream — тип выходного потока.
        \param os Выходной поток.
    */
    template<bool uriFragment, typename OutputStream>
    bool Stringify(OutputStream& os) const {
        RAPIDJSON_ASSERT(IsValid());

        if (uriFragment)
            os.Put('#');

        for (Token *t = tokens_; t != tokens_ + tokenCount_; ++t) {
            os.Put('/');
            for (size_t j = 0; j < t->length; j++) {
                Ch c = t->name[j];
                if (c == '~') {
                    os.Put('~');
                    os.Put('0');
                }
                else if (c == '/') {
                    os.Put('~');
                    os.Put('1');
                }
                else if (uriFragment && NeedPercentEncode(c)) {
                    // Перекодировать в последовательность UTF8
                    GenericStringStream<typename ValueType::EncodingType> source(&t->name[j]);
                    PercentEncodeStream<OutputStream> target(os);
                    if (!Transcoder<EncodingType, UTF8<> >().Validate(source, target))
                        return false;
                    j += source.Tell() - 1;
                }
                else
                    os.Put(c);
            }
        }
        return true;
    }

    //! Вспомогательный поток для декодирования последовательности с процентным кодированием в единицу кода.
    /*!
        Этот поток декодирует триплет % XY в кодовую единицу (0–255).
        Если он встречает недопустимые символы, он устанавливает единицу выходного кода как 0 и
        пометить как недействительный и должен быть проверенIsValid().
    */
    class PercentDecodeStream {
    public:
        typedef typename ValueType::Ch Ch;

        //! Конструктор
        /*!
            \param source Начало потока
            \param end Конец потока.
        */
        PercentDecodeStream(const Ch* source, const Ch* end) : src_(source), head_(source), end_(end), valid_(true) {}

        Ch Take() {
            if (*src_ != '%' || src_ + 3 > end_) { // % триплета XY
                valid_ = false;
                return 0;
            }
            src_++;
            Ch c = 0;
            for (int j = 0; j < 2; j++) {
                c = static_cast<Ch>(c << 4);
                Ch h = *src_;
                if      (h >= '0' && h <= '9') c = static_cast<Ch>(c + h - '0');
                else if (h >= 'A' && h <= 'F') c = static_cast<Ch>(c + h - 'A' + 10);
                else if (h >= 'a' && h <= 'f') c = static_cast<Ch>(c + h - 'a' + 10);
                else {
                    valid_ = false;
                    return 0;
                }
                src_++;
            }
            return c;
        }

        size_t Tell() const { return static_cast<size_t>(src_ - head_); }
        bool IsValid() const { return valid_; }

    private:
        const Ch* src_;     //!< Текущая позиция чтения.
        const Ch* head_;    //!< Исходный заголовок строки.
        const Ch* end_;     //!< Позиция за концом.
        bool valid_;        //!< Действителен ли анализ.
    };

    //! Вспомогательный поток для кодирования символов (кодовая единица UTF -8) в последовательность с процентным кодированием.
    template <typename OutputStream>
    class PercentEncodeStream {
    public:
        PercentEncodeStream(OutputStream& os) : os_(os) {}
        void Put(char c) { // UTF -8 должен быть байтом
            unsigned char u = static_cast<unsigned char>(c);
            static const char hexDigits[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
            os_.Put('%');
            os_.Put(static_cast<typename OutputStream::Ch>(hexDigits[u >> 4]));
            os_.Put(static_cast<typename OutputStream::Ch>(hexDigits[u & 15]));
        }
    private:
        OutputStream& os_;
    };

    Allocator* allocator_;                  //!< Текущий распределитель. Он либо пользуется пользователем, либо равенownAllocator_.
    Allocator* ownAllocator_;               //!< Распределитель, принадлежащий этому указателю.
    Ch* nameBuffer_;                        //!< Буфер, содержащий все имена в токенах.
    Token* tokens_;                         //!< Список токенов.
    size_t tokenCount_;                     //!< Количество токенов вtokens_.
    size_t parseErrorOffset_;               //!< Смещение в единице кода при сбое синтаксического анализа.
    PointerParseErrorCode parseErrorCode_;  //!< Код ошибки синтаксического анализа.
};

//! GenericPointer для значений (UTF-8, распределитель по умолчанию).
typedef GenericPointer<Value> Pointer;

//! @nameСправочные функции для GenericPointer
//@{

//////////////////////////////////////////////////////////////////////////////

template <typename T>
typename T::ValueType& CreateValueByPointer(T& root, const GenericPointer<typename T::ValueType>& pointer, typename T::AllocatorType& a) {
    return pointer.Create(root, a);
}

template <typename T, typename CharType, size_t N>
typename T::ValueType& CreateValueByPointer(T& root, const CharType(&source)[N], typename T::AllocatorType& a) {
    return GenericPointer<typename T::ValueType>(source, N - 1).Create(root, a);
}

// Нет параметра распределителя

template <typename DocumentType>
typename DocumentType::ValueType& CreateValueByPointer(DocumentType& document, const GenericPointer<typename DocumentType::ValueType>& pointer) {
    return pointer.Create(document);
}

template <typename DocumentType, typename CharType, size_t N>
typename DocumentType::ValueType& CreateValueByPointer(DocumentType& document, const CharType(&source)[N]) {
    return GenericPointer<typename DocumentType::ValueType>(source, N - 1).Create(document);
}

//////////////////////////////////////////////////////////////////////////////

template <typename T>
typename T::ValueType* GetValueByPointer(T& root, const GenericPointer<typename T::ValueType>& pointer, size_t* unresolvedTokenIndex = 0) {
    return pointer.Get(root, unresolvedTokenIndex);
}

template <typename T>
const typename T::ValueType* GetValueByPointer(const T& root, const GenericPointer<typename T::ValueType>& pointer, size_t* unresolvedTokenIndex = 0) {
    return pointer.Get(root, unresolvedTokenIndex);
}

template <typename T, typename CharType, size_t N>
typename T::ValueType* GetValueByPointer(T& root, const CharType (&source)[N], size_t* unresolvedTokenIndex = 0) {
    return GenericPointer<typename T::ValueType>(source, N - 1).Get(root, unresolvedTokenIndex);
}

template <typename T, typename CharType, size_t N>
const typename T::ValueType* GetValueByPointer(const T& root, const CharType(&source)[N], size_t* unresolvedTokenIndex = 0) {
    return GenericPointer<typename T::ValueType>(source, N - 1).Get(root, unresolvedTokenIndex);
}

//////////////////////////////////////////////////////////////////////////////

template <typename T>
typename T::ValueType& GetValueByPointerWithDefault(T& root, const GenericPointer<typename T::ValueType>& pointer, const typename T::ValueType& defaultValue, typename T::AllocatorType& a) {
    return pointer.GetWithDefault(root, defaultValue, a);
}

template <typename T>
typename T::ValueType& GetValueByPointerWithDefault(T& root, const GenericPointer<typename T::ValueType>& pointer, const typename T::Ch* defaultValue, typename T::AllocatorType& a) {
    return pointer.GetWithDefault(root, defaultValue, a);
}

#if RAPIDJSON_HAS_STDSTRING
template <typename T>
typename T::ValueType& GetValueByPointerWithDefault(T& root, const GenericPointer<typename T::ValueType>& pointer, const std::basic_string<typename T::Ch>& defaultValue, typename T::AllocatorType& a) {
    return pointer.GetWithDefault(root, defaultValue, a);
}
#endif

template <typename T, typename T2>
RAPIDJSON_DISABLEIF_RETURN((internal::OrExpr<internal::IsPointer<T2>, internal::IsGenericValue<T2> >), (typename T::ValueType&))
GetValueByPointerWithDefault(T& root, const GenericPointer<typename T::ValueType>& pointer, T2 defaultValue, typename T::AllocatorType& a) {
    return pointer.GetWithDefault(root, defaultValue, a);
}

template <typename T, typename CharType, size_t N>
typename T::ValueType& GetValueByPointerWithDefault(T& root, const CharType(&source)[N], const typename T::ValueType& defaultValue, typename T::AllocatorType& a) {
    return GenericPointer<typename T::ValueType>(source, N - 1).GetWithDefault(root, defaultValue, a);
}

template <typename T, typename CharType, size_t N>
typename T::ValueType& GetValueByPointerWithDefault(T& root, const CharType(&source)[N], const typename T::Ch* defaultValue, typename T::AllocatorType& a) {
    return GenericPointer<typename T::ValueType>(source, N - 1).GetWithDefault(root, defaultValue, a);
}

#if RAPIDJSON_HAS_STDSTRING
template <typename T, typename CharType, size_t N>
typename T::ValueType& GetValueByPointerWithDefault(T& root, const CharType(&source)[N], const std::basic_string<typename T::Ch>& defaultValue, typename T::AllocatorType& a) {
    return GenericPointer<typename T::ValueType>(source, N - 1).GetWithDefault(root, defaultValue, a);
}
#endif

template <typename T, typename CharType, size_t N, typename T2>
RAPIDJSON_DISABLEIF_RETURN((internal::OrExpr<internal::IsPointer<T2>, internal::IsGenericValue<T2> >), (typename T::ValueType&))
GetValueByPointerWithDefault(T& root, const CharType(&source)[N], T2 defaultValue, typename T::AllocatorType& a) {
    return GenericPointer<typename T::ValueType>(source, N - 1).GetWithDefault(root, defaultValue, a);
}

// Нет параметра распределителя

template <typename DocumentType>
typename DocumentType::ValueType& GetValueByPointerWithDefault(DocumentType& document, const GenericPointer<typename DocumentType::ValueType>& pointer, const typename DocumentType::ValueType& defaultValue) {
    return pointer.GetWithDefault(document, defaultValue);
}

template <typename DocumentType>
typename DocumentType::ValueType& GetValueByPointerWithDefault(DocumentType& document, const GenericPointer<typename DocumentType::ValueType>& pointer, const typename DocumentType::Ch* defaultValue) {
    return pointer.GetWithDefault(document, defaultValue);
}

#if RAPIDJSON_HAS_STDSTRING
template <typename DocumentType>
typename DocumentType::ValueType& GetValueByPointerWithDefault(DocumentType& document, const GenericPointer<typename DocumentType::ValueType>& pointer, const std::basic_string<typename DocumentType::Ch>& defaultValue) {
    return pointer.GetWithDefault(document, defaultValue);
}
#endif

template <typename DocumentType, typename T2>
RAPIDJSON_DISABLEIF_RETURN((internal::OrExpr<internal::IsPointer<T2>, internal::IsGenericValue<T2> >), (typename DocumentType::ValueType&))
GetValueByPointerWithDefault(DocumentType& document, const GenericPointer<typename DocumentType::ValueType>& pointer, T2 defaultValue) {
    return pointer.GetWithDefault(document, defaultValue);
}

template <typename DocumentType, typename CharType, size_t N>
typename DocumentType::ValueType& GetValueByPointerWithDefault(DocumentType& document, const CharType(&source)[N], const typename DocumentType::ValueType& defaultValue) {
    return GenericPointer<typename DocumentType::ValueType>(source, N - 1).GetWithDefault(document, defaultValue);
}

template <typename DocumentType, typename CharType, size_t N>
typename DocumentType::ValueType& GetValueByPointerWithDefault(DocumentType& document, const CharType(&source)[N], const typename DocumentType::Ch* defaultValue) {
    return GenericPointer<typename DocumentType::ValueType>(source, N - 1).GetWithDefault(document, defaultValue);
}

#if RAPIDJSON_HAS_STDSTRING
template <typename DocumentType, typename CharType, size_t N>
typename DocumentType::ValueType& GetValueByPointerWithDefault(DocumentType& document, const CharType(&source)[N], const std::basic_string<typename DocumentType::Ch>& defaultValue) {
    return GenericPointer<typename DocumentType::ValueType>(source, N - 1).GetWithDefault(document, defaultValue);
}
#endif

template <typename DocumentType, typename CharType, size_t N, typename T2>
RAPIDJSON_DISABLEIF_RETURN((internal::OrExpr<internal::IsPointer<T2>, internal::IsGenericValue<T2> >), (typename DocumentType::ValueType&))
GetValueByPointerWithDefault(DocumentType& document, const CharType(&source)[N], T2 defaultValue) {
    return GenericPointer<typename DocumentType::ValueType>(source, N - 1).GetWithDefault(document, defaultValue);
}

//////////////////////////////////////////////////////////////////////////////

template <typename T>
typename T::ValueType& SetValueByPointer(T& root, const GenericPointer<typename T::ValueType>& pointer, typename T::ValueType& value, typename T::AllocatorType& a) {
    return pointer.Set(root, value, a);
}

template <typename T>
typename T::ValueType& SetValueByPointer(T& root, const GenericPointer<typename T::ValueType>& pointer, const typename T::ValueType& value, typename T::AllocatorType& a) {
    return pointer.Set(root, value, a);
}

template <typename T>
typename T::ValueType& SetValueByPointer(T& root, const GenericPointer<typename T::ValueType>& pointer, const typename T::Ch* value, typename T::AllocatorType& a) {
    return pointer.Set(root, value, a);
}

#if RAPIDJSON_HAS_STDSTRING
template <typename T>
typename T::ValueType& SetValueByPointer(T& root, const GenericPointer<typename T::ValueType>& pointer, const std::basic_string<typename T::Ch>& value, typename T::AllocatorType& a) {
    return pointer.Set(root, value, a);
}
#endif

template <typename T, typename T2>
RAPIDJSON_DISABLEIF_RETURN((internal::OrExpr<internal::IsPointer<T2>, internal::IsGenericValue<T2> >), (typename T::ValueType&))
SetValueByPointer(T& root, const GenericPointer<typename T::ValueType>& pointer, T2 value, typename T::AllocatorType& a) {
    return pointer.Set(root, value, a);
}

template <typename T, typename CharType, size_t N>
typename T::ValueType& SetValueByPointer(T& root, const CharType(&source)[N], typename T::ValueType& value, typename T::AllocatorType& a) {
    return GenericPointer<typename T::ValueType>(source, N - 1).Set(root, value, a);
}

template <typename T, typename CharType, size_t N>
typename T::ValueType& SetValueByPointer(T& root, const CharType(&source)[N], const typename T::ValueType& value, typename T::AllocatorType& a) {
    return GenericPointer<typename T::ValueType>(source, N - 1).Set(root, value, a);
}

template <typename T, typename CharType, size_t N>
typename T::ValueType& SetValueByPointer(T& root, const CharType(&source)[N], const typename T::Ch* value, typename T::AllocatorType& a) {
    return GenericPointer<typename T::ValueType>(source, N - 1).Set(root, value, a);
}

#if RAPIDJSON_HAS_STDSTRING
template <typename T, typename CharType, size_t N>
typename T::ValueType& SetValueByPointer(T& root, const CharType(&source)[N], const std::basic_string<typename T::Ch>& value, typename T::AllocatorType& a) {
    return GenericPointer<typename T::ValueType>(source, N - 1).Set(root, value, a);
}
#endif

template <typename T, typename CharType, size_t N, typename T2>
RAPIDJSON_DISABLEIF_RETURN((internal::OrExpr<internal::IsPointer<T2>, internal::IsGenericValue<T2> >), (typename T::ValueType&))
SetValueByPointer(T& root, const CharType(&source)[N], T2 value, typename T::AllocatorType& a) {
    return GenericPointer<typename T::ValueType>(source, N - 1).Set(root, value, a);
}

// Нет параметра распределителя

template <typename DocumentType>
typename DocumentType::ValueType& SetValueByPointer(DocumentType& document, const GenericPointer<typename DocumentType::ValueType>& pointer, typename DocumentType::ValueType& value) {
    return pointer.Set(document, value);
}

template <typename DocumentType>
typename DocumentType::ValueType& SetValueByPointer(DocumentType& document, const GenericPointer<typename DocumentType::ValueType>& pointer, const typename DocumentType::ValueType& value) {
    return pointer.Set(document, value);
}

template <typename DocumentType>
typename DocumentType::ValueType& SetValueByPointer(DocumentType& document, const GenericPointer<typename DocumentType::ValueType>& pointer, const typename DocumentType::Ch* value) {
    return pointer.Set(document, value);
}

#if RAPIDJSON_HAS_STDSTRING
template <typename DocumentType>
typename DocumentType::ValueType& SetValueByPointer(DocumentType& document, const GenericPointer<typename DocumentType::ValueType>& pointer, const std::basic_string<typename DocumentType::Ch>& value) {
    return pointer.Set(document, value);
}
#endif

template <typename DocumentType, typename T2>
RAPIDJSON_DISABLEIF_RETURN((internal::OrExpr<internal::IsPointer<T2>, internal::IsGenericValue<T2> >), (typename DocumentType::ValueType&))
SetValueByPointer(DocumentType& document, const GenericPointer<typename DocumentType::ValueType>& pointer, T2 value) {
    return pointer.Set(document, value);
}

template <typename DocumentType, typename CharType, size_t N>
typename DocumentType::ValueType& SetValueByPointer(DocumentType& document, const CharType(&source)[N], typename DocumentType::ValueType& value) {
    return GenericPointer<typename DocumentType::ValueType>(source, N - 1).Set(document, value);
}

template <typename DocumentType, typename CharType, size_t N>
typename DocumentType::ValueType& SetValueByPointer(DocumentType& document, const CharType(&source)[N], const typename DocumentType::ValueType& value) {
    return GenericPointer<typename DocumentType::ValueType>(source, N - 1).Set(document, value);
}

template <typename DocumentType, typename CharType, size_t N>
typename DocumentType::ValueType& SetValueByPointer(DocumentType& document, const CharType(&source)[N], const typename DocumentType::Ch* value) {
    return GenericPointer<typename DocumentType::ValueType>(source, N - 1).Set(document, value);
}

#if RAPIDJSON_HAS_STDSTRING
template <typename DocumentType, typename CharType, size_t N>
typename DocumentType::ValueType& SetValueByPointer(DocumentType& document, const CharType(&source)[N], const std::basic_string<typename DocumentType::Ch>& value) {
    return GenericPointer<typename DocumentType::ValueType>(source, N - 1).Set(document, value);
}
#endif

template <typename DocumentType, typename CharType, size_t N, typename T2>
RAPIDJSON_DISABLEIF_RETURN((internal::OrExpr<internal::IsPointer<T2>, internal::IsGenericValue<T2> >), (typename DocumentType::ValueType&))
SetValueByPointer(DocumentType& document, const CharType(&source)[N], T2 value) {
    return GenericPointer<typename DocumentType::ValueType>(source, N - 1).Set(document, value);
}

//////////////////////////////////////////////////////////////////////////////

template <typename T>
typename T::ValueType& SwapValueByPointer(T& root, const GenericPointer<typename T::ValueType>& pointer, typename T::ValueType& value, typename T::AllocatorType& a) {
    return pointer.Swap(root, value, a);
}

template <typename T, typename CharType, size_t N>
typename T::ValueType& SwapValueByPointer(T& root, const CharType(&source)[N], typename T::ValueType& value, typename T::AllocatorType& a) {
    return GenericPointer<typename T::ValueType>(source, N - 1).Swap(root, value, a);
}

template <typename DocumentType>
typename DocumentType::ValueType& SwapValueByPointer(DocumentType& document, const GenericPointer<typename DocumentType::ValueType>& pointer, typename DocumentType::ValueType& value) {
    return pointer.Swap(document, value);
}

template <typename DocumentType, typename CharType, size_t N>
typename DocumentType::ValueType& SwapValueByPointer(DocumentType& document, const CharType(&source)[N], typename DocumentType::ValueType& value) {
    return GenericPointer<typename DocumentType::ValueType>(source, N - 1).Swap(document, value);
}

//////////////////////////////////////////////////////////////////////////////

template <typename T>
bool EraseValueByPointer(T& root, const GenericPointer<typename T::ValueType>& pointer) {
    return pointer.Erase(root);
}

template <typename T, typename CharType, size_t N>
bool EraseValueByPointer(T& root, const CharType(&source)[N]) {
    return GenericPointer<typename T::ValueType>(source, N - 1).Erase(root);
}

//@}

RAPIDJSON_NAMESPACE_END

#if defined(__clang__) || defined(_MSC_VER)
RAPIDJSON_DIAG_POP
#endif

#endif // RAPIDJSON_POINTER_H_
