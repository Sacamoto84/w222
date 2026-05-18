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

/*
 * Кодер/декодер Лемпеля-Зива-Уэлча ( LZW ), автор Гильерме Р. Ламперт (guilherme.ronaldo.lampert @gmail .com)

 * Это схема сжатия, используемая форматом изображений GIF и инструментом сжатия Unix.
 * Основные отличия от этой реализации заключаются в том, что коды окончания ввода ( EOI ) и очистки ( CC )
 * не сохраняются в выходных данных, а максимальная длина кода в битах равна 12 по сравнению с 16 при сжатии.
 *
 * EOI просто обнаруживается по концу потока данных, тогда как CC возникает, если
 * словарь заполняется. Данные записываются/читаются из битовых потоков, которые обрабатывают
 * выравнивание байтов для нас прозрачным способом.

 * Декодер опирается на жестко закодированное расположение данных, создаваемое кодером, поскольку
 * никакие дополнительные данные реконструкции не добавляются к выходным данным, поэтому они должны совпадать.
 * Самое приятное в LZW то, что мы можем восстановить словарь непосредственно из
 * поток кодов, генерируемый кодером, что позволяет избежать хранения дополнительных
 * заголовки в битовом потоке.

 * Длина выходного кода является переменной. Он начинается с минимального количества битов.
 * требуется для хранения базового словаря размером в байт и автоматически увеличивается
 * по мере увеличения словаря (он начинается с 9 бит и увеличивается до 10 бит, когда
 * добавляется код 512, затем 11 бит при добавлении 1024 и так далее). Если словарь
 * заполняется (4096 элементов для 12-битного словаря), все это очищается и
 * процесс начинается заново. Это основная причина, по которой кодер и декодер
 * должно идеально совпадать, поскольку длины кодов не будут указываться с помощью
 * сами данные.

 * USEFUL LINKS:
 * https://en.wikipedia.org/wiki/Lempel%E2%80%93Ziv%E2%80%93Welch
 * http://rosettacode.org/wiki/LZW_compression
 * http://www.cs.duke.edu/csed/curious/compression/lzw.html
 * http://www.cs.cf.ac.uk/Dave/Multimedia/node214.html
 * http://marknelson.us/1989/10/01/lzw-data-compression/
 */
#include "config.h"
#include "thorvg.h"

#include <string>
#include <memory.h>
#include "tvgCompressor.h"

namespace tvg {


/************************************************************************/
/* LZW Реализация                                                   */
/************************************************************************/


//LZW Помощник по словарю:
constexpr int Nil = -1;
constexpr int MaxDictBits = 12;
constexpr int StartBits = 9;
constexpr int FirstCode = (1 << (StartBits - 1)); // 256
constexpr int MaxDictEntries = (1 << MaxDictBits);     // 4096


//Округлите до следующего числа, кратного степени двойки, например. 37 => 64
static int nextPowerOfTwo(int num)
{
    --num;
    for (size_t i = 1; i < sizeof(num) * 8; i <<= 1) {
        num = num | num >> i;
    }
    return ++num;
}


struct BitStreamWriter
{
    uint8_t* stream;       //Растущий буфер для хранения наших битов. Куча выделена и принадлежит экземпляру класса.
    int bytesAllocated;    //Текущий размер буфера потока, выделенного в куче *в байтах*.
    int granularity;       //Сумма bytesAllocated умножается на при автоматическом изменении размера в appendBit() .
    int currBytePos;       //Текущий байт, в который выполняется запись, от 0 до bytesAllocated-1.
    int nextBitPos;        //Позиция бита в текущем байте для доступа к следующему. от 0 до 7.
    int numBitsWritten;    //Количество используемых бит из буфера потока, не включая заполнение округления байтов.

    void internalInit()
    {
        stream = nullptr;
        bytesAllocated = 0;
        granularity = 2;
        currBytePos = 0;
        nextBitPos = 0;
        numBitsWritten = 0;
    }

