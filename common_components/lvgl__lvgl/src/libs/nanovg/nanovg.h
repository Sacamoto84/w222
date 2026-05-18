//
// Copyright (c) 2013 Mikko Mononen memon@inside.org
//
// Данное программное обеспечение предоставляется «как есть», без каких-либо явных или подразумеваемых
// гарантия.  Ни при каких обстоятельствах авторы не несут ответственности за какой-либо ущерб.
// возникшие в результате использования данного программного обеспечения.
// Разрешение предоставляется каждому на использование этого программного обеспечения для любых целей,
// включая коммерческие приложения, а также изменять его и распространять
// свободно, с соблюдением следующих ограничений:
// 1. The origin of this software must not be misrepresented; you must not
//    утверждайте, что вы написали оригинальное программное обеспечение. Если вы используете это программное обеспечение
//    в продукте подтверждение в документации по продукту будет
//    приветствуется, но не является обязательным.
// 2. Altered source versions must be plainly marked as such, and must not be
//    ошибочно представлено как оригинальное программное обеспечение.
// 3. This notice may not be removed or altered from any source distribution.
//

#ifndef NANOVG_H
#define NANOVG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_conf_internal.h"

#if LV_USE_NANOVG

#define NVG_PI 3.14159265358979323846264338327f

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4201)  // используется нестандартное расширение: безымянная структура/объединение
#endif

typedef struct NVGcontext NVGcontext;

union NVGcolor {
    float rgba[4];
    struct {
        float r, g, b, a;
    } ch;
};
typedef union NVGcolor NVGcolor;

struct NVGpaint {
    float xform[6];
    float extent[2];
    float radius;
    float feather;
    NVGcolor innerColor;
    NVGcolor outerColor;
    int image;
};
typedef struct NVGpaint NVGpaint;

enum NVGwinding {
    NVG_CCW = 1,            // Намотка для твердых форм
    NVG_CW = 2,             // Намотка для отверстий
};

enum NVGsolidity {
    NVG_SOLID = 1,          // CCW
    NVG_HOLE = 2,           // CW
};

enum NVGlineCap {
    NVG_BUTT,
    NVG_ROUND,
    NVG_SQUARE,
    NVG_BEVEL,
    NVG_MITER,
};

enum NVGalign {
    // Горизонтальное выравнивание
    NVG_ALIGN_LEFT      = 1 << 0, // По умолчанию выравнивание текста горизонтально по левому краю.
    NVG_ALIGN_CENTER    = 1 << 1, // Выровняйте текст по горизонтали по центру.
    NVG_ALIGN_RIGHT     = 1 << 2, // Выровняйте текст по горизонтали вправо.
    // Вертикальное выравнивание
    NVG_ALIGN_TOP       = 1 << 3, // Выровняйте текст по вертикали сверху.
    NVG_ALIGN_MIDDLE    = 1 << 4, // Выровняйте текст вертикально по центру.
    NVG_ALIGN_BOTTOM    = 1 << 5, // Выровняйте текст вертикально вниз.
    NVG_ALIGN_BASELINE  = 1 << 6, // По умолчанию текст выравнивается вертикально по базовой линии.
};

enum NVGblendFactor {
    NVG_ZERO = 1 << 0,
    NVG_ONE = 1 << 1,
    NVG_SRC_COLOR = 1 << 2,
    NVG_ONE_MINUS_SRC_COLOR = 1 << 3,
    NVG_DST_COLOR = 1 << 4,
    NVG_ONE_MINUS_DST_COLOR = 1 << 5,
    NVG_SRC_ALPHA = 1 << 6,
    NVG_ONE_MINUS_SRC_ALPHA = 1 << 7,
    NVG_DST_ALPHA = 1 << 8,
    NVG_ONE_MINUS_DST_ALPHA = 1 << 9,
    NVG_SRC_ALPHA_SATURATE = 1 << 10,
};

enum NVGcompositeOperation {
    NVG_SOURCE_OVER,
    NVG_SOURCE_IN,
    NVG_SOURCE_OUT,
    NVG_ATOP,
    NVG_DESTINATION_OVER,
    NVG_DESTINATION_IN,
    NVG_DESTINATION_OUT,
    NVG_DESTINATION_ATOP,
    NVG_LIGHTER,
    NVG_COPY,
    NVG_XOR,
};

