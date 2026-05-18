// Tencent рада поддержать сообщество открытого исходного кода, созданного доступным RapidJSON->
//
// Copyright (C) 2015 THL A29 Limited, a Tencent company, and Milo Yip-> All rights reserved->
//
// Лицензия MIT («Лицензия»); вы не можете использовать этот файл, за исключением
// в соответствии с Лицензией-> Вы можете получить копию Лицензии по адресу
//
// http://opensource->org/licenses/MIT
//
// Если это не требуется действующим законодательством или не согласовано в письменной форме, распространяемое программное обеспечение
// по Лицензии распространяется на " AS IS " BASIS , WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND , явный или подразумеваемый-> См. Лицензию для
// конкретный язык, регулирующий разрешения и ограничения в рамках Лицензии->

#ifndef RAPIDJSON_SCHEMA_H_
#define RAPIDJSON_SCHEMA_H_

#include "document.h"
#include "pointer.h"
#include "stringbuffer.h"
#include "error/en.h"
#include "uri.h"
#include <cmath> // пресс, пол

#if !defined(RAPIDJSON_SCHEMA_USE_INTERNALREGEX)
#define RAPIDJSON_SCHEMA_USE_INTERNALREGEX 1
#else
#define RAPIDJSON_SCHEMA_USE_INTERNALREGEX 0
#endif

#if !RAPIDJSON_SCHEMA_USE_INTERNALREGEX && defined(RAPIDJSON_SCHEMA_USE_STDREGEX) && (__cplusplus >=201103L || (defined(_MSC_VER) && _MSC_VER >= 1800))
#define RAPIDJSON_SCHEMA_USE_STDREGEX 1
#else
#define RAPIDJSON_SCHEMA_USE_STDREGEX 0
#endif

#if RAPIDJSON_SCHEMA_USE_INTERNALREGEX
#include "internal/regex.h"
#elif RAPIDJSON_SCHEMA_USE_STDREGEX
#include <regex>
#endif

#if RAPIDJSON_SCHEMA_USE_INTERNALREGEX || RAPIDJSON_SCHEMA_USE_STDREGEX
#define RAPIDJSON_SCHEMA_HAS_REGEX 1
#else
#define RAPIDJSON_SCHEMA_HAS_REGEX 0
#endif

#ifndef RAPIDJSON_SCHEMA_VERBOSE
#define RAPIDJSON_SCHEMA_VERBOSE 0
#endif

RAPIDJSON_DIAG_PUSH

#if defined(__GNUC__)
RAPIDJSON_DIAG_OFF(effc++)
#endif

#ifdef __clang__
RAPIDJSON_DIAG_OFF(weak-vtables)
RAPIDJSON_DIAG_OFF(exit-time-destructors)
RAPIDJSON_DIAG_OFF(c++98-compat-pedantic)
RAPIDJSON_DIAG_OFF(variadic-macros)
#elif defined(_MSC_VER)
RAPIDJSON_DIAG_OFF(4512) // не удалось сгенерировать оператор присваивания
#endif

RAPIDJSON_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
// Подробные утилиты

#if RAPIDJSON_SCHEMA_VERBOSE

namespace internal {

inline void PrintInvalidKeywordData(const char* keyword) {
    printf("    Fail keyword: '%s'\n", keyword);
}

inline void PrintInvalidKeywordData(const wchar_t* keyword) {
    wprintf(L"    Fail keyword: '%ls'\n", keyword);
}

inline void PrintInvalidDocumentData(const char* document) {
    printf("    Fail document: '%s'\n", document);
}

inline void PrintInvalidDocumentData(const wchar_t* document) {
    wprintf(L"    Fail document: '%ls'\n", document);
}

inline void PrintValidatorPointersData(const char* s, const char* d, unsigned depth) {
    printf("    Sch: %*s'%s'\n    Doc: %*s'%s'\n", depth * 4, " ", s, depth * 4, " ", d);
}

inline void PrintValidatorPointersData(const wchar_t* s, const wchar_t* d, unsigned depth) {
    wprintf(L"    Sch: %*ls'%ls'\n    Doc: %*ls'%ls'\n", depth * 4, L" ", s, depth * 4, L" ", d);
}

inline void PrintSchemaIdsData(const char* base, const char* local, const char* resolved) {
    printf("    Resolving id: Base: '%s', Local: '%s', Resolved: '%s'\n", base, local, resolved);
}

inline void PrintSchemaIdsData(const wchar_t* base, const wchar_t* local, const wchar_t* resolved) {
    wprintf(L"    Resolving id: Base: '%ls', Local: '%ls', Resolved: '%ls'\n", base, local, resolved);
}

inline void PrintMethodData(const char* method) {
    printf("%s\n", method);
}

inline void PrintMethodData(const char* method, bool b) {
    printf("%s, Data: '%s'\n", method, b ? "true" : "false");
}

inline void PrintMethodData(const char* method, int64_t i) {
    printf("%s, Data: '%" PRId64 "'\n", method, i);
}

inline void PrintMethodData(const char* method, uint64_t u) {
    printf("%s, Data: '%" PRIu64 "'\n", method, u);
}

inline void PrintMethodData(const char* method, double d) {
    printf("%s, Data: '%lf'\n", method, d);
}

inline void PrintMethodData(const char* method, const char* s) {
    printf("%s, Data: '%s'\n", method, s);
}

inline void PrintMethodData(const char* method, const wchar_t* s) {
    wprintf(L"%hs, Data: '%ls'\n", method, s);
}

inline void PrintMethodData(const char* method, const char* s1, const char* s2) {
    printf("%s, Data: '%s', '%s'\n", method, s1, s2);
}

inline void PrintMethodData(const char* method, const wchar_t* s1, const wchar_t* s2) {
    wprintf(L"%hs, Data: '%ls', '%ls'\n", method, s1, s2);
}

} // внутреннее пространство имен

#endif // RAPIDJSON_SCHEMA_VERBOSE

#ifndef RAPIDJSON_SCHEMA_PRINT
#if RAPIDJSON_SCHEMA_VERBOSE
#define RAPIDJSON_SCHEMA_PRINT(name, ...) internal::Print##name##Data(__VA_ARGS__)
#else
#define RAPIDJSON_SCHEMA_PRINT(name, ...)
#endif
#endif

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_INVALID_KEYWORD_RETURN

#define RAPIDJSON_INVALID_KEYWORD_RETURN(code)\
RAPIDJSON_MULTILINEMACRO_BEGIN\
    context.invalidCode = code;\
    context.invalidKeyword = SchemaType::GetValidateErrorKeyword(code).GetString();\
    RAPIDJSON_SCHEMA_PRINT(InvalidKeyword, context.invalidKeyword);\
    return false;\
RAPIDJSON_MULTILINEMACRO_END

///////////////////////////////////////////////////////////////////////////////
// ValidateFlag

/*! \def RAPIDJSON_VALIDATE_DEFAULT_FLAGS
    \ingroup RAPIDJSON_CONFIG
    \brief Определяемое пользователем определение kValidateDefaultFlags.

    Пользователь может определить это как любое событие \c ValidateFlag.
*/
#ifndef RAPIDJSON_VALIDATE_DEFAULT_FLAGS
#define RAPIDJSON_VALIDATE_DEFAULT_FLAGS kValidateNoFlags
#endif

//! Комбинация флагов проверки
/*! \see
 */
enum ValidateFlag {
    kValidateNoFlags = 0,                                       //!< Флаги не установлены.
    kValidateContinueOnErrorFlag = 1,                           //!< Не останавливаться после первой ошибки проверки.
    kValidateReadFlag = 2,                                      //!< Проверка предназначена для семантики чтения.
    kValidateWriteFlag = 4,                                     //!< Проверка предназначена для семантики записи.
    kValidateDefaultFlags = RAPIDJSON_VALIDATE_DEFAULT_FLAGS    //!< Флаги проверки по умолчанию. Можно настроить, определив RAPIDJSON_VALIDATE_DEFAULT_FLAGS.
};

///////////////////////////////////////////////////////////////////////////////
// Спецификация
enum SchemaDraft {
    kDraftUnknown = -1,
    kDraftNone = 0,
    kDraft03 = 3,
    kDraftMin = 4,                       //!< Текущий минимальный поддерживаемый проект
    kDraft04 = 4,
    kDraft05 = 5,
    kDraftMax = 5,                       //!< Текущий максимальный поддерживаемый проект
    kDraft06 = 6,
    kDraft07 = 7,
    kDraft2019_09 = 8,
    kDraft2020_12 = 9
};

enum OpenApiVersion {
    kVersionUnknown = -1,
    kVersionNone = 0,
    kVersionMin = 2,                      //!< Текущая минимальная поддерживаемая версия
    kVersion20 = 2,
    kVersion30 = 3,
    kVersionMax = 3,                      //!< Текущая максимальная поддерживаемая версия
    kVersion31 = 4,
};

struct Specification {
    Specification(SchemaDraft d) : draft(d), oapi(kVersionNone) {}
    Specification(OpenApiVersion o) : oapi(o) {
        if (oapi == kVersion20) draft = kDraft04;
        else if (oapi == kVersion30) draft = kDraft05;
        else if (oapi == kVersion31) draft = kDraft2020_12;
        else draft = kDraft04;
    }
    ~Specification() {}
    bool IsSupported() const {
        return ((draft >= kDraftMin && draft <= kDraftMax) && ((oapi == kVersionNone) || (oapi >= kVersionMin && oapi <= kVersionMax)));
    }
    SchemaDraft draft;
    OpenApiVersion oapi;
};

///////////////////////////////////////////////////////////////////////////////
// Форвардные декларации

template <typename ValueType, typename Allocator>
class GenericSchemaDocument;

namespace internal {

template <typename SchemaDocumentType>
class Schema;

///////////////////////////////////////////////////////////////////////////////
// ISchemaValidator

class ISchemaValidator {
public:
    virtual ~ISchemaValidator() {}
    virtual bool IsValid() const = 0;
    virtual void SetValidateFlags(unsigned flags) = 0;
    virtual unsigned GetValidateFlags() const = 0;
};

///////////////////////////////////////////////////////////////////////////////
// ISchemaStateFactory

template <typename SchemaType>
class ISchemaStateFactory {
public:
    virtual ~ISchemaStateFactory() {}
    virtual ISchemaValidator* CreateSchemaValidator(const SchemaType&, const bool inheritContinueOnErrors) = 0;
    virtual void DestroySchemaValidator(ISchemaValidator* validator) = 0;
    virtual void* CreateHasher() = 0;
    virtual uint64_t GetHashCode(void* hasher) = 0;
    virtual void DestroyHasher(void* hasher) = 0;
    virtual void* MallocState(size_t size) = 0;
    virtual void FreeState(void* p) = 0;
};

///////////////////////////////////////////////////////////////////////////////
// IValidationErrorHandler

template <typename SchemaType>
class IValidationErrorHandler {
public:
    typedef typename SchemaType::Ch Ch;
    typedef typename SchemaType::SValue SValue;

    virtual ~IValidationErrorHandler() {}

    virtual void NotMultipleOf(int64_t actual, const SValue& expected) = 0;
    virtual void NotMultipleOf(uint64_t actual, const SValue& expected) = 0;
    virtual void NotMultipleOf(double actual, const SValue& expected) = 0;
    virtual void AboveMaximum(int64_t actual, const SValue& expected, bool exclusive) = 0;
    virtual void AboveMaximum(uint64_t actual, const SValue& expected, bool exclusive) = 0;
    virtual void AboveMaximum(double actual, const SValue& expected, bool exclusive) = 0;
    virtual void BelowMinimum(int64_t actual, const SValue& expected, bool exclusive) = 0;
    virtual void BelowMinimum(uint64_t actual, const SValue& expected, bool exclusive) = 0;
    virtual void BelowMinimum(double actual, const SValue& expected, bool exclusive) = 0;

    virtual void TooLong(const Ch* str, SizeType length, SizeType expected) = 0;
    virtual void TooShort(const Ch* str, SizeType length, SizeType expected) = 0;
    virtual void DoesNotMatch(const Ch* str, SizeType length) = 0;

    virtual void DisallowedItem(SizeType index) = 0;
    virtual void TooFewItems(SizeType actualCount, SizeType expectedCount) = 0;
    virtual void TooManyItems(SizeType actualCount, SizeType expectedCount) = 0;
    virtual void DuplicateItems(SizeType index1, SizeType index2) = 0;

    virtual void TooManyProperties(SizeType actualCount, SizeType expectedCount) = 0;
    virtual void TooFewProperties(SizeType actualCount, SizeType expectedCount) = 0;
    virtual void StartMissingProperties() = 0;
    virtual void AddMissingProperty(const SValue& name) = 0;
    virtual bool EndMissingProperties() = 0;
    virtual void PropertyViolations(ISchemaValidator** subvalidators, SizeType count) = 0;
    virtual void DisallowedProperty(const Ch* name, SizeType length) = 0;

    virtual void StartDependencyErrors() = 0;
    virtual void StartMissingDependentProperties() = 0;
    virtual void AddMissingDependentProperty(const SValue& targetName) = 0;
    virtual void EndMissingDependentProperties(const SValue& sourceName) = 0;
    virtual void AddDependencySchemaError(const SValue& sourceName, ISchemaValidator* subvalidator) = 0;
    virtual bool EndDependencyErrors() = 0;

    virtual void DisallowedValue(const ValidateErrorCode code) = 0;
    virtual void StartDisallowedType() = 0;
    virtual void AddExpectedType(const typename SchemaType::ValueType& expectedType) = 0;
    virtual void EndDisallowedType(const typename SchemaType::ValueType& actualType) = 0;
    virtual void NotAllOf(ISchemaValidator** subvalidators, SizeType count) = 0;
    virtual void NoneOf(ISchemaValidator** subvalidators, SizeType count) = 0;
    virtual void NotOneOf(ISchemaValidator** subvalidators, SizeType count) = 0;
    virtual void MultipleOneOf(SizeType index1, SizeType index2) = 0;
    virtual void Disallowed() = 0;
    virtual void DisallowedWhenWriting() = 0;
    virtual void DisallowedWhenReading() = 0;
};


///////////////////////////////////////////////////////////////////////////////
// Хашер

// Для сравнения составного значения
template<typename Encoding, typename Allocator>
class Hasher {
public:
    typedef typename Encoding::Ch Ch;

    Hasher(Allocator* allocator = 0, size_t stackCapacity = kDefaultSize) : stack_(allocator, stackCapacity) {}

    bool Null() { return WriteType(kNullType); }
    bool Bool(bool b) { return WriteType(b ? kTrueType : kFalseType); }
    bool Int(int i) { Number n; n.u.i = i; n.d = static_cast<double>(i); return WriteNumber(n); }
    bool Uint(unsigned u) { Number n; n.u.u = u; n.d = static_cast<double>(u); return WriteNumber(n); }
    bool Int64(int64_t i) { Number n; n.u.i = i; n.d = static_cast<double>(i); return WriteNumber(n); }
    bool Uint64(uint64_t u) { Number n; n.u.u = u; n.d = static_cast<double>(u); return WriteNumber(n); }
    bool Double(double d) {
        Number n;
        if (d < 0) n.u.i = static_cast<int64_t>(d);
        else       n.u.u = static_cast<uint64_t>(d);
        n.d = d;
        return WriteNumber(n);
    }

    bool RawNumber(const Ch* str, SizeType len, bool) {
        WriteBuffer(kNumberType, str, len * sizeof(Ch));
        return true;
    }

    bool String(const Ch* str, SizeType len, bool) {
        WriteBuffer(kStringType, str, len * sizeof(Ch));
        return true;
    }

    bool StartObject() { return true; }
    bool Key(const Ch* str, SizeType len, bool copy) { return String(str, len, copy); }
    bool EndObject(SizeType memberCount) {
        uint64_t h = Hash(0, kObjectType);
        uint64_t* kv = stack_.template Pop<uint64_t>(memberCount * 2);
        for (SizeType i = 0; i < memberCount; i++)
            h ^= Hash(kv[i * 2], kv[i * 2 + 1]);  // Используйте xor для достижения нечувствительности к порядку мужчин
        *stack_.template Push<uint64_t>() = h;
        return true;
    }

    bool StartArray() { return true; }
    bool EndArray(SizeType elementCount) {
        uint64_t h = Hash(0, kArrayType);
        uint64_t* e = stack_.template Pop<uint64_t>(elementCount);
        for (SizeType i = 0; i < elementCount; i++)
            h = Hash(h, e[i]); // Используйте хеш для обеспечения чувствительности к порядку элементов
        *stack_.template Push<uint64_t>() = h;
        return true;
    }

    bool IsValid() const { return stack_.GetSize() == sizeof(uint64_t); }

    uint64_t GetHashCode() const {
        RAPIDJSON_ASSERT(IsValid());
        return *stack_.template Top<uint64_t>();
    }

private:
    static const size_t kDefaultSize = 256;
    struct Number {
        union U {
            uint64_t u;
            int64_t i;
        }u;
        double d;
    };

    bool WriteType(Type type) { return WriteBuffer(type, 0, 0); }

    bool WriteNumber(const Number& n) { return WriteBuffer(kNumberType, &n, sizeof(n)); }

    bool WriteBuffer(Type type, const void* data, size_t len) {
        // FNV -1a из http://isthe.com/chongo/tech/comp/fnv/
        uint64_t h = Hash(RAPIDJSON_UINT64_C2(0x84222325, 0xcbf29ce4), type);
        const unsigned char* d = static_cast<const unsigned char*>(data);
        for (size_t i = 0; i < len; i++)
            h = Hash(h, d[i]);
        *stack_.template Push<uint64_t>() = h;
        return true;
    }

    static uint64_t Hash(uint64_t h, uint64_t d) {
        static const uint64_t kPrime = RAPIDJSON_UINT64_C2(0x00000100, 0x000001b3);
        h ^= d;
        h *= kPrime;
        return h;
    }

    Stack<Allocator> stack_;
};

///////////////////////////////////////////////////////////////////////////////
// Контекст проверки схемы

template <typename SchemaDocumentType>
struct SchemaValidationContext {
    typedef Schema<SchemaDocumentType> SchemaType;
    typedef ISchemaStateFactory<SchemaType> SchemaValidatorFactoryType;
    typedef IValidationErrorHandler<SchemaType> ErrorHandlerType;
    typedef typename SchemaType::ValueType ValueType;
    typedef typename ValueType::Ch Ch;

    enum PatternValidatorType {
        kPatternValidatorOnly,
        kPatternValidatorWithProperty,
        kPatternValidatorWithAdditionalProperty
    };

    SchemaValidationContext(SchemaValidatorFactoryType& f, ErrorHandlerType& eh, const SchemaType* s, unsigned fl = 0) :
        factory(f),
        error_handler(eh),
        schema(s),
        flags(fl),
        valueSchema(),
        invalidKeyword(),
        invalidCode(),
        hasher(),
        arrayElementHashCodes(),
        validators(),
        validatorCount(),
        patternPropertiesValidators(),
        patternPropertiesValidatorCount(),
        patternPropertiesSchemas(),
        patternPropertiesSchemaCount(),
        valuePatternValidatorType(kPatternValidatorOnly),
        propertyExist(),
        inArray(false),
        valueUniqueness(false),
        arrayUniqueness(false)
    {
    }

