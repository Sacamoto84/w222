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

#ifndef _TVG_TEXT_H
#define _TVG_TEXT_H

#include <cstring>
#include "tvgShape.h"
#include "tvgFill.h"
#include "tvgLoader.h"

struct Text::Impl
{
    FontLoader* loader = nullptr;
    Text* paint;
    Shape* shape;
    char* utf8 = nullptr;
    float fontSize;
    bool italic = false;
    bool changed = false;

    Impl(Text* p) : paint(p), shape(Shape::gen().release())
    {
    }

    ~Impl()
    {
        lv_free(utf8);
        LoaderMgr::retrieve(loader);
        delete(shape);
    }

    Result text(const char* utf8)
    {
        lv_free(this->utf8);
        if (utf8) this->utf8 = lv_strdup(utf8);
        else this->utf8 = nullptr;
        changed = true;

        return Result::Success;
    }

    Result font(const char* name, float size, const char* style)
    {
        auto loader = LoaderMgr::loader(name);
        if (!loader) return Result::InsufficientCondition;

        if (style && strstr(style, "italic")) italic = true;
        else italic = false;

        fontSize = size;

        //Тот же ресурс был загружен.
        if (this->loader == loader) {
            this->loader->sharing--;  //убедитесь, что подсчет ссылок.
            return Result::Success;
        } else if (this->loader) {
            LoaderMgr::retrieve(this->loader);
        }
        this->loader = static_cast<FontLoader*>(loader);

        changed = true;
        return Result::Success;
    }

    RenderRegion bounds(RenderMethod* renderer)
    {
        return P(shape)->bounds(renderer);
    }

    bool render(RenderMethod* renderer)
    {
        if (!loader) return true;
        renderer->blend(PP(paint)->blendMethod);
        return PP(shape)->render(renderer);
    }

    bool load()
    {
        if (!loader) return false;

        loader->request(shape, utf8);
        //перезагрузить
        if (changed) {
            loader->read();
            changed = false;
        }
        return loader->transform(shape, fontSize, italic);
    }

    RenderData update(RenderMethod* renderer, const Matrix& transform, Array<RenderData>& clips, uint8_t opacity, RenderUpdateFlag pFlag, TVG_UNUSED bool clipper)
    {
        if (!load()) return nullptr;

        //преобразуйте координаты градиента на основе окончательного масштабированного шрифта.
        auto fill = P(shape)->rs.fill;
        if (fill && P(shape)->flag & RenderUpdateFlag::Gradient) {
            auto scale = 1.0f / loader->scale;
            if (fill->type() == Type::LinearGradient) {
                P(static_cast<LinearGradient*>(fill))->x1 *= scale;
                P(static_cast<LinearGradient*>(fill))->y1 *= scale;
                P(static_cast<LinearGradient*>(fill))->x2 *= scale;
                P(static_cast<LinearGradient*>(fill))->y2 *= scale;
            } else {
                P(static_cast<RadialGradient*>(fill))->cx *= scale;
                P(static_cast<RadialGradient*>(fill))->cy *= scale;
                P(static_cast<RadialGradient*>(fill))->r *= scale;
                P(static_cast<RadialGradient*>(fill))->fx *= scale;
                P(static_cast<RadialGradient*>(fill))->fy *= scale;
                P(static_cast<RadialGradient*>(fill))->fr *= scale;
            }
        }
        return PP(shape)->update(renderer, transform, clips, opacity, pFlag, false);
    }

    bool bounds(float* x, float* y, float* w, float* h, TVG_UNUSED bool stroking)
    {
        if (!load()) return false;
        PP(shape)->bounds(x, y, w, h, true, true, false);
        return true;
    }

    Paint* duplicate(Paint* ret)
    {
        if (ret) TVGERR("RENDERER", "TODO: duplicate()");

        load();

        auto text = Text::gen().release();
        auto dup = text->pImpl;
        P(shape)->duplicate(dup->shape);

        if (loader) {
            dup->loader = loader;
            ++dup->loader->sharing;
        }

        dup->utf8 = lv_strdup(utf8);
        dup->italic = italic;
        dup->fontSize = fontSize;

        return text;
    }

    Iterator* iterator()
    {
        return nullptr;
    }
};



#endif //_TVG_TEXT_H

#endif /* LV_USE_THORVG_INTERNAL */

