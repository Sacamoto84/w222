/*
 * Copyright (c) 2021 - 2024 the ThorVG project. All rights reserved.

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

#include "tvgCommon.h"
#include "tvgSaveModule.h"
#include "tvgPaint.h"

#ifdef THORVG_TVG_SAVER_SUPPORT
    #include "tvgTvgSaver.h"
#endif
#ifdef THORVG_GIF_SAVER_SUPPORT
    #include "tvgGifSaver.h"
#endif

/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/

struct Saver::Impl
{
    SaveModule* saveModule = nullptr;
    Paint* bg = nullptr;

    ~Impl()
    {
        delete(saveModule);
        delete(bg);
    }
};


static SaveModule* _find(FileType type)
{
    switch(type) {
        case FileType::Tvg: {
#ifdef THORVG_TVG_SAVER_SUPPORT
            return new TvgSaver;
#endif
            break;
        }
        case FileType::Gif: {
#ifdef THORVG_GIF_SAVER_SUPPORT
            return new GifSaver;
#endif
            break;
        }
        default: {
            break;
        }
    }

#ifdef THORVG_LOG_ENABLED
    const char *format;
    switch(type) {
        case FileType::Tvg: {
            format = "TVG";
            break;
        }
        case FileType::Gif: {
            format = "GIF";
            break;
        }
        default: {
            format = "???";
            break;
        }
    }
    TVGLOG("RENDERER", "%s format is not supported", format);
#endif
    return nullptr;
}


static SaveModule* _find(const string& path)
{
    auto ext = path.substr(path.find_last_of(".") + 1);
    if (!ext.compare("tvg")) {
        return _find(FileType::Tvg);
    } else if (!ext.compare("gif")) {
        return _find(FileType::Gif);
    }
    return nullptr;
}


/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/

Saver::Saver() : pImpl(new Impl())
{
}


Saver::~Saver()
{
    delete(pImpl);
}


Result Saver::save(std::unique_ptr<Paint> paint, const string& path, bool compress) noexcept
{
    auto p = paint.release();
    if (!p) return Result::MemoryCorruption;

    //Уже на сохранении другого ресурса.
    if (pImpl->saveModule) {
        if (P(p)->refCnt == 0) delete(p);
        return Result::InsufficientCondition;
    }

    if (auto saveModule = _find(path)) {
        if (saveModule->save(p, path, compress)) {
            pImpl->saveModule = saveModule;
            return Result::Success;
        } else {
            if (P(p)->refCnt == 0) delete(p);
            delete(saveModule);
            return Result::Unknown;
        }
    }
    if (P(p)->refCnt == 0) delete(p);
    return Result::NonSupport;
}


Result Saver::background(unique_ptr<Paint> paint) noexcept
{
    delete(pImpl->bg);
    pImpl->bg = paint.release();

    return Result::Success;
}


Result Saver::save(unique_ptr<Animation> animation, const string& path, uint32_t quality, uint32_t fps) noexcept
{
    auto a = animation.release();
    if (!a) return Result::MemoryCorruption;

    //анимация удерживает изображение, внизу должно быть 1.
    auto remove = PP(a->picture())->refCnt <= 1 ? true : false;

    if (tvg::zero(a->totalFrame())) {
        if (remove) delete(a);
        return Result::InsufficientCondition;
    }

    //Уже на сохранении другого ресурса.
    if (pImpl->saveModule) {
        if (remove) delete(a);
        return Result::InsufficientCondition;
    }

    if (auto saveModule = _find(path)) {
        if (saveModule->save(a, pImpl->bg, path, quality, fps)) {
            pImpl->saveModule = saveModule;
            return Result::Success;
        } else {
            if (remove) delete(a);
            delete(saveModule);
            return Result::Unknown;
        }
    }
    if (remove) delete(a);
    return Result::NonSupport;
}


Result Saver::sync() noexcept
{
    if (!pImpl->saveModule) return Result::InsufficientCondition;
    pImpl->saveModule->close();
    delete(pImpl->saveModule);
    pImpl->saveModule = nullptr;

    return Result::Success;
}


unique_ptr<Saver> Saver::gen() noexcept
{
    return unique_ptr<Saver>(new Saver);
}

#endif /* LV_USE_THORVG_INTERNAL */

