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

#include <cstring>
#include "tvgSvgUtil.h"

/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/

static uint8_t _hexCharToDec(const char c)
{
    if (c >= 'a') return c - 'a' + 10;
    else if (c >= 'A') return c - 'A' + 10;
    else return c - '0';
}


/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/

size_t svgUtilURLDecode(const char *src, char** dst)
{
    if (!src) return 0;

    auto length = strlen(src);
    if (length == 0) return 0;

    char* decoded = (char*)lv_malloc(sizeof(char) * length + 1);
    LV_ASSERT_MALLOC(decoded);

    char a, b;
    int idx =0;
    while (*src) {
        if (*src == '%' &&
            ((a = src[1]) && (b = src[2])) &&
            (isxdigit(a) && isxdigit(b))) {
            decoded[idx++] = (_hexCharToDec(a) << 4) + _hexCharToDec(b);
            src+=3;
        } else if (*src == '+') {
            decoded[idx++] = ' ';
            src++;
        } else {
            decoded[idx++] = *src++;
        }
    }
    decoded[idx] = '\0';

    *dst = decoded;
    return idx + 1;
}


#endif /* LV_USE_THORVG_INTERNAL */

