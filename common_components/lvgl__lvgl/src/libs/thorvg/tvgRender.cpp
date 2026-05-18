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

#include "tvgMath.h"
#include "tvgRender.h"

/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/


/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/

uint32_t RenderMethod::ref()
{
    ScopedLock lock(key);
    return (++refCnt);
}


uint32_t RenderMethod::unref()
{
    ScopedLock lock(key);
    return (--refCnt);
}


void RenderRegion::intersect(const RenderRegion& rhs)
{
    auto x1 = x + w;
    auto y1 = y + h;
    auto x2 = rhs.x + rhs.w;
    auto y2 = rhs.y + rhs.h;

    x = (x > rhs.x) ? x : rhs.x;
    y = (y > rhs.y) ? y : rhs.y;
    w = ((x1 < x2) ? x1 : x2) - x;
    h = ((y1 < y2) ? y1 : y2) - y;

    if (w < 0) w = 0;
    if (h < 0) h = 0;
}


void RenderRegion::add(const RenderRegion& rhs)
{
    if (rhs.x < x) {
        w += (x - rhs.x);
        x = rhs.x;
    }
    if (rhs.y < y) {
        h += (y - rhs.y);
        y = rhs.y;
    }
    if (rhs.x + rhs.w > x + w) w = (rhs.x + rhs.w) - x;
    if (rhs.y + rhs.h > y + h) h = (rhs.y + rhs.h) - y;
}

#endif /* LV_USE_THORVG_INTERNAL */

