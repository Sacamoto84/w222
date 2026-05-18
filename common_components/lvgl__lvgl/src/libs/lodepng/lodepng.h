/*
LodePNG версия 20230410

Copyright (c) 2005-2023 Lode Vandevenne

Данное программное обеспечение предоставляется «как есть», без каких-либо явных или подразумеваемых
гарантия. Ни при каких обстоятельствах авторы не несут ответственности за какой-либо ущерб.
возникшие в результате использования данного программного обеспечения.

Разрешение предоставляется каждому на использование этого программного обеспечения для любых целей,
включая коммерческие приложения, а также изменять его и распространять
свободно, с соблюдением следующих ограничений:

    1. The origin of this software must not be misrepresented; you must not
    утверждайте, что вы написали оригинальное программное обеспечение. Если вы используете это программное обеспечение
    в продукте подтверждение в документации по продукту будет
    приветствуется, но не является обязательным.

    2. Altered source versions must be plainly marked as such, and must not be
    ошибочно представлено как оригинальное программное обеспечение.

    3. This notice may not be removed or altered from any source
    распространение.
*/

#ifndef LODEPNG_H
#define LODEPNG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../lvgl.h"
#if LV_USE_LODEPNG
#include LV_STDDEF_INCLUDE /*для size_t*/
LV_ATTRIBUTE_EXTERN_DATA extern const char * LODEPNG_VERSION_STRING;

/*
Следующие #defines используются для создания разделов кода. Их можно отключить
отключить разделы кода, что может ускорить компиляцию и уменьшить размер двоичного файла.
Определения «NO_COMPILE» предназначены для передачи определений в
команда компилятора, чтобы отключить их без изменения этого заголовка, например.
- DLODEPNG_NO_COMPILE_ZLIB для gcc или clang.
*/
/*выкачать и zlib. Если отключено, вы должны указать альтернативные функции zlib в
поле custom_zlib настроек сжатия и распаковки*/
#ifndef LODEPNG_NO_COMPILE_ZLIB
    /*передайте - DLODEPNG_NO_COMPILE_ZLIB компилятору, чтобы отключить это, или закомментируйте LODEPNG_COMPILE_ZLIB ниже.*/
    #define LODEPNG_COMPILE_ZLIB
#endif

/*Кодер PNG и декодер PNG*/
#ifndef LODEPNG_NO_COMPILE_PNG
    /*передайте - DLODEPNG_NO_COMPILE_PNG компилятору, чтобы отключить это, или закомментируйте LODEPNG_COMPILE_PNG ниже.*/
    #define LODEPNG_COMPILE_PNG
#endif

/*декодер deflate&zlib и декодер png*/
#ifndef LODEPNG_NO_COMPILE_DECODER
    /*передайте - DLODEPNG_NO_COMPILE_DECODER компилятору, чтобы отключить это, или закомментируйте LODEPNG_COMPILE_DECODER ниже.*/
    #define LODEPNG_COMPILE_DECODER
#endif

/*Кодер deflate&zlib и кодер png*/
#ifndef LODEPNG_NO_COMPILE_ENCODER
    /*передайте - DLODEPNG_NO_COMPILE_ENCODER компилятору, чтобы отключить это, или закомментируйте LODEPNG_COMPILE_ENCODER ниже.*/
    #define LODEPNG_COMPILE_ENCODER
#endif

/*дополнительные встроенные функции загрузки и сохранения файлов на жестком диске*/
#ifndef LODEPNG_NO_COMPILE_DISK
    /*передайте - DLODEPNG_NO_COMPILE_DISK компилятору, чтобы отключить это, или закомментируйте LODEPNG_COMPILE_DISK ниже.*/
    #define LODEPNG_COMPILE_DISK
#endif

/*поддержка чанков, отличных от IHDR, IDAT, PLTE, tRNS, IEND: вспомогательные и неизвестные чанки*/
#ifndef LODEPNG_NO_COMPILE_ANCILLARY_CHUNKS
    /*передать - DLODEPNG_NO_COMPILE_ANCILLARY_CHUNKS компилятору, чтобы отключить это,
    или прокомментируйте LODEPNG_COMPILE_ANCILLARY_CHUNKS ниже*/
    #define LODEPNG_COMPILE_ANCILLARY_CHUNKS
#endif

/*возможность конвертировать числовые коды ошибок в текстовую строку на английском языке*/
#ifndef LODEPNG_NO_COMPILE_ERROR_TEXT
    /*передать - DLODEPNG_NO_COMPILE_ERROR_TEXT компилятору, чтобы отключить это,
    или прокомментируйте LODEPNG_COMPILE_ERROR_TEXT ниже*/
    #define LODEPNG_COMPILE_ERROR_TEXT
#endif

/*Скомпилируйте распределители по умолчанию (C free, malloc и realloc). Если вы отключите это,
вы можете определить функции lodepng_free , lodepng_malloc и lodepng_realloc в вашем
исходные файлы с пользовательскими распределителями.*/
#ifndef LODEPNG_NO_COMPILE_ALLOCATORS
    /*передать — DLODEPNG_NO_COMPILE_ALLOCATORS компилятору для отключения встроенных,
    или прокомментируйте LODEPNG_COMPILE_ALLOCATORS ниже*/
    #define LODEPNG_COMPILE_ALLOCATORS
#endif

/*Отключите встроенную функцию CRC, в этом случае это пользовательская реализация.
lodepng_crc32 должен быть определен снаружи, чтобы его можно было подключить.
Встроенный код CRC по умолчанию поставляется с таблицами поиска размером 8 КБ, поэтому для среды с ограниченной памятью он может понадобиться.
отключен и обеспечивает гораздо меньшую внешнюю реализацию, как сказано выше. Вы можете найти такой пример реализации
в комментарии к файлу lodepng.c (pp) в регистре else в доступном для поиска разделе LODEPNG_COMPILE_CRC.*/
#ifndef LODEPNG_NO_COMPILE_CRC
    /*передать — DLODEPNG_NO_COMPILE_CRC компилятору для отключения встроенного,
    или прокомментируйте LODEPNG_COMPILE_CRC ниже*/
    #define LODEPNG_COMPILE_CRC
#endif

/*скомпилировать версию C++ (здесь можно отключить оболочку C++ даже при компиляции для C++)*/
#ifdef __cplusplus
    #ifndef LODEPNG_NO_COMPILE_CPP
        /*передать - DLODEPNG_NO_COMPILE_CPP компилятору, чтобы отключить C++ (не требуется, если компилятор только C),
        или прокомментируйте LODEPNG_COMPILE_CPP ниже*/
        #define LODEPNG_COMPILE_CPP
    #endif
#endif

#ifdef LODEPNG_COMPILE_CPP
    #include <vector>
    #include <string>
#endif /*LODEPNG_COMPILE_CPP*/

#ifdef LODEPNG_COMPILE_PNG
/*Цветовые типы PNG (также используются для необработанных изображений).*/
typedef enum LodePNGColorType {
    LCT_GREY = 0, /*grayscale: 1,2,4,8,16 bit*/
    LCT_RGB = 2, /*RGB: 8,16 bit*/
    LCT_PALETTE = 3, /*palette: 1,2,4,8 bit*/
    LCT_GREY_ALPHA = 4, /*оттенки серого с альфа: 8,16 бит*/
    LCT_RGBA = 6, /*RGB с альфа: 8,16 бит*/
    /*LCT_MAX_OCTET_VALUE позволяет компилятору разрешить этому перечислению представлять любые недопустимые значения.
    значение байта от 0 до 255, которое может присутствовать в недопустимом заголовке файла PNG. Делай
    не используйте, не сравнивайте и не устанавливайте имя LCT_MAX_OCTET_VALUE, вместо этого используйте либо
    допустимые названия типов цвета, указанные выше, или числовые значения, например 1 или 7, при проверке
    определенные запрещенные байтовые значения цветового типа или привести к целому числу для его печати.*/
    LCT_MAX_OCTET_VALUE = 255
} LodePNGColorType;

#ifdef LODEPNG_COMPILE_DECODER
/*
Преобразует данные PNG в памяти в необработанные данные пикселей.
out: Output parameter. Pointer to buffer that will contain the raw pixel data.
     После декодирования его размер составляет w*h*(байт на пиксель) байт больше, чем
     изначально. Количество байтов на пиксель зависит от типа цвета и разрядности.
     Должен быть освобожден после использования с помощью free(*out).
     Note: for 16-bit per channel colors, uses big endian format like PNG does.
w: Output parameter. Pointer to width of pixel data.
h: Output parameter. Pointer to height of pixel data.
in: Memory buffer with the PNG file.
insize: size of the in buffer.
colortype: the desired color type for the raw output image. See explanation on PNG color types.
bitdepth: the desired bit depth for the raw output image. See explanation on PNG color types.
Возвращаемое значение: код ошибки LodePNG (0 означает отсутствие ошибки).
*/
unsigned lodepng_decode_memory(unsigned char ** out, unsigned * w, unsigned * h,
                               const unsigned char * in, size_t insize,
                               LodePNGColorType colortype, unsigned bitdepth);

/*То же, что lodepng_decode_memory, но всегда декодирует в 32-битное необработанное изображение RGBA.*/
unsigned lodepng_decode32(unsigned char ** out, unsigned * w, unsigned * h,
                          const unsigned char * in, size_t insize);

/*То же, что lodepng_decode_memory, но всегда декодирует в 24-битное необработанное изображение RGB.*/
unsigned lodepng_decode24(unsigned char ** out, unsigned * w, unsigned * h,
                          const unsigned char * in, size_t insize);

#ifdef LODEPNG_COMPILE_DISK
/*
Загрузите PNG с диска из файла с заданным именем.
То же, что и другие функции декодирования, но вместо этого принимает на вход имя файла.

NOTE: Wide-character filenames are not supported, you can use an external method
для обработки таких файлов и декодирования в памяти.*/
unsigned lodepng_decode_file(unsigned char ** out, unsigned * w, unsigned * h,
                             const char * filename,
                             LodePNGColorType colortype, unsigned bitdepth);

/*То же, что и lodepng_decode_file, но всегда декодируется в 32-битное необработанное изображение RGBA.

NOTE: Wide-character filenames are not supported, you can use an external method
для обработки таких файлов и декодирования в памяти.*/
unsigned lodepng_decode32_file(unsigned char ** out, unsigned * w, unsigned * h,
                               const char * filename);

/*То же, что и lodepng_decode_file, но всегда декодируется в 24-битное необработанное изображение RGB.

NOTE: Wide-character filenames are not supported, you can use an external method
для обработки таких файлов и декодирования в памяти.*/
unsigned lodepng_decode24_file(unsigned char ** out, unsigned * w, unsigned * h,
                               const char * filename);
#endif /*LODEPNG_COMPILE_DISK*/
#endif /*LODEPNG_COMPILE_DECODER*/


#ifdef LODEPNG_COMPILE_ENCODER
/*
Преобразует необработанные данные пикселей в изображение PNG в памяти. Цветотип и битовая глубина
  выходного изображения PNG выбрать нельзя, они определяются автоматически
  по типу цвета, глубине цвета и содержимому входных пиксельных данных.
  Note: for 16-bit per channel colors, needs big endian format like PNG does.
out: Output parameter. Pointer to buffer that will contain the PNG image data.
     Должен быть освобожден после использования с помощью free(*out).
outsize: Output parameter. Pointer to the size in bytes of the out buffer.
image: The raw pixel data to encode. The size of this buffer should be
       w * h * (байты на пиксель), количество байтов на пиксель зависит от типа цвета и битовой глубины.
w: width of the raw pixel data in pixels.
h: height of the raw pixel data in pixels.
colortype: the color type of the raw input image. See explanation on PNG color types.
bitdepth: the bit depth of the raw input image. See explanation on PNG color types.
Возвращаемое значение: код ошибки LodePNG (0 означает отсутствие ошибки).
*/
unsigned lodepng_encode_memory(unsigned char ** out, size_t * outsize,
                               const unsigned char * image, unsigned w, unsigned h,
                               LodePNGColorType colortype, unsigned bitdepth);

/*То же, что lodepng_encode_memory , но всегда кодирует 32-битное необработанное изображение RGBA.*/
unsigned lodepng_encode32(unsigned char ** out, size_t * outsize,
                          const unsigned char * image, unsigned w, unsigned h);

/*То же, что lodepng_encode_memory , но всегда кодирует 24-битное необработанное изображение RGB.*/
unsigned lodepng_encode24(unsigned char ** out, size_t * outsize,
                          const unsigned char * image, unsigned w, unsigned h);

#ifdef LODEPNG_COMPILE_DISK
/*
Преобразует необработанные данные пикселей в файл PNG на диске.
То же, что и другие функции кодирования, но вместо этого в качестве вывода принимает имя файла.

NOTE: This overwrites existing files without warning!

NOTE: Wide-character filenames are not supported, you can use an external method
для обработки таких файлов и кодирования в памяти.*/
unsigned lodepng_encode_file(const char * filename,
                             const unsigned char * image, unsigned w, unsigned h,
                             LodePNGColorType colortype, unsigned bitdepth);

/*То же, что lodepng_encode_file , но всегда кодирует 32-битное необработанное изображение RGBA.

NOTE: Wide-character filenames are not supported, you can use an external method
для обработки таких файлов и кодирования в памяти.*/
unsigned lodepng_encode32_file(const char * filename,
                               const unsigned char * image, unsigned w, unsigned h);

/*То же, что lodepng_encode_file , но всегда кодирует 24-битное необработанное изображение RGB.

NOTE: Wide-character filenames are not supported, you can use an external method
для обработки таких файлов и кодирования в памяти.*/
unsigned lodepng_encode24_file(const char * filename,
                               const unsigned char * image, unsigned w, unsigned h);
#endif /*LODEPNG_COMPILE_DISK*/
#endif /*LODEPNG_COMPILE_ENCODER*/