struct NVGcompositeOperationState {
    int srcRGB;
    int dstRGB;
    int srcAlpha;
    int dstAlpha;
};
typedef struct NVGcompositeOperationState NVGcompositeOperationState;

struct NVGglyphPosition {
    const char * str;   // Позиция глифа во входной строке.
    float x;            // Координата X логического положения глифа.
    float minx, maxx;   // Границы формы глифа.
};
typedef struct NVGglyphPosition NVGglyphPosition;

struct NVGtextRow {
    const char * start; // Указатель на входной текст, где начинается строка.
    const char * end;   // Указатель на входной текст, где заканчивается строка (на один после последнего символа).
    const char * next;  // Указатель на начало следующей строки.
    float width;        // Логическая ширина строки.
    float minx,
          maxx;   // Фактические границы строки. Логические операции и границы могут различаться из-за кернинга и расширения некоторых частей.
};
typedef struct NVGtextRow NVGtextRow;

enum NVGimageFlags {
    NVG_IMAGE_GENERATE_MIPMAPS  = 1 << 0,   // Генерируйте MIP-карты во время создания изображения.
    NVG_IMAGE_REPEATX           = 1 << 1,   // Повторите изображение в направлении X.
    NVG_IMAGE_REPEATY           = 1 << 2,   // Повторите изображение в направлении Y.
    NVG_IMAGE_FLIPY             = 1 << 3,   // Переворачивает (инвертирует) изображение в направлении Y при рендеринге.
    NVG_IMAGE_PREMULTIPLIED     = 1 << 4,   // Данные изображения имеют предварительно умноженную альфу.
    NVG_IMAGE_NEAREST           = 1 << 5,   // Интерполяция изображения является ближайшей, а не линейной.
};

// Начинаем рисовать новую рамку
// Вызовы рисования nanovg API должны быть заключены в nvgBeginFrame() и nvgEndFrame().
// nvgBeginFrame() определяет размер окна для рендеринга относительно текущего момента.
// установите область просмотра (т. е. glViewport на бэкэндах GL). Соотношение пикселей устройства позволяет
// управлять рендерингом на устройствах Hi-DPI.
// Например, GLFW возвращает два измерения открытого окна: размер окна и
// размер буфера кадра. В этом случае вы должны установить windowWidth/Height в размер окна.
// devicePixelRatio к:frameBufferWidth/windowWidth.
void nvgBeginFrame(NVGcontext * ctx, float windowWidth, float windowHeight, float devicePixelRatio);

// Отменяет рисование текущего кадра.
void nvgCancelFrame(NVGcontext * ctx);

// Завершает рисование, сбрасывая оставшееся состояние рендеринга.
void nvgEndFrame(NVGcontext * ctx);

//
// Композитная операция
//
// Составные операции в NanoVG смоделированы по образцу HTML Canvas API и
// функция смешивания основана на OpenGL (дополнительную информацию см. в соответствующих руководствах).
// Цвета в состоянии смешивания имеют предварительно умноженную альфу.

// Устанавливает составную операцию. Параметр op должен быть одним из NVGcompositeOperation.
void nvgGlobalCompositeOperation(NVGcontext * ctx, int op);

// Устанавливает составную операцию с пользовательской пиксельной арифметикой. Параметры должны быть одним из NVGblendFactor.
void nvgGlobalCompositeBlendFunc(NVGcontext * ctx, int sfactor, int dfactor);

// Устанавливает составную операцию с пользовательской пиксельной арифметикой для RGB и альфа-компонентов отдельно. Параметры должны быть одним из NVGblendFactor.
void nvgGlobalCompositeBlendFuncSeparate(NVGcontext * ctx, int srcRGB, int dstRGB, int srcAlpha, int dstAlpha);

//
// Цветовые утилиты
//
// Цвета в NanoVG хранятся как целые числа без знака в формате ABGR.

// Возвращает значение цвета из значений красного, зеленого и синего. Альфа будет установлена ​​на 255 (1.0f).
NVGcolor nvgRGB(unsigned char r, unsigned char g, unsigned char b);

// Возвращает значение цвета из значений красного, зеленого и синего. Альфа будет установлена ​​на 1.0f.
NVGcolor nvgRGBf(float r, float g, float b);


// Возвращает значение цвета из значений красного, зеленого, синего и альфа.
NVGcolor nvgRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

