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

#include "tvgCanvas.h"

#ifdef THORVG_WG_RASTER_SUPPORT
    #include "tvgWgRenderer.h"
#endif

/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/

struct WgCanvas::Impl
{
};


/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/

#ifdef THORVG_WG_RASTER_SUPPORT
WgCanvas::WgCanvas() : Canvas(WgRenderer::gen()), pImpl(nullptr)
#else
WgCanvas::WgCanvas() : Canvas(nullptr), pImpl(nullptr)
#endif
{
}


WgCanvas::~WgCanvas()
{
#ifdef THORVG_WG_RASTER_SUPPORT
    auto renderer = static_cast<WgRenderer*>(Canvas::pImpl->renderer);
    renderer->target(nullptr, 0, 0);
#endif
}


Result WgCanvas::target(void* instance, void* surface, uint32_t w, uint32_t h, void* device) noexcept
{
#ifdef THORVG_WG_RASTER_SUPPORT
    if (Canvas::pImpl->status != Status::Damaged && Canvas::pImpl->status != Status::Synced) {
        return Result::InsufficientCondition;
    }

    if (!instance || !surface || (w == 0) || (h == 0)) return Result::InvalidArguments;

    //Мы знаем тип рендерера, избегайтеdynamic_castдля повышения производительности.
    auto renderer = static_cast<WgRenderer*>(Canvas::pImpl->renderer);
    if (!renderer) return Result::MemoryCorruption;

    if (!renderer->target((WGPUInstance)instance, (WGPUSurface)surface, w, h, (WGPUDevice)device)) return Result::Unknown;
    Canvas::pImpl->vport = {0, 0, (int32_t)w, (int32_t)h};
    renderer->viewport(Canvas::pImpl->vport);

    //Краски необходимо снова обновить с учетом этой новой цели.
    Canvas::pImpl->status = Status::Damaged;

    return Result::Success;
#endif
    return Result::NonSupport;
}


unique_ptr<WgCanvas> WgCanvas::gen() noexcept
{
#ifdef THORVG_WG_RASTER_SUPPORT
    return unique_ptr<WgCanvas>(new WgCanvas);
#endif
    return nullptr;
}

#endif /* LV_USE_THORVG_INTERNAL */

