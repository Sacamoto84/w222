/*
 * QR Библиотека генератора кода (C)
 *
 * Copyright (c) Project Nayuki. (MIT License)
 * https://www.nayuki.io/page/qr-code-generator-library
 *
 * Разрешение настоящим предоставляется бесплатно любому лицу, получившему копию
 * данное программное обеспечение и связанные с ним файлы документации («Программное обеспечение») для решения
 * Программное обеспечение без ограничений, включая, помимо прочего, права на
 * использовать, копировать, изменять, объединять, публиковать, распространять, сублицензировать и/или продавать копии
 * Программное обеспечение и разрешать лицам, которым предоставлено Программное обеспечение, делать это,
 * при соблюдении следующих условий:
 * - The above copyright notice and this permission notice shall be included in
 *   все копии или существенные части Программного обеспечения.
 * - The Software is provided "as is", without warranty of any kind, express or
 *   подразумеваемые, включая, помимо прочего, гарантии товарной пригодности,
 *   пригодность для конкретной цели и отсутствие нарушений. Ни в коем случае
 *   авторы или правообладатели несут ответственность за любые претензии, ущерб или другие
 *   ответственность, будь то по договору, правонарушению или иным образом, вытекающая из:
 *   вне или в связи с Программным обеспечением или использованием или другими сделками в
 *   Программное обеспечение.
 */

#include "qrcodegen.h"
#include "../../misc/lv_assert.h"

#if LV_USE_QRCODE
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#ifndef QRCODEGEN_TEST
    #define testable static  // Сохраняйте функции конфиденциальными
#else
    #define testable  // Раскрытие частных функций
#endif


/*---- Предварительные объявления для частных функций ----*/

// Что касается всех общедоступных и частных функций, определенных в этом исходном файле:
// - They require all pointer/array arguments to be not null unless the array length is zero.
// - They only read input scalar/array arguments, write to output pointer/array
//   аргументы и возвращаемые скалярные значения; это «чистые» функции.
// - They don't read mutable global variables or write to any global variables.
// - They don't perform I/O, read the clock, print to console, etc.
// - They allocate a small and constant amount of stack memory.
// - They don't allocate or free any memory on the heap.
// - They don't recurse or mutually recurse. All the code
//   может быть встроен в публичные функции верхнего уровня.
// - They run in at most quadratic time with respect to input arguments.
//   Большинство функций выполняются за линейное время, а некоторые — за постоянное.
//   Здесь нет неограниченных циклов или неочевидных условий завершения.
// - They are completely thread-safe if the caller does not give the
//   тот же записываемый буфер для одновременных вызовов этих функций.

testable void appendBitsToBuffer(unsigned int val, int numBits, uint8_t buffer[], int * bitLen);

testable void addEccAndInterleave(uint8_t data[], int version, enum qrcodegen_Ecc ecl, uint8_t result[]);
testable int getNumDataCodewords(int version, enum qrcodegen_Ecc ecl);
testable int getNumRawDataModules(int ver);

testable void calcReedSolomonGenerator(int degree, uint8_t result[]);
testable void calcReedSolomonRemainder(const uint8_t data[], int dataLen,
                                       const uint8_t generator[], int degree, uint8_t result[]);
testable uint8_t finiteFieldMultiply(uint8_t x, uint8_t y);

testable void initializeFunctionModules(int version, uint8_t qrcode[]);
static void drawWhiteFunctionModules(uint8_t qrcode[], int version);
static void drawFormatBits(enum qrcodegen_Ecc ecl, enum qrcodegen_Mask mask, uint8_t qrcode[]);
testable int getAlignmentPatternPositions(int version, uint8_t result[7]);
static void fillRectangle(int left, int top, int width, int height, uint8_t qrcode[]);

static void drawCodewords(const uint8_t data[], int dataLen, uint8_t qrcode[]);
static void applyMask(const uint8_t functionModules[], uint8_t qrcode[], enum qrcodegen_Mask mask);
static long getPenaltyScore(const uint8_t qrcode[]);
static void addRunToHistory(unsigned char run, unsigned char history[7]);
static bool hasFinderLikePattern(const unsigned char runHistory[7]);

testable bool getModule(const uint8_t qrcode[], int x, int y);
testable void setModule(uint8_t qrcode[], int x, int y, bool isBlack);
testable void setModuleBounded(uint8_t qrcode[], int x, int y, bool isBlack);
static bool getBit(int x, int i);

testable int calcSegmentBitLength(enum qrcodegen_Mode mode, size_t numChars);
testable int getTotalBits(const struct qrcodegen_Segment segs[], size_t len, int version);
static int numCharCountBits(enum qrcodegen_Mode mode, int version);



/*---- Частные таблицы констант ----*/

// Набор всех допустимых символов в буквенно-цифровом режиме, где каждый символ
// значение сопоставляется с индексом в строке. Для проверки текста и сегментов кодирования.
static const char * ALPHANUMERIC_CHARSET = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

// Для генерации кодов исправления ошибок.
testable const int8_t ECC_CODEWORDS_PER_BLOCK[4][41] = {
    // Version: (note that index 0 is for padding, and is set to an illegal value)
    //0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40 Уровень коррекции ошибок
    {-1,  7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24, 26, 30, 22, 24, 28, 30, 28, 28, 28, 28, 30, 30, 26, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},  // Низкий
    {-1, 10, 16, 26, 18, 24, 16, 18, 22, 22, 26, 30, 22, 22, 24, 24, 28, 28, 26, 26, 26, 26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28},  // Средний
    {-1, 13, 22, 18, 26, 18, 24, 18, 22, 20, 24, 28, 26, 24, 20, 30, 24, 28, 28, 26, 30, 28, 30, 30, 30, 30, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},  // Квартиль
    {-1, 17, 28, 22, 16, 22, 28, 26, 26, 24, 28, 24, 28, 22, 24, 24, 30, 28, 28, 26, 28, 30, 24, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},  // Высокий
};

#define qrcodegen_REED_SOLOMON_DEGREE_MAX 30  // На основе таблицы выше