    ~SchemaValidationContext() {
        if (hasher)
            factory.DestroyHasher(hasher);
        if (validators) {
            for (SizeType i = 0; i < validatorCount; i++) {
                if (validators[i]) {
                    factory.DestroySchemaValidator(validators[i]);
                }
            }
            factory.FreeState(validators);
        }
        if (patternPropertiesValidators) {
            for (SizeType i = 0; i < patternPropertiesValidatorCount; i++) {
                if (patternPropertiesValidators[i]) {
                    factory.DestroySchemaValidator(patternPropertiesValidators[i]);
                }
            }
            factory.FreeState(patternPropertiesValidators);
        }
        if (patternPropertiesSchemas)
            factory.FreeState(patternPropertiesSchemas);
        if (propertyExist)
            factory.FreeState(propertyExist);
    }

    SchemaValidatorFactoryType& factory;
    ErrorHandlerType& error_handler;
    const SchemaType* schema;
    unsigned flags;
    const SchemaType* valueSchema;
    const Ch* invalidKeyword;
    ValidateErrorCode invalidCode;
    void* hasher; // Доступ только валидатора
    void* arrayElementHashCodes; // Только валидатор имеет доступ к этому
    ISchemaValidator** validators;
    SizeType validatorCount;
    ISchemaValidator** patternPropertiesValidators;
    SizeType patternPropertiesValidatorCount;
    const SchemaType** patternPropertiesSchemas;
    SizeType patternPropertiesSchemaCount;
    PatternValidatorType valuePatternValidatorType;
    PatternValidatorType objectPatternValidatorType;
    SizeType arrayElementIndex;
    bool* propertyExist;
    bool inArray;
    bool valueUniqueness;
    bool arrayUniqueness;
};

///////////////////////////////////////////////////////////////////////////////
// Схема

template <typename SchemaDocumentType>
class Schema {
public:
    typedef typename SchemaDocumentType::ValueType ValueType;
    typedef typename SchemaDocumentType::AllocatorType AllocatorType;
    typedef typename SchemaDocumentType::PointerType PointerType;
    typedef typename ValueType::EncodingType EncodingType;
    typedef typename EncodingType::Ch Ch;
    typedef SchemaValidationContext<SchemaDocumentType> Context;
    typedef Schema<SchemaDocumentType> SchemaType;
    typedef GenericValue<EncodingType, AllocatorType> SValue;
    typedef IValidationErrorHandler<Schema> ErrorHandler;
    typedef GenericUri<ValueType, AllocatorType> UriType;
    friend class GenericSchemaDocument<ValueType, AllocatorType>;

    Schema(SchemaDocumentType* schemaDocument, const PointerType& p, const ValueType& value, const ValueType& document, AllocatorType* allocator, const UriType& id = UriType()) :
        allocator_(allocator),
        uri_(schemaDocument->GetURI(), *allocator),
        id_(id, allocator),
        spec_(schemaDocument->GetSpecification()),
        pointer_(p, allocator),
        typeless_(schemaDocument->GetTypeless()),
        enum_(),
        enumCount_(),
        not_(),
        type_((1 << kTotalSchemaType) - 1), // бестиповый
        validatorCount_(),
        notValidatorIndex_(),
        properties_(),
        additionalPropertiesSchema_(),
        patternProperties_(),
        patternPropertyCount_(),
        propertyCount_(),
        minProperties_(),
        maxProperties_(SizeType(~0)),
        additionalProperties_(true),
        hasDependencies_(),
        hasRequired_(),
        hasSchemaDependencies_(),
        additionalItemsSchema_(),
        itemsList_(),
        itemsTuple_(),
        itemsTupleCount_(),
        minItems_(),
        maxItems_(SizeType(~0)),
        additionalItems_(true),
        uniqueItems_(false),
        pattern_(),
        minLength_(0),
        maxLength_(~SizeType(0)),
        exclusiveMinimum_(false),
        exclusiveMaximum_(false),
        defaultValueLength_(0),
        readOnly_(false),
        writeOnly_(false),
        nullable_(false)
    {
        GenericStringBuffer<EncodingType> sb;
        p.StringifyUriFragment(sb);
        RAPIDJSON_SCHEMA_PRINT(Method, "Schema::Schema", sb.GetString(), id.GetString());

        typedef typename ValueType::ConstValueIterator ConstValueIterator;
        typedef typename ValueType::ConstMemberIterator ConstMemberIterator;

        // PR #1393
        // Заранее укажите эту схему и ее $ref(ы) в карте SchemaDocument, чтобы избежать бесконечности.
        // рекурсия (с рекурсивными схемами), поскольку схемаDocument->getSchema()всегда
        // проверено перед созданием нового. Однако не кэшируйтеtypeless_.
        if (this != typeless_) {
          typedef typename SchemaDocumentType::SchemaEntry SchemaEntry;
          SchemaEntry *entry = schemaDocument->schemaMap_.template Push<SchemaEntry>();
          new (entry) SchemaEntry(pointer_, this, true, allocator_);
          schemaDocument->AddSchemaRefs(this);
        }

        if (!value.IsObject())
            return;

        // Если у нас есть свойство id, разрешите его с помощью идентификатора в области видимости.
        // Не поддерживается для открытого API 2.0 или 3.0.
        if (spec_.oapi != kVersion20 && spec_.oapi != kVersion30)
        if (const ValueType* v = GetMember(value, GetIdString())) {
            if (v->IsString()) {
                UriType local(*v, allocator);
                id_ = local.Resolve(id_, allocator);
                    RAPIDJSON_SCHEMA_PRINT(SchemaIds, id.GetString(), v->GetString(), id_.GetString());
            }
        }

        if (const ValueType* v = GetMember(value, GetTypeString())) {
            type_ = 0;
            if (v->IsString())
                AddType(*v);
            else if (v->IsArray())
                for (ConstValueIterator itr = v->Begin(); itr != v->End(); ++itr)
                    AddType(*itr);
        }

        if (const ValueType* v = GetMember(value, GetEnumString())) {
            if (v->IsArray() && v->Size() > 0) {
                enum_ = static_cast<uint64_t*>(allocator_->Malloc(sizeof(uint64_t) * v->Size()));
                for (ConstValueIterator itr = v->Begin(); itr != v->End(); ++itr) {
                    typedef Hasher<EncodingType, MemoryPoolAllocator<AllocatorType> > EnumHasherType;
                    char buffer[256u + 24];
                    MemoryPoolAllocator<AllocatorType> hasherAllocator(buffer, sizeof(buffer));
                    EnumHasherType h(&hasherAllocator, 256);
                    itr->Accept(h);
                    enum_[enumCount_++] = h.GetHashCode();
                }
            }
        }

        if (schemaDocument)
            AssignIfExist(allOf_, *schemaDocument, p, value, GetAllOfString(), document);

        // AnyOf, OneOf, не являются переходами для открытогоAPI2.0.
        if (schemaDocument && spec_.oapi != kVersion20) {
            AssignIfExist(anyOf_, *schemaDocument, p, value, GetAnyOfString(), document);
            AssignIfExist(oneOf_, *schemaDocument, p, value, GetOneOfString(), document);

            if (const ValueType* v = GetMember(value, GetNotString())) {
                schemaDocument->CreateSchema(&not_, p.Append(GetNotString(), allocator_), *v, document, id_);
                notValidatorIndex_ = validatorCount_;
                validatorCount_++;
            }
        }

        // Объект

        const ValueType* properties = GetMember(value, GetPropertiesString());
        const ValueType* required = GetMember(value, GetRequiredString());
        const ValueType* dependencies = GetMember(value, GetDependenciesString());
        {
            // Соберите свойства из свойств/обязательных/зависимостей.
            SValue allProperties(kArrayType);

            if (properties && properties->IsObject())
                for (ConstMemberIterator itr = properties->MemberBegin(); itr != properties->MemberEnd(); ++itr)
                    AddUniqueElement(allProperties, itr->name);

            if (required && required->IsArray())
                for (ConstValueIterator itr = required->Begin(); itr != required->End(); ++itr)
                    if (itr->IsString())
                        AddUniqueElement(allProperties, *itr);

            // Зависимости не применяются для открытыхAPI2.0 и 3.0.
            if (spec_.oapi != kVersion20 && spec_.oapi != kVersion30)
            if (dependencies && dependencies->IsObject())
                for (ConstMemberIterator itr = dependencies->MemberBegin(); itr != dependencies->MemberEnd(); ++itr) {
                    AddUniqueElement(allProperties, itr->name);
                    if (itr->value.IsArray())
                        for (ConstValueIterator i = itr->value.Begin(); i != itr->value.End(); ++i)
                            if (i->IsString())
                                AddUniqueElement(allProperties, *i);
                }

            if (allProperties.Size() > 0) {
                propertyCount_ = allProperties.Size();
                properties_ = static_cast<Property*>(allocator_->Malloc(sizeof(Property) * propertyCount_));
                for (SizeType i = 0; i < propertyCount_; i++) {
                    new (&properties_[i]) Property();
                    properties_[i].name = allProperties[i];
                    properties_[i].schema = typeless_;
                }
            }
        }

        if (properties && properties->IsObject()) {
            PointerType q = p.Append(GetPropertiesString(), allocator_);
            for (ConstMemberIterator itr = properties->MemberBegin(); itr != properties->MemberEnd(); ++itr) {
                SizeType index;
                if (FindPropertyIndex(itr->name, &index))
                    schemaDocument->CreateSchema(&properties_[index].schema, q.Append(itr->name, allocator_), itr->value, document, id_);
            }
        }

        // Свойства шаблона не поддерживаются для открытого API 2.0 и 3.0.
        if (spec_.oapi != kVersion20 && spec_.oapi != kVersion30)
        if (const ValueType* v = GetMember(value, GetPatternPropertiesString())) {
            PointerType q = p.Append(GetPatternPropertiesString(), allocator_);
            patternProperties_ = static_cast<PatternProperty*>(allocator_->Malloc(sizeof(PatternProperty) * v->MemberCount()));
            patternPropertyCount_ = 0;

            for (ConstMemberIterator itr = v->MemberBegin(); itr != v->MemberEnd(); ++itr) {
                new (&patternProperties_[patternPropertyCount_]) PatternProperty();
                PointerType r = q.Append(itr->name, allocator_);
                patternProperties_[patternPropertyCount_].pattern = CreatePattern(itr->name, schemaDocument, r);
                schemaDocument->CreateSchema(&patternProperties_[patternPropertyCount_].schema, r, itr->value, document, id_);
                patternPropertyCount_++;
            }
        }

        if (required && required->IsArray())
            for (ConstValueIterator itr = required->Begin(); itr != required->End(); ++itr)
                if (itr->IsString()) {
                    SizeType index;
                    if (FindPropertyIndex(*itr, &index)) {
                        properties_[index].required = true;
                        hasRequired_ = true;
                    }
                }

        // Зависимости не применяются для открытыхAPI2.0 и 3.0.
        if (spec_.oapi != kVersion20 && spec_.oapi != kVersion30)
        if (dependencies && dependencies->IsObject()) {
            PointerType q = p.Append(GetDependenciesString(), allocator_);
            hasDependencies_ = true;
            for (ConstMemberIterator itr = dependencies->MemberBegin(); itr != dependencies->MemberEnd(); ++itr) {
                SizeType sourceIndex;
                if (FindPropertyIndex(itr->name, &sourceIndex)) {
                    if (itr->value.IsArray()) {
                        properties_[sourceIndex].dependencies = static_cast<bool*>(allocator_->Malloc(sizeof(bool) * propertyCount_));
                        std::memset(properties_[sourceIndex].dependencies, 0, sizeof(bool)* propertyCount_);
                        for (ConstValueIterator targetItr = itr->value.Begin(); targetItr != itr->value.End(); ++targetItr) {
                            SizeType targetIndex;
                            if (FindPropertyIndex(*targetItr, &targetIndex))
                                properties_[sourceIndex].dependencies[targetIndex] = true;
                        }
                    }
                    else if (itr->value.IsObject()) {
                        hasSchemaDependencies_ = true;
                        schemaDocument->CreateSchema(&properties_[sourceIndex].dependenciesSchema, q.Append(itr->name, allocator_), itr->value, document, id_);
                        properties_[sourceIndex].dependenciesValidatorIndex = validatorCount_;
                        validatorCount_++;
                    }
                }
            }
        }

        if (const ValueType* v = GetMember(value, GetAdditionalPropertiesString())) {
            if (v->IsBool())
                additionalProperties_ = v->GetBool();
            else if (v->IsObject())
                schemaDocument->CreateSchema(&additionalPropertiesSchema_, p.Append(GetAdditionalPropertiesString(), allocator_), *v, document, id_);
        }

        AssignIfExist(minProperties_, value, GetMinPropertiesString());
        AssignIfExist(maxProperties_, value, GetMaxPropertiesString());

        // Массив
        if (const ValueType* v = GetMember(value, GetItemsString())) {
            PointerType q = p.Append(GetItemsString(), allocator_);
            if (v->IsObject()) // Проверка списка
                schemaDocument->CreateSchema(&itemsList_, q, *v, document, id_);
            else if (v->IsArray()) { // Проверка кортежа
                itemsTuple_ = static_cast<const Schema**>(allocator_->Malloc(sizeof(const Schema*) * v->Size()));
                SizeType index = 0;
                for (ConstValueIterator itr = v->Begin(); itr != v->End(); ++itr, index++)
                    schemaDocument->CreateSchema(&itemsTuple_[itemsTupleCount_++], q.Append(index, allocator_), *itr, document, id_);
            }
        }

        AssignIfExist(minItems_, value, GetMinItemsString());
        AssignIfExist(maxItems_, value, GetMaxItemsString());

        // Дополнительные элементы не требуются для openapi 2.0 и 3.0.
        if (spec_.oapi != kVersion20 && spec_.oapi != kVersion30)
        if (const ValueType* v = GetMember(value, GetAdditionalItemsString())) {
            if (v->IsBool())
                additionalItems_ = v->GetBool();
            else if (v->IsObject())
                schemaDocument->CreateSchema(&additionalItemsSchema_, p.Append(GetAdditionalItemsString(), allocator_), *v, document, id_);
        }

        AssignIfExist(uniqueItems_, value, GetUniqueItemsString());

        // Строка
        AssignIfExist(minLength_, value, GetMinLengthString());
        AssignIfExist(maxLength_, value, GetMaxLengthString());

        if (const ValueType* v = GetMember(value, GetPatternString()))
            pattern_ = CreatePattern(*v, schemaDocument, p.Append(GetPatternString(), allocator_));

        // Номер
        if (const ValueType* v = GetMember(value, GetMinimumString()))
            if (v->IsNumber())
                minimum_.CopyFrom(*v, *allocator_);

        if (const ValueType* v = GetMember(value, GetMaximumString()))
            if (v->IsNumber())
                maximum_.CopyFrom(*v, *allocator_);

        AssignIfExist(exclusiveMinimum_, value, GetExclusiveMinimumString());
        AssignIfExist(exclusiveMaximum_, value, GetExclusiveMaximumString());

        if (const ValueType* v = GetMember(value, GetMultipleOfString()))
            if (v->IsNumber() && v->GetDouble() > 0.0)
                multipleOf_.CopyFrom(*v, *allocator_);

        // По умолчанию
        if (const ValueType* v = GetMember(value, GetDefaultValueString()))
            if (v->IsString())
                defaultValueLength_ = v->GetStringLength();

        // ReadOnly - только открытый API (до поддержки проекта 7)
        // WriteOnly - только открытый API 3 (до тех пор, пока не будет поддерживаться проект 7)
        // И то и другое не может быть правдой
        if (spec_.oapi != kVersionNone)
            AssignIfExist(readOnly_, value, GetReadOnlyString());
        if (spec_.oapi >= kVersion30)
            AssignIfExist(writeOnly_, value, GetWriteOnlyString());
        if (readOnly_ && writeOnly_)
            schemaDocument->SchemaError(kSchemaErrorReadOnlyAndWriteOnly, p);

        // Nullable - открыть только API 3
        // Если это правда, то разделы «null» должны быть допустимого типа.
        if (spec_.oapi >= kVersion30) {
            AssignIfExist(nullable_, value, GetNullableString());
            if (nullable_)
                AddType(GetNullString());
        }
    }

    ~Schema() {
        AllocatorType::Free(enum_);
        if (properties_) {
            for (SizeType i = 0; i < propertyCount_; i++)
                properties_[i].~Property();
            AllocatorType::Free(properties_);
        }
        if (patternProperties_) {
            for (SizeType i = 0; i < patternPropertyCount_; i++)
                patternProperties_[i].~PatternProperty();
            AllocatorType::Free(patternProperties_);
        }
        AllocatorType::Free(itemsTuple_);
#if RAPIDJSON_SCHEMA_HAS_REGEX
        if (pattern_) {
            pattern_->~RegexType();
            AllocatorType::Free(pattern_);
        }
#endif
    }

    const SValue& GetURI() const {
        return uri_;
    }

    const UriType& GetId() const {
        return id_;
    }

    const Specification& GetSpecification() const {
        return spec_;
    }

    const PointerType& GetPointer() const {
        return pointer_;
    }

    bool BeginValue(Context& context) const {
        RAPIDJSON_SCHEMA_PRINT(Method, "Schema::BeginValue");
        if (context.inArray) {
            if (uniqueItems_)
                context.valueUniqueness = true;

            if (itemsList_)
                context.valueSchema = itemsList_;
            else if (itemsTuple_) {
                if (context.arrayElementIndex < itemsTupleCount_)
                    context.valueSchema = itemsTuple_[context.arrayElementIndex];
                else if (additionalItemsSchema_)
                    context.valueSchema = additionalItemsSchema_;
                else if (additionalItems_)
                    context.valueSchema = typeless_;
                else {
                    context.error_handler.DisallowedItem(context.arrayElementIndex);
                    // Необходимо установить valueSchema, если установлен kValidateContinueOnErrorFlag, иначе сообщается о ложной форме типа Dragon.
                    context.valueSchema = typeless_;
                    // Необходимо увеличить arrayElementIndex, когда установлен kValidateContinueOnErrorFlag.
                    context.arrayElementIndex++;
                    RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorAdditionalItems);
                }
            }
            else
                context.valueSchema = typeless_;

            context.arrayElementIndex++;
        }
        return true;
    }

