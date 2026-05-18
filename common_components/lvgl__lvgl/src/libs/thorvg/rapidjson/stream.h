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

#include "rapidjson.h"

#ifndef RAPIDJSON_STREAM_H_
#define RAPIDJSON_STREAM_H_

#include "encodings.h"

RAPIDJSON_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//  поток

/*! \класс RapidJSON::Stream
    \brief Концепция чтения и написания символов.

    Для потока только для чтения нет необходимости реализовывать PutBegin() , Put() , Flush() и PutEnd() .

    Для потока только для записи необходимо реализовать только Put() и Flush().

\code
концепция потока {
    имя типа Ch;    //!< Тип символа потока.

    //! Считайте текущий символ из потока, не перемещая курсор чтения.
    Ch Peek() const;

    //! Прочитайте текущий символ из потока и переместите курсор чтения на следующий символ.
    Ч Take() ;

    //! Получить текущий курсор чтения.
    //! \return Количество символов, прочитанных с начала.
    size_t Tell() ;

    //! Начните операцию записи с текущего указателя чтения.
    //! \return Указатель начала записи.
    Ч* PutBegin() ;

    //! Напишите персонажа.
    недействительный Put (Ch c);

    //! Промойте буфер.
    недействителен Flush() ;

    //! Завершите операцию записи.
    //! \param Begin Указатель начала записи, возвращаемый PutBegin().
    //! \return Количество записанных символов.
    size_t PutEnd(Ch* начало);
}
\endcode
*/

//! Предоставляет дополнительную информацию для потока.
/*!
    Используя шаблон признаков, этот тип обеспечивает конфигурацию потока по умолчанию.
    Для пользовательского потока этот тип может быть специализирован для другой конфигурации.
    См., например, TEST (Reader, CustomStringStream) в readertest.cpp.
*/
template<typename Stream>
struct StreamTraits {
    //! Делать ли локальную копию потока для оптимизации во время анализа.
    /*!
        По умолчанию в целях безопасности потоки не используют оптимизацию локального копирования.
        Поток, который можно быстро скопировать, должен специализироваться на этом, например StreamTraits<StringStream>.
    */
    enum { copyOptimization = 0 };
};

//! Зарезервируйте n символов для записи в поток.
template<typename Stream>
inline void PutReserve(Stream& stream, size_t count) {
    (void)stream;
    (void)count;
}

//! Записать символ в поток, предполагая, что буфер зарезервирован.
template<typename Stream>
inline void PutUnsafe(Stream& stream, typename Stream::Ch c) {
    stream.Put(c);
}

//! Поместите N копий символа в поток.
template<typename Stream, typename Ch>
inline void PutN(Stream& stream, Ch c, size_t n) {
    PutReserve(stream, n);
    for (size_t i = 0; i < n; i++)
        PutUnsafe(stream, c);
}

///////////////////////////////////////////////////////////////////////////////
// GenericStreamWrapper

//! Обертка потока
/*! \tЭтот строковый поток является оберткой для любого потока, просто пересылая любой
    \trecived сообщение в исходный поток.
    \note реализует концепцию Stream
*/

#if defined(_MSC_VER) && _MSC_VER <= 1800
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(4702)  // недостижимый код
RAPIDJSON_DIAG_OFF(4512)  // не удалось сгенерировать оператор присваивания
#endif

template <typename InputStream, typename Encoding = UTF8<> >
class GenericStreamWrapper {
public:
    typedef typename Encoding::Ch Ch;
    GenericStreamWrapper(InputStream& is): is_(is) {}

    Ch Peek() const { return is_.Peek(); }
    Ch Take() { return is_.Take(); }
    size_t Tell() { return is_.Tell(); }
    Ch* PutBegin() { return is_.PutBegin(); }
    void Put(Ch ch) { is_.Put(ch); }
    void Flush() { is_.Flush(); }
    size_t PutEnd(Ch* ch) { return is_.PutEnd(ch); }

    // оболочка для MemoryStream
    const Ch* Peek4() const { return is_.Peek4(); }

    // оболочка для AutoUTFInputStream
    UTFType GetType() const { return is_.GetType(); }
    bool HasBOM() const { return is_.HasBOM(); }

protected:
    InputStream& is_;
};

#if defined(_MSC_VER) && _MSC_VER <= 1800
RAPIDJSON_DIAG_POP
#endif

///////////////////////////////////////////////////////////////////////////////
// Строковый поток

//! Строковый поток только для чтения.
/*! \note реализует концепцию Stream
*/
template <typename Encoding>
struct GenericStringStream {
    typedef typename Encoding::Ch Ch;

    GenericStringStream(const Ch *src) : src_(src), head_(src) {}

    Ch Peek() const { return *src_; }
    Ch Take() { return *src_++; }
    size_t Tell() const { return static_cast<size_t>(src_ - head_); }

    Ch* PutBegin() { RAPIDJSON_ASSERT(false); return 0; }
    void Put(Ch) { RAPIDJSON_ASSERT(false); }
    void Flush() { RAPIDJSON_ASSERT(false); }
    size_t PutEnd(Ch*) { RAPIDJSON_ASSERT(false); return 0; }

    const Ch* src_;     //!< Текущая позиция чтения.
    const Ch* head_;    //!< Исходный заголовок строки.
};

template <typename Encoding>
struct StreamTraits<GenericStringStream<Encoding> > {
    enum { copyOptimization = 1 };
};

//! Строковый поток с кодировкой UTF8.
typedef GenericStringStream<UTF8<> > StringStream;

///////////////////////////////////////////////////////////////////////////////
// InsituStringStream

//! Строковый поток для чтения и записи.
/*! Этот строковый поток специально разработан для анализа на месте.
    \note реализует концепцию Stream
*/
template <typename Encoding>
struct GenericInsituStringStream {
    typedef typename Encoding::Ch Ch;

    GenericInsituStringStream(Ch *src) : src_(src), dst_(0), head_(src) {}

    // Читать
    Ch Peek() { return *src_; }
    Ch Take() { return *src_++; }
    size_t Tell() { return static_cast<size_t>(src_ - head_); }

    // Написать
    void Put(Ch c) { RAPIDJSON_ASSERT(dst_ != 0); *dst_++ = c; }

    Ch* PutBegin() { return dst_ = src_; }
    size_t PutEnd(Ch* begin) { return static_cast<size_t>(dst_ - begin); }
    void Flush() {}

    Ch* Push(size_t count) { Ch* begin = dst_; dst_ += count; return begin; }
    void Pop(size_t count) { dst_ -= count; }

    Ch* src_;
    Ch* dst_;
    Ch* head_;
};

template <typename Encoding>
struct StreamTraits<GenericInsituStringStream<Encoding> > {
    enum { copyOptimization = 1 };
};

//! Поток строк Insitu с кодировкой UTF8.
typedef GenericInsituStringStream<UTF8<> > InsituStringStream;

RAPIDJSON_NAMESPACE_END

#endif // RAPIDJSON_STREAM_H_