// Возвращает значение цвета из значений красного, зеленого, синего и альфа.
NVGcolor nvgRGBAf(float r, float g, float b, float a);


// Линейно интерполирует цвет от c0 до c1 и возвращает результирующее значение цвета.
NVGcolor nvgLerpRGBA(NVGcolor c0, NVGcolor c1, float u);

// Устанавливает прозрачность значения цвета.
NVGcolor nvgTransRGBA(NVGcolor c0, unsigned char a);

// Устанавливает прозрачность значения цвета.
NVGcolor nvgTransRGBAf(NVGcolor c0, float a);

// Возвращает значение цвета, заданное оттенком, насыщенностью и яркостью.
// Все значения HSL находятся в диапазоне [0..1], альфа будет установлена на 255.
NVGcolor nvgHSL(float h, float s, float l);

// Возвращает значение цвета, заданное оттенком, насыщенностью, яркостью и альфа-каналом.
// Все значения HSL находятся в диапазоне [0..1], альфа в диапазоне [0..255]
NVGcolor nvgHSLA(float h, float s, float l, unsigned char a);

//
// Обработка состояния
//
// NanoVG содержит состояние, которое показывает, как будут отображаться пути.
// Состояние содержит стили преобразования, заливки и обводки, стили текста и шрифта,
// и стрижка ножницами.

// Отправляет и сохраняет текущее состояние рендеринга в стек состояний.
// Для восстановления состояния необходимо использовать соответствующий nvgRestore().
void nvgSave(NVGcontext * ctx);

// Вызывает и восстанавливает текущее состояние рендеринга.
void nvgRestore(NVGcontext * ctx);

// Сбрасывает текущее состояние рендеринга к значениям по умолчанию. Не влияет на стек состояний рендеринга.
void nvgReset(NVGcontext * ctx);

//
// Стили рендеринга
//
// Стиль рендеринга заливки и обводки может быть либо сплошным цветом, либо краской, которая представляет собой градиент или узор.
// Сплошной цвет определяется просто как значение цвета, можно создавать различные виды красок.
// используя nvgLinearGradient(), nvgBoxGradient(), nvgRadialGradient() и nvgImagePattern().
//
// Текущий стиль рендеринга можно сохранить и восстановить с помощью nvgSave() и nvgRestore().

// Устанавливает, рисовать ли сглаживание для nvgStroke() и nvgFill(). Он включен по умолчанию.
void nvgShapeAntiAlias(NVGcontext * ctx, int enabled);

// Устанавливает текущий стиль обводки сплошным цветом.
void nvgStrokeColor(NVGcontext * ctx, NVGcolor color);

// Устанавливает текущий стиль обводки в качестве краски, которая может быть одним из градиентов или узором.
void nvgStrokePaint(NVGcontext * ctx, NVGpaint paint);

// Устанавливает текущий стиль заливки сплошным цветом.
void nvgFillColor(NVGcontext * ctx, NVGcolor color);

// Устанавливает текущий стиль заливки в виде краски, которая может быть одним из градиентов или узором.
void nvgFillPaint(NVGcontext * ctx, NVGpaint paint);

// Устанавливает предел угла скоса для стиля штриха.
// Ограничение скоса контролируется при скосе острого угла.
void nvgMiterLimit(NVGcontext * ctx, float limit);

// Устанавливает ширину обводки стиля обводки.
void nvgStrokeWidth(NVGcontext * ctx, float size);

// Устанавливает способ рисования конца линии (конца),
// Может быть одним из: NVG_BUTT (по умолчанию), NVG_ROUND, NVG_SQUARE.
void nvgLineCap(NVGcontext * ctx, int cap);

// Устанавливает, насколько острые углы контура прорисовываются.
// Может быть одним из NVG_MITER (по умолчанию), NVG_ROUND, NVG_BEVEL.
void nvgLineJoin(NVGcontext * ctx, int join);

// Устанавливает прозрачность, применяемую ко всем отображаемым фигурам.
// Уже прозрачные пути станут пропорционально более прозрачными.
void nvgGlobalAlpha(NVGcontext * ctx, float alpha);