    RAPIDJSON_FORCEINLINE bool EndValue(Context& context) const {
        RAPIDJSON_SCHEMA_PRINT(Method, "Schema::EndValue");
        // Проверяйте свойства шаблона только в том случае, если у нас есть валидаторы.
        if (context.patternPropertiesValidatorCount > 0) {
            bool otherValid = false;
            SizeType count = context.patternPropertiesValidatorCount;
            if (context.objectPatternValidatorType != Context::kPatternValidatorOnly)
                otherValid = context.patternPropertiesValidators[--count]->IsValid();

            bool patternValid = true;
            for (SizeType i = 0; i < count; i++)
                if (!context.patternPropertiesValidators[i]->IsValid()) {
                    patternValid = false;
                    break;
                }

            if (context.objectPatternValidatorType == Context::kPatternValidatorOnly) {
                if (!patternValid) {
                    context.error_handler.PropertyViolations(context.patternPropertiesValidators, count);
                    RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorPatternProperties);
                }
            }
            else if (context.objectPatternValidatorType == Context::kPatternValidatorWithProperty) {
                if (!patternValid || !otherValid) {
                    context.error_handler.PropertyViolations(context.patternPropertiesValidators, count + 1);
                    RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorPatternProperties);
                }
            }
            else if (!patternValid && !otherValid) { // kPatternValidatorWithAdditionalProperty)
                context.error_handler.PropertyViolations(context.patternPropertiesValidators, count + 1);
                RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorPatternProperties);
            }
        }

        // Только для перечислений проверяйте, есть ли у нас хэшер
        if (enum_ && context.hasher) {
            const uint64_t h = context.factory.GetHashCode(context.hasher);
            for (SizeType i = 0; i < enumCount_; i++)
                if (enum_[i] == h)
                    goto foundEnum;
            context.error_handler.DisallowedValue(kValidateErrorEnum);
            RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorEnum);
            foundEnum:;
        }

        // Проверяйте allOf и т. д. д. только в том случае, если у нас есть валидаторы.
        if (context.validatorCount > 0) {
            if (allOf_.schemas)
                for (SizeType i = allOf_.begin; i < allOf_.begin + allOf_.count; i++)
                    if (!context.validators[i]->IsValid()) {
                        context.error_handler.NotAllOf(&context.validators[allOf_.begin], allOf_.count);
                        RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorAllOf);
                    }

            if (anyOf_.schemas) {
                for (SizeType i = anyOf_.begin; i < anyOf_.begin + anyOf_.count; i++)
                    if (context.validators[i]->IsValid())
                        goto foundAny;
                context.error_handler.NoneOf(&context.validators[anyOf_.begin], anyOf_.count);
                RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorAnyOf);
                foundAny:;
            }

            if (oneOf_.schemas) {
                bool oneValid = false;
                SizeType firstMatch = 0;
                for (SizeType i = oneOf_.begin; i < oneOf_.begin + oneOf_.count; i++)
                    if (context.validators[i]->IsValid()) {
                        if (oneValid) {
                            context.error_handler.MultipleOneOf(firstMatch, i - oneOf_.begin);
                            RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorOneOfMatch);
                        } else {
                            oneValid = true;
                            firstMatch = i - oneOf_.begin;
                        }
                    }
                if (!oneValid) {
                    context.error_handler.NotOneOf(&context.validators[oneOf_.begin], oneOf_.count);
                    RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorOneOf);
                }
            }

            if (not_ && context.validators[notValidatorIndex_]->IsValid()) {
                context.error_handler.Disallowed();
                RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorNot);
            }
        }

        return true;
    }

    bool Null(Context& context) const {
        RAPIDJSON_SCHEMA_PRINT(Method, "Schema::Null");
        if (!(type_ & (1 << kNullSchemaType))) {
            DisallowedType(context, GetNullString());
            RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorType);
        }
        return CreateParallelValidator(context);
    }

    bool Bool(Context& context, bool b) const {
        RAPIDJSON_SCHEMA_PRINT(Method, "Schema::Bool", b);
        if (!CheckBool(context, b))
            return false;
        return CreateParallelValidator(context);
    }

    bool Int(Context& context, int i) const {
        RAPIDJSON_SCHEMA_PRINT(Method, "Schema::Int", (int64_t)i);
        if (!CheckInt(context, i))
            return false;
        return CreateParallelValidator(context);
    }

    bool Uint(Context& context, unsigned u) const {
        RAPIDJSON_SCHEMA_PRINT(Method, "Schema::Uint", (uint64_t)u);
        if (!CheckUint(context, u))
            return false;
        return CreateParallelValidator(context);
    }

    bool Int64(Context& context, int64_t i) const {
        RAPIDJSON_SCHEMA_PRINT(Method, "Schema::Int64", i);
        if (!CheckInt(context, i))
            return false;
        return CreateParallelValidator(context);
    }

    bool Uint64(Context& context, uint64_t u) const {
        RAPIDJSON_SCHEMA_PRINT(Method, "Schema::Uint64", u);
        if (!CheckUint(context, u))
            return false;
        return CreateParallelValidator(context);
    }

    bool Double(Context& context, double d) const {
        RAPIDJSON_SCHEMA_PRINT(Method, "Schema::Double", d);
        if (!(type_ & (1 << kNumberSchemaType))) {
            DisallowedType(context, GetNumberString());
            RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorType);
        }

        if (!minimum_.IsNull() && !CheckDoubleMinimum(context, d))
            return false;

        if (!maximum_.IsNull() && !CheckDoubleMaximum(context, d))
            return false;

        if (!multipleOf_.IsNull() && !CheckDoubleMultipleOf(context, d))
            return false;

        return CreateParallelValidator(context);
    }

    bool String(Context& context, const Ch* str, SizeType length, bool) const {
        RAPIDJSON_SCHEMA_PRINT(Method, "Schema::String", str);
        if (!(type_ & (1 << kStringSchemaType))) {
            DisallowedType(context, GetStringString());
            RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorType);
        }

        if (minLength_ != 0 || maxLength_ != SizeType(~0)) {
            SizeType count;
            if (internal::CountStringCodePoint<EncodingType>(str, length, &count)) {
                if (count < minLength_) {
                    context.error_handler.TooShort(str, length, minLength_);
                    RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorMinLength);
                }
                if (count > maxLength_) {
                    context.error_handler.TooLong(str, length, maxLength_);
                    RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorMaxLength);
                }
            }
        }

        if (pattern_ && !IsPatternMatch(pattern_, str, length)) {
            context.error_handler.DoesNotMatch(str, length);
            RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorPattern);
        }

        return CreateParallelValidator(context);
    }

    bool StartObject(Context& context) const {
        RAPIDJSON_SCHEMA_PRINT(Method, "Schema::StartObject");
        if (!(type_ & (1 << kObjectSchemaType))) {
            DisallowedType(context, GetObjectString());
            RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorType);
        }

        if (hasDependencies_ || hasRequired_) {
            context.propertyExist = static_cast<bool*>(context.factory.MallocState(sizeof(bool) * propertyCount_));
            std::memset(context.propertyExist, 0, sizeof(bool) * propertyCount_);
        }

        if (patternProperties_) { // предварительно выделить массив схемы
            SizeType count = patternPropertyCount_ + 1; // дополнительно для valuePatternValidatorType
            context.patternPropertiesSchemas = static_cast<const SchemaType**>(context.factory.MallocState(sizeof(const SchemaType*) * count));
            context.patternPropertiesSchemaCount = 0;
            std::memset(context.patternPropertiesSchemas, 0, sizeof(SchemaType*) * count);
        }

        return CreateParallelValidator(context);
    }

    bool Key(Context& context, const Ch* str, SizeType len, bool) const {
        RAPIDJSON_SCHEMA_PRINT(Method, "Schema::Key", str);

        if (patternProperties_) {
            context.patternPropertiesSchemaCount = 0;
            for (SizeType i = 0; i < patternPropertyCount_; i++)
                if (patternProperties_[i].pattern && IsPatternMatch(patternProperties_[i].pattern, str, len)) {
                    context.patternPropertiesSchemas[context.patternPropertiesSchemaCount++] = patternProperties_[i].schema;
                    context.valueSchema = typeless_;
                }
        }

        SizeType index  = 0;
        if (FindPropertyIndex(ValueType(str, len).Move(), &index)) {
            if (context.patternPropertiesSchemaCount > 0) {
                context.patternPropertiesSchemas[context.patternPropertiesSchemaCount++] = properties_[index].schema;
                context.valueSchema = typeless_;
                context.valuePatternValidatorType = Context::kPatternValidatorWithProperty;
            }
            else
                context.valueSchema = properties_[index].schema;

            if (context.propertyExist)
                context.propertyExist[index] = true;

            return true;
        }

        if (additionalPropertiesSchema_) {
            if (context.patternPropertiesSchemaCount > 0) {
                context.patternPropertiesSchemas[context.patternPropertiesSchemaCount++] = additionalPropertiesSchema_;
                context.valueSchema = typeless_;
                context.valuePatternValidatorType = Context::kPatternValidatorWithAdditionalProperty;
            }
            else
                context.valueSchema = additionalPropertiesSchema_;
            return true;
        }
        else if (additionalProperties_) {
            context.valueSchema = typeless_;
            return true;
        }

        if (context.patternPropertiesSchemaCount == 0) { // PatternProperties не производятся вручную.
            // Необходимо установить valueSchema, если установлен kValidateContinueOnErrorFlag, иначе сообщается о ложной форме типа Dragon.
            context.valueSchema = typeless_;
            context.error_handler.DisallowedProperty(str, len);
            RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorAdditionalProperties);
        }

        return true;
    }

    bool EndObject(Context& context, SizeType memberCount) const {
        RAPIDJSON_SCHEMA_PRINT(Method, "Schema::EndObject");
        if (hasRequired_) {
            context.error_handler.StartMissingProperties();
            for (SizeType index = 0; index < propertyCount_; index++)
                if (properties_[index].required && !context.propertyExist[index])
                    if (properties_[index].schema->defaultValueLength_ == 0 )
                        context.error_handler.AddMissingProperty(properties_[index].name);
            if (context.error_handler.EndMissingProperties())
                RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorRequired);
        }

        if (memberCount < minProperties_) {
            context.error_handler.TooFewProperties(memberCount, minProperties_);
            RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorMinProperties);
        }

        if (memberCount > maxProperties_) {
            context.error_handler.TooManyProperties(memberCount, maxProperties_);
            RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorMaxProperties);
        }

        if (hasDependencies_) {
            context.error_handler.StartDependencyErrors();
            for (SizeType sourceIndex = 0; sourceIndex < propertyCount_; sourceIndex++) {
                const Property& source = properties_[sourceIndex];
                if (context.propertyExist[sourceIndex]) {
                    if (source.dependencies) {
                        context.error_handler.StartMissingDependentProperties();
                        for (SizeType targetIndex = 0; targetIndex < propertyCount_; targetIndex++)
                            if (source.dependencies[targetIndex] && !context.propertyExist[targetIndex])
                                context.error_handler.AddMissingDependentProperty(properties_[targetIndex].name);
                        context.error_handler.EndMissingDependentProperties(source.name);
                    }
                    else if (source.dependenciesSchema) {
                        ISchemaValidator* dependenciesValidator = context.validators[source.dependenciesValidatorIndex];
                        if (!dependenciesValidator->IsValid())
                            context.error_handler.AddDependencySchemaError(source.name, dependenciesValidator);
                    }
                }
            }
            if (context.error_handler.EndDependencyErrors())
                RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorDependencies);
        }

        return true;
    }

    bool StartArray(Context& context) const {
        RAPIDJSON_SCHEMA_PRINT(Method, "Schema::StartArray");
        context.arrayElementIndex = 0;
        context.inArray = true;  // Убедитесь, что мы заметили, что находимся в массиве

        if (!(type_ & (1 << kArraySchemaType))) {
            DisallowedType(context, GetArrayString());
            RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorType);
        }

        return CreateParallelValidator(context);
    }

    bool EndArray(Context& context, SizeType elementCount) const {
        RAPIDJSON_SCHEMA_PRINT(Method, "Schema::EndArray");
        context.inArray = false;

        if (elementCount < minItems_) {
            context.error_handler.TooFewItems(elementCount, minItems_);
            RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorMinItems);
        }

        if (elementCount > maxItems_) {
            context.error_handler.TooManyItems(elementCount, maxItems_);
            RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorMaxItems);
        }

        return true;
    }

    static const ValueType& GetValidateErrorKeyword(ValidateErrorCode validateErrorCode) {
        switch (validateErrorCode) {
            case kValidateErrorMultipleOf:              return GetMultipleOfString();
            case kValidateErrorMaximum:                 return GetMaximumString();
            case kValidateErrorExclusiveMaximum:        return GetMaximumString(); // То же самое
            case kValidateErrorMinimum:                 return GetMinimumString();
            case kValidateErrorExclusiveMinimum:        return GetMinimumString(); // То же самое

            case kValidateErrorMaxLength:               return GetMaxLengthString();
            case kValidateErrorMinLength:               return GetMinLengthString();
            case kValidateErrorPattern:                 return GetPatternString();

            case kValidateErrorMaxItems:                return GetMaxItemsString();
            case kValidateErrorMinItems:                return GetMinItemsString();
            case kValidateErrorUniqueItems:             return GetUniqueItemsString();
            case kValidateErrorAdditionalItems:         return GetAdditionalItemsString();

            case kValidateErrorMaxProperties:           return GetMaxPropertiesString();
            case kValidateErrorMinProperties:           return GetMinPropertiesString();
            case kValidateErrorRequired:                return GetRequiredString();
            case kValidateErrorAdditionalProperties:    return GetAdditionalPropertiesString();
            case kValidateErrorPatternProperties:       return GetPatternPropertiesString();
            case kValidateErrorDependencies:            return GetDependenciesString();

            case kValidateErrorEnum:                    return GetEnumString();
            case kValidateErrorType:                    return GetTypeString();

            case kValidateErrorOneOf:                   return GetOneOfString();
            case kValidateErrorOneOfMatch:              return GetOneOfString(); // То же самое
            case kValidateErrorAllOf:                   return GetAllOfString();
            case kValidateErrorAnyOf:                   return GetAnyOfString();
            case kValidateErrorNot:                     return GetNotString();

            case kValidateErrorReadOnly:                return GetReadOnlyString();
            case kValidateErrorWriteOnly:               return GetWriteOnlyString();

            default:                                    return GetNullString();
        }
    }


    // Сгенерируйте функции для строкового литерала в соответствии с гл.
#define RAPIDJSON_STRING_(name, ...) \
    static const ValueType& Get##name##String() {\
        static const Ch s[] = { __VA_ARGS__, '\0' };\
        static const ValueType v(s, static_cast<SizeType>(sizeof(s) / sizeof(Ch) - 1));\
        return v;\
    }

    RAPIDJSON_STRING_(Null, 'n', 'u', 'l', 'l')
    RAPIDJSON_STRING_(Boolean, 'b', 'o', 'o', 'l', 'e', 'a', 'n')
    RAPIDJSON_STRING_(Object, 'o', 'b', 'j', 'e', 'c', 't')
    RAPIDJSON_STRING_(Array, 'a', 'r', 'r', 'a', 'y')
    RAPIDJSON_STRING_(String, 's', 't', 'r', 'i', 'n', 'g')
    RAPIDJSON_STRING_(Number, 'n', 'u', 'm', 'b', 'e', 'r')
    RAPIDJSON_STRING_(Integer, 'i', 'n', 't', 'e', 'g', 'e', 'r')
    RAPIDJSON_STRING_(Type, 't', 'y', 'p', 'e')
    RAPIDJSON_STRING_(Enum, 'e', 'n', 'u', 'm')
    RAPIDJSON_STRING_(AllOf, 'a', 'l', 'l', 'O', 'f')
    RAPIDJSON_STRING_(AnyOf, 'a', 'n', 'y', 'O', 'f')
    RAPIDJSON_STRING_(OneOf, 'o', 'n', 'e', 'O', 'f')
    RAPIDJSON_STRING_(Not, 'n', 'o', 't')
    RAPIDJSON_STRING_(Properties, 'p', 'r', 'o', 'p', 'e', 'r', 't', 'i', 'e', 's')
    RAPIDJSON_STRING_(Required, 'r', 'e', 'q', 'u', 'i', 'r', 'e', 'd')
    RAPIDJSON_STRING_(Dependencies, 'd', 'e', 'p', 'e', 'n', 'd', 'e', 'n', 'c', 'i', 'e', 's')
    RAPIDJSON_STRING_(PatternProperties, 'p', 'a', 't', 't', 'e', 'r', 'n', 'P', 'r', 'o', 'p', 'e', 'r', 't', 'i', 'e', 's')
    RAPIDJSON_STRING_(AdditionalProperties, 'a', 'd', 'd', 'i', 't', 'i', 'o', 'n', 'a', 'l', 'P', 'r', 'o', 'p', 'e', 'r', 't', 'i', 'e', 's')
    RAPIDJSON_STRING_(MinProperties, 'm', 'i', 'n', 'P', 'r', 'o', 'p', 'e', 'r', 't', 'i', 'e', 's')
    RAPIDJSON_STRING_(MaxProperties, 'm', 'a', 'x', 'P', 'r', 'o', 'p', 'e', 'r', 't', 'i', 'e', 's')
    RAPIDJSON_STRING_(Items, 'i', 't', 'e', 'm', 's')
    RAPIDJSON_STRING_(MinItems, 'm', 'i', 'n', 'I', 't', 'e', 'm', 's')
    RAPIDJSON_STRING_(MaxItems, 'm', 'a', 'x', 'I', 't', 'e', 'm', 's')
    RAPIDJSON_STRING_(AdditionalItems, 'a', 'd', 'd', 'i', 't', 'i', 'o', 'n', 'a', 'l', 'I', 't', 'e', 'm', 's')
    RAPIDJSON_STRING_(UniqueItems, 'u', 'n', 'i', 'q', 'u', 'e', 'I', 't', 'e', 'm', 's')
    RAPIDJSON_STRING_(MinLength, 'm', 'i', 'n', 'L', 'e', 'n', 'g', 't', 'h')
    RAPIDJSON_STRING_(MaxLength, 'm', 'a', 'x', 'L', 'e', 'n', 'g', 't', 'h')
    RAPIDJSON_STRING_(Pattern, 'p', 'a', 't', 't', 'e', 'r', 'n')
    RAPIDJSON_STRING_(Minimum, 'm', 'i', 'n', 'i', 'm', 'u', 'm')
    RAPIDJSON_STRING_(Maximum, 'm', 'a', 'x', 'i', 'm', 'u', 'm')
    RAPIDJSON_STRING_(ExclusiveMinimum, 'e', 'x', 'c', 'l', 'u', 's', 'i', 'v', 'e', 'M', 'i', 'n', 'i', 'm', 'u', 'm')
    RAPIDJSON_STRING_(ExclusiveMaximum, 'e', 'x', 'c', 'l', 'u', 's', 'i', 'v', 'e', 'M', 'a', 'x', 'i', 'm', 'u', 'm')
    RAPIDJSON_STRING_(MultipleOf, 'm', 'u', 'l', 't', 'i', 'p', 'l', 'e', 'O', 'f')
    RAPIDJSON_STRING_(DefaultValue, 'd', 'e', 'f', 'a', 'u', 'l', 't')
    RAPIDJSON_STRING_(Schema, '$', 's', 'c', 'h', 'e', 'm', 'a')
    RAPIDJSON_STRING_(Ref, '$', 'r', 'e', 'f')
    RAPIDJSON_STRING_(Id, 'i', 'd')
    RAPIDJSON_STRING_(Swagger, 's', 'w', 'a', 'g', 'g', 'e', 'r')
    RAPIDJSON_STRING_(OpenApi, 'o', 'p', 'e', 'n', 'a', 'p', 'i')
    RAPIDJSON_STRING_(ReadOnly, 'r', 'e', 'a', 'd', 'O', 'n', 'l', 'y')
    RAPIDJSON_STRING_(WriteOnly, 'w', 'r', 'i', 't', 'e', 'O', 'n', 'l', 'y')
    RAPIDJSON_STRING_(Nullable, 'n', 'u', 'l', 'l', 'a', 'b', 'l', 'e')

