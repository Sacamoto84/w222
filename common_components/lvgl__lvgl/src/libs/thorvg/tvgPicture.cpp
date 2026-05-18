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

#include "tvgPaint.h"
#include "tvgPicture.h"

/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/

RenderUpdateFlag Picture::Impl::load()
{
    if (loader) {
        if (paint) {
            loader->sync();
        } else {
            paint = loader->paint();
            if (paint) {
                if (w != loader->w || h != loader->h) {
                    if (!resizing) {
                        w = loader->w;
                        h = loader->h;
                    }
                    loader->resize(paint, w, h);
                    resizing = false;
                }
                return RenderUpdateFlag::None;
            }
        }
        if (!surface) {
            if ((surface = loader->bitmap())) {
                return RenderUpdateFlag::Image;
            }
        }
    }
    return RenderUpdateFlag::None;
}


bool Picture::Impl::needComposition(uint8_t opacity)
{
    //В этом случае краска (сцена) сама попытается составить композицию.
    if (opacity < 255) return false;

    //Тест на состав
    const Paint* target;
    auto method = picture->composite(&target);
    if (!target || method == tvg::CompositeMethod::ClipPath) return false;
    if (target->pImpl->opacity == 255 || target->pImpl->opacity == 0) return false;

    return true;
}


bool Picture::Impl::render(RenderMethod* renderer)
{
    bool ret = false;
    renderer->blend(PP(picture)->blendMethod);

    if (surface) return renderer->renderImage(rd);
    else if (paint) {
        RenderCompositor* cmp = nullptr;
        if (needComp) {
            cmp = renderer->target(bounds(renderer), renderer->colorSpace());
            renderer->beginComposite(cmp, CompositeMethod::None, 255);
        }
        ret = paint->pImpl->render(renderer);
        if (cmp) renderer->endComposite(cmp);
    }
    return ret;
}


bool Picture::Impl::size(float w, float h)
{
    this->w = w;
    this->h = h;
    resizing = true;
    return true;
}


RenderRegion Picture::Impl::bounds(RenderMethod* renderer)
{
    if (rd) return renderer->region(rd);
    if (paint) return paint->pImpl->bounds(renderer);
    return {0, 0, 0, 0};
}


Result Picture::Impl::load(ImageLoader* loader)
{
    //Тот же ресурс был загружен.
    if (this->loader == loader) {
        this->loader->sharing--;  //убедитесь, что подсчет ссылок.
        return Result::Success;
    } else if (this->loader) {
        LoaderMgr::retrieve(this->loader);
    }

    this->loader = loader;

    if (!loader->read()) return Result::Unknown;

    this->w = loader->w;
    this->h = loader->h;    

    return Result::Success;
}



/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/

Picture::Picture() : pImpl(new Impl(this))
{
}


Picture::~Picture()
{
    delete(pImpl);
}


unique_ptr<Picture> Picture::gen() noexcept
{
    return unique_ptr<Picture>(new Picture);
}


TVG_DEPRECATED uint32_t Picture::identifier() noexcept
{
    return (uint32_t) Type::Picture;
}


Type Picture::type() const noexcept
{
    return Type::Picture;
}


Result Picture::load(const std::string& path) noexcept
{
    if (path.empty()) return Result::InvalidArguments;

    return pImpl->load(path);
}


Result Picture::load(const char* data, uint32_t size, const string& mimeType, bool copy) noexcept
{
    if (!data || size <= 0) return Result::InvalidArguments;

    return pImpl->load(data, size, mimeType, copy);
}


TVG_DEPRECATED Result Picture::load(const char* data, uint32_t size, bool copy) noexcept
{
    return load(data, size, "", copy);
}


Result Picture::load(uint32_t* data, uint32_t w, uint32_t h, bool copy) noexcept
{
    if (!data || w <= 0 || h <= 0) return Result::InvalidArguments;

    return pImpl->load(data, w, h, copy);
}


Result Picture::size(float w, float h) noexcept
{
    if (pImpl->size(w, h)) return Result::Success;
    return Result::InsufficientCondition;
}


Result Picture::size(float* w, float* h) const noexcept
{
    if (!pImpl->loader) return Result::InsufficientCondition;
    if (w) *w = pImpl->w;
    if (h) *h = pImpl->h;
    return Result::Success;
}


const Paint* Picture::paint(uint32_t id) noexcept
{
    struct Value
    {
        uint32_t id;
        const Paint* ret;
    } value = {id, nullptr};

    auto cb = [](const tvg::Paint* paint, void* data) -> bool
    {
        auto p = static_cast<Value*>(data);
        if (p->id == paint->id) {
            p->ret = paint;
            return false;
        }
        return true;
    };

    tvg::Accessor::gen()->set(this, cb, &value);
    return value.ret;
}

#endif /* LV_USE_THORVG_INTERNAL */