//
// Преобразует
//
// Пути, градиенты, узоры и ножничная область преобразуются преобразованием
// матрица в момент их передачи в API.
// Текущая матрица преобразования является аффинной матрицей:
//   [sx kx tx]
//   [ky sy ty]
//   [ 0  0  1]
// Where: sx,sy define scaling, kx,ky skewing, and tx,ty translation.
// Предполагается, что последняя строка равна 0,0,1 и не сохраняется.
//
// Помимо nvgResetTransform(), каждая функция преобразования сначала создает
// конкретную матрицу преобразования и предварительно умножает на нее текущее преобразование.
//
// Текущую систему координат (преобразование) можно сохранить и восстановить с помощью nvgSave() и nvgRestore().

// Сбрасывает текущее преобразование в единичную матрицу.
void nvgResetTransform(NVGcontext * ctx);

// Предварительно умножает текущую систему координат на указанную матрицу.
// Параметры интерпретируются как матричные следующим образом:
//   [a c e]
//   [b d f]
//   [0 0 1]
void nvgTransform(NVGcontext * ctx, float a, float b, float c, float d, float e, float f);

// Переводит текущую систему координат.
void nvgTranslate(NVGcontext * ctx, float x, float y);

// Поворачивает текущую систему координат. Угол указывается в радианах.
void nvgRotate(NVGcontext * ctx, float angle);

// Смещает текущую систему координат по оси X. Угол указывается в радианах.
void nvgSkewX(NVGcontext * ctx, float angle);

// Смещает текущую систему координат по оси Y. Угол указывается в радианах.
void nvgSkewY(NVGcontext * ctx, float angle);

// Масштабирует текущую систему координат.
void nvgScale(NVGcontext * ctx, float x, float y);

// Сохраняет верхнюю часть (a-f) текущей матрицы преобразования в указанный буфер.
//   [a c e]
//   [b d f]
//   [0 0 1]
// В буфере возврата должно быть место для 6 чисел с плавающей запятой для значений a-f.
void nvgCurrentTransform(NVGcontext * ctx, float * xform);


// Следующие функции можно использовать для вычислений с матрицами преобразования 2x3.
// Матрица 2x3 представлена как float[6].

// Устанавливает преобразование в единичную матрицу.
void nvgTransformIdentity(float * dst);

// Устанавливает преобразование в матрицу перевода.
void nvgTransformTranslate(float * dst, float tx, float ty);

// Устанавливает преобразование для масштабирования матрицы.
void nvgTransformScale(float * dst, float sx, float sy);

// Устанавливает преобразование для поворота матрицы. Угол указывается в радианах.
void nvgTransformRotate(float * dst, float a);

// Устанавливает преобразование в матрицу skew-x. Угол указывается в радианах.
void nvgTransformSkewX(float * dst, float a);

// Устанавливает преобразование в асимметричную матрицу. Угол указывается в радианах.
void nvgTransformSkewY(float * dst, float a);

// Устанавливает преобразование в результат умножения двух преобразований A = A*B.
void nvgTransformMultiply(float * dst, const float * src);

// Устанавливает преобразование в результат умножения двух преобразований A = B*A.
void nvgTransformPremultiply(float * dst, const float * src);

// Устанавливает место назначения, обратное указанному преобразованию.
// Возвращает 1, если обратное значение можно вычислить, иначе 0.
int nvgTransformInverse(float * dst, const float * src);

// Преобразовать точку заданным преобразованием.
void nvgTransformPoint(float * dstx, float * dsty, const float * xform, float srcx, float srcy);

// Преобразует градусы в радианы и наоборот.
float nvgDegToRad(float deg);
float nvgRadToDeg(float rad);

//
// Изображения
//
// NanoVG позволяет загружать файлы jpg, png, psd, tga, pic и gif, которые будут использоваться для рендеринга.
// Кроме того, вы можете загрузить свое собственное изображение. Загрузку изображения обеспечивает stb_image.
// Параметр imageFlags представляет собой комбинацию флагов, определенных в NVGimageFlags.

// Создает изображение из указанных данных изображения в пользовательском формате.
// format: see NVGtexture.
// Возвращает дескриптор изображения.
int nvgCreateImage(NVGcontext * ctx, int w, int h, int imageFlags, int format, const unsigned char * data);

// Обновляет данные изображения, указанные дескриптором изображения.
void nvgUpdateImage(NVGcontext * ctx, int image, const unsigned char * data);

// Возвращает размеры созданного изображения.
void nvgImageSize(NVGcontext * ctx, int image, int * w, int * h);

// Удаляет созданное изображение.
void nvgDeleteImage(NVGcontext * ctx, int image);

