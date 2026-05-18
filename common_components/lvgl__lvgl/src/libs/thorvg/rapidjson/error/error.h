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

#ifndef RAPIDJSON_ERROR_ERROR_H_
#define RAPIDJSON_ERROR_ERROR_H_

#include "../rapidjson.h"

#ifdef __clang__
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(padded)
#endif

/*! \file error.h */

/*! \defgroup RAPIDJSON_ERRORS RapidJSON error handling */

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_ERROR_CHARTYPE

//! Тип символов сообщений об ошибках.
/*! \ingroup RAPIDJSON_ERRORS
    Тип символа по умолчанию — \c char.
    В Windows пользователь может определить этот макрос как \cTCHARдля поддержки обоих.
    настройки юникода/не юникода.
*/
#ifndef RAPIDJSON_ERROR_CHARTYPE
#define RAPIDJSON_ERROR_CHARTYPE char
#endif

///////////////////////////////////////////////////////////////////////////////
// RAPIDJSON_ERROR_STRING

//! Макрос для преобразования строкового литерала в \refRAPIDJSON_ERROR_CHARTYPE[].
/*! \ingroup RAPIDJSON_ERRORS
    По умолчанию этот макрос преобразования ничего не делает.
    В Windows пользователь может определить этот макрос как \c _T(x) для поддержки обоих сторон.
    настройки юникода/не юникода.
*/
#ifndef RAPIDJSON_ERROR_STRING
#define RAPIDJSON_ERROR_STRING(x) x
#endif

RAPIDJSON_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
// Парсеерроркоде

//! Код ошибки парсинга.
/*! \ingroup RAPIDJSON_ERRORS
    \see GenericReader::Parse, GenericReader::GetParseErrorCode
*/
enum ParseErrorCode {
    kParseErrorNone = 0,                        //!< Нет ошибок.

    kParseErrorDocumentEmpty,                   //!< Документ пуст.
    kParseErrorDocumentRootNotSingular,         //!< За корнем документа не должны следовать другие значения.

    kParseErrorValueInvalid,                    //!< Недопустимое значение.

    kParseErrorObjectMissName,                  //!< Отсутствует имя члена объекта.
    kParseErrorObjectMissColon,                 //!< Отсутствует двоеточие после имени члена объекта.
    kParseErrorObjectMissCommaOrCurlyBracket,   //!< Отсутствует запятая или '}' после члена объекта.

    kParseErrorArrayMissCommaOrSquareBracket,   //!< Отсутствует запятая или ']' после элемента массива.

    kParseErrorStringUnicodeEscapeInvalidHex,   //!< Неверная шестнадцатеричная цифра после \\u escape в строке.
    kParseErrorStringUnicodeSurrogateInvalid,   //!< Недопустимая суррогатная пара в строке.
    kParseErrorStringEscapeInvalid,             //!< Недопустимый escape-символ в строке.
    kParseErrorStringMissQuotationMark,         //!< В строке отсутствует закрывающая кавычка.
    kParseErrorStringInvalidEncoding,           //!< Неверная кодировка в строке.

    kParseErrorNumberTooBig,                    //!< Число слишком велико для хранения в формате double.
    kParseErrorNumberMissFraction,              //!< Пропущена дробная часть в числе.
    kParseErrorNumberMissExponent,              //!< Недостающая экспонента в числе.

    kParseErrorTermination,                     //!< Анализ прекращен.
    kParseErrorUnspecificSyntaxError            //!< Неопределенная синтаксическая ошибка.
};

//! Анализ результата (обертывает ParseErrorCode)
/*!
    \ingroup RAPIDJSON_ERRORS
    \code
        Документ-документ;
        ParseResult ok = doc.Parse("[42]");
        if (!ok) {
            fprintf(stderr, "Ошибка анализаJSON: %s (%u)",
                    GetParseError_En (ок.Code()), ок. Offset());
            выход(EXIT_FAILURE);
        }
    \endcode
    \see GenericReader::Parse, GenericDocument::Parse
*/
struct ParseResult {
    //!! Неопределенный логический тип
    typedef bool (ParseResult::*BooleanType)() const;
public:
    //! Конструктор по умолчанию, без ошибок.
    ParseResult() : code_(kParseErrorNone), offset_(0) {}
    //! Конструктор для установки ошибки.
    ParseResult(ParseErrorCode code, size_t offset) : code_(code), offset_(offset) {}

    //! Получите код ошибки.
    ParseErrorCode Code() const { return code_; }
    //! Ошибка размещения, если \refIsError(), в противном случае 0.
    size_t Offset() const { return offset_; }

    //! Явное преобразование в \c bool возвращает \c true, iff !\refIsError().
    operator BooleanType() const { return !IsError() ? &ParseResult::IsError : NULL; }
    //! Является ли результат ошибкой.
    bool IsError() const { return code_ != kParseErrorNone; }

