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

#include <string.h>

#include "tvgInlist.h"
#include "tvgLoader.h"
#include "tvgLock.h"

#ifdef THORVG_SVG_LOADER_SUPPORT
    #include "tvgSvgLoader.h"
#endif

#ifdef THORVG_PNG_LOADER_SUPPORT
    #include "tvgPngLoader.h"
#endif

#ifdef THORVG_TVG_LOADER_SUPPORT
    #include "tvgTvgLoader.h"
#endif

#ifdef THORVG_JPG_LOADER_SUPPORT
    #include "tvgJpgLoader.h"
#endif

#ifdef THORVG_WEBP_LOADER_SUPPORT
    #include "tvgWebpLoader.h"
#endif

#ifdef THORVG_TTF_LOADER_SUPPORT
    #include "tvgTtfLoader.h"
#endif

#ifdef THORVG_LOTTIE_LOADER_SUPPORT
    #include "tvgLottieLoader.h"
#endif

#include "tvgRawLoader.h"


uintptr_t HASH_KEY(const char* data)
{
    return reinterpret_cast<uintptr_t>(data);
}

/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/

ColorSpace ImageLoader::cs = ColorSpace::ARGB8888;

static Key key;
static Inlist<LoadModule> _activeLoaders;


static LoadModule* _find(FileType type)
{
    switch(type) {
        case FileType::Png: {
#ifdef THORVG_PNG_LOADER_SUPPORT
            return new PngLoader;
#endif
            break;
        }
        case FileType::Jpg: {
#ifdef THORVG_JPG_LOADER_SUPPORT
            return new JpgLoader;
#endif
            break;
        }
        case FileType::Webp: {
#ifdef THORVG_WEBP_LOADER_SUPPORT
            return new WebpLoader;
#endif
            break;
        }
        case FileType::Tvg: {
#ifdef THORVG_TVG_LOADER_SUPPORT
            return new TvgLoader;
#endif
            break;
        }
        case FileType::Svg: {
#ifdef THORVG_SVG_LOADER_SUPPORT
            return new SvgLoader;
#endif
            break;
        }
        case FileType::Ttf: {
#ifdef THORVG_TTF_LOADER_SUPPORT
            return new TtfLoader;
#endif
            break;
        }
        case FileType::Lottie: {
#ifdef THORVG_LOTTIE_LOADER_SUPPORT
            return new LottieLoader;
#endif
            break;
        }
        case FileType::Raw: {
            return new RawLoader;
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
        case FileType::Svg: {
            format = "SVG";
            break;
        }
        case FileType::Ttf: {
            format = "TTF";
            break;
        }
        case FileType::Lottie: {
            format = "lottie(json)";
            break;
        }
        case FileType::Raw: {
            format = "RAW";
            break;
        }
        case FileType::Png: {
            format = "PNG";
            break;
        }
        case FileType::Jpg: {
            format = "JPG";
            break;
        }
        case FileType::Webp: {
            format = "WEBP";
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


static LoadModule* _findByPath(const string& path)
{
    auto ext = path.substr(path.find_last_of(".") + 1);
    if (!ext.compare("tvg")) return _find(FileType::Tvg);
    if (!ext.compare("svg")) return _find(FileType::Svg);
    if (!ext.compare("json")) return _find(FileType::Lottie);
    if (!ext.compare("png")) return _find(FileType::Png);
    if (!ext.compare("jpg")) return _find(FileType::Jpg);
    if (!ext.compare("webp")) return _find(FileType::Webp);
    if (!ext.compare("ttf") || !ext.compare("ttc")) return _find(FileType::Ttf);
    if (!ext.compare("otf") || !ext.compare("otc")) return _find(FileType::Ttf);
    return nullptr;
}


static FileType _convert(const string& mimeType)
{
    auto type = FileType::Unknown;

    if (mimeType == "tvg") type = FileType::Tvg;
    else if (mimeType == "svg" || mimeType == "svg+xml") type = FileType::Svg;
    else if (mimeType == "ttf" || mimeType == "otf") type = FileType::Ttf;
    else if (mimeType == "lottie") type = FileType::Lottie;
    else if (mimeType == "raw") type = FileType::Raw;
    else if (mimeType == "png") type = FileType::Png;
    else if (mimeType == "jpg" || mimeType == "jpeg") type = FileType::Jpg;
    else if (mimeType == "webp") type = FileType::Webp;
    else TVGLOG("RENDERER", "Given mimetype is unknown = \"%s\".", mimeType.c_str());

    return type;
}


static LoadModule* _findByType(const string& mimeType)
{
    return _find(_convert(mimeType));
}


static LoadModule* _findFromCache(const string& path)
{
    ScopedLock lock(key);

    auto loader = _activeLoaders.head;

    while (loader) {
        if (loader->pathcache && !strcmp(loader->hashpath, path.c_str())) {
            ++loader->sharing;
            return loader;
        }
        loader = loader->next;
    }
    return nullptr;
}


static LoadModule* _findFromCache(const char* data, uint32_t size, const string& mimeType)
{
    auto type = _convert(mimeType);
    if (type == FileType::Unknown) return nullptr;

    ScopedLock lock(key);
    auto loader = _activeLoaders.head;

    auto key = HASH_KEY(data);

    while (loader) {
        if (loader->type == type && loader->hashkey == key) {
            ++loader->sharing;
            return loader;
        }
        loader = loader->next;
    }
    return nullptr;
}


/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/


bool LoaderMgr::init()
{
    return true;
}


bool LoaderMgr::term()
{
    auto loader = _activeLoaders.head;

    //почистите оставшиеся загрузчики шрифтов, которые используются глобально.
    while (loader && loader->type == FileType::Ttf) {
        auto ret = loader->close();
        auto tmp = loader;
        loader = loader->next;
        _activeLoaders.remove(tmp);
        if (ret) delete(tmp);
    }
    return true;
}


bool LoaderMgr::retrieve(LoadModule* loader)
{
    if (!loader) return false;
    if (loader->close()) {
        if (loader->cached()) {
            ScopedLock lock(key);
            _activeLoaders.remove(loader);
        }
        delete(loader);
    }
    return true;
}


LoadModule* LoaderMgr::loader(const string& path, bool* invalid)
{
    *invalid = false;

    //TODO: SVG и Lottie не подлежат совместному использованию.
    auto allowCache = true;
    auto ext = path.substr(path.find_last_of(".") + 1);
    if (!ext.compare("svg") || !ext.compare("json")) allowCache = false;

    if (allowCache) {
        if (auto loader = _findFromCache(path)) return loader;
    }

    if (auto loader = _findByPath(path)) {
        if (loader->open(path)) {
            if (allowCache) {
                loader->hashpath = lv_strdup(path.c_str());
                loader->pathcache = true;
                {
                    ScopedLock lock(key);
                    _activeLoaders.back(loader);
                }
            }
            return loader;
        }
        delete(loader);
    }
    //Неизвестный MimeType. попробуй с кандидатами в порядке
    for (int i = 0; i < static_cast<int>(FileType::Raw); i++) {
        if (auto loader = _find(static_cast<FileType>(i))) {
            if (loader->open(path)) {
                if (allowCache) {
                    loader->hashpath = lv_strdup(path.c_str());
                    loader->pathcache = true;
                    {
                        ScopedLock lock(key);
                        _activeLoaders.back(loader);
                    }
                }
                return loader;
            }
            delete(loader);
        }
    }
    *invalid = true;
    return nullptr;
}


bool LoaderMgr::retrieve(const string& path)
{
    return retrieve(_findFromCache(path));
}


LoadModule* LoaderMgr::loader(const char* key)
{
    auto loader = _activeLoaders.head;

    while (loader) {
        if (loader->pathcache && strstr(loader->hashpath, key)) {
            ++loader->sharing;
            return loader;
        }
        loader = loader->next;
    }
    return nullptr;
}


LoadModule* LoaderMgr::loader(const char* data, uint32_t size, const string& mimeType, bool copy)
{
    //Обратите внимание, что пользователи могут использовать один и тот же указатель данных с разным содержимым.
    //Таким образом, кэширование допустимо только для разделяемых файлов.
    auto allowCache = !copy;

    //TODO: Лотти не подлежит совместному использованию.
    if (allowCache) {
        auto type = _convert(mimeType);
        if (type == FileType::Lottie) allowCache = false;
    }

    if (allowCache) {
        if (auto loader = _findFromCache(data, size, mimeType)) return loader;
    }

    //попробуйте использовать данный MimeType
    if (!mimeType.empty()) {
        if (auto loader = _findByType(mimeType)) {
            if (loader->open(data, size, copy)) {
                if (allowCache) {
                    loader->hashkey = HASH_KEY(data);
                    ScopedLock lock(key);
                    _activeLoaders.back(loader);
                }
                return loader;
            } else {
                TVGLOG("LOADER", "Given mimetype \"%s\" seems incorrect or not supported.", mimeType.c_str());
                delete(loader);
            }
        }
    }
    //Неизвестный MimeType. попробуй с кандидатами в порядке
    for (int i = 0; i < static_cast<int>(FileType::Raw); i++) {
        auto loader = _find(static_cast<FileType>(i));
        if (loader) {
            if (loader->open(data, size, copy)) {
                if (allowCache) {
                    loader->hashkey = HASH_KEY(data);
                    ScopedLock lock(key);
                    _activeLoaders.back(loader);
                }
                return loader;
            }
            delete(loader);
        }
    }
    return nullptr;
}


LoadModule* LoaderMgr::loader(const uint32_t *data, uint32_t w, uint32_t h, bool copy)
{
    //Обратите внимание, что пользователи могут использовать один и тот же указатель данных с разным содержимым.
    //Таким образом, кэширование допустимо только для разделяемых файлов.
    if (!copy) {
        //TODO: should we check premultiplied??
        if (auto loader = _findFromCache((const char*)(data), w * h, "raw")) return loader;
    }

    //функция предназначена только для необработанных изображений
    auto loader = new RawLoader;
    if (loader->open(data, w, h, copy)) {
        if (!copy) {
            loader->hashkey = HASH_KEY((const char*)data);
            ScopedLock lock(key);
            _activeLoaders.back(loader);
        }
        return loader;
    }
    delete(loader);
    return nullptr;
}


//загружает шрифты из памяти — загрузчик кэшируется (независимо от значения копии), чтобы получить к нему доступ при настройке шрифта
LoadModule* LoaderMgr::loader(const char* name, const char* data, uint32_t size, TVG_UNUSED const string& mimeType, bool copy)
{
#ifdef THORVG_TTF_LOADER_SUPPORT
    //TODO: add check for mimetype ?
    if (auto loader = _findFromCache(name)) return loader;

    //Функции для загрузчика ttf (единственный прикладной загрузчик шрифтов)
    auto loader = new TtfLoader;
    if (loader->open(data, size, copy)) {
        loader->hashpath = lv_strdup(name);
        loader->pathcache = true;
        ScopedLock lock(key);
        _activeLoaders.back(loader);
        return loader;
    }

    TVGLOG("LOADER", "The font data \"%s\" could not be loaded.", name);
    delete(loader);
#endif
    return nullptr;
}

#endif /* LV_USE_THORVG_INTERNAL */