// Для генерации кодов исправления ошибок.
testable const int8_t NUM_ERROR_CORRECTION_BLOCKS[4][41] = {
    // Version: (note that index 0 is for padding, and is set to an illegal value)
    //0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40 Уровень коррекции ошибок
    {-1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4,  4,  4,  4,  4,  6,  6,  6,  6,  7,  8,  8,  9,  9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25},  // Низкий
    {-1, 1, 1, 1, 2, 2, 4, 4, 4, 5, 5,  5,  8,  9,  9, 10, 10, 11, 13, 14, 16, 17, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 33, 35, 37, 38, 40, 43, 45, 47, 49},  // Средний
    {-1, 1, 1, 2, 2, 4, 4, 6, 6, 8, 8,  8, 10, 12, 16, 12, 17, 16, 18, 21, 20, 23, 23, 25, 27, 29, 34, 34, 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62, 65, 68},  // Квартиль
    {-1, 1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16, 19, 21, 25, 25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81},  // Высокий
};

// Для автоматического выбора шаблона маски.
static const int PENALTY_N1 =  3;
static const int PENALTY_N2 =  3;
static const int PENALTY_N3 = 40;
static const int PENALTY_N4 = 10;



/*---- Функции кодирования кода высокого уровня QR ----*/

// Открытая функция — см. комментарий к документации в заголовочном файле.
bool qrcodegen_encodeText(const char * text, uint8_t tempBuffer[], uint8_t qrcode[],
                          enum qrcodegen_Ecc ecl, int minVersion, int maxVersion, enum qrcodegen_Mask mask, bool boostEcl)
{

    size_t textLen = strlen(text);
    if(textLen == 0)
        return qrcodegen_encodeSegmentsAdvanced(NULL, 0, ecl, minVersion, maxVersion, mask, boostEcl, tempBuffer, qrcode);
    size_t bufLen = qrcodegen_BUFFER_LEN_FOR_VERSION(maxVersion);

    struct qrcodegen_Segment seg;
    if(qrcodegen_isNumeric(text)) {
        if(qrcodegen_calcSegmentBufferSize(qrcodegen_Mode_NUMERIC, textLen) > bufLen)
            goto fail;
        seg = qrcodegen_makeNumeric(text, tempBuffer);
    }
    else if(qrcodegen_isAlphanumeric(text)) {
        if(qrcodegen_calcSegmentBufferSize(qrcodegen_Mode_ALPHANUMERIC, textLen) > bufLen)
            goto fail;
        seg = qrcodegen_makeAlphanumeric(text, tempBuffer);
    }
    else {
        if(textLen > bufLen)
            goto fail;
        for(size_t i = 0; i < textLen; i++)
            tempBuffer[i] = (uint8_t)text[i];
        seg.mode = qrcodegen_Mode_BYTE;
        seg.bitLength = calcSegmentBitLength(seg.mode, textLen);
        if(seg.bitLength == -1)
            goto fail;
        seg.numChars = (int)textLen;
        seg.data = tempBuffer;
    }
    return qrcodegen_encodeSegmentsAdvanced(&seg, 1, ecl, minVersion, maxVersion, mask, boostEcl, tempBuffer, qrcode);

fail:
    qrcode[0] = 0;  // Установите для размера недопустимое значение в целях безопасности.
    return false;
}


// Открытая функция — см. комментарий к документации в заголовочном файле.
bool qrcodegen_encodeBinary(uint8_t dataAndTemp[], size_t dataLen, uint8_t qrcode[],
                            enum qrcodegen_Ecc ecl, int minVersion, int maxVersion, enum qrcodegen_Mask mask, bool boostEcl)
{

    struct qrcodegen_Segment seg;
    seg.mode = qrcodegen_Mode_BYTE;
    seg.bitLength = calcSegmentBitLength(seg.mode, dataLen);
    if(seg.bitLength == -1) {
        qrcode[0] = 0;  // Установите для размера недопустимое значение в целях безопасности.
        return false;
    }
    seg.numChars = (int)dataLen;
    seg.data = dataAndTemp;
    return qrcodegen_encodeSegmentsAdvanced(&seg, 1, ecl, minVersion, maxVersion, mask, boostEcl, dataAndTemp, qrcode);
}


// Добавляет заданное количество младших бит заданного значения к заданному байтовому значению.
// битовый буфер, увеличивающий длину бита. Требуется 0 <= numBits <= 16 и val < 2^numBits.
testable void appendBitsToBuffer(unsigned int val, int numBits, uint8_t buffer[], int * bitLen)
{
    LV_ASSERT(0 <= numBits && numBits <= 16 && (unsigned long)val >> numBits == 0);
    for(int i = numBits - 1; i >= 0; i--, (*bitLen)++)
        buffer[*bitLen >> 3] |= ((val >> i) & 1) << (7 - (*bitLen & 7));
}



/*---- Функции кодирования низкоуровневого кода QR ----*/

// Открытая функция — см. комментарий к документации в заголовочном файле.
bool qrcodegen_encodeSegments(const struct qrcodegen_Segment segs[], size_t len,
                              enum qrcodegen_Ecc ecl, uint8_t tempBuffer[], uint8_t qrcode[])
{
    return qrcodegen_encodeSegmentsAdvanced(segs, len, ecl,
                                            qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, -1, true, tempBuffer, qrcode);
}


