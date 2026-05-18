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
#include "tvgLoadModule.h"

#ifdef THORVG_SW_RASTER_SUPPORT
    #include "tvgSwRenderer.h"
#else
    class SwRenderer : public RenderMethod
    {
        //Не поддерживается. Фиктивный класс */
    };
#endif

/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/

struct SwCanvas::Impl
{
};


/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/

#ifdef THORVG_SW_RASTER_SUPPORT
SwCanvas::SwCanvas() : Canvas(SwRenderer::gen()), pImpl(nullptr)
#else
SwCanvas::SwCanvas() : Canvas(nullptr), pImpl(nullptr)
#endif
{
}


SwCanvas::~SwCanvas()
{
    delete(pImpl);
}


Result SwCanvas::mempool(MempoolPolicy policy) noexcept
{
#ifdef THORVG_SW_RASTER_SUPPORT
    //Мы знаем тип рендерера, избегайте dynamic_cast для повышения производительности.
    auto renderer = static_cast<SwRenderer*>(Canvas::pImpl->renderer);
    if (!renderer) return Result::MemoryCorruption;

    //Он не может изменить политику во время работы.
    if (!Canvas::pImpl->paints.empty()) return Result::InsufficientCondition;

    if (policy == MempoolPolicy::Individual) renderer->mempool(false);
    else renderer->mempool(true);

    return Result::Success;
#endif
    return Result::NonSupport;
}


Result SwCanvas::target(uint32_t* buffer, uint32_t stride, uint32_t w, uint32_t h, Colorspace cs) noexcept
{
#ifdef THORVG_SW_RASTER_SUPPORT
    if (Canvas::pImpl->status != Status::Damaged && Canvas::pImpl->status != Status::Synced) {
        return Result::InsufficientCondition;
    }

    //Мы знаем тип рендерера, избегайте dynamic_cast для повышения производительности.
    auto renderer = static_cast<SwRenderer*>(Canvas::pImpl->renderer);
    if (!renderer) return Result::MemoryCorruption;

    if (!renderer->target(buffer, stride, w, h, static_cast<ColorSpace>(cs))) return Result::InvalidArguments;
    Canvas::pImpl->vport = {0, 0, (int32_t)w, (int32_t)h};
    renderer->viewport(Canvas::pImpl->vport);

    //FIXME: The value must be associated with an individual canvas instance.
    ImageLoader::cs = static_cast<ColorSpace>(cs);

    //Краски необходимо снова обновить с учетом этой новой цели.
    Canvas::pImpl->status = Status::Damaged;

    return Result::Success;
#endif
    return Result::NonSupport;
}


unique_ptr<SwCanvas> SwCanvas::gen() noexcept
{
#ifdef THORVG_SW_RASTER_SUPPORT
    if (SwRenderer::init() <= 0) return nullptr;
    return unique_ptr<SwCanvas>(new SwCanvas);
#endif
    return nullptr;
}

#endif /* LV_USE_THORVG_INTERNAL */

