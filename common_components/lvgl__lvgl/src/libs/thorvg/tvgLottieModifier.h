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

#ifndef _TVG_LOTTIE_MODIFIER_H_
#define _TVG_LOTTIE_MODIFIER_H_

#include "tvgCommon.h"
#include "tvgArray.h"
#include "tvgMath.h"


struct LottieRoundnessModifier
{
    static constexpr float ROUNDNESS_EPSILON = 1.0f;
    float r;

    LottieRoundnessModifier(float r) : r(r) {};

    bool modifyPath(const PathCommand* inCmds, uint32_t inCmdsCnt, const Point* inPts, uint32_t inPtsCnt, Array<PathCommand>& outCmds, Array<Point>& outPts, Matrix* transform) const;
    bool modifyPolystar(const Array<PathCommand>& inCmds, const Array<Point>& inPts, Array<PathCommand>& outCmds, Array<Point>& outPts, float outerRoundness, bool hasRoundness) const;
    bool modifyRect(const Point& size, float& r) const;
};


struct LottieOffsetModifier
{
    float offset;
    float miterLimit;
    StrokeJoin join;

    LottieOffsetModifier(float offset, float miter = 4.0f, StrokeJoin join = StrokeJoin::Round) : offset(offset), miterLimit(miter), join(join) {};

    bool modifyPath(const PathCommand* inCmds, uint32_t inCmdsCnt, const Point* inPts, uint32_t inPtsCnt, Array<PathCommand>& outCmds, Array<Point>& outPts) const;
    bool modifyPolystar(const Array<PathCommand>& inCmds, const Array<Point>& inPts, Array<PathCommand>& outCmds, Array<Point>& outPts) const;
    bool modifyRect(const PathCommand* inCmds, uint32_t inCmdsCnt, const Point* inPts, uint32_t inPtsCnt, Array<PathCommand>& outCmds, Array<Point>& outPts) const;
    bool modifyEllipse(float& rx, float& ry) const;

private:
    struct State
    {
        Line line{};
        Line firstLine{};
        bool moveto = false;
        uint32_t movetoOutIndex = 0;
        uint32_t movetoInIndex = 0;
    };

    void line(const PathCommand* inCmds, uint32_t inCmdsCnt, const Point* inPts, uint32_t& currentPt, uint32_t currentCmd, State& state, bool degenerated, Array<PathCommand>& cmds, Array<Point>& pts, float offset) const;
    void corner(const Line& line, const Line& nextLine, uint32_t movetoIndex, bool nextClose, Array<PathCommand>& cmds, Array<Point>& pts) const;
};

#endif

#endif /* LV_USE_THORVG_INTERNAL */

