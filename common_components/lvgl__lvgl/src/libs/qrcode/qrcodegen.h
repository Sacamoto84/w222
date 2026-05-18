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

#pragma once

#include "../../../lvgl.h"
#ifdef LV_USE_QRCODE

#include LV_STDBOOL_INCLUDE
#include LV_STDDEF_INCLUDE
#include LV_STDINT_INCLUDE


#ifdef __cplusplus
extern "C" {
#endif


/*
 * Эта библиотека создает символы кода QR, которые представляют собой тип двумерного штрих-кода.
 * Изобретён Denso Wave и описан в стандарте ISO/IEC 18004.
 * Структура кода QR представляет собой неизменяемую квадратную сетку из черных и белых ячеек.
 * Библиотека предоставляет функции для создания кода QR из текстовых или двоичных данных.
 * Библиотека охватывает спецификацию QR Code Model 2, поддерживая все версии (размеры).
 * от 1 до 40, все 4 уровня исправления ошибок и 4 режима кодировки символов.
 *
 * Способы создания объекта кода QR:
 * - High level: Take the payload data and call qrcodegen_encodeText() or qrcodegen_encodeBinary().
 * - Low level: Custom-make the list of segments and call
 *   qrcodegen_encodeSegments() или qrcodegen_encodeSegmentsAdvanced().
 * (Обратите внимание, что все способы требуют предоставления желаемого уровня исправления ошибок и различных байтовых буферов.)
 */


/*---- Типы перечислений и структур ----*/

/*
 * Уровень исправления ошибок в символе кода QR.
 */
enum qrcodegen_Ecc {
    // Должно быть объявлено в порядке возрастания защиты от ошибок.
    // чтобы внутренняя функция qrcodegen работала правильно
    qrcodegen_Ecc_LOW = 0,   // Код QR допускает около 7% ошибочных кодовых слов.
    qrcodegen_Ecc_MEDIUM,    // Код QR допускает около 15% ошибочных кодовых слов.
    qrcodegen_Ecc_QUARTILE,  // Код QR допускает около 25% ошибочных кодовых слов.
    qrcodegen_Ecc_HIGH,      // Код QR допускает около 30% ошибочных кодовых слов.
};


/*
 * Шаблон маски, используемый в символе кода QR.
 */
enum qrcodegen_Mask {
    // Специальное значение, которое нужно указать кодировщику кода QR.
    // автоматически выбирает подходящий шаблон маски
    qrcodegen_Mask_AUTO = -1,
    // Восемь реальных шаблонов масок
    qrcodegen_Mask_0 = 0,
    qrcodegen_Mask_1,
    qrcodegen_Mask_2,
    qrcodegen_Mask_3,
    qrcodegen_Mask_4,
    qrcodegen_Mask_5,
    qrcodegen_Mask_6,
    qrcodegen_Mask_7,
};


/*
 * Описывает, как интерпретируются биты данных сегмента.
 */
enum qrcodegen_Mode {
    qrcodegen_Mode_NUMERIC      = 0x1,
    qrcodegen_Mode_ALPHANUMERIC = 0x2,
    qrcodegen_Mode_BYTE         = 0x4,
    qrcodegen_Mode_KANJI        = 0x8,
    qrcodegen_Mode_ECI          = 0x7,
};


/*
 * Сегмент символьных/двоичных/управляющих данных в символе кода QR.
 * Промежуточный способ создания сегмента — взять данные полезной нагрузки.
 * и вызвать фабричную функцию, например qrcodegen_makeNumeric().
 * Низкоуровневый способ создания сегмента — создание битового буфера по индивидуальному заказу.
 * и инициализируйте структуру qrcodegen_Segment соответствующими значениями.
 * Даже в самых благоприятных условиях код QR может содержать только 7089 символов данных.
 * Любой сегмент длиннее этого значения не имеет смысла для генерации кодов QR.
 * Более того, максимально допустимая длина бит равна 32767, поскольку
 * самый большой код QR (версия 40) имеет 31329 модулей.
 */
struct qrcodegen_Segment {
    // Индикатор режима этого сегмента.
    enum qrcodegen_Mode mode;

    // Длина незакодированных данных этого сегмента. Измеряется в символах для
    // числовой/буквенно-цифровой/кандзи режим, байты для байтового режима и 0 для режима ECI.
    // Всегда ноль или положительный. Не то же самое, что битовая длина данных.
    int numChars;

    // Биты данных этого сегмента упакованы с побитовым обратным порядком байтов.
    // Может быть нулевым, если длина бита равна нулю.
    uint8_t * data;

    // Количество действительных битов данных, используемых в буфере. Требуется
    // 0 <= bitLength <= 32767 и bitLength <= (емкость массива данных) * 8.
    // Количество символов (numChars) должно соответствовать режиму и длине битового буфера.
    int bitLength;
};



/*---- Макрос-константы и функции ----*/

#define qrcodegen_VERSION_MIN   1  // Минимальный номер версии, поддерживаемый стандартом QR Code Model 2.
#define qrcodegen_VERSION_MAX  40  // Максимальный номер версии, поддерживаемый стандартом QR Code Model 2.

// Вычисляет количество байтов, необходимых для хранения любого кода QR до заданного номера версии включительно.
// как константа времени компиляции. Например, 'uint8_t буфер[ qrcodegen_BUFFER_LEN_FOR_VERSION (25)];'
// может хранить любой одиночный код QR от версии 1 до 25 (включительно). Результат соответствует int (или int16).
// Требуется qrcodegen_VERSION_MIN <= n <= qrcodegen_VERSION_MAX .
#define qrcodegen_BUFFER_LEN_FOR_VERSION(n)  ((((n) * 4 + 17) * ((n) * 4 + 17) + 7) / 8 + 1)

// Наихудшее количество байтов, необходимое для хранения одного кода QR, до
// версия 40. Это значение равно 3918, что чуть меньше 4 килобайт.
// Используйте это более удобное значение, чтобы избежать расчета более жестких границ памяти для буферов.
#define qrcodegen_BUFFER_LEN_MAX  qrcodegen_BUFFER_LEN_FOR_VERSION(qrcodegen_VERSION_MAX)



/*---- Функции (высокого уровня) для генерации кодов QR ----*/

/*
 * Кодирует данную текстовую строку в код QR, возвращая true, если кодирование прошло успешно.
 * Если данные слишком длинные, чтобы поместиться в любую версию в заданном диапазоне.
 * на заданном уровне ECC возвращается false.
 * - The input text must be encoded in UTF-8 and contain no NULs.
 * - The variables ecl and mask must correspond to enum constant values.
 * - Requires 1 <= minVersion <= maxVersion <= 40.
 * - The arrays tempBuffer and qrcode must each have a length
 *   не ниже qrcodegen_BUFFER_LEN_FOR_VERSION (maxVersion).
 * - After the function returns, tempBuffer contains no useful data.
 * - If successful, the resulting QR Code may use numeric,
 *   буквенно-цифровой или байтовый режим кодирования текста.
 * - In the most optimistic case, a QR Code at version 40 with low ECC
 *   может содержать любую строку UTF -8 длиной до 2953 байт или любую буквенно-цифровую строку.
 *   до 4296 символов или любая строка цифр до 7089 символов.
 *   Эти цифры представляют собой жесткий верхний предел стандарта кода QR.
 * - Please consult the QR Code specification for information on
 *   емкость данных для каждой версии, уровня ECC и режима кодирования текста.
 */
bool qrcodegen_encodeText(const char * text, uint8_t tempBuffer[], uint8_t qrcode[],
                          enum qrcodegen_Ecc ecl, int minVersion, int maxVersion, enum qrcodegen_Mask mask, bool boostEcl);


/*
 * Кодирует заданные двоичные данные в код QR, возвращая true, если кодирование прошло успешно.
 * Если данные слишком длинные, чтобы поместиться в любую версию в заданном диапазоне.
 * на заданном уровне ECC возвращается false.
 * - The input array range dataAndTemp[0 : dataLen] should normally be
 *   действительный текст UTF -8, но не требуется стандартом кода QR.
 * - The variables ecl and mask must correspond to enum constant values.
 * - Requires 1 <= minVersion <= maxVersion <= 40.
 * - The arrays dataAndTemp and qrcode must each have a length
 *   не ниже qrcodegen_BUFFER_LEN_FOR_VERSION (maxVersion).
 * - After the function returns, the contents of dataAndTemp may have changed,
 *   и больше не представляет полезных данных.
 * - If successful, the resulting QR Code will use byte mode to encode the data.
 * - In the most optimistic case, a QR Code at version 40 with low ECC can hold any byte
 *   последовательность длиной до 2953. Это жесткий верхний предел стандарта кода QR.
 * - Please consult the QR Code specification for information on
 *   емкость данных для каждой версии, уровня ECC и режима кодирования текста.
 */
bool qrcodegen_encodeBinary(uint8_t dataAndTemp[], size_t dataLen, uint8_t qrcode[],
                            enum qrcodegen_Ecc ecl, int minVersion, int maxVersion, enum qrcodegen_Mask mask, bool boostEcl);


/*---- Функции (низкого уровня) для генерации кодов QR ----*/

/*
 * Отображает код QR, представляющий заданные сегменты на заданном уровне исправления ошибок.
 * Для вывода автоматически выбирается наименьшая возможная версия кода QR. Возвращает истину, если
 * QR Создание кода выполнено успешно или неверно, если данные слишком длинные и не помещаются ни в одну версию. Уровень ECC
 * Результат может быть больше, чем аргумент ecl, если это можно сделать без увеличения версии.
 * Эта функция позволяет пользователю создавать собственную последовательность сегментов, которая переключает
 * между режимами (например, буквенно-цифровым и байтовым) для кодирования текста в меньшем пространстве.
 * Это низкоуровневый API ; API высокого уровня — это qrcodegen_encodeText () и qrcodegen_encodeBinary ().
 * Для экономии памяти буферы данных сегментов могут использовать псевдоним tempBuffer или перекрывать его.
 * в результате они будут затерты, но вывод кода QR по-прежнему будет правильным.
 * Но массив qrcode не должен перекрывать tempBuffer или буфер данных любого сегмента.
 */
bool qrcodegen_encodeSegments(const struct qrcodegen_Segment segs[], size_t len,
                              enum qrcodegen_Ecc ecl, uint8_t tempBuffer[], uint8_t qrcode[]);


/*
 * Отображает код QR, представляющий заданные сегменты с заданными параметрами кодирования.
 * Возвращает true, если создание кода QR прошло успешно, или false, если данные слишком длинные и не помещаются в диапазон версий.
 * Наименьшая возможная версия кода QR в пределах заданного диапазона автоматически
 * выбран для вывода. Если boostEcl имеет значение true, то уровень результата ECC
 * может быть выше аргумента ecl, если это можно сделать без увеличения
 * версия. Номер маски может быть от 0 до 7 (включительно).
 * маска или -1, чтобы автоматически выбрать подходящую маску (что может быть медленным).
 * Эта функция позволяет пользователю создавать собственную последовательность сегментов, которая переключает
 * между режимами (например, буквенно-цифровым и байтовым) для кодирования текста в меньшем пространстве.
 * Это низкоуровневый API ; API высокого уровня — это qrcodegen_encodeText () и qrcodegen_encodeBinary ().
 * Для экономии памяти буферы данных сегментов могут использовать псевдоним tempBuffer или перекрывать его.
 * в результате они будут затерты, но вывод кода QR по-прежнему будет правильным.
 * Но массив qrcode не должен перекрывать tempBuffer или буфер данных любого сегмента.
 */
bool qrcodegen_encodeSegmentsAdvanced(const struct qrcodegen_Segment segs[], size_t len, enum qrcodegen_Ecc ecl,
                                      int minVersion, int maxVersion, int mask, bool boostEcl, uint8_t tempBuffer[], uint8_t qrcode[]);


/*
 * Проверяет, может ли данная строка быть закодирована как сегмент в буквенно-цифровом режиме.
 * Строка является кодируемой тогда и только тогда, когда каждый символ находится в следующем наборе: от 0 до 9, от A до Z.
 * (только заглавные буквы), пробел, доллар, процент, звездочка, плюс, дефис, точка, косая черта, двоеточие.
 */
bool qrcodegen_isAlphanumeric(const char * text);


/*
 * Проверяет, может ли данная строка быть закодирована как сегмент в числовом режиме.
 * Строка является кодируемой тогда и только тогда, когда каждый символ находится в диапазоне от 0 до 9.
 */
bool qrcodegen_isNumeric(const char * text);


/*
 * Возвращает количество байтов ( uint8_t ), необходимое для буфера данных сегмента.
 * содержащий заданное количество символов с использованием данного режима. Примечания:
 * - Returns SIZE_MAX on failure, i.e. numChars > INT16_MAX or
 *   количество необходимых бит превышает INT16_MAX (т.е. 32767).
 * - Otherwise, all valid results are in the range [0, ceil(INT16_MAX / 8)], i.e. at most 4096.
 * - It is okay for the user to allocate more bytes for the buffer than needed.
 * - For byte mode, numChars measures the number of bytes, not Unicode code points.
 * - For ECI mode, numChars must be 0, and the worst-case number of bytes is returned.
 *   Фактический сегмент ECI может содержать более короткие данные. Для режимов, отличных от ECI, результат является точным.
 */
size_t qrcodegen_calcSegmentBufferSize(enum qrcodegen_Mode mode, size_t numChars);


/*
 * Возвращает сегмент, представляющий заданные двоичные данные, закодированные в
 * байтовый режим. Все входные массивы байтов приемлемы. Любая текстовая строка
 * может быть преобразован в UTF -8 байт и закодирован как сегмент байтового режима.
 */
struct qrcodegen_Segment qrcodegen_makeBytes(const uint8_t data[], size_t len, uint8_t buf[]);


/*
 * Возвращает сегмент, представляющий заданную строку десятичных цифр, закодированную в числовом режиме.
 */
struct qrcodegen_Segment qrcodegen_makeNumeric(const char * digits, uint8_t buf[]);


/*
 * Возвращает сегмент, представляющий данную текстовую строку, закодированную в буквенно-цифровом режиме.
 * Допустимые символы: от 0 до 9, от A до Z (только заглавные), пробел,
 * доллар, процент, звездочка, плюс, дефис, точка, косая черта, двоеточие.
 */
struct qrcodegen_Segment qrcodegen_makeAlphanumeric(const char * text, uint8_t buf[]);


/*
 * Возвращает сегмент, представляющий расширенную интерпретацию канала.
 * ( ECI ) указатель с заданным значением назначения.
 */
struct qrcodegen_Segment qrcodegen_makeEci(long assignVal, uint8_t buf[]);


/*---- Функции для извлечения необработанных данных из кодов QR ----*/

/*
 * Возвращает длину стороны данного кода QR, предполагая, что кодирование прошло успешно.
 * Результат находится в диапазоне [21, 177]. Обратите внимание, что длина буфера массива
 * связано с длиной стороны - каждый ' uint8_t qrcode[]' должен иметь длину не менее
 * qrcodegen_BUFFER_LEN_FOR_VERSION (версия), что равно ceil(size^2 / 8 + 1).
 */
int qrcodegen_getSize(const uint8_t qrcode[]);


/*
 * Возвращает цвет модуля (пикселя) по заданным координатам, что неверно.
 * для белого или true для черного. В верхнем левом углу указаны координаты (x=0, y=0).
 * Если данные координаты выходят за пределы, возвращается false (белый).
 */
bool qrcodegen_getModule(const uint8_t qrcode[], int x, int y);

/*
 * Возвращает размер qrcode указанной версии. Возвращает -1 в случае неудачи
 */
int qrcodegen_version2size(int version);
/*
 * Возвращает минимальную версию данных, которые можно сохранить. Возвращает -1 в случае неудачи
 */
int qrcodegen_getMinFitVersion(enum qrcodegen_Ecc ecl, size_t dataLen);

#ifdef __cplusplus
}
#endif

#endif