    uint8_t* allocBytes(const int bytesWanted, uint8_t * oldPtr, const int oldSize)
    {
        auto newMemory = static_cast<uint8_t *>(lv_malloc(bytesWanted));
        LV_ASSERT_MALLOC(newMemory);
        memset(newMemory, 0, bytesWanted);

        if (oldPtr) {
            memcpy(newMemory, oldPtr, oldSize);
            free(oldPtr);
        }
        return newMemory;
    }

    BitStreamWriter()
    {
        /* 8192 бита для начала (1024 байта). При необходимости он изменит размер.
           Детализация по умолчанию равна 2. */
        internalInit();
        allocate(8192);
    }

    BitStreamWriter(const int initialSizeInBits, const int growthGranularity = 2)
    {
        internalInit();
        setGranularity(growthGranularity);
        allocate(initialSizeInBits);
    }

    ~BitStreamWriter()
    {
        free(stream);
    }

    void allocate(int bitsWanted)
    {
        //Требуется хотя бы байт.
        if (bitsWanted <= 0) bitsWanted = 8;

        //При необходимости округлить вверх:
        if ((bitsWanted % 8) != 0) bitsWanted = nextPowerOfTwo(bitsWanted);

        //Возможно, у нас уже есть необходимое количество.
        const int sizeInBytes = bitsWanted / 8;
        if (sizeInBytes <= bytesAllocated) return;

        stream = allocBytes(sizeInBytes, stream, bytesAllocated);
        bytesAllocated = sizeInBytes;
    }

    void appendBit(const int bit)
    {
        const uint32_t mask = uint32_t(1) << nextBitPos;
        stream[currBytePos] = (stream[currBytePos] & ~mask) | (-bit & mask);
        ++numBitsWritten;

        if (++nextBitPos == 8) {
            nextBitPos = 0;
            if (++currBytePos == bytesAllocated) allocate(bytesAllocated * granularity * 8);
        }
    }

    void appendBitsU64(const uint64_t num, const int bitCount)
    {
        for (int b = 0; b < bitCount; ++b) {
            const uint64_t mask = uint64_t(1) << b;
            const int bit = !!(num & mask);
            appendBit(bit);
        }
    }

    uint8_t* release()
    {
        auto oldPtr = stream;
        internalInit();
        return oldPtr;
    }

    void setGranularity(const int growthGranularity)
    {
        granularity = (growthGranularity >= 2) ? growthGranularity : 2;
    }

    int getByteCount() const
    {
        int usedBytes = numBitsWritten / 8;
        int leftovers = numBitsWritten % 8;
        if (leftovers != 0) ++usedBytes;
        return usedBytes;
    }
};


struct BitStreamReader
{
    const uint8_t* stream;       // Указатель на внешний битовый поток. Не принадлежит читателю.
    const int sizeInBytes;       // Размер потока *в байтах*. Может включать прокладку.
    const int sizeInBits;        // Размер потока *в битах*, заполнение *не* включено.
    int currBytePos = 0;         // Текущий байт, читаемый в потоке.
    int nextBitPos = 0;          // Позиция бита в текущем байте для доступа к следующему. от 0 до 7.
    int numBitsRead = 0;         // Всего битов, прочитанных из потока на данный момент. Никогда не включает заполнение для округления байтов.

    BitStreamReader(const uint8_t* bitStream, const int byteCount, const int bitCount) : stream(bitStream), sizeInBytes(byteCount), sizeInBits(bitCount)
    {
    }

    bool readNextBit(int& bitOut)
    {
        if (numBitsRead >= sizeInBits) return false; //Мы закончили.

        const uint32_t mask = uint32_t(1) << nextBitPos;
        bitOut = !!(stream[currBytePos] & mask);
        ++numBitsRead;

        if (++nextBitPos == 8) {
            nextBitPos = 0;
            ++currBytePos;
        }
        return true;
    }

    uint64_t readBitsU64(const int bitCount)
    {
        uint64_t num = 0;
        for (int b = 0; b < bitCount; ++b) {
            int bit;
            if (!readNextBit(bit)) break;
            /* На основе «Стэнфордского бит-хака»:
               http://graphics.stanford.edu/~seander/bithacks.html#ConditionalSetOrClearBitsWithoutBranching */
            const uint64_t mask = uint64_t(1) << b;
            num = (num & ~mask) | (-bit & mask);
        }
        return num;
    }

