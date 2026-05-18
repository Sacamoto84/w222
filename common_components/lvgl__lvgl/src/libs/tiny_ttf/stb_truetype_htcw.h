// stb_truetype.h — v1.26htcw (вилка для включения потоковой передачи и сред с низким объемом памяти)
// stb_truetype.h - v1.26 - общественное достояние
// автор: Шон Барретт / RAD Game Tools, 2009–2021 гг.
//
// =======================================================================
//
//    NO SECURITY GUARANTEE -- DO NOT USE THIS ON UNTRUSTED FONT FILES
//
// Эта библиотека не проверяет диапазон смещений, найденных в файле.
// это означает, что злоумышленник может использовать его для чтения произвольной памяти.
//
// =======================================================================
//
//   Эта библиотека обрабатывает файлы TrueType:
//        анализировать файлы
//        извлечь метрики глифа
//        извлекать формы глифов
//        рендеринг глифов в одноканальные растровые изображения со сглаживанием (коробчатый фильтр)
//        визуализировать глифы в одноканальные растровые изображения SDF (поле/функция расстояния со знаком)
//
//   Дела:
//        cmaps, отличные от MS
//        защита от сбоев при работе с плохими данными
//        hinting? (no longer patented)
//        cleartype-style AA?
//        optimize: use simple memory allocator for intermediates
//        optimize: build edge-list directly from curves
//        optimize: rasterize directly from curves?
//
// ADDITIONAL CONTRIBUTORS
//
//   Микко Мононен: поддержка составных фигур, больше форматов cmap
//   Тор Андерссон: кернинг, субпиксельный рендеринг
//   Дугал Джонсон: обработка шрифтов OpenType/Type 2
//   Даниэль Рибейро Масиэль: базовый кернинг на основе GPOS
//
//   Разное другое:
//       Райан Гордон
//       Саймон Гласс
//       github:IntellectualKitty
//       Иманол Селайя
//       Даниэль Рибейро Масиэль
//
//   Отчеты об ошибках/предупреждениях/исправлениях:
//       «Zer» на mollyrocket Фабиан «ryg» Гизен github:NiLuJe
//       Касс Эверитт Мартинс Можейко github: aloucks
//       стоико (Haemimont Games) Cap Petschulat github:oyvindjam
//       Брайан Хук Омар Корнат github:vassvik
//       Уолтер ван Нифтрик Райан Григ
//       Дэвид Гоу, Питер ЛаВалль
//       Дэвид Гивен Сергей Попов
//       Иван-Ассен Иванов Гюмо X. Кланджор
//       Энтони Пеш Хигор Еврипид
//       Йохан Дюпарк Томас Филдс
//       Хоу Цимин Дерек Виньярд
//       Роб Лоуч Корт Стрэттон
//       Кенни Филлис-младший Брайан Костабайл
//       Кен Воскуил (каэсве)
//
// VERSION HISTORY
//
//   1.26 (28 августа 2021 г.) исправлен сломанный растеризатор.
//   1.25 (11 июля 2021 г.) множество исправлений
//   1.24 (05.02.2020) исправлено предупреждение
//   1.23 (02.02.2020) запрос данных SVG для глифов; запросить всю таблицу кернинга (но только кернинг, а не GPOS )
//   1.22 (11 августа 2019 г.) минимизирует дублирование отсутствующих символов; исправить кернинг, если определены оба 'GPOS' и 'kern'
//   1.21 (25 февраля 2019 г.) исправлено предупреждение
//   1.20 (07.02.2019) PackFontRange пропускает недостающие кодовые точки;  GetScaleFontVMetrics()
//   1.19 (11.02.2018) GPOS кернинг, STBTT_fmod
//   1.18 (29.01.2018) добавлена недостающая функция
//   1.17 (23 июля 2017 г.) добавить больше аргументов const; исправление документа
//   1.16 (12.07.2017) Поддержка SDF
//   1.15 (03.03.2017) приведите больше аргументов const
//   1.14 (16.01.2017) функция num-fonts-in- TTC
//   1.13 (02.01.2017) поддерживает шрифты OpenType, некоторые шрифты Apple.
//   1.12 (25 октября 2016 г.) подавляет предупреждения об удалении const с помощью -Wcast-qual
//   1.11 (02.04.2016) исправлено предупреждение о неиспользуемой переменной.
//   1.10 (02.04.2016) определяемый пользователем fabs() ; редкая утечка памяти; удалить дубликат typedef
//   1.09 (16 января 2016 г.) исправление предупреждения; избежать сбоя при выходе из памяти; правильно используйте распределение пользовательских данных
//   1.08 (13 сентября 2015 г.) документ stbtt_Rasterize (); исправления для вертикальных и горизонтальных краев
//   1.07 (01 августа 2015 г.) позволяет PackFontRanges принимать массивы разреженных кодовых точек;
//                     вариант PackFontRanges для упаковки и рендеринга на отдельных этапах;
//                     fix stbtt_GetFontOFfsetForIndex (never worked for non-0 input?);
//                     исправлена ошибка assert() в новом растеризаторе
//                     замените assert() на STBTT_assert() в новом растеризаторе
//
//   Полную историю можно найти в конце этого файла.
//
// LICENSE
//
//   Информацию о лицензии смотрите в конце файла.
//
// USAGE
//
//   Включите этот файл во все места, где на него нужно ссылаться. В ONE C/C++
//   файл, напишите:
//      #define STB_TRUETYPE_IMPLEMENTATION
//   перед #include этого файла. Это расширяет фактическое
//   реализацию в этот файл C/C++.
//
//   Чтобы сделать реализацию частной для файла, который ее генерирует,
//      #define STBTT_STATIC
//
//   Простой 3D API (не отправляйте его, но он подойдет для инструментов и быстрого старта)
//           stbtt_BakeFontBitmap () — записать шрифт в растровое изображение для использования в качестве текстуры.
//           stbtt_GetBakedQuad () — вычисляет квадрат для рисования заданного символа
//
//   Улучшенный 3D API (больше возможности поставки):
//           #include "stb_rect_pack.h" -- необязательно, но очень хочется
//           stbtt_PackBegin ()
//           stbtt_PackSetOversampling() — для улучшения качества на мелких шрифтах
//           stbtt_PackFontRanges() -- упаковывает и рендерит
//           stbtt_PackEnd ()
//           stbtt_GetPackedQuad ()
//
//   «Загрузить» файл шрифта из буфера памяти (буфер должен оставаться загруженным)
//           stbtt_InitFont ()
//           stbtt_GetFontOffsetForIndex() -- индексация для коллекций шрифтов TTC
//           stbtt_GetNumberOfFonts() -- количество шрифтов для коллекций шрифтов TTC
//
//   Преобразование кодовой точки Юникода в растровое изображение
//           stbtt_GetCodepointBitmap () — выделяет и возвращает растровое изображение
//           stbtt_MakeCodepointBitmap () — рендерится в предоставленное вами растровое изображение.
//           stbtt_GetCodepointBitmapBox () — насколько большим должно быть растровое изображение
//
//   Продвижение/позиционирование персонажа
//           stbtt_GetCodepointHMetrics ()
//           stbtt_GetFontVMetrics ()
//           stbtt_GetFontVMetricsOS2 ()
//           stbtt_GetCodepointKernAdvance ()
//
//   Начиная с версии 1.06 растеризатор заменен на новый,
//   более быстрый и, как правило, более точный растеризатор. Новый растеризатор подробнее
//   точно измеряет покрытие пикселей для сглаживания, за исключением случая
//   где несколько фигур перекрываются, и в этом случае значение пикселя AA переоценивается.
//   покрытие. Таким образом, сглаживание пересекающихся фигур может выглядеть неправильно. Если
//   это оказывается проблемой, вы можете снова включить старый растеризатор с помощью
//        #define STBTT_RASTERIZER_VERSION 1
//   что приведет к снижению скорости примерно на 15%.
//
// ADDITIONAL DOCUMENTATION
//
//   Сразу после этого блока комментариев идет ряд примеров программ.
//
//   После примеров программ идет раздел «заголовочный файл». Этот раздел
//   включает документацию для каждой функции API.
//
//   Некоторые важные понятия, которые необходимо понимать при использовании этой библиотеки:
//
//      Кодовая точка
//         Символы определяются кодовыми точками Юникода, например. 65 это
//         заглавная буква A, 231 — строчная буква c с седилем, 0x7e30 —
//         хирагана для «ма».
//
//      Глиф
//         Визуальная форма символа (каждая кодовая точка отображается как
//         какой-то глиф)
//
//      Индекс глифа
//         Целое число ID, зависящее от шрифта, представляющее глиф.
//
//      Базовый уровень
//         Формы глифов определяются относительно базовой линии, которая является
//         нижняя часть символов верхнего регистра. Символы расширяют оба вышеуказанных
//         и ниже базовой линии.
//
//      Текущая точка
//         Когда вы рисуете текст на экране, вы отслеживаете «текущую точку».
//         что является происхождением каждого персонажа. Вертикаль текущей точки
//         позиция является базовой. Даже «запеченные шрифты» используют эту модель.
//
//      Метрики вертикального шрифта
//         Вертикальные качества шрифта, используемые для вертикального позиционирования.
//         и расставьте символы. См. документацию для stbtt_GetFontVMetrics.
//
//      Размер шрифта в пикселях или пунктах
//         Предпочтительный интерфейс для указания размеров шрифта в stb_truetype
//         заключается в указании высоты шрифта по вертикали в пикселях.
//         Если это звучит достаточно хорошо, пропустите следующий абзац.
//
//         Вместо этого большинство API шрифтов используют «точки», которые являются обычным типографским шрифтом.
//         измерение для описания размера шрифта, определяемое как 72 пункта на дюйм.
//         stb_truetype предоставляет точку API для совместимости. Однако правда
//         Условные обозначения «на дюйм» не имеют особого смысла на компьютерных дисплеях.
//         поскольку разные мониторы имеют разное количество пикселей на
//         дюйм. Например, Windows традиционно использует соглашение, согласно которому
//         на дюйм приходится 96 пикселей, что делает дюймовые измерения более точными.
//         ничего общего с дюймами и, таким образом, эффективно определяя точку для
//         быть 1,333 пикселя. Кроме того, данные шрифта TrueType предоставляют
//         явный масштабный коэффициент для масштабирования глифов данного шрифта до точек,
//         но автор заметил, что этот масштабный коэффициент часто ошибочен
//         для некоммерческих шрифтов, что позволяет масштабировать шрифты в пунктах
//         в соответствии со спецификацией TrueType на практике имеет бессвязный размер.
//
// DETAILED USAGE:
//
//  Масштаб:
//    Выберите высоту шрифта в пунктах или пикселях.
//    Вызовите ScaleForPixelHeight или ScaleForMappingEmToPixels для вычисления.
//    масштабный коэффициент SF, который будет использоваться всеми другими функциями.
//
//  Базовый уровень:
//    Вам нужно выбрать координату Y, которая является базовой линией того места, где
//    появится ваш текст. Вызовите GetFontBoundingBox, чтобы получить относительное значение базовой линии.
//    ограничивающая рамка для всех персонажей.  SF *-y0 — расстояние в пикселях.
//    что символ наихудшего случая может выходить за пределы базовой линии, поэтому, если
//    вы хотите, чтобы верхний край символов отображался вверху
//    экране, где y=0, то вы должны установить базовую линию на SF *-y0.
//
//  Текущая точка:
//    Установите текущую точку, где появится первый символ.
//    первый символ может продолжаться влево от текущей точки; это шрифт
//    зависимый. Вы можете выбрать текущую точку, которая является крайней левой
//    точку и надежду, или добавьте отступы, или установите ограничительную рамку, или
//    левый подшипник первого символа, который будет отображаться и устанавливаться
//    текущая точка на основе этого.
//
//  Отображение персонажа:
//    Вычислите ограничивающую рамку персонажа. Он будет содержать подписанные значения
//    относительно <current_point, базовый уровень>. Т.е. если он возвращает x0,y0,x1,y1,
//    тогда символ должен отображаться в прямоугольнике из
//    < current_point + SF *x0, базовый+ SF *y0> до < current_point + SF *x1, базовый+ SF *y1).
//
//  Переход к следующему персонажу:
//    Вызовите GlyphHMetrics и вычислите «current_point += SF * заранее».
//
//
// ADVANCED USAGE
//
//   Качество:
//
//    - Use the functions with Subpixel at the end to allow your characters
//      иметь субпиксельное позиционирование. Поскольку шрифт сглажен, не
//      намекнул, что это очень важно для качества. (Это невозможно с
//      запеченные шрифты.)
//
//    - Kerning is now supported, and if you're supporting subpixel rendering
//      тогда стоит использовать кернинг, чтобы придать тексту безупречный вид.
//
//   Производительность:
//
//    - Convert Unicode codepoints to glyph indexes and operate on the glyphs;
//      если вы этого не сделаете, stb_truetype будет вынужден выполнить преобразование
//      каждый звонок.
//
//    - There are a lot of memory allocations. We should modify it to take
//      временный буфер и выделить из него (без освобождения),
//      должно сильно помочь в производительности.
//
// NOTES
//
//   Система использует необработанные данные, найденные в файле .ttf, не изменяя их.
//   и без построения вспомогательных структур данных. Это немного неэффективно
//   в системах с прямым порядком байтов (данные имеют прямой порядок байтов), но при условии, что вы
//   кэширование растровых изображений или фигур глифов не должно быть большой проблемой.
//
//   Кажется, очень сложно программно определить, какой шрифт
//   данный файл находится в общем виде. Для этого я предоставляю API, но у меня нет
//   рекомендую это.
//
//
// PERFORMANCE MEASUREMENTS FOR 1.06:
//
//                      32-битный 64-битный
//   Предыдущий выпуск: 8,83 с 7,68 с
//   Распределение пулов: 7,72 с 6,34 с
//   Встроенная сортировка: 6,54 с 5,65 с
//   Новый растеризатор: 5,63 с 5,00 с

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
////
////  SAMPLE PROGRAMS
////
//
//  Неполный пример text-in-3d-api, который правильно выровняет четырехугольники и не приведет к потерям.
//  Полную версию см. в разделе «tests/truetype_demo_win32.c».
#if 0
#define STB_TRUETYPE_IMPLEMENTATION  // заставить следующее включить для генерации реализации
#include "stb_truetype.h"

unsigned char ttf_buffer[1 << 20];
unsigned char temp_bitmap[512 * 512];

stbtt_bakedchar cdata[96]; // ASCII 32..126 — 95 символов.
GLuint ftex;