//
// Краски
//
// NanoVG поддерживает четыре типа красок: линейный градиент, прямоугольный градиент, радиальный градиент и узор изображения.
// Их можно использовать в качестве красок для штрихов и заливок.

// Создает и возвращает линейный градиент. Параметры (sx,sy)-(ex,ey) определяют координаты начала и конца.
// Для линейного градиента icol определяет начальный цвет, а ocol — конечный цвет.
// Градиент преобразуется текущим преобразованием, когда он передается nvgFillPaint() или nvgStrokePaint().
NVGpaint nvgLinearGradient(NVGcontext * ctx, float sx, float sy, float ex, float ey,
                           NVGcolor icol, NVGcolor ocol);

// Создает и возвращает градиент прямоугольника. Градиент коробки — это растушеванный прямоугольник со скругленными углами, он полезен для рендеринга.
// тени или блики для коробок. Параметры (x,y) определяют верхний левый угол прямоугольника.
// (w,h) определяют размер прямоугольника, r определяет радиус угла, а f растушевка. Растушевка определяет, насколько размыта
// граница прямоугольника. Параметр icol определяет внутренний цвет, а ocol — внешний цвет градиента.
// Градиент преобразуется текущим преобразованием, когда он передается nvgFillPaint() или nvgStrokePaint().
NVGpaint nvgBoxGradient(NVGcontext * ctx, float x, float y, float w, float h,
                        float r, float f, NVGcolor icol, NVGcolor ocol);

// Создает и возвращает радиальный градиент. Параметры (cx,cy) определяют центр, inr и outr определяют
// внутренний и внешний радиус градиента, icol определяет начальный цвет, а ocol — конечный цвет.
// Градиент преобразуется текущим преобразованием, когда он передается nvgFillPaint() или nvgStrokePaint().
NVGpaint nvgRadialGradient(NVGcontext * ctx, float cx, float cy, float inr, float outr,
                           NVGcolor icol, NVGcolor ocol);

// Создает и возвращает шаблон изображения. Параметры (ox,oy) определяют расположение шаблона изображения слева вверху.
// (например, ey) размер одного изображения, угол поворота вокруг верхнего левого угла, изображение — это дескриптор изображения для рендеринга.
// Градиент преобразуется текущим преобразованием, когда он передается nvgFillPaint() или nvgStrokePaint().
NVGpaint nvgImagePattern(NVGcontext * ctx, float ox, float oy, float ex, float ey,
                         float angle, int image, float alpha);

//
// ножницы
//
// Вырезание позволяет вам обрезать рендеринг в прямоугольник. Это полезно для различных
// случаи пользовательского интерфейса, такие как рендеринг редактирования текста или временной шкалы.

// Устанавливает текущий прямоугольник-ножницы.
// Прямоугольник-ножницы преобразуется текущим преобразованием.
void nvgScissor(NVGcontext * ctx, float x, float y, float w, float h);

// Пересекает текущий прямоугольник-ножницы с указанным прямоугольником.
// Прямоугольник-ножницы преобразуется текущим преобразованием.
// Note: in case the rotation of previous scissor rect differs from
// текущий, пересечение будет выполнено между указанными
// прямоугольник и предыдущий прямоугольник-ножницы преобразуются в текущий
// трансформировать пространство. Полученная форма всегда является прямоугольником.
void nvgIntersectScissor(NVGcontext * ctx, float x, float y, float w, float h);

// Сброс и отключение ножниц.
void nvgResetScissor(NVGcontext * ctx);

//
// Пути
//
// Рисование новой фигуры начинается с nvgBeginPath(), при этом очищаются все определенные в данный момент пути.
// Затем вы определяете один или несколько путей и подпутей, описывающих форму. Это функции
// рисовать общие формы, такие как прямоугольники и круги, а также пошаговые функции более низкого уровня,
// которые позволяют определять кривую пути за кривой.
//
// NanoVG использует правило заливки чет-нечет для рисования фигур. Твердые фигуры должны располагаться против часовой стрелки.
// обмотка и отверстия должны располагаться против часовой стрелки. Указать извилистость пути можно
// позвоните nvgPathWinding(). Это особенно полезно для распространенных фигур, которые рисуются CCW.
//
// Наконец, вы можете заполнить путь, используя текущий стиль заливки, вызвав nvgFill() и обведя его.
// с текущим стилем штриха, вызвав nvgStroke() .
//
// Сегменты кривой и подпути преобразуются текущим преобразованием.

