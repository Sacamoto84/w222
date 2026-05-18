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

#ifndef _TVG_INLIST_H_
#define _TVG_INLIST_H_

namespace tvg {

//NOTE: объявите это в своем списке
#define INLIST_ITEM(T) \
    T* prev; \
    T* next

template<typename T>
struct Inlist
{
    T* head = nullptr;
    T* tail = nullptr;

    void free()
    {
        while (head) {
            auto t = head;
            head = t->next;
            delete(t);
        }
        head = tail = nullptr;
    }

    void back(T* element)
    {
        if (tail) {
            tail->next = element;
            element->prev = tail;
            element->next = nullptr;
            tail = element;
        } else {
            head = tail = element;
            element->prev = nullptr;
            element->next = nullptr;
        }
    }

    void front(T* element)
    {
        if (head) {
            head->prev = element;
            element->prev = nullptr;
            element->next = head;
            head = element;
        } else {
            head = tail = element;
            element->prev = nullptr;
            element->next = nullptr;
        }
    }

    T* back()
    {
        if (!tail) return nullptr;
        auto t = tail;
        tail = t->prev;
        if (!tail) head = nullptr;
        return t;
    }

    T* front()
    {
        if (!head) return nullptr;
        auto t = head;
        head = t->next;
        if (!head) tail = nullptr;
        return t;
    }

    void remove(T* element)
    {
        if (element->prev) element->prev->next = element->next;
        if (element->next) element->next->prev = element->prev;
        if (element == head) head = element->next;
        if (element == tail) tail = element->prev;
    }

    bool empty() const
    {
        return head ? false : true;
    }
};

}

#endif // _TVG_INLIST_H_

#endif /* LV_USE_THORVG_INTERNAL */