// Открытая функция — см. комментарий к документации в заголовочном файле.
bool qrcodegen_encodeSegmentsAdvanced(const struct qrcodegen_Segment segs[], size_t len, enum qrcodegen_Ecc ecl,
                                      int minVersion, int maxVersion, int mask, bool boostEcl, uint8_t tempBuffer[], uint8_t qrcode[])
{
    LV_ASSERT(segs != NULL || len == 0);
    LV_ASSERT(qrcodegen_VERSION_MIN <= minVersion && minVersion <= maxVersion && maxVersion <= qrcodegen_VERSION_MAX);
    LV_ASSERT(0 <= (int)ecl && (int)ecl <= 3 && -1 <= (int)mask && (int)mask <= 7);

    // Найдите минимальный номер версии для использования
    int version, dataUsedBits;
    for(version = minVersion; ; version++) {
        int dataCapacityBits = getNumDataCodewords(version, ecl) * 8;  // Количество доступных бит данных
        dataUsedBits = getTotalBits(segs, len, version);
        if(dataUsedBits != -1 && dataUsedBits <= dataCapacityBits)
            break;  // Этот номер версии признан подходящим
        if(version >= maxVersion) {   // Все версии в линейке не соответствуют заданным данным.
            qrcode[0] = 0;  // Установите для размера недопустимое значение в целях безопасности.
            return false;
        }
    }
    LV_ASSERT(dataUsedBits != -1);

    // Увеличьте уровень исправления ошибок, пока данные по-прежнему соответствуют текущему номеру версии.
    for(int i = (int)qrcodegen_Ecc_MEDIUM; i <= (int)qrcodegen_Ecc_HIGH; i++) {   // От низкого к высокому
        if(boostEcl && dataUsedBits <= getNumDataCodewords(version, (enum qrcodegen_Ecc)i) * 8)
            ecl = (enum qrcodegen_Ecc)i;
    }

    // Объедините все сегменты для создания строки битов данных.
    memset(qrcode, 0, qrcodegen_BUFFER_LEN_FOR_VERSION(version) * sizeof(qrcode[0]));
    int bitLen = 0;
    for(size_t i = 0; i < len; i++) {
        const struct qrcodegen_Segment * seg = &segs[i];
        appendBitsToBuffer((int)seg->mode, 4, qrcode, &bitLen);
        appendBitsToBuffer(seg->numChars, numCharCountBits(seg->mode, version), qrcode, &bitLen);
        for(int j = 0; j < seg->bitLength; j++)
            appendBitsToBuffer((seg->data[j >> 3] >> (7 - (j & 7))) & 1, 1, qrcode, &bitLen);
    }
    LV_ASSERT(bitLen == dataUsedBits);

    // Добавьте терминатор и дополните до байта, если это применимо.
    int dataCapacityBits = getNumDataCodewords(version, ecl) * 8;
    LV_ASSERT(bitLen <= dataCapacityBits);
    int terminatorBits = dataCapacityBits - bitLen;
    if(terminatorBits > 4)
        terminatorBits = 4;
    appendBitsToBuffer(0, terminatorBits, qrcode, &bitLen);
    appendBitsToBuffer(0, (8 - bitLen % 8) % 8, qrcode, &bitLen);
    LV_ASSERT(bitLen % 8 == 0);

    // Заполнение чередующимися байтами до тех пор, пока не будет достигнута емкость данных.
    for(uint8_t padByte = 0xEC; bitLen < dataCapacityBits; padByte ^= 0xEC ^ 0x11)
        appendBitsToBuffer(padByte, 8, qrcode, &bitLen);

    // Нарисуйте модули функций и кодовых слов данных
    addEccAndInterleave(qrcode, version, ecl, tempBuffer);
    initializeFunctionModules(version, qrcode);
    drawCodewords(tempBuffer, getNumRawDataModules(version) / 8, qrcode);
    drawWhiteFunctionModules(qrcode, version);
    initializeFunctionModules(version, tempBuffer);

    // Маскирование ручки
    if(mask == qrcodegen_Mask_AUTO) {   // Автоматически выбирать лучшую маску
        long minPenalty = LONG_MAX;
        for(int i = 0; i < 8; i++) {
            enum qrcodegen_Mask msk = (enum qrcodegen_Mask)i;
            applyMask(tempBuffer, qrcode, msk);
            drawFormatBits(ecl, msk, qrcode);
            long penalty = getPenaltyScore(qrcode);
            if(penalty < minPenalty) {
                mask = msk;
                minPenalty = penalty;
            }
            applyMask(tempBuffer, qrcode, msk);  // Отменяет маску из-за XOR
        }
    }
    LV_ASSERT(0 <= (int)mask && (int)mask <= 7);
    applyMask(tempBuffer, qrcode, mask);
    drawFormatBits(ecl, mask, qrcode);
    return true;
}



/*---- Функции генерации кода коррекции ошибок ----*/

// Добавляет байты исправления ошибок к каждому блоку данного массива данных, затем чередует
// байты из блоков и сохраняет их в массиве результатов. data[0 : dataLen] содержит
// входные данные. data[dataLen : rawCodewords] используется как временная рабочая область и будет
// быть забиты этой функцией. Окончательный ответ сохраняется в result[0 : rawCodewords].
testable void addEccAndInterleave(uint8_t data[], int version, enum qrcodegen_Ecc ecl, uint8_t result[])
{
    // Рассчитать номера параметров
    LV_ASSERT(0 <= (int)ecl && (int)ecl < 4 && qrcodegen_VERSION_MIN <= version && version <= qrcodegen_VERSION_MAX);
    int numBlocks = NUM_ERROR_CORRECTION_BLOCKS[(int)ecl][version];
    int blockEccLen = ECC_CODEWORDS_PER_BLOCK  [(int)ecl][version];
    int rawCodewords = getNumRawDataModules(version) / 8;
    int dataLen = getNumDataCodewords(version, ecl);
    int numShortBlocks = numBlocks - rawCodewords % numBlocks;
    int shortBlockDataLen = rawCodewords / numBlocks - blockEccLen;

    // Разделите данные на блоки, вычислите ECC и чередуйте
    // (не объединять) байты в одну последовательность
    uint8_t generator[qrcodegen_REED_SOLOMON_DEGREE_MAX];
    calcReedSolomonGenerator(blockEccLen, generator);
    const uint8_t * dat = data;
    for(int i = 0; i < numBlocks; i++) {
        int datLen = shortBlockDataLen + (i < numShortBlocks ? 0 : 1);
        uint8_t * ecc = &data[dataLen]; // Временное хранение
        calcReedSolomonRemainder(dat, datLen, generator, blockEccLen, ecc);
        for(int j = 0, k = i; j < datLen; j++, k += numBlocks) {   // Копировать данные
            if(j == shortBlockDataLen)
                k -= numShortBlocks;
            result[k] = dat[j];
        }
        for(int j = 0, k = dataLen + i; j < blockEccLen; j++, k += numBlocks)   // Копировать ECC
            result[k] = ecc[j];
        dat += datLen;
    }
}


// Возвращает количество 8-битных кодовых слов, которые можно использовать для хранения данных (не ECC ),
// для данного номера версии и уровня исправления ошибок. Результат находится в пределах [9, 2956].
testable int getNumDataCodewords(int version, enum qrcodegen_Ecc ecl)
{
    int v = version, e = (int)ecl;
    LV_ASSERT(0 <= e && e < 4);
    return getNumRawDataModules(v) / 8
           - ECC_CODEWORDS_PER_BLOCK    [e][v]
           * NUM_ERROR_CORRECTION_BLOCKS[e][v];
}


