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

#ifndef _TVG_COMMON_H_
#define _TVG_COMMON_H_

#include "config.h"
#include "thorvg.h"
#include <mutex>

// использование пространства имен std;
using std::string;

using std::lock_guard;
using std::unique_ptr;

using namespace tvg;

//для MSVC Совместимость
#ifdef _MSC_VER
    #define TVG_UNUSED
    #define strncasecmp _strnicmp
    #define strcasecmp _stricmp
#else
    #define TVG_UNUSED __attribute__ ((__unused__))
#endif

// Переносной атрибут «проваливания»
#if __has_cpp_attribute(fallthrough)
    #ifdef _MSC_VER
        #define TVG_FALLTHROUGH [[fallthrough]];
    #else
        #define TVG_FALLTHROUGH __attribute__ ((fallthrough));
    #endif
#else
    #define TVG_FALLTHROUGH
#endif

#if defined(_MSC_VER) && defined(__clang__)
    #define strncpy strncpy_s
    #define strdup _strdup
#endif

enum class FileType { Png = 0, Jpg, Webp, Tvg, Svg, Lottie, Ttf, Raw, Gif, Unknown };

using Size = Point;

#ifdef THORVG_LOG_ENABLED
    constexpr auto ErrorColor = "\033[31m";  //красный
    constexpr auto ErrorBgColor = "\033[41m";//фон красный
    constexpr auto LogColor = "\033[32m";    //зеленый
    constexpr auto LogBgColor = "\033[42m";  //темно-зеленый
    constexpr auto GreyColor = "\033[90m";   //серый
    constexpr auto ResetColors = "\033[0m";  //по умолчанию
    #define TVGERR(tag, fmt, ...) fprintf(stderr, "%s[E]%s %s" tag "%s (%s %d): %s" fmt "\n", ErrorBgColor, ResetColors, ErrorColor, GreyColor, __FILE__, __LINE__, ResetColors, ##__VA_ARGS__)
    #define TVGLOG(tag, fmt, ...) fprintf(stdout, "%s[L]%s %s" tag "%s (%s %d): %s" fmt "\n", LogBgColor, ResetColors, LogColor, GreyColor, __FILE__, __LINE__, ResetColors, ##__VA_ARGS__)
#else
    #define TVGERR(...) do {} while(0)
    #define TVGLOG(...) do {} while(0)
#endif

uint16_t THORVG_VERSION_NUMBER();


#define P(A) ((A)->pImpl)              //Доступ к прыщу.
#define PP(A) (((Paint*)(A))->pImpl)   //Доступ к прыщу.


//для отладки
#if 0
#include <sys/time.h>
static inline double THORVG_TIMESTAMP()
{
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return (tv.tv_sec + tv.tv_usec / 1000000.0);
}
#endif

#endif //_TVG_COMMON_H_

#endif /* LV_USE_THORVG_INTERNAL */

