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

#ifndef RAPIDJSON_CLZLL_H_
#define RAPIDJSON_CLZLL_H_

#include "../rapidjson.h"

#if defined(_MSC_VER) && !defined(UNDER_CE)
#include <intrin.h>
#if defined(_WIN64)
#pragma intrinsic(_BitScanReverse64)
#else
#pragma intrinsic(_BitScanReverse)
#endif
#endif

RAPIDJSON_NAMESPACE_BEGIN
namespace internal {

inline uint32_t clzll(uint64_t x) {
    // Передача 0 в__builtin_clzllозначаетUBвGCCи приводит к
    // бесконечный цикл в реализации программного обеспечения.
    RAPIDJSON_ASSERT(x != 0);

#if defined(_MSC_VER) && !defined(UNDER_CE)
    unsigned long r = 0;
#if defined(_WIN64)
    _BitScanReverse64(&r, x);
#else
    // Сканируйте старшие 32 бита.
    if (_BitScanReverse(&r, static_cast<uint32_t>(x >> 32)))
        return 63 - (r + 32);

    // Сканируйте младшие 32 бита.
    _BitScanReverse(&r, static_cast<uint32_t>(x & 0xFFFFFFFF));
#endif // _WIN64

    return 63 - r;
#elif (defined(__GNUC__) && __GNUC__ >= 4) || RAPIDJSON_HAS_BUILTIN(__builtin_clzll)
    // Обертка __builtin_clzll
    return static_cast<uint32_t>(__builtin_clzll(x));
#else
    // наивная версия
    uint32_t r = 0;
    while (!(x & (static_cast<uint64_t>(1) << 63))) {
        x <<= 1;
        ++r;
    }

    return r;
#endif // _MSC_VER
}

#define RAPIDJSON_CLZLL RAPIDJSON_NAMESPACE::internal::clzll

} // внутреннее пространство имен
RAPIDJSON_NAMESPACE_END

#endif // RAPIDJSON_CLZLL_H_