void my_stbtt_initfont(void)
{
    fread(ttf_buffer, 1, 1 << 20, fopen("c:/windows/fonts/times.ttf", "rb"));
    stbtt_BakeFontBitmap(ttf_buffer, 0, 32.0, temp_bitmap, 512, 512, 32, 96, cdata); // нет гарантии, что это подойдет!
    // можно освободить ttf_buffer на этом этапе
    glGenTextures(1, &ftex);
    glBindTexture(GL_TEXTURE_2D, ftex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
    // можно освободить temp_bitmap на этом этапе
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void my_stbtt_print(float x, float y, char * text)
{
    // предположим, что ортогональная проекция имеет единицы измерения = пиксели экрана, начало координат вверху слева.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ftex);
    glBegin(GL_QUADS);
    while(*text) {
        if(*text >= 32 && *text < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(cdata, 512, 512, *text - 32, &x, &y, &q, 1);//1=открытый язык и d3d10+,0=d3d9
            glTexCoord2f(q.s0, q.t0);
            glVertex2f(q.x0, q.y0);
            glTexCoord2f(q.s1, q.t0);
            glVertex2f(q.x1, q.y0);
            glTexCoord2f(q.s1, q.t1);
            glVertex2f(q.x1, q.y1);
            glTexCoord2f(q.s0, q.t1);
            glVertex2f(q.x0, q.y1);
        }
        ++text;
    }
    glEnd();
}
#endif
//
//
//////////////////////////////////////////////////////////////////////////////
//
// Полная программа (она компилируется): получить одно растровое изображение, распечатать как ASCII.
//
#if 0
#include <stdio.h>
#define STB_TRUETYPE_IMPLEMENTATION  // заставить следующее включить для генерации реализации
#include "stb_truetype.h"

char ttf_buffer[1 << 25];

int main(int argc, char ** argv)
{
    stbtt_fontinfo font;
    unsigned char * bitmap;
    int w, h, i, j, c = (argc > 1 ? atoi(argv[1]) : 'a'), s = (argc > 2 ? atoi(argv[2]) : 20);

    fread(ttf_buffer, 1, 1 << 25, fopen(argc > 3 ? argv[3] : "c:/windows/fonts/arialbd.ttf", "rb"));

    stbtt_InitFont(&font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer, 0));
    bitmap = stbtt_GetCodepointBitmap(&font, 0, stbtt_ScaleForPixelHeight(&font, s), c, &w, &h, 0, 0);

    for(j = 0; j < h; ++j) {
        for(i = 0; i < w; ++i)
            putchar(" .:ioVM@"[bitmap[j * w + i] >> 5]);
        putchar('\n');
    }
    return 0;
}
#endif
//
// Выход:
//
//     .ii.
//    @@@@@@.
//   В @Mio @ @o
//   :i.  V@V
//     :oM@@M
//   :@@@MM@M
//   @@o  o@M
//  :@@.  M@M
//   @@@o@@@@
//   :M@@V:@@.
//
//////////////////////////////////////////////////////////////////////////////
//
// Полная программа: напечатайте «Hello World!» баннер, с ошибками
//
#if 0
char buffer[24 << 20];
unsigned char screen[20][79];

int main(int arg, char ** argv)
{
    stbtt_fontinfo font;
    int i, j, ascent, baseline, ch = 0;
    float scale, xpos = 2; // оставьте небольшой отступ на случай, если символ выдвинется влево
    char * text = "Heljo World!"; // намеренно написана с ошибкой, чтобы показать сломанность буквы «lj»

    fread(buffer, 1, 1000000, fopen("c:/windows/fonts/arialbd.ttf", "rb"));
    stbtt_InitFont(&font, buffer, 0);

    scale = stbtt_ScaleForPixelHeight(&font, 15);
    stbtt_GetFontVMetrics(&font, &ascent, 0, 0);
    baseline = (int)(ascent * scale);

    while(text[ch]) {
        int advance, lsb, x0, y0, x1, y1;
        float x_shift = xpos - (float)floor(xpos);
        stbtt_GetCodepointHMetrics(&font, text[ch], &advance, &lsb);
        stbtt_GetCodepointBitmapBoxSubpixel(&font, text[ch], scale, scale, x_shift, 0, &x0, &y0, &x1, &y1);
        stbtt_MakeCodepointBitmapSubpixel(&font, &screen[baseline + y0][(int)xpos + x0], x1 - x0, y1 - y0, 79, scale, scale,
                                          x_shift, 0, text[ch]);
        // обратите внимание, что это уничтожает старые данные, поэтому, когда поля символов перекрываются (например, «lj»), это неправильно
        // потому что этот API на самом деле предназначен для запекания растровых изображений персонажей в текстуры. если вы хотите визуализировать
        // последовательность символов, вам действительно нужно визуализировать каждое растровое изображение во временный буфер, а затем
        // «альфа-смесь» в рабочий буфер
        xpos += (advance * scale);
        if(text[ch + 1])
            xpos += scale * stbtt_GetCodepointKernAdvance(&font, text[ch], text[ch + 1]);
        ++ch;
    }

    for(j = 0; j < 20; ++j) {
        for(i = 0; i < 78; ++i)
            putchar(" .:ioVM@"[screen[j][i] >> 5]);
        putchar('\n');
    }

    return 0;
}
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
////
////   INTEGRATION WITH YOUR CODEBASE
////
//// Следующие разделы позволяют вам предоставлять альтернативные определения
//// функций библиотеки C, используемых stb_truetype, например. если ты этого не сделаешь
//// связь с библиотекой времени выполнения C.

#ifdef STB_TRUETYPE_IMPLEMENTATION
// #define свой собственный (u) stbtt_int8 /16/32, прежде чем включать его, чтобы переопределить это
#ifndef stbtt_uint8
    typedef unsigned char   stbtt_uint8;
    typedef signed   char   stbtt_int8;
    typedef unsigned short  stbtt_uint16;
    typedef signed   short  stbtt_int16;
    typedef unsigned int    stbtt_uint32;
    typedef signed   int    stbtt_int32;
#endif

typedef char stbtt__check_size32[sizeof(stbtt_int32) == 4 ? 1 : -1];
typedef char stbtt__check_size16[sizeof(stbtt_int16) == 2 ? 1 : -1];

// определить STBTT_STDIO_STREAM для потоковой передачи из объекта FILE
// а не по памяти. Или определите STBTT_STREAM_TYPE ,
// STBTT_STREAM_READ и STBTT_STREAM_SEEK для реализации
// другой источник потокового вещания
#ifdef STBTT_STDIO_STREAM
    #include <stdio.h>
    #define STBTT_STREAM_TYPE FILE*
    #define STBTT_STREAM_READ(s,x,y) fread(x,1,y,s);
    #define STBTT_STREAM_SEEK(s,x)  fseek(s,x,SEEK_SET);
#endif

// размеры коэффициента кучи для различного количества объектов
// подстроиться под вашу платформу. Ниже подходит для
// современные машины класса PC.
#ifndef STBTT_HEAP_FACTOR_SIZE_32
    #define STBTT_HEAP_FACTOR_SIZE_32 2000
#endif

#ifndef STBTT_HEAP_FACTOR_SIZE_128
    #define STBTT_HEAP_FACTOR_SIZE_128 800
#endif

#ifndef STBTT_HEAP_FACTOR_SIZE_DEFAULT
    #define STBTT_HEAP_FACTOR_SIZE_DEFAULT 100
#endif

// например  #define свой собственный STBTT_ifloor/STBTT_iceil (), чтобы избежать math.h
#ifndef STBTT_ifloor
    #include <math.h>
    #define STBTT_ifloor(x)   ((int) floor(x))
    #define STBTT_iceil(x)    ((int) ceil(x))
#endif

#ifndef STBTT_sqrt
    #include <math.h>
    #define STBTT_sqrt(x)      (float)sqrt(x)
    #define STBTT_pow(x,y)     pow(x,y)
#endif

#ifndef STBTT_fmod
    #include <math.h>
    #define STBTT_fmod(x,y)    fmod(x,y)
#endif

#ifndef STBTT_cos
    #include <math.h>
    #define STBTT_cos(x)       cos(x)
    #define STBTT_acos(x)      acos(x)
#endif

#ifndef STBTT_fabs
    #include <math.h>
    #define STBTT_fabs(x)      (float)fabs(x)
#endif

// #define ваши собственные функции «STBTT_malloc» / «STBTT_free», чтобы избежать malloc.h
#ifndef STBTT_malloc
    #include <stdlib.h>
    #define STBTT_malloc(x,u)  ((void)(u),malloc(x))
    #define STBTT_free(x,u)    ((void)(u),free(x))
#endif

#ifndef STBTT_assert
    #include <assert.h>
    #define STBTT_assert(x)    assert(x)
#endif

#ifndef STBTT_strlen
    #include <string.h>
    #define STBTT_strlen(x)    strlen(x)
#endif

#ifndef STBTT_memcpy
    #include <string.h>
    #define STBTT_memcpy       memcpy
    #define STBTT_memset       memset
#endif
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////
////   INTERFACE
////
////

#ifndef __STB_INCLUDE_STB_TRUETYPE_H__
#define __STB_INCLUDE_STB_TRUETYPE_H__

#ifdef STBTT_STATIC
    #define STBTT_DEF static
#else
    #define STBTT_DEF extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

// частная структура
typedef struct {
#ifdef STBTT_STREAM_TYPE
    STBTT_STREAM_TYPE data;
    stbtt_uint32 offset;
#else
    unsigned char * data;
#endif
    int cursor;
    int size;
} stbtt__buf;

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

//////////////////////////////////////////////////////////////////////////////
//
// TEXTURE BAKING API
//
// Если вы используете этот API , вам нужно будет вызвать только две функции.
//

typedef struct {
    unsigned short x0, y0, x1, y1; // координаты bbox в растровом изображении
    float xoff, yoff, xadvance;
} stbtt_bakedchar;

typedef struct {
    float x0, y0, s0, t0; // вверху слева
    float x1, y1, s1, t1; // внизу справа
} stbtt_aligned_quad;

STBTT_DEF void stbtt_GetBakedQuad(const stbtt_bakedchar * chardata, int pw, int ph, // те же данные, что и выше
                                  int char_index,             // символ для отображения
                                  float * xpos, float * ypos, // указатели на текущую позицию в пространстве пикселей экрана
                                  stbtt_aligned_quad * q,     // output: quad to draw
                                  int opengl_fillrule);       // true, если правило заполнения opengl; false, если DX9 или более ранняя версия
// Вызовите GetBakedQuad с char_index = 'character - first_char ', и он
// создает квадрат, который вам нужно нарисовать, и перемещает текущую позицию.
//
// Используемая система координат предполагает, что y увеличивается вниз.
//
// Символы будут расширяться как выше, так и ниже текущей позиции;
// см. обсуждение «BASELINE» выше.
//
// Это неэффективно; возможно, вы захотите его c&p и оптимизировать.

//////////////////////////////////////////////////////////////////////////////
//
// NEW TEXTURE BAKING API
//
// Это дает возможность упаковать несколько шрифтов в один атлас, а не
// отлично, но лучше, чем ничего.

typedef struct {
    unsigned short x0, y0, x1, y1; // координаты bbox в растровом изображении
    float xoff, yoff, xadvance;
    float xoff2, yoff2;
} stbtt_packedchar;

/// @cond
/**
 *  Сообщает Doxygen игнорировать дублирующее объявление.
 */
typedef struct stbtt_pack_context stbtt_pack_context;
typedef struct stbtt_fontinfo stbtt_fontinfo;
/// @endcond

#ifndef STB_RECT_PACK_VERSION
/// @cond
/**
 *  Сообщает Doxygen игнорировать дублирующее объявление.
 */
typedef struct stbrp_rect stbrp_rect;
/// @endcond

#endif
STBTT_DEF int  stbtt_PackBegin(stbtt_pack_context * spc, unsigned char * pixels, int width, int height,
                               int stride_in_bytes, int padding, void * alloc_context);

// Инициализирует контекст упаковки, хранящийся в переданном stbtt_pack_context .
// Будущие вызовы, использующие этот контекст, будут упаковывать символы в переданное растровое изображение.
// здесь: одноканальное растровое изображение шириной * высотой.  stride_in_bytes это
// расстояние от одного ряда до другого (или 0, чтобы означать, что они плотно упакованы).
// вместе). «отступ» — это количество отступов, которое нужно оставить между каждым
// символ (обычно вам нужно «1» для растровых изображений, которые вы будете использовать в качестве текстур с
// билинейная фильтрация).
//
// Возвращает 0 в случае неудачи, 1 в случае успеха.

STBTT_DEF void stbtt_PackEnd(stbtt_pack_context * spc);
// Очищает контекст упаковки и освобождает всю память.

#define STBTT_POINT_SIZE(x)   (-(x))

typedef struct {
    float font_size;
    int first_unicode_codepoint_in_range;  // если не ноль, то символы непрерывны, и это первая кодовая точка
    int * array_of_unicode_codepoints;      // если не ноль, то это массив кодовых точек Юникода
    int num_chars;
    stbtt_packedchar * chardata_for_range; // вывод
    unsigned char h_oversample, v_oversample; // не устанавливайте их, они используются внутри
} stbtt_pack_range;

STBTT_DEF void stbtt_PackSetOversampling(stbtt_pack_context * spc, unsigned int h_oversample,
                                         unsigned int v_oversample);
// Передискретизация шрифта повышает качество, позволяя использовать субпиксели более высокого качества.
// позиционирования и особенно ценен при небольших размерах текста.
//
// Эта функция устанавливает величину передискретизации для всех последующих вызовов.
// stbtt_PackFontRange(s) или stbtt_PackFontRangesGatherRects для данного
// контекст пакета. Значение по умолчанию (без передискретизации) достигается h_oversample =1.
// и v_oversample =1. Общее количество требуемых пикселей равно
// h_oversample * v_oversample больше, чем по умолчанию; например, 2х2
// передискретизация требует в 4 раза больше памяти, чем 1x1. Для достижения наилучших результатов выполните рендеринг
// текстуры с передискретизацией и билинейной фильтрацией. Посмотрите readme в
// stb/tests/oversample для получения информации о шрифтах с избыточной выборкой.
//
// Чтобы использовать PackFontRangesGather и т. д., вы должны установить его перед вызовами.
// вызов PackFontRangesGatherRects.

STBTT_DEF void stbtt_PackSetSkipMissingCodepoints(stbtt_pack_context * spc, int skip);
// Если Skip != 0, это говорит stb_truetype пропускать любые кодовые точки, для которых
// соответствующего глифа нет. Если пропуск=0 (это значение по умолчанию), то
// кодовые точки без глифа получили глиф «отсутствующий символ» шрифта,
// обычно это пустая коробка по соглашению.

STBTT_DEF void stbtt_GetPackedQuad(const stbtt_packedchar * chardata, int pw, int ph, // те же данные, что и выше
                                   int char_index,             // символ для отображения
                                   float * xpos, float * ypos, // указатели на текущую позицию в пространстве пикселей экрана
                                   stbtt_aligned_quad * q,     // output: quad to draw
                                   int align_to_integer);

STBTT_DEF int  stbtt_PackFontRangesGatherRects(stbtt_pack_context * spc, const stbtt_fontinfo * info,
                                               stbtt_pack_range * ranges, int num_ranges, stbrp_rect * rects);
STBTT_DEF void stbtt_PackFontRangesPackRects(stbtt_pack_context * spc, stbrp_rect * rects, int num_rects);
STBTT_DEF int  stbtt_PackFontRangesRenderIntoRects(stbtt_pack_context * spc, const stbtt_fontinfo * info,
                                                   stbtt_pack_range * ranges, int num_ranges, stbrp_rect * rects);
// Последовательный вызов этих функций примерно эквивалентен вызову
// stbtt_PackFontRanges (). Если у вас больше контроля над упаковкой нескольких
// шрифты или если вы хотите упаковать пользовательские данные в текстуру шрифта, взгляните
// в исходном коде stbtt_PackFontRanges() и создайте собственную версию
// используя эти функции, например. вызвать GatherRects несколько раз,
// создаем один массив прямоугольников, затем один раз вызываем PackRects,
// затем повторно вызовите RenderIntoRects. Это может привести к
// лучшая упаковка, чем многократный вызов PackFontRanges
// (а может и нет).

// это непрозрачная структура, с которой не стоит связываться, которая держится
// весь необходимый контекст от PackBegin до PackEnd.
struct stbtt_pack_context {
    void * user_allocator_context;
    void * pack_info;
    int   width;
    int   height;
    int   stride_in_bytes;
    int   padding;
    int   skip_missing;
    unsigned int   h_oversample, v_oversample;
    unsigned char * pixels;
    void * nodes;
};

//////////////////////////////////////////////////////////////////////////////
//
// FONT LOADING
//
//
#ifdef STBTT_STREAM_TYPE
STBTT_DEF int stbtt_GetNumberOfFonts(STBTT_STREAM_TYPE data);
#else
STBTT_DEF int stbtt_GetNumberOfFonts(const unsigned char * data);
#endif
// Эта функция определит количество шрифтов в файле шрифтов.  TrueType
// Файлы коллекции (.ttc) могут содержать несколько шрифтов, а шрифт TrueType
// Файлы (.ttf) содержат только один шрифт. Количество шрифтов можно использовать для
// индексация с помощью предыдущей функции, где индекс находится между нулем и единицей
// меньше, чем общее количество шрифтов. В случае возникновения ошибки возвращается -1.
#ifdef STBTT_STREAM_TYPE
STBTT_DEF int stbtt_GetFontOffsetForIndex(STBTT_STREAM_TYPE, int index);
#else
STBTT_DEF int stbtt_GetFontOffsetForIndex(const unsigned char * data, int index);
#endif

// Каждый файл .ttf/.ttc может содержать более одного шрифта. Каждый шрифт имеет последовательный
// индексный номер, начиная с 0. Вызовите эту функцию, чтобы получить смещение шрифта для
// заданный индекс; он возвращает -1, если индекс выходит за пределы диапазона. Обычный .ttf
// файл будет определять только один шрифт, и он всегда будет со смещением 0, поэтому он будет
// верните «0» для индекса 0 и -1 для всех остальных индексов.

// Следующая структура определена публично, поэтому вы можете объявить ее на
// стек или как глобальный и т. д., но вы должны относиться к нему как к непрозрачному.
struct stbtt_fontinfo {
    void * userdata;
#ifdef STBTT_STREAM_TYPE
    STBTT_STREAM_TYPE data;
#else
    unsigned char * data;             // указатель на файл .ttf
#endif
    int              fontstart;         // смещение начала шрифта

    int numGlyphs;                     // количество глифов, необходимое для проверки диапазона

    int loca, head, glyf, hhea, hmtx, kern, gpos, svg; // расположение таблиц как смещение от начала .ttf
    int index_map;                     // отображение cmap для выбранной нами кодировки символов
    int indexToLocFormat;              // формат, необходимый для сопоставления индекса глифа с глифом

    stbtt__buf cff;                    // данные шрифта cff
    stbtt__buf charstrings;            // индекс символьной строки
    stbtt__buf gsubrs;                 // Глобальный индекс функций символьной строки
    stbtt__buf subrs;                  // индекс подпрограмм частной символьной строки
    stbtt__buf fontdicts;              // массив диктовок шрифтов
    stbtt__buf fdselect;               // карта от глифа к шрифту
};
#ifdef STBTT_STREAM_TYPE
STBTT_DEF int stbtt_InitFont(stbtt_fontinfo * info, STBTT_STREAM_TYPE data, int offset);
#else
STBTT_DEF int stbtt_InitFont(stbtt_fontinfo * info, const unsigned char * data, int offset);
#endif
// Учитывая смещение в файле, определяющем шрифт, эта функция строит
// необходимая кэшированная информация для остальной части системы. Вы должны выделить
// stbtt_fontinfo самостоятельно, а stbtt_InitFont заполнит его. Вы не
// нужно сделать что-то особенное, чтобы освободить его, потому что содержимое чистое
// значения данных без каких-либо дополнительных структур данных. Возвращает 0 в случае неудачи.

//////////////////////////////////////////////////////////////////////////////
//
// CHARACTER TO GLYPH - INDEX ПРЕОБРАЗОВАНИЕ

STBTT_DEF int stbtt_FindGlyphIndex(const stbtt_fontinfo * info, int unicode_codepoint);
// Если вы собираетесь выполнить несколько операций над одним и тем же персонажем
// и вам нужно ускорение, вызовите эту функцию с персонажем, которым вы являетесь
// собираетесь обрабатывать, а затем используйте функции на основе глифов вместо
// функции на основе кодовых точек.
// Возвращает 0, если код символа не определен в шрифте.

//////////////////////////////////////////////////////////////////////////////
//
// CHARACTER PROPERTIES
//

STBTT_DEF float stbtt_ScaleForPixelHeight(const stbtt_fontinfo * info, float pixels);
// вычисляет коэффициент масштабирования для создания шрифта, «высота» которого равна высоте «пикселей».
// Высота измеряется как расстояние от самого высокого человека до самого нижнего.
// спусковое устройство; другими словами, это эквивалентно вызову stbtt_GetFontVMetrics
// и вычисления:
//       scale = pixels / (ascent - descent)
// поэтому, если вы предпочитаете измерять высоту только по подъему, используйте аналогичный расчет.

STBTT_DEF float stbtt_ScaleForMappingEmToPixels(const stbtt_fontinfo * info, float pixels);
// вычисляет коэффициент масштабирования для создания шрифта, размер EM которого сопоставлен с
// высота пикселей. Вероятно, это то, что вычисляют традиционные API, но
// Я не уверен.

STBTT_DEF void stbtt_GetFontVMetrics(const stbtt_fontinfo * info, int * ascent, int * descent, int * lineGap);
// восхождение — это координата над базовой линией, на которую простирается шрифт; спуск
// — это координата ниже базовой линии, на которую расширяется шрифт (т. е. обычно она отрицательна)
// lineGap — это расстояние между спуском одной строки и подъемом следующей строки...
// поэтому вам следует увеличить вертикальное положение на "*ascent - *descent + *lineGap"
//   они выражены в немасштабированных координатах, поэтому вам необходимо умножить на
//   масштабный коэффициент для данного размера

STBTT_DEF int  stbtt_GetFontVMetricsOS2(const stbtt_fontinfo * info, int * typoAscent, int * typoDescent,
                                        int * typoLineGap);
// аналогичен GetFontVMetrics, но возвращает «типографские» значения из OS/2.
// таблица (специфична для файлов MS/Windows TTF).
//
// Возвращает 1 в случае успеха (таблица присутствует), 0 в случае неудачи.

STBTT_DEF void stbtt_GetFontBoundingBox(const stbtt_fontinfo * info, int * x0, int * y0, int * x1, int * y1);
// ограничивающая рамка вокруг всех возможных символов

STBTT_DEF void stbtt_GetCodepointHMetrics(const stbtt_fontinfo * info, int codepoint, int * advanceWidth,
                                          int * leftSideBearing);
// leftSideBearing — это смещение от текущего горизонтального положения до левого края символа.
// заранееWidth — это смещение от текущей горизонтальной позиции до следующей горизонтальной позиции.
//   они выражены в немасштабированных координатах

STBTT_DEF int  stbtt_GetCodepointKernAdvance(const stbtt_fontinfo * info, int ch1, int ch2);
// дополнительная сумма для добавления к значению «аванс» между каналом 1 и каналом 2

STBTT_DEF int stbtt_GetCodepointBox(const stbtt_fontinfo * info, int codepoint, int * x0, int * y0, int * x1, int * y1);
// Получает ограничивающую рамку видимой части глифа в немасштабированных координатах.

STBTT_DEF void stbtt_GetGlyphHMetrics(const stbtt_fontinfo * info, int glyph_index, int * advanceWidth,
                                      int * leftSideBearing);
STBTT_DEF int  stbtt_GetGlyphKernAdvance(const stbtt_fontinfo * info, int glyph1, int glyph2);
STBTT_DEF int  stbtt_GetGlyphBox(const stbtt_fontinfo * info, int glyph_index, int * x0, int * y0, int * x1, int * y1);
// как указано выше, но для большей эффективности принимает один или несколько индексов глифа.

typedef struct _stbtt_kerningentry {
    int glyph1; // используйте stbtt_FindGlyphIndex
    int glyph2;
    int advance;
} stbtt_kerningentry;

STBTT_DEF int  stbtt_GetKerningTableLength(const stbtt_fontinfo * info);
STBTT_DEF int  stbtt_GetKerningTable(const stbtt_fontinfo * info, stbtt_kerningentry * table, int table_length);
// Получает полный список всех пар кернинга, предоставленных шрифтом.
// stbtt_GetKerningTable никогда не записывает больше записей, чем table_length, и возвращает количество записанных записей.
// The table will be sorted by (a.glyph1 == b.glyph1)?(a.glyph2 < b.glyph2):(a.glyph1 < b.glyph1)

//////////////////////////////////////////////////////////////////////////////
//
// GLYPH SHAPES (вероятно, они вам не нужны, но их нужно убрать раньше)
// растровые изображения по причинам порядка объявления C)
//

#ifndef STBTT_vmove // you can predefine these to use different values (but why?)
enum {
    STBTT_vmove = 1,
    STBTT_vline,
    STBTT_vcurve,
    STBTT_vcubic
};
#endif

#ifndef stbtt_vertex // вы можете предопределить это, чтобы использовать разные значения
// (мы разделяем это с другим кодом в RAD)
#define stbtt_vertex_type short // не могу использовать stbtt_int16, потому что он не отображается в заголовочном файле
typedef struct {
    stbtt_vertex_type x, y, cx, cy, cx1, cy1;
    unsigned char type, padding;
} stbtt_vertex;
#endif

STBTT_DEF int stbtt_IsGlyphEmpty(const stbtt_fontinfo * info, int glyph_index);
// возвращает ненулевое значение, если для этого глифа ничего не нарисовано

STBTT_DEF int stbtt_GetCodepointShape(const stbtt_fontinfo * info, int unicode_codepoint, stbtt_vertex ** vertices);
STBTT_DEF int stbtt_GetGlyphShape(const stbtt_fontinfo * info, int glyph_index, stbtt_vertex ** vertices);
// возвращает количество вершин и заполняет *vertices указателем на них
//   они выражены в «немасштабированных» координатах
//
// Форма представляет собой серию контуров. Каждый из них начинается с
// STBTT_moveto, то состоит из серии смешанных
// Сегменты STBTT_lineto и STBTT_curveto. линиято
// рисует линию от предыдущей конечной точки до ее x,y; кривая
// рисует квадратичную кривую Безье от предыдущей конечной точки до
// это x,y, используя cx,cy в качестве контрольной точки Безье.

STBTT_DEF void stbtt_FreeShape(const stbtt_fontinfo * info, stbtt_vertex * vertices);
// освобождает данные, выделенные выше

STBTT_DEF stbtt_uint32 stbtt_FindSVGDoc(const stbtt_fontinfo * info, int gl);
STBTT_DEF int stbtt_GetCodepointSVG(const stbtt_fontinfo * info, int unicode_codepoint, stbtt_uint32 * svgOfs);
STBTT_DEF int stbtt_GetGlyphSVG(const stbtt_fontinfo * info, int gl, stbtt_uint32 * svgOfs);
// заполняет svg данными SVG персонажа.
// возвращает размер данных или 0, если SVG не найден.

//////////////////////////////////////////////////////////////////////////////
//
// BITMAP RENDERING
//

STBTT_DEF void stbtt_FreeBitmap(unsigned char * bitmap, void * userdata);
// освобождает растровое изображение, расположенное ниже

STBTT_DEF unsigned char * stbtt_GetCodepointBitmap(const stbtt_fontinfo * info, float scale_x, float scale_y,
                                                   int codepoint, int * width, int * height, int * xoff, int * yoff);
// выделяет достаточно большое одноканальное растровое изображение с разрешением 8 бит на пиксель и визуализирует
// указанный символ/глиф в указанном масштабе, с
// antialiasing. 0 is no coverage (transparent), 255 is fully covered (opaque).
// *ширина и *высота заполняются шириной и высотой растрового изображения,
// который хранится слева направо и сверху вниз.
//
// xoff/yoff — это смещение в пикселях от начала координат глифа до верхнего левого угла растрового изображения.

STBTT_DEF unsigned char * stbtt_GetCodepointBitmapSubpixel(const stbtt_fontinfo * info, float scale_x, float scale_y,
                                                           float shift_x, float shift_y, int codepoint, int * width, int * height, int * xoff, int * yoff);
// то же, что stbtt_GetCodepointBitmap, но вы можете указать субпиксель
// сдвиг для персонажа

STBTT_DEF void stbtt_MakeCodepointBitmap(const stbtt_fontinfo * info, unsigned char * output, int out_w, int out_h,
                                         int out_stride, float scale_x, float scale_y, int codepoint);
// то же, что stbtt_GetCodepointBitmap, но вы передаете память для растрового изображения
// в виде «вывода» с интервалом между строками «out_stride» байт. растровое изображение
// обрезается до байтов out_w/out_h. Позвоните stbtt_GetCodepointBitmapBox, чтобы получить
// ширина и высота, а также информация о их расположении в первую очередь.

STBTT_DEF void stbtt_MakeCodepointBitmapSubpixel(const stbtt_fontinfo * info, unsigned char * output, int out_w,
                                                 int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int codepoint);
// то же, что stbtt_MakeCodepointBitmap, но вы можете указать субпиксель
// сдвиг для персонажа

STBTT_DEF void stbtt_MakeCodepointBitmapSubpixelPrefilter(const stbtt_fontinfo * info, unsigned char * output,
                                                          int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int oversample_x,
                                                          int oversample_y, float * sub_x, float * sub_y, int codepoint);
// то же, что stbtt_MakeCodepointBitmapSubpixel, но с предварительной фильтрацией
// выполняется (см. stbtt_PackSetOversampling )

STBTT_DEF void stbtt_GetCodepointBitmapBox(const stbtt_fontinfo * font, int codepoint, float scale_x, float scale_y,
                                           int * ix0, int * iy0, int * ix1, int * iy1);
// получить bbox растрового изображения, центрированный вокруг начала координат глифа; так что
// ширина растрового изображения — ix1-ix0, высота — iy1-iy0 и место для размещения.
// растровое изображение вверху слева — (leftSideBearing*scale,iy0).
// (Обратите внимание, что растровое изображение использует y-increases-down, а фигура использует
// y-увеличивается вверх, поэтому CodepointBitmapBox и CodepointBox инвертируются.)

STBTT_DEF void stbtt_GetCodepointBitmapBoxSubpixel(const stbtt_fontinfo * font, int codepoint, float scale_x,
                                                   float scale_y, float shift_x, float shift_y, int * ix0, int * iy0, int * ix1, int * iy1);
// то же, что stbtt_GetCodepointBitmapBox, но вы можете указать субпиксель
// сдвиг для персонажа

// следующие функции эквивалентны вышеуказанным функциям, но работают
// на индексах глифов вместо кодовых точек Unicode (для эффективности)
STBTT_DEF unsigned char * stbtt_GetGlyphBitmap(const stbtt_fontinfo * info, float scale_x, float scale_y, int glyph,
                                               int * width, int * height, int * xoff, int * yoff);
STBTT_DEF unsigned char * stbtt_GetGlyphBitmapSubpixel(const stbtt_fontinfo * info, float scale_x, float scale_y,
                                                       float shift_x, float shift_y, int glyph, int * width, int * height, int * xoff, int * yoff);
STBTT_DEF void stbtt_MakeGlyphBitmap(const stbtt_fontinfo * info, unsigned char * output, int out_w, int out_h,
                                     int out_stride, float scale_x, float scale_y, int glyph);
STBTT_DEF void stbtt_MakeGlyphBitmapSubpixel(const stbtt_fontinfo * info, unsigned char * output, int out_w, int out_h,
                                             int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int glyph);
STBTT_DEF void stbtt_MakeGlyphBitmapSubpixelPrefilter(const stbtt_fontinfo * info, unsigned char * output, int out_w,
                                                      int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int oversample_x,
                                                      int oversample_y, float * sub_x, float * sub_y, int glyph);
STBTT_DEF void stbtt_GetGlyphBitmapBox(const stbtt_fontinfo * font, int glyph, float scale_x, float scale_y, int * ix0,
                                       int * iy0, int * ix1, int * iy1);
STBTT_DEF void stbtt_GetGlyphBitmapBoxSubpixel(const stbtt_fontinfo * font, int glyph, float scale_x, float scale_y,
                                               float shift_x, float shift_y, int * ix0, int * iy0, int * ix1, int * iy1);

// @TODO: don't expose this structure
typedef struct {
    int w, h, stride;
    unsigned char * pixels;
} stbtt__bitmap;

// растрировать фигуру с квадратичным Безье в растровое изображение
STBTT_DEF void stbtt_Rasterize(stbtt__bitmap * result,       // 1-канальное растровое изображение для рисования
                               float flatness_in_pixels,     // допустимая погрешность кривой в пикселях
                               stbtt_vertex * vertices,      // массив вершин, определяющих форму
                               int num_verts,                // количество вершин в массиве выше
                               float scale_x, float scale_y, // масштаб, примененный к входным вершинам
                               float shift_x, float shift_y, // перевод, примененный к входным вершинам
                               int x_off, int y_off,         // к вводу применен другой перевод
                               int invert,                   // если не ноль, перевернуть фигуру по вертикали
                               void * userdata);             // контекст для STBTT_MALLOC

//////////////////////////////////////////////////////////////////////////////
//
// Отрисовка функции знакового расстояния (или поля)

STBTT_DEF void stbtt_FreeSDF(unsigned char * bitmap, void * userdata);
// освобождает растровое изображение SDF, расположенное ниже

STBTT_DEF unsigned char * stbtt_GetGlyphSDF(const stbtt_fontinfo * info, float scale, int glyph, int padding,
                                            unsigned char onedge_value, float pixel_dist_scale, int * width, int * height, int * xoff, int * yoff);
STBTT_DEF unsigned char * stbtt_GetCodepointSDF(const stbtt_fontinfo * info, float scale, int codepoint, int padding,
                                                unsigned char onedge_value, float pixel_dist_scale, int * width, int * height, int * xoff, int * yoff);
// Эти функции вычисляют дискретизированное поле SDF для одного символа, подходящее для хранения
// в одноканальной текстуре, выборка с билинейной фильтрацией и тестирование на соответствие
// больше некоторого порога для создания масштабируемых шрифтов.
//        информация - шрифт
//        масштаб — управляет размером результирующего растрового изображения SDF так же, как при создании обычного растрового изображения.
//        глиф/кодовая точка — символ для генерации SDF для
//        отступы — дополнительные «пиксели» вокруг символа, которые заполняются расстоянием до символа (не 0),
//                                 который позволяет использовать такие эффекты, как битовые контуры
//        onedge_value — значение 0–255 для проверки SDF для восстановления персонажа (т. е. изоконтура персонажа)
//        pixel_dist_scale - на какое значение должен увеличиться SDF при перемещении SDF на один "пиксель" от края (по шкале 0..255)
//                                 если положительный, > onedge_value находится внутри; если отрицательный, <onedge_value находится внутри
//        width,height — выходная высота и ширина растрового изображения SDF (включая отступы)
//        xoff,yoff — начало координат символа
//        возвращаемое значение — двумерный массив байтов 0..255, ширина*высота по размеру.
//
// pixel_dist_scale и onedge_value — это масштаб и смещение, которые позволяют вам делать
// оптимальное использование ограниченного диапазона 0..255 для вашего приложения, компромисс между точностью
// и спецэффекты.  Значения SDF вне диапазона 0..255 фиксируются до 0..255.
//
// Пример:
//      scale = stbtt_ScaleForPixelHeight(22)
//      отступ = 5
//      onedge_value = 180
//      pixel_dist_scale = 180/5,0 = 36,0
//
//      Это создаст растровое изображение SDF, в котором размер символа составляет около 22 пикселей.
//      высокий, но все растровое изображение имеет высоту около 22+5+5=32 пикселей. Для изготовления заполненного
//      форму, выберите SDF в каждом пикселе и заполните пиксель, если значение SDF
//      больше или равно 180/255. (На самом деле вам понадобится сглаживание,
//      что выходит за рамки этого примера.) Кроме того, вы можете вычислить
//      смещение контуров (например, для обводки границы символа внутри и снаружи,
//      или только снаружи). Например, для заполнения вне символа до 3 SDF
//      пикселей, вы можете сравнить их с (180-36,0*3)/255 = 72/255. Вышеупомянутое
//      выбор переменных отображает диапазон от 5 пикселей за пределами формы до
//      2 пикселя внутри фигуры до 0..255; это предназначено в первую очередь для применения
//      только внешние эффекты (внутренний диапазон необходим для обеспечения правильного
//      сглаживание шрифта *меньших* размеров)
//
// Функция вычисляет SDF аналитически для каждого пикселя SDF, а не, например,
// создание растрового изображения более высокого разрешения и его аппроксимация. Теоретически качество
// должно быть как можно выше для SDF такого размера и представления, но
// неясно, верно ли это на практике (возможно, создание растрового изображения с более высоким разрешением
// и вычисления на основе этого могут позволить предотвратить отсев).
//
// Алгоритм вообще не оптимизирован, поэтому ожидайте, что он будет медленным.
// при вычислении большого количества символов или очень больших размеров.

//////////////////////////////////////////////////////////////////////////////
//
// Ищем правильный шрифт...
//
// Вам действительно следует просто решить эту проблему в автономном режиме, сохранить свои собственные таблицы.
// что это за шрифт, и не пытайтесь получить это из файла .ttf.
// Это потому, что извлечь его из файла .ttf очень сложно, потому что
// имена в файле могут встречаться во многих возможных кодировках, во многих
// возможные языки и, например, если вам нужно сравнение без учета регистра,
// детали этого сложным образом зависят от кодировки и языка.
// (на самом деле в истинном типе не указан, но тоже гигантский).
//
// Но использовать предоставленные функции можно двумя возможными способами:
//     stbtt_FindMatchingFont () будет использовать сравнения *с учетом регистра*
//             имена в кодировке Unicode, чтобы попытаться найти нужный шрифт;
//             вы можете запустить это перед вызовом stbtt_InitFont()
//
//     stbtt_GetFontNameString() позволяет получить любую из различных строк
//             из файла самостоятельно и сравните их самостоятельно.
//             Сначала вам нужно было вызвать stbtt_InitFont().

#ifdef STBTT_STREAM_TYPE
STBTT_DEF int stbtt_FindMatchingFont(STBTT_STREAM_TYPE fontdata, const char * name, int flags);
#else
STBTT_DEF int stbtt_FindMatchingFont(const unsigned char * fontdata, const char * name, int flags);
#endif
// возвращает смещение (не индекс) соответствующего шрифта или -1, если его нет
//   если вы используете STBTT_MACSTYLE_DONTCARE, используйте имя шрифта, например «Arial Bold».
//   если вы используете любой другой флаг, используйте имя шрифта, например «Arial»; это проверяет
//     поле заголовка «macStyle»; я не знаю, устанавливают ли шрифты это последовательно
#define STBTT_MACSTYLE_DONTCARE     0
#define STBTT_MACSTYLE_BOLD         1
#define STBTT_MACSTYLE_ITALIC       2
#define STBTT_MACSTYLE_UNDERSCORE   4
#define STBTT_MACSTYLE_NONE         8   // <= не то же самое, что 0, это заставляет нас проверять, что битовое поле равно 0

#ifdef STBTT_STREAM_TYPE
STBTT_DEF int stbtt_CompareUTF8toUTF16_bigendian(const char * s1, int len1, STBTT_STREAM_TYPE s2, stbtt_uint32 s2offs,
                                                 int len2);
#else
STBTT_DEF int stbtt_CompareUTF8toUTF16_bigendian(const char * s1, int len1, const char * s2, stbtt_uint32 s2offs,
                                                 int len2);
#endif
// возвращает 1/0, идентична ли первая строка, интерпретируемая как utf8, идентична
// вторая строка интерпретируется как utf16 с прямым порядком байтов... полезна для строк из следующей функции

STBTT_DEF stbtt_uint32 stbtt_GetFontNameString(const stbtt_fontinfo * font, int * length, int platformID,
                                               int encodingID, int languageID, int nameID);

// возвращает строку (которая может быть двухбайтовой с обратным порядком байтов, например, для Юникода)
// и помещает длину в байтах в *length.
//
// некоторые значения идентификаторов приведены ниже; подробнее см. спецификацию truetype:
//     http://developer.apple.com/textfonts/TTRefMan/RM06/Chap6name.html
//     http://www.microsoft.com/typography/otspec/name.htm

enum { // идентификатор платформы
    STBTT_PLATFORM_ID_UNICODE = 0,
    STBTT_PLATFORM_ID_MAC = 1,
    STBTT_PLATFORM_ID_ISO = 2,
    STBTT_PLATFORM_ID_MICROSOFT = 3
};

enum { // кодированияID для STBTT_PLATFORM_ID_UNICODE
    STBTT_UNICODE_EID_UNICODE_1_0 = 0,
    STBTT_UNICODE_EID_UNICODE_1_1 = 1,
    STBTT_UNICODE_EID_ISO_10646 = 2,
    STBTT_UNICODE_EID_UNICODE_2_0_BMP = 3,
    STBTT_UNICODE_EID_UNICODE_2_0_FULL = 4
};

enum { // кодированияID для STBTT_PLATFORM_ID_MICROSOFT
    STBTT_MS_EID_SYMBOL = 0,
    STBTT_MS_EID_UNICODE_BMP = 1,
    STBTT_MS_EID_SHIFTJIS = 2,
    STBTT_MS_EID_UNICODE_FULL = 10
};

enum { // идентификатор кодировки для STBTT_PLATFORM_ID_MAC ; то же, что и коды диспетчера сценариев
    STBTT_MAC_EID_ROMAN = 0, STBTT_MAC_EID_ARABIC = 4,
    STBTT_MAC_EID_JAPANESE = 1, STBTT_MAC_EID_HEBREW = 5,
    STBTT_MAC_EID_CHINESE_TRAD = 2, STBTT_MAC_EID_GREEK = 6,
    STBTT_MAC_EID_KOREAN = 3, STBTT_MAC_EID_RUSSIAN = 7
};

enum { // идентификатор языка для STBTT_PLATFORM_ID_MICROSOFT ; то же, что LCID...
    // проблематично, потому что есть, например. 16 LCID на английском языке и 16 LCID на арабском языке
    STBTT_MS_LANG_ENGLISH = 0x0409, STBTT_MS_LANG_ITALIAN = 0x0410,
    STBTT_MS_LANG_CHINESE = 0x0804, STBTT_MS_LANG_JAPANESE = 0x0411,
    STBTT_MS_LANG_DUTCH = 0x0413, STBTT_MS_LANG_KOREAN = 0x0412,
    STBTT_MS_LANG_FRENCH = 0x040c, STBTT_MS_LANG_RUSSIAN = 0x0419,
    STBTT_MS_LANG_GERMAN = 0x0407, STBTT_MS_LANG_SPANISH = 0x0409,
    STBTT_MS_LANG_HEBREW = 0x040d, STBTT_MS_LANG_SWEDISH = 0x041D
};

enum { // идентификатор языка для STBTT_PLATFORM_ID_MAC
    STBTT_MAC_LANG_ENGLISH = 0, STBTT_MAC_LANG_JAPANESE = 11,
    STBTT_MAC_LANG_ARABIC = 12, STBTT_MAC_LANG_KOREAN = 23,
    STBTT_MAC_LANG_DUTCH = 4, STBTT_MAC_LANG_RUSSIAN = 32,
    STBTT_MAC_LANG_FRENCH = 1, STBTT_MAC_LANG_SPANISH = 6,
    STBTT_MAC_LANG_GERMAN = 2, STBTT_MAC_LANG_SWEDISH = 5,
    STBTT_MAC_LANG_HEBREW = 10, STBTT_MAC_LANG_CHINESE_SIMPLIFIED = 33,
    STBTT_MAC_LANG_ITALIAN = 3, STBTT_MAC_LANG_CHINESE_TRAD = 19
};

#ifdef __cplusplus
}
#endif

#endif // __STB_INCLUDE_STB_TRUETYPE_H__

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////
////   IMPLEMENTATION
////
////

#ifdef STB_TRUETYPE_IMPLEMENTATION

#ifndef STBTT_MAX_OVERSAMPLE
    #define STBTT_MAX_OVERSAMPLE   8
#endif

#if STBTT_MAX_OVERSAMPLE > 255
    #error "STBTT_MAX_OVERSAMPLE cannot be > 255"
#endif

typedef int stbtt__test_oversample_pow2[(STBTT_MAX_OVERSAMPLE & (STBTT_MAX_OVERSAMPLE - 1)) == 0 ? 1 : -1];

#ifndef STBTT_RASTERIZER_VERSION
    #define STBTT_RASTERIZER_VERSION 2
#endif

#ifdef _MSC_VER
    #define STBTT__NOTUSED(v)  (void)(v)
#else
    #define STBTT__NOTUSED(v)  (void)sizeof(v)
#endif

//////////////////////////////////////////////////////////////////////////
//
// Помощники stbtt__buf для анализа данных из файла
//

static stbtt_uint8 stbtt__buf_get8(stbtt__buf * b)
{
    if(b->cursor >= b->size)
        return 0;
#ifdef STBTT_STREAM_TYPE
    long pos = (long)(b->cursor + b->offset);
    STBTT_STREAM_SEEK(b->data, pos);
    stbtt_uint8 result;
    STBTT_STREAM_READ(b->data, &result, 1);
    ++b->cursor;
    return result;
#else
    return b->data[b->cursor++];
#endif

}

static stbtt_uint8 stbtt__buf_peek8(stbtt__buf * b)
{
    if(b->cursor >= b->size)
        return 0;
#ifdef STBTT_STREAM_TYPE
    long pos = (long)(b->cursor + b->offset);
    STBTT_STREAM_SEEK(b->data, pos);
    stbtt_uint8 result;
    STBTT_STREAM_READ(b->data, &result, 1);
    return result;
#else
    return b->data[b->cursor];
#endif

}

static void stbtt__buf_seek(stbtt__buf * b, int o)
{
    STBTT_assert(!(o > b->size || o < 0));
    b->cursor = (o > b->size || o < 0) ? b->size : o;
}

static void stbtt__buf_skip(stbtt__buf * b, int o)
{
    stbtt__buf_seek(b, b->cursor + o);
}

static stbtt_uint32 stbtt__buf_get(stbtt__buf * b, int n)
{
    stbtt_uint32 v = 0;
    int i;
    STBTT_assert(n >= 1 && n <= 4);
    for(i = 0; i < n; i++)
        v = (v << 8) | stbtt__buf_get8(b);
    return v;
}
#ifdef STBTT_STREAM_TYPE
    static stbtt__buf stbtt__new_buf(STBTT_STREAM_TYPE s, size_t size)
#else
    static stbtt__buf stbtt__new_buf(const void * p, size_t size)