    bool operator==(const ParseResult& that) const { return code_ == that.code_; }
    bool operator==(ParseErrorCode code) const { return code_ == code; }
    friend bool operator==(ParseErrorCode code, const ParseResult & err) { return code == err.code_; }

    bool operator!=(const ParseResult& that) const { return !(*this == that); }
    bool operator!=(ParseErrorCode code) const { return !(*this == code); }
    friend bool operator!=(ParseErrorCode code, const ParseResult & err) { return err != code; }

    //! Сбросьте код ошибки.
    void Clear() { Set(kParseErrorNone); }
    //! Обновите код ошибки и смещение.
    void Set(ParseErrorCode code, size_t offset = 0) { code_ = code; offset_ = offset; }

private:
    ParseErrorCode code_;
    size_t offset_;
};

//! Тип указателя функцииGetParseError().
/*! \ingroup RAPIDJSON_ERRORS

    Это прототип \c GetParseError_X(), где \c X — локальный.
    Пользователь может динамически изменять локаль во время выполнения, например:
\code
    GetParseErrorFunc GetParseError =GetParseError_En; // или что-то еще
    constRAPIDJSON_ERROR_CHARTYPE* s = GetParseError(document. GetParseErrorCode());
\endcode
*/
typedef const RAPIDJSON_ERROR_CHARTYPE* (*GetParseErrorFunc)(ParseErrorCode);

///////////////////////////////////////////////////////////////////////////////
// Валидатеерроркоде

//! Коды ошибок при проверке.
/*! \ingroup RAPIDJSON_ERRORS
    \см. GenericSchemaValidator
*/
enum ValidateErrorCode {
    kValidateErrors    = -1,                   //!< Код ошибки верхнего уровня, когда установлен kValidateContinueOnErrorsFlag.
    kValidateErrorNone = 0,                    //!< Нет ошибок.

    kValidateErrorMultipleOf,                  //!< Число не кратно результат MultipleOf.
    kValidateErrorMaximum,                     //!< Число больше максимального значения.
    kValidateErrorExclusiveMaximum,            //!< Число больше или равно максимальному значению.
    kValidateErrorMinimum,                     //!< Число меньше минимального значения.
    kValidateErrorExclusiveMinimum,            //!< Число меньше или равно «минимальному» значению.

    kValidateErrorMaxLength,                   //!< Строка длиннее значения maxLength.
    kValidateErrorMinLength,                   //!< Строка длиннее значения maxLength.
    kValidateErrorPattern,                     //!< Строка не соответствует регулярному выражению «шаблон».

    kValidateErrorMaxItems,                    //!< Массив длиннее значения maxItems.
    kValidateErrorMinItems,                    //!< Массив значений minItems.
    kValidateErrorUniqueItems,                 //!< В массиве есть повторяющиеся элементы, но значение «uniqueItems» истинно.
    kValidateErrorAdditionalItems,             //!< Массив содержит дополнительные элементы, не разрешенные схемой.

    kValidateErrorMaxProperties,               //!< Объект имеет больше участников, чем значение 'maxProperties'.
    kValidateErrorMinProperties,               //!< Объект имеет меньшее число участников, чем значение minProperties.
    kValidateErrorRequired,                    //!< В объекте отсутствует один или несколько членов, требуемых схемой.
    kValidateErrorAdditionalProperties,        //!< Объект имеет дополнительные члены, которые не разрешены схемой.
    kValidateErrorPatternProperties,           //!< См. другие ошибки.
    kValidateErrorDependencies,                //!< У объекта отсутствуют зависимости свойств или схемы.

    kValidateErrorEnum,                        //!< Свойство имеет значение, не входящее в число допустимых перечислимых значений.
    kValidateErrorType,                        //!< Тип свойства не разрешен схемой.

    kValidateErrorOneOf,                       //!< Свойство не соответствует ни одной из подсхем, указанных в параметре oneOf.
    kValidateErrorOneOfMatch,                  //!< Свойство соответствует более чем одной из подсхем, указанной в параметре oneOf.
    kValidateErrorAllOf,                       //!< Свойство не соответствует всем подсхемам, указанным в параметре allOf.
    kValidateErrorAnyOf,                       //!< Свойство не соответствует ни одной из подсхем, указанных в параметре AnyOf.
    kValidateErrorNot,                         //!< Свойство соответствует подсхеме, указанной в параметре «не».

    kValidateErrorReadOnly,                    //!< Свойство доступно только для чтения, но предоставляется, когда проверка предназначена для записи.
    kValidateErrorWriteOnly                    //!< Свойство доступно только для записи, но предоставляется, когда проверка предназначена для чтения.
};

