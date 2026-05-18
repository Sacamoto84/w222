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

#ifndef _TVG_SVG_LOADER_H_
#define _TVG_SVG_LOADER_H_

#include "tvgTaskScheduler.h"
#include "tvgSvgLoaderCommon.h"

using std::ifstream;

class SvgLoader : public ImageLoader, public Task
{
public:
    string filePath;
    string svgPath = "";
    char* content = nullptr;
    uint32_t size = 0;

    SvgLoaderData loaderData;
    Scene* root = nullptr;

    bool copy = false;

    SvgLoader();
    ~SvgLoader();

    bool open(const string& path) override;
    bool open(const char* data, uint32_t size, bool copy) override;
    bool resize(Paint* paint, float w, float h) override;
    bool read() override;
    bool close() override;

    Paint* paint() override;

private:
    SvgViewFlag viewFlag = SvgViewFlag::None;
    AspectRatioAlign align = AspectRatioAlign::XMidYMid;
    AspectRatioMeetOrSlice meetOrSlice = AspectRatioMeetOrSlice::Meet;
    float vx = 0;
    float vy = 0;
    float vw = 0;
    float vh = 0;

    bool header();
    void clear(bool all = true);
    void run(unsigned tid) override;
};


#endif //_TVG_SVG_LOADER_H_

#endif /* LV_USE_THORVG_INTERNAL */

