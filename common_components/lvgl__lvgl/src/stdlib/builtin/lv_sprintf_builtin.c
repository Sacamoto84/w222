///////////////////////////////////////////////////////////////////////////////
// \автор(с) Марко Паланд (info@paland.com)
//             2014-2019, PALANDesign Ганновер, Германия
//
// \license ЛицензияMIT(MIT)
//
// Разрешение настоящим предоставляется бесплатно любому лицу, получившему копию.
// данного программного обеспечения и связанных с ним файлов документации («Программное обеспечение») для решения
// в Программном обеспечении без ограничений, включая, помимо прочего, права
// использовать, копировать, изменять, объединять, публиковать, распространять, сублицензировать и/или продавать
// копий Программного обеспечения и разрешать лицам, которым Программное обеспечение
// предоставлено для этого при соблюдении следующих условий:
//
// Вышеупомянутое уведомление об авторских правах и настоящее уведомление о разрешении должны быть включены в
// все копии или существенные части Программного обеспечения.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// \brief Реализация Tiny printf, sprintf и (v)snprintf, консервативная для скорости
//        встроенные системы с очень ограниченными ресурсами. Эти процедуры являются потоками
//        безопасный и реентерабельный!
//        Используйте это вместо раздутого стандарта/newlib printf, потому что они используют
//        malloc для printf (и может быть небезопасным для потоков).
//
///////////////////////////////////////////////////////////////////////////////

/*Оригинальный репозиторий: https://github.com/mpaland/printf*/

#include "../../lv_conf_internal.h"
#if LV_USE_STDLIB_SPRINTF == LV_STDLIB_BUILTIN

#include "../lv_sprintf.h"
#include "../lv_string.h"
#include "../../misc/lv_types.h"

#define PRINTF_DISABLE_SUPPORT_FLOAT    (!LV_USE_FLOAT)

// Размер буфера преобразования 'ntoa', он должен быть достаточно большим, чтобы вместить один преобразованный
// числовое число, включая дополненные нули (динамически создается в стеке)
// default: 32 байта
#ifndef PRINTF_NTOA_BUFFER_SIZE
    #define PRINTF_NTOA_BUFFER_SIZE    32U
#endif

// Размер буфера преобразования 'ftoa', он должен быть достаточно большим, чтобы вместить один преобразованный
// число с плавающей запятой, включая дополненные нули (динамически создается в стеке)
// default: 32 байта
#ifndef PRINTF_FTOA_BUFFER_SIZE
    #define PRINTF_FTOA_BUFFER_SIZE    32U
#endif

// поддержка типа с плавающей запятой (%f)
// default: активирован
#if !PRINTF_DISABLE_SUPPORT_FLOAT
    #define PRINTF_SUPPORT_FLOAT
#endif

// поддержка экспоненциальной записи с плавающей запятой (%e/%g)
// default: активирован
#ifndef PRINTF_DISABLE_SUPPORT_EXPONENTIAL
    #define PRINTF_SUPPORT_EXPONENTIAL
#endif

// определить точность с плавающей запятой по умолчанию
// default: 6 цифр
#ifndef PRINTF_DEFAULT_FLOAT_PRECISION
    #define PRINTF_DEFAULT_FLOAT_PRECISION 6U
#endif

// определить наибольшее число с плавающей запятой, подходящее для печати, с помощью %f
// по умолчанию: 1e9
#ifndef PRINTF_MAX_FLOAT
    #define PRINTF_MAX_FLOAT 1e9
#endif

// поддержка типа long long (%llu или %p)
// default: активирован
#ifndef PRINTF_DISABLE_SUPPORT_LONG_LONG
    #define PRINTF_SUPPORT_LONG_LONG
#endif

// поддержка типаptrdiff_t(%t)
// ptrdiff_t обычно определяется <stddef.h> как тип long или long long.
// default: активирован
#ifndef PRINTF_DISABLE_SUPPORT_PTRDIFF_T
    #define PRINTF_SUPPORT_PTRDIFF_T
