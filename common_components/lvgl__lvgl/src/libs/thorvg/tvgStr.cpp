/*
 * Copyright (c) 2020 - 2024 the ThorVG project. All rights reserved.

 * Разрешение настоящим предоставляется бесплатно любому лицу, получившему копию.
 * данного программного обеспечения и связанных с ним файлов документации («Программное обеспечение») для решения
 * в Программном обеспечении без ограничений, включая, помимо прочего, права
 * использовать, копировать, изменять, объединять, публиковать, распространять, сублицензировать и/или продавать
 * копий Программного обеспечения и разрешать лицам, которым Программное обеспечение
 * предоставлено для этого при соблюдении следующих условий:

 * Вышеупомянутое уведомление об авторских правах и настоящее уведомление о разрешении должны быть включены во все
 * копии или существенные части Программного обеспечения.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "../../lv_conf_internal.h"
#if LV_USE_THORVG_INTERNAL

#include "config.h"
#include <cmath>
#include <cstring>
#include <memory.h>
#include "tvgMath.h"
#include "tvgStr.h"


/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/

static inline bool _floatExact(float a, float b)
{
    return memcmp(&a, &b, sizeof(float)) == 0;
}


/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/

namespace tvg {

/*
 * https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/strtof-strtof-l-wcstof-wcstof-l?view=msvc-160
 *
 * src должна иметь одну из следующих форм:
 *
 * [пробел] [знак] {цифры [основание цифр] | системы счисления} [{e | E} [знак] цифры]
 * [пробел] [знак] { INF |  INFINITY }
 * [пробел] [знак] NAN [последовательность]
 *
 * Шестнадцатеричная форма не поддерживается.
 * последовательность после NAN не поддерживается
 */
float strToFloat(const char *nPtr, char **endPtr)
{
    if (endPtr) *endPtr = (char *) (nPtr);
    if (!nPtr) return 0.0f;

    auto a = nPtr;
    auto iter = nPtr;
    auto val = 0.0f;
    unsigned long long integerPart = 0;
    int minus = 1;

    //игнорировать ведущие пробелы
    while (isspace(*iter)) iter++;

    //подписано или нет
    if (*iter == '-') {
        minus = -1;
        iter++;
    } else if (*iter == '+') {
        iter++;
    }

    if (tolower(*iter) == 'i') {
        if ((tolower(*(iter + 1)) == 'n') && (tolower(*(iter + 2)) == 'f')) iter += 3;
        else goto error;

        if (tolower(*(iter)) == 'i') {
            if ((tolower(*(iter + 1)) == 'n') && (tolower(*(iter + 2)) == 'i') && (tolower(*(iter + 3)) == 't') &&
                (tolower(*(iter + 4)) == 'y'))
                iter += 5;
            else goto error;
        }
        if (endPtr) *endPtr = (char *) (iter);
        return (minus == -1) ? -INFINITY : INFINITY;
    }

    if (tolower(*iter) == 'n') {
        if ((tolower(*(iter + 1)) == 'a') && (tolower(*(iter + 2)) == 'n')) iter += 3;
        else goto error;

        if (endPtr) *endPtr = (char *) (iter);
        return (minus == -1) ? -NAN : NAN;
    }

    //Optional: целая часть перед точкой
    if (isdigit(*iter)) {
        for (; isdigit(*iter); iter++) {
            integerPart = integerPart * 10ULL + (unsigned long long) (*iter - '0');
        }
        a = iter;
    } else if (*iter != '.') {
        goto success;
    }

    val = static_cast<float>(integerPart);

    //Optional: десятичная часть после точки
    if (*iter == '.') {
        unsigned long long decimalPart = 0;
        unsigned long long pow10 = 1;
        int count = 0;

        iter++;

        if (isdigit(*iter)) {
            for (; isdigit(*iter); iter++, count++) {
                if (count < 19) {
                    decimalPart = decimalPart * 10ULL + +static_cast<unsigned long long>(*iter - '0');
                    pow10 *= 10ULL;
                }
            }
        } else if (isspace(*iter)) { //пропустить, если после точки есть пробел.
            a = iter;
            goto success;
        }

        val += static_cast<float>(decimalPart) / static_cast<float>(pow10);
        a = iter;
    }

    //Optional: показатель степени
    if (*iter == 'e' || *iter == 'E') {
        ++iter;

        //Exception: svg может иметь единицу измерения шрифтов «em». например) 5эм, 10,5эм
        if ((*iter == 'm') || (*iter == 'M')) {
            //TODO: Сейчас мы не поддерживаем единицу измерения шрифта em, но позже нам придется умножить размер шрифта val *...
            a = iter + 1;
            goto success;
        }

        //подписано или нет
        int minus_e = 1;

        if (*iter == '-') {
            minus_e = -1;
            ++iter;
        } else if (*iter == '+') {
            iter++;
        }

        unsigned int exponentPart = 0;

        if (isdigit(*iter)) {
            while (*iter == '0') iter++;
            for (; isdigit(*iter); iter++) {
                exponentPart = exponentPart * 10U + static_cast<unsigned int>(*iter - '0');
            }
        } else if (!isdigit(*(a - 1))) {
            a = nPtr;
            goto success;
        } else if (*iter == 0) {
            goto success;
        }

        //if ((_floatExact(val, 2.2250738585072011f)) && ((minus_e*static_cast<int>(exponPart)) <= -308)) {
        if ((_floatExact(val, 1.175494351f)) && ((minus_e * static_cast<int>(exponentPart)) <= -38)) {
            //вал *= 1.0e-308f;
            val *= 1.0e-38f;
            a = iter;
            goto success;
        }

        a = iter;
        auto scale = 1.0f;

        while (exponentPart >= 8U) {
            scale *= 1E8;
            exponentPart -= 8U;
        }
        while (exponentPart > 0U) {
            scale *= 10.0f;
            exponentPart--;
        }
        val = (minus_e == -1) ? (val / scale) : (val * scale);
    } else if ((iter > nPtr) && !isdigit(*(iter - 1))) {
        a = nPtr;
        goto success;
    }

success:
    if (endPtr) *endPtr = (char *)(a);
    if (!std::isfinite(val)) return 0.0f;

    return minus * val;

error:
    if (endPtr) *endPtr = (char *)(nPtr);
    return 0.0f;
}

char* strDuplicate(const char *str, size_t n)
{
    auto len = strlen(str);
    if (len < n) n = len;

    auto ret = (char *) lv_malloc(n + 1);
    LV_ASSERT_MALLOC(ret);
    if (!ret) return nullptr;
    ret[n] = '\0';

    return (char *) memcpy(ret, str, n);
}

char* strAppend(char* lhs, const char* rhs, size_t n)
{
    if (!rhs) return lhs;
    if (!lhs) return strDuplicate(rhs, n);
    lhs = (char*)lv_realloc(lhs, strlen(lhs) + n + 1);
    LV_ASSERT_MALLOC(lhs);
    return strncat(lhs, rhs, n);
}

char* strDirname(const char* path)
{
    const char *ptr = strrchr(path, '/');
#ifdef _WIN32
    if (ptr) ptr = strrchr(ptr + 1, '\\');
#endif
    int len = int(ptr + 1 - path);  // +1, чтобы включить '/'
    return strDuplicate(path, len);
}

}

#endif /* LV_USE_THORVG_INTERNAL */