#ifdef LODEPNG_COMPILE_CPP
namespace lodepng
{
#ifdef LODEPNG_COMPILE_DECODER
/*То же, что lodepng_decode_memory, но декодирует в std::vector. Цветотип
— формат вывода пикселей. По умолчанию — RGBA 8 бит на канал.*/
unsigned decode(std::vector<unsigned char> & out, unsigned & w, unsigned & h,
                const unsigned char * in, size_t insize,
                LodePNGColorType colortype = LCT_RGBA, unsigned bitdepth = 8);
unsigned decode(std::vector<unsigned char> & out, unsigned & w, unsigned & h,
                const std::vector<unsigned char> & in,
                LodePNGColorType colortype = LCT_RGBA, unsigned bitdepth = 8);
#ifdef LODEPNG_COMPILE_DISK
/*
Преобразует файл PNG с диска в необработанные пиксельные данные в памяти.
То же, что и другие функции декодирования, но вместо этого принимает на вход имя файла.

NOTE: Wide-character filenames are not supported, you can use an external method
для обработки таких файлов и декодирования в памяти.
*/
unsigned decode(std::vector<unsigned char> & out, unsigned & w, unsigned & h,
                const std::string & filename,
                LodePNGColorType colortype = LCT_RGBA, unsigned bitdepth = 8);
#endif /* LODEPNG_COMPILE_DISK */
#endif /* LODEPNG_COMPILE_DECODER */

#ifdef LODEPNG_COMPILE_ENCODER
/*То же, что lodepng_encode_memory, но кодирует в std::vector. цветотип
это исходные входные данные. Тип выходного цвета PNG будет выбран автоматически.*/
unsigned encode(std::vector<unsigned char> & out,
                const unsigned char * in, unsigned w, unsigned h,
                LodePNGColorType colortype = LCT_RGBA, unsigned bitdepth = 8);
unsigned encode(std::vector<unsigned char> & out,
                const std::vector<unsigned char> & in, unsigned w, unsigned h,
                LodePNGColorType colortype = LCT_RGBA, unsigned bitdepth = 8);
#ifdef LODEPNG_COMPILE_DISK
/*
Преобразует 32-битные необработанные пиксельные данные RGBA в файл PNG на диске.
То же, что и другие функции кодирования, но вместо этого в качестве вывода принимает имя файла.

NOTE: This overwrites existing files without warning!

NOTE: Wide-character filenames are not supported, you can use an external method
для обработки таких файлов и декодирования в памяти.
*/
unsigned encode(const std::string & filename,
                const unsigned char * in, unsigned w, unsigned h,
                LodePNGColorType colortype = LCT_RGBA, unsigned bitdepth = 8);
unsigned encode(const std::string & filename,
                const std::vector<unsigned char> & in, unsigned w, unsigned h,
                LodePNGColorType colortype = LCT_RGBA, unsigned bitdepth = 8);
#endif /* LODEPNG_COMPILE_DISK */
#endif /* LODEPNG_COMPILE_ENCODER */
} /* пространство имен lodepng */
#endif /*LODEPNG_COMPILE_CPP*/
#endif /*LODEPNG_COMPILE_PNG*/

#ifdef LODEPNG_COMPILE_ERROR_TEXT
    /*Возвращает английское описание числового кода ошибки.*/
    const char * lodepng_error_text(unsigned code);
#endif /*LODEPNG_COMPILE_ERROR_TEXT*/

#ifdef LODEPNG_COMPILE_DECODER
/*Настройки для распаковки zlib*/
typedef struct LodePNGDecompressSettings LodePNGDecompressSettings;
struct LodePNGDecompressSettings {
    /* Проверьте LodePNGDecoderSettings на наличие более игнорируемых ошибок, таких как ignore_crc. */
    unsigned ignore_adler32; /*если 1, продолжить и не выдавать сообщение об ошибке, если контрольная сумма Adler32 повреждена*/
    unsigned ignore_nlen; /*игнорировать дополнение контрольной суммы len в несжатых блоках*/

    /*Максимальный размер распаковки, за пределами которого декодер может (и рекомендуется) прекратить декодирование.
    верните ошибку, выведите размер данных > max_output_size и все данные до этого момента. Это
    не является ни жестким ограничением, ни гарантией, но может предотвратить чрезмерное использование памяти. Эта настройка
    игнорируется декодером PNG, но используется декодером deflate/zlib и может использоваться пользовательскими декодерами.
    Установите значение 0, чтобы не налагать ограничений (по умолчанию).*/
    size_t max_output_size;

    /*используйте собственный декодер zlib вместо встроенного (по умолчанию: ноль).
    Должен возвращать 0 в случае успеха и любое значение, отличное от 0, в случае ошибки (числовое значение не отображается).*/
    unsigned(*custom_zlib)(unsigned char **, size_t *,
                           const unsigned char *, size_t,
                           const LodePNGDecompressSettings *);
    /*использовать собственный декодер дефляции вместо встроенного (по умолчанию: ноль)
    если custom_zlib не равен нулю, custom_inflate игнорируется (в формате zlib используется deflate).
    Должен возвращать 0 в случае успеха и любое значение, отличное от 0, в случае ошибки (числовое значение не отображается).*/
    unsigned(*custom_inflate)(unsigned char **, size_t *,
                              const unsigned char *, size_t,
                              const LodePNGDecompressSettings *);

    const void * custom_context; /*дополнительные пользовательские настройки для пользовательских функций*/
};

LV_ATTRIBUTE_EXTERN_DATA extern const LodePNGDecompressSettings lodepng_default_decompress_settings;
void lodepng_decompress_settings_init(LodePNGDecompressSettings * settings);
#endif /*LODEPNG_COMPILE_DECODER*/

#ifdef LODEPNG_COMPILE_ENCODER
/*
Настройки сжатия zlib. Изменение этих настроек меняет баланс.
между скоростью и степенью сжатия.
*/
typedef struct LodePNGCompressSettings LodePNGCompressSettings;
struct LodePNGCompressSettings { /*deflate = compress*/
    /*LZ77 связанные настройки*/
    unsigned btype; /*тип блока для LZ (0, 1, 2 или 3, см. стандарт zlib). Должно быть 2 для правильного сжатия.*/
    unsigned use_lz77; /*использовать или нет LZ77 . Должно быть 1 для правильного сжатия.*/
    unsigned windowsize; /*должно быть степенью двойки <= 32768. Чем выше сжатие, тем медленнее. Значение по умолчанию: 2048.*/
    unsigned minmatch; /*минимальная длина lz77. Обычно лучше всего использовать значение 3, для некоторых PNG лучше использовать значение 6. По умолчанию: 0*/
    unsigned nicematch; /*прекратите поиск, если найдена >= эта длина. Установите значение 258 для наилучшего сжатия. По умолчанию: 128*/
    unsigned lazymatching; /*используйте ленивое сопоставление: лучшее сжатие, но немного медленнее. По умолчанию: правда*/

    /*использовать собственный кодировщик zlib вместо встроенного (по умолчанию: ноль)*/
    unsigned(*custom_zlib)(unsigned char **, size_t *,
                           const unsigned char *, size_t,
                           const LodePNGCompressSettings *);
    /*использовать собственный кодировщик выкачивания вместо встроенного (по умолчанию: ноль)
    если используется custom_zlib, custom_deflate игнорируется, поскольку только встроенный
    Функция zlib вызовет custom_deflate*/
    unsigned(*custom_deflate)(unsigned char **, size_t *,
                              const unsigned char *, size_t,
                              const LodePNGCompressSettings *);

    const void * custom_context; /*дополнительные пользовательские настройки для пользовательских функций*/
};

LV_ATTRIBUTE_EXTERN_DATA extern const LodePNGCompressSettings lodepng_default_compress_settings;
void lodepng_compress_settings_init(LodePNGCompressSettings * settings);
#endif /*LODEPNG_COMPILE_ENCODER*/

#ifdef LODEPNG_COMPILE_PNG
/*
Цветовой режим изображения. Содержит всю информацию, необходимую для декодирования пикселя.
биты в цвета RGBA. Эта информация такая же, как и в файле PNG.
формате и используется как для PNG, так и для необработанных данных изображения в LodePNG.
*/
typedef struct LodePNGColorMode {
    /*заголовок ( IHDR )*/
    LodePNGColorType colortype; /*тип цвета, см. стандарт PNG или документацию далее в этом заголовочном файле.*/
    unsigned bitdepth;  /*бит на выборку, см. стандарт PNG или документацию далее в этом заголовочном файле.*/

    /*
    палитра ( PLTE и tRNS)

    Динамически распределяется по цветам палитры, включая альфа.
    Это поле не может быть выделено напрямую, сначала используйте lodepng_color_mode_init,
    затем lodepng_palette_add для каждого цвета, чтобы правильно его инициализировать (чтобы обеспечить размер
    ровно 1024 байта).

    Альфа-каналы также должны быть установлены, установите их на 255 для непрозрачных изображений.

    При декодировании с настройками по умолчанию эту палитру можно игнорировать, так как
    LodePNG уже заполняет цвета палитры в пикселях необработанного вывода RGBA.
    но при декодировании в исходный цветовой режим PNG необходимо восстановить
    цвета.

    Палитра поддерживается только для типа цвета 3.
    */
    unsigned char * palette; /*палитра в RGBARGBA... заказ. Должно быть либо 0, либо при выделении должно иметь 1024 байта.*/
    size_t palettesize; /*размер палитры в количестве цветов (количество используемых байт равно 4 * размер палитры)*/

    /*
    прозрачный цветной ключ (tRNS)

    Этот цвет использует ту же разрядность, что и значение разрядности в этой структуре, которое может составлять от 1 до 16 бит.
    Для PNG в оттенках серого все три параметра r, g и b будут установлены на одно и то же значение.

    При декодировании по умолчанию можно игнорировать эту информацию, поскольку LodePNG устанавливает
    пиксели с этим ключом становятся прозрачными уже в необработанном выводе RGBA.

    Цветовой ключ поддерживается только для типов цвета 0 и 2.
    */
    unsigned key_defined; /*is a transparent color key given? 0 = false, 1 = true*/
    unsigned key_r;       /*красный/полутоновый компонент цветового ключа*/
    unsigned key_g;       /*зеленый компонент цветового ключа*/
    unsigned key_b;       /*синий компонент цветового ключа*/
} LodePNGColorMode;

/*функции инициализации, очистки и копирования для использования с этой структурой*/
void lodepng_color_mode_init(LodePNGColorMode * info);
void lodepng_color_mode_cleanup(LodePNGColorMode * info);
/*возвращаемое значение — это код ошибки (0 означает отсутствие ошибки)*/
unsigned lodepng_color_mode_copy(LodePNGColorMode * dest, const LodePNGColorMode * source);
/* Создает временный LodePNGColorMode, который не требует очистки (без палитры). */
LodePNGColorMode lodepng_color_mode_make(LodePNGColorType colortype, unsigned bitdepth);

void lodepng_palette_clear(LodePNGColorMode * info);
/*добавить 1 цвет в палитру*/
unsigned lodepng_palette_add(LodePNGColorMode * info,
                             unsigned char r, unsigned char g, unsigned char b, unsigned char a);

/*получить общее количество бит на пиксель на основе типа цвета и глубины цвета в структуре*/
unsigned lodepng_get_bpp(const LodePNGColorMode * info);
/*получить количество используемых цветовых каналов на основе типа цвета в структуре.
Если используется палитра, она считается за 1 канал.*/
unsigned lodepng_get_channels(const LodePNGColorMode * info);
/*is it a grayscale type? (only colortype 0 or 4)*/
unsigned lodepng_is_greyscale_type(const LodePNGColorMode * info);
/*has it got an alpha channel? (only colortype 2 or 6)*/
unsigned lodepng_is_alpha_type(const LodePNGColorMode * info);
/*has it got a palette? (only colortype 3)*/
unsigned lodepng_is_palette_type(const LodePNGColorMode * info);
/*возвращает true только в том случае, если существует палитра и в палитре есть значение с альфа < 255.
Просматривает палитру, чтобы проверить это.*/
unsigned lodepng_has_palette_alpha(const LodePNGColorMode * info);
/*
Проверьте, указывает ли данная информация о цвете на возможность наличия непрозрачных пикселей в изображении PNG.
Возвращает true, если изображение может иметь полупрозрачные или невидимые пиксели (оно все равно будет непрозрачным, если такие пиксели не используются).
Возвращает false, если изображение может иметь только непрозрачные пиксели.
Более подробно, он возвращает true, только если это цветотип с альфа-каналом или палитра с непрозрачными значениями.
или если «key_defined» истинно.
*/
unsigned lodepng_can_have_alpha(const LodePNGColorMode * info);
/*Возвращает размер в байтах буфера необработанного изображения с заданной шириной, высотой и цветовым режимом.*/
size_t lodepng_get_raw_size(unsigned w, unsigned h, const LodePNGColorMode * color);

#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
/*Информация о фрагменте времени в PNG.*/
typedef struct LodePNGTime {
    unsigned year;    /*Использовано 2 байта (0-65535)*/
    unsigned month;   /*1-12*/
    unsigned day;     /*1-31*/
    unsigned hour;    /*0-23*/
    unsigned minute;  /*0-59*/
    unsigned second;  /*0–60 (чтобы учесть високосные секунды)*/
} LodePNGTime;
#endif /*LODEPNG_COMPILE_ANCILLARY_CHUNKS*/

