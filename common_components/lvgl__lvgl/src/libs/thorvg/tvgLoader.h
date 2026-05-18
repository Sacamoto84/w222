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

#ifndef _TVG_LOADER_H_
#define _TVG_LOADER_H_

#include "tvgLoadModule.h"

struct LoaderMgr
{
    static bool init();
    static bool term();
    static LoadModule* loader(const string& path, bool* invalid);
    static LoadModule* loader(const char* data, uint32_t size, const string& mimeType, bool copy);
    static LoadModule* loader(const uint32_t* data, uint32_t w, uint32_t h, bool copy);
    static LoadModule* loader(const char* name, const char* data, uint32_t size, const string& mimeType, bool copy);
    static LoadModule* loader(const char* key);
    static bool retrieve(const string& path);
    static bool retrieve(LoadModule* loader);
};

#endif //_TVG_LOADER_H_

#endif /* LV_USE_THORVG_INTERNAL */

