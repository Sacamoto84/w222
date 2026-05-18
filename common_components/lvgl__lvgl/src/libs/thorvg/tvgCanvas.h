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

#ifndef _TVG_CANVAS_H_
#define _TVG_CANVAS_H_

#include "tvgPaint.h"

using std::list;

enum Status : uint8_t {Synced = 0, Updating, Drawing, Damaged};

struct Canvas::Impl
{
    list<Paint*> paints;
    RenderMethod* renderer;
    RenderRegion vport = {0, 0, INT32_MAX, INT32_MAX};
    Status status = Status::Synced;

    Impl(RenderMethod* pRenderer) : renderer(pRenderer)
    {
        renderer->ref();
    }

    ~Impl()
    {
        //убедитесь, что все отложенные задания
        renderer->sync();
        renderer->clear();

        clearPaints();

        if (renderer->unref() == 0) delete(renderer);
    }

    void clearPaints()
    {
        for (auto paint : paints) {
            if (P(paint)->unref() == 0) delete(paint);
        }
        paints.clear();
    }

    Result push(unique_ptr<Paint> paint)
    {
        //Вы не можете нажимать краски во время рендеринга.
        if (status == Status::Drawing) return Result::InsufficientCondition;

        auto p = paint.release();
        if (!p) return Result::MemoryCorruption;
        PP(p)->ref();
        paints.push_back(p);

        return update(p, true);
    }

    Result clear(bool free)
    {
        //Очистите цель рендеринга перед рисованием
        if (!renderer->clear()) return Result::InsufficientCondition;

        //Бесплатные краски
        if (free) clearPaints();

        status = Status::Synced;

        return Result::Success;
    }

    Result update(Paint* paint, bool force)
    {
        if (paints.empty() || status == Status::Drawing) return Result::InsufficientCondition;

        Array<RenderData> clips;
        auto flag = RenderUpdateFlag::None;
        if (status == Status::Damaged || force) flag = RenderUpdateFlag::All;

        auto m = Matrix{1, 0, 0, 0, 1, 0, 0, 0, 1};

        if (paint) {
            paint->pImpl->update(renderer, m, clips, 255, flag);
        } else {
            for (auto paint : paints) {
                paint->pImpl->update(renderer, m, clips, 255, flag);
            }
        }
        status = Status::Updating;
        return Result::Success;
    }

    Result draw()
    {
        if (status == Status::Damaged) update(nullptr, false);
        if (status == Status::Drawing || paints.empty() || !renderer->preRender()) return Result::InsufficientCondition;

        bool rendered = false;
        for (auto paint : paints) {
            if (paint->pImpl->render(renderer)) rendered = true;
        }

        if (!rendered || !renderer->postRender()) return Result::InsufficientCondition;

        status = Status::Drawing;
        return Result::Success;
    }

    Result sync()
    {
        if (status == Status::Synced || status == Status::Damaged) return Result::InsufficientCondition;

        if (renderer->sync()) {
            status = Status::Synced;
            return Result::Success;
        }

        return Result::Unknown;
    }

    Result viewport(int32_t x, int32_t y, int32_t w, int32_t h)
    {
        if (status != Status::Damaged && status != Status::Synced) return Result::InsufficientCondition;

        RenderRegion val = {x, y, w, h};
        //пересекаются, если целевой буфер уже установлен.
        auto surface = renderer->mainSurface();
        if (surface && surface->w > 0 && surface->h > 0) {
            val.intersect({0, 0, (int32_t)surface->w, (int32_t)surface->h});
        }
        if (vport == val) return Result::Success;
        renderer->viewport(val);
        vport = val;
        status = Status::Damaged;
        return Result::Success;
    }
};

#endif /* _TVG_CANVAS_H_ */

#endif /* LV_USE_THORVG_INTERNAL */

