/*
 * Лицензия MIT
 *
 * Copyright (c) 2023 Benign X
 *
 * Разрешение настоящим предоставляется бесплатно любому лицу, получившему копию.
 * данного программного обеспечения и связанных с ним файлов документации («Программное обеспечение») для решения
 * в Программном обеспечении без ограничений, включая, помимо прочего, права
 * использовать, копировать, изменять, объединять, публиковать, распространять, сублицензировать и/или продавать
 * копий Программного обеспечения и разрешать лицам, которым Программное обеспечение
 * предоставлено для этого при соблюдении следующих условий:
 *
 * Вышеупомянутое уведомление об авторских правах и настоящее уведомление о разрешении должны быть включены во все
 * копии или существенные части Программного обеспечения.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef RND_UNICODES_LIBRARY_H
#define RND_UNICODES_LIBRARY_H

#include <stdint.h>

typedef uint32_t unicode_t;
typedef uint8_t utf8_t;

extern const uint32_t ALPHANUM_AND_CJK_TABLE[];
extern const uint32_t ALPHANUM_AND_CJK_TABLE_LEN;

extern const uint32_t ONLY_CJK_TABLE[];
extern const uint32_t ONLY_CJK_TABLE_LEN;

extern const uint32_t ALPHANUM_TABLE[];
extern const uint32_t ALPHANUM_TABLE_LEN;

uint32_t RANDOM_CALL_PROCESS(void);

int random_utf8_chars(utf8_t * buf, int buf_len, const unicode_t * ranges, uint32_t range_num, int char_num);

#endif //RND_UNICODES_LIBRARY_H