// Возвращает количество битов данных, которые могут быть сохранены в коде QR заданного номера версии после
// все функциональные модули исключены. Сюда входят биты остатка, поэтому оно может быть не кратно 8.
// Результат находится в диапазоне [208, 29648]. Это можно реализовать в виде таблицы поиска из 40 записей.
testable int getNumRawDataModules(int ver)
{
    LV_ASSERT(qrcodegen_VERSION_MIN <= ver && ver <= qrcodegen_VERSION_MAX);
    int result = (16 * ver + 128) * ver + 64;
    if(ver >= 2) {
        int numAlign = ver / 7 + 2;
        result -= (25 * numAlign - 10) * numAlign - 55;
        if(ver >= 7)
            result -= 36;
    }
    return result;
}



/*---- Функции генератора Рида-Соломона ECC ----*/

// Вычисляет полином генератора Рида-Соломона заданной степени, сохраняя в result[0 : степень].
testable void calcReedSolomonGenerator(int degree, uint8_t result[])
{
    // Начните с монома x^0
    LV_ASSERT(1 <= degree && degree <= qrcodegen_REED_SOLOMON_DEGREE_MAX);
    memset(result, 0, degree * sizeof(result[0]));
    result[degree - 1] = 1;

    // Вычислите полином произведения (x - r^0) * (x - r^1) * (x - r^2) * ... * (x - r^{степень-1}),
    // отбросьте старший член и сохраните остальные коэффициенты в порядке убывания степени.
    // Обратите внимание, что r = 0x02, который является порождающим элементом этого поля GF (2^8/ 0x11D).
    uint8_t root = 1;
    for(int i = 0; i < degree; i++) {
        // Умножьте текущий продукт на (x - r^i)
        for(int j = 0; j < degree; j++) {
            result[j] = finiteFieldMultiply(result[j], root);
            if(j + 1 < degree)
                result[j] ^= result[j + 1];
        }
        root = finiteFieldMultiply(root, 0x02);
    }
}


// Вычисляет остаток полинома data[0 : dataLen] при делении на генератор[0 : степень], где все
// полиномы имеют обратный порядок байтов, а генератор имеет неявный ведущий член 1, сохраняющий результат в result[0 : степень].
testable void calcReedSolomonRemainder(const uint8_t data[], int dataLen,
                                       const uint8_t generator[], int degree, uint8_t result[])
{

    // Выполнить полиномиальное деление
    LV_ASSERT(1 <= degree && degree <= qrcodegen_REED_SOLOMON_DEGREE_MAX);
    memset(result, 0, degree * sizeof(result[0]));
    for(int i = 0; i < dataLen; i++) {
        uint8_t factor = data[i] ^ result[0];
        memmove(&result[0], &result[1], (degree - 1) * sizeof(result[0]));
        result[degree - 1] = 0;
        for(int j = 0; j < degree; j++)
            result[j] ^= finiteFieldMultiply(generator[j], factor);
    }
}

#undef qrcodegen_REED_SOLOMON_DEGREE_MAX


// Возвращает произведение двух заданных элементов поля по модулю GF (2^8/ 0x11D ).
// Все входные данные действительны. Это можно реализовать как справочную таблицу размером 256*256.
testable uint8_t finiteFieldMultiply(uint8_t x, uint8_t y)
{
    // Русское крестьянское умножение
    uint8_t z = 0;
    for(int i = 7; i >= 0; i--) {
        z = (z << 1) ^ ((z >> 7) * 0x11D);
        z ^= ((y >> i) & 1) * x;
    }
    return z;
}



/*---- Функциональные модули рисования ----*/

// Очищает данную сетку кода QR с белыми модулями для заданных
// размер версии, затем помечает каждый функциональный модуль черным цветом.
testable void initializeFunctionModules(int version, uint8_t qrcode[])
{
    // Инициализируйте код QR
    int qrsize = version * 4 + 17;
    memset(qrcode, 0, ((qrsize * qrsize + 7) / 8 + 1) * sizeof(qrcode[0]));
    qrcode[0] = (uint8_t)qrsize;

    // Заполните горизонтальные и вертикальные временные шаблоны
    fillRectangle(6, 0, 1, qrsize, qrcode);
    fillRectangle(0, 6, qrsize, 1, qrcode);

    // Заполните 3 шаблона поиска (все углы, кроме нижнего правого) и отформатируйте биты.
    fillRectangle(0, 0, 9, 9, qrcode);
    fillRectangle(qrsize - 8, 0, 8, 9, qrcode);
    fillRectangle(0, qrsize - 8, 9, 8, qrcode);

    // Заполните многочисленные шаблоны выравнивания
    uint8_t alignPatPos[7];
    int numAlign = getAlignmentPatternPositions(version, alignPatPos);
    for(int i = 0; i < numAlign; i++) {
        for(int j = 0; j < numAlign; j++) {
            // Не рисуйте по трем углам видоискателя
            if(!((i == 0 && j == 0) || (i == 0 && j == numAlign - 1) || (i == numAlign - 1 && j == 0)))
                fillRectangle(alignPatPos[i] - 2, alignPatPos[j] - 2, 5, 5, qrcode);
        }
    }

    // Заполнить блоки версий
    if(version >= 7) {
        fillRectangle(qrsize - 11, 0, 3, 6, qrcode);
        fillRectangle(0, qrsize - 11, 6, 3, qrcode);
    }
}