#undef RAPIDJSON_STRING_

private:
    enum SchemaValueType {
        kNullSchemaType,
        kBooleanSchemaType,
        kObjectSchemaType,
        kArraySchemaType,
        kStringSchemaType,
        kNumberSchemaType,
        kIntegerSchemaType,
        kTotalSchemaType
    };

#if RAPIDJSON_SCHEMA_USE_INTERNALREGEX
        typedef internal::GenericRegex<EncodingType, AllocatorType> RegexType;
#elif RAPIDJSON_SCHEMA_USE_STDREGEX
        typedef std::basic_regex<Ch> RegexType;
#else
        typedef char RegexType;
#endif

    struct SchemaArray {
        SchemaArray() : schemas(), count() {}
        ~SchemaArray() { AllocatorType::Free(schemas); }
        const SchemaType** schemas;
        SizeType begin; // начать индекс context.validators
        SizeType count;
    };

    template <typename V1, typename V2>
    void AddUniqueElement(V1& a, const V2& v) {
        for (typename V1::ConstValueIterator itr = a.Begin(); itr != a.End(); ++itr)
            if (*itr == v)
                return;
        V1 c(v, *allocator_);
        a.PushBack(c, *allocator_);
    }

    static const ValueType* GetMember(const ValueType& value, const ValueType& name) {
        typename ValueType::ConstMemberIterator itr = value.FindMember(name);
        return itr != value.MemberEnd() ? &(itr->value) : 0;
    }

    static void AssignIfExist(bool& out, const ValueType& value, const ValueType& name) {
        if (const ValueType* v = GetMember(value, name))
            if (v->IsBool())
                out = v->GetBool();
    }

    static void AssignIfExist(SizeType& out, const ValueType& value, const ValueType& name) {
        if (const ValueType* v = GetMember(value, name))
            if (v->IsUint64() && v->GetUint64() <= SizeType(~0))
                out = static_cast<SizeType>(v->GetUint64());
    }

    void AssignIfExist(SchemaArray& out, SchemaDocumentType& schemaDocument, const PointerType& p, const ValueType& value, const ValueType& name, const ValueType& document) {
        if (const ValueType* v = GetMember(value, name)) {
            if (v->IsArray() && v->Size() > 0) {
                PointerType q = p.Append(name, allocator_);
                out.count = v->Size();
                out.schemas = static_cast<const Schema**>(allocator_->Malloc(out.count * sizeof(const Schema*)));
                memset(out.schemas, 0, sizeof(Schema*)* out.count);
                for (SizeType i = 0; i < out.count; i++)
                    schemaDocument.CreateSchema(&out.schemas[i], q.Append(i, allocator_), (*v)[i], document, id_);
                out.begin = validatorCount_;
                validatorCount_ += out.count;
            }
        }
    }

#if RAPIDJSON_SCHEMA_USE_INTERNALREGEX
    template <typename ValueType>
    RegexType* CreatePattern(const ValueType& value, SchemaDocumentType* sd, const PointerType& p) {
        if (value.IsString()) {
            RegexType* r = new (allocator_->Malloc(sizeof(RegexType))) RegexType(value.GetString(), allocator_);
            if (!r->IsValid()) {
                sd->SchemaErrorValue(kSchemaErrorRegexInvalid, p, value.GetString(), value.GetStringLength());
                r->~RegexType();
                AllocatorType::Free(r);
                r = 0;
            }
            return r;
        }
        return 0;
    }

    static bool IsPatternMatch(const RegexType* pattern, const Ch *str, SizeType) {
        GenericRegexSearch<RegexType> rs(*pattern);
        return rs.Search(str);
    }
#elif RAPIDJSON_SCHEMA_USE_STDREGEX
    template <typename ValueType>
    RegexType* CreatePattern(const ValueType& value, SchemaDocumentType* sd, const PointerType& p) {
        if (value.IsString()) {
            RegexType *r = static_cast<RegexType*>(allocator_->Malloc(sizeof(RegexType)));
            try {
                return new (r) RegexType(value.GetString(), std::size_t(value.GetStringLength()), std::regex_constants::ECMAScript);
            }
            catch (const std::regex_error& e) {
                sd->SchemaErrorValue(kSchemaErrorRegexInvalid, p, value.GetString(), value.GetStringLength());
                AllocatorType::Free(r);
            }
        }
        return 0;
    }

    static bool IsPatternMatch(const RegexType* pattern, const Ch *str, SizeType length) {
        std::match_results<const Ch*> r;
        return std::regex_search(str, str + length, r, *pattern);
    }
#else
    template <typename ValueType>
    RegexType* CreatePattern(const ValueType&) {
        return 0;
    }

    static bool IsPatternMatch(const RegexType*, const Ch *, SizeType) { return true; }
#endif // RAPIDJSON_SCHEMA_USE_STDREGEX

    void AddType(const ValueType& type) {
        if      (type == GetNullString()   ) type_ |= 1 << kNullSchemaType;
        else if (type == GetBooleanString()) type_ |= 1 << kBooleanSchemaType;
        else if (type == GetObjectString() ) type_ |= 1 << kObjectSchemaType;
        else if (type == GetArrayString()  ) type_ |= 1 << kArraySchemaType;
        else if (type == GetStringString() ) type_ |= 1 << kStringSchemaType;
        else if (type == GetIntegerString()) type_ |= 1 << kIntegerSchemaType;
        else if (type == GetNumberString() ) type_ |= (1 << kNumberSchemaType) | (1 << kIntegerSchemaType);
    }

    // При необходимости создаются параллельные валидаторы для зависимостей allOf, AnyOf, oneOf, not и схемы.
    // Также при необходимости создает хеш для перечислений и уникальности массива.
    // Также полезное место для добавления независимых от типа проверок ошибок.
    bool CreateParallelValidator(Context& context) const {
        if (enum_ || context.arrayUniqueness)
            context.hasher = context.factory.CreateHasher();

        if (validatorCount_) {
            RAPIDJSON_ASSERT(context.validators == 0);
            context.validators = static_cast<ISchemaValidator**>(context.factory.MallocState(sizeof(ISchemaValidator*) * validatorCount_));
            std::memset(context.validators, 0, sizeof(ISchemaValidator*) * validatorCount_);
            context.validatorCount = validatorCount_;

            // Всегда возвращайтесь после первой неудачи для этих суб-валидаторов.
            if (allOf_.schemas)
                CreateSchemaValidators(context, allOf_, false);

            if (anyOf_.schemas)
                CreateSchemaValidators(context, anyOf_, false);

            if (oneOf_.schemas)
                CreateSchemaValidators(context, oneOf_, false);

            if (not_)
                context.validators[notValidatorIndex_] = context.factory.CreateSchemaValidator(*not_, false);

            if (hasSchemaDependencies_) {
                for (SizeType i = 0; i < propertyCount_; i++)
                    if (properties_[i].dependenciesSchema)
                        context.validators[properties_[i].dependenciesValidatorIndex] = context.factory.CreateSchemaValidator(*properties_[i].dependenciesSchema, false);
            }
        }

        // Добавьте сюда любые другие независимые от типа проверки.
        if (readOnly_ && (context.flags & kValidateWriteFlag)) {
            context.error_handler.DisallowedWhenWriting();
            RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorReadOnly);
        }
        if (writeOnly_ && (context.flags & kValidateReadFlag)) {
            context.error_handler.DisallowedWhenReading();
            RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorWriteOnly);
        }

        return true;
    }

    void CreateSchemaValidators(Context& context, const SchemaArray& schemas, const bool inheritContinueOnErrors) const {
        for (SizeType i = 0; i < schemas.count; i++)
            context.validators[schemas.begin + i] = context.factory.CreateSchemaValidator(*schemas.schemas[i], inheritContinueOnErrors);
    }

    // O(n)
    bool FindPropertyIndex(const ValueType& name, SizeType* outIndex) const {
        SizeType len = name.GetStringLength();
        const Ch* str = name.GetString();
        for (SizeType index = 0; index < propertyCount_; index++)
            if (properties_[index].name.GetStringLength() == len &&
                (std::memcmp(properties_[index].name.GetString(), str, sizeof(Ch) * len) == 0))
            {
                *outIndex = index;
                return true;
            }
        return false;
    }

    bool CheckBool(Context& context, bool) const {
        if (!(type_ & (1 << kBooleanSchemaType))) {
            DisallowedType(context, GetBooleanString());
            RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorType);
        }
        return true;
    }

    bool CheckInt(Context& context, int64_t i) const {
        if (!(type_ & ((1 << kIntegerSchemaType) | (1 << kNumberSchemaType)))) {
            DisallowedType(context, GetIntegerString());
            RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorType);
        }

        if (!minimum_.IsNull()) {
            if (minimum_.IsInt64()) {
                if (exclusiveMinimum_ ? i <= minimum_.GetInt64() : i < minimum_.GetInt64()) {
                    context.error_handler.BelowMinimum(i, minimum_, exclusiveMinimum_);
                    RAPIDJSON_INVALID_KEYWORD_RETURN(exclusiveMinimum_ ? kValidateErrorExclusiveMinimum : kValidateErrorMinimum);
                }
            }
            else if (minimum_.IsUint64()) {
                context.error_handler.BelowMinimum(i, minimum_, exclusiveMinimum_);
                RAPIDJSON_INVALID_KEYWORD_RETURN(exclusiveMinimum_ ? kValidateErrorExclusiveMinimum : kValidateErrorMinimum); // я <= макс (int64_t) < минимум.  GetUint64()
            }
            else if (!CheckDoubleMinimum(context, static_cast<double>(i)))
                return false;
        }

        if (!maximum_.IsNull()) {
            if (maximum_.IsInt64()) {
                if (exclusiveMaximum_ ? i >= maximum_.GetInt64() : i > maximum_.GetInt64()) {
                    context.error_handler.AboveMaximum(i, maximum_, exclusiveMaximum_);
                    RAPIDJSON_INVALID_KEYWORD_RETURN(exclusiveMaximum_ ? kValidateErrorExclusiveMaximum : kValidateErrorMaximum);
                }
            }
            else if (maximum_.IsUint64()) { }
                /* ничего не делать */ // я <= max(int64_t) <maximum_.  GetUint64()
            else if (!CheckDoubleMaximum(context, static_cast<double>(i)))
                return false;
        }

        if (!multipleOf_.IsNull()) {
            if (multipleOf_.IsUint64()) {
                if (static_cast<uint64_t>(i >= 0 ? i : -i) % multipleOf_.GetUint64() != 0) {
                    context.error_handler.NotMultipleOf(i, multipleOf_);
                    RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorMultipleOf);
                }
            }
            else if (!CheckDoubleMultipleOf(context, static_cast<double>(i)))
                return false;
        }

        return true;
    }

    bool CheckUint(Context& context, uint64_t i) const {
        if (!(type_ & ((1 << kIntegerSchemaType) | (1 << kNumberSchemaType)))) {
            DisallowedType(context, GetIntegerString());
            RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorType);
        }

        if (!minimum_.IsNull()) {
            if (minimum_.IsUint64()) {
                if (exclusiveMinimum_ ? i <= minimum_.GetUint64() : i < minimum_.GetUint64()) {
                    context.error_handler.BelowMinimum(i, minimum_, exclusiveMinimum_);
                    RAPIDJSON_INVALID_KEYWORD_RETURN(exclusiveMinimum_ ? kValidateErrorExclusiveMinimum : kValidateErrorMinimum);
                }
            }
            else if (minimum_.IsInt64())
                /* ничего не делать */; // я >= 0 > минимум.  Getint64()
            else if (!CheckDoubleMinimum(context, static_cast<double>(i)))
                return false;
        }

        if (!maximum_.IsNull()) {
            if (maximum_.IsUint64()) {
                if (exclusiveMaximum_ ? i >= maximum_.GetUint64() : i > maximum_.GetUint64()) {
                    context.error_handler.AboveMaximum(i, maximum_, exclusiveMaximum_);
                    RAPIDJSON_INVALID_KEYWORD_RETURN(exclusiveMaximum_ ? kValidateErrorExclusiveMaximum : kValidateErrorMaximum);
                }
            }
            else if (maximum_.IsInt64()) {
                context.error_handler.AboveMaximum(i, maximum_, exclusiveMaximum_);
                RAPIDJSON_INVALID_KEYWORD_RETURN(exclusiveMaximum_ ? kValidateErrorExclusiveMaximum : kValidateErrorMaximum); // я >= 0 > maximum_
            }
            else if (!CheckDoubleMaximum(context, static_cast<double>(i)))
                return false;
        }

        if (!multipleOf_.IsNull()) {
            if (multipleOf_.IsUint64()) {
                if (i % multipleOf_.GetUint64() != 0) {
                    context.error_handler.NotMultipleOf(i, multipleOf_);
                    RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorMultipleOf);
                }
            }
            else if (!CheckDoubleMultipleOf(context, static_cast<double>(i)))
                return false;
        }

        return true;
    }

    bool CheckDoubleMinimum(Context& context, double d) const {
        if (exclusiveMinimum_ ? d <= minimum_.GetDouble() : d < minimum_.GetDouble()) {
            context.error_handler.BelowMinimum(d, minimum_, exclusiveMinimum_);
            RAPIDJSON_INVALID_KEYWORD_RETURN(exclusiveMinimum_ ? kValidateErrorExclusiveMinimum : kValidateErrorMinimum);
        }
        return true;
    }

    bool CheckDoubleMaximum(Context& context, double d) const {
        if (exclusiveMaximum_ ? d >= maximum_.GetDouble() : d > maximum_.GetDouble()) {
            context.error_handler.AboveMaximum(d, maximum_, exclusiveMaximum_);
            RAPIDJSON_INVALID_KEYWORD_RETURN(exclusiveMaximum_ ? kValidateErrorExclusiveMaximum : kValidateErrorMaximum);
        }
        return true;
    }

    bool CheckDoubleMultipleOf(Context& context, double d) const {
        double a = std::abs(d), b = std::abs(multipleOf_.GetDouble());
        double q = std::floor(a / b);
        double r = a - q * b;
        if (r > 0.0) {
            context.error_handler.NotMultipleOf(d, multipleOf_);
            RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorMultipleOf);
        }
        return true;
    }

    void DisallowedType(Context& context, const ValueType& actualType) const {
        ErrorHandler& eh = context.error_handler;
        eh.StartDisallowedType();

        if (type_ & (1 << kNullSchemaType)) eh.AddExpectedType(GetNullString());
        if (type_ & (1 << kBooleanSchemaType)) eh.AddExpectedType(GetBooleanString());
        if (type_ & (1 << kObjectSchemaType)) eh.AddExpectedType(GetObjectString());
        if (type_ & (1 << kArraySchemaType)) eh.AddExpectedType(GetArrayString());
        if (type_ & (1 << kStringSchemaType)) eh.AddExpectedType(GetStringString());

        if (type_ & (1 << kNumberSchemaType)) eh.AddExpectedType(GetNumberString());
        else if (type_ & (1 << kIntegerSchemaType)) eh.AddExpectedType(GetIntegerString());

        eh.EndDisallowedType(actualType);
    }

    struct Property {
        Property() : schema(), dependenciesSchema(), dependenciesValidatorIndex(), dependencies(), required(false) {}
        ~Property() { AllocatorType::Free(dependencies); }
        SValue name;
        const SchemaType* schema;
        const SchemaType* dependenciesSchema;
        SizeType dependenciesValidatorIndex;
        bool* dependencies;
        bool required;
    };

    struct PatternProperty {
        PatternProperty() : schema(), pattern() {}
        ~PatternProperty() {
            if (pattern) {
                pattern->~RegexType();
                AllocatorType::Free(pattern);
            }
        }
        const SchemaType* schema;
        RegexType* pattern;
    };

    AllocatorType* allocator_;
    SValue uri_;
    UriType id_;
    Specification spec_;
    PointerType pointer_;
    const SchemaType* typeless_;
    uint64_t* enum_;
    SizeType enumCount_;
    SchemaArray allOf_;
    SchemaArray anyOf_;
    SchemaArray oneOf_;
    const SchemaType* not_;
    unsigned type_; // битовая маска kSchemaType
    SizeType validatorCount_;
    SizeType notValidatorIndex_;

    Property* properties_;
    const SchemaType* additionalPropertiesSchema_;
    PatternProperty* patternProperties_;
    SizeType patternPropertyCount_;
    SizeType propertyCount_;
    SizeType minProperties_;
    SizeType maxProperties_;
    bool additionalProperties_;
    bool hasDependencies_;
    bool hasRequired_;
    bool hasSchemaDependencies_;

    const SchemaType* additionalItemsSchema_;
    const SchemaType* itemsList_;
    const SchemaType** itemsTuple_;
    SizeType itemsTupleCount_;
    SizeType minItems_;
    SizeType maxItems_;
    bool additionalItems_;
    bool uniqueItems_;

    RegexType* pattern_;
    SizeType minLength_;
    SizeType maxLength_;

    SValue minimum_;
    SValue maximum_;
    SValue multipleOf_;
    bool exclusiveMinimum_;
    bool exclusiveMaximum_;

    SizeType defaultValueLength_;

    bool readOnly_;
    bool writeOnly_;
    bool nullable_;
};

template<typename Stack, typename Ch>
struct TokenHelper {
    RAPIDJSON_FORCEINLINE static void AppendIndexToken(Stack& documentStack, SizeType index) {
        *documentStack.template Push<Ch>() = '/';
        char buffer[21];
        size_t length = static_cast<size_t>((sizeof(SizeType) == 4 ? u32toa(index, buffer) : u64toa(index, buffer)) - buffer);
        for (size_t i = 0; i < length; i++)
            *documentStack.template Push<Ch>() = static_cast<Ch>(buffer[i]);
    }
};

// Частная специализированная версия для char для предотвращения копирования буфера.
template <typename Stack>
struct TokenHelper<Stack, char> {
    RAPIDJSON_FORCEINLINE static void AppendIndexToken(Stack& documentStack, SizeType index) {
        if (sizeof(SizeType) == 4) {
            char *buffer = documentStack.template Push<char>(1 + 10); // '/' + уинт
            *buffer++ = '/';
            const char* end = internal::u32toa(index, buffer);
             documentStack.template Pop<char>(static_cast<size_t>(10 - (end - buffer)));
        }
        else {
            char *buffer = documentStack.template Push<char>(1 + 20); // '/' + uint64
            *buffer++ = '/';
            const char* end = internal::u64toa(index, buffer);
            documentStack.template Pop<char>(static_cast<size_t>(20 - (end - buffer)));
        }
    }
};

} // внутреннее пространство имен

///////////////////////////////////////////////////////////////////////////////
// IGenericRemoteSchemaDocumentProvider

