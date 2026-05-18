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

#ifndef RAPIDJSON_PRETTYWRITER_H_
#define RAPIDJSON_PRETTYWRITER_H_

#include "writer.h"

#ifdef __GNUC__
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(effc++)
#endif

#if defined(__clang__)
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(c++98-compat)
#endif

RAPIDJSON_NAMESPACE_BEGIN

//! Комбинация флагов формата PrettyWriter.
/*! \см. PrettyWriter::SetFormatOptions
 */
enum PrettyFormatOptions {
    kFormatDefault = 0,         //!< Красивое форматирование по умолчанию.
    kFormatSingleLineArray = 1  //!< Форматирует массивы в одну строку.
};

//! Writer с отступами и интервалами.
/*!
    \tparam OutputStream Тип выходной ОС.
    \tparam SourceEncoding Кодировка исходной строки.
    \tparam TargetEncoding Кодирование выходного потока.
    \tparam StackAllocator Тип распределителя для выделения памяти стека.
*/
template<typename OutputStream, typename SourceEncoding = UTF8<>, typename TargetEncoding = UTF8<>, typename StackAllocator = CrtAllocator, unsigned writeFlags = kWriteDefaultFlags>
class PrettyWriter : public Writer<OutputStream, SourceEncoding, TargetEncoding, StackAllocator, writeFlags> {
public:
    typedef Writer<OutputStream, SourceEncoding, TargetEncoding, StackAllocator, writeFlags> Base;
    typedef typename Base::Ch Ch;

    //! Конструктор
    /*! \param os Выходной поток.
        \param allocator Пользовательский распределитель. Если оно равно нулю, будет создан частный.
        \param levelDepth Начальная емкость стека.
    */
    explicit PrettyWriter(OutputStream& os, StackAllocator* allocator = 0, size_t levelDepth = Base::kDefaultLevelDepth) :
        Base(os, allocator, levelDepth), indentChar_(' '), indentCharCount_(4), formatOptions_(kFormatDefault) {}


    explicit PrettyWriter(StackAllocator* allocator = 0, size_t levelDepth = Base::kDefaultLevelDepth) :
        Base(allocator, levelDepth), indentChar_(' '), indentCharCount_(4), formatOptions_(kFormatDefault) {}

#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
    PrettyWriter(PrettyWriter&& rhs) :
        Base(std::forward<PrettyWriter>(rhs)), indentChar_(rhs.indentChar_), indentCharCount_(rhs.indentCharCount_), formatOptions_(rhs.formatOptions_) {}
#endif

    //! Установите собственный отступ.
    /*! \param indentChar Символ отступа. Должен быть символом пробела (' ', '\\t', '\\n', '\\r').
        \param indentCharCount Число символов отступа для каждого уровня отступа.
        \note Отступ по умолчанию составляет 4 пробела.
    */
    PrettyWriter& SetIndent(Ch indentChar, unsigned indentCharCount) {
        RAPIDJSON_ASSERT(indentChar == ' ' || indentChar == '\t' || indentChar == '\n' || indentChar == '\r');
        indentChar_ = indentChar;
        indentCharCount_ = indentCharCount;
        return *this;
    }

    //! Установите красивые параметры форматирования.
    /*! \param options Параметры форматирования.
    */
    PrettyWriter& SetFormatOptions(PrettyFormatOptions options) {
        formatOptions_ = options;
        return *this;
    }

    /*! @name Реализация обработчика
        \см. обработчик
    */
    //@{

    bool Null()                 { PrettyPrefix(kNullType);   return Base::EndValue(Base::WriteNull()); }
    bool Bool(bool b)           { PrettyPrefix(b ? kTrueType : kFalseType); return Base::EndValue(Base::WriteBool(b)); }
    bool Int(int i)             { PrettyPrefix(kNumberType); return Base::EndValue(Base::WriteInt(i)); }
    bool Uint(unsigned u)       { PrettyPrefix(kNumberType); return Base::EndValue(Base::WriteUint(u)); }
    bool Int64(int64_t i64)     { PrettyPrefix(kNumberType); return Base::EndValue(Base::WriteInt64(i64)); }
    bool Uint64(uint64_t u64)   { PrettyPrefix(kNumberType); return Base::EndValue(Base::WriteUint64(u64));  }
    bool Double(double d)       { PrettyPrefix(kNumberType); return Base::EndValue(Base::WriteDouble(d)); }

    bool RawNumber(const Ch* str, SizeType length, bool copy = false) {
        RAPIDJSON_ASSERT(str != 0);
        (void)copy;
        PrettyPrefix(kNumberType);
        return Base::EndValue(Base::WriteString(str, length));
    }

    bool String(const Ch* str, SizeType length, bool copy = false) {
        RAPIDJSON_ASSERT(str != 0);
        (void)copy;
        PrettyPrefix(kStringType);
        return Base::EndValue(Base::WriteString(str, length));
    }

#if RAPIDJSON_HAS_STDSTRING
    bool String(const std::basic_string<Ch>& str) {
        return String(str.data(), SizeType(str.size()));
    }
#endif

    bool StartObject() {
        PrettyPrefix(kObjectType);
        new (Base::level_stack_.template Push<typename Base::Level>()) typename Base::Level(false);
        return Base::WriteStartObject();
    }

