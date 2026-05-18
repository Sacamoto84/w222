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

#include "tvgLottieLoader.h"
#include "tvgLottieModel.h"
#include "tvgLottieParser.h"
#include "tvgLottieBuilder.h"
#include "tvgStr.h"

/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/

void LottieLoader::run(unsigned tid)
{
    //обновить кадр
    if (comp) {
        builder->update(comp, frameNo);
    //первоначальная загрузка
    } else {
        LottieParser parser(content, dirName);
        if (!parser.parse()) return;
        {
            ScopedLock lock(key);
            comp = parser.comp;
        }
        builder->build(comp);

        release();
    }
    rebuild = false;
}


void LottieLoader::release()
{
    if (copy) {
        lv_free((char*)content);
        content = nullptr;
    }
    lv_free(dirName);
    dirName = nullptr;
}


/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/

LottieLoader::LottieLoader() : FrameModule(FileType::Lottie), builder(new LottieBuilder)
{

}


LottieLoader::~LottieLoader()
{
    done();

    release();

    //TODO: correct position?
    delete(comp);
    delete(builder);
}


bool LottieLoader::header()
{
    //Один поток не должен выполнять интенсивные задачи.
    if (TaskScheduler::threads() == 0) {
        LoadModule::read();
        run(0);
        if (comp) {
            w = static_cast<float>(comp->w);
            h = static_cast<float>(comp->h);
            frameDuration = comp->duration();
            frameCnt = comp->frameCnt();
            frameRate = comp->frameRate;
            return true;
        } else {
            return false;
        }
    }

    //Быстро проверьте данный файл Лотти без его анализа, чтобы получить информацию об анимации.
    auto startFrame = 0.0f;
    auto endFrame = 0.0f;
    uint32_t depth = 0;

    auto p = content;

    while (*p != '\0') {
        if (*p == '{') {
            ++depth;
            ++p;
            continue;
        }
        if (*p == '}') {
            --depth;
            ++p;
            continue;
        }
        if (depth != 1) {
            ++p;
            continue;
        }
        //версия.
        if (!strncmp(p, "\"v\":", 4)) {
            p += 4;
            continue;
        }

        //частота кадров
        if (!strncmp(p, "\"fr\":", 5)) {
            p += 5;
            auto e = strstr(p, ",");
            if (!e) e = strstr(p, "}");
            frameRate = strToFloat(p, nullptr);
            p = e;
            continue;
        }

        //начальный кадр
        if (!strncmp(p, "\"ip\":", 5)) {
            p += 5;
            auto e = strstr(p, ",");
            if (!e) e = strstr(p, "}");
            startFrame = strToFloat(p, nullptr);
            p = e;
            continue;
        }

        //конечный кадр
        if (!strncmp(p, "\"op\":", 5)) {
            p += 5;
            auto e = strstr(p, ",");
            if (!e) e = strstr(p, "}");
            endFrame = strToFloat(p, nullptr);
            p = e;
            continue;
        }

        //ширина
        if (!strncmp(p, "\"w\":", 4)) {
            p += 4;
            auto e = strstr(p, ",");
            if (!e) e = strstr(p, "}");
            w = strToFloat(p, nullptr);
            p = e;
            continue;
        }
        //высота
        if (!strncmp(p, "\"h\":", 4)) {
            p += 4;
            auto e = strstr(p, ",");
            if (!e) e = strstr(p, "}");
            h = strToFloat(p, nullptr);
            p = e;
            continue;
        }
        ++p;
    }

    if (frameRate < FLOAT_EPSILON) {
        TVGLOG("LOTTIE", "Not a Lottie file? Frame rate is 0!");
        return false;
    }

    frameCnt = (endFrame - startFrame);
    frameDuration = frameCnt / frameRate;

    TVGLOG("LOTTIE", "info: frame rate = %f, duration = %f size = %f x %f", frameRate, frameDuration, w, h);

    return true;
}


bool LottieLoader::open(const char* data, uint32_t size, bool copy)
{
    if (copy) {
        content = (char*)lv_malloc(size + 1);
        LV_ASSERT_MALLOC(content);
        if (!content) return false;
        memcpy((char*)content, data, size);
        const_cast<char*>(content)[size] = '\0';
    } else content = data;

    this->dirName = lv_strdup(".");

    this->size = size;
    this->copy = copy;

    return header();
}


