#ifndef _THORVG_H_
#define _THORVG_H_

#include "../../lv_conf_internal.h"

/*Тестирование зависимостей*/
#if LV_USE_THORVG && LV_USE_VECTOR_GRAPHIC == 0
#error "ThorVG: LV_USE_VECTOR_GRAPHIC is required. Enable it in lv_conf.h"
#endif

#if LV_USE_THORVG_INTERNAL
#define TVG_BUILD 1


#include <functional>
#include <cstdint>
#include <memory>
#include <string>
#include <list>
#include "../../stdlib/lv_mem.h"
#include "../../stdlib/lv_string.h"
#include "../../misc/lv_assert.h"

#ifdef TVG_API
    #undef TVG_API
#endif

#ifndef TVG_STATIC
    #ifdef _WIN32
        #if TVG_BUILD
            #define TVG_API __declspec(dllexport)
        #else
            #define TVG_API __declspec(dllimport)
        #endif
    #elif (defined(__SUNPRO_C)  || defined(__SUNPRO_CC))
        #define TVG_API __global
    #else
        #if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__INTEL_COMPILER)
            #define TVG_API __attribute__ ((visibility("default")))
        #else
            #define TVG_API
        #endif
    #endif
#else
    #define TVG_API
#endif

#ifdef TVG_DEPRECATED
    #undef TVG_DEPRECATED
#endif

#ifdef _WIN32
    #define TVG_DEPRECATED __declspec(deprecated)
#elif __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
    #define TVG_DEPRECATED __attribute__ ((__deprecated__))
#else
    #define TVG_DEPRECATED
#endif

#define _TVG_DECLARE_PRIVATE(A) \
    struct Impl; \
    Impl* pImpl; \
protected: \
    A(const A&) = delete; \
    const A& operator=(const A&) = delete; \
    A()

#define _TVG_DISABLE_CTOR(A) \
    A() = delete; \
    ~A() = delete

#define _TVG_DECLARE_ACCESSOR(A) \
    friend A

namespace tvg
{

class RenderMethod;
class Animation;

/**
 * @defgroup ТорВГ ТорВГ
 * @brief ThorVG classes and enumerations providing C++ APIs.
 */

/**@{*/

/**
 * @brief Enumeration specifying the result from the APIs.
 *
 * Все API ThorVG потенциально могут возвращать одно из значений в списке.
 * Обратите внимание, что некоторые API могут дополнительно указывать причины, по которым возвращаются их значения.
 *
 */
enum class Result
{
    Success = 0,           ///< Значение, возвращаемое в случае корректного выполнения запроса.
    InvalidArguments,      ///< Значение, возвращаемое в случае проблемы с аргументами, переданными в API - например. пустые пути или нулевые указатели.
    InsufficientCondition, ///< Значение, возвращаемое в случае, если запрос не может быть обработан - например. запрос свойств объекта, которого не существует.
    FailedAllocation,      ///< Значение, возвращаемое в случае неудачного выделения памяти.
    MemoryCorruption,      ///< Значение, возвращаемое в случае неправильной обработки памяти – например. сбой при освобождении или приведении указателя
    NonSupport,            ///< Значение, возвращаемое в случае выбора неподдерживаемых функций (опций) движка.
    Unknown                ///< Значение, возвращаемое во всех остальных случаях.
};


/**
 * @brief Enumeration specifying the values of the path commands accepted by TVG.
 *
 * Не путать с командами пути из элемента пути svg (например, M, L, Q, H и многими другими).
 * TVG интерпретирует их все и преобразует в значения PathCommand.
 */
enum class PathCommand
{
    Close = 0, ///< Заканчивает текущий подпуть и соединяет его с начальной точкой. Эта команда не ожидает никаких очков.
    MoveTo,    ///< Устанавливает новую начальную точку подпути и новую текущую точку. Эта команда ожидает 1 точку: стартовую позицию.
    LineTo,    ///< Рисует линию от текущей точки до заданной точки и устанавливает новое значение текущей точки. Эта команда ожидает 1 точку: конечную позицию линии.
    CubicTo    ///< Рисует кубическую кривую Безье от текущей точки до заданной точки, используя две заданные контрольные точки, и устанавливает новое значение текущей точки. Эта команда ожидает 3 точки: 1-я контрольная точка, 2-я контрольная точка и конечная точка кривой.
};


/**
 * @brief Enumeration determining the ending type of a stroke in the open sub-paths.
 */
enum class StrokeCap
{
    Square = 0, ///< Обводка расширяется в обеих конечных точках подпути на прямоугольник с шириной, равной ширине обводки, и длиной, равной половине ширины обводки. Для подпутей нулевой длины квадрат отображается с размером ширины штриха.
    Round,      ///< Штрих расширяется в обеих конечных точках подпути на полукруг с радиусом, равным половине ширины штриха. Для подпутей нулевой длины отображается полный круг.
    Butt        ///< Штрих заканчивается точно в каждой из двух конечных точек подпути. Для подпутей нулевой длины штрих не отображается.
};


/**
 * @brief Enumeration determining the style used at the corners of joined stroked path segments.
 */
enum class StrokeJoin
{
    Bevel = 0, ///< Внешний угол соединяемых сегментов пути скошен в точке соединения. Треугольная область угла ограничена прямой линией между внешними углами каждого штриха.
    Round,     ///< Внешний угол соединяемых сегментов пути закруглен. Круглая область центрируется в точке соединения.
    Miter      ///< Внешний угол соединенных сегментов пути имеет шипы. Шип создается путем расширения за пределы точки соединения внешних краев обводки до их пересечения. Если расширение выходит за пределы ограничения, стиль соединения преобразуется в стиль «Скос».
};


/**
 * @brief Enumeration specifying how to fill the area outside the gradient bounds.
 */
enum class FillSpread
{
    Pad = 0, ///< Оставшаяся область заполняется ближайшим стоп-цветом.
    Reflect, ///< Узор градиента отражается за пределами области градиента до тех пор, пока ожидаемая область не будет заполнена.
    Repeat   ///< Шаблон градиента непрерывно повторяется за пределами области градиента, пока ожидаемая область не будет заполнена.
};


/**
 * @brief Enumeration specifying the algorithm used to establish which parts of the shape are treated as the inside of the shape.
 */
enum class FillRule
{
    Winding = 0, ///< Рисуется линия от точки до места за пределами фигуры. Подсчитываются пересечения линии с сегментом пути фигуры. Начиная с нуля, если сегмент пути фигуры пересекает линию по часовой стрелке, единица добавляется, в противном случае единица вычитается. Если полученная сумма не равна нулю, точка находится внутри фигуры.
    EvenOdd      ///< Рисуется линия от точки до места за пределами фигуры и подсчитываются ее пересечения с сегментами пути фигуры. Если количество пересечений нечетное, точка находится внутри фигуры.
};


/**
 * @brief Enumeration indicating the method used in the composition of two objects - the target and the source.
 *
 * Notation: S(Source), T(Target), SA(Source Alpha), TA(Target Alpha)
 *
 * @see Paint::composite()
 */
enum class CompositeMethod
{
    None = 0,           ///< Композиция не применяется.
    ClipPath,           ///< Определяется пересечение источника и цели, и визуализируются только результирующие пиксели из источника. Обратите внимание, что ClipPath поддерживает только тип Shape.  @deprecated Вместо этого используйте Paint:: clip().
    AlphaMask,          ///< Альфа-маскирование с использованием пикселей цели композиции в качестве значения альфа.
    InvAlphaMask,       ///< Альфа-маскирование с использованием дополнения к пикселям цели композиции в качестве значения альфа.
    LumaMask,           ///< Альфа-маскирование с использованием оттенков серого (0,2125R + 0,7154G + 0,0721*B) пикселей цели композитинга.  @since 0.9
    InvLumaMask,        ///< Альфа-маскирование с использованием оттенков серого (0,2125R + 0,7154G + 0,0721*B) дополнения к пикселям цели композиции.  @since 0.11
    AddMask,            ///< Объединяет пиксели целевого и исходного объектов с использованием целевой альфа. (T*TA) + (S*(255 - TA)) (Экспериментальный API)
    SubtractMask,       ///< Вычитает исходный цвет из целевого цвета, учитывая соответствующую целевую альфу. (T*TA) — (S*(255 — TA)) (Экспериментальный API)
    IntersectMask,      ///< Вычисляет результат, беря минимальное значение между целевой альфа и исходной альфа и умножая его на целевой цвет. (T * min( TA , SA )) (Экспериментальный API )
    DifferenceMask,     ///< Вычисляет абсолютную разницу между целевым цветом и исходным цветом, умноженную на дополнение целевой альфа. abs(T - S * (255 - TA )) (Экспериментальный API )
    LightenMask,        ///< При пересечении нескольких масок используется наибольшее значение прозрачности. (Экспериментальный API)
    DarkenMask          ///< При пересечении нескольких масок используется наименьшее значение прозрачности. (Экспериментальный API)
};


/**
 * @brief Enumeration indicates the method used for blending paint. Please refer to the respective formulas for each method.
 *
 * Notation: S(source paint as the top layer), D(destination as the bottom layer), Sa(source paint alpha), Da(destination alpha)
 *
 * @see Paint::blend()
 *
 * @since 0,15
 */
enum class BlendMethod : uint8_t
{
    Normal = 0,        ///< Выполняем альфа-смешение (по умолчанию). S если (Sa == 255), иначе (Sa * S) + (255 - Sa) * D
    Multiply,          ///< Принимает значения канала RGB от 0 до 255 для каждого пикселя верхнего слоя и умножает их на значения для соответствующего пикселя нижнего слоя. (С*Д)
    Screen,            ///< Значения пикселей в двух слоях инвертируются, умножаются, а затем снова инвертируются. (С+Д) - (С*Д)
    Overlay,           ///< Сочетает режимы наложения «Умножение» и «Экран». (2*С*Д), если (2*Д<Да), иначе (Са*Да) — 2*(Да — С)*(Са — Д)
    SrcOver,           ///< Заменить нижний слой верхним.
    Darken,            ///< Создает пиксель, который сохраняет наименьшие компоненты пикселей верхнего и нижнего слоев. мин(С, Д)
    Lighten,           ///< Only имеет действие, противоположное «Только затемнить». макс(С, Д)
    ColorDodge,        ///< Делит нижний слой на инвертированный верхний слой. Д/(255 - С)
    ColorBurn,         ///< Делит инвертированный нижний слой на верхний слой, а затем инвертирует результат. 255 - (255 - Д)/С
    HardLight,         ///< То же, что и Overlay, но с обратным расположением цветов. (2*С*Д), если (С<Са), иначе (Са*Да)-2*(Да-С)*(Са-Д)
    SoftLight,         ///< То же, что и наложение, но с применением чистого черного или белого цвета не получается чистый черный или белый цвет. (1 - 2*С)*(Д^2) + (2*С*Д)
    Difference,        ///< Вычитает нижний слой из верхнего слоя или наоборот, чтобы всегда получать неотрицательное значение. (S - D), если (S > D), иначе (D - S)
    Exclusion,         ///< Результат — удвоенное произведение верхнего и нижнего слоев, вычтенное из их суммы. с + д - (2 * с * д)
    Hue,               ///< Зарезервировано. Не поддерживается.
    Saturation,        ///< Зарезервировано. Не поддерживается.
    Color,             ///< Зарезервировано. Не поддерживается.
    Luminosity,        ///< Зарезервировано. Не поддерживается.
    Add,               ///< Просто добавляет значения пикселей одного слоя к другому. (С + Д)
    HardMix            ///< Зарезервировано. Не поддерживается.
};


/**
 * @brief Enumeration that defines methods used for Scene Effects.
 *
 * Это перечисление предоставляет возможности применения к сцене различных эффектов постобработки.
 * Эффекты сцены обычно применяются для изменения окончательного внешнего вида визуализированной сцены, например, для размытия.
 *
 * @see Scene::push(SceneEffect effect, ...)
 *
 * @note Experimental API
 */
enum class SceneEffect : uint8_t
{
    ClearAll = 0,      ///< Сброс всех ранее примененных эффектов сцены, возвращая сцену в исходное состояние.
    GaussianBlur       ///< Примените эффект размытия с помощью фильтра Гаусса. Param(3) = {sigma(float)[> 0], направление(int)[оба: 0 / по горизонтали: 1 / по вертикали: 2], граница (int)[дубликат: 0 / перенос: 1], качество (int)[0 - 100]}
};


/**
 * @brief Enumeration specifying the engine type used for the graphics backend. For multiple backends bitwise operation is allowed.
 */
enum class CanvasEngine
{
    Sw = (1 << 1), ///< Растеризатор CPU.
    Gl = (1 << 2), ///< Растеризатор OpenGL.
    Wg = (1 << 3), ///< Растеризатор WebGPU.  @since 0,15
};


/**
 * @brief Enumeration specifying the ThorVG class type value.
 *
 * Объекты рисования ThorVG могут возвращать значения типа класса, что позволяет вам идентифицировать конкретный класс каждого объекта.
 *
 * @see Paint::type()
 * @see Fill::type()
 *
 * @note Experimental API
 */
enum class Type : uint8_t
{
    Undefined = 0,         ///< Неизвестный класс
    Shape,                 ///< Класс формы
    Scene,                 ///< Класс сцены
    Picture,               ///< Класс изображения
    Text,                  ///< Текстовый класс
    LinearGradient = 10,   ///< Класс LinearGradient
    RadialGradient         ///< Класс RadialGradient
};


/**
 * @brief A data structure representing a point in two-dimensional space.
 */
struct Point
{
    float x, y;
};


/**
 * @brief A data structure representing a three-dimensional matrix.
 *
 * Элементы e11, e12, e21 и e22 представляют матрицу вращения, включая коэффициент масштабирования.
 * Элементы e13 и e23 определяют перемещение объекта по осям x и y соответственно.
 * Элементы e31 и e32 установлены в 0, e33 — в 1.
 */
struct Matrix
{
    float e11, e12, e13;
    float e21, e22, e23;
    float e31, e32, e33;
};


/**
 * @class Краска
 *
 * @brief An abstract class for managing graphical elements.
 *
 * Графический элемент в TVG — это любой объект, составленный в Canvas.
 * Paint представляет собой такой графический объект и его поведение, такое как дублирование, преобразование и композиция.
 * TVG рекомендует пользователю рассматривать покраску как набор нестабильных команд. Они могут подготовить Paint, а затем запросить Canvas для их запуска.
 */
class TVG_API Paint
{
public:
    virtual ~Paint();

