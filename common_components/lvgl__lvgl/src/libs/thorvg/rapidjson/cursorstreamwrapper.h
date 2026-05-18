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

#ifndef RAPIDJSON_CURSORSTREAMWRAPPER_H_
#define RAPIDJSON_CURSORSTREAMWRAPPER_H_

#include "stream.h"

#if defined(__GNUC__)
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(effc++)
#endif

#if defined(_MSC_VER) && _MSC_VER <= 1800
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(4702)  // недостижимый код
RAPIDJSON_DIAG_OFF(4512)  // не удалось сгенерировать оператор присваивания
#endif

RAPIDJSON_NAMESPACE_BEGIN


//! Оболочка потока курсоров для подсчета количества строк и столбцов в случае возникновения ошибки.
/*!
    \tparam InputStream Любой поток, реализующий формирование потока.
*/
template <typename InputStream, typename Encoding = UTF8<> >
class CursorStreamWrapper : public GenericStreamWrapper<InputStream, Encoding> {
public:
    typedef typename Encoding::Ch Ch;

    CursorStreamWrapper(InputStream& is):
        GenericStreamWrapper<InputStream, Encoding>(is), line_(1), col_(0) {}

    // подсчет номера строки и столбца
    Ch Take() {
        Ch ch = this->is_.Take();
        if(ch == '\n') {
            line_ ++;
            col_ = 0;
        } else {
            col_ ++;
        }
        return ch;
    }

    //! Получите номер строки ошибки, если ошибка существует.
    size_t GetLine() const { return line_; }
    //! Получите номер столбца ошибки, если ошибка существует.
    size_t GetColumn() const { return col_; }

private:
    size_t line_;   //!< Текущая строка
    size_t col_;    //!< Текущий столбец
};

#if defined(_MSC_VER) && _MSC_VER <= 1800
RAPIDJSON_DIAG_POP
#endif

#if defined(__GNUC__)
RAPIDJSON_DIAG_POP
#endif

RAPIDJSON_NAMESPACE_END

#endif // RAPIDJSON_CURSORSTREAMWRAPPER_H_