/*Информация об изображении PNG, кроме пикселей, ширины и высоты.*/
typedef struct LodePNGInfo {
    /*фрагменты заголовка ( IHDR ), палитры ( PLTE ) и прозрачности (tRNS )*/
    unsigned compression_method;/*метод сжатия исходного файла. Всегда 0.*/
    unsigned filter_method;     /*метод фильтра исходного файла*/
    unsigned interlace_method;  /*метод чересстрочной развертки исходного файла: 0=нет, 1=Adam7*/
    LodePNGColorMode color;     /*тип цвета и биты, палитра и прозрачность файла PNG*/

#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
    /*
    Рекомендуемый блок цвета фона (bKGD)

    Здесь используется тот же цветовой режим и битовая глубина, что и у PNG (за исключением отсутствия альфа-канала),
    со значениями, усеченными до разрядности целого числа без знака.

    Для изображений PNG в оттенках серого и палитры значение сохраняется в background_r. Ценности
    в background_g и background_b тогда не используются. Декодер установит их
    равны background_r , в этом случае кодер игнорирует их.

    При декодировании вы можете получить их в цветовом режиме, отличном от того, который вы запрашивали.
    для необработанных пикселей: тип цвета и разрядность, определенные info_png .color, то есть
    используются те, которые определены в заголовке образа PNG.

    При кодировании с помощью auto_convert вы должны использовать цветовую модель, определенную в info_png .color для
    эти ценности. Кодер обычно игнорирует info_png .color, когда auto_convert включен, но будет
    используйте его для интерпретации этих значений (и преобразования их копий в выбранную цветовую модель).

    При кодировании избегайте установки дорогого цвета, например значения, отличного от серого.
    когда изображение серое, иначе сжатие будет хуже, так как оно будет вынуждено
    напишите PNG с более дорогим цветовым режимом (когда auto_convert включен).

    Декодер не использует этот цвет фона для редактирования цвета пикселей. Это
    совершенно необязательная функция метаданных.
    */
    unsigned background_defined; /*is a suggested background color given?*/
    unsigned background_r;       /*компонент красного/серого/палитры предлагаемого цвета фона*/
    unsigned background_g;       /*зеленый компонент предлагаемого цвета фона*/
    unsigned background_b;       /*синий компонент предлагаемого цвета фона*/

    /*
    Немеждународные текстовые фрагменты (tEXt и zTXt)

    Каждый из массивов char** содержит числовые строки. Фактические сообщения находятся в
    text_strings , а text_keys — это ключевые слова, которые дают краткое описание того, что
    фактический текст представляет, например. Название, Автор, Описание или что-нибудь еще.

    Все строковые поля ниже, включая строки, ключи, имена и языковые теги, завершаются нулем.
    Спецификация PNG использует нулевые символы для ключей, имен и тегов и запрещает нулевые символы.
    символы, которые будут появляться в основном тексте, поэтому мы можем использовать здесь нулевое завершение везде.

    Ключевое слово имеет длину минимум 1 символ и максимум 79 символов (плюс
    дополнительный нулевой терминатор). Не рекомендуется использовать одну длину строки.
    для текстов длиной более 79 символов.

    Не выделяйте эти текстовые буферы самостоятельно. Используйте функции инициализации/очистки
    правильно и используйте lodepng_add_text и lodepng_clear_text .

    Стандартные ключевые слова и строки текстовых фрагментов кодируются с использованием Latin-1.
    */
    size_t text_num; /*количество текстов в этих буферах char** (в itext может быть больше текстов)*/
    char ** text_keys; /*ключевое слово фрагмента текста (например, «Комментарий»)*/
    char ** text_strings; /*фактический текст*/

    /*
    Международные текстовые фрагменты (iTXt)
    Аналогично немеждународным текстовым фрагментам, но с дополнительными строками.
    «langtags» и «transkeys», а также используются следующие кодировки текста:
    keys: Latin-1, langtags: ASCII, transkeys and strings: UTF-8.
    ключи должны состоять из 1–79 символов (плюс дополнительный нулевой знак завершения), остальные
    строки имеют любую длину.
    */
    size_t itext_num; /*количество международных текстов в этом PNG*/
    char ** itext_keys; /*английское ключевое слово фрагмента текста (например, «Комментарий»)*/
    char ** itext_langtags; /*языковой тег для языка этого текста, строка ISO / IEC 646, например  ISO 639 языковой тег*/
    char ** itext_transkeys; /*ключевое слово, переведенное на международный язык - строка UTF -8*/
    char ** itext_strings; /*фактический международный текст - строка UTF -8*/

    /*временной фрагмент (tIME)*/
    unsigned time_defined; /*установите значение 1, чтобы кодер генерировал фрагмент времени*/
    LodePNGTime time;

    /*физический фрагмент (pHYs)*/
    unsigned phys_defined; /*если 0, фрагмента pHYs нет, и значения ниже не определены, если 1, то он есть.*/
    unsigned phys_x; /*пикселей на единицу в направлении x*/
    unsigned phys_y; /*пикселей на единицу в направлении y*/
    unsigned phys_unit; /*может быть 0 (неизвестная единица измерения) или 1 (метр).*/

    /*
    Блоки, связанные с цветовым профилем: gAMA, CHRM, sRGB, iCPP, sBIT.

    LodePNG не применяет никаких преобразований цвета к пикселям в кодере или декодере и не интерпретирует эти цвета.
    значения профиля. Он просто передает информацию. Если вы хотите использовать цветовые профили и конвертировать цвета, пожалуйста,
    используйте эти значения с библиотекой управления цветом.

    Дополнительную информацию о значении этих значений см. в спецификациях PNG, ICC и sRGB.
    */

    /* Чанк gAMA: необязательный, переопределяется sRGB или iCCP, если они присутствуют. */
    unsigned gama_defined; /* Присутствует ли фрагмент gAMA (0 = нет, 1 = присутствует). */
    unsigned gama_gamma;   /* Гамма-показатель, умноженный на 100000 */

    /* Чанк chRM: необязательный, переопределяется sRGB или iCCP, если они присутствуют. */
    unsigned chrm_defined; /* Присутствует ли фрагмент chRM (0 = нет, 1 = присутствует). */
    unsigned chrm_white_x; /* Белая точка x раз по 100000 */
    unsigned chrm_white_y; /* Белая точка у раз 100000 */
    unsigned chrm_red_x;   /* Красный x раз 100000 */
    unsigned chrm_red_y;   /* Красный у раз 100000 */
    unsigned chrm_green_x; /* Зеленый x раз 100000 */
    unsigned chrm_green_y; /* Зеленый у раз 100000 */
    unsigned chrm_blue_x;  /* Синий х раз 100000 */
    unsigned chrm_blue_y;  /* Синий у раз 100000 */

    /*
    Часть sRGB: необязательно. Может не появляться одновременно с iCCP.
    Если gAMA также присутствует, gAMA должна содержать значение 45455.
    Если также присутствует chRM, то chRM должен содержать соответственно 31270,32900,64000,33000,30000,60000,15000,6000.
    */
    unsigned srgb_defined; /* Присутствует ли фрагмент sRGB (0 = отсутствует, 1 = присутствует). */
    unsigned srgb_intent;  /* Цель рендеринга: 0=перцептуальный, 1=отн. колориметрический, 2=насыщенность, 3=абс. колориметрический */

    /*
    Чанк iCCP: необязательно. Может не отображаться одновременно с sRGB.

    LodePNG не анализирует и не использует профиль ICC (за исключением поля заголовка цветового пространства для краевого случая),
    для использования цвета необходима отдельная библиотека для обработки данных формата ICC (не включена в LodePNG).
    управление и конверсии.

    Для кодирования, если присутствует iCCP, рекомендуется также добавить gAMA и chRM со значениями, соответствующими ICC.
    профилируйте как можно точнее, если вы хотите это сделать, вам следует указать правильные значения gAMA и chRM и
    включите их флаги «_define», поскольку LodePNG не будет автоматически вычислять их из профиля ICC.

    Для кодирования профиль ICC согласно спецификации PNG должен быть профилем «RGB» для несерых
    Цветовые типы PNG и профиль «GRAY» для серых цветовых типов PNG. Если вы отключите auto_convert, вы должны убедиться, что
    тип профиля ICC соответствует запрошенному вами типу цвета, иначе кодировщик выдаст ошибку. Если auto_convert
    включен (по умолчанию), а профиль ICC не соответствует данным пикселей, это приведет к тому, что кодировщик
    ошибка, если пиксельные данные содержат несерые пиксели для профиля GRAY или тихое, менее оптимальное сжатие пикселя
    данные, если пиксели могут быть закодированы в оттенках серого, но профиль ICC — RGB.

    Чтобы избежать этого, не устанавливайте в изображении профиль ICC, если для этого нет веской причины, и при этом
    убедитесь, что вы вычисляете его тщательно, чтобы избежать вышеупомянутых проблем.
    */
    unsigned iccp_defined;      /* Присутствует ли чанк iCCP (0 = нет, 1 = присутствует). */
    char * iccp_name;           /* Строка с нулевым завершением и именем профиля, 1–79 байт. */
    /*
    Профиль ICC в байтах iccp_profile_size.
    Не выделяйте этот буфер самостоятельно. Используйте функции инициализации/очистки
    правильно и используйте lodepng_set_icc и lodepng_clear_icc .
    */
    unsigned char * iccp_profile;
    unsigned iccp_profile_size; /* Размер iccp_profile в байтах */

    /*
    Блок sBIT: значащие биты. Необязательные метаданные, устанавливайте их только при необходимости.

    Если эти значения определены, они дают разрядность исходных данных. Поскольку PNG хранит только 1, 2, 4, 8 или 16-битные
    для каждого канала данных значение значащих битов может использоваться для указания того, что исходные закодированные данные имеют другое значение.
    глубина выборки, например 10 или 12.

    Кодировщики, использующие это значение, при хранении данных пикселей должны использовать старшие биты.
    данных для хранения исходных битов и используйте хороший метод масштабирования глубины выборки, например
    «репликация левого бита» для заполнения младших битов, а не нулей.

    Декодеры, использующие это значение, если могут работать с данными, например. 10-бит или 12-бит, должно быть правильно
    сдвинуть данные, чтобы вернуться к исходной разрядности, но декодерам также разрешено игнорировать
    сбит и работай, например. с 8-битными или 16-битными данными напрямую из PNG, так как спасибо
    Согласно контракту кодировщика, значения, закодированные в PNG, находятся в допустимом диапазоне для разрядности PNG.

    Для изображений в оттенках серого sbit_g и sbit_b не используются, а для изображений, в которых не используется цвет.
    типа RGBA или оттенки серого+альфа, sbit_a не используется (не используется даже для изображений палитры с
    значения полупрозрачной палитры или изображения с цветовым ключом). Используемые значения должны быть
    больше нуля и меньше или равно битовой глубине PNG.

    Тип цвета из заголовка изображения PNG определяет эти используемые и неиспользуемые поля: если
    декодирование с преобразованием цветового режима, например, всегда декодирование в RGBA, эти метаданные по-прежнему
    использует только тип цвета исходного PNG и может, например, не хватает информации об альфа-канале
    если бы PNG был RGB . При кодировании с помощью auto_convert (как и без него) также всегда
    цветовая модель, определенная в info_png.color определяет это.

    NOTE: enabling sbit can hurt compression, because the encoder can then not always use
    auto_convert для выбора более оптимального цветового режима для данных, поскольку формат PNG имеет
    строгие требования к допустимым значениям сбит в сочетании с цветовыми режимами.
    Например, установка для этих полей значения 10 бит заставит кодер продолжать использовать 16 бит на канал.
    цветовой режим, даже если данные пикселей на самом деле соответствуют более эффективному 8-битному режиму.
    */
    unsigned sbit_defined; /*is significant bits given? if not, the values below are unused*/
    unsigned sbit_r;       /*красный или серый компонент значащих битов*/
    unsigned sbit_g;       /*зеленая составляющая значащих битов*/
    unsigned sbit_b;       /*синяя составляющая значащих битов*/
    unsigned sbit_a;       /*альфа-компонент значащих битов*/

    /* Конец фрагментов, связанных с цветовым профилем */


    /*
    неизвестные фрагменты: фрагменты, неизвестные LodePNG, передаваемые побайтно.

    Есть 3 буфера, по одному на каждую позицию в PNG, где могут появляться неизвестные фрагменты.
    Каждый буфер последовательно содержит все неизвестные фрагменты для этой позиции.
    3 позиции:
    0: between IHDR and PLTE, 1: between PLTE and IDAT, 2: between IDAT and IEND.

    Для кодирования не сохраняйте критические фрагменты или известные фрагменты, которые включены с помощью флага «_define».
    выше здесь, поскольку кодировщик будет слепо следовать этому и затем может закодировать неверный файл PNG.
    (например, один с двумя фрагментами IHDR или недопустимая комбинация sRGB с iCCP). Но используйте
    это если вы хотите сохранить вспомогательный фрагмент, который не поддерживается LodePNG (например, sPLT или hIST),
    или любой нестандартный чанк PNG.

    Не выделяйте и не перемещайте эти данные самостоятельно. Используйте объявленные функции обхода фрагментов
    позже, например lodepng_chunk_next и lodepng_chunk_append, для чтения/записи этой структуры.
    */
    unsigned char * unknown_chunks_data[3];
    size_t unknown_chunks_size[3]; /*размер в байтах неизвестных фрагментов, предоставленных для защиты*/
#endif /*LODEPNG_COMPILE_ANCILLARY_CHUNKS*/
} LodePNGInfo;

/*функции инициализации, очистки и копирования для использования с этой структурой*/
void lodepng_info_init(LodePNGInfo * info);
void lodepng_info_cleanup(LodePNGInfo * info);
/*возвращаемое значение — это код ошибки (0 означает отсутствие ошибки)*/
unsigned lodepng_info_copy(LodePNGInfo * dest, const LodePNGInfo * source);

#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
unsigned lodepng_add_text(LodePNGInfo * info, const char * key, const char * str); /*отодвинуть оба текста одновременно*/
void lodepng_clear_text(LodePNGInfo * info); /*используйте это, чтобы снова очистить тексты после их заполнения*/

unsigned lodepng_add_itext(LodePNGInfo * info, const char * key, const char * langtag,
                           const char * transkey, const char * str); /*отодвинуть 4 текста одного фрагмента одновременно*/
void lodepng_clear_itext(LodePNGInfo * info); /*используйте это, чтобы снова очистить itexts после того, как вы их заполнили*/

/*заменяет, если существует*/
unsigned lodepng_set_icc(LodePNGInfo * info, const char * name, const unsigned char * profile, unsigned profile_size);
void lodepng_clear_icc(LodePNGInfo * info); /*используйте это, чтобы снова очистить тексты после их заполнения*/
#endif /*LODEPNG_COMPILE_ANCILLARY_CHUNKS*/

/*
Преобразует необработанный буфер из одного типа цвета в другой на основе
Структуры LodePNGColorMode для описания типа входного и выходного цвета.
См. справочное руководство в конце этого заголовочного файла, чтобы узнать, какие преобразования цветов поддерживаются.
возвращаемое значение = код ошибки LodePNG (0, если все прошло нормально, ошибка, если преобразование не поддерживается)
Выходной буфер должен иметь размер (w * h * bpp + 7)/8, где bpp — количество бит на пиксель.
типа выходного цвета ( lodepng_get_bpp ).
Для изображений с разрешением < 8 бит на пиксель в конце строк развертки не должно быть битов заполнения.
Для 16-битных цветов на канал используется формат с прямым порядком байтов, например PNG.
Возвращаемое значение — код ошибки LodePNG.
*/
unsigned lodepng_convert(unsigned char * out, const unsigned char * in,
                         const LodePNGColorMode * mode_out, const LodePNGColorMode * mode_in,
                         unsigned w, unsigned h);

#ifdef LODEPNG_COMPILE_DECODER
/*
Настройки декодера. Здесь содержатся настройки для PNG и Zlib.
декодер, но не настройки Info из структур Info.
*/
typedef struct LodePNGDecoderSettings {
    LodePNGDecompressSettings zlibsettings; /*здесь настройка игнорирования контрольных сумм Adler32*/

    /* Проверьте LodePNGDecompressSettings на наличие более игнорируемых ошибок, таких как ignore_adler32. */
    unsigned ignore_crc; /*игнорировать контрольные суммы CRC*/
    unsigned ignore_critical; /*игнорировать неизвестные критические фрагменты*/
    unsigned ignore_end; /*по возможности игнорировать проблемы в конце файла (отсутствует чанк IEND, слишком большой чанк,...)*/
    /* TODO: make a system involving warnings with levels and a strict mode instead. Other potentially recoverable
       errors: srgb rendering intent value, size of content of ancillary chunks, more than 79 characters for some
       строки, правила размещения/комбинации вспомогательных фрагментов, контрольная сумма неизвестных фрагментов, разрешенные символы
       в строковых ключах и т.д. */

    unsigned color_convert; /*нужно ли конвертировать PNG в нужный вам цветотип. По умолчанию: да*/

#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
    unsigned read_text_chunks; /*если ложно, но remember_unknown_chunks истинно, они хранятся в неизвестных фрагментах*/

    /*хранить все байты из неизвестных фрагментов в LodePNGInfo (по умолчанию отключено, полезно для редактора png)*/
    unsigned remember_unknown_chunks;

    /* максимальный размер фрагментов распакованного текста. Если текст текстового фрагмента больше этого значения, возвращается ошибка.
    если только чтение фрагментов текста не отключено или этот предел не установлен выше или не отключен. Установите значение 0, чтобы разрешить любой размер.
    По умолчанию это значение, которое предотвращает использование памяти неоправданно большими строками. */
    size_t max_text_size;

    /* максимальный размер сжатых фрагментов ICC. Если профиль ICC больше этого значения, будет возвращена ошибка. Установить на
    0, чтобы разрешить любой размер. По умолчанию это значение, которое предотвращает профили ICC, которые будут намного больше, чем любой другой.
    законный профиль может занимать память. */
    size_t max_icc_size;
#endif /*LODEPNG_COMPILE_ANCILLARY_CHUNKS*/
} LodePNGDecoderSettings;

