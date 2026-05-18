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

#include "tvgCanvas.h"

/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/

Canvas::Canvas(RenderMethod *pRenderer):pImpl(new Impl(pRenderer))
{
}


Canvas::~Canvas()
{
    delete(pImpl);
}


Result Canvas::reserve(TVG_UNUSED uint32_t n) noexcept
{
    return Result::NonSupport;
}


list<Paint*>& Canvas::paints() noexcept
{
    return pImpl->paints;
}


Result Canvas::push(unique_ptr<Paint> paint) noexcept
{
    return pImpl->push(std::move(paint));
}


Result Canvas::clear(bool free) noexcept
{
    return pImpl->clear(free);
}


Result Canvas::draw() noexcept
{
    TVGLOG("RENDERER", "Draw S. -------------------------------- Canvas(%p)", this);
    auto ret = pImpl->draw();
    TVGLOG("RENDERER", "Draw E. -------------------------------- Canvas(%p)", this);

    return ret;
}


Result Canvas::update(Paint* paint) noexcept
{
    TVGLOG("RENDERER", "Update S. ------------------------------ Canvas(%p)", this);
    auto ret = pImpl->update(paint, false);
    TVGLOG("RENDERER", "Update E. ------------------------------ Canvas(%p)", this);

    return ret;
}


Result Canvas::viewport(int32_t x, int32_t y, int32_t w, int32_t h) noexcept
{
    return pImpl->viewport(x, y, w, h);
}


Result Canvas::sync() noexcept
{
    return pImpl->sync();
}

#endif /* LV_USE_THORVG_INTERNAL */

