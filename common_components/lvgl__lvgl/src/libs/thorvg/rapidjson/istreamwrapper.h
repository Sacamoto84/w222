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

#ifndef RAPIDJSON_ISTREAMWRAPPER_H_
#define RAPIDJSON_ISTREAMWRAPPER_H_

#include "stream.h"
#include <iosfwd>
#include <ios>

#ifdef __clang__
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(padded)
#elif defined(_MSC_VER)
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(4351) // новое поведение: элементы массива «array» будут инициализированы по умолчанию.
#endif

RAPIDJSON_NAMESPACE_BEGIN

//! Обертка \c std::basic_istream в концепцию RapidJSON Stream.
/*!
    Классы могут быть обернуты, включая, помимо прочего:

    - \c std::istringstream
    - \c std::stringstream
    - \c std::wistringstream
    - \c std::wstringstream
    - \c std::ifstream
    - \c std::fstream
    - \c std::wifstream
    - \c std::wfstream

    \tparam Класс StreamType, производный от \c std::basic_istream .
*/

template <typename StreamType>
class BasicIStreamWrapper {
public:
    typedef typename StreamType::char_type Ch;

    //! Конструктор.
    /*!
        \param поток поток, открытый для чтения.
    */
    BasicIStreamWrapper(StreamType &stream) : stream_(stream), buffer_(peekBuffer_), bufferSize_(4), bufferLast_(0), current_(buffer_), readCount_(0), count_(0), eof_(false) {
        Read();
    }

    //! Конструктор.
    /*!
        \param поток поток, открытый для чтения.
        \param buffer Буфер, предоставляемый пользователем.
        \paramufferSize размер буфера в байтах. Должно >= 4 байта.
    */
    BasicIStreamWrapper(StreamType &stream, char* buffer, size_t bufferSize) : stream_(stream), buffer_(buffer), bufferSize_(bufferSize), bufferLast_(0), current_(buffer_), readCount_(0), count_(0), eof_(false) {
        RAPIDJSON_ASSERT(bufferSize >= 4);
        Read();
    }

    Ch Peek() const { return *current_; }
    Ch Take() { Ch c = *current_; Read(); return c; }
    size_t Tell() const { return count_ + static_cast<size_t>(current_ - buffer_); }

    // Не реализовано
    void Put(Ch) { RAPIDJSON_ASSERT(false); }
    void Flush() { RAPIDJSON_ASSERT(false); }
    Ch* PutBegin() { RAPIDJSON_ASSERT(false); return 0; }
    size_t PutEnd(Ch*) { RAPIDJSON_ASSERT(false); return 0; }

    // Только для обнаружения кодировки.
    const Ch* Peek4() const {
        return (current_ + 4 - !eof_ <= bufferLast_) ? current_ : 0;
    }

private:
    BasicIStreamWrapper();
    BasicIStreamWrapper(const BasicIStreamWrapper&);
    BasicIStreamWrapper& operator=(const BasicIStreamWrapper&);

    void Read() {
        if (current_ < bufferLast_)
            ++current_;
        else if (!eof_) {
            count_ += readCount_;
            readCount_ = bufferSize_;
            bufferLast_ = buffer_ + readCount_ - 1;
            current_ = buffer_;

            if (!stream_.read(buffer_, static_cast<std::streamsize>(bufferSize_))) {
                readCount_ = static_cast<size_t>(stream_.gcount());
                *(bufferLast_ = buffer_ + readCount_) = '\0';
                eof_ = true;
            }
        }
    }

    StreamType &stream_;
    Ch peekBuffer_[4], *buffer_;
    size_t bufferSize_;
    Ch *bufferLast_;
    Ch *current_;
    size_t readCount_;
    size_t count_;  //!< Количество прочитанных символов
    bool eof_;
};

typedef BasicIStreamWrapper<std::istream> IStreamWrapper;
typedef BasicIStreamWrapper<std::wistream> WIStreamWrapper;

#if defined(__clang__) || defined(_MSC_VER)
RAPIDJSON_DIAG_POP
#endif

RAPIDJSON_NAMESPACE_END

#endif // RAPIDJSON_ISTREAMWRAPPER_H_