// Рисует белые функциональные модули и, возможно, некоторые черные модули в заданном коде QR без изменений.
// нефункциональные модули. Это не рисует биты формата. Для этого необходимо, чтобы все функциональные модули были предварительно
// помечены черным (а именно initializeFunctionModules() ), поскольку при этом можно пропустить перерисовку черных функциональных модулей.
static void drawWhiteFunctionModules(uint8_t qrcode[], int version)
{
    // Нарисуйте горизонтальные и вертикальные временные шаблоны
    int qrsize = qrcodegen_getSize(qrcode);
    for(int i = 7; i < qrsize - 7; i += 2) {
        setModule(qrcode, 6, i, false);
        setModule(qrcode, i, 6, false);
    }

    // Нарисуйте 3 шаблона поиска (все углы, кроме правого нижнего; перезаписывает некоторые модули синхронизации)
    for(int dy = -4; dy <= 4; dy++) {
        for(int dx = -4; dx <= 4; dx++) {
            int dist = abs(dx);
            if(abs(dy) > dist)
                dist = abs(dy);
            if(dist == 2 || dist == 4) {
                setModuleBounded(qrcode, 3 + dx, 3 + dy, false);
                setModuleBounded(qrcode, qrsize - 4 + dx, 3 + dy, false);
                setModuleBounded(qrcode, 3 + dx, qrsize - 4 + dy, false);
            }
        }
    }

    // Нарисуйте многочисленные шаблоны выравнивания
    uint8_t alignPatPos[7];
    int numAlign = getAlignmentPatternPositions(version, alignPatPos);
    for(int i = 0; i < numAlign; i++) {
        for(int j = 0; j < numAlign; j++) {
            if((i == 0 && j == 0) || (i == 0 && j == numAlign - 1) || (i == numAlign - 1 && j == 0))
                continue;  // Не рисуйте по трем углам видоискателя
            for(int dy = -1; dy <= 1; dy++) {
                for(int dx = -1; dx <= 1; dx++)
                    setModule(qrcode, alignPatPos[i] + dx, alignPatPos[j] + dy, dx == 0 && dy == 0);
            }
        }
    }

    // Нарисовать блоки версий
    if(version >= 7) {
        // Вычислить код исправления ошибок и упаковать биты
        int rem = version;  // версия — uint6, в диапазоне [7, 40]
        for(int i = 0; i < 12; i++)
            rem = (rem << 1) ^ ((rem >> 11) * 0x1F25);
        long bits = (long)version << 12 | rem;  // uint18
        LV_ASSERT(bits >> 18 == 0);

        // Нарисуйте две копии
        for(int i = 0; i < 6; i++) {
            for(int j = 0; j < 3; j++) {
                int k = qrsize - 11 + j;
                setModule(qrcode, k, i, (bits & 1) != 0);
                setModule(qrcode, i, k, (bits & 1) != 0);
                bits >>= 1;
            }
        }
    }
}


// Рисует две копии битов формата (со своим собственным кодом исправления ошибок) на основе
// по заданной маске и уровню коррекции ошибок. Это всегда рисует все модули
// биты формата, в отличие от drawWhiteFunctionModules(), который может пропускать черные модули.
static void drawFormatBits(enum qrcodegen_Ecc ecl, enum qrcodegen_Mask mask, uint8_t qrcode[])
{
    // Вычислить код исправления ошибок и упаковать биты
    LV_ASSERT(0 <= (int)mask && (int)mask <= 7);
    static const int table[] = {1, 0, 3, 2};
    int data = table[(int)ecl] << 3 | (int)mask;  // errCorrLvl — uint2, маска — uint3
    int rem = data;
    for(int i = 0; i < 10; i++)
        rem = (rem << 1) ^ ((rem >> 9) * 0x537);
    int bits = (data << 10 | rem) ^ 0x5412;  // uint15
    LV_ASSERT(bits >> 15 == 0);

    // Нарисуйте первую копию
    for(int i = 0; i <= 5; i++)
        setModule(qrcode, 8, i, getBit(bits, i));
    setModule(qrcode, 8, 7, getBit(bits, 6));
    setModule(qrcode, 8, 8, getBit(bits, 7));
    setModule(qrcode, 7, 8, getBit(bits, 8));
    for(int i = 9; i < 15; i++)
        setModule(qrcode, 14 - i, 8, getBit(bits, i));

    // Нарисуйте вторую копию
    int qrsize = qrcodegen_getSize(qrcode);
    for(int i = 0; i < 8; i++)
        setModule(qrcode, qrsize - 1 - i, 8, getBit(bits, i));
    for(int i = 8; i < 15; i++)
        setModule(qrcode, 8, qrsize - 15 + i, getBit(bits, i));
    setModule(qrcode, 8, qrsize - 8, true);  // Всегда черный
}


// Вычисляет и сохраняет возрастающий список позиций шаблонов выравнивания.
// для этого номера версии, возвращая длину списка (в диапазоне [0,7]).
// Каждая позиция находится в диапазоне [0,177) и используется как по осям x, так и по осям y.
// Это можно реализовать в виде таблицы поиска из 40 списков беззнаковых байтов переменной длины.
testable int getAlignmentPatternPositions(int version, uint8_t result[7])
{
    if(version == 1)
        return 0;
    int numAlign = version / 7 + 2;
    int step = (version == 32) ? 26 :
               (version * 4 + numAlign * 2 + 1) / (numAlign * 2 - 2) * 2;
    for(int i = numAlign - 1, pos = version * 4 + 10; i >= 1; i--, pos -= step)
        result[i] = pos;
    result[0] = 6;
    return numAlign;
}


// Устанавливает каждый пиксель в диапазоне [левый: левый + ширина] * [верхний: верх + высота] черным.
static void fillRectangle(int left, int top, int width, int height, uint8_t qrcode[])
{
    for(int dy = 0; dy < height; dy++) {
        for(int dx = 0; dx < width; dx++)
            setModule(qrcode, left + dx, top + dy, true);
    }
}



/*---- Отрисовка модулей данных и маскировка ----*/

// Рисует необработанные кодовые слова (включая данные и ECC ) в заданный код QR. Для этого необходимо исходное состояние
// Код QR должен быть черным для функциональных модулей и белым для модулей кодовых слов (включая неиспользуемые биты остатка).
static void drawCodewords(const uint8_t data[], int dataLen, uint8_t qrcode[])
{
    int qrsize = qrcodegen_getSize(qrcode);
    int i = 0;  // Битовый индекс в данных
    // Сделайте забавное зигзагообразное сканирование.
    for(int right = qrsize - 1; right >= 1; right -= 2) {   // Индекс правого столбца в каждой паре столбцов
        if(right == 6)
            right = 5;
        for(int vert = 0; vert < qrsize; vert++) {   // Вертикальный счетчик
            for(int j = 0; j < 2; j++) {
                int x = right - j;  // Фактическая координата X
                bool upward = ((right + 1) & 2) == 0;
                int y = upward ? qrsize - 1 - vert : vert;  // Фактическая координата Y
                if(!getModule(qrcode, x, y) && i < dataLen * 8) {
                    bool black = getBit(data[i >> 3], 7 - (i & 7));
                    setModule(qrcode, x, y, black);
                    i++;
                }
                // Если этот код QR имеет какие-либо оставшиеся биты (от 0 до 7), они были назначены как
                // 0/false/white конструктором и этим методом не изменяется.
            }
        }
    }
    LV_ASSERT(i == dataLen * 8);
}


