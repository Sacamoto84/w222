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

#ifndef RAPIDJSON_INTERNAL_SWAP_H_
#define RAPIDJSON_INTERNAL_SWAP_H_

#include "../rapidjson.h"

#if defined(__clang__)
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(c++98-compat)
#endif

RAPIDJSON_NAMESPACE_BEGIN
namespace internal {

//! Пользовательский swap(), чтобы избежать зависимости от заголовка <algorithm> C++.
/*! \tparam T Тип аргументов для замены, должен быть создан только с использованием примитивных типов C++.
    \note Имеет ту же семантику, что и std::swap().
*/
template <typename T>
inline void Swap(T& a, T& b) RAPIDJSON_NOEXCEPT {
    T tmp = a;
        a = b;
        b = tmp;
}

} // внутреннее пространство имен
RAPIDJSON_NAMESPACE_END

#if defined(__clang__)
RAPIDJSON_DIAG_POP
#endif

#endif // RAPIDJSON_INTERNAL_SWAP_H_