#endif

///////////////////////////////////////////////////////////////////////////////

// определения внутренних флагов
#define FLAGS_ZEROPAD   (1U <<  0U)
#define FLAGS_LEFT      (1U <<  1U)
#define FLAGS_PLUS      (1U <<  2U)
#define FLAGS_SPACE     (1U <<  3U)
#define FLAGS_HASH      (1U <<  4U)
#define FLAGS_UPPERCASE (1U <<  5U)
#define FLAGS_CHAR      (1U <<  6U)
#define FLAGS_SHORT     (1U <<  7U)
#define FLAGS_LONG      (1U <<  8U)
#define FLAGS_LONG_LONG (1U <<  9U)
#define FLAGS_PRECISION (1U << 10U)
#define FLAGS_ADAPT_EXP (1U << 11U)

typedef struct {
    const char * fmt;
    va_list * va;
} lv_vaformat_t;

// импортироватьfloat.hдля DBL_MAX
#if defined(PRINTF_SUPPORT_FLOAT)
    #include <float.h>
#endif

// тип выходной функции
typedef void (*out_fct_type)(char character, void * buffer, size_t idx, size_t maxlen);

// оболочка (используется как буфер) для типа выходной функции
typedef struct {
    void (*fct)(char character, void * arg);
    void * arg;
} out_fct_wrap_type;

// выход внутреннего буфера
static inline void _out_buffer(char character, void * buffer, size_t idx, size_t maxlen)
{
    if(idx < maxlen) {
        ((char *)buffer)[idx] = character;
    }
}

// внутренний нулевой вывод
static inline void _out_null(char character, void * buffer, size_t idx, size_t maxlen)
{
    LV_UNUSED(character);
    LV_UNUSED(buffer);
    LV_UNUSED(idx);
    LV_UNUSED(maxlen);
}

// внутренняя проверка, представляет собой цифру (0-9)
// \return true, если char — цифра
static inline bool _is_digit(char ch)
{
    return (ch >= '0') && (ch <= '9');
}

// преобразование внутренней строки ASCII в беззнаковое целое число
static unsigned int _atoi(const char ** str)
{
    unsigned int i = 0U;
    while(_is_digit(**str)) {
        i = i * 10U + (unsigned int)(*((*str)++) - '0');
    }
    return i;
}

// выведите указанную строку в обратном порядке, учитывая любое дополнение нулями
static size_t _out_rev(out_fct_type out, char * buffer, size_t idx, size_t maxlen, const char * buf, size_t len,
                       unsigned int width, unsigned int flags)
{
    const size_t start_idx = idx;

    // пробелы до заданной ширины
    if(!(flags & FLAGS_LEFT) && !(flags & FLAGS_ZEROPAD)) {
        size_t i;
        for(i = len; i < width; i++) {
            out(' ', buffer, idx++, maxlen);
        }
    }

    // обратная строка
    while(len) {
        out(buf[--len], buffer, idx++, maxlen);
    }

    // добавить пробелы до заданной ширины
    if(flags & FLAGS_LEFT) {
        while(idx - start_idx < width) {
            out(' ', buffer, idx++, maxlen);
        }
    }

    return idx;
}

