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

#ifndef _TVG_ARRAY_H_
#define _TVG_ARRAY_H_

#include "tvgCommon.h"
#include <memory.h>
#include <cstdint>
#include <cstdlib>
#include "tvgCommon.h"

namespace tvg
{

template<class T>
struct Array
{
    T* data = nullptr;
    uint32_t count = 0;
    uint32_t reserved = 0;

    Array(){}

    Array(int32_t size)
    {
        reserve(size);
    }

    Array(const Array& rhs)
    {
        reset();
        *this = rhs;
    }

    void push(T element)
    {
        if (count + 1 > reserved) {
            reserved = count + (count + 2) / 2;
            data = static_cast<T*>(lv_realloc(data, sizeof(T) * reserved));
            LV_ASSERT_MALLOC(data);
        }
        data[count++] = element;
    }

    void push(const Array<T>& rhs)
    {
        if (rhs.count == 0) return;
        grow(rhs.count);
        memcpy(data + count, rhs.data, rhs.count * sizeof(T));
        count += rhs.count;
    }

    bool reserve(uint32_t size)
    {
        if (size > reserved) {
            reserved = size;
            data = static_cast<T*>(lv_realloc(data, sizeof(T) * reserved));
            LV_ASSERT_MALLOC(data);
        }
        return true;
    }

    bool grow(uint32_t size)
    {
        return reserve(count + size);
    }

    const T& operator[](size_t idx) const
    {
        return data[idx];
    }

    T& operator[](size_t idx)
    {
        return data[idx];
    }

    const T* begin() const
    {
        return data;
    }

    T* begin()
    {
        return data;
    }

    T* end()
    {
        return data + count;
    }

    const T* end() const
    {
        return data + count;
    }

    const T& last() const
    {
        return data[count - 1];
    }

    const T& first() const
    {
        return data[0];
    }

    T& last()
    {
        return data[count - 1];
    }

    T& first()
    {
        return data[0];
    }

    void pop()
    {
        if (count > 0) --count;
    }

    void reset()
    {
        lv_free(data);
        data = nullptr;
        count = reserved = 0;
    }

    void clear()
    {
        count = 0;
    }

    bool empty() const
    {
        return count == 0;
    }

    template<class COMPARE>
    void sort()
    {
        qsort<COMPARE>(data, 0, static_cast<int32_t>(count) - 1);
    }

    void operator=(const Array& rhs)
    {
        reserve(rhs.count);
        if (rhs.count > 0) memcpy(data, rhs.data, sizeof(T) * rhs.count);
        count = rhs.count;
    }

    ~Array()
    {
        lv_free(data);
    }

private:
    template<class COMPARE>
    void qsort(T* arr, int32_t low, int32_t high)
    {
        if (low < high) {
            int32_t i = low;
            int32_t j = high;
            T tmp = arr[low];
            while (i < j) {
                while (i < j && !COMPARE{}(arr[j], tmp)) --j;
                if (i < j) {
                    arr[i] = arr[j];
                    ++i;
                }
                while (i < j && COMPARE{}(arr[i], tmp)) ++i;
                if (i < j) {
                    arr[j] = arr[i];
                    --j;
                }
            }
            arr[i] = tmp;
            qsort<COMPARE>(arr, low, i - 1);
            qsort<COMPARE>(arr, i + 1, high);
        }
    }
};

}

#endif //_TVG_ARRAY_H_

#endif /* LV_USE_THORVG_INTERNAL */

