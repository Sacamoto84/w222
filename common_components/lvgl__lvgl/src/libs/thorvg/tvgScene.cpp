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

#include <cstdarg>
#include "tvgScene.h"

/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/

Result Scene::Impl::resetEffects()
{
    if (effects) {
        for (auto e = effects->begin(); e < effects->end(); ++e) {
            delete(*e);
        }
        delete(effects);
        effects = nullptr;
    }
    return Result::Success;
}


/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/

Scene::Scene() : pImpl(new Impl(this))
{
}


Scene::~Scene()
{
    delete(pImpl);
}


unique_ptr<Scene> Scene::gen() noexcept
{
    return unique_ptr<Scene>(new Scene);
}


TVG_DEPRECATED uint32_t Scene::identifier() noexcept
{
    return (uint32_t) Type::Scene;
}


Type Scene::type() const noexcept
{
    return Type::Scene;
}


Result Scene::push(unique_ptr<Paint> paint) noexcept
{
    auto p = paint.release();
    if (!p) return Result::MemoryCorruption;
    PP(p)->ref();
    pImpl->paints.push_back(p);

    return Result::Success;
}


Result Scene::reserve(TVG_UNUSED uint32_t size) noexcept
{
    return Result::NonSupport;
}


Result Scene::clear(bool free) noexcept
{
    pImpl->clear(free);

    return Result::Success;
}


list<Paint*>& Scene::paints() noexcept
{
    return pImpl->paints;
}


Result Scene::push(SceneEffect effect, ...) noexcept
{
    if (effect == SceneEffect::ClearAll) return pImpl->resetEffects();

    if (!pImpl->effects) pImpl->effects = new Array<RenderEffect*>;

    va_list args;
    va_start(args, effect);

    RenderEffect* re = nullptr;

    switch (effect) {
        case SceneEffect::GaussianBlur: {
            re = RenderEffectGaussian::gen(args);
            break;
        }
        default: break;
    }

    if (!re) return Result::InvalidArguments;

    pImpl->effects->push(re);

    return Result::Success;
}

#endif /* LV_USE_THORVG_INTERNAL */

