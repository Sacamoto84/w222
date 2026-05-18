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


#include "tvgText.h"


/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/



/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/


Text::Text() : pImpl(new Impl(this))
{
}


Text::~Text()
{
    delete(pImpl);
}


Result Text::text(const char* text) noexcept
{
    return pImpl->text(text);
}


Result Text::font(const char* name, float size, const char* style) noexcept
{
    return pImpl->font(name, size, style);
}


Result Text::load(const std::string& path) noexcept
{
    bool invalid; //неверный путь
    if (!LoaderMgr::loader(path, &invalid)) {
        if (invalid) return Result::InvalidArguments;
        else return Result::NonSupport;
    }

    return Result::Success;
}


Result Text::load(const char* name, const char* data, uint32_t size, const string& mimeType, bool copy) noexcept
{
    if (!name || (size == 0 && data)) return Result::InvalidArguments;

    //выгрузить шрифт
    if (!data) {
        if (LoaderMgr::retrieve(name)) return Result::Success;
        return Result::InsufficientCondition;
    }

    if (!LoaderMgr::loader(name, data, size, mimeType, copy)) return Result::NonSupport;
    return Result::Success;
}


Result Text::unload(const std::string& path) noexcept
{
    if (LoaderMgr::retrieve(path)) return Result::Success;
    return Result::InsufficientCondition;
}


Result Text::fill(uint8_t r, uint8_t g, uint8_t b) noexcept
{
    return pImpl->shape->fill(r, g, b);
}


Result Text::fill(unique_ptr<Fill> f) noexcept
{
    return pImpl->shape->fill(std::move(f));
}


unique_ptr<Text> Text::gen() noexcept
{
    return unique_ptr<Text>(new Text);
}


Type Text::type() const noexcept
{
    return Type::Text;
}

#endif /* LV_USE_THORVG_INTERNAL */