template <typename SchemaDocumentType>
class IGenericRemoteSchemaDocumentProvider {
public:
    typedef typename SchemaDocumentType::Ch Ch;
    typedef typename SchemaDocumentType::ValueType ValueType;
    typedef typename SchemaDocumentType::AllocatorType AllocatorType;

    virtual ~IGenericRemoteSchemaDocumentProvider() {}
    virtual const SchemaDocumentType* GetRemoteDocument(const Ch* uri, SizeType length) = 0;
    virtual const SchemaDocumentType* GetRemoteDocument(const GenericUri<ValueType, AllocatorType> uri, Specification& spec) {
        // Реализация по умолчанию просто требует совместимости.
        // Следующая строка подавляет предупреждение о неиспользуемых параметрах.
        (void)spec;
        // printf("GetRemoteDocument: %d %d\n", spec.draft, spec.oapi);
        return GetRemoteDocument(uri.GetBaseString(), uri.GetBaseStringLength());
    }
};

///////////////////////////////////////////////////////////////////////////////
// GenericSchemaDocument

//!  Документ схемы JSON.
/*!
    Документ схемы JSON представляет собой скомпилированную версию схемы JSON.
    По сути, это дерево внутренней::Схемы.

    \note Это неизменяемый класс (т. е. его экземпляр не может быть изменен после создания).
    \tparam ValueT Тип значенияJSON(например, \c Value), а также указана кодировка.
    \tparam Распределитель типа распределителя для выделения памяти для этого документа.
*/
template <typename ValueT, typename Allocator = CrtAllocator>
class GenericSchemaDocument {
public:
    typedef ValueT ValueType;
    typedef IGenericRemoteSchemaDocumentProvider<GenericSchemaDocument> IRemoteSchemaDocumentProviderType;
    typedef Allocator AllocatorType;
    typedef typename ValueType::EncodingType EncodingType;
    typedef typename EncodingType::Ch Ch;
    typedef internal::Schema<GenericSchemaDocument> SchemaType;
    typedef GenericPointer<ValueType, Allocator> PointerType;
    typedef GenericValue<EncodingType, AllocatorType> GValue;
    typedef GenericUri<ValueType, Allocator> UriType;
    typedef GenericStringRef<Ch> StringRefType;
    friend class internal::Schema<GenericSchemaDocument>;
    template <typename, typename, typename>
    friend class GenericSchemaValidator;

    //! Конструктор.
    /*!
        Скомпилируйте документ JSON в документ схемы.

        \param document ДокументJSONв качестве источника.
        \param uri БазовыйURIсхемы этого документа для целей отчетности о нарушениях.
        \param uriLength Длина имени \c в кодовых точках.
        \param RemoteProvider Необязательный поставщик документов удаленной схемы для разрешения удаленных ссылок. Может быть нулевым.
        \param allocator Необязательный экземпляр распределителя для выделения памяти. Может быть нулевым.
        \param pointer Необязательный указательJSONв начало схемы документа.
        \param spec Необязательный черновик схемы или версия OpenAPI. Используется, если в документе нет характеристик. По умолчанию черновик-04.
    */
    explicit GenericSchemaDocument(const ValueType& document, const Ch* uri = 0, SizeType uriLength = 0,
        IRemoteSchemaDocumentProviderType* remoteProvider = 0, Allocator* allocator = 0,
        const PointerType& pointer = PointerType(), // PR #1393
        const Specification& spec = Specification(kDraft04)) :
        remoteProvider_(remoteProvider),
        allocator_(allocator),
        ownAllocator_(),
        root_(),
        typeless_(),
        schemaMap_(allocator, kInitialSchemaMapSize),
        schemaRef_(allocator, kInitialSchemaRefSize),
        spec_(spec),
        error_(kObjectType),
        currentError_()
    {
        RAPIDJSON_SCHEMA_PRINT(Method, "GenericSchemaDocument::GenericSchemaDocument");
        if (!allocator_)
            ownAllocator_ = allocator_ = RAPIDJSON_NEW(Allocator)();

        Ch noUri[1] = {0};
        uri_.SetString(uri ? uri : noUri, uriLength, *allocator_);
        docId_ = UriType(uri_, allocator_);

        typeless_ = static_cast<SchemaType*>(allocator_->Malloc(sizeof(SchemaType)));
        new (typeless_) SchemaType(this, PointerType(), ValueType(kObjectType).Move(), ValueType(kObjectType).Move(), allocator_, docId_);

        // Создайте черновик схемы или откройте версию API.
        // Мы всегда ищем только «$schema», «swagger» или «openapi» в корне документа.
        SetSchemaSpecification(document);

        // Создайте корневую схему, она вызываетCreateSchema()для создания схемы под,
        // ВызовитеHandleRefSchema(), если есть $ref.
        // PR #1393 использовать указатель ввода, если он есть.
        root_ = typeless_;
        if (pointer.GetTokenCount() == 0) {
            CreateSchemaRecursive(&root_, pointer, document, document, docId_);
        }
        else if (const ValueType* v = pointer.Get(document)) {
            CreateSchema(&root_, pointer, *v, document, docId_);
        }
        else {
            GenericStringBuffer<EncodingType> sb;
            pointer.StringifyUriFragment(sb);
            SchemaErrorValue(kSchemaErrorStartUnknown, PointerType(), sb.GetString(), static_cast<SizeType>(sb.GetSize() / sizeof(Ch)));
        }

        RAPIDJSON_ASSERT(root_ != 0);

        schemaRef_.ShrinkToFit(); // Освободить всю память для ссылки
    }

#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
    //! Переместить конструктор в C++11
    GenericSchemaDocument(GenericSchemaDocument&& rhs) RAPIDJSON_NOEXCEPT :
        remoteProvider_(rhs.remoteProvider_),
        allocator_(rhs.allocator_),
        ownAllocator_(rhs.ownAllocator_),
        root_(rhs.root_),
        typeless_(rhs.typeless_),
        schemaMap_(std::move(rhs.schemaMap_)),
        schemaRef_(std::move(rhs.schemaRef_)),
        uri_(std::move(rhs.uri_)),
        docId_(std::move(rhs.docId_)),
        spec_(rhs.spec_),
        error_(std::move(rhs.error_)),
        currentError_(std::move(rhs.currentError_))
    {
        rhs.remoteProvider_ = 0;
        rhs.allocator_ = 0;
        rhs.ownAllocator_ = 0;
        rhs.typeless_ = 0;
    }
#endif

    //! Деструктор
    ~GenericSchemaDocument() {
        while (!schemaMap_.Empty())
            schemaMap_.template Pop<SchemaEntry>(1)->~SchemaEntry();

        if (typeless_) {
            typeless_->~SchemaType();
            Allocator::Free(typeless_);
        }

        // они могут сохранить некоторые данные распределителя, поэтому очистите их перед удалениемownAllocator_.
        uri_.SetNull();
        error_.SetNull();
        currentError_.SetNull();

        RAPIDJSON_DELETE(ownAllocator_);
    }

    const GValue& GetURI() const { return uri_; }

    const Specification& GetSpecification() const { return spec_; }
    bool IsSupportedSpecification() const { return spec_.IsSupported(); }

    //! Статический метод для получения спецификации любого документа схемы
    //  Возвращает kDraftNone, если документ молчит
    static const Specification GetSpecification(const ValueType& document) {
      SchemaDraft draft = GetSchemaDraft(document);
      if (draft != kDraftNone)
        return Specification(draft);
      else {
        OpenApiVersion oapi = GetOpenApiVersion(document);
        if (oapi != kVersionNone)
          return Specification(oapi);
      }
      return Specification(kDraftNone);
    }

    //! Получите корневую схему.
    const SchemaType& GetRoot() const { return *root_; }

    //! Получает объект ошибки.
    GValue& GetError() { return error_; }
    const GValue& GetError() const { return error_; }

    static const StringRefType& GetSchemaErrorKeyword(SchemaErrorCode schemaErrorCode) {
        switch (schemaErrorCode) {
            case kSchemaErrorStartUnknown:             return GetStartUnknownString();
            case kSchemaErrorRefPlainName:             return GetRefPlainNameString();
            case kSchemaErrorRefInvalid:               return GetRefInvalidString();
            case kSchemaErrorRefPointerInvalid:        return GetRefPointerInvalidString();
            case kSchemaErrorRefUnknown:               return GetRefUnknownString();
            case kSchemaErrorRefCyclical:              return GetRefCyclicalString();
            case kSchemaErrorRefNoRemoteProvider:      return GetRefNoRemoteProviderString();
            case kSchemaErrorRefNoRemoteSchema:        return GetRefNoRemoteSchemaString();
            case kSchemaErrorRegexInvalid:             return GetRegexInvalidString();
            case kSchemaErrorSpecUnknown:              return GetSpecUnknownString();
            case kSchemaErrorSpecUnsupported:          return GetSpecUnsupportedString();
            case kSchemaErrorSpecIllegal:              return GetSpecIllegalString();
            case kSchemaErrorReadOnlyAndWriteOnly:     return GetReadOnlyAndWriteOnlyString();
            default:                                   return GetNullString();
        }
    }

    //! Метод ошибки по умолчанию
    void SchemaError(const SchemaErrorCode code, const PointerType& location) {
      currentError_ = GValue(kObjectType);
      AddCurrentError(code, location);
    }

    //! Метод ошибки с вставкой одного строкового значения
    void SchemaErrorValue(const SchemaErrorCode code, const PointerType& location, const Ch* value, SizeType length) {
      currentError_ = GValue(kObjectType);
      currentError_.AddMember(GetValueString(), GValue(value, length, *allocator_).Move(), *allocator_);
      AddCurrentError(code, location);
    }

    //! Метод ошибки с неверным указателем
    void SchemaErrorPointer(const SchemaErrorCode code, const PointerType& location, const Ch* value, SizeType length, const PointerType& pointer) {
      currentError_ = GValue(kObjectType);
      currentError_.AddMember(GetValueString(), GValue(value, length, *allocator_).Move(), *allocator_);
      currentError_.AddMember(GetOffsetString(), static_cast<SizeType>(pointer.GetParseErrorOffset() / sizeof(Ch)), *allocator_);
      AddCurrentError(code, location);
    }

  private:
    //! Запретить копирование
    GenericSchemaDocument(const GenericSchemaDocument&);
    //! Запретить назначение
    GenericSchemaDocument& operator=(const GenericSchemaDocument&);

    typedef const PointerType* SchemaRefPtr; // PR #1393

    struct SchemaEntry {
        SchemaEntry(const PointerType& p, SchemaType* s, bool o, Allocator* allocator) : pointer(p, allocator), schema(s), owned(o) {}
        ~SchemaEntry() {
            if (owned) {
                schema->~SchemaType();
                Allocator::Free(schema);
            }
        }
        PointerType pointer;
        SchemaType* schema;
        bool owned;
    };

    void AddErrorInstanceLocation(GValue& result, const PointerType& location) {
      GenericStringBuffer<EncodingType> sb;
      location.StringifyUriFragment(sb);
      GValue instanceRef(sb.GetString(), static_cast<SizeType>(sb.GetSize() / sizeof(Ch)), *allocator_);
      result.AddMember(GetInstanceRefString(), instanceRef, *allocator_);
    }

    void AddError(GValue& keyword, GValue& error) {
      typename GValue::MemberIterator member = error_.FindMember(keyword);
      if (member == error_.MemberEnd())
        error_.AddMember(keyword, error, *allocator_);
      else {
        if (member->value.IsObject()) {
          GValue errors(kArrayType);
          errors.PushBack(member->value, *allocator_);
          member->value = errors;
        }
        member->value.PushBack(error, *allocator_);
      }
    }

    void AddCurrentError(const SchemaErrorCode code, const PointerType& location) {
      RAPIDJSON_SCHEMA_PRINT(InvalidKeyword, GetSchemaErrorKeyword(code));
      currentError_.AddMember(GetErrorCodeString(), code, *allocator_);
      AddErrorInstanceLocation(currentError_, location);
      AddError(GValue(GetSchemaErrorKeyword(code)).Move(), currentError_);
    }

#define RAPIDJSON_STRING_(name, ...) \
    static const StringRefType& Get##name##String() {\
        static const Ch s[] = { __VA_ARGS__, '\0' };\
        static const StringRefType v(s, static_cast<SizeType>(sizeof(s) / sizeof(Ch) - 1)); \
        return v;\
    }

    RAPIDJSON_STRING_(InstanceRef, 'i', 'n', 's', 't', 'a', 'n', 'c', 'e', 'R', 'e', 'f')
    RAPIDJSON_STRING_(ErrorCode, 'e', 'r', 'r', 'o', 'r', 'C', 'o', 'd', 'e')
    RAPIDJSON_STRING_(Value, 'v', 'a', 'l', 'u', 'e')
    RAPIDJSON_STRING_(Offset, 'o', 'f', 'f', 's', 'e', 't')

    RAPIDJSON_STRING_(Null, 'n', 'u', 'l', 'l')
    RAPIDJSON_STRING_(SpecUnknown, 'S', 'p', 'e', 'c', 'U', 'n', 'k', 'n', 'o', 'w', 'n')
    RAPIDJSON_STRING_(SpecUnsupported, 'S', 'p', 'e', 'c', 'U', 'n', 's', 'u', 'p', 'p', 'o', 'r', 't', 'e', 'd')
    RAPIDJSON_STRING_(SpecIllegal, 'S', 'p', 'e', 'c', 'I', 'l', 'l', 'e', 'g', 'a', 'l')
    RAPIDJSON_STRING_(StartUnknown, 'S', 't', 'a', 'r', 't', 'U', 'n', 'k', 'n', 'o', 'w', 'n')
    RAPIDJSON_STRING_(RefPlainName, 'R', 'e', 'f', 'P', 'l', 'a', 'i', 'n', 'N', 'a', 'm', 'e')
    RAPIDJSON_STRING_(RefInvalid, 'R', 'e', 'f', 'I', 'n', 'v', 'a', 'l', 'i', 'd')
    RAPIDJSON_STRING_(RefPointerInvalid, 'R', 'e', 'f', 'P', 'o', 'i', 'n', 't', 'e', 'r', 'I', 'n', 'v', 'a', 'l', 'i', 'd')
    RAPIDJSON_STRING_(RefUnknown, 'R', 'e', 'f', 'U', 'n', 'k', 'n', 'o', 'w', 'n')
    RAPIDJSON_STRING_(RefCyclical, 'R', 'e', 'f', 'C', 'y', 'c', 'l', 'i', 'c', 'a', 'l')
    RAPIDJSON_STRING_(RefNoRemoteProvider, 'R', 'e', 'f', 'N', 'o', 'R', 'e', 'm', 'o', 't', 'e', 'P', 'r', 'o', 'v', 'i', 'd', 'e', 'r')
    RAPIDJSON_STRING_(RefNoRemoteSchema, 'R', 'e', 'f', 'N', 'o', 'R', 'e', 'm', 'o', 't', 'e', 'S', 'c', 'h', 'e', 'm', 'a')
    RAPIDJSON_STRING_(ReadOnlyAndWriteOnly, 'R', 'e', 'a', 'd', 'O', 'n', 'l', 'y', 'A', 'n', 'd', 'W', 'r', 'i', 't', 'e', 'O', 'n', 'l', 'y')
    RAPIDJSON_STRING_(RegexInvalid, 'R', 'e', 'g', 'e', 'x', 'I', 'n', 'v', 'a', 'l', 'i', 'd')

