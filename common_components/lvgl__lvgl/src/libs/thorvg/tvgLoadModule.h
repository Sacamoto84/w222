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

#ifndef _TVG_LOAD_MODULE_H_
#define _TVG_LOAD_MODULE_H_

#include "tvgRender.h"
#include "tvgInlist.h"


struct LoadModule
{
    INLIST_ITEM(LoadModule);

    //Используйте либо хеш-ключ (данные), либо хэш-путь (путь).
    union {
        uintptr_t hashkey;
        char* hashpath = nullptr;
    };

    FileType type;                                  //текущий тип файла загрузчика
    uint16_t sharing = 0;                           //счетчик ссылок
    bool readied = false;                           //прочитал уже готово.
    bool pathcache = false;                         //кэшируется по пути

    LoadModule(FileType type) : type(type) {}
    virtual ~LoadModule()
    {
        if (pathcache) lv_free(hashpath);
    }

    virtual bool open(const string& path) { return false; }
    virtual bool open(const char* data, uint32_t size, bool copy) { return false; }
    virtual bool resize(Paint* paint, float w, float h) { return false; }
    virtual void sync() {};  //завершить немедленно, если есть какие-либо задания асинхронного обновления.

    virtual bool read()
    {
        if (readied) return false;
        readied = true;
        return true;
    }

    bool cached()
    {
        if (hashkey) return true;
        return false;
    }

    virtual bool close()
    {
        if (sharing == 0) return true;
        --sharing;
        return false;
    }
};


struct ImageLoader : LoadModule
{
    static ColorSpace cs;                           //желаемое значение

    float w = 0, h = 0;                             //размер изображения по умолчанию
    RenderSurface surface;

    ImageLoader(FileType type) : LoadModule(type) {}

    virtual bool animatable() { return false; }  //правда, если этот загрузчик поддерживает анимацию.
    virtual Paint* paint() { return nullptr; }

    virtual RenderSurface* bitmap()
    {
        if (surface.data) return &surface;
        return nullptr;
    }
};


struct FontLoader : LoadModule
{
    float scale = 1.0f;

    FontLoader(FileType type) : LoadModule(type) {}

    virtual bool request(Shape* shape, char* text) = 0;
    virtual bool transform(Paint* paint, float fontSize, bool italic) = 0;
};

#endif //_TVG_LOAD_MODULE_H_

#endif /* LV_USE_THORVG_INTERNAL */

