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

#include <fstream>
#include <string.h>
#include "tvgLoader.h"
#include "tvgRawLoader.h"

/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/


/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/

RawLoader::RawLoader() : ImageLoader(FileType::Raw)
{
}


RawLoader::~RawLoader()
{
    if (copy) lv_free(surface.buf32);
}


bool RawLoader::open(const uint32_t* data, uint32_t w, uint32_t h, bool copy)
{
    if (!LoadModule::read()) return true;

    if (!data || w == 0 || h == 0) return false;

    this->w = (float)w;
    this->h = (float)h;
    this->copy = copy;

    if (copy) {
        surface.buf32 = (uint32_t*)lv_malloc(sizeof(uint32_t) * w * h);
        LV_ASSERT_MALLOC(surface.buf32);
        if (!surface.buf32) return false;
        memcpy((void*)surface.buf32, data, sizeof(uint32_t) * w * h);
    }
    else surface.buf32 = const_cast<uint32_t*>(data);

    //настроить поверхность
    surface.stride = w;
    surface.w = w;
    surface.h = h;
    surface.cs = ColorSpace::ARGB8888;
    surface.channelSize = sizeof(uint32_t);
    surface.premultiplied = true;

    return true;
}


bool RawLoader::read()
{
    LoadModule::read();

    return true;
}

#endif /* LV_USE_THORVG_INTERNAL */

