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

#ifndef _TVG_COMPRESSOR_H_
#define _TVG_COMPRESSOR_H_

#include <cstdint>

namespace tvg
{
    uint8_t* lzwEncode(const uint8_t* uncompressed, uint32_t uncompressedSizeBytes, uint32_t* compressedSizeBytes, uint32_t* compressedSizeBits);
    uint8_t* lzwDecode(const uint8_t* compressed, uint32_t compressedSizeBytes, uint32_t compressedSizeBits, uint32_t uncompressedSizeBytes);
    size_t b64Decode(const char* encoded, const size_t len, char** decoded);
    unsigned long djb2Encode(const char* str);
}

#endif  //_TVG_COMPRESSOR_H_

#endif /* LV_USE_THORVG_INTERNAL */

