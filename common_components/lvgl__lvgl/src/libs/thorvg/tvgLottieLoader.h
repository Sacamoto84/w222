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

#ifndef _TVG_LOTTIE_LOADER_H_
#define _TVG_LOTTIE_LOADER_H_

#include "tvgCommon.h"
#include "tvgFrameModule.h"
#include "tvgTaskScheduler.h"

struct LottieComposition;
struct LottieBuilder;

class LottieLoader : public FrameModule, public Task
{
public:
    const char* content = nullptr;      //данные файла лотереи
    uint32_t size = 0;                  //размер данных лотереи
    float frameNo = 0.0f;               //текущий номер кадра
    float frameCnt = 0.0f;
    float frameDuration = 0.0f;
    float frameRate = 0.0f;

    LottieBuilder* builder;
    LottieComposition* comp = nullptr;

    Key key;
    char* dirName = nullptr;            //базовый каталог ресурсов
    bool copy = false;                  //«контент» принадлежит этому загрузчику
    bool overridden = false;             //переопределенные свойства со слотами
    bool rebuild = false;               //требуется построить сцену лотереи

    LottieLoader();
    ~LottieLoader();

    bool open(const string& path) override;
    bool open(const char* data, uint32_t size, bool copy) override;
    bool resize(Paint* paint, float w, float h) override;
    bool read() override;
    Paint* paint() override;
    bool override(const char* slot);

    //Элементы управления кадром
    bool frame(float no) override;
    float totalFrame() override;
    float curFrame() override;
    float duration() override;
    void sync() override;

    //Маркерные опоры
    uint32_t markersCnt();
    const char* markers(uint32_t index);
    bool segment(const char* marker, float& begin, float& end);

private:
    bool ready();
    bool header();
    void clear();
    float startFrame();
    void run(unsigned tid) override;
    void release();
};


#endif //_TVG_LOTTIELOADER_H_

#endif /* LV_USE_THORVG_INTERNAL */