    /**
     * @brief Sets the angle by which the object is rotated.
     *
     * Угол измеряется по часовой стрелке от горизонтальной оси.
     * Ось вращения проходит через точку на объекте с нулевыми координатами.
     *
     * @param [in] градус Значение угла в градусах.
     *
     * @retval Result::InsufficientCondition in case a custom transform is applied.
     * @see Paint::transform()
     */
    Result rotate(float degree) noexcept;

    /**
     * @brief Sets the scale value of the object.
     *
     * @param Коэффициент [in] Значение коэффициента масштабирования. Значение по умолчанию — 1.
     *
     * @retval Result::InsufficientCondition in case a custom transform is applied.
     * @see Paint::transform()
     */
    Result scale(float factor) noexcept;

    /**
     * @brief Sets the values by which the object is moved in a two-dimensional space.
     *
     * Начало системы координат находится в верхнем левом углу холста.
     * Горизонтальная и вертикальная оси указывают вправо и вниз соответственно.
     *
     * @param [in] x Значение горизонтального сдвига.
     * @param [in] y Значение вертикального смещения.
     *
     * @retval Result::InsufficientCondition in case a custom transform is applied.
     * @see Paint::transform()
     */
    Result translate(float x, float y) noexcept;

    /**
     * @brief Sets the matrix of the affine transformation for the object.
     *
     * Ожидается получение расширенной матрицы преобразования.
     *
     * @param [дюймы] м Расширенная матрица 3х3.
     */
    Result transform(const Matrix& m) noexcept;

    /**
     * @brief Gets the matrix of the affine transformation of the object.
     *
     * Значения матрицы могут быть установлены с помощью transform() API , а также с помощью translate() ,
     * scale() и rotate() . Если преобразование не применялось, возвращается единичная матрица.
     *
     * @return The augmented transformation matrix.
     *
     * @since 0.4
     */
    Matrix transform() noexcept;

    /**
     * @brief Sets the opacity of the object.
     *
     * @param [in] o Значение непрозрачности в диапазоне [0 ~ 255], где 0 — полная прозрачность, а 255 — непрозрачность.
     *
     * @note Setting the opacity with this API may require multiple render pass for composition. It is recommended to avoid changing the opacity if possible.
     */
    Result opacity(uint8_t o) noexcept;

    /**
     * @brief Sets the composition target object and the composition method.
     *
     * @param [in] target Краска целевого объекта.
     * @param [in] метод Метод, используемый для объединения исходного объекта с целевым.
     */
    Result composite(std::unique_ptr<Paint> target, CompositeMethod method) noexcept;

    /**
     * @brief Clip the drawing region of the paint object.
     *
     * Эта функция ограничивает область рисования объекта рисования путями указанной фигуры.
     *
     * @param [in] Clipper Объект формы в качестве обрезки.
     *
     * @retval Result::NonSupport If the @p clipper type is not Shape.
     *
     * @note @p clipper only supports the Shape type.
     * @note Experimental API
     */
    Result clip(std::unique_ptr<Paint> clipper) noexcept;

    /**
     * @brief Sets the blending method for the paint object.
     *
     * Функция смешивания позволяет комбинировать цвета для создания визуально привлекательных эффектов, включая, среди прочего, прозрачность, освещение, затенение и смешивание цветов.
     * его процесс включает в себя сочетание цветов или изображений исходного объекта рисования с целевым (изображением нижнего слоя) с использованием операций смешивания.
     * Операция смешивания определяется выбранным @p BlendMethod, который определяет, как комбинируются цвета или изображения.
     *
     * @param [in] метод Устанавливаемый метод смешивания.
     *
     * @note Experimental API
     */
    Result blend(BlendMethod method) noexcept;

    /**
     * @deprecated Use bounds(float* x, float* y, float* w, float* h, bool transformed) instead
     */
    TVG_DEPRECATED Result bounds(float* x, float* y, float* w, float* h) const noexcept;

    /**
     * @brief Gets the axis-aligned bounding box of the paint object.
     *
     * @param [out] x Координата X верхнего левого угла объекта.
     * @param [out] y Координата Y верхнего левого угла объекта.
     * @param [out] w Ширина объекта.
     * @param [out] h Высота объекта.
     * @param [in] Transformed Если @c true, преобразования краски учитываются в сцене, к которой она принадлежит. В противном случае это не так.
     *
     * @note This is useful when you need to figure out the bounding box of the paint in the canvas space.
     * @note The bounding box doesn't indicate the actual drawing region. It's the smallest rectangle that encloses the object.
     * @note If @p transformed is @c true, the paint needs to be pushed into a canvas and updated before this api is called.
     * @see Canvas::update()
     */
    Result bounds(float* x, float* y, float* w, float* h, bool transformed) const noexcept;

    /**
     * @brief Duplicates the object.
     *
     * Создает новый объект и устанавливает все его свойства как в исходном объекте.
     *
     * @return The created object when succeed, @c nullptr otherwise.
     */
    Paint* duplicate() const noexcept;

    /**
     * @brief Gets the opacity value of the object.
     *
     * @return The opacity value in the range [0 ~ 255], where 0 is completely transparent and 255 is opaque.
     */
    uint8_t opacity() const noexcept;

    /**
     * @brief Gets the composition target object and the composition method.
     *
     * @param [out] target Краска целевого объекта.
     *
     * @return The method used to composite the source object with the target.
     *
     * @since 0,5
     */
    CompositeMethod composite(const Paint** target) const noexcept;

    /**
     * @brief Returns the ID value of this class.
     *
     * Этот метод можно использовать для проверки текущего конкретного типа экземпляра.
     *
     * @return The class type ID of the Paint instance.
     *
     * @since Экспериментальный API
     */
    virtual Type type() const noexcept = 0;

    /**
     * @brief Unique ID of this instance.
     *
     * Это зарезервировано для указания экземпляра рисования в сцене.
     *
     * @since Экспериментальный API
     */
    uint32_t id = 0;

    /**
     * @see Paint::type()
     */
    TVG_DEPRECATED uint32_t identifier() const noexcept;