// Выполняет XOR модулей кодовых слов в этом коде QR с заданным шаблоном маски.
// Функциональные модули должны быть отмечены и биты кодового слова должны быть нарисованы.
// перед маскировкой. Из-за арифметики XOR вызов applyMask() с помощью
// то же значение маски во второй раз приведет к отмене маски. Окончательный хорошо сформированный
// Для кода QR требуется применить ровно одну (а не ноль, две и т. д.) маску.
static void applyMask(const uint8_t functionModules[], uint8_t qrcode[], enum qrcodegen_Mask mask)
{
    LV_ASSERT(0 <= (int)mask && (int)mask <= 7);  // Запрещает qrcodegen_Mask_AUTO
    int qrsize = qrcodegen_getSize(qrcode);
    for(int y = 0; y < qrsize; y++) {
        for(int x = 0; x < qrsize; x++) {
            if(getModule(functionModules, x, y))
                continue;
            bool invert;
            switch((int)mask) {
                case 0:
                    invert = (x + y) % 2 == 0;
                    break;
                case 1:
                    invert = y % 2 == 0;
                    break;
                case 2:
                    invert = x % 3 == 0;
                    break;
                case 3:
                    invert = (x + y) % 3 == 0;
                    break;
                case 4:
                    invert = (x / 3 + y / 2) % 2 == 0;
                    break;
                case 5:
                    invert = x * y % 2 + x * y % 3 == 0;
                    break;
                case 6:
                    invert = (x * y % 2 + x * y % 3) % 2 == 0;
                    break;
                case 7:
                    invert = ((x + y) % 2 + x * y % 3) % 2 == 0;
                    break;
                default:
                    LV_ASSERT(false);
                    return;
            }
            bool val = getModule(qrcode, x, y);
            setModule(qrcode, x, y, val ^ invert);
        }
    }
}


// Вычисляет и возвращает штрафной балл на основе состояния текущих модулей данного кода QR.
// Это используется алгоритмом автоматического выбора маски для поиска шаблона маски, который дает наименьший балл.
static long getPenaltyScore(const uint8_t qrcode[])
{
    int qrsize = qrcodegen_getSize(qrcode);
    long result = 0;

    // Соседние модули в ряду имеют одинаковый цвет и узоры, похожие на искатель.
    for(int y = 0; y < qrsize; y++) {
        unsigned char runHistory[7] = {0};
        bool color = false;
        unsigned char runX = 0;
        for(int x = 0; x < qrsize; x++) {
            if(getModule(qrcode, x, y) == color) {
                runX++;
                if(runX == 5)
                    result += PENALTY_N1;
                else if(runX > 5)
                    result++;
            }
            else {
                addRunToHistory(runX, runHistory);
                if(!color && hasFinderLikePattern(runHistory))
                    result += PENALTY_N3;
                color = getModule(qrcode, x, y);
                runX = 1;
            }
        }
        addRunToHistory(runX, runHistory);
        if(color)
            addRunToHistory(0, runHistory);  // Пустышка белого цвета
        if(hasFinderLikePattern(runHistory))
            result += PENALTY_N3;
    }
    // Соседние модули в столбце имеют одинаковый цвет и узоры в виде искателя.
    for(int x = 0; x < qrsize; x++) {
        unsigned char runHistory[7] = {0};
        bool color = false;
        unsigned char runY = 0;
        for(int y = 0; y < qrsize; y++) {
            if(getModule(qrcode, x, y) == color) {
                runY++;
                if(runY == 5)
                    result += PENALTY_N1;
                else if(runY > 5)
                    result++;
            }
            else {
                addRunToHistory(runY, runHistory);
                if(!color && hasFinderLikePattern(runHistory))
                    result += PENALTY_N3;
                color = getModule(qrcode, x, y);
                runY = 1;
            }
        }
        addRunToHistory(runY, runHistory);
        if(color)
            addRunToHistory(0, runHistory);  // Пустышка белого цвета
        if(hasFinderLikePattern(runHistory))
            result += PENALTY_N3;
    }

    // 2*2 блока модулей одного цвета
    for(int y = 0; y < qrsize - 1; y++) {
        for(int x = 0; x < qrsize - 1; x++) {
            bool  color = getModule(qrcode, x, y);
            if(color == getModule(qrcode, x + 1, y) &&
               color == getModule(qrcode, x, y + 1) &&
               color == getModule(qrcode, x + 1, y + 1))
                result += PENALTY_N2;
        }
    }

    // Баланс чёрных и белых модулей
    int black = 0;
    for(int y = 0; y < qrsize; y++) {
        for(int x = 0; x < qrsize; x++) {
            if(getModule(qrcode, x, y))
                black++;
        }
    }
    int total = qrsize * qrsize;  // Обратите внимание, что размер нечетный, поэтому черный/всего!= 1/2.
    // Вычислите наименьшее целое число k >= 0 такое, что (45-5k)% <= black/total <= (55+5k)%
    int k = (int)((labs(black * 20L - total * 10L) + total - 1) / total) - 1;
    result += k * PENALTY_N4;
    return result;
}


// Вставляет заданное значение в начало данного массива, который смещается по
// существующие значения и удаляет последнее значение. Вспомогательная функция для getPenaltyScore().
static void addRunToHistory(unsigned char run, unsigned char history[7])
{
    memmove(&history[1], &history[0], 6 * sizeof(history[0]));
    history[0] = run;
}


// Проверяет, имеет ли данная история выполнения шаблон соотношения 1:1:3:1:1 в середине, и
// окружен как минимум 4 на одном или обоих концах. Вспомогательная функция для getPenaltyScore().
// Должен вызываться только сразу после завершения выполнения белых модулей.
static bool hasFinderLikePattern(const unsigned char runHistory[7])
{
    unsigned char n = runHistory[1];
    // Максимальный размер кода QR составляет 177, следовательно, длина серии n <= 177.
    // Арифметика преобразуется в int, поэтому n*4 не будет переполняться.
    return n > 0 && runHistory[2] == n && runHistory[4] == n && runHistory[5] == n
           && runHistory[3] == n * 3 && (runHistory[0] >= n * 4 || runHistory[6] >= n * 4);
}



/*---- Базовая информация о коде QR ----*/

// Открытая функция — см. комментарий к документации в заголовочном файле.
int qrcodegen_getSize(const uint8_t qrcode[])
{
    LV_ASSERT(qrcode != NULL);
    int result = qrcode[0];
    LV_ASSERT((qrcodegen_VERSION_MIN * 4 + 17) <= result
           && result <= (qrcodegen_VERSION_MAX * 4 + 17));
    return result;
}