// внутренний формат ITOA
static size_t _ntoa_format(out_fct_type out, char * buffer, size_t idx, size_t maxlen, char * buf, size_t len,
                           bool negative, unsigned int base, unsigned int prec, unsigned int width, unsigned int flags)
{
    // дополнить ведущие нули
    if(!(flags & FLAGS_LEFT)) {
        if(width && (flags & FLAGS_ZEROPAD) && (negative || (flags & (FLAGS_PLUS | FLAGS_SPACE)))) {
            width--;
        }
        while((len < prec) && (len < PRINTF_NTOA_BUFFER_SIZE)) {
            buf[len++] = '0';
        }
        while((flags & FLAGS_ZEROPAD) && (len < width) && (len < PRINTF_NTOA_BUFFER_SIZE)) {
            buf[len++] = '0';
        }
    }

    // обрабатывать хэш
    if(flags & FLAGS_HASH) {
        if(!(flags & FLAGS_PRECISION) && len && ((len == prec) || (len == width))) {
            len--;
            if(len && (base == 16U)) {
                len--;
            }
        }
        if((base == 16U) && !(flags & FLAGS_UPPERCASE) && (len < PRINTF_NTOA_BUFFER_SIZE)) {
            buf[len++] = 'x';
        }
        else if((base == 16U) && (flags & FLAGS_UPPERCASE) && (len < PRINTF_NTOA_BUFFER_SIZE)) {
            buf[len++] = 'X';
        }
        else if((base == 2U) && (len < PRINTF_NTOA_BUFFER_SIZE)) {
            buf[len++] = 'b';
        }
        if(len < PRINTF_NTOA_BUFFER_SIZE) {
            buf[len++] = '0';
        }
    }

    if(len < PRINTF_NTOA_BUFFER_SIZE) {
        if(negative) {
            buf[len++] = '-';
        }
        else if(flags & FLAGS_PLUS) {
            buf[len++] = '+';  // игнорировать пробел, если существует «+»
        }
        else if(flags & FLAGS_SPACE) {
            buf[len++] = ' ';
        }
    }

    return _out_rev(out, buffer, idx, maxlen, buf, len, width, flags);
}

// внутренний itoa для «длинного» типа
static size_t _ntoa_long(out_fct_type out, char * buffer, size_t idx, size_t maxlen, unsigned long value, bool negative,
                         unsigned long base, unsigned int prec, unsigned int width, unsigned int flags)
{
    char buf[PRINTF_NTOA_BUFFER_SIZE];
    size_t len = 0U;

    // нет хеша для 0 значений
    if(!value) {
        flags &= ~FLAGS_HASH;
    }

    // напишите, если точность!= 0 и значение!= 0
    if(!(flags & FLAGS_PRECISION) || value) {
        do {
            const char digit = (char)(value % base);
            buf[len++] = digit < 10 ? '0' + digit : (flags & FLAGS_UPPERCASE ? 'A' : 'a') + digit - 10;
            value /= base;
        } while(value && (len < PRINTF_NTOA_BUFFER_SIZE));
    }

    return _ntoa_format(out, buffer, idx, maxlen, buf, len, negative, (unsigned int)base, prec, width, flags);
}

// внутренний itoa типа «длинный длинный»
#if defined(PRINTF_SUPPORT_LONG_LONG)
static size_t _ntoa_long_long(out_fct_type out, char * buffer, size_t idx, size_t maxlen, unsigned long long value,
                              bool negative, unsigned long long base, unsigned int prec, unsigned int width, unsigned int flags)
{
    char buf[PRINTF_NTOA_BUFFER_SIZE];
    size_t len = 0U;

    // нет хеша для 0 значений
    if(!value) {
        flags &= ~FLAGS_HASH;
    }

    // напишите, если точность!= 0 и значение!= 0
    if(!(flags & FLAGS_PRECISION) || value) {
        do {
            const char digit = (char)(value % base);
            buf[len++] = digit < 10 ? '0' + digit : (flags & FLAGS_UPPERCASE ? 'A' : 'a') + digit - 10;
            value /= base;
        } while(value && (len < PRINTF_NTOA_BUFFER_SIZE));
    }

    return _ntoa_format(out, buffer, idx, maxlen, buf, len, negative, (unsigned int)base, prec, width, flags);
}
#endif  // PRINTF_SUPPORT_LONG_LONG

#if defined(PRINTF_SUPPORT_FLOAT)

#if defined(PRINTF_SUPPORT_EXPONENTIAL)
// объявление, чтобы _ftoa могло перейти на обозначенные премии exp для отзывов > PRINTF_MAX_FLOAT
static size_t _etoa(out_fct_type out, char * buffer, size_t idx, size_t maxlen, double value, unsigned int prec,
                    unsigned int width, unsigned int flags);
#endif