    _TVG_DECLARE_PRIVATE(Paint);
};


/**
 * @class Заполнить
 *
 * @brief An abstract class representing the gradient fill of the Shape object.
 *
 * Содержит информацию о цветах градиента и их расположении.
 * внутри границ градиента. Границы градиентов определяются в LinearGradient.
 * или класс RadialGradient, в зависимости от типа используемого градиента.
 * Он определяет поведение градиента в случае, если область, определяемая границами градиента,
 * меньше площади, подлежащей заполнению.
 */
class TVG_API Fill
{
public:
    /**
     * @brief A data structure storing the information about the color and its relative position inside the gradient bounds.
     */
    struct ColorStop
    {
        float offset; /**< The relative position of the color. */
        uint8_t r;    /**< The red color channel value in the range [0 ~ 255]. */
        uint8_t g;    /**< The green color channel value in the range [0 ~ 255]. */
        uint8_t b;    /**< The blue color channel value in the range [0 ~ 255]. */
        uint8_t a;    /**< The alpha channel value in the range [0 ~ 255], where 0 is completely transparent and 255 is opaque. */
    };

    virtual ~Fill();

    /**
     * @brief Sets the parameters of the colors of the gradient and their position.
     *
     * @param [in] colorStops Массив структуры данных ColorStop.
     * @param [in] cnt Счетчик массива colorStops @p, равный количеству цветов, используемых в градиенте.
     */
    Result colorStops(const ColorStop* colorStops, uint32_t cnt) noexcept;

    /**
     * @brief Sets the FillSpread value, which specifies how to fill the area outside the gradient bounds.
     *
     * @param [in] s Значение FillSpread.
     */
    Result spread(FillSpread s) noexcept;

    /**
     * @brief Sets the matrix of the affine transformation for the gradient fill.
     *
     * Ожидается получение расширенной матрицы преобразования.
     *
     * @param [дюймы] м Расширенная матрица 3х3.
     */
    Result transform(const Matrix& m) noexcept;

    /**
     * @brief Gets the parameters of the colors of the gradient, their position and number.
     *
     * @param [out] colorStops Указатель на ячейку памяти, где хранится массив ColorStop градиента.
     *
     * @return The number of colors used in the gradient. This value corresponds to the length of the @p colorStops array.
     */
    uint32_t colorStops(const ColorStop** colorStops) const noexcept;

    /**
     * @brief Gets the FillSpread value of the fill.
     *
     * @return The FillSpread value of this Fill.
     */
    FillSpread spread() const noexcept;

    /**
     * @brief Gets the matrix of the affine transformation of the gradient fill.
     *
     * Если преобразование не применялось, возвращается единичная матрица.
     *
     * @return The augmented transformation matrix.
     */
    Matrix transform() const noexcept;

    /**
     * @brief Creates a copy of the Fill object.
     *
     * Верните вновь созданный объект Fill со свойствами, скопированными из оригинала.
     *
     * @return A copied Fill object when succeed, @c nullptr otherwise.
     */
    Fill* duplicate() const noexcept;

    /**
     * @brief Returns the ID value of this class.
     *
     * Этот метод можно использовать для проверки текущего конкретного типа экземпляра.
     *
     * @return The class type ID of the Fill instance.
     *
     * @since Экспериментальный API
     */
    virtual Type type() const noexcept = 0;

    /**
     * @see Fill::type()
     */
    TVG_DEPRECATED uint32_t identifier() const noexcept;

    _TVG_DECLARE_PRIVATE(Fill);
};


/**
 * @class Холст
 *
 * @brief An abstract class for drawing graphical elements.
 *
 * Холст — это объект, отвечающий за рисование цели. Он настраивает механизм рисования и буфер, который можно рисовать на экране. Он также управляет заданными объектами Paint.
 *
 * @note A Canvas behavior depends on the raster engine though the final content of the buffer is expected to be identical.
 * @warning The Paint objects belonging to one Canvas can't be shared among multiple Canvases.
 */
class TVG_API Canvas
{
public:
    Canvas(RenderMethod*);
    virtual ~Canvas();

    TVG_DEPRECATED Result reserve(uint32_t n) noexcept;

    /**
     * @brief Returns the list of the paints that currently held by the Canvas.
     *
     * Эта функция предоставляет список узлов рисования, предоставляя пользователям прямую возможность изменять дерево сцены.
     *
     * @warning Please avoid accessing the paints during Canvas update/draw. You can access them after calling sync().
     * @see Canvas::sync()
     *
     * @note Experimental API
     */
    std::list<Paint*>& paints() noexcept;

    /**
     * @brief Passes drawing elements to the Canvas using Paint objects.
     *
     * Только нарисованные на холсте краски будут целями рисования.
     * Они сохраняются в холсте до тех пор, пока вы не вызовете Canvas::clear().
     *
     * @param [in] Paint Объект Paint, который нужно нарисовать.
     *
     * @retval Result::MemoryCorruption In case a @c nullptr is passed as the argument.
     *
     * @note The rendering order of the paints is the same as the order as they were pushed into the canvas. Consider sorting the paints before pushing them if you intend to use layering.
     * @see Canvas::paints()
     * @see Canvas::clear()
     */
    virtual Result push(std::unique_ptr<Paint> paint) noexcept;

    /**
     * @brief Clear the internal canvas resources that used for the drawing.
     *
     * Этот API устанавливает общее количество красок, помещенных на холст, в ноль.
     * В зависимости от значения аргумента free @p краски либо освобождаются, либо сохраняются.
     * Поэтому, если вам нужно обновить свойства рисования, сохраняя при этом существующую структуру сцены, вы можете установить @p free = false.
     *
     * @param [in] free Если @c true, память, занятая красками, освобождается, в противном случае — нет.
     *
     *
     * @see Canvas::push()
     * @see Canvas::paints()
     */
    virtual Result clear(bool free = true) noexcept;

    /**
     * @brief Request the canvas to update the paint objects.
     *
     * Если передается нулевой параметр @c, все объекты рисования, сохраняемые Canvas, обновляются.
     * в противном случае только краска, на которую указывает данная краска @p.
     *
     * @param [in] Paint Указатель на объект Paint или @c nullptr.
     *
     * @note The Update behavior can be asynchronous if the assigned thread number is greater than zero.
     */
    virtual Result update(Paint* paint = nullptr) noexcept;

    /**
     * @brief Requests the canvas to draw the Paint objects.
     *
     * @note Drawing can be asynchronous if the assigned thread number is greater than zero. To guarantee the drawing is done, call sync() afterwards.
     * @see Canvas::sync()
     */
    virtual Result draw() noexcept;

    /**
     * @brief Sets the drawing region in the canvas.
     *
     * Эта функция определяет прямоугольную область холста, которая будет использоваться для операций рисования.
     * Указанный область просмотра используется для обрезки вывода рендеринга по границам прямоугольника.
     *
     * @param [in] x Координата X верхнего левого угла прямоугольника.
     * @param [in] y Координата Y верхнего левого угла прямоугольника.
     * @param [in] w Ширина прямоугольника.
     * @param [in] h Высота прямоугольника.
     *
     * @see SwCanvas::target()
     * @see GlCanvas::target()
     * @see WgCanvas::target()
     *
     * @warning It's not allowed to change the viewport during Canvas::push() - Canvas::sync() or Canvas::update() - Canvas::sync().
     *
     * @note When resetting the target, the viewport will also be reset to the target size.
     * @since 0,15
     */
    virtual Result viewport(int32_t x, int32_t y, int32_t w, int32_t h) noexcept;

    /**
     * @brief Guarantees that drawing task is finished.
     *
     * Рендеринг Canvas может выполняться асинхронно. Чтобы убедиться, что рендеринг завершен,
     * sync() должен вызываться после draw() независимо от потока.
     *
     * @retval Result::InsufficientCondition: The canvas is either already in sync condition or in a damaged condition (a draw is required before syncing).
     *
     * @see Canvas::draw()
     */
    virtual Result sync() noexcept;

    _TVG_DECLARE_PRIVATE(Canvas);
};


/**
 * @class Линейный градиент
 *
 * @brief A class representing the linear gradient fill of the Shape object.
 *
 * Помимо API, унаследованных от класса Fill, он позволяет устанавливать и получать границы линейного градиента.
 * Поведение за пределами границ градиента зависит от значения, указанного в разбросе API.
 */
class TVG_API LinearGradient final : public Fill
{
public:
    ~LinearGradient();

    /**
     * @brief Sets the linear gradient bounds.
     *
     * Границы линейного градиента определяются как поверхность, ограниченная двумя параллельными линиями, пересекающими
     * заданные точки ( @p x1, @p y1) и ( @p x2, @p y2) соответственно. Обе линии перпендикулярны линии, соединяющей
     * ( @p x1, @p y1) и ( @p x2, @p y2).
     *
     * @param [in] x1 Горизонтальная координата первой точки, используемая для определения границ градиента.
     * @param [in] y1 Вертикальная координата первой точки, используемой для определения границ градиента.
     * @param [in] x2 Горизонтальная координата второй точки, используемая для определения границ градиента.
     * @param [in] y2 Вертикальная координата второй точки, используемая для определения границ градиента.
     *
     * @note In case the first and the second points are equal, an object is filled with a single color using the last color specified in the colorStops().
     * @see Fill::colorStops()
     */
    Result linear(float x1, float y1, float x2, float y2) noexcept;

    /**
     * @brief Gets the linear gradient bounds.
     *
     * Границы линейного градиента определяются как поверхность, ограниченная двумя параллельными линиями, пересекающими
     * заданные точки ( @p x1, @p y1) и ( @p x2, @p y2) соответственно. Обе линии перпендикулярны линии, соединяющей
     * ( @p x1, @p y1) и ( @p x2, @p y2).
     *
     * @param [out] x1 Горизонтальная координата первой точки, используемой для определения границ градиента.
     * @param [out] y1 Вертикальная координата первой точки, используемой для определения границ градиента.
     * @param [out] x2 Горизонтальная координата второй точки, используемая для определения границ градиента.
     * @param [out] y2 Вертикальная координата второй точки, используемая для определения границ градиента.
     */
    Result linear(float* x1, float* y1, float* x2, float* y2) const noexcept;

    /**
     * @brief Creates a new LinearGradient object.
     *
     * @return A new LinearGradient object.
     */
    static std::unique_ptr<LinearGradient> gen() noexcept;

    /**
     * @brief Returns the ID value of this class.
     *
     * Этот метод можно использовать для проверки текущего конкретного типа экземпляра.
     *
     * @return The class type ID of the LinearGradient instance.
     *
     * @since Экспериментальный API
     */
    Type type() const noexcept override;

    /**
     * @see LinearGradient::type()
     */
    TVG_DEPRECATED static uint32_t identifier() noexcept;

    _TVG_DECLARE_PRIVATE(LinearGradient);
};


/**
 * @class Радиальный градиент
 *
 * @brief A class representing the radial gradient fill of the Shape object.
 *
 */
class TVG_API RadialGradient final : public Fill
{
public:
    ~RadialGradient();

