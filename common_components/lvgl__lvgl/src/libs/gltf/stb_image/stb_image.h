/* stb_image - v2.30 - public domain image loader - http://nothings.org/stb
                                  никакая гарантия не подразумевается; используйте на свой страх и риск

   Сделайте это:
      #определить STB_IMAGE_IMPLEMENTATION
   прежде чем включать этот файл в *один* файл C или C++ для создания реализации.

   // то есть это должно выглядеть так:
   #включить...
   #включить...
   #включить...
   #определить STB_IMAGE_IMPLEMENTATION
   #включить "stb_image.h"

   Вы можете использовать #define STBI_ASSERT (x) перед #include, чтобы избежать использования assert.h .
   И #define STBI_MALLOC , STBI_REALLOC и STBI_FREE, чтобы избежать использования malloc,realloc,free.


   QUICK NOTES:
      В первую очередь представляет интерес для разработчиков игр и других людей, которые могут
          избегайте проблемных изображений и используйте только тривиальный интерфейс

      JPEG базовая и прогрессивная (12 бит/канал/арифметика не поддерживаются, как и стандартная библиотека IJG)
      PNG 1/2/4/8/16 бит на канал

      TGA (не уверен, какое подмножество, если подмножество)
      BMP не 1bpp, не RLE
      PSD (только составной вид, без дополнительных каналов, 8/16 бит на канал)

      GIF (*comp всегда сообщает как 4-канальный)
      HDR (формат излучения RGBE)
      PIC (Softimage PIC)
      PNM (только двоичные файлы PPM и PGM)

      Анимированному GIF по-прежнему нужен правильный API, но есть один из способов сделать это:
          http://gist.github.com/urraka/685d9a6340b26b830d49

      - decode from memory or through FILE (define STBI_NO_STDIO to remove code)
      - decode from arbitrary I/O callbacks
      - SIMD acceleration on x86/x64 (SSE2) and ARM (NEON)

   Полная документация в разделе «DOCUMENTATION» ниже.


LICENSE

  Информацию о лицензии смотрите в конце файла.

RECENT REVISION HISTORY:

      2.30 (31 мая 2024 г.) избегайте ошибочного предупреждения gcc
      2.29 (2023-05-xx) оптимизация
      2.28 (29 января 2023 г.) множество исправлений ошибок, ошибок безопасности и просто куча всего.
      2.27 (11 июля 2021 г.) документ stbi_info улучшен, поддержка 16-битного PNM, исправления ошибок
      2.26 (13 июля 2020 г.) множество мелких исправлений.
      2.25 (02.02.2020) исправлены предупреждения
      2.24 (02.02.2020) исправлены предупреждения; локальные для потока failure_reason и flip_vertically
      2.23 (11 августа 2019 г.) исправлено предупреждение статического анализа clang.
      2.22 (04.03.2019) исправления gif, исправления предупреждений
      2.21 (25 февраля 2019 г.) исправлена опечатка в комментарии
      2.20 (07.02.2019) поддерживает имена файлов utf8 в Windows; исправить предупреждения и ifdef платформы
      2.19 (11 февраля 2018 г.) исправлено предупреждение
      2.18 (30 января 2018 г.) исправлены предупреждения.
      2.17 (29 января 2018 г.) исправление ошибки, 1-битный BMP, 16-битный запрос, исправление предупреждений
      2.16 (23 июля 2017 г.) все функции имеют 16-битные варианты; оптимизации; исправления ошибок
      2.15 (18.03.2017) исправление png-1,2,4; все файлы Imagenet в формате JPG; нет обнаружения времени выполнения SSE на GCC
      2.14 (03.03.2017) удален устаревший STBI_JPEG_OLD ; исправления для изображений Imagenet JPG
      2.13 (04.12.2016) экспериментальный 16-битный API, пока только для PNG; исправления
      2.12 (02.04.2016) исправлена опечатка в исправлении 2.11 PSD, приводившая к сбоям
      2.11 (02.04.2016) 16-битный PNGS ; включить SSE2 в x64 без gcc
                         RGB -формат JPEG ; убрать белое матирование в PSD ;
                         выделять в стеке большие структуры;
                         правильное количество каналов для PNG и BMP
      2.10 (22 января 2016 г.) избегать предупреждений, появившихся в версии 2.09.
      2.09 (16.01.2016) 16-битный TGA ; комментарии в файлах PNM;  STBI_REALLOC_SIZED

   Полную историю изменений смотрите в конце файла.


 =========================== Авторы ========================

 Форматы изображений Расширения, возможности
    Шон Барретт (jpeg, png, bmp) Джетро Лауха (stbi_info)
    Николас Шульц (hdr, psd) Мартин "SpartanJ" Голини ( stbi_info )
    Джонатан Даммер (tga) Джеймс «moose2000» Браун (iPhone PNG)
    Жан-Марк Лиенер (гифка) Бен «Диш» Венгер (обратные вызовы io)
    Том Седдон (рис.) Омар Корнут (1/2/4-битный PNG)
    Тэтчер Ульрих (psd) Николя Гиймо (вертикальный флип)
    Кен Миллер (pgm, ppm) Ричард Миттон (16-битный PSD )
    github:urraka (анимационный gif) Джунггон Ким (комментарии PNM)
    Кристофер Форсет (анимационный gif) Дэниел Гибсон (16-бит TGA)
                                           носки-лиса (16-битный PNG )
                                           Джереми Савики (обрабатывает все файлы JPG ImageNet)
 Оптимизации и исправления Михаил Морозов (1-бит BMP )
    Фабиан «ryg» Гизен Анаэль Сегецци (16-битный запрос)
    Арсений Капулькин Саймон Бреусс (16-бит PNM )
    Джон-Марк Аллен
    Кармело Дж. Фдез-Агера

 Исправления ошибок и предупреждений
    Марк Леблан Дэвид Ву Гийом Джордж Мартинс Можейко
    Крисфер Ллойд Джерри Янссон Джозеф Томсон Блажей Дариуш Рошковски
    Фил Джордан Дэйв Мур Рой Элтэм
    Хаяки Сайто Натан Рид Вон Чун
    Люк Грэм Йохан Дюпарк Ник Веригакис, сообщество Horde3D
    Томас Руф Ронни Шевалье github:rlyeh
    Янез Земва Джон Варфоломей Михал Сихон github:romigrou
    Джонатан Блоу Кен Хамада Теро Ханнинен github:svdijk
    Евгений Голушков Лоран Гомила Корт Страттон github:snagar
    Аруэльен Пошевиль Серджио Гонсалес Тибо Рей github:Zelex
    Касс Эверитт Раймонд Барбьеро github:grim210
    Поль Дюбуа Энгин Манап Альдо Калкикондор github: sammyhw
    Филипп Виземанн Дейл Вейлер Ориол Феррер Месия github:phprus
    Джош Тобин Нил Бикфорд Мэтью Греган github:poppolopoppo
    Джулиан Рашке Грегори Маллен Кристиан Флуазан и github: darealshinji
    Бальдур Карлссон Кевин Шмидт JR Смит github: Michaelangel007
                            Брэд Вайнбергер Матвей Черевко github:mosra
    Лука Сас Александр Веселов Зак Миддлтон [в резерве]
    Райан С. Гордон [зарезервировано] [зарезервировано]
                     DO NOT ADD YOUR NAME HERE

                     Джеко Диркс

  Чтобы добавить свое имя в титры, выберите случайное пустое место посередине и заполните его.
  80% конфликтов слияний в PR-записях stb происходят из-за того, что люди добавляют свое имя в конце.
  кредитов.
*/

#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STBI_INCLUDE_STB_IMAGE_H

// DOCUMENTATION
//
// Ограничения:
//    - no 12-bit-per-channel JPEG
//    - no JPEGs with arithmetic coding
//    - GIF always returns *comp=4
//
// Базовое использование (об использовании HDR см. обсуждение HDR ниже):
//    интервал х, у, п;
//    unsigned char *data = stbi_load (имя файла, &x, &y, &n, 0);
//    // ... обрабатываем данные, если не NULL ...
//    // ... x = ширина, y = высота, n = # 8-битные компоненты на пиксель ...
//    // ... замените '0' на '1'..'4', чтобы получить такое же количество компонентов на пиксель
//    // ... но 'n' всегда будет тем числом, которое было бы, если бы вы сказали 0
//    stbi_image_free (данные);
//
// Стандартные параметры:
//    int *x — выводит ширину изображения в пикселях
//    int *y — выводит высоту изображения в пикселях
//    int * channels_in_file — выводит количество компонентов изображения в файле изображения.
//    int desired_channels -- если не ноль, количество компонентов изображения, запрошенных в результате
//
// Возвращаемое значение от загрузчика изображений — это «беззнаковый символ *», который указывает
// к данным пикселей или NULL в случае сбоя выделения или если изображение
// поврежден или недействителен. Данные пикселей состоят из *y строк развертки из *x пикселей,
// при этом каждый пиксель состоит из N чередующихся 8-битных компонентов; первый
// пиксель, на который указывает, находится в верхнем левом углу изображения. Между ними нет прокладки
// развертки изображения или между пикселями, независимо от формата. Количество
// компоненты N равны ' desired_channels ', если desired_channels не равен нулю, или
// * channels_in_file иначе. Если desired_channels не равен нулю,
// * channels_in_file содержит то же количество компонентов, что и _would_.
// вывод в противном случае. Например. если вы установите desired_channels на 4, вы всегда будете
// получите вывод RGBA, но вы можете проверить * channels_in_file, чтобы убедиться, что это тривиально
// непрозрачный, потому что, например в исходном изображении было всего 3 канала.
//
// Выходное изображение с N компонентами имеет чередующиеся следующие компоненты:
// в таком порядке в каждом пикселе:
//
//     N = компоненты #comp
//       1 серый
//       2 серых, альфа
//       3 красных, зеленых, синих
//       4 красных, зеленых, синих, альфа
//
// Если по какой-либо причине загрузка изображения не удалась, возвращаемое значение будет NULL ,
// и *x, *y, * channels_in_file останутся без изменений. Функция
// stbi_failure_reason() можно запросить для получения очень краткой информации для конечного пользователя.
// недружелюбное объяснение того, почему загрузка не удалась. Определите STBI_NO_FAILURE_STRINGS
// чтобы вообще не компилировать эти строки, и STBI_FAILURE_USERMSG, чтобы немного получить
// более удобные для пользователя.
//
// Изображения PNG , BMP , GIF и PIC с палитрой автоматически депалитируются.
//
// Чтобы запросить ширину, высоту и количество компонентов изображения без необходимости
// декодировать полный файл, вы можете использовать семейство функций stbi_info:
//
//   int х, у, п, ок;
//   ok = stbi_info(filename, &x, &y, &n);
//   // возвращает ok=1 и устанавливает x, y, n, если формат изображения поддерживается,
//   // 0 иначе.
//
// Обратите внимание, что stb_image повсеместно использует целые числа в своем публичном API для размеров.
// включая размеры буферов памяти. Теперь это часть API и, следовательно,
// трудно изменить, не вызывая поломки. В результате различные изображения
// все загрузчики имеют определенные ограничения на размер изображения; они несколько отличаются
// по формату, но обычно сводится к размеру чуть менее 2 ГБ или чуть меньше
// 1 ГБ. Если декодированное изображение будет больше этого значения, декодирование stb_image
// потерпит неудачу.
//
// Кроме того, stb_image отклоняет файлы изображений, у которых есть какие-либо из их
// для размеров установлено большее значение, чем настраиваемое STBI_MAX_DIMENSIONS ,
// по умолчанию 2**24 = 16777216 пикселей. Из-за вышеуказанного ограничения памяти,
// единственный способ правильно загрузить изображение с такими размерами
// заключается в том, что у него довольно экстремальное соотношение сторон. В любом случае,
// предположение здесь заключается в том, что такие большие изображения, скорее всего, будут искажены.
// или злонамеренный. Если вам нужно загрузить изображение с отдельными размерами
// больше этого размера, и он по-прежнему соответствует общему ограничению размера, вы можете
// #define STBI_MAX_DIMENSIONS самостоятельно стать чем-то большим.
//
// ===========================================================================
//
// UNICODE:
//
//   Если вы компилируете для Windows и хотите использовать имена файлов в Юникоде, скомпилируйте
//   с
//       #define STBI_WINDOWS_UTF8
//   и передавать имена файлов в кодировке utf8. Позвоните stbi_convert_wchar_to_utf8, чтобы конвертировать
//   Имена файлов Windows wchar_t в utf8.
//
// ===========================================================================
//
// Философия
//
// stb библиотеки разработаны со следующими приоритетами:
//
//    1. easy to use
//    2. easy to maintain
//    3. good performance
//
// Иногда я позволяю «хорошей производительности» иметь приоритет над «простотой в обслуживании»,
// и для лучшей производительности я могу предоставить менее простые в использовании API, которые дают более высокую производительность.
// производительность, в дополнение к простым в использовании. Тем не менее, это важно
// иметь в виду, что с точки зрения вас, клиента этой библиотеки,
// все, что вас волнует, это №1 и №3, а библиотеки stb DO NOT прежде всего подчеркивают №3.
//
// Некоторые второстепенные приоритеты вытекают непосредственно из первых двух, некоторые из которых
// укажите более явные причины, по которым производительность не может быть подчеркнута.
//
//    - Portable ("ease of use")
//    - Small source code footprint ("easy to maintain")
//    - No dependencies ("ease of use")
//
// ===========================================================================
//
// Обратные вызовы ввода-вывода
//
// Обратные вызовы ввода-вывода позволяют вам читать из произвольных источников, например упакованных
// файлы или какой-либо другой источник. Данные, считанные из обратных вызовов, обрабатываются
// через небольшой внутренний буфер (на данный момент 128 байт), чтобы попытаться уменьшить
// накладные расходы.
//
// Три функции, которые вы должны определить: «чтение» (читает несколько байтов данных),
// «skip» (пропускает некоторые байты данных), «eof» (сообщает, находится ли поток в конце).
//
// ===========================================================================
//
// Поддержка SIMD
//
// Декодер JPEG попытается автоматически использовать ядра SIMD на x86, когда
// поддерживается компилятором. Для поддержки ARM Neon вы должны явно
// запросите это.
//
// (Старый самодельный SIMD API больше не поддерживается в текущей
// код.)
//
// На x86 SSE2 будет автоматически использоваться, если он доступен, в зависимости от времени выполнения.
// тест; в противном случае общие версии C используются как запасной вариант. На целях ARM,
// типичный путь — создание отдельных сборок для устройств NEON и устройств, отличных от NEON.
// (по крайней мере, это справедливо для iOS и Android). Поэтому поддержка NEON
// переключается флагом сборки: определите STBI_NEON, чтобы получить циклы NEON.
//
// Если по какой-то причине вы не хотите использовать какой-либо код SIMD или если
// у вас есть проблемы с его компиляцией, вы можете полностью отключить его,
// определение STBI_NO_SIMD .
//
// ===========================================================================
//
// Поддержка изображений HDR (отключите, определив STBI_NO_HDR )
//
// stb_image поддерживает загрузку изображений HDR в целом, и в настоящее время Radiance
// . В частности, формат файла HDR. Вы по-прежнему можете загрузить любой файл через существующий
// интерфейс; если вы попытаетесь загрузить файл HDR, он будет автоматически переназначен
// до LDR , предполагая гамму 2,2 и произвольный масштабный коэффициент, равный по умолчанию 1;
// обе эти константы можно перенастроить через этот интерфейс:
//
//     stbi_hdr_to_ldr_gamma (2.2f);
//     stbi_hdr_to_ldr_scale (1.0f);
//
// (обратите внимание: не используйте константы _inverse_; stbi_image инвертирует их.
// соответственно).
//
// Кроме того, имеется новый параллельный интерфейс для загрузки файлов в виде
// (линейное) плавает, чтобы сохранить полный динамический диапазон:
//
//    float *data = stbi_loadf (имя файла, &x, &y, &n, 0);
//
// Если вы загрузите изображения LDR через этот интерфейс, эти изображения будут
// быть повышен до значений с плавающей запятой, выполнить обратное значение
// константы, соответствующие приведенному выше:
//
//     stbi_ldr_to_hdr_scale (1.0f);
//     stbi_ldr_to_hdr_gamma (2.2f);
//
// Наконец, учитывая имя файла (или открытого файла или блока памяти — см. заголовок
// файл для получения подробной информации), содержащий данные изображения, вы можете запросить «наиболее
// соответствующий» интерфейс (то есть, является ли изображение HDR или
// нет), используя:
//
//     stbi_is_hdr (символ *имя файла);
//
// ===========================================================================
//
// Поддержка iPhone PNG:
//
// Мы дополнительно поддерживаем преобразование PNG-файлов в формате iPhone (которые хранят
// предварительно умноженные BGRA) обратно в RGB, даже если они закодированы внутри
// по-другому. Чтобы включить это преобразование, позвоните
// stbi_convert_iphone_png_to_rgb (1).
//
// Также вызовите stbi_set_unpremultiply_on_load (1), чтобы принудительно разделить
// пиксель для удаления любой предварительно умноженной альфа *только*, если файл изображения явно
// говорит, что есть предварительно умноженные данные (пока это происходит только с изображениями iPhone,
// и только если на iPhone включена обработка преобразования в RGB).
//
// ===========================================================================
//
// ADDITIONAL CONFIGURATION
//
//  - You can suppress implementation of any of the decoders to reduce
//    ваш код, созданный #defining, одним или несколькими из следующих действий
//    символы перед созданием реализации.
//
//        STBI_NO_JPEG
//        STBI_NO_PNG
//        STBI_NO_BMP
//        STBI_NO_PSD
//        STBI_NO_TGA
//        STBI_NO_GIF
//        STBI_NO_HDR
//        STBI_NO_PIC
//        STBI_NO_PNM (.ppm и .pgm)
//
//  - You can request *only* certain decoders and suppress all other ones
//    (это будет более совместимо с будущими версиями, поскольку добавлены новые декодеры
//    не требует их явного отключения):
//
//        STBI_ONLY_JPEG
//        STBI_ONLY_PNG
//        STBI_ONLY_BMP
//        STBI_ONLY_PSD
//        STBI_ONLY_TGA
//        STBI_ONLY_GIF
//        STBI_ONLY_HDR
//        STBI_ONLY_PIC
//        STBI_ONLY_PNM (.ppm и .pgm)
//
//   - If you use STBI_NO_PNG (or _ONLY_ without PNG), and you still
//     хочу, чтобы декодер zlib был доступен, #define STBI_SUPPORT_ZLIB
//
//  - If you define STBI_MAX_DIMENSIONS, stb_image will reject images greater
//    больше этого размера (по ширине или высоте) без дальнейшей обработки.
//    Это сделано для того, чтобы программы в дикой природе могли устанавливать верхнюю границу, чтобы предотвратить
//    атаки типа «отказ в обслуживании» на ненадежные данные, поскольку можно создать
//    действительный образ гигантских размеров и заставить stb_image выделить
//    огромный блок памяти и тратить непропорционально много времени на его декодирование. Автор
//    по умолчанию установлено значение (1 << 24), что равно 16777216, но это все равно
//    очень большой.

#ifndef STBI_NO_STDIO
    #include <stdio.h>
#endif // STBI_NO_STDIO

#define STBI_VERSION 1

enum {
    STBI_default = 0, // используется только для desired_channels

    STBI_grey       = 1,
    STBI_grey_alpha = 2,
    STBI_rgb        = 3,
    STBI_rgb_alpha  = 4
};

#include <stdlib.h>
typedef unsigned char stbi_uc;
typedef unsigned short stbi_us;

#ifdef __cplusplus
extern "C" {
#endif

#ifndef STBIDEF
#ifdef STB_IMAGE_STATIC
#define STBIDEF static
#else
#define STBIDEF extern
#endif
#endif

//////////////////////////////////////////////////////////////////////////////
//
// PRIMARY API — работает с изображениями любого типа.
//

//
// загрузить изображение по имени файла, открытому файлу или буферу памяти
//

typedef struct {
    int (*read)(void * user, char * data,
                int size);      // заполните «данные» байтами «размера».  вернуть количество фактически прочитанных байтов
    void (*skip)(void * user, int
                 n);                     // пропустить следующие 'n' байты или 'отменить' последние -n байты, если отрицательный результат
    int (*eof)(void * user);                              // возвращает ненулевое значение, если мы находимся в конце файла/данных
} stbi_io_callbacks;

////////////////////////////////////
//
// 8-битный интерфейс на канал
//

STBIDEF stbi_uc * stbi_load_from_memory(stbi_uc           const * buffer, int len, int * x, int * y,
                                        int * channels_in_file, int desired_channels);
STBIDEF stbi_uc * stbi_load_from_callbacks(stbi_io_callbacks const * clbk, void * user, int * x, int * y,
                                           int * channels_in_file, int desired_channels);

#ifndef STBI_NO_STDIO
STBIDEF stbi_uc * stbi_load(char const * filename, int * x, int * y, int * channels_in_file, int desired_channels);
STBIDEF stbi_uc * stbi_load_from_file(FILE * f, int * x, int * y, int * channels_in_file, int desired_channels);
// для stbi_load_from_file указатель файла остается сразу после изображения
#endif

#ifndef STBI_NO_GIF
STBIDEF stbi_uc * stbi_load_gif_from_memory(stbi_uc const * buffer, int len, int ** delays, int * x, int * y, int * z,
                                            int * comp, int req_comp);
#endif

#ifdef STBI_WINDOWS_UTF8
STBIDEF int stbi_convert_wchar_to_utf8(char * buffer, size_t bufferlen, const wchar_t * input);
#endif

////////////////////////////////////
//
// 16-битный интерфейс
//

STBIDEF stbi_us * stbi_load_16_from_memory(stbi_uc const * buffer, int len, int * x, int * y, int * channels_in_file,
                                           int desired_channels);
STBIDEF stbi_us * stbi_load_16_from_callbacks(stbi_io_callbacks const * clbk, void * user, int * x, int * y,
                                              int * channels_in_file, int desired_channels);

#ifndef STBI_NO_STDIO
STBIDEF stbi_us * stbi_load_16(char const * filename, int * x, int * y, int * channels_in_file, int desired_channels);
STBIDEF stbi_us * stbi_load_from_file_16(FILE * f, int * x, int * y, int * channels_in_file, int desired_channels);
#endif

////////////////////////////////////
//
// интерфейс с плавающей запятой на канал
//
#ifndef STBI_NO_LINEAR
STBIDEF float * stbi_loadf_from_memory(stbi_uc const * buffer, int len, int * x, int * y, int * channels_in_file,
                                       int desired_channels);
STBIDEF float * stbi_loadf_from_callbacks(stbi_io_callbacks const * clbk, void * user, int * x, int * y,
                                          int * channels_in_file, int desired_channels);

#ifndef STBI_NO_STDIO
STBIDEF float * stbi_loadf(char const * filename, int * x, int * y, int * channels_in_file, int desired_channels);
STBIDEF float * stbi_loadf_from_file(FILE * f, int * x, int * y, int * channels_in_file, int desired_channels);
#endif
#endif

#ifndef STBI_NO_HDR
STBIDEF void   stbi_hdr_to_ldr_gamma(float gamma);
STBIDEF void   stbi_hdr_to_ldr_scale(float scale);
#endif // STBI_NO_HDR

#ifndef STBI_NO_LINEAR
STBIDEF void   stbi_ldr_to_hdr_gamma(float gamma);
STBIDEF void   stbi_ldr_to_hdr_scale(float scale);
#endif // STBI_NO_LINEAR

// stbi_is_hdr всегда определен, но всегда возвращает false, если STBI_NO_HDR
STBIDEF int    stbi_is_hdr_from_callbacks(stbi_io_callbacks const * clbk, void * user);
STBIDEF int    stbi_is_hdr_from_memory(stbi_uc const * buffer, int len);
#ifndef STBI_NO_STDIO
STBIDEF int      stbi_is_hdr(char const * filename);
STBIDEF int      stbi_is_hdr_from_file(FILE * f);
#endif // STBI_NO_STDIO


// получить краткую причину неудачи VERY
// на большинстве компиляторов (и современных основных компиляторах ALL) это потокобезопасно
STBIDEF const char * stbi_failure_reason(void);

// освободите загруженное изображение -- это просто free()
STBIDEF void     stbi_image_free(void * retval_from_stbi_load);

// получить размеры и компоненты изображения без полного декодирования
STBIDEF int      stbi_info_from_memory(stbi_uc const * buffer, int len, int * x, int * y, int * comp);
STBIDEF int      stbi_info_from_callbacks(stbi_io_callbacks const * clbk, void * user, int * x, int * y, int * comp);
STBIDEF int      stbi_is_16_bit_from_memory(stbi_uc const * buffer, int len);
STBIDEF int      stbi_is_16_bit_from_callbacks(stbi_io_callbacks const * clbk, void * user);

#ifndef STBI_NO_STDIO
STBIDEF int      stbi_info(char const * filename,     int * x, int * y, int * comp);
STBIDEF int      stbi_info_from_file(FILE * f,                  int * x, int * y, int * comp);
STBIDEF int      stbi_is_16_bit(char const * filename);
STBIDEF int      stbi_is_16_bit_from_file(FILE * f);
#endif



// для форматов изображений, в которых явно указано, что у них есть предварительное умножение альфа,
// мы просто возвращаем цвета, хранящиеся в файле. установите этот флаг для принудительного
// непредумножение. результаты не определены, если переполнение без предварительного умножения.
STBIDEF void stbi_set_unpremultiply_on_load(int flag_true_if_should_unpremultiply);

// указать, следует ли нам обрабатывать изображения iPhone обратно в канонический формат,
// или просто передать их «как есть»
STBIDEF void stbi_convert_iphone_png_to_rgb(int flag_true_if_should_convert);

// переверните изображение по вертикали, чтобы первый пиксель в выходном массиве был внизу слева
STBIDEF void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip);

// как указано выше, но применяется только к изображениям, загруженным в поток, вызывающий функцию
// эта функция доступна только в том случае, если ваш компилятор поддерживает локальные переменные потока;
// вызов его не сможет связать, если ваш компилятор этого не сделает
STBIDEF void stbi_set_unpremultiply_on_load_thread(int flag_true_if_should_unpremultiply);
STBIDEF void stbi_convert_iphone_png_to_rgb_thread(int flag_true_if_should_convert);
STBIDEF void stbi_set_flip_vertically_on_load_thread(int flag_true_if_should_flip);

// Клиент ZLIB — используется PNG, доступен для других целей.

STBIDEF char * stbi_zlib_decode_malloc_guesssize(const char * buffer, int len, int initial_size, int * outlen);
STBIDEF char * stbi_zlib_decode_malloc_guesssize_headerflag(const char * buffer, int len, int initial_size,
                                                            int * outlen, int parse_header);
STBIDEF char * stbi_zlib_decode_malloc(const char * buffer, int len, int * outlen);
STBIDEF int   stbi_zlib_decode_buffer(char * obuffer, int olen, const char * ibuffer, int ilen);

STBIDEF char * stbi_zlib_decode_noheader_malloc(const char * buffer, int len, int * outlen);
STBIDEF int   stbi_zlib_decode_noheader_buffer(char * obuffer, int olen, const char * ibuffer, int ilen);


#ifdef __cplusplus
}
#endif

//
//
//// конец файла заголовка //////////////////////////////////////////////////////
#endif // STBI_INCLUDE_STB_IMAGE_H

#ifdef STB_IMAGE_IMPLEMENTATION

#if defined(STBI_ONLY_JPEG) || defined(STBI_ONLY_PNG) || defined(STBI_ONLY_BMP) \
    || defined(STBI_ONLY_TGA) || defined(STBI_ONLY_GIF) || defined(STBI_ONLY_PSD) \
    || defined(STBI_ONLY_HDR) || defined(STBI_ONLY_PIC) || defined(STBI_ONLY_PNM) \
    || defined(STBI_ONLY_ZLIB)
    #ifndef STBI_ONLY_JPEG
        #define STBI_NO_JPEG
    #endif
    #ifndef STBI_ONLY_PNG
        #define STBI_NO_PNG
    #endif
    #ifndef STBI_ONLY_BMP
        #define STBI_NO_BMP
    #endif
    #ifndef STBI_ONLY_PSD
        #define STBI_NO_PSD
    #endif
    #ifndef STBI_ONLY_TGA
        #define STBI_NO_TGA
    #endif
    #ifndef STBI_ONLY_GIF
        #define STBI_NO_GIF
    #endif
    #ifndef STBI_ONLY_HDR
        #define STBI_NO_HDR
    #endif
    #ifndef STBI_ONLY_PIC
        #define STBI_NO_PIC
    #endif
    #ifndef STBI_ONLY_PNM
        #define STBI_NO_PNM
    #endif
#endif

#if defined(STBI_NO_PNG) && !defined(STBI_SUPPORT_ZLIB) && !defined(STBI_NO_ZLIB)
    #define STBI_NO_ZLIB
#endif


#include <stdarg.h>
#include <stddef.h> // ptrdiff_t на OS X
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#if !defined(STBI_NO_LINEAR) || !defined(STBI_NO_HDR)
    #include <math.h>  // ldexp, пау
#endif

#ifndef STBI_NO_STDIO
    #include <stdio.h>
#endif

#ifndef STBI_ASSERT
    #include <assert.h>
    #define STBI_ASSERT(x) assert(x)
#endif

#ifdef __cplusplus
    #define STBI_EXTERN extern "C"
#else
    #define STBI_EXTERN extern
#endif


#ifndef _MSC_VER
    #ifdef __cplusplus
        #define stbi_inline inline
    #else
        #define stbi_inline
    #endif
#else
    #define stbi_inline __forceinline
#endif

#ifndef STBI_NO_THREAD_LOCALS
    #if defined(__cplusplus) &&  __cplusplus >= 201103L
        #define STBI_THREAD_LOCAL       thread_local
    #elif defined(__GNUC__) && __GNUC__ < 5
        #define STBI_THREAD_LOCAL       __thread
    #elif defined(_MSC_VER)
        #define STBI_THREAD_LOCAL       __declspec(thread)
    #elif defined (__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
        #define STBI_THREAD_LOCAL       _Thread_local
    #endif

    #ifndef STBI_THREAD_LOCAL
        #if defined(__GNUC__)
            #define STBI_THREAD_LOCAL       __thread
        #endif
    #endif
#endif

#if defined(_MSC_VER) || defined(__SYMBIAN32__)
    typedef unsigned short stbi__uint16;
    typedef   signed short stbi__int16;
    typedef unsigned int   stbi__uint32;
    typedef   signed int   stbi__int32;
#else
    #include <stdint.h>
    typedef uint16_t stbi__uint16;
    typedef int16_t  stbi__int16;
    typedef uint32_t stbi__uint32;
    typedef int32_t  stbi__int32;
#endif

// должен выдать ошибку компилятора, если размер неправильный
typedef unsigned char validate_uint32[sizeof(stbi__uint32) == 4 ? 1 : -1];

#ifdef _MSC_VER
    #define STBI_NOTUSED(v)  (void)(v)
#else
    #define STBI_NOTUSED(v)  (void)sizeof(v)
#endif

#ifdef _MSC_VER
    #define STBI_HAS_LROTL
#endif

#ifdef STBI_HAS_LROTL
    #define stbi_lrot(x,y)  _lrotl(x,y)
#else
    #define stbi_lrot(x,y)  (((x) << (y)) | ((x) >> (-(y) & 31)))
#endif

#if defined(STBI_MALLOC) && defined(STBI_FREE) && (defined(STBI_REALLOC) || defined(STBI_REALLOC_SIZED))
    // ok
#elif !defined(STBI_MALLOC) && !defined(STBI_FREE) && !defined(STBI_REALLOC) && !defined(STBI_REALLOC_SIZED)
    // ok
#else
    #error "Must define all or none of STBI_MALLOC, STBI_FREE, and STBI_REALLOC (or STBI_REALLOC_SIZED)."
#endif

#ifndef STBI_MALLOC
    #define STBI_MALLOC(sz)           malloc(sz)
    #define STBI_REALLOC(p,newsz)     realloc(p,newsz)
    #define STBI_FREE(p)              free(p)
#endif

#ifndef STBI_REALLOC_SIZED
    #define STBI_REALLOC_SIZED(p,oldsz,newsz) STBI_REALLOC(p,newsz)
#endif

// Обнаружение x86/x64
#if defined(__x86_64__) || defined(_M_X64)
    #define STBI__X64_TARGET
#elif defined(__i386) || defined(_M_IX86)
    #define STBI__X86_TARGET
#endif

#if defined(__GNUC__) && defined(STBI__X86_TARGET) && !defined(__SSE2__) && !defined(STBI_NO_SIMD)
    // gcc не поддерживает встроенные функции sse2, если вы не компилируете с -msse2,
    // что, в свою очередь, означает, что SSE2 можно будет использовать повсюду. Это прискорбно,
    // но предыдущие попытки предоставить функциям SSE2 среду выполнения
    // обнаружение вызвало множество проблем. Способы расширения архитектуры
    // К сожалению, представленный в GCC /Clang, не совсем подходит для однофайловых библиотек.
    // Новое поведение: при компиляции с параметром -msse2 мы используем SSE2 без каких-либо
    // обнаружение; если нет, то мы его вообще не используем.
    #define STBI_NO_SIMD
#endif

#if defined(__MINGW32__) && defined(STBI__X86_TARGET) && !defined(STBI_MINGW_ENABLE_SSE2) && !defined(STBI_NO_SIMD)
    // Обратите внимание, что __MINGW32__ на самом деле не означает 32-битность, поэтому нам следует избегать STBI__X64_TARGET.
    //
    // 32-битный MinGW хочет, чтобы ESP был выровнен по 16 байтам, но это не в
    // Windows ABI и VC ++, а также библиотеки Windows DLL не поддерживают этот инвариант.
    // В результате включение SSE2 на 32-битном MinGW опасно, если не
    // одновременно включив «-mstackrealign».
    //
    // См. https://github.com/nothings/stb/issues/81 для получения дополнительной информации.
    //
    // Поэтому по умолчанию SSE2 на 32-битном MinGW отсутствует. Если вы дочитали до этого места и добавили
    // -mstackrealign к настройкам вашей сборки, смело используйте #define STBI_MINGW_ENABLE_SSE2 .
    #define STBI_NO_SIMD
#endif

#if !defined(STBI_NO_SIMD) && (defined(STBI__X86_TARGET) || defined(STBI__X64_TARGET))
#define STBI_SSE2
#include <emmintrin.h>

#ifdef _MSC_VER

#if _MSC_VER >= 1400  // не VC6
#include <intrin.h> // __cpuid
static int stbi__cpuid3(void)
{
    int info[4];
    __cpuid(info, 1);
    return info[3];
}
#else
static int stbi__cpuid3(void)
{
    int res;
    __asm {
        mov  eax, 1
        cpuid
        mov  res, edx
    }
    return res;
}
#endif

#define STBI_SIMD_ALIGN(type, name) __declspec(align(16)) type name

#if !defined(STBI_NO_JPEG) && defined(STBI_SSE2)
static int stbi__sse2_available(void)
{
    int info3 = stbi__cpuid3();
    return ((info3 >> 26) & 1) != 0;
}
#endif

#else // предположим, что стиль GCC, если не VC ++
#define STBI_SIMD_ALIGN(type, name) type name __attribute__((aligned(16)))

#if !defined(STBI_NO_JPEG) && defined(STBI_SSE2)
static int stbi__sse2_available(void)
{
    // Если мы даже попытаемся скомпилировать это на GCC /Clang, это означает, что
    // -msse2 включен, что означает, что компилятору разрешено использовать SSE2.
    // инструкции по своему желанию, и мы тоже.
    return 1;
}
#endif

#endif
#endif

// ARM NEON
#if defined(STBI_NO_SIMD) && defined(STBI_NEON)
    #undef STBI_NEON
#endif

#ifdef STBI_NEON
    #include <arm_neon.h>
    #ifdef _MSC_VER
        #define STBI_SIMD_ALIGN(type, name) __declspec(align(16)) type name
    #else
        #define STBI_SIMD_ALIGN(type, name) type name __attribute__((aligned(16)))
    #endif
#endif

#ifndef STBI_SIMD_ALIGN
    #define STBI_SIMD_ALIGN(type, name) type name
#endif

#ifndef STBI_MAX_DIMENSIONS
    #define STBI_MAX_DIMENSIONS (1 << 24)
#endif

///////////////////////////////////////////////
//
//  Структура stbi__context и функции start_xxx

// Структура stbi__context — это наш базовый контекст, используемый всеми изображениями, поэтому она
// содержит весь контекст IO, а также некоторую базовую информацию об изображении.
typedef struct {
    stbi__uint32 img_x, img_y;
    int img_n, img_out_n;

    stbi_io_callbacks io;
    void * io_user_data;

    int read_from_callbacks;
    int buflen;
    stbi_uc buffer_start[128];
    int callback_already_read;

    stbi_uc * img_buffer, * img_buffer_end;
    stbi_uc * img_buffer_original, * img_buffer_original_end;
} stbi__context;


static void stbi__refill_buffer(stbi__context * s);

// инициализировать контекст декодирования памяти
static void stbi__start_mem(stbi__context * s, stbi_uc const * buffer, int len)
{
    s->io.read = NULL;
    s->read_from_callbacks = 0;
    s->callback_already_read = 0;
    s->img_buffer = s->img_buffer_original = (stbi_uc *) buffer;
    s->img_buffer_end = s->img_buffer_original_end = (stbi_uc *) buffer + len;
}

// инициализировать контекст на основе обратного вызова
static void stbi__start_callbacks(stbi__context * s, stbi_io_callbacks * c, void * user)
{
    s->io = *c;
    s->io_user_data = user;
    s->buflen = sizeof(s->buffer_start);
    s->read_from_callbacks = 1;
    s->callback_already_read = 0;
    s->img_buffer = s->img_buffer_original = s->buffer_start;
    stbi__refill_buffer(s);
    s->img_buffer_original_end = s->img_buffer_end;
}

#ifndef STBI_NO_STDIO

static int stbi__stdio_read(void * user, char * data, int size)
{
    return (int) fread(data, 1, size, (FILE *) user);
}

static void stbi__stdio_skip(void * user, int n)
{
    int ch;
    fseek((FILE *) user, n, SEEK_CUR);
    ch = fgetc((FILE *) user); /* нужно прочитать байт, чтобы сбросить флаг feof() */
    if(ch != EOF) {
        ungetc(ch, (FILE *) user);  /* отправить байт обратно в поток, если он действителен. */
    }
}

static int stbi__stdio_eof(void * user)
{
    return feof((FILE *) user) || ferror((FILE *) user);
}

static stbi_io_callbacks stbi__stdio_callbacks = {
    stbi__stdio_read,
    stbi__stdio_skip,
    stbi__stdio_eof,
};

static void stbi__start_file(stbi__context * s, FILE * f)
{
    stbi__start_callbacks(s, &stbi__stdio_callbacks, (void *) f);
}

//статическая пустота stop_file ( stbi__context *s) { }

#endif // !STBI_NO_STDIO

static void stbi__rewind(stbi__context * s)
{
    // концептуально перемотка SHOULD перемотка к началу потока,
    // но мы просто перематываемся в начало исходного буфера, потому что
    // мы используем его только после выполнения «теста», который всегда просматривает не более 92 байтов.
    s->img_buffer = s->img_buffer_original;
    s->img_buffer_end = s->img_buffer_original_end;
}

enum {
    STBI_ORDER_RGB,
    STBI_ORDER_BGR
};

typedef struct {
    int bits_per_channel;
    int num_channels;
    int channel_order;
} stbi__result_info;

#ifndef STBI_NO_JPEG
    static int      stbi__jpeg_test(stbi__context * s);
    static void  *  stbi__jpeg_load(stbi__context * s, int * x, int * y, int * comp, int req_comp, stbi__result_info * ri);
    static int      stbi__jpeg_info(stbi__context * s, int * x, int * y, int * comp);
#endif

#ifndef STBI_NO_PNG
    static int      stbi__png_test(stbi__context * s);
    static void  *  stbi__png_load(stbi__context * s, int * x, int * y, int * comp, int req_comp, stbi__result_info * ri);
    static int      stbi__png_info(stbi__context * s, int * x, int * y, int * comp);
    static int      stbi__png_is16(stbi__context * s);
#endif

#ifndef STBI_NO_BMP
    static int      stbi__bmp_test(stbi__context * s);
    static void  *  stbi__bmp_load(stbi__context * s, int * x, int * y, int * comp, int req_comp, stbi__result_info * ri);
    static int      stbi__bmp_info(stbi__context * s, int * x, int * y, int * comp);
#endif

#ifndef STBI_NO_TGA
    static int      stbi__tga_test(stbi__context * s);
    static void  *  stbi__tga_load(stbi__context * s, int * x, int * y, int * comp, int req_comp, stbi__result_info * ri);
    static int      stbi__tga_info(stbi__context * s, int * x, int * y, int * comp);
#endif

#ifndef STBI_NO_PSD
static int      stbi__psd_test(stbi__context * s);
static void  *  stbi__psd_load(stbi__context * s, int * x, int * y, int * comp, int req_comp, stbi__result_info * ri,
                               int bpc);
static int      stbi__psd_info(stbi__context * s, int * x, int * y, int * comp);
static int      stbi__psd_is16(stbi__context * s);
#endif

#ifndef STBI_NO_HDR
    static int      stbi__hdr_test(stbi__context * s);
    static float  * stbi__hdr_load(stbi__context * s, int * x, int * y, int * comp, int req_comp, stbi__result_info * ri);
    static int      stbi__hdr_info(stbi__context * s, int * x, int * y, int * comp);
#endif

#ifndef STBI_NO_PIC
    static int      stbi__pic_test(stbi__context * s);
    static void  *  stbi__pic_load(stbi__context * s, int * x, int * y, int * comp, int req_comp, stbi__result_info * ri);
    static int      stbi__pic_info(stbi__context * s, int * x, int * y, int * comp);
#endif

#ifndef STBI_NO_GIF
static int      stbi__gif_test(stbi__context * s);
static void  *  stbi__gif_load(stbi__context * s, int * x, int * y, int * comp, int req_comp, stbi__result_info * ri);
static void  *  stbi__load_gif_main(stbi__context * s, int ** delays, int * x, int * y, int * z, int * comp,
                                    int req_comp);
static int      stbi__gif_info(stbi__context * s, int * x, int * y, int * comp);
#endif

#ifndef STBI_NO_PNM
    static int      stbi__pnm_test(stbi__context * s);
    static void  *  stbi__pnm_load(stbi__context * s, int * x, int * y, int * comp, int req_comp, stbi__result_info * ri);
    static int      stbi__pnm_info(stbi__context * s, int * x, int * y, int * comp);
    static int      stbi__pnm_is16(stbi__context * s);
#endif

static
#ifdef STBI_THREAD_LOCAL
    STBI_THREAD_LOCAL
#endif
const char * stbi__g_failure_reason;

STBIDEF const char * stbi_failure_reason(void)
{
    return stbi__g_failure_reason;
}

#ifndef STBI_NO_FAILURE_STRINGS
static int stbi__err(const char * str)
{
    stbi__g_failure_reason = str;
    return 0;
}
#endif

static void * stbi__malloc(size_t size)
{
    return STBI_MALLOC(size);
}

// stb_image повсеместно использует целые числа, в том числе для вычислений смещения.
// поэтому наибольший размер декодированного изображения, который мы можем поддерживать с помощью
// текущий код, даже для 64-битных целей, — INT_MAX. это не
// существенное ограничение для предполагаемого варианта использования.
//
// однако нам необходимо убедиться, что наши расчеты размеров не
// переполнение. следовательно, несколько вспомогательных функций для расчета размера, которые
// перемножить целые числа, убедившись, что они неотрицательны
// и никакого переполнения не происходит.

// возвращает 1, если сумма действительна, и 0 при переполнении.
// отрицательные термины считаются недействительными.
static int stbi__addsizes_valid(int a, int b)
{
    if(b < 0) return 0;
    // теперь 0 <= b <= INT_MAX , следовательно, также
    // 0 <= INT_MAX - b <= INTMAX.
    // И «a + b <= INT_MAX» (который может переполниться) — это
    // то же, что a <= INT_MAX - b (без переполнения)
    return a <= INT_MAX - b;
}

// возвращает 1, если продукт действителен, и 0 при переполнении.
// отрицательные факторы считаются недействительными.
static int stbi__mul2sizes_valid(int a, int b)
{
    if(a < 0 || b < 0) return 0;
    if(b == 0) return 1;  // mul-by-0 всегда безопасно
    // портативный способ проверить отсутствие переполнений в a*b
    return a <= INT_MAX / b;
}

#if !defined(STBI_NO_JPEG) || !defined(STBI_NO_PNG) || !defined(STBI_NO_TGA) || !defined(STBI_NO_HDR)
// возвращает 1, если «a*b + add» не имеет отрицательных членов/факторов и не переполняется
static int stbi__mad2sizes_valid(int a, int b, int add)
{
    return stbi__mul2sizes_valid(a, b) && stbi__addsizes_valid(a * b, add);
}
#endif

// возвращает 1, если "a*b*c + add" не имеет отрицательных членов/факторов и не переполняется
static int stbi__mad3sizes_valid(int a, int b, int c, int add)
{
    return stbi__mul2sizes_valid(a, b) && stbi__mul2sizes_valid(a * b, c) &&
           stbi__addsizes_valid(a * b * c, add);
}

// возвращает 1, если "a*b*c*d + add" не имеет отрицательных членов/факторов и не переполняется
#if !defined(STBI_NO_LINEAR) || !defined(STBI_NO_HDR) || !defined(STBI_NO_PNM)
static int stbi__mad4sizes_valid(int a, int b, int c, int d, int add)
{
    return stbi__mul2sizes_valid(a, b) && stbi__mul2sizes_valid(a * b, c) &&
           stbi__mul2sizes_valid(a * b * c, d) && stbi__addsizes_valid(a * b * c * d, add);
}
#endif

#if !defined(STBI_NO_JPEG) || !defined(STBI_NO_PNG) || !defined(STBI_NO_TGA) || !defined(STBI_NO_HDR)
// mallocs с проверкой переполнения размера
static void * stbi__malloc_mad2(int a, int b, int add)
{
    if(!stbi__mad2sizes_valid(a, b, add)) return NULL;
    return stbi__malloc(a * b + add);
}
#endif

static void * stbi__malloc_mad3(int a, int b, int c, int add)
{
    if(!stbi__mad3sizes_valid(a, b, c, add)) return NULL;
    return stbi__malloc(a * b * c + add);
}

#if !defined(STBI_NO_LINEAR) || !defined(STBI_NO_HDR) || !defined(STBI_NO_PNM)
static void * stbi__malloc_mad4(int a, int b, int c, int d, int add)
{
    if(!stbi__mad4sizes_valid(a, b, c, d, add)) return NULL;
    return stbi__malloc(a * b * c * d + add);
}
#endif

// возвращает 1, если сумма двух целых чисел со знаком действительна (от -2^31 до 2^31-1 включительно), 0 при переполнении.
static int stbi__addints_valid(int a, int b)
{
    if((a >= 0) != (b >= 0)) return 1;  // a и b имеют разные знаки, поэтому переполнения нет.
    if(a < 0 && b < 0) return a >= INT_MIN - b;  // то же, что a + b >= INT_MIN ;  INT_MIN — b не может переполниться, поскольку b < 0.
    return a <= INT_MAX - b;
}

// возвращает 1, если произведение двух целых чисел помещается в короткое число со знаком, и 0 при переполнении.
static int stbi__mul2shorts_valid(int a, int b)
{
    if(b == 0 || b == -1) return 1;  // умножение на 0 всегда равно 0; проверьте -1, чтобы SHRT_MIN /b не переполнялся
    if((a >= 0) == (b >= 0)) return a <= SHRT_MAX / b; // продукт положительный, поэтому похож на mul2sizes_valid
    if(b < 0) return a <= SHRT_MIN / b;  // то же, что a * b >= SHRT_MIN
    return a >= SHRT_MIN / b;
}

// stbi__err - ошибка
// stbi__errpf - error returning pointer to float
// stbi__errpuc - error returning pointer to unsigned char

#ifdef STBI_NO_FAILURE_STRINGS
    #define stbi__err(x,y)  0
#elif defined(STBI_FAILURE_USERMSG)
    #define stbi__err(x,y)  stbi__err(y)
#else
    #define stbi__err(x,y)  stbi__err(x)
#endif

#define stbi__errpf(x,y)   ((float *)(size_t) (stbi__err(x,y)?NULL:NULL))
#define stbi__errpuc(x,y)  ((unsigned char *)(size_t) (stbi__err(x,y)?NULL:NULL))

STBIDEF void stbi_image_free(void * retval_from_stbi_load)
{
    STBI_FREE(retval_from_stbi_load);
}

#ifndef STBI_NO_LINEAR
    static float  * stbi__ldr_to_hdr(stbi_uc * data, int x, int y, int comp);
#endif

#ifndef STBI_NO_HDR
    static stbi_uc * stbi__hdr_to_ldr(float  * data, int x, int y, int comp);
#endif

static int stbi__vertically_flip_on_load_global = 0;

STBIDEF void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip)
{
    stbi__vertically_flip_on_load_global = flag_true_if_should_flip;
}

#ifndef STBI_THREAD_LOCAL
#define stbi__vertically_flip_on_load  stbi__vertically_flip_on_load_global
#else
static STBI_THREAD_LOCAL int stbi__vertically_flip_on_load_local, stbi__vertically_flip_on_load_set;

STBIDEF void stbi_set_flip_vertically_on_load_thread(int flag_true_if_should_flip)
{
    stbi__vertically_flip_on_load_local = flag_true_if_should_flip;
    stbi__vertically_flip_on_load_set = 1;
}

#define stbi__vertically_flip_on_load  (stbi__vertically_flip_on_load_set       \
                                        ? stbi__vertically_flip_on_load_local  \
                                        : stbi__vertically_flip_on_load_global)
#endif // STBI_THREAD_LOCAL

static void * stbi__load_main(stbi__context * s, int * x, int * y, int * comp, int req_comp, stbi__result_info * ri,
                              int bpc)
{
    memset(ri, 0, sizeof(*ri)); // убедитесь, что он инициализирован, если мы добавим новые поля
    ri->bits_per_channel = 8; // по умолчанию — 8, поэтому большинство путей менять не нужно.
    ri->channel_order = STBI_ORDER_RGB; // все текущие входные и выходные данные здесь, но это здесь, чтобы мы могли добавить порядок BGR
    ri->num_channels = 0;

    // сначала протестируйте форматы с очень явным заголовком (по крайней мере, FOURCC
    // или сначала отличительный магический номер)
#ifndef STBI_NO_PNG
    if(stbi__png_test(s))  return stbi__png_load(s, x, y, comp, req_comp, ri);
#endif
#ifndef STBI_NO_BMP
    if(stbi__bmp_test(s))  return stbi__bmp_load(s, x, y, comp, req_comp, ri);
#endif
#ifndef STBI_NO_GIF
    if(stbi__gif_test(s))  return stbi__gif_load(s, x, y, comp, req_comp, ri);
#endif
#ifndef STBI_NO_PSD
    if(stbi__psd_test(s))  return stbi__psd_load(s, x, y, comp, req_comp, ri, bpc);
#else
    STBI_NOTUSED(bpc);
#endif
#ifndef STBI_NO_PIC
    if(stbi__pic_test(s))  return stbi__pic_load(s, x, y, comp, req_comp, ri);
#endif

    // тогда форматы, которые могут в конечном итоге попытаться загрузить всего 1 или 2
    // байты, соответствующие ожиданиям; они склонны к ложным срабатываниям, поэтому
    // попробуй их позже
#ifndef STBI_NO_JPEG
    if(stbi__jpeg_test(s)) return stbi__jpeg_load(s, x, y, comp, req_comp, ri);
#endif
#ifndef STBI_NO_PNM
    if(stbi__pnm_test(s))  return stbi__pnm_load(s, x, y, comp, req_comp, ri);
#endif

#ifndef STBI_NO_HDR
    if(stbi__hdr_test(s)) {
        float * hdr = stbi__hdr_load(s, x, y, comp, req_comp, ri);
        return stbi__hdr_to_ldr(hdr, *x, *y, req_comp ? req_comp : *comp);
    }
#endif

#ifndef STBI_NO_TGA
    // тестируйте tga последним, потому что это дрянной тест!
    if(stbi__tga_test(s))
        return stbi__tga_load(s, x, y, comp, req_comp, ri);
#endif

    return stbi__errpuc("unknown image type", "Image not of any known type, or corrupt");
}

static stbi_uc * stbi__convert_16_to_8(stbi__uint16 * orig, int w, int h, int channels)
{
    int i;
    int img_len = w * h * channels;
    stbi_uc * reduced;

    reduced = (stbi_uc *) stbi__malloc(img_len);
    if(reduced == NULL) return stbi__errpuc("outofmem", "Out of memory");

    for(i = 0; i < img_len; ++i)
        reduced[i] = (stbi_uc)((orig[i] >> 8) & 0xFF); // верхней половины каждого байта достаточно для масштабирования примерно 16->8 бит.

    STBI_FREE(orig);
    return reduced;
}

static stbi__uint16 * stbi__convert_8_to_16(stbi_uc * orig, int w, int h, int channels)
{
    int i;
    int img_len = w * h * channels;
    stbi__uint16 * enlarged;

    enlarged = (stbi__uint16 *) stbi__malloc(img_len * 2);
    if(enlarged == NULL) return (stbi__uint16 *) stbi__errpuc("outofmem", "Out of memory");

    for(i = 0; i < img_len; ++i)
        enlarged[i] = (stbi__uint16)((orig[i] << 8) + orig[i]); // репликация в старший и младший байт, карты 0->0, 255-> 0xffff

    STBI_FREE(orig);
    return enlarged;
}

static void stbi__vertical_flip(void * image, int w, int h, int bytes_per_pixel)
{
    int row;
    size_t bytes_per_row = (size_t)w * bytes_per_pixel;
    stbi_uc temp[2048];
    stbi_uc * bytes = (stbi_uc *)image;

    for(row = 0; row < (h >> 1); row++) {
        stbi_uc * row0 = bytes + row * bytes_per_row;
        stbi_uc * row1 = bytes + (h - row - 1) * bytes_per_row;
        // поменять строку 0 на строку 1
        size_t bytes_left = bytes_per_row;
        while(bytes_left) {
            size_t bytes_copy = (bytes_left < sizeof(temp)) ? bytes_left : sizeof(temp);
            memcpy(temp, row0, bytes_copy);
            memcpy(row0, row1, bytes_copy);
            memcpy(row1, temp, bytes_copy);
            row0 += bytes_copy;
            row1 += bytes_copy;
            bytes_left -= bytes_copy;
        }
    }
}

#ifndef STBI_NO_GIF
static void stbi__vertical_flip_slices(void * image, int w, int h, int z, int bytes_per_pixel)
{
    int slice;
    int slice_size = w * h * bytes_per_pixel;

    stbi_uc * bytes = (stbi_uc *)image;
    for(slice = 0; slice < z; ++slice) {
        stbi__vertical_flip(bytes, w, h, bytes_per_pixel);
        bytes += slice_size;
    }
}
#endif

static unsigned char * stbi__load_and_postprocess_8bit(stbi__context * s, int * x, int * y, int * comp, int req_comp)
{
    stbi__result_info ri;
    void * result = stbi__load_main(s, x, y, comp, req_comp, &ri, 8);

    if(result == NULL)
        return NULL;

    // ответственность за то, чтобы мы получили либо 8, либо 16 бит, лежит на загрузчиках.
    STBI_ASSERT(ri.bits_per_channel == 8 || ri.bits_per_channel == 16);

    if(ri.bits_per_channel != 8) {
        result = stbi__convert_16_to_8((stbi__uint16 *) result, *x, *y, req_comp == 0 ? *comp : req_comp);
        ri.bits_per_channel = 8;
    }

    // @TODO: move stbi__convert_format to here

    if(stbi__vertically_flip_on_load) {
        int channels = req_comp ? req_comp : *comp;
        stbi__vertical_flip(result, *x, *y, channels * sizeof(stbi_uc));
    }

    return (unsigned char *) result;
}

static stbi__uint16 * stbi__load_and_postprocess_16bit(stbi__context * s, int * x, int * y, int * comp, int req_comp)
{
    stbi__result_info ri;
    void * result = stbi__load_main(s, x, y, comp, req_comp, &ri, 16);

    if(result == NULL)
        return NULL;

    // ответственность за то, чтобы мы получили либо 8, либо 16 бит, лежит на загрузчиках.
    STBI_ASSERT(ri.bits_per_channel == 8 || ri.bits_per_channel == 16);

    if(ri.bits_per_channel != 16) {
        result = stbi__convert_8_to_16((stbi_uc *) result, *x, *y, req_comp == 0 ? *comp : req_comp);
        ri.bits_per_channel = 16;
    }

    // @TODO: move stbi__convert_format16 to here
    // @TODO: special case RGB-to-Y (and RGBA-to-YA) for 8-bit-to-16-bit case to keep more precision

    if(stbi__vertically_flip_on_load) {
        int channels = req_comp ? req_comp : *comp;
        stbi__vertical_flip(result, *x, *y, channels * sizeof(stbi__uint16));
    }

    return (stbi__uint16 *) result;
}

#if !defined(STBI_NO_HDR) && !defined(STBI_NO_LINEAR)
static void stbi__float_postprocess(float * result, int * x, int * y, int * comp, int req_comp)
{
    if(stbi__vertically_flip_on_load && result != NULL) {
        int channels = req_comp ? req_comp : *comp;
        stbi__vertical_flip(result, *x, *y, channels * sizeof(float));
    }
}
#endif

#ifndef STBI_NO_STDIO

#if defined(_WIN32) && defined(STBI_WINDOWS_UTF8)
STBI_EXTERN __declspec(dllimport) int __stdcall MultiByteToWideChar(unsigned int cp, unsigned long flags,
                                                                    const char * str, int cbmb, wchar_t * widestr, int cchwide);
STBI_EXTERN __declspec(dllimport) int __stdcall WideCharToMultiByte(unsigned int cp, unsigned long flags,
                                                                    const wchar_t * widestr, int cchwide, char * str, int cbmb, const char * defchar, int * used_default);
#endif

#if defined(_WIN32) && defined(STBI_WINDOWS_UTF8)
STBIDEF int stbi_convert_wchar_to_utf8(char * buffer, size_t bufferlen, const wchar_t * input)
{
    return WideCharToMultiByte(65001 /* UTF8 */, 0, input, -1, buffer, (int) bufferlen, NULL, NULL);
}
#endif

static FILE * stbi__fopen(char const * filename, char const * mode)
{
    FILE * f;
#if defined(_WIN32) && defined(STBI_WINDOWS_UTF8)
    wchar_t wMode[64];
    wchar_t wFilename[1024];
    if(0 == MultiByteToWideChar(65001 /* UTF8 */, 0, filename, -1, wFilename, sizeof(wFilename) / sizeof(*wFilename)))
        return 0;

    if(0 == MultiByteToWideChar(65001 /* UTF8 */, 0, mode, -1, wMode, sizeof(wMode) / sizeof(*wMode)))
        return 0;

#if defined(_MSC_VER) && _MSC_VER >= 1400
    if(0 != _wfopen_s(&f, wFilename, wMode))
        f = 0;
#else
    f = _wfopen(wFilename, wMode);
#endif

#elif defined(_MSC_VER) && _MSC_VER >= 1400
    if(0 != fopen_s(&f, filename, mode))
        f = 0;
#else
    f = fopen(filename, mode);
#endif
    return f;
}


STBIDEF stbi_uc * stbi_load(char const * filename, int * x, int * y, int * comp, int req_comp)
{
    FILE * f = stbi__fopen(filename, "rb");
    unsigned char * result;
    if(!f) return stbi__errpuc("can't fopen", "Unable to open file");
    result = stbi_load_from_file(f, x, y, comp, req_comp);
    fclose(f);
    return result;
}

STBIDEF stbi_uc * stbi_load_from_file(FILE * f, int * x, int * y, int * comp, int req_comp)
{
    unsigned char * result;
    stbi__context s;
    stbi__start_file(&s, f);
    result = stbi__load_and_postprocess_8bit(&s, x, y, comp, req_comp);
    if(result) {
        // необходимо «отключить» все символы в буфере IO
        fseek(f, - (int)(s.img_buffer_end - s.img_buffer), SEEK_CUR);
    }
    return result;
}

STBIDEF stbi__uint16 * stbi_load_from_file_16(FILE * f, int * x, int * y, int * comp, int req_comp)
{
    stbi__uint16 * result;
    stbi__context s;
    stbi__start_file(&s, f);
    result = stbi__load_and_postprocess_16bit(&s, x, y, comp, req_comp);
    if(result) {
        // необходимо «отключить» все символы в буфере IO
        fseek(f, - (int)(s.img_buffer_end - s.img_buffer), SEEK_CUR);
    }
    return result;
}

STBIDEF stbi_us * stbi_load_16(char const * filename, int * x, int * y, int * comp, int req_comp)
{
    FILE * f = stbi__fopen(filename, "rb");
    stbi__uint16 * result;
    if(!f) return (stbi_us *) stbi__errpuc("can't fopen", "Unable to open file");
    result = stbi_load_from_file_16(f, x, y, comp, req_comp);
    fclose(f);
    return result;
}


#endif //!STBI_NO_STDIO

STBIDEF stbi_us * stbi_load_16_from_memory(stbi_uc const * buffer, int len, int * x, int * y, int * channels_in_file,
                                           int desired_channels)
{
    stbi__context s;
    stbi__start_mem(&s, buffer, len);
    return stbi__load_and_postprocess_16bit(&s, x, y, channels_in_file, desired_channels);
}

STBIDEF stbi_us * stbi_load_16_from_callbacks(stbi_io_callbacks const * clbk, void * user, int * x, int * y,
                                              int * channels_in_file, int desired_channels)
{
    stbi__context s;
    stbi__start_callbacks(&s, (stbi_io_callbacks *)clbk, user);
    return stbi__load_and_postprocess_16bit(&s, x, y, channels_in_file, desired_channels);
}

STBIDEF stbi_uc * stbi_load_from_memory(stbi_uc const * buffer, int len, int * x, int * y, int * comp, int req_comp)
{
    stbi__context s;
    stbi__start_mem(&s, buffer, len);
    return stbi__load_and_postprocess_8bit(&s, x, y, comp, req_comp);
}

STBIDEF stbi_uc * stbi_load_from_callbacks(stbi_io_callbacks const * clbk, void * user, int * x, int * y, int * comp,
                                           int req_comp)
{
    stbi__context s;
    stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
    return stbi__load_and_postprocess_8bit(&s, x, y, comp, req_comp);
}

#ifndef STBI_NO_GIF
STBIDEF stbi_uc * stbi_load_gif_from_memory(stbi_uc const * buffer, int len, int ** delays, int * x, int * y, int * z,
                                            int * comp, int req_comp)
{
    unsigned char * result;
    stbi__context s;
    stbi__start_mem(&s, buffer, len);

    result = (unsigned char *) stbi__load_gif_main(&s, delays, x, y, z, comp, req_comp);
    if(stbi__vertically_flip_on_load) {
        stbi__vertical_flip_slices(result, *x, *y, *z, *comp);
    }

    return result;
}
#endif

#ifndef STBI_NO_LINEAR
static float * stbi__loadf_main(stbi__context * s, int * x, int * y, int * comp, int req_comp)
{
    unsigned char * data;
#ifndef STBI_NO_HDR
    if(stbi__hdr_test(s)) {
        stbi__result_info ri;
        float * hdr_data = stbi__hdr_load(s, x, y, comp, req_comp, &ri);
        if(hdr_data)
            stbi__float_postprocess(hdr_data, x, y, comp, req_comp);
        return hdr_data;
    }
#endif
    data = stbi__load_and_postprocess_8bit(s, x, y, comp, req_comp);
    if(data)
        return stbi__ldr_to_hdr(data, *x, *y, req_comp ? req_comp : *comp);
    return stbi__errpf("unknown image type", "Image not of any known type, or corrupt");
}

STBIDEF float * stbi_loadf_from_memory(stbi_uc const * buffer, int len, int * x, int * y, int * comp, int req_comp)
{
    stbi__context s;
    stbi__start_mem(&s, buffer, len);
    return stbi__loadf_main(&s, x, y, comp, req_comp);
}

STBIDEF float * stbi_loadf_from_callbacks(stbi_io_callbacks const * clbk, void * user, int * x, int * y, int * comp,
                                          int req_comp)
{
    stbi__context s;
    stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
    return stbi__loadf_main(&s, x, y, comp, req_comp);
}

#ifndef STBI_NO_STDIO
STBIDEF float * stbi_loadf(char const * filename, int * x, int * y, int * comp, int req_comp)
{
    float * result;
    FILE * f = stbi__fopen(filename, "rb");
    if(!f) return stbi__errpf("can't fopen", "Unable to open file");
    result = stbi_loadf_from_file(f, x, y, comp, req_comp);
    fclose(f);
    return result;
}

STBIDEF float * stbi_loadf_from_file(FILE * f, int * x, int * y, int * comp, int req_comp)
{
    stbi__context s;
    stbi__start_file(&s, f);
    return stbi__loadf_main(&s, x, y, comp, req_comp);
}
#endif // !STBI_NO_STDIO

#endif // !STBI_NO_LINEAR

// эти параметры «is-hdr-or-not» не зависят от того, является ли STBI_NO_LINEAR
// определено для простоты API; если STBI_NO_LINEAR определен, он всегда
// сообщает ложь!

STBIDEF int stbi_is_hdr_from_memory(stbi_uc const * buffer, int len)
{
#ifndef STBI_NO_HDR
    stbi__context s;
    stbi__start_mem(&s, buffer, len);
    return stbi__hdr_test(&s);
#else
    STBI_NOTUSED(buffer);
    STBI_NOTUSED(len);
    return 0;
#endif
}

#ifndef STBI_NO_STDIO
STBIDEF int      stbi_is_hdr(char const * filename)
{
    FILE * f = stbi__fopen(filename, "rb");
    int result = 0;
    if(f) {
        result = stbi_is_hdr_from_file(f);
        fclose(f);
    }
    return result;
}

STBIDEF int stbi_is_hdr_from_file(FILE * f)
{
#ifndef STBI_NO_HDR
    long pos = ftell(f);
    int res;
    stbi__context s;
    stbi__start_file(&s, f);
    res = stbi__hdr_test(&s);
    fseek(f, pos, SEEK_SET);
    return res;
#else
    STBI_NOTUSED(f);
    return 0;
#endif
}
#endif // !STBI_NO_STDIO

STBIDEF int      stbi_is_hdr_from_callbacks(stbi_io_callbacks const * clbk, void * user)
{
#ifndef STBI_NO_HDR
    stbi__context s;
    stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
    return stbi__hdr_test(&s);
#else
    STBI_NOTUSED(clbk);
    STBI_NOTUSED(user);
    return 0;
#endif
}

#ifndef STBI_NO_LINEAR
static float stbi__l2h_gamma = 2.2f, stbi__l2h_scale = 1.0f;

STBIDEF void   stbi_ldr_to_hdr_gamma(float gamma)
{
    stbi__l2h_gamma = gamma;
}
STBIDEF void   stbi_ldr_to_hdr_scale(float scale)
{
    stbi__l2h_scale = scale;
}
#endif

static float stbi__h2l_gamma_i = 1.0f / 2.2f, stbi__h2l_scale_i = 1.0f;

STBIDEF void   stbi_hdr_to_ldr_gamma(float gamma)
{
    stbi__h2l_gamma_i = 1 / gamma;
}
STBIDEF void   stbi_hdr_to_ldr_scale(float scale)
{
    stbi__h2l_scale_i = 1 / scale;
}


//////////////////////////////////////////////////////////////////////////////
//
// Общий код, используемый всеми загрузчиками изображений.
//

enum {
    STBI__SCAN_load = 0,
    STBI__SCAN_type,
    STBI__SCAN_header
};

static void stbi__refill_buffer(stbi__context * s)
{
    int n = (s->io.read)(s->io_user_data, (char *)s->buffer_start, s->buflen);
    s->callback_already_read += (int)(s->img_buffer - s->img_buffer_original);
    if(n == 0) {
        // в конце файла действовать так же, как если бы из памяти, но нужно обрабатывать регистр
        // где s-> img_buffer не указывает на безопасную память, например. 0-байтовый файл
        s->read_from_callbacks = 0;
        s->img_buffer = s->buffer_start;
        s->img_buffer_end = s->buffer_start + 1;
        *s->img_buffer = 0;
    }
    else {
        s->img_buffer = s->buffer_start;
        s->img_buffer_end = s->buffer_start + n;
    }
}

stbi_inline static stbi_uc stbi__get8(stbi__context * s)
{
    if(s->img_buffer < s->img_buffer_end)
        return *s->img_buffer++;
    if(s->read_from_callbacks) {
        stbi__refill_buffer(s);
        return *s->img_buffer++;
    }
    return 0;
}

#if defined(STBI_NO_JPEG) && defined(STBI_NO_HDR) && defined(STBI_NO_PIC) && defined(STBI_NO_PNM)
// ничего
#else
stbi_inline static int stbi__at_eof(stbi__context * s)
{
    if(s->io.read) {
        if(!(s->io.eof)(s->io_user_data)) return 0;
        // если feof() истинно, проверьте, имеет ли буфер = конец
        // особый случай: у нас есть только специальный символ 0 в конце
        if(s->read_from_callbacks == 0) return 1;
    }

    return s->img_buffer >= s->img_buffer_end;
}
#endif

#if defined(STBI_NO_JPEG) && defined(STBI_NO_PNG) && defined(STBI_NO_BMP) && defined(STBI_NO_PSD) && defined(STBI_NO_TGA) && defined(STBI_NO_GIF) && defined(STBI_NO_PIC)
// ничего
#else
static void stbi__skip(stbi__context * s, int n)
{
    if(n == 0) return;   // уже там!
    if(n < 0) {
        s->img_buffer = s->img_buffer_end;
        return;
    }
    if(s->io.read) {
        int blen = (int)(s->img_buffer_end - s->img_buffer);
        if(blen < n) {
            s->img_buffer = s->img_buffer_end;
            (s->io.skip)(s->io_user_data, n - blen);
            return;
        }
    }
    s->img_buffer += n;
}
#endif

#if defined(STBI_NO_PNG) && defined(STBI_NO_TGA) && defined(STBI_NO_HDR) && defined(STBI_NO_PNM)
// ничего
#else
static int stbi__getn(stbi__context * s, stbi_uc * buffer, int n)
{
    if(s->io.read) {
        int blen = (int)(s->img_buffer_end - s->img_buffer);
        if(blen < n) {
            int res, count;

            memcpy(buffer, s->img_buffer, blen);

            count = (s->io.read)(s->io_user_data, (char *) buffer + blen, n - blen);
            res = (count == (n - blen));
            s->img_buffer = s->img_buffer_end;
            return res;
        }
    }

    if(s->img_buffer + n <= s->img_buffer_end) {
        memcpy(buffer, s->img_buffer, n);
        s->img_buffer += n;
        return 1;
    }
    else
        return 0;
}
#endif

#if defined(STBI_NO_JPEG) && defined(STBI_NO_PNG) && defined(STBI_NO_PSD) && defined(STBI_NO_PIC)
// ничего
#else
static int stbi__get16be(stbi__context * s)
{
    int z = stbi__get8(s);
    return (z << 8) + stbi__get8(s);
}
#endif

#if defined(STBI_NO_PNG) && defined(STBI_NO_PSD) && defined(STBI_NO_PIC)
// ничего
#else
static stbi__uint32 stbi__get32be(stbi__context * s)
{
    stbi__uint32 z = stbi__get16be(s);
    return (z << 16) + stbi__get16be(s);
}
#endif

#if defined(STBI_NO_BMP) && defined(STBI_NO_TGA) && defined(STBI_NO_GIF)
// ничего
#else
static int stbi__get16le(stbi__context * s)
{
    int z = stbi__get8(s);
    return z + (stbi__get8(s) << 8);
}
#endif

#ifndef STBI_NO_BMP
static stbi__uint32 stbi__get32le(stbi__context * s)
{
    stbi__uint32 z = stbi__get16le(s);
    z += (stbi__uint32)stbi__get16le(s) << 16;
    return z;
}
#endif

#define STBI__BYTECAST(x)  ((stbi_uc) ((x) & 255))  // обрезать целое число до байта без предупреждений

#if defined(STBI_NO_JPEG) && defined(STBI_NO_PNG) && defined(STBI_NO_BMP) && defined(STBI_NO_PSD) && defined(STBI_NO_TGA) && defined(STBI_NO_GIF) && defined(STBI_NO_PIC) && defined(STBI_NO_PNM)
// ничего
#else
//////////////////////////////////////////////////////////////////////////////
//
//  универсальный конвертер из встроенного img_n в req_comp
//    отдельные типы делают это автоматически, насколько это возможно (например, jpeg
//    выполняет все случаи внутри себя, поскольку в любом случае необходимо преобразовать цветовое пространство,
//    и у него никогда не бывает альфа, поэтому случаев очень мало). PNG может автоматически
//    чередовать канал с альфа=255, но в других случаях возвращается к этому
//
//  предположим, что буфер данных выделен, поэтому выделите новый и освободите его.
//  единственный режим сбоя — сбой malloc

static stbi_uc stbi__compute_y(int r, int g, int b)
{
    return (stbi_uc)(((r * 77) + (g * 150) + (29 * b)) >> 8);
}
#endif

#if defined(STBI_NO_PNG) && defined(STBI_NO_BMP) && defined(STBI_NO_PSD) && defined(STBI_NO_TGA) && defined(STBI_NO_GIF) && defined(STBI_NO_PIC) && defined(STBI_NO_PNM)
// ничего
#else
static unsigned char * stbi__convert_format(unsigned char * data, int img_n, int req_comp, unsigned int x,
                                            unsigned int y)
{
    int i, j;
    unsigned char * good;

    if(req_comp == img_n) return data;
    STBI_ASSERT(req_comp >= 1 && req_comp <= 4);

    good = (unsigned char *) stbi__malloc_mad3(req_comp, x, y, 0);
    if(good == NULL) {
        STBI_FREE(data);
        return stbi__errpuc("outofmem", "Out of memory");
    }

    for(j = 0; j < (int) y; ++j) {
        unsigned char * src  = data + j * x * img_n   ;
        unsigned char * dest = good + j * x * req_comp;

#define STBI__COMBO(a,b)  ((a)*8+(b))
#define STBI__CASE(a,b)   case STBI__COMBO(a,b): for(i=x-1; i >= 0; --i, src += a, dest += b)
        // преобразовать исходное изображение с компонентами img_n в изображение с компонентами req_comp;
        // избегайте переключения попиксельно, поэтому используйте переключение по строке сканирования и массивные макросы
        switch(STBI__COMBO(img_n, req_comp)) {
                STBI__CASE(1, 2) {
                    dest[0] = src[0];
                    dest[1] = 255;
                }
                break;
                STBI__CASE(1, 3) {
                    dest[0] = dest[1] = dest[2] = src[0];
                }
                break;
                STBI__CASE(1, 4) {
                    dest[0] = dest[1] = dest[2] = src[0];
                    dest[3] = 255;
                }
                break;
                STBI__CASE(2, 1) {
                    dest[0] = src[0];
                }
                break;
                STBI__CASE(2, 3) {
                    dest[0] = dest[1] = dest[2] = src[0];
                }
                break;
                STBI__CASE(2, 4) {
                    dest[0] = dest[1] = dest[2] = src[0];
                    dest[3] = src[1];
                }
                break;
                STBI__CASE(3, 4) {
                    dest[0] = src[0];
                    dest[1] = src[1];
                    dest[2] = src[2];
                    dest[3] = 255;
                }
                break;
                STBI__CASE(3, 1) {
                    dest[0] = stbi__compute_y(src[0], src[1], src[2]);
                }
                break;
                STBI__CASE(3, 2) {
                    dest[0] = stbi__compute_y(src[0], src[1], src[2]);
                    dest[1] = 255;
                }
                break;
                STBI__CASE(4, 1) {
                    dest[0] = stbi__compute_y(src[0], src[1], src[2]);
                }
                break;
                STBI__CASE(4, 2) {
                    dest[0] = stbi__compute_y(src[0], src[1], src[2]);
                    dest[1] = src[3];
                }
                break;
                STBI__CASE(4, 3) {
                    dest[0] = src[0];
                    dest[1] = src[1];
                    dest[2] = src[2];
                }
                break;
            default:
                STBI_ASSERT(0);
                STBI_FREE(data);
                STBI_FREE(good);
                return stbi__errpuc("unsupported", "Unsupported format conversion");
        }
#undef STBI__CASE
    }

    STBI_FREE(data);
    return good;
}
#endif

#if defined(STBI_NO_PNG) && defined(STBI_NO_PSD)
// ничего
#else
static stbi__uint16 stbi__compute_y_16(int r, int g, int b)
{
    return (stbi__uint16)(((r * 77) + (g * 150) + (29 * b)) >> 8);
}
#endif

#if defined(STBI_NO_PNG) && defined(STBI_NO_PSD)
// ничего
#else
static stbi__uint16 * stbi__convert_format16(stbi__uint16 * data, int img_n, int req_comp, unsigned int x,
                                             unsigned int y)
{
    int i, j;
    stbi__uint16 * good;

    if(req_comp == img_n) return data;
    STBI_ASSERT(req_comp >= 1 && req_comp <= 4);

    good = (stbi__uint16 *) stbi__malloc(req_comp * x * y * 2);
    if(good == NULL) {
        STBI_FREE(data);
        return (stbi__uint16 *) stbi__errpuc("outofmem", "Out of memory");
    }

    for(j = 0; j < (int) y; ++j) {
        stbi__uint16 * src  = data + j * x * img_n   ;
        stbi__uint16 * dest = good + j * x * req_comp;

#define STBI__COMBO(a,b)  ((a)*8+(b))
#define STBI__CASE(a,b)   case STBI__COMBO(a,b): for(i=x-1; i >= 0; --i, src += a, dest += b)
        // преобразовать исходное изображение с компонентами img_n в изображение с компонентами req_comp;
        // избегайте переключения попиксельно, поэтому используйте переключение по строке сканирования и массивные макросы
        switch(STBI__COMBO(img_n, req_comp)) {
                STBI__CASE(1, 2) {
                    dest[0] = src[0];
                    dest[1] = 0xffff;
                }
                break;
                STBI__CASE(1, 3) {
                    dest[0] = dest[1] = dest[2] = src[0];
                }
                break;
                STBI__CASE(1, 4) {
                    dest[0] = dest[1] = dest[2] = src[0];
                    dest[3] = 0xffff;
                }
                break;
                STBI__CASE(2, 1) {
                    dest[0] = src[0];
                }
                break;
                STBI__CASE(2, 3) {
                    dest[0] = dest[1] = dest[2] = src[0];
                }
                break;
                STBI__CASE(2, 4) {
                    dest[0] = dest[1] = dest[2] = src[0];
                    dest[3] = src[1];
                }
                break;
                STBI__CASE(3, 4) {
                    dest[0] = src[0];
                    dest[1] = src[1];
                    dest[2] = src[2];
                    dest[3] = 0xffff;
                }
                break;
                STBI__CASE(3, 1) {
                    dest[0] = stbi__compute_y_16(src[0], src[1], src[2]);
                }
                break;
                STBI__CASE(3, 2) {
                    dest[0] = stbi__compute_y_16(src[0], src[1], src[2]);
                    dest[1] = 0xffff;
                }
                break;
                STBI__CASE(4, 1) {
                    dest[0] = stbi__compute_y_16(src[0], src[1], src[2]);
                }
                break;
                STBI__CASE(4, 2) {
                    dest[0] = stbi__compute_y_16(src[0], src[1], src[2]);
                    dest[1] = src[3];
                }
                break;
                STBI__CASE(4, 3) {
                    dest[0] = src[0];
                    dest[1] = src[1];
                    dest[2] = src[2];
                }
                break;
            default:
                STBI_ASSERT(0);
                STBI_FREE(data);
                STBI_FREE(good);
                return (stbi__uint16 *) stbi__errpuc("unsupported", "Unsupported format conversion");
        }
#undef STBI__CASE
    }

    STBI_FREE(data);
    return good;
}
#endif

#ifndef STBI_NO_LINEAR
static float  * stbi__ldr_to_hdr(stbi_uc * data, int x, int y, int comp)
{
    int i, k, n;
    float * output;
    if(!data) return NULL;
    output = (float *) stbi__malloc_mad4(x, y, comp, sizeof(float), 0);
    if(output == NULL) {
        STBI_FREE(data);
        return stbi__errpf("outofmem", "Out of memory");
    }
    // вычислить количество неальфа-компонентов
    if(comp & 1) n = comp;
    else n = comp - 1;
    for(i = 0; i < x * y; ++i) {
        for(k = 0; k < n; ++k) {
            output[i * comp + k] = (float)(pow(data[i * comp + k] / 255.0f, stbi__l2h_gamma) * stbi__l2h_scale);
        }
    }
    if(n < comp) {
        for(i = 0; i < x * y; ++i) {
            output[i * comp + n] = data[i * comp + n] / 255.0f;
        }
    }
    STBI_FREE(data);
    return output;
}
#endif

#ifndef STBI_NO_HDR
#define stbi__float2int(x)   ((int) (x))
static stbi_uc * stbi__hdr_to_ldr(float  * data, int x, int y, int comp)
{
    int i, k, n;
    stbi_uc * output;
    if(!data) return NULL;
    output = (stbi_uc *) stbi__malloc_mad3(x, y, comp, 0);
    if(output == NULL) {
        STBI_FREE(data);
        return stbi__errpuc("outofmem", "Out of memory");
    }
    // вычислить количество неальфа-компонентов
    if(comp & 1) n = comp;
    else n = comp - 1;
    for(i = 0; i < x * y; ++i) {
        for(k = 0; k < n; ++k) {
            float z = (float) pow(data[i * comp + k] * stbi__h2l_scale_i, stbi__h2l_gamma_i) * 255 + 0.5f;
            if(z < 0) z = 0;
            if(z > 255) z = 255;
            output[i * comp + k] = (stbi_uc) stbi__float2int(z);
        }
        if(k < comp) {
            float z = data[i * comp + k] * 255 + 0.5f;
            if(z < 0) z = 0;
            if(z > 255) z = 255;
            output[i * comp + k] = (stbi_uc) stbi__float2int(z);
        }
    }
    STBI_FREE(data);
    return output;
}
#endif

//////////////////////////////////////////////////////////////////////////////
//
//  «базовый» декодер JPEG/JFIF
//
//    простая реализация
//      - doesn't support delayed output of y-dimension
//      - simple interface (only one output format: 8-bit interleaved RGB)
//      - doesn't try to recover corrupt jpegs
//      - doesn't allow partial loading, loading multiple at once
//      - still fast on x86 (copying globals into locals doesn't help x86)
//      - allocates lots of intermediate memory (full size of all components)
//        - non-interleaved case requires this anyway
//        - allows good upsampling (see next)
//    высококачественный
//      - upsampled channels are bilinearly interpolated, even across blocks
//      - quality integer IDCT derived from IJG's 'slow'
//    производительность
//      - fast huffman; reasonable integer IDCT
//      - some SIMD kernels for common paths on targets with SSE2/NEON
//      - uses a lot of intermediate memory, could cache poorly

#ifndef STBI_NO_JPEG

// ускорение декодирования Хаффмана
#define FAST_BITS   9  // больший размер позволяет обрабатывать больше дел; меньше топает меньше кэша

typedef struct {
    stbi_uc  fast[1 << FAST_BITS];
    // как ни странно, переупаковка этого в AoS - это потеря скорости на 10%, а не выигрыш
    stbi__uint16 code[256];
    stbi_uc  values[256];
    stbi_uc  size[257];
    unsigned int maxcode[18];
    int    delta[17];   // старый «firstsymbol» — старый «firstcode»
} stbi__huffman;

typedef struct {
    stbi__context * s;
    stbi__huffman huff_dc[4];
    stbi__huffman huff_ac[4];
    stbi__uint16 dequant[4][64];
    stbi__int16 fast_ac[4][1 << FAST_BITS];

    // размеры компонентов, чередующиеся микроконтроллеры
    int img_h_max, img_v_max;
    int img_mcu_x, img_mcu_y;
    int img_mcu_w, img_mcu_h;

    // определение компонента изображения jpeg
    struct {
        int id;
        int h, v;
        int tq;
        int hd, ha;
        int dc_pred;

        int x, y, w2, h2;
        stbi_uc * data;
        void * raw_data, * raw_coeff;
        stbi_uc * linebuf;
        short  * coeff;   // только прогрессивный
        int      coeff_w, coeff_h; // количество блоков коэффициентов 8x8
    } img_comp[4];

    stbi__uint32   code_buffer; // JPEG-буфер с энтропийным кодированием
    int            code_bits;   // количество действительных битов
    unsigned char  marker;      // marker seen while filling entropy buffer
    int            nomore;      // отметьте, если мы увидели маркер, поэтому должны остановиться

    int            progressive;
    int            spec_start;
    int            spec_end;
    int            succ_high;
    int            succ_low;
    int            eob_run;
    int            jfif;
    int            app14_color_transform; // Тег Adobe APP14
    int            rgb;

    int scan_n, order[4];
    int restart_interval, todo;

    // ядра
    void (*idct_block_kernel)(stbi_uc * out, int out_stride, short data[64]);
    void (*YCbCr_to_RGB_kernel)(stbi_uc * out, const stbi_uc * y, const stbi_uc * pcb, const stbi_uc * pcr, int count,
                                int step);
    stbi_uc * (*resample_row_hv_2_kernel)(stbi_uc * out, stbi_uc * in_near, stbi_uc * in_far, int w, int hs);
} stbi__jpeg;

static int stbi__build_huffman(stbi__huffman * h, int * count)
{
    int i, j, k = 0;
    unsigned int code;
    // список размеров сборки для каждого символа (из спецификации JPEG)
    for(i = 0; i < 16; ++i) {
        for(j = 0; j < count[i]; ++j) {
            h->size[k++] = (stbi_uc)(i + 1);
            if(k >= 257) return stbi__err("bad size list", "Corrupt JPEG");
        }
    }
    h->size[k] = 0;

    // вычислить фактические символы (из спецификации jpeg)
    code = 0;
    k = 0;
    for(j = 1; j <= 16; ++j) {
        // вычислить дельту, чтобы добавить ее в код для вычисления идентификатора символа
        h->delta[j] = k - code;
        if(h->size[k] == j) {
            while(h->size[k] == j)
                h->code[k++] = (stbi__uint16)(code++);
            if(code - 1 >= (1u << j)) return stbi__err("bad code lengths", "Corrupt JPEG");
        }
        // вычислить наибольший код + 1 для этого размера, предварительно сдвинутый при необходимости позже
        h->maxcode[j] = code << (16 - j);
        code <<= 1;
    }
    h->maxcode[j] = 0xffffffff;

    // построить неспецифическую таблицу ускорений; 255 — флаг неускоренного режима.
    memset(h->fast, 255, 1 << FAST_BITS);
    for(i = 0; i < k; ++i) {
        int s = h->size[i];
        if(s <= FAST_BITS) {
            int c = h->code[i] << (FAST_BITS - s);
            int m = 1 << (FAST_BITS - s);
            for(j = 0; j < m; ++j) {
                h->fast[c + j] = (stbi_uc) i;
            }
        }
    }
    return 1;
}

// построить таблицу, которая расшифровывает как величину, так и значение малых АС в
// один раз.
static void stbi__build_fast_ac(stbi__int16 * fast_ac, stbi__huffman * h)
{
    int i;
    for(i = 0; i < (1 << FAST_BITS); ++i) {
        stbi_uc fast = h->fast[i];
        fast_ac[i] = 0;
        if(fast < 255) {
            int rs = h->values[fast];
            int run = (rs >> 4) & 15;
            int magbits = rs & 15;
            int len = h->size[fast];

            if(magbits && len + magbits <= FAST_BITS) {
                // код величины, за которым следует код receive_extend
                int k = ((i << len) & ((1 << FAST_BITS) - 1)) >> (FAST_BITS - magbits);
                int m = 1 << (magbits - 1);
                if(k < m) k += (~0U << magbits) + 1;
                // если результат достаточно мал, мы можем поместить его в таблицу fast_ac
                if(k >= -128 && k <= 127)
                    fast_ac[i] = (stbi__int16)((k * 256) + (run * 16) + (len + magbits));
            }
        }
    }
}

static void stbi__grow_buffer_unsafe(stbi__jpeg * j)
{
    do {
        unsigned int b = j->nomore ? 0 : stbi__get8(j->s);
        if(b == 0xff) {
            int c = stbi__get8(j->s);
            while(c == 0xff) c = stbi__get8(j->s);  // потреблять байты заполнения
            if(c != 0) {
                j->marker = (unsigned char) c;
                j->nomore = 1;
                return;
            }
        }
        j->code_buffer |= b << (24 - j->code_bits);
        j->code_bits += 8;
    } while(j->code_bits <= 24);
}

// (1 << n) - 1
static const stbi__uint32 stbi__bmask[17] = {0, 1, 3, 7, 15, 31, 63, 127, 255, 511, 1023, 2047, 4095, 8191, 16383, 32767, 65535};

// декодировать значение Хаффмана в формате JPEG из битового потока
stbi_inline static int stbi__jpeg_huff_decode(stbi__jpeg * j, stbi__huffman * h)
{
    unsigned int temp;
    int c, k;

    if(j->code_bits < 16) stbi__grow_buffer_unsafe(j);

    // посмотрите на верхний FAST_BITS и определите, что это за символ ID,
    // если код <= FAST_BITS
    c = (j->code_buffer >> (32 - FAST_BITS)) & ((1 << FAST_BITS) - 1);
    k = h->fast[c];
    if(k < 255) {
        int s = h->size[k];
        if(s > j->code_bits)
            return -1;
        j->code_buffer <<= s;
        j->code_bits -= s;
        return h->values[k];
    }

    // наивный тест состоит в том, чтобы сдвинуть code_buffer вниз, чтобы k бит были
    // действительный, затем проверьте его на соответствие maxcode. Чтобы ускорить это, мы
    // предварительно сдвинутый maxcode влево, чтобы в его начале было (16-k) 0
    // конец; другими словами, независимо от количества битов, это
    // хочет, чтобы его сравнили с чем-то, измененным на 16;
    // таким образом нам не нужно перемещаться внутри цикла.
    temp = j->code_buffer >> 16;
    for(k = FAST_BITS + 1 ; ; ++k)
        if(temp < h->maxcode[k])
            break;
    if(k == 17) {
        // ошибка! код не найден
        j->code_bits -= 16;
        return -1;
    }

    if(k > j->code_bits)
        return -1;

    // преобразовать код Хаффмана в идентификатор символа
    c = ((j->code_buffer >> (32 - k)) & stbi__bmask[k]) + h->delta[k];
    if(c < 0 || c >= 256) // Идентификатор символа выходит за пределы!
        return -1;
    STBI_ASSERT((((j->code_buffer) >> (32 - h->size[c])) & stbi__bmask[h->size[c]]) == h->code[c]);

    // преобразовать идентификатор в символ
    j->code_bits -= k;
    j->code_buffer <<= k;
    return h->values[c];
}

// смещение[n] = (-1<<n) + 1
static const int stbi__jbias[16] = {0, -1, -3, -7, -15, -31, -63, -127, -255, -511, -1023, -2047, -4095, -8191, -16383, -32767};

// объединенные JPEG «получать» и JPEG «расширять», начиная с базовой линии
// всегда расширяет все, что получает.
stbi_inline static int stbi__extend_receive(stbi__jpeg * j, int n)
{
    unsigned int k;
    int sgn;
    if(j->code_bits < n) stbi__grow_buffer_unsafe(j);
    if(j->code_bits < n) return 0;  // закончились биты из потока, верните 0 вместо продолжения

    sgn = j->code_buffer >> 31; // знаковый бит всегда в MSB ; 0, если MSB сброшен (положительный), 1, если MSB установлен (отрицательный).
    k = stbi_lrot(j->code_buffer, n);
    j->code_buffer = k & ~stbi__bmask[n];
    k &= stbi__bmask[n];
    j->code_bits -= n;
    return k + (stbi__jbias[n] & (sgn - 1));
}

// получить несколько беззнаковых битов
stbi_inline static int stbi__jpeg_get_bits(stbi__jpeg * j, int n)
{
    unsigned int k;
    if(j->code_bits < n) stbi__grow_buffer_unsafe(j);
    if(j->code_bits < n) return 0;  // закончились биты из потока, верните 0 вместо продолжения
    k = stbi_lrot(j->code_buffer, n);
    j->code_buffer = k & ~stbi__bmask[n];
    k &= stbi__bmask[n];
    j->code_bits -= n;
    return k;
}

stbi_inline static int stbi__jpeg_get_bit(stbi__jpeg * j)
{
    unsigned int k;
    if(j->code_bits < 1) stbi__grow_buffer_unsafe(j);
    if(j->code_bits < 1) return 0;  // закончились биты из потока, верните 0 вместо продолжения
    k = j->code_buffer;
    j->code_buffer <<= 1;
    --j->code_bits;
    return k & 0x80000000;
}

// учитывая значение, которое находится в позиции X в зигзагообразном потоке,
// where does it appear in the 8x8 matrix coded as row-major?
static const stbi_uc stbi__jpeg_dezigzag[64 + 15] = {
    0,  1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63,
    // пусть испорченный входной образец пройдет мимо конца
    63, 63, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 63, 63, 63, 63
};

// декодировать один блок из 64 записей --
static int stbi__jpeg_decode_block(stbi__jpeg * j, short data[64], stbi__huffman * hdc, stbi__huffman * hac,
                                   stbi__int16 * fac, int b, stbi__uint16 * dequant)
{
    int diff, dc, k;
    int t;

    if(j->code_bits < 16) stbi__grow_buffer_unsafe(j);
    t = stbi__jpeg_huff_decode(j, hdc);
    if(t < 0 || t > 15) return stbi__err("bad huffman code", "Corrupt JPEG");

    // 0 всех значений переменного тока, поэтому мы можем делать это по 32 бита за раз
    memset(data, 0, 64 * sizeof(data[0]));

    diff = t ? stbi__extend_receive(j, t) : 0;
    if(!stbi__addints_valid(j->img_comp[b].dc_pred, diff)) return stbi__err("bad delta", "Corrupt JPEG");
    dc = j->img_comp[b].dc_pred + diff;
    j->img_comp[b].dc_pred = dc;
    if(!stbi__mul2shorts_valid(dc, dequant[0])) return stbi__err("can't merge dc and ac", "Corrupt JPEG");
    data[0] = (short)(dc * dequant[0]);

    // декодировать компоненты AC, см. спецификацию JPEG
    k = 1;
    do {
        unsigned int zig;
        int c, r, s;
        if(j->code_bits < 16) stbi__grow_buffer_unsafe(j);
        c = (j->code_buffer >> (32 - FAST_BITS)) & ((1 << FAST_BITS) - 1);
        r = fac[c];
        if(r) {  // быстрый путь AC
            k += (r >> 4) & 15; // бежать
            s = r & 15; // общая длина
            if(s > j->code_bits) return stbi__err("bad huffman code", "Combined length longer than code bits available");
            j->code_buffer <<= s;
            j->code_bits -= s;
            // декодировать в не зигзагообразное место
            zig = stbi__jpeg_dezigzag[k++];
            data[zig] = (short)((r >> 8) * dequant[zig]);
        }
        else {
            int rs = stbi__jpeg_huff_decode(j, hac);
            if(rs < 0) return stbi__err("bad huffman code", "Corrupt JPEG");
            s = rs & 15;
            r = rs >> 4;
            if(s == 0) {
                if(rs != 0xf0) break;  // конечный блок
                k += 16;
            }
            else {
                k += r;
                // декодировать в не зигзагообразное место
                zig = stbi__jpeg_dezigzag[k++];
                data[zig] = (short)(stbi__extend_receive(j, s) * dequant[zig]);
            }
        }
    } while(k < 64);
    return 1;
}

static int stbi__jpeg_decode_block_prog_dc(stbi__jpeg * j, short data[64], stbi__huffman * hdc, int b)
{
    int diff, dc;
    int t;
    if(j->spec_end != 0) return stbi__err("can't merge dc and ac", "Corrupt JPEG");

    if(j->code_bits < 16) stbi__grow_buffer_unsafe(j);

    if(j->succ_high == 0) {
        // первое сканирование коэффициента DC, должно быть первым
        memset(data, 0, 64 * sizeof(data[0])); // 0 все значения переменного тока сейчас
        t = stbi__jpeg_huff_decode(j, hdc);
        if(t < 0 || t > 15) return stbi__err("can't merge dc and ac", "Corrupt JPEG");
        diff = t ? stbi__extend_receive(j, t) : 0;

        if(!stbi__addints_valid(j->img_comp[b].dc_pred, diff)) return stbi__err("bad delta", "Corrupt JPEG");
        dc = j->img_comp[b].dc_pred + diff;
        j->img_comp[b].dc_pred = dc;
        if(!stbi__mul2shorts_valid(dc, 1 << j->succ_low)) return stbi__err("can't merge dc and ac", "Corrupt JPEG");
        data[0] = (short)(dc * (1 << j->succ_low));
    }
    else {
        // уточнение сканирования коэффициента DC
        if(stbi__jpeg_get_bit(j))
            data[0] += (short)(1 << j->succ_low);
    }
    return 1;
}

// @OPTIMIZE : хранить без зигзагов во время проходов декодирования,
// и только дезигзаг при деквантовании
static int stbi__jpeg_decode_block_prog_ac(stbi__jpeg * j, short data[64], stbi__huffman * hac, stbi__int16 * fac)
{
    int k;
    if(j->spec_start == 0) return stbi__err("can't merge dc and ac", "Corrupt JPEG");

    if(j->succ_high == 0) {
        int shift = j->succ_low;

        if(j->eob_run) {
            --j->eob_run;
            return 1;
        }

        k = j->spec_start;
        do {
            unsigned int zig;
            int c, r, s;
            if(j->code_bits < 16) stbi__grow_buffer_unsafe(j);
            c = (j->code_buffer >> (32 - FAST_BITS)) & ((1 << FAST_BITS) - 1);
            r = fac[c];
            if(r) {  // быстрый путь AC
                k += (r >> 4) & 15; // бежать
                s = r & 15; // общая длина
                if(s > j->code_bits) return stbi__err("bad huffman code", "Combined length longer than code bits available");
                j->code_buffer <<= s;
                j->code_bits -= s;
                zig = stbi__jpeg_dezigzag[k++];
                data[zig] = (short)((r >> 8) * (1 << shift));
            }
            else {
                int rs = stbi__jpeg_huff_decode(j, hac);
                if(rs < 0) return stbi__err("bad huffman code", "Corrupt JPEG");
                s = rs & 15;
                r = rs >> 4;
                if(s == 0) {
                    if(r < 15) {
                        j->eob_run = (1 << r);
                        if(r)
                            j->eob_run += stbi__jpeg_get_bits(j, r);
                        --j->eob_run;
                        break;
                    }
                    k += 16;
                }
                else {
                    k += r;
                    zig = stbi__jpeg_dezigzag[k++];
                    data[zig] = (short)(stbi__extend_receive(j, s) * (1 << shift));
                }
            }
        } while(k <= j->spec_end);
    }
    else {
        // уточнение сканирования для этих коэффициентов AC

        short bit = (short)(1 << j->succ_low);

        if(j->eob_run) {
            --j->eob_run;
            for(k = j->spec_start; k <= j->spec_end; ++k) {
                short * p = &data[stbi__jpeg_dezigzag[k]];
                if(*p != 0)
                    if(stbi__jpeg_get_bit(j))
                        if((*p & bit) == 0) {
                            if(*p > 0)
                                *p += bit;
                            else
                                *p -= bit;
                        }
            }
        }
        else {
            k = j->spec_start;
            do {
                int r, s;
                int rs = stbi__jpeg_huff_decode(j, hac); // @OPTIMIZE посмотрим, сможем ли мы использовать здесь быстрый путь, продвижение по r очень медленное, а
                if(rs < 0) return stbi__err("bad huffman code", "Corrupt JPEG");
                s = rs & 15;
                r = rs >> 4;
                if(s == 0) {
                    if(r < 15) {
                        j->eob_run = (1 << r) - 1;
                        if(r)
                            j->eob_run += stbi__jpeg_get_bits(j, r);
                        r = 64; // принудительное завершение блока
                    }
                    else {
                        // r=15 s=0 должно записать 16 0, поэтому мы просто делаем
                        // серию из 15 0, а затем напишите s (что равно 0),
                        // поэтому нам не нужно делать здесь ничего особенного
                    }
                }
                else {
                    if(s != 1) return stbi__err("bad huffman code", "Corrupt JPEG");
                    // знаковый бит
                    if(stbi__jpeg_get_bit(j))
                        s = bit;
                    else
                        s = -bit;
                }

                // вперед на r
                while(k <= j->spec_end) {
                    short * p = &data[stbi__jpeg_dezigzag[k++]];
                    if(*p != 0) {
                        if(stbi__jpeg_get_bit(j))
                            if((*p & bit) == 0) {
                                if(*p > 0)
                                    *p += bit;
                                else
                                    *p -= bit;
                            }
                    }
                    else {
                        if(r == 0) {
                            *p = (short) s;
                            break;
                        }
                        --r;
                    }
                }
            } while(k <= j->spec_end);
        }
    }
    return 1;
}

// возьмите значение -128..127, stbi__clamp и преобразуйте его в 0..255.
stbi_inline static stbi_uc stbi__clamp(int x)
{
    // трюк, позволяющий использовать один тест для выявления обоих случаев
    if((unsigned int) x > 255) {
        if(x < 0) return 0;
        if(x > 255) return 255;
    }
    return (stbi_uc) x;
}

#define stbi__f2f(x)  ((int) (((x) * 4096 + 0.5)))
#define stbi__fsh(x)  ((x) * 4096)

// получено из jidctint -- DCT_ISLOW
#define STBI__IDCT_1D(s0,s1,s2,s3,s4,s5,s6,s7) \
    int t0,t1,t2,t3,p1,p2,p3,p4,p5,x0,x1,x2,x3; \
    p2 = s2;                                    \
    p3 = s6;                                    \
    p1 = (p2+p3) * stbi__f2f(0.5411961f);       \
    t2 = p1 + p3*stbi__f2f(-1.847759065f);      \
    t3 = p1 + p2*stbi__f2f( 0.765366865f);      \
    p2 = s0;                                    \
    p3 = s4;                                    \
    t0 = stbi__fsh(p2+p3);                      \
    t1 = stbi__fsh(p2-p3);                      \
    x0 = t0+t3;                                 \
    x3 = t0-t3;                                 \
    x1 = t1+t2;                                 \
    x2 = t1-t2;                                 \
    t0 = s7;                                    \
    t1 = s5;                                    \
    t2 = s3;                                    \
    t3 = s1;                                    \
    p3 = t0+t2;                                 \
    p4 = t1+t3;                                 \
    p1 = t0+t3;                                 \
    p2 = t1+t2;                                 \
    p5 = (p3+p4)*stbi__f2f( 1.175875602f);      \
    t0 = t0*stbi__f2f( 0.298631336f);           \
    t1 = t1*stbi__f2f( 2.053119869f);           \
    t2 = t2*stbi__f2f( 3.072711026f);           \
    t3 = t3*stbi__f2f( 1.501321110f);           \
    p1 = p5 + p1*stbi__f2f(-0.899976223f);      \
    p2 = p5 + p2*stbi__f2f(-2.562915447f);      \
    p3 = p3*stbi__f2f(-1.961570560f);           \
    p4 = p4*stbi__f2f(-0.390180644f);           \
    t3 += p1+p4;                                \
    t2 += p2+p3;                                \
    t1 += p2+p4;                                \
    t0 += p1+p3;

static void stbi__idct_block(stbi_uc * out, int out_stride, short data[64])
{
    int i, val[64], * v = val;
    stbi_uc * o;
    short * d = data;

    // столбцы
    for(i = 0; i < 8; ++i, ++d, ++v) {
        // если все нули, используйте ярлык - это позволяет избежать деквантования нулей и IDCT
        if(d[ 8] == 0 && d[16] == 0 && d[24] == 0 && d[32] == 0
           && d[40] == 0 && d[48] == 0 && d[56] == 0) {
            //    нет ярлыка 0 секунд
            //    (1|2|3|4|5|6|7)==0 0 секунд
            //    все отдельно -0,047 секунды
            //    1 && 2|3 && 4|5 && 6|7: -0,047 секунды
            int dcterm = d[0] * 4;
            v[0] = v[8] = v[16] = v[24] = v[32] = v[40] = v[48] = v[56] = dcterm;
        }
        else {
            STBI__IDCT_1D(d[ 0], d[ 8], d[16], d[24], d[32], d[40], d[48], d[56])
            // константы увеличивали масштаб на 1<<12; давай вернем их обратно
            // вниз, но сохраняйте 2 дополнительных бита точности
            x0 += 512;
            x1 += 512;
            x2 += 512;
            x3 += 512;
            v[ 0] = (x0 + t3) >> 10;
            v[56] = (x0 - t3) >> 10;
            v[ 8] = (x1 + t2) >> 10;
            v[48] = (x1 - t2) >> 10;
            v[16] = (x2 + t1) >> 10;
            v[40] = (x2 - t1) >> 10;
            v[24] = (x3 + t0) >> 10;
            v[32] = (x3 - t0) >> 10;
        }
    }

    for(i = 0, v = val, o = out; i < 8; ++i, v += 8, o += out_stride) {
        // нет быстрого случая, поскольку первые 1D IDCT разложили компоненты
        STBI__IDCT_1D(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7])
        // константы увеличивали масштаб на 1<<12, плюс у нас было 1<<2 с самого начала
        // цикл, плюс каждое масштабирование по горизонтали и вертикали с помощью sqrt(8), так что вместе
        // у нас есть лишние 1<<3, поэтому всего 1<<17 нам нужно удалить.
        // поэтому мы хотим округлить это значение, что означает добавление 0,5 * 1<<17,
        // он же 65536. Кроме того, в итоге мы получим желаемые значения от -128 до 127.
        // для кодирования как 0..255, добавив 128, поэтому мы добавим это перед сдвигом
        x0 += 65536 + (128 << 17);
        x1 += 65536 + (128 << 17);
        x2 += 65536 + (128 << 17);
        x3 += 65536 + (128 << 17);
        // попробовал вычислить сдвиги в темпах или посмотреть темпы, чтобы увидеть
        // если бы кто-то был вне зоны досягаемости, но это было медленнее
        o[0] = stbi__clamp((x0 + t3) >> 17);
        o[7] = stbi__clamp((x0 - t3) >> 17);
        o[1] = stbi__clamp((x1 + t2) >> 17);
        o[6] = stbi__clamp((x1 - t2) >> 17);
        o[2] = stbi__clamp((x2 + t1) >> 17);
        o[5] = stbi__clamp((x2 - t1) >> 17);
        o[3] = stbi__clamp((x3 + t0) >> 17);
        o[4] = stbi__clamp((x3 - t0) >> 17);
    }
}

#ifdef STBI_SSE2
// sse2 целое число IDCT . не самая быстрая возможная реализация, но это
// выдает результаты, идентичные общей версии C, поэтому
// полностью «прозрачный».
static void stbi__idct_simd(stbi_uc * out, int out_stride, short data[64])
{
    // Оно создано так, чтобы точно соответствовать нашему обычному (общему) целому числу IDCT.
    __m128i row0, row1, row2, row3, row4, row5, row6, row7;
    __m128i tmp;

    // Константа скалярного произведения: четные элементы = x, нечетные элементы = y
#define dct_const(x,y)  _mm_setr_epi16((x),(y),(x),(y),(x),(y),(x),(y))

    // out(0) = c0[чётный]*x + c0[нечетный]*y (c0, x, y 16 бит, out 32 бита)
    // out(1) = c1[чётный]*x + c1[нечетный]*y
#define dct_rot(out0,out1, x,y,c0,c1) \
    __m128i c0##lo = _mm_unpacklo_epi16((x),(y)); \
    __m128i c0##hi = _mm_unpackhi_epi16((x),(y)); \
    __m128i out0##_l = _mm_madd_epi16(c0##lo, c0); \
    __m128i out0##_h = _mm_madd_epi16(c0##hi, c0); \
    __m128i out1##_l = _mm_madd_epi16(c0##lo, c1); \
    __m128i out1##_h = _mm_madd_epi16(c0##hi, c1)

    // out = in << 12  (in 16-bit, out 32-bit)
#define dct_widen(out, in) \
    __m128i out##_l = _mm_srai_epi32(_mm_unpacklo_epi16(_mm_setzero_si128(), (in)), 4); \
    __m128i out##_h = _mm_srai_epi32(_mm_unpackhi_epi16(_mm_setzero_si128(), (in)), 4)

    // широкое дополнение
#define dct_wadd(out, a, b) \
    __m128i out##_l = _mm_add_epi32(a##_l, b##_l); \
    __m128i out##_h = _mm_add_epi32(a##_h, b##_h)

    // широкий сабвуфер
#define dct_wsub(out, a, b) \
    __m128i out##_l = _mm_sub_epi32(a##_l, b##_l); \
    __m128i out##_h = _mm_sub_epi32(a##_h, b##_h)

    // бабочка a/b, добавьте уклон, затем сдвиньте на «s» и упакуйте
#define dct_bfly32o(out0, out1, a,b,bias,s) \
    { \
        __m128i abiased_l = _mm_add_epi32(a##_l, bias); \
        __m128i abiased_h = _mm_add_epi32(a##_h, bias); \
        dct_wadd(sum, abiased, b); \
        dct_wsub(dif, abiased, b); \
        out0 = _mm_packs_epi32(_mm_srai_epi32(sum_l, s), _mm_srai_epi32(sum_h, s)); \
        out1 = _mm_packs_epi32(_mm_srai_epi32(dif_l, s), _mm_srai_epi32(dif_h, s)); \
    }

    // 8-битный шаг чередования (для транспонирования)
#define dct_interleave8(a, b) \
    tmp = a; \
    a = _mm_unpacklo_epi8(a, b); \
    b = _mm_unpackhi_epi8(tmp, b)

    // 16-битный шаг чередования (для транспонирования)
#define dct_interleave16(a, b) \
    tmp = a; \
    a = _mm_unpacklo_epi16(a, b); \
    b = _mm_unpackhi_epi16(tmp, b)

#define dct_pass(bias,shift) \
    { \
        /* даже часть */ \
        dct_rot(t2e,t3e, row2,row6, rot0_0,rot0_1); \
        __m128i sum04 = _mm_add_epi16(row0, row4); \
        __m128i dif04 = _mm_sub_epi16(row0, row4); \
        dct_widen(t0e, sum04); \
        dct_widen(t1e, dif04); \
        dct_wadd(x0, t0e, t3e); \
        dct_wsub(x3, t0e, t3e); \
        dct_wadd(x1, t1e, t2e); \
        dct_wsub(x2, t1e, t2e); \
        /* нечетная часть */ \
        dct_rot(y0o,y2o, row7,row3, rot2_0,rot2_1); \
        dct_rot(y1o,y3o, row5,row1, rot3_0,rot3_1); \
        __m128i sum17 = _mm_add_epi16(row1, row7); \
        __m128i sum35 = _mm_add_epi16(row3, row5); \
        dct_rot(y4o,y5o, sum17,sum35, rot1_0,rot1_1); \
        dct_wadd(x4, y0o, y4o); \
        dct_wadd(x5, y1o, y5o); \
        dct_wadd(x6, y2o, y5o); \
        dct_wadd(x7, y3o, y4o); \
        dct_bfly32o(row0,row7, x0,x7,bias,shift); \
        dct_bfly32o(row1,row6, x1,x6,bias,shift); \
        dct_bfly32o(row2,row5, x2,x5,bias,shift); \
        dct_bfly32o(row3,row4, x3,x4,bias,shift); \
    }

    __m128i rot0_0 = dct_const(stbi__f2f(0.5411961f), stbi__f2f(0.5411961f) + stbi__f2f(-1.847759065f));
    __m128i rot0_1 = dct_const(stbi__f2f(0.5411961f) + stbi__f2f(0.765366865f), stbi__f2f(0.5411961f));
    __m128i rot1_0 = dct_const(stbi__f2f(1.175875602f) + stbi__f2f(-0.899976223f), stbi__f2f(1.175875602f));
    __m128i rot1_1 = dct_const(stbi__f2f(1.175875602f), stbi__f2f(1.175875602f) + stbi__f2f(-2.562915447f));
    __m128i rot2_0 = dct_const(stbi__f2f(-1.961570560f) + stbi__f2f(0.298631336f), stbi__f2f(-1.961570560f));
    __m128i rot2_1 = dct_const(stbi__f2f(-1.961570560f), stbi__f2f(-1.961570560f) + stbi__f2f(3.072711026f));
    __m128i rot3_0 = dct_const(stbi__f2f(-0.390180644f) + stbi__f2f(2.053119869f), stbi__f2f(-0.390180644f));
    __m128i rot3_1 = dct_const(stbi__f2f(-0.390180644f), stbi__f2f(-0.390180644f) + stbi__f2f(1.501321110f));

    // смещения округления при проходах по столбцу/строке, пояснения см. в stbi__idct_block.
    __m128i bias_0 = _mm_set1_epi32(512);
    __m128i bias_1 = _mm_set1_epi32(65536 + (128 << 17));

    // нагрузка
    row0 = _mm_load_si128((const __m128i *)(data + 0 * 8));
    row1 = _mm_load_si128((const __m128i *)(data + 1 * 8));
    row2 = _mm_load_si128((const __m128i *)(data + 2 * 8));
    row3 = _mm_load_si128((const __m128i *)(data + 3 * 8));
    row4 = _mm_load_si128((const __m128i *)(data + 4 * 8));
    row5 = _mm_load_si128((const __m128i *)(data + 5 * 8));
    row6 = _mm_load_si128((const __m128i *)(data + 6 * 8));
    row7 = _mm_load_si128((const __m128i *)(data + 7 * 8));

    // проход колонны
    dct_pass(bias_0, 10);

    {
        // Транспонирование 16 бит 8x8, проход 1
        dct_interleave16(row0, row4);
        dct_interleave16(row1, row5);
        dct_interleave16(row2, row6);
        dct_interleave16(row3, row7);

        // транспонировать проход 2
        dct_interleave16(row0, row2);
        dct_interleave16(row1, row3);
        dct_interleave16(row4, row6);
        dct_interleave16(row5, row7);

        // транспонировать проход 3
        dct_interleave16(row0, row1);
        dct_interleave16(row2, row3);
        dct_interleave16(row4, row5);
        dct_interleave16(row6, row7);
    }

    // пропуск по ряду
    dct_pass(bias_1, 17);

    {
        // упаковка
        __m128i p0 = _mm_packus_epi16(row0, row1); // a0a1a2a3...a7b0b1b2b3...b7
        __m128i p1 = _mm_packus_epi16(row2, row3);
        __m128i p2 = _mm_packus_epi16(row4, row5);
        __m128i p3 = _mm_packus_epi16(row6, row7);

        // 8-битное транспонирование 8x8, проход 1
        dct_interleave8(p0, p2); // a0e0a1e1...
        dct_interleave8(p1, p3); // c0g0c1g1...

        // транспонировать проход 2
        dct_interleave8(p0, p1); // a0c0e0g0...
        dct_interleave8(p2, p3); // b0d0f0h0...

        // транспонировать проход 3
        dct_interleave8(p0, p2); // a0b0c0d0...
        dct_interleave8(p1, p3); // a4b4c4d4...

        // магазин
        _mm_storel_epi64((__m128i *) out, p0);
        out += out_stride;
        _mm_storel_epi64((__m128i *) out, _mm_shuffle_epi32(p0, 0x4e));
        out += out_stride;
        _mm_storel_epi64((__m128i *) out, p2);
        out += out_stride;
        _mm_storel_epi64((__m128i *) out, _mm_shuffle_epi32(p2, 0x4e));
        out += out_stride;
        _mm_storel_epi64((__m128i *) out, p1);
        out += out_stride;
        _mm_storel_epi64((__m128i *) out, _mm_shuffle_epi32(p1, 0x4e));
        out += out_stride;
        _mm_storel_epi64((__m128i *) out, p3);
        out += out_stride;
        _mm_storel_epi64((__m128i *) out, _mm_shuffle_epi32(p3, 0x4e));
    }

#undef dct_const
#undef dct_rot
#undef dct_widen
#undef dct_wadd
#undef dct_wsub
#undef dct_bfly32o
#undef dct_interleave8
#undef dct_interleave16
#undef dct_pass
}

#endif // STBI_SSE2

#ifdef STBI_NEON

// NEON целое число IDCT . должен выдавать бит-идентичный
// результаты к общей версии C.
static void stbi__idct_simd(stbi_uc * out, int out_stride, short data[64])
{
    int16x8_t row0, row1, row2, row3, row4, row5, row6, row7;

    int16x4_t rot0_0 = vdup_n_s16(stbi__f2f(0.5411961f));
    int16x4_t rot0_1 = vdup_n_s16(stbi__f2f(-1.847759065f));
    int16x4_t rot0_2 = vdup_n_s16(stbi__f2f(0.765366865f));
    int16x4_t rot1_0 = vdup_n_s16(stbi__f2f(1.175875602f));
    int16x4_t rot1_1 = vdup_n_s16(stbi__f2f(-0.899976223f));
    int16x4_t rot1_2 = vdup_n_s16(stbi__f2f(-2.562915447f));
    int16x4_t rot2_0 = vdup_n_s16(stbi__f2f(-1.961570560f));
    int16x4_t rot2_1 = vdup_n_s16(stbi__f2f(-0.390180644f));
    int16x4_t rot3_0 = vdup_n_s16(stbi__f2f(0.298631336f));
    int16x4_t rot3_1 = vdup_n_s16(stbi__f2f(2.053119869f));
    int16x4_t rot3_2 = vdup_n_s16(stbi__f2f(3.072711026f));
    int16x4_t rot3_3 = vdup_n_s16(stbi__f2f(1.501321110f));

#define dct_long_mul(out, inq, coeff) \
    int32x4_t out##_l = vmull_s16(vget_low_s16(inq), coeff); \
    int32x4_t out##_h = vmull_s16(vget_high_s16(inq), coeff)

#define dct_long_mac(out, acc, inq, coeff) \
    int32x4_t out##_l = vmlal_s16(acc##_l, vget_low_s16(inq), coeff); \
    int32x4_t out##_h = vmlal_s16(acc##_h, vget_high_s16(inq), coeff)

#define dct_widen(out, inq) \
    int32x4_t out##_l = vshll_n_s16(vget_low_s16(inq), 12); \
    int32x4_t out##_h = vshll_n_s16(vget_high_s16(inq), 12)

    // широкое дополнение
#define dct_wadd(out, a, b) \
    int32x4_t out##_l = vaddq_s32(a##_l, b##_l); \
    int32x4_t out##_h = vaddq_s32(a##_h, b##_h)

    // широкий сабвуфер
#define dct_wsub(out, a, b) \
    int32x4_t out##_l = vsubq_s32(a##_l, b##_l); \
    int32x4_t out##_h = vsubq_s32(a##_h, b##_h)

    // бабочка a/b, затем переключитесь с помощью «shifttop» на «s» и упакуйте
#define dct_bfly32o(out0,out1, a,b,shiftop,s) \
    { \
        dct_wadd(sum, a, b); \
        dct_wsub(dif, a, b); \
        out0 = vcombine_s16(shiftop(sum_l, s), shiftop(sum_h, s)); \
        out1 = vcombine_s16(shiftop(dif_l, s), shiftop(dif_h, s)); \
    }

#define dct_pass(shiftop, shift) \
    { \
        /* даже часть */ \
        int16x8_t sum26 = vaddq_s16(row2, row6); \
        dct_long_mul(p1e, sum26, rot0_0); \
        dct_long_mac(t2e, p1e, row6, rot0_1); \
        dct_long_mac(t3e, p1e, row2, rot0_2); \
        int16x8_t sum04 = vaddq_s16(row0, row4); \
        int16x8_t dif04 = vsubq_s16(row0, row4); \
        dct_widen(t0e, sum04); \
        dct_widen(t1e, dif04); \
        dct_wadd(x0, t0e, t3e); \
        dct_wsub(x3, t0e, t3e); \
        dct_wadd(x1, t1e, t2e); \
        dct_wsub(x2, t1e, t2e); \
        /* нечетная часть */ \
        int16x8_t sum15 = vaddq_s16(row1, row5); \
        int16x8_t sum17 = vaddq_s16(row1, row7); \
        int16x8_t sum35 = vaddq_s16(row3, row5); \
        int16x8_t sum37 = vaddq_s16(row3, row7); \
        int16x8_t sumodd = vaddq_s16(sum17, sum35); \
        dct_long_mul(p5o, sumodd, rot1_0); \
        dct_long_mac(p1o, p5o, sum17, rot1_1); \
        dct_long_mac(p2o, p5o, sum35, rot1_2); \
        dct_long_mul(p3o, sum37, rot2_0); \
        dct_long_mul(p4o, sum15, rot2_1); \
        dct_wadd(sump13o, p1o, p3o); \
        dct_wadd(sump24o, p2o, p4o); \
        dct_wadd(sump23o, p2o, p3o); \
        dct_wadd(sump14o, p1o, p4o); \
        dct_long_mac(x4, sump13o, row7, rot3_0); \
        dct_long_mac(x5, sump24o, row5, rot3_1); \
        dct_long_mac(x6, sump23o, row3, rot3_2); \
        dct_long_mac(x7, sump14o, row1, rot3_3); \
        dct_bfly32o(row0,row7, x0,x7,shiftop,shift); \
        dct_bfly32o(row1,row6, x1,x6,shiftop,shift); \
        dct_bfly32o(row2,row5, x2,x5,shiftop,shift); \
        dct_bfly32o(row3,row4, x3,x4,shiftop,shift); \
    }

    // нагрузка
    row0 = vld1q_s16(data + 0 * 8);
    row1 = vld1q_s16(data + 1 * 8);
    row2 = vld1q_s16(data + 2 * 8);
    row3 = vld1q_s16(data + 3 * 8);
    row4 = vld1q_s16(data + 4 * 8);
    row5 = vld1q_s16(data + 5 * 8);
    row6 = vld1q_s16(data + 6 * 8);
    row7 = vld1q_s16(data + 7 * 8);

    // добавить смещение DC
    row0 = vaddq_s16(row0, vsetq_lane_s16(1024, vdupq_n_s16(0), 0));

    // проход колонны
    dct_pass(vrshrn_n_s32, 10);

    // Транспонирование 16 бит 8x8
    {
        // эти три сопоставляются с одним VTRN .16, VTRN .32 и VSWP соответственно.
        // К сожалению, поймут ли это на самом деле компиляторы, это уже другая история.
#define dct_trn16(x, y) { int16x8x2_t t = vtrnq_s16(x, y); x = t.val[0]; y = t.val[1]; }
#define dct_trn32(x, y) { int32x4x2_t t = vtrnq_s32(vreinterpretq_s32_s16(x), vreinterpretq_s32_s16(y)); x = vreinterpretq_s16_s32(t.val[0]); y = vreinterpretq_s16_s32(t.val[1]); }
#define dct_trn64(x, y) { int16x8_t x0 = x; int16x8_t y0 = y; x = vcombine_s16(vget_low_s16(x0), vget_low_s16(y0)); y = vcombine_s16(vget_high_s16(x0), vget_high_s16(y0)); }

        // пройти 1
        dct_trn16(row0, row1); // a0b0a2b2a4b4a6b6
        dct_trn16(row2, row3);
        dct_trn16(row4, row5);
        dct_trn16(row6, row7);

        // пройти 2
        dct_trn32(row0, row2); // a0b0c0d0a4b4c4d4
        dct_trn32(row1, row3);
        dct_trn32(row4, row6);
        dct_trn32(row5, row7);

        // пройти 3
        dct_trn64(row0, row4); // a0b0c0d0e0f0g0h0
        dct_trn64(row1, row5);
        dct_trn64(row2, row6);
        dct_trn64(row3, row7);

#undef dct_trn16
#undef dct_trn32
#undef dct_trn64
    }

    // пропуск по ряду
    // vrshrn_n_s32 поддерживает только смены до 16, нам нужно
    // 17. so do a non-rounding shift of 16 first then follow
    // вверх с округлением на 1.
    dct_pass(vshrn_n_s32, 16);

    {
        // упаковать и закруглить
        uint8x8_t p0 = vqrshrun_n_s16(row0, 1);
        uint8x8_t p1 = vqrshrun_n_s16(row1, 1);
        uint8x8_t p2 = vqrshrun_n_s16(row2, 1);
        uint8x8_t p3 = vqrshrun_n_s16(row3, 1);
        uint8x8_t p4 = vqrshrun_n_s16(row4, 1);
        uint8x8_t p5 = vqrshrun_n_s16(row5, 1);
        uint8x8_t p6 = vqrshrun_n_s16(row6, 1);
        uint8x8_t p7 = vqrshrun_n_s16(row7, 1);

        // опять же, их можно преобразовать в одну инструкцию, но часто этого не происходит.
#define dct_trn8_8(x, y) { uint8x8x2_t t = vtrn_u8(x, y); x = t.val[0]; y = t.val[1]; }
#define dct_trn8_16(x, y) { uint16x4x2_t t = vtrn_u16(vreinterpret_u16_u8(x), vreinterpret_u16_u8(y)); x = vreinterpret_u8_u16(t.val[0]); y = vreinterpret_u8_u16(t.val[1]); }
#define dct_trn8_32(x, y) { uint32x2x2_t t = vtrn_u32(vreinterpret_u32_u8(x), vreinterpret_u32_u8(y)); x = vreinterpret_u8_u32(t.val[0]); y = vreinterpret_u8_u32(t.val[1]); }

        // к сожалению, здесь нельзя использовать чередующиеся хранилища, поскольку мы пишем только
        // 8 байт на каждую строку сканирования!

        // 8x8 8-битный проход транспонирования 1
        dct_trn8_8(p0, p1);
        dct_trn8_8(p2, p3);
        dct_trn8_8(p4, p5);
        dct_trn8_8(p6, p7);

        // пройти 2
        dct_trn8_16(p0, p2);
        dct_trn8_16(p1, p3);
        dct_trn8_16(p4, p6);
        dct_trn8_16(p5, p7);

        // пройти 3
        dct_trn8_32(p0, p4);
        dct_trn8_32(p1, p5);
        dct_trn8_32(p2, p6);
        dct_trn8_32(p3, p7);

        // магазин
        vst1_u8(out, p0);
        out += out_stride;
        vst1_u8(out, p1);
        out += out_stride;
        vst1_u8(out, p2);
        out += out_stride;
        vst1_u8(out, p3);
        out += out_stride;
        vst1_u8(out, p4);
        out += out_stride;
        vst1_u8(out, p5);
        out += out_stride;
        vst1_u8(out, p6);
        out += out_stride;
        vst1_u8(out, p7);

#undef dct_trn8_8
#undef dct_trn8_16
#undef dct_trn8_32
    }

#undef dct_long_mul
#undef dct_long_mac
#undef dct_widen
#undef dct_wadd
#undef dct_wsub
#undef dct_bfly32o
#undef dct_pass
}

#endif // STBI_NEON

#define STBI__MARKER_none  0xff
// если из потока энтропии есть ожидающий маркер, верните его
// в противном случае извлеките данные из потока и получите маркер. если нет
// маркер, верните 0xff, который никогда не является допустимым значением маркера
static stbi_uc stbi__get_marker(stbi__jpeg * j)
{
    stbi_uc x;
    if(j->marker != STBI__MARKER_none) {
        x = j->marker;
        j->marker = STBI__MARKER_none;
        return x;
    }
    x = stbi__get8(j->s);
    if(x != 0xff) return STBI__MARKER_none;
    while(x == 0xff)
        x = stbi__get8(j->s); // использовать повторяющиеся байты заполнения 0xff
    return x;
}

// в каждом сканировании у нас будут компоненты scan_n, и порядок
// компонентов указывается order[]
#define STBI__RESTART(x)     ((x) >= 0xd0 && (x) <= 0xd7)

// после интервала перезапуска stbi__jpeg_reset энтропийный декодер и
// предсказание постоянного тока
static void stbi__jpeg_reset(stbi__jpeg * j)
{
    j->code_bits = 0;
    j->code_buffer = 0;
    j->nomore = 0;
    j->img_comp[0].dc_pred = j->img_comp[1].dc_pred = j->img_comp[2].dc_pred = j->img_comp[3].dc_pred = 0;
    j->marker = STBI__MARKER_none;
    j->todo = j->restart_interval ? j->restart_interval : 0x7fffffff;
    j->eob_run = 0;
    // no more than 1<<31 MCUs if no restart_interal? that's plenty safe,
    // поскольку мы не допускаем даже 1<<30 пикселей
}

static int stbi__parse_entropy_coded_data(stbi__jpeg * z)
{
    stbi__jpeg_reset(z);
    if(!z->progressive) {
        if(z->scan_n == 1) {
            int i, j;
            STBI_SIMD_ALIGN(short, data[64]);
            int n = z->order[0];
            // нечередующиеся данные, нам просто нужно обрабатывать по одному блоку за раз,
            // в тривиальном порядке развертки
            // количество блоков, которые нужно сделать, зависит только от того, сколько фактических «пикселей» это
            // компонент имеет, независимо от чередующейся блокировки MCU и тому подобное
            int w = (z->img_comp[n].x + 7) >> 3;
            int h = (z->img_comp[n].y + 7) >> 3;
            for(j = 0; j < h; ++j) {
                for(i = 0; i < w; ++i) {
                    int ha = z->img_comp[n].ha;
                    if(!stbi__jpeg_decode_block(z, data, z->huff_dc + z->img_comp[n].hd, z->huff_ac + ha, z->fast_ac[ha], n,
                                                z->dequant[z->img_comp[n].tq])) return 0;
                    z->idct_block_kernel(z->img_comp[n].data + z->img_comp[n].w2 * j * 8 + i * 8, z->img_comp[n].w2, data);
                    // каждый блок данных представляет собой MCU, поэтому отсчитайте интервал перезапуска.
                    if(--z->todo <= 0) {
                        if(z->code_bits < 24) stbi__grow_buffer_unsafe(z);
                        // если это NOT перезагрузка, то просто залог, так мы получим повреждённые данные
                        // а не отсутствие данных
                        if(!STBI__RESTART(z->marker)) return 1;
                        stbi__jpeg_reset(z);
                    }
                }
            }
            return 1;
        }
        else {   // чередующийся
            int i, j, k, x, y;
            STBI_SIMD_ALIGN(short, data[64]);
            for(j = 0; j < z->img_mcu_y; ++j) {
                for(i = 0; i < z->img_mcu_x; ++i) {
                    // сканировать чередующийся микроконтроллер... обрабатывать компоненты scan_n по порядку
                    for(k = 0; k < z->scan_n; ++k) {
                        int n = z->order[k];
                        // отсканируйте стоимость этого компонента в микроконтроллере; это только что определено
                        // по базовым значениям H и V, указанным для компонента
                        for(y = 0; y < z->img_comp[n].v; ++y) {
                            for(x = 0; x < z->img_comp[n].h; ++x) {
                                int x2 = (i * z->img_comp[n].h + x) * 8;
                                int y2 = (j * z->img_comp[n].v + y) * 8;
                                int ha = z->img_comp[n].ha;
                                if(!stbi__jpeg_decode_block(z, data, z->huff_dc + z->img_comp[n].hd, z->huff_ac + ha, z->fast_ac[ha], n,
                                                            z->dequant[z->img_comp[n].tq])) return 0;
                                z->idct_block_kernel(z->img_comp[n].data + z->img_comp[n].w2 * y2 + x2, z->img_comp[n].w2, data);
                            }
                        }
                    }
                    // после всех чередующихся компонентов это чередующийся MCU ,
                    // так что теперь отсчитайте интервал перезапуска
                    if(--z->todo <= 0) {
                        if(z->code_bits < 24) stbi__grow_buffer_unsafe(z);
                        if(!STBI__RESTART(z->marker)) return 1;
                        stbi__jpeg_reset(z);
                    }
                }
            }
            return 1;
        }
    }
    else {
        if(z->scan_n == 1) {
            int i, j;
            int n = z->order[0];
            // нечередующиеся данные, нам просто нужно обрабатывать по одному блоку за раз,
            // в тривиальном порядке развертки
            // количество блоков, которые нужно сделать, зависит только от того, сколько фактических «пикселей» это
            // компонент имеет, независимо от чередующейся блокировки MCU и тому подобное
            int w = (z->img_comp[n].x + 7) >> 3;
            int h = (z->img_comp[n].y + 7) >> 3;
            for(j = 0; j < h; ++j) {
                for(i = 0; i < w; ++i) {
                    short * data = z->img_comp[n].coeff + 64 * (i + j * z->img_comp[n].coeff_w);
                    if(z->spec_start == 0) {
                        if(!stbi__jpeg_decode_block_prog_dc(z, data, &z->huff_dc[z->img_comp[n].hd], n))
                            return 0;
                    }
                    else {
                        int ha = z->img_comp[n].ha;
                        if(!stbi__jpeg_decode_block_prog_ac(z, data, &z->huff_ac[ha], z->fast_ac[ha]))
                            return 0;
                    }
                    // каждый блок данных представляет собой MCU, поэтому отсчитайте интервал перезапуска.
                    if(--z->todo <= 0) {
                        if(z->code_bits < 24) stbi__grow_buffer_unsafe(z);
                        if(!STBI__RESTART(z->marker)) return 1;
                        stbi__jpeg_reset(z);
                    }
                }
            }
            return 1;
        }
        else {   // чередующийся
            int i, j, k, x, y;
            for(j = 0; j < z->img_mcu_y; ++j) {
                for(i = 0; i < z->img_mcu_x; ++i) {
                    // сканировать чередующийся микроконтроллер... обрабатывать компоненты scan_n по порядку
                    for(k = 0; k < z->scan_n; ++k) {
                        int n = z->order[k];
                        // отсканируйте стоимость этого компонента в микроконтроллере; это только что определено
                        // по базовым значениям H и V, указанным для компонента
                        for(y = 0; y < z->img_comp[n].v; ++y) {
                            for(x = 0; x < z->img_comp[n].h; ++x) {
                                int x2 = (i * z->img_comp[n].h + x);
                                int y2 = (j * z->img_comp[n].v + y);
                                short * data = z->img_comp[n].coeff + 64 * (x2 + y2 * z->img_comp[n].coeff_w);
                                if(!stbi__jpeg_decode_block_prog_dc(z, data, &z->huff_dc[z->img_comp[n].hd], n))
                                    return 0;
                            }
                        }
                    }
                    // после всех чередующихся компонентов это чередующийся MCU ,
                    // так что теперь отсчитайте интервал перезапуска
                    if(--z->todo <= 0) {
                        if(z->code_bits < 24) stbi__grow_buffer_unsafe(z);
                        if(!STBI__RESTART(z->marker)) return 1;
                        stbi__jpeg_reset(z);
                    }
                }
            }
            return 1;
        }
    }
}

static void stbi__jpeg_dequantize(short * data, stbi__uint16 * dequant)
{
    int i;
    for(i = 0; i < 64; ++i)
        data[i] *= dequant[i];
}

static void stbi__jpeg_finish(stbi__jpeg * z)
{
    if(z->progressive) {
        // деквантовать и идентифицировать данные
        int i, j, n;
        for(n = 0; n < z->s->img_n; ++n) {
            int w = (z->img_comp[n].x + 7) >> 3;
            int h = (z->img_comp[n].y + 7) >> 3;
            for(j = 0; j < h; ++j) {
                for(i = 0; i < w; ++i) {
                    short * data = z->img_comp[n].coeff + 64 * (i + j * z->img_comp[n].coeff_w);
                    stbi__jpeg_dequantize(data, z->dequant[z->img_comp[n].tq]);
                    z->idct_block_kernel(z->img_comp[n].data + z->img_comp[n].w2 * j * 8 + i * 8, z->img_comp[n].w2, data);
                }
            }
        }
    }
}

static int stbi__process_marker(stbi__jpeg * z, int m)
{
    int L;
    switch(m) {
        case STBI__MARKER_none: // маркер не найден
            return stbi__err("expected marker", "Corrupt JPEG");

        case 0xDD: // DRI - specify restart interval
            if(stbi__get16be(z->s) != 4) return stbi__err("bad DRI len", "Corrupt JPEG");
            z->restart_interval = stbi__get16be(z->s);
            return 1;

        case 0xDB: // DQT - define quantization table
            L = stbi__get16be(z->s) - 2;
            while(L > 0) {
                int q = stbi__get8(z->s);
                int p = q >> 4, sixteen = (p != 0);
                int t = q & 15, i;
                if(p != 0 && p != 1) return stbi__err("bad DQT type", "Corrupt JPEG");
                if(t > 3) return stbi__err("bad DQT table", "Corrupt JPEG");

                for(i = 0; i < 64; ++i)
                    z->dequant[t][stbi__jpeg_dezigzag[i]] = (stbi__uint16)(sixteen ? stbi__get16be(z->s) : stbi__get8(z->s));
                L -= (sixteen ? 129 : 65);
            }
            return L == 0;

        case 0xC4: // DHT - define huffman table
            L = stbi__get16be(z->s) - 2;
            while(L > 0) {
                stbi_uc * v;
                int sizes[16], i, n = 0;
                int q = stbi__get8(z->s);
                int tc = q >> 4;
                int th = q & 15;
                if(tc > 1 || th > 3) return stbi__err("bad DHT header", "Corrupt JPEG");
                for(i = 0; i < 16; ++i) {
                    sizes[i] = stbi__get8(z->s);
                    n += sizes[i];
                }
                if(n > 256) return stbi__err("bad DHT header", "Corrupt JPEG"); // Цикл над i < n запишет прошлый конец значений!
                L -= 17;
                if(tc == 0) {
                    if(!stbi__build_huffman(z->huff_dc + th, sizes)) return 0;
                    v = z->huff_dc[th].values;
                }
                else {
                    if(!stbi__build_huffman(z->huff_ac + th, sizes)) return 0;
                    v = z->huff_ac[th].values;
                }
                for(i = 0; i < n; ++i)
                    v[i] = stbi__get8(z->s);
                if(tc != 0)
                    stbi__build_fast_ac(z->fast_ac[th], z->huff_ac + th);
                L -= n;
            }
            return L == 0;
    }

    // проверьте наличие блока комментариев или блоков APP
    if((m >= 0xE0 && m <= 0xEF) || m == 0xFE) {
        L = stbi__get16be(z->s);
        if(L < 2) {
            if(m == 0xFE)
                return stbi__err("bad COM len", "Corrupt JPEG");
            else
                return stbi__err("bad APP len", "Corrupt JPEG");
        }
        L -= 2;

        if(m == 0xE0 && L >= 5) {  // JFIF APP0 сегмент
            static const unsigned char tag[5] = {'J', 'F', 'I', 'F', '\0'};
            int ok = 1;
            int i;
            for(i = 0; i < 5; ++i)
                if(stbi__get8(z->s) != tag[i])
                    ok = 0;
            L -= 5;
            if(ok)
                z->jfif = 1;
        }
        else if(m == 0xEE && L >= 12) {    // Сегмент Adobe APP14
            static const unsigned char tag[6] = {'A', 'd', 'o', 'b', 'e', '\0'};
            int ok = 1;
            int i;
            for(i = 0; i < 6; ++i)
                if(stbi__get8(z->s) != tag[i])
                    ok = 0;
            L -= 6;
            if(ok) {
                stbi__get8(z->s); // версия
                stbi__get16be(z->s); // флаги0
                stbi__get16be(z->s); // флаги1
                z->app14_color_transform = stbi__get8(z->s); // преобразование цвета
                L -= 6;
            }
        }

        stbi__skip(z->s, L);
        return 1;
    }

    return stbi__err("unknown marker", "Corrupt JPEG");
}

// после того, как мы увидим SOS
static int stbi__process_scan_header(stbi__jpeg * z)
{
    int i;
    int Ls = stbi__get16be(z->s);
    z->scan_n = stbi__get8(z->s);
    if(z->scan_n < 1 || z->scan_n > 4 ||
       z->scan_n > (int) z->s->img_n) return stbi__err("bad SOS component count", "Corrupt JPEG");
    if(Ls != 6 + 2 * z->scan_n) return stbi__err("bad SOS len", "Corrupt JPEG");
    for(i = 0; i < z->scan_n; ++i) {
        int id = stbi__get8(z->s), which;
        int q = stbi__get8(z->s);
        for(which = 0; which < z->s->img_n; ++which)
            if(z->img_comp[which].id == id)
                break;
        if(which == z->s->img_n) return 0;  // нет совпадений
        z->img_comp[which].hd = q >> 4;
        if(z->img_comp[which].hd > 3) return stbi__err("bad DC huff", "Corrupt JPEG");
        z->img_comp[which].ha = q & 15;
        if(z->img_comp[which].ha > 3) return stbi__err("bad AC huff", "Corrupt JPEG");
        z->order[i] = which;
    }

    {
        int aa;
        z->spec_start = stbi__get8(z->s);
        z->spec_end   = stbi__get8(z->s); // должно быть 63, но может быть 0
        aa = stbi__get8(z->s);
        z->succ_high = (aa >> 4);
        z->succ_low  = (aa & 15);
        if(z->progressive) {
            if(z->spec_start > 63 || z->spec_end > 63  || z->spec_start > z->spec_end || z->succ_high > 13 || z->succ_low > 13)
                return stbi__err("bad SOS", "Corrupt JPEG");
        }
        else {
            if(z->spec_start != 0) return stbi__err("bad SOS", "Corrupt JPEG");
            if(z->succ_high != 0 || z->succ_low != 0) return stbi__err("bad SOS", "Corrupt JPEG");
            z->spec_end = 63;
        }
    }

    return 1;
}

static int stbi__free_jpeg_components(stbi__jpeg * z, int ncomp, int why)
{
    int i;
    for(i = 0; i < ncomp; ++i) {
        if(z->img_comp[i].raw_data) {
            STBI_FREE(z->img_comp[i].raw_data);
            z->img_comp[i].raw_data = NULL;
            z->img_comp[i].data = NULL;
        }
        if(z->img_comp[i].raw_coeff) {
            STBI_FREE(z->img_comp[i].raw_coeff);
            z->img_comp[i].raw_coeff = 0;
            z->img_comp[i].coeff = 0;
        }
        if(z->img_comp[i].linebuf) {
            STBI_FREE(z->img_comp[i].linebuf);
            z->img_comp[i].linebuf = NULL;
        }
    }
    return why;
}

static int stbi__process_frame_header(stbi__jpeg * z, int scan)
{
    stbi__context * s = z->s;
    int Lf, p, i, q, h_max = 1, v_max = 1, c;
    Lf = stbi__get16be(s);
    if(Lf < 11) return stbi__err("bad SOF len", "Corrupt JPEG"); // JPEG
    p  = stbi__get8(s);
    if(p != 8) return stbi__err("only 8-bit", "JPEG format not supported: 8-bit only"); // Базовый уровень JPEG
    s->img_y = stbi__get16be(s);
    if(s->img_y == 0) return stbi__err("no header height",
                                           "JPEG format not supported: delayed height");  // Законно, но мы этим не занимаемся, как и IJG.
    s->img_x = stbi__get16be(s);
    if(s->img_x == 0) return stbi__err("0 width", "Corrupt JPEG"); // JPEG требует
    if(s->img_y > STBI_MAX_DIMENSIONS) return stbi__err("too large", "Very large image (corrupt?)");
    if(s->img_x > STBI_MAX_DIMENSIONS) return stbi__err("too large", "Very large image (corrupt?)");
    c = stbi__get8(s);
    if(c != 3 && c != 1 && c != 4) return stbi__err("bad component count", "Corrupt JPEG");
    s->img_n = c;
    for(i = 0; i < c; ++i) {
        z->img_comp[i].data = NULL;
        z->img_comp[i].linebuf = NULL;
    }

    if(Lf != 8 + 3 * s->img_n) return stbi__err("bad SOF len", "Corrupt JPEG");

    z->rgb = 0;
    for(i = 0; i < s->img_n; ++i) {
        static const unsigned char rgb[3] = { 'R', 'G', 'B' };
        z->img_comp[i].id = stbi__get8(s);
        if(s->img_n == 3 && z->img_comp[i].id == rgb[i])
            ++z->rgb;
        q = stbi__get8(s);
        z->img_comp[i].h = (q >> 4);
        if(!z->img_comp[i].h || z->img_comp[i].h > 4) return stbi__err("bad H", "Corrupt JPEG");
        z->img_comp[i].v = q & 15;
        if(!z->img_comp[i].v || z->img_comp[i].v > 4) return stbi__err("bad V", "Corrupt JPEG");
        z->img_comp[i].tq = stbi__get8(s);
        if(z->img_comp[i].tq > 3) return stbi__err("bad TQ", "Corrupt JPEG");
    }

    if(scan != STBI__SCAN_load) return 1;

    if(!stbi__mad3sizes_valid(s->img_x, s->img_y, s->img_n, 0)) return stbi__err("too large", "Image too large to decode");

    for(i = 0; i < s->img_n; ++i) {
        if(z->img_comp[i].h > h_max) h_max = z->img_comp[i].h;
        if(z->img_comp[i].v > v_max) v_max = z->img_comp[i].v;
    }

    // проверить, что коэффициенты плоской подвыборки являются целочисленными отношениями; наши ресэмплеры не умеют работать с дробными соотношениями
    // и я никогда не видел, чтобы неповрежденный файл JPEG действительно их использовал
    for(i = 0; i < s->img_n; ++i) {
        if(h_max % z->img_comp[i].h != 0) return stbi__err("bad H", "Corrupt JPEG");
        if(v_max % z->img_comp[i].v != 0) return stbi__err("bad V", "Corrupt JPEG");
    }

    // вычислить чередующуюся информацию о микроконтроллере
    z->img_h_max = h_max;
    z->img_v_max = v_max;
    z->img_mcu_w = h_max * 8;
    z->img_mcu_h = v_max * 8;
    // эти размеры не могут быть больше 17 бит
    z->img_mcu_x = (s->img_x + z->img_mcu_w - 1) / z->img_mcu_w;
    z->img_mcu_y = (s->img_y + z->img_mcu_h - 1) / z->img_mcu_h;

    for(i = 0; i < s->img_n; ++i) {
        // количество эффективных пикселей (например, для нечередующихся MCU )
        z->img_comp[i].x = (s->img_x * z->img_comp[i].h + h_max - 1) / h_max;
        z->img_comp[i].y = (s->img_y * z->img_comp[i].v + v_max - 1) / v_max;
        // чтобы упростить генерацию, мы выделим достаточно памяти для декодирования
        // фиктивные данные большого размера из-за использования чередующихся микроконтроллеров и их
        // большие блоки (например, iMCU 16x16 на изображении шириной 33); мы не будем
        // отбросить лишние данные до преобразования цветового пространства
        //
        // img_mcu_x , img_mcu_y : <= 17 бит; comp[i].h and .v are <=4 (checked earlier)
        // поэтому эти мулы не могут переполняться 32-битными целыми числами (которые нам нужны)
        z->img_comp[i].w2 = z->img_mcu_x * z->img_comp[i].h * 8;
        z->img_comp[i].h2 = z->img_mcu_y * z->img_comp[i].v * 8;
        z->img_comp[i].coeff = 0;
        z->img_comp[i].raw_coeff = 0;
        z->img_comp[i].linebuf = NULL;
        z->img_comp[i].raw_data = stbi__malloc_mad2(z->img_comp[i].w2, z->img_comp[i].h2, 15);
        if(z->img_comp[i].raw_data == NULL)
            return stbi__free_jpeg_components(z, i + 1, stbi__err("outofmem", "Out of memory"));
        // выровнять блоки для idct с помощью mmx/sse
        z->img_comp[i].data = (stbi_uc *)(((size_t) z->img_comp[i].raw_data + 15) & ~15);
        if(z->progressive) {
            // w2, h2 кратны 8 (см. выше)
            z->img_comp[i].coeff_w = z->img_comp[i].w2 / 8;
            z->img_comp[i].coeff_h = z->img_comp[i].h2 / 8;
            z->img_comp[i].raw_coeff = stbi__malloc_mad3(z->img_comp[i].w2, z->img_comp[i].h2, sizeof(short), 15);
            if(z->img_comp[i].raw_coeff == NULL)
                return stbi__free_jpeg_components(z, i + 1, stbi__err("outofmem", "Out of memory"));
            z->img_comp[i].coeff = (short *)(((size_t) z->img_comp[i].raw_coeff + 15) & ~15);
        }
    }

    return 1;
}

// используйте сравнения, поскольку в некоторых случаях мы обрабатываем более одного случая (например, SOF )
#define stbi__DNL(x)         ((x) == 0xdc)
#define stbi__SOI(x)         ((x) == 0xd8)
#define stbi__EOI(x)         ((x) == 0xd9)
#define stbi__SOF(x)         ((x) == 0xc0 || (x) == 0xc1 || (x) == 0xc2)
#define stbi__SOS(x)         ((x) == 0xda)

#define stbi__SOF_progressive(x)   ((x) == 0xc2)

static int stbi__decode_jpeg_header(stbi__jpeg * z, int scan)
{
    int m;
    z->jfif = 0;
    z->app14_color_transform = -1; // допустимые значения: 0,1,2
    z->marker = STBI__MARKER_none; // инициализировать кэшированный маркер пустым
    m = stbi__get_marker(z);
    if(!stbi__SOI(m)) return stbi__err("no SOI", "Corrupt JPEG");
    if(scan == STBI__SCAN_type) return 1;
    m = stbi__get_marker(z);
    while(!stbi__SOF(m)) {
        if(!stbi__process_marker(z, m)) return 0;
        m = stbi__get_marker(z);
        while(m == STBI__MARKER_none) {
            // некоторые файлы имеют дополнительные поля после блоков, так что ок, мы просканируем
            if(stbi__at_eof(z->s)) return stbi__err("no SOF", "Corrupt JPEG");
            m = stbi__get_marker(z);
        }
    }
    z->progressive = stbi__SOF_progressive(m);
    if(!stbi__process_frame_header(z, scan)) return 0;
    return 1;
}

static stbi_uc stbi__skip_jpeg_junk_at_end(stbi__jpeg * j)
{
    // в некоторых JPEG-файлах есть мусор в конце, пропустите его, но если мы найдем то, что выглядит
    // как действительный маркер, возобновите там
    while(!stbi__at_eof(j->s)) {
        stbi_uc x = stbi__get8(j->s);
        while(x == 0xff) {  // может быть маркер
            if(stbi__at_eof(j->s)) return STBI__MARKER_none;
            x = stbi__get8(j->s);
            if(x != 0x00 && x != 0xff) {
                // не набитый ноль или переход к другому маркеру, выглядит
                // как настоящий маркер, верните его
                return x;
            }
            // заполненный ноль теперь имеет x=0, что завершает цикл, то есть мы идем
            // вернуться к обычному циклу сканирования.
            // повторяющийся 0xff продолжает попытки прочитать следующий байт маркера.
        }
    }
    return STBI__MARKER_none;
}

// декодировать изображение в формат YCbCr
static int stbi__decode_jpeg_image(stbi__jpeg * j)
{
    int m;
    for(m = 0; m < 4; m++) {
        j->img_comp[m].raw_data = NULL;
        j->img_comp[m].raw_coeff = NULL;
    }
    j->restart_interval = 0;
    if(!stbi__decode_jpeg_header(j, STBI__SCAN_load)) return 0;
    m = stbi__get_marker(j);
    while(!stbi__EOI(m)) {
        if(stbi__SOS(m)) {
            if(!stbi__process_scan_header(j)) return 0;
            if(!stbi__parse_entropy_coded_data(j)) return 0;
            if(j->marker == STBI__MARKER_none) {
                j->marker = stbi__skip_jpeg_junk_at_end(j);
                // если мы достигнем eof, не коснувшись маркера, stbi__get_marker() ниже завершится неудачно, и в конечном итоге мы вернем 0
            }
            m = stbi__get_marker(j);
            if(STBI__RESTART(m))
                m = stbi__get_marker(j);
        }
        else if(stbi__DNL(m)) {
            int Ld = stbi__get16be(j->s);
            stbi__uint32 NL = stbi__get16be(j->s);
            if(Ld != 4) return stbi__err("bad DNL len", "Corrupt JPEG");
            if(NL != j->s->img_y) return stbi__err("bad DNL height", "Corrupt JPEG");
            m = stbi__get_marker(j);
        }
        else {
            if(!stbi__process_marker(j, m)) return 1;
            m = stbi__get_marker(j);
        }
    }
    if(j->progressive)
        stbi__jpeg_finish(j);
    return 1;
}

// статическая повторная выборка, ориентированная на jfif (через границы блоков)

typedef stbi_uc * (*resample_row_func)(stbi_uc * out, stbi_uc * in0, stbi_uc * in1,
                                       int w, int hs);

#define stbi__div4(x) ((stbi_uc) ((x) >> 2))

static stbi_uc * resample_row_1(stbi_uc * out, stbi_uc * in_near, stbi_uc * in_far, int w, int hs)
{
    STBI_NOTUSED(out);
    STBI_NOTUSED(in_far);
    STBI_NOTUSED(w);
    STBI_NOTUSED(hs);
    return in_near;
}

static stbi_uc * stbi__resample_row_v_2(stbi_uc * out, stbi_uc * in_near, stbi_uc * in_far, int w, int hs)
{
    // необходимо сгенерировать два образца по вертикали для каждого входного сигнала
    int i;
    STBI_NOTUSED(hs);
    for(i = 0; i < w; ++i)
        out[i] = stbi__div4(3 * in_near[i] + in_far[i] + 2);
    return out;
}

static stbi_uc * stbi__resample_row_h_2(stbi_uc * out, stbi_uc * in_near, stbi_uc * in_far, int w, int hs)
{
    // необходимо сгенерировать два образца по горизонтали для каждого входного сигнала
    int i;
    stbi_uc * input = in_near;

    if(w == 1) {
        // если только один образец, интерполяция невозможна
        out[0] = out[1] = input[0];
        return out;
    }

    out[0] = input[0];
    out[1] = stbi__div4(input[0] * 3 + input[1] + 2);
    for(i = 1; i < w - 1; ++i) {
        int n = 3 * input[i] + 2;
        out[i * 2 + 0] = stbi__div4(n + input[i - 1]);
        out[i * 2 + 1] = stbi__div4(n + input[i + 1]);
    }
    out[i * 2 + 0] = stbi__div4(input[w - 2] * 3 + input[w - 1] + 2);
    out[i * 2 + 1] = input[w - 1];

    STBI_NOTUSED(in_far);
    STBI_NOTUSED(hs);

    return out;
}

#define stbi__div16(x) ((stbi_uc) ((x) >> 4))

static stbi_uc * stbi__resample_row_hv_2(stbi_uc * out, stbi_uc * in_near, stbi_uc * in_far, int w, int hs)
{
    // необходимо сгенерировать образцы 2x2 для каждого входного сигнала
    int i, t0, t1;
    if(w == 1) {
        out[0] = out[1] = stbi__div4(3 * in_near[0] + in_far[0] + 2);
        return out;
    }

    t1 = 3 * in_near[0] + in_far[0];
    out[0] = stbi__div4(t1 + 2);
    for(i = 1; i < w; ++i) {
        t0 = t1;
        t1 = 3 * in_near[i] + in_far[i];
        out[i * 2 - 1] = stbi__div16(3 * t0 + t1 + 8);
        out[i * 2  ] = stbi__div16(3 * t1 + t0 + 8);
    }
    out[w * 2 - 1] = stbi__div4(t1 + 2);

    STBI_NOTUSED(hs);

    return out;
}

#if defined(STBI_SSE2) || defined(STBI_NEON)
static stbi_uc * stbi__resample_row_hv_2_simd(stbi_uc * out, stbi_uc * in_near, stbi_uc * in_far, int w, int hs)
{
    // необходимо сгенерировать образцы 2x2 для каждого входного сигнала
    int i = 0, t0, t1;

    if(w == 1) {
        out[0] = out[1] = stbi__div4(3 * in_near[0] + in_far[0] + 2);
        return out;
    }

    t1 = 3 * in_near[0] + in_far[0];
    // обрабатывать группы по 8 пикселей так долго, как только сможем.
    // обратите внимание, что мы не можем обработать последний пиксель подряд в этом цикле
    // потому что нам нужно обработать граничные условия фильтра.
    for(; i < ((w - 1) & ~7); i += 8) {
#if defined(STBI_SSE2)
        // загрузить и выполнить проход вертикальной фильтрации
        // здесь используется 3*x + y = 4*x + (y - x)
        __m128i zero  = _mm_setzero_si128();
        __m128i farb  = _mm_loadl_epi64((__m128i *)(in_far + i));
        __m128i nearb = _mm_loadl_epi64((__m128i *)(in_near + i));
        __m128i farw  = _mm_unpacklo_epi8(farb, zero);
        __m128i nearw = _mm_unpacklo_epi8(nearb, zero);
        __m128i diff  = _mm_sub_epi16(farw, nearw);
        __m128i nears = _mm_slli_epi16(nearw, 2);
        __m128i curr  = _mm_add_epi16(nears, diff); // текущая строка

        // горизонтальный фильтр работает так же, основываясь на смещенной версии тока
        // ряд. «prev» — текущая строка, сдвинутая вправо на 1 пиксель; нам нужно
        // вставьте предыдущее значение пикселя (из t1).
        // «следующий» — текущая строка, сдвинутая влево на 1 пиксель, причем первый пиксель
        // следующего блока из 8 пикселей.
        __m128i prv0 = _mm_slli_si128(curr, 2);
        __m128i nxt0 = _mm_srli_si128(curr, 2);
        __m128i prev = _mm_insert_epi16(prv0, t1, 0);
        __m128i next = _mm_insert_epi16(nxt0, 3 * in_near[i + 8] + in_far[i + 8], 7);

        // горизонтальный фильтр, многофазная реализация, так как удобно:
        // четные пиксели = 3*cur + prev = cur*4 + (prev - cur)
        // нечетные пиксели = 3*cur + next = cur*4 + (next — cur)
        // обратите внимание на общий термин.
        __m128i bias  = _mm_set1_epi16(8);
        __m128i curs = _mm_slli_epi16(curr, 2);
        __m128i prvd = _mm_sub_epi16(prev, curr);
        __m128i nxtd = _mm_sub_epi16(next, curr);
        __m128i curb = _mm_add_epi16(curs, bias);
        __m128i even = _mm_add_epi16(prvd, curb);
        __m128i odd  = _mm_add_epi16(nxtd, curb);

        // чередуйте четные и нечетные пиксели, затем отмените масштабирование.
        __m128i int0 = _mm_unpacklo_epi16(even, odd);
        __m128i int1 = _mm_unpackhi_epi16(even, odd);
        __m128i de0  = _mm_srli_epi16(int0, 4);
        __m128i de1  = _mm_srli_epi16(int1, 4);

        // упаковать и записать вывод
        __m128i outv = _mm_packus_epi16(de0, de1);
        _mm_storeu_si128((__m128i *)(out + i * 2), outv);
#elif defined(STBI_NEON)
        // загрузить и выполнить проход вертикальной фильтрации
        // здесь используется 3*x + y = 4*x + (y - x)
        uint8x8_t farb  = vld1_u8(in_far + i);
        uint8x8_t nearb = vld1_u8(in_near + i);
        int16x8_t diff  = vreinterpretq_s16_u16(vsubl_u8(farb, nearb));
        int16x8_t nears = vreinterpretq_s16_u16(vshll_n_u8(nearb, 2));
        int16x8_t curr  = vaddq_s16(nears, diff); // текущая строка

        // горизонтальный фильтр работает так же, основываясь на смещенной версии тока
        // ряд. «prev» — текущая строка, сдвинутая вправо на 1 пиксель; нам нужно
        // вставьте предыдущее значение пикселя (из t1).
        // «следующий» — текущая строка, сдвинутая влево на 1 пиксель, причем первый пиксель
        // следующего блока из 8 пикселей.
        int16x8_t prv0 = vextq_s16(curr, curr, 7);
        int16x8_t nxt0 = vextq_s16(curr, curr, 1);
        int16x8_t prev = vsetq_lane_s16(t1, prv0, 0);
        int16x8_t next = vsetq_lane_s16(3 * in_near[i + 8] + in_far[i + 8], nxt0, 7);

        // горизонтальный фильтр, многофазная реализация, так как удобно:
        // четные пиксели = 3*cur + prev = cur*4 + (prev - cur)
        // нечетные пиксели = 3*cur + next = cur*4 + (next — cur)
        // обратите внимание на общий термин.
        int16x8_t curs = vshlq_n_s16(curr, 2);
        int16x8_t prvd = vsubq_s16(prev, curr);
        int16x8_t nxtd = vsubq_s16(next, curr);
        int16x8_t even = vaddq_s16(curs, prvd);
        int16x8_t odd  = vaddq_s16(curs, nxtd);

        // отменить масштабирование и округление, затем сохранить с чередованием четных/нечетных фаз
        uint8x8x2_t o;
        o.val[0] = vqrshrun_n_s16(even, 4);
        o.val[1] = vqrshrun_n_s16(odd,  4);
        vst2_u8(out + i * 2, o);
#endif

        // «предыдущее» значение для следующего итера
        t1 = 3 * in_near[i + 7] + in_far[i + 7];
    }

    t0 = t1;
    t1 = 3 * in_near[i] + in_far[i];
    out[i * 2] = stbi__div16(3 * t1 + t0 + 8);

    for(++i; i < w; ++i) {
        t0 = t1;
        t1 = 3 * in_near[i] + in_far[i];
        out[i * 2 - 1] = stbi__div16(3 * t0 + t1 + 8);
        out[i * 2  ] = stbi__div16(3 * t1 + t0 + 8);
    }
    out[w * 2 - 1] = stbi__div4(t1 + 2);

    STBI_NOTUSED(hs);

    return out;
}
#endif

static stbi_uc * stbi__resample_row_generic(stbi_uc * out, stbi_uc * in_near, stbi_uc * in_far, int w, int hs)
{
    // повторная выборка с ближайшим соседом
    int i, j;
    STBI_NOTUSED(in_far);
    for(i = 0; i < w; ++i)
        for(j = 0; j < hs; ++j)
            out[i * hs + j] = in_near[i];
    return out;
}

// this is a reduced-precision calculation of YCbCr-to- RGB  introduced
// чтобы убедиться, что код дает одинаковые результаты как в SIMD, так и в скалярном
#define stbi__float2fixed(x)  (((int) ((x) * 4096.0f + 0.5f)) << 8)
static void stbi__YCbCr_to_RGB_row(stbi_uc * out, const stbi_uc * y, const stbi_uc * pcb, const stbi_uc * pcr,
                                   int count, int step)
{
    int i;
    for(i = 0; i < count; ++i) {
        int y_fixed = (y[i] << 20) + (1 << 19); // округление
        int r, g, b;
        int cr = pcr[i] - 128;
        int cb = pcb[i] - 128;
        r = y_fixed +  cr * stbi__float2fixed(1.40200f);
        g = y_fixed + (cr * -stbi__float2fixed(0.71414f)) + ((cb * -stbi__float2fixed(0.34414f)) & 0xffff0000);
        b = y_fixed                                     +   cb * stbi__float2fixed(1.77200f);
        r >>= 20;
        g >>= 20;
        b >>= 20;
        if((unsigned) r > 255) {
            if(r < 0) r = 0;
            else r = 255;
        }
        if((unsigned) g > 255) {
            if(g < 0) g = 0;
            else g = 255;
        }
        if((unsigned) b > 255) {
            if(b < 0) b = 0;
            else b = 255;
        }
        out[0] = (stbi_uc)r;
        out[1] = (stbi_uc)g;
        out[2] = (stbi_uc)b;
        out[3] = 255;
        out += step;
    }
}

#if defined(STBI_SSE2) || defined(STBI_NEON)
static void stbi__YCbCr_to_RGB_simd(stbi_uc * out, stbi_uc const * y, stbi_uc const * pcb, stbi_uc const * pcr,
                                    int count, int step)
{
    int i = 0;

#ifdef STBI_SSE2
    // шаг == 3 довольно уродлив на последнем чередовании, и я не уверен
    // это полезно на практике (например, вы не будете использовать его для текстур).
    // так что просто ускорьте шаг == 4 случай.
    if(step == 4) {
        // это довольно простая реализация, не слишком оптимизированная.
        __m128i signflip  = _mm_set1_epi8(-0x80);
        __m128i cr_const0 = _mm_set1_epi16((short)(1.40200f * 4096.0f + 0.5f));
        __m128i cr_const1 = _mm_set1_epi16(- (short)(0.71414f * 4096.0f + 0.5f));
        __m128i cb_const0 = _mm_set1_epi16(- (short)(0.34414f * 4096.0f + 0.5f));
        __m128i cb_const1 = _mm_set1_epi16((short)(1.77200f * 4096.0f + 0.5f));
        __m128i y_bias = _mm_set1_epi8((char)(unsigned char) 128);
        __m128i xw = _mm_set1_epi16(255); // альфа-канал

        for(; i + 7 < count; i += 8) {
            // нагрузка
            __m128i y_bytes = _mm_loadl_epi64((__m128i *)(y + i));
            __m128i cr_bytes = _mm_loadl_epi64((__m128i *)(pcr + i));
            __m128i cb_bytes = _mm_loadl_epi64((__m128i *)(pcb + i));
            __m128i cr_biased = _mm_xor_si128(cr_bytes, signflip); // -128
            __m128i cb_biased = _mm_xor_si128(cb_bytes, signflip); // -128

            // распаковать в шорт (и сдвинуть влево cr, cb на 8)
            __m128i yw  = _mm_unpacklo_epi8(y_bias, y_bytes);
            __m128i crw = _mm_unpacklo_epi8(_mm_setzero_si128(), cr_biased);
            __m128i cbw = _mm_unpacklo_epi8(_mm_setzero_si128(), cb_biased);

            // преобразование цвета
            __m128i yws = _mm_srli_epi16(yw, 4);
            __m128i cr0 = _mm_mulhi_epi16(cr_const0, crw);
            __m128i cb0 = _mm_mulhi_epi16(cb_const0, cbw);
            __m128i cb1 = _mm_mulhi_epi16(cbw, cb_const1);
            __m128i cr1 = _mm_mulhi_epi16(crw, cr_const1);
            __m128i rws = _mm_add_epi16(cr0, yws);
            __m128i gwt = _mm_add_epi16(cb0, yws);
            __m128i bws = _mm_add_epi16(yws, cb1);
            __m128i gws = _mm_add_epi16(gwt, cr1);

            // удалить накипь
            __m128i rw = _mm_srai_epi16(rws, 4);
            __m128i bw = _mm_srai_epi16(bws, 4);
            __m128i gw = _mm_srai_epi16(gws, 4);

            // обратно в байт, настроено на транспонирование
            __m128i brb = _mm_packus_epi16(rw, bw);
            __m128i gxb = _mm_packus_epi16(gw, xw);

            // транспонировать в чередующиеся каналы
            __m128i t0 = _mm_unpacklo_epi8(brb, gxb);
            __m128i t1 = _mm_unpackhi_epi8(brb, gxb);
            __m128i o0 = _mm_unpacklo_epi16(t0, t1);
            __m128i o1 = _mm_unpackhi_epi16(t0, t1);

            // магазин
            _mm_storeu_si128((__m128i *)(out + 0), o0);
            _mm_storeu_si128((__m128i *)(out + 16), o1);
            out += 32;
        }
    }
#endif

#ifdef STBI_NEON
    // in this version, step=3 support would be easy to add. but is there demand?
    if(step == 4) {
        // это довольно простая реализация, не слишком оптимизированная.
        uint8x8_t signflip = vdup_n_u8(0x80);
        int16x8_t cr_const0 = vdupq_n_s16((short)(1.40200f * 4096.0f + 0.5f));
        int16x8_t cr_const1 = vdupq_n_s16(- (short)(0.71414f * 4096.0f + 0.5f));
        int16x8_t cb_const0 = vdupq_n_s16(- (short)(0.34414f * 4096.0f + 0.5f));
        int16x8_t cb_const1 = vdupq_n_s16((short)(1.77200f * 4096.0f + 0.5f));

        for(; i + 7 < count; i += 8) {
            // нагрузка
            uint8x8_t y_bytes  = vld1_u8(y + i);
            uint8x8_t cr_bytes = vld1_u8(pcr + i);
            uint8x8_t cb_bytes = vld1_u8(pcb + i);
            int8x8_t cr_biased = vreinterpret_s8_u8(vsub_u8(cr_bytes, signflip));
            int8x8_t cb_biased = vreinterpret_s8_u8(vsub_u8(cb_bytes, signflip));

            // расширить до s16
            int16x8_t yws = vreinterpretq_s16_u16(vshll_n_u8(y_bytes, 4));
            int16x8_t crw = vshll_n_s8(cr_biased, 7);
            int16x8_t cbw = vshll_n_s8(cb_biased, 7);

            // преобразование цвета
            int16x8_t cr0 = vqdmulhq_s16(crw, cr_const0);
            int16x8_t cb0 = vqdmulhq_s16(cbw, cb_const0);
            int16x8_t cr1 = vqdmulhq_s16(crw, cr_const1);
            int16x8_t cb1 = vqdmulhq_s16(cbw, cb_const1);
            int16x8_t rws = vaddq_s16(yws, cr0);
            int16x8_t gws = vaddq_s16(vaddq_s16(yws, cb0), cr1);
            int16x8_t bws = vaddq_s16(yws, cb1);

            // отменить масштабирование, округлить, преобразовать в байт
            uint8x8x4_t o;
            o.val[0] = vqrshrun_n_s16(rws, 4);
            o.val[1] = vqrshrun_n_s16(gws, 4);
            o.val[2] = vqrshrun_n_s16(bws, 4);
            o.val[3] = vdup_n_u8(255);

            // сохранение, чередование r/g/b/a
            vst4_u8(out, o);
            out += 8 * 4;
        }
    }
#endif

    for(; i < count; ++i) {
        int y_fixed = (y[i] << 20) + (1 << 19); // округление
        int r, g, b;
        int cr = pcr[i] - 128;
        int cb = pcb[i] - 128;
        r = y_fixed + cr * stbi__float2fixed(1.40200f);
        g = y_fixed + cr * -stbi__float2fixed(0.71414f) + ((cb * -stbi__float2fixed(0.34414f)) & 0xffff0000);
        b = y_fixed                                   +   cb * stbi__float2fixed(1.77200f);
        r >>= 20;
        g >>= 20;
        b >>= 20;
        if((unsigned) r > 255) {
            if(r < 0) r = 0;
            else r = 255;
        }
        if((unsigned) g > 255) {
            if(g < 0) g = 0;
            else g = 255;
        }
        if((unsigned) b > 255) {
            if(b < 0) b = 0;
            else b = 255;
        }
        out[0] = (stbi_uc)r;
        out[1] = (stbi_uc)g;
        out[2] = (stbi_uc)b;
        out[3] = 255;
        out += step;
    }
}
#endif

// настроить ядра
static void stbi__setup_jpeg(stbi__jpeg * j)
{
    j->idct_block_kernel = stbi__idct_block;
    j->YCbCr_to_RGB_kernel = stbi__YCbCr_to_RGB_row;
    j->resample_row_hv_2_kernel = stbi__resample_row_hv_2;

#ifdef STBI_SSE2
    if(stbi__sse2_available()) {
        j->idct_block_kernel = stbi__idct_simd;
        j->YCbCr_to_RGB_kernel = stbi__YCbCr_to_RGB_simd;
        j->resample_row_hv_2_kernel = stbi__resample_row_hv_2_simd;
    }
#endif

#ifdef STBI_NEON
    j->idct_block_kernel = stbi__idct_simd;
    j->YCbCr_to_RGB_kernel = stbi__YCbCr_to_RGB_simd;
    j->resample_row_hv_2_kernel = stbi__resample_row_hv_2_simd;
#endif
}

// очистить временные буферы компонентов
static void stbi__cleanup_jpeg(stbi__jpeg * j)
{
    stbi__free_jpeg_components(j, j->s->img_n, 0);
}

typedef struct {
    resample_row_func resample;
    stbi_uc * line0, * line1;
    int hs, vs;  // коэффициент расширения по каждой оси
    int w_lores; // предварительное расширение горизонтальных пикселей
    int ystep;   // как далеко мы продвинулись в вертикальном расширении
    int ypos;    // в какой строке перед раскрытием мы находимся
} stbi__resample;

// быстрое 0..255 * 0..255 => 0..255 округленное умножение
static stbi_uc stbi__blinn_8x8(stbi_uc x, stbi_uc y)
{
    unsigned int t = x * y + 128;
    return (stbi_uc)((t + (t >> 8)) >> 8);
}

static stbi_uc * load_jpeg_image(stbi__jpeg * z, int * out_x, int * out_y, int * comp, int req_comp)
{
    int n, decode_n, is_rgb;
    z->s->img_n = 0; // сделать stbi__cleanup_jpeg безопасным

    // подтвердить req_comp
    if(req_comp < 0 || req_comp > 4) return stbi__errpuc("bad req_comp", "Internal error");

    // загрузите изображение jpeg из любого источника, но оставьте в формате YCbCr
    if(!stbi__decode_jpeg_image(z)) {
        stbi__cleanup_jpeg(z);
        return NULL;
    }

    // определить фактическое количество компонентов для создания
    n = req_comp ? req_comp : z->s->img_n >= 3 ? 3 : 1;

    is_rgb = z->s->img_n == 3 && (z->rgb == 3 || (z->app14_color_transform == 0 && !z->jfif));

    if(z->s->img_n == 3 && n < 3 && !is_rgb)
        decode_n = 1;
    else
        decode_n = z->s->img_n;

    // ничего делать, если никакие компоненты не запрошены; проверьте это сейчас, чтобы избежать
    // доступ к неинициализированному coutput[0] позже
    if(decode_n <= 0) {
        stbi__cleanup_jpeg(z);
        return NULL;
    }

    // передискретизация и преобразование цвета
    {
        int k;
        unsigned int i, j;
        stbi_uc * output;
        stbi_uc * coutput[4] = { NULL, NULL, NULL, NULL };

        stbi__resample res_comp[4];

        for(k = 0; k < decode_n; ++k) {
            stbi__resample * r = &res_comp[k];

            // выделить строковый буфер, достаточно большой для повышения дискретизации по краям
            // с коэффициентом повышения дискретизации 4
            z->img_comp[k].linebuf = (stbi_uc *) stbi__malloc(z->s->img_x + 3);
            if(!z->img_comp[k].linebuf) {
                stbi__cleanup_jpeg(z);
                return stbi__errpuc("outofmem", "Out of memory");
            }

            r->hs      = z->img_h_max / z->img_comp[k].h;
            r->vs      = z->img_v_max / z->img_comp[k].v;
            r->ystep   = r->vs >> 1;
            r->w_lores = (z->s->img_x + r->hs - 1) / r->hs;
            r->ypos    = 0;
            r->line0   = r->line1 = z->img_comp[k].data;

            if(r->hs == 1 && r->vs == 1) r->resample = resample_row_1;
            else if(r->hs == 1 && r->vs == 2) r->resample = stbi__resample_row_v_2;
            else if(r->hs == 2 && r->vs == 1) r->resample = stbi__resample_row_h_2;
            else if(r->hs == 2 && r->vs == 2) r->resample = z->resample_row_hv_2_kernel;
            else                               r->resample = stbi__resample_row_generic;
        }

        // после этого не может быть ошибки, так что это безопасно
        output = (stbi_uc *) stbi__malloc_mad3(n, z->s->img_x, z->s->img_y, 1);
        if(!output) {
            stbi__cleanup_jpeg(z);
            return stbi__errpuc("outofmem", "Out of memory");
        }

        // теперь продолжайте и повторите выборку
        for(j = 0; j < z->s->img_y; ++j) {
            stbi_uc * out = output + n * z->s->img_x * j;
            for(k = 0; k < decode_n; ++k) {
                stbi__resample * r = &res_comp[k];
                int y_bot = r->ystep >= (r->vs >> 1);
                coutput[k] = r->resample(z->img_comp[k].linebuf,
                                         y_bot ? r->line1 : r->line0,
                                         y_bot ? r->line0 : r->line1,
                                         r->w_lores, r->hs);
                if(++r->ystep >= r->vs) {
                    r->ystep = 0;
                    r->line0 = r->line1;
                    if(++r->ypos < z->img_comp[k].y)
                        r->line1 += z->img_comp[k].w2;
                }
            }
            if(n >= 3) {
                stbi_uc * y = coutput[0];
                if(z->s->img_n == 3) {
                    if(is_rgb) {
                        for(i = 0; i < z->s->img_x; ++i) {
                            out[0] = y[i];
                            out[1] = coutput[1][i];
                            out[2] = coutput[2][i];
                            out[3] = 255;
                            out += n;
                        }
                    }
                    else {
                        z->YCbCr_to_RGB_kernel(out, y, coutput[1], coutput[2], z->s->img_x, n);
                    }
                }
                else if(z->s->img_n == 4) {
                    if(z->app14_color_transform == 0) {  // CMYK
                        for(i = 0; i < z->s->img_x; ++i) {
                            stbi_uc m = coutput[3][i];
                            out[0] = stbi__blinn_8x8(coutput[0][i], m);
                            out[1] = stbi__blinn_8x8(coutput[1][i], m);
                            out[2] = stbi__blinn_8x8(coutput[2][i], m);
                            out[3] = 255;
                            out += n;
                        }
                    }
                    else if(z->app14_color_transform == 2) {    // YCCK
                        z->YCbCr_to_RGB_kernel(out, y, coutput[1], coutput[2], z->s->img_x, n);
                        for(i = 0; i < z->s->img_x; ++i) {
                            stbi_uc m = coutput[3][i];
                            out[0] = stbi__blinn_8x8(255 - out[0], m);
                            out[1] = stbi__blinn_8x8(255 - out[1], m);
                            out[2] = stbi__blinn_8x8(255 - out[2], m);
                            out += n;
                        }
                    }
                    else {   // YCbCr + alpha?  Ignore the fourth channel for now
                        z->YCbCr_to_RGB_kernel(out, y, coutput[1], coutput[2], z->s->img_x, n);
                    }
                }
                else
                    for(i = 0; i < z->s->img_x; ++i) {
                        out[0] = out[1] = out[2] = y[i];
                        out[3] = 255; // не используется, если n==3
                        out += n;
                    }
            }
            else {
                if(is_rgb) {
                    if(n == 1)
                        for(i = 0; i < z->s->img_x; ++i)
                            *out++ = stbi__compute_y(coutput[0][i], coutput[1][i], coutput[2][i]);
                    else {
                        for(i = 0; i < z->s->img_x; ++i, out += 2) {
                            out[0] = stbi__compute_y(coutput[0][i], coutput[1][i], coutput[2][i]);
                            out[1] = 255;
                        }
                    }
                }
                else if(z->s->img_n == 4 && z->app14_color_transform == 0) {
                    for(i = 0; i < z->s->img_x; ++i) {
                        stbi_uc m = coutput[3][i];
                        stbi_uc r = stbi__blinn_8x8(coutput[0][i], m);
                        stbi_uc g = stbi__blinn_8x8(coutput[1][i], m);
                        stbi_uc b = stbi__blinn_8x8(coutput[2][i], m);
                        out[0] = stbi__compute_y(r, g, b);
                        out[1] = 255;
                        out += n;
                    }
                }
                else if(z->s->img_n == 4 && z->app14_color_transform == 2) {
                    for(i = 0; i < z->s->img_x; ++i) {
                        out[0] = stbi__blinn_8x8(255 - coutput[0][i], coutput[3][i]);
                        out[1] = 255;
                        out += n;
                    }
                }
                else {
                    stbi_uc * y = coutput[0];
                    if(n == 1)
                        for(i = 0; i < z->s->img_x; ++i) out[i] = y[i];
                    else
                        for(i = 0; i < z->s->img_x; ++i) {
                            *out++ = y[i];
                            *out++ = 255;
                        }
                }
            }
        }
        stbi__cleanup_jpeg(z);
        *out_x = z->s->img_x;
        *out_y = z->s->img_y;
        if(comp) *comp = z->s->img_n >= 3 ? 3 : 1;  // сообщать об исходных компонентах, а не о выводе
        return output;
    }
}

static void * stbi__jpeg_load(stbi__context * s, int * x, int * y, int * comp, int req_comp, stbi__result_info * ri)
{
    unsigned char * result;
    stbi__jpeg * j = (stbi__jpeg *) stbi__malloc(sizeof(stbi__jpeg));
    if(!j) return stbi__errpuc("outofmem", "Out of memory");
    memset(j, 0, sizeof(stbi__jpeg));
    STBI_NOTUSED(ri);
    j->s = s;
    stbi__setup_jpeg(j);
    result = load_jpeg_image(j, x, y, comp, req_comp);
    STBI_FREE(j);
    return result;
}

static int stbi__jpeg_test(stbi__context * s)
{
    int r;
    stbi__jpeg * j = (stbi__jpeg *)stbi__malloc(sizeof(stbi__jpeg));
    if(!j) return stbi__err("outofmem", "Out of memory");
    memset(j, 0, sizeof(stbi__jpeg));
    j->s = s;
    stbi__setup_jpeg(j);
    r = stbi__decode_jpeg_header(j, STBI__SCAN_type);
    stbi__rewind(s);
    STBI_FREE(j);
    return r;
}

static int stbi__jpeg_info_raw(stbi__jpeg * j, int * x, int * y, int * comp)
{
    if(!stbi__decode_jpeg_header(j, STBI__SCAN_header)) {
        stbi__rewind(j->s);
        return 0;
    }
    if(x) *x = j->s->img_x;
    if(y) *y = j->s->img_y;
    if(comp) *comp = j->s->img_n >= 3 ? 3 : 1;
    return 1;
}

static int stbi__jpeg_info(stbi__context * s, int * x, int * y, int * comp)
{
    int result;
    stbi__jpeg * j = (stbi__jpeg *)(stbi__malloc(sizeof(stbi__jpeg)));
    if(!j) return stbi__err("outofmem", "Out of memory");
    memset(j, 0, sizeof(stbi__jpeg));
    j->s = s;
    result = stbi__jpeg_info_raw(j, x, y, comp);
    STBI_FREE(j);
    return result;
}
#endif

// общедоступное декодирование zlib v0.2 Шон Барретт 18 ноября 2006 г.
//    простая реализация
//      - all input must be provided in an upfront buffer
//      - all output is written to a single output buffer (can malloc/realloc)
//    производительность
//      - fast huffman

#ifndef STBI_NO_ZLIB

// быстрый способ проверить быстрее, чем jpeg Huffman, но медленный способ медленнее
#define STBI__ZFAST_BITS  9 // ускорить все случаи в таблицах по умолчанию
#define STBI__ZFAST_MASK  ((1 << STBI__ZFAST_BITS) - 1)
#define STBI__ZNSYMS 288 // количество символов в буквенном/длинном алфавите

// Кодирование Хаффмана в стиле zlib
// (пакеты jpegs слева, zlib справа, поэтому нельзя поделиться кодом)
typedef struct {
    stbi__uint16 fast[1 << STBI__ZFAST_BITS];
    stbi__uint16 firstcode[16];
    int maxcode[17];
    stbi__uint16 firstsymbol[16];
    stbi_uc  size[STBI__ZNSYMS];
    stbi__uint16 value[STBI__ZNSYMS];
} stbi__zhuffman;

stbi_inline static int stbi__bitreverse16(int n)
{
    n = ((n & 0xAAAA) >>  1) | ((n & 0x5555) << 1);
    n = ((n & 0xCCCC) >>  2) | ((n & 0x3333) << 2);
    n = ((n & 0xF0F0) >>  4) | ((n & 0x0F0F) << 4);
    n = ((n & 0xFF00) >>  8) | ((n & 0x00FF) << 8);
    return n;
}

stbi_inline static int stbi__bit_reverse(int v, int bits)
{
    STBI_ASSERT(bits <= 16);
    // чтобы перевернуть n бит, перевернуть 16 и сдвинуть
    // например 11 бит, бит реверс и сдвиг 5
    return stbi__bitreverse16(v) >> (16 - bits);
}

static int stbi__zbuild_huffman(stbi__zhuffman * z, const stbi_uc * sizelist, int num)
{
    int i, k = 0;
    int code, next_code[16], sizes[17];

    // Спецификация DEFLATE для генерации кодов
    memset(sizes, 0, sizeof(sizes));
    memset(z->fast, 0, sizeof(z->fast));
    for(i = 0; i < num; ++i)
        ++sizes[sizelist[i]];
    sizes[0] = 0;
    for(i = 1; i < 16; ++i)
        if(sizes[i] > (1 << i))
            return stbi__err("bad sizes", "Corrupt PNG");
    code = 0;
    for(i = 1; i < 16; ++i) {
        next_code[i] = code;
        z->firstcode[i] = (stbi__uint16) code;
        z->firstsymbol[i] = (stbi__uint16) k;
        code = (code + sizes[i]);
        if(sizes[i])
            if(code - 1 >= (1 << i)) return stbi__err("bad codelengths", "Corrupt PNG");
        z->maxcode[i] = code << (16 - i); // предварительный сдвиг для внутреннего цикла
        code <<= 1;
        k += sizes[i];
    }
    z->maxcode[16] = 0x10000; // дозорный
    for(i = 0; i < num; ++i) {
        int s = sizelist[i];
        if(s) {
            int c = next_code[s] - z->firstcode[s] + z->firstsymbol[s];
            stbi__uint16 fastv = (stbi__uint16)((s << 9) | i);
            z->size [c] = (stbi_uc) s;
            z->value[c] = (stbi__uint16) i;
            if(s <= STBI__ZFAST_BITS) {
                int j = stbi__bit_reverse(next_code[s], s);
                while(j < (1 << STBI__ZFAST_BITS)) {
                    z->fast[j] = fastv;
                    j += (1 << s);
                }
            }
            ++next_code[s];
        }
    }
    return 1;
}

// реализация zlib-from-memory для чтения PNG
//    поскольку PNG позволяет произвольно разбивать поток zlib,
//    и структурно раздражает то, что PNG вызывает ZLIB и вызывает PNG ,
//    нам требуется, чтобы PNG прочитал все IDAT и объединил их в один
//    буфер памяти

typedef struct {
    stbi_uc * zbuffer, * zbuffer_end;
    int num_bits;
    int hit_zeof_once;
    stbi__uint32 code_buffer;

    char * zout;
    char * zout_start;
    char * zout_end;
    int   z_expandable;

    stbi__zhuffman z_length, z_distance;
} stbi__zbuf;

stbi_inline static int stbi__zeof(stbi__zbuf * z)
{
    return (z->zbuffer >= z->zbuffer_end);
}

stbi_inline static stbi_uc stbi__zget8(stbi__zbuf * z)
{
    return stbi__zeof(z) ? 0 : *z->zbuffer++;
}

static void stbi__fill_bits(stbi__zbuf * z)
{
    do {
        if(z->code_buffer >= (1U << z->num_bits)) {
            z->zbuffer = z->zbuffer_end;  /* рассматривайте это как EOF, и мы потерпим неудачу. */
            return;
        }
        z->code_buffer |= (unsigned int) stbi__zget8(z) << z->num_bits;
        z->num_bits += 8;
    } while(z->num_bits <= 24);
}

stbi_inline static unsigned int stbi__zreceive(stbi__zbuf * z, int n)
{
    unsigned int k;
    if(z->num_bits < n) stbi__fill_bits(z);
    k = z->code_buffer & ((1 << n) - 1);
    z->code_buffer >>= n;
    z->num_bits -= n;
    return k;
}

static int stbi__zhuffman_decode_slowpath(stbi__zbuf * a, stbi__zhuffman * z)
{
    int b, s, k;
    // не разрешается с помощью быстрой таблицы, поэтому вычисляйте ее медленным способом
    // используйте подход jpeg, который требует MSbits сверху
    k = stbi__bit_reverse(a->code_buffer, 16);
    for(s = STBI__ZFAST_BITS + 1; ; ++s)
        if(k < z->maxcode[s])
            break;
    if(s >= 16) return -1;  // неверный код!
    // размер кода равен s, поэтому:
    b = (k >> (16 - s)) - z->firstcode[s] + z->firstsymbol[s];
    if(b >= STBI__ZNSYMS) return -1;  // некоторые данные где-то были повреждены!
    if(z->size[b] != s) return -1;   // изначально было утверждением, но вместо этого сообщило об ошибке.
    a->code_buffer >>= s;
    a->num_bits -= s;
    return z->value[b];
}

stbi_inline static int stbi__zhuffman_decode(stbi__zbuf * a, stbi__zhuffman * z)
{
    int b, s;
    if(a->num_bits < 16) {
        if(stbi__zeof(a)) {
            if(!a->hit_zeof_once) {
                // Это первый раз, когда мы нажимаем eof, вставляем 16 дополнительных отступов btis
                // чтобы позволить нам продолжать идти; если мы действительно потребляем любой из них
                // хотя это неверные данные. Это поймано позже.
                a->hit_zeof_once = 1;
                a->num_bits += 16; // добавить 16 неявных нулевых битов
            }
            else {
                // Мы уже вставили дополнительные 16 бит заполнения и снова
                // out, этот поток фактически преждевременно завершается.
                return -1;
            }
        }
        else {
            stbi__fill_bits(a);
        }
    }
    b = z->fast[a->code_buffer & STBI__ZFAST_MASK];
    if(b) {
        s = b >> 9;
        a->code_buffer >>= s;
        a->num_bits -= s;
        return b & 511;
    }
    return stbi__zhuffman_decode_slowpath(a, z);
}

static int stbi__zexpand(stbi__zbuf * z, char * zout, int n) // нужно освободить место для n байт
{
    char * q;
    unsigned int cur, limit, old_limit;
    z->zout = zout;
    if(!z->z_expandable) return stbi__err("output buffer limit", "Corrupt PNG");
    cur   = (unsigned int)(z->zout - z->zout_start);
    limit = old_limit = (unsigned)(z->zout_end - z->zout_start);
    if(UINT_MAX - cur < (unsigned) n) return stbi__err("outofmem", "Out of memory");
    while(cur + n > limit) {
        if(limit > UINT_MAX / 2) return stbi__err("outofmem", "Out of memory");
        limit *= 2;
    }
    q = (char *) STBI_REALLOC_SIZED(z->zout_start, old_limit, limit);
    STBI_NOTUSED(old_limit);
    if(q == NULL) return stbi__err("outofmem", "Out of memory");
    z->zout_start = q;
    z->zout       = q + cur;
    z->zout_end   = q + limit;
    return 1;
}

static const int stbi__zlength_base[31] = {
    3, 4, 5, 6, 7, 8, 9, 10, 11, 13,
    15, 17, 19, 23, 27, 31, 35, 43, 51, 59,
    67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0
};

static const int stbi__zlength_extra[31] =
{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 0, 0 };

static const int stbi__zdist_base[32] = { 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
                                          257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577, 0, 0
                                        };

static const int stbi__zdist_extra[32] =
{ 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

static int stbi__parse_huffman_block(stbi__zbuf * a)
{
    char * zout = a->zout;
    for(;;) {
        int z = stbi__zhuffman_decode(a, &a->z_length);
        if(z < 256) {
            if(z < 0) return stbi__err("bad huffman code", "Corrupt PNG"); // ошибка в кодах Хаффмана
            if(zout >= a->zout_end) {
                if(!stbi__zexpand(a, zout, 1)) return 0;
                zout = a->zout;
            }
            *zout++ = (char) z;
        }
        else {
            stbi_uc * p;
            int len, dist;
            if(z == 256) {
                a->zout = zout;
                if(a->hit_zeof_once && a->num_bits < 16) {
                    // Когда мы впервые нажали zeof, мы вставили в наш бит 16 дополнительных нулевых битов.
                    // буфер, чтобы декодер мог просто выполнить спекулятивное декодирование. Но если мы
                    // фактически потребляет любой из этих битов (это тот случай, когда num_bits < 16),
                    // поток на самом деле читается дальше конца, поэтому он искажен.
                    return stbi__err("unexpected end", "Corrupt PNG");
                }
                return 1;
            }
            if(z >= 286) return stbi__err("bad huffman code",
                                              "Corrupt PNG"); // согласно DEFLATE коды длины 286 и 287 не должны появляться в сжатых данных.
            z -= 257;
            len = stbi__zlength_base[z];
            if(stbi__zlength_extra[z]) len += stbi__zreceive(a, stbi__zlength_extra[z]);
            z = stbi__zhuffman_decode(a, &a->z_distance);
            if(z < 0 ||
               z >= 30) return stbi__err("bad huffman code",
                                             "Corrupt PNG"); // согласно DEFLATE, коды расстояния 30 и 31 не должны появляться в сжатых данных.
            dist = stbi__zdist_base[z];
            if(stbi__zdist_extra[z]) dist += stbi__zreceive(a, stbi__zdist_extra[z]);
            if(zout - a->zout_start < dist) return stbi__err("bad dist", "Corrupt PNG");
            if(len > a->zout_end - zout) {
                if(!stbi__zexpand(a, zout, len)) return 0;
                zout = a->zout;
            }
            p = (stbi_uc *)(zout - dist);
            if(dist == 1) {  // пробег одного байта; часто встречается на изображениях.
                stbi_uc v = *p;
                if(len) {
                    do * zout++ = v;
                    while(--len);
                }
            }
            else {
                if(len) {
                    do * zout++ = *p++;
                    while(--len);
                }
            }
        }
    }
}

static int stbi__compute_huffman_codes(stbi__zbuf * a)
{
    static const stbi_uc length_dezigzag[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
    stbi__zhuffman z_codelength;
    stbi_uc lencodes[286 + 32 + 137]; //заполнение для максимальной одиночной операции
    stbi_uc codelength_sizes[19];
    int i, n;

    int hlit  = stbi__zreceive(a, 5) + 257;
    int hdist = stbi__zreceive(a, 5) + 1;
    int hclen = stbi__zreceive(a, 4) + 4;
    int ntot  = hlit + hdist;

    memset(codelength_sizes, 0, sizeof(codelength_sizes));
    for(i = 0; i < hclen; ++i) {
        int s = stbi__zreceive(a, 3);
        codelength_sizes[length_dezigzag[i]] = (stbi_uc) s;
    }
    if(!stbi__zbuild_huffman(&z_codelength, codelength_sizes, 19)) return 0;

    n = 0;
    while(n < ntot) {
        int c = stbi__zhuffman_decode(a, &z_codelength);
        if(c < 0 || c >= 19) return stbi__err("bad codelengths", "Corrupt PNG");
        if(c < 16)
            lencodes[n++] = (stbi_uc) c;
        else {
            stbi_uc fill = 0;
            if(c == 16) {
                c = stbi__zreceive(a, 2) + 3;
                if(n == 0) return stbi__err("bad codelengths", "Corrupt PNG");
                fill = lencodes[n - 1];
            }
            else if(c == 17) {
                c = stbi__zreceive(a, 3) + 3;
            }
            else if(c == 18) {
                c = stbi__zreceive(a, 7) + 11;
            }
            else {
                return stbi__err("bad codelengths", "Corrupt PNG");
            }
            if(ntot - n < c) return stbi__err("bad codelengths", "Corrupt PNG");
            memset(lencodes + n, fill, c);
            n += c;
        }
    }
    if(n != ntot) return stbi__err("bad codelengths", "Corrupt PNG");
    if(!stbi__zbuild_huffman(&a->z_length, lencodes, hlit)) return 0;
    if(!stbi__zbuild_huffman(&a->z_distance, lencodes + hlit, hdist)) return 0;
    return 1;
}

static int stbi__parse_uncompressed_block(stbi__zbuf * a)
{
    stbi_uc header[4];
    int len, nlen, k;
    if(a->num_bits & 7)
        stbi__zreceive(a, a->num_bits & 7); // выбросить
    // слить битовые данные в заголовок
    k = 0;
    while(a->num_bits > 0) {
        header[k++] = (stbi_uc)(a->code_buffer & 255);  // подавить проверку времени выполнения MSVC
        a->code_buffer >>= 8;
        a->num_bits -= 8;
    }
    if(a->num_bits < 0) return stbi__err("zlib corrupt", "Corrupt PNG");
    // теперь заполните заголовок обычным способом
    while(k < 4)
        header[k++] = stbi__zget8(a);
    len  = header[1] * 256 + header[0];
    nlen = header[3] * 256 + header[2];
    if(nlen != (len ^ 0xffff)) return stbi__err("zlib corrupt", "Corrupt PNG");
    if(a->zbuffer + len > a->zbuffer_end) return stbi__err("read past buffer", "Corrupt PNG");
    if(a->zout + len > a->zout_end)
        if(!stbi__zexpand(a, a->zout, len)) return 0;
    memcpy(a->zout, a->zbuffer, len);
    a->zbuffer += len;
    a->zout += len;
    return 1;
}

static int stbi__parse_zlib_header(stbi__zbuf * a)
{
    int cmf   = stbi__zget8(a);
    int cm    = cmf & 15;
    /* int cinfo = cmf >> 4; */
    int flg   = stbi__zget8(a);
    if(stbi__zeof(a)) return stbi__err("bad zlib header", "Corrupt PNG"); // спецификация zlib
    if((cmf * 256 + flg) % 31 != 0) return stbi__err("bad zlib header", "Corrupt PNG"); // спецификация zlib
    if(flg & 32) return stbi__err("no preset dict", "Corrupt PNG"); // предустановленный словарь не разрешен в формате png
    if(cm != 8) return stbi__err("bad compression", "Corrupt PNG"); // Для png требуется DEFLATE
    // window = 1 << (8 + cinfo)... but who cares, we fully buffer output
    return 1;
}

static const stbi_uc stbi__zdefault_length[STBI__ZNSYMS] = {
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8
};
static const stbi_uc stbi__zdefault_distance[32] = {
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
};
/*
Алгоритм инициализации:
{
   интервал я;   // используйте <= для четкого соответствия спецификации
   для (i=0; i <= 143; ++i) stbi__zdefault_length [i] = 8;
   для ( ; i <= 255; ++i) stbi__zdefault_length [i] = 9;
   для ( ; i <= 279; ++i) stbi__zdefault_length [i] = 7;
   для ( ; i <= 287; ++i) stbi__zdefault_length [i] = 8;

   для (i=0; i <= 31; ++i) stbi__zdefault_distance [i] = 5;
}
*/

static int stbi__parse_zlib(stbi__zbuf * a, int parse_header)
{
    int final, type;
    if(parse_header)
        if(!stbi__parse_zlib_header(a)) return 0;
    a->num_bits = 0;
    a->code_buffer = 0;
    a->hit_zeof_once = 0;
    do {
        final = stbi__zreceive(a, 1);
        type = stbi__zreceive(a, 2);
        if(type == 0) {
            if(!stbi__parse_uncompressed_block(a)) return 0;
        }
        else if(type == 3) {
            return 0;
        }
        else {
            if(type == 1) {
                // используйте фиксированную длину кода
                if(!stbi__zbuild_huffman(&a->z_length, stbi__zdefault_length, STBI__ZNSYMS)) return 0;
                if(!stbi__zbuild_huffman(&a->z_distance, stbi__zdefault_distance,  32)) return 0;
            }
            else {
                if(!stbi__compute_huffman_codes(a)) return 0;
            }
            if(!stbi__parse_huffman_block(a)) return 0;
        }
    } while(!final);
    return 1;
}

static int stbi__do_zlib(stbi__zbuf * a, char * obuf, int olen, int exp, int parse_header)
{
    a->zout_start = obuf;
    a->zout       = obuf;
    a->zout_end   = obuf + olen;
    a->z_expandable = exp;

    return stbi__parse_zlib(a, parse_header);
}

STBIDEF char * stbi_zlib_decode_malloc_guesssize(const char * buffer, int len, int initial_size, int * outlen)
{
    stbi__zbuf a;
    char * p = (char *) stbi__malloc(initial_size);
    if(p == NULL) return NULL;
    a.zbuffer = (stbi_uc *) buffer;
    a.zbuffer_end = (stbi_uc *) buffer + len;
    if(stbi__do_zlib(&a, p, initial_size, 1, 1)) {
        if(outlen) *outlen = (int)(a.zout - a.zout_start);
        return a.zout_start;
    }
    else {
        STBI_FREE(a.zout_start);
        return NULL;
    }
}

STBIDEF char * stbi_zlib_decode_malloc(char const * buffer, int len, int * outlen)
{
    return stbi_zlib_decode_malloc_guesssize(buffer, len, 16384, outlen);
}

STBIDEF char * stbi_zlib_decode_malloc_guesssize_headerflag(const char * buffer, int len, int initial_size,
                                                            int * outlen, int parse_header)
{
    stbi__zbuf a;
    char * p = (char *) stbi__malloc(initial_size);
    if(p == NULL) return NULL;
    a.zbuffer = (stbi_uc *) buffer;
    a.zbuffer_end = (stbi_uc *) buffer + len;
    if(stbi__do_zlib(&a, p, initial_size, 1, parse_header)) {
        if(outlen) *outlen = (int)(a.zout - a.zout_start);
        return a.zout_start;
    }
    else {
        STBI_FREE(a.zout_start);
        return NULL;
    }
}

STBIDEF int stbi_zlib_decode_buffer(char * obuffer, int olen, char const * ibuffer, int ilen)
{
    stbi__zbuf a;
    a.zbuffer = (stbi_uc *) ibuffer;
    a.zbuffer_end = (stbi_uc *) ibuffer + ilen;
    if(stbi__do_zlib(&a, obuffer, olen, 0, 1))
        return (int)(a.zout - a.zout_start);
    else
        return -1;
}

STBIDEF char * stbi_zlib_decode_noheader_malloc(char const * buffer, int len, int * outlen)
{
    stbi__zbuf a;
    char * p = (char *) stbi__malloc(16384);
    if(p == NULL) return NULL;
    a.zbuffer = (stbi_uc *) buffer;
    a.zbuffer_end = (stbi_uc *) buffer + len;
    if(stbi__do_zlib(&a, p, 16384, 1, 0)) {
        if(outlen) *outlen = (int)(a.zout - a.zout_start);
        return a.zout_start;
    }
    else {
        STBI_FREE(a.zout_start);
        return NULL;
    }
}

STBIDEF int stbi_zlib_decode_noheader_buffer(char * obuffer, int olen, const char * ibuffer, int ilen)
{
    stbi__zbuf a;
    a.zbuffer = (stbi_uc *) ibuffer;
    a.zbuffer_end = (stbi_uc *) ibuffer + ilen;
    if(stbi__do_zlib(&a, obuffer, olen, 0, 0))
        return (int)(a.zout - a.zout_start);
    else
        return -1;
}
#endif

// общедоступный «базовый» декодер PNG v0.10 Шон Барретт 18 ноября 2006 г.
//    простая реализация
//      - only 8-bit samples
//      - no CRC checking
//      - allocates lots of intermediate memory
//        - avoids problem of streaming data between subsystems
//        - avoids explicit window management
//    производительность
//      - uses stb_zlib, a PD zlib implementation with fast huffman decoding

#ifndef STBI_NO_PNG
typedef struct {
    stbi__uint32 length;
    stbi__uint32 type;
} stbi__pngchunk;

static stbi__pngchunk stbi__get_chunk_header(stbi__context * s)
{
    stbi__pngchunk c;
    c.length = stbi__get32be(s);
    c.type   = stbi__get32be(s);
    return c;
}

static int stbi__check_png_header(stbi__context * s)
{
    static const stbi_uc png_sig[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
    int i;
    for(i = 0; i < 8; ++i)
        if(stbi__get8(s) != png_sig[i]) return stbi__err("bad png sig", "Not a PNG");
    return 1;
}

typedef struct {
    stbi__context * s;
    stbi_uc * idata, * expanded, * out;
    int depth;
} stbi__png;


enum {
    STBI__F_none = 0,
    STBI__F_sub = 1,
    STBI__F_up = 2,
    STBI__F_avg = 3,
    STBI__F_paeth = 4,
    // синтетический фильтр, используемый для первой строки сканирования, чтобы избежать необходимости использования фиктивной строки из 0
    STBI__F_avg_first
};

static stbi_uc first_row_filter[5] = {
    STBI__F_none,
    STBI__F_sub,
    STBI__F_none,
    STBI__F_avg_first,
    STBI__F_sub // Paeth с b=c=0 оказывается эквивалентным sub
};

static int stbi__paeth(int a, int b, int c)
{
    // Эта формулировка сильно отличается от ссылки в спецификации PNG, но
    // фактически эквивалентен, имеет благоприятные зависимости данных и допускает прямое
    // генерация кода без ветвей, что значительно повышает производительность.
    int thresh = c * 3 - (a + b);
    int lo = a < b ? a : b;
    int hi = a < b ? b : a;
    int t0 = (hi <= thresh) ? lo : c;
    int t1 = (thresh <= lo) ? hi : t0;
    return t1;
}

static const stbi_uc stbi__depth_scale_table[9] = { 0, 0xff, 0x55, 0, 0x11, 0, 0, 0, 0x01 };

// добавляет дополнительный альфа-канал all-255
// dest == src допустимо
// img_n должно быть 1 или 3.
static void stbi__create_png_alpha_expand8(stbi_uc * dest, stbi_uc * src, stbi__uint32 x, int img_n)
{
    int i;
    // должен обрабатывать данные в обратном порядке, поскольку мы разрешаем dest==src
    if(img_n == 1) {
        for(i = x - 1; i >= 0; --i) {
            dest[i * 2 + 1] = 255;
            dest[i * 2 + 0] = src[i];
        }
    }
    else {
        STBI_ASSERT(img_n == 3);
        for(i = x - 1; i >= 0; --i) {
            dest[i * 4 + 3] = 255;
            dest[i * 4 + 2] = src[i * 3 + 2];
            dest[i * 4 + 1] = src[i * 3 + 1];
            dest[i * 4 + 0] = src[i * 3 + 0];
        }
    }
}

// создать данные png из постдефлированных данных
static int stbi__create_png_image_raw(stbi__png * a, stbi_uc * raw, stbi__uint32 raw_len, int out_n, stbi__uint32 x,
                                      stbi__uint32 y, int depth, int color)
{
    int bytes = (depth == 16 ? 2 : 1);
    stbi__context * s = a->s;
    stbi__uint32 i, j, stride = x * out_n * bytes;
    stbi__uint32 img_len, img_width_bytes;
    stbi_uc * filter_buf;
    int all_ok = 1;
    int k;
    int img_n = s->img_n; // скопируйте его в локальную папку, чтобы позже

    int output_bytes = out_n * bytes;
    int filter_bytes = img_n * bytes;
    int width = x;

    STBI_ASSERT(out_n == s->img_n || out_n == s->img_n + 1);
    a->out = (stbi_uc *) stbi__malloc_mad3(x, y, output_bytes, 0); // дополнительные байты для списания конца в
    if(!a->out) return stbi__err("outofmem", "Out of memory");

    // note: error exits here don't need to clean up a->out individually,
    // stbi__do_png всегда делает это при ошибке.
    if(!stbi__mad3sizes_valid(img_n, x, depth, 7)) return stbi__err("too large", "Corrupt PNG");
    img_width_bytes = (((img_n * x * depth) + 7) >> 3);
    if(!stbi__mad2sizes_valid(img_width_bytes, y, img_width_bytes)) return stbi__err("too large", "Corrupt PNG");
    img_len = (img_width_bytes + 1) * y;

    // раньше мы проверяли точное совпадение между raw_len и img_len в нечересстрочных PNG-файлах,
    // но проблема № 276 сообщала о PNG в дикой природе, у которого в конце были дополнительные данные (все нули),
    // так что просто всегда проверяйте raw_len < img_len.
    if(raw_len < img_len) return stbi__err("not enough pixels", "Corrupt PNG");

    // Выделите две строки сканирования в буфере рабочей области фильтра.
    filter_buf = (stbi_uc *) stbi__malloc_mad2(img_width_bytes, 2, 0);
    if(!filter_buf) return stbi__err("outofmem", "Out of memory");

    // Фильтрация изображений с низкой битовой глубиной
    if(depth < 8) {
        filter_bytes = 1;
        width = img_width_bytes;
    }

    for(j = 0; j < y; ++j) {
        // Буферы текущего/предыдущего фильтра чередуются
        stbi_uc * cur = filter_buf + (j & 1) * img_width_bytes;
        stbi_uc * prior = filter_buf + (~j & 1) * img_width_bytes;
        stbi_uc * dest = a->out + stride * j;
        int nk = width * filter_bytes;
        int filter = *raw++;

        // проверьте тип фильтра
        if(filter > 4) {
            all_ok = stbi__err("invalid filter", "Corrupt PNG");
            break;
        }

        // если первая строка, используйте специальный фильтр, который не выбирает предыдущую строку
        if(j == 0) filter = first_row_filter[filter];

        // выполнить фактическую фильтрацию
        switch(filter) {
            case STBI__F_none:
                memcpy(cur, raw, nk);
                break;
            case STBI__F_sub:
                memcpy(cur, raw, filter_bytes);
                for(k = filter_bytes; k < nk; ++k)
                    cur[k] = STBI__BYTECAST(raw[k] + cur[k - filter_bytes]);
                break;
            case STBI__F_up:
                for(k = 0; k < nk; ++k)
                    cur[k] = STBI__BYTECAST(raw[k] + prior[k]);
                break;
            case STBI__F_avg:
                for(k = 0; k < filter_bytes; ++k)
                    cur[k] = STBI__BYTECAST(raw[k] + (prior[k] >> 1));
                for(k = filter_bytes; k < nk; ++k)
                    cur[k] = STBI__BYTECAST(raw[k] + ((prior[k] + cur[k - filter_bytes]) >> 1));
                break;
            case STBI__F_paeth:
                for(k = 0; k < filter_bytes; ++k)
                    cur[k] = STBI__BYTECAST(raw[k] + prior[k]); // предшествующий[k] == stbi__paeth (0,предыдущий[k],0)
                for(k = filter_bytes; k < nk; ++k)
                    cur[k] = STBI__BYTECAST(raw[k] + stbi__paeth(cur[k - filter_bytes], prior[k], prior[k - filter_bytes]));
                break;
            case STBI__F_avg_first:
                memcpy(cur, raw, filter_bytes);
                for(k = filter_bytes; k < nk; ++k)
                    cur[k] = STBI__BYTECAST(raw[k] + (cur[k - filter_bytes] >> 1));
                break;
        }

        raw += nk;

        // расширить декодированные биты в cur до dest, а также добавить дополнительный альфа-канал, если это необходимо
        if(depth < 8) {
            stbi_uc scale = (color == 0) ? stbi__depth_scale_table[depth] : 1; // масштабировать значения оттенков серого в диапазоне 0..255
            stbi_uc * in = cur;
            stbi_uc * out = dest;
            stbi_uc inb = 0;
            stbi__uint32 nsmp = x * img_n;

            // сначала расширить биты до байтов
            if(depth == 4) {
                for(i = 0; i < nsmp; ++i) {
                    if((i & 1) == 0) inb = *in++;
                    *out++ = scale * (inb >> 4);
                    inb <<= 4;
                }
            }
            else if(depth == 2) {
                for(i = 0; i < nsmp; ++i) {
                    if((i & 3) == 0) inb = *in++;
                    *out++ = scale * (inb >> 6);
                    inb <<= 2;
                }
            }
            else {
                STBI_ASSERT(depth == 1);
                for(i = 0; i < nsmp; ++i) {
                    if((i & 7) == 0) inb = *in++;
                    *out++ = scale * (inb >> 7);
                    inb <<= 1;
                }
            }

            // при необходимости вставьте значения альфа=255
            if(img_n != out_n)
                stbi__create_png_alpha_expand8(dest, dest, x, img_n);
        }
        else if(depth == 8) {
            if(img_n == out_n)
                memcpy(dest, cur, x * img_n);
            else
                stbi__create_png_alpha_expand8(dest, cur, x, img_n);
        }
        else if(depth == 16) {
            // преобразовать данные изображения из обратного порядка байтов в собственные для платформы
            stbi__uint16 * dest16 = (stbi__uint16 *)dest;
            stbi__uint32 nsmp = x * img_n;

            if(img_n == out_n) {
                for(i = 0; i < nsmp; ++i, ++dest16, cur += 2)
                    * dest16 = (cur[0] << 8) | cur[1];
            }
            else {
                STBI_ASSERT(img_n + 1 == out_n);
                if(img_n == 1) {
                    for(i = 0; i < x; ++i, dest16 += 2, cur += 2) {
                        dest16[0] = (cur[0] << 8) | cur[1];
                        dest16[1] = 0xffff;
                    }
                }
                else {
                    STBI_ASSERT(img_n == 3);
                    for(i = 0; i < x; ++i, dest16 += 4, cur += 6) {
                        dest16[0] = (cur[0] << 8) | cur[1];
                        dest16[1] = (cur[2] << 8) | cur[3];
                        dest16[2] = (cur[4] << 8) | cur[5];
                        dest16[3] = 0xffff;
                    }
                }
            }
        }
    }

    STBI_FREE(filter_buf);
    if(!all_ok) return 0;

    return 1;
}

static int stbi__create_png_image(stbi__png * a, stbi_uc * image_data, stbi__uint32 image_data_len, int out_n,
                                  int depth, int color, int interlaced)
{
    int bytes = (depth == 16 ? 2 : 1);
    int out_bytes = out_n * bytes;
    stbi_uc * final;
    int p;
    if(!interlaced)
        return stbi__create_png_image_raw(a, image_data, image_data_len, out_n, a->s->img_x, a->s->img_y, depth, color);

    // деинтерлейсинг
    final = (stbi_uc *) stbi__malloc_mad3(a->s->img_x, a->s->img_y, out_bytes, 0);
    if(!final) return stbi__err("outofmem", "Out of memory");
    for(p = 0; p < 7; ++p) {
        int xorig[] = { 0, 4, 0, 2, 0, 1, 0 };
        int yorig[] = { 0, 0, 4, 0, 2, 0, 1 };
        int xspc[]  = { 8, 8, 4, 4, 2, 2, 1 };
        int yspc[]  = { 8, 8, 8, 4, 4, 2, 2 };
        int i, j, x, y;
        // pass1_x [4] = 0, pass1_x [5] = 1, pass1_x [12] = 1
        x = (a->s->img_x - xorig[p] + xspc[p] - 1) / xspc[p];
        y = (a->s->img_y - yorig[p] + yspc[p] - 1) / yspc[p];
        if(x && y) {
            stbi__uint32 img_len = ((((a->s->img_n * x * depth) + 7) >> 3) + 1) * y;
            if(!stbi__create_png_image_raw(a, image_data, image_data_len, out_n, x, y, depth, color)) {
                STBI_FREE(final);
                return 0;
            }
            for(j = 0; j < y; ++j) {
                for(i = 0; i < x; ++i) {
                    int out_y = j * yspc[p] + yorig[p];
                    int out_x = i * xspc[p] + xorig[p];
                    memcpy(final + out_y * a->s->img_x * out_bytes + out_x * out_bytes,
                           a->out + (j * x + i)*out_bytes, out_bytes);
                }
            }
            STBI_FREE(a->out);
            image_data += img_len;
            image_data_len -= img_len;
        }
    }
    a->out = final;

    return 1;
}

static int stbi__compute_transparency(stbi__png * z, stbi_uc tc[3], int out_n)
{
    stbi__context * s = z->s;
    stbi__uint32 i, pixel_count = s->img_x * s->img_y;
    stbi_uc * p = z->out;

    // вычислить прозрачность на основе цвета, предполагая, что мы
    // уже получил 255 в качестве альфа-значения на выходе
    STBI_ASSERT(out_n == 2 || out_n == 4);

    if(out_n == 2) {
        for(i = 0; i < pixel_count; ++i) {
            p[1] = (p[0] == tc[0] ? 0 : 255);
            p += 2;
        }
    }
    else {
        for(i = 0; i < pixel_count; ++i) {
            if(p[0] == tc[0] && p[1] == tc[1] && p[2] == tc[2])
                p[3] = 0;
            p += 4;
        }
    }
    return 1;
}

static int stbi__compute_transparency16(stbi__png * z, stbi__uint16 tc[3], int out_n)
{
    stbi__context * s = z->s;
    stbi__uint32 i, pixel_count = s->img_x * s->img_y;
    stbi__uint16 * p = (stbi__uint16 *) z->out;

    // вычислить прозрачность на основе цвета, предполагая, что мы
    // уже получил 65535 в качестве альфа-значения на выходе
    STBI_ASSERT(out_n == 2 || out_n == 4);

    if(out_n == 2) {
        for(i = 0; i < pixel_count; ++i) {
            p[1] = (p[0] == tc[0] ? 0 : 65535);
            p += 2;
        }
    }
    else {
        for(i = 0; i < pixel_count; ++i) {
            if(p[0] == tc[0] && p[1] == tc[1] && p[2] == tc[2])
                p[3] = 0;
            p += 4;
        }
    }
    return 1;
}

static int stbi__expand_png_palette(stbi__png * a, stbi_uc * palette, int len, int pal_img_n)
{
    stbi__uint32 i, pixel_count = a->s->img_x * a->s->img_y;
    stbi_uc * p, * temp_out, * orig = a->out;

    p = (stbi_uc *) stbi__malloc_mad2(pixel_count, pal_img_n, 0);
    if(p == NULL) return stbi__err("outofmem", "Out of memory");

    // между здесь и свободным(вне) внизу выход будет утечкой
    temp_out = p;

    if(pal_img_n == 3) {
        for(i = 0; i < pixel_count; ++i) {
            int n = orig[i] * 4;
            p[0] = palette[n  ];
            p[1] = palette[n + 1];
            p[2] = palette[n + 2];
            p += 3;
        }
    }
    else {
        for(i = 0; i < pixel_count; ++i) {
            int n = orig[i] * 4;
            p[0] = palette[n  ];
            p[1] = palette[n + 1];
            p[2] = palette[n + 2];
            p[3] = palette[n + 3];
            p += 4;
        }
    }
    STBI_FREE(a->out);
    a->out = temp_out;

    STBI_NOTUSED(len);

    return 1;
}

static int stbi__unpremultiply_on_load_global = 0;
static int stbi__de_iphone_flag_global = 0;

STBIDEF void stbi_set_unpremultiply_on_load(int flag_true_if_should_unpremultiply)
{
    stbi__unpremultiply_on_load_global = flag_true_if_should_unpremultiply;
}

STBIDEF void stbi_convert_iphone_png_to_rgb(int flag_true_if_should_convert)
{
    stbi__de_iphone_flag_global = flag_true_if_should_convert;
}

#ifndef STBI_THREAD_LOCAL
#define stbi__unpremultiply_on_load  stbi__unpremultiply_on_load_global
#define stbi__de_iphone_flag  stbi__de_iphone_flag_global
#else
static STBI_THREAD_LOCAL int stbi__unpremultiply_on_load_local, stbi__unpremultiply_on_load_set;
static STBI_THREAD_LOCAL int stbi__de_iphone_flag_local, stbi__de_iphone_flag_set;

STBIDEF void stbi_set_unpremultiply_on_load_thread(int flag_true_if_should_unpremultiply)
{
    stbi__unpremultiply_on_load_local = flag_true_if_should_unpremultiply;
    stbi__unpremultiply_on_load_set = 1;
}

STBIDEF void stbi_convert_iphone_png_to_rgb_thread(int flag_true_if_should_convert)
{
    stbi__de_iphone_flag_local = flag_true_if_should_convert;
    stbi__de_iphone_flag_set = 1;
}

#define stbi__unpremultiply_on_load  (stbi__unpremultiply_on_load_set           \
                                      ? stbi__unpremultiply_on_load_local      \
                                      : stbi__unpremultiply_on_load_global)
#define stbi__de_iphone_flag  (stbi__de_iphone_flag_set                         \
                               ? stbi__de_iphone_flag_local                    \
                               : stbi__de_iphone_flag_global)
#endif // STBI_THREAD_LOCAL

static void stbi__de_iphone(stbi__png * z)
{
    stbi__context * s = z->s;
    stbi__uint32 i, pixel_count = s->img_x * s->img_y;
    stbi_uc * p = z->out;

    if(s->img_out_n == 3) {   // конвертировать bgr в rgb
        for(i = 0; i < pixel_count; ++i) {
            stbi_uc t = p[0];
            p[0] = p[2];
            p[2] = t;
            p += 3;
        }
    }
    else {
        STBI_ASSERT(s->img_out_n == 4);
        if(stbi__unpremultiply_on_load) {
            // конвертировать bgr в rgb и без предварительного умножения
            for(i = 0; i < pixel_count; ++i) {
                stbi_uc a = p[3];
                stbi_uc t = p[0];
                if(a) {
                    stbi_uc half = a / 2;
                    p[0] = (p[2] * 255 + half) / a;
                    p[1] = (p[1] * 255 + half) / a;
                    p[2] = (t   * 255 + half) / a;
                }
                else {
                    p[0] = p[2];
                    p[2] = t;
                }
                p += 4;
            }
        }
        else {
            // конвертировать bgr в rgb
            for(i = 0; i < pixel_count; ++i) {
                stbi_uc t = p[0];
                p[0] = p[2];
                p[2] = t;
                p += 4;
            }
        }
    }
}

#define STBI__PNG_TYPE(a,b,c,d)  (((unsigned) (a) << 24) + ((unsigned) (b) << 16) + ((unsigned) (c) << 8) + (unsigned) (d))

static int stbi__parse_png_file(stbi__png * z, int scan, int req_comp)
{
    stbi_uc palette[1024], pal_img_n = 0;
    stbi_uc has_trans = 0, tc[3] = {0};
    stbi__uint16 tc16[3];
    stbi__uint32 ioff = 0, idata_limit = 0, i, pal_len = 0;
    int first = 1, k, interlace = 0, color = 0, is_iphone = 0;
    stbi__context * s = z->s;

    z->expanded = NULL;
    z->idata = NULL;
    z->out = NULL;

    if(!stbi__check_png_header(s)) return 0;

    if(scan == STBI__SCAN_type) return 1;

    for(;;) {
        stbi__pngchunk c = stbi__get_chunk_header(s);
        switch(c.type) {
            case STBI__PNG_TYPE('C', 'g', 'B', 'I'):
                is_iphone = 1;
                stbi__skip(s, c.length);
                break;
            case STBI__PNG_TYPE('I', 'H', 'D', 'R'): {
                    int comp, filter;
                    if(!first) return stbi__err("multiple IHDR", "Corrupt PNG");
                    first = 0;
                    if(c.length != 13) return stbi__err("bad IHDR len", "Corrupt PNG");
                    s->img_x = stbi__get32be(s);
                    s->img_y = stbi__get32be(s);
                    if(s->img_y > STBI_MAX_DIMENSIONS) return stbi__err("too large", "Very large image (corrupt?)");
                    if(s->img_x > STBI_MAX_DIMENSIONS) return stbi__err("too large", "Very large image (corrupt?)");
                    z->depth = stbi__get8(s);
                    if(z->depth != 1 && z->depth != 2 && z->depth != 4 && z->depth != 8 &&
                       z->depth != 16)  return stbi__err("1/2/4/8/16-bit only", "PNG not supported: 1/2/4/8/16-bit only");
                    color = stbi__get8(s);
                    if(color > 6)         return stbi__err("bad ctype", "Corrupt PNG");
                    if(color == 3 && z->depth == 16)                  return stbi__err("bad ctype", "Corrupt PNG");
                    if(color == 3) pal_img_n = 3;
                    else if(color & 1) return stbi__err("bad ctype", "Corrupt PNG");
                    comp  = stbi__get8(s);
                    if(comp) return stbi__err("bad comp method", "Corrupt PNG");
                    filter = stbi__get8(s);
                    if(filter) return stbi__err("bad filter method", "Corrupt PNG");
                    interlace = stbi__get8(s);
                    if(interlace > 1) return stbi__err("bad interlace method", "Corrupt PNG");
                    if(!s->img_x || !s->img_y) return stbi__err("0-pixel image", "Corrupt PNG");
                    if(!pal_img_n) {
                        s->img_n = (color & 2 ? 3 : 1) + (color & 4 ? 1 : 0);
                        if((1 << 30) / s->img_x / s->img_n < s->img_y) return stbi__err("too large", "Image too large to decode");
                    }
                    else {
                        // если палитра, то pal_n — наши конечные компоненты, и
                        // img_n — # компонентов для распаковки/фильтрации.
                        s->img_n = 1;
                        if((1 << 30) / s->img_x / 4 < s->img_y) return stbi__err("too large", "Corrupt PNG");
                    }
                    // даже с SCAN_header придется сканировать, чтобы увидеть, есть ли у нас tRNS
                    break;
                }

            case STBI__PNG_TYPE('P', 'L', 'T', 'E'):  {
                    if(first) return stbi__err("first not IHDR", "Corrupt PNG");
                    if(c.length > 256 * 3) return stbi__err("invalid PLTE", "Corrupt PNG");
                    pal_len = c.length / 3;
                    if(pal_len * 3 != c.length) return stbi__err("invalid PLTE", "Corrupt PNG");
                    for(i = 0; i < pal_len; ++i) {
                        palette[i * 4 + 0] = stbi__get8(s);
                        palette[i * 4 + 1] = stbi__get8(s);
                        palette[i * 4 + 2] = stbi__get8(s);
                        palette[i * 4 + 3] = 255;
                    }
                    break;
                }

            case STBI__PNG_TYPE('t', 'R', 'N', 'S'): {
                    if(first) return stbi__err("first not IHDR", "Corrupt PNG");
                    if(z->idata) return stbi__err("tRNS after IDAT", "Corrupt PNG");
                    if(pal_img_n) {
                        if(scan == STBI__SCAN_header) {
                            s->img_n = 4;
                            return 1;
                        }
                        if(pal_len == 0) return stbi__err("tRNS before PLTE", "Corrupt PNG");
                        if(c.length > pal_len) return stbi__err("bad tRNS len", "Corrupt PNG");
                        pal_img_n = 4;
                        for(i = 0; i < c.length; ++i)
                            palette[i * 4 + 3] = stbi__get8(s);
                    }
                    else {
                        if(!(s->img_n & 1)) return stbi__err("tRNS with alpha", "Corrupt PNG");
                        if(c.length != (stbi__uint32) s->img_n * 2) return stbi__err("bad tRNS len", "Corrupt PNG");
                        has_trans = 1;
                        // без палитры с tRNS = константа альфа. если сканирование заголовков, мы можем остановиться сейчас.
                        if(scan == STBI__SCAN_header) {
                            ++s->img_n;
                            return 1;
                        }
                        if(z->depth == 16) {
                            for(k = 0; k < s->img_n && k < 3; ++k)  // дополнительный тест контура для подавления ложного предупреждения GCC
                                tc16[k] = (stbi__uint16)stbi__get16be(s); // скопируйте значения как есть
                        }
                        else {
                            for(k = 0; k < s->img_n && k < 3; ++k)
                                tc[k] = (stbi_uc)(stbi__get16be(s) & 255) * stbi__depth_scale_table[z->depth]; // не8-битные изображения будут больше
                        }
                    }
                    break;
                }

            case STBI__PNG_TYPE('I', 'D', 'A', 'T'): {
                    if(first) return stbi__err("first not IHDR", "Corrupt PNG");
                    if(pal_img_n && !pal_len) return stbi__err("no PLTE", "Corrupt PNG");
                    if(scan == STBI__SCAN_header) {
                        // сканирование заголовка определенно останавливается сначала IDAT
                        if(pal_img_n)
                            s->img_n = pal_img_n;
                        return 1;
                    }
                    if(c.length > (1u << 30)) return stbi__err("IDAT size limit", "IDAT section larger than 2^30 bytes");
                    if((int)(ioff + c.length) < (int)ioff) return 0;
                    if(ioff + c.length > idata_limit) {
                        stbi__uint32 idata_limit_old = idata_limit;
                        stbi_uc * p;
                        if(idata_limit == 0) idata_limit = c.length > 4096 ? c.length : 4096;
                        while(ioff + c.length > idata_limit)
                            idata_limit *= 2;
                        STBI_NOTUSED(idata_limit_old);
                        p = (stbi_uc *) STBI_REALLOC_SIZED(z->idata, idata_limit_old, idata_limit);
                        if(p == NULL) return stbi__err("outofmem", "Out of memory");
                        z->idata = p;
                    }
                    if(!stbi__getn(s, z->idata + ioff, c.length)) return stbi__err("outofdata", "Corrupt PNG");
                    ioff += c.length;
                    break;
                }

            case STBI__PNG_TYPE('I', 'E', 'N', 'D'): {
                    stbi__uint32 raw_len, bpl;
                    if(first) return stbi__err("first not IHDR", "Corrupt PNG");
                    if(scan != STBI__SCAN_load) return 1;
                    if(z->idata == NULL) return stbi__err("no IDAT", "Corrupt PNG");
                    // первоначальное предположение о размере декодированных данных, чтобы избежать ненужного перераспределения
                    bpl = (s->img_x * z->depth + 7) / 8; // байт в строке, на компонент
                    raw_len = bpl * s->img_y * s->img_n /* пиксели */ + s->img_y /* режим фильтра для каждой строки */;
                    z->expanded = (stbi_uc *) stbi_zlib_decode_malloc_guesssize_headerflag((char *) z->idata, ioff, raw_len,
                                                                                           (int *) &raw_len, !is_iphone);
                    if(z->expanded == NULL) return 0;  // zlib должен установить ошибку
                    STBI_FREE(z->idata);
                    z->idata = NULL;
                    if((req_comp == s->img_n + 1 && req_comp != 3 && !pal_img_n) || has_trans)
                        s->img_out_n = s->img_n + 1;
                    else
                        s->img_out_n = s->img_n;
                    if(!stbi__create_png_image(z, z->expanded, raw_len, s->img_out_n, z->depth, color, interlace)) return 0;
                    if(has_trans) {
                        if(z->depth == 16) {
                            if(!stbi__compute_transparency16(z, tc16, s->img_out_n)) return 0;
                        }
                        else {
                            if(!stbi__compute_transparency(z, tc, s->img_out_n)) return 0;
                        }
                    }
                    if(is_iphone && stbi__de_iphone_flag && s->img_out_n > 2)
                        stbi__de_iphone(z);
                    if(pal_img_n) {
                        // pal_img_n == 3 или 4
                        s->img_n = pal_img_n; // запишите реальные цвета, которые у нас были
                        s->img_out_n = pal_img_n;
                        if(req_comp >= 3) s->img_out_n = req_comp;
                        if(!stbi__expand_png_palette(z, palette, pal_len, s->img_out_n))
                            return 0;
                    }
                    else if(has_trans) {
                        // изображение без палитры с tRNS -> исходное изображение имеет (постоянную) альфу
                        ++s->img_n;
                    }
                    STBI_FREE(z->expanded);
                    z->expanded = NULL;
                    // конец фрагмента PNG, прочитайте и пропустите CRC
                    stbi__get32be(s);
                    return 1;
                }

            default:
                // если критично, провалите
                if(first) return stbi__err("first not IHDR", "Corrupt PNG");
                if((c.type & (1 << 29)) == 0) {
#ifndef STBI_NO_FAILURE_STRINGS
                    // не потокобезопасный
                    static char invalid_chunk[] = "XXXX PNG chunk not known";
                    invalid_chunk[0] = STBI__BYTECAST(c.type >> 24);
                    invalid_chunk[1] = STBI__BYTECAST(c.type >> 16);
                    invalid_chunk[2] = STBI__BYTECAST(c.type >>  8);
                    invalid_chunk[3] = STBI__BYTECAST(c.type >>  0);
#endif
                    return stbi__err(invalid_chunk, "PNG not supported: unknown PNG chunk type");
                }
                stbi__skip(s, c.length);
                break;
        }
        // конец фрагмента PNG, прочитайте и пропустите CRC
        stbi__get32be(s);
    }
}

static void * stbi__do_png(stbi__png * p, int * x, int * y, int * n, int req_comp, stbi__result_info * ri)
{
    void * result = NULL;
    if(req_comp < 0 || req_comp > 4) return stbi__errpuc("bad req_comp", "Internal error");
    if(stbi__parse_png_file(p, STBI__SCAN_load, req_comp)) {
        if(p->depth <= 8)
            ri->bits_per_channel = 8;
        else if(p->depth == 16)
            ri->bits_per_channel = 16;
        else
            return stbi__errpuc("bad bits_per_channel", "PNG not supported: unsupported color depth");
        result = p->out;
        p->out = NULL;
        if(req_comp && req_comp != p->s->img_out_n) {
            if(ri->bits_per_channel == 8)
                result = stbi__convert_format((unsigned char *) result, p->s->img_out_n, req_comp, p->s->img_x, p->s->img_y);
            else
                result = stbi__convert_format16((stbi__uint16 *) result, p->s->img_out_n, req_comp, p->s->img_x, p->s->img_y);
            p->s->img_out_n = req_comp;
            if(result == NULL) return result;
        }
        *x = p->s->img_x;
        *y = p->s->img_y;
        if(n) *n = p->s->img_n;
    }
    STBI_FREE(p->out);
    p->out      = NULL;
    STBI_FREE(p->expanded);
    p->expanded = NULL;
    STBI_FREE(p->idata);
    p->idata    = NULL;

    return result;
}

static void * stbi__png_load(stbi__context * s, int * x, int * y, int * comp, int req_comp, stbi__result_info * ri)
{
    stbi__png p;
    p.s = s;
    return stbi__do_png(&p, x, y, comp, req_comp, ri);
}

static int stbi__png_test(stbi__context * s)
{
    int r;
    r = stbi__check_png_header(s);
    stbi__rewind(s);
    return r;
}

static int stbi__png_info_raw(stbi__png * p, int * x, int * y, int * comp)
{
    if(!stbi__parse_png_file(p, STBI__SCAN_header, 0)) {
        stbi__rewind(p->s);
        return 0;
    }
    if(x) *x = p->s->img_x;
    if(y) *y = p->s->img_y;
    if(comp) *comp = p->s->img_n;
    return 1;
}

static int stbi__png_info(stbi__context * s, int * x, int * y, int * comp)
{
    stbi__png p;
    p.s = s;
    return stbi__png_info_raw(&p, x, y, comp);
}

static int stbi__png_is16(stbi__context * s)
{
    stbi__png p;
    p.s = s;
    if(!stbi__png_info_raw(&p, NULL, NULL, NULL))
        return 0;
    if(p.depth != 16) {
        stbi__rewind(p.s);
        return 0;
    }
    return 1;
}
#endif

// Образ Microsoft/Windows BMP

#ifndef STBI_NO_BMP
static int stbi__bmp_test_raw(stbi__context * s)
{
    int r;
    int sz;
    if(stbi__get8(s) != 'B') return 0;
    if(stbi__get8(s) != 'M') return 0;
    stbi__get32le(s); // отказаться от размера файла
    stbi__get16le(s); // отбросить зарезервировано
    stbi__get16le(s); // отбросить зарезервировано
    stbi__get32le(s); // отбросить смещение данных
    sz = stbi__get32le(s);
    r = (sz == 12 || sz == 40 || sz == 56 || sz == 108 || sz == 124);
    return r;
}

static int stbi__bmp_test(stbi__context * s)
{
    int r = stbi__bmp_test_raw(s);
    stbi__rewind(s);
    return r;
}


// возвращает 0..31 для старшего установленного бита
static int stbi__high_bit(unsigned int z)
{
    int n = 0;
    if(z == 0) return -1;
    if(z >= 0x10000) {
        n += 16;
        z >>= 16;
    }
    if(z >= 0x00100) {
        n +=  8;
        z >>=  8;
    }
    if(z >= 0x00010) {
        n +=  4;
        z >>=  4;
    }
    if(z >= 0x00004) {
        n +=  2;
        z >>=  2;
    }
    if(z >= 0x00002) {
        n +=  1;/* >>=  1;*/
    }
    return n;
}

static int stbi__bitcount(unsigned int a)
{
    a = (a & 0x55555555) + ((a >>  1) & 0x55555555); // максимум 2
    a = (a & 0x33333333) + ((a >>  2) & 0x33333333); // максимум 4
    a = (a + (a >> 4)) & 0x0f0f0f0f; // максимум 8 на 4, теперь 8 бит
    a = (a + (a >> 8)); // максимум 16 на 8 бит
    a = (a + (a >> 16)); // максимум 32 на 8 бит
    return a & 0xff;
}

// извлечь произвольно выровненное N-битное значение (N=биты)
// из v, а затем сделать его длиной 8 бит и дробно
// расширьте его до полного диапазона.
static int stbi__shiftsigned(unsigned int v, int shift, int bits)
{
    static unsigned int mul_table[9] = {
        0,
        0xff/*0b11111111*/, 0x55/*0b01010101*/, 0x49/*0b01001001*/, 0x11/*0b00010001*/,
        0x21/*0b00100001*/, 0x41/*0b01000001*/, 0x81/*0b10000001*/, 0x01/*0b00000001*/,
    };
    static unsigned int shift_table[9] = {
        0, 0, 0, 1, 0, 2, 4, 6, 0,
    };
    if(shift < 0)
        v <<= -shift;
    else
        v >>= shift;
    STBI_ASSERT(v < 256);
    v >>= (8 - bits);
    STBI_ASSERT(bits >= 0 && bits <= 8);
    return (int)((unsigned) v * mul_table[bits]) >> shift_table[bits];
}

typedef struct {
    int bpp, offset, hsz;
    unsigned int mr, mg, mb, ma, all_a;
    int extra_read;
} stbi__bmp_data;

static int stbi__bmp_set_mask_defaults(stbi__bmp_data * info, int compress)
{
    // BI_BITFIELDS явно указывает маски, не переопределяйте их.
    if(compress == 3)
        return 1;

    if(compress == 0) {
        if(info->bpp == 16) {
            info->mr = 31u << 10;
            info->mg = 31u <<  5;
            info->mb = 31u <<  0;
        }
        else if(info->bpp == 32) {
            info->mr = 0xffu << 16;
            info->mg = 0xffu <<  8;
            info->mb = 0xffu <<  0;
            info->ma = 0xffu << 24;
            info->all_a = 0; // если all_a в конце равен 0, то мы загрузили альфа-канал, но все было 0
        }
        else {
            // в противном случае используйте значения по умолчанию, то есть все-0.
            info->mr = info->mg = info->mb = info->ma = 0;
        }
        return 1;
    }
    return 0; // ошибка
}

static void * stbi__bmp_parse_header(stbi__context * s, stbi__bmp_data * info)
{
    int hsz;
    if(stbi__get8(s) != 'B' || stbi__get8(s) != 'M') return stbi__errpuc("not BMP", "Corrupt BMP");
    stbi__get32le(s); // отказаться от размера файла
    stbi__get16le(s); // отбросить зарезервировано
    stbi__get16le(s); // отбросить зарезервировано
    info->offset = stbi__get32le(s);
    info->hsz = hsz = stbi__get32le(s);
    info->mr = info->mg = info->mb = info->ma = 0;
    info->extra_read = 14;

    if(info->offset < 0) return stbi__errpuc("bad BMP", "bad BMP");

    if(hsz != 12 && hsz != 40 && hsz != 56 && hsz != 108 &&
       hsz != 124) return stbi__errpuc("unknown BMP", "BMP type not supported: unknown");
    if(hsz == 12) {
        s->img_x = stbi__get16le(s);
        s->img_y = stbi__get16le(s);
    }
    else {
        s->img_x = stbi__get32le(s);
        s->img_y = stbi__get32le(s);
    }
    if(stbi__get16le(s) != 1) return stbi__errpuc("bad BMP", "bad BMP");
    info->bpp = stbi__get16le(s);
    if(hsz != 12) {
        int compress = stbi__get32le(s);
        if(compress == 1 || compress == 2) return stbi__errpuc("BMP RLE", "BMP type not supported: RLE");
        if(compress >= 4) return stbi__errpuc("BMP JPEG/PNG",
                                                  "BMP type not supported: unsupported compression");  // сюда входят режимы PNG/JPEG
        if(compress == 3 && info->bpp != 16 &&
           info->bpp != 32) return stbi__errpuc("bad BMP", "bad BMP");  // битовые поля требуют 16 или 32 бит/пиксель
        stbi__get32le(s); // отказаться от sizeof
        stbi__get32le(s); // отказаться от часов
        stbi__get32le(s); // отказаться от vres
        stbi__get32le(s); // отказаться от использованных цветов
        stbi__get32le(s); // отбросить максимально важное
        if(hsz == 40 || hsz == 56) {
            if(hsz == 56) {
                stbi__get32le(s);
                stbi__get32le(s);
                stbi__get32le(s);
                stbi__get32le(s);
            }
            if(info->bpp == 16 || info->bpp == 32) {
                if(compress == 0) {
                    stbi__bmp_set_mask_defaults(info, compress);
                }
                else if(compress == 3) {
                    info->mr = stbi__get32le(s);
                    info->mg = stbi__get32le(s);
                    info->mb = stbi__get32le(s);
                    info->extra_read += 12;
                    // не задокументировано, но создано в Photoshop и обработано mspaint
                    if(info->mr == info->mg && info->mg == info->mb) {
                        // ?!?!?
                        return stbi__errpuc("bad BMP", "bad BMP");
                    }
                }
                else
                    return stbi__errpuc("bad BMP", "bad BMP");
            }
        }
        else {
            // Заголовок V4/V5
            int i;
            if(hsz != 108 && hsz != 124)
                return stbi__errpuc("bad BMP", "bad BMP");
            info->mr = stbi__get32le(s);
            info->mg = stbi__get32le(s);
            info->mb = stbi__get32le(s);
            info->ma = stbi__get32le(s);
            if(compress != 3)  // переопределить mr/mg/mb, если только он не находится в режиме BI_BITFIELDS, согласно документам
                stbi__bmp_set_mask_defaults(info, compress);
            stbi__get32le(s); // отказаться от цветового пространства
            for(i = 0; i < 12; ++i)
                stbi__get32le(s); // отказаться от параметров цветового пространства
            if(hsz == 124) {
                stbi__get32le(s); // отказаться от намерения рендеринга
                stbi__get32le(s); // отбросить смещение данных профиля
                stbi__get32le(s); // отказаться от размера данных профиля
                stbi__get32le(s); // отбросить зарезервировано
            }
        }
    }
    return (void *) 1;
}


static void * stbi__bmp_load(stbi__context * s, int * x, int * y, int * comp, int req_comp, stbi__result_info * ri)
{
    stbi_uc * out;
    unsigned int mr = 0, mg = 0, mb = 0, ma = 0, all_a;
    stbi_uc pal[256][4];
    int psize = 0, i, j, width;
    int flip_vertically, pad, target;
    stbi__bmp_data info;
    STBI_NOTUSED(ri);

    info.all_a = 255;
    if(stbi__bmp_parse_header(s, &info) == NULL)
        return NULL; // код ошибки уже установлен

    flip_vertically = ((int) s->img_y) > 0;
    s->img_y = abs((int) s->img_y);

    if(s->img_y > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large", "Very large image (corrupt?)");
    if(s->img_x > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large", "Very large image (corrupt?)");

    mr = info.mr;
    mg = info.mg;
    mb = info.mb;
    ma = info.ma;
    all_a = info.all_a;

    if(info.hsz == 12) {
        if(info.bpp < 24)
            psize = (info.offset - info.extra_read - 24) / 3;
    }
    else {
        if(info.bpp < 16)
            psize = (info.offset - info.extra_read - info.hsz) >> 2;
    }
    if(psize == 0) {
        // принять некоторое количество дополнительных байтов после заголовка, но если смещение указывает либо на перед
        // заголовок заканчивается или подразумевает большое количество дополнительных данных, отклонить файл как некорректный
        int bytes_read_so_far = s->callback_already_read + (int)(s->img_buffer - s->img_buffer_original);
        int header_limit = 1024; // max, который мы на самом деле читаем, в настоящее время составляет менее 256 байт.
        int extra_data_limit = 256 * 4; // обычно здесь идет палитра; 256 записей*4 байта — максимальный размер.
        if(bytes_read_so_far <= 0 || bytes_read_so_far > header_limit) {
            return stbi__errpuc("bad header", "Corrupt BMP");
        }
        // мы установили, что bytes_read_so_far положителен и разумен.
        // первая половина этого теста отклоняет смещения, которые являются либо слишком маленькими положительными, либо
        // отрицательный и гарантирует, что info.offset >= bytes_read_so_far > 0. это, в свою очередь,
        // гарантирует, что число, вычисленное во второй половине теста, не сможет переполниться.
        if(info.offset < bytes_read_so_far || info.offset - bytes_read_so_far > extra_data_limit) {
            return stbi__errpuc("bad offset", "Corrupt BMP");
        }
        else {
            stbi__skip(s, info.offset - bytes_read_so_far);
        }
    }

    if(info.bpp == 24 && ma == 0xff000000)
        s->img_n = 3;
    else
        s->img_n = ma ? 4 : 3;
    if(req_comp && req_comp >= 3)  // мы можем напрямую декодировать 3 или 4
        target = req_comp;
    else
        target = s->img_n; // если они хотят монохромный, мы постконвертируем

    // размер проверки работоспособности
    if(!stbi__mad3sizes_valid(target, s->img_x, s->img_y, 0))
        return stbi__errpuc("too large", "Corrupt BMP");

    out = (stbi_uc *) stbi__malloc_mad3(target, s->img_x, s->img_y, 0);
    if(!out) return stbi__errpuc("outofmem", "Out of memory");
    if(info.bpp < 16) {
        int z = 0;
        if(psize == 0 || psize > 256) {
            STBI_FREE(out);
            return stbi__errpuc("invalid", "Corrupt BMP");
        }
        for(i = 0; i < psize; ++i) {
            pal[i][2] = stbi__get8(s);
            pal[i][1] = stbi__get8(s);
            pal[i][0] = stbi__get8(s);
            if(info.hsz != 12) stbi__get8(s);
            pal[i][3] = 255;
        }
        stbi__skip(s, info.offset - info.extra_read - info.hsz - psize * (info.hsz == 12 ? 3 : 4));
        if(info.bpp == 1) width = (s->img_x + 7) >> 3;
        else if(info.bpp == 4) width = (s->img_x + 1) >> 1;
        else if(info.bpp == 8) width = s->img_x;
        else {
            STBI_FREE(out);
            return stbi__errpuc("bad bpp", "Corrupt BMP");
        }
        pad = (-width) & 3;
        if(info.bpp == 1) {
            for(j = 0; j < (int) s->img_y; ++j) {
                int bit_offset = 7, v = stbi__get8(s);
                for(i = 0; i < (int) s->img_x; ++i) {
                    int color = (v >> bit_offset) & 0x1;
                    out[z++] = pal[color][0];
                    out[z++] = pal[color][1];
                    out[z++] = pal[color][2];
                    if(target == 4) out[z++] = 255;
                    if(i + 1 == (int) s->img_x) break;
                    if((--bit_offset) < 0) {
                        bit_offset = 7;
                        v = stbi__get8(s);
                    }
                }
                stbi__skip(s, pad);
            }
        }
        else {
            for(j = 0; j < (int) s->img_y; ++j) {
                for(i = 0; i < (int) s->img_x; i += 2) {
                    int v = stbi__get8(s), v2 = 0;
                    if(info.bpp == 4) {
                        v2 = v & 15;
                        v >>= 4;
                    }
                    out[z++] = pal[v][0];
                    out[z++] = pal[v][1];
                    out[z++] = pal[v][2];
                    if(target == 4) out[z++] = 255;
                    if(i + 1 == (int) s->img_x) break;
                    v = (info.bpp == 8) ? stbi__get8(s) : v2;
                    out[z++] = pal[v][0];
                    out[z++] = pal[v][1];
                    out[z++] = pal[v][2];
                    if(target == 4) out[z++] = 255;
                }
                stbi__skip(s, pad);
            }
        }
    }
    else {
        int rshift = 0, gshift = 0, bshift = 0, ashift = 0, rcount = 0, gcount = 0, bcount = 0, acount = 0;
        int z = 0;
        int easy = 0;
        stbi__skip(s, info.offset - info.extra_read - info.hsz);
        if(info.bpp == 24) width = 3 * s->img_x;
        else if(info.bpp == 16) width = 2 * s->img_x;
        else /* bpp = 32 and pad = 0 */ width = 0;
        pad = (-width) & 3;
        if(info.bpp == 24) {
            easy = 1;
        }
        else if(info.bpp == 32) {
            if(mb == 0xff && mg == 0xff00 && mr == 0x00ff0000 && ma == 0xff000000)
                easy = 2;
        }
        if(!easy) {
            if(!mr || !mg || !mb) {
                STBI_FREE(out);
                return stbi__errpuc("bad masks", "Corrupt BMP");
            }
            // сдвиг вправо, чтобы поставить старший бит в позицию №7
            rshift = stbi__high_bit(mr) - 7;
            rcount = stbi__bitcount(mr);
            gshift = stbi__high_bit(mg) - 7;
            gcount = stbi__bitcount(mg);
            bshift = stbi__high_bit(mb) - 7;
            bcount = stbi__bitcount(mb);
            ashift = stbi__high_bit(ma) - 7;
            acount = stbi__bitcount(ma);
            if(rcount > 8 || gcount > 8 || bcount > 8 || acount > 8) {
                STBI_FREE(out);
                return stbi__errpuc("bad masks", "Corrupt BMP");
            }
        }
        for(j = 0; j < (int) s->img_y; ++j) {
            if(easy) {
                for(i = 0; i < (int) s->img_x; ++i) {
                    unsigned char a;
                    out[z + 2] = stbi__get8(s);
                    out[z + 1] = stbi__get8(s);
                    out[z + 0] = stbi__get8(s);
                    z += 3;
                    a = (easy == 2 ? stbi__get8(s) : 255);
                    all_a |= a;
                    if(target == 4) out[z++] = a;
                }
            }
            else {
                int bpp = info.bpp;
                for(i = 0; i < (int) s->img_x; ++i) {
                    stbi__uint32 v = (bpp == 16 ? (stbi__uint32) stbi__get16le(s) : stbi__get32le(s));
                    unsigned int a;
                    out[z++] = STBI__BYTECAST(stbi__shiftsigned(v & mr, rshift, rcount));
                    out[z++] = STBI__BYTECAST(stbi__shiftsigned(v & mg, gshift, gcount));
                    out[z++] = STBI__BYTECAST(stbi__shiftsigned(v & mb, bshift, bcount));
                    a = (ma ? stbi__shiftsigned(v & ma, ashift, acount) : 255);
                    all_a |= a;
                    if(target == 4) out[z++] = STBI__BYTECAST(a);
                }
            }
            stbi__skip(s, pad);
        }
    }

    // если в альфа-канале все 0, замените на все 255.
    if(target == 4 && all_a == 0)
        for(i = 4 * s->img_x * s->img_y - 1; i >= 0; i -= 4)
            out[i] = 255;

    if(flip_vertically) {
        stbi_uc t;
        for(j = 0; j < (int) s->img_y >> 1; ++j) {
            stbi_uc * p1 = out +      j     * s->img_x * target;
            stbi_uc * p2 = out + (s->img_y - 1 - j) * s->img_x * target;
            for(i = 0; i < (int) s->img_x * target; ++i) {
                t = p1[i];
                p1[i] = p2[i];
                p2[i] = t;
            }
        }
    }

    if(req_comp && req_comp != target) {
        out = stbi__convert_format(out, target, req_comp, s->img_x, s->img_y);
        if(out == NULL) return out;  // stbi__convert_format освобождает вход в случае сбоя
    }

    *x = s->img_x;
    *y = s->img_y;
    if(comp) *comp = s->img_n;
    return out;
}
#endif

// Тарга Truevision — TGA
// Джонатан Даммер
#ifndef STBI_NO_TGA
// возвращает STBI_rgb или что-то еще, 0 в случае ошибки
static int stbi__tga_get_comp(int bits_per_pixel, int is_grey, int * is_rgb16)
{
    // разрешены только RGB или RGBA (включая 16 бит) или серый цвет
    if(is_rgb16) *is_rgb16 = 0;
    switch(bits_per_pixel) {
        case 8:
            return STBI_grey;
        case 16:
            if(is_grey) return STBI_grey_alpha;
        // провал
        case 15:
            if(is_rgb16) *is_rgb16 = 1;
            return STBI_rgb;
        case 24: // провал
        case 32:
            return bits_per_pixel / 8;
        default:
            return 0;
    }
}

static int stbi__tga_info(stbi__context * s, int * x, int * y, int * comp)
{
    int tga_w, tga_h, tga_comp, tga_image_type, tga_bits_per_pixel, tga_colormap_bpp;
    int sz, tga_colormap_type;
    stbi__get8(s);                   // отказаться от смещения
    tga_colormap_type = stbi__get8(s); // тип цветовой карты
    if(tga_colormap_type > 1) {
        stbi__rewind(s);
        return 0;      // разрешены только RGB или индексированные
    }
    tga_image_type = stbi__get8(s); // тип изображения
    if(tga_colormap_type == 1) {    // изображение с цветовой картой (палитрой)
        if(tga_image_type != 1 && tga_image_type != 9) {
            stbi__rewind(s);
            return 0;
        }
        stbi__skip(s, 4);      // пропустить индекс первой записи цветовой карты и количество записей
        sz = stbi__get8(s);    //   проверить биты для каждой записи цвета палитры
        if((sz != 8) && (sz != 15) && (sz != 16) && (sz != 24) && (sz != 32)) {
            stbi__rewind(s);
            return 0;
        }
        stbi__skip(s, 4);      // пропустить начало изображения x и y
        tga_colormap_bpp = sz;
    }
    else {   // «нормальное» изображение без цветовой карты — разрешен только RGB или серый, +/- RLE
        if((tga_image_type != 2) && (tga_image_type != 3) && (tga_image_type != 10) && (tga_image_type != 11)) {
            stbi__rewind(s);
            return 0; // разрешен только RGB или серый, +/- RLE
        }
        stbi__skip(s, 9); // пропустить спецификацию цветовой карты и начало координат изображения по x/y
        tga_colormap_bpp = 0;
    }
    tga_w = stbi__get16le(s);
    if(tga_w < 1) {
        stbi__rewind(s);
        return 0;   // ширина теста
    }
    tga_h = stbi__get16le(s);
    if(tga_h < 1) {
        stbi__rewind(s);
        return 0;   // высота испытания
    }
    tga_bits_per_pixel = stbi__get8(s); // бит на пиксель
    stbi__get8(s); // игнорировать альфа-биты
    if(tga_colormap_bpp != 0) {
        if((tga_bits_per_pixel != 8) && (tga_bits_per_pixel != 16)) {
            // при использовании карты цветов tga_bits_per_pixel — это размер индексов
            // Я не думаю, что что-либо, кроме 8- или 16-битных индексов, имеет смысл.
            stbi__rewind(s);
            return 0;
        }
        tga_comp = stbi__tga_get_comp(tga_colormap_bpp, 0, NULL);
    }
    else {
        tga_comp = stbi__tga_get_comp(tga_bits_per_pixel, (tga_image_type == 3) || (tga_image_type == 11), NULL);
    }
    if(!tga_comp) {
        stbi__rewind(s);
        return 0;
    }
    if(x) *x = tga_w;
    if(y) *y = tga_h;
    if(comp) *comp = tga_comp;
    return 1;                   // вроде все прошел
}

static int stbi__tga_test(stbi__context * s)
{
    int res = 0;
    int sz, tga_color_type;
    stbi__get8(s);      //   отказаться от смещения
    tga_color_type = stbi__get8(s);   //   цветотип
    if(tga_color_type > 1) goto errorEnd;      //   разрешены только RGB или индексированные
    sz = stbi__get8(s);   //   тип изображения
    if(tga_color_type == 1) {    // изображение с цветовой картой (палитрой)
        if(sz != 1 && sz != 9) goto errorEnd;  // цветотип 1 требует типа изображения 1 или 9
        stbi__skip(s, 4);      // пропустить индекс первой записи цветовой карты и количество записей
        sz = stbi__get8(s);    //   проверить биты для каждой записи цвета палитры
        if((sz != 8) && (sz != 15) && (sz != 16) && (sz != 24) && (sz != 32)) goto errorEnd;
        stbi__skip(s, 4);      // пропустить начало изображения x и y
    }
    else {   // «нормальное» изображение без цветовой карты
        if((sz != 2) && (sz != 3) && (sz != 10) && (sz != 11)) goto errorEnd;    // разрешен только RGB или серый, +/- RLE
        stbi__skip(s, 9); // пропустить спецификацию цветовой карты и начало координат изображения по x/y
    }
    if(stbi__get16le(s) < 1) goto errorEnd;         //   ширина теста
    if(stbi__get16le(s) < 1) goto errorEnd;         //   высота испытания
    sz = stbi__get8(s);   //   бит на пиксель
    if((tga_color_type == 1) && (sz != 8) &&
       (sz != 16)) goto errorEnd;    // для изображений с цветовой картой bpp — это размер индекса
    if((sz != 8) && (sz != 15) && (sz != 16) && (sz != 24) && (sz != 32)) goto errorEnd;

    res = 1; // если мы зашли так далеко, все хорошо и мы можем вернуть 1 вместо 0

errorEnd:
    stbi__rewind(s);
    return res;
}

// прочитать 16-битное значение и преобразовать в 24-битное RGB
static void stbi__tga_read_rgb16(stbi__context * s, stbi_uc * out)
{
    stbi__uint16 px = (stbi__uint16)stbi__get16le(s);
    stbi__uint16 fiveBitMask = 31;
    // у нас есть 3 канала по 5 бит каждый
    int r = (px >> 10) & fiveBitMask;
    int g = (px >> 5) & fiveBitMask;
    int b = px & fiveBitMask;
    // Обратите внимание, что при этом данные сохраняются в порядке RGB (A), поэтому их не нужно менять позже.
    out[0] = (stbi_uc)((r * 255) / 31);
    out[1] = (stbi_uc)((g * 255) / 31);
    out[2] = (stbi_uc)((b * 255) / 31);

    // некоторые люди утверждают, что самый старший бит может использоваться для альфа
    // (возможно, если в «байте дескриптора изображения» установлен альфа-бит)
    // но это сделало полностью прозрачными только 16-битные тестовые изображения.
    // поэтому давайте рассматривать все 15- и 16-битные TGA как RGB без альфы.
}

static void * stbi__tga_load(stbi__context * s, int * x, int * y, int * comp, int req_comp, stbi__result_info * ri)
{
    //   почитайте в шапке TGA материал
    int tga_offset = stbi__get8(s);
    int tga_indexed = stbi__get8(s);
    int tga_image_type = stbi__get8(s);
    int tga_is_RLE = 0;
    int tga_palette_start = stbi__get16le(s);
    int tga_palette_len = stbi__get16le(s);
    int tga_palette_bits = stbi__get8(s);
    int tga_x_origin = stbi__get16le(s);
    int tga_y_origin = stbi__get16le(s);
    int tga_width = stbi__get16le(s);
    int tga_height = stbi__get16le(s);
    int tga_bits_per_pixel = stbi__get8(s);
    int tga_comp, tga_rgb16 = 0;
    int tga_inverted = stbi__get8(s);
    // int tga_alpha_bits = tga_inverted & 15; // the 4 lowest bits - unused (useless?)
    //   данные изображения
    unsigned char * tga_data;
    unsigned char * tga_palette = NULL;
    int i, j;
    unsigned char raw_data[4] = {0};
    int RLE_count = 0;
    int RLE_repeating = 0;
    int read_next_pixel = 1;
    STBI_NOTUSED(ri);
    STBI_NOTUSED(tga_x_origin); // @TODO
    STBI_NOTUSED(tga_y_origin); // @TODO

    if(tga_height > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large", "Very large image (corrupt?)");
    if(tga_width > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large", "Very large image (corrupt?)");

    //   сделать небольшую прецессию
    if(tga_image_type >= 8) {
        tga_image_type -= 8;
        tga_is_RLE = 1;
    }
    tga_inverted = 1 - ((tga_inverted >> 5) & 1);

    //   Если у меня палитра, то я буду использовать количество бит из палитры
    if(tga_indexed) tga_comp = stbi__tga_get_comp(tga_palette_bits, 0, &tga_rgb16);
    else tga_comp = stbi__tga_get_comp(tga_bits_per_pixel, (tga_image_type == 3), &tga_rgb16);

    if(!tga_comp) // на самом деле не должно было случиться, stbi__tga_test() должен был обеспечить базовую согласованность
        return stbi__errpuc("bad format", "Can't find out TGA pixelformat");

    //   информация о ТГА
    *x = tga_width;
    *y = tga_height;
    if(comp) *comp = tga_comp;

    if(!stbi__mad3sizes_valid(tga_width, tga_height, tga_comp, 0))
        return stbi__errpuc("too large", "Corrupt TGA");

    tga_data = (unsigned char *)stbi__malloc_mad3(tga_width, tga_height, tga_comp, 0);
    if(!tga_data) return stbi__errpuc("outofmem", "Out of memory");

    // перейти к начальной позиции данных (обычно смещение = 0)
    stbi__skip(s, tga_offset);

    if(!tga_indexed && !tga_is_RLE && !tga_rgb16) {
        for(i = 0; i < tga_height; ++i) {
            int row = tga_inverted ? tga_height - i - 1 : i;
            stbi_uc * tga_row = tga_data + row * tga_width * tga_comp;
            stbi__getn(s, tga_row, tga_width * tga_comp);
        }
    }
    else  {
        //   do I need to load a palette?
        if(tga_indexed) {
            if(tga_palette_len == 0) {   /* у вас должна быть хотя бы одна запись! */
                STBI_FREE(tga_data);
                return stbi__errpuc("bad palette", "Corrupt TGA");
            }

            //   any data to skip? (offset usually = 0)
            stbi__skip(s, tga_palette_start);
            //   загрузить палитру
            tga_palette = (unsigned char *)stbi__malloc_mad2(tga_palette_len, tga_comp, 0);
            if(!tga_palette) {
                STBI_FREE(tga_data);
                return stbi__errpuc("outofmem", "Out of memory");
            }
            if(tga_rgb16) {
                stbi_uc * pal_entry = tga_palette;
                STBI_ASSERT(tga_comp == STBI_rgb);
                for(i = 0; i < tga_palette_len; ++i) {
                    stbi__tga_read_rgb16(s, pal_entry);
                    pal_entry += tga_comp;
                }
            }
            else if(!stbi__getn(s, tga_palette, tga_palette_len * tga_comp)) {
                STBI_FREE(tga_data);
                STBI_FREE(tga_palette);
                return stbi__errpuc("bad palette", "Corrupt TGA");
            }
        }
        //   загрузить данные
        for(i = 0; i < tga_width * tga_height; ++i) {
            //   if I'm in RLE mode, do I need to get a RLE stbi__pngchunk?
            if(tga_is_RLE) {
                if(RLE_count == 0) {
                    //   да, получить следующий байт как команду RLE
                    int RLE_cmd = stbi__get8(s);
                    RLE_count = 1 + (RLE_cmd & 127);
                    RLE_repeating = RLE_cmd >> 7;
                    read_next_pixel = 1;
                }
                else if(!RLE_repeating) {
                    read_next_pixel = 1;
                }
            }
            else {
                read_next_pixel = 1;
            }
            //   OK, если мне нужно прочитать пиксель, сделаю это сейчас
            if(read_next_pixel) {
                //   загрузить, сколько бы данных у нас ни было
                if(tga_indexed) {
                    // прочитать в индексе, затем выполнить поиск
                    int pal_idx = (tga_bits_per_pixel == 8) ? stbi__get8(s) : stbi__get16le(s);
                    if(pal_idx >= tga_palette_len) {
                        // неверный индекс
                        pal_idx = 0;
                    }
                    pal_idx *= tga_comp;
                    for(j = 0; j < tga_comp; ++j) {
                        raw_data[j] = tga_palette[pal_idx + j];
                    }
                }
                else if(tga_rgb16) {
                    STBI_ASSERT(tga_comp == STBI_rgb);
                    stbi__tga_read_rgb16(s, raw_data);
                }
                else {
                    //   прочитать в необработанных данных
                    for(j = 0; j < tga_comp; ++j) {
                        raw_data[j] = stbi__get8(s);
                    }
                }
                //   очистить флаг чтения для следующего пикселя
                read_next_pixel = 0;
            } // конец чтения пикселя

            // копировать данные
            for(j = 0; j < tga_comp; ++j)
                tga_data[i * tga_comp + j] = raw_data[j];

            //   если мы находимся в режиме RLE, продолжайте обратный отсчет
            --RLE_count;
        }
        //   do I need to invert the image?
        if(tga_inverted) {
            for(j = 0; j * 2 < tga_height; ++j) {
                int index1 = j * tga_width * tga_comp;
                int index2 = (tga_height - 1 - j) * tga_width * tga_comp;
                for(i = tga_width * tga_comp; i > 0; --i) {
                    unsigned char temp = tga_data[index1];
                    tga_data[index1] = tga_data[index2];
                    tga_data[index2] = temp;
                    ++index1;
                    ++index2;
                }
            }
        }
        //   очисти мою палитру, если бы она у меня была
        if(tga_palette != NULL) {
            STBI_FREE(tga_palette);
        }
    }

    // swap RGB — если исходные данные были RGB16, они уже в правильном порядке
    if(tga_comp >= 3 && !tga_rgb16) {
        unsigned char * tga_pixel = tga_data;
        for(i = 0; i < tga_width * tga_height; ++i) {
            unsigned char temp = tga_pixel[0];
            tga_pixel[0] = tga_pixel[2];
            tga_pixel[2] = temp;
            tga_pixel += tga_comp;
        }
    }

    // преобразовать в целевое количество компонентов
    if(req_comp && req_comp != tga_comp)
        tga_data = stbi__convert_format(tga_data, tga_comp, req_comp, tga_width, tga_height);

    //   что я делаю, чтобы избавиться от сообщения об ошибке и при этом сохранить
    //   Компиляторы C от Microsoft довольны... [8^(
    tga_palette_start = tga_palette_len = tga_palette_bits =
    tga_x_origin = tga_y_origin = 0;
    STBI_NOTUSED(tga_palette_start);
    //   OK, готово
    return tga_data;
}
#endif

// *************************************************************************************************
// Загрузчик Photoshop PSD — PD Тэтчер Ульрих, интеграция Николаса Шульца, настройка STB

#ifndef STBI_NO_PSD
static int stbi__psd_test(stbi__context * s)
{
    int r = (stbi__get32be(s) == 0x38425053);
    stbi__rewind(s);
    return r;
}

static int stbi__psd_decode_rle(stbi__context * s, stbi_uc * p, int pixelCount)
{
    int count, nleft, len;

    count = 0;
    while((nleft = pixelCount - count) > 0) {
        len = stbi__get8(s);
        if(len == 128) {
            // No-op.
        }
        else if(len < 128) {
            // Скопируйте следующие len+1 байт буквально.
            len++;
            if(len > nleft) return 0;  // поврежденные данные
            count += len;
            while(len) {
                *p = stbi__get8(s);
                p += 4;
                len--;
            }
        }
        else if(len > 128) {
            stbi_uc   val;
            // Следующие -len+1 байты в dest реплицируются из следующего исходного байта.
            // (Интерпретируйте len как отрицательное 8-битное целое число.)
            len = 257 - len;
            if(len > nleft) return 0;  // поврежденные данные
            val = stbi__get8(s);
            count += len;
            while(len) {
                *p = val;
                p += 4;
                len--;
            }
        }
    }

    return 1;
}

static void * stbi__psd_load(stbi__context * s, int * x, int * y, int * comp, int req_comp, stbi__result_info * ri,
                             int bpc)
{
    int pixelCount;
    int channelCount, compression;
    int channel, i;
    int bitdepth;
    int w, h;
    stbi_uc * out;
    STBI_NOTUSED(ri);

    // Проверить идентификатор
    if(stbi__get32be(s) != 0x38425053)    // "8BPS"
        return stbi__errpuc("not PSD", "Corrupt PSD image");

    // Проверьте версию типа файла.
    if(stbi__get16be(s) != 1)
        return stbi__errpuc("wrong version", "Unsupported version of PSD image");

    // Пропустить 6 зарезервированных байтов.
    stbi__skip(s, 6);

    // Прочтите количество каналов (R, G, B, A и т. д.).
    channelCount = stbi__get16be(s);
    if(channelCount < 0 || channelCount > 16)
        return stbi__errpuc("wrong channel count", "Unsupported number of channels in PSD image");

    // Прочитайте строки и столбцы изображения.
    h = stbi__get32be(s);
    w = stbi__get32be(s);

    if(h > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large", "Very large image (corrupt?)");
    if(w > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large", "Very large image (corrupt?)");

    // Убедитесь, что глубина составляет 8 бит.
    bitdepth = stbi__get16be(s);
    if(bitdepth != 8 && bitdepth != 16)
        return stbi__errpuc("unsupported bit depth", "PSD bit depth is not 8 or 16 bit");

    // Убедитесь, что цветовой режим — RGB.
    // Допустимые варианты:
    //   0: Bitmap
    //   1: Grayscale
    //   2: Indexed color
    //   3: RGB color
    //   4: CMYK color
    //   7: Multichannel
    //   8: Duotone
    //   9: Lab color
    if(stbi__get16be(s) != 3)
        return stbi__errpuc("wrong color format", "PSD is not in RGB color format");

    // Пропустите данные режима.  (Это палитра индексированных цветов; другая информация для других режимов.)
    stbi__skip(s, stbi__get32be(s));

    // Пропустите ресурсы изображений.  (разрешение, траектории инструментов пера и т. д.)
    stbi__skip(s, stbi__get32be(s));

    // Пропустить зарезервированные данные.
    stbi__skip(s, stbi__get32be(s));

    // Узнайте, сжаты ли данные.
    // Известные значения:
    //   0: no compression
    //   1: RLE compressed
    compression = stbi__get16be(s);
    if(compression > 1)
        return stbi__errpuc("bad compression", "PSD has an unknown compression format");

    // Проверьте размер
    if(!stbi__mad3sizes_valid(4, w, h, 0))
        return stbi__errpuc("too large", "Corrupt PSD");

    // Создайте изображение назначения.

    if(!compression && bitdepth == 16 && bpc == 16) {
        out = (stbi_uc *) stbi__malloc_mad3(8, w, h, 0);
        ri->bits_per_channel = 16;
    }
    else
        out = (stbi_uc *) stbi__malloc(4 * w * h);

    if(!out) return stbi__errpuc("outofmem", "Out of memory");
    pixelCount = w * h;

    // Инициализируйте данные до нуля.
    //memset(out, 0, PixelCount * 4);

    // Наконец, данные изображения.
    if(compression) {
        // RLE, используемый . PSD и . TIFF
        // Выполняйте цикл, пока не получите ожидаемое количество распакованных байтов:
        //     Считайте следующий исходный байт в n.
        //     Если n находится в диапазоне от 0 до 127 включительно, скопируйте следующие n+1 байт буквально.
        //     В противном случае, если n находится в диапазоне от -127 до -1 включительно, скопируйте следующий байт -n+1 раз.
        //     В противном случае, если n равно 128, нет.
        // Концевой цикл

        // Данным, сжатым RLE, предшествует 2-байтовый счетчик данных для каждой строки данных.
        // который мы собираемся просто пропустить.
        stbi__skip(s, h * channelCount * 2);

        // Считайте данные RLE по каналам.
        for(channel = 0; channel < 4; channel++) {
            stbi_uc * p;

            p = out + channel;
            if(channel >= channelCount) {
                // Заполните этот канал данными по умолчанию.
                for(i = 0; i < pixelCount; i++, p += 4)
                    * p = (channel == 3 ? 255 : 0);
            }
            else {
                // Прочтите данные RLE.
                if(!stbi__psd_decode_rle(s, p, pixelCount)) {
                    STBI_FREE(out);
                    return stbi__errpuc("corrupt", "bad RLE data");
                }
            }
        }

    }
    else {
        // Мы находимся у необработанных данных изображения.  Это каждый канал по порядку (красный, зеленый, синий, альфа, ...).
        // где каждый канал состоит из 8-битного (или 16-битного) значения для каждого пикселя изображения.

        // Считайте данные по каналам.
        for(channel = 0; channel < 4; channel++) {
            if(channel >= channelCount) {
                // Заполните этот канал данными по умолчанию.
                if(bitdepth == 16 && bpc == 16) {
                    stbi__uint16 * q = ((stbi__uint16 *) out) + channel;
                    stbi__uint16 val = channel == 3 ? 65535 : 0;
                    for(i = 0; i < pixelCount; i++, q += 4)
                        * q = val;
                }
                else {
                    stbi_uc * p = out + channel;
                    stbi_uc val = channel == 3 ? 255 : 0;
                    for(i = 0; i < pixelCount; i++, p += 4)
                        * p = val;
                }
            }
            else {
                if(ri->bits_per_channel == 16) {     // выходной бит на канал
                    stbi__uint16 * q = ((stbi__uint16 *) out) + channel;
                    for(i = 0; i < pixelCount; i++, q += 4)
                        * q = (stbi__uint16) stbi__get16be(s);
                }
                else {
                    stbi_uc * p = out + channel;
                    if(bitdepth == 16) {   // входной бит/код
                        for(i = 0; i < pixelCount; i++, p += 4)
                            * p = (stbi_uc)(stbi__get16be(s) >> 8);
                    }
                    else {
                        for(i = 0; i < pixelCount; i++, p += 4)
                            * p = stbi__get8(s);
                    }
                }
            }
        }
    }

    // удалить странную белую матовую поверхность из PSD
    if(channelCount >= 4) {
        if(ri->bits_per_channel == 16) {
            for(i = 0; i < w * h; ++i) {
                stbi__uint16 * pixel = (stbi__uint16 *) out + 4 * i;
                if(pixel[3] != 0 && pixel[3] != 65535) {
                    float a = pixel[3] / 65535.0f;
                    float ra = 1.0f / a;
                    float inv_a = 65535.0f * (1 - ra);
                    pixel[0] = (stbi__uint16)(pixel[0] * ra + inv_a);
                    pixel[1] = (stbi__uint16)(pixel[1] * ra + inv_a);
                    pixel[2] = (stbi__uint16)(pixel[2] * ra + inv_a);
                }
            }
        }
        else {
            for(i = 0; i < w * h; ++i) {
                unsigned char * pixel = out + 4 * i;
                if(pixel[3] != 0 && pixel[3] != 255) {
                    float a = pixel[3] / 255.0f;
                    float ra = 1.0f / a;
                    float inv_a = 255.0f * (1 - ra);
                    pixel[0] = (unsigned char)(pixel[0] * ra + inv_a);
                    pixel[1] = (unsigned char)(pixel[1] * ra + inv_a);
                    pixel[2] = (unsigned char)(pixel[2] * ra + inv_a);
                }
            }
        }
    }

    // конвертировать в желаемый выходной формат
    if(req_comp && req_comp != 4) {
        if(ri->bits_per_channel == 16)
            out = (stbi_uc *) stbi__convert_format16((stbi__uint16 *) out, 4, req_comp, w, h);
        else
            out = stbi__convert_format(out, 4, req_comp, w, h);
        if(out == NULL) return out;  // stbi__convert_format освобождает вход в случае сбоя
    }

    if(comp) *comp = 4;
    *y = h;
    *x = w;

    return out;
}
#endif

// *************************************************************************************************
// Загрузчик Softimage PIC
// Том Седдон
//
// См. http://softimage.wiki.softimage.com/index.php/INFO:_PIC_file_format.
// См. http://ozviz.wasp.uwa.edu.au/~pbourke/dataformats/softimagepic/.

#ifndef STBI_NO_PIC
static int stbi__pic_is4(stbi__context * s, const char * str)
{
    int i;
    for(i = 0; i < 4; ++i)
        if(stbi__get8(s) != (stbi_uc)str[i])
            return 0;

    return 1;
}

static int stbi__pic_test_core(stbi__context * s)
{
    int i;

    if(!stbi__pic_is4(s, "\x53\x80\xF6\x34"))
        return 0;

    for(i = 0; i < 84; ++i)
        stbi__get8(s);

    if(!stbi__pic_is4(s, "PICT"))
        return 0;

    return 1;
}

typedef struct {
    stbi_uc size, type, channel;
} stbi__pic_packet;

static stbi_uc * stbi__readval(stbi__context * s, int channel, stbi_uc * dest)
{
    int mask = 0x80, i;

    for(i = 0; i < 4; ++i, mask >>= 1) {
        if(channel & mask) {
            if(stbi__at_eof(s)) return stbi__errpuc("bad file", "PIC file too short");
            dest[i] = stbi__get8(s);
        }
    }

    return dest;
}

static void stbi__copyval(int channel, stbi_uc * dest, const stbi_uc * src)
{
    int mask = 0x80, i;

    for(i = 0; i < 4; ++i, mask >>= 1)
        if(channel & mask)
            dest[i] = src[i];
}

static stbi_uc * stbi__pic_load_core(stbi__context * s, int width, int height, int * comp, stbi_uc * result)
{
    int act_comp = 0, num_packets = 0, y, chained;
    stbi__pic_packet packets[10];

    // это будет (должно...) обслуживать даже такие странные вещи, как наличие данных
    // для одного и того же канала в нескольких пакетах.
    do {
        stbi__pic_packet * packet;

        if(num_packets == sizeof(packets) / sizeof(packets[0]))
            return stbi__errpuc("bad format", "too many packets");

        packet = &packets[num_packets++];

        chained = stbi__get8(s);
        packet->size    = stbi__get8(s);
        packet->type    = stbi__get8(s);
        packet->channel = stbi__get8(s);

        act_comp |= packet->channel;

        if(stbi__at_eof(s))          return stbi__errpuc("bad file", "file too short (reading packets)");
        if(packet->size != 8)  return stbi__errpuc("bad format", "packet isn't 8bpp");
    } while(chained);

    *comp = (act_comp & 0x10 ? 4 : 3); // has alpha channel?

    for(y = 0; y < height; ++y) {
        int packet_idx;

        for(packet_idx = 0; packet_idx < num_packets; ++packet_idx) {
            stbi__pic_packet * packet = &packets[packet_idx];
            stbi_uc * dest = result + y * width * 4;

            switch(packet->type) {
                default:
                    return stbi__errpuc("bad format", "packet has bad compression type");

                case 0: {//несжатый
                        int x;

                        for(x = 0; x < width; ++x, dest += 4)
                            if(!stbi__readval(s, packet->channel, dest))
                                return 0;
                        break;
                    }

                case 1: { //Чистый RLE
                        int left = width, i;

                        while(left > 0) {
                            stbi_uc count, value[4];

                            count = stbi__get8(s);
                            if(stbi__at_eof(s))   return stbi__errpuc("bad file", "file too short (pure read count)");

                            if(count > left)
                                count = (stbi_uc) left;

                            if(!stbi__readval(s, packet->channel, value))  return 0;

                            for(i = 0; i < count; ++i, dest += 4)
                                stbi__copyval(packet->channel, dest, value);
                            left -= count;
                        }
                    }
                    break;

                case 2: {//Смешанный RLE
                        int left = width;
                        while(left > 0) {
                            int count = stbi__get8(s), i;
                            if(stbi__at_eof(s))  return stbi__errpuc("bad file", "file too short (mixed read count)");

                            if(count >= 128) {  // Повторный
                                stbi_uc value[4];

                                if(count == 128)
                                    count = stbi__get16be(s);
                                else
                                    count -= 127;
                                if(count > left)
                                    return stbi__errpuc("bad file", "scanline overrun");

                                if(!stbi__readval(s, packet->channel, value))
                                    return 0;

                                for(i = 0; i < count; ++i, dest += 4)
                                    stbi__copyval(packet->channel, dest, value);
                            }
                            else {   // сырой
                                ++count;
                                if(count > left) return stbi__errpuc("bad file", "scanline overrun");

                                for(i = 0; i < count; ++i, dest += 4)
                                    if(!stbi__readval(s, packet->channel, dest))
                                        return 0;
                            }
                            left -= count;
                        }
                        break;
                    }
            }
        }
    }

    return result;
}

static void * stbi__pic_load(stbi__context * s, int * px, int * py, int * comp, int req_comp, stbi__result_info * ri)
{
    stbi_uc * result;
    int i, x, y, internal_comp;
    STBI_NOTUSED(ri);

    if(!comp) comp = &internal_comp;

    for(i = 0; i < 92; ++i)
        stbi__get8(s);

    x = stbi__get16be(s);
    y = stbi__get16be(s);

    if(y > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large", "Very large image (corrupt?)");
    if(x > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large", "Very large image (corrupt?)");

    if(stbi__at_eof(s))  return stbi__errpuc("bad file", "file too short (pic header)");
    if(!stbi__mad3sizes_valid(x, y, 4, 0)) return stbi__errpuc("too large", "PIC image too large to decode");

    stbi__get32be(s); //пропустить «соотношение»
    stbi__get16be(s); //пропустить `поля'
    stbi__get16be(s); //пропустить `pad'

    // промежуточный буфер — RGBA
    result = (stbi_uc *) stbi__malloc_mad3(x, y, 4, 0);
    if(!result) return stbi__errpuc("outofmem", "Out of memory");
    memset(result, 0xff, x * y * 4);

    if(!stbi__pic_load_core(s, x, y, comp, result)) {
        STBI_FREE(result);
        result = 0;
    }
    *px = x;
    *py = y;
    if(req_comp == 0) req_comp = *comp;
    result = stbi__convert_format(result, 4, req_comp, x, y);

    return result;
}

static int stbi__pic_test(stbi__context * s)
{
    int r = stbi__pic_test_core(s);
    stbi__rewind(s);
    return r;
}
#endif

// *************************************************************************************************
// Загрузчик GIF – общественное достояние Жана-Марка Лиенера – упрощено/уменьшено stb

#ifndef STBI_NO_GIF
typedef struct {
    stbi__int16 prefix;
    stbi_uc first;
    stbi_uc suffix;
} stbi__gif_lzw;

typedef struct {
    int w, h;
    stbi_uc * out;                // выходной буфер (всегда 4 компонента)
    stbi_uc * background;         // Текущий «фон» в отношении gif.
    stbi_uc * history;
    int flags, bgindex, ratio, transparent, eflags;
    stbi_uc  pal[256][4];
    stbi_uc lpal[256][4];
    stbi__gif_lzw codes[8192];
    stbi_uc * color_table;
    int parse, step;
    int lflags;
    int start_x, start_y;
    int max_x, max_y;
    int cur_x, cur_y;
    int line_size;
    int delay;
} stbi__gif;

static int stbi__gif_test_raw(stbi__context * s)
{
    int sz;
    if(stbi__get8(s) != 'G' || stbi__get8(s) != 'I' || stbi__get8(s) != 'F' || stbi__get8(s) != '8') return 0;
    sz = stbi__get8(s);
    if(sz != '9' && sz != '7') return 0;
    if(stbi__get8(s) != 'a') return 0;
    return 1;
}

static int stbi__gif_test(stbi__context * s)
{
    int r = stbi__gif_test_raw(s);
    stbi__rewind(s);
    return r;
}

static void stbi__gif_parse_colortable(stbi__context * s, stbi_uc pal[256][4], int num_entries, int transp)
{
    int i;
    for(i = 0; i < num_entries; ++i) {
        pal[i][2] = stbi__get8(s);
        pal[i][1] = stbi__get8(s);
        pal[i][0] = stbi__get8(s);
        pal[i][3] = transp == i ? 0 : 255;
    }
}

static int stbi__gif_header(stbi__context * s, stbi__gif * g, int * comp, int is_info)
{
    stbi_uc version;
    if(stbi__get8(s) != 'G' || stbi__get8(s) != 'I' || stbi__get8(s) != 'F' || stbi__get8(s) != '8')
        return stbi__err("not GIF", "Corrupt GIF");

    version = stbi__get8(s);
    if(version != '7' && version != '9')    return stbi__err("not GIF", "Corrupt GIF");
    if(stbi__get8(s) != 'a')                return stbi__err("not GIF", "Corrupt GIF");

    stbi__g_failure_reason = "";
    g->w = stbi__get16le(s);
    g->h = stbi__get16le(s);
    g->flags = stbi__get8(s);
    g->bgindex = stbi__get8(s);
    g->ratio = stbi__get8(s);
    g->transparent = -1;

    if(g->w > STBI_MAX_DIMENSIONS) return stbi__err("too large", "Very large image (corrupt?)");
    if(g->h > STBI_MAX_DIMENSIONS) return stbi__err("too large", "Very large image (corrupt?)");

    if(comp != 0) *comp = 4;   // на самом деле не могу сказать, 3 или 4, пока не проанализируем комментарии

    if(is_info) return 1;

    if(g->flags & 0x80)
        stbi__gif_parse_colortable(s, g->pal, 2 << (g->flags & 7), -1);

    return 1;
}

static int stbi__gif_info_raw(stbi__context * s, int * x, int * y, int * comp)
{
    stbi__gif * g = (stbi__gif *) stbi__malloc(sizeof(stbi__gif));
    if(!g) return stbi__err("outofmem", "Out of memory");
    if(!stbi__gif_header(s, g, comp, 1)) {
        STBI_FREE(g);
        stbi__rewind(s);
        return 0;
    }
    if(x) *x = g->w;
    if(y) *y = g->h;
    STBI_FREE(g);
    return 1;
}

static void stbi__out_gif_code(stbi__gif * g, stbi__uint16 code)
{
    stbi_uc * p, * c;
    int idx;

    // рекурсия для декодирования префиксов, поскольку связанный список расположен в обратном направлении,
    // и работать в обратном направлении через чередующееся изображение было бы неприятно
    if(g->codes[code].prefix >= 0)
        stbi__out_gif_code(g, g->codes[code].prefix);

    if(g->cur_y >= g->max_y) return;

    idx = g->cur_x + g->cur_y;
    p = &g->out[idx];
    g->history[idx / 4] = 1;

    c = &g->color_table[g->codes[code].suffix * 4];
    if(c[3] > 128) {  // не отображать прозрачные пиксели;
        p[0] = c[2];
        p[1] = c[1];
        p[2] = c[0];
        p[3] = c[3];
    }
    g->cur_x += 4;

    if(g->cur_x >= g->max_x) {
        g->cur_x = g->start_x;
        g->cur_y += g->step;

        while(g->cur_y >= g->max_y && g->parse > 0) {
            g->step = (1 << g->parse) * g->line_size;
            g->cur_y = g->start_y + (g->step >> 1);
            --g->parse;
        }
    }
}

static stbi_uc * stbi__process_gif_raster(stbi__context * s, stbi__gif * g)
{
    stbi_uc lzw_cs;
    stbi__int32 len, init_code;
    stbi__uint32 first;
    stbi__int32 codesize, codemask, avail, oldcode, bits, valid_bits, clear;
    stbi__gif_lzw * p;

    lzw_cs = stbi__get8(s);
    if(lzw_cs > 12) return NULL;
    clear = 1 << lzw_cs;
    first = 1;
    codesize = lzw_cs + 1;
    codemask = (1 << codesize) - 1;
    bits = 0;
    valid_bits = 0;
    for(init_code = 0; init_code < clear; init_code++) {
        g->codes[init_code].prefix = -1;
        g->codes[init_code].first = (stbi_uc) init_code;
        g->codes[init_code].suffix = (stbi_uc) init_code;
    }

    // поддержка отсутствия запуска чистого кода
    avail = clear + 2;
    oldcode = -1;

    len = 0;
    for(;;) {
        if(valid_bits < codesize) {
            if(len == 0) {
                len = stbi__get8(s); // начать новый блок
                if(len == 0)
                    return g->out;
            }
            --len;
            bits |= (stbi__int32) stbi__get8(s) << valid_bits;
            valid_bits += 8;
        }
        else {
            stbi__int32 code = bits & codemask;
            bits >>= codesize;
            valid_bits -= codesize;
            // @OPTIMIZE: is there some way we can accelerate the non-clear path?
            if(code == clear) {   // очистить код
                codesize = lzw_cs + 1;
                codemask = (1 << codesize) - 1;
                avail = clear + 2;
                oldcode = -1;
                first = 0;
            }
            else if(code == clear + 1) {    // конец кода потока
                stbi__skip(s, len);
                while((len = stbi__get8(s)) > 0)
                    stbi__skip(s, len);
                return g->out;
            }
            else if(code <= avail) {
                if(first) {
                    return stbi__errpuc("no clear code", "Corrupt GIF");
                }

                if(oldcode >= 0) {
                    p = &g->codes[avail++];
                    if(avail > 8192) {
                        return stbi__errpuc("too many codes", "Corrupt GIF");
                    }

                    p->prefix = (stbi__int16) oldcode;
                    p->first = g->codes[oldcode].first;
                    p->suffix = (code == avail) ? p->first : g->codes[code].first;
                }
                else if(code == avail)
                    return stbi__errpuc("illegal code in raster", "Corrupt GIF");

                stbi__out_gif_code(g, (stbi__uint16) code);

                if((avail & codemask) == 0 && avail <= 0x0FFF) {
                    codesize++;
                    codemask = (1 << codesize) - 1;
                }

                oldcode = code;
            }
            else {
                return stbi__errpuc("illegal code in raster", "Corrupt GIF");
            }
        }
    }
}

// эта функция предназначена для поддержки анимированных изображений gif, хотя stb_image ее не поддерживает.
// два назад — это изображение двух кадров назад, используемое для очень специфического формата удаления.
static stbi_uc * stbi__gif_load_next(stbi__context * s, stbi__gif * g, int * comp, int req_comp, stbi_uc * two_back)
{
    int dispose;
    int first_frame;
    int pi;
    int pcount;
    STBI_NOTUSED(req_comp);

    // в первом кадре любые незаписанные пиксели получают цвет фона (непрозрачный)
    first_frame = 0;
    if(g->out == 0) {
        if(!stbi__gif_header(s, g, comp, 0)) return 0; // stbi__g_failure_reason установлен stbi__gif_header
        if(!stbi__mad3sizes_valid(4, g->w, g->h, 0))
            return stbi__errpuc("too large", "GIF image is too large");
        pcount = g->w * g->h;
        g->out = (stbi_uc *) stbi__malloc(4 * pcount);
        g->background = (stbi_uc *) stbi__malloc(4 * pcount);
        g->history = (stbi_uc *) stbi__malloc(pcount);
        if(!g->out || !g->background || !g->history)
            return stbi__errpuc("outofmem", "Out of memory");

        // изображение вначале рассматривается как «прозрачное», т.е. ничто не перезаписывает текущий фон;
        // цвет фона используется только для пикселей, которые не отображаются в первом кадре, после этого «фона».
        // цвет относится к цвету, который был в предыдущем кадре.
        memset(g->out, 0x00, 4 * pcount);
        memset(g->background, 0x00, 4 * pcount); // состояние фона (начинается с прозрачности)
        memset(g->history, 0x00, pcount);        // пиксели, на которые повлиял предыдущий кадр
        first_frame = 1;
    }
    else {
        // second frame - how do we dispose of the previous one?
        dispose = (g->eflags & 0x1C) >> 2;
        pcount = g->w * g->h;

        if((dispose == 3) && (two_back == 0)) {
            dispose = 2; // если у меня нет изображения, к которому можно вернуться, по умолчанию используется старый фон
        }

        if(dispose == 3) {  // использовать предыдущее изображение
            for(pi = 0; pi < pcount; ++pi) {
                if(g->history[pi]) {
                    memcpy(&g->out[pi * 4], &two_back[pi * 4], 4);
                }
            }
        }
        else if(dispose == 2) {
            // восстановить то, что было изменено в последнем кадре, на фон перед этим кадром;
            for(pi = 0; pi < pcount; ++pi) {
                if(g->history[pi]) {
                    memcpy(&g->out[pi * 4], &g->background[pi * 4], 4);
                }
            }
        }
        else {
            // В любом случае это случай, когда нельзя утилизировать, так что просто
            // оставьте пиксели как есть, и они станут новым фоном
            // 1: do not dispose
            // 0:  not specified.
        }

        // фон — это то, что осталось после отмены предыдущего кадра;
        memcpy(g->background, g->out, 4 * g->w * g->h);
    }

    // очистить мою историю;
    memset(g->history, 0x00, g->w * g->h);          // пиксели, на которые повлиял предыдущий кадр

    for(;;) {
        int tag = stbi__get8(s);
        switch(tag) {
            case 0x2C: { /* Дескриптор изображения */
                    stbi__int32 x, y, w, h;
                    stbi_uc * o;

                    x = stbi__get16le(s);
                    y = stbi__get16le(s);
                    w = stbi__get16le(s);
                    h = stbi__get16le(s);
                    if(((x + w) > (g->w)) || ((y + h) > (g->h)))
                        return stbi__errpuc("bad Image Descriptor", "Corrupt GIF");

                    g->line_size = g->w * 4;
                    g->start_x = x * 4;
                    g->start_y = y * g->line_size;
                    g->max_x   = g->start_x + w * 4;
                    g->max_y   = g->start_y + h * g->line_size;
                    g->cur_x   = g->start_x;
                    g->cur_y   = g->start_y;

                    // если ширина указанного прямоугольника равна 0, это означает
                    // мы можем не видеть *никаких* пикселей или изображение искажено;
                    // чтобы убедиться, что это уловлено, переместите текущий y вниз на
                    // max_y (что и проверяет out_gif_code).
                    if(w == 0)
                        g->cur_y = g->max_y;

                    g->lflags = stbi__get8(s);

                    if(g->lflags & 0x40) {
                        g->step = 8 * g->line_size; // первый чересстрочный интервал
                        g->parse = 3;
                    }
                    else {
                        g->step = g->line_size;
                        g->parse = 0;
                    }

                    if(g->lflags & 0x80) {
                        stbi__gif_parse_colortable(s, g->lpal, 2 << (g->lflags & 7), g->eflags & 0x01 ? g->transparent : -1);
                        g->color_table = (stbi_uc *) g->lpal;
                    }
                    else if(g->flags & 0x80) {
                        g->color_table = (stbi_uc *) g->pal;
                    }
                    else
                        return stbi__errpuc("missing color table", "Corrupt GIF");

                    o = stbi__process_gif_raster(s, g);
                    if(!o) return NULL;

                    // если бы это был первый кадр,
                    pcount = g->w * g->h;
                    if(first_frame && (g->bgindex > 0)) {
                        // если первый кадр, любой пиксель, который не отрисовывается, получает цвет фона
                        for(pi = 0; pi < pcount; ++pi) {
                            if(g->history[pi] == 0) {
                                g->pal[g->bgindex][3] = 255; // на всякий случай отмените это; При необходимости он будет сброшен в следующем кадре;
                                memcpy(&g->out[pi * 4], &g->pal[g->bgindex], 4);
                            }
                        }
                    }

                    return o;
                }

            case 0x21: { // Расширение комментариев.
                    int len;
                    int ext = stbi__get8(s);
                    if(ext == 0xF9) {  // Расширение графического управления.
                        len = stbi__get8(s);
                        if(len == 4) {
                            g->eflags = stbi__get8(s);
                            g->delay = 10 * stbi__get16le(s); // delay - 1/100th of a second, saving as 1/1000ths.

                            // сбросить старый прозрачный
                            if(g->transparent >= 0) {
                                g->pal[g->transparent][3] = 255;
                            }
                            if(g->eflags & 0x01) {
                                g->transparent = stbi__get8(s);
                                if(g->transparent >= 0) {
                                    g->pal[g->transparent][3] = 0;
                                }
                            }
                            else {
                                // не нужен прозрачный
                                stbi__skip(s, 1);
                                g->transparent = -1;
                            }
                        }
                        else {
                            stbi__skip(s, len);
                            break;
                        }
                    }
                    while((len = stbi__get8(s)) != 0) {
                        stbi__skip(s, len);
                    }
                    break;
                }

            case 0x3B: // код завершения потока gif
                return (stbi_uc *) s; // использование '1' вызывает предупреждение в некоторых компиляторах

            default:
                return stbi__errpuc("unknown code", "Corrupt GIF");
        }
    }
}

static void * stbi__load_gif_main_outofmem(stbi__gif * g, stbi_uc * out, int ** delays)
{
    STBI_FREE(g->out);
    STBI_FREE(g->history);
    STBI_FREE(g->background);

    if(out) STBI_FREE(out);
    if(delays && *delays) STBI_FREE(*delays);
    return stbi__errpuc("outofmem", "Out of memory");
}

static void * stbi__load_gif_main(stbi__context * s, int ** delays, int * x, int * y, int * z, int * comp, int req_comp)
{
    if(stbi__gif_test(s)) {
        int layers = 0;
        stbi_uc * u = 0;
        stbi_uc * out = 0;
        stbi_uc * two_back = 0;
        stbi__gif g;
        int stride;
        int out_size = 0;
        int delays_size = 0;

        STBI_NOTUSED(out_size);
        STBI_NOTUSED(delays_size);

        memset(&g, 0, sizeof(g));
        if(delays) {
            *delays = 0;
        }

        do {
            u = stbi__gif_load_next(s, &g, comp, req_comp, two_back);
            if(u == (stbi_uc *) s) u = 0;   // маркер конца анимированного GIF-изображения

            if(u) {
                *x = g.w;
                *y = g.h;
                ++layers;
                stride = g.w * g.h * 4;

                if(out) {
                    void * tmp = (stbi_uc *) STBI_REALLOC_SIZED(out, out_size, layers * stride);
                    if(!tmp)
                        return stbi__load_gif_main_outofmem(&g, out, delays);
                    else {
                        out = (stbi_uc *) tmp;
                        out_size = layers * stride;
                    }

                    if(delays) {
                        int * new_delays = (int *) STBI_REALLOC_SIZED(*delays, delays_size, sizeof(int) * layers);
                        if(!new_delays)
                            return stbi__load_gif_main_outofmem(&g, out, delays);
                        *delays = new_delays;
                        delays_size = layers * sizeof(int);
                    }
                }
                else {
                    out = (stbi_uc *)stbi__malloc(layers * stride);
                    if(!out)
                        return stbi__load_gif_main_outofmem(&g, out, delays);
                    out_size = layers * stride;
                    if(delays) {
                        *delays = (int *) stbi__malloc(layers * sizeof(int));
                        if(!*delays)
                            return stbi__load_gif_main_outofmem(&g, out, delays);
                        delays_size = layers * sizeof(int);
                    }
                }
                memcpy(out + ((layers - 1) * stride), u, stride);
                if(layers >= 2) {
                    two_back = out - 2 * stride;
                }

                if(delays) {
                    (*delays)[layers - 1U] = g.delay;
                }
            }
        } while(u != 0);

        // свободный временный буфер;
        STBI_FREE(g.out);
        STBI_FREE(g.history);
        STBI_FREE(g.background);

        // выполните окончательное преобразование после загрузки всего;
        if(req_comp && req_comp != 4)
            out = stbi__convert_format(out, 4, req_comp, layers * g.w, g.h);

        *z = layers;
        return out;
    }
    else {
        return stbi__errpuc("not GIF", "Image was not as a gif type.");
    }
}

static void * stbi__gif_load(stbi__context * s, int * x, int * y, int * comp, int req_comp, stbi__result_info * ri)
{
    stbi_uc * u = 0;
    stbi__gif g;
    memset(&g, 0, sizeof(g));
    STBI_NOTUSED(ri);

    u = stbi__gif_load_next(s, &g, comp, req_comp, 0);
    if(u == (stbi_uc *) s) u = 0;   // маркер конца анимированного GIF-изображения
    if(u) {
        *x = g.w;
        *y = g.h;

        // перенесено преобразование после успешной загрузки, чтобы то же самое
        // можно сделать для нескольких кадров.
        if(req_comp && req_comp != 4)
            u = stbi__convert_format(u, 4, req_comp, g.w, g.h);
    }
    else if(g.out) {
        // если произошла ошибка и мы выделили буфер изображения, освободите его!
        STBI_FREE(g.out);
    }

    // свободные буферы, необходимые для загрузки нескольких кадров;
    STBI_FREE(g.history);
    STBI_FREE(g.background);

    return u;
}

static int stbi__gif_info(stbi__context * s, int * x, int * y, int * comp)
{
    return stbi__gif_info_raw(s, x, y, comp);
}
#endif

// *************************************************************************************************
// Загрузчик Radiance RGBE HDR
// первоначально Николас Шульц
#ifndef STBI_NO_HDR
static int stbi__hdr_test_core(stbi__context * s, const char * signature)
{
    int i;
    for(i = 0; signature[i]; ++i)
        if(stbi__get8(s) != signature[i])
            return 0;
    stbi__rewind(s);
    return 1;
}

static int stbi__hdr_test(stbi__context * s)
{
    int r = stbi__hdr_test_core(s, "#?RADIANCE\n");
    stbi__rewind(s);
    if(!r) {
        r = stbi__hdr_test_core(s, "#?RGBE\n");
        stbi__rewind(s);
    }
    return r;
}

#define STBI__HDR_BUFLEN  1024
static char * stbi__hdr_gettoken(stbi__context * z, char * buffer)
{
    int len = 0;
    char c = '\0';

    c = (char) stbi__get8(z);

    while(!stbi__at_eof(z) && c != '\n') {
        buffer[len++] = c;
        if(len == STBI__HDR_BUFLEN - 1) {
            // сбросить до конца строки
            while(!stbi__at_eof(z) && stbi__get8(z) != '\n')
                ;
            break;
        }
        c = (char) stbi__get8(z);
    }

    buffer[len] = 0;
    return buffer;
}

static void stbi__hdr_convert(float * output, stbi_uc * input, int req_comp)
{
    if(input[3] != 0) {
        float f1;
        // Экспонента
        f1 = (float) ldexp(1.0f, input[3] - (int)(128 + 8));
        if(req_comp <= 2)
            output[0] = (input[0] + input[1] + input[2]) * f1 / 3;
        else {
            output[0] = input[0] * f1;
            output[1] = input[1] * f1;
            output[2] = input[2] * f1;
        }
        if(req_comp == 2) output[1] = 1;
        if(req_comp == 4) output[3] = 1;
    }
    else {
        switch(req_comp) {
            case 4:
                output[3] = 1; /* провал */
            case 3:
                output[0] = output[1] = output[2] = 0;
                break;
            case 2:
                output[1] = 1; /* провал */
            case 1:
                output[0] = 0;
                break;
        }
    }
}

static float * stbi__hdr_load(stbi__context * s, int * x, int * y, int * comp, int req_comp, stbi__result_info * ri)
{
    char buffer[STBI__HDR_BUFLEN];
    char * token;
    int valid = 0;
    int width, height;
    stbi_uc * scanline;
    float * hdr_data;
    int len;
    unsigned char count, value;
    int i, j, k, c1, c2, z;
    const char * headerToken;
    STBI_NOTUSED(ri);

    // Проверить идентификатор
    headerToken = stbi__hdr_gettoken(s, buffer);
    if(strcmp(headerToken, "#?RADIANCE") != 0 && strcmp(headerToken, "#?RGBE") != 0)
        return stbi__errpf("not HDR", "Corrupt HDR image");

    // Разобрать заголовок
    for(;;) {
        token = stbi__hdr_gettoken(s, buffer);
        if(token[0] == 0) break;
        if(strcmp(token, "FORMAT=32-bit_rle_rgbe") == 0) valid = 1;
    }

    if(!valid)    return stbi__errpf("unsupported format", "Unsupported HDR format");

    // Ширина и высота разбора
    // невозможно использовать sscanf(), если мы не используем stdio!
    token = stbi__hdr_gettoken(s, buffer);
    if(strncmp(token, "-Y ", 3))  return stbi__errpf("unsupported data layout", "Unsupported HDR format");
    token += 3;
    height = (int) strtol(token, &token, 10);
    while(*token == ' ') ++token;
    if(strncmp(token, "+X ", 3))  return stbi__errpf("unsupported data layout", "Unsupported HDR format");
    token += 3;
    width = (int) strtol(token, NULL, 10);

    if(height > STBI_MAX_DIMENSIONS) return stbi__errpf("too large", "Very large image (corrupt?)");
    if(width > STBI_MAX_DIMENSIONS) return stbi__errpf("too large", "Very large image (corrupt?)");

    *x = width;
    *y = height;

    if(comp) *comp = 3;
    if(req_comp == 0) req_comp = 3;

    if(!stbi__mad4sizes_valid(width, height, req_comp, sizeof(float), 0))
        return stbi__errpf("too large", "HDR image is too large");

    // Чтение данных
    hdr_data = (float *) stbi__malloc_mad4(width, height, req_comp, sizeof(float), 0);
    if(!hdr_data)
        return stbi__errpf("outofmem", "Out of memory");

    // Загрузить данные изображения
    // данные изображения сохраняются в виде некоторого количества изображений.
    if(width < 8 || width >= 32768) {
        // Чтение плоских данных
        for(j = 0; j < height; ++j) {
            for(i = 0; i < width; ++i) {
                stbi_uc rgbe[4];
main_decode_loop:
                stbi__getn(s, rgbe, 4);
                stbi__hdr_convert(hdr_data + j * width * req_comp + i * req_comp, rgbe, req_comp);
            }
        }
    }
    else {
        // Чтение данных в кодировке RLE
        scanline = NULL;

        for(j = 0; j < height; ++j) {
            c1 = stbi__get8(s);
            c2 = stbi__get8(s);
            len = stbi__get8(s);
            if(c1 != 2 || c2 != 2 || (len & 0x80)) {
                // не закодированы по длине, поэтому нам фактически приходится использовать данные THIS в качестве декодированных
                // пиксель (обратите внимание, что это не может быть действительный пиксель — один из RGB должен быть >= 128)
                stbi_uc rgbe[4];
                rgbe[0] = (stbi_uc) c1;
                rgbe[1] = (stbi_uc) c2;
                rgbe[2] = (stbi_uc) len;
                rgbe[3] = (stbi_uc) stbi__get8(s);
                stbi__hdr_convert(hdr_data, rgbe, req_comp);
                i = 1;
                j = 0;
                STBI_FREE(scanline);
                goto main_decode_loop; // да, это не имеет смысла
            }
            len <<= 8;
            len |= stbi__get8(s);
            if(len != width) {
                STBI_FREE(hdr_data);
                STBI_FREE(scanline);
                return stbi__errpf("invalid decoded scanline length", "corrupt HDR");
            }
            if(scanline == NULL) {
                scanline = (stbi_uc *) stbi__malloc_mad2(width, 4, 0);
                if(!scanline) {
                    STBI_FREE(hdr_data);
                    return stbi__errpf("outofmem", "Out of memory");
                }
            }

            for(k = 0; k < 4; ++k) {
                int nleft;
                i = 0;
                while((nleft = width - i) > 0) {
                    count = stbi__get8(s);
                    if(count > 128) {
                        // Беги
                        value = stbi__get8(s);
                        count -= 128;
                        if((count == 0) || (count > nleft)) {
                            STBI_FREE(hdr_data);
                            STBI_FREE(scanline);
                            return stbi__errpf("corrupt", "bad RLE data in HDR");
                        }
                        for(z = 0; z < count; ++z)
                            scanline[i++ * 4 + k] = value;
                    }
                    else {
                        // Дамп
                        if((count == 0) || (count > nleft)) {
                            STBI_FREE(hdr_data);
                            STBI_FREE(scanline);
                            return stbi__errpf("corrupt", "bad RLE data in HDR");
                        }
                        for(z = 0; z < count; ++z)
                            scanline[i++ * 4 + k] = stbi__get8(s);
                    }
                }
            }
            for(i = 0; i < width; ++i)
                stbi__hdr_convert(hdr_data + (j * width + i) * req_comp, scanline + i * 4, req_comp);
        }
        if(scanline)
            STBI_FREE(scanline);
    }

    return hdr_data;
}

static int stbi__hdr_info(stbi__context * s, int * x, int * y, int * comp)
{
    char buffer[STBI__HDR_BUFLEN];
    char * token;
    int valid = 0;
    int dummy;

    if(!x) x = &dummy;
    if(!y) y = &dummy;
    if(!comp) comp = &dummy;

    if(stbi__hdr_test(s) == 0) {
        stbi__rewind(s);
        return 0;
    }

    for(;;) {
        token = stbi__hdr_gettoken(s, buffer);
        if(token[0] == 0) break;
        if(strcmp(token, "FORMAT=32-bit_rle_rgbe") == 0) valid = 1;
    }

    if(!valid) {
        stbi__rewind(s);
        return 0;
    }
    token = stbi__hdr_gettoken(s, buffer);
    if(strncmp(token, "-Y ", 3)) {
        stbi__rewind(s);
        return 0;
    }
    token += 3;
    *y = (int) strtol(token, &token, 10);
    while(*token == ' ') ++token;
    if(strncmp(token, "+X ", 3)) {
        stbi__rewind(s);
        return 0;
    }
    token += 3;
    *x = (int) strtol(token, NULL, 10);
    *comp = 3;
    return 1;
}
#endif // STBI_NO_HDR

#ifndef STBI_NO_BMP
static int stbi__bmp_info(stbi__context * s, int * x, int * y, int * comp)
{
    void * p;
    stbi__bmp_data info;

    info.all_a = 255;
    p = stbi__bmp_parse_header(s, &info);
    if(p == NULL) {
        stbi__rewind(s);
        return 0;
    }
    if(x) *x = s->img_x;
    if(y) *y = s->img_y;
    if(comp) {
        if(info.bpp == 24 && info.ma == 0xff000000)
            *comp = 3;
        else
            *comp = info.ma ? 4 : 3;
    }
    return 1;
}
#endif

#ifndef STBI_NO_PSD
static int stbi__psd_info(stbi__context * s, int * x, int * y, int * comp)
{
    int channelCount, dummy, depth;
    if(!x) x = &dummy;
    if(!y) y = &dummy;
    if(!comp) comp = &dummy;
    if(stbi__get32be(s) != 0x38425053) {
        stbi__rewind(s);
        return 0;
    }
    if(stbi__get16be(s) != 1) {
        stbi__rewind(s);
        return 0;
    }
    stbi__skip(s, 6);
    channelCount = stbi__get16be(s);
    if(channelCount < 0 || channelCount > 16) {
        stbi__rewind(s);
        return 0;
    }
    *y = stbi__get32be(s);
    *x = stbi__get32be(s);
    depth = stbi__get16be(s);
    if(depth != 8 && depth != 16) {
        stbi__rewind(s);
        return 0;
    }
    if(stbi__get16be(s) != 3) {
        stbi__rewind(s);
        return 0;
    }
    *comp = 4;
    return 1;
}

static int stbi__psd_is16(stbi__context * s)
{
    int channelCount, depth;
    if(stbi__get32be(s) != 0x38425053) {
        stbi__rewind(s);
        return 0;
    }
    if(stbi__get16be(s) != 1) {
        stbi__rewind(s);
        return 0;
    }
    stbi__skip(s, 6);
    channelCount = stbi__get16be(s);
    if(channelCount < 0 || channelCount > 16) {
        stbi__rewind(s);
        return 0;
    }
    STBI_NOTUSED(stbi__get32be(s));
    STBI_NOTUSED(stbi__get32be(s));
    depth = stbi__get16be(s);
    if(depth != 16) {
        stbi__rewind(s);
        return 0;
    }
    return 1;
}
#endif

#ifndef STBI_NO_PIC
static int stbi__pic_info(stbi__context * s, int * x, int * y, int * comp)
{
    int act_comp = 0, num_packets = 0, chained, dummy;
    stbi__pic_packet packets[10];

    if(!x) x = &dummy;
    if(!y) y = &dummy;
    if(!comp) comp = &dummy;

    if(!stbi__pic_is4(s, "\x53\x80\xF6\x34")) {
        stbi__rewind(s);
        return 0;
    }

    stbi__skip(s, 88);

    *x = stbi__get16be(s);
    *y = stbi__get16be(s);
    if(stbi__at_eof(s)) {
        stbi__rewind(s);
        return 0;
    }
    if((*x) != 0 && (1 << 28) / (*x) < (*y)) {
        stbi__rewind(s);
        return 0;
    }

    stbi__skip(s, 8);

    do {
        stbi__pic_packet * packet;

        if(num_packets == sizeof(packets) / sizeof(packets[0]))
            return 0;

        packet = &packets[num_packets++];
        chained = stbi__get8(s);
        packet->size    = stbi__get8(s);
        packet->type    = stbi__get8(s);
        packet->channel = stbi__get8(s);
        act_comp |= packet->channel;

        if(stbi__at_eof(s)) {
            stbi__rewind(s);
            return 0;
        }
        if(packet->size != 8) {
            stbi__rewind(s);
            return 0;
        }
    } while(chained);

    *comp = (act_comp & 0x10 ? 4 : 3);

    return 1;
}
#endif

// *************************************************************************************************
// Портативная серая карта и портативный загрузчик пиксельных карт
// Кен Миллер
//
// PGM : http://netpbm.sourceforge.net/doc/pgm.html
// PPM : http://netpbm.sourceforge.net/doc/ppm.html
//
// Известные ограничения:
//    Не поддерживает комментарии в разделе заголовка.
//    Не поддерживает данные изображения ASCII (форматы P2 и P3)

#ifndef STBI_NO_PNM

static int      stbi__pnm_test(stbi__context * s)
{
    char p, t;
    p = (char) stbi__get8(s);
    t = (char) stbi__get8(s);
    if(p != 'P' || (t != '5' && t != '6')) {
        stbi__rewind(s);
        return 0;
    }
    return 1;
}

static void * stbi__pnm_load(stbi__context * s, int * x, int * y, int * comp, int req_comp, stbi__result_info * ri)
{
    stbi_uc * out;
    STBI_NOTUSED(ri);

    ri->bits_per_channel = stbi__pnm_info(s, (int *)&s->img_x, (int *)&s->img_y, (int *)&s->img_n);
    if(ri->bits_per_channel == 0)
        return 0;

    if(s->img_y > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large", "Very large image (corrupt?)");
    if(s->img_x > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large", "Very large image (corrupt?)");

    *x = s->img_x;
    *y = s->img_y;
    if(comp) *comp = s->img_n;

    if(!stbi__mad4sizes_valid(s->img_n, s->img_x, s->img_y, ri->bits_per_channel / 8, 0))
        return stbi__errpuc("too large", "PNM too large");

    out = (stbi_uc *) stbi__malloc_mad4(s->img_n, s->img_x, s->img_y, ri->bits_per_channel / 8, 0);
    if(!out) return stbi__errpuc("outofmem", "Out of memory");
    if(!stbi__getn(s, out, s->img_n * s->img_x * s->img_y * (ri->bits_per_channel / 8))) {
        STBI_FREE(out);
        return stbi__errpuc("bad PNM", "PNM file truncated");
    }

    if(req_comp && req_comp != s->img_n) {
        if(ri->bits_per_channel == 16) {
            out = (stbi_uc *) stbi__convert_format16((stbi__uint16 *) out, s->img_n, req_comp, s->img_x, s->img_y);
        }
        else {
            out = stbi__convert_format(out, s->img_n, req_comp, s->img_x, s->img_y);
        }
        if(out == NULL) return out;  // stbi__convert_format освобождает вход в случае сбоя
    }
    return out;
}

static int      stbi__pnm_isspace(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

static void     stbi__pnm_skip_whitespace(stbi__context * s, char * c)
{
    for(;;) {
        while(!stbi__at_eof(s) && stbi__pnm_isspace(*c))
            *c = (char) stbi__get8(s);

        if(stbi__at_eof(s) || *c != '#')
            break;

        while(!stbi__at_eof(s) && *c != '\n' && *c != '\r')
            *c = (char) stbi__get8(s);
    }
}

static int      stbi__pnm_isdigit(char c)
{
    return c >= '0' && c <= '9';
}

static int      stbi__pnm_getinteger(stbi__context * s, char * c)
{
    int value = 0;

    while(!stbi__at_eof(s) && stbi__pnm_isdigit(*c)) {
        value = value * 10 + (*c - '0');
        *c = (char) stbi__get8(s);
        if((value > 214748364) || (value == 214748364 && *c > '7'))
            return stbi__err("integer parse overflow", "Parsing an integer in the PPM header overflowed a 32-bit int");
    }

    return value;
}

static int      stbi__pnm_info(stbi__context * s, int * x, int * y, int * comp)
{
    int maxv, dummy;
    char c, p, t;

    if(!x) x = &dummy;
    if(!y) y = &dummy;
    if(!comp) comp = &dummy;

    stbi__rewind(s);

    // Получить идентификатор
    p = (char) stbi__get8(s);
    t = (char) stbi__get8(s);
    if(p != 'P' || (t != '5' && t != '6')) {
        stbi__rewind(s);
        return 0;
    }

    *comp = (t == '6') ? 3 : 1;  // «5» — это 1-компонентный файл .pgm; «6» — 3-компонентный .ppm

    c = (char) stbi__get8(s);
    stbi__pnm_skip_whitespace(s, &c);

    *x = stbi__pnm_getinteger(s, &c); // ширина чтения
    if(*x == 0)
        return stbi__err("invalid width", "PPM image header had zero or overflowing width");
    stbi__pnm_skip_whitespace(s, &c);

    *y = stbi__pnm_getinteger(s, &c); // высота чтения
    if(*y == 0)
        return stbi__err("invalid width", "PPM image header had zero or overflowing width");
    stbi__pnm_skip_whitespace(s, &c);

    maxv = stbi__pnm_getinteger(s, &c);  // прочитать максимальное значение
    if(maxv > 65535)
        return stbi__err("max value > 65535", "PPM image supports only 8-bit and 16-bit images");
    else if(maxv > 255)
        return 16;
    else
        return 8;
}

static int stbi__pnm_is16(stbi__context * s)
{
    if(stbi__pnm_info(s, NULL, NULL, NULL) == 16)
        return 1;
    return 0;
}
#endif

static int stbi__info_main(stbi__context * s, int * x, int * y, int * comp)
{
#ifndef STBI_NO_JPEG
    if(stbi__jpeg_info(s, x, y, comp)) return 1;
#endif

#ifndef STBI_NO_PNG
    if(stbi__png_info(s, x, y, comp))  return 1;
#endif

#ifndef STBI_NO_GIF
    if(stbi__gif_info(s, x, y, comp))  return 1;
#endif

#ifndef STBI_NO_BMP
    if(stbi__bmp_info(s, x, y, comp))  return 1;
#endif

#ifndef STBI_NO_PSD
    if(stbi__psd_info(s, x, y, comp))  return 1;
#endif

#ifndef STBI_NO_PIC
    if(stbi__pic_info(s, x, y, comp))  return 1;
#endif

#ifndef STBI_NO_PNM
    if(stbi__pnm_info(s, x, y, comp))  return 1;
#endif

#ifndef STBI_NO_HDR
    if(stbi__hdr_info(s, x, y, comp))  return 1;
#endif

    // тестируйте tga последним, потому что это дрянной тест!
#ifndef STBI_NO_TGA
    if(stbi__tga_info(s, x, y, comp))
        return 1;
#endif
    return stbi__err("unknown image type", "Image not of any known type, or corrupt");
}

static int stbi__is_16_main(stbi__context * s)
{
#ifndef STBI_NO_PNG
    if(stbi__png_is16(s))  return 1;
#endif

#ifndef STBI_NO_PSD
    if(stbi__psd_is16(s))  return 1;
#endif

#ifndef STBI_NO_PNM
    if(stbi__pnm_is16(s))  return 1;
#endif
    return 0;
}

#ifndef STBI_NO_STDIO
STBIDEF int stbi_info(char const * filename, int * x, int * y, int * comp)
{
    FILE * f = stbi__fopen(filename, "rb");
    int result;
    if(!f) return stbi__err("can't fopen", "Unable to open file");
    result = stbi_info_from_file(f, x, y, comp);
    fclose(f);
    return result;
}

STBIDEF int stbi_info_from_file(FILE * f, int * x, int * y, int * comp)
{
    int r;
    stbi__context s;
    long pos = ftell(f);
    stbi__start_file(&s, f);
    r = stbi__info_main(&s, x, y, comp);
    fseek(f, pos, SEEK_SET);
    return r;
}

STBIDEF int stbi_is_16_bit(char const * filename)
{
    FILE * f = stbi__fopen(filename, "rb");
    int result;
    if(!f) return stbi__err("can't fopen", "Unable to open file");
    result = stbi_is_16_bit_from_file(f);
    fclose(f);
    return result;
}

STBIDEF int stbi_is_16_bit_from_file(FILE * f)
{
    int r;
    stbi__context s;
    long pos = ftell(f);
    stbi__start_file(&s, f);
    r = stbi__is_16_main(&s);
    fseek(f, pos, SEEK_SET);
    return r;
}
#endif // !STBI_NO_STDIO

STBIDEF int stbi_info_from_memory(stbi_uc const * buffer, int len, int * x, int * y, int * comp)
{
    stbi__context s;
    stbi__start_mem(&s, buffer, len);
    return stbi__info_main(&s, x, y, comp);
}

STBIDEF int stbi_info_from_callbacks(stbi_io_callbacks const * c, void * user, int * x, int * y, int * comp)
{
    stbi__context s;
    stbi__start_callbacks(&s, (stbi_io_callbacks *) c, user);
    return stbi__info_main(&s, x, y, comp);
}

STBIDEF int stbi_is_16_bit_from_memory(stbi_uc const * buffer, int len)
{
    stbi__context s;
    stbi__start_mem(&s, buffer, len);
    return stbi__is_16_main(&s);
}

STBIDEF int stbi_is_16_bit_from_callbacks(stbi_io_callbacks const * c, void * user)
{
    stbi__context s;
    stbi__start_callbacks(&s, (stbi_io_callbacks *) c, user);
    return stbi__is_16_main(&s);
}

#endif // STB_IMAGE_IMPLEMENTATION

/*
   история изменений:
      2.20 (07.02.2019) поддерживает имена файлов utf8 в Windows; исправить предупреждения и ifdef платформы
      2.19 (11 февраля 2018 г.) исправлено предупреждение
      2.18 (30 января 2018 г.) исправлены предупреждения.
      2.17 (29 января 2018 г.) изменен sbti__shiftsigned, чтобы избежать лязга - ошибка O2
                         1 бит BMP
                         *_is_16_bit API
                         избегать предупреждений
      2.16 (23 июля 2017 г.) все функции имеют 16-битные варианты;
                         STBI_NO_STDIO снова работает;
                         исправления компиляции;
                         исправить округление в unpremultiply;
                         оптимизировать вертикальный переворот;
                         отключить проверку raw_len;
                         исправления документации
      2.15 (18.03.2017) исправлена ошибка png-1,2,4; теперь все файлы Imagenet JPG декодируются;
                         исправления предупреждений; отключить обнаружение SSE во время выполнения на gcc;
                         унифицированная обработка необязательных «возвращаемых» значений;
                         потокобезопасная инициализация таблиц zlib
      2.14 (03.03.2017) удален устаревший STBI_JPEG_OLD ; исправления для изображений Imagenet JPG
      2.13 (29.11.2016) добавлен 16-битный API, сейчас поддерживается только для PNG.
      2.12 (02.04.2016) исправлена опечатка в исправлении 2.11 PSD, приводившая к сбоям
      2.11 (02.04.2016) размещение больших структур в стеке
                         удалить белое матирование для прозрачного PSD
                         исправлено количество сообщаемых каналов для PNG и BMP
                         повторно включить SSE2 в 64-битной версии, отличной от gcc
                         поддержка RGB - в формате JPEG
                         читать 16-битные PNG (только как 8-битные)
      2.10 (22 января 2016 г.) избегать предупреждений, введенных в версии 2.09 STBI_REALLOC_SIZED
      2.09 (16 января 2016 г.) разрешает комментарии в файлах PNM.
                         16 бит на пиксель TGA (не бит на компонент)
                         info() для TGA мог сломаться из-за обработки .hdr.
                         info() для BMP для совместного использования кода вместо небрежного анализа
                         можно использовать STBI_REALLOC_SIZED, если распределитель не поддерживает перераспределение
                         очистка кода
      2.08 (13 сентября 2015 г.) исправление очистки версии 2.07, чтение RGB PSD как RGBA.
      2.07 (13 сентября 2015 г.) исправлены предупреждения компилятора.
                         частичная поддержка анимированного GIF
                         ограниченная поддержка PSD 16 бит на канал
                         #ifdef неиспользуемые функции
                         ошибка с <92 байтами PIC, PNM, HDR, TGA
      2.06 (19 апреля 2015 г.) исправлена ошибка, из-за которой PSD возвращал неправильное значение '*comp'.
      2.05 (19 апреля 2015 г.) исправлена ошибка в обработке прогрессивного JPEG, исправлено предупреждение.
      2.04 (15 апреля 2015 г.) попробуйте повторно включить SIMD на 64-разрядной версии MinGW.
      2.03 (12 апреля 2015 г.) дополнительная проверка на наличие повреждений (mmozeiko)
                         stbi_set_flip_vertically_on_load (нгильмот)
                         исправлена поддержка NEON; исправить поддержку mingw
      2.02 (19 января 2015 г.) исправлено неправильное утверждение, исправлено предупреждение
      2.01 (17 января 2015 г.) исправлены различные предупреждения; подавить SIMD в 32-битной версии gcc без -msse2
      2.00b (25 декабря 2014 г.) исправление STBI_MALLOC в прогрессивном JPEG
      2.00 (25 декабря 2014 г.) оптимизация JPG, включая x86 SSE2 и NEON SIMD (ryg)
                         прогрессивный JPEG (stb)
                         Поддержка PGM / PPM (Кен Миллер)
                         STBI_MALLOC,STBI_REALLOC,STBI_FREE
                         Исправление GIF - похоже, никогда не работало
                         STBI_NO_*, STBI_ONLY_*
      1.48 (14 декабря 2014 г.) исправлено неправильное имя assert().
      1.47 (14 декабря 2014 г.) Поддержка 1/2/4-битного PNG, как прямая, так и с палитрой (Омар Корнут и stb)
                         оптимизировать PNG (рыг)
                         исправить ошибку в чересстрочном формате PNG с заданным пользователем количеством каналов (stb)
      1.46  (2014-08-26)
              исправить сломанный фрагмент tRNS (прозрачность в стиле цветового ключа) в PNG без палитры
      1.45  (2014-08-16)
              исправить внутреннюю ошибку компилятора MSVC - ARM путем переноса malloc
      1.44  (2014-08-07)
              различные исправления предупреждений от Ронни Шевалье
      1.43  (2014-07-15)
              исправить MSVC - проблема только с компилятором в коде, измененном в версии 1.42
      1.42  (2014-07-09)
              не определять _CRT_SECURE_NO_WARNINGS (влияет на пользовательский код)
              исправления в пути stbi__cleanup_jpeg
              добавлен STBI_ASSERT, чтобы не требовать assert.h
      1.41  (2014-06-25)
              исправлен поиск и замена из версии 1.36, из-за которого искажались комментарии/сообщения об ошибках
      1.40  (2014-06-22)
              исправить предупреждение об инициализации структуры gcc
      1.39  (2014-06-15)
              исправление оптимизации TGA, когда req_comp != количество компонентов в TGA ;
              исправлена загрузка GIF, потому что BMP не перематывалась (упс, в моем наборе тестов нет GIF-файлов)
              добавить поддержку BMP версии 5 (больше игнорируемых полей)
      1.38  (2014-06-06)
              подавлять предупреждения MSVC при целочисленном приведении значений, усекающих значения
              исправить случайное переименование поля пропуска ввода/вывода
      1.37  (2014-06-04)
              удалить дубликат typedef
      1.36  (2014-06-03)
              конвертировать в заголовочный файл однофайловую библиотеку
              если de-iphone не установлен, загрузите изображения iphone с измененным цветом вместо возврата NULL
      1.35  (2014-05-27)
              различные предупреждения
              исправить сломанный путь STBI_SIMD
              исправлена ошибка, из-за которой stbi_load_from_file больше не оставлял указатель файла в правильном месте
              исправить сломанный непростой путь для 32-битного BMP (возможно, никогда не использовался)
              Оптимизация TGA от Арсения Капулькина
      1,34 (неизвестно)
              используйте STBI_NOTUSED в stbi__resample_row_generic(), исправьте еще одну утечку в случае сбоя tga
      1.33  (2011-07-14)
              заставить stbi_is_hdr работать в STBI_NO_HDR (как указано), небольшие улучшения для компилятора
      1.32  (2011-07-13)
              поддержка функции «информация» для всех поддерживаемых типов файлов (SpartanJ)
      1.31  (2011-06-20)
              еще несколько исправлений утечек, ошибка в обработке PNG (SpartanJ)
      1.30  (2011-06-11)
              добавлена возможность загружать файлы с помощью обратных вызовов для соответствия пользовательским потокам ввода (Бен Венгер)
              удалены устаревшие функции тестирования/загрузки, специфичные для формата.
              удалена поддержка устанавливаемых форматов файлов ( stbi_loader ) - в любом случае она была бы сломана для обратных вызовов IO
              случаи ошибок в bmp и tga выдают сообщения и не просачиваются (Раймонд Барбьеро, Гриша)
              исправление неэффективности декодирования 32-битного BMP (Дэвид Ву)
      1.29  (2010-08-16)
              различные исправления предупреждений от Орельена Пошевиля
      1.28  (2010-08-01)
              исправить ошибку в прозрачности палитры GIF (SpartanJ)
      1.27  (2010-08-01)
              приведение к stbi_uc для исправления предупреждений
      1.26  (2010-07-24)
              исправить ошибку в буферизации файлов для PNG, о которой сообщил SpartanJ
      1.25  (2010-07-17)
              исправить предупреждение trans_data (Вон Чун)
      1.24  (2010-07-12)
              улучшения производительности при чтении файлов на платформах с тяжелыми блокировками fgetc()
              небольшие улучшения производительности для JPEG
              устаревшие функции, специфичные для типа, поэтому мы получим обратную связь, если они понадобятся
              попытка исправить предупреждение trans_data (Вон Чун)
      1.23 исправлена ошибка в поддержке iPhone
      1.22  (2010-07-10)
              удалено изображение *пишу* поддержка
              Поддержка stbi_info от Jetro Lauha
              Поддержка GIF от Жана-Марка Лиенера
              iPhone PNG – расширения от Джеймса Брауна
              исправления предупреждений от Николаса Шульца и Янеза Земвы (т. е. stbi__err . Janez (U+017D)emva)
      1.21 исправлено использование «stbi_uc» в заголовке (сообщил Джон Блоу)
      В версии 1.20 добавлена поддержка Softimage PIC от Тома Седдона.
      1.19 ошибка в проверке целостности чересстрочного PNG (найдена ryg)
      1.18  (2008-08-02)
              исправить ошибку многопоточности (локальная изменяемая статика)
      1.17 поддерживает чересстрочную развертку PNG
      Основное исправление версии 1.16: stbi__convert_format преобразовывал слишком много пикселей.
      1.15 инициализировать некоторые поля для обеспечения потокобезопасности
      1.14 исправлена ошибка потокобезопасного преобразования
              версия только для заголовочного файла (#define STBI_HEADER_FILE_ONLY перед включением)
      1.13 потокобезопасный
      1.12 константные квалификаторы в API
      1.11 Поддержка устанавливаемого IDCT, процедур преобразования цветового пространства.
      1.10 Исправления для 64-битной версии (не используйте «unsigned long»)
              оптимизированная повышающая дискретизация Фабиана «ryg» Гизена
      1.09 Исправлено преобразование формата для кода PSD (неправильные глобальные переменные!)
      1.08 Код PSD Тэтчер Ульрих, интегрированный Николасом Шульцем
      1.07 снова попытайтесь исправить предупреждения/ошибки C++.
      1.06 снова попытайтесь исправить предупреждения/ошибки C++.
      1.05 исправлена загрузка TGA, чтобы вернуть правильный *comp и использовать хороший расчет яркости
      В версии 1.04 альфа-версия по умолчанию равна 1, а не 255; используйте «void *» для stbi_image_free
      1.03 исправления ошибок в STBI_NO_STDIO, STBI_NO_HDR
      1.02 поддержка (подмножества) файлов HDR, плавающий интерфейс для предпочтительного доступа к ним.
      1.01 исправлена ошибка: возможная ошибка при обработке BMP с правой стороны... не уверен
              исправить ошибку: функции stbi__bmp_load() и stbi__tga_load() вообще не работали
      1.00 интерфейс для zlib, который пропускает заголовок zlib
      0.99 правильная обработка альфы в палитре
      0.98 Загрузчик TGA от Lonesock; динамически добавлять загрузчики (непроверено)
      0,97 ошибок jpeg в слишком большом файле; также поймать еще один сбой malloc
      0.96 исправлено обнаружение неверного значения v - форум Particleman @mollyrocket
      0,95 во время сканирования заголовка, поиск маркеров в случае заполнения
      0.94 STBI_NO_STDIO для отключения использования stdio; переименуйте все #defines одинаково
      0.93 обрабатывает вывод jpegtran; подробные ошибки
      0.92 читает 4,8,16,24,32-битные файлы BMP нескольких форматов.
      0.91 выходные 24-битные файлы Windows 3.0 BMP
      0.90 исправляет еще несколько предупреждений; увеличить номер версии, чтобы он приблизился к 1.0
      Исправления ошибок 0.61 благодаря Марку Леблану и Кристоферу Ллойду.
      0.60 исправлена компиляция как C++
      Предупреждения об исправлениях версии 0.59: объедините исправления Дэйва Мура -Wall
      0.58 исправлена ошибка: в несжатом режиме zlib len/nlen был неправильный порядок байтов
      0.57 исправлена ошибка: последний символ Хаффмана в формате jpg перед маркером был >9 бит, но доступно менее 16.
      0.56 исправлена ошибка: несжатый режим zlib len vs. nlen
      0.55 исправлена ошибка: restart_interval не инициализировался до 0.
      0.54 позволяет использовать NULL для 'int *comp'
      0.53 исправлена ошибка в png 3->4; ускорение декодирования png
      0.52 png обрабатывает req_comp =3,4 напрямую; мелкая уборка; комментарии в формате JPEG
      0.51 подчиняются запросам req_comp, 1-компонентные jpeg возвращаются как 1-компонентные,
              в «тесте» проверяется только тип, а не поддерживаем ли мы этот вариант
      0.50  (2006-11-19)
              первая выпущенная версия
*/


/*
------------------------------------------------------------------------------
Это программное обеспечение доступно по двум лицензиям — выбирайте ту, которую предпочитаете.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT Лицензия
Copyright (c) 2017 Sean Barrett
Разрешение настоящим предоставляется бесплатно любому лицу, получившему копию
данное программное обеспечение и связанные с ним файлы документации («Программное обеспечение») для решения
Программное обеспечение без ограничений, включая, помимо прочего, права на
использовать, копировать, изменять, объединять, публиковать, распространять, сублицензировать и/или продавать копии
Программного обеспечения и разрешать лицам, которым предоставлено Программное обеспечение, делать
Итак, при соблюдении следующих условий:
Вышеупомянутое уведомление об авторских правах и настоящее уведомление о разрешении должны быть включены во все
копии или существенные части Программного обеспечения.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - общественное достояние ( www.unlicense.org)
Это бесплатное и ничем не обремененное программное обеспечение, общедоступное.
Любой может свободно копировать, изменять, публиковать, использовать, компилировать, продавать или распространять это
программное обеспечение в виде исходного кода или в виде скомпилированного двоичного файла для любых целей,
коммерческие или некоммерческие, и любыми способами.
В юрисдикциях, признающих законы об авторском праве, автор или авторы настоящего
программное обеспечение передать все права, связанные с авторскими правами на программное обеспечение, для общественности
домен. Мы делаем это на благо общества в целом и
в ущерб нашим наследникам и преемникам. Мы намерены, чтобы это посвящение стало
явный акт отказа навечно от всех нынешних и будущих прав на
это программное обеспечение согласно закону об авторском праве.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/
