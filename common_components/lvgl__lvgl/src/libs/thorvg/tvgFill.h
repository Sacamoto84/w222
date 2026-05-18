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

#ifndef _TVG_FILL_H_
#define _TVG_FILL_H_

#include <cstdlib>
#include <cstring>
#include "tvgCommon.h"

template<typename T>
struct DuplicateMethod
{
    virtual ~DuplicateMethod() {}
    virtual T* duplicate() = 0;
};

template<class T>
struct FillDup : DuplicateMethod<Fill>
{
    T* inst = nullptr;

    FillDup(T* _inst) : inst(_inst) {}
    ~FillDup() {}

    Fill* duplicate() override
    {
        return inst->duplicate();
    }
};

struct Fill::Impl
{
    ColorStop* colorStops = nullptr;
    Matrix* transform = nullptr;
    uint32_t cnt = 0;
    FillSpread spread;
    DuplicateMethod<Fill>* dup = nullptr;

    ~Impl()
    {
        delete(dup);
        lv_free(colorStops);
        lv_free(transform);
    }

    void method(DuplicateMethod<Fill>* dup)
    {
        this->dup = dup;
    }

    Fill* duplicate()
    {
        auto ret = dup->duplicate();
        if (!ret) return nullptr;

        ret->pImpl->cnt = cnt;
        ret->pImpl->spread = spread;
        ret->pImpl->colorStops = static_cast<ColorStop*>(lv_malloc(sizeof(ColorStop) * cnt));
        LV_ASSERT_MALLOC(ret->pImpl->colorStops);
        memcpy(ret->pImpl->colorStops, colorStops, sizeof(ColorStop) * cnt);
        if (transform) {
            ret->pImpl->transform = static_cast<Matrix*>(lv_malloc(sizeof(Matrix)));
			LV_ASSERT_MALLOC(ret->pImpl->transform);
            *ret->pImpl->transform = *transform;
        }
        return ret;
    }
};


struct RadialGradient::Impl
{
    float cx = 0.0f, cy = 0.0f;
    float fx = 0.0f, fy = 0.0f;
    float r = 0.0f, fr = 0.0f;

    Fill* duplicate();
    Result radial(float cx, float cy, float r, float fx, float fy, float fr);
};


struct LinearGradient::Impl
{
    float x1 = 0.0f;
    float y1 = 0.0f;
    float x2 = 0.0f;
    float y2 = 0.0f;

    Fill* duplicate();
};


#endif  //_TVG_FILL_H_

#endif /* LV_USE_THORVG_INTERNAL */

