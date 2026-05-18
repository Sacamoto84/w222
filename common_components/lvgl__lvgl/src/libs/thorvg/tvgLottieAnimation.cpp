/*
 * Copyright (c) 2024 the ThorVG project. All rights reserved.

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

#include "tvgCommon.h"
#include "thorvg_lottie.h"
#include "tvgLottieLoader.h"
#include "tvgAnimation.h"


/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/

/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/

LottieAnimation::~LottieAnimation()
{
}


Result LottieAnimation::override(const char* slot) noexcept
{
    if (!pImpl->picture->pImpl->loader) return Result::InsufficientCondition;

    if (static_cast<LottieLoader*>(pImpl->picture->pImpl->loader)->override(slot)) return Result::Success;

    return Result::InvalidArguments;
}


Result LottieAnimation::segment(const char* marker) noexcept
{
    auto loader = pImpl->picture->pImpl->loader;
    if (!loader) return Result::InsufficientCondition;

    if (!marker) {
        static_cast<FrameModule*>(loader)->segment(0.0f, 1.0f);
        return Result::Success;
    }
    
    float begin, end;
    if (!static_cast<LottieLoader*>(loader)->segment(marker, begin, end)) return Result::InvalidArguments;

    return static_cast<Animation*>(this)->segment(begin, end);
}


uint32_t LottieAnimation::markersCnt() noexcept
{
    auto loader = pImpl->picture->pImpl->loader;
    if (!loader) return 0;
    return static_cast<LottieLoader*>(loader)->markersCnt();
}


const char* LottieAnimation::marker(uint32_t idx) noexcept
{
    auto loader = pImpl->picture->pImpl->loader;
    if (!loader) return nullptr;
    return static_cast<LottieLoader*>(loader)->markers(idx);
}


unique_ptr<LottieAnimation> LottieAnimation::gen() noexcept
{
    return unique_ptr<LottieAnimation>(new LottieAnimation);
}

#endif /* LV_USE_THORVG_INTERNAL */

