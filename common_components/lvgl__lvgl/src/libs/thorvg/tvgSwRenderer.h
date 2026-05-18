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

#ifndef _TVG_SW_RENDERER_H_
#define _TVG_SW_RENDERER_H_

#include "tvgRender.h"

struct SwSurface;
struct SwTask;
struct SwCompositor;
struct SwMpool;

namespace tvg
{

class SwRenderer : public RenderMethod
{
public:
    RenderData prepare(const RenderShape& rshape, RenderData data, const Matrix& transform, Array<RenderData>& clips, uint8_t opacity, RenderUpdateFlag flags, bool clipper) override;
    RenderData prepare(RenderSurface* surface, RenderData data, const Matrix& transform, Array<RenderData>& clips, uint8_t opacity, RenderUpdateFlag flags) override;
    bool preRender() override;
    bool renderShape(RenderData data) override;
    bool renderImage(RenderData data) override;
    bool postRender() override;
    void dispose(RenderData data) override;
    RenderRegion region(RenderData data) override;
    RenderRegion viewport() override;
    bool viewport(const RenderRegion& vp) override;
    bool blend(BlendMethod method) override;
    ColorSpace colorSpace() override;
    const RenderSurface* mainSurface() override;

    bool clear() override;
    bool sync() override;
    bool target(pixel_t* data, uint32_t stride, uint32_t w, uint32_t h, ColorSpace cs);
    bool mempool(bool shared);

    RenderCompositor* target(const RenderRegion& region, ColorSpace cs) override;
    bool beginComposite(RenderCompositor* cmp, CompositeMethod method, uint8_t opacity) override;
    bool endComposite(RenderCompositor* cmp) override;
    void clearCompositors();

    bool prepare(RenderEffect* effect) override;
    bool effect(RenderCompositor* cmp, const RenderEffect* effect) override;

    static SwRenderer* gen();
    static bool init(uint32_t threads);
    static int32_t init();
    static bool term();

private:
    SwSurface*           surface = nullptr;           //активная поверхность
    Array<SwTask*>       tasks;                       //список асинхронных задач
    Array<SwSurface*>    compositors;                 //список кэша целей рендеринга
    SwMpool*             mpool;                       //частный пул памяти
    RenderRegion         vport;                       //область просмотра
    bool                 sharedMpool = true;          //политика поведения пула памяти

    SwRenderer();
    ~SwRenderer();

    SwSurface* request(int channelSize);
    RenderData prepareCommon(SwTask* task, const Matrix& transform, const Array<RenderData>& clips, uint8_t opacity, RenderUpdateFlag flags);
};

}

#endif /* _TVG_SW_RENDERER_H_ */

#endif /* LV_USE_THORVG_INTERNAL */