// Открытая функция — см. комментарий к документации в заголовочном файле.
bool qrcodegen_getModule(const uint8_t qrcode[], int x, int y)
{
    LV_ASSERT(qrcode != NULL);
    int qrsize = qrcode[0];
    return (0 <= x && x < qrsize && 0 <= y && y < qrsize) && getModule(qrcode, x, y);
}


// Получает модуль по заданным координатам, которые должны находиться в пределах.
testable bool getModule(const uint8_t qrcode[], int x, int y)
{
    int qrsize = qrcode[0];
    LV_ASSERT(21 <= qrsize && qrsize <= 177 && 0 <= x && x < qrsize && 0 <= y && y < qrsize);
    int index = y * qrsize + x;
    return getBit(qrcode[(index >> 3) + 1], index & 7);
}


// Устанавливает модуль по заданным координатам, которые должны находиться в пределах.
testable void setModule(uint8_t qrcode[], int x, int y, bool isBlack)
{
    int qrsize = qrcode[0];
    LV_ASSERT(21 <= qrsize && qrsize <= 177 && 0 <= x && x < qrsize && 0 <= y && y < qrsize);
    int index = y * qrsize + x;
    int bitIndex = index & 7;
    int byteIndex = (index >> 3) + 1;
    if(isBlack)
        qrcode[byteIndex] |= 1 << bitIndex;
    else
        qrcode[byteIndex] &= (1 << bitIndex) ^ 0xFF;
}


// Устанавливает модуль по заданным координатам, ничего не делая, если он выходит за пределы.
testable void setModuleBounded(uint8_t qrcode[], int x, int y, bool isBlack)
{
    int qrsize = qrcode[0];
    if(0 <= x && x < qrsize && 0 <= y && y < qrsize)
        setModule(qrcode, x, y, isBlack);
}


// Возвращает true, если i-й бит x установлен в 1. Требуется x >= 0 и 0 <= i <= 14.
static bool getBit(int x, int i)
{
    return ((x >> i) & 1) != 0;
}



/*---- Обработка сегментов ----*/

// Открытая функция — см. комментарий к документации в заголовочном файле.
bool qrcodegen_isAlphanumeric(const char * text)
{
    LV_ASSERT(text != NULL);
    for(; *text != '\0'; text++) {
        if(strchr(ALPHANUMERIC_CHARSET, *text) == NULL)
            return false;
    }
    return true;
}


// Открытая функция — см. комментарий к документации в заголовочном файле.
bool qrcodegen_isNumeric(const char * text)
{
    LV_ASSERT(text != NULL);
    for(; *text != '\0'; text++) {
        if(*text < '0' || *text > '9')
            return false;
    }
    return true;
}


// Открытая функция — см. комментарий к документации в заголовочном файле.
size_t qrcodegen_calcSegmentBufferSize(enum qrcodegen_Mode mode, size_t numChars)
{
    int temp = calcSegmentBitLength(mode, numChars);
    if(temp == -1)
        return SIZE_MAX;
    LV_ASSERT(0 <= temp && temp <= INT16_MAX);
    return ((size_t)temp + 7) / 8;
}


// Возвращает количество битов данных, необходимых для представления сегмента.
// содержащий заданное количество символов с использованием данного режима. Примечания:
// - Returns -1 on failure, i.e. numChars > INT16_MAX or
//   количество необходимых бит превышает INT16_MAX (т.е. 32767).
// - Otherwise, all valid results are in the range [0, INT16_MAX].
// - For byte mode, numChars measures the number of bytes, not Unicode code points.
// - For ECI mode, numChars must be 0, and the worst-case number of bits is returned.
//   Фактический сегмент ECI может содержать более короткие данные. Для режимов, отличных от ECI, результат является точным.
testable int calcSegmentBitLength(enum qrcodegen_Mode mode, size_t numChars)
{
    // Все расчеты разработаны таким образом, чтобы избежать переполнения на всех платформах.
    if(numChars > (unsigned int)INT16_MAX)
        return -1;
    long result = (long)numChars;
    if(mode == qrcodegen_Mode_NUMERIC)
        result = (result * 10 + 2) / 3;  // ячейка(10/3 * n)
    else if(mode == qrcodegen_Mode_ALPHANUMERIC)
        result = (result * 11 + 1) / 2;  // ячейка(11/2 * n)
    else if(mode == qrcodegen_Mode_BYTE)
        result *= 8;
    else if(mode == qrcodegen_Mode_KANJI)
        result *= 13;
    else if(mode == qrcodegen_Mode_ECI && numChars == 0)
        result = 3 * 8;
    else {  // Неверный аргумент
        LV_ASSERT(false);
        return -1;
    }
    LV_ASSERT(result >= 0);
    if((unsigned int)result > (unsigned int)INT16_MAX)
        return -1;
    return (int)result;
}


// Открытая функция — см. комментарий к документации в заголовочном файле.
struct qrcodegen_Segment qrcodegen_makeBytes(const uint8_t data[], size_t len, uint8_t buf[])
{
    LV_ASSERT(data != NULL || len == 0);
    struct qrcodegen_Segment result;
    result.mode = qrcodegen_Mode_BYTE;
    result.bitLength = calcSegmentBitLength(result.mode, len);
    LV_ASSERT(result.bitLength != -1);
    result.numChars = (int)len;
    if(len > 0)
        memcpy(buf, data, len * sizeof(buf[0]));
    result.data = buf;
    return result;
}


// Открытая функция — см. комментарий к документации в заголовочном файле.
struct qrcodegen_Segment qrcodegen_makeNumeric(const char * digits, uint8_t buf[])
{
    LV_ASSERT(digits != NULL);
    struct qrcodegen_Segment result;
    size_t len = strlen(digits);
    result.mode = qrcodegen_Mode_NUMERIC;
    int bitLen = calcSegmentBitLength(result.mode, len);
    LV_ASSERT(bitLen != -1);
    result.numChars = (int)len;
    if(bitLen > 0)
        memset(buf, 0, ((size_t)bitLen + 7) / 8 * sizeof(buf[0]));
    result.bitLength = 0;

    unsigned int accumData = 0;
    int accumCount = 0;
    for(; *digits != '\0'; digits++) {
        char c = *digits;
        LV_ASSERT('0' <= c && c <= '9');
        accumData = accumData * 10 + (unsigned int)(c - '0');
        accumCount++;
        if(accumCount == 3) {
            appendBitsToBuffer(accumData, 10, buf, &result.bitLength);
            accumData = 0;
            accumCount = 0;
        }
    }
    if(accumCount > 0)   // Осталась 1 или 2 цифры
        appendBitsToBuffer(accumData, accumCount * 3 + 1, buf, &result.bitLength);
    LV_ASSERT(result.bitLength == bitLen);
    result.data = buf;
    return result;
}