void lodepng_decoder_settings_init(LodePNGDecoderSettings * settings);
#endif /*LODEPNG_COMPILE_DECODER*/

#ifdef LODEPNG_COMPILE_ENCODER
/*автоматически использовать тип цвета с меньшим количеством бит на пиксель, если это возможно без потерь. По умолчанию: AUTO*/
typedef enum LodePNGFilterStrategy {
    /*каждый фильтр на нуле*/
    LFS_ZERO = 0,
    /*каждый фильтр по 1, 2, 3 или 4 (paeth), в отличие от LFS_ZERO не лучший выбор, но для тестирования*/
    LFS_ONE = 1,
    LFS_TWO = 2,
    LFS_THREE = 3,
    LFS_FOUR = 4,
    /*Используйте фильтр, который дает минимальную сумму, как описано в официальной эвристике фильтра PNG.*/
    LFS_MINSUM,
    /*Используйте тип фильтра, который дает наименьшую энтропию Шеннона для этой строки сканирования. В зависимости
    на изображении это лучше или хуже минимума.*/
    LFS_ENTROPY,
    /*
    Фильтры PNG грубого поиска путем сжатия каждого фильтра для каждой строки сканирования.
    Экспериментальный, очень медленный и лишь в редких случаях обеспечивает лучшее сжатие, чем MINSUM.
    */
    LFS_BRUTE_FORCE,
    /*использовать буфер predefined_filters: вы указываете тип фильтра для каждой строки сканирования*/
    LFS_PREDEFINED
} LodePNGFilterStrategy;

/*Дает характеристики целочисленных цветов изображения RGBA (количество, использование альфа-канала, разрядность, ...),
что помогает решить, какую цветовую модель использовать для кодирования.
Используется внутри по умолчанию, если «auto_convert» включен. Публичный, потому что он полезен для пользовательских алгоритмов.*/
typedef struct LodePNGColorStats {
    unsigned colored; /*не оттенки серого*/
    unsigned key; /*изображение не является непрозрачным, и вместо полной альфа-карты возможен цветовой ключ.*/
    unsigned short key_r; /*значения ключей, всегда 16-битные, в 8-битном случае байт дублируется, например. 65535 означает 255*/
    unsigned short key_g;
    unsigned short key_b;
    unsigned alpha; /*изображение не непрозрачно и требуется альфа-канал или альфа-палитра*/
    unsigned numcolors; /*количество цветов до 257. Недействительно, если биты == 16 или allow_palette отключены.*/
    unsigned char
    palette[1024]; /*Запоминает до первых 256 цветов RGBA в произвольном порядке, действует только тогда, когда допустимо значение numcolors.*/
    unsigned bits; /*бит на канал (не для палитры). 1,2 или 4 только для оттенков серого. 16, если требуется 16 бит на канал.*/
    size_t numpixels;

    /*пользовательские настройки для расчета/использования статистики*/
    unsigned allow_palette; /*по умолчанию 1. если 0, запретить выбор типа цвета палитры в auto_choose_color и не считать количество цветов.*/
    unsigned allow_greyscale; /*по умолчанию 1. если 0, выберите RGB или RGBA, даже если изображение имеет только серые цвета.*/
} LodePNGColorStats;

void lodepng_color_stats_init(LodePNGColorStats * stats);

/*Получите LodePNGColorStats изображения. Статистика уже должна быть запущена.
Возвращает код ошибки (например, сбой выделения) или 0, если все в порядке.*/
unsigned lodepng_compute_color_stats(LodePNGColorStats * stats,
                                     const unsigned char * image, unsigned w, unsigned h,
                                     const LodePNGColorMode * mode_in);

/*Настройки кодировщика.*/
typedef struct LodePNGEncoderSettings {
    LodePNGCompressSettings zlibsettings; /*настройки кодировщика zlib, такие как размер окна,...*/

    unsigned auto_convert; /*автоматически выбирает выходной тип цвета PNG. По умолчанию: правда*/

    /*Если это правда, то следует официальной эвристике PNG: если PNG использует палитру или ниже
    Глубина 8 бит, установите все фильтры на ноль. В противном случае используйте filter_strategy. Обратите внимание, что для
    полностью следовать официальной эвристике PNG, filter_palette_zero должно быть истинным и
    filter_strategy должен быть LFS_MINSUM*/
    unsigned filter_palette_zero;
    /*Какую стратегию фильтрации использовать, если не используются нули из-за filter_palette_zero .
    Установите filter_palette_zero на 0, чтобы всегда использовать выбранную вами стратегию. По умолчанию: LFS_MINSUM*/
    LodePNGFilterStrategy filter_strategy;
    /*используется, если filter_strategy равен LFS_PREDEFINED . В этом случае это должно указывать на буфер с
    той же длины, что и количество строк развертки в изображении, и каждое значение должно быть <= 5.
    необходимо очистить этот буфер, LodePNG никогда его не освободит. Не забывай, что filter_palette_zero
    должен быть установлен на 0, чтобы гарантировать, что он также используется для изображений палитры или изображений с низкой битовой глубиной.*/
    const unsigned char * predefined_filters;

    /*принудительно создать чанк PLTE, если тип цвета равен 2 или 6 (= предлагаемая палитра).
    Если цветовой тип равен 3, всегда создается PLTE. Если тип цвета задан явно
    к типу оттенков серого (1 или 4), это не делается и игнорируется. Если это включено,
    палитра должна присутствовать в info_png.
    NOTE: enabling this may worsen compression if auto_convert is used to choose
    оптимальный цветовой режим, поскольку в этом случае нельзя использовать цветовые режимы в оттенках серого*/
    unsigned force_palette;
#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
    /*добавить идентификатор и версию LodePNG в виде текстового фрагмента для отладки*/
    unsigned add_id;
    /*кодировать текстовые фрагменты как фрагменты zTXt вместо фрагментов tEXt и использовать сжатие в фрагментах iTXt*/
    unsigned text_compression;
#endif /*LODEPNG_COMPILE_ANCILLARY_CHUNKS*/
} LodePNGEncoderSettings;

void lodepng_encoder_settings_init(LodePNGEncoderSettings * settings);
#endif /*LODEPNG_COMPILE_ENCODER*/


#if defined(LODEPNG_COMPILE_DECODER) || defined(LODEPNG_COMPILE_ENCODER)
/*Настройки, состояние и информация для расширенного кодирования и декодирования.*/
typedef struct LodePNGState {
#ifdef LODEPNG_COMPILE_DECODER
    LodePNGDecoderSettings decoder; /*настройки декодирования*/
#endif /*LODEPNG_COMPILE_DECODER*/
#ifdef LODEPNG_COMPILE_ENCODER
    LodePNGEncoderSettings encoder; /*настройки кодирования*/
#endif /*LODEPNG_COMPILE_ENCODER*/
    LodePNGColorMode info_raw; /*указывает формат, в котором вы хотите получить необработанный буфер пикселей.*/
    LodePNGInfo info_png; /*информация об изображении PNG, полученном после декодирования*/
    unsigned error;
} LodePNGState;

/*функции инициализации, очистки и копирования для использования с этой структурой*/
void lodepng_state_init(LodePNGState * state);
void lodepng_state_cleanup(LodePNGState * state);
void lodepng_state_copy(LodePNGState * dest, const LodePNGState * source);
#endif /* определено( LODEPNG_COMPILE_DECODER ) || определено( LODEPNG_COMPILE_ENCODER ) */

#ifdef LODEPNG_COMPILE_DECODER
/*
То же, что lodepng_decode_memory, но использует LodePNGState, чтобы разрешить пользовательские настройки и
получить гораздо больше информации об изображении и цветовом режиме PNG.
*/
unsigned lodepng_decode(unsigned char ** out, unsigned * w, unsigned * h,
                        LodePNGState * state,
                        const unsigned char * in, size_t insize);

/*
Прочтите заголовок PNG, но не сами данные. Это возвращает только информацию
это находится в фрагменте IHDR PNG , например, ширина, высота и тип цвета.
информация помещается в поле info_png LodePNGState.
*/
unsigned lodepng_inspect(unsigned * w, unsigned * h,
                         LodePNGState * state,
                         const unsigned char * in, size_t insize);
#endif /*LODEPNG_COMPILE_DECODER*/

/*
Считывает один фрагмент метаданных (кроме IHDR, который обрабатывается lodepng_inspect).
файла PNG и выводит то, что он прочитал в состоянии. Возвращает код ошибки в случае сбоя.
Сначала используйте lodepng_inspect с новым состоянием, а затем, например.  lodepng_chunk_find_const
чтобы найти нужный тип фрагмента, и если он не равен нулю, используйте lodepng_inspect_chunk (с
chunk_pointer - start_of_file as pos).
Поддерживает большинство фрагментов метаданных стандарта PNG (gAMA, bKGD, tEXt, ...).
Игнорирует неподдерживаемые, неизвестные, неметаданные или фрагменты IHDR (без ошибок).
Requirements: &in[pos] must point to start of a chunk, must use regular
lodepng_inspect первым, поскольку формат большинства других фрагментов зависит от IHDR, и если
есть чанк PLTE, который необходимо проверить перед tRNS или bKGD.
*/
unsigned lodepng_inspect_chunk(LodePNGState * state, size_t pos,
                               const unsigned char * in, size_t insize);

#ifdef LODEPNG_COMPILE_ENCODER
/*Эта функция выделяет выходной буфер с помощью стандартного malloc и сохраняет размер в *outsize.*/
unsigned lodepng_encode(unsigned char ** out, size_t * outsize,
                        const unsigned char * image, unsigned w, unsigned h,
                        LodePNGState * state);
#endif /*LODEPNG_COMPILE_ENCODER*/

/*
Функции lodepng_chunk обычно не нужны, кроме как для обхода
неизвестные фрагменты, хранящиеся в структуре LodePNGInfo, или добавляйте в нее новые.
Это также позволяет самостоятельно просматривать фрагменты закодированного файла PNG.

Указатель чанка всегда указывает на начало самого чанка, т.е.
первый байт из 4 байтов длины.

В формате файла PNG чанки имеют следующий формат:
-4 байта длина: длина данных чанка в байтах (сам чанк на 12 байт длиннее)
Тип фрагмента -4 байта (только ASCII a-z, A-Z, см. ниже)
-длина данных в байтах (может быть 0 байт, если длина равна 0)
-4 байта CRC, вычисляемые по имени чанка + данным

Первый фрагмент начинается с 8-го байта файла PNG, вся остальная часть файла
существует из объединенных фрагментов указанного выше формата.

Стандартные правила именования PNG чанка ASCII:
-Первый байт: верхний регистр = критический, нижний регистр = вспомогательный.
-Второй байт: верхний регистр = общедоступный, нижний регистр = частный.
-Третий байт: должен быть в верхнем регистре
-Четвертый байт: верхний регистр = копирование небезопасно, нижний регистр = копирование безопасно.
*/

/*
Получает длину данных фрагмента. Общая длина фрагмента на 12 байт больше.
Должно быть не менее 4 байтов для чтения. Если значение результата слишком велико,
это могут быть поврежденные данные.
*/
unsigned lodepng_chunk_length(const unsigned char * chunk);

/*помещает 4-байтовый тип в строку, завершающуюся нулем*/
void lodepng_chunk_type(char type[5], const unsigned char * chunk);

/*проверьте, является ли тип заданным типом*/
unsigned char lodepng_chunk_type_equals(const unsigned char * chunk, const char * type);

/*0: it's one of the critical chunk types, 1: it's an ancillary chunk (see PNG standard)*/
unsigned char lodepng_chunk_ancillary(const unsigned char * chunk);

/*0: public, 1: private (see PNG standard)*/
unsigned char lodepng_chunk_private(const unsigned char * chunk);

/*0: the chunk is unsafe to copy, 1: the chunk is safe to copy (see PNG standard)*/
unsigned char lodepng_chunk_safetocopy(const unsigned char * chunk);

/*получить указатель на данные фрагмента, где входные данные указывают на заголовок фрагмента*/
unsigned char * lodepng_chunk_data(unsigned char * chunk);
const unsigned char * lodepng_chunk_data_const(const unsigned char * chunk);

/*возвращает 0, если crc правильный, 1, если он неправильный (0 для OK, как обычно!)*/
unsigned lodepng_chunk_check_crc(const unsigned char * chunk);

/*генерирует правильный CRC из данных и помещает его в последние 4 байта фрагмента*/
void lodepng_chunk_generate_crc(unsigned char * chunk);

/*
Итерация к следующим фрагментам позволяет перебирать все фрагменты файла PNG.
Ввод должен находиться в начале фрагмента (результат предыдущего вызова lodepng_chunk_next,
или 8-й байт файла PNG, который всегда имеет первый фрагмент), или, альтернативно, может
указывает на первый байт файла PNG (который является не чаном, а магическим заголовком,
функция затем пропустит его и вернет первый реальный фрагмент).
Выведет указатель на начало следующего фрагмента или на конец файла или за его пределами, если таковой имеется.
после этого чанка больше нет или, возможно, если чанк поврежден.
Запустите этот процесс с 8-го байта файла PNG.
В неповрежденном файле PNG последний фрагмент должен иметь имя «IEND».
*/
unsigned char * lodepng_chunk_next(unsigned char * chunk, unsigned char * end);
const unsigned char * lodepng_chunk_next_const(const unsigned char * chunk, const unsigned char * end);

/*Находит первый чанк заданного типа в диапазоне [chunk, end) или возвращает NULL, если не найден.*/
unsigned char * lodepng_chunk_find(unsigned char * chunk, unsigned char * end, const char type[5]);
const unsigned char * lodepng_chunk_find_const(const unsigned char * chunk, const unsigned char * end,
                                               const char type[5]);

/*
Добавляет фрагмент к данным на выходе. Данный чанк уже должен иметь заголовок чанка.
Переменная out и outsize обновляются, чтобы отразить новый перераспределенный буфер.
Возвращает код ошибки (0, если все прошло нормально)
*/
unsigned lodepng_chunk_append(unsigned char ** out, size_t * outsize, const unsigned char * chunk);