    bool Key(const Ch* str, SizeType length, bool copy = false) { return String(str, length, copy); }

#if RAPIDJSON_HAS_STDSTRING
    bool Key(const std::basic_string<Ch>& str) {
        return Key(str.data(), SizeType(str.size()));
    }
#endif

    bool EndObject(SizeType memberCount = 0) {
        (void)memberCount;
        RAPIDJSON_ASSERT(Base::level_stack_.GetSize() >= sizeof(typename Base::Level)); // не внутри объекта
        RAPIDJSON_ASSERT(!Base::level_stack_.template Top<typename Base::Level>()->inArray); // в настоящее время внутри массива, а не объекта
        RAPIDJSON_ASSERT(0 == Base::level_stack_.template Top<typename Base::Level>()->valueCount % 2); // Объект имеет ключ без значения

        bool empty = Base::level_stack_.template Pop<typename Base::Level>(1)->valueCount == 0;

        if (!empty) {
            Base::os_->Put('\n');
            WriteIndent();
        }
        bool ret = Base::EndValue(Base::WriteEndObject());
        (void)ret;
        RAPIDJSON_ASSERT(ret == true);
        if (Base::level_stack_.Empty()) // конец текста JSON
            Base::Flush();
        return true;
    }

    bool StartArray() {
        PrettyPrefix(kArrayType);
        new (Base::level_stack_.template Push<typename Base::Level>()) typename Base::Level(true);
        return Base::WriteStartArray();
    }

    bool EndArray(SizeType memberCount = 0) {
        (void)memberCount;
        RAPIDJSON_ASSERT(Base::level_stack_.GetSize() >= sizeof(typename Base::Level));
        RAPIDJSON_ASSERT(Base::level_stack_.template Top<typename Base::Level>()->inArray);
        bool empty = Base::level_stack_.template Pop<typename Base::Level>(1)->valueCount == 0;

        if (!empty && !(formatOptions_ & kFormatSingleLineArray)) {
            Base::os_->Put('\n');
            WriteIndent();
        }
        bool ret = Base::EndValue(Base::WriteEndArray());
        (void)ret;
        RAPIDJSON_ASSERT(ret == true);
        if (Base::level_stack_.Empty()) // конец текста JSON
            Base::Flush();
        return true;
    }

    //@}

    /*! @name Convenience extensions */
    //@{

    //! Более простая, но медленная перегрузка.
    bool String(const Ch* str) { return String(str, internal::StrLen(str)); }
    bool Key(const Ch* str) { return Key(str, internal::StrLen(str)); }

    //@}

    //! Запишите необработанное значение JSON.
    /*!
        Чтобы пользователь мог записать в качестве значения строковое значение JSON.

        \param json Правильно сформированное значение JSON. Он не должен содержать нулевой символ в диапазоне [0, длина – 1].
        \param length Длина json.
        \param type Тип корня json.
        \note При использовании PrettyWriter:: RawValue() отступ в json может быть неправильным.
    */
    bool RawValue(const Ch* json, size_t length, Type type) {
        RAPIDJSON_ASSERT(json != 0);
        PrettyPrefix(type);
        return Base::EndValue(Base::WriteRawValue(json, length));
    }

protected:
    void PrettyPrefix(Type type) {
        (void)type;
        if (Base::level_stack_.GetSize() != 0) { // это значение не является корневым
            typename Base::Level* level = Base::level_stack_.template Top<typename Base::Level>();

            if (level->inArray) {
                if (level->valueCount > 0) {
                    Base::os_->Put(','); // добавьте запятую, если это не первый элемент массива
                    if (formatOptions_ & kFormatSingleLineArray)
                        Base::os_->Put(' ');
                }

                if (!(formatOptions_ & kFormatSingleLineArray)) {
                    Base::os_->Put('\n');
                    WriteIndent();
                }
            }
            else {  // в объекте
                if (level->valueCount > 0) {
                    if (level->valueCount % 2 == 0) {
                        Base::os_->Put(',');
                        Base::os_->Put('\n');
                    }
                    else {
                        Base::os_->Put(':');
                        Base::os_->Put(' ');
                    }
                }
                else
                    Base::os_->Put('\n');

                if (level->valueCount % 2 == 0)
                    WriteIndent();
            }
            if (!level->inArray && level->valueCount % 2 == 0)
                RAPIDJSON_ASSERT(type == kStringType);  // если он находится в объекте, то четное число должно быть именем
            level->valueCount++;
        }
        else {
            RAPIDJSON_ASSERT(!Base::hasRoot_);  // Должно иметь только один и только один корень.
            Base::hasRoot_ = true;
        }
    }

    void WriteIndent()  {
        size_t count = (Base::level_stack_.GetSize() / sizeof(typename Base::Level)) * indentCharCount_;
        PutN(*Base::os_, static_cast<typename OutputStream::Ch>(indentChar_), count);
    }

    Ch indentChar_;
    unsigned indentCharCount_;
    PrettyFormatOptions formatOptions_;

private:
    // Запретить конструктор копирования и оператор присваивания.
    PrettyWriter(const PrettyWriter&);
    PrettyWriter& operator=(const PrettyWriter&);
};

RAPIDJSON_NAMESPACE_END

#if defined(__clang__)
RAPIDJSON_DIAG_POP
#endif

#ifdef __GNUC__
RAPIDJSON_DIAG_POP
#endif

#endif // RAPIDJSON_RAPIDJSON_H_