// Очищает текущий путь и подпути.
void nvgBeginPath(NVGcontext * ctx);

// Начинает новый подпуть с указанной точки в качестве первой точки.
void nvgMoveTo(NVGcontext * ctx, float x, float y);

// Добавляет сегмент линии из последней точки пути к указанной точке.
void nvgLineTo(NVGcontext * ctx, float x, float y);

// Добавляет кубический сегмент Безье от последней точки пути через две контрольные точки до указанной точки.
void nvgBezierTo(NVGcontext * ctx, float c1x, float c1y, float c2x, float c2y, float x, float y);

// Добавляет квадратичный сегмент Безье из последней точки пути через контрольную точку в указанную точку.
void nvgQuadTo(NVGcontext * ctx, float cx, float cy, float x, float y);

// Добавляет сегмент дуги в углу, определенном последней точкой пути, и двух указанных точках.
void nvgArcTo(NVGcontext * ctx, float x1, float y1, float x2, float y2, float radius);

// Закрывает текущий подпуть отрезком линии.
void nvgClosePath(NVGcontext * ctx);

// Устанавливает текущую обмотку подпути, см. NVGwinding и NVGsolidity.
void nvgPathWinding(NVGcontext * ctx, int dir);

// Создает новый подпуть в форме дуги окружности. Центр дуги находится в cx,cy, радиус дуги равен r,
// и дуга рисуется от угла a0 до a1 и движется в направлении dir ( NVG_CCW или NVG_CW ).
// Углы указаны в радианах.
void nvgArc(NVGcontext * ctx, float cx, float cy, float r, float a0, float a1, int dir);

// Создает новый подпуть в форме прямоугольника.
void nvgRect(NVGcontext * ctx, float x, float y, float w, float h);

// Создает новый подпуть в форме закругленного прямоугольника.
void nvgRoundedRect(NVGcontext * ctx, float x, float y, float w, float h, float r);

// Создает новый подпуть в форме скругленного прямоугольника с различными радиусами для каждого угла.
void nvgRoundedRectVarying(NVGcontext * ctx, float x, float y, float w, float h, float radTopLeft, float radTopRight,
                           float radBottomRight, float radBottomLeft);

// Создает новый подпуть в форме эллипса.
void nvgEllipse(NVGcontext * ctx, float cx, float cy, float rx, float ry);

// Создает новый подпуть в форме круга.
void nvgCircle(NVGcontext * ctx, float cx, float cy, float r);

// Заполняет текущий путь текущим стилем заливки.
void nvgFill(NVGcontext * ctx);

// Заполняет текущий путь текущим стилем обводки.
void nvgStroke(NVGcontext * ctx);


//
// Текст
//
// NanoVG позволяет загружать файлы .ttf и использовать шрифт для рендеринга текста.
//
// Внешний вид текста можно определить, установив текущий стиль текста.
// и указав цвет заливки. Общие настройки текста и шрифта, такие как
// Поддерживаются размер шрифта, межбуквенный интервал и выравнивание текста. Размытие шрифта позволяет вам
// для создания простых текстовых эффектов, таких как тени.
//
// Во время рендеринга шрифт может быть установлен на основе дескрипторов или имени шрифта.
//
// Функции измерения шрифта возвращают значения в локальном пространстве, вычисления
// выполняется в том же разрешении, что и окончательный рендеринг. Это сделано потому, что
// Позиции текстовых глифов привязываются к четкому рендерингу с точностью до ближайших пикселей.
//
// Локальное пространство означает, что значения не поворачиваются и не масштабируются в соответствии с текущим значением.
// трансформация. Например, если вы установите размер шрифта равным 12, это будет означать, что
// высота строки равна 16, то независимо от текущего масштабирования и поворота
// высота возвращаемой строки всегда равна 16. Некоторые показатели могут отличаться из-за масштабирования.
// начиная с вышеупомянутой привязки пикселей.
//
// Хотя это может показаться немного странным, настройка позволяет всегда отображать
// одинаково независимо от масштабирования. Т.е. следующие работы независимо от масштабирования:
//
//      const char* txt = "Напиши мне.";
//      nvgTextBounds(vg, x,y, txt, NULL, границы);
//      nvgBeginPath (вг);
//      nvgRect(vg,bounds[0],bounds[1],bounds[2]-bounds[0],bounds[3]-bounds[1]);
//      nvgFill (вг);
//
// Note: currently only solid color fill is supported for text.