    /**
     * @brief Sets the radial gradient bounds.
     *
     * Границы радиального градиента определяются как окружность с центром в заданной точке ( @p cx, @p cy) заданного радиуса.
     *
     * @param [in] cx Горизонтальная координата центра ограничивающего круга.
     * @param [in] cy Вертикальная координата центра ограничивающего круга.
     * @param [дюйм] радиус Радиус ограничивающего круга.
     *
     * @retval Result::InvalidArguments in case the @p radius value is zero or less.
     *
     * @note In case the @p radius is zero, an object is filled with a single color using the last color specified in the colorStops().
     */
    Result radial(float cx, float cy, float radius) noexcept;

    /**
     * @brief Gets the radial gradient bounds.
     *
     * Границы радиального градиента определяются как окружность с центром в заданной точке ( @p cx, @p cy) заданного радиуса.
     *
     * @param [out] cx Горизонтальная координата центра ограничивающего круга.
     * @param [out] cy Вертикальная координата центра ограничивающего круга.
     * @param [out] radius Радиус ограничивающего круга.
     *
     */
    Result radial(float* cx, float* cy, float* radius) const noexcept;

    /**
     * @brief Creates a new RadialGradient object.
     *
     * @return A new RadialGradient object.
     */
    static std::unique_ptr<RadialGradient> gen() noexcept;

    /**
     * @brief Returns the ID value of this class.
     *
     * Этот метод можно использовать для проверки текущего конкретного типа экземпляра.
     *
     * @return The class type ID of the LinearGradient instance.
     *
     * @since Экспериментальный API
     */
    Type type() const noexcept override;

    /**
     * @see RadialGradient::type()
     */
    TVG_DEPRECATED static uint32_t identifier() noexcept;

    _TVG_DECLARE_PRIVATE(RadialGradient);
};


/**
 * Форма @class
 *
 * @brief A class representing two-dimensional figures and their properties.
 *
 * Форма имеет три основных свойства: контур фигуры, обводка, заполнение. Контур фигуры сохраняется как путь.
 * Путь может быть составлен путем накопления примитивных команд, таких как moveTo(), lineTo(), cubicTo(), или полных интерфейсов формы, таких как appendRect(), appendCircle() и т. д.
 * Путь может состоять из подпутей. Один подпуть определяется командой закрытия.
 *
 * Обводка Shape — это необязательное свойство, если фигуру необходимо представить с контурными границами или без них.
 * Это эффективно, поскольку путь формы и путь обводки можно использовать совместно. Это также удобно при управлении обоими в одном контексте.
 */
class TVG_API Shape final : public Paint
{
public:
    ~Shape();

    /**
     * @brief Resets the shape path.
     *
     * Свойства матрицы преобразования, цвета, заливки и обводки сохраняются.
     *
     * @note The memory where the path data is stored is not deallocated at this stage to allow for caching.
     */
    Result reset() noexcept;

    /**
     * @brief Sets the initial point of the sub-path.
     *
     * Значение текущей точки устанавливается в данную точку.
     *
     * @param [in] x Горизонтальная координата начальной точки подпути.
     * @param [in] y Вертикальная координата начальной точки подпути.
     */
    Result moveTo(float x, float y) noexcept;

    /**
     * @brief Adds a new point to the sub-path, which results in drawing a line from the current point to the given end-point.
     *
     * Значение текущей точки устанавливается в заданную конечную точку.
     *
     * @param [in] x Горизонтальная координата конечной точки линии.
     * @param [in] y Вертикальная координата конечной точки линии.
     *
     * @note In case this is the first command in the path, it corresponds to the moveTo() call.
     */
    Result lineTo(float x, float y) noexcept;

    /**
     * @brief Adds new points to the sub-path, which results in drawing a cubic Bezier curve starting
     * в текущей точке и заканчивая в заданной конечной точке ( @p x, @p y) с использованием контрольных точек ( @p cx1, @p cy1) и ( @p cx2, @p cy2).
     *
     * Значение текущей точки устанавливается в заданную конечную точку.
     *
     * @param [in] cx1 Горизонтальная координата первой контрольной точки.
     * @param [in] cy1 Вертикальная координата первой контрольной точки.
     * @param [in] cx2 Горизонтальная координата второй контрольной точки.
     * @param [in] cy2 Вертикальная координата второй контрольной точки.
     * @param [in] x Горизонтальная координата конечной точки кривой.
     * @param [in] y Вертикальная координата конечной точки кривой.
     *
     * @note In case this is the first command in the path, no data from the path are rendered.
     */
    Result cubicTo(float cx1, float cy1, float cx2, float cy2, float x, float y) noexcept;

    /**
     * @brief Closes the current sub-path by drawing a line from the current point to the initial point of the sub-path.
     *
     * Значение текущей точки устанавливается в начальную точку замкнутого подпути.
     *
     * @note In case the sub-path does not contain any points, this function has no effect.
     */
    Result close() noexcept;

    /**
     * @brief Appends a rectangle to the path.
     *
     * Прямоугольник с закругленными углами можно получить, установив ненулевые значения для аргументов @p rx и @p ry.
     * Значения @p rx и @p ry определяют радиусы эллипса, определяющие скругление углов.
     *
     * Положение прямоугольника задается координатами его верхнего левого угла — аргументами @p x и @p y.
     *
     * Прямоугольник рассматривается как новый подпуть — он не связан с предыдущим подпутем.
     *
     * Значение текущей точки устанавливается на ( @p x + @p rx, @p y) - в случае, если @p rx больше
     * чем @p w/2, текущая точка устанавливается на ( @p x + @p w/2, @p y)
     *
     * @param [in] x Горизонтальная координата верхнего левого угла прямоугольника.
     * @param [in] y Вертикальная координата верхнего левого угла прямоугольника.
     * @param [in] w Ширина прямоугольника.
     * @param [in] h Высота прямоугольника.
     * @param [in] rx Радиус эллипса по оси X, определяющий закругленные углы прямоугольника.
     * @param [in] ry Радиус эллипса по оси Y, определяющий закругленные углы прямоугольника.
     *
     * @note For @p rx and @p ry greater than or equal to the half of @p w and the half of @p h, respectively, the shape become an ellipse.
     */
    Result appendRect(float x, float y, float w, float h, float rx = 0, float ry = 0) noexcept;

    /**
     * @brief Appends an ellipse to the path.
     *
     * Положение эллипса задается координатами его центра — аргументами @p cx и @p cy.
     *
     * Эллипс рассматривается как новый подпуть — он не связан с предыдущим подпутем.
     *
     * Значение текущей точки установлено на ( @p cx, @p cy - @p ry).
     *
     * @param [in] cx Горизонтальная координата центра эллипса.
     * @param [in] cy Вертикальная координата центра эллипса.
     * @param [in] rx Радиус эллипса по оси X.
     * @param [in] ry Радиус эллипса по оси Y.
     *
     */
    Result appendCircle(float cx, float cy, float rx, float ry) noexcept;

    /**
     * @brief Appends a circular arc to the path.
     *
     * Дуга рассматривается как новый подпуть - она не связана с предыдущим подпутем.
     * Текущее значение точки устанавливается в конечную точку дуги, если круговая диаграмма @p имеет значение @c false, и в центр дуги в противном случае.
     *
     * @param [in] cx Горизонтальная координата центра дуги.
     * @param [in] cy Вертикальная координата центра дуги.
     * @param [дюймы] радиус Радиус дуги.
     * @param [in] startAngle Начальный угол дуги, заданный в градусах и измеренный против часовой стрелки от горизонтальной линии.
     * @param [in] развертка Центральный угол дуги, заданный в градусах и измеренный против часовой стрелки от @p startAngle.
     * @param [in] круговая диаграмма Определяет, следует ли рисовать радиусы от центра дуги до обеих ее конечных точек — рисуется, если @c истинно.
     *
     * @note Setting @p sweep value greater than 360 degrees, is equivalent to calling appendCircle(cx, cy, radius, radius).
     */
    Result appendArc(float cx, float cy, float radius, float startAngle, float sweep, bool pie) noexcept;

    /**
     * @brief Appends a given sub-path to the path.
     *
     * Текущее значение точки устанавливается на последнюю точку подпути.
     * Для каждой команды из cmds-массива @p необходимо указать соответствующее количество точек в массиве @p pts.
     * Если количество точек в массиве точек @p отличается от количества, требуемого массивом cmds @p, фигура с этим подпутем не будет отображаться на экране.
     *
     * @param [in] cmds Массив команд в подпути.
     * @param [in] cmdCnt Количество команд подпути.
     * @param [in] pts Массив двумерных точек.
     * @param [in] ptsCnt Число точек в массиве @p pts.
     *
     * @note The interface is designed for optimal path setting if the caller has a completed path commands already.
     */
    Result appendPath(const PathCommand* cmds, uint32_t cmdCnt, const Point* pts, uint32_t ptsCnt) noexcept;

    /**
     * @brief Sets the stroke width for all of the figures from the path.
     *
     * @param [дюймы] ширина Ширина обводки. Значение по умолчанию — 0.
     *
     */
    Result stroke(float width) noexcept;