bool LottieLoader::open(const string& path)
{
    auto f = fopen(path.c_str(), "r");
    if (!f) return false;

    fseek(f, 0, SEEK_END);

    size = ftell(f);
    if (size == 0) {
        fclose(f);
        return false;
    }

    auto content = (char*)(lv_malloc(sizeof(char) * size + 1));
    LV_ASSERT_MALLOC(content);
    fseek(f, 0, SEEK_SET);
    auto ret = fread(content, sizeof(char), size, f);
    if (ret < size) {
        fclose(f);
        return false;
    }
    content[size] = '\0';

    fclose(f);

    this->dirName = strDirname(path.c_str());
    this->content = content;
    this->copy = true;

    return header();
}


bool LottieLoader::resize(Paint* paint, float w, float h)
{
    if (!paint) return false;

    auto sx = w / this->w;
    auto sy = h / this->h;
    Matrix m = {sx, 0, 0, 0, sy, 0, 0, 0, 1};
    paint->transform(m);

    //нанесите чешуйку на базовую машинку для стрижки
    const Paint* clipper;
    paint->composite(&clipper);
    if (clipper) const_cast<Paint*>(clipper)->transform(m);

    return true;
}


bool LottieLoader::read()
{
    //загрузка уже завершена
    if (!LoadModule::read()) return true;

    if (!content || size == 0) return false;

    TaskScheduler::request(this);

    return true;
}


Paint* LottieLoader::paint()
{
    done();

    if (!comp) return nullptr;
    comp->initiated = true;
    return comp->root->scene;
}


bool LottieLoader::override(const char* slot)
{
    if (!ready() || comp->slots.count == 0) return false;

    auto success = true;

    //переопределить слоты
    if (slot) {
        //Скопируйте входные данные, потому что парсер JSON немедленно закодирует данные.
        auto temp = lv_strdup(slot);

        //анализ слота json
        LottieParser parser(temp, dirName);
        parser.comp = comp;

        auto idx = 0;
        while (auto sid = parser.sid(idx == 0)) {
            for (auto s = comp->slots.begin(); s < comp->slots.end(); ++s) {
                if (strcmp((*s)->sid, sid)) continue;
                if (!parser.apply(*s)) success = false;
                break;
            }
            ++idx;
        }

        if (idx < 1) success = false;
        lv_free(temp);
        rebuild = overridden = success;
    //сбросить слоты
    } else if (overridden) {
        for (auto s = comp->slots.begin(); s < comp->slots.end(); ++s) {
            (*s)->reset();
        }
        overridden = false;
        rebuild = true;
    }
    return success;
}


bool LottieLoader::frame(float no)
{
    auto frameNo = no + startFrame();

    //Это гарантирует достижение целевого номера кадра.
    frameNo *= 10000.0f;
    frameNo = nearbyintf(frameNo);
    frameNo *= 0.0001f;

    //Пропустить обновление, если разница кадров слишком мала.
    if (fabsf(this->frameNo - frameNo) <= 0.0009f) return false;

    this->done();

    this->frameNo = frameNo;

    TaskScheduler::request(this);

    return true;
}


float LottieLoader::startFrame()
{
    return frameCnt * segmentBegin;
}


float LottieLoader::totalFrame()
{
    return (segmentEnd - segmentBegin) * frameCnt;
}


float LottieLoader::curFrame()
{
    return frameNo - startFrame();
}


float LottieLoader::duration()
{
    if (segmentBegin == 0.0f && segmentEnd == 1.0f) return frameDuration;
    return frameCnt * (segmentEnd - segmentBegin) / frameRate;
}


void LottieLoader::sync()
{
    done();

    if (rebuild) run(0);
}


uint32_t LottieLoader::markersCnt()
{
    return ready() ? comp->markers.count : 0;
}


const char* LottieLoader::markers(uint32_t index)
{
    if (!ready() || index >= comp->markers.count) return nullptr;
    auto marker = comp->markers.begin() + index;
    return (*marker)->name;
}


bool LottieLoader::segment(const char* marker, float& begin, float& end)
{
    if (!ready() || comp->markers.count == 0) return false;

    for (auto m = comp->markers.begin(); m < comp->markers.end(); ++m) {
        if (!strcmp(marker, (*m)->name)) {
            begin = (*m)->time / frameCnt;
            end = ((*m)->time + (*m)->duration) / frameCnt;
            return true;
        }
    }
    return false;
}


bool LottieLoader::ready()
{
    {
        ScopedLock lock(key);
        if (comp) return true;
    }
    done();
    if (comp) return true;
    return false;
}

#endif /* LV_USE_THORVG_INTERNAL */