// Создает шрифт, загружая его с диска из указанного имени файла.
// Возвращает дескриптор шрифта.
int nvgCreateFont(NVGcontext * ctx, const char * name, const char * filename);

// FontIndex указывает, какой шрифт загружается из файла .ttf/.ttc.
int nvgCreateFontAtIndex(NVGcontext * ctx, const char * name, const char * filename, const int fontIndex);

// Создает шрифт, загружая его из указанного фрагмента памяти.
// Возвращает дескриптор шрифта.
int nvgCreateFontMem(NVGcontext * ctx, const char * name, unsigned char * data, int ndata, int freeData);

// FontIndex указывает, какой шрифт загружается из файла .ttf/.ttc.
int nvgCreateFontMemAtIndex(NVGcontext * ctx, const char * name, unsigned char * data, int ndata, int freeData,
                            const int fontIndex);

// Находит загруженный шрифт с указанным именем и возвращает его дескриптор или -1, если шрифт не найден.
int nvgFindFont(NVGcontext * ctx, const char * name);

// Добавляет запасной шрифт по дескриптору.
int nvgAddFallbackFontId(NVGcontext * ctx, int baseFont, int fallbackFont);

// Добавляет запасной шрифт по имени.
int nvgAddFallbackFont(NVGcontext * ctx, const char * baseFont, const char * fallbackFont);

// Сбрасывает резервные шрифты по дескриптору.
void nvgResetFallbackFontsId(NVGcontext * ctx, int baseFont);

// Сбрасывает резервные шрифты по имени.
void nvgResetFallbackFonts(NVGcontext * ctx, const char * baseFont);

// Устанавливает размер шрифта текущего стиля текста.
void nvgFontSize(NVGcontext * ctx, float size);

// Устанавливает размытие текущего стиля текста.
void nvgFontBlur(NVGcontext * ctx, float blur);

// Устанавливает интервал между буквами текущего стиля текста.
void nvgTextLetterSpacing(NVGcontext * ctx, float spacing);

// Устанавливает пропорциональную высоту строки текущего стиля текста. Высота строки задается кратной размеру шрифта.
void nvgTextLineHeight(NVGcontext * ctx, float lineHeight);

// Устанавливает выравнивание текста текущего стиля текста, параметры см. в разделе NVGalign.
void nvgTextAlign(NVGcontext * ctx, int align);

// Устанавливает вид шрифта на основе указанного идентификатора текущего стиля текста.
void nvgFontFaceId(NVGcontext * ctx, int font);

// Устанавливает вид шрифта на основе указанного имени текущего стиля текста.
void nvgFontFace(NVGcontext * ctx, const char * font);

// Рисует текстовую строку в указанном месте. Если указан конец, рисуется только подстрока до конца.
float nvgText(NVGcontext * ctx, float x, float y, const char * string, const char * end);

// Рисует многострочную текстовую строку в указанном месте, завернутую в указанную ширину. Если указан конец, рисуется только подстрока до конца.
// Пробелы удаляются в начале строк, текст разделяется по границам слов или при появлении символов новой строки.
// Слова длиннее максимальной ширины разрезаются по ближайшему символу (т. е. без переносов).
void nvgTextBox(NVGcontext * ctx, float x, float y, float breakRowWidth, const char * string, const char * end);

// Измеряет указанную текстовую строку. Границы параметра должны быть указателем на float[4],
// если должна быть возвращена ограничивающая рамка текста. Значения границ: [xmin,ymin, xmax,ymax]
// Возвращает горизонтальное перемещение измеренного текста (т. е. места, где должен быть нарисован следующий символ).
// Измеренные значения возвращаются в локальном пространстве координат.
float nvgTextBounds(NVGcontext * ctx, float x, float y, const char * string, const char * end, float * bounds);

// Измеряет указанную многотекстовую строку. Границы параметра должны быть указателем на float[4],
// если должна быть возвращена ограничивающая рамка текста. Значения границ: [xmin,ymin, xmax,ymax]
// Измеренные значения возвращаются в локальном пространстве координат.
void nvgTextBoxBounds(NVGcontext * ctx, float x, float y, float breakRowWidth, const char * string, const char * end,
                      float * bounds);

