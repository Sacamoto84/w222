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

#ifdef THORVG_AVX_VECTOR_SUPPORT

#include <immintrin.h>

#define N_32BITS_IN_128REG 4
#define N_32BITS_IN_256REG 8

static inline __m128i ALPHA_BLEND(__m128i c, __m128i a)
{
    //1. установите маски для каналов A/G и R/B
    auto AG = _mm_set1_epi32(0xff00ff00);
    auto RB = _mm_set1_epi32(0x00ff00ff);

    //2. замаскировать альфа-вектор — изначально квартет [a, a, a, a]
    auto aAG = _mm_and_si128(a, AG);
    auto aRB = _mm_and_si128(a, RB);

    //3. рассчитать альфа-смешение 2-го и 4-го каналов
    //- замаскировать цветовой вектор
    //- умножьте его на замаскированный альфа-вектор
    //- добавьте коррекцию для компенсации сдвига битов, используемого вместо деления на 255
    //- бит сдвига - соответствует делению на 256
    auto even = _mm_and_si128(c, RB);
    even = _mm_mullo_epi16(even, aRB);
    even =_mm_add_epi16(even, RB);
    even = _mm_srli_epi16(even, 8);

    //4. вычислите альфа-смешение 1-го и 3-го каналов:
    //- замаскировать цветовой вектор
    //- умножьте его на соответствующий замаскированный альфа-вектор и сохраните старшие биты результата
    //- добавьте поправку для компенсации деления на 256 вместо 255 (следующий шаг)
    //- удалите младшие 8 бит, чтобы имитировать деление на 256
    auto odd = _mm_and_si128(c, AG);
    odd = _mm_mulhi_epu16(odd, aAG);
    odd = _mm_add_epi16(odd, RB);
    odd = _mm_and_si128(odd, AG);

    //5. окончательный результат
    return _mm_or_si128(odd, even);
}


static void avxRasterGrayscale8(uint8_t* dst, uint8_t val, uint32_t offset, int32_t len) 
{
    dst += offset; 

    __m256i vecVal = _mm256_set1_epi8(val);

    int32_t i = 0;
    for (; i <= len - 32; i += 32) {
        _mm256_storeu_si256((__m256i*)(dst + i), vecVal);
    }

    for (; i < len; ++i) {
        dst[i] = val;
    }
}


static void avxRasterPixel32(uint32_t *dst, uint32_t val, uint32_t offset, int32_t len)
{
    //1. подсчитайте, сколько итераций нам нужно, чтобы покрыть длину
    uint32_t iterations = len / N_32BITS_IN_256REG;
    uint32_t avxFilled = iterations * N_32BITS_IN_256REG;

    //2. установить начало массива
    dst += offset;

    //3. заполнить октеты
    for (uint32_t i = 0; i < iterations; ++i, dst += N_32BITS_IN_256REG) {
        _mm256_storeu_si256((__m256i*)dst, _mm256_set1_epi32(val));
    }

    //4. заполнить остатки (на первом этапе нам нужно установить указатель на место, где выполняется задание avx)
    int32_t leftovers = len - avxFilled;
    while (leftovers--) *dst++ = val;
}