// внутренняя ftoa для фиксированной десятичной с плавающей запятой
static size_t _ftoa(out_fct_type out, char * buffer, size_t idx, size_t maxlen, double value, unsigned int prec,
                    unsigned int width, unsigned int flags)
{
    char buf[PRINTF_FTOA_BUFFER_SIZE];
    size_t len  = 0U;
    double diff = 0.0;

    // степени 10
    static const double pow10[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };

    // проверка на специальные значения
    if(value != value)
        return _out_rev(out, buffer, idx, maxlen, "nan", 3, width, flags);
    if(value < -DBL_MAX)
        return _out_rev(out, buffer, idx, maxlen, "fni-", 4, width, flags);
    if(value > DBL_MAX)
        return _out_rev(out, buffer, idx, maxlen, (flags & FLAGS_PLUS) ? "fni+" : "fni", (flags & FLAGS_PLUS) ? 4U : 3U, width,
                        flags);

    // тест на очень большие значения
    // стандартное поведение printf заключается в целой цифреEVERY, которая может состоять из сотен символов, переполняя ваши буферы == плохо
    if((value > PRINTF_MAX_FLOAT) || (value < -PRINTF_MAX_FLOAT)) {
#if defined(PRINTF_SUPPORT_EXPONENTIAL)
        return _etoa(out, buffer, idx, maxlen, value, prec, width, flags);
#else
        return 0U;
#endif
    }

    // тест на отрицательный результат
    bool negative = false;
    if(value < 0) {
        negative = true;
        value = 0 - value;
    }

    // установить точность по умолчанию, если она не задана явно
    if(!(flags & FLAGS_PRECISION)) {
        prec = PRINTF_DEFAULT_FLOAT_PRECISION;
    }
    // ограничьте точность до 9, потому что prec >= 10 может привести к ошибкам затруднения
    while((len < PRINTF_FTOA_BUFFER_SIZE) && (prec > 9U)) {
        buf[len++] = '0';
        prec--;
    }

    int whole = (int)value;
    double tmp = (value - whole) * pow10[prec];
    unsigned long frac = (unsigned long)tmp;
    diff = tmp - frac;

    if(diff > 0.5) {
        ++frac;
        // ручка прокидывания, например случай 0,99 с точностью 1 равно 1,0
        if(frac >= pow10[prec]) {
            frac = 0;
            ++whole;
        }
    }
    else if(diff < 0.5) {
    }
    else if((frac == 0U) || (frac & 1U)) {
        // если на полпути, округлить в большую сторону, если нечетно OR, если последняя цифра 0
        ++frac;
    }

    if(prec == 0U) {
        diff = value - (double)whole;
        if((!(diff < 0.5) || (diff > 0.5)) && (whole & 1)) {
            // ровно 0,5 и ODD , затем округляем в большую сторону
            // 1,5 -> 2, но 2,5 -> 2
            ++whole;
        }
    }
    else {
        unsigned int count = prec;
        // теперь делаем дробную часть, как беззнаковое число
        while(len < PRINTF_FTOA_BUFFER_SIZE) {
            --count;
            buf[len++] = (char)(48U + (frac % 10U));
            if(!(frac /= 10U)) {
                break;
            }
        }
        // добавить дополнительные 0
        while((len < PRINTF_FTOA_BUFFER_SIZE) && (count-- > 0U)) {
            buf[len++] = '0';
        }
        if(len < PRINTF_FTOA_BUFFER_SIZE) {
            // добавить десятичную дробь
            buf[len++] = '.';
        }
    }

    // сделать целую часть, номер обратный
    while(len < PRINTF_FTOA_BUFFER_SIZE) {
        buf[len++] = (char)(48 + (whole % 10));
        if(!(whole /= 10)) {
            break;
        }
    }

    // дополнить ведущие нули
    if(!(flags & FLAGS_LEFT) && (flags & FLAGS_ZEROPAD)) {
        if(width && (negative || (flags & (FLAGS_PLUS | FLAGS_SPACE)))) {
            width--;
        }
        while((len < width) && (len < PRINTF_FTOA_BUFFER_SIZE)) {
            buf[len++] = '0';
        }
    }

    if(len < PRINTF_FTOA_BUFFER_SIZE) {
        if(negative) {
            buf[len++] = '-';
        }
        else if(flags & FLAGS_PLUS) {
            buf[len++] = '+';  // игнорировать пробел, если существует «+»
        }
        else if(flags & FLAGS_SPACE) {
            buf[len++] = ' ';
        }
    }

    return _out_rev(out, buffer, idx, maxlen, buf, len, width, flags);
}

