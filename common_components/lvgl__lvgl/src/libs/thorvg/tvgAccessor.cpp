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

#include "tvgIteratorAccessor.h"
#include "tvgCompressor.h"

using std::function;

/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/

static bool accessChildren(Iterator* it, function<bool(const Paint* paint, void* data)> func, void* data)
{
    while (auto child = it->next()) {
        //Доступ к ребенку
        if (!func(child, data)) return false;

        //Доступ к детям ребенка
        if (auto it2 = IteratorAccessor::iterator(child)) {
            if (!accessChildren(it2, func, data)) {
                delete(it2);
                return false;
            }
            delete(it2);
        }
    }
    return true;
}


/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/

TVG_DEPRECATED unique_ptr<Picture> Accessor::set(unique_ptr<Picture> picture, function<bool(const Paint* paint)> func) noexcept
{
    auto backward = [](const tvg::Paint* paint, void* data) -> bool
    {
        auto func = reinterpret_cast<function<bool(const Paint* paint)>*>(data);
        if (!(*func)(paint)) return false;
        return true;
    };

    set(picture.get(), backward, reinterpret_cast<void*>(&func));
    return picture;
}


Result Accessor::set(const Picture* picture, function<bool(const Paint* paint, void* data)> func, void* data) noexcept
{
    if (!picture || !func) return Result::InvalidArguments;

    //Используйте дерево предзаказа

    //Корень
    if (!func(picture, data)) return Result::Success;

    //Дети
    if (auto it = IteratorAccessor::iterator(picture)) {
        accessChildren(it, func, data);
        delete(it);
    }
    return Result::Success;
}


uint32_t Accessor::id(const char* name) noexcept
{
    return djb2Encode(name);
}


Accessor::~Accessor()
{

}


Accessor::Accessor() : pImpl(nullptr)
{

}


unique_ptr<Accessor> Accessor::gen() noexcept
{
    return unique_ptr<Accessor>(new Accessor);
}

#endif /* LV_USE_THORVG_INTERNAL */