    /**
     * @brief Sets the color of the stroke for all of the figures from the path.
     *
     * @param [in] r Значение канала красного цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
     * @param [in] g Значение канала зеленого цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
     * @param [in] b Значение канала синего цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
     * @param [in] a Значение альфа-канала в диапазоне [0 ~ 255], где 0 — полностью прозрачный, а 255 — непрозрачный. Значение по умолчанию — 0.
     *
     */
    Result stroke(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept;

    /**
     * @brief Sets the gradient fill of the stroke for all of the figures from the path.
     *
     * @param [in] f Градиентная заливка.
     *
     * @retval Result::MemoryCorruption In case a @c nullptr is passed as the argument.
     */
    Result stroke(std::unique_ptr<Fill> f) noexcept;

    /**
     * @brief Sets the dash pattern of the stroke.
     *
     * @param [in] DashPattern Массив последовательных парных значений длины штриха и длины пробела.
     * @param [in] cnt Длина массива DashPattern @p.
     *
     * @retval Result::InvalidArguments In case @p dashPattern is @c nullptr and @p cnt > 0, @p cnt is zero, any of the dash pattern values is zero or less.
     *
     * @note To reset the stroke dash pattern, pass @c nullptr to @p dashPattern and zero to @p cnt.
     * @warning @p cnt must be greater than 1 if the dash pattern is valid.
     */
    Result stroke(const float* dashPattern, uint32_t cnt) noexcept;

    /**
     * @brief Sets the cap style of the stroke in the open sub-paths.
     *
     * @param [in] cap Значение стиля шапки. Значение по умолчанию — @c StrokeCap::Square.
     *
     */
    Result stroke(StrokeCap cap) noexcept;

    /**
     * @brief Sets the join style for stroked path segments.
     *
     * Стиль соединения используется для соединения двух сегментов линии при обводке пути.
     *
     * @param [in] join Значение стиля соединения. Значение по умолчанию — @c StrokeJoin::Bevel.
     *
     */
    Result stroke(StrokeJoin join) noexcept;

    /**
     * @brief Sets the stroke miterlimit.
     *
     * @param [in] miterlimit Miterlimit накладывает ограничение на размер соединения штрихов, когда установлен стиль соединения @c StrokeJoin::Miter. Значение по умолчанию — 4.
     *
     * @retval Result::InvalidArgument for @p miterlimit values less than zero.
     *
     * @since 0.11
     */
    Result strokeMiterlimit(float miterlimit) noexcept;

    /**
     * @brief Sets the trim of the stroke along the defined path segment, allowing control over which part of the stroke is visible.
     *
     * Если значения аргументов @p Begin и @p End превышают диапазон 0–1, они переносятся аналогично переносу углов, эффективно рассматривая диапазон как круговой.
     *
     * @param [in] начало Указывает начало сегмента, отображаемого по пути.
     * @param [in] end Указывает конец сегмента, отображаемого вдоль пути.
     * @param [in] одновременно Определяет, как обрезать несколько контуров внутри одной фигуры. Если установлено значение @c true (по умолчанию), обрезка применяется одновременно ко всем путям;
     * В противном случае все пути рассматриваются как единый объект с общей длиной, равной сумме их отдельных длин, и обрезаются как таковые.
     *
     * @note Experimental API
     */
    Result strokeTrim(float begin, float end, bool simultaneous = true) noexcept;

    /**
     * @brief Sets the solid color for all of the figures from the path.
     *
     * Части фигуры, определенные как внутренние, окрашены.
     *
     * @param [in] r Значение канала красного цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
     * @param [in] g Значение канала зеленого цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
     * @param [in] b Значение канала синего цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
     * @param [in] a Значение альфа-канала в диапазоне [0 ~ 255], где 0 — полностью прозрачный, а 255 — непрозрачный. Значение по умолчанию — 0.
     *
     * @note Either a solid color or a gradient fill is applied, depending on what was set as last.
     */
    Result fill(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept;

    /**
     * @brief Sets the gradient fill for all of the figures from the path.
     *
     * Части фигуры, определенные как внутренние, заполняются.
     *
     * @param [in] f Уникальный указатель на градиентную заливку.
     *
     * @note Either a solid color or a gradient fill is applied, depending on what was set as last.
     */
    Result fill(std::unique_ptr<Fill> f) noexcept;

    /**
     * @brief Sets the fill rule for the Shape object.
     *
     * @param [in] r Значение правила заполнения. Значение по умолчанию — @c FillRule::Winding.
     */
    Result fill(FillRule r) noexcept;

    /**
     * @brief Sets the rendering order of the stroke and the fill.
     *
     * @param [in]strokeFirst Если @c true, обводка отображается перед заливкой, в противном случае обводка отображается как вторая (параметр по умолчанию).
     *
     * @since 0.10
     */
    Result order(bool strokeFirst) noexcept;

    /**
     * @brief Gets the commands data of the path.
     *
     * @param [out] cmds Указатель на массив команд из пути.
     *
     * @return The length of the @p cmds array when succeed, zero otherwise.
     */
    uint32_t pathCommands(const PathCommand** cmds) const noexcept;

    /**
     * @brief Gets the points values of the path.
     *
     * @param [out] pts Указатель на массив двумерных точек пути.
     *
     * @return The length of the @p pts array when succeed, zero otherwise.
     */
    uint32_t pathCoords(const Point** pts) const noexcept;

    /**
     * @brief Gets the pointer to the gradient fill of the shape.
     *
     * @return The pointer to the gradient fill of the stroke when succeed, @c nullptr in case no fill was set.
     */
    const Fill* fill() const noexcept;

    /**
     * @brief Gets the solid color of the shape.
     *
     * @param [out] r Значение канала красного цвета в диапазоне [0 ~ 255].
     * @param [out] g Значение канала зеленого цвета в диапазоне [0 ~ 255].
     * @param [out] b Значение канала синего цвета в диапазоне [0 ~ 255].
     * @param [out] a Значение альфа-канала в диапазоне [0 ~ 255], где 0 — полностью прозрачный, а 255 — непрозрачный.
     *
     */
    Result fillColor(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a = nullptr) const noexcept;

    /**
     * @brief Gets the fill rule value.
     *
     * @return The fill rule value of the shape.
     */
    FillRule fillRule() const noexcept;

    /**
     * @brief Gets the stroke width.
     *
     * @return The stroke width value when succeed, zero if no stroke was set.
     */
    float strokeWidth() const noexcept;

    /**
     * @brief Gets the color of the shape's stroke.
     *
     * @param [out] r Значение канала красного цвета в диапазоне [0 ~ 255].
     * @param [out] g Значение канала зеленого цвета в диапазоне [0 ~ 255].
     * @param [out] b Значение канала синего цвета в диапазоне [0 ~ 255].
     * @param [out] a Значение альфа-канала в диапазоне [0 ~ 255], где 0 — полностью прозрачный, а 255 — непрозрачный.
     *
     */
    Result strokeColor(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a = nullptr) const noexcept;

    /**
     * @brief Gets the pointer to the gradient fill of the stroke.
     *
     * @return The pointer to the gradient fill of the stroke when succeed, @c nullptr otherwise.
     */
    const Fill* strokeFill() const noexcept;

    /**
     * @brief Gets the dash pattern of the stroke.
     *
     * @param [out] DashPattern Указатель на память, где хранится массив образцов штрихов.
     *
     * @return The length of the @p dashPattern array.
     */
    uint32_t strokeDash(const float** dashPattern) const noexcept;

    /**
     * @brief Gets the cap style used for stroking the path.
     *
     * @return The cap style value of the stroke.
     */
    StrokeCap strokeCap() const noexcept;

    /**
     * @brief Gets the join style value used for stroking the path.
     *
     * @return The join style value of the stroke.
     */
    StrokeJoin strokeJoin() const noexcept;

    /**
     * @brief Gets the stroke miterlimit.
     *
     * @return The stroke miterlimit value when succeed, 4 if no stroke was set.
     *
     * @since 0.11
     */
    float strokeMiterlimit() const noexcept;

    /**
     * @brief Creates a new Shape object.
     *
     * @return A new Shape object.
     */
    static std::unique_ptr<Shape> gen() noexcept;

    /**
     * @brief Returns the ID value of this class.
     *
     * Этот метод можно использовать для проверки текущего конкретного типа экземпляра.
     *
     * @return The class type ID of the Shape instance.
     *
     * @since Экспериментальный API
     */
    Type type() const noexcept override;

    /**
     * @see Shape::type()
     */
    TVG_DEPRECATED static uint32_t identifier() noexcept;

    _TVG_DECLARE_PRIVATE(Shape);
};


/**
 * @class Изображение
 *
 * @brief A class representing an image read in one of the supported formats: raw, svg, png, jpg, lottie(json) and etc.
 * Помимо методов, унаследованных от Paint, он предоставляет методы для загрузки и рисования изображений на холсте.
 *
 * @note Supported formats are depended on the available TVG loaders.
 * @note See Animation class if the picture data is animatable.
 */
class TVG_API Picture final : public Paint
{
public:
    ~Picture();

    /**
     * @brief Loads a picture data directly from a file.
     *
     * ThorVG эффективно кэширует загруженные данные, используя указанный путь @p в качестве ключа.
     * Это означает, что повторная загрузка того же файла не приведет к дублированию операций;
     * вместо этого ThorVG будет повторно использовать ранее загруженные данные изображения.
     *
     * @param [in] путь Путь к файлу изображения.
     *
     * @retval Result::InvalidArguments In case the @p path is invalid.
     * @retval Result::NonSupport When trying to load a file with an unknown extension.
     *
     * @note The Load behavior can be asynchronous if the assigned thread number is greater than zero.
     * @see Initializer::init()
     */
    Result load(const std::string& path) noexcept;

    /**
     * @deprecated Use load(const char* data, uint32_t size, const std::string& mimeType, bool copy) instead.
     */
    TVG_DEPRECATED Result load(const char* data, uint32_t size, bool copy = false) noexcept;

    /**
     * @brief Loads a picture data from a memory block of a given size.
     *
     * ThorVG эффективно кэширует загруженные данные, используя указанный адрес данных @p в качестве ключа.
     * когда копия @p имеет @c false. Это означает, что повторная загрузка тех же данных не приведет к дублированию операций.
     * для общих данных @p. Вместо этого ThorVG будет повторно использовать ранее загруженные данные изображения.
     *
     * @param [in] data Указатель на ячейку памяти, где хранится содержимое файла изображения. Для недвоичных данных ожидается строка с нулевым завершением, если копия @p имеет значение @c false.
     * @param [in] size Размер в байтах памяти, занимаемой данными @p.
     * @param [in] mimeType Mimetype или расширение данных, например «jpg», «jpeg», «lottie», «svg», «svg+xml», «png» и т. д. Если указана пустая строка или неизвестный тип, загрузчики будут проверены один за другим.
     * @param [in] copy Если @c true, данные копируются в локальный буфер механизма, в противном случае — нет.
     *
     * @retval Result::InvalidArguments In case no data are provided or the @p size is zero or less.
     * @retval Result::NonSupport When trying to load a file with an unknown extension.
     *
     * @warning It's the user responsibility to release the @p data memory.
     *
     * @note If you are unsure about the MIME type, you can provide an empty value like @c "", and thorvg will attempt to figure it out.
     * @since 0,5
     */
    Result load(const char* data, uint32_t size, const std::string& mimeType, bool copy = false) noexcept;

    /**
     * @brief Resizes the picture content to the given width and height.
     *
     * Размер содержимого изображения изменяется с сохранением соотношения сторон размера по умолчанию.
     * Коэффициент масштабирования устанавливается для каждого из измерений, и к обоим из них применяется меньшее значение.
     *
     * @param [in] w Новая ширина изображения в пикселях.
     * @param [in] h Новая высота изображения в пикселях.
     *
     */
    Result size(float w, float h) noexcept;

    /**
     * @brief Gets the size of the image.
     *
     * @param [out] w Ширина изображения в пикселях.
     * @param [out] h Высота изображения в пикселях.
     *
     */
    Result size(float* w, float* h) const noexcept;

    /**
     * @brief Loads raw data in ARGB8888 format from a memory block of the given size.
     *
     * ThorVG эффективно кэширует загруженные данные, используя указанный адрес данных @p в качестве ключа.
     * когда копия @p имеет @c false. Это означает, что повторная загрузка тех же данных не приведет к дублированию операций.
     * для общих данных @p. Вместо этого ThorVG будет повторно использовать ранее загруженные данные изображения.
     *
     * @param [in] data Указатель на ячейку памяти, где хранится содержимое необработанных данных изображения.
     * @param [in] w Ширина данных @p изображения в пикселях.
     * @param [in] h Высота изображения, данные @p, в пикселях.
     * @param [in] с предварительным умножением Если @c true, данные изображения предварительно умножаются в альфа-канале.
     * @param [in] copy Если @c true, данные копируются в локальный буфер механизма, в противном случае — нет.
     *
     * @since 0.9
     */
    Result load(uint32_t* data, uint32_t w, uint32_t h, bool copy) noexcept;

    /**
     * @brief Retrieve a paint object from the Picture scene by its Unique ID.
     *
     * Эта функция ищет объект рисования в сцене изображения, соответствующий предоставленному идентификатору @p.
     *
     * @param [in] id Уникальный ID объекта рисования.
     *
     * @return A pointer to the paint object that matches the given identifier, or @c nullptr if no matching paint object is found.
     *
     * @see Accessor::id()
     *
     * @note Experimental API
     */
    const Paint* paint(uint32_t id) noexcept;

    /**
     * @brief Creates a new Picture object.
     *
     * @return A new Picture object.
     */
    static std::unique_ptr<Picture> gen() noexcept;

    /**
     * @brief Returns the ID value of this class.
     *
     * Этот метод можно использовать для проверки текущего конкретного типа экземпляра.
     *
     * @return The class type ID of the Picture instance.
     *
     * @since Экспериментальный API
     */
    Type type() const noexcept override;

    /**
     * @see Picture::type()
     */
    TVG_DEPRECATED static uint32_t identifier() noexcept;

    _TVG_DECLARE_ACCESSOR(Animation);
    _TVG_DECLARE_PRIVATE(Picture);
};


/**
 * @class Сцена
 *
 * @brief A class to composite children paints.
 *
 * В качестве традиционного метода рендеринга графики TVG также включает механизм графа сцены.
 * Эта функция поддерживает функцию массива для управления несколькими отрисовками как одной групповой отрисовкой.
 *
 * Как группу сцену можно трансформировать, сделать полупрозрачной и скомпоновать с другими целевыми красками.
 * на его детей будет влиять мир сцены.
 */
class TVG_API Scene final : public Paint
{
public:
    ~Scene();

    /**
     * @brief Passes drawing elements to the Scene using Paint objects.
     *
     * Нарисованными целями будут только краски, попавшие в сцену.
     * Краски сохраняются в сцене до тех пор, пока не будет вызвана Scene::clear().
     *
     * @param [in] Paint Объект Paint, который нужно нарисовать.
     *
     * @note The rendering order of the paints is the same as the order as they were pushed. Consider sorting the paints before pushing them if you intend to use layering.
     * @see Scene::paints()
     * @see Scene::clear()
     */
    Result push(std::unique_ptr<Paint> paint) noexcept;

    TVG_DEPRECATED Result reserve(uint32_t size) noexcept;

    /**
     * @brief Returns the list of the paints that currently held by the Scene.
     *
     * Эта функция предоставляет список узлов рисования, предоставляя пользователям прямую возможность изменять дерево сцены.
     *
     * @warning Please avoid accessing the paints during Scene update/draw. You can access them after calling Canvas::sync().
     * @see Canvas::sync()
     * @see Scene::push(std::unique_ptr<Paint> paint)
     * @see Scene::clear()
     *
     * @note Experimental API
     */
    std::list<Paint*>& paints() noexcept;

    /**
     * @brief Sets the total number of the paints pushed into the scene to be zero.
     * В зависимости от значения аргумента free @p краски освобождаются или нет.
     *
     * @param [in] free Если @c true, память, занятая красками, освобождается, в противном случае — нет.
     *
     * @warning If you don't free the paints they become dangled. They are supposed to be reused, otherwise you are responsible for their lives. Thus please use the @p free argument only when you know how it works, otherwise it's not recommended.
     *
     * @since 0.2
     */
    Result clear(bool free = true) noexcept;

    /**
     * @brief Apply a post-processing effect to the scene.
     *
     * Эта функция добавляет указанный эффект сцены, например очистку всех эффектов или применение размытия по Гауссу.
     * на сцену после ее рендеринга. Несколько эффектов могут применяться последовательно.
     *
     * @param [in] effect Применяемый эффект сцены. Параметры определяются в перечислении SceneEffect.
     *                   Например, используйте SceneEffect::GaussianBlur, чтобы применить размытие с определенными параметрами.
     * @param [in] ... Дополнительные переменные параметры, необходимые для определенных эффектов (например, сигма и направление для GaussianBlur).
     *
     * @note Experimental API
     */
    Result push(SceneEffect effect, ...) noexcept;

    /**
     * @brief Creates a new Scene object.
     *
     * @return A new Scene object.
     */
    static std::unique_ptr<Scene> gen() noexcept;

    /**
     * @brief Returns the ID value of this class.
     *
     * Этот метод можно использовать для проверки текущего конкретного типа экземпляра.
     *
     * @return The class type ID of the Scene instance.
     *
     * @since Экспериментальный API
     */
    Type type() const noexcept override;

    /**
     * @see Scene::type()
     */
    TVG_DEPRECATED static uint32_t identifier() noexcept;

    _TVG_DECLARE_PRIVATE(Scene);
};


/**
 * @class Текст
 *
 * @brief A class to represent text objects in a graphical context, allowing for rendering and manipulation of unicode text.
 *
 * @since 0,15
 */
class TVG_API Text final : public Paint
{
public:
    ~Text();

    /**
     * @brief Sets the font properties for the text.
     *
     * Эта функция позволяет вам определить характеристики шрифта, используемые для рендеринга текста.
     * Он устанавливает имя шрифта, его размер и, при необходимости, стиль.
     *
     * @param [in] name Имя шрифта. Это должно соответствовать шрифту, доступному на холсте.
     * @param [in] size Размер шрифта в пунктах. Это определяет, насколько большим будет текст.
     * @param [in] style Стиль шрифта. Его можно использовать для установки шрифта «курсив».
     *                  Если не указано, используется стиль по умолчанию. В настоящее время поддерживается только курсив.
     *
     * @retval Result::InsufficientCondition when the specified @p name cannot be found.
     *
     * @note Experimental API
     */
    Result font(const char* name, float size, const char* style = nullptr) noexcept;

    /**
     * @brief Assigns the given unicode text to be rendered.
     *
     * Эта функция устанавливает строку Юникода, которая будет отображаться системой рендеринга.
     * Текст задается в соответствии с указанным методом кодирования UTF, который по умолчанию равен UTF -8.
     *
     * @param [in] текст Многобайтовый текст, закодированный строкой utf8 для отображения.
     *
     * @note Experimental API
     */
    Result text(const char* text) noexcept;

    /**
     * @brief Sets the text color.
     *
     * @param [in] r Значение канала красного цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
     * @param [in] g Значение канала зеленого цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
     * @param [in] b Значение канала синего цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
     *
     * @see Text::font()
     *
     * @since 0,15
     */
    Result fill(uint8_t r, uint8_t g, uint8_t b) noexcept;

    /**
     * @brief Sets the gradient fill for all of the figures from the text.
     *
     * Части текста, определенные как внутренние, заполняются.
     *
     * @param [in] f Уникальный указатель на градиентную заливку.
     *
     * @note Either a solid color or a gradient fill is applied, depending on what was set as last.
     * @see Text::font()
     *
     * @since 0,15
     */
    Result fill(std::unique_ptr<Fill> f) noexcept;

    /**
     * @brief Loads a scalable font data (ttf) from a file.
     *
     * ThorVG эффективно кэширует загруженные данные, используя указанный путь @p в качестве ключа.
     * Это означает, что повторная загрузка того же файла не приведет к дублированию операций;
     * вместо этого ThorVG будет повторно использовать ранее загруженные данные шрифта.
     *
     * @param [in] путь Путь к файлу шрифта.
     *
     * @retval Result::InvalidArguments In case the @p path is invalid.
     * @retval Result::NonSupport When trying to load a file with an unknown extension.
     *
     * @see Text::unload(const std::string& path)
     *
     * @since 0,15
     */
    static Result load(const std::string& path) noexcept;

    /**
     * @brief Loads a scalable font data (ttf) from a memory block of a given size.
     *
     * ThorVG эффективно кэширует загруженные данные шрифта, используя указанное имя @p в качестве ключа.
     * Это означает, что повторная загрузка тех же шрифтов не приведет к дублированию операций.
     * Вместо этого ThorVG будет повторно использовать ранее загруженные данные шрифта.
     *
     * @param [in] name Имя, под которым шрифт будет храниться и доступен (например, в @p font() API ).
     * @param [in] data Указатель на ячейку памяти, где хранится содержимое данных шрифта.
     * @param [in] size Размер в байтах памяти, занимаемой данными @p.
     * @param [in] mimeType Mime-тип или расширение данных шрифта. В случае указания пустой строки загрузчик будет определен автоматически.
     * @param [in] copy Если @c true, данные копируются в локальный буфер механизма, в противном случае они не копируются (по умолчанию).
     *
     * @retval Result::InvalidArguments If no name is provided or if @p size is zero while @p data points to a valid memory location.
     * @retval Result::NonSupport When trying to load a file with an unsupported extension.
     * @retval Result::InsufficientCondition If attempting to unload the font data that has not been previously loaded.
     *
     * @warning It's the user responsibility to release the @p data memory.
     *
     * @note To unload the font data loaded using this API, pass the proper @p name and @c nullptr as @p data.
     * @note If you are unsure about the MIME type, you can provide an empty value like @c "", and thorvg will attempt to figure it out.
     * @see Text::font(const char* name, float size, const char* style)
     *
     * @note 0.15
     */
    static Result load(const char* name, const char* data, uint32_t size, const std::string& mimeType = "ttf", bool copy = false) noexcept;

    /**
     * @brief Unloads the specified scalable font data (TTF) that was previously loaded.
     *
     * Эта функция используется для освобождения ресурсов, связанных с файлом шрифта, загруженным в память.
     *
     * @param [in] путь Путь к файлу загруженного шрифта.
     *
     * @retval Result::InsufficientCondition Fails if the loader is not initialized.
     *
     * @note If the font data is currently in use, it will not be immediately unloaded.
     * @see Text::load(const std::string& path)
     *
     * @since 0,15
     */
    static Result unload(const std::string& path) noexcept;

    /**
     * @brief Creates a new Text object.
     *
     * @return A new Text object.
     *
     * @since 0,15
     */
    static std::unique_ptr<Text> gen() noexcept;

    /**
     * @brief Returns the ID value of this class.
     *
     * Этот метод можно использовать для проверки текущего конкретного типа экземпляра.
     *
     * @return The class type ID of the Text instance.
     *
     * @since Экспериментальный API
     */
    Type type() const noexcept override;

    _TVG_DECLARE_PRIVATE(Text);
};


/**
 * @class SwCanvas
 *
 * @brief A class for the rendering graphical elements with a software raster engine.
 */
class TVG_API SwCanvas final : public Canvas
{
public:
    ~SwCanvas();

    /**
     * @brief Enumeration specifying the methods of combining the 8-bit color channels into 32-bit color.
     */
    enum Colorspace
    {
        ABGR8888 = 0,      ///< Каналы соединяются в порядке: альфа, синий, зеленый, красный. Цвета предварительно умножаются на альфа-канал. (а << 24 | б << 16 | г << 8 | г)
        ARGB8888,          ///< Каналы соединяются в порядке: альфа, красный, зеленый, синий. Цвета предварительно умножаются на альфа-канал. (а << 24 | г << 16 | г << 8 | б)
        ABGR8888S,         ///< Каналы соединяются в порядке: альфа, синий, зеленый, красный. Цвета не умножаются по альфа-каналу.  @since 0.12
        ARGB8888S,         ///< Каналы соединяются в порядке: альфа, красный, зеленый, синий. Цвета не умножаются по альфа-каналу.  @since 0.12
    };

    /**
     * @brief Enumeration specifying the methods of Memory Pool behavior policy.
     * @since 0.4
     */
    enum MempoolPolicy
    {
        Default = 0, ///< Поведение по умолчанию, для которого предназначен ThorVG.
        Shareable,   ///< Пул памяти является общим для SwCanvas.
        Individual   ///< Выделить назначенный пул памяти, который будет использоваться только текущим экземпляром.
    };

    /**
     * @brief Sets the drawing target for the rasterization.
     *
     * Буфер желаемого размера должен быть выделен и принадлежать вызывающей стороне.
     *
     * Буфер @param [in] Указатель на блок памяти размером @p шаг x @p h, где хранятся растровые данные.
     * @param [in] шаг Шаг растрового изображения – больше или равен @p w.
     * @param [in] w Ширина растрового изображения.
     * @param [in] h Высота растрового изображения.
     * @param [in] cs Значение, определяющее способ чтения/записи 32-битных цветов.
     *
     * @retval Result::InvalidArguments In case no valid pointer is provided or the width, or the height or the stride is zero.
     * @retval Result::InsufficientCondition if the canvas is performing rendering. Please ensure the canvas is synced.
     * @retval Result::NonSupport In case the software engine is not supported.
     *
     * @warning Do not access @p buffer during Canvas::push() - Canvas::sync(). It should not be accessed while the engine is writing on it.
     *
     * @see Canvas::viewport()
     * @see Canvas::sync()
    */
    Result target(uint32_t* buffer, uint32_t stride, uint32_t w, uint32_t h, Colorspace cs) noexcept;

    /**
     * @brief Set sw engine memory pool behavior policy.
     *
     * По сути, ThorVG рисует множество фигур, выделяет/освобождает часть памяти.
     * во время обработки рендеринга. Он внутренне использует один общий пул памяти.
     * который можно повторно использовать среди холстов, чтобы избежать накладных расходов на память.
     *
     * Таким образом, ThorVG предлагает использовать политику пула памяти для удовлетворения требований пользователей.
     * если ему необходимо гарантировать потокобезопасность доступа к внутренним данным.
     *
     * @param [in] policy Метод, определяющий поведение пула памяти. Значение по умолчанию — @c MempoolPolicy::Default.
     *
     * @retval Result::InsufficientCondition If the canvas contains some paints already.
     * @retval Result::NonSupport In case the software engine is not supported.
     *
     * @note When @c policy is set as @c MempoolPolicy::Individual, the current instance of canvas uses its own individual
     *       данные памяти, которые не используются совместно с другими. Это необходимо, когда доступ к холсту осуществляется через рабочий поток.
     *
     * @warning It's not allowed after pushing any paints.
     *
     * @since 0.4
    */
    Result mempool(MempoolPolicy policy) noexcept;

    /**
     * @brief Creates a new SwCanvas object.
     * @return A new SwCanvas object.
     */
    static std::unique_ptr<SwCanvas> gen() noexcept;

    _TVG_DECLARE_PRIVATE(SwCanvas);
};


/**
 * @class GlCanvas
 *
 * @brief A class for the rendering graphic elements with a GL raster engine.
 *
 * @since 0.14
 */
class TVG_API GlCanvas final : public Canvas
{
public:
    ~GlCanvas();

    /**
     * @brief Sets the drawing target for rasterization.
     *
     * Эта функция определяет цель рисования, в которой будет происходить растеризация. Он может быть нацелен
     * конкретный объект кадрового буфера ( FBO ) или основную поверхность.
     *
     * @param [in] id Цель GL ID , обычно указывающая FBO ID . Значение @c 0 определяет основную поверхность.
     * @param [in] w Ширина (в пикселях) растрового изображения.
     * @param [in] h Высота (в пикселях) растрового изображения.
     *
     * @retval Result::InsufficientCondition if the canvas is performing rendering. Please ensure the canvas is synced.
     * @retval Result::NonSupport In case the gl engine is not supported.
     *
     * @see Canvas::viewport()
     * @see Canvas::sync()
     *
     * @note Currently, this only allows the GL_RGBA8 color space format.
     * @note Experimental API
    */
    Result target(int32_t id, uint32_t w, uint32_t h) noexcept;

    /**
     * @brief Creates a new GlCanvas object.
     *
     * @return A new GlCanvas object.
     *
     * @since 0.14
     */
    static std::unique_ptr<GlCanvas> gen() noexcept;

    _TVG_DECLARE_PRIVATE(GlCanvas);
};


/**
 * @class WgCanvas
 *
 * @brief A class for the rendering graphic elements with a WebGPU raster engine.
 *
 * @warning Please do not use it. This class is not fully supported yet.
 *
 * @since 0,15
 */
class TVG_API WgCanvas final : public Canvas
{
public:
    ~WgCanvas();

    /**
     * @brief Sets the drawing target for the rasterization.
     *
     * @param [in] экземпляр WGPUInstance, контекст для всех остальных объектов wgpu.
     * @param [in] поверхность WGPUSurface, дескриптор презентабельной поверхности.
     * @param [дюймы] w Ширина поверхности.
     * @param [дюймы] ч Высота поверхности.
     * @param [in] устройство WGPDevice, желаемый дескриптор устройства wgpu. Если это @c nullptr, ThorVG внутренне назначит соответствующее устройство.
     *
     * @retval Result::InsufficientCondition if the canvas is performing rendering. Please ensure the canvas is synced.
     * @retval Result::NonSupport In case the wg engine is not supported.
     *
     * @note Experimental API
     *
     * @see Canvas::viewport()
     * @see Canvas::sync()
     */
    Result target(void* instance, void* surface, uint32_t w, uint32_t h, void* device = nullptr) noexcept;

    /**
     * @brief Creates a new WgCanvas object.
     *
     * @return A new WgCanvas object.
     *
     * @since 0,15
     */
    static std::unique_ptr<WgCanvas> gen() noexcept;

    _TVG_DECLARE_PRIVATE(WgCanvas);
};


/**
 * @class Инициализатор
 *
 * @brief A class that enables initialization and termination of the TVG engines.
 */
class TVG_API Initializer final
{
public:
    /**
     * @brief Initializes TVG engines.
     *
     * TVG требует среды работающего двигателя.
     * TVG использует собственный планировщик задач для эффективного распараллеливания задач рендеринга.
     * Вы можете указать количество потоков, количество которых обозначается потоками @p.
     * На этапе инициализации TVG будет генерировать/создавать потоки в соответствии с количеством потоков @p.
     *
     * @param [in] engine Типы механизмов, подлежащие инициализации. Это относится к типам Canvas, в которых он будет использоваться. Для нескольких серверов разрешены побитовые операции.
     * @param [in] threads Количество дополнительных потоков. Ноль указывает, что будет использоваться только основной поток.
     *
     * @retval Result::NonSupport In case the engine type is not supported on the system.
     *
     * @note The Initializer keeps track of the number of times it was called. Threads count is fixed at the first init() call.
     * @see Initializer::term()
     */
    static Result init(CanvasEngine engine, uint32_t threads) noexcept;

    /**
     * @brief Terminates TVG engines.
     *
     * @param [in] engine Типы механизмов, которые необходимо завершить. Это относится к типам Canvas, в которых он будет использоваться. Для нескольких серверов разрешены побитовые операции.
     *
     * @retval Result::InsufficientCondition In case there is nothing to be terminated.
     * @retval Result::NonSupport In case the engine type is not supported on the system.
     *
     * @note Initializer does own reference counting for multiple calls.
     * @see Initializer::init()
     */
    static Result term(CanvasEngine engine) noexcept;

    /**
     * @brief Retrieves the version of the TVG engine.
     *
     * @param [out] major Основной номер версии.
     * @param [out] второстепенный номер версии.
     * @param [out] micro Номер микро-версии.
     *
     * @return The version of the engine in the format major.minor.micro, or a @p nullptr in case of an internal error.
     *
     * @since 0,15
     */
    static const char* version(uint32_t* major, uint32_t* minor, uint32_t* micro) noexcept;

    _TVG_DISABLE_CTOR(Initializer);
};


/**
 * @class Анимация
 *
 * @brief The Animation class enables manipulation of animatable images.
 *
 * Этот класс поддерживает отображение и управление кадрами анимации.
 *
 * @since 0.13
 */

class TVG_API Animation
{
public:
    ~Animation();

    /**
     * @brief Specifies the current frame in the animation.
     *
     * @param [in] нет Индекс кадра анимации, который будет отображаться. Индекс должен быть меньше totalFrame().
     *
     * @retval Result::InsufficientCondition if the given @p no is the same as the current frame value.
     * @retval Result::NonSupport The current Picture data does not support animations.
     *
     * @note For efficiency, ThorVG ignores updates to the new frame value if the difference from the current frame value
     *       меньше 0,001. В таких случаях он возвращает @c Result::InsufficientCondition.
     *       Значения меньше 0,001 могут быть проигнорированы и не могут быть точно сохранены анимацией.
     *
     * @see totalFrame()
     *
     */
    Result frame(float no) noexcept;

    /**
     * @brief Retrieves a picture instance associated with this animation instance.
     *
     * Эта функция обеспечивает доступ к экземпляру изображения, который можно использовать для загрузки форматов анимации, таких как Lottie(json).
     * После настройки изображения его можно переместить на назначенный холст, что позволит управлять кадрами анимации.
     * с этим экземпляром анимации.
     *
     * @return A picture instance that is tied to this animation.
     *
     * @warning The picture instance is owned by Animation. It should not be deleted manually.
     *
     */
    Picture* picture() const noexcept;

    /**
     * @brief Retrieves the current frame number of the animation.
     *
     * @return The current frame number of the animation, between 0 and totalFrame() - 1.
     *
     * @note If the Picture is not properly configured, this function will return 0.
     *
     * @see Animation::frame(float no)
     * @see Animation::totalFrame()
     *
     */
    float curFrame() const noexcept;

    /**
     * @brief Retrieves the total number of frames in the animation.
     *
     * @return The total number of frames in the animation.
     *
     * @note Frame numbering starts from 0.
     * @note If the Picture is not properly configured, this function will return 0.
     *
     */
    float totalFrame() const noexcept;

    /**
     * @brief Retrieves the duration of the animation in seconds.
     *
     * @return The duration of the animation in seconds.
     *
     * @note If the Picture is not properly configured, this function will return 0.
     *
     */
    float duration() const noexcept;

    /**
     * @brief Specifies the playback segment of the animation.
     *
     * Установленный сегмент обозначается как игровая область анимации.
     * Это полезно для воспроизведения определенного сегмента всей анимации.
     * После настройки рассчитывается количество кадров анимации и время воспроизведения.
     * путем сопоставления сегмента воспроизведения как всего диапазона.
     *
     * @param [in] начало сегмента.
     * @param [in] конец сегмента конец.
     *
     * @retval Result::InsufficientCondition In case the animation is not loaded.
     * @retval Result::NonSupport When it's not animatable.
     *
     * @note Animation allows a range from 0.0 to 1.0. @p end should not be higher than @p begin.
     * @note If a marker has been specified, its range will be disregarded.
     * @see LottieAnimation::segment(const char* marker)
     *
     * @note Experimental API
     */
    Result segment(float begin, float end) noexcept;

    /**
     * @brief Gets the current segment.
     *
     * @param [out] начало сегмента.
     * @param [out] конец сегмента конец.
     *
     * @retval Result::InsufficientCondition In case the animation is not loaded.
     * @retval Result::NonSupport When it's not animatable.
     *
     * @note Experimental API
     */
    Result segment(float* begin, float* end = nullptr) noexcept;

    /**
     * @brief Creates a new Animation object.
     *
     * @return A new Animation object.
     *
     */
    static std::unique_ptr<Animation> gen() noexcept;

    _TVG_DECLARE_PRIVATE(Animation);
};


/**
 * @class Заставка
 *
 * @brief A class for exporting a paint object into a specified file, from which to recover the paint data later.
 *
 * ThorVG предоставляет функцию экспорта и импорта данных о красках. Роль Saver заключается в экспорте данных рисования в файл.
 * Это полезно, когда вам нужно сохранить созданную сцену или изображение из объекта рисования и воссоздать его позже.
 *
 * Формат файла определяется именем расширения (например, «*.tvg»), а поддерживаемые форматы зависят от среды упаковки TVG.
 * Если формат файла не поддерживается, метод save() возвращает результат @c Result::NonSupport.
 *
 * После успешного экспорта краски в файл вы можете воссоздать ее с помощью класса Picture.
 *
 * @see Picture::load()
 *
 * @since 0,5
 */
class TVG_API Saver final
{
public:
    ~Saver();

    /**
     * @brief Sets the base background content for the saved image.
     *
     * @param [in] краска Краска, которая будет отображаться в качестве фонового изображения для сохраняемой краски.
     *
     * @note Experimental API
     */
    Result background(std::unique_ptr<Paint> paint) noexcept;

    /**
     * @brief Exports the given @p paint data to the given @p path
     *
     * Если модуль сохранения поддерживает какой-либо механизм сжатия, он оптимизирует размер данных.
     * В некоторых случаях это может повлиять на время кодирования/декодирования. Вы можете отключить сжатие
     * если вы хотите оптимизировать скорость.
     *
     * @param [in] краска Краска, которую необходимо сохранить, со всеми связанными с ней свойствами.
     * @param [in] путь Путь к файлу, в котором должны быть сохранены данные рисования.
     * @param [in] compress Если @c true, то сжимайте данные, если это возможно.
     *
     * @retval Result::InsufficientCondition If currently saving other resources.
     * @retval Result::NonSupport When trying to save a file with an unknown extension or in an unsupported format.
     * @retval Result::Unknown In case an empty paint is to be saved.
     *
     * @note Saving can be asynchronous if the assigned thread number is greater than zero. To guarantee the saving is done, call sync() afterwards.
     * @see Saver::sync()
     *
     * @since 0,5
     */
    Result save(std::unique_ptr<Paint> paint, const std::string& path, bool compress = true) noexcept;

    /**
     * @brief Export the provided animation data to the specified file path.
     *
     * Эта функция экспортирует данные анимации по указанному пути к файлу. Вы также можете указать желаемую частоту кадров в кадрах в секунду ( FPS ), указав параметр fps.
     *
     * @param [in] анимация Сохраняемая анимация, включая все связанные свойства.
     * @param [in] путь Путь к файлу, в котором будет сохранена анимация.
     * @param [in] качество Уровень качества кодирования.  @c 0 — минимальное значение, @c 100 — максимальное значение (рекомендуется).
     * @param [in] fps Желаемое количество кадров в секунду ( FPS ). Например, чтобы закодировать данные по адресу 60 FPS, передайте 60. Передайте 0, чтобы сохранить исходные данные кадра.
     *
     * @retval Result::InsufficientCondition if there are ongoing resource-saving operations.
     * @retval Result::NonSupport if an attempt is made to save the file with an unknown extension or in an unsupported format.
     * @retval Result::Unknown if attempting to save an empty paint.
     *
     * @note A higher frames per second (FPS) would result in a larger file size. It is recommended to use the default value.
     * @note Saving can be asynchronous if the assigned thread number is greater than zero. To guarantee the saving is done, call sync() afterwards.
     *
     * @see Saver::sync()
     *
     * @note Experimental API
     */
    Result save(std::unique_ptr<Animation> animation, const std::string& path, uint32_t quality = 100, uint32_t fps = 0) noexcept;

    /**
     * @brief Guarantees that the saving task is finished.
     *
     * Поведение Saver работает на основе синхронизации/асинхронности, в зависимости от настроек потоков инициализатора.
     * Таким образом, если вы хотите получить от этого выгоду, вы должны вызвать sync() после save() в нужное время задержки.
     * В противном случае вы можете немедленно позвонить sync().
     *
     * @note The asynchronous tasking is dependent on the Saver module implementation.
     * @see Saver::save()
     *
     * @since 0,5
     */
    Result sync() noexcept;

    /**
     * @brief Creates a new Saver object.
     *
     * @return A new Saver object.
     *
     * @since 0,5
     */
    static std::unique_ptr<Saver> gen() noexcept;

    _TVG_DECLARE_PRIVATE(Saver);
};


/**
 * @class Аксессуар
 *
 * @brief The Accessor is a utility class to debug the Scene structure by traversing the scene-tree.
 *
 * Аксессор помогает вам искать определенные узлы, читать информацию о свойствах, определять структуру дерева сцены и его размер.
 *
 * @warning We strongly warn you not to change the paints of a scene unless you really know the design-structure.
 *
 * @since 0.10
 */
class TVG_API Accessor final
{
public:
    ~Accessor();

    TVG_DEPRECATED std::unique_ptr<Picture> set(std::unique_ptr<Picture> picture, std::function<bool(const Paint* paint)> func) noexcept;

    /**
     * @brief Set the access function for traversing the Picture scene tree nodes.
     *
     * @param [in] image Узел изображения для обхода внутреннего дерева сцены.
     * @param [in] func Функция обратного вызова, вызывающая все узлы рисования изображения.
     * @param [in] data Данные, передаваемые в функцию @p в качестве аргумента.
     *
     * @note The bitmap based picture might not have the scene-tree.
     *
     * @note Experimental API
     */
    Result set(const Picture* picture, std::function<bool(const Paint* paint, void* data)> func, void* data) noexcept;

    /**
     * @brief Generate a unique ID (hash key) from a given name.
     *
     * Эта функция вычисляет уникальное значение идентификатора на основе предоставленной строки.
     * Вы можете использовать это, чтобы назначить уникальный ID объекту Paint.
     *
     * @param [in] name Входная строка, на основе которой создается уникальный идентификатор.
     *
     * @return The generated unique identifier value.
     *
     * @see Paint::id
     *
     * @note Experimental API
     */
    static uint32_t id(const char* name) noexcept;

    /**
     * @brief Creates a new Accessor object.
     *
     * @return A new Accessor object.
     */
    static std::unique_ptr<Accessor> gen() noexcept;

    _TVG_DECLARE_PRIVATE(Accessor);
};


/**
 * @brief The cast() function is a utility function used to cast a 'Paint' to type 'T'.
 * @since 0.11
 */
template<typename T = tvg::Paint>
std::unique_ptr<T> cast(Paint* paint)
{
    return std::unique_ptr<T>(static_cast<T*>(paint));
}


/**
 * @brief The cast() function is a utility function used to cast a 'Fill' to type 'T'.
 * @since 0.11
 */
template<typename T = tvg::Fill>
std::unique_ptr<T> cast(Fill* fill)
{
    return std::unique_ptr<T>(static_cast<T*>(fill));
}


/** @}*/

} //пространство имен

#endif //_THORVG_H_

#endif /* LV_USE_THORVG_INTERNAL */