// Вычисляет позиции x глифа указанного текста. Если указан конец, будет использоваться только подстрока.
// Измеренные значения возвращаются в локальном пространстве координат.
int nvgTextGlyphPositions(NVGcontext * ctx, float x, float y, const char * string, const char * end,
                          NVGglyphPosition * positions, int maxPositions);

// Возвращает вертикальные показатели на основе текущего стиля текста.
// Измеренные значения возвращаются в локальном пространстве координат.
void nvgTextMetrics(NVGcontext * ctx, float * ascender, float * descender, float * lineh);

// Разбивает указанный текст на строки. Если указан конец, будет использоваться только подстрока.
// Пробелы удаляются в начале строк, текст разделяется по границам слов или при появлении символов новой строки.
// Слова длиннее максимальной ширины разрезаются по ближайшему символу (т. е. без переносов).
int nvgTextBreakLines(NVGcontext * ctx, const char * string, const char * end, float breakRowWidth, NVGtextRow * rows,
                      int maxRows);

//
// Внутренний рендер API
//
enum NVGtexture {
    NVG_TEXTURE_ALPHA = 0x01,
    NVG_TEXTURE_BGRA = 0x02,   /* Формат ARGB8888 (порядок памяти: B-G-R-A) */
    NVG_TEXTURE_RGBA = 0x03,   /* Стандартный формат OpenGL RGBA */
    NVG_TEXTURE_BGR = 0x04,    /* Формат RGB888 (порядок памяти: B-G-R) */
    NVG_TEXTURE_RGB565 = 0x05, /* Формат RGB565 */
    NVG_TEXTURE_BGRX = 0x06,   /* Формат XRGB8888 (порядок памяти: B-G-R-X, X игнорируется) */
};

struct NVGscissor {
    float xform[6];
    float extent[2];
};
typedef struct NVGscissor NVGscissor;

struct NVGvertex {
    float x, y, u, v;
};
typedef struct NVGvertex NVGvertex;

struct NVGpath {
    int first;
    int count;
    unsigned char closed;
    int nbevel;
    NVGvertex * fill;
    int nfill;
    NVGvertex * stroke;
    int nstroke;
    int winding;
    int convex;
};
typedef struct NVGpath NVGpath;

struct NVGparams {
    void * userPtr;
    int edgeAntiAlias;
    int (*renderCreate)(void * uptr);
    int (*renderCreateTexture)(void * uptr, int type, int w, int h, int imageFlags, const unsigned char * data);
    int (*renderDeleteTexture)(void * uptr, int image);
    int (*renderUpdateTexture)(void * uptr, int image, int x, int y, int w, int h, const unsigned char * data);
    int (*renderGetTextureSize)(void * uptr, int image, int * w, int * h);
    void (*renderViewport)(void * uptr, float width, float height, float devicePixelRatio);
    void (*renderCancel)(void * uptr);
    void (*renderFlush)(void * uptr);
    void (*renderFill)(void * uptr, NVGpaint * paint, NVGcompositeOperationState compositeOperation, NVGscissor * scissor,
                       float fringe, const float * bounds, const NVGpath * paths, int npaths);
    void (*renderStroke)(void * uptr, NVGpaint * paint, NVGcompositeOperationState compositeOperation, NVGscissor * scissor,
                         float fringe, float strokeWidth, const NVGpath * paths, int npaths);
    void (*renderTriangles)(void * uptr, NVGpaint * paint, NVGcompositeOperationState compositeOperation,
                            NVGscissor * scissor, const NVGvertex * verts, int nverts, float fringe);
    void (*renderDelete)(void * uptr);
};
typedef struct NVGparams NVGparams;

// Конструктор и деструктор, вызываемые серверной частью рендеринга.
NVGcontext * nvgCreateInternal(NVGparams * params);
void nvgDeleteInternal(NVGcontext * ctx);

NVGparams * nvgInternalParams(NVGcontext * ctx);

// Функция отладки для сброса данных кэшированного пути.
void nvgDebugDumpPathCache(NVGcontext * ctx);

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#define NVG_NOTUSED(v) for (;;) { (void)(1 ? (void)0 : ( (void)(v) ) ); break; }

#endif // LV_USE_NANOVG

#ifdef __cplusplus
}
#endif

#endif // NANOVG_H
