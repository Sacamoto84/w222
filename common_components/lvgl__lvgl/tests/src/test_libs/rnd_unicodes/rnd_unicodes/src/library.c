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

#include "library.h"

__attribute__((weak)) uint32_t RANDOM_CALL_PROCESS(void)
{
    /*Алгоритм «xor» из п. 4 Марсали, «Ксоршифт ГСЧ»*/
    static uint32_t x = 0x114514;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    return x;
}

const uint32_t ALPHANUM_AND_CJK_TABLE[] = {
    0x4e00, 0x9fa5, // CJK унифицированные иероглифы
    'A', 'Z',
    'a', 'z',
    '0', '9',
};
const uint32_t ALPHANUM_AND_CJK_TABLE_LEN = sizeof(ALPHANUM_AND_CJK_TABLE) / sizeof(unicode_t) / 2;

const uint32_t ONLY_CJK_TABLE[] = {
    0x4e00, 0x9fa5, // CJK унифицированные иероглифы
};
const uint32_t ONLY_CJK_TABLE_LEN = sizeof(ALPHANUM_AND_CJK_TABLE) / sizeof(unicode_t) / 2;

const uint32_t ALPHANUM_TABLE[] = {
    'A', 'Z',
    'a', 'z',
    '0', '9',
};
const uint32_t ALPHANUM_TABLE_LEN = sizeof(ALPHANUM_TABLE) / sizeof(unicode_t) / 2;

static int unicode_to_utf8_bytes_len(unicode_t unicode)
{
    if(unicode < 0x80) {
        return 1;
    }
    else if(unicode < 0x800) {
        return 2;
    }
    else if(unicode < 0x10000) {
        return 3;
    }
    else if(unicode < 0x200000) {
        return 4;
    }
    else if(unicode < 0x4000000) {
        return 5;
    }
    else {
        return 6;
    }
}

static int unicode_to_uft8(utf8_t * buf, uint32_t buf_len, unicode_t unicode)
{
    uint32_t unicode_len = unicode_to_utf8_bytes_len(unicode);
    if(buf_len < unicode_len) {
        return -1;
    }

    int buf_index = 0;

    switch(unicode_len) {
        case 1:
            buf[buf_index++] = (uint8_t) unicode;
            break;
        case 2:
            buf[buf_index++] = (uint8_t)(0xc0 | (unicode >> 6));
            buf[buf_index++] = (uint8_t)(0x80 | (unicode & 0x3f));
            break;
        case 3:
            buf[buf_index++] = (uint8_t)(0xe0 | (unicode >> 12));
            buf[buf_index++] = (uint8_t)(0x80 | ((unicode >> 6) & 0x3f));
            buf[buf_index++] = (uint8_t)(0x80 | (unicode & 0x3f));
            break;
        case 4:
            buf[buf_index++] = (uint8_t)(0xf0 | (unicode >> 18));
            buf[buf_index++] = (uint8_t)(0x80 | ((unicode >> 12) & 0x3f));
            buf[buf_index++] = (uint8_t)(0x80 | ((unicode >> 6) & 0x3f));
            buf[buf_index++] = (uint8_t)(0x80 | (unicode & 0x3f));
            break;
        case 5:
            buf[buf_index++] = (uint8_t)(0xf8 | (unicode >> 24));
            buf[buf_index++] = (uint8_t)(0x80 | ((unicode >> 18) & 0x3f));
            buf[buf_index++] = (uint8_t)(0x80 | ((unicode >> 12) & 0x3f));
            buf[buf_index++] = (uint8_t)(0x80 | ((unicode >> 6) & 0x3f));
            buf[buf_index++] = (uint8_t)(0x80 | (unicode & 0x3f));
            break;
        case 6:
            buf[buf_index++] = (uint8_t)(0xfc | (unicode >> 30));
            buf[buf_index++] = (uint8_t)(0x80 | ((unicode >> 24) & 0x3f));
            buf[buf_index++] = (uint8_t)(0x80 | ((unicode >> 18) & 0x3f));
            buf[buf_index++] = (uint8_t)(0x80 | ((unicode >> 12) & 0x3f));
            buf[buf_index++] = (uint8_t)(0x80 | ((unicode >> 6) & 0x3f));
            buf[buf_index++] = (uint8_t)(0x80 | (unicode & 0x3f));
            break;
        default:
            return -1;
    }
    return buf_index;
}

static int random_one_utf8_char(utf8_t * buf, int buf_len, unicode_t char_range_min, unicode_t char_range_max)
{
    if(buf_len < 1) {
        return -1;
    }

    uint32_t r = RANDOM_CALL_PROCESS() % (char_range_max - char_range_min + 1) + char_range_min;

    return unicode_to_uft8(buf, buf_len, r);
}

int random_utf8_chars(utf8_t * buf, int buf_len, const unicode_t * ranges, uint32_t range_num, int char_num)
{
    if(buf_len < char_num) {
        return -1;
    }

    int buf_index = 0;
    for(int i = 0; i < char_num && buf_index < buf_len; i++) {
        int range_index = RANDOM_CALL_PROCESS() % range_num;
        int ret = random_one_utf8_char(buf + buf_index,
                                       buf_len - buf_index,
                                       ranges[2 * range_index],
                                       ranges[2 * range_index + 1]);
        if(ret < 0) {
            return -1;
        }
        buf_index += ret;
    }

    buf[buf_index] = '\0';
    return buf_index;
}