#endif
{
    stbtt__buf r;
    STBTT_assert(size < 0x40000000);
#ifdef STBTT_STREAM_TYPE
    r.data = s;
    r.offset = 0;
#else
    r.data = (stbtt_uint8 *)p;
#endif
    r.size = (int)size;
    r.cursor = 0;
    return r;
}

#define stbtt__buf_get16(b)  stbtt__buf_get((b), 2)
#define stbtt__buf_get32(b)  stbtt__buf_get((b), 4)

static stbtt__buf stbtt__buf_range(const stbtt__buf * b, int o, int s)
{
    stbtt__buf r = stbtt__new_buf(NULL, 0);
    if(o < 0 || s < 0 || o > b->size || s > b->size - o) return r;
#ifdef STBTT_STREAM_TYPE
    r.data = b->data;
    r.offset = b->offset + o;
#else
    r.data = b->data + o;
#endif
    r.size = s;
    return r;
}

static stbtt__buf stbtt__cff_get_index(stbtt__buf * b)
{
    int count, start, offsize;
    start = b->cursor;
    count = stbtt__buf_get16(b);
    if(count) {
        offsize = stbtt__buf_get8(b);
        STBTT_assert(offsize >= 1 && offsize <= 4);
        stbtt__buf_skip(b, offsize * count);
        stbtt__buf_skip(b, stbtt__buf_get(b, offsize) - 1);
    }
    return stbtt__buf_range(b, start, b->cursor - start);
}

static stbtt_uint32 stbtt__cff_int(stbtt__buf * b)
{
    int b0 = stbtt__buf_get8(b);
    if(b0 >= 32 && b0 <= 246)       return b0 - 139;
    else if(b0 >= 247 && b0 <= 250) return (b0 - 247) * 256 + stbtt__buf_get8(b) + 108;
    else if(b0 >= 251 && b0 <= 254) return -(b0 - 251) * 256 - stbtt__buf_get8(b) - 108;
    else if(b0 == 28)               return stbtt__buf_get16(b);
    else if(b0 == 29)               return stbtt__buf_get32(b);
    STBTT_assert(0);
    return 0;
}

static void stbtt__cff_skip_operand(stbtt__buf * b)
{
    int v, b0 = stbtt__buf_peek8(b);
    STBTT_assert(b0 >= 28);
    if(b0 == 30) {
        stbtt__buf_skip(b, 1);
        while(b->cursor < b->size) {
            v = stbtt__buf_get8(b);
            if((v & 0xF) == 0xF || (v >> 4) == 0xF)
                break;
        }
    }
    else {
        stbtt__cff_int(b);
    }
}

static stbtt__buf stbtt__dict_get(stbtt__buf * b, int key)
{
    stbtt__buf_seek(b, 0);
    while(b->cursor < b->size) {
        int start = b->cursor, end, op;
        while(stbtt__buf_peek8(b) >= 28)
            stbtt__cff_skip_operand(b);
        end = b->cursor;
        op = stbtt__buf_get8(b);
        if(op == 12)  op = stbtt__buf_get8(b) | 0x100;
        if(op == key) return stbtt__buf_range(b, start, end - start);
    }
    return stbtt__buf_range(b, 0, 0);
}

static void stbtt__dict_get_ints(stbtt__buf * b, int key, int outcount, stbtt_uint32 * out)
{
    int i;
    stbtt__buf operands = stbtt__dict_get(b, key);
    for(i = 0; i < outcount && operands.cursor < operands.size; i++)
        out[i] = stbtt__cff_int(&operands);
}

static int stbtt__cff_index_count(stbtt__buf * b)
{
    stbtt__buf_seek(b, 0);
    return stbtt__buf_get16(b);
}

static stbtt__buf stbtt__cff_index_get(stbtt__buf b, int i)
{
    int count, offsize, start, end;
    stbtt__buf_seek(&b, 0);
    count = stbtt__buf_get16(&b);
    offsize = stbtt__buf_get8(&b);
    STBTT_assert(i >= 0 && i < count);
    STBTT_assert(offsize >= 1 && offsize <= 4);
    stbtt__buf_skip(&b, i * offsize);
    start = stbtt__buf_get(&b, offsize);
    end = stbtt__buf_get(&b, offsize);
    return stbtt__buf_range(&b, 2 + (count + 1) * offsize + start, end - start);
}

//////////////////////////////////////////////////////////////////////////
//
// средства доступа для анализа данных из файла
//

// на платформах, которые не допускают невыровненного чтения, если мы хотим разрешить
// шрифты truetype, которые не дополняются для выравнивания, определяют ALLOW_UNALIGNED_TRUETYPE

#ifdef STBTT_STREAM_TYPE
static stbtt_uint8 ttBYTE(STBTT_STREAM_TYPE s, stbtt_uint32 offset)
{
    STBTT_STREAM_SEEK(s, offset);
    stbtt_uint8 r;
    STBTT_STREAM_READ(s, &r, 1);
    return r;
}
#define ttCHAR(s, offset)     ((stbtt_int8)ttBYTE(s,offset))
static stbtt_uint16 ttUSHORT(STBTT_STREAM_TYPE s, stbtt_uint32 offset)
{
    STBTT_STREAM_SEEK(s, offset);
    stbtt_uint8 r[2];
    STBTT_STREAM_READ(s, &r, 2);
    return r[0] * 256 + r[1];
}
static stbtt_int16 ttSHORT(STBTT_STREAM_TYPE s, stbtt_uint32 offset)
{
    STBTT_STREAM_SEEK(s, offset);
    stbtt_uint8 r[2];
    STBTT_STREAM_READ(s, &r, 2);
    return r[0] * 256 + r[1];
}
static stbtt_uint32 ttULONG(STBTT_STREAM_TYPE s, stbtt_uint32 offset)
{
    STBTT_STREAM_SEEK(s, offset);
    stbtt_uint8 r[4];
    STBTT_STREAM_READ(s, &r, 4);
    return (r[0] << 24) + (r[1] << 16) + (r[2] << 8) + r[3];
}
static stbtt_int32 ttLONG(STBTT_STREAM_TYPE s, stbtt_uint32 offset)
{
    STBTT_STREAM_SEEK(s, offset);
    stbtt_uint8 r[4];
    STBTT_STREAM_READ(s, &r, 4);
    return (r[0] << 24) + (r[1] << 16) + (r[2] << 8) + r[3];
}
#else
#define ttBYTE(p, offset)     (* (stbtt_uint8 *) (p+offset))
#define ttCHAR(p, offset)     (* (stbtt_int8 *) (p+offset))
static stbtt_uint16 ttUSHORT(const stbtt_uint8 * p, stbtt_uint32 offset)
{
    return p[offset + 0] * 256 + p[offset + 1];
}
static stbtt_int16 ttSHORT(const stbtt_uint8 * p, stbtt_uint32 offset)
{
    return p[offset + 0] * 256 + p[offset + 1];
}
static stbtt_uint32 ttULONG(const stbtt_uint8 * p, stbtt_uint32 offset)
{
    return (p[offset + 0] << 24) + (p[offset + 1] << 16) + (p[offset + 2] << 8) + p[offset + 3];
}
static stbtt_int32 ttLONG(const stbtt_uint8 * p, stbtt_uint32 offset)
{
    return (p[offset + 0] << 24) + (p[offset + 1] << 16) + (p[offset + 2] << 8) + p[offset + 3];
}
#endif
#define ttFixed(p, offset)    ttLONG(p, offset)

#define stbtt_tag4(p,c0,c1,c2,c3) ((p)[0] == (c0) && (p)[1] == (c1) && (p)[2] == (c2) && (p)[3] == (c3))
#define stbtt_tag(p,str)           stbtt_tag4(p,str[0],str[1],str[2],str[3])
#ifdef STBTT_STREAM_TYPE
    static int stbtt__isfont(STBTT_STREAM_TYPE stream, stbtt_uint32 offs)
#else
    static int stbtt__isfont(stbtt_uint8 * font, stbtt_uint32 offs)
#endif
{
#ifdef STBTT_STREAM_TYPE
    stbtt_uint8 font[4];
    STBTT_STREAM_SEEK(stream, offs);
    STBTT_STREAM_READ(stream, font, 4);
#else
    font += offs;
#endif
    // проверьте номер версии
    if(stbtt_tag4(font, '1', 0, 0, 0))  return 1;  // TrueType 1
    if(stbtt_tag(font, "typ1"))   return 1;  // TrueType со шрифтом типа 1 — мы не поддерживаем это!
    if(stbtt_tag(font, "OTTO"))   return 1;  // OpenType с CFF
    if(stbtt_tag4(font, 0, 1, 0, 0)) return 1;  // ОпенТайп 1.0
    if(stbtt_tag(font, "true"))   return 1;  // Спецификация Apple для шрифтов TrueType
    return 0;
}

// @OPTIMIZE : бинарный поиск
#ifdef STBTT_STREAM_TYPE
    static stbtt_uint32 stbtt__find_table(STBTT_STREAM_TYPE data, stbtt_uint32 fontstart, const char * tag)
#else
    static stbtt_uint32 stbtt__find_table(stbtt_uint8 * data, stbtt_uint32 fontstart, const char * tag)
#endif
{
    stbtt_int32 num_tables = ttUSHORT(data, fontstart + 4);
    stbtt_uint32 tabledir = fontstart + 12;
    stbtt_int32 i;
    for(i = 0; i < num_tables; ++i) {
        stbtt_uint32 loc = tabledir + 16 * i;
#ifdef STBTT_STREAM_TYPE
        stbtt_uint8 buf[4];
        STBTT_STREAM_SEEK(data, loc + 0);
        STBTT_STREAM_READ(data, buf, 4);
        if(stbtt_tag(buf, tag))
            return ttULONG(data, loc + 8);
#else
        if(stbtt_tag(data + loc + 0, tag))
            return ttULONG(data, loc + 8);
#endif
    }
    return 0;
}
#ifdef STBTT_STREAM_TYPE
    static int stbtt_GetFontOffsetForIndex_internal(STBTT_STREAM_TYPE font_collection, int index)
#else
    static int stbtt_GetFontOffsetForIndex_internal(unsigned char * font_collection, int index)