    bool isEndOfStream() const
    {
        return numBitsRead >= sizeInBits;
    }
};


struct Dictionary
{
    struct Entry
    {
        int code;
        int value;
    };

    //Записи словаря 0–255 всегда зарезервированы для диапазона байт/ASCII.
    int size;
    Entry entries[MaxDictEntries];

    Dictionary()
    {
        /* Первые 256 записей словаря зарезервированы для диапазона byte/ASCII.
           Дополнительные записи следуют за последовательностями символов, найденными во входных данных.
           До 4096–256 (MaxDictEntries — FirstCode). */
        size = FirstCode;

        for (int i = 0; i < size; ++i) {
            entries[i].code  = Nil;
            entries[i].value = i;
        }
    }

    int findIndex(const int code, const int value) const
    {
        if (code == Nil) return value;

        //Линейный поиск на данный момент.
        //TODO: Worth optimizing with a proper hash-table?
        for (int i = 0; i < size; ++i) {
            if (entries[i].code == code && entries[i].value == value) return i;
        }
        return Nil;
    }

    bool add(const int code, const int value)
    {
        if (size == MaxDictEntries) return false;
        entries[size].code  = code;
        entries[size].value = value;
        ++size;
        return true;
    }

    bool flush(int & codeBitsWidth)
    {
        if (size == (1 << codeBitsWidth)) {
            ++codeBitsWidth;
            if (codeBitsWidth > MaxDictBits) {
                //Очистите словарь (кроме первых 256-байтовых записей).
                codeBitsWidth = StartBits;
                size = FirstCode;
                return true;
            }
        }
        return false;
    }
};


static bool outputByte(int code, uint8_t*& output, int outputSizeBytes, int& bytesDecodedSoFar)
{
    if (bytesDecodedSoFar >= outputSizeBytes) return false;
    *output++ = static_cast<uint8_t>(code);
    ++bytesDecodedSoFar;
    return true;
}


static bool outputSequence(const Dictionary& dict, int code, uint8_t*& output, int outputSizeBytes, int& bytesDecodedSoFar, int& firstByte)
{
    /* Последовательность хранится в обратном порядке, поэтому нам приходится писать
       его до температуры, а затем выведите буфер в обратном порядке. */
    int i = 0;
    uint8_t sequence[MaxDictEntries];

    do {
        sequence[i++] = dict.entries[code].value;
        code = dict.entries[code].code;
    } while (code >= 0);

    firstByte = sequence[--i];

    for (; i >= 0; --i) {
        if (!outputByte(sequence[i], output, outputSizeBytes, bytesDecodedSoFar)) return false;
    }
    return true;
}


uint8_t* lzwDecode(const uint8_t* compressed, uint32_t compressedSizeBytes, uint32_t compressedSizeBits, uint32_t uncompressedSizeBytes)
{
    int code = Nil;
    int prevCode = Nil;
    int firstByte = 0;
    int bytesDecoded = 0;
    int codeBitsWidth = StartBits;
    auto uncompressed = (uint8_t*) lv_malloc(sizeof(uint8_t) * uncompressedSizeBytes);
    LV_ASSERT_MALLOC(uncompressed);
    auto ptr = uncompressed;

    /* Восстановим словарь на основе кодов битового потока.
       В отличие от кодирования Хаффмана, мы не сохраняем словарь в качестве префикса к данным. */
    Dictionary dictionary;
    BitStreamReader bitStream(compressed, compressedSizeBytes, compressedSizeBits);

    /* Проверяем, чтобы избежать переполнения пользовательского буфера.
       Если буфер меньше распакованного размера, мы прерываем цикл и возвращаем текущий счетчик распаковки. */
    while (!bitStream.isEndOfStream()) {
        code = static_cast<int>(bitStream.readBitsU64(codeBitsWidth));

        if (prevCode == Nil) {
            if (!outputByte(code, ptr, uncompressedSizeBytes, bytesDecoded)) break;
            firstByte = code;
            prevCode  = code;
            continue;
        }
        if (code >= dictionary.size) {
            if (!outputSequence(dictionary, prevCode, ptr, uncompressedSizeBytes, bytesDecoded, firstByte)) break;
            if (!outputByte(firstByte, ptr, uncompressedSizeBytes, bytesDecoded)) break;
        } else if (!outputSequence(dictionary, code, ptr, uncompressedSizeBytes, bytesDecoded, firstByte)) break;

        dictionary.add(prevCode, firstByte);
        if (dictionary.flush(codeBitsWidth)) prevCode = Nil;
        else prevCode = code;
    }

    return uncompressed;
}


uint8_t* lzwEncode(const uint8_t* uncompressed, uint32_t uncompressedSizeBytes, uint32_t* compressedSizeBytes, uint32_t* compressedSizeBits)
{
    //Контекст кодирования LZW:
    int code = Nil;
    int codeBitsWidth = StartBits;
    Dictionary dictionary;

    //Выходной битовый поток, в который мы пишем. Это позволит выделить память по мере необходимости для размещения закодированных данных.
    BitStreamWriter bitStream;

    for (; uncompressedSizeBytes > 0; --uncompressedSizeBytes, ++uncompressed) {
        const int value = *uncompressed;
        const int index = dictionary.findIndex(code, value);

        if (index != Nil) {
            code = index;
            continue;
        }

        //Напишите код словаря, используя минимальный бит-с:
        bitStream.appendBitsU64(code, codeBitsWidth);

        //Очистите его, когда он заполнится, чтобы мы могли перезапустить последовательности.
        if (!dictionary.flush(codeBitsWidth)) {
            //Для этой последовательности еще есть место.
            dictionary.add(code, value);
        }
        code = value;
    }

    //Остаточный код в конце:
    if (code != Nil) bitStream.appendBitsU64(code, codeBitsWidth);

    //Передайте право собственности на буфер сжатых данных пользовательскому указателю:
    *compressedSizeBytes = bitStream.getByteCount();
    *compressedSizeBits = bitStream.numBitsWritten;

    return bitStream.release();
}


/************************************************************************/
/* B64 Реализация                                                   */
/************************************************************************/


size_t b64Decode(const char* encoded, const size_t len, char** decoded)
{
    static constexpr const char B64_INDEX[256] =
    {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  62, 63, 62, 62, 63, 52, 53, 54, 55, 56, 57,
        58, 59, 60, 61, 0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,
        7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
        25, 0,  0,  0,  0,  63, 0,  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
        37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
    };


    if (!decoded || !encoded || len == 0) return 0;

    auto reserved = 3 * (1 + (len >> 2)) + 1;
    auto output = static_cast<char*>(lv_malloc(reserved * sizeof(char)));
    LV_ASSERT_MALLOC(output);
    if (!output) return 0;
    output[reserved - 1] = '\0';

    size_t idx = 0;

    while (*encoded && *(encoded + 1)) {
        if (*encoded <= 0x20) {
            ++encoded;
            continue;
        }

        auto value1 = B64_INDEX[(size_t)encoded[0]];
        auto value2 = B64_INDEX[(size_t)encoded[1]];
        output[idx++] = (value1 << 2) + ((value2 & 0x30) >> 4);

        if (!encoded[2] || encoded[3] < 0 || encoded[2] == '=' || encoded[2] == '.') break;
        auto value3 = B64_INDEX[(size_t)encoded[2]];
        output[idx++] = ((value2 & 0x0f) << 4) + ((value3 & 0x3c) >> 2);

        if (!encoded[3] || encoded[3] < 0 || encoded[3] == '=' || encoded[3] == '.') break;
        auto value4 = B64_INDEX[(size_t)encoded[3]];
        output[idx++] = ((value3 & 0x03) << 6) + value4;
        encoded += 4;
    }
    *decoded = output;
    return idx;
}


/************************************************************************/
/* DJB2 Реализация                                                   */
/************************************************************************/

unsigned long djb2Encode(const char* str)
{
    if (!str) return 0;

    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // хеш * 33 + с
    }
    return hash;
}

}

#endif /* LV_USE_THORVG_INTERNAL */