// Открытая функция — см. комментарий к документации в заголовочном файле.
struct qrcodegen_Segment qrcodegen_makeAlphanumeric(const char * text, uint8_t buf[])
{
    LV_ASSERT(text != NULL);
    struct qrcodegen_Segment result;
    size_t len = strlen(text);
    result.mode = qrcodegen_Mode_ALPHANUMERIC;
    int bitLen = calcSegmentBitLength(result.mode, len);
    LV_ASSERT(bitLen != -1);
    result.numChars = (int)len;
    if(bitLen > 0)
        memset(buf, 0, ((size_t)bitLen + 7) / 8 * sizeof(buf[0]));
    result.bitLength = 0;

    unsigned int accumData = 0;
    int accumCount = 0;
    for(; *text != '\0'; text++) {
        const char * temp = strchr(ALPHANUMERIC_CHARSET, *text);
        LV_ASSERT(temp != NULL);
        accumData = accumData * 45 + (unsigned int)(temp - ALPHANUMERIC_CHARSET);
        accumCount++;
        if(accumCount == 2) {
            appendBitsToBuffer(accumData, 11, buf, &result.bitLength);
            accumData = 0;
            accumCount = 0;
        }
    }
    if(accumCount > 0)   // остался 1 символ
        appendBitsToBuffer(accumData, 6, buf, &result.bitLength);
    LV_ASSERT(result.bitLength == bitLen);
    result.data = buf;
    return result;
}


// Открытая функция — см. комментарий к документации в заголовочном файле.
struct qrcodegen_Segment qrcodegen_makeEci(long assignVal, uint8_t buf[])
{
    struct qrcodegen_Segment result;
    result.mode = qrcodegen_Mode_ECI;
    result.numChars = 0;
    result.bitLength = 0;
    if(assignVal < 0) {
        LV_ASSERT(false);
    }
    else if(assignVal < (1 << 7)) {
        memset(buf, 0, 1 * sizeof(buf[0]));
        appendBitsToBuffer(assignVal, 8, buf, &result.bitLength);
    }
    else if(assignVal < (1 << 14)) {
        memset(buf, 0, 2 * sizeof(buf[0]));
        appendBitsToBuffer(2, 2, buf, &result.bitLength);
        appendBitsToBuffer(assignVal, 14, buf, &result.bitLength);
    }
    else if(assignVal < 1000000L) {
        memset(buf, 0, 3 * sizeof(buf[0]));
        appendBitsToBuffer(6, 3, buf, &result.bitLength);
        appendBitsToBuffer(assignVal >> 10, 11, buf, &result.bitLength);
        appendBitsToBuffer(assignVal & 0x3FF, 10, buf, &result.bitLength);
    }
    else {
        LV_ASSERT(false);
    }
    result.data = buf;
    return result;
}


// Вычисляет количество бит, необходимое для кодирования заданных сегментов в данной версии.
// Возвращает неотрицательное число в случае успеха. В противном случае возвращается -1, если сегмент имеет слишком
// много символов, соответствующих его полю длины, или общее количество бит превышает INT16_MAX .
testable int getTotalBits(const struct qrcodegen_Segment segs[], size_t len, int version)
{
    LV_ASSERT(segs != NULL || len == 0);
    long result = 0;
    for(size_t i = 0; i < len; i++) {
        int numChars  = segs[i].numChars;
        int bitLength = segs[i].bitLength;
        LV_ASSERT(0 <= numChars  && numChars  <= INT16_MAX);
        LV_ASSERT(0 <= bitLength && bitLength <= INT16_MAX);
        int ccbits = numCharCountBits(segs[i].mode, version);
        LV_ASSERT(0 <= ccbits && ccbits <= 16);
        if(numChars >= (1L << ccbits))
            return -1;  // Длина сегмента не соответствует разрядности поля.
        result += 4L + ccbits + bitLength;
        if(result > INT16_MAX)
            return -1;  // Сумма может переполнить тип int
    }
    LV_ASSERT(0 <= result && result <= INT16_MAX);
    return (int)result;
}


// Возвращает разрядность поля количества символов для сегмента в заданном режиме.
// в коде QR с заданным номером версии. Результат находится в диапазоне [0, 16].
static int numCharCountBits(enum qrcodegen_Mode mode, int version)
{
    LV_ASSERT(qrcodegen_VERSION_MIN <= version && version <= qrcodegen_VERSION_MAX);
    int i = (version + 7) / 17;
    switch(mode) {
        case qrcodegen_Mode_NUMERIC     : {
                static const int temp[] = {10, 12, 14};
                return temp[i];
            }
        case qrcodegen_Mode_ALPHANUMERIC: {
                static const int temp[] = { 9, 11, 13};
                return temp[i];
            }
        case qrcodegen_Mode_BYTE        : {
                static const int temp[] = { 8, 16, 16};
                return temp[i];
            }
        case qrcodegen_Mode_KANJI       : {
                static const int temp[] = { 8, 10, 12};
                return temp[i];
            }
        case qrcodegen_Mode_ECI         :
            return 0;
        default:
            LV_ASSERT(false);
            return -1;  // Фиктивное значение
    }
}

int qrcodegen_getMinFitVersion(enum qrcodegen_Ecc ecl, size_t dataLen)
{
    struct qrcodegen_Segment seg;
    seg.mode = qrcodegen_Mode_BYTE;
    seg.bitLength = calcSegmentBitLength(seg.mode, dataLen);
    seg.numChars = (int)dataLen;

    for(int version = qrcodegen_VERSION_MIN; version <= qrcodegen_VERSION_MAX; version++) {
        int dataCapacityBits = getNumDataCodewords(version, ecl) * 8;  // Количество доступных бит данных
        int dataUsedBits = getTotalBits(&seg, 1, version);
        if(dataUsedBits != -1 && dataUsedBits <= dataCapacityBits)
            return version;
    }
    return -1;
}

int qrcodegen_version2size(int version)
{
    if(version < qrcodegen_VERSION_MIN || version > qrcodegen_VERSION_MAX) {
        return -1;
    }

    return ((version - 1) * 4 + 21);
}
#endif