#undef RAPIDJSON_STRING_

    // Статический метод для получения черновика схемы любого документа схемы
    static SchemaDraft GetSchemaDraft(const ValueType& document) {
        static const Ch kDraft03String[] = { 'h', 't', 't', 'p', ':', '/', '/', 'j', 's', 'o', 'n', '-', 's', 'c', 'h', 'e', 'm', 'a', '.', 'o', 'r', 'g', '/', 'd', 'r', 'a', 'f', 't', '-', '0', '3', '/', 's', 'c', 'h', 'e', 'm', 'a', '#', '\0' };
        static const Ch kDraft04String[] = { 'h', 't', 't', 'p', ':', '/', '/', 'j', 's', 'o', 'n', '-', 's', 'c', 'h', 'e', 'm', 'a', '.', 'o', 'r', 'g', '/', 'd', 'r', 'a', 'f', 't', '-', '0', '4', '/', 's', 'c', 'h', 'e', 'm', 'a', '#', '\0' };
        static const Ch kDraft05String[] = { 'h', 't', 't', 'p', ':', '/', '/', 'j', 's', 'o', 'n', '-', 's', 'c', 'h', 'e', 'm', 'a', '.', 'o', 'r', 'g', '/', 'd', 'r', 'a', 'f', 't', '-', '0', '5', '/', 's', 'c', 'h', 'e', 'm', 'a', '#', '\0' };
        static const Ch kDraft06String[] = { 'h', 't', 't', 'p', ':', '/', '/', 'j', 's', 'o', 'n', '-', 's', 'c', 'h', 'e', 'm', 'a', '.', 'o', 'r', 'g', '/', 'd', 'r', 'a', 'f', 't', '-', '0', '6', '/', 's', 'c', 'h', 'e', 'm', 'a', '#', '\0' };
        static const Ch kDraft07String[] = { 'h', 't', 't', 'p', ':', '/', '/', 'j', 's', 'o', 'n', '-', 's', 'c', 'h', 'e', 'm', 'a', '.', 'o', 'r', 'g', '/', 'd', 'r', 'a', 'f', 't', '-', '0', '7', '/', 's', 'c', 'h', 'e', 'm', 'a', '#', '\0' };
        static const Ch kDraft2019_09String[] = { 'h', 't', 't', 'p', 's', ':', '/', '/', 'j', 's', 'o', 'n', '-', 's', 'c', 'h', 'e', 'm', 'a', '.', 'o', 'r', 'g', '/', 'd', 'r', 'a', 'f', 't', '/', '2', '0', '1', '9', '-', '0', '9', '/', 's', 'c', 'h', 'e', 'm', 'a', '\0' };
        static const Ch kDraft2020_12String[] = { 'h', 't', 't', 'p', 's', ':', '/', '/', 'j', 's', 'o', 'n', '-', 's', 'c', 'h', 'e', 'm', 'a', '.', 'o', 'r', 'g', '/', 'd', 'r', 'a', 'f', 't', '/', '2', '0', '2', '0', '-', '1', '2', '/', 's', 'c', 'h', 'e', 'm', 'a', '\0' };

        if (!document.IsObject()) {
            return kDraftNone;
        }

        // Получите схему черновика по ключевому слову $schema, указанному в месте.
        typename ValueType::ConstMemberIterator itr = document.FindMember(SchemaType::GetSchemaString());
        if (itr != document.MemberEnd()) {
            if (!itr->value.IsString()) return kDraftUnknown;
            const UriType draftUri(itr->value);
            // Проверьте базовый URI на предмет соответствия
            if (draftUri.Match(UriType(kDraft04String), false)) return kDraft04;
            if (draftUri.Match(UriType(kDraft05String), false)) return kDraft05;
            if (draftUri.Match(UriType(kDraft06String), false)) return kDraft06;
            if (draftUri.Match(UriType(kDraft07String), false)) return kDraft07;
            if (draftUri.Match(UriType(kDraft03String), false)) return kDraft03;
            if (draftUri.Match(UriType(kDraft2019_09String), false)) return kDraft2019_09;
            if (draftUri.Match(UriType(kDraft2020_12String), false)) return kDraft2020_12;
            return kDraftUnknown;
        }
        // $схема не найдена
        return kDraftNone;
    }


    // Получите открытую версию API любого документа схемы.
    static OpenApiVersion GetOpenApiVersion(const ValueType& document) {
        static const Ch kVersion20String[] = { '2', '.', '0', '\0' };
        static const Ch kVersion30String[] = { '3', '.', '0', '.', '\0' }; // игнорировать уровень патча
        static const Ch kVersion31String[] = { '3', '.', '1', '.', '\0' }; // игнорировать уровень патча
        static SizeType len = internal::StrLen<Ch>(kVersion30String);

        if (!document.IsObject()) {
            return kVersionNone;
        }

        // Получите открытую версию API, используя ключевое слово swagger/openapi в указанном месте.
        typename ValueType::ConstMemberIterator itr = document.FindMember(SchemaType::GetSwaggerString());
        if (itr == document.MemberEnd()) itr = document.FindMember(SchemaType::GetOpenApiString());
        if (itr != document.MemberEnd()) {
            if (!itr->value.IsString()) return kVersionUnknown;
            const ValueType kVersion20Value(kVersion20String);
            if (kVersion20Value == itr->value) return kVersion20; // должно точно соответствовать 2.0
            const ValueType kVersion30Value(kVersion30String);
            if (itr->value.GetStringLength() > len && kVersion30Value == ValueType(itr->value.GetString(), len)) return kVersion30; // должно соответствовать версии 3.0.x
            const ValueType kVersion31Value(kVersion31String);
            if (itr->value.GetStringLength() > len && kVersion31Value == ValueType(itr->value.GetString(), len)) return kVersion31; // должно соответствовать версии 3.1.x
            return kVersionUnknown;
        }
        // чванство или openapi не найдены
        return kVersionNone;
    }

    // Получите черновик схемы или версию с открытым API (что подразумевает черновик).
    // Сообщите об ошибке, если черновик схемы или открытая версия API не поддерживаются или не распознаются, или и то, и другое в документе, и продолжайте.
    void SetSchemaSpecification(const ValueType& document) {
        // Найдите ключевое слово «$schema», «swagger» или «openapi» в корне документа.
        SchemaDraft docDraft = GetSchemaDraft(document);
        OpenApiVersion docOapi = GetOpenApiVersion(document);
        // Ошибка, если оба в документе
        if (docDraft != kDraftNone && docOapi != kVersionNone)
          SchemaError(kSchemaErrorSpecIllegal, PointerType());
        // Используйте черновик документа или открытую версию API, если она есть, или используйте спецификацию из конструктора.
        if (docDraft != kDraftNone)
            spec_ = Specification(docDraft);
        else if (docOapi != kVersionNone)
            spec_ = Specification(docOapi);
        // Ошибка, если черновик или версия неизвестна
        if (spec_.draft == kDraftUnknown || spec_.oapi == kVersionUnknown)
          SchemaError(kSchemaErrorSpecUnknown, PointerType());
        else if (!spec_.IsSupported())
            SchemaError(kSchemaErrorSpecUnsupported, PointerType());
    }

    // Изменено PR #1393
    void CreateSchemaRecursive(const SchemaType** schema, const PointerType& pointer, const ValueType& v, const ValueType& document, const UriType& id) {
        if (v.GetType() == kObjectType) {
            UriType newid = UriType(CreateSchema(schema, pointer, v, document, id), allocator_);

            for (typename ValueType::ConstMemberIterator itr = v.MemberBegin(); itr != v.MemberEnd(); ++itr)
                CreateSchemaRecursive(0, pointer.Append(itr->name, allocator_), itr->value, document, newid);
        }
        else if (v.GetType() == kArrayType)
            for (SizeType i = 0; i < v.Size(); i++)
                CreateSchemaRecursive(0, pointer.Append(i, allocator_), v[i], document, id);
    }

    // Изменено PR #1393
    const UriType& CreateSchema(const SchemaType** schema, const PointerType& pointer, const ValueType& v, const ValueType& document, const UriType& id) {
        RAPIDJSON_ASSERT(pointer.IsValid());
        GenericStringBuffer<EncodingType> sb;
        pointer.StringifyUriFragment(sb);
        RAPIDJSON_SCHEMA_PRINT(Method, "GenericSchemaDocument::CreateSchema", sb.GetString(), id.GetString());
        if (v.IsObject()) {
            if (const SchemaType* sc = GetSchema(pointer)) {
                if (schema)
                    *schema = sc;
                AddSchemaRefs(const_cast<SchemaType*>(sc));
            }
            else if (!HandleRefSchema(pointer, schema, v, document, id)) {
                // Новый конструктор схем включает себя и свои $ref(ы) вschemaMap_.
                SchemaType* s = new (allocator_->Malloc(sizeof(SchemaType))) SchemaType(this, pointer, v, document, allocator_, id);
                if (schema)
                    *schema = s;
                return s->GetId();
            }
        }
        else {
            if (schema)
                *schema = typeless_;
            AddSchemaRefs(typeless_);
        }
        return id;
    }

    // Изменено PR #1393
    // TODO should this return a UriType& ?
    bool HandleRefSchema(const PointerType& source, const SchemaType** schema, const ValueType& v, const ValueType& document, const UriType& id) {
        typename ValueType::ConstMemberIterator itr = v.FindMember(SchemaType::GetRefString());
        if (itr == v.MemberEnd())
            return false;

        GenericStringBuffer<EncodingType> sb;
        source.StringifyUriFragment(sb);
        RAPIDJSON_SCHEMA_PRINT(Method, "GenericSchemaDocument::HandleRefSchema", sb.GetString(), id.GetString());
        // Разрешить указатель источника на схеме $ref (наконец)
        new (schemaRef_.template Push<SchemaRefPtr>()) SchemaRefPtr(&source);

        if (itr->value.IsString()) {
            SizeType len = itr->value.GetStringLength();
            if (len == 0)
                SchemaError(kSchemaErrorRefInvalid, source);
            else {
                // Сначала разрешите $ref по идентификатору в данной области.
                UriType scopeId = UriType(id, allocator_);
                UriType ref = UriType(itr->value, allocator_).Resolve(scopeId, allocator_);
                RAPIDJSON_SCHEMA_PRINT(SchemaIds, id.GetString(), itr->value.GetString(), ref.GetString());
                // Таким образом, соответствует ли разрешенный фрагмент $ref минус разрешенному идентификатору в этом документе.
                // Поиск от корня. Возвращает подсхему в документе и ее абсолютный указатель JSON.
                PointerType basePointer = PointerType();
                const ValueType *base = FindId(document, ref, basePointer, docId_, false);
                if (!base) {
                    // Удаленная ссылка — позвоните поставщику удаленных документов.
                    if (!remoteProvider_)
                        SchemaError(kSchemaErrorRefNoRemoteProvider, source);
                    else {
                        if (const GenericSchemaDocument* remoteDocument = remoteProvider_->GetRemoteDocument(ref, spec_)) {
                            const Ch* s = ref.GetFragString();
                            len = ref.GetFragStringLength();
                            if (len <= 1 || s[1] == '/') {
                                // Фрагмент указателя JSON, абсолютный в удаленной схеме
                                const PointerType pointer(s, len, allocator_);
                                if (!pointer.IsValid())
                                    SchemaErrorPointer(kSchemaErrorRefPointerInvalid, source, s, len, pointer);
                                else {
                                    // Получить подсхему
                                    if (const SchemaType *sc = remoteDocument->GetSchema(pointer)) {
                                        if (schema)
                                            *schema = sc;
                                        AddSchemaRefs(const_cast<SchemaType *>(sc));
                                        return true;
                                    } else
                                        SchemaErrorValue(kSchemaErrorRefUnknown, source, ref.GetString(), ref.GetStringLength());
                                }
                            } else
                                // Фрагмент простого имени, не разрешенный в удаленной схеме.
                                SchemaErrorValue(kSchemaErrorRefPlainName, source, s, len);
                        } else
                          SchemaErrorValue(kSchemaErrorRefNoRemoteSchema, source, ref.GetString(), ref.GetStringLength());
                    }
                }
                else { // Местная ссылка
                    const Ch* s = ref.GetFragString();
                    len = ref.GetFragStringLength();
                    if (len <= 1 || s[1] == '/') {
                        // Фрагмент указателя JSON относительно разрешенного URI
                        const PointerType relPointer(s, len, allocator_);
                        if (!relPointer.IsValid())
                            SchemaErrorPointer(kSchemaErrorRefPointerInvalid, source, s, len, relPointer);
                        else {
                            // Получить подсхему
                            if (const ValueType *pv = relPointer.Get(*base)) {
                                // Теперь получим абсолютный указатель JSON, добавив его относительно базы
                                PointerType pointer(basePointer, allocator_);
                                for (SizeType i = 0; i < relPointer.GetTokenCount(); i++)
                                    pointer = pointer.Append(relPointer.GetTokens()[i], allocator_);
                                if (IsCyclicRef(pointer))
                                    SchemaErrorValue(kSchemaErrorRefCyclical, source, ref.GetString(), ref.GetStringLength());
                                else {
                                    // Вызовите CreateSchema рекурсивно, но сначала вычислите идентификатор в области действия для цели $ref, когда мы туда перешли.
                                    // TODO: указатель кэша <-> сопоставление идентификаторов
                                    size_t unresolvedTokenIndex;
                                    scopeId = pointer.GetUri(document, docId_, &unresolvedTokenIndex, allocator_);
                                    CreateSchema(schema, pointer, *pv, document, scopeId);
                                    return true;
                                }
                            } else
                                SchemaErrorValue(kSchemaErrorRefUnknown, source, ref.GetString(), ref.GetStringLength());
                        }
                    } else {
                        // Фрагмент простого имени относительно разрешенного URI
                        // Не открываетсяAPI2.0 и 3.0.
                        PointerType pointer(allocator_);
                        if (spec_.oapi == kVersion20 || spec_.oapi == kVersion30)
                            SchemaErrorValue(kSchemaErrorRefPlainName, source, s, len);
                        // Посмотрите, соответствует ли фрагмент идентификатору в этом документе.
                        // Поиск по базе, которую мы только что создали. Возвращает подсхему в документе и ее абсолютный указатель JSON.
                        else if (const ValueType *pv = FindId(*base, ref, pointer, UriType(ref.GetBaseString(), ref.GetBaseStringLength(), allocator_), true, basePointer)) {
                            if (IsCyclicRef(pointer))
                                SchemaErrorValue(kSchemaErrorRefCyclical, source, ref.GetString(), ref.GetStringLength());
                            else {
                                // Вызовите CreateSchema рекурсивно, но сначала вычислите идентификатор в области действия для цели $ref, когда мы туда перешли.
                                // TODO: указатель кэша <-> сопоставление идентификаторов
                                size_t unresolvedTokenIndex;
                                scopeId = pointer.GetUri(document, docId_, &unresolvedTokenIndex, allocator_);
                                CreateSchema(schema, pointer, *pv, document, scopeId);
                                return true;
                            }
                        } else
                            SchemaErrorValue(kSchemaErrorRefUnknown, source, ref.GetString(), ref.GetStringLength());
                    }
                }
            }
        }

        // Неверный/неизвестный $ref
        if (schema)
            *schema = typeless_;
        AddSchemaRefs(typeless_);
        return true;
    }

    //! Найдите первую подсхему с разрешенным идентификатором, соответствующим указанному URI.
    // Если указано полное значение, используйте все URI, иначе игнорируйте фрагмент.
    // Если найдено, верните указатель на подсхему и ее указатель JSON.
    // TODO указатель кэша <-> сопоставление идентификаторов
    ValueType* FindId(const ValueType& doc, const UriType& finduri, PointerType& resptr, const UriType& baseuri, bool full, const PointerType& here = PointerType()) const {
        SizeType i = 0;
        ValueType* resval = 0;
        UriType tempuri = UriType(finduri, allocator_);
        UriType localuri = UriType(baseuri, allocator_);
        if (doc.GetType() == kObjectType) {
            // Установите базу URI этого объекта.
            typename ValueType::ConstMemberIterator m = doc.FindMember(SchemaType::GetIdString());
            if (m != doc.MemberEnd() && m->value.GetType() == kStringType) {
                localuri = UriType(m->value, allocator_).Resolve(baseuri, allocator_);
            }
            // Посмотрите, совпадает ли оно
            if (localuri.Match(finduri, full)) {
                RAPIDJSON_SCHEMA_PRINT(Method, "GenericSchemaDocument::FindId (match)", full ? localuri.GetString() : localuri.GetBaseString());
                resval = const_cast<ValueType *>(&doc);
                resptr = here;
                return resval;
            }
            // Нет совпадений, продолжайте поиск
            for (m = doc.MemberBegin(); m != doc.MemberEnd(); ++m) {
                if (m->value.GetType() == kObjectType || m->value.GetType() == kArrayType) {
                    resval = FindId(m->value, finduri, resptr, localuri, full, here.Append(m->name.GetString(), m->name.GetStringLength(), allocator_));
                }
                if (resval) break;
            }
        } else if (doc.GetType() == kArrayType) {
            // Продолжить поиск
            for (typename ValueType::ConstValueIterator v = doc.Begin(); v != doc.End(); ++v) {
                if (v->GetType() == kObjectType || v->GetType() == kArrayType) {
                    resval = FindId(*v, finduri, resptr, localuri, full, here.Append(i, allocator_));
                }
                if (resval) break;
                i++;
            }
        }
        return resval;
    }

    // Добавил PR #1393
    void AddSchemaRefs(SchemaType* schema) {
        RAPIDJSON_SCHEMA_PRINT(Method, "GenericSchemaDocument::AddSchemaRefs");
        while (!schemaRef_.Empty()) {
            SchemaRefPtr *ref = schemaRef_.template Pop<SchemaRefPtr>(1);
            SchemaEntry *entry = schemaMap_.template Push<SchemaEntry>();
            new (entry) SchemaEntry(**ref, schema, false, allocator_);
        }
    }

    // Добавил PR #1393
    bool IsCyclicRef(const PointerType& pointer) const {
        for (const SchemaRefPtr* ref = schemaRef_.template Bottom<SchemaRefPtr>(); ref != schemaRef_.template End<SchemaRefPtr>(); ++ref)
            if (pointer == **ref)
                return true;
        return false;
    }

    const SchemaType* GetSchema(const PointerType& pointer) const {
        for (const SchemaEntry* target = schemaMap_.template Bottom<SchemaEntry>(); target != schemaMap_.template End<SchemaEntry>(); ++target)
            if (pointer == target->pointer)
                return target->schema;
        return 0;
    }

    PointerType GetPointer(const SchemaType* schema) const {
        for (const SchemaEntry* target = schemaMap_.template Bottom<SchemaEntry>(); target != schemaMap_.template End<SchemaEntry>(); ++target)
            if (schema == target->schema)
                return target->pointer;
        return PointerType();
    }

    const SchemaType* GetTypeless() const { return typeless_; }

    static const size_t kInitialSchemaMapSize = 64;
    static const size_t kInitialSchemaRefSize = 64;

    IRemoteSchemaDocumentProviderType* remoteProvider_;
    Allocator *allocator_;
    Allocator *ownAllocator_;
    const SchemaType* root_;                //!< Корневая схема.
    SchemaType* typeless_;
    internal::Stack<Allocator> schemaMap_;  // Созданные магазины Указатель -> Схемы
    internal::Stack<Allocator> schemaRef_;  // Сохраняет указатели из $ref(s) до тех пор, пока они не будут решены.
    GValue uri_;                            // Документ схемы URI
    UriType docId_;
    Specification spec_;
    GValue error_;
    GValue currentError_;
};

//! GenericSchemaDocument с использованием типа Value.
typedef GenericSchemaDocument<Value> SchemaDocument;
//! IGenericRemoteSchemaDocumentProvider с использованием SchemaDocument.
typedef IGenericRemoteSchemaDocumentProvider<SchemaDocument> IRemoteSchemaDocumentProvider;

///////////////////////////////////////////////////////////////////////////////
// GenericSchemaValidator

//!  JSON Валидатор схемы.
/*!
    Валидатор схемы JSON в стиле SAX.
    Он использует \c GenericSchemaDocument для проверки событий SAX.
    Он делегирует входящие события SAX обработчику вывода.
    Обработчик вывода по умолчанию ничего не делает.
    Его можно использовать повторно несколько раз, вызвав \cReset().

    \tparam SchemaDocumentType Тип документа схемы.
    \tparam OutputHandler Тип обработчика результата. Обработчик по умолчанию ничего не делает.
    \tparam StateAllocator Распределитель для хранения внутренних проверок.
*/
template <
    typename SchemaDocumentType,
    typename OutputHandler = BaseReaderHandler<typename SchemaDocumentType::SchemaType::EncodingType>,
    typename StateAllocator = CrtAllocator>