/*
Добавляет новый фрагмент в out. Добавляемый фрагмент задается указанием его длины, типа
и данные отдельно. Тип — строка из 4 букв.
Переменная out и outsize обновляются, чтобы отразить новый перераспределенный буфер.
Вернуть код ошибки (0, если все прошло нормально)
*/
unsigned lodepng_chunk_create(unsigned char ** out, size_t * outsize, size_t length,
                              const char * type, const unsigned char * data);


/*Вычислить CRC32 буфера*/
unsigned lodepng_crc32(const unsigned char * buf, size_t len);
#endif /*LODEPNG_COMPILE_PNG*/


#ifdef LODEPNG_COMPILE_ZLIB
/*
Эту часть zlib можно использовать независимо для сжатия и распаковки zlib.
буфер. Однако его нельзя использовать для создания файлов gzip, и он поддерживает только
часть zlib, необходимая для PNG, она не поддерживает словари.
*/

#ifdef LODEPNG_COMPILE_DECODER
/*Раздуть буфер. Накачивание — это этап декомпрессии сдувания. После использования буфер должен быть освобожден.*/
unsigned lodepng_inflate(unsigned char ** out, size_t * outsize,
                         const unsigned char * in, size_t insize,
                         const LodePNGDecompressSettings * settings);

/*
Распаковывает данные Zlib. Перераспределяет выходной буфер и добавляет данные.
данные должны соответствовать спецификации zlib.
Либо *out должен быть NULL, а *outsize должен быть 0, либо *out должен быть допустимым.
буфер и *увеличить его размер в байтах. out должен быть освобожден пользователем после использования.
*/
unsigned lodepng_zlib_decompress(unsigned char ** out, size_t * outsize,
                                 const unsigned char * in, size_t insize,
                                 const LodePNGDecompressSettings * settings);
#endif /*LODEPNG_COMPILE_DECODER*/

#ifdef LODEPNG_COMPILE_ENCODER
/*
Сжимает данные с помощью Zlib. Перераспределяет выходной буфер и добавляет данные.
Zlib добавляет небольшой заголовок и трейлер вокруг данных выкачивания.
Данные выводятся в формате спецификации zlib.
Либо *out должен быть NULL, а *outsize должен быть 0, либо *out должен быть допустимым.
буфер и *увеличить его размер в байтах. out должен быть освобожден пользователем после использования.
*/
unsigned lodepng_zlib_compress(unsigned char ** out, size_t * outsize,
                               const unsigned char * in, size_t insize,
                               const LodePNGCompressSettings * settings);

/*
Найдите ограниченный по длине код Хаффмана для заданных частот. Эта функция находится в
общедоступный интерфейс только для тестов, он используется внутри lodepng_deflate.
*/
unsigned lodepng_huffman_code_lengths(unsigned * lengths, const unsigned * frequencies,
                                      size_t numcodes, unsigned maxbitlen);

/*Сжать буфер с помощью Deflate. См. RFC 1951. После использования внешний буфер должен быть освобожден.*/
unsigned lodepng_deflate(unsigned char ** out, size_t * outsize,
                         const unsigned char * in, size_t insize,
                         const LodePNGCompressSettings * settings);

#endif /*LODEPNG_COMPILE_ENCODER*/
#endif /*LODEPNG_COMPILE_ZLIB*/

#ifdef LODEPNG_COMPILE_DISK
    /*
    Загрузите файл с диска в буфер. Функция выделяет выходной буфер и
    после использования вы должны освободить его.
    out: output parameter, contains pointer to loaded buffer.
    outsize: output parameter, size of the allocated out buffer
    filename: the path to the file to load
    Возвращаемое значение: код ошибки (0 означает ок)

    NOTE: Wide-character filenames are not supported, you can use an external method
    для обработки таких файлов и декодирования в памяти.
    */
    unsigned lodepng_load_file(unsigned char ** out, size_t * outsize, const char * filename);

    /*
    Сохраните файл из буфера на диск. Внимание, если она существует, эта функция перезаписывает
    файл без предупреждения!
    buffer: the buffer to write
    buffersize: size of the buffer to write
    filename: the path to the file to save to
    Возвращаемое значение: код ошибки (0 означает ок)

    NOTE: Wide-character filenames are not supported, you can use an external method
    для обработки таких файлов и кодирования в памяти
    */
    unsigned lodepng_save_file(const unsigned char * buffer, size_t buffersize, const char * filename);
#endif /*LODEPNG_COMPILE_DISK*/

#ifdef LODEPNG_COMPILE_CPP
/* Оболочка LodePNG C++ использует std::vectors вместо выделенных вручную буферов памяти. */
namespace lodepng
{
#ifdef LODEPNG_COMPILE_PNG
class State : public LodePNGState
{
    public:
        State();
        State(const State & other);
        ~State();
        State & operator=(const State & other);
};

#ifdef LODEPNG_COMPILE_DECODER
/* То же, что и другие lodepng::decode, но с использованием состояния для получения дополнительных настроек и информации. */
unsigned decode(std::vector<unsigned char> & out, unsigned & w, unsigned & h,
                State & state,
                const unsigned char * in, size_t insize);
unsigned decode(std::vector<unsigned char> & out, unsigned & w, unsigned & h,
                State & state,
                const std::vector<unsigned char> & in);
#endif /*LODEPNG_COMPILE_DECODER*/

#ifdef LODEPNG_COMPILE_ENCODER
/* То же, что и другие lodepng::encode, но с использованием состояния для получения дополнительных настроек и информации. */
unsigned encode(std::vector<unsigned char> & out,
                const unsigned char * in, unsigned w, unsigned h,
                State & state);
unsigned encode(std::vector<unsigned char> & out,
                const std::vector<unsigned char> & in, unsigned w, unsigned h,
                State & state);
#endif /*LODEPNG_COMPILE_ENCODER*/

#ifdef LODEPNG_COMPILE_DISK
    /*
    Загрузите файл с диска в std::vector.
    Возвращаемое значение: код ошибки (0 означает ок)

    NOTE: Wide-character filenames are not supported, you can use an external method
    для обработки таких файлов и декодирования в памяти
    */
    unsigned load_file(std::vector<unsigned char> & buffer, const std::string & filename);

    /*
    Сохраните двоичные данные в std::vector в файл на диске. Файл перезаписан
    без предупреждения.

    NOTE: Wide-character filenames are not supported, you can use an external method
    для обработки таких файлов и кодирования в памяти
    */
    unsigned save_file(const std::vector<unsigned char> & buffer, const std::string & filename);
#endif /* LODEPNG_COMPILE_DISK */
#endif /* LODEPNG_COMPILE_PNG */

#ifdef LODEPNG_COMPILE_ZLIB
#ifdef LODEPNG_COMPILE_DECODER
/* Zlib-распаковать буфер беззнаковых символов */
unsigned decompress(std::vector<unsigned char> & out, const unsigned char * in, size_t insize,
                    const LodePNGDecompressSettings & settings = lodepng_default_decompress_settings);

/* Zlib-распаковать std::vector */
unsigned decompress(std::vector<unsigned char> & out, const std::vector<unsigned char> & in,
                    const LodePNGDecompressSettings & settings = lodepng_default_decompress_settings);
#endif /* LODEPNG_COMPILE_DECODER */

#ifdef LODEPNG_COMPILE_ENCODER
/* Zlib-сжать буфер беззнаковых символов */
unsigned compress(std::vector<unsigned char> & out, const unsigned char * in, size_t insize,
                  const LodePNGCompressSettings & settings = lodepng_default_compress_settings);

/* Zlib-сжать std::vector */
unsigned compress(std::vector<unsigned char> & out, const std::vector<unsigned char> & in,
                  const LodePNGCompressSettings & settings = lodepng_default_compress_settings);
#endif /* LODEPNG_COMPILE_ENCODER */
#endif /* LODEPNG_COMPILE_ZLIB */
} /* пространство имен lodepng */
#endif /*LODEPNG_COMPILE_CPP*/

/*
TODO:
[.] проверить, нет ли утечек памяти или уязвимостей безопасности — сделано много, но нужно часто проверять
[.] проверка совместимости с различными компиляторами — сделано, но необходимо переделывать для каждой новой версии
[X] преобразование цвета в 16-битный для каждого типа канала
[X] поддерживают типы фрагментов цветового профиля (но никогда не позволяйте им касаться значений RGB по умолчанию)
[ ] поддерживают все общедоступные типы фрагментов PNG (почти готовы, кроме sPLT и hIST)
[ ] убедитесь, что кодер не генерирует фрагменты размером > (2^31)-1
[ ] частичное декодирование (потоковая обработка)
[X] разрешить функции «isFullyOpaque» проверять цветовые клавиши и прозрачные палитры.
[X] лучшее имя для переменных "codes", "codesD", "codelengthcodes", "clcl" и "lldl"
[ ] позволяют обрабатывать некоторые ошибки как предупреждения, когда изображение можно восстановить (например, 69, 57, 58)
[ ] выдает предупреждения, такие как: палитра oob, ошибка контрольной суммы, данные после iend, неправильный/неизвестный критический фрагмент, отсутствие нулевого терминатора в тексте, ...
[ ] сообщения об ошибках с номерами строк (и версией)
[ ] errors in state instead of as return code?
[ ] новые ошибки/предупреждения, такие как подозрительно большой распакованный фрагмент ztxt или iccp.
[ ] позволить оболочке C++ перехватывать исключения, поступающие из стандартной библиотеки, и возвращать коды ошибок LodePNG
[ ] позволяет пользователю предоставлять собственные функции преобразования цветов, например для предварительно умноженной альфа, битов заполнения или нет,...
[ ] разрешить пользователю передавать данные (void*) в пользовательский распределитель
[X] предоставляет альтернативы функциям библиотеки C, отсутствующим на некоторых платформах (memcpy,...)
*/

#endif /*LV_USE_LODEPNG*/

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /*LODEPNG_H предохранитель включения*/