//! Тип указателя функцииGetValidateError().
/*! \ingroup RAPIDJSON_ERRORS

    Это прототип \c GetValidateError_X(), где \c X — локальный.
    Пользователь может динамически изменять локаль во время выполнения, например:
\code
    GetValidateErrorFunc GetValidateError =GetValidateError_En; // или что-то еще
    constRAPIDJSON_ERROR_CHARTYPE* s = GetValidateError(validator. GetInvalidSchemaCode());
\endcode
*/
typedef const RAPIDJSON_ERROR_CHARTYPE* (*GetValidateErrorFunc)(ValidateErrorCode);

///////////////////////////////////////////////////////////////////////////////
// СхемаErrorCode

//! Коды ошибок при проверке.
/*! \ingroup RAPIDJSON_ERRORS
    \см. GenericSchemaValidator
*/
enum SchemaErrorCode {
    kSchemaErrorNone = 0,                      //!< Нет ошибок.

    kSchemaErrorStartUnknown,                  //!< Указатель на начало схемы не соответствует местоположению в документе.
    kSchemaErrorRefPlainName,                  //!< Фрагмент $ref должен быть указателемJSON.
    kSchemaErrorRefInvalid,                    //!< $ref не должна быть пустой строкой
    kSchemaErrorRefPointerInvalid,             //!< Фрагмент $ref не является допустимым указателемJSONпо смещению.
    kSchemaErrorRefUnknown,                    //!< $ref не соответствует местуположению в целевом документе.
    kSchemaErrorRefCyclical,                   //!< $ref является циклическим
    kSchemaErrorRefNoRemoteProvider,           //!< $ref является удаленным, но удаленного поставщика нет.
    kSchemaErrorRefNoRemoteSchema,             //!< $ref является удаленным, но удаленный поставщик не вернул схему
    kSchemaErrorRegexInvalid,                  //!< Недопустимое регулярное выражение в «шаблоне» или «patternProperties».
    kSchemaErrorSpecUnknown,                   //!< Черновик схемыJSONили версия OpenAPI не распознается
    kSchemaErrorSpecUnsupported,               //!< Черновик схемыJSONили версия OpenAPI не используется.
    kSchemaErrorSpecIllegal,                   //!< Как черновик схемы JSON, так и версия OpenAPI, найденная в документе.
    kSchemaErrorReadOnlyAndWriteOnly           //!< Свойство не должно быть одновременно «readOnly» и «writeOnly».
};

//! Тип указателя функцииGetSchemaError().
/*! \ingroup RAPIDJSON_ERRORS

    Это прототип \c GetSchemaError_X(), где \c X — локальный.
    Пользователь может динамически изменять локаль во время выполнения, например:
\code
    GetSchemaErrorFunc GetSchemaError =GetSchemaError_En; // или что-то еще
    constRAPIDJSON_ERROR_CHARTYPE* s = GetSchemaError(validator. GetInvalidSchemaCode());
\endcode
*/
typedef const RAPIDJSON_ERROR_CHARTYPE* (*GetSchemaErrorFunc)(SchemaErrorCode);

///////////////////////////////////////////////////////////////////////////////
// Поинтерпарсеерркоде

//! Код ошибки анализа указателя JSON.
/*! \ingroup RAPIDJSON_ERRORS
    \see GenericPointer::GenericPointer, GenericPointer::GetParseErrorCode
*/
enum PointerParseErrorCode {
    kPointerParseErrorNone = 0,                     //!< Анализ прошел успешно

    kPointerParseErrorTokenMustBeginWithSolidus,    //!< Токен должен начинаться с '/'
    kPointerParseErrorInvalidEscape,                //!< Неверный побег
    kPointerParseErrorInvalidPercentEncoding,       //!< Недопустимая процентная кодировка во фрагменте URI.
    kPointerParseErrorCharacterMustPercentEncode    //!< Символ должен быть закодирован в процентах во фрагменте URI.
};

//! Тип указателя функцииGetPointerParseError().
/*! \ingroup RAPIDJSON_ERRORS

    Это прототип \c GetPointerParseError_X(), где \c X — локальный.
    Пользователь может динамически изменять локаль во время выполнения, например:
\code
    GetPointerParseErrorFunc GetPointerParseError =GetPointerParseError_En; // или что-то еще
    constRAPIDJSON_ERROR_CHARTYPE* s = GetPointerParseError (pointer. GetParseErrorCode());
\endcode
*/
typedef const RAPIDJSON_ERROR_CHARTYPE* (*GetPointerParseErrorFunc)(PointerParseErrorCode);


RAPIDJSON_NAMESPACE_END

#ifdef __clang__
RAPIDJSON_DIAG_POP
#endif

#endif // RAPIDJSON_ERROR_ERROR_H_
