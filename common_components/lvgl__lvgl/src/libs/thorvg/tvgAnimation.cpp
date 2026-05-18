/*
 * Copyright (c) 2023 - 2024 the ThorVG project. All rights reserved.

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

#include "tvgFrameModule.h"
#include "tvgAnimation.h"

/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/

/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/

Animation::~Animation()
{
    delete(pImpl);
}


Animation::Animation() : pImpl(new Impl)
{
}


Result Animation::frame(float no) noexcept
{
    auto loader = pImpl->picture->pImpl->loader;

    if (!loader) return Result::InsufficientCondition;
    if (!loader->animatable()) return Result::NonSupport;

    if (static_cast<FrameModule*>(loader)->frame(no)) return Result::Success;
    return Result::InsufficientCondition;
}


Picture* Animation::picture() const noexcept
{
    return pImpl->picture;
}


float Animation::curFrame() const noexcept
{
    auto loader = pImpl->picture->pImpl->loader;

    if (!loader) return 0;
    if (!loader->animatable()) return 0;

    return static_cast<FrameModule*>(loader)->curFrame();
}


float Animation::totalFrame() const noexcept
{
    auto loader = pImpl->picture->pImpl->loader;

    if (!loader) return 0;
    if (!loader->animatable()) return 0;

    return static_cast<FrameModule*>(loader)->totalFrame();
}


float Animation::duration() const noexcept
{
    auto loader = pImpl->picture->pImpl->loader;

    if (!loader) return 0;
    if (!loader->animatable()) return 0;

    return static_cast<FrameModule*>(loader)->duration();
}


Result Animation::segment(float begin, float end) noexcept
{
    if (begin < 0.0 || end > 1.0 || begin > end) return Result::InvalidArguments;

    auto loader = pImpl->picture->pImpl->loader;
    if (!loader) return Result::InsufficientCondition;
    if (!loader->animatable()) return Result::NonSupport;

    static_cast<FrameModule*>(loader)->segment(begin, end);

    return Result::Success;
}


Result Animation::segment(float *begin, float *end) noexcept
{
    auto loader = pImpl->picture->pImpl->loader;
    if (!loader) return Result::InsufficientCondition;
    if (!loader->animatable()) return Result::NonSupport;
    if (!begin && !end) return Result::InvalidArguments;

    static_cast<FrameModule*>(loader)->segment(begin, end);

    return Result::Success;
}


unique_ptr<Animation> Animation::gen() noexcept
{
    return unique_ptr<Animation>(new Animation);
}

#endif /* LV_USE_THORVG_INTERNAL */

