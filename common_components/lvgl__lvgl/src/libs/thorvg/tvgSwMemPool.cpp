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

#include "tvgSwCommon.h"


/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/


/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/

SwOutline* mpoolReqOutline(SwMpool* mpool, unsigned idx)
{
    return &mpool->outline[idx];
}


void mpoolRetOutline(SwMpool* mpool, unsigned idx)
{
    mpool->outline[idx].pts.clear();
    mpool->outline[idx].cntrs.clear();
    mpool->outline[idx].types.clear();
    mpool->outline[idx].closed.clear();
}


SwOutline* mpoolReqStrokeOutline(SwMpool* mpool, unsigned idx)
{
    return &mpool->strokeOutline[idx];
}


void mpoolRetStrokeOutline(SwMpool* mpool, unsigned idx)
{
    mpool->strokeOutline[idx].pts.clear();
    mpool->strokeOutline[idx].cntrs.clear();
    mpool->strokeOutline[idx].types.clear();
    mpool->strokeOutline[idx].closed.clear();
}


SwOutline* mpoolReqDashOutline(SwMpool* mpool, unsigned idx)
{
    return &mpool->dashOutline[idx];
}


void mpoolRetDashOutline(SwMpool* mpool, unsigned idx)
{
    mpool->dashOutline[idx].pts.clear();
    mpool->dashOutline[idx].cntrs.clear();
    mpool->dashOutline[idx].types.clear();
    mpool->dashOutline[idx].closed.clear();
}


SwMpool* mpoolInit(uint32_t threads)
{
    auto allocSize = threads + 1;

    auto mpool = static_cast<SwMpool*>(lv_zalloc(sizeof(SwMpool)));
    LV_ASSERT_MALLOC(mpool);
    mpool->outline = static_cast<SwOutline*>(lv_zalloc(sizeof(SwOutline) * allocSize));
    LV_ASSERT_MALLOC(mpool->outline);
    mpool->strokeOutline = static_cast<SwOutline*>(lv_zalloc(sizeof(SwOutline) * allocSize));
    LV_ASSERT_MALLOC(mpool->strokeOutline);
    mpool->dashOutline = static_cast<SwOutline*>(lv_zalloc(sizeof(SwOutline) * allocSize));
    LV_ASSERT_MALLOC(mpool->dashOutline);
    mpool->allocSize = allocSize;

    return mpool;
}


bool mpoolClear(SwMpool* mpool)
{
    for (unsigned i = 0; i < mpool->allocSize; ++i) {
        mpool->outline[i].pts.reset();
        mpool->outline[i].cntrs.reset();
        mpool->outline[i].types.reset();
        mpool->outline[i].closed.reset();

        mpool->strokeOutline[i].pts.reset();
        mpool->strokeOutline[i].cntrs.reset();
        mpool->strokeOutline[i].types.reset();
        mpool->strokeOutline[i].closed.reset();

        mpool->dashOutline[i].pts.reset();
        mpool->dashOutline[i].cntrs.reset();
        mpool->dashOutline[i].types.reset();
        mpool->dashOutline[i].closed.reset();
    }

    return true;
}


bool mpoolTerm(SwMpool* mpool)
{
    if (!mpool) return false;

    mpoolClear(mpool);

    lv_free(mpool->outline);
    lv_free(mpool->strokeOutline);
    lv_free(mpool->dashOutline);
    lv_free(mpool);

    return true;
}

#endif /* LV_USE_THORVG_INTERNAL */

