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

#ifndef _TVG_FRAME_MODULE_H_
#define _TVG_FRAME_MODULE_H_

#include "tvgLoadModule.h"

namespace tvg
{

class FrameModule: public ImageLoader
{
public:
    float segmentBegin = 0.0f;
    float segmentEnd = 1.0f;

    FrameModule(FileType type) : ImageLoader(type) {}
    virtual ~FrameModule() {}

    virtual bool frame(float no) = 0;       //установить текущий номер кадра
    virtual float totalFrame() = 0;         //вернуть общее количество кадров
    virtual float curFrame() = 0;           //вернуть текущий номер кадра
    virtual float duration() = 0;           //вернуть продолжительность анимации в секундах

    void segment(float* begin, float* end)
    {
        if (begin) *begin = segmentBegin;
        if (end) *end = segmentEnd;
    }

    void segment(float begin, float end)
    {
        segmentBegin = begin;
        segmentEnd = end;
    }

    virtual bool animatable() override { return true; }
};

}

#endif //_TVG_FRAME_MODULE_H_

#endif /* LV_USE_THORVG_INTERNAL */