static bool avxRasterTranslucentRect(SwSurface* surface, const SwBBox& region, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    auto h = static_cast<uint32_t>(region.max.y - region.min.y);
    auto w = static_cast<uint32_t>(region.max.x - region.min.x);

    //32-битные каналы
    if (surface->channelSize == sizeof(uint32_t)) {
        auto color = surface->join(r, g, b, a);
        auto buffer = surface->buf32 + (region.min.y * surface->stride) + region.min.x;

        uint32_t ialpha = 255 - a;

        auto avxColor = _mm_set1_epi32(color);
        auto avxIalpha = _mm_set1_epi8(ialpha);

        for (uint32_t y = 0; y < h; ++y) {
            auto dst = &buffer[y * surface->stride];

            //1. заполнить невыровненную память (для 128-битных регистров требуется выравнивание по 16 байт)
            auto notAligned = ((uintptr_t)dst & 0xf) / 4;
            if (notAligned) {
                notAligned = (N_32BITS_IN_128REG - notAligned > w ? w : N_32BITS_IN_128REG - notAligned);
                for (uint32_t x = 0; x < notAligned; ++x, ++dst) {
                    *dst = color + ALPHA_BLEND(*dst, ialpha);
                }
            }

            //2. заполнить выровненную память — пикселиN_32BITS_IN_128REGобрабатываются сразу
            uint32_t iterations = (w - notAligned) / N_32BITS_IN_128REG;
            uint32_t avxFilled = iterations * N_32BITS_IN_128REG;
            auto avxDst = (__m128i*)dst;
            for (uint32_t x = 0; x < iterations; ++x, ++avxDst) {
                *avxDst = _mm_add_epi32(avxColor, ALPHA_BLEND(*avxDst, avxIalpha));
            }

            //3. заполнить оставшиеся пиксели
            int32_t leftovers = w - notAligned - avxFilled;
            dst += avxFilled;
            while (leftovers--) {
                *dst = color + ALPHA_BLEND(*dst, ialpha);
                dst++;
            }
        }
    //8-битный оттенок серого
    } else if (surface->channelSize == sizeof(uint8_t)) {
        TVGLOG("SW_ENGINE", "Require AVX Optimization, Channel Size = %d", surface->channelSize);
        auto buffer = surface->buf8 + (region.min.y * surface->stride) + region.min.x;
        auto ialpha = ~a;
        for (uint32_t y = 0; y < h; ++y) {
            auto dst = &buffer[y * surface->stride];
            for (uint32_t x = 0; x < w; ++x, ++dst) {
                *dst = a + MULTIPLY(*dst, ialpha);
            }
        }
    }
    return true;
}


static bool avxRasterTranslucentRle(SwSurface* surface, const SwRle* rle, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    auto span = rle->spans;

    //32-битные каналы
    if (surface->channelSize == sizeof(uint32_t)) {
        auto color = surface->join(r, g, b, a);
        uint32_t src;

        for (uint32_t i = 0; i < rle->size; ++i) {
            auto dst = &surface->buf32[span->y * surface->stride + span->x];

            if (span->coverage < 255) src = ALPHA_BLEND(color, span->coverage);
            else src = color;

        auto ialpha = IA(src);

            //1. заполнить невыровненную память (для 128-битных регистров требуется выравнивание по 16 байт)
            auto notAligned = ((uintptr_t)dst & 0xf) / 4;
            if (notAligned) {
                notAligned = (N_32BITS_IN_128REG - notAligned > span->len ? span->len : N_32BITS_IN_128REG - notAligned);
                for (uint32_t x = 0; x < notAligned; ++x, ++dst) {
                    *dst = src + ALPHA_BLEND(*dst, ialpha);
                }
            }

            //2. заполнить выровненную память, используя avx — пикселиN_32BITS_IN_128REGобрабатываются одновременно
            //Чтобы избежать ненужных бесплатных запросов avx, производя проверку, есть ли вообще какие-либо итерации.
            uint32_t iterations = (span->len - notAligned) / N_32BITS_IN_128REG;
            uint32_t avxFilled = 0;
            if (iterations > 0) {
                auto avxSrc = _mm_set1_epi32(src);
                auto avxIalpha = _mm_set1_epi8(ialpha);

                avxFilled = iterations * N_32BITS_IN_128REG;
                auto avxDst = (__m128i*)dst;
                for (uint32_t x = 0; x < iterations; ++x, ++avxDst) {
                    *avxDst = _mm_add_epi32(avxSrc, ALPHA_BLEND(*avxDst, avxIalpha));
                }
            }

            //3. заполнить оставшиеся пиксели
            int32_t leftovers = span->len - notAligned - avxFilled;
            dst += avxFilled;
            while (leftovers--) {
                *dst = src + ALPHA_BLEND(*dst, ialpha);
                dst++;
            }

            ++span;
        }
    //8-битный оттенок серого
    } else if (surface->channelSize == sizeof(uint8_t)) {
        TVGLOG("SW_ENGINE", "Require AVX Optimization, Channel Size = %d", surface->channelSize);
        uint8_t src;
        for (uint32_t i = 0; i < rle->size; ++i, ++span) {
            auto dst = &surface->buf8[span->y * surface->stride + span->x];
            if (span->coverage < 255) src = MULTIPLY(span->coverage, a);
            else src = a;
            auto ialpha = ~a;
            for (uint32_t x = 0; x < span->len; ++x, ++dst) {
                *dst = src + MULTIPLY(*dst, ialpha);
            }
        }
    }
    return true;
}


#endif

#endif /* LV_USE_THORVG_INTERNAL */

