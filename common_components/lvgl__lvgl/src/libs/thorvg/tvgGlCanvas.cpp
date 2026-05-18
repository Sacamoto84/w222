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

#ifdef THORVG_GL_RASTER_SUPPORT
    #include "tvgGlRenderer.h"
#else
    class GlRenderer : public RenderMethod
    {
        //Не поддерживается. Фиктивный класс */
    };
#endif

/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/

struct GlCanvas::Impl
{
};


/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/

#ifdef THORVG_GL_RASTER_SUPPORT
GlCanvas::GlCanvas() : Canvas(GlRenderer::gen()), pImpl(nullptr)
#else
GlCanvas::GlCanvas() : Canvas(nullptr), pImpl(nullptr)
#endif
{
}


GlCanvas::~GlCanvas()
{
    delete(pImpl);
}


Result GlCanvas::target(int32_t id, uint32_t w, uint32_t h) noexcept
{
#ifdef THORVG_GL_RASTER_SUPPORT
    if (Canvas::pImpl->status != Status::Damaged && Canvas::pImpl->status != Status::Synced) {
        return Result::InsufficientCondition;
    }

    //Мы знаем тип рендерера, избегайтеdynamic_castдля повышения производительности.
    auto renderer = static_cast<GlRenderer*>(Canvas::pImpl->renderer);
    if (!renderer) return Result::MemoryCorruption;

    if (!renderer->target(id, w, h)) return Result::Unknown;
    Canvas::pImpl->vport = {0, 0, (int32_t)w, (int32_t)h};
    renderer->viewport(Canvas::pImpl->vport);

    //Краски необходимо снова обновить с учетом этой новой цели.
    Canvas::pImpl->status = Status::Damaged;

    return Result::Success;
#endif
    return Result::NonSupport;
}


unique_ptr<GlCanvas> GlCanvas::gen() noexcept
{
#ifdef THORVG_GL_RASTER_SUPPORT
    if (GlRenderer::init() <= 0) return nullptr;
    return unique_ptr<GlCanvas>(new GlCanvas);
#endif
    return nullptr;
}

#endif /* LV_USE_THORVG_INTERNAL */