class GenericSchemaValidator :
    public internal::ISchemaStateFactory<typename SchemaDocumentType::SchemaType>,
    public internal::ISchemaValidator,
    public internal::IValidationErrorHandler<typename SchemaDocumentType::SchemaType> {
public:
    typedef typename SchemaDocumentType::SchemaType SchemaType;
    typedef typename SchemaDocumentType::PointerType PointerType;
    typedef typename SchemaType::EncodingType EncodingType;
    typedef typename SchemaType::SValue SValue;
    typedef typename EncodingType::Ch Ch;
    typedef GenericStringRef<Ch> StringRefType;
    typedef GenericValue<EncodingType, StateAllocator> ValueType;

    //! Конструктор без обработчика вывода.
    /*!
        \param SchemaDocument Документ схемы, который необходимо соблюдать.
        \param allocator Необязательный распределитель для хранения внутренних проверок.
        \param SchemaStackCapacity Необязательная начальная емкость схемы путей стека.
        \param documentStackCapacity Необязательная начальная емкость стека путей к документу.
    */
    GenericSchemaValidator(
        const SchemaDocumentType& schemaDocument,
        StateAllocator* allocator = 0,
        size_t schemaStackCapacity = kDefaultSchemaStackCapacity,
        size_t documentStackCapacity = kDefaultDocumentStackCapacity)
        :
        schemaDocument_(&schemaDocument),
        root_(schemaDocument.GetRoot()),
        stateAllocator_(allocator),
        ownStateAllocator_(0),
        schemaStack_(allocator, schemaStackCapacity),
        documentStack_(allocator, documentStackCapacity),
        outputHandler_(0),
        error_(kObjectType),
        currentError_(),
        missingDependents_(),
        valid_(true),
        flags_(kValidateDefaultFlags),
        depth_(0)
    {
        RAPIDJSON_SCHEMA_PRINT(Method, "GenericSchemaValidator::GenericSchemaValidator");
    }

    //! Конструктор с обработчиком вывода.
    /*!
        \param SchemaDocument Документ схемы, который необходимо соблюдать.
        \param allocator Необязательный распределитель для хранения внутренних проверок.
        \param SchemaStackCapacity Необязательная начальная емкость схемы путей стека.
        \param documentStackCapacity Необязательная начальная емкость стека путей к документу.
    */
    GenericSchemaValidator(
        const SchemaDocumentType& schemaDocument,
        OutputHandler& outputHandler,
        StateAllocator* allocator = 0,
        size_t schemaStackCapacity = kDefaultSchemaStackCapacity,
        size_t documentStackCapacity = kDefaultDocumentStackCapacity)
        :
        schemaDocument_(&schemaDocument),
        root_(schemaDocument.GetRoot()),
        stateAllocator_(allocator),
        ownStateAllocator_(0),
        schemaStack_(allocator, schemaStackCapacity),
        documentStack_(allocator, documentStackCapacity),
        outputHandler_(&outputHandler),
        error_(kObjectType),
        currentError_(),
        missingDependents_(),
        valid_(true),
        flags_(kValidateDefaultFlags),
        depth_(0)
    {
        RAPIDJSON_SCHEMA_PRINT(Method, "GenericSchemaValidator::GenericSchemaValidator (output handler)");
    }

    //! Деструктор.
    ~GenericSchemaValidator() {
        Reset();
        RAPIDJSON_DELETE(ownStateAllocator_);
    }

    //! Сброс внутренних состояний.
    void Reset() {
        while (!schemaStack_.Empty())
            PopSchema();
        documentStack_.Clear();
        ResetError();
    }

    //! Сбросьте состояние ошибки.
    void ResetError() {
        error_.SetObject();
        currentError_.SetNull();
        missingDependents_.SetNull();
        valid_ = true;
    }

    //! Реализация ISchemaValidator
    void SetValidateFlags(unsigned flags) {
        flags_ = flags;
    }
    virtual unsigned GetValidateFlags() const {
        return flags_;
    }

    virtual bool IsValid() const {
        if (!valid_) return false;
        if (GetContinueOnErrors() && !error_.ObjectEmpty()) return false;
        return true;
    }
    //! Окончание реализации ISchemaValidator

    //! Получает объект ошибки.
    ValueType& GetError() { return error_; }
    const ValueType& GetError() const { return error_; }

    //! Получает указатель JSON, указывающий на недопустимую схему.
    //  Если сообщается обо всех ошибках, стек будет пуст.
    PointerType GetInvalidSchemaPointer() const {
        return schemaStack_.Empty() ? PointerType() : CurrentSchema().GetPointer();
    }

    //! Получает ключевое слово недопустимой схемы.
    //  Если вы сообщаете обо всех ошибках, стек будет пуст, поэтому возвращайте «ошибки».
    const Ch* GetInvalidSchemaKeyword() const {
        if (!schemaStack_.Empty()) return CurrentContext().invalidKeyword;
        if (GetContinueOnErrors() && !error_.ObjectEmpty()) return (const Ch*)GetErrorsString();
        return 0;
    }

    //! Получает код ошибки недопустимой схемы.
    //  Если вы сообщаете обо всех ошибках, стек будет пуст, поэтому верните kValidateErrors.
    ValidateErrorCode GetInvalidSchemaCode() const {
        if (!schemaStack_.Empty()) return CurrentContext().invalidCode;
        if (GetContinueOnErrors() && !error_.ObjectEmpty()) return kValidateErrors;
        return kValidateErrorNone;
    }

    //! Получает указатель JSON, указывающий на недопустимое значение.
    //  Если сообщается обо всех ошибках, стек будет пуст.
    PointerType GetInvalidDocumentPointer() const {
        if (documentStack_.Empty()) {
            return PointerType();
        }
        else {
            return PointerType(documentStack_.template Bottom<Ch>(), documentStack_.GetSize() / sizeof(Ch));
        }
    }

    void NotMultipleOf(int64_t actual, const SValue& expected) {
        AddNumberError(kValidateErrorMultipleOf, ValueType(actual).Move(), expected);
    }
    void NotMultipleOf(uint64_t actual, const SValue& expected) {
        AddNumberError(kValidateErrorMultipleOf, ValueType(actual).Move(), expected);
    }
    void NotMultipleOf(double actual, const SValue& expected) {
        AddNumberError(kValidateErrorMultipleOf, ValueType(actual).Move(), expected);
    }
    void AboveMaximum(int64_t actual, const SValue& expected, bool exclusive) {
        AddNumberError(exclusive ? kValidateErrorExclusiveMaximum : kValidateErrorMaximum, ValueType(actual).Move(), expected,
            exclusive ? &SchemaType::GetExclusiveMaximumString : 0);
    }
    void AboveMaximum(uint64_t actual, const SValue& expected, bool exclusive) {
        AddNumberError(exclusive ? kValidateErrorExclusiveMaximum : kValidateErrorMaximum, ValueType(actual).Move(), expected,
            exclusive ? &SchemaType::GetExclusiveMaximumString : 0);
    }
    void AboveMaximum(double actual, const SValue& expected, bool exclusive) {
        AddNumberError(exclusive ? kValidateErrorExclusiveMaximum : kValidateErrorMaximum, ValueType(actual).Move(), expected,
            exclusive ? &SchemaType::GetExclusiveMaximumString : 0);
    }
    void BelowMinimum(int64_t actual, const SValue& expected, bool exclusive) {
        AddNumberError(exclusive ? kValidateErrorExclusiveMinimum : kValidateErrorMinimum, ValueType(actual).Move(), expected,
            exclusive ? &SchemaType::GetExclusiveMinimumString : 0);
    }
    void BelowMinimum(uint64_t actual, const SValue& expected, bool exclusive) {
        AddNumberError(exclusive ? kValidateErrorExclusiveMinimum : kValidateErrorMinimum, ValueType(actual).Move(), expected,
            exclusive ? &SchemaType::GetExclusiveMinimumString : 0);
    }
    void BelowMinimum(double actual, const SValue& expected, bool exclusive) {
        AddNumberError(exclusive ? kValidateErrorExclusiveMinimum : kValidateErrorMinimum, ValueType(actual).Move(), expected,
            exclusive ? &SchemaType::GetExclusiveMinimumString : 0);
    }

    void TooLong(const Ch* str, SizeType length, SizeType expected) {
        AddNumberError(kValidateErrorMaxLength,
            ValueType(str, length, GetStateAllocator()).Move(), SValue(expected).Move());
    }
    void TooShort(const Ch* str, SizeType length, SizeType expected) {
        AddNumberError(kValidateErrorMinLength,
            ValueType(str, length, GetStateAllocator()).Move(), SValue(expected).Move());
    }
    void DoesNotMatch(const Ch* str, SizeType length) {
        currentError_.SetObject();
        currentError_.AddMember(GetActualString(), ValueType(str, length, GetStateAllocator()).Move(), GetStateAllocator());
        AddCurrentError(kValidateErrorPattern);
    }

    void DisallowedItem(SizeType index) {
        currentError_.SetObject();
        currentError_.AddMember(GetDisallowedString(), ValueType(index).Move(), GetStateAllocator());
        AddCurrentError(kValidateErrorAdditionalItems, true);
    }
    void TooFewItems(SizeType actualCount, SizeType expectedCount) {
        AddNumberError(kValidateErrorMinItems,
            ValueType(actualCount).Move(), SValue(expectedCount).Move());
    }
    void TooManyItems(SizeType actualCount, SizeType expectedCount) {
        AddNumberError(kValidateErrorMaxItems,
            ValueType(actualCount).Move(), SValue(expectedCount).Move());
    }
    void DuplicateItems(SizeType index1, SizeType index2) {
        ValueType duplicates(kArrayType);
        duplicates.PushBack(index1, GetStateAllocator());
        duplicates.PushBack(index2, GetStateAllocator());
        currentError_.SetObject();
        currentError_.AddMember(GetDuplicatesString(), duplicates, GetStateAllocator());
        AddCurrentError(kValidateErrorUniqueItems, true);
    }

    void TooManyProperties(SizeType actualCount, SizeType expectedCount) {
        AddNumberError(kValidateErrorMaxProperties,
            ValueType(actualCount).Move(), SValue(expectedCount).Move());
    }
    void TooFewProperties(SizeType actualCount, SizeType expectedCount) {
        AddNumberError(kValidateErrorMinProperties,
            ValueType(actualCount).Move(), SValue(expectedCount).Move());
    }
    void StartMissingProperties() {
        currentError_.SetArray();
    }
    void AddMissingProperty(const SValue& name) {
        currentError_.PushBack(ValueType(name, GetStateAllocator()).Move(), GetStateAllocator());
    }
    bool EndMissingProperties() {
        if (currentError_.Empty())
            return false;
        ValueType error(kObjectType);
        error.AddMember(GetMissingString(), currentError_, GetStateAllocator());
        currentError_ = error;
        AddCurrentError(kValidateErrorRequired);
        return true;
    }
    void PropertyViolations(ISchemaValidator** subvalidators, SizeType count) {
        for (SizeType i = 0; i < count; ++i)
            MergeError(static_cast<GenericSchemaValidator*>(subvalidators[i])->GetError());
    }
    void DisallowedProperty(const Ch* name, SizeType length) {
        currentError_.SetObject();
        currentError_.AddMember(GetDisallowedString(), ValueType(name, length, GetStateAllocator()).Move(), GetStateAllocator());
        AddCurrentError(kValidateErrorAdditionalProperties, true);
    }

    void StartDependencyErrors() {
        currentError_.SetObject();
    }
    void StartMissingDependentProperties() {
        missingDependents_.SetArray();
    }
    void AddMissingDependentProperty(const SValue& targetName) {
        missingDependents_.PushBack(ValueType(targetName, GetStateAllocator()).Move(), GetStateAllocator());
    }
    void EndMissingDependentProperties(const SValue& sourceName) {
        if (!missingDependents_.Empty()) {
            // Создайте эквивалентную «требуемую» ошибку
            ValueType error(kObjectType);
            ValidateErrorCode code = kValidateErrorRequired;
            error.AddMember(GetMissingString(), missingDependents_.Move(), GetStateAllocator());
            AddErrorCode(error, code);
            AddErrorInstanceLocation(error, false);
            // При добавлении к указателю убедитесь, что используется его распределитель.
            PointerType schemaRef = GetInvalidSchemaPointer().Append(SchemaType::GetValidateErrorKeyword(kValidateErrorDependencies), &GetInvalidSchemaPointer().GetAllocator());
            AddErrorSchemaLocation(error, schemaRef.Append(sourceName.GetString(), sourceName.GetStringLength(), &GetInvalidSchemaPointer().GetAllocator()));
            ValueType wrapper(kObjectType);
            wrapper.AddMember(ValueType(SchemaType::GetValidateErrorKeyword(code), GetStateAllocator()).Move(), error, GetStateAllocator());
            currentError_.AddMember(ValueType(sourceName, GetStateAllocator()).Move(), wrapper, GetStateAllocator());
        }
    }
    void AddDependencySchemaError(const SValue& sourceName, ISchemaValidator* subvalidator) {
        currentError_.AddMember(ValueType(sourceName, GetStateAllocator()).Move(),
            static_cast<GenericSchemaValidator*>(subvalidator)->GetError(), GetStateAllocator());
    }
    bool EndDependencyErrors() {
        if (currentError_.ObjectEmpty())
            return false;
        ValueType error(kObjectType);
        error.AddMember(GetErrorsString(), currentError_, GetStateAllocator());
        currentError_ = error;
        AddCurrentError(kValidateErrorDependencies);
        return true;
    }

    void DisallowedValue(const ValidateErrorCode code = kValidateErrorEnum) {
        currentError_.SetObject();
        AddCurrentError(code);
    }
    void StartDisallowedType() {
        currentError_.SetArray();
    }
    void AddExpectedType(const typename SchemaType::ValueType& expectedType) {
        currentError_.PushBack(ValueType(expectedType, GetStateAllocator()).Move(), GetStateAllocator());
    }
    void EndDisallowedType(const typename SchemaType::ValueType& actualType) {
        ValueType error(kObjectType);
        error.AddMember(GetExpectedString(), currentError_, GetStateAllocator());
        error.AddMember(GetActualString(), ValueType(actualType, GetStateAllocator()).Move(), GetStateAllocator());
        currentError_ = error;
        AddCurrentError(kValidateErrorType);
    }
    void NotAllOf(ISchemaValidator** subvalidators, SizeType count) {
        // Рассматриваем allOf как oneOf и AnyOf для соответствия https://rapidjson.org/md_doc_schema.html#allOf-anyOf-oneOf
        AddErrorArray(kValidateErrorAllOf, subvalidators, count);
        //for (SizeType я = 0; я <количество; ++i) {
        //    MergeError( static_cast <GenericSchemaValidator*>(субвалидаторы[i])-> GetError() );
        //}
    }
    void NoneOf(ISchemaValidator** subvalidators, SizeType count) {
        AddErrorArray(kValidateErrorAnyOf, subvalidators, count);
    }
    void NotOneOf(ISchemaValidator** subvalidators, SizeType count) {
        AddErrorArray(kValidateErrorOneOf, subvalidators, count);
    }
    void MultipleOneOf(SizeType index1, SizeType index2) {
        ValueType matches(kArrayType);
        matches.PushBack(index1, GetStateAllocator());
        matches.PushBack(index2, GetStateAllocator());
        currentError_.SetObject();
        currentError_.AddMember(GetMatchesString(), matches, GetStateAllocator());
        AddCurrentError(kValidateErrorOneOfMatch);
    }
    void Disallowed() {
        currentError_.SetObject();
        AddCurrentError(kValidateErrorNot);
    }
    void DisallowedWhenWriting() {
        currentError_.SetObject();
        AddCurrentError(kValidateErrorReadOnly);
    }
    void DisallowedWhenReading() {
        currentError_.SetObject();
        AddCurrentError(kValidateErrorWriteOnly);
    }

#define RAPIDJSON_STRING_(name, ...) \
    static const StringRefType& Get##name##String() {\
        static const Ch s[] = { __VA_ARGS__, '\0' };\
        static const StringRefType v(s, static_cast<SizeType>(sizeof(s) / sizeof(Ch) - 1)); \
        return v;\
    }

    RAPIDJSON_STRING_(InstanceRef, 'i', 'n', 's', 't', 'a', 'n', 'c', 'e', 'R', 'e', 'f')
    RAPIDJSON_STRING_(SchemaRef, 's', 'c', 'h', 'e', 'm', 'a', 'R', 'e', 'f')
    RAPIDJSON_STRING_(Expected, 'e', 'x', 'p', 'e', 'c', 't', 'e', 'd')
    RAPIDJSON_STRING_(Actual, 'a', 'c', 't', 'u', 'a', 'l')
    RAPIDJSON_STRING_(Disallowed, 'd', 'i', 's', 'a', 'l', 'l', 'o', 'w', 'e', 'd')
    RAPIDJSON_STRING_(Missing, 'm', 'i', 's', 's', 'i', 'n', 'g')
    RAPIDJSON_STRING_(Errors, 'e', 'r', 'r', 'o', 'r', 's')
    RAPIDJSON_STRING_(ErrorCode, 'e', 'r', 'r', 'o', 'r', 'C', 'o', 'd', 'e')
    RAPIDJSON_STRING_(ErrorMessage, 'e', 'r', 'r', 'o', 'r', 'M', 'e', 's', 's', 'a', 'g', 'e')
    RAPIDJSON_STRING_(Duplicates, 'd', 'u', 'p', 'l', 'i', 'c', 'a', 't', 'e', 's')
    RAPIDJSON_STRING_(Matches, 'm', 'a', 't', 'c', 'h', 'e', 's')

#undef RAPIDJSON_STRING_

#define RAPIDJSON_SCHEMA_HANDLE_BEGIN_(method, arg1)\
    if (!valid_) return false; \
    if ((!BeginValue() && !GetContinueOnErrors()) || (!CurrentSchema().method arg1 && !GetContinueOnErrors())) {\
        *documentStack_.template Push<Ch>() = '\0';\
        documentStack_.template Pop<Ch>(1);\
        RAPIDJSON_SCHEMA_PRINT(InvalidDocument, documentStack_.template Bottom<Ch>());\
        valid_ = false;\
        return valid_;\
    }

#define RAPIDJSON_SCHEMA_HANDLE_PARALLEL_(method, arg2)\
    for (Context* context = schemaStack_.template Bottom<Context>(); context != schemaStack_.template End<Context>(); context++) {\
        if (context->hasher)\
            static_cast<HasherType*>(context->hasher)->method arg2;\
        if (context->validators)\
            for (SizeType i_ = 0; i_ < context->validatorCount; i_++)\
                static_cast<GenericSchemaValidator*>(context->validators[i_])->method arg2;\
        if (context->patternPropertiesValidators)\
            for (SizeType i_ = 0; i_ < context->patternPropertiesValidatorCount; i_++)\
                static_cast<GenericSchemaValidator*>(context->patternPropertiesValidators[i_])->method arg2;\
    }

#define RAPIDJSON_SCHEMA_HANDLE_END_(method, arg2)\
    valid_ = (EndValue() || GetContinueOnErrors()) && (!outputHandler_ || outputHandler_->method arg2);\
    return valid_;