#if defined(PRINTF_SUPPORT_EXPONENTIAL)
// внутренний вариант ftoa для экспоненциального типа с плавающей запятой, предоставлен Мартейном Джасперсом <m.jasperse@gmail.com>
static size_t _etoa(out_fct_type out, char * buffer, size_t idx, size_t maxlen, double value, unsigned int prec,
                    unsigned int width, unsigned int flags)
{
    // проверьте NaN и специальные значения
    if((value != value) || (value > DBL_MAX) || (value < -DBL_MAX)) {
        return _ftoa(out, buffer, idx, maxlen, value, prec, width, flags);
    }

    // определить знак
    const bool negative = value < 0;
    if(negative) {
        value = -value;
    }

    // точность по умолчанию
    if(!(flags & FLAGS_PRECISION)) {
        prec = PRINTF_DEFAULT_FLOAT_PRECISION;
    }

    // определить десятичный показатель
    // на основе алгоритма Дэвида Гэя ( https://www.ampl.com/netlib/fp/dtoa.c)
    union {
        uint64_t U;
        double   F;
    } conv;

    conv.F = value;
    int exp2 = (int)((conv.U >> 52U) & 0x07FFU) - 1023;           // эффективно log2
    conv.U = (conv.U & ((1ULL << 52U) - 1U)) | (1023ULL << 52U);  // отбросьте показатель степени, чтобы conv.F теперь находился в [1,2)
    // теперь аппроксимируйте log10 из целочисленной части log2 и разложите ln около 1,5.
    int expval = (int)(0.1760912590558 + exp2 * 0.301029995663981 + (conv.F - 1.5) * 0.289529654602168);
    // теперь мы хотим вычислить 10^expval, но хотим быть уверены, что оно не переполнится
    exp2 = (int)(expval * 3.321928094887362 + 0.5);
    const double z  = expval * 2.302585092994046 - exp2 * 0.6931471805599453;
    const double z2 = z * z;
    conv.U = (uint64_t)(exp2 + 1023) << 52U;
    // вычислить exp(z), используя цепные дроби, см.  https://en.wikipedia.org/wiki/Exponential_function#Continued_fractions_for_ex
    conv.F *= 1 + 2 * z / (2 - z + (z2 / (6 + (z2 / (10 + z2 / 14)))));
    // исправить ошибки округления
    if(value < conv.F) {
        expval--;
        conv.F /= 10;
    }

    // формат экспоненты — «%+03d», а наибольшее значение — «307», поэтому отложите 4–5 символов.
    unsigned int minwidth = ((expval < 100) && (expval > -100)) ? 4U : 5U;

    // в режиме «%g» «prec» — это количество *значающих цифр*, а не десятичных дробей.
    if(flags & FLAGS_ADAPT_EXP) {
        // do we want to fall-back to "%f" mode?
        if((value >= 1e-4) && (value < 1e6)) {
            if((int)prec > expval) {
                prec = (unsigned)((int)prec - expval - 1);
            }
            else {
                prec = 0;
            }
            flags |= FLAGS_PRECISION;   // убедитесь, что _ftoa соблюдает точность
            // нет символов в экспоненте
            minwidth = 0U;
            expval   = 0;
        }
        else {
            // мы используем одну сигфигу для всей части
            if((prec > 0) && (flags & FLAGS_PRECISION)) {
                --prec;
            }
        }
    }

    // will everything fit?
    unsigned int fwidth = width;
    if(width > minwidth) {
        // мы не откатились, поэтому вычли символы, необходимые для экспоненты
        fwidth -= minwidth;
    }
    else {
        // недостаточно символов, поэтому вернитесь к размеру по умолчанию
        fwidth = 0U;
    }
    if((flags & FLAGS_LEFT) && minwidth) {
        // если мы заполняем правую часть, DON 'T заполняем плавающую часть
        fwidth = 0U;
    }

    // изменить масштаб значения с плавающей запятой
    if(expval) {
        value /= conv.F;
    }

    // вывести плавающую часть
    const size_t start_idx = idx;
    idx = _ftoa(out, buffer, idx, maxlen, negative ? -value : value, prec, fwidth, flags & ~FLAGS_ADAPT_EXP);

    // вывести показательную часть
    if(minwidth) {
        // вывести экспоненциальный символ
        out((flags & FLAGS_UPPERCASE) ? 'E' : 'e', buffer, idx++, maxlen);
        // вывести значение показателя степени
        idx = _ntoa_long(out, buffer, idx, maxlen, (expval < 0) ? -expval : expval, expval < 0, 10, 0, minwidth - 1,
                         FLAGS_ZEROPAD | FLAGS_PLUS);
        // может потребоваться пробелы справа
        if(flags & FLAGS_LEFT) {
            while(idx - start_idx < width) out(' ', buffer, idx++, maxlen);
        }
    }
    return idx;
}
#endif  // PRINTF_SUPPORT_EXPONENTIAL
#endif  // PRINTF_SUPPORT_FLOAT

