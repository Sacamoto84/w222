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

#ifndef _TVG_LOTTIE_BUILDER_H_
#define _TVG_LOTTIE_BUILDER_H_

#include "tvgCommon.h"
#include "tvgInlist.h"
#include "tvgPaint.h"
#include "tvgShape.h"
#include "tvgLottieExpressions.h"
#include "tvgLottieModifier.h"

struct LottieComposition;

struct RenderRepeater
{
    int cnt;
    Matrix transform;
    float offset;
    Point position;
    Point anchor;
    Point scale;
    float rotation;
    uint8_t startOpacity;
    uint8_t endOpacity;
    bool interpOpacity;
    bool inorder;
};

struct RenderContext
{
    INLIST_ITEM(RenderContext);

    Shape* propagator = nullptr;  //для распространения свойств формы, за исключением путей
    Shape* merging = nullptr;  //объединение фигур, если это возможно (если фигуры имеют одинаковые свойства)
    LottieObject** begin = nullptr; //точка входа в итерацию
    Array<RenderRepeater> repeaters;
    Matrix* transform = nullptr;
    LottieRoundnessModifier* roundness = nullptr;
    LottieOffsetModifier* offsetPath = nullptr;
    bool fragmenting = false;  //контекст рендеринга был фрагментирован путем заполнения
    bool reqFragment = false;  //требование фрагментировать контекст рендеринга

    RenderContext(Shape* propagator)
    {
        P(propagator)->reset();
        PP(propagator)->ref();
        this->propagator = propagator;
    }

    ~RenderContext()
    {
        PP(propagator)->unref();
        lv_free(transform);
        delete(roundness);
        delete(offsetPath);
    }

    RenderContext(const RenderContext& rhs, Shape* propagator, bool mergeable = false)
    {
        if (mergeable) merging = rhs.merging;
        PP(propagator)->ref();
        this->propagator = propagator;
        this->repeaters = rhs.repeaters;
        if (rhs.roundness) this->roundness = new LottieRoundnessModifier(rhs.roundness->r);
        if (rhs.offsetPath) this->offsetPath = new LottieOffsetModifier(rhs.offsetPath->offset, rhs.offsetPath->miterLimit, rhs.offsetPath->join);
    }
};

struct LottieBuilder
{
    LottieBuilder()
    {
        exps = LottieExpressions::instance();
    }

    ~LottieBuilder()
    {
        LottieExpressions::retrieve(exps);
    }

    bool update(LottieComposition* comp, float progress);
    void build(LottieComposition* comp);

private:
    void updateEffect(LottieLayer* layer, float frameNo);
    void updateLayer(LottieComposition* comp, Scene* scene, LottieLayer* layer, float frameNo);
    bool updateMatte(LottieComposition* comp, float frameNo, Scene* scene, LottieLayer* layer);
    void updatePrecomp(LottieComposition* comp, LottieLayer* precomp, float frameNo);
    void updateSolid(LottieLayer* layer);
    void updateImage(LottieGroup* layer);
    void updateText(LottieLayer* layer, float frameNo);
    void updateMaskings(LottieLayer* layer, float frameNo);
    void updateTransform(LottieLayer* layer, float frameNo);
    void updateChildren(LottieGroup* parent, float frameNo, Inlist<RenderContext>& contexts);
    void updateGroup(LottieGroup* parent, LottieObject** child, float frameNo, Inlist<RenderContext>& pcontexts, RenderContext* ctx);
    void updateTransform(LottieGroup* parent, LottieObject** child, float frameNo, Inlist<RenderContext>& contexts, RenderContext* ctx);
    void updateSolidFill(LottieGroup* parent, LottieObject** child, float frameNo, Inlist<RenderContext>& contexts, RenderContext* ctx);
    void updateSolidStroke(LottieGroup* parent, LottieObject** child, float frameNo, Inlist<RenderContext>& contexts, RenderContext* ctx);
    void updateGradientFill(LottieGroup* parent, LottieObject** child, float frameNo, Inlist<RenderContext>& contexts, RenderContext* ctx);
    void updateGradientStroke(LottieGroup* parent, LottieObject** child, float frameNo, Inlist<RenderContext>& contexts, RenderContext* ctx);
    void updateRect(LottieGroup* parent, LottieObject** child, float frameNo, Inlist<RenderContext>& contexts, RenderContext* ctx);
    void updateEllipse(LottieGroup* parent, LottieObject** child, float frameNo, Inlist<RenderContext>& contexts, RenderContext* ctx);
    void updatePath(LottieGroup* parent, LottieObject** child, float frameNo, Inlist<RenderContext>& contexts, RenderContext* ctx);
    void updatePolystar(LottieGroup* parent, LottieObject** child, float frameNo, Inlist<RenderContext>& contexts, RenderContext* ctx);
    void updateTrimpath(LottieGroup* parent, LottieObject** child, float frameNo, Inlist<RenderContext>& contexts, RenderContext* ctx);
    void updateRepeater(LottieGroup* parent, LottieObject** child, float frameNo, Inlist<RenderContext>& contexts, RenderContext* ctx);
    void updateRoundedCorner(LottieGroup* parent, LottieObject** child, float frameNo, Inlist<RenderContext>& contexts, RenderContext* ctx);
    void updateOffsetPath(LottieGroup* parent, LottieObject** child, float frameNo, Inlist<RenderContext>& contexts, RenderContext* ctx);

    LottieExpressions* exps;
};

#endif //_TVG_LOTTIE_BUILDER_H

#endif /* LV_USE_THORVG_INTERNAL */