#endif
{
    // если это просто шрифт, то существует только один действительный индекс
    if(stbtt__isfont(font_collection, 0))
        return index == 0 ? 0 : -1;

    // проверь, TTC ли это
#ifdef STBTT_STREAM_TYPE
    stbtt_uint8 buf[4];
    STBTT_STREAM_SEEK(font_collection, 0);
    STBTT_STREAM_READ(font_collection, buf, 4);
    if(stbtt_tag(buf, "ttcf")) {
#else
    if(stbtt_tag(font_collection, "ttcf")) {
#endif
        // version 1?
        if(ttULONG(font_collection, 4) == 0x00010000 || ttULONG(font_collection, 4) == 0x00020000) {
            stbtt_int32 n = ttLONG(font_collection, 8);
            if(index >= n)
                return -1;
            return ttULONG(font_collection, 12 + index * 4);
        }
    }
    return -1;
}
#ifdef STBTT_STREAM_TYPE
    static int stbtt_GetNumberOfFonts_internal(STBTT_STREAM_TYPE font_collection)
#else
    static int stbtt_GetNumberOfFonts_internal(unsigned char * font_collection)
#endif
{
    // если это просто шрифт, то есть только один допустимый шрифт
    if(stbtt__isfont(font_collection, 0))
        return 1;

    // проверь, TTC ли это
#ifdef STBTT_STREAM_TYPE
    stbtt_uint8 buf[4];
    STBTT_STREAM_SEEK(font_collection, 0);
    STBTT_STREAM_READ(font_collection, buf, 4);
    if(stbtt_tag(buf, "ttcf")) {
#else
    if(stbtt_tag(font_collection, "ttcf")) {
#endif
        // version 1?
        if(ttULONG(font_collection, 4) == 0x00010000 || ttULONG(font_collection, 4) == 0x00020000) {
            return ttLONG(font_collection, 8);
        }
    }
    return 0;
}

static stbtt__buf stbtt__get_subrs(stbtt__buf cff, stbtt__buf fontdict)
{
    stbtt_uint32 subrsoff = 0, private_loc[2] = { 0, 0 };
    stbtt__buf pdict;
    stbtt__dict_get_ints(&fontdict, 18, 2, private_loc);
    if(!private_loc[1] || !private_loc[0]) return stbtt__new_buf(NULL, 0);
    pdict = stbtt__buf_range(&cff, private_loc[1], private_loc[0]);
    stbtt__dict_get_ints(&pdict, 19, 1, &subrsoff);
    if(!subrsoff) return stbtt__new_buf(NULL, 0);
    stbtt__buf_seek(&cff, private_loc[1] + subrsoff);
    return stbtt__cff_get_index(&cff);
}

// поскольку большинство людей не будут ее использовать, найдите эту таблицу при первой необходимости.
static int stbtt__get_svg(stbtt_fontinfo * info)
{
    stbtt_uint32 t;
    if(info->svg < 0) {
        t = stbtt__find_table(info->data, info->fontstart, "SVG ");
        if(t) {
            stbtt_uint32 offset = ttULONG(info->data, t + 2);
            info->svg = t + offset;
        }
        else {
            info->svg = 0;
        }
    }
    return info->svg;
}
#ifdef STBTT_STREAM_TYPE
    static int stbtt_InitFont_internal(stbtt_fontinfo * info, STBTT_STREAM_TYPE data, int fontstart)
#else
    static int stbtt_InitFont_internal(stbtt_fontinfo * info, unsigned char * data, int fontstart)
#endif
{
    stbtt_uint32 cmap, t;
    stbtt_int32 i, numTables;

    info->data = data;
    info->fontstart = fontstart;
    info->cff = stbtt__new_buf(NULL, 0);

    cmap = stbtt__find_table(data, fontstart, "cmap");       // требуется
    info->loca = stbtt__find_table(data, fontstart, "loca"); // требуется
    info->head = stbtt__find_table(data, fontstart, "head"); // требуется
    info->glyf = stbtt__find_table(data, fontstart, "glyf"); // требуется
    info->hhea = stbtt__find_table(data, fontstart, "hhea"); // требуется
    info->hmtx = stbtt__find_table(data, fontstart, "hmtx"); // требуется
    info->kern = stbtt__find_table(data, fontstart, "kern"); // не требуется
    info->gpos = stbtt__find_table(data, fontstart, "GPOS"); // не требуется

    if(!cmap || !info->head || !info->hhea || !info->hmtx)
        return 0;
    if(info->glyf) {
        // требуется для истинного типа
        if(!info->loca) return 0;
    }
    else {
        // инициализация для шрифтов CFF/Type2 (OTF)
        stbtt__buf b, topdict, topdictidx;
        stbtt_uint32 cstype = 2, charstrings = 0, fdarrayoff = 0, fdselectoff = 0;
        stbtt_uint32 cff;

        cff = stbtt__find_table(data, fontstart, "CFF ");
        if(!cff) return 0;

        info->fontdicts = stbtt__new_buf(NULL, 0);
        info->fdselect = stbtt__new_buf(NULL, 0);

        // @TODO следует использовать размер из таблицы (не 512 МБ)

#ifdef STBTT_STREAM_TYPE
        info->cff = stbtt__new_buf(info->data, 512 * 1024 * 1024);
        info->cff.offset = cff;
#else
        info->cff = stbtt__new_buf(info->data + cff, 512 * 1024 * 1024);
#endif
        b = info->cff;

        // прочитай заголовок
        stbtt__buf_skip(&b, 2);
        stbtt__buf_seek(&b, stbtt__buf_get8(&b)); // hdrsize

        // @TODO имя INDEX может содержать несколько шрифтов,
        // но мы просто используем первый.
        stbtt__cff_get_index(&b);  // имя INDEX
        topdictidx = stbtt__cff_get_index(&b);
        topdict = stbtt__cff_index_get(topdictidx, 0);
        stbtt__cff_get_index(&b);  // строка INDEX
        info->gsubrs = stbtt__cff_get_index(&b);

        stbtt__dict_get_ints(&topdict, 17, 1, &charstrings);
        stbtt__dict_get_ints(&topdict, 0x100 | 6, 1, &cstype);
        stbtt__dict_get_ints(&topdict, 0x100 | 36, 1, &fdarrayoff);
        stbtt__dict_get_ints(&topdict, 0x100 | 37, 1, &fdselectoff);
        info->subrs = stbtt__get_subrs(b, topdict);

        // мы поддерживаем только символьные строки типа 2
        if(cstype != 2) return 0;
        if(charstrings == 0) return 0;

        if(fdarrayoff) {
            // похоже на шрифт CID
            if(!fdselectoff) return 0;
            stbtt__buf_seek(&b, fdarrayoff);
            info->fontdicts = stbtt__cff_get_index(&b);
            info->fdselect = stbtt__buf_range(&b, fdselectoff, b.size - fdselectoff);
        }

        stbtt__buf_seek(&b, charstrings);
        info->charstrings = stbtt__cff_get_index(&b);
    }

    t = stbtt__find_table(data, fontstart, "maxp");
    if(t)
        info->numGlyphs = ttUSHORT(data, t + 4);
    else
        info->numGlyphs = 0xffff;

    info->svg = -1;

    // найдите таблицу кодировки cmap, которую мы понимаем *теперь*, чтобы избежать поиска
    // позже. (todo: можно сделать это устанавливаемым)
    // то же самое, независимо от глифа.
    numTables = ttUSHORT(data, cmap + 2);
    info->index_map = 0;
    for(i = 0; i < numTables; ++i) {
        stbtt_uint32 encoding_record = cmap + 4 + 8 * i;
        // найдите понятную нам кодировку:
        switch(ttUSHORT(data, encoding_record)) {
            case STBTT_PLATFORM_ID_MICROSOFT:
                switch(ttUSHORT(data, encoding_record + 2)) {
                    case STBTT_MS_EID_UNICODE_BMP:
                    case STBTT_MS_EID_UNICODE_FULL:
                        // MS/Unicode
                        info->index_map = cmap + ttULONG(data, encoding_record + 4);
                        break;
                }
                break;
            case STBTT_PLATFORM_ID_UNICODE:
                // Mac/iOS имеет эти
                // все кодировки имеют кодировку Unicode, поэтому мы не проверяем их.
                info->index_map = cmap + ttULONG(data, encoding_record + 4);
                break;
        }
    }
    if(info->index_map == 0)
        return 0;

    info->indexToLocFormat = ttUSHORT(data, info->head + 50);
    return 1;
}

STBTT_DEF int stbtt_FindGlyphIndex(const stbtt_fontinfo * info, int unicode_codepoint)
{
#ifdef STBTT_STREAM_TYPE
    STBTT_STREAM_TYPE data = info->data;
#else
    stbtt_uint8 * data = info->data;
#endif
    stbtt_uint32 index_map = info->index_map;

    stbtt_uint16 format = ttUSHORT(data, index_map + 0);
    if(format == 0) {  // яблочное байтовое кодирование
        stbtt_int32 bytes = ttUSHORT(data, index_map + 2);
        if(unicode_codepoint < bytes - 6)
            return ttBYTE(data, index_map + 6 + unicode_codepoint);
        return 0;
    }
    else if(format == 6) {
        stbtt_uint32 first = ttUSHORT(data, index_map + 6);
        stbtt_uint32 count = ttUSHORT(data, index_map + 8);
        if((stbtt_uint32)unicode_codepoint >= first && (stbtt_uint32)unicode_codepoint < first + count)
            return ttUSHORT(data, index_map + 10 + (unicode_codepoint - first) * 2);
        return 0;
    }
    else if(format == 2) {
        STBTT_assert(0); // @TODO: high-byte mapping for japanese/chinese/korean
        return 0;
    }
    else if(format == 4) {  // стандартное сопоставление для шрифтов Windows: набор диапазонов двоичного поиска
        stbtt_uint16 segcount = ttUSHORT(data, index_map + 6) >> 1;
        stbtt_uint16 searchRange = ttUSHORT(data, index_map + 8) >> 1;
        stbtt_uint16 entrySelector = ttUSHORT(data, index_map + 10);
        stbtt_uint16 rangeShift = ttUSHORT(data, index_map + 12) >> 1;

        // выполнить двоичный поиск сегментов
        stbtt_uint32 endCount = index_map + 14;
        stbtt_uint32 search = endCount;

        if(unicode_codepoint > 0xffff)
            return 0;

        // они лежат от endCount.. endCount+segCount
        // но searchRange — это ближайшая степень двойки, так что...
        if(unicode_codepoint >= ttUSHORT(data, search + rangeShift * 2))
            search += rangeShift * 2;

        // теперь правильно уменьшаем смещение, чтобы найти наименьший
        search -= 2;
        while(entrySelector) {
            stbtt_uint16 end;
            searchRange >>= 1;
            end = ttUSHORT(data, search + searchRange * 2);
            if(unicode_codepoint > end)
                search += searchRange * 2;
            --entrySelector;
        }
        search += 2;

        {
            stbtt_uint16 offset, start, last;
            stbtt_uint16 item = (stbtt_uint16)((search - endCount) >> 1);

            start = ttUSHORT(data, index_map + 14 + segcount * 2 + 2 + 2 * item);
            last = ttUSHORT(data, endCount + 2 * item);
            if(unicode_codepoint < start || unicode_codepoint > last)
                return 0;

            offset = ttUSHORT(data, index_map + 14 + segcount * 6 + 2 + 2 * item);
            if(offset == 0)
                return (stbtt_uint16)(unicode_codepoint + ttSHORT(data, index_map + 14 + segcount * 4 + 2 + 2 * item));

            return ttUSHORT(data, offset + (unicode_codepoint - start) * 2 + index_map + 14 + segcount * 6 + 2 + 2 * item);
        }
    }
    else if(format == 12 || format == 13) {
        stbtt_uint32 ngroups = ttULONG(data, index_map + 12);
        stbtt_int32 low, high;
        low = 0;
        high = (stbtt_int32)ngroups;
        // Бинарный поиск нужной группы.
        while(low < high) {
            stbtt_int32 mid = low + ((high - low) >> 1); // округляет вниз, поэтому низко <= средне < высоко
            stbtt_uint32 start_char = ttULONG(data, index_map + 16 + mid * 12);
            stbtt_uint32 end_char = ttULONG(data, index_map + 16 + mid * 12 + 4);
            if((stbtt_uint32)unicode_codepoint < start_char)
                high = mid;
            else if((stbtt_uint32)unicode_codepoint > end_char)
                low = mid + 1;
            else {
                stbtt_uint32 start_glyph = ttULONG(data, index_map + 16 + mid * 12 + 8);
                if(format == 12)
                    return start_glyph + unicode_codepoint - start_char;
                else // формат == 13
                    return start_glyph;
            }
        }
        return 0; // не найден
    }
    // @TODO
    STBTT_assert(0);
    return 0;
}

STBTT_DEF int stbtt_GetCodepointShape(const stbtt_fontinfo * info, int unicode_codepoint, stbtt_vertex * *vertices)
{
    return stbtt_GetGlyphShape(info, stbtt_FindGlyphIndex(info, unicode_codepoint), vertices);
}

static void stbtt_setvertex(stbtt_vertex * v, stbtt_uint8 type, stbtt_int32 x, stbtt_int32 y, stbtt_int32 cx,
                            stbtt_int32 cy)
{
    v->type = type;
    v->x = (stbtt_int16)x;
    v->y = (stbtt_int16)y;
    v->cx = (stbtt_int16)cx;
    v->cy = (stbtt_int16)cy;
}

static int stbtt__GetGlyfOffset(const stbtt_fontinfo * info, int glyph_index)
{
    int g1, g2;

    STBTT_assert(!info->cff.size);

    if(glyph_index >= info->numGlyphs) return -1;  // индекс глифа вне диапазона
    if(info->indexToLocFormat >= 2)    return -1;  // неизвестный индекс->формат карты глифов

    if(info->indexToLocFormat == 0) {
        g1 = info->glyf + ttUSHORT(info->data, info->loca + glyph_index * 2) * 2;
        g2 = info->glyf + ttUSHORT(info->data, info->loca + glyph_index * 2 + 2) * 2;
    }
    else {
        g1 = info->glyf + ttULONG(info->data, info->loca + glyph_index * 4);
        g2 = info->glyf + ttULONG(info->data, info->loca + glyph_index * 4 + 4);
    }

    return g1 == g2 ? -1 : g1; // если длина равна 0, верните -1
}

static int stbtt__GetGlyphInfoT2(const stbtt_fontinfo * info, int glyph_index, int * x0, int * y0, int * x1, int * y1);

STBTT_DEF int stbtt_GetGlyphBox(const stbtt_fontinfo * info, int glyph_index, int * x0, int * y0, int * x1, int * y1)
{
    if(info->cff.size) {
        stbtt__GetGlyphInfoT2(info, glyph_index, x0, y0, x1, y1);
    }
    else {
        int g = stbtt__GetGlyfOffset(info, glyph_index);
        if(g < 0) return 0;

        if(x0) *x0 = ttSHORT(info->data, g + 2);
        if(y0) *y0 = ttSHORT(info->data, g + 4);
        if(x1) *x1 = ttSHORT(info->data, g + 6);
        if(y1) *y1 = ttSHORT(info->data, g + 8);
    }
    return 1;
}

STBTT_DEF int stbtt_GetCodepointBox(const stbtt_fontinfo * info, int codepoint, int * x0, int * y0, int * x1, int * y1)
{
    return stbtt_GetGlyphBox(info, stbtt_FindGlyphIndex(info, codepoint), x0, y0, x1, y1);
}

STBTT_DEF int stbtt_IsGlyphEmpty(const stbtt_fontinfo * info, int glyph_index)
{
    stbtt_int16 numberOfContours;
    int g;
    if(info->cff.size)
        return stbtt__GetGlyphInfoT2(info, glyph_index, NULL, NULL, NULL, NULL) == 0;
    g = stbtt__GetGlyfOffset(info, glyph_index);
    if(g < 0) return 1;
    numberOfContours = ttSHORT(info->data, g);
    return numberOfContours == 0;
}

static int stbtt__close_shape(stbtt_vertex * vertices, int num_vertices, int was_off, int start_off,
                              stbtt_int32 sx, stbtt_int32 sy, stbtt_int32 scx, stbtt_int32 scy, stbtt_int32 cx, stbtt_int32 cy)
{
    if(start_off) {
        if(was_off)
            stbtt_setvertex(&vertices[num_vertices++], STBTT_vcurve, (cx + scx) >> 1, (cy + scy) >> 1, cx, cy);
        stbtt_setvertex(&vertices[num_vertices++], STBTT_vcurve, sx, sy, scx, scy);
    }
    else {
        if(was_off)
            stbtt_setvertex(&vertices[num_vertices++], STBTT_vcurve, sx, sy, cx, cy);
        else
            stbtt_setvertex(&vertices[num_vertices++], STBTT_vline, sx, sy, 0, 0);
    }
    return num_vertices;
}

static int stbtt__GetGlyphShapeTT(const stbtt_fontinfo * info, int glyph_index, stbtt_vertex * *pvertices)
{
    stbtt_int16 numberOfContours;
    stbtt_uint32 endPtsOfContours;
#ifdef STBTT_STREAM_TYPE
    STBTT_STREAM_TYPE data = info->data;
#else
    stbtt_uint8 * data = info->data;
#endif
    stbtt_vertex * vertices = 0;
    int num_vertices = 0;
    int g = stbtt__GetGlyfOffset(info, glyph_index);

    *pvertices = NULL;

    if(g < 0) return 0;

    numberOfContours = ttSHORT(data, g);

    if(numberOfContours > 0) {
        stbtt_uint8 flags = 0, flagcount;
        stbtt_int32 ins, i, j = 0, m, n, next_move, was_off = 0, off, start_off = 0;
        stbtt_int32 x, y, cx, cy, sx, sy, scx, scy;
        stbtt_uint32 points;
        endPtsOfContours = (g + 10);
        ins = ttUSHORT(data, g + 10 + numberOfContours * 2);
        points = g + 10 + numberOfContours * 2 + 2 + ins;

        n = 1 + ttUSHORT(data, endPtsOfContours + numberOfContours * 2 - 2);

        m = n + 2 * numberOfContours;  // свободная граница количества вершин, которые нам могут понадобиться
        vertices = (stbtt_vertex *)STBTT_malloc(m * sizeof(vertices[0]), info->userdata);
        if(vertices == 0)
            return 0;

        next_move = 0;
        flagcount = 0;

        // на первом проходе мы загружаем неинтерпретированные данные в выделенный массив
        // выше, сдвинуто в конец массива, чтобы не перезаписывать его при
        // мы создаем наши окончательные данные, начиная с лицевой стороны

        off = m - n; // начальное смещение для неинтерпретированных данных, независимо от того, как в конечном итоге вычисляется m

        // флаги первой загрузки

        for(i = 0; i < n; ++i) {
            if(flagcount == 0) {
                flags = ttBYTE(data, points++);
                if(flags & 8)
                    flagcount = ttBYTE(data, points++);
            }
            else
                --flagcount;
            vertices[off + i].type = flags;
        }

        // теперь загрузим координаты X
        x = 0;
        for(i = 0; i < n; ++i) {
            flags = vertices[off + i].type;
            if(flags & 2) {
                stbtt_int16 dx = ttBYTE(data, points++);
                x += (flags & 16) ? dx : -dx; // ???
            }
            else {
                if(!(flags & 16)) {
                    x = x + (stbtt_int16)(ttBYTE(data, points) * 256 + ttBYTE(data, points + 1));
                    points += 2;
                }
            }
            vertices[off + i].x = (stbtt_int16)x;
        }

        // теперь загрузим координаты Y
        y = 0;
        for(i = 0; i < n; ++i) {
            flags = vertices[off + i].type;
            if(flags & 4) {
                stbtt_int16 dy = ttBYTE(data, points++);
                y += (flags & 32) ? dy : -dy; // ???
            }
            else {
                if(!(flags & 32)) {
                    y = y + (stbtt_int16)(ttBYTE(data, points) * 256 + ttBYTE(data, points + 1));
                    points += 2;
                }
            }
            vertices[off + i].y = (stbtt_int16)y;
        }

        // теперь преобразуйте их в наш формат
        num_vertices = 0;
        sx = sy = cx = cy = scx = scy = 0;
        for(i = 0; i < n; ++i) {
            flags = vertices[off + i].type;
            x = (stbtt_int16)vertices[off + i].x;
            y = (stbtt_int16)vertices[off + i].y;

            if(next_move == i) {
                if(i != 0)
                    num_vertices = stbtt__close_shape(vertices, num_vertices, was_off, start_off, sx, sy, scx, scy, cx, cy);

                // теперь начни новый
                start_off = !(flags & 1);
                if(start_off) {
                    // если мы начнем с точки отклонения от кривой, тогда нам нужно найти точку на кривой
                    // с чего мы можем начать, и нам нужно сохранить некоторое состояние на время завершения.
                    scx = x;
                    scy = y;
                    if(!(vertices[off + i + 1].type & 1)) {
                        // следующая точка также является точкой кривой, поэтому интерполируйте кривую по точке.
                        sx = (x + (stbtt_int32)vertices[off + i + 1].x) >> 1;
                        sy = (y + (stbtt_int32)vertices[off + i + 1].y) >> 1;
                    }
                    else {
                        // в противном случае просто используйте следующую точку в качестве отправной точки.
                        sx = (stbtt_int32)vertices[off + i + 1].x;
                        sy = (stbtt_int32)vertices[off + i + 1].y;
                        ++i; // мы используем точку i+1 в качестве отправной точки, поэтому пропустите ее
                    }
                }
                else {
                    sx = x;
                    sy = y;
                }
                stbtt_setvertex(&vertices[num_vertices++], STBTT_vmove, sx, sy, 0, 0);
                was_off = 0;
                next_move = 1 + ttUSHORT(data, endPtsOfContours + j * 2);
                ++j;
            }
            else {
                if(!(flags & 1)) {  // если это кривая
                    if(was_off)  // две контрольные точки, не соответствующие кривой, подряд означают интерполяцию средней точки, лежащей на кривой.
                        stbtt_setvertex(&vertices[num_vertices++], STBTT_vcurve, (cx + x) >> 1, (cy + y) >> 1, cx, cy);
                    cx = x;
                    cy = y;
                    was_off = 1;
                }
                else {
                    if(was_off)
                        stbtt_setvertex(&vertices[num_vertices++], STBTT_vcurve, x, y, cx, cy);
                    else
                        stbtt_setvertex(&vertices[num_vertices++], STBTT_vline, x, y, 0, 0);
                    was_off = 0;
                }
            }
        }
        num_vertices = stbtt__close_shape(vertices, num_vertices, was_off, start_off, sx, sy, scx, scy, cx, cy);
    }
    else if(numberOfContours < 0) {
        // Сложные формы.
        int more = 1;
        stbtt_uint32 comp = g + 10;
        num_vertices = 0;
        vertices = 0;
        while(more) {
            stbtt_uint16 flags, gidx;
            int comp_num_verts = 0, i;
            stbtt_vertex * comp_verts = 0, * tmp = 0;
            float mtx[6] = { 1, 0, 0, 1, 0, 0 }, m, n;

            flags = ttSHORT(data, comp);
            comp += 2;
            gidx = ttSHORT(data, comp);
            comp += 2;

            if(flags & 2) {  // Значения XY
                if(flags & 1) {  // шорты
                    mtx[4] = ttSHORT(data, comp);
                    comp += 2;
                    mtx[5] = ttSHORT(data, comp);
                    comp += 2;
                }
                else {
                    mtx[4] = ttCHAR(data, comp);
                    comp += 1;
                    mtx[5] = ttCHAR(data, comp);
                    comp += 1;
                }
            }
            else {
                // @TODO обрабатывает точку сопоставления
                STBTT_assert(0);
            }
            if(flags & (1 << 3)) {  // WE_HAVE_A_SCALE
                mtx[0] = mtx[3] = ttSHORT(data, comp) / 16384.0f;
                comp += 2;
                mtx[1] = mtx[2] = 0;
            }
            else if(flags & (1 << 6)) {  // WE_HAVE_AN_X_AND_YSCALE
                mtx[0] = ttSHORT(data, comp) / 16384.0f;
                comp += 2;
                mtx[1] = mtx[2] = 0;
                mtx[3] = ttSHORT(data, comp) / 16384.0f;
                comp += 2;
            }
            else if(flags & (1 << 7)) {  // WE_HAVE_A_TWO_BY_TWO
                mtx[0] = ttSHORT(data, comp) / 16384.0f;
                comp += 2;
                mtx[1] = ttSHORT(data, comp) / 16384.0f;
                comp += 2;
                mtx[2] = ttSHORT(data, comp) / 16384.0f;
                comp += 2;
                mtx[3] = ttSHORT(data, comp) / 16384.0f;
                comp += 2;
            }

            // Найдите масштабы трансформации.
            m = (float)STBTT_sqrt(mtx[0] * mtx[0] + mtx[1] * mtx[1]);
            n = (float)STBTT_sqrt(mtx[2] * mtx[2] + mtx[3] * mtx[3]);

            // Получить индексированный глиф.
            comp_num_verts = stbtt_GetGlyphShape(info, gidx, &comp_verts);
            if(comp_num_verts > 0) {
                // Преобразование вершин.
                for(i = 0; i < comp_num_verts; ++i) {
                    stbtt_vertex * v = &comp_verts[i];
                    stbtt_vertex_type x, y;
                    x = v->x;
                    y = v->y;
                    v->x = (stbtt_vertex_type)(m * (mtx[0] * x + mtx[2] * y + mtx[4]));
                    v->y = (stbtt_vertex_type)(n * (mtx[1] * x + mtx[3] * y + mtx[5]));
                    x = v->cx;
                    y = v->cy;
                    v->cx = (stbtt_vertex_type)(m * (mtx[0] * x + mtx[2] * y + mtx[4]));
                    v->cy = (stbtt_vertex_type)(n * (mtx[1] * x + mtx[3] * y + mtx[5]));
                }
                // Добавить вершины.
                tmp = (stbtt_vertex *)STBTT_malloc((num_vertices + comp_num_verts) * sizeof(stbtt_vertex), info->userdata);
                if(!tmp) {
                    if(vertices) STBTT_free(vertices, info->userdata);
                    if(comp_verts) STBTT_free(comp_verts, info->userdata);
                    return 0;
                }
                if(num_vertices > 0 && vertices) STBTT_memcpy(tmp, vertices, num_vertices * sizeof(stbtt_vertex));
                STBTT_memcpy(tmp + num_vertices, comp_verts, comp_num_verts * sizeof(stbtt_vertex));
                if(vertices) STBTT_free(vertices, info->userdata);
                vertices = tmp;
                STBTT_free(comp_verts, info->userdata);
                num_vertices += comp_num_verts;
            }
            // More components ?
            more = flags & (1 << 5);
        }
    }
    else {
        // NumberOfCounters == 0, ничего не делать
    }

    *pvertices = vertices;
    return num_vertices;
}

typedef struct {
    int bounds;
    int started;
    float first_x, first_y;
    float x, y;
    stbtt_int32 min_x, max_x, min_y, max_y;

    stbtt_vertex * pvertices;
    int num_vertices;
} stbtt__csctx;

#define STBTT__CSCTX_INIT(bounds) {bounds,0, 0,0, 0,0, 0,0,0,0, NULL, 0}

static void stbtt__track_vertex(stbtt__csctx * c, stbtt_int32 x, stbtt_int32 y)
{
    if(x > c->max_x || !c->started) c->max_x = x;
    if(y > c->max_y || !c->started) c->max_y = y;
    if(x < c->min_x || !c->started) c->min_x = x;
    if(y < c->min_y || !c->started) c->min_y = y;
    c->started = 1;
}

static void stbtt__csctx_v(stbtt__csctx * c, stbtt_uint8 type, stbtt_int32 x, stbtt_int32 y, stbtt_int32 cx,
                           stbtt_int32 cy, stbtt_int32 cx1, stbtt_int32 cy1)
{
    if(c->bounds) {
        stbtt__track_vertex(c, x, y);
        if(type == STBTT_vcubic) {
            stbtt__track_vertex(c, cx, cy);
            stbtt__track_vertex(c, cx1, cy1);
        }
    }
    else {
        stbtt_setvertex(&c->pvertices[c->num_vertices], type, x, y, cx, cy);
        c->pvertices[c->num_vertices].cx1 = (stbtt_int16)cx1;
        c->pvertices[c->num_vertices].cy1 = (stbtt_int16)cy1;
    }
    c->num_vertices++;
}

static void stbtt__csctx_close_shape(stbtt__csctx * ctx)
{
    if(ctx->first_x != ctx->x || ctx->first_y != ctx->y)
        stbtt__csctx_v(ctx, STBTT_vline, (int)ctx->first_x, (int)ctx->first_y, 0, 0, 0, 0);
}

static void stbtt__csctx_rmove_to(stbtt__csctx * ctx, float dx, float dy)
{
    stbtt__csctx_close_shape(ctx);
    ctx->first_x = ctx->x = ctx->x + dx;
    ctx->first_y = ctx->y = ctx->y + dy;
    stbtt__csctx_v(ctx, STBTT_vmove, (int)ctx->x, (int)ctx->y, 0, 0, 0, 0);
}

static void stbtt__csctx_rline_to(stbtt__csctx * ctx, float dx, float dy)
{
    ctx->x += dx;
    ctx->y += dy;
    stbtt__csctx_v(ctx, STBTT_vline, (int)ctx->x, (int)ctx->y, 0, 0, 0, 0);
}

static void stbtt__csctx_rccurve_to(stbtt__csctx * ctx, float dx1, float dy1, float dx2, float dy2, float dx3,
                                    float dy3)
{
    float cx1 = ctx->x + dx1;
    float cy1 = ctx->y + dy1;
    float cx2 = cx1 + dx2;
    float cy2 = cy1 + dy2;
    ctx->x = cx2 + dx3;
    ctx->y = cy2 + dy3;
    stbtt__csctx_v(ctx, STBTT_vcubic, (int)ctx->x, (int)ctx->y, (int)cx1, (int)cy1, (int)cx2, (int)cy2);
}

static stbtt__buf stbtt__get_subr(stbtt__buf idx, int n)
{
    int count = stbtt__cff_index_count(&idx);
    int bias = 107;
    if(count >= 33900)
        bias = 32768;
    else if(count >= 1240)
        bias = 1131;
    n += bias;
    if(n < 0 || n >= count)
        return stbtt__new_buf(NULL, 0);
    return stbtt__cff_index_get(idx, n);
}

static stbtt__buf stbtt__cid_get_glyph_subrs(const stbtt_fontinfo * info, int glyph_index)
{
    stbtt__buf fdselect = info->fdselect;
    int nranges, start, end, v, fmt, fdselector = -1, i;

    stbtt__buf_seek(&fdselect, 0);
    fmt = stbtt__buf_get8(&fdselect);
    if(fmt == 0) {
        // непроверенный
        stbtt__buf_skip(&fdselect, glyph_index);
        fdselector = stbtt__buf_get8(&fdselect);
    }
    else if(fmt == 3) {
        nranges = stbtt__buf_get16(&fdselect);
        start = stbtt__buf_get16(&fdselect);
        for(i = 0; i < nranges; i++) {
            v = stbtt__buf_get8(&fdselect);
            end = stbtt__buf_get16(&fdselect);
            if(glyph_index >= start && glyph_index < end) {
                fdselector = v;
                break;
            }
            start = end;
        }
    }
    if(fdselector == -1) stbtt__new_buf(NULL, 0);
    return stbtt__get_subrs(info->cff, stbtt__cff_index_get(info->fontdicts, fdselector));
}

static int stbtt__run_charstring(const stbtt_fontinfo * info, int glyph_index, stbtt__csctx * c)
{
    int in_header = 1, maskbits = 0, subr_stack_height = 0, sp = 0, v, i, b0;
    int has_subrs = 0, clear_stack;
    float s[48];
    stbtt__buf subr_stack[10], subrs = info->subrs, b;
    float f;

#define STBTT__CSERR(s) (0)

    // в настоящее время это игнорирует начальное значение ширины, которое не требуется, если у нас есть hmtx
    b = stbtt__cff_index_get(info->charstrings, glyph_index);
    while(b.cursor < b.size) {
        i = 0;
        clear_stack = 1;
        b0 = stbtt__buf_get8(&b);
        switch(b0) {
            // @TODO реализует хинтинг
            case 0x13: // маска подсказки
            case 0x14: // центральная маска
                if(in_header)
                    maskbits += (sp / 2); // неявное "встем"
                in_header = 0;
                stbtt__buf_skip(&b, (maskbits + 7) / 8);
                break;

            case 0x01: // hstem
            case 0x03: // vstem
            case 0x12: // хстемхм
            case 0x17: // встемхм
                maskbits += (sp / 2);
                break;

            case 0x15: // рмовето
                in_header = 0;
                if(sp < 2) return STBTT__CSERR("rmoveto stack");
                stbtt__csctx_rmove_to(c, s[sp - 2], s[sp - 1]);
                break;
            case 0x04: // вмовето
                in_header = 0;
                if(sp < 1) return STBTT__CSERR("vmoveto stack");
                stbtt__csctx_rmove_to(c, 0, s[sp - 1]);
                break;
            case 0x16: // хмовето
                in_header = 0;
                if(sp < 1) return STBTT__CSERR("hmoveto stack");
                stbtt__csctx_rmove_to(c, s[sp - 1], 0);
                break;

            case 0x05: // рлинето
                if(sp < 2) return STBTT__CSERR("rlineto stack");
                for(; i + 1 < sp; i += 2)
                    stbtt__csctx_rline_to(c, s[i], s[i + 1]);
                break;

            // hlineto/vlineto и vhcurveto/hvcurveto чередуются по горизонтали и вертикали
            // начиная с другого места.

            case 0x07: // vlineto
                if(sp < 1) return STBTT__CSERR("vlineto stack");
                goto vlineto;
            case 0x06: // хлинето
                if(sp < 1) return STBTT__CSERR("hlineto stack");
                for(;;) {
                    if(i >= sp) break;
                    stbtt__csctx_rline_to(c, s[i], 0);
                    i++;
vlineto:
                    if(i >= sp) break;
                    stbtt__csctx_rline_to(c, 0, s[i]);
                    i++;
                }
                break;

            case 0x1F: // hvcurveto
                if(sp < 4) return STBTT__CSERR("hvcurveto stack");
                goto hvcurveto;
            case 0x1E: // vhcurveto
                if(sp < 4) return STBTT__CSERR("vhcurveto stack");
                for(;;) {
                    if(i + 3 >= sp) break;
                    stbtt__csctx_rccurve_to(c, 0, s[i], s[i + 1], s[i + 2], s[i + 3], (sp - i == 5) ? s[i + 4] : 0.0f);
                    i += 4;
hvcurveto:
                    if(i + 3 >= sp) break;
                    stbtt__csctx_rccurve_to(c, s[i], 0, s[i + 1], s[i + 2], (sp - i == 5) ? s[i + 4] : 0.0f, s[i + 3]);
                    i += 4;
                }
                break;

            case 0x08: // rrcurveto
                if(sp < 6) return STBTT__CSERR("rcurveline stack");
                for(; i + 5 < sp; i += 6)
                    stbtt__csctx_rccurve_to(c, s[i], s[i + 1], s[i + 2], s[i + 3], s[i + 4], s[i + 5]);
                break;

            case 0x18: // rкривая линия
                if(sp < 8) return STBTT__CSERR("rcurveline stack");
                for(; i + 5 < sp - 2; i += 6)
                    stbtt__csctx_rccurve_to(c, s[i], s[i + 1], s[i + 2], s[i + 3], s[i + 4], s[i + 5]);
                if(i + 1 >= sp) return STBTT__CSERR("rcurveline stack");
                stbtt__csctx_rline_to(c, s[i], s[i + 1]);
                break;

            case 0x19: // линиякривая
                if(sp < 8) return STBTT__CSERR("rlinecurve stack");
                for(; i + 1 < sp - 6; i += 2)
                    stbtt__csctx_rline_to(c, s[i], s[i + 1]);
                if(i + 5 >= sp) return STBTT__CSERR("rlinecurve stack");
                stbtt__csctx_rccurve_to(c, s[i], s[i + 1], s[i + 2], s[i + 3], s[i + 4], s[i + 5]);
                break;

            case 0x1A: // vvcurveto
            case 0x1B: // hhcurveto
                if(sp < 4) return STBTT__CSERR("(vv|hh)curveto stack");
                f = 0.0;
                if(sp & 1) {
                    f = s[i];
                    i++;
                }
                for(; i + 3 < sp; i += 4) {
                    if(b0 == 0x1B)
                        stbtt__csctx_rccurve_to(c, s[i], f, s[i + 1], s[i + 2], s[i + 3], 0.0);
                    else
                        stbtt__csctx_rccurve_to(c, f, s[i], s[i + 1], s[i + 2], 0.0, s[i + 3]);
                    f = 0.0;
                }
                break;

            case 0x0A: // коллсубр
                if(!has_subrs) {
                    if(info->fdselect.size)
                        subrs = stbtt__cid_get_glyph_subrs(info, glyph_index);
                    has_subrs = 1;
                }
            // FALLTHROUGH
            case 0x1D: // вызовgsubr
                if(sp < 1) return STBTT__CSERR("call(g|)subr stack");
                v = (int)s[--sp];
                if(subr_stack_height >= 10) return STBTT__CSERR("recursion limit");
                subr_stack[subr_stack_height++] = b;
                b = stbtt__get_subr(b0 == 0x0A ? subrs : info->gsubrs, v);
                if(b.size == 0) return STBTT__CSERR("subr not found");
                b.cursor = 0;
                clear_stack = 0;
                break;

            case 0x0B: // возвращение
                if(subr_stack_height <= 0) return STBTT__CSERR("return outside subr");
                b = subr_stack[--subr_stack_height];
                clear_stack = 0;
                break;

            case 0x0E: // конечный символ
                stbtt__csctx_close_shape(c);
                return 1;

            case 0x0C: { // двухбайтовый escape
                    float dx1, dx2, dx3, dx4, dx5, dx6, dy1, dy2, dy3, dy4, dy5, dy6;
                    float dx, dy;
                    int b1 = stbtt__buf_get8(&b);
                    switch(b1) {
                        // @TODO Эти «гибкие» реализации игнорируют глубину гибкости и разрешение.
                        // и всегда рисуйте Безье.
                        case 0x22: // hflex
                            if(sp < 7) return STBTT__CSERR("hflex stack");
                            dx1 = s[0];
                            dx2 = s[1];
                            dy2 = s[2];
                            dx3 = s[3];
                            dx4 = s[4];
                            dx5 = s[5];
                            dx6 = s[6];
                            stbtt__csctx_rccurve_to(c, dx1, 0, dx2, dy2, dx3, 0);
                            stbtt__csctx_rccurve_to(c, dx4, 0, dx5, -dy2, dx6, 0);
                            break;

                        case 0x23: // гибкий
                            if(sp < 13) return STBTT__CSERR("flex stack");
                            dx1 = s[0];
                            dy1 = s[1];
                            dx2 = s[2];
                            dy2 = s[3];
                            dx3 = s[4];
                            dy3 = s[5];
                            dx4 = s[6];
                            dy4 = s[7];
                            dx5 = s[8];
                            dy5 = s[9];
                            dx6 = s[10];
                            dy6 = s[11];
                            //fd is s[12]
                            stbtt__csctx_rccurve_to(c, dx1, dy1, dx2, dy2, dx3, dy3);
                            stbtt__csctx_rccurve_to(c, dx4, dy4, dx5, dy5, dx6, dy6);
                            break;

                        case 0x24: // hflex1
                            if(sp < 9) return STBTT__CSERR("hflex1 stack");
                            dx1 = s[0];
                            dy1 = s[1];
                            dx2 = s[2];
                            dy2 = s[3];
                            dx3 = s[4];
                            dx4 = s[5];
                            dx5 = s[6];
                            dy5 = s[7];
                            dx6 = s[8];
                            stbtt__csctx_rccurve_to(c, dx1, dy1, dx2, dy2, dx3, 0);
                            stbtt__csctx_rccurve_to(c, dx4, 0, dx5, dy5, dx6, -(dy1 + dy2 + dy5));
                            break;

                        case 0x25: // гибкий1
                            if(sp < 11) return STBTT__CSERR("flex1 stack");
                            dx1 = s[0];
                            dy1 = s[1];
                            dx2 = s[2];
                            dy2 = s[3];
                            dx3 = s[4];
                            dy3 = s[5];
                            dx4 = s[6];
                            dy4 = s[7];
                            dx5 = s[8];
                            dy5 = s[9];
                            dx6 = dy6 = s[10];
                            dx = dx1 + dx2 + dx3 + dx4 + dx5;
                            dy = dy1 + dy2 + dy3 + dy4 + dy5;
                            if(STBTT_fabs(dx) > STBTT_fabs(dy))
                                dy6 = -dy;
                            else
                                dx6 = -dx;
                            stbtt__csctx_rccurve_to(c, dx1, dy1, dx2, dy2, dx3, dy3);
                            stbtt__csctx_rccurve_to(c, dx4, dy4, dx5, dy5, dx6, dy6);
                            break;

                        default:
                            return STBTT__CSERR("unimplemented");
                    }
                }
                break;

            default:
                if(b0 != 255 && b0 != 28 && b0 < 32)
                    return STBTT__CSERR("reserved operator");

                // нажать немедленно
                if(b0 == 255) {
                    f = (float)(stbtt_int32)stbtt__buf_get32(&b) / 0x10000;
                }
                else {
                    stbtt__buf_skip(&b, -1);
                    f = (float)(stbtt_int16)stbtt__cff_int(&b);
                }
                if(sp >= 48) return STBTT__CSERR("push stack overflow");
                s[sp++] = f;
                clear_stack = 0;
                break;
        }
        if(clear_stack) sp = 0;
    }
    return STBTT__CSERR("no endchar");

#undef STBTT__CSERR
}

static int stbtt__GetGlyphShapeT2(const stbtt_fontinfo * info, int glyph_index, stbtt_vertex * *pvertices)
{
    // запускает строку символов дважды: один раз для подсчета и один раз для вывода (чтобы избежать перераспределения)
    stbtt__csctx count_ctx = STBTT__CSCTX_INIT(1);
    stbtt__csctx output_ctx = STBTT__CSCTX_INIT(0);
    if(stbtt__run_charstring(info, glyph_index, &count_ctx)) {
        *pvertices = (stbtt_vertex *)STBTT_malloc(count_ctx.num_vertices * sizeof(stbtt_vertex), info->userdata);
        output_ctx.pvertices = *pvertices;
        if(stbtt__run_charstring(info, glyph_index, &output_ctx)) {
            STBTT_assert(output_ctx.num_vertices == count_ctx.num_vertices);
            return output_ctx.num_vertices;
        }
    }
    *pvertices = NULL;
    return 0;
}

static int stbtt__GetGlyphInfoT2(const stbtt_fontinfo * info, int glyph_index, int * x0, int * y0, int * x1, int * y1)
{
    stbtt__csctx c = STBTT__CSCTX_INIT(1);
    int r = stbtt__run_charstring(info, glyph_index, &c);
    if(x0)  *x0 = r ? c.min_x : 0;
    if(y0)  *y0 = r ? c.min_y : 0;
    if(x1)  *x1 = r ? c.max_x : 0;
    if(y1)  *y1 = r ? c.max_y : 0;
    return r ? c.num_vertices : 0;
}

STBTT_DEF int stbtt_GetGlyphShape(const stbtt_fontinfo * info, int glyph_index, stbtt_vertex * *pvertices)
{
    if(!info->cff.size)
        return stbtt__GetGlyphShapeTT(info, glyph_index, pvertices);
    else
        return stbtt__GetGlyphShapeT2(info, glyph_index, pvertices);
}

STBTT_DEF void stbtt_GetGlyphHMetrics(const stbtt_fontinfo * info, int glyph_index, int * advanceWidth,
                                      int * leftSideBearing)
{
    stbtt_uint16 numOfLongHorMetrics = ttUSHORT(info->data, info->hhea + 34);
    if(glyph_index < numOfLongHorMetrics) {
        if(advanceWidth)     *advanceWidth = ttSHORT(info->data, info->hmtx + 4 * glyph_index);
        if(leftSideBearing)  *leftSideBearing = ttSHORT(info->data, info->hmtx + 4 * glyph_index + 2);
    }
    else {
        if(advanceWidth)     *advanceWidth = ttSHORT(info->data, info->hmtx + 4 * (numOfLongHorMetrics - 1));
        if(leftSideBearing)  *leftSideBearing = ttSHORT(info->data,
                                                            info->hmtx + 4 * numOfLongHorMetrics + 2 * (glyph_index - numOfLongHorMetrics));
    }
}

STBTT_DEF int  stbtt_GetKerningTableLength(const stbtt_fontinfo * info)
{
    // мы смотрим только на первую таблицу. он должен быть «горизонтальным» и иметь формат 0.
    if(!info->kern)
        return 0;
    if(ttUSHORT(info->data, 2 + info->kern) < 1)  // количество столов, нужно минимум 1
        return 0;
    if(ttUSHORT(info->data, 8 + info->kern) != 1)  // горизонтальный флаг должен быть установлен в формате
        return 0;

    return ttUSHORT(info->data, 10 + info->kern);
}

STBTT_DEF int stbtt_GetKerningTable(const stbtt_fontinfo * info, stbtt_kerningentry * table, int table_length)
{
    int k, length;

    // мы смотрим только на первую таблицу. он должен быть «горизонтальным» и иметь формат 0.
    if(!info->kern)
        return 0;
    if(ttUSHORT(info->data, 2 + info->kern) < 1)  // количество столов, нужно минимум 1
        return 0;
    if(ttUSHORT(info->data, 8 + info->kern) != 1)  // горизонтальный флаг должен быть установлен в формате
        return 0;

    length = ttUSHORT(info->data, 10 + info->kern);
    if(table_length < length)
        length = table_length;

    for(k = 0; k < length; k++) {
        table[k].glyph1 = ttUSHORT(info->data, 18 + (k * 6) + info->kern);
        table[k].glyph2 = ttUSHORT(info->data, 20 + (k * 6) + info->kern);
        table[k].advance = ttSHORT(info->data, 22 + (k * 6) + info->kern);
    }

    return length;
}

static int stbtt__GetGlyphKernInfoAdvance(const stbtt_fontinfo * info, int glyph1, int glyph2)
{
    stbtt_uint32 needle, straw;
    int l, r, m;

    // мы смотрим только на первую таблицу. он должен быть «горизонтальным» и иметь формат 0.
    if(!info->kern)
        return 0;
    if(ttUSHORT(info->data, info->kern + 2) < 1)  // количество столов, нужно минимум 1
        return 0;
    if(ttUSHORT(info->data, info->kern + 8) != 1)  // горизонтальный флаг должен быть установлен в формате
        return 0;

    l = 0;
    r = ttUSHORT(info->data, info->kern + 10) - 1;
    needle = glyph1 << 16 | glyph2;
    while(l <= r) {
        m = (l + r) >> 1;
        straw = ttULONG(info->data, info->kern + 18 + (m * 6)); // note: unaligned read
        if(needle < straw)
            r = m - 1;
        else if(needle > straw)
            l = m + 1;
        else
            return ttSHORT(info->data, info->kern + 22 + (m * 6));
    }
    return 0;
}
#ifdef STBTT_STREAM_TYPE
    static stbtt_int32 stbtt__GetCoverageIndex(STBTT_STREAM_TYPE data, stbtt_uint32 coverageTable, int glyph)
#else
    static stbtt_int32 stbtt__GetCoverageIndex(const stbtt_uint8 * data, stbtt_uint32 coverageTable, int glyph)
#endif
{
    stbtt_uint16 coverageFormat = ttUSHORT(data, coverageTable);
    switch(coverageFormat) {
        case 1: {
                stbtt_uint16 glyphCount = ttUSHORT(data, coverageTable + 2);

                // Бинарный поиск.
                stbtt_int32 l = 0, r = glyphCount - 1, m;
                int straw, needle = glyph;
                while(l <= r) {
                    stbtt_uint32 glyphArray = coverageTable + 4;
                    stbtt_uint16 glyphID;
                    m = (l + r) >> 1;
                    glyphID = ttUSHORT(data, glyphArray + 2 * m);
                    straw = glyphID;
                    if(needle < straw)
                        r = m - 1;
                    else if(needle > straw)
                        l = m + 1;
                    else {
                        return m;
                    }
                }
                break;
            }

        case 2: {
                stbtt_uint16 rangeCount = ttUSHORT(data, coverageTable + 2);
                stbtt_uint32 rangeArray = coverageTable + 4;

                // Бинарный поиск.
                stbtt_int32 l = 0, r = rangeCount - 1, m;
                int strawStart, strawEnd, needle = glyph;
                while(l <= r) {
                    stbtt_uint32 rangeRecord;
                    m = (l + r) >> 1;
                    rangeRecord = rangeArray + 6 * m;
                    strawStart = ttUSHORT(data, rangeRecord);
                    strawEnd = ttUSHORT(data, rangeRecord + 2);
                    if(needle < strawStart)
                        r = m - 1;
                    else if(needle > strawEnd)
                        l = m + 1;
                    else {
                        stbtt_uint16 startCoverageIndex = ttUSHORT(data, rangeRecord + 4);
                        return startCoverageIndex + glyph - strawStart;
                    }
                }
                break;
            }

        default:
            return -1; // неподдерживаемый
    }

    return -1;
}
#ifdef STBTT_STREAM_TYPE
    static stbtt_int32  stbtt__GetGlyphClass(STBTT_STREAM_TYPE data, stbtt_uint32 classDefTable, int glyph)
#else
    static stbtt_int32  stbtt__GetGlyphClass(const stbtt_uint8 * data, stbtt_uint32 classDefTable, int glyph)
#endif
{
    stbtt_uint16 classDefFormat = ttUSHORT(data, classDefTable);
    switch(classDefFormat) {
        case 1: {
                stbtt_uint16 startGlyphID = ttUSHORT(data, classDefTable + 2);
                stbtt_uint16 glyphCount = ttUSHORT(data, classDefTable + 4);
                stbtt_uint32 classDef1ValueArray = classDefTable + 6;

                if(glyph >= startGlyphID && glyph < startGlyphID + glyphCount)
                    return (stbtt_int32)ttUSHORT(data, classDef1ValueArray + 2 * (glyph - startGlyphID));
                break;
            }

        case 2: {
                stbtt_uint16 classRangeCount = ttUSHORT(data, classDefTable + 2);
                stbtt_uint32 classRangeRecords = classDefTable + 4;

                // Бинарный поиск.
                stbtt_int32 l = 0, r = classRangeCount - 1, m;
                int strawStart, strawEnd, needle = glyph;
                while(l <= r) {
                    stbtt_uint32 classRangeRecord;
                    m = (l + r) >> 1;
                    classRangeRecord = classRangeRecords + 6 * m;
                    strawStart = ttUSHORT(data, classRangeRecord);
                    strawEnd = ttUSHORT(data, classRangeRecord + 2);
                    if(needle < strawStart)
                        r = m - 1;
                    else if(needle > strawEnd)
                        l = m + 1;
                    else
                        return (stbtt_int32)ttUSHORT(data, classRangeRecord + 4);
                }
                break;
            }

        default:
            return -1; // Неподдерживаемый тип определения, возврат ошибки.
    }

    // «Все глифы, не отнесенные к классу, попадают в класс 0». (спецификация OpenType)
    return 0;
}

// Определите STBTT_assert (x), если вы хотите использовать нереализованные форматы.
#define STBTT_GPOS_TODO_assert(x)

static stbtt_int32 stbtt__GetGlyphGPOSInfoAdvance(const stbtt_fontinfo * info, int glyph1, int glyph2)
{
    stbtt_uint16 lookupListOffset;
    stbtt_uint32 lookupList;
    stbtt_uint16 lookupCount;
#ifdef STBTT_STREAM_TYPE
    STBTT_STREAM_TYPE data = info->data;
#else
    const stbtt_uint8 * data = info->data;
#endif
    stbtt_int32 i, sti;

    if(!info->gpos) return 0;

    if(ttUSHORT(data, 0 + info->gpos) != 1) return 0;  // Основная версия 1
    if(ttUSHORT(data, 2 + info->gpos) != 0) return 0;  // Второстепенная версия 0

    lookupListOffset = ttUSHORT(data, 8 + info->gpos);
    lookupList = info->gpos + lookupListOffset;
    lookupCount = ttUSHORT(data, lookupList);

    for(i = 0; i < lookupCount; ++i) {
        stbtt_uint16 lookupOffset = ttUSHORT(data, lookupList + 2 + 2 * i);
        stbtt_uint32 lookupTable = lookupList + lookupOffset;

        stbtt_uint16 lookupType = ttUSHORT(data, lookupTable);
        stbtt_uint16 subTableCount = ttUSHORT(data, lookupTable + 4);
        stbtt_uint32 subTableOffsets = lookupTable + 6;
        if(lookupType != 2)  // Дополнительная таблица позиционирования парной регулировки
            continue;

        for(sti = 0; sti < subTableCount; sti++) {
            stbtt_uint16 subtableOffset = ttUSHORT(data, subTableOffsets + 2 * sti);
            stbtt_uint32 table = lookupTable + subtableOffset;
            stbtt_uint16 posFormat = ttUSHORT(data, table);
            stbtt_uint16 coverageOffset = ttUSHORT(data, table + 2);
            stbtt_int32 coverageIndex = stbtt__GetCoverageIndex(data, table + coverageOffset, glyph1);
            if(coverageIndex == -1) continue;

            switch(posFormat) {
                case 1: {
                        stbtt_int32 l, r, m;
                        int straw, needle;
                        stbtt_uint16 valueFormat1 = ttUSHORT(data, table + 4);
                        stbtt_uint16 valueFormat2 = ttUSHORT(data, table + 6);
                        if(valueFormat1 == 4 && valueFormat2 == 0) {  // Support more formats?
                            stbtt_int32 valueRecordPairSizeInBytes = 2;
                            stbtt_uint16 pairSetCount = ttUSHORT(data, table + 8);
                            stbtt_uint16 pairPosOffset = ttUSHORT(data, table + 10 + 2 * coverageIndex);
                            stbtt_uint32 pairValueTable = table + pairPosOffset;
                            stbtt_uint16 pairValueCount = ttUSHORT(data, pairValueTable);
                            stbtt_uint32 pairValueArray = pairValueTable + 2;

                            if(coverageIndex >= pairSetCount) return 0;

                            needle = glyph2;
                            r = pairValueCount - 1;
                            l = 0;

                            // Бинарный поиск.
                            while(l <= r) {
                                stbtt_uint16 secondGlyph;
                                stbtt_uint32 pairValue;
                                m = (l + r) >> 1;
                                pairValue = pairValueArray + (2 + valueRecordPairSizeInBytes) * m;
                                secondGlyph = ttUSHORT(data, pairValue);
                                straw = secondGlyph;
                                if(needle < straw)
                                    r = m - 1;
                                else if(needle > straw)
                                    l = m + 1;
                                else {
                                    stbtt_int16 xAdvance = ttSHORT(data, pairValue + 2);
                                    return xAdvance;
                                }
                            }
                        }
                        else
                            return 0;
                        break;
                    }

                case 2: {
                        stbtt_uint16 valueFormat1 = ttUSHORT(data, table + 4);
                        stbtt_uint16 valueFormat2 = ttUSHORT(data, table + 6);
                        if(valueFormat1 == 4 && valueFormat2 == 0) {  // Support more formats?
                            stbtt_uint16 classDef1Offset = ttUSHORT(data, table + 8);
                            stbtt_uint16 classDef2Offset = ttUSHORT(data, table + 10);
                            int glyph1class = stbtt__GetGlyphClass(data, table + classDef1Offset, glyph1);
                            int glyph2class = stbtt__GetGlyphClass(data, table + classDef2Offset, glyph2);

                            stbtt_uint16 class1Count = ttUSHORT(data, table + 12);
                            stbtt_uint16 class2Count = ttUSHORT(data, table + 14);
                            stbtt_uint32 class1Records, class2Records;
                            stbtt_int16 xAdvance;

                            if(glyph1class < 0 || glyph1class >= class1Count) return 0;  // уродливый
                            if(glyph2class < 0 || glyph2class >= class2Count) return 0;  // уродливый

                            class1Records = table + 16;
                            class2Records = class1Records + 2 * (glyph1class * class2Count);
                            xAdvance = ttSHORT(data, class2Records + 2 * glyph2class);
                            return xAdvance;
                        }
                        else
                            return 0;
                        break;
                    }

                default:
                    return 0; // Неподдерживаемый формат позиции
            }
        }
    }
    return 0;
}

STBTT_DEF int stbtt_KernTableCheck(const stbtt_fontinfo * info)
{
    if(info->gpos) {
        stbtt_uint16 lookupListOffset;
        stbtt_uint32 lookupList;
        stbtt_uint16 lookupCount;
#ifdef STBTT_STREAM_TYPE
        STBTT_STREAM_TYPE data = info->data;
#else
        const stbtt_uint8 * data = info->data;
#endif
        stbtt_int32 i;

        if(!info->gpos) return 0;

        if(ttUSHORT(data, 0 + info->gpos) != 1) return 0;  // Основная версия 1
        if(ttUSHORT(data, 2 + info->gpos) != 0) return 0;  // Второстепенная версия 0

        lookupListOffset = ttUSHORT(data, 8 + info->gpos);
        lookupList = info->gpos + lookupListOffset;
        lookupCount = ttUSHORT(data, lookupList);

        for(i = 0; i < lookupCount; ++i) {
            stbtt_uint16 lookupOffset = ttUSHORT(data, lookupList + 2 + 2 * i);
            stbtt_uint32 lookupTable = lookupList + lookupOffset;

            stbtt_uint16 lookupType = ttUSHORT(data, lookupTable);

            if(lookupType != 2)  // Дополнительная таблица позиционирования парной регулировки
                continue;

            return 1; // у нас есть полезная таблица поиска.
        }
        return 0;
    }
    else if(info->kern) {
        return 1;
    }
    return 0;
}

STBTT_DEF int  stbtt_GetGlyphKernAdvance(const stbtt_fontinfo * info, int g1, int g2)
{
    int xAdvance = 0;

    if(info->gpos)
        xAdvance += stbtt__GetGlyphGPOSInfoAdvance(info, g1, g2);
    else if(info->kern)
        xAdvance += stbtt__GetGlyphKernInfoAdvance(info, g1, g2);

    return xAdvance;
}

STBTT_DEF int  stbtt_GetCodepointKernAdvance(const stbtt_fontinfo * info, int ch1, int ch2)
{
    if(!info->kern && !info->gpos)  // если нет таблицы кернинга, не тратьте время на поиск обоих кодовых точек->глифов
        return 0;
    return stbtt_GetGlyphKernAdvance(info, stbtt_FindGlyphIndex(info, ch1), stbtt_FindGlyphIndex(info, ch2));
}

STBTT_DEF void stbtt_GetCodepointHMetrics(const stbtt_fontinfo * info, int codepoint, int * advanceWidth,
                                          int * leftSideBearing)
{
    stbtt_GetGlyphHMetrics(info, stbtt_FindGlyphIndex(info, codepoint), advanceWidth, leftSideBearing);
}

STBTT_DEF void stbtt_GetFontVMetrics(const stbtt_fontinfo * info, int * ascent, int * descent, int * lineGap)
{
    if(ascent) *ascent = ttSHORT(info->data, info->hhea + 4);
    if(descent) *descent = ttSHORT(info->data, info->hhea + 6);
    if(lineGap) *lineGap = ttSHORT(info->data, info->hhea + 8);
}

STBTT_DEF int  stbtt_GetFontVMetricsOS2(const stbtt_fontinfo * info, int * typoAscent, int * typoDescent,
                                        int * typoLineGap)
{
    int tab = stbtt__find_table(info->data, info->fontstart, "OS/2");
    if(!tab)
        return 0;
    if(typoAscent) *typoAscent = ttSHORT(info->data, tab + 68);
    if(typoDescent) *typoDescent = ttSHORT(info->data, tab + 70);
    if(typoLineGap) *typoLineGap = ttSHORT(info->data, tab + 72);
    return 1;
}

STBTT_DEF void stbtt_GetFontBoundingBox(const stbtt_fontinfo * info, int * x0, int * y0, int * x1, int * y1)
{
    *x0 = ttSHORT(info->data, info->head + 36);
    *y0 = ttSHORT(info->data, info->head + 38);
    *x1 = ttSHORT(info->data, info->head + 40);
    *y1 = ttSHORT(info->data, info->head + 42);
}

STBTT_DEF float stbtt_ScaleForPixelHeight(const stbtt_fontinfo * info, float height)
{
    int fheight = ttSHORT(info->data, info->hhea + 4) - ttSHORT(info->data, info->hhea + 6);
    return (float)height / fheight;
}

STBTT_DEF float stbtt_ScaleForMappingEmToPixels(const stbtt_fontinfo * info, float pixels)
{
    int unitsPerEm = ttUSHORT(info->data, info->head + 18);
    return pixels / unitsPerEm;
}

STBTT_DEF void stbtt_FreeShape(const stbtt_fontinfo * info, stbtt_vertex * v)
{
    STBTT_free(v, info->userdata);
}

STBTT_DEF stbtt_uint32 stbtt_FindSVGDoc(const stbtt_fontinfo * info, int gl)
{
    int i;
    stbtt_uint32 svg_doc_list = stbtt__get_svg((stbtt_fontinfo *)info);

    int numEntries = ttUSHORT(info->data, svg_doc_list);
    stbtt_uint32 svg_docs = svg_doc_list + 2;

    for(i = 0; i < numEntries; i++) {
        stbtt_uint32 svg_doc = svg_docs + (12 * i);
        if((gl >= ttUSHORT(info->data, svg_doc)) && (gl <= ttUSHORT(info->data, svg_doc + 2)))
            return svg_doc;
    }
    return 0;
}

STBTT_DEF int stbtt_GetGlyphSVG(const stbtt_fontinfo * info, int gl, stbtt_uint32 * svgOfs)
{
    stbtt_uint32 svg_doc;

    if(info->svg == 0)
        return 0;

    svg_doc = stbtt_FindSVGDoc(info, gl);
    if(svg_doc != 0) {
        *svgOfs = info->svg + ttULONG(info->data, svg_doc + 4);
        return ttULONG(info->data, svg_doc + 8);
    }
    else {
        return 0;
    }
}

STBTT_DEF int stbtt_GetCodepointSVG(const stbtt_fontinfo * info, int unicode_codepoint, stbtt_uint32 * svgOfs)
{
    return stbtt_GetGlyphSVG(info, stbtt_FindGlyphIndex(info, unicode_codepoint), svgOfs);
}

//////////////////////////////////////////////////////////////////////////////
//
// программное обеспечение для сглаживания, растеризации
//

STBTT_DEF void stbtt_GetGlyphBitmapBoxSubpixel(const stbtt_fontinfo * font, int glyph, float scale_x, float scale_y,
                                               float shift_x, float shift_y, int * ix0, int * iy0, int * ix1, int * iy1)
{
    int x0 = 0, y0 = 0, x1, y1; // =0 подавляет предупреждение компилятора
    if(!stbtt_GetGlyphBox(font, glyph, &x0, &y0, &x1, &y1)) {
        // например космический персонаж
        if(ix0) *ix0 = 0;
        if(iy0) *iy0 = 0;
        if(ix1) *ix1 = 0;
        if(iy1) *iy1 = 0;
    }
    else {
        // move to integral bboxes (treating pixels as little squares, what pixels get touched)?
        if(ix0) *ix0 = STBTT_ifloor(x0 * scale_x + shift_x);
        if(iy0) *iy0 = STBTT_ifloor(-y1 * scale_y + shift_y);
        if(ix1) *ix1 = STBTT_iceil(x1 * scale_x + shift_x);
        if(iy1) *iy1 = STBTT_iceil(-y0 * scale_y + shift_y);
    }
}

STBTT_DEF void stbtt_GetGlyphBitmapBox(const stbtt_fontinfo * font, int glyph, float scale_x, float scale_y, int * ix0,
                                       int * iy0, int * ix1, int * iy1)
{
    stbtt_GetGlyphBitmapBoxSubpixel(font, glyph, scale_x, scale_y, 0.0f, 0.0f, ix0, iy0, ix1, iy1);
}

STBTT_DEF void stbtt_GetCodepointBitmapBoxSubpixel(const stbtt_fontinfo * font, int codepoint, float scale_x,
                                                   float scale_y, float shift_x, float shift_y, int * ix0, int * iy0, int * ix1, int * iy1)
{
    stbtt_GetGlyphBitmapBoxSubpixel(font, stbtt_FindGlyphIndex(font, codepoint), scale_x, scale_y, shift_x, shift_y, ix0,
                                    iy0, ix1, iy1);
}

STBTT_DEF void stbtt_GetCodepointBitmapBox(const stbtt_fontinfo * font, int codepoint, float scale_x, float scale_y,
                                           int * ix0, int * iy0, int * ix1, int * iy1)
{
    stbtt_GetCodepointBitmapBoxSubpixel(font, codepoint, scale_x, scale_y, 0.0f, 0.0f, ix0, iy0, ix1, iy1);
}

//////////////////////////////////////////////////////////////////////////////
//
//  растеризатор

typedef struct stbtt__hheap_chunk {
    struct stbtt__hheap_chunk * next;
} stbtt__hheap_chunk;

typedef struct stbtt__hheap {
    struct stbtt__hheap_chunk * head;
    void * first_free;
    int    num_remaining_in_head_chunk;
} stbtt__hheap;

static void * stbtt__hheap_alloc(stbtt__hheap * hh, size_t size, void * userdata)
{
    if(hh->first_free) {
        void * p = hh->first_free;
        hh->first_free = *(void **)p;
        return p;
    }
    else {
        if(hh->num_remaining_in_head_chunk == 0) {
            int count = (size < 32 ? STBTT_HEAP_FACTOR_SIZE_32 : size < 128 ? STBTT_HEAP_FACTOR_SIZE_128 :
                         STBTT_HEAP_FACTOR_SIZE_DEFAULT);
            stbtt__hheap_chunk * c = (stbtt__hheap_chunk *)STBTT_malloc(sizeof(stbtt__hheap_chunk) + size * count, userdata);
            if(c == NULL)
                return NULL;
            c->next = hh->head;
            hh->head = c;
            hh->num_remaining_in_head_chunk = count;
        }
        --hh->num_remaining_in_head_chunk;
        return (char *)(hh->head) + sizeof(stbtt__hheap_chunk) + size * hh->num_remaining_in_head_chunk;
    }
}

static void stbtt__hheap_free(stbtt__hheap * hh, void * p)
{
    *(void **)p = hh->first_free;
    hh->first_free = p;
}

static void stbtt__hheap_cleanup(stbtt__hheap * hh, void * userdata)
{
    stbtt__hheap_chunk * c = hh->head;
    while(c) {
        stbtt__hheap_chunk * n = c->next;
        STBTT_free(c, userdata);
        c = n;
    }
}

typedef struct stbtt__edge {
    float x0, y0, x1, y1;
    int invert;
} stbtt__edge;

typedef struct stbtt__active_edge {
    struct stbtt__active_edge * next;
#if STBTT_RASTERIZER_VERSION==1
    int x, dx;
    float ey;
    int direction;
#elif STBTT_RASTERIZER_VERSION==2
    float fx, fdx, fdy;
    float direction;
    float sy;
    float ey;
#else
#error "Unrecognized value of STBTT_RASTERIZER_VERSION"
#endif
} stbtt__active_edge;

#if STBTT_RASTERIZER_VERSION == 1
#define STBTT_FIXSHIFT   10
#define STBTT_FIX        (1 << STBTT_FIXSHIFT)
#define STBTT_FIXMASK    (STBTT_FIX-1)

static stbtt__active_edge * stbtt__new_active(stbtt__hheap * hh, stbtt__edge * e, int off_x, float start_point,
                                              void * userdata)
{
    stbtt__active_edge * z = (stbtt__active_edge *)stbtt__hheap_alloc(hh, sizeof(*z), userdata);
    float dxdy = (e->x1 - e->x0) / (e->y1 - e->y0);
    STBTT_assert(z != NULL);
    if(!z) return z;

    // округляем dx в меньшую сторону, чтобы избежать превышения значения
    if(dxdy < 0)
        z->dx = -STBTT_ifloor(STBTT_FIX * -dxdy);
    else
        z->dx = STBTT_ifloor(STBTT_FIX * dxdy);

    z->x = STBTT_ifloor(STBTT_FIX * e->x0 + z->dx * (start_point -
                                                     e->y0)); // используйте z->dx, чтобы, когда мы позже сместим, оно было на ту же сумму
    z->x -= off_x * STBTT_FIX;

    z->ey = e->y1;
    z->next = 0;
    z->direction = e->invert ? 1 : -1;
    return z;
}
#elif STBTT_RASTERIZER_VERSION == 2
static stbtt__active_edge * stbtt__new_active(stbtt__hheap * hh, stbtt__edge * e, int off_x, float start_point,
                                              void * userdata)
{
    stbtt__active_edge * z = (stbtt__active_edge *)stbtt__hheap_alloc(hh, sizeof(*z), userdata);
    float dxdy = (e->x1 - e->x0) / (e->y1 - e->y0);
    STBTT_assert(z != NULL);
    //STBTT_assert (e->y0 <= start_point );
    if(!z) return z;
    z->fdx = dxdy;
    z->fdy = dxdy != 0.0f ? (1.0f / dxdy) : 0.0f;
    z->fx = e->x0 + dxdy * (start_point - e->y0);
    z->fx -= off_x;
    z->direction = e->invert ? 1.0f : -1.0f;
    z->sy = e->y0;
    z->ey = e->y1;
    z->next = 0;
    return z;
}
#else
#error "Unrecognized value of STBTT_RASTERIZER_VERSION"
#endif

#if STBTT_RASTERIZER_VERSION == 1
// note: this routine clips fills that extend off the edges... ideally this
// этого бы не произошло, но это могло бы произойти, если бы ограничительные рамки глифа truetype
// неверны или если пользователь предоставляет слишком маленькое растровое изображение.
static void stbtt__fill_active_edges(unsigned char * scanline, int len, stbtt__active_edge * e, int max_weight)
{
    // ненулевое заполнение обмотки
    int x0 = 0, w = 0;

    while(e) {
        if(w == 0) {
            // если мы сейчас находимся на нуле, нам нужно записать начальную точку края
            x0 = e->x;
            w += e->direction;
        }
        else {
            int x1 = e->x;
            w += e->direction;
            // если мы дошли до нуля, нам нужно нарисовать
            if(w == 0) {
                int i = x0 >> STBTT_FIXSHIFT;
                int j = x1 >> STBTT_FIXSHIFT;

                if(i < len && j >= 0) {
                    if(i == j) {
                        // x0,x1 — один и тот же пиксель, поэтому вычислите комбинированное покрытие
                        scanline[i] = scanline[i] + (stbtt_uint8)((x1 - x0) * max_weight >> STBTT_FIXSHIFT);
                    }
                    else {
                        if(i >= 0)  // добавить сглаживание для x0
                            scanline[i] = scanline[i] + (stbtt_uint8)(((STBTT_FIX - (x0 & STBTT_FIXMASK)) * max_weight) >> STBTT_FIXSHIFT);
                        else
                            i = -1; // клип

                        if(j < len)  // добавить сглаживание для x1
                            scanline[j] = scanline[j] + (stbtt_uint8)(((x1 & STBTT_FIXMASK) * max_weight) >> STBTT_FIXSHIFT);
                        else
                            j = len; // клип

                        for(++i; i < j; ++i)  // заполнить пиксели между x0 и x1
                            scanline[i] = scanline[i] + (stbtt_uint8)max_weight;
                    }
                }
            }
        }

        e = e->next;
    }
}

static void stbtt__rasterize_sorted_edges(stbtt__bitmap * result, stbtt__edge * e, int n, int vsubsample, int off_x,
                                          int off_y, void * userdata)
{
    stbtt__hheap hh = { 0, 0, 0 };
    stbtt__active_edge * active = NULL;
    int y, j = 0;
    int max_weight = (255 / vsubsample);  // вес на вертикальную строку развертки
    int s; // вертикальный индекс подвыборки
    unsigned char scanline_data[512], * scanline;

    if(result->w > 512)
        scanline = (unsigned char *)STBTT_malloc(result->w, userdata);
    else
        scanline = scanline_data;

    y = off_y * vsubsample;
    e[n].y0 = (off_y + result->h) * (float)vsubsample + 1;

    while(j < result->h) {
        STBTT_memset(scanline, 0, result->w);
        for(s = 0; s < vsubsample; ++s) {
            // найти центр пикселя для этой строки сканирования
            float scan_y = y + 0.5f;
            stbtt__active_edge ** step = &active;

            // обновить все активные ребра;
            // удалить все активные края, которые заканчиваются перед центром этой строки сканирования
            while(*step) {
                stbtt__active_edge * z = *step;
                if(z->ey <= scan_y) {
                    *step = z->next; // удалить из списка
                    STBTT_assert(z->direction);
                    z->direction = 0;
                    stbtt__hheap_free(&hh, z);
                }
                else {
                    z->x += z->dx; // перейти к позиции текущей строки сканирования
                    step = &((*step)->next); // продвигаться по списку
                }
            }

            // при необходимости измените список
            for(;;) {
                int changed = 0;
                step = &active;
                while(*step && (*step)->next) {
                    if((*step)->x > (*step)->next->x) {
                        stbtt__active_edge * t = *step;
                        stbtt__active_edge * q = t->next;

                        t->next = q->next;
                        q->next = t;
                        *step = q;
                        changed = 1;
                    }
                    step = &(*step)->next;
                }
                if(!changed) break;
            }

            // вставьте все ребра, которые начинаются до центра этой строки сканирования, — опустите те, которые также заканчиваются на этой строке сканирования.
            while(e->y0 <= scan_y) {
                if(e->y1 > scan_y) {
                    stbtt__active_edge * z = stbtt__new_active(&hh, e, off_x, scan_y, userdata);
                    if(z != NULL) {
                        // найти точку вставки
                        if(active == NULL)
                            active = z;
                        else if(z->x < active->x) {
                            // вставить спереди
                            z->next = active;
                            active = z;
                        }
                        else {
                            // найди, что можно вставить AFTER
                            stbtt__active_edge * p = active;
                            while(p->next && p->next->x < z->x)
                                p = p->next;
                            // в этот момент p->next->x равно NOT < z->x
                            z->next = p->next;
                            p->next = z;
                        }
                    }
                }
                ++e;
            }

            // теперь обрабатываем все активные края в стиле XOR
            if(active)
                stbtt__fill_active_edges(scanline, result->w, active, max_weight);

            ++y;
        }
        STBTT_memcpy(result->pixels + j * result->stride, scanline, result->w);
        ++j;
    }

    stbtt__hheap_cleanup(&hh, userdata);

    if(scanline != scanline_data)
        STBTT_free(scanline, userdata);
}

#elif STBTT_RASTERIZER_VERSION == 2

// переданное здесь ребро не пересекает вертикальную линию в точке x или вертикальную линию в точке x+1
// (т.е. оно уже было обрезано к ним)
static void stbtt__handle_clipped_edge(float * scanline, int x, stbtt__active_edge * e, float x0, float y0, float x1,
                                       float y1)
{
    if(y0 == y1) return;
    STBTT_assert(y0 < y1);
    STBTT_assert(e->sy <= e->ey);
    if(y0 > e->ey) return;
    if(y1 < e->sy) return;
    if(y0 < e->sy) {
        x0 += (x1 - x0) * (e->sy - y0) / (y1 - y0);
        y0 = e->sy;
    }
    if(y1 > e->ey) {
        x1 += (x1 - x0) * (e->ey - y1) / (y1 - y0);
        y1 = e->ey;
    }

    if(x0 == x)
        STBTT_assert(x1 <= x + 1);
    else if(x0 == x + 1)
        STBTT_assert(x1 >= x);
    else if(x0 <= x)
        STBTT_assert(x1 <= x);
    else if(x0 >= x + 1)
        STBTT_assert(x1 >= x + 1);
    else
        STBTT_assert(x1 >= x && x1 <= x + 1);

    if(x0 <= x && x1 <= x)
        scanline[x] += e->direction * (y1 - y0);
    else if(x0 >= x + 1 && x1 >= x + 1) {
        /*Нечего делать*/;
    }
    else {
        STBTT_assert(x0 >= x && x0 <= x + 1 && x1 >= x && x1 <= x + 1);
        scanline[x] += e->direction * (y1 - y0) * (1 - ((x0 - x) + (x1 - x)) / 2); // coverage = 1 - average x position
    }
}

static float stbtt__sized_trapezoid_area(float height, float top_width, float bottom_width)
{
    STBTT_assert(top_width >= 0);
    STBTT_assert(bottom_width >= 0);
    return (top_width + bottom_width) / 2.0f * height;
}

static float stbtt__position_trapezoid_area(float height, float tx0, float tx1, float bx0, float bx1)
{
    return stbtt__sized_trapezoid_area(height, tx1 - tx0, bx1 - bx0);
}

static float stbtt__sized_triangle_area(float height, float width)
{
    return height * width / 2;
}

static void stbtt__fill_active_edges_new(float * scanline, float * scanline_fill, int len, stbtt__active_edge * e,
                                         float y_top)
{
    float y_bottom = y_top + 1;

    while(e) {
        // перебор каждого пикселя

        // вычислить точки пересечения с верхом и низом
        STBTT_assert(e->ey >= y_top);

        if(e->fdx == 0) {
            float x0 = e->fx;
            if(x0 < len) {
                if(x0 >= 0) {
                    stbtt__handle_clipped_edge(scanline, (int)x0, e, x0, y_top, x0, y_bottom);
                    stbtt__handle_clipped_edge(scanline_fill - 1, (int)x0 + 1, e, x0, y_top, x0, y_bottom);
                }
                else {
                    stbtt__handle_clipped_edge(scanline_fill - 1, 0, e, x0, y_top, x0, y_bottom);
                }
            }
        }
        else {
            float x0 = e->fx;
            float dx = e->fdx;
            float xb = x0 + dx;
            float x_top, x_bottom;
            float sy0, sy1;
            float dy = e->fdy;
            STBTT_assert(e->sy <= y_bottom && e->ey >= y_top);

            // вычислить конечные точки сегмента линии, обрезанного до этой строки сканирования (если
            // сегмент линии начинается на этой строке развертки. x0 — пересечение
            // линия с y_top , но она может находиться за пределами сегмента линии.
            if(e->sy > y_top) {
                x_top = x0 + dx * (e->sy - y_top);
                sy0 = e->sy;
            }
            else {
                x_top = x0;
                sy0 = y_top;
            }
            if(e->ey < y_bottom) {
                x_bottom = x0 + dx * (e->ey - y_top);
                sy1 = e->ey;
            }
            else {
                x_bottom = xb;
                sy1 = y_bottom;
            }

            if(x_top >= 0 && x_bottom >= 0 && x_top < len && x_bottom < len) {
                // с этого момента нам не нужно проверять диапазон значений x

                if((int)x_top == (int)x_bottom) {
                    float height;
                    // простой случай, охватывает только один пиксель
                    int x = (int)x_top;
                    height = (sy1 - sy0) * e->direction;
                    STBTT_assert(x >= 0 && x < len);
                    scanline[x] += stbtt__position_trapezoid_area(height, x_top, x + 1.0f, x_bottom, x + 1.0f);
                    scanline_fill[x] += height; // все справа от этого пикселя заполнено
                }
                else {
                    int x, x1, x2;
                    float y_crossing, y_final, step, sign, area;
                    // охватывает 2+ пикселей
                    if(x_top > x_bottom) {
                        // перевернуть строку развертки по вертикали; подписанная область та же самая
                        float t;
                        sy0 = y_bottom - (sy0 - y_top);
                        sy1 = y_bottom - (sy1 - y_top);
                        t = sy0, sy0 = sy1, sy1 = t;
                        t = x_bottom, x_bottom = x_top, x_top = t;
                        dx = -dx;
                        dy = -dy;
                        t = x0, x0 = xb, xb = t;
                    }
                    STBTT_assert(dy >= 0);
                    STBTT_assert(dx >= 0);

                    x1 = (int)x_top;
                    x2 = (int)x_bottom;
                    // вычислить пересечение с осью y в точке x1+1
                    y_crossing = y_top + dy * (x1 + 1 - x0);

                    // вычислить пересечение с осью y в точке x2
                    y_final = y_top + dy * (x2 - x0);

                    //           x1 x_top x2 x_bottom
                    //     y_top +------|-----+------------+-------------+--------|---+------------+
                    //            |            |            |            |            |            |
                    //            |            |            |            |            |            |
                    //       си0 |      Тххххх|............|...........|...........|............|
                    // y_crossing |            *xxxxxx.......|...........|...........|............|
                    //            |            |     ххххх..|............|............|............|
                    //            |            |     /- xx*xxxx........|............|.....|
                    //            |            | ды < |    xxxxxx..|............|............|
                    //   y_final |            |     \- |          хх*ххх.........|............|
                    //       си1 |            |            |            |   xxxxxxB...|............|
                    //            |            |            |            |            |            |
                    //            |            |            |            |            |            |
                    //  y_bottom +------------+------------+------------+------------+------------+
                    //
                    // Цель состоит в том, чтобы измерить площадь, покрытую '.' в каждом пикселе

                    // если x2 находится прямо у правого края x1, y_crossing может взорваться, github #1057
                    // @TODO: maybe test against sy1 rather than y_bottom?
                    if(y_crossing > y_bottom)
                        y_crossing = y_bottom;

                    sign = e->direction;

                    // площадь прямоугольника, закрытая от sy0.. y_crossing
                    area = sign * (y_crossing - sy0);

                    // площадь треугольника ( x_top ,sy0), (x1+1,sy0), (x1+1, y_crossing )
                    scanline[x1] += stbtt__sized_triangle_area(area, x1 + 1 - x_top);

                    // проверить, не взорван ли финальный y_crossing; нет тестового примера для этого
                    if(y_final > y_bottom) {
                        y_final = y_bottom;
                        dy = (y_final - y_crossing) / (x2 - (x1 + 1)); // если denom=0, y_final = y_crossing , поэтому y_final <= y_bottom
                    }

                    // во втором пикселе, область, покрытая сегментом линии, найденным в первом пикселе
                    // всегда представляет собой прямоугольник шириной 1 * высота этого отрезка; это
                    // это именно то, что хранит переменная «площадь». оно также получает вклад
                    // от сегмента линии внутри него. пиксель THIRD получит первый
                    // вклад прямоугольника пикселя, вклад прямоугольника второго пикселя,
                    // и свой собственный вклад. «собственный вклад» одинаков для каждого пикселя, кроме
                    // крайний левый и крайний правый — трапеция, которая скользит вниз в каждом пикселе.
                    // вклад второго пикселя в третий пиксель будет
                    // Ширина прямоугольника 1 умножает изменение высоты во втором пикселе, который равен dy.

                    step = sign * dy * 1; // dy — это dy/dx, изменение y на каждое 1 изменение x,
                    // которое, умноженное на ширину 1 пиксель, показывает, насколько изменяется площадь пикселя для каждого шага по x.
                    // поэтому область каждый раз продвигается «шагом»

                    for(x = x1 + 1; x < x2; ++x) {
                        scanline[x] += area + step / 2; // площадь трапеции равна 1*шаг/2
                        area += step;
                    }
                    STBTT_assert(STBTT_fabs(area) <= 1.01f); // накопленная ошибка из области += шаг, если мы не округлим шаг вниз
                    STBTT_assert(sy1 > y_final - 0.01f);

                    // область, охватываемая последним пикселем, представляет собой прямоугольник из всех пикселей слева,
                    // плюс трапеция, заполненная отрезком линии в этом пикселе до самого правого края
                    scanline[x2] += area + sign * stbtt__position_trapezoid_area(sy1 - y_final, (float)x2, x2 + 1.0f, x_bottom, x2 + 1.0f);

                    // остальная часть строки заполняется исходя из общей высоты отрезка линии в этом пикселе
                    scanline_fill[x2] += sign * (sy1 - sy0);
                }
            }
            else {
                // если край выходит за пределы рамки, которую мы рисуем, нам требуется
                // логика обрезки. так как это не соответствует предполагаемому использованию
                // этой библиотеки мы используем другой, очень медленный брут
                // применение силы
                // однако обратите внимание, что иногда это случается, потому что
                // x_top и x_bottom можно экстраполировать вверх и вниз.
                // форма и фактически лежат за пределами ограничивающей рамки
                int x;
                for(x = 0; x < len; ++x) {
                    // случаи:
                    //
                    // с пикселем может быть до двух пересечений. любое пересечение
                    // с левым или правым краем можно обрабатывать путем разделения на два (или три)
                    // регионы. пересечения с верхом и низом не требуют логики с учетом регистра.
                    //
                    // старый способ сделать это нашел пересечения с левым и правым краями,
                    // затем использовал простую логику для создания до трех сегментов в отсортированном порядке.
                    // сверху вниз. однако здесь была проблема: если ребро x было эпсилоном
                    // через границу x, то соответствующая позиция y может быть не различима
                    // из другого сегмента y, и его можно игнорировать как пустой сегмент. избегать
                    // то нам нужно явно создавать сегменты на основе позиций x.

                    // переименуйте переменные в четко определенные пары
                    float y0 = y_top;
                    float x1 = (float)(x);
                    float x2 = (float)(x + 1);
                    float x3 = xb;
                    float y3 = y_bottom;

                    // x = e->x + e->dx * (y-y_top)
                    // (y- y_top ) = (x - e->x) / e->dx
                    // y = (x - e->x) / e->dx + y_top
                    float y1 = (x - x0) / dx + y_top;
                    float y2 = (x + 1 - x0) / dx + y_top;

                    if(x0 < x1 && x3 > x2) {          // три сегмента, спускающиеся вниз-вправо
                        stbtt__handle_clipped_edge(scanline, x, e, x0, y0, x1, y1);
                        stbtt__handle_clipped_edge(scanline, x, e, x1, y1, x2, y2);
                        stbtt__handle_clipped_edge(scanline, x, e, x2, y2, x3, y3);
                    }
                    else if(x3 < x1 && x0 > x2) {   // три сегмента, спускающиеся вниз-влево
                        stbtt__handle_clipped_edge(scanline, x, e, x0, y0, x2, y2);
                        stbtt__handle_clipped_edge(scanline, x, e, x2, y2, x1, y1);
                        stbtt__handle_clipped_edge(scanline, x, e, x1, y1, x3, y3);
                    }
                    else if(x0 < x1 && x3 > x1) {   // два сегмента поперек x, вниз-вправо
                        stbtt__handle_clipped_edge(scanline, x, e, x0, y0, x1, y1);
                        stbtt__handle_clipped_edge(scanline, x, e, x1, y1, x3, y3);
                    }
                    else if(x3 < x1 && x0 > x1) {   // два сегмента поперек x, слева внизу
                        stbtt__handle_clipped_edge(scanline, x, e, x0, y0, x1, y1);
                        stbtt__handle_clipped_edge(scanline, x, e, x1, y1, x3, y3);
                    }
                    else if(x0 < x2 && x3 > x2) {   // два сегмента поперек x+1, вниз-вправо
                        stbtt__handle_clipped_edge(scanline, x, e, x0, y0, x2, y2);
                        stbtt__handle_clipped_edge(scanline, x, e, x2, y2, x3, y3);
                    }
                    else if(x3 < x2 && x0 > x2) {   // два сегмента поперек x+1, слева внизу
                        stbtt__handle_clipped_edge(scanline, x, e, x0, y0, x2, y2);
                        stbtt__handle_clipped_edge(scanline, x, e, x2, y2, x3, y3);
                    }
                    else {  // один сегмент
                        stbtt__handle_clipped_edge(scanline, x, e, x0, y0, x3, y3);
                    }
                }
            }
        }
        e = e->next;
    }
}

// непосредственно AA растрирует края без суперсэмплинга
static void stbtt__rasterize_sorted_edges(stbtt__bitmap * result, stbtt__edge * e, int n, int vsubsample, int off_x,
                                          int off_y, void * userdata)
{
    stbtt__hheap hh = { 0, 0, 0 };
    stbtt__active_edge * active = NULL;
    int y, j = 0, i;
    float scanline_data[129], * scanline, * scanline2;

    STBTT__NOTUSED(vsubsample);

    if(result->w > 64)
        scanline = (float *)STBTT_malloc((result->w * 2 + 1) * sizeof(float), userdata);
    else
        scanline = scanline_data;

    scanline2 = scanline + result->w;

    y = off_y;
    e[n].y0 = (float)(off_y + result->h) + 1;

    while(j < result->h) {
        // найти центр пикселя для этой строки сканирования
        float scan_y_top = y + 0.0f;
        float scan_y_bottom = y + 1.0f;
        stbtt__active_edge ** step = &active;

        STBTT_memset(scanline, 0, result->w * sizeof(scanline[0]));
        STBTT_memset(scanline2, 0, (result->w + 1) * sizeof(scanline[0]));

        // обновить все активные ребра;
        // удалить все активные края, которые заканчиваются перед вершиной этой строки сканирования
        while(*step) {
            stbtt__active_edge * z = *step;
            if(z->ey <= scan_y_top) {
                *step = z->next; // удалить из списка
                STBTT_assert(z->direction);
                z->direction = 0;
                stbtt__hheap_free(&hh, z);
            }
            else {
                step = &((*step)->next); // продвигаться по списку
            }
        }

        // вставьте все края, которые начинаются до нижней части этой строки сканирования
        while(e->y0 <= scan_y_bottom) {
            if(e->y0 != e->y1) {
                stbtt__active_edge * z = stbtt__new_active(&hh, e, off_x, scan_y_top, userdata);
                if(z != NULL) {
                    if(j == 0 && off_y != 0) {
                        if(z->ey < scan_y_top) {
                            // это может произойти из-за позиционирования субпикселей и какой-то ошибки округления fp, я думаю
                            z->ey = scan_y_top;
                        }
                    }
                    STBTT_assert(z->ey >= scan_y_top); // если нам действительно не повезет, небольшое преимущество может оказаться за пределами поля зрения
                    // вставить спереди
                    z->next = active;
                    active = z;
                }
            }
            ++e;
        }

        // теперь обработаем все активные края
        if(active)
            stbtt__fill_active_edges_new(scanline, scanline2 + 1, result->w, active, scan_y_top);

        {
            float sum = 0;
            for(i = 0; i < result->w; ++i) {
                float k;
                int m;
                sum += scanline2[i];
                k = scanline[i] + sum;
                k = (float)STBTT_fabs(k) * 255 + 0.5f;
                m = (int)k;
                if(m > 255) m = 255;
                result->pixels[j * result->stride + i] = (unsigned char)m;
            }
        }
        // продвигать все края
        step = &active;
        while(*step) {
            stbtt__active_edge * z = *step;
            z->fx += z->fdx; // перейти к позиции текущей строки сканирования
            step = &((*step)->next); // продвигаться по списку
        }

        ++y;
        ++j;
    }

    stbtt__hheap_cleanup(&hh, userdata);

    if(scanline != scanline_data)
        STBTT_free(scanline, userdata);
}
#else
#error "Unrecognized value of STBTT_RASTERIZER_VERSION"
#endif

#define STBTT__COMPARE(a,b)  ((a)->y0 < (b)->y0)

static void stbtt__sort_edges_ins_sort(stbtt__edge * p, int n)
{
    int i, j;
    for(i = 1; i < n; ++i) {
        stbtt__edge t = p[i], * a = &t;
        j = i;
        while(j > 0) {
            stbtt__edge * b = &p[j - 1];
            int c = STBTT__COMPARE(a, b);
            if(!c) break;
            p[j] = p[j - 1];
            --j;
        }
        if(i != j)
            p[j] = t;
    }
}

static void stbtt__sort_edges_quicksort(stbtt__edge * p, int n)
{
    /* порог перехода к сортировке вставками */
    while(n > 12) {
        stbtt__edge t;
        int c01, c12, c, m, i, j;

        /* вычислить медиану трех */
        m = n >> 1;
        c01 = STBTT__COMPARE(&p[0], &p[m]);
        c12 = STBTT__COMPARE(&p[m], &p[n - 1]);
        /* если 0 >= середина >= конец или 0 < середина < конец, то используйте середину */
        if(c01 != c12) {
            /* в противном случае нам нужно будет заменить что-то еще на средний */
            int z;
            c = STBTT__COMPARE(&p[0], &p[n - 1]);
            /* 0>mid && middle<n: 0>n => n; 0<n => 0 */
            /* 0<mid && middle>n: 0>n => 0; 0<n => n */
            z = (c == c12) ? 0 : n - 1;
            t = p[z];
            p[z] = p[m];
            p[m] = t;
        }
        /* теперь p[m] — медиана трех */
        /* поменяй его на начало, чтобы он не двигался */
        t = p[0];
        p[0] = p[m];
        p[m] = t;

        /* цикл разделения */
        i = 1;
        j = n - 1;
        for(;;) {
            /* обеспечение равенства здесь имеет решающее значение */
            /* для дозорных и эффективности с дубликатами */
            for(;; ++i) {
                if(!STBTT__COMPARE(&p[i], &p[0])) break;
            }
            for(;; --j) {
                if(!STBTT__COMPARE(&p[0], &p[j])) break;
            }
            /* убедиться, что мы не пересеклись */
            if(i >= j) break;
            t = p[i];
            p[i] = p[j];
            p[j] = t;

            ++i;
            --j;
        }
        /* рекурсия по меньшей стороне, повторение по большей */
        if(j < (n - i)) {
            stbtt__sort_edges_quicksort(p, j);
            p = p + i;
            n = n - i;
        }
        else {
            stbtt__sort_edges_quicksort(p + i, n - i);
            n = j;
        }
    }
}

static void stbtt__sort_edges(stbtt__edge * p, int n)
{
    stbtt__sort_edges_quicksort(p, n);
    stbtt__sort_edges_ins_sort(p, n);
}

typedef struct {
    float x, y;
} stbtt__point;

static void stbtt__rasterize(stbtt__bitmap * result, stbtt__point * pts, int * wcount, int windings, float scale_x,
                             float scale_y, float shift_x, float shift_y, int off_x, int off_y, int invert, void * userdata)
{
    float y_scale_inv = invert ? -scale_y : scale_y;
    stbtt__edge * e;
    int n, i, j, k, m;
#if STBTT_RASTERIZER_VERSION == 1
    int vsubsample = result->h < 8 ? 15 : 5;
#elif STBTT_RASTERIZER_VERSION == 2
    int vsubsample = 1;
#else
#error "Unrecognized value of STBTT_RASTERIZER_VERSION"
#endif
    // vssubsample должен делить 255 поровну; иначе мы не достигнем полной непрозрачности

    // теперь нам нужно разбить обмотки на явные списки ребер
    n = 0;
    for(i = 0; i < windings; ++i)
        n += wcount[i];

    e = (stbtt__edge *)STBTT_malloc(sizeof(*e) * (n + 1), userdata); // добавить еще одного в качестве дозорного
    if(e == 0) return;
    n = 0;

    m = 0;
    for(i = 0; i < windings; ++i) {
        stbtt__point * p = pts + m;
        m += wcount[i];
        j = wcount[i] - 1;
        for(k = 0; k < wcount[i]; j = k++) {
            int a = k, b = j;
            // пропустить край, если он горизонтальный
            if(p[j].y == p[k].y)
                continue;
            // добавить ребро от j до k в список
            e[n].invert = 0;
            if(invert ? p[j].y > p[k].y : p[j].y < p[k].y) {
                e[n].invert = 1;
                a = j, b = k;
            }
            e[n].x0 = p[a].x * scale_x + shift_x;
            e[n].y0 = (p[a].y * y_scale_inv + shift_y) * vsubsample;
            e[n].x1 = p[b].x * scale_x + shift_x;
            e[n].y1 = (p[b].y * y_scale_inv + shift_y) * vsubsample;
            ++n;
        }
    }

    // теперь отсортируйте края по их самой высокой точке (должно быть привязано к целому числу, а затем по x)
    //STBTT_sort (e, n, sizeof(e[0]), stbtt__edge_compare );
    stbtt__sort_edges(e, n);

    // теперь пройдите по строкам сканирования и найдите пересечения на каждой строке сканирования, используйте правило обмотки xor
    stbtt__rasterize_sorted_edges(result, e, n, vsubsample, off_x, off_y, userdata);

    STBTT_free(e, userdata);
}

static void stbtt__add_point(stbtt__point * points, int n, float x, float y)
{
    if(!points) return;  // во время первого прохода он нераспределен
    points[n].x = x;
    points[n].y = y;
}

// тесселяция до тех пор, пока порог p не станет удовлетворительным... @TODO деформирован, чтобы компенсировать нелинейное растяжение
static int stbtt__tesselate_curve(stbtt__point * points, int * num_points, float x0, float y0, float x1, float y1,
                                  float x2, float y2, float objspace_flatness_squared, int n)
{
    // середина
    float mx = (x0 + 2 * x1 + x2) / 4;
    float my = (y0 + 2 * y1 + y2) / 4;
    // по сравнению с прямо нарисованной линией
    float dx = (x0 + x2) / 2 - mx;
    float dy = (y0 + y2) / 2 - my;
    if(n > 16)  // 65536 сегментов на одной кривой будет достаточно!
        return 1;
    if(dx * dx + dy * dy > objspace_flatness_squared) {  // допускается ошибка в полпикселя... должно быть меньше, если AA
        stbtt__tesselate_curve(points, num_points, x0, y0, (x0 + x1) / 2.0f, (y0 + y1) / 2.0f, mx, my,
                               objspace_flatness_squared, n + 1);
        stbtt__tesselate_curve(points, num_points, mx, my, (x1 + x2) / 2.0f, (y1 + y2) / 2.0f, x2, y2,
                               objspace_flatness_squared, n + 1);
    }
    else {
        stbtt__add_point(points, *num_points, x2, y2);
        *num_points = *num_points + 1;
    }
    return 1;
}

static void stbtt__tesselate_cubic(stbtt__point * points, int * num_points, float x0, float y0, float x1, float y1,
                                   float x2, float y2, float x3, float y3, float objspace_flatness_squared, int n)
{
    // @TODO этот расчет «плоскостности» — просто выдуманная ерунда, которая, похоже, работает достаточно хорошо.
    float dx0 = x1 - x0;
    float dy0 = y1 - y0;
    float dx1 = x2 - x1;
    float dy1 = y2 - y1;
    float dx2 = x3 - x2;
    float dy2 = y3 - y2;
    float dx = x3 - x0;
    float dy = y3 - y0;
    float longlen = (float)(STBTT_sqrt(dx0 * dx0 + dy0 * dy0) + STBTT_sqrt(dx1 * dx1 + dy1 * dy1) + STBTT_sqrt(
                                dx2 * dx2 + dy2 * dy2));
    float shortlen = (float)STBTT_sqrt(dx * dx + dy * dy);
    float flatness_squared = longlen * longlen - shortlen * shortlen;

    if(n > 16)  // 65536 сегментов на одной кривой будет достаточно!
        return;

    if(flatness_squared > objspace_flatness_squared) {
        float x01 = (x0 + x1) / 2;
        float y01 = (y0 + y1) / 2;
        float x12 = (x1 + x2) / 2;
        float y12 = (y1 + y2) / 2;
        float x23 = (x2 + x3) / 2;
        float y23 = (y2 + y3) / 2;

        float xa = (x01 + x12) / 2;
        float ya = (y01 + y12) / 2;
        float xb = (x12 + x23) / 2;
        float yb = (y12 + y23) / 2;

        float mx = (xa + xb) / 2;
        float my = (ya + yb) / 2;

        stbtt__tesselate_cubic(points, num_points, x0, y0, x01, y01, xa, ya, mx, my, objspace_flatness_squared, n + 1);
        stbtt__tesselate_cubic(points, num_points, mx, my, xb, yb, x23, y23, x3, y3, objspace_flatness_squared, n + 1);
    }
    else {
        stbtt__add_point(points, *num_points, x3, y3);
        *num_points = *num_points + 1;
    }
}

// возвращает количество контуров
static stbtt__point * stbtt_FlattenCurves(stbtt_vertex * vertices, int num_verts, float objspace_flatness,
                                          int ** contour_lengths, int * num_contours, void * userdata)
{
    stbtt__point * points = 0;
    int num_points = 0;

    float objspace_flatness_squared = objspace_flatness * objspace_flatness;
    int i, n = 0, start = 0, pass;

    // подсчитайте, сколько «ходов» нужно, чтобы получить количество контуров
    for(i = 0; i < num_verts; ++i)
        if(vertices[i].type == STBTT_vmove)
            ++n;

    *num_contours = n;
    if(n == 0) return 0;

    *contour_lengths = (int *)STBTT_malloc(sizeof(**contour_lengths) * n, userdata);

    if(*contour_lengths == 0) {
        *num_contours = 0;
        return 0;
    }

    // сделайте два прохода по точкам, чтобы нам не нужно было перераспределять
    for(pass = 0; pass < 2; ++pass) {
        float x = 0, y = 0;
        if(pass == 1) {
            points = (stbtt__point *)STBTT_malloc(num_points * sizeof(points[0]), userdata);
            if(points == NULL) goto error;
        }
        num_points = 0;
        n = -1;
        for(i = 0; i < num_verts; ++i) {
            switch(vertices[i].type) {
                case STBTT_vmove:
                    // начать следующий контур
                    if(n >= 0)
                        (*contour_lengths)[n] = num_points - start;
                    ++n;
                    start = num_points;

                    x = vertices[i].x, y = vertices[i].y;
                    stbtt__add_point(points, num_points++, x, y);
                    break;
                case STBTT_vline:
                    x = vertices[i].x, y = vertices[i].y;
                    stbtt__add_point(points, num_points++, x, y);
                    break;
                case STBTT_vcurve:
                    stbtt__tesselate_curve(points, &num_points, x, y,
                                           vertices[i].cx, vertices[i].cy,
                                           vertices[i].x, vertices[i].y,
                                           objspace_flatness_squared, 0);
                    x = vertices[i].x, y = vertices[i].y;
                    break;
                case STBTT_vcubic:
                    stbtt__tesselate_cubic(points, &num_points, x, y,
                                           vertices[i].cx, vertices[i].cy,
                                           vertices[i].cx1, vertices[i].cy1,
                                           vertices[i].x, vertices[i].y,
                                           objspace_flatness_squared, 0);
                    x = vertices[i].x, y = vertices[i].y;
                    break;
            }
        }
        (*contour_lengths)[n] = num_points - start;
    }

    return points;
error:
    STBTT_free(points, userdata);
    STBTT_free(*contour_lengths, userdata);
    *contour_lengths = 0;
    *num_contours = 0;
    return NULL;
}

STBTT_DEF void stbtt_Rasterize(stbtt__bitmap * result, float flatness_in_pixels, stbtt_vertex * vertices, int num_verts,
                               float scale_x, float scale_y, float shift_x, float shift_y, int x_off, int y_off, int invert, void * userdata)
{
    float scale = scale_x > scale_y ? scale_y : scale_x;
    int winding_count = 0;
    int * winding_lengths = NULL;
    stbtt__point * windings = stbtt_FlattenCurves(vertices, num_verts, flatness_in_pixels / scale, &winding_lengths,
                                                  &winding_count, userdata);
    if(windings) {
        stbtt__rasterize(result, windings, winding_lengths, winding_count, scale_x, scale_y, shift_x, shift_y, x_off, y_off,
                         invert, userdata);
        STBTT_free(winding_lengths, userdata);
        STBTT_free(windings, userdata);
    }
}

STBTT_DEF void stbtt_FreeBitmap(unsigned char * bitmap, void * userdata)
{
    STBTT_free(bitmap, userdata);
}

STBTT_DEF unsigned char * stbtt_GetGlyphBitmapSubpixel(const stbtt_fontinfo * info, float scale_x, float scale_y,
                                                       float shift_x, float shift_y, int glyph, int * width, int * height, int * xoff, int * yoff)
{
    int ix0, iy0, ix1, iy1;
    stbtt__bitmap gbm;
    stbtt_vertex * vertices;
    int num_verts = stbtt_GetGlyphShape(info, glyph, &vertices);

    if(scale_x == 0) scale_x = scale_y;
    if(scale_y == 0) {
        if(scale_x == 0) {
            STBTT_free(vertices, info->userdata);
            return NULL;
        }
        scale_y = scale_x;
    }

    stbtt_GetGlyphBitmapBoxSubpixel(info, glyph, scale_x, scale_y, shift_x, shift_y, &ix0, &iy0, &ix1, &iy1);

    // теперь мы получаем размер
    gbm.w = (ix1 - ix0);
    gbm.h = (iy1 - iy0);
    gbm.pixels = NULL; // на случай ошибки

    if(width) *width = gbm.w;
    if(height) *height = gbm.h;
    if(xoff) *xoff = ix0;
    if(yoff) *yoff = iy0;

    if(gbm.w && gbm.h) {
        gbm.pixels = (unsigned char *)STBTT_malloc(gbm.w * gbm.h, info->userdata);
        if(gbm.pixels) {
            gbm.stride = gbm.w;

            stbtt_Rasterize(&gbm, 0.35f, vertices, num_verts, scale_x, scale_y, shift_x, shift_y, ix0, iy0, 1, info->userdata);
        }
    }
    STBTT_free(vertices, info->userdata);
    return gbm.pixels;
}

STBTT_DEF unsigned char * stbtt_GetGlyphBitmap(const stbtt_fontinfo * info, float scale_x, float scale_y, int glyph,
                                               int * width, int * height, int * xoff, int * yoff)
{
    return stbtt_GetGlyphBitmapSubpixel(info, scale_x, scale_y, 0.0f, 0.0f, glyph, width, height, xoff, yoff);
}

STBTT_DEF void stbtt_MakeGlyphBitmapSubpixel(const stbtt_fontinfo * info, unsigned char * output, int out_w, int out_h,
                                             int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int glyph)
{
    int ix0, iy0;
    stbtt_vertex * vertices;
    int num_verts = stbtt_GetGlyphShape(info, glyph, &vertices);
    stbtt__bitmap gbm;

    stbtt_GetGlyphBitmapBoxSubpixel(info, glyph, scale_x, scale_y, shift_x, shift_y, &ix0, &iy0, 0, 0);
    gbm.pixels = output;
    gbm.w = out_w;
    gbm.h = out_h;
    gbm.stride = out_stride;

    if(gbm.w && gbm.h)
        stbtt_Rasterize(&gbm, 0.35f, vertices, num_verts, scale_x, scale_y, shift_x, shift_y, ix0, iy0, 1, info->userdata);

    STBTT_free(vertices, info->userdata);
}

STBTT_DEF void stbtt_MakeGlyphBitmap(const stbtt_fontinfo * info, unsigned char * output, int out_w, int out_h,
                                     int out_stride, float scale_x, float scale_y, int glyph)
{
    stbtt_MakeGlyphBitmapSubpixel(info, output, out_w, out_h, out_stride, scale_x, scale_y, 0.0f, 0.0f, glyph);
}

STBTT_DEF unsigned char * stbtt_GetCodepointBitmapSubpixel(const stbtt_fontinfo * info, float scale_x, float scale_y,
                                                           float shift_x, float shift_y, int codepoint, int * width, int * height, int * xoff, int * yoff)
{
    return stbtt_GetGlyphBitmapSubpixel(info, scale_x, scale_y, shift_x, shift_y, stbtt_FindGlyphIndex(info, codepoint),
                                        width, height, xoff, yoff);
}

STBTT_DEF void stbtt_MakeCodepointBitmapSubpixelPrefilter(const stbtt_fontinfo * info, unsigned char * output,
                                                          int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int oversample_x,
                                                          int oversample_y, float * sub_x, float * sub_y, int codepoint)
{
    stbtt_MakeGlyphBitmapSubpixelPrefilter(info, output, out_w, out_h, out_stride, scale_x, scale_y, shift_x, shift_y,
                                           oversample_x, oversample_y, sub_x, sub_y, stbtt_FindGlyphIndex(info, codepoint));
}

STBTT_DEF void stbtt_MakeCodepointBitmapSubpixel(const stbtt_fontinfo * info, unsigned char * output, int out_w,
                                                 int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int codepoint)
{
    stbtt_MakeGlyphBitmapSubpixel(info, output, out_w, out_h, out_stride, scale_x, scale_y, shift_x, shift_y,
                                  stbtt_FindGlyphIndex(info, codepoint));
}

STBTT_DEF unsigned char * stbtt_GetCodepointBitmap(const stbtt_fontinfo * info, float scale_x, float scale_y,
                                                   int codepoint, int * width, int * height, int * xoff, int * yoff)
{
    return stbtt_GetCodepointBitmapSubpixel(info, scale_x, scale_y, 0.0f, 0.0f, codepoint, width, height, xoff, yoff);
}

STBTT_DEF void stbtt_MakeCodepointBitmap(const stbtt_fontinfo * info, unsigned char * output, int out_w, int out_h,
                                         int out_stride, float scale_x, float scale_y, int codepoint)
{
    stbtt_MakeCodepointBitmapSubpixel(info, output, out_w, out_h, out_stride, scale_x, scale_y, 0.0f, 0.0f, codepoint);
}

//////////////////////////////////////////////////////////////////////////////
//
// запекание растровых изображений
//
// Это упаковка SUPER - CRAPPY, чтобы исходный код был небольшим.
#ifdef STBTT_STREAM_TYPE
static int stbtt_BakeFontBitmap_internal(STBTT_STREAM_TYPE data,
                                         int offset,  // расположение шрифта (используйте смещение = 0 для обычного .ttf)
                                         float pixel_height,                     // высота шрифта в пикселях
                                         unsigned char * pixels, int pw, int ph, // растровое изображение для заполнения
                                         int first_char, int num_chars,          // персонажи для запекания
                                         stbtt_bakedchar * chardata)
#else
static int stbtt_BakeFontBitmap_internal(unsigned char * data,
                                         int offset, // расположение шрифта (используйте смещение = 0 для обычного .ttf)
                                         float pixel_height,                     // высота шрифта в пикселях
                                         unsigned char * pixels, int pw, int ph, // растровое изображение для заполнения
                                         int first_char, int num_chars,          // персонажи для запекания
                                         stbtt_bakedchar * chardata)
#endif
{
    float scale;
    int x, y, bottom_y, i;
    stbtt_fontinfo f;
    f.userdata = NULL;
    if(!stbtt_InitFont(&f, data, offset))
        return -1;
    STBTT_memset(pixels, 0, pw * ph); // фон 0 вокруг пикселей
    x = y = 1;
    bottom_y = 1;

    scale = stbtt_ScaleForPixelHeight(&f, pixel_height);

    for(i = 0; i < num_chars; ++i) {
        int advance, lsb, x0, y0, x1, y1, gw, gh;
        int g = stbtt_FindGlyphIndex(&f, first_char + i);
        stbtt_GetGlyphHMetrics(&f, g, &advance, &lsb);
        stbtt_GetGlyphBitmapBox(&f, g, scale, scale, &x0, &y0, &x1, &y1);
        gw = x1 - x0;
        gh = y1 - y0;
        if(x + gw + 1 >= pw)
            y = bottom_y, x = 1; // перейти к следующей строке
        if(y + gh + 1 >= ph)  // проверьте, соответствует ли он вертикально AFTER, возможно, перейдет к следующей строке
            return -i;
        STBTT_assert(x + gw < pw);
        STBTT_assert(y + gh < ph);
        stbtt_MakeGlyphBitmap(&f, pixels + x + y * pw, gw, gh, pw, scale, scale, g);
        chardata[i].x0 = (stbtt_int16)x;
        chardata[i].y0 = (stbtt_int16)y;
        chardata[i].x1 = (stbtt_int16)(x + gw);
        chardata[i].y1 = (stbtt_int16)(y + gh);
        chardata[i].xadvance = scale * advance;
        chardata[i].xoff = (float)x0;
        chardata[i].yoff = (float)y0;
        x = x + gw + 1;
        if(y + gh + 1 > bottom_y)
            bottom_y = y + gh + 1;
    }
    return bottom_y;
}

STBTT_DEF void stbtt_GetBakedQuad(const stbtt_bakedchar * chardata, int pw, int ph, int char_index, float * xpos,
                                  float * ypos, stbtt_aligned_quad * q, int opengl_fillrule)
{
    float d3d_bias = opengl_fillrule ? 0 : -0.5f;
    float ipw = 1.0f / pw, iph = 1.0f / ph;
    const stbtt_bakedchar * b = chardata + char_index;
    int round_x = STBTT_ifloor((*xpos + b->xoff) + 0.5f);
    int round_y = STBTT_ifloor((*ypos + b->yoff) + 0.5f);

    q->x0 = round_x + d3d_bias;
    q->y0 = round_y + d3d_bias;
    q->x1 = round_x + b->x1 - b->x0 + d3d_bias;
    q->y1 = round_y + b->y1 - b->y0 + d3d_bias;

    q->s0 = b->x0 * ipw;
    q->t0 = b->y0 * iph;
    q->s1 = b->x1 * ipw;
    q->t1 = b->y1 * iph;

    *xpos += b->xadvance;
}

//////////////////////////////////////////////////////////////////////////////
//
// процедуры замены прямоугольной упаковки, если у вас нет stb_rect_pack.h
//

#ifndef STB_RECT_PACK_VERSION

typedef int stbrp_coord;

////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
//                                                                                //
// COMPILER WARNING ?!?!?                                                         //
//                                                                                //
//                                                                                //
// если вы получите предупреждение компиляции из-за того, что эти символы определены более //
// один раз переместите #include " stb_rect_pack.h " перед #include " stb_truetype.h " //
//                                                                                //
////////////////////////////////////////////////////////////////////////////////////

typedef struct {
    int width, height;
    int x, y, bottom_y;
} stbrp_context;

typedef struct {
    unsigned char x;
} stbrp_node;

struct stbrp_rect {
    stbrp_coord x, y;
    int id, w, h, was_packed;
};

static void stbrp_init_target(stbrp_context * con, int pw, int ph, stbrp_node * nodes, int num_nodes)
{
    con->width = pw;
    con->height = ph;
    con->x = 0;
    con->y = 0;
    con->bottom_y = 0;
    STBTT__NOTUSED(nodes);
    STBTT__NOTUSED(num_nodes);
}

static void stbrp_pack_rects(stbrp_context * con, stbrp_rect * rects, int num_rects)
{
    int i;
    for(i = 0; i < num_rects; ++i) {
        if(con->x + rects[i].w > con->width) {
            con->x = 0;
            con->y = con->bottom_y;
        }
        if(con->y + rects[i].h > con->height)
            break;
        rects[i].x = con->x;
        rects[i].y = con->y;
        rects[i].was_packed = 1;
        con->x += rects[i].w;
        if(con->y + rects[i].h > con->bottom_y)
            con->bottom_y = con->y + rects[i].h;
    }
    for(; i < num_rects; ++i)
        rects[i].was_packed = 0;
}
#endif

//////////////////////////////////////////////////////////////////////////////
//
// запекание растровых изображений
//
// Это упаковка SUPER — AWESOME (tm Ryan Gordon) с использованием stb_rect_pack.h. Если
// stb_rect_pack.h недоступен, он использует стратегию BakeFontBitmap.

STBTT_DEF int stbtt_PackBegin(stbtt_pack_context * spc, unsigned char * pixels, int pw, int ph, int stride_in_bytes,
                              int padding, void * alloc_context)
{
    stbrp_context * context = (stbrp_context *)STBTT_malloc(sizeof(*context), alloc_context);
    int            num_nodes = pw - padding;
    stbrp_node * nodes = (stbrp_node *)STBTT_malloc(sizeof(*nodes) * num_nodes, alloc_context);

    if(context == NULL || nodes == NULL) {
        if(context != NULL) STBTT_free(context, alloc_context);
        if(nodes != NULL) STBTT_free(nodes, alloc_context);
        return 0;
    }

    spc->user_allocator_context = alloc_context;
    spc->width = pw;
    spc->height = ph;
    spc->pixels = pixels;
    spc->pack_info = context;
    spc->nodes = nodes;
    spc->padding = padding;
    spc->stride_in_bytes = stride_in_bytes != 0 ? stride_in_bytes : pw;
    spc->h_oversample = 1;
    spc->v_oversample = 1;
    spc->skip_missing = 0;

    stbrp_init_target(context, pw - padding, ph - padding, nodes, num_nodes);

    if(pixels)
        STBTT_memset(pixels, 0, pw * ph); // фон 0 вокруг пикселей

    return 1;
}

STBTT_DEF void stbtt_PackEnd(stbtt_pack_context * spc)
{
    STBTT_free(spc->nodes, spc->user_allocator_context);
    STBTT_free(spc->pack_info, spc->user_allocator_context);
}

STBTT_DEF void stbtt_PackSetOversampling(stbtt_pack_context * spc, unsigned int h_oversample, unsigned int v_oversample)
{
    STBTT_assert(h_oversample <= STBTT_MAX_OVERSAMPLE);
    STBTT_assert(v_oversample <= STBTT_MAX_OVERSAMPLE);
    if(h_oversample <= STBTT_MAX_OVERSAMPLE)
        spc->h_oversample = h_oversample;
    if(v_oversample <= STBTT_MAX_OVERSAMPLE)
        spc->v_oversample = v_oversample;
}

STBTT_DEF void stbtt_PackSetSkipMissingCodepoints(stbtt_pack_context * spc, int skip)
{
    spc->skip_missing = skip;
}

#define STBTT__OVER_MASK  (STBTT_MAX_OVERSAMPLE-1)

static void stbtt__h_prefilter(unsigned char * pixels, int w, int h, int stride_in_bytes, unsigned int kernel_width)
{
    unsigned char buffer[STBTT_MAX_OVERSAMPLE];
    int safe_w = w - kernel_width;
    int j;
    STBTT_memset(buffer, 0, STBTT_MAX_OVERSAMPLE); // подавить ложное предупреждение от VS2013 -analyze
    for(j = 0; j < h; ++j) {
        int i;
        unsigned int total;
        STBTT_memset(buffer, 0, kernel_width);

        total = 0;

        // в общих случаях сделайте kernel_width константой, чтобы компилятор мог оптимизировать деление
        switch(kernel_width) {
            case 2:
                for(i = 0; i <= safe_w; ++i) {
                    total += pixels[i] - buffer[i & STBTT__OVER_MASK];
                    buffer[(i + kernel_width) & STBTT__OVER_MASK] = pixels[i];
                    pixels[i] = (unsigned char)(total / 2);
                }
                break;
            case 3:
                for(i = 0; i <= safe_w; ++i) {
                    total += pixels[i] - buffer[i & STBTT__OVER_MASK];
                    buffer[(i + kernel_width) & STBTT__OVER_MASK] = pixels[i];
                    pixels[i] = (unsigned char)(total / 3);
                }
                break;
            case 4:
                for(i = 0; i <= safe_w; ++i) {
                    total += pixels[i] - buffer[i & STBTT__OVER_MASK];
                    buffer[(i + kernel_width) & STBTT__OVER_MASK] = pixels[i];
                    pixels[i] = (unsigned char)(total / 4);
                }
                break;
            case 5:
                for(i = 0; i <= safe_w; ++i) {
                    total += pixels[i] - buffer[i & STBTT__OVER_MASK];
                    buffer[(i + kernel_width) & STBTT__OVER_MASK] = pixels[i];
                    pixels[i] = (unsigned char)(total / 5);
                }
                break;
            default:
                for(i = 0; i <= safe_w; ++i) {
                    total += pixels[i] - buffer[i & STBTT__OVER_MASK];
                    buffer[(i + kernel_width) & STBTT__OVER_MASK] = pixels[i];
                    pixels[i] = (unsigned char)(total / kernel_width);
                }
                break;
        }

        for(; i < w; ++i) {
            STBTT_assert(pixels[i] == 0);
            total -= buffer[i & STBTT__OVER_MASK];
            pixels[i] = (unsigned char)(total / kernel_width);
        }

        pixels += stride_in_bytes;
    }
}

static void stbtt__v_prefilter(unsigned char * pixels, int w, int h, int stride_in_bytes, unsigned int kernel_width)
{
    unsigned char buffer[STBTT_MAX_OVERSAMPLE];
    int safe_h = h - kernel_width;
    int j;
    STBTT_memset(buffer, 0, STBTT_MAX_OVERSAMPLE); // подавить ложное предупреждение от VS2013 -analyze
    for(j = 0; j < w; ++j) {
        int i;
        unsigned int total;
        STBTT_memset(buffer, 0, kernel_width);

        total = 0;

        // в общих случаях сделайте kernel_width константой, чтобы компилятор мог оптимизировать деление
        switch(kernel_width) {
            case 2:
                for(i = 0; i <= safe_h; ++i) {
                    total += pixels[i * stride_in_bytes] - buffer[i & STBTT__OVER_MASK];
                    buffer[(i + kernel_width) & STBTT__OVER_MASK] = pixels[i * stride_in_bytes];
                    pixels[i * stride_in_bytes] = (unsigned char)(total / 2);
                }
                break;
            case 3:
                for(i = 0; i <= safe_h; ++i) {
                    total += pixels[i * stride_in_bytes] - buffer[i & STBTT__OVER_MASK];
                    buffer[(i + kernel_width) & STBTT__OVER_MASK] = pixels[i * stride_in_bytes];
                    pixels[i * stride_in_bytes] = (unsigned char)(total / 3);
                }
                break;
            case 4:
                for(i = 0; i <= safe_h; ++i) {
                    total += pixels[i * stride_in_bytes] - buffer[i & STBTT__OVER_MASK];
                    buffer[(i + kernel_width) & STBTT__OVER_MASK] = pixels[i * stride_in_bytes];
                    pixels[i * stride_in_bytes] = (unsigned char)(total / 4);
                }
                break;
            case 5:
                for(i = 0; i <= safe_h; ++i) {
                    total += pixels[i * stride_in_bytes] - buffer[i & STBTT__OVER_MASK];
                    buffer[(i + kernel_width) & STBTT__OVER_MASK] = pixels[i * stride_in_bytes];
                    pixels[i * stride_in_bytes] = (unsigned char)(total / 5);
                }
                break;
            default:
                for(i = 0; i <= safe_h; ++i) {
                    total += pixels[i * stride_in_bytes] - buffer[i & STBTT__OVER_MASK];
                    buffer[(i + kernel_width) & STBTT__OVER_MASK] = pixels[i * stride_in_bytes];
                    pixels[i * stride_in_bytes] = (unsigned char)(total / kernel_width);
                }
                break;
        }

        for(; i < h; ++i) {
            STBTT_assert(pixels[i * stride_in_bytes] == 0);
            total -= buffer[i & STBTT__OVER_MASK];
            pixels[i * stride_in_bytes] = (unsigned char)(total / kernel_width);
        }

        pixels += 1;
    }
}

static float stbtt__oversample_shift(int oversample)
{
    if(!oversample)
        return 0.0f;

    // Предварительный фильтр представляет собой коробчатый фильтр ширины «передискретизации»,
    // который сдвигает фазу на (передискретизацию - 1)/2 пикселя в
    // пространство с передискретизацией. Мы хотим сдвинуться в противоположном направлении
    // направление противодействия этому.
    return (float) - (oversample - 1) / (2.0f * (float)oversample);
}

// Массив rects должен быть достаточно большим, чтобы вместить все символы в заданных диапазонах.
STBTT_DEF int stbtt_PackFontRangesGatherRects(stbtt_pack_context * spc, const stbtt_fontinfo * info,
                                              stbtt_pack_range * ranges, int num_ranges, stbrp_rect * rects)
{
    int i, j, k;
    int missing_glyph_added = 0;

    k = 0;
    for(i = 0; i < num_ranges; ++i) {
        float fh = ranges[i].font_size;
        float scale = fh > 0 ? stbtt_ScaleForPixelHeight(info, fh) : stbtt_ScaleForMappingEmToPixels(info, -fh);
        ranges[i].h_oversample = (unsigned char)spc->h_oversample;
        ranges[i].v_oversample = (unsigned char)spc->v_oversample;
        for(j = 0; j < ranges[i].num_chars; ++j) {
            int x0, y0, x1, y1;
            int codepoint = ranges[i].array_of_unicode_codepoints == NULL ? ranges[i].first_unicode_codepoint_in_range + j :
                            ranges[i].array_of_unicode_codepoints[j];
            int glyph = stbtt_FindGlyphIndex(info, codepoint);
            if(glyph == 0 && (spc->skip_missing || missing_glyph_added)) {
                rects[k].w = rects[k].h = 0;
            }
            else {
                stbtt_GetGlyphBitmapBoxSubpixel(info, glyph,
                                                scale * spc->h_oversample,
                                                scale * spc->v_oversample,
                                                0, 0,
                                                &x0, &y0, &x1, &y1);
                rects[k].w = (stbrp_coord)(x1 - x0 + spc->padding + spc->h_oversample - 1);
                rects[k].h = (stbrp_coord)(y1 - y0 + spc->padding + spc->v_oversample - 1);
                if(glyph == 0)
                    missing_glyph_added = 1;
            }
            ++k;
        }
    }

    return k;
}

STBTT_DEF void stbtt_MakeGlyphBitmapSubpixelPrefilter(const stbtt_fontinfo * info, unsigned char * output, int out_w,
                                                      int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int prefilter_x, int prefilter_y,
                                                      float * sub_x, float * sub_y, int glyph)
{
    stbtt_MakeGlyphBitmapSubpixel(info,
                                  output,
                                  out_w - (prefilter_x - 1),
                                  out_h - (prefilter_y - 1),
                                  out_stride,
                                  scale_x,
                                  scale_y,
                                  shift_x,
                                  shift_y,
                                  glyph);

    if(prefilter_x > 1)
        stbtt__h_prefilter(output, out_w, out_h, out_stride, prefilter_x);

    if(prefilter_y > 1)
        stbtt__v_prefilter(output, out_w, out_h, out_stride, prefilter_y);

    *sub_x = stbtt__oversample_shift(prefilter_x);
    *sub_y = stbtt__oversample_shift(prefilter_y);
}

// Массив rects должен быть достаточно большим, чтобы вместить все символы в заданных диапазонах.
STBTT_DEF int stbtt_PackFontRangesRenderIntoRects(stbtt_pack_context * spc, const stbtt_fontinfo * info,
                                                  stbtt_pack_range * ranges, int num_ranges, stbrp_rect * rects)
{
    int i, j, k, missing_glyph = -1, return_value = 1;

    // сохранить текущие значения
    int old_h_over = spc->h_oversample;
    int old_v_over = spc->v_oversample;

    k = 0;
    for(i = 0; i < num_ranges; ++i) {
        float fh = ranges[i].font_size;
        float scale = fh > 0 ? stbtt_ScaleForPixelHeight(info, fh) : stbtt_ScaleForMappingEmToPixels(info, -fh);
        float recip_h, recip_v, sub_x, sub_y;
        spc->h_oversample = ranges[i].h_oversample;
        spc->v_oversample = ranges[i].v_oversample;
        recip_h = 1.0f / spc->h_oversample;
        recip_v = 1.0f / spc->v_oversample;
        sub_x = stbtt__oversample_shift(spc->h_oversample);
        sub_y = stbtt__oversample_shift(spc->v_oversample);
        for(j = 0; j < ranges[i].num_chars; ++j) {
            stbrp_rect * r = &rects[k];
            if(r->was_packed && r->w != 0 && r->h != 0) {
                stbtt_packedchar * bc = &ranges[i].chardata_for_range[j];
                int advance, lsb, x0, y0, x1, y1;
                int codepoint = ranges[i].array_of_unicode_codepoints == NULL ? ranges[i].first_unicode_codepoint_in_range + j :
                                ranges[i].array_of_unicode_codepoints[j];
                int glyph = stbtt_FindGlyphIndex(info, codepoint);
                stbrp_coord pad = (stbrp_coord)spc->padding;

                // накладка слева и сверху
                r->x += pad;
                r->y += pad;
                r->w -= pad;
                r->h -= pad;
                stbtt_GetGlyphHMetrics(info, glyph, &advance, &lsb);
                stbtt_GetGlyphBitmapBox(info, glyph,
                                        scale * spc->h_oversample,
                                        scale * spc->v_oversample,
                                        &x0, &y0, &x1, &y1);
                stbtt_MakeGlyphBitmapSubpixel(info,
                                              spc->pixels + r->x + r->y * spc->stride_in_bytes,
                                              r->w - spc->h_oversample + 1,
                                              r->h - spc->v_oversample + 1,
                                              spc->stride_in_bytes,
                                              scale * spc->h_oversample,
                                              scale * spc->v_oversample,
                                              0, 0,
                                              glyph);

                if(spc->h_oversample > 1)
                    stbtt__h_prefilter(spc->pixels + r->x + r->y * spc->stride_in_bytes,
                                       r->w, r->h, spc->stride_in_bytes,
                                       spc->h_oversample);

                if(spc->v_oversample > 1)
                    stbtt__v_prefilter(spc->pixels + r->x + r->y * spc->stride_in_bytes,
                                       r->w, r->h, spc->stride_in_bytes,
                                       spc->v_oversample);

                bc->x0 = (stbtt_int16)r->x;
                bc->y0 = (stbtt_int16)r->y;
                bc->x1 = (stbtt_int16)(r->x + r->w);
                bc->y1 = (stbtt_int16)(r->y + r->h);
                bc->xadvance = scale * advance;
                bc->xoff = (float)x0 * recip_h + sub_x;
                bc->yoff = (float)y0 * recip_v + sub_y;
                bc->xoff2 = (x0 + r->w) * recip_h + sub_x;
                bc->yoff2 = (y0 + r->h) * recip_v + sub_y;

                if(glyph == 0)
                    missing_glyph = j;
            }
            else if(spc->skip_missing) {
                return_value = 0;
            }
            else if(r->was_packed && r->w == 0 && r->h == 0 && missing_glyph >= 0) {
                ranges[i].chardata_for_range[j] = ranges[i].chardata_for_range[missing_glyph];
            }
            else {
                return_value = 0; // если что-то не получается, сообщите об ошибке
            }

            ++k;
        }
    }

    // восстановить исходные значения
    spc->h_oversample = old_h_over;
    spc->v_oversample = old_v_over;

    return return_value;
}

#ifdef STBTT_STREAM_TYPE
STBTT_DEF int stbtt_PackFontRanges(stbtt_pack_context * spc, STBTT_STREAM_TYPE fontdata, int font_index,
                                   stbtt_pack_range * ranges, int num_ranges);
#else
STBTT_DEF int stbtt_PackFontRanges(stbtt_pack_context * spc, const unsigned char * fontdata, int font_index,
                                   stbtt_pack_range * ranges, int num_ranges);
#endif

STBTT_DEF void stbtt_PackFontRangesPackRects(stbtt_pack_context * spc, stbrp_rect * rects, int num_rects)
{
    stbrp_pack_rects((stbrp_context *)spc->pack_info, rects, num_rects);
}
#ifdef STBTT_STREAM_TYPE
STBTT_DEF int stbtt_PackFontRanges(stbtt_pack_context * spc, STBTT_STREAM_TYPE fontdata, int font_index,
                                   stbtt_pack_range * ranges, int num_ranges)
#else
STBTT_DEF int stbtt_PackFontRanges(stbtt_pack_context * spc, const unsigned char * fontdata, int font_index,
                                   stbtt_pack_range * ranges, int num_ranges)
#endif
{
    stbtt_fontinfo info;
    int i, j, n, return_value = 1;
    //stbrp_context *context = ( stbrp_context *) spc-> pack_info ;
    stbrp_rect * rects;

    // пометить все символы как упакованные NOT
    for(i = 0; i < num_ranges; ++i)
        for(j = 0; j < ranges[i].num_chars; ++j)
            ranges[i].chardata_for_range[j].x0 =
                ranges[i].chardata_for_range[j].y0 =
                    ranges[i].chardata_for_range[j].x1 =
                        ranges[i].chardata_for_range[j].y1 = 0;

    n = 0;
    for(i = 0; i < num_ranges; ++i)
        n += ranges[i].num_chars;

    rects = (stbrp_rect *)STBTT_malloc(sizeof(*rects) * n, spc->user_allocator_context);
    if(rects == NULL)
        return 0;

    info.userdata = spc->user_allocator_context;
    stbtt_InitFont(&info, fontdata, stbtt_GetFontOffsetForIndex(fontdata, font_index));

    n = stbtt_PackFontRangesGatherRects(spc, &info, ranges, num_ranges, rects);

    stbtt_PackFontRangesPackRects(spc, rects, n);

    return_value = stbtt_PackFontRangesRenderIntoRects(spc, &info, ranges, num_ranges, rects);

    STBTT_free(rects, spc->user_allocator_context);
    return return_value;
}

#ifdef STBTT_STREAM_TYPE
STBTT_DEF int stbtt_PackFontRange(stbtt_pack_context * spc, STBTT_STREAM_TYPE fontdata, int font_index, float font_size,
                                  int first_unicode_codepoint_in_range, int num_chars_in_range, stbtt_packedchar * chardata_for_range);
#else
STBTT_DEF int stbtt_PackFontRange(stbtt_pack_context * spc, const unsigned char * fontdata, int font_index,
                                  float font_size,
                                  int first_unicode_codepoint_in_range, int num_chars_in_range, stbtt_packedchar * chardata_for_range);
#endif

#ifdef STBTT_STREAM_TYPE
STBTT_DEF int stbtt_PackFontRange(stbtt_pack_context * spc, STBTT_STREAM_TYPE fontdata, int font_index, float font_size,
                                  int first_unicode_codepoint_in_range, int num_chars_in_range, stbtt_packedchar * chardata_for_range)
#else
STBTT_DEF int stbtt_PackFontRange(stbtt_pack_context * spc, const unsigned char * fontdata, int font_index,
                                  float font_size,
                                  int first_unicode_codepoint_in_range, int num_chars_in_range, stbtt_packedchar * chardata_for_range)
#endif
{
    stbtt_pack_range range;
    range.first_unicode_codepoint_in_range = first_unicode_codepoint_in_range;
    range.array_of_unicode_codepoints = NULL;
    range.num_chars = num_chars_in_range;
    range.chardata_for_range = chardata_for_range;
    range.font_size = font_size;
    return stbtt_PackFontRanges(spc, fontdata, font_index, &range, 1);
}

#ifdef STBTT_STREAM_TYPE
STBTT_DEF void stbtt_GetScaledFontVMetrics(STBTT_STREAM_TYPE fontdata, int index, float size, float * ascent,
                                           float * descent, float * lineGap);
#else
STBTT_DEF void stbtt_GetScaledFontVMetrics(const unsigned char * fontdata, int index, float size, float * ascent,
                                           float * descent, float * lineGap);
#endif

#ifdef STBTT_STREAM_TYPE
STBTT_DEF void stbtt_GetScaledFontVMetrics(STBTT_STREAM_TYPE fontdata, int index, float size, float * ascent,
                                           float * descent, float * lineGap)
#else
STBTT_DEF void stbtt_GetScaledFontVMetrics(const unsigned char * fontdata, int index, float size, float * ascent,
                                           float * descent, float * lineGap)
#endif
{
    int i_ascent, i_descent, i_lineGap;
    float scale;
    stbtt_fontinfo info;
    stbtt_InitFont(&info, fontdata, stbtt_GetFontOffsetForIndex(fontdata, index));
    scale = size > 0 ? stbtt_ScaleForPixelHeight(&info, size) : stbtt_ScaleForMappingEmToPixels(&info, -size);
    stbtt_GetFontVMetrics(&info, &i_ascent, &i_descent, &i_lineGap);
    *ascent = (float)i_ascent * scale;
    *descent = (float)i_descent * scale;
    *lineGap = (float)i_lineGap * scale;
}

STBTT_DEF void stbtt_GetPackedQuad(const stbtt_packedchar * chardata, int pw, int ph, int char_index, float * xpos,
                                   float * ypos, stbtt_aligned_quad * q, int align_to_integer)
{
    float ipw = 1.0f / pw, iph = 1.0f / ph;
    const stbtt_packedchar * b = chardata + char_index;

    if(align_to_integer) {
        float x = (float)STBTT_ifloor((*xpos + b->xoff) + 0.5f);
        float y = (float)STBTT_ifloor((*ypos + b->yoff) + 0.5f);
        q->x0 = x;
        q->y0 = y;
        q->x1 = x + b->xoff2 - b->xoff;
        q->y1 = y + b->yoff2 - b->yoff;
    }
    else {
        q->x0 = *xpos + b->xoff;
        q->y0 = *ypos + b->yoff;
        q->x1 = *xpos + b->xoff2;
        q->y1 = *ypos + b->yoff2;
    }

    q->s0 = b->x0 * ipw;
    q->t0 = b->y0 * iph;
    q->s1 = b->x1 * ipw;
    q->t1 = b->y1 * iph;

    *xpos += b->xadvance;
}

//////////////////////////////////////////////////////////////////////////////
//
// SDF-вычисление
//

#define STBTT_min(a,b)  ((a) < (b) ? (a) : (b))
#define STBTT_max(a,b)  ((a) < (b) ? (b) : (a))

static int stbtt__ray_intersect_bezier(float orig[2], float ray[2], float q0[2], float q1[2], float q2[2],
                                       float hits[2][2])
{
    float q0perp = q0[1] * ray[0] - q0[0] * ray[1];
    float q1perp = q1[1] * ray[0] - q1[0] * ray[1];
    float q2perp = q2[1] * ray[0] - q2[0] * ray[1];
    float roperp = orig[1] * ray[0] - orig[0] * ray[1];

    float a = q0perp - 2 * q1perp + q2perp;
    float b = q1perp - q0perp;
    float c = q0perp - roperp;

    float s0 = 0., s1 = 0.;
    int num_s = 0;

    if(a != 0.0f) {
        float discr = b * b - a * c;
        if(discr > 0.0f) {
            float rcpna = -1 / a;
            float d = (float)STBTT_sqrt(discr);
            s0 = (b + d) * rcpna;
            s1 = (b - d) * rcpna;
            if(s0 >= 0.0f && s0 <= 1.0f)
                num_s = 1;
            if(d > 0.0f && s1 >= 0.0f && s1 <= 1.0f) {
                if(num_s == 0) s0 = s1;
                ++num_s;
            }
        }
    }
    else {
        // 2*b*s + c = 0
        // s = -c / (2*b)
        s0 = c / (-2 * b);
        if(s0 >= 0.0f && s0 <= 1.0f)
            num_s = 1;
    }

    if(num_s == 0)
        return 0;
    else {
        float rcp_len2 = 1 / (ray[0] * ray[0] + ray[1] * ray[1]);
        float rayn_x = ray[0] * rcp_len2, rayn_y = ray[1] * rcp_len2;

        float q0d = q0[0] * rayn_x + q0[1] * rayn_y;
        float q1d = q1[0] * rayn_x + q1[1] * rayn_y;
        float q2d = q2[0] * rayn_x + q2[1] * rayn_y;
        float rod = orig[0] * rayn_x + orig[1] * rayn_y;

        float q10d = q1d - q0d;
        float q20d = q2d - q0d;
        float q0rd = q0d - rod;

        hits[0][0] = q0rd + s0 * (2.0f - 2.0f * s0) * q10d + s0 * s0 * q20d;
        hits[0][1] = a * s0 + b;

        if(num_s > 1) {
            hits[1][0] = q0rd + s1 * (2.0f - 2.0f * s1) * q10d + s1 * s1 * q20d;
            hits[1][1] = a * s1 + b;
            return 2;
        }
        else {
            return 1;
        }
    }
}

static int equal(float * a, float * b)
{
    return (a[0] == b[0] && a[1] == b[1]);
}

static int stbtt__compute_crossings_x(float x, float y, int nverts, stbtt_vertex * verts)
{
    int i;
    float orig[2], ray[2] = { 1, 0 };
    float y_frac;
    int winding = 0;

    // убедитесь, что y никогда не проходит через вершину фигуры
    y_frac = (float)STBTT_fmod(y, 1.0f);
    if(y_frac < 0.01f)
        y += 0.01f;
    else if(y_frac > 0.99f)
        y -= 0.01f;

    orig[0] = x;
    orig[1] = y;

    // проверить луч от (-бесконечность,y) до (x,y)
    for(i = 0; i < nverts; ++i) {
        if(verts[i].type == STBTT_vline) {
            int x0 = (int)verts[i - 1].x, y0 = (int)verts[i - 1].y;
            int x1 = (int)verts[i].x, y1 = (int)verts[i].y;
            if(y > STBTT_min(y0, y1) && y < STBTT_max(y0, y1) && x > STBTT_min(x0, x1)) {
                float x_inter = (y - y0) / (y1 - y0) * (x1 - x0) + x0;
                if(x_inter < x)
                    winding += (y0 < y1) ? 1 : -1;
            }
        }
        if(verts[i].type == STBTT_vcurve) {
            int x0 = (int)verts[i - 1].x, y0 = (int)verts[i - 1].y;
            int x1 = (int)verts[i].cx, y1 = (int)verts[i].cy;
            int x2 = (int)verts[i].x, y2 = (int)verts[i].y;
            int ax = STBTT_min(x0, STBTT_min(x1, x2)), ay = STBTT_min(y0, STBTT_min(y1, y2));
            int by = STBTT_max(y0, STBTT_max(y1, y2));
            if(y > ay && y < by && x > ax) {
                float q0[2], q1[2], q2[2];
                float hits[2][2];
                q0[0] = (float)x0;
                q0[1] = (float)y0;
                q1[0] = (float)x1;
                q1[1] = (float)y1;
                q2[0] = (float)x2;
                q2[1] = (float)y2;
                if(equal(q0, q1) || equal(q1, q2)) {
                    x0 = (int)verts[i - 1].x;
                    y0 = (int)verts[i - 1].y;
                    x1 = (int)verts[i].x;
                    y1 = (int)verts[i].y;
                    if(y > STBTT_min(y0, y1) && y < STBTT_max(y0, y1) && x > STBTT_min(x0, x1)) {
                        float x_inter = (y - y0) / (y1 - y0) * (x1 - x0) + x0;
                        if(x_inter < x)
                            winding += (y0 < y1) ? 1 : -1;
                    }
                }
                else {
                    int num_hits = stbtt__ray_intersect_bezier(orig, ray, q0, q1, q2, hits);
                    if(num_hits >= 1)
                        if(hits[0][0] < 0)
                            winding += (hits[0][1] < 0 ? -1 : 1);
                    if(num_hits >= 2)
                        if(hits[1][0] < 0)
                            winding += (hits[1][1] < 0 ? -1 : 1);
                }
            }
        }
    }
    return winding;
}

static float stbtt__cuberoot(float x)
{
    if(x < 0)
        return -(float)STBTT_pow(-x, 1.0f / 3.0f);
    else
        return (float)STBTT_pow(x, 1.0f / 3.0f);
}

// x^3 + a*x^2 + b*x + c = 0
static int stbtt__solve_cubic(float a, float b, float c, float * r)
{
    float s = -a / 3;
    float p = b - a * a / 3;
    float q = a * (2 * a * a - 9 * b) / 27 + c;
    float p3 = p * p * p;
    float d = q * q + 4 * p3 / 27;
    if(d >= 0) {
        float z = (float)STBTT_sqrt(d);
        float u = (-q + z) / 2;
        float v = (-q - z) / 2;
        u = stbtt__cuberoot(u);
        v = stbtt__cuberoot(v);
        r[0] = s + u + v;
        return 1;
    }
    else {
        float u = (float)STBTT_sqrt(-p / 3);
        float v = (float)STBTT_acos(-STBTT_sqrt(-27 / p3) * q / 2) / 3; // p3 должен быть отрицательным, поскольку d отрицательно
        float m = (float)STBTT_cos(v);
        float n = (float)STBTT_cos(v - 3.141592f / 2) * 1.732050808f;
        r[0] = s + u * 2 * m;
        r[1] = s - u * (m + n);
        r[2] = s - u * (m - n);

        //STBTT_assert( STBTT_fabs(((r[0]+a)*r[0]+b)*r[0]+c) < 0.05f);  // these asserts may not be safe at all scales, though they're in bezier t parameter units so maybe?
        //STBTT_assert ( STBTT_fabs (((r[1]+a)*r[1]+b)*r[1]+c) < 0,05f);
        //STBTT_assert ( STBTT_fabs (((r[2]+a)*r[2]+b)*r[2]+c) < 0,05f);
        return 3;
    }
}

STBTT_DEF unsigned char * stbtt_GetGlyphSDF(const stbtt_fontinfo * info, float scale, int glyph, int padding,
                                            unsigned char onedge_value, float pixel_dist_scale, int * width, int * height, int * xoff, int * yoff)
{
    float scale_x = scale, scale_y = scale;
    int ix0, iy0, ix1, iy1;
    int w, h;
    unsigned char * data;

    if(scale == 0) return NULL;

    stbtt_GetGlyphBitmapBoxSubpixel(info, glyph, scale, scale, 0.0f, 0.0f, &ix0, &iy0, &ix1, &iy1);

    // если пусто, верните NULL
    if(ix0 == ix1 || iy0 == iy1)
        return NULL;

    ix0 -= padding;
    iy0 -= padding;
    ix1 += padding;
    iy1 += padding;

    w = (ix1 - ix0);
    h = (iy1 - iy0);

    if(width) *width = w;
    if(height) *height = h;
    if(xoff) *xoff = ix0;
    if(yoff) *yoff = iy0;

    // инвертировать растровые изображения по оси y вниз
    scale_y = -scale_y;

    {
        int x, y, i, j;
        float * precompute;
        stbtt_vertex * verts;
        int num_verts = stbtt_GetGlyphShape(info, glyph, &verts);
        data = (unsigned char *)STBTT_malloc(w * h, info->userdata);
        precompute = (float *)STBTT_malloc(num_verts * sizeof(float), info->userdata);

        for(i = 0, j = num_verts - 1; i < num_verts; j = i++) {
            if(verts[i].type == STBTT_vline) {
                float x0 = verts[i].x * scale_x, y0 = verts[i].y * scale_y;
                float x1 = verts[j].x * scale_x, y1 = verts[j].y * scale_y;
                float dist = (float)STBTT_sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
                precompute[i] = (dist == 0) ? 0.0f : 1.0f / dist;
            }
            else if(verts[i].type == STBTT_vcurve) {
                float x2 = verts[j].x * scale_x, y2 = verts[j].y * scale_y;
                float x1 = verts[i].cx * scale_x, y1 = verts[i].cy * scale_y;
                float x0 = verts[i].x * scale_x, y0 = verts[i].y * scale_y;
                float bx = x0 - 2 * x1 + x2, by = y0 - 2 * y1 + y2;
                float len2 = bx * bx + by * by;
                if(len2 != 0.0f)
                    precompute[i] = 1.0f / (bx * bx + by * by);
                else
                    precompute[i] = 0.0f;
            }
            else
                precompute[i] = 0.0f;
        }

        for(y = iy0; y < iy1; ++y) {
            for(x = ix0; x < ix1; ++x) {
                float val;
                float min_dist = 999999.0f;
                float sx = (float)x + 0.5f;
                float sy = (float)y + 0.5f;
                float x_gspace = (sx / scale_x);
                float y_gspace = (sy / scale_y);

                int winding = stbtt__compute_crossings_x(x_gspace, y_gspace, num_verts,
                                                         verts); // @OPTIMIZE: это может быть просто растеризация, но это должны быть линии, а не немозаичные кривые, поэтому новый путь

                for(i = 0; i < num_verts; ++i) {
                    float x0 = verts[i].x * scale_x, y0 = verts[i].y * scale_y;

                    if(verts[i].type == STBTT_vline && precompute[i] != 0.0f) {
                        float x1 = verts[i - 1].x * scale_x, y1 = verts[i - 1].y * scale_y;

                        float dist, dist2 = (x0 - sx) * (x0 - sx) + (y0 - sy) * (y0 - sy);
                        if(dist2 < min_dist * min_dist)
                            min_dist = (float)STBTT_sqrt(dist2);

                        // грубая отбраковка против bbox
                        //if (sx > STBTT_min (x0,x1)- min_dist && sx < STBTT_max (x0,x1)+ min_dist &&
                        //    sy > STBTT_min (y0,y1)- min_dist && sy < STBTT_max (y0,y1)+ min_dist )
                        dist = (float)STBTT_fabs((x1 - x0) * (y0 - sy) - (y1 - y0) * (x0 - sx)) * precompute[i];
                        STBTT_assert(i != 0);
                        if(dist < min_dist) {
                            // проверить положение вдоль линии
                            // x' = x0 + t*(x1-x0), y' = y0 + t*(y1-y0)
                            // минимизировать (x'-sx)*(x'-sx)+(y'-sy)*(y'-sy)
                            float dx = x1 - x0, dy = y1 - y0;
                            float px = x0 - sx, py = y0 - sy;
                            // минимизировать (px+t*dx)^2 + (py+t*dy)^2 = px*px + 2*px*dx*t + t^2*dx*dx + py*py + 2*py*dy*t + t^2*dy*dy
                            // derivative: 2*px*dx + 2*py*dy + (2*dx*dx+2*dy*dy)*t, set to 0 and solve
                            float t = -(px * dx + py * dy) / (dx * dx + dy * dy);
                            if(t >= 0.0f && t <= 1.0f)
                                min_dist = dist;
                        }
                    }
                    else if(verts[i].type == STBTT_vcurve) {
                        float x2 = verts[i - 1].x * scale_x, y2 = verts[i - 1].y * scale_y;
                        float x1 = verts[i].cx * scale_x, y1 = verts[i].cy * scale_y;
                        float box_x0 = STBTT_min(STBTT_min(x0, x1), x2);
                        float box_y0 = STBTT_min(STBTT_min(y0, y1), y2);
                        float box_x1 = STBTT_max(STBTT_max(x0, x1), x2);
                        float box_y1 = STBTT_max(STBTT_max(y0, y1), y2);
                        // грубая отсечка по bbox, чтобы избежать ненужных кубических вычислений
                        if(sx > box_x0 - min_dist && sx < box_x1 + min_dist && sy > box_y0 - min_dist && sy < box_y1 + min_dist) {
                            int num = 0;
                            float ax = x1 - x0, ay = y1 - y0;
                            float bx = x0 - 2 * x1 + x2, by = y0 - 2 * y1 + y2;
                            float mx = x0 - sx, my = y0 - sy;
                            float res[3] = { 0.f, 0.f, 0.f };
                            float px, py, t, it, dist2;
                            float a_inv = precompute[i];
                            if(a_inv == 0.0f) {  // если a_inv равен 0, это 2-я степень, поэтому используйте квадратичную формулу
                                float a = 3 * (ax * bx + ay * by);
                                float b = 2 * (ax * ax + ay * ay) + (mx * bx + my * by);
                                float c = mx * ax + my * ay;
                                if(a == 0.0f) {  // если а равно 0, оно линейно
                                    if(b != 0.0f) {
                                        res[num++] = -c / b;
                                    }
                                }
                                else {
                                    float discriminant = b * b - 4 * a * c;
                                    if(discriminant < 0)
                                        num = 0;
                                    else {
                                        float root = (float)STBTT_sqrt(discriminant);
                                        res[0] = (-b - root) / (2 * a);
                                        res[1] = (-b + root) / (2 * a);
                                        num = 2; // не беспокойтесь о различении случая с 1 решением, так как приведенный ниже код все равно будет работать
                                    }
                                }
                            }
                            else {
                                float b = 3 * (ax * bx + ay * by) * a_inv; // можно предварительно вычислить это, поскольку это не зависит от точки выборки
                                float c = (2 * (ax * ax + ay * ay) + (mx * bx + my * by)) * a_inv;
                                float d = (mx * ax + my * ay) * a_inv;
                                num = stbtt__solve_cubic(b, c, d, res);
                            }
                            dist2 = (x0 - sx) * (x0 - sx) + (y0 - sy) * (y0 - sy);
                            if(dist2 < min_dist * min_dist)
                                min_dist = (float)STBTT_sqrt(dist2);

                            if(num >= 1 && res[0] >= 0.0f && res[0] <= 1.0f) {
                                t = res[0], it = 1.0f - t;
                                px = it * it * x0 + 2 * t * it * x1 + t * t * x2;
                                py = it * it * y0 + 2 * t * it * y1 + t * t * y2;
                                dist2 = (px - sx) * (px - sx) + (py - sy) * (py - sy);
                                if(dist2 < min_dist * min_dist)
                                    min_dist = (float)STBTT_sqrt(dist2);
                            }
                            if(num >= 2 && res[1] >= 0.0f && res[1] <= 1.0f) {
                                t = res[1], it = 1.0f - t;
                                px = it * it * x0 + 2 * t * it * x1 + t * t * x2;
                                py = it * it * y0 + 2 * t * it * y1 + t * t * y2;
                                dist2 = (px - sx) * (px - sx) + (py - sy) * (py - sy);
                                if(dist2 < min_dist * min_dist)
                                    min_dist = (float)STBTT_sqrt(dist2);
                            }
                            if(num >= 3 && res[2] >= 0.0f && res[2] <= 1.0f) {
                                t = res[2], it = 1.0f - t;
                                px = it * it * x0 + 2 * t * it * x1 + t * t * x2;
                                py = it * it * y0 + 2 * t * it * y1 + t * t * y2;
                                dist2 = (px - sx) * (px - sx) + (py - sy) * (py - sy);
                                if(dist2 < min_dist * min_dist)
                                    min_dist = (float)STBTT_sqrt(dist2);
                            }
                        }
                    }
                }
                if(winding == 0)
                    min_dist = -min_dist;  // если вне формы, значение отрицательное
                val = onedge_value + pixel_dist_scale * min_dist;
                if(val < 0)
                    val = 0;
                else if(val > 255)
                    val = 255;
                data[(y - iy0) * w + (x - ix0)] = (unsigned char)val;
            }
        }
        STBTT_free(precompute, info->userdata);
        STBTT_free(verts, info->userdata);
    }
    return data;
}

STBTT_DEF unsigned char * stbtt_GetCodepointSDF(const stbtt_fontinfo * info, float scale, int codepoint, int padding,
                                                unsigned char onedge_value, float pixel_dist_scale, int * width, int * height, int * xoff, int * yoff)
{
    return stbtt_GetGlyphSDF(info, scale, stbtt_FindGlyphIndex(info, codepoint), padding, onedge_value, pixel_dist_scale,
                             width, height, xoff, yoff);
}

STBTT_DEF void stbtt_FreeSDF(unsigned char * bitmap, void * userdata)
{
    STBTT_free(bitmap, userdata);
}

//////////////////////////////////////////////////////////////////////////////
//
// соответствие имени шрифта – рекомендуется не использовать это
//

// проверьте, содержит ли строка utf8 префикс, который является строкой utf16; если да, то верните длину соответствующей строки utf8
#ifdef STBTT_STREAM_TYPE
static stbtt_int32 stbtt__CompareUTF8toUTF16_bigendian_prefix(stbtt_uint8 * s1, stbtt_int32 len1, STBTT_STREAM_TYPE s2,
                                                              stbtt_uint32 s2offs, stbtt_int32 len2)
#else
static stbtt_int32 stbtt__CompareUTF8toUTF16_bigendian_prefix(stbtt_uint8 * s1, stbtt_int32 len1, stbtt_uint8 * s2,
                                                              stbtt_uint32 s2offs, stbtt_int32 len2)
#endif
{
    stbtt_int32 i = 0;

    // конвертируйте utf16 в utf8 и сравнивайте результаты при конвертации
    while(len2) {
        stbtt_uint16 ch = ttUSHORT(s2, s2offs);
        if(ch < 0x80) {
            if(i >= len1) return -1;
            if(s1[i++] != ch) return -1;
        }
        else if(ch < 0x800) {
            if(i + 1 >= len1) return -1;
            if(s1[i++] != 0xc0 + (ch >> 6)) return -1;
            if(s1[i++] != 0x80 + (ch & 0x3f)) return -1;
        }
        else if(ch >= 0xd800 && ch < 0xdc00) {
            stbtt_uint32 c;
            stbtt_uint16 ch2 = ttUSHORT(s2, s2offs + 2);
            if(i + 3 >= len1) return -1;
            c = ((ch - 0xd800) << 10) + (ch2 - 0xdc00) + 0x10000;
            if(s1[i++] != 0xf0 + (c >> 18)) return -1;
            if(s1[i++] != 0x80 + ((c >> 12) & 0x3f)) return -1;
            if(s1[i++] != 0x80 + ((c >> 6) & 0x3f)) return -1;
            if(s1[i++] != 0x80 + ((c) & 0x3f)) return -1;
            s2offs += 2; // плюс еще 2 ниже
            len2 -= 2;
        }
        else if(ch >= 0xdc00 && ch < 0xe000) {
            return -1;
        }
        else {
            if(i + 2 >= len1) return -1;
            if(s1[i++] != 0xe0 + (ch >> 12)) return -1;
            if(s1[i++] != 0x80 + ((ch >> 6) & 0x3f)) return -1;
            if(s1[i++] != 0x80 + ((ch) & 0x3f)) return -1;
        }
        s2offs += 2;
        len2 -= 2;
    }
    return i;
}
#ifdef STBTT_STREAM_TYPE
static int stbtt_CompareUTF8toUTF16_bigendian_internal(char * s1, int len1, STBTT_STREAM_TYPE s2, stbtt_uint32 s2offs,
                                                       int len2)
{
    return len1 == stbtt__CompareUTF8toUTF16_bigendian_prefix((stbtt_uint8 *)s1, len1, s2, s2offs, len2);
}
#else
static int stbtt_CompareUTF8toUTF16_bigendian_internal(char * s1, int len1, char * s2, stbtt_uint32 s2offs, int len2)
{
    return len1 == stbtt__CompareUTF8toUTF16_bigendian_prefix((stbtt_uint8 *)s1, len1, (stbtt_uint8 *)s2, s2offs, len2);
}
#endif
// возвращает результаты в любой запрошенной вами кодировке... но обратите внимание, что 2-байтовые кодировки
// будет BIG - ENDIAN... используйте stbtt_CompareUTF8toUTF16_bigendian() для сравнения
STBTT_DEF stbtt_uint32 stbtt_GetFontNameString(const stbtt_fontinfo * font, int * length, int platformID,
                                               int encodingID, int languageID, int nameID)
{
    stbtt_int32 i, count, stringOffset;
    stbtt_uint32 offset = font->fontstart;
    stbtt_uint32 nm = stbtt__find_table(font->data, offset, "name");
    if(!nm) return 0;

    count = ttUSHORT(font->data, nm + 2);
    stringOffset = nm + ttUSHORT(font->data, nm + 4);
    for(i = 0; i < count; ++i) {
        stbtt_uint32 loc = nm + 6 + 12 * i;
        if(platformID == ttUSHORT(font->data, loc + 0) && encodingID == ttUSHORT(font->data, loc + 2)
           && languageID == ttUSHORT(font->data, loc + 4) && nameID == ttUSHORT(font->data, loc + 6)) {
            *length = ttUSHORT(font->data, loc + 8);
            return stringOffset + ttUSHORT(font->data, loc + 10);
        }
    }
    return 0;
}
#ifdef STBTT_STREAM_TYPE
static int stbtt__matchpair(STBTT_STREAM_TYPE fc, stbtt_uint32 nm, stbtt_uint8 * name, stbtt_int32 nlen,
                            stbtt_int32 target_id, stbtt_int32 next_id)
#else
static int stbtt__matchpair(stbtt_uint8 * fc, stbtt_uint32 nm, stbtt_uint8 * name, stbtt_int32 nlen,
                            stbtt_int32 target_id, stbtt_int32 next_id)
#endif
{
    stbtt_int32 i;
    stbtt_int32 count = ttUSHORT(fc, nm + 2);
    stbtt_int32 stringOffset = nm + ttUSHORT(fc, nm + 4);

    for(i = 0; i < count; ++i) {
        stbtt_uint32 loc = nm + 6 + 12 * i;
        stbtt_int32 id = ttUSHORT(fc, loc + 6);
        if(id == target_id) {
            // найди кодировку
            stbtt_int32 platform = ttUSHORT(fc, loc + 0), encoding = ttUSHORT(fc, loc + 2), language = ttUSHORT(fc, loc + 4);

            // is this a Unicode encoding?
            if(platform == 0 || (platform == 3 && encoding == 1) || (platform == 3 && encoding == 10)) {
                stbtt_int32 slen = ttUSHORT(fc, loc + 8);
                stbtt_int32 off = ttUSHORT(fc, loc + 10);

                // проверьте, есть ли совпадение префикса
                stbtt_int32 matchlen = stbtt__CompareUTF8toUTF16_bigendian_prefix(name, nlen, fc, stringOffset + off, slen);
                if(matchlen >= 0) {
                    // проверьте наличие target_id +1 сразу после него с той же кодировкой и языком
                    if(i + 1 < count && ttUSHORT(fc, loc + 12 + 6) == next_id && ttUSHORT(fc, loc + 12) == platform &&
                       ttUSHORT(fc, loc + 12 + 2) == encoding && ttUSHORT(fc, loc + 12 + 4) == language) {
                        slen = ttUSHORT(fc, loc + 12 + 8);
                        off = ttUSHORT(fc, loc + 12 + 10);
                        if(slen == 0) {
                            if(matchlen == nlen)
                                return 1;
                        }
                        else if(matchlen < nlen && name[matchlen] == ' ') {
                            ++matchlen;
#ifdef STBTT_STREAM_TYPE
                            if(stbtt_CompareUTF8toUTF16_bigendian_internal((char *)(name + matchlen), nlen - matchlen, fc, stringOffset + off,
                                                                           slen))
#else
                            if(stbtt_CompareUTF8toUTF16_bigendian_internal((char *)(name + matchlen), nlen - matchlen, (char *)fc,
                                                                           stringOffset + off, slen))
#endif
                                return 1;
                        }
                    }
                    else {
                        // если ничего не следует сразу за этим
                        if(matchlen == nlen)
                            return 1;
                    }
                }
            }

            // @TODO обрабатывает другие кодировки.
        }
    }
    return 0;
}
#ifdef STBTT_STREAM_TYPE
    static int stbtt__matches(STBTT_STREAM_TYPE fc, stbtt_uint32 offset, stbtt_uint8 * name, stbtt_int32 flags)
#else
    static int stbtt__matches(stbtt_uint8 * fc, stbtt_uint32 offset, stbtt_uint8 * name, stbtt_int32 flags)
#endif

{
    stbtt_int32 nlen = (stbtt_int32)STBTT_strlen((char *)name);
    stbtt_uint32 nm, hd;
    if(!stbtt__isfont(fc, offset)) return 0;

    // проверьте флаги курсива/жирного шрифта/подчеркивания в macStyle...
    if(flags) {
        hd = stbtt__find_table(fc, offset, "head");
        if((ttUSHORT(fc, hd + 44) & 7) != (flags & 7)) return 0;
    }

    nm = stbtt__find_table(fc, offset, "name");
    if(!nm) return 0;

    if(flags) {
        if(name == NULL) return 1;
        // если мы проверили флаги macStyle, то просто проверяем семейство и игнорируем подсемейство
        if(stbtt__matchpair(fc, nm, name, nlen, 16, -1))  return 1;
        if(stbtt__matchpair(fc, nm, name, nlen, 1, -1))  return 1;
        if(stbtt__matchpair(fc, nm, name, nlen, 3, -1))  return 1;
    }
    else {
        if(name == NULL) return 1;
        if(stbtt__matchpair(fc, nm, name, nlen, 16, 17))  return 1;
        if(stbtt__matchpair(fc, nm, name, nlen, 1, 2))  return 1;
        if(stbtt__matchpair(fc, nm, name, nlen, 3, -1))  return 1;
    }

    return 0;
}
#ifdef STBTT_STREAM_TYPE
    static int stbtt_FindMatchingFont_internal(STBTT_STREAM_TYPE font_collection, char * name_utf8, stbtt_int32 flags)
#else
    static int stbtt_FindMatchingFont_internal(unsigned char * font_collection, char * name_utf8, stbtt_int32 flags)
#endif
{
    stbtt_int32 i;
    for(i = 0;; ++i) {
        stbtt_int32 off = stbtt_GetFontOffsetForIndex(font_collection, i);
        if(off < 0) return off;
#ifdef STBTT_STREAM_TYPE
        if(stbtt__matches(font_collection, off, (stbtt_uint8 *)name_utf8, flags))
#else
        if(stbtt__matches((stbtt_uint8 *)font_collection, off, (stbtt_uint8 *)name_utf8, flags))
#endif
            return off;
    }
}

#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-qual"
#endif

#ifdef STBTT_STREAM_TYPE
STBTT_DEF int stbtt_BakeFontBitmap(STBTT_STREAM_TYPE data, int offset,
                                   float pixel_height, unsigned char * pixels, int pw, int ph,
                                   int first_char, int num_chars, stbtt_bakedchar * chardata);
#else
STBTT_DEF int stbtt_BakeFontBitmap(const unsigned char * data, int offset,
                                   float pixel_height, unsigned char * pixels, int pw, int ph,
                                   int first_char, int num_chars, stbtt_bakedchar * chardata);
#endif

#ifdef STBTT_STREAM_TYPE
STBTT_DEF int stbtt_BakeFontBitmap(STBTT_STREAM_TYPE data, int offset,
                                   float pixel_height, unsigned char * pixels, int pw, int ph,
                                   int first_char, int num_chars, stbtt_bakedchar * chardata)
#else
STBTT_DEF int stbtt_BakeFontBitmap(const unsigned char * data, int offset,
                                   float pixel_height, unsigned char * pixels, int pw, int ph,
                                   int first_char, int num_chars, stbtt_bakedchar * chardata)
#endif
{
#ifdef STBTT_STREAM_TYPE
    return stbtt_BakeFontBitmap_internal(data, offset, pixel_height, pixels, pw, ph, first_char, num_chars, chardata);
#else
    return stbtt_BakeFontBitmap_internal((unsigned char *)data, offset, pixel_height, pixels, pw, ph, first_char, num_chars,
                                         chardata);
#endif
}
#ifdef STBTT_STREAM_TYPE
STBTT_DEF int stbtt_GetFontOffsetForIndex(STBTT_STREAM_TYPE data, int index)
{
    return stbtt_GetFontOffsetForIndex_internal(data, index);
}
#else
STBTT_DEF int stbtt_GetFontOffsetForIndex(const unsigned char * data, int index)
{
    return stbtt_GetFontOffsetForIndex_internal((unsigned char *)data, index);
}
#endif
#ifdef STBTT_STREAM_TYPE
STBTT_DEF int stbtt_GetNumberOfFonts(STBTT_STREAM_TYPE data)
{
    return stbtt_GetNumberOfFonts_internal(data);
}
#else
STBTT_DEF int stbtt_GetNumberOfFonts(const unsigned char * data)
{
    return stbtt_GetNumberOfFonts_internal((unsigned char *)data);
}
#endif
#ifdef STBTT_STREAM_TYPE
STBTT_DEF int stbtt_InitFont(stbtt_fontinfo * info, STBTT_STREAM_TYPE data, int offset)
{
    return stbtt_InitFont_internal(info, data, offset);
}
#else
STBTT_DEF int stbtt_InitFont(stbtt_fontinfo * info, const unsigned char * data, int offset)
{
    return stbtt_InitFont_internal(info, (unsigned char *)data, offset);
}
#endif
#ifdef STBTT_STREAM_TYPE
STBTT_DEF int stbtt_FindMatchingFont(STBTT_STREAM_TYPE fontdata, const char * name, int flags)
{
    return stbtt_FindMatchingFont_internal(fontdata, (char *)name, flags);
}
#else
STBTT_DEF int stbtt_FindMatchingFont(const unsigned char * fontdata, const char * name, int flags)
{
    return stbtt_FindMatchingFont_internal((unsigned char *)fontdata, (char *)name, flags);
}
#endif
#ifdef STBTT_STREAM_TYPE
STBTT_DEF int stbtt_CompareUTF8toUTF16_bigendian(const char * s1, int len1, STBTT_STREAM_TYPE s2, stbtt_uint32 s2offs,
                                                 int len2)
{
    return stbtt_CompareUTF8toUTF16_bigendian_internal((char *)s1, len1, s2, s2offs, len2);
}
#else
STBTT_DEF int stbtt_CompareUTF8toUTF16_bigendian(const char * s1, int len1, const char * s2, stbtt_uint32 s2offs,
                                                 int len2)
{
    return stbtt_CompareUTF8toUTF16_bigendian_internal((char *)s1, len1, (char *)s2, s2offs, len2);
}
#endif

#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
    #pragma GCC diagnostic pop
#endif

#endif // STB_TRUETYPE_IMPLEMENTATION

// FULL VERSION HISTORY
//
//   1.25 (11 июля 2021 г.) множество исправлений
//   1.24 (05.02.2020) исправлено предупреждение
//   1.23 (02.02.2020) запрос данных SVG для глифов; запросить всю таблицу кернинга (но только кернинг, а не GPOS )
//   1.22 (11 августа 2019 г.) минимизирует дублирование отсутствующих символов; исправить кернинг, если определены оба 'GPOS' и 'kern'
//   1.21 (25 февраля 2019 г.) исправлено предупреждение
//   1.20 (07.02.2019) PackFontRange пропускает недостающие кодовые точки;  GetScaleFontVMetrics()
//   1.19 (11.02.2018) Кернинг OpenType GPOS (только по горизонтали), STBTT_fmod
//   1.18 (29.01.2018) добавлена недостающая функция
//   1.17 (23 июля 2017 г.) добавить больше аргументов const; исправление документа
//   1.16 (12.07.2017) Поддержка SDF
//   1.15 (03.03.2017) приведите больше аргументов const
//   1.14 (16.01.2017) функция num-fonts-in- TTC
//   1.13 (02.01.2017) поддерживает шрифты OpenType, некоторые шрифты Apple.
//   1.12 (25 октября 2016 г.) подавляет предупреждения об удалении const с помощью -Wcast-qual
//   1.11 (02.04.2016) исправлено предупреждение о неиспользуемой переменной.
//   Версия 1.10 (02.04.2016) допускает замену fabs(), определяемую пользователем.
//                     исправить утечку памяти, если размер шрифта = 0.0
//                     исправить предупреждение из-за дублирования typedef
//   1.09 (16 января 2016 г.) исправление предупреждения; избежать сбоя при выходе из памяти; использовать пользовательские данные alloc для PackFontRanges
//   1.08 (13 сентября 2015 г.) документ stbtt_Rasterize (); исправления для вертикальных и горизонтальных краев
//   1.07 (01 августа 2015 г.) позволяет PackFontRanges принимать массивы разреженных кодовых точек;
//                     разрешить PackFontRanges упаковывать и отображать на отдельных этапах;
//                     fix stbtt_GetFontOFfsetForIndex (never worked for non-0 input?);
//                     исправлена ошибка assert() в новом растеризаторе
//                     замените assert() на STBTT_assert() в новом растеризаторе
//   1.06 (14 июля 2015 г.) улучшена производительность (примерно на 35 % быстрее на x86 и x64 на тестовой машине)
//                     также более точный растеризатор AA, за исключением случаев, когда фигуры перекрываются
//                     убрать необходимость в STBTT_sort
//   1.05 (15 апреля 2015 г.) исправлены неверные определения для STBTT_STATIC.
//   1.04 (15 апреля 2015 г.) опечатка в примере
//   1.03 (12.04.2015) STBTT_STATIC , исправлена утечка памяти в новой упаковке, различные исправления
//   1.02 (10 декабря 2014 г.) исправлены различные предупреждения и проблемы компиляции с stb_rect_pack , C++.
//   1.01 (08 декабря 2014 г.) исправлено положение субпикселя при передискретизации для точного соответствия
//                        без передискретизации;  STBTT_POINT_SIZE только для упакованного ящика
//   1.00 (06 декабря 2014 г.) добавлен новый PackBegin и т. д. API с поддержкой передискретизации.
//   0.99 (18 сентября 2014 г.) исправлено множество ошибок с субпиксельным рендерингом (ryg)
//   0.9 (07.08.2014) поддерживают определенные шрифты Mac/iOS без идентификатора платформы MS.
//   0.8b (07.07.2014) исправлено предупреждение
//   0.8 (25 мая 2014 г.) исправлено еще несколько предупреждений.
//   0.7 (25 сентября 2013 г.) исправление: ошибка с субпиксельными глифами, исправленная в версии 0.5, вернулась
//   0.6c (24 июля 2012 г.) улучшить документацию
//   0.6b (20 июля 2012 г.) исправлено еще несколько предупреждений.
//   0.6 (17 июля 2012 г.) исправлены предупреждения; добавил stbtt_ScaleForMappingEmToPixels ,
//                        stbtt_GetFontBoundingBox , stbtt_IsGlyphEmpty
//   0.5 (09.12.2011) исправления:
//                        Средство рендеринга субпиксельных глифов вычислило неверную ограничивающую рамку
//                        первая вершина фигуры может быть не кривой (FreeSans)
//   0.4b (03.12.2011) исправлена ошибка в примере запекания шрифта.
//   0.4 (01.12.2011) кернинг, субпиксельный рендеринг (tor)
//                    исправления для:
//                        Преобразование кодовой точки в глиф с использованием таблицы fmt=12
//                        Преобразование кодовой точки в глиф с использованием таблицы fmt=4
//                        stbtt_GetBakedQuad с неквадратной текстурой (Zer)
//                    обновлено Привет, мир! образец использования кернинга и субпикселя
//                    исправлены некоторые предупреждения
//   0,3 (24 июня 2009 г.) cmap fmt=12, составные фигуры ( MM )
//                    пользовательские данные, malloc-from-userdata, ненулевое заполнение (stb)
//   0.2 (11 марта 2009 г.) Исправлены предупреждения о беззнаковых/подписанных символах.
//   0.1 (09.03.2009) Первый публичный выпуск
//

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