// внутренний vsnprintf
static int lv_vsnprintf_inner(out_fct_type out, char * buffer, const size_t maxlen, const char * format, va_list va)
{
    unsigned int flags, width, precision, n;
    size_t idx = 0U;

    if(!buffer) {
        // использовать нулевую функцию вывода
        out = _out_null;
    }

    while(*format) {
        // format specifier?  %[flags][width][.precision][length]
        if(*format != '%') {
            // no
            out(*format, buffer, idx++, maxlen);
            format++;
            continue;
        }
        else {
            // да, оцени это
            format++;
        }

        // оценивать флаги
        flags = 0U;
        do {
            switch(*format) {
                case '0':
                    flags |= FLAGS_ZEROPAD;
                    format++;
                    n = 1U;
                    break;
                case '-':
                    flags |= FLAGS_LEFT;
                    format++;
                    n = 1U;
                    break;
                case '+':
                    flags |= FLAGS_PLUS;
                    format++;
                    n = 1U;
                    break;
                case ' ':
                    flags |= FLAGS_SPACE;
                    format++;
                    n = 1U;
                    break;
                case '#':
                    flags |= FLAGS_HASH;
                    format++;
                    n = 1U;
                    break;
                default :
                    n = 0U;
                    break;
            }
        } while(n);

        // оценить поле ширины
        width = 0U;
        if(_is_digit(*format)) {
            width = _atoi(&format);
        }
        else if(*format == '*') {
            const int w = va_arg(va, int);
            if(w < 0) {
                flags |= FLAGS_LEFT;    // обратное заполнение
                width = (unsigned int) - w;
            }
            else {
                width = (unsigned int)w;
            }
            format++;
        }

        // оценить прецизионное поле
        precision = 0U;
        if(*format == '.') {
            flags |= FLAGS_PRECISION;
            format++;
            if(_is_digit(*format)) {
                precision = _atoi(&format);
            }
            else if(*format == '*') {
                const int prec = (int)va_arg(va, int);
                precision = prec > 0 ? (unsigned int)prec : 0U;
                format++;
            }
        }

        // оценить поле длины
        switch(*format) {
            case 'l' :
                flags |= FLAGS_LONG;
                format++;
                if(*format == 'l') {
                    flags |= FLAGS_LONG_LONG;
                    format++;
                }
                break;
            case 'h' :
                flags |= FLAGS_SHORT;
                format++;
                if(*format == 'h') {
                    flags |= FLAGS_CHAR;
                    format++;
                }
                break;
#if defined(PRINTF_SUPPORT_PTRDIFF_T)
            case 't' :
                flags |= (sizeof(ptrdiff_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
                format++;
                break;
#endif
            case 'j' :
                flags |= (sizeof(intmax_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
                format++;
                break;
            case 'z' :
                flags |= (sizeof(size_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
                format++;
                break;
            default :
                break;
        }

        // оценить спецификатор
        switch(*format) {
            case 'd' :
            case 'i' :
            case 'u' :
            case 'x' :
            case 'X' :
            case 'p' :
            case 'P' :
            case 'o' :
            case 'b' : {
                    // установить базу
                    unsigned int base;
                    if(*format == 'x' || *format == 'X') {
                        base = 16U;
                    }
                    else if(*format == 'p' || *format == 'P') {
                        base = 16U;
                        flags |= FLAGS_HASH;   // всегда хэш для формата указателя
#if defined(PRINTF_SUPPORT_LONG_LONG)
                        if(sizeof(uintptr_t) == sizeof(long long))
                            flags |= FLAGS_LONG_LONG;
                        else
#endif
                            flags |= FLAGS_LONG;

                        if(*(format + 1) == 'V')
                            format++;
                    }
                    else if(*format == 'o') {
                        base =  8U;
                    }
                    else if(*format == 'b') {
                        base =  2U;
                    }
                    else {
                        base = 10U;
                        flags &= ~FLAGS_HASH;   // нет хеша для декабрьской формы
                    }
                    // верхний регистр
                    if(*format == 'X' || *format == 'P') {
                        flags |= FLAGS_UPPERCASE;
                    }

                    // нет флага плюса или пробела для u, x, X, o, b
                    if((*format != 'i') && (*format != 'd')) {
                        flags &= ~(FLAGS_PLUS | FLAGS_SPACE);
                    }

                    // игнорировать флаг '0', когда задана точность
                    if(flags & FLAGS_PRECISION) {
                        flags &= ~FLAGS_ZEROPAD;
                    }

                    // преобразовать целое число
                    if((*format == 'i') || (*format == 'd')) {
                        // подписано
                        if(flags & FLAGS_LONG_LONG) {
#if defined(PRINTF_SUPPORT_LONG_LONG)
                            const long long value = va_arg(va, long long);
                            idx = _ntoa_long_long(out, buffer, idx, maxlen, (unsigned long long)(value > 0 ? value : 0 - value), value < 0, base,
                                                  precision, width, flags);
#endif
                        }
                        else if(flags & FLAGS_LONG) {
                            const long value = va_arg(va, long);
                            idx = _ntoa_long(out, buffer, idx, maxlen, (unsigned long)(value > 0 ? value : 0 - value), value < 0, base, precision,
                                             width, flags);
                        }
                        else {
                            const int value = (flags & FLAGS_CHAR) ? (char)va_arg(va, int) : (flags & FLAGS_SHORT) ? (short int)va_arg(va,
                                                                                                                                       int) : va_arg(va, int);
                            idx = _ntoa_long(out, buffer, idx, maxlen, (unsigned int)(value > 0 ? value : 0 - value), value < 0, base, precision,
                                             width, flags);
                        }
                    }
                    else if(*format == 'V') {
                        lv_vaformat_t * vaf = va_arg(va, lv_vaformat_t *);
                        va_list copy;

                        va_copy(copy, *vaf->va);
                        idx += lv_vsnprintf_inner(out, buffer + idx, maxlen - idx, vaf->fmt, copy);
                        va_end(copy);
                    }
                    else {
                        // без подписи
                        if(flags & FLAGS_LONG_LONG) {
#if defined(PRINTF_SUPPORT_LONG_LONG)
                            idx = _ntoa_long_long(out, buffer, idx, maxlen, va_arg(va, unsigned long long), false, base, precision, width, flags);
#endif
                        }
                        else if(flags & FLAGS_LONG) {
                            idx = _ntoa_long(out, buffer, idx, maxlen, va_arg(va, unsigned long), false, base, precision, width, flags);
                        }
                        else {
                            const unsigned int value = (flags & FLAGS_CHAR) ? (unsigned char)va_arg(va,
                                                                                                    unsigned int) : (flags & FLAGS_SHORT) ? (unsigned short int)va_arg(va, unsigned int) : va_arg(va, unsigned int);
                            idx = _ntoa_long(out, buffer, idx, maxlen, value, false, base, precision, width, flags);
                        }
                    }
                    format++;
                    break;
                }
#if defined(PRINTF_SUPPORT_FLOAT)
            case 'f' :
            case 'F' :
                if(*format == 'F') flags |= FLAGS_UPPERCASE;
                idx = _ftoa(out, buffer, idx, maxlen, va_arg(va, double), precision, width, flags);
                format++;
                break;
#if defined(PRINTF_SUPPORT_EXPONENTIAL)
            case 'e':
            case 'E':
            case 'g':
            case 'G':
                if((*format == 'g') || (*format == 'G')) flags |= FLAGS_ADAPT_EXP;
                if((*format == 'E') || (*format == 'G')) flags |= FLAGS_UPPERCASE;
                idx = _etoa(out, buffer, idx, maxlen, va_arg(va, double), precision, width, flags);
                format++;
                break;
#endif  // PRINTF_SUPPORT_EXPONENTIAL
#endif  // PRINTF_SUPPORT_FLOAT
            case 'c' : {
                    unsigned int l = 1U;
                    // предварительное заполнение
                    if(!(flags & FLAGS_LEFT)) {
                        while(l++ < width) {
                            out(' ', buffer, idx++, maxlen);
                        }
                    }
                    // вывод символов
                    out((char)va_arg(va, int), buffer, idx++, maxlen);
                    // заполнение постов
                    if(flags & FLAGS_LEFT) {
                        while(l++ < width) {
                            out(' ', buffer, idx++, maxlen);
                        }
                    }
                    format++;
                    break;
                }

            case 's' : {
                    const char * p = va_arg(va, char *);
                    unsigned int l = lv_strnlen(p, precision ? precision : (size_t) -1);
                    // предварительное заполнение
                    if(flags & FLAGS_PRECISION) {
                        l = (l < precision ? l : precision);
                    }
                    if(!(flags & FLAGS_LEFT)) {
                        while(l++ < width) {
                            out(' ', buffer, idx++, maxlen);
                        }
                    }
                    // строковый вывод
                    while((*p != 0) && (!(flags & FLAGS_PRECISION) || precision--)) {
                        out(*(p++), buffer, idx++, maxlen);
                    }
                    // заполнение постов
                    if(flags & FLAGS_LEFT) {
                        while(l++ < width) {
                            out(' ', buffer, idx++, maxlen);
                        }
                    }
                    format++;
                    break;
                }

            case '%' :
                out('%', buffer, idx++, maxlen);
                format++;
                break;

            default :
                out(*format, buffer, idx++, maxlen);
                format++;
                break;
        }
    }

    // прекращение
    out((char)0, buffer, idx < maxlen ? idx : maxlen - 1U, maxlen);

    // вернуть записанные символы без завершения \0
    return (int)idx;
}

///////////////////////////////////////////////////////////////////////////////
/// GLOBAL FUNCTIONS FOR LVGL
///////////////////////////////////////////////////////////////////////////////

int lv_snprintf(char * buffer, size_t count, const char * format, ...)
{
    va_list va;
    va_start(va, format);
    const int ret = lv_vsnprintf_inner(_out_buffer, buffer, count, format, va);
    va_end(va);
    return ret;
}

int lv_vsnprintf(char * buffer, size_t count, const char * format, va_list va)
{
    return lv_vsnprintf_inner(_out_buffer, buffer, count, format, va);
}

#endif /*LV_STDLIB_BUILTIN*/