#define RAPIDJSON_SCHEMA_HANDLE_VALUE_(method, arg1, arg2) \
    RAPIDJSON_SCHEMA_HANDLE_BEGIN_   (method, arg1);\
    RAPIDJSON_SCHEMA_HANDLE_PARALLEL_(method, arg2);\
    RAPIDJSON_SCHEMA_HANDLE_END_     (method, arg2)

    bool Null()             { RAPIDJSON_SCHEMA_HANDLE_VALUE_(Null,   (CurrentContext()), ( )); }
    bool Bool(bool b)       { RAPIDJSON_SCHEMA_HANDLE_VALUE_(Bool,   (CurrentContext(), b), (b)); }
    bool Int(int i)         { RAPIDJSON_SCHEMA_HANDLE_VALUE_(Int,    (CurrentContext(), i), (i)); }
    bool Uint(unsigned u)   { RAPIDJSON_SCHEMA_HANDLE_VALUE_(Uint,   (CurrentContext(), u), (u)); }
    bool Int64(int64_t i)   { RAPIDJSON_SCHEMA_HANDLE_VALUE_(Int64,  (CurrentContext(), i), (i)); }
    bool Uint64(uint64_t u) { RAPIDJSON_SCHEMA_HANDLE_VALUE_(Uint64, (CurrentContext(), u), (u)); }
    bool Double(double d)   { RAPIDJSON_SCHEMA_HANDLE_VALUE_(Double, (CurrentContext(), d), (d)); }
    bool RawNumber(const Ch* str, SizeType length, bool copy)
                                    { RAPIDJSON_SCHEMA_HANDLE_VALUE_(String, (CurrentContext(), str, length, copy), (str, length, copy)); }
    bool String(const Ch* str, SizeType length, bool copy)
                                    { RAPIDJSON_SCHEMA_HANDLE_VALUE_(String, (CurrentContext(), str, length, copy), (str, length, copy)); }

    bool StartObject() {
        RAPIDJSON_SCHEMA_PRINT(Method, "GenericSchemaValidator::StartObject");
        RAPIDJSON_SCHEMA_HANDLE_BEGIN_(StartObject, (CurrentContext()));
        RAPIDJSON_SCHEMA_HANDLE_PARALLEL_(StartObject, ());
        valid_ = !outputHandler_ || outputHandler_->StartObject();
        return valid_;
    }

    bool Key(const Ch* str, SizeType len, bool copy) {
        RAPIDJSON_SCHEMA_PRINT(Method, "GenericSchemaValidator::Key", str);
        if (!valid_) return false;
        AppendToken(str, len);
        if (!CurrentSchema().Key(CurrentContext(), str, len, copy) && !GetContinueOnErrors()) {
            valid_ = false;
            return valid_;
        }
        RAPIDJSON_SCHEMA_HANDLE_PARALLEL_(Key, (str, len, copy));
        valid_ = !outputHandler_ || outputHandler_->Key(str, len, copy);
        return valid_;
    }

    bool EndObject(SizeType memberCount) {
        RAPIDJSON_SCHEMA_PRINT(Method, "GenericSchemaValidator::EndObject");
        if (!valid_) return false;
        RAPIDJSON_SCHEMA_HANDLE_PARALLEL_(EndObject, (memberCount));
        if (!CurrentSchema().EndObject(CurrentContext(), memberCount) && !GetContinueOnErrors()) {
            valid_ = false;
            return valid_;
        }
        RAPIDJSON_SCHEMA_HANDLE_END_(EndObject, (memberCount));
    }

    bool StartArray() {
        RAPIDJSON_SCHEMA_PRINT(Method, "GenericSchemaValidator::StartArray");
        RAPIDJSON_SCHEMA_HANDLE_BEGIN_(StartArray, (CurrentContext()));
        RAPIDJSON_SCHEMA_HANDLE_PARALLEL_(StartArray, ());
        valid_ = !outputHandler_ || outputHandler_->StartArray();
        return valid_;
    }

    bool EndArray(SizeType elementCount) {
        RAPIDJSON_SCHEMA_PRINT(Method, "GenericSchemaValidator::EndArray");
        if (!valid_) return false;
        RAPIDJSON_SCHEMA_HANDLE_PARALLEL_(EndArray, (elementCount));
        if (!CurrentSchema().EndArray(CurrentContext(), elementCount) && !GetContinueOnErrors()) {
            valid_ = false;
            return valid_;
        }
        RAPIDJSON_SCHEMA_HANDLE_END_(EndArray, (elementCount));
    }

#undef RAPIDJSON_SCHEMA_HANDLE_BEGIN_
#undef RAPIDJSON_SCHEMA_HANDLE_PARALLEL_
#undef RAPIDJSON_SCHEMA_HANDLE_VALUE_

    // Реализация ISchemaStateFactory<SchemaType>
    virtual ISchemaValidator* CreateSchemaValidator(const SchemaType& root, const bool inheritContinueOnErrors) {
        *documentStack_.template Push<Ch>() = '\0';
        documentStack_.template Pop<Ch>(1);
        ISchemaValidator* sv = new (GetStateAllocator().Malloc(sizeof(GenericSchemaValidator))) GenericSchemaValidator(*schemaDocument_, root, documentStack_.template Bottom<char>(), documentStack_.GetSize(),
        depth_ + 1,
        &GetStateAllocator());
        sv->SetValidateFlags(inheritContinueOnErrors ? GetValidateFlags() : GetValidateFlags() & ~(unsigned)kValidateContinueOnErrorFlag);
        return sv;
    }

    virtual void DestroySchemaValidator(ISchemaValidator* validator) {
        GenericSchemaValidator* v = static_cast<GenericSchemaValidator*>(validator);
        v->~GenericSchemaValidator();
        StateAllocator::Free(v);
    }

    virtual void* CreateHasher() {
        return new (GetStateAllocator().Malloc(sizeof(HasherType))) HasherType(&GetStateAllocator());
    }

    virtual uint64_t GetHashCode(void* hasher) {
        return static_cast<HasherType*>(hasher)->GetHashCode();
    }

    virtual void DestroyHasher(void* hasher) {
        HasherType* h = static_cast<HasherType*>(hasher);
        h->~HasherType();
        StateAllocator::Free(h);
    }

    virtual void* MallocState(size_t size) {
        return GetStateAllocator().Malloc(size);
    }

    virtual void FreeState(void* p) {
        StateAllocator::Free(p);
    }
    // Окончание реализации ISchemaStateFactory<SchemaType>

private:
    typedef typename SchemaType::Context Context;
    typedef GenericValue<UTF8<>, StateAllocator> HashCodeArray;
    typedef internal::Hasher<EncodingType, StateAllocator> HasherType;

    GenericSchemaValidator(
        const SchemaDocumentType& schemaDocument,
        const SchemaType& root,
        const char* basePath, size_t basePathSize,
        unsigned depth,
        StateAllocator* allocator = 0,
        size_t schemaStackCapacity = kDefaultSchemaStackCapacity,
        size_t documentStackCapacity = kDefaultDocumentStackCapacity)
        :
        schemaDocument_(&schemaDocument),
        root_(root),
        stateAllocator_(allocator),
        ownStateAllocator_(0),
        schemaStack_(allocator, schemaStackCapacity),
        documentStack_(allocator, documentStackCapacity),
        outputHandler_(0),
        error_(kObjectType),
        currentError_(),
        missingDependents_(),
        valid_(true),
        flags_(kValidateDefaultFlags),
        depth_(depth)
    {
        RAPIDJSON_SCHEMA_PRINT(Method, "GenericSchemaValidator::GenericSchemaValidator (internal)", basePath && basePathSize ? basePath : "");
        if (basePath && basePathSize)
            memcpy(documentStack_.template Push<char>(basePathSize), basePath, basePathSize);
    }

    StateAllocator& GetStateAllocator() {
        if (!stateAllocator_)
            stateAllocator_ = ownStateAllocator_ = RAPIDJSON_NEW(StateAllocator)();
        return *stateAllocator_;
    }

    bool GetContinueOnErrors() const {
        return flags_ & kValidateContinueOnErrorFlag;
    }

    bool BeginValue() {
        RAPIDJSON_SCHEMA_PRINT(Method, "GenericSchemaValidator::BeginValue");
        if (schemaStack_.Empty())
            PushSchema(root_);
        else {
            if (CurrentContext().inArray)
                internal::TokenHelper<internal::Stack<StateAllocator>, Ch>::AppendIndexToken(documentStack_, CurrentContext().arrayElementIndex);

            if (!CurrentSchema().BeginValue(CurrentContext()) && !GetContinueOnErrors())
                return false;

            SizeType count = CurrentContext().patternPropertiesSchemaCount;
            const SchemaType** sa = CurrentContext().patternPropertiesSchemas;
            typename Context::PatternValidatorType patternValidatorType = CurrentContext().valuePatternValidatorType;
            bool valueUniqueness = CurrentContext().valueUniqueness;
            RAPIDJSON_ASSERT(CurrentContext().valueSchema);
            PushSchema(*CurrentContext().valueSchema);

            if (count > 0) {
                CurrentContext().objectPatternValidatorType = patternValidatorType;
                ISchemaValidator**& va = CurrentContext().patternPropertiesValidators;
                SizeType& validatorCount = CurrentContext().patternPropertiesValidatorCount;
                va = static_cast<ISchemaValidator**>(MallocState(sizeof(ISchemaValidator*) * count));
                std::memset(va, 0, sizeof(ISchemaValidator*) * count);
                for (SizeType i = 0; i < count; i++)
                    va[validatorCount++] = CreateSchemaValidator(*sa[i], true);  // Наблюдение за continueOnError
            }

            CurrentContext().arrayUniqueness = valueUniqueness;
        }
        return true;
    }

    bool EndValue() {
        RAPIDJSON_SCHEMA_PRINT(Method, "GenericSchemaValidator::EndValue");
        if (!CurrentSchema().EndValue(CurrentContext()) && !GetContinueOnErrors())
            return false;

        GenericStringBuffer<EncodingType> sb;
        schemaDocument_->GetPointer(&CurrentSchema()).StringifyUriFragment(sb);
        *documentStack_.template Push<Ch>() = '\0';
        documentStack_.template Pop<Ch>(1);
        RAPIDJSON_SCHEMA_PRINT(ValidatorPointers, sb.GetString(), documentStack_.template Bottom<Ch>(), depth_);
        void* hasher = CurrentContext().hasher;
        uint64_t h = hasher && CurrentContext().arrayUniqueness ? static_cast<HasherType*>(hasher)->GetHashCode() : 0;

        PopSchema();

        if (!schemaStack_.Empty()) {
            Context& context = CurrentContext();
            // Проверяйте уникальность только при наличии хэшера
            if (hasher && context.valueUniqueness) {
                HashCodeArray* a = static_cast<HashCodeArray*>(context.arrayElementHashCodes);
                if (!a)
                    CurrentContext().arrayElementHashCodes = a = new (GetStateAllocator().Malloc(sizeof(HashCodeArray))) HashCodeArray(kArrayType);
                for (typename HashCodeArray::ConstValueIterator itr = a->Begin(); itr != a->End(); ++itr)
                    if (itr->GetUint64() == h) {
                        DuplicateItems(static_cast<SizeType>(itr - a->Begin()), a->Size());
                        // Очистка перед возвратом, если продолжите
                        if (GetContinueOnErrors()) {
                            a->PushBack(h, GetStateAllocator());
                            while (!documentStack_.Empty() && *documentStack_.template Pop<Ch>(1) != '/');
                        }
                        RAPIDJSON_INVALID_KEYWORD_RETURN(kValidateErrorUniqueItems);
                    }
                a->PushBack(h, GetStateAllocator());
            }
        }

        // Удалить последний токен указателя документа
        while (!documentStack_.Empty() && *documentStack_.template Pop<Ch>(1) != '/')
            ;

        return true;
    }

    void AppendToken(const Ch* str, SizeType len) {
        documentStack_.template Reserve<Ch>(1 + len * 2); // в худшем случае все символы экранируются как два символа
        *documentStack_.template PushUnsafe<Ch>() = '/';
        for (SizeType i = 0; i < len; i++) {
            if (str[i] == '~') {
                *documentStack_.template PushUnsafe<Ch>() = '~';
                *documentStack_.template PushUnsafe<Ch>() = '0';
            }
            else if (str[i] == '/') {
                *documentStack_.template PushUnsafe<Ch>() = '~';
                *documentStack_.template PushUnsafe<Ch>() = '1';
            }
            else
                *documentStack_.template PushUnsafe<Ch>() = str[i];
        }
    }

    RAPIDJSON_FORCEINLINE void PushSchema(const SchemaType& schema) { new (schemaStack_.template Push<Context>()) Context(*this, *this, &schema, flags_); }

    RAPIDJSON_FORCEINLINE void PopSchema() {
        Context* c = schemaStack_.template Pop<Context>(1);
        if (HashCodeArray* a = static_cast<HashCodeArray*>(c->arrayElementHashCodes)) {
            a->~HashCodeArray();
            StateAllocator::Free(a);
        }
        c->~Context();
    }

    void AddErrorInstanceLocation(ValueType& result, bool parent) {
        GenericStringBuffer<EncodingType> sb;
        PointerType instancePointer = GetInvalidDocumentPointer();
        ((parent && instancePointer.GetTokenCount() > 0)
         ? PointerType(instancePointer.GetTokens(), instancePointer.GetTokenCount() - 1)
         : instancePointer).StringifyUriFragment(sb);
        ValueType instanceRef(sb.GetString(), static_cast<SizeType>(sb.GetSize() / sizeof(Ch)),
                              GetStateAllocator());
        result.AddMember(GetInstanceRefString(), instanceRef, GetStateAllocator());
    }

    void AddErrorSchemaLocation(ValueType& result, PointerType schema = PointerType()) {
        GenericStringBuffer<EncodingType> sb;
        SizeType len = CurrentSchema().GetURI().GetStringLength();
        if (len) memcpy(sb.Push(len), CurrentSchema().GetURI().GetString(), len * sizeof(Ch));
        if (schema.GetTokenCount()) schema.StringifyUriFragment(sb);
        else GetInvalidSchemaPointer().StringifyUriFragment(sb);
        ValueType schemaRef(sb.GetString(), static_cast<SizeType>(sb.GetSize() / sizeof(Ch)),
            GetStateAllocator());
        result.AddMember(GetSchemaRefString(), schemaRef, GetStateAllocator());
    }

    void AddErrorCode(ValueType& result, const ValidateErrorCode code) {
        result.AddMember(GetErrorCodeString(), code, GetStateAllocator());
    }

    void AddError(ValueType& keyword, ValueType& error) {
        typename ValueType::MemberIterator member = error_.FindMember(keyword);
        if (member == error_.MemberEnd())
            error_.AddMember(keyword, error, GetStateAllocator());
        else {
            if (member->value.IsObject()) {
                ValueType errors(kArrayType);
                errors.PushBack(member->value, GetStateAllocator());
                member->value = errors;
            }
            member->value.PushBack(error, GetStateAllocator());
        }
    }

    void AddCurrentError(const ValidateErrorCode code, bool parent = false) {
        AddErrorCode(currentError_, code);
        AddErrorInstanceLocation(currentError_, parent);
        AddErrorSchemaLocation(currentError_);
        AddError(ValueType(SchemaType::GetValidateErrorKeyword(code), GetStateAllocator(), false).Move(), currentError_);
    }

    void MergeError(ValueType& other) {
        for (typename ValueType::MemberIterator it = other.MemberBegin(), end = other.MemberEnd(); it != end; ++it) {
            AddError(it->name, it->value);
        }
    }

    void AddNumberError(const ValidateErrorCode code, ValueType& actual, const SValue& expected,
        const typename SchemaType::ValueType& (*exclusive)() = 0) {
        currentError_.SetObject();
        currentError_.AddMember(GetActualString(), actual, GetStateAllocator());
        currentError_.AddMember(GetExpectedString(), ValueType(expected, GetStateAllocator()).Move(), GetStateAllocator());
        if (exclusive)
            currentError_.AddMember(ValueType(exclusive(), GetStateAllocator()).Move(), true, GetStateAllocator());
        AddCurrentError(code);
    }

    void AddErrorArray(const ValidateErrorCode code,
        ISchemaValidator** subvalidators, SizeType count) {
        ValueType errors(kArrayType);
        for (SizeType i = 0; i < count; ++i)
            errors.PushBack(static_cast<GenericSchemaValidator*>(subvalidators[i])->GetError(), GetStateAllocator());
        currentError_.SetObject();
        currentError_.AddMember(GetErrorsString(), errors, GetStateAllocator());
        AddCurrentError(code);
    }

    const SchemaType& CurrentSchema() const { return *schemaStack_.template Top<Context>()->schema; }
    Context& CurrentContext() { return *schemaStack_.template Top<Context>(); }
    const Context& CurrentContext() const { return *schemaStack_.template Top<Context>(); }

    static const size_t kDefaultSchemaStackCapacity = 1024;
    static const size_t kDefaultDocumentStackCapacity = 256;
    const SchemaDocumentType* schemaDocument_;
    const SchemaType& root_;
    StateAllocator* stateAllocator_;
    StateAllocator* ownStateAllocator_;
    internal::Stack<StateAllocator> schemaStack_;    //!< стек для хранения текущего пути к схеме (BaseSchemaType *)
    internal::Stack<StateAllocator> documentStack_;  //!< стек для хранения текущего пути проверки документа (Ch)
    OutputHandler* outputHandler_;
    ValueType error_;
    ValueType currentError_;
    ValueType missingDependents_;
    bool valid_;
    unsigned flags_;
    unsigned depth_;
};

typedef GenericSchemaValidator<SchemaDocument> SchemaValidator;

///////////////////////////////////////////////////////////////////////////////
// СхемаValidatingReader

//! Вспомогательный класс для анализа с проверкой.
/*!
    Этот вспомогательный класс представляет собой функтор, созданный как параметр \ref GenericDocument::Populate().

    \tparam parseFlags Комбинация \ref ParseFlag.
    \tparam InputStream Тип входного потока, реализующий образующий поток.
    \tparam SourceEncoding Кодирование входного потока.
    \tparam SchemaDocumentType Тип документа схемы.
    \tparam StackAllocator Тип распределения для стека.
*/
template <
    unsigned parseFlags,
    typename InputStream,
    typename SourceEncoding,
    typename SchemaDocumentType = SchemaDocument,
    typename StackAllocator = CrtAllocator>
class SchemaValidatingReader {
public:
    typedef typename SchemaDocumentType::PointerType PointerType;
    typedef typename InputStream::Ch Ch;
    typedef GenericValue<SourceEncoding, StackAllocator> ValueType;

    //! Конструктор
    /*!
        \param — входной поток.
        \param sd Схема документа.
    */
    SchemaValidatingReader(InputStream& is, const SchemaDocumentType& sd) : is_(is), sd_(sd), invalidSchemaKeyword_(), invalidSchemaCode_(kValidateErrorNone), error_(kObjectType), isValid_(true) {}

    template <typename Handler>
    bool operator()(Handler& handler) {
        GenericReader<SourceEncoding, typename SchemaDocumentType::EncodingType, StackAllocator> reader;
        GenericSchemaValidator<SchemaDocumentType, Handler> validator(sd_, handler);
        parseResult_ = reader.template Parse<parseFlags>(is_, validator);

        isValid_ = validator.IsValid();
        if (isValid_) {
            invalidSchemaPointer_ = PointerType();
            invalidSchemaKeyword_ = 0;
            invalidDocumentPointer_ = PointerType();
            error_.SetObject();
        }
        else {
            invalidSchemaPointer_ = validator.GetInvalidSchemaPointer();
            invalidSchemaKeyword_ = validator.GetInvalidSchemaKeyword();
            invalidSchemaCode_ = validator.GetInvalidSchemaCode();
            invalidDocumentPointer_ = validator.GetInvalidDocumentPointer();
            error_.CopyFrom(validator.GetError(), allocator_);
        }

        return parseResult_;
    }

    const ParseResult& GetParseResult() const { return parseResult_; }
    bool IsValid() const { return isValid_; }
    const PointerType& GetInvalidSchemaPointer() const { return invalidSchemaPointer_; }
    const Ch* GetInvalidSchemaKeyword() const { return invalidSchemaKeyword_; }
    const PointerType& GetInvalidDocumentPointer() const { return invalidDocumentPointer_; }
    const ValueType& GetError() const { return error_; }
    ValidateErrorCode GetInvalidSchemaCode() const { return invalidSchemaCode_; }

private:
    InputStream& is_;
    const SchemaDocumentType& sd_;

    ParseResult parseResult_;
    PointerType invalidSchemaPointer_;
    const Ch* invalidSchemaKeyword_;
    PointerType invalidDocumentPointer_;
    ValidateErrorCode invalidSchemaCode_;
    StackAllocator allocator_;
    ValueType error_;
    bool isValid_;
};

RAPIDJSON_NAMESPACE_END
RAPIDJSON_DIAG_POP

#endif // RAPIDJSON_SCHEMA_H_