/*
Документация LodePNG
---------------------

0. table of contents
--------------------

  1. about
   1.1. поддерживаемые функции
   1.2. функции не поддерживаются
  2. C and C++ version
  3. security
  4. decoding
  5. encoding
  6. color conversions
    6.1.  Цветотипы PNG
    6.2. преобразование цвета
    6.3. биты заполнения
    6.4. Примечание о 16 битах на канал и порядке байтов.
  7. error values
  8. chunks and PNG editing
  9. compiler support
  10. examples
   10.1. пример декодера C++
   10.2. пример декодера C
  11. state settings reference
  12. changes
  13. contact information


1. about
--------

PNG — это формат файла для хранения растровых изображений без потерь с хорошим сжатием.
поддержка различных типов цвета и альфа-канала.

LodePNG — это кодек PNG в соответствии с Portable Network Graphics (PNG).
Спецификация (второе издание) – Рекомендация W3C от 10 ноября 2003 г.

Используемые спецификации:

*) Спецификация портативной сетевой графики (PNG) (второе издание):
     http://www.w3.org/TR/2003/REC-PNG-20031110
*) RFC 1950 ZLIB Формат сжатых данных, версия 3.3:
     http://www.gzip.org/zlib/rfc-zlib.html
*) RFC 1951 DEFLATE Спецификация формата сжатых данных, версия 1.3:
     http://www.gzip.org/zlib/rfc-deflate.html

Самую последнюю версию LodePNG в настоящее время можно найти по адресу:
http://lodev.org/lodepng/

LodePNG работает как на C ( ISO C90 ), так и на C++, с оболочкой C++, которая добавляет
дополнительный функционал.

LodePNG состоит из двух файлов:
- lodepng.h: заголовочный файл для C и C++.
- lodepng.c (pp): дайте ему имя lodepng.c или lodepng.cpp (или .cc) в зависимости от вашего использования.

Если вы хотите сразу начать использовать LodePNG, не читая этот документ, получите
примеры с веб-сайта LodePNG, чтобы узнать, как использовать его в коде, или проверьте
меньшие примеры в главе 13 здесь.

LodePNG прост, но поддерживает только основные требования. Чтобы достичь
простота, были выбраны следующие варианты дизайна: Зависимости отсутствуют.
в любой внешней библиотеке. Существуют функции для декодирования и кодирования PNG с помощью
один вызов функции и расширенные версии этих функций, принимающие
Структура LodePNGState, позволяющая указать или получить дополнительную информацию. По умолчанию
цвета необработанного изображения всегда RGB или RGBA, независимо от типа цвета.
используется файл PNG. Для чтения и записи файлов существуют простые функции.
конвертировать файлы в/из буферов в памяти.

Все это делает LodePNG подходящим для загрузки текстур в играх, демо-версиях и небольших
программы,... Он менее подходит для полноценных редакторов изображений, загружающих PNG.
по сети (требуется, чтобы все данные изображения были доступны, прежде чем можно будет декодировать
начало), жизненно важные системы,...

1.1. поддерживаемые функции
-----------------------

Декодер поддерживает следующие функции:

*) декодирование PNG-файлов с любым типом цвета, битовой глубиной и режимом чересстрочной развертки в 24- или 32-битное цветное необработанное изображение,
   или того же цвета, что и PNG
*) кодирование PNG из любого необработанного изображения в 24- или 32-битный цвет или тот же тип цвета, что и необработанное изображение.
*) Адам7 интерлейс и деинтерлейс для любого цветотипа
*) загрузка изображения с жесткого диска или декодирование его из буфера из других источников, кроме жесткого диска
*) поддержка альфа-каналов, включая цветовую модель RGBA, полупрозрачные палитры и цветовое кодирование.
*) декомпрессия zlib (раздувание)
*) сжатие zlib (выкачивание)
*) Контрольные суммы CRC32 и ADLER32
*) колориметрические преобразования цветовых профилей: в настоящее время экспериментально доступны только в lodepng_util.cpp,
   плюс альтернативная возможность передавать информацию о профилях цветности/гаммы/ICC в другую систему управления цветом.
*) обработка неизвестных фрагментов, позволяющая создать редактор PNG, в котором хранятся пользовательские и неизвестные фрагменты.
*) следующие фрагменты поддерживаются как кодером, так и декодером:
    IHDR: header information
    PLTE: color palette
    IDAT: pixel data
    IEND: the final chunk
    tRNS: transparency for palettized images
    tEXt: textual information
    zTXt: compressed textual information
    iTXt: international textual information
    bKGD: suggested background color
    pHYs: physical dimensions
    tIME: modification time
    cHRM: RGB chromaticities
    gAMA: RGB gamma correction
    iCCP: ICC color profile
    sRGB: rendering intent
    sBIT: significant bits

1.2. функции не поддерживаются
---------------------------

Следующие функции (пока) не поддерживаются:

*) некоторые функции, необходимые для создания совместимого с PNG -Editor, могут все еще отсутствовать.
*) частичная загрузка/потоковая обработка. Все данные должны быть доступны и обрабатываются за один вызов.
*) Публичные чанки hIST и sPLT (пока) не поддерживаются, но рассматриваются как неизвестные чанки.


2. C and C++ version
--------------------

Версия C использует буферы, выделенные с помощью alloc, которые вам нужны для free().
себя. Вам необходимо использовать функции инициализации и очистки для каждой структуры всякий раз, когда
использование структуры из версии C, чтобы избежать эксплойтов и утечек памяти.

Версия C++ имеет дополнительные функции с std::vectors в интерфейсе и
lodepng::State класс, который представляет собой LodePNGState с конструктором и деструктором.

Эти файлы работают без изменений как для компиляторов C, так и для C++, поскольку все
дополнительный код C++ находится в блоках «#ifdef __cplusplus», которые составляют C-компиляторы.
игнорируйте это, и код C будет компилироваться как со строгим ISO C90, так и с C++.

Чтобы использовать версию C++, вам необходимо переименовать исходный файл в lodepng.cpp.
(вместо lodepng.c) и скомпилируйте его компилятором C++.

Чтобы использовать версию C, вам необходимо переименовать исходный файл в lodepng.c (вместо этого
из lodepng.cpp) и скомпилируйте его компилятором C.


3. Security
-----------

Даже при тщательном проектировании всегда возможно, что LodePNG содержит возможные
подвиги. Если вы обнаружите такую ​​ошибку, сообщите мне, и она будет исправлена.

При использовании LodePNG необходимо также соблюдать осторожность с версией LodePNG C.
как структуры в стиле C при работе с C++. Используются следующие соглашения
для всех структур в стиле C:

- если структура имеет соответствующую функцию инициализации, всегда вызывайте функцию инициализации при создании новой
- если структура имеет соответствующую функцию очистки, вызовите ее до исчезновения структуры, чтобы избежать утечек памяти.
-если структура имеет соответствующую функцию копирования, используйте функцию копирования вместо "=".
 Пункт назначения также должен быть уже инициализирован.


4. Decoding
-----------

Декодирование преобразует сжатое изображение PNG в буфер необработанных пикселей.

Большая часть документации по использованию декодера находится в его объявлениях в шапке.
выше. Для C простое декодирование можно выполнить с помощью таких функций, как
lodepng_decode32, а более сложное декодирование можно выполнить с помощью структуры
LodePNGState и lodepng_decode. В C++ все декодирование можно выполнить с помощью
различные функции lodepng::decode, а lodepng::State можно использовать для расширенных
особенности.

При использовании LodePNGState для декодирования используются следующие поля:
*) LodePNGInfo info_png: здесь хранится дополнительная информация о PNG (входные данные).
*) LodePNGColorMode info_raw: здесь вы можете указать, какой цветовой режим необработанного изображения (выходного изображения) вы хотите получить.
*) Декодер LodePNGDecoderSettings: вы можете указать несколько дополнительных настроек для использования декодером.

LodePNGInfo info_png
--------------------

После декодирования оно содержит дополнительную информацию об изображении PNG, кроме фактического
пикселей, ширины и высоты, поскольку они уже получены непосредственно из декодера
функции.

Он содержит, например, исходный цветовой тип изображения PNG, текстовые комментарии,
предлагаемый цвет фона и т. д. Более подробная информация о структуре LodePNGInfo приведена ниже.
в своей декларационной документации.

LodePNGColorMode info_raw
-------------------------

При расшифровке здесь можно указать какой цветотип вы хотите
результирующее необработанное изображение. Если он отличается от цветотипа
PNG , то декодер автоматически преобразует результат. Это преобразование
всегда работает, за исключением случаев, когда вы хотите преобразовать цвет PNG в оттенки серого или в
палитра с недостающими цветами.

По умолчанию для результата используется 32-битный цвет.

LodePNGDecoderНастройки декодера
------------------------------

Эти настройки можно использовать для игнорирования ошибок, созданных недействительными CRC и Adler32.
фрагменты и отключить декодирование фрагментов tEXt.

Также есть настройка color_convert, по умолчанию истинная. Если false, преобразования нет.
готово, результирующие данные будут такими же, как в PNG (после распаковки)
и вам придется самостоятельно разгадывать цвета пикселей, используя
информация о типе цвета в файле LodePNGInfo.


5. Encoding
-----------

Кодирование преобразует необработанный буфер пикселей в сжатое изображение PNG.

Большая часть документации по использованию кодировщика находится в его объявлениях в шапке.
выше. Для C простое кодирование можно выполнить с помощью таких функций, как
lodepng_encode32, а более сложное декодирование можно выполнить с помощью структуры
LodePNGState и lodepng_encode. В C++ все кодирование можно выполнить с помощью
различные функции lodepng::encode, а lodepng::State можно использовать для расширенных
особенности.

Как и декодер, кодер также может выдавать ошибки. Однако это дает меньше ошибок
поскольку вход кодировщика является доверенным, вход декодера (изображение PNG, которое может
быть подделанным кем-либо) не вызывает доверия.

При использовании LodePNGState для кодирования используются следующие поля:
*) LodePNGInfo info_png: здесь вы указываете, каким должен быть PNG (выходной результат).
*) LodePNGColorMode info_raw: здесь вы указываете, какой тип цвета имеет необработанное изображение (входное изображение).
*) Кодер LodePNGencoderSettings: вы можете указать несколько настроек, которые будет использовать кодировщик.

LodePNGInfo info_png
--------------------

При кодировании вы используете это противоположным образом, чем при декодировании: для кодирования
вы заполняете значения, которые хотите, чтобы PNG имел перед кодированием. По умолчанию это
не нужно указывать тип цвета для PNG, поскольку он выбирается автоматически,
но при правильных настройках можно выбрать его самостоятельно.

Кодировщик не всегда будет точно соответствовать указанной вами структуре LodePNGInfo.
он старается как можно ближе. Некоторые вещи игнорируются кодировщиком.
кодер использует, например, следующие настройки из него, когда это применимо:
тип цвета и разрядность, фрагменты текста, фрагмент времени, цветовой ключ, палитра,
цвет фона, метод чересстрочной развертки, неизвестные фрагменты, ...

При кодировании в PNG с цветовым типом 3 кодер сгенерирует фрагмент PLTE.
Если палитра содержит цвета, у которых альфа-канал не равен 255 (т. е.
в палитре есть полупрозрачные цвета), это добавит кусок tRNS.

LodePNGColorMode info_raw
-------------------------

Здесь вы указываете тип цвета необработанного изображения, которое вы передаете на вход,
включая возможный прозрачный цветовой ключ и палитру, которую вы используете в
ваши необработанные данные изображения.

По умолчанию предполагается 32-битный цвет, то есть вводимые данные должны быть в формате RGBA.
формат с 4 байтами (беззнаковыми символами) на пиксель.

Кодер LodePNGEncoderSettings
------------------------------

Поддерживаются следующие настройки (некоторые находятся в подструктурах):
*) auto_convert : когда эта опция включена, кодер будет
автоматически выбирает минимально возможный цветовой режим (включая цветовой ключ), который
может кодировать цвета всех пикселей без потери информации.
*) btype: тип блока для LZ77. 0 = несжатый, 1 = фиксированное дерево Хаффмана,
   2 = dynamic huffman tree (best compression). Should be 2 for proper
   сжатие.
*) use_lz77 : использовать или нет LZ77 для сжатых типов блоков. Должно быть
   верно для правильного сжатия.
*) размер окна: размер окна, используемый кодировщиком LZ77 (1–32768). Имеет ценность
   По умолчанию 2048, но можно установить на 32768 для лучшего, но медленного сжатия.
*) force_palette: если тип цвета 2 или 6, вы можете заставить кодировщик записать PLTE.
   chunk, если force_palette истинно. Это можно использовать в качестве предлагаемой палитры для преобразования
   для зрителей, которые не поддерживают более 256 цветов (если они еще существуют)
*) add_id : добавить к изображению текстовый фрагмент «Кодировщик: LodePNG <версия>».
*) text_compression: по умолчанию 1. Если 1, текст будет храниться как zTXt вместо фрагментов tEXt.
  Чанки zTXt используют сжатие текста zlib. Это дает меньший результат
  большие тексты, но больший результат для небольших текстов (например, одно имя программы).
  Хотя это либо tEXt, либо zTXt, отдельной настройки для каждого текста пока нет.


6. color conversions
--------------------

В отношении LodePNG важно отметить, что тип цвета PNG и
цветовой тип необработанного изображения полностью независимы. По умолчанию, когда
вы декодируете PNG и получаете результат в виде необработанного изображения с нужным вам цветовым типом,
независимо от того, был ли PNG закодирован с помощью палитры, оттенков серого или цвета RGBA.
А если вы закодируете изображение, по умолчанию LodePNG автоматически выберет PNG.
тип цвета, обеспечивающий хорошее сжатие на основе значений цветов и количества
цветов на изображении. Его можно настроить так, чтобы вы могли управлять им вместо этого, как
ну, однако.

Чтобы это сделать, LodePNG выполняет преобразование из одного цветового режима в другой.
Он может конвертировать практически любой цветовой тип в любой другой цветовой тип, за исключением
следующие преобразования: RGB в оттенки серого не поддерживаются, а преобразование в
палитра, когда в палитре нет необходимого цвета, не поддерживается. Это
не поддерживается намеренно: это потеря информации, требующая цвета
алгоритм преобразования, выходящий за рамки возможностей кодера PNG (да, RGB в серый цвет).
это легко, но есть несколько способов, если вы хотите дать некоторым каналам больше
вес).

По умолчанию при декодировании вы получаете необработанное изображение в 32-битном RGBA или 24-битном RGB.
цвет, независимо от того, какой цветотип у PNG. И по умолчанию при кодировании
LodePNG автоматически выбирает лучшую цветовую модель для вывода PNG и ожидает
входное изображение должно быть 32-битным RGBA или 24-битным RGB . Итак, если вы не хотите контролировать
цветовой формат изображений самостоятельно, эту главу можно пропустить.

6.1.  Цветотипы PNG
--------------------

Изображение PNG может иметь множество типов цвета: от 1-битного цвета до 64-битного цвета.
а также палитры цветовых режимов. После распаковки и отмены фильтрации zlib
в изображении PNG готовы, необработанные пиксельные данные будут иметь этот тип цвета и, следовательно,
определенное количество бит на пиксель. Если вы хотите получить необработанное изображение после
декодирование для получения другого типа цвета, преобразование выполняется LodePNG.

Спецификация PNG предоставляет следующие типы цветов:

0: grayscale, bit depths 1, 2, 4, 8, 16
2: RGB, bit depths 8 and 16
3: palette, bit depths 1, 2, 4 and 8
4: grayscale with alpha, bit depths 8 and 16
6: RGBA, bit depths 8 and 16

Битовая глубина — это количество бит на пиксель на цветовой канал. Итак, общая сумма
бит на пиксель составляет: количество каналов * разрядность.

6.2. преобразование цвета
----------------------

Как поясняется в разделах о кодере и декодере, вы можете указать
типы цвета и разрядность в info_png и info_raw, чтобы изменить значение по умолчанию.
поведение.

Если при декодировании вы хотите, чтобы необработанное изображение отличалось от изображения по умолчанию,
вам нужно установить тип цвета и разрядность в LodePNGColorMode,
или параметры цветового типа и разрядности простой функции декодирования.

Если при кодировании вы используете другой тип цвета, чем тот, который установлен по умолчанию в необработанных входных данных
изображения, вам необходимо указать его тип цвета и разрядность в параметре LodePNGColorMode.
необработанного изображения или используйте параметры цветового типа и глубины цвета простого
функция кодирования.

Если при кодировании вы не хотите, чтобы LodePNG выбирал выходной тип цвета PNG
но управляйте сами, нужно в настройках энкодера поставить auto_convert
значение false и укажите нужный тип цвета в LodePNGInfo файла
кодер (включая палитру: он может генерировать палитру, если auto_convert имеет значение true,
иначе нет).

Если тип входного и выходного цвета различается (выбран пользователем или выбран автоматически),
LodePNG выполнит преобразование цветов в соответствии с приведенными ниже правилами и может
иногда приводит к ошибке.

Чтобы избежать некоторой путаницы:
-декодер преобразует PNG в необработанное изображение
-кодер конвертирует необработанное изображение в PNG
-тип цвета и глубина цвета в LodePNGColorMode info_raw соответствуют необработанному изображению.
- тип цвета и разрядность в поле цвета LodePNGInfo info_png соответствуют PNG.
-при кодировании тип цвета в LodePNGInfo игнорируется, если auto_convert
 включен, вместо этого он генерируется автоматически
-при декодировании тип цвета в LodePNGInfo устанавливается декодером равным цвету оригинала
 Изображение PNG, но его можно игнорировать, поскольку вместо этого необработанное изображение имеет запрошенный вами тип цвета.
-если тип цвета изображений LodePNGColorMode и PNG не совпадает, происходит преобразование
 между типами цвета выполняется, если типы цвета поддерживаются. Если это не
 поддерживается, возвращается ошибка. Если типы одинаковы, преобразование не выполняется.
- хотя некоторые преобразования не поддерживаются, LodePNG поддерживает загрузку PNG из любого
 цветотип и сохранение PNG в любой цветотип, иногда требуется просто подготовка
 необработанное изображение правильно перед кодированием.
- и кодер, и декодер используют один и тот же преобразователь цветов.

Функция lodepng_convert выполняет преобразование цвета. Он доступен в
интерфейс, но обычно он не нужен, поскольку кодер и декодер уже вызывают
it.

Неподдерживаемые преобразования цветов:
-перевести цвет в оттенки серого, если присутствуют несерые пиксели: ошибка не выдается, но
результат будет выглядеть некрасиво, потому что занят только красный канал (он предполагает все
в этом случае три канала одинаковы, поэтому зеленый и синий игнорируются). Причина
ошибка не выдается, чтобы разрешить преобразование трехканальных изображений в оттенках серого в
одноканальный, даже если имеются числовые неточности.
-что-либо для палитры, если палитра не имеет точного соответствия исходному цвету
в нем: в этом случае выдается ошибка

Поддерживаемые преобразования цветов:
-что угодно для 8-битного RGB, 8-битного RGBA, 16-битного RGB, 16-битного RGBA
-любой серый или серый+альфа, до серого или серого+альфа
-что угодно в палитре, если в палитре есть запрошенные цвета
-удаление альфа-канала
-от большей к меньшей битовой глубине, и наоборот

Если вы не хотите, чтобы преобразование цветов выполнялось (например, для скорости или контроля):
-В кодировщике вы можете сохранить PNG с любым типом цвета, указав
цветовой режим raw и LodePNGInfo в одном и том же цветовом режиме, а для auto_convert установите значение
ложь.
-В декодере вы можете заставить его хранить данные пикселей в одном цветовом типе.
как это сделал PNG, установив для параметра color_convert значение false. Настройки в
info_raw тогда игнорируются.

6.3. биты заполнения
-----------------

В формате файла PNG, если используется тип цвета менее 8 бит на пиксель и строки развертки
имеют количество битов, не кратное 8, тогда используются биты заполнения, так что каждый
строка сканирования начинается с нового байта. Но это справедливо для необработанных входных и выходных данных LodePNG.
Необработанное входное изображение, которое вы передаете кодировщику, и необработанное выходное изображение, которое вы получаете от декодера.
будут ли NOT иметь эти биты заполнения, например. в случае 1-битного изображения шириной
из 7 пикселей первый пиксель второй строки развертки будет 8-м битом первого байта,
не первый бит нового байта.

6.4. Примечание о 16 битах на канал и порядке байтов.
----------------------------------------------------

LodePNG также использует массивы беззнаковых символов для 16-битных цветов на канал, как и
для любого другого цветового формата. 16-битные значения хранятся с прямым порядком байтов (большинство
значащий байт первым) в этих массивах. Это обратный порядок
с прямым порядком байтов, используемый x86 CPU.

LodePNG всегда использует обратный порядок байтов, поскольку формат файла PNG использует это внутри себя.
Преобразования в другие форматы, отличные от внутреннего использования PNG, не поддерживаются.
LodePNG специально, существует множество форматов, включая 16-битный порядок байтов.
цвета, порядок хранения R, G, B и A и т. д. Поддержка и
преобразование во/из всего, что выходит за рамки LodePNG.

Это может означать, что в зависимости от вашего варианта использования вы можете захотеть преобразовать большие
вывод LodePNG с порядком байтов в формате с прямым порядком байтов с помощью цикла for. Это конечно не
Всегда необходим, многие приложения и библиотеки поддерживают 16-битные цвета с прямым порядком байтов.
в любом случае, но это означает, что вы не можете просто привести буфер без знака char* к
беззнаковый короткий* буфер на процессорах x86.


7. error values
---------------

Все функции в LodePNG, возвращающие код ошибки, возвращают 0, если всё прошло успешно.
OK или ненулевой код, если произошла ошибка.

Значение значений ошибок LodePNG можно получить с помощью функции
lodepng_error_text: given the numerical error code, it returns a description
ошибки на английском языке в виде строки.

Проверьте реализацию lodepng_error_text, чтобы увидеть значение каждого кода.

Не рекомендуется использовать числовые значения для программного создания
разные решения в зависимости от типов ошибок, поскольку числа не гарантированно совпадают
оставайтесь обратно совместимыми. Они предназначены только для потребления человеком. Программно
имеет значение только 0 или не 0.


8. chunks and PNG editing
-------------------------

Если вы хотите добавить дополнительные фрагменты к PNG, который вы кодируете, или используйте LodePNG для PNG.
редактор, который должен следовать правилам обработки неизвестных фрагментов, или если ваш
программа может читать другие типы фрагментов, кроме тех, которые обрабатываются LodePNG,
тогда это возможно с помощью функций фрагментов LodePNG.

Чанк PNG имеет следующую структуру:

длина 4 байта
Имя типа длиной 4 байта
длина данных в байтах
4 байта CRC

8.1. перебор кусков
-----------------------------

Если у вас есть буфер, содержащий данные изображения PNG, то первый фрагмент (файл
IHDR) начинается с байта номер 8 этого буфера. Первые 8 байт – это
подпись PNG и не являются частью чанка. Но если вы начнете с байта 8
тогда у вас есть кусок, и вы можете проверить следующие его вещи.

NOTE: none of these functions check for memory buffer boundaries. To avoid
эксплойтов, всегда проверяйте, что буфер содержит все данные фрагментов.
При использовании lodepng_chunk_next убедитесь, что возвращаемое значение находится в пределах
выделенная память.

беззнаковый lodepng_chunk_length (const unsigned char* chunk):

Получите длину данных фрагмента. Общая длина чанка равна этой длине + 12.

void lodepng_chunk_type (тип символа[5], фрагмент const беззнакового символа*):
unsigned char lodepng_chunk_type_equals (const unsigned char* chunk, const char* тип):

Получите тип чанка или сравните, принадлежит ли он определенному типу.

unsigned char lodepng_chunk_critical (const unsigned char* chunk):
unsigned char lodepng_chunk_private (const unsigned char* chunk):
unsigned char lodepng_chunk_safetocopy (const unsigned char* chunk):

Проверьте, является ли чанк критическим в стандарте PNG (только IHDR, PLTE, IDAT и IEND).
Проверьте, является ли чанк частным (публичные чанки являются частью стандарта, а частные — нет).
Проверьте, безопасно ли копировать чанк. Если это не так, то при изменении данных в критическом
фрагмент, небезопасно копировать фрагменты старого образа, возможно, NOT будет сохранен в новом, если ваш
программа не обрабатывает этот тип неизвестного фрагмента.

беззнаковый символ* lodepng_chunk_data (кусок беззнакового символа*):
const unsigned char* lodepng_chunk_data_const (const unsigned char* chunk):

Получите указатель на начало данных чанка.

беззнаковый lodepng_chunk_check_crc (const unsigned char* chunk):
void lodepng_chunk_generate_crc (чанк беззнакового символа*):

Проверьте правильность crc или создайте правильный.

беззнаковый символ* lodepng_chunk_next (кусок беззнакового символа*):
const unsigned char* lodepng_chunk_next_const (const unsigned char* chunk):

Перейдите к следующему фрагменту. Это работает, если у вас есть буфер с последовательными фрагментами. Обратите внимание, что эти
функции не выполняют никакой проверки границ выделенных данных, поэтому убедитесь, что их достаточно.
данные, доступные в буфере, чтобы иметь возможность перейти к следующему фрагменту.

беззнаковый lodepng_chunk_append (беззнаковый символ**, size_t * нестандартный размер, константный беззнаковый символ* чанк):
беззнаковый lodepng_chunk_create (беззнаковый символ**, size_t * нестандартный размер, беззнаковая длина,
                              тип const char*, данные const без знака char*):

Эти функции используются для создания новых фрагментов, которые добавляются к данным в *out, которые имеют
длина *негабарит. Функция добавления добавляет существующий фрагмент к новым данным. Создание
Функция создает новый чанк с заданными параметрами и добавляет его. Тип — 4-буквенный
название чанка.

8.2. куски в info_png
-----------------------

Структура LodePNGInfo содержит поля с неизвестным фрагментом. У него 3
буферы (каждый имеет размер), содержащие 3 типа неизвестных фрагментов:
те, которые идут перед куском PLTE, те, которые идут между PLTE
и фрагменты IDAT, а также те, которые идут после фрагментов IDAT.
Необходимо различать эти три случая, поскольку PNG
стандартные силы, чтобы сохранить порядок неизвестных фрагментов по сравнению с критическими
куски, но не навязывает никаких других правил упорядочивания.

info_png . unknown_chunks_data [0] — это фрагменты перед PLTE.
info_png . unknown_chunks_data [1] — это фрагменты после PLTE и перед IDAT.
info_png . unknown_chunks_data [2] — это фрагменты после IDAT.

Фрагменты в этих трех буферах можно перебирать и читать, используя один и тот же
способ, описанный в предыдущем подразделе.

При использовании декодера для декодирования PNG вы можете сохранить в нем все неизвестные фрагменты.
если вы установили параметры настройки. remember_unknown_chunks на 1. По умолчанию это
опция выключена (0).

Кодер всегда будет кодировать неизвестные фрагменты, хранящиеся в info_png.
Если вам нужно добавить определенный фрагмент, который не известен LodePNG, вы можете
используйте lodepng_chunk_append или lodepng_chunk_create для данных чанка в
info_png . unknown_chunks_data [х].

Чанки, известные LodePNG, не следует добавлять таким образом. Например. сделать
LodePNG добавьте чанк bKGD, установите для background_defined значение true и добавьте правильный
параметры там вместо этого.


9. compiler support
-------------------

Для компиляции не требуются никакие библиотеки, кроме текущей стандартной библиотеки C.
ЛодеПНГ. Для версии C++ сверху нужна только стандартная библиотека C++.
Добавьте в свой проект файлы lodepng.c (pp) и lodepng.h, включая
lodepng.h там, где это необходимо, и ваша программа сможет читать/записывать файлы PNG.

Он совместим с C90 и более поздними версиями, а также с C++03 и более поздними версиями.

Если важна производительность, используйте оптимизацию при компиляции! Для обоих
кодер и декодер, это имеет большое значение.

Убедитесь, что LodePNG скомпилирован тем же компилятором той же версии.
и с теми же настройками, что и остальная часть программы, или интерфейсы с
std::vectors и std::strings в C++ могут быть несовместимы.

CHAR_BITS должно быть 8 или выше, поскольку LodePNG использует для октетов беззнаковые символы.

*) gcc и g++

LodePNG разработан в gcc, поэтому этот компилятор поддерживается изначально. Это не дает
предупреждения с параметрами компилятора "-Wall -Wextra -pedantic -ansi", с gcc и g++
версия 4.7.1 для Linux, 32-битная и 64-битная.

*) Кланг

Полная поддержка и отсутствие предупреждений.

*) Мингв

Компилятор Mingw (порт gcc для Windows) должен полностью поддерживаться
ЛодеПНГ.

*) Visual Studio и Visual C++ Express Edition.

LodePNG не должен содержать предупреждений с уровнем предупреждений W4 . Два предупреждения отключены.
хотя с прагмами: предупреждение 4244 о неявных преобразованиях и предупреждение 4996.
где он хочет использовать нестандартную функцию fopen_s вместо стандартной функции C
fopen.

Visual Studio может захотеть, чтобы файлы «stdafx.h» были включены в каждый исходный файл и
выдает ошибку «неожиданный конец файла при поиске предварительно скомпилированного заголовка».
Это не стандартный C++ и не будет добавлен в стандартный LodePNG. Вы можете
отключите его для lodepng.cpp, только щелкнув его правой кнопкой мыши, Свойства, C/C++,
Предварительно скомпилированные заголовки и установите там значение «Не использовать предварительно скомпилированные заголовки».

NOTE: Modern versions of VS should be fully supported, but old versions, e.g.
VS6 , работа не гарантируется.

*) Компиляторы на Macintosh

Сообщается, что LodePNG работает как с gcc, так и с LLVM для Macintosh, как для
С и С++.

*) Другие компиляторы

Если у вас возникнут проблемы с каким-либо компилятором, дайте мне знать, и я могу
попробуйте исправить это, если компилятор современный и соответствует стандартам.


10. examples
------------

В этом примере декодера показано самое простое использование LodePNG. Более сложный
примеры можно найти на сайте LodePNG.

NOTE: these examples do not support wide-character filenames, you can use an
внешний метод для обработки таких файлов и кодирования или декодирования в памяти

10.1. пример декодера C++
-------------------------

#включить "lodepng.h"
#включить <iostream>

int main(int argc, char *argv[]) {
  const char* filename = argc > 1 ? argv[1] : "test.png";

  //загрузить и декодировать
  std::vector<unsigned char> image;
  беззнаковая ширина, высота;
  беззнаковая ошибка = lodepng::decode(изображение, ширина, высота, имя файла);

  //если есть ошибка, отобразите ее
  if(error) std::cout << "ошибка декодера " << error << ": " << lodepng_error_text (ошибка) << std::endl;

  //пиксели теперь находятся в векторном «изображении», 4 байта на пиксель, упорядоченном RGBARGBA..., используйте его как текстуру, нарисуйте,...
}

10.2. пример декодера C
-----------------------

#включить "lodepng.h"

int main(int argc, char *argv[]) {
  беззнаковая ошибка;
  изображение без знака char*;
  size_t ширина, высота;
  const char* filename = argc > 1 ? argv[1] : "test.png";

  error = lodepng_decode32_file(&image, &width, &height, filename);

  if(error) printf("Ошибка декодера %u: %s\n", error, lodepng_error_text (ошибка));

  / * use image here * /

  бесплатно (изображение);
  вернуть 0;
}

11. state settings reference
----------------------------

Краткий справочник некоторых настроек, которые можно установить в LodePNGState.

Для декодирования:

state.decoder.zlibsettings. ignore_adler32 : игнорировать контрольные суммы ADLER32.
state.decoder.zlibsettings. custom_ ...: использовать пользовательскую функцию надувания
состояние.декодер. ignore_crc : игнорировать контрольные суммы CRC.
состояние.декодер. ignore_critical: игнорировать неизвестные критические фрагменты.
состояние.декодер. ignore_end : игнорировать отсутствующий фрагмент IEND. Может произойти сбой, если это повреждение вызывает другие ошибки.
состояние.декодер. color_convert: преобразовать внутренний цвет PNG в выбранный.
состояние.декодер. read_text_chunks : читать ли фрагменты текстовых метаданных.
состояние.декодер. remember_unknown_chunks : читать ли неизвестные фрагменты
государство. info_raw .colortype: желаемый тип цвета для декодированного изображения.
государство. info_raw .bitlength: желаемая разрядность декодированного изображения.
государство. info_raw ....: дополнительные настройки цвета см. в структуре LodePNGColorMode.
государство. info_png ....: нет настроек для декодера, но есть вывод, см. структуру LodePNGInfo

Для кодирования:

state.encoder.zlibsettings.btype: отключите сжатие, установив для него значение 0.
state.encoder.zlibsettings. use_lz77: использовать LZ77 при сжатии.
state.encoder.zlibsettings.windowsize: настройка размера окна LZ77
state.encoder.zlibsettings.minmatch: настроить минимальную длину LZ77 для соответствия
state.encoder.zlibsettings.nicematch: настройка совпадения LZ77, где прекратить поиск
state.encoder.zlibsettings.lazymatching: попробуйте еще одно сопоставление LZ77
state.encoder.zlibsettings. custom_ ...: использовать пользовательскую функцию сдувания
состояние.энкодер. auto_convert: выберите оптимальный тип цвета PNG, если 0, используется info_png.
состояние.энкодер. filter_palette_zero : Стратегия фильтра PNG для палитры.
состояние.энкодер. filter_strategy : Стратегия фильтра PNG для кодирования с помощью
состояние.энкодер. force_palette: добавить палитру, даже если она не кодируется в одну
состояние.энкодер. add_id: добавить идентификатор и версию LodePNG в виде текстового фрагмента.
состояние.энкодер. text_compression: использовать сжатые текстовые фрагменты для метаданных.
государство. info_raw .colortype: тип цвета предоставленного вами необработанного входного изображения.
государство. info_raw .bitlength: разрядность исходного входного изображения, которое вы предоставляете.
государство. info_raw: дополнительные настройки цвета см. в структуре LodePNGColorMode.
государство. info_png .color.colortype: желаемый тип цвета, если auto_convert имеет значение false
государство. info_png .color.bitlength: желаемая разрядность, если auto_convert имеет значение false
государство. info_png .color....: дополнительные настройки цвета см. в структуре LodePNGColorMode.
государство. info_png ....: дополнительные настройки, связанные с PNG, см. в структуре LodePNGInfo.


12. changes
-----------

Номер версии LodePNG — это дата изменения, указанная в формате.
ггггммдд.

Некоторые изменения не имеют обратной совместимости. Они обозначаются знаком (!)
символ.

Здесь перечислены не все изменения, в истории коммитов на github указано больше:
https://github.com/lvandeve/lodepng

*) 10 апреля 2023 г.: более быстрая реализация CRC32, но с большей таблицей поиска.
*) 13 июня 2022 г.: добавлена поддержка чанка sBIT.
*) 09 января 2022 г.: незначительные улучшения скорости декодера.
*) 27 июня 2021 г.: добавлены предупреждения о том, что функции чтения/записи файлов не поддерживаются.
   имена файлов с широкими символами (поддержка не планируется, открытие файлов
   не является основной частью декодирования/декодирования PNG и зависит от платформы).
*) 17 октября 2020 г.: по умолчанию запрещено декодирование слишком больших фрагментов текста/icc.
*) 06 марта 2020 г.: упрощены некоторые операции распределения динамической памяти.
*) 12 января 2020 г.: (!) добавлен аргумент «конец» в lodepng_chunk_next, позволяющий корректно
   проверки переполнения.
*) 14 августа 2019 г.: декодирование примерно на 25 % быстрее благодаря справочным таблицам Хаффмана.
*) 15 июня 2019: (!) auto_choose_color API изменен (исправление: не использовать палитру
   если серый профиль ICC) и не ICC LodePNGColorProfile переименован в
   LodePNGColorStats.
*) 30 декабря 2018 г.: изменен только стиль кода: удалены символы новой строки перед открытием скобок.
*) 10 сен 2018: добавлен способ проверки фрагментов метаданных без полного декодирования.
*) 19 августа 2018: (!) Исправлен цветовой режим bKGD, который закодирован и использован
   индекс палитры в случае палитры.
*) 10 авг 2018: (!) добавлена поддержка фрагментов gAMA, chRM, sRGB и iCCP. Это
   изменения обратно совместимы, если только вы не полагались на unknown_chunks для них.
*) 11 июня 2018 г.: менее строгая проверка целочисленного переполнения размера пикселя
*) 14 января 2018 г.: разрешено опционально игнорировать еще несколько устранимых ошибок.
*) 17 сен 2017: исправлена утечка памяти в некоторых случаях ошибок ввода кодировщика.
*) 27 ноября 2016 г.: исправление ошибки автоматического определения цветовой модели серый+альфа.
*) 18 апреля 2016 г.: qsort изменен на пользовательскую стабильную сортировку (для платформ без qsort).
*) 09 апреля 2016 г.: исправлено обнаружение использования цветных клавиш и улучшена загрузка файлов (в пределах
   пределы чистого C90).
*) 08 декабря 2015 г.: Сделана функция load_file, возвращающая ошибку, если файл не может быть открыт.
*) 24 октября 2015: Исправление декодирования вывода палитры.
*) 18 апреля 2015 г.: Boundary PM вместо простого слияния пакетов для более быстрого кодирования.
*) 24 августа 2014 г.: перенесено на github.
*) 23 августа 2014 г.: Уменьшено ненужное использование памяти декодером.
*) 28 июня 2014 г.: удалена настройка fix_png, всегда поддерживается палитра OOB для
    простота. ColorProfile стал общедоступным.
*) 09 июня 2014 г.: Ускорено кодирование за счет исправления ошибки хеширования и дополнительной оптимизации нулей.
*) 22 декабря 2013 г.: Для оптимизации требуется мощность двух окон.
*) 15 апреля 2013: Исправлена ошибка с LAC_ALPHA и цветовой клавишей.
*) 25 марта 2013 г.: Добавлена дополнительная функция, позволяющая игнорировать некоторые ошибки PNG ( fix_png ).
*) 11 марта 2013: (!) Исправление с кастомной бесплатной версией. Изменено с «мой» на «lodepng_».
    префикс для пользовательских распределителей и сделал возможным с помощью нового #define
    используйте собственные в своем проекте без необходимости изменения кода lodepng.
*) 28 января 2013 г.: Исправлена ошибка с цветовой клавишей.
*) 27 октября 2012 г.: Улучшения в обработке ошибок длины ключевого слова фрагмента текста.
*) 8 октября 2012: (!) Добавлена новая стратегия фильтра (энтропия) и новый автоматический цветовой режим.
    (без палитры). Лучше выкачать древовидную кодировку. Новые настройки сжатия.
    Более быстрое преобразование цветов при декодировании. Немного внутренней чистки.
*) 23 сентября 2012 г.: Немного уменьшено количество предупреждений в Visual Studio.
*) 1 сентября 2012: (!) Удалены #define для предоставления пользовательских функций (де)сжатия.
    и вместо этого заставил его работать с указателями на функции.
*) 23 июня 2012 г.: Добавлены дополнительные стратегии фильтрации. Стало проще использовать пользовательское распределение
    и свободные функции и переключать #defines из флагов компилятора. Небольшие исправления.
*) 6 мая 2012: (!) Добавление пользовательских функций zlib/deflate стало более гибким.
*) 22 апр 2012: (!) Сделан интерфейс более единообразным, многое переименовано. Удален
    избыточные классы кодеков C++. Уменьшено количество структур. Все изменилось,
    но теперь, имхо, чище и функциональность осталась прежней. Также исправлено
    несколько ошибок и сократил код реализации. Сделал новые образцы.
*) 6 ноября 2011: (!) По умолчанию кодировщик теперь автоматически выбирает лучший
    Цветовая модель PNG и разрядность в зависимости от количества и типа цветов
    сырое изображение. Для этого autoLeaveOutAlphaChannel заменяется на auto_choose_color.
*) 9 октября 2011 г.: упрощенная реализация хэш-цепочки для кодировщика.
*) 8 сентября 2011 г.: ленивое сопоставление кодировщика lz77 вместо жадного сопоставления.
*) 23 августа 2011 г.: изменены параметры сжатия zlib после тестирования.
    Исправлена ошибка с эвристикой типа фильтра PNG, из-за которой она выбирает много
    лучшие (это весьма существенно). Настройка для экспериментального, медленного,
    Добавлен перебор поиска по типам фильтров PNG.
*) 17 августа 2011 г.: (!) изменены имена некоторых функций, связанных с C zlib.
*) 16 августа 2011: код стал менее широким (максимум 120 символов в строке).
*) 17 апреля 2011: чистка кода. Исправления ошибок. Преобразование низкого цвета в 16-битный на образец цвета.
*) 21 февраля 2011: исправлена компиляция для C90. Исправлена ​​компиляция с отключенными разделами.
*) 11 декабря 2010 г.: кодирование стало быстрее по предложению Питера Истмана.
    оптимизировать длинные последовательности нулей.
*) 13 ноября 2010 г.: добавлены LodePNG_InfoColor_hasPaletteAlpha и
    LodePNG_InfoColor_canHaveAlpha функционирует для удобства.
*) 7 ноября 2010 г.: добавлена функция LodePNG_error_text для получения описания кода ошибки.
*) 30 октября 2010: декодирование стало немного быстрее
*) 26 октября 2010 г.: (!) изменены имена некоторых функций и структур C (более единообразные).
     Реорганизовали документацию и порядок объявления в шапке.
*) 08 авг 2010: изменены только некоторые комментарии и внешние образцы.
*) 05 июля 2010: исправлена ошибка благодаря предупреждениям в новой версии gcc.
*) 14 марта 2010 г.: исправлена ошибка, из-за которой слишком много памяти выделялось для буферов символов.
*) 02 сентября 2008 г.: исправлена ошибка, из-за которой могло создаваться пустое дерево, которое могли создавать приложения Linux.
    прочитайте, игнорируя проблему, но приложения Windows не смогли.
*) 06 июня 2008 г.: добавлены дополнительные проверки ошибок в случаях нехватки памяти.
*) 26 апреля 2008 г.: добавлено еще несколько проверок здесь и там для большей безопасности.
*) 06 марта 2008: исправлен сбой при кодировании строк
*) 02 февраля 2008 г.: добавлена поддержка международных текстовых фрагментов (iTXt).
*) 23 января 2008: небольшие очистки и #defines для разделения кода на секции.
*) 20 января 2008 г.: поддержка неизвестных фрагментов, позволяющая использовать LodePNG в качестве редактора.
*) 18 января 2008 г.: в кодировщик и декодер добавлена поддержка фрагментов time и pHYs.
*) 17 января 2008: добавлена возможность кодировать и декодировать сжатые фрагменты zTXt.
    Также различные исправления, например, в коде выкачивания и заполнения битов.
*) 13 января 2008 г.: Добавлена возможность кодирования чересстрочных изображений Adam7. Улучшенный
    фильтрующий код кодера.
*) 07 января 2008: (!) изменен LodePNG на использование ISO C90 вместо C++. А
    Оболочка C++ вокруг этого обеспечивает интерфейс, почти идентичный предыдущему.
    Наличие чистого ISO C90 в LodePNG делает его более портативным. Код C и C++
    находятся вместе в этих файлах, но это работает как для компиляторов C, так и для C++.
*) 29 декабря 2007: (!) большинство целочисленных типов заменены на unsigned int + другие настройки.
*) 30 августа 2007 г.: исправлена ошибка, делающая этот продукт совместимым с Borland C++.
*) 09 августа 2007 г.: некоторые предупреждения VS2005 снова удалены.
*) 21 июля 2007 г.: код дефляции помещен в новое пространство имен отдельно от кода zlib.
*) 08 июня 2007: исправлена ошибка с 2- и 4-битным цветом и маленькими чересстрочными изображениями.
*) 04 июня 2007 г.: улучшена поддержка Visual Studio 2005: сбой при доступе
    исправлен недопустимый элемент std::vector [0] и удалены предупреждения уровня 3 и 4.
*) 02 июня 2007: заставил кодировщик добавлять тег с версией по умолчанию
*) 27 мая 2007 г.: коды zlib и png разделены (но все еще в одном файле),
    добавлены простые функции кодирования/декодера для более простых случаев использования
*) 19 мая 2007 г.: мелкие исправления, некоторая очистка кода, добавлена новая ошибка (ошибка 69),
    некоторые примеры перенесены отсюда в lodepng_examples.cpp
*) 12 мая 2007: исправлена ошибка декодирования палитры.
*) 24 апреля 2007 г.: изменена лицензия с BSD на лицензию zlib.
*) 11 марта 2007: очень простое дополнение: возможность кодировать фрагменты bKGD.
*) 04 марта 2007: (!) Исправления, связанные с фрагментами текста, и поддержка кодирования.
    палитрированные изображения PNG. Плюс небольшие изменения интерфейса с палитрой и текстами.
*) 03 марта 2007: Сделано более короткое динамическое кодирование Хаффмана с повторяющимися кодами.
    Исправлена ошибка, из-за которой конечный код блока имел длину 0 в дереве Хаффмана.
*) 26 февраля 2007 г.: реализовано сжатие Хаффмана с динамическими деревьями (BTYPE 2).
    и поддерживается кодировщиком, в результате чего на выходе получаются PNG-файлы меньшего размера.
*) 27 января 2007: Ускорили испытания Адлера-32, чтобы не тратить время.
*) 24 января 2007 г.: кодировщик выдал ошибку интерфейса. Добавлено преобразование цветов из любого
    от типа шкалы серого до 8-битной шкалы серого с альфа-каналом или без него.
*) 21 янв 2007: (!) Полностью изменил интерфейс. Это позволяет больше типов цвета
    для преобразования и более единообразен. Посмотрите руководство, как это работает сейчас.
*) 07 января 2007: Некоторые очистки и исправления, а также несколько изменений за последние дни:
    кодировать/декодировать пользовательские фрагменты текста, отдельные классы для zlib и deflate, а также
    наконец-то заставил декодер выдавать ошибки из-за неправильного Adler32 или Crc.
*) 01 января 2007 г.: Исправлена ошибка кодирования PNG с длиной менее 8 бит на канал.
*) 29 декабря 2006 г.: добавлена поддержка кодирования изображений без альфа-канала и
    очистили код, а также сделали некоторые части быстрее.
*) 28 декабря 2006: Добавлены «Настройки» в кодировщик.
*) 26 декабря 2006 г.: Кодер теперь выполняет кодирование LZ77 и создает файлы гораздо меньшего размера.
    Убрано некоторое дублирование кода в декодере. Исправлена ​​небольшая ошибка в примере.
*) 09 декабря 2006: (!) Поместил выходные параметры общедоступных функций в качестве первого параметра.
    Исправлена ошибка декодера с 16-битным цветом.
*) 15 октября 2006 г.: Изменена структура документации.
*) 09 октября 2006 г.: Добавлен класс кодировщика. Он кодирует допустимое изображение PNG из
    данный буфер изображения, однако на данный момент он не сжат.
*) 08 сентября 2006: (!) Изменен интерфейс с классом Decoder.
*) 30 июля 2006: (!) LodePNG_InfoPng , ширина и высота теперь извлекаются в разных
    путь. DecodePNG переименован в decodePNGGeneric.
*) 29 июля 2006: (!) Изменен интерфейс: информация об изображении теперь возвращается в виде
    структуру типа LodePNG::LodePNG_Info вместо вектора, что было немного неуклюже.
*) 28 июля 2006 г.: Очищен код и добавлены новые проверки ошибок.
    Исправлена терминология «сдувать» на «раздувать».
*) 23 июня 2006: Добавил пример SDL в документацию в шапке, это
    example позволяет легко отладить, отображая PNG и его прозрачность.
*) 22 июня 2006: (!) Изменен способ получения значения ошибки. Добавлено
    loadFile для удобства. Ускорено декодирование PNG32.
*) 21 июня 2006: (!) Изменен тип информационного вектора на беззнаковый.
    Изменено положение палитры в информационном векторе. Исправлена важная ошибка, которая
    произошло с PNG-файлами с несжатым блоком.
*) 16 июня 2006 г.: Внутренне изменен unsigned на unsigned, где
    необходимо, и выполнил некоторые оптимизации.
*) 07 июня 2006: (!) Переименовал функции в decodePNG и поместил их
    в пространстве имен LodePNG. Изменен порядок параметров. Переписал
    документация в шапке. Переименованы файлы в lodepng.cpp и lodepng.h.
*) 22 апреля 2006 г.: Оптимизирован и улучшен код.
*) 07 сентября 2005: (!) Интерфейс изменен на std::vector.
*) 12 августа 2005 г.: первый выпуск (только C++, декодер).


13. contact information
-----------------------

Не стесняйтесь обращаться ко мне с предложениями, проблемами, комментариями, ... относительно
ЛодеПНГ. Если вы столкнулись с образом PNG, который не работает должным образом с этим
декодер, пришлите его, и я воспользуюсь им, чтобы найти и устранить проблему.

Мой адрес электронной почты (сопоставьте учетную запись и домен вместе с символом @):
Domain: gmail dot com.
Account: lode dot vandevenne.


Copyright (c) 2005-2022 Lode Vandevenne
*/
