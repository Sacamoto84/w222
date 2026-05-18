/*!
* \файл thorvg_capi.h
*
* \brief Модуль предоставляет привязки C для библиотеки ThorVG.
* Пожалуйста, обратитесь к src/examples/Capi.cpp, чтобы найти примеры использования thorvg_capi.
*
* Модуль thorvg_capi позволяет реализовать клиент ThorVG и предоставляет
* следующие функциональные возможности:
* - drawing shapes: line, curve, polygon, circle, user-defined, ...
* - filling: solid, linear and radial gradient
* - scene graph & affine transformation (translation, rotation, scale, ...)
* - stroking: width, join, cap, dash
* - composition: blending, masking, path clipping
* - pictures: SVG, PNG, JPG, bitmap
*
*/

#include "../../lv_conf_internal.h"
#if LV_USE_THORVG_INTERNAL
#define TVG_BUILD 1

#ifndef __THORVG_CAPI_H__
#define __THORVG_CAPI_H__

#include <stdint.h>
#include <stdbool.h>

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

#ifdef __cplusplus
extern "C" {
#endif

/**
* \defgroup ThorVG_CAPI ThorVG_CAPI
* \brief API привязки ThorVG к языку C.
*
* \{
*/


/**
* \brief Структура, отвечающая за управление графическими элементами и их отрисовку.
*
* Он устанавливает целевой буфер, который можно отобразить на экране. Он хранит объекты Tvg_Paint (Форма, Сцена, Изображение).
*/
typedef struct _Tvg_Canvas Tvg_Canvas;


/**
* \brief Структура, представляющая графический элемент.
*
* \предупреждение Объекты TvgPaint не могут использоваться совместно между холстами.
*/
typedef struct _Tvg_Paint Tvg_Paint;


/**
* \brief Структура, представляющая градиентную заливку объекта Tvg_Paint.
*/
typedef struct _Tvg_Gradient Tvg_Gradient;


/**
* \brief Структура, представляющая объект, позволяющая сохранить объект Tvg_Paint в файл.
*/
typedef struct _Tvg_Saver Tvg_Saver;

/**
* \brief Структура, представляющая объект контроллера анимации.
*/
typedef struct _Tvg_Animation Tvg_Animation;


/**
* \brief Перечисление, определяющее тип механизма, используемого для графического процессора. Для нескольких серверов разрешены побитовые операции.
*
* \ingroup ThorVGCapi_Initializer
*/
typedef enum {
    TVG_ENGINE_SW = (1 << 1),   ///< Растеризатор CPU.
    TVG_ENGINE_GL = (1 << 2)    ///< Растеризатор OpenGL.
} Tvg_Engine;


/**
 * \brief Перечисление, определяющее результат API.
 *
 * Все API ThorVG потенциально могут возвращать одно из значений в списке.
 * Обратите внимание, что некоторые API могут дополнительно указывать причины, по которым возвращаются их значения.
 *
 */
typedef enum {
    TVG_RESULT_SUCCESS = 0,            ///< Значение, возвращаемое в случае корректного выполнения запроса.
    TVG_RESULT_INVALID_ARGUMENT,       ///< Значение, возвращаемое в случае проблемы с аргументами, переданными в API - например. пустые пути или нулевые указатели.
    TVG_RESULT_INSUFFICIENT_CONDITION, ///< Значение, возвращаемое в случае, если запрос не может быть обработан - например. запрос свойств объекта, которого не существует.
    TVG_RESULT_FAILED_ALLOCATION,      ///< Значение, возвращаемое в случае неудачного выделения памяти.
    TVG_RESULT_MEMORY_CORRUPTION,      ///< Значение, возвращаемое в случае неправильной обработки памяти – например. сбой при освобождении или приведении указателя
    TVG_RESULT_NOT_SUPPORTED,          ///< Значение, возвращаемое в случае выбора неподдерживаемых функций (опций) движка.
    TVG_RESULT_UNKNOWN                 ///< Значение, возвращаемое во всех остальных случаях.
} Tvg_Result;


/**
 * \brief Перечисление, указывающее метод, используемый в составе двух объектов - целевого и исходного.
 *
 * \ingroup ThorVGCapi_Paint
 */
typedef enum {
    TVG_COMPOSITE_METHOD_NONE = 0,           ///< Композиция не применяется.
    TVG_COMPOSITE_METHOD_CLIP_PATH,          ///< Определяется пересечение источника и цели, и визуализируются только результирующие пиксели из источника. Обратите внимание, что ClipPath поддерживает только тип Shape.  @deprecated Вместо этого используйте Paint:: clip().
    TVG_COMPOSITE_METHOD_ALPHA_MASK,         ///< Пиксели источника и цели альфа-смешены. В результате видна только та часть источника, которая пересекается с целью.
    TVG_COMPOSITE_METHOD_INVERSE_ALPHA_MASK, ///< Пиксели источника и дополнения к целевым пикселям альфа-смешены. В результате видна только та часть источника, которая не покрыта целью.
    TVG_COMPOSITE_METHOD_LUMA_MASK,          ///< Исходные пиксели преобразуются в оттенки серого (значение яркости), а альфа-канал смешивается с целевым. В результате видна только та часть источника, которая пересекается с целью. \с 0,9
    TVG_COMPOSITE_METHOD_INVERSE_LUMA_MASK   ///< Исходные пиксели преобразуются в оттенки серого (значение яркости), а в дополнение к целевым пикселям выполняется альфа-смешение. В результате видна только та часть источника, которая не покрыта целью. \с 0,14
} Tvg_Composite_Method;

/**
 * @brief Enumeration indicates the method used for blending paint. Please refer to the respective formulas for each method.
 *
 * \ingroup ThorVGCapi_Paint
 *
 * \с 0,15
 */
typedef enum {
    TVG_BLEND_METHOD_NORMAL = 0,        ///< Выполняем альфа-смешение (по умолчанию). S если (Sa == 255), иначе (Sa * S) + (255 - Sa) * D
    TVG_BLEND_METHOD_MULTIPLY,          ///< Принимает значения канала RGB от 0 до 255 для каждого пикселя верхнего слоя и умножает их на значения для соответствующего пикселя нижнего слоя. (С*Д)
    TVG_BLEND_METHOD_SCREEN,            ///< Значения пикселей в двух слоях инвертируются, умножаются, а затем снова инвертируются. (С+Д) - (С*Д)
    TVG_BLEND_METHOD_OVERLAY,           ///< Сочетает режимы наложения «Умножение» и «Экран». (2*С*Д), если (2*Д<Да), иначе (Са*Да) — 2*(Да — С)*(Са — Д)
    TVG_BLEND_METHOD_SRCOVER,           ///< Заменить нижний слой верхним.
    TVG_BLEND_METHOD_DARKEN,            ///< Создает пиксель, который сохраняет наименьшие компоненты пикселей верхнего и нижнего слоев. мин(С, Д)
    TVG_BLEND_METHOD_LIGHTEN,           ///< Only имеет действие, противоположное «Только затемнить». макс(С, Д)
    TVG_BLEND_METHOD_COLORDODGE,        ///< Делит нижний слой на инвертированный верхний слой. Д/(255 - С)
    TVG_BLEND_METHOD_COLORBURN,         ///< Делит инвертированный нижний слой на верхний слой, а затем инвертирует результат. 255 - (255 - Д)/С
    TVG_BLEND_METHOD_HARDLIGHT,         ///< То же, что и Overlay, но с обратным расположением цветов. (2*С*Д), если (С<Са), иначе (Са*Да)-2*(Да-С)*(Са-Д)
    TVG_BLEND_METHOD_SOFTLIGHT,         ///< То же, что и наложение, но с применением чистого черного или белого цвета не получается чистый черный или белый цвет. (1 - 2*С)*(Д^2) + (2*С*Д)
    TVG_BLEND_METHOD_DIFFERENCE,        ///< Вычитает нижний слой из верхнего слоя или наоборот, чтобы всегда получать неотрицательное значение. (S - D), если (S > D), иначе (D - S)
    TVG_BLEND_METHOD_EXCLUSION,         ///< Результат — удвоенное произведение верхнего и нижнего слоев, вычтенное из их суммы. с + д - (2 * с * д)
    TVG_BLEND_METHOD_HUE,               ///< Зарезервировано. Не поддерживается.
    TVG_BLEND_METHOD_SATURATION,        ///< Зарезервировано. Не поддерживается.
    TVG_BLEND_METHOD_COLOR,             ///< Зарезервировано. Не поддерживается.
    TVG_BLEND_METHOD_LUMINOSITY,        ///< Зарезервировано. Не поддерживается.
    TVG_BLEND_METHOD_ADD,               ///< Просто добавляет значения пикселей одного слоя к другому. (С + Д)
    TVG_BLEND_METHOD_HARDMIX            ///< Зарезервировано. Не поддерживается.
} Tvg_Blend_Method;


/**
 * \см. Tvg_Type
 * \deprecated
 */
typedef enum {
    TVG_IDENTIFIER_UNDEF = 0,   ///< Неопределенный тип.
    TVG_IDENTIFIER_SHAPE,       ///< Краска типа формы.
    TVG_IDENTIFIER_SCENE,       ///< Краска типа сцены.
    TVG_IDENTIFIER_PICTURE,     ///< Краска типа изображения.
    TVG_IDENTIFIER_LINEAR_GRAD, ///< Тип линейного градиента.
    TVG_IDENTIFIER_RADIAL_GRAD, ///< Тип радиального градиента.
    TVG_IDENTIFIER_TEXT         ///< Краска текстового типа.
} Tvg_Identifier;


/**
 * \brief Перечисление, указывающее значение типа объекта ThorVG.
 *
 * Объекты рисования ThorVG могут возвращать значения типов объектов, что позволяет вам идентифицировать конкретный тип каждого объекта.
 *
 * \ingroup ThorVGCapi_Paint
 *
 * \см. tvg_paint_get_type ()
 * \см. tvg_gradient_get_type ()
 *
 * \note Экспериментальный API
 */
typedef enum {
    TVG_TYPE_UNDEF = 0,        ///< Неопределенный тип.
    TVG_TYPE_SHAPE,            ///< Краска типа формы.
    TVG_TYPE_SCENE,            ///< Краска типа сцены.
    TVG_TYPE_PICTURE,          ///< Краска типа изображения.
    TVG_TYPE_TEXT,             ///< Краска текстового типа.
    TVG_TYPE_LINEAR_GRAD = 10, ///< Тип линейного градиента.
    TVG_TYPE_RADIAL_GRAD       ///< Тип радиального градиента.
} Tvg_Type;


/**
 * \addtogroup ThorVGCapi_Shape
 * \{
 */

/**
 * \brief Перечисление, указывающее значения команд пути, принимаемых TVG .
 *
 * Не путать с командами пути из элемента пути svg (например, M, L, Q, H и многими другими).
 * TVG интерпретирует их все и преобразует в значения PathCommand.
 */
typedef enum {
    TVG_PATH_COMMAND_CLOSE = 0, ///< Завершает текущий подпуть и соединяет его с его начальной точкой — соответствует команде Z в командах пути svg.
    TVG_PATH_COMMAND_MOVE_TO,   ///< Устанавливает новую начальную точку подпути и новую текущую точку — соответствует команде M в командах пути svg.
    TVG_PATH_COMMAND_LINE_TO,   ///< Рисует линию от текущей точки до заданной точки и устанавливает новое значение текущей точки — соответствует команде L в командах пути svg.
    TVG_PATH_COMMAND_CUBIC_TO   ///< Рисует кубическую кривую Безье от текущей точки до заданной точки, используя две заданные контрольные точки, и устанавливает новое значение текущей точки — соответствует команде C в командах пути svg.
} Tvg_Path_Command;


/**
 * \brief Перечисление, определяющее тип окончания штриха в открытых подпутях.
 */
typedef enum {
    TVG_STROKE_CAP_SQUARE = 0, ///< Обводка расширяется в обеих конечных точках подпути на прямоугольник с шириной, равной ширине обводки, и длиной, равной половине ширины обводки. Для подпутей нулевой длины квадрат отображается с размером ширины штриха.
    TVG_STROKE_CAP_ROUND,      ///< Штрих расширяется в обеих конечных точках подпути на полукруг с радиусом, равным половине ширины штриха. Для подпутей нулевой длины отображается полный круг.
    TVG_STROKE_CAP_BUTT        ///< Штрих заканчивается точно в каждой из двух конечных точек подпути. Для подпутей нулевой длины штрих не отображается.
} Tvg_Stroke_Cap;


/**
 * \brief Перечисление, указывающее, как заполнить область за пределами границ градиента.
 */
typedef enum {
    TVG_STROKE_JOIN_BEVEL = 0, ///< Внешний угол соединяемых сегментов пути скошен в точке соединения. Треугольная область угла ограничена прямой линией между внешними углами каждого штриха.
    TVG_STROKE_JOIN_ROUND,     ///< Внешний угол соединяемых сегментов пути закруглен. Круглая область центрируется в точке соединения.
    TVG_STROKE_JOIN_MITER      ///< Внешний угол соединенных сегментов пути имеет шипы. Шип создается путем расширения за пределы точки соединения внешних краев обводки до их пересечения. Если расширение выходит за пределы ограничения, стиль соединения преобразуется в стиль «Скос».
} Tvg_Stroke_Join;


/**
 * \brief Перечисление, указывающее, как заполнить область за пределами границ градиента.
 */
typedef enum {
    TVG_STROKE_FILL_PAD = 0, ///< Оставшаяся область заполняется ближайшим стоп-цветом.
    TVG_STROKE_FILL_REFLECT, ///< Узор градиента отражается за пределами области градиента до тех пор, пока ожидаемая область не будет заполнена.
    TVG_STROKE_FILL_REPEAT   ///< Шаблон градиента непрерывно повторяется за пределами области градиента, пока ожидаемая область не будет заполнена.
} Tvg_Stroke_Fill;


/**
 * \brief Перечисление, определяющее алгоритм, используемый для определения того, какие части фигуры считаются внутренней частью фигуры.
 */
typedef enum {
    TVG_FILL_RULE_WINDING = 0, ///< Рисуется линия от точки до места за пределами фигуры. Подсчитываются пересечения линии с сегментом пути фигуры. Начиная с нуля, если сегмент пути фигуры пересекает линию по часовой стрелке, единица добавляется, в противном случае единица вычитается. Если полученная сумма не равна нулю, точка находится внутри фигуры.
    TVG_FILL_RULE_EVEN_ODD     ///< Рисуется линия от точки до места за пределами фигуры и подсчитываются ее пересечения с сегментами пути фигуры. Если количество пересечений нечетное, точка находится внутри фигуры.
} Tvg_Fill_Rule;

/** \} */   // конец добавления в группу ThorVGCapi_Shape


/*!
* \addtogroup ThorVGCapi_Gradient
* \{
*/

/*!
* \brief Структура данных, хранящая информацию о цвете и его относительном положении внутри границ градиента.
*/
typedef struct
{
    float offset; /**< The relative position of the color. */
    uint8_t r;    /**< The red color channel value in the range [0 ~ 255]. */
    uint8_t g;    /**< The green color channel value in the range [0 ~ 255]. */
    uint8_t b;    /**< The blue color channel value in the range [0 ~ 255]. */
    uint8_t a;    /**< The alpha channel value in the range [0 ~ 255], where 0 is completely transparent and 255 is opaque. */
} Tvg_Color_Stop;

/** \} */   // конец добавления в группу ThorVGCapi_Gradient


/**
 * \brief Структура данных, представляющая точку в двумерном пространстве.
 */
typedef struct
{
    float x, y;
} Tvg_Point;


/**
 * \brief Структура данных, представляющая трехмерную матрицу.
 *
 * Элементы e11, e12, e21 и e22 представляют матрицу вращения, включая коэффициент масштабирования.
 * Элементы e13 и e23 определяют перемещение объекта по осям x и y соответственно.
 * Элементы e31 и e32 установлены в 0, e33 — в 1.
 */
typedef struct
{
    float e11, e12, e13;
    float e21, e22, e23;
    float e31, e32, e33;
} Tvg_Matrix;


/**
* \defgroup Инициализатор ThorVGCapi_Initializer
* \brief Модуль, позволяющий инициализировать и завершить работу двигателей TVG.
*
* \{
*/

/************************************************************************/
/* Двигатель API                                                           */
/************************************************************************/
/*!
* \brief Инициализирует двигатели TVG.
*
* TVG требует среды работающего двигателя.
* TVG использует собственный планировщик задач для эффективного распараллеливания задач рендеринга.
* Вы можете указать количество потоков, количество которых обозначается потоками @p.
* На этапе инициализации TVG будет генерировать/создавать потоки в соответствии с количеством потоков @p.
*
* \code
* tvg_engine_init (TVG_ENGINE_SW, 0);  //Инициализируем программный рендерер и используем только основной поток
* \endcode
*
* \param[in] engine_method Типы механизмов для инициализации. Это относится к типам Canvas, в которых он будет использоваться. Для нескольких серверов разрешены побитовые операции.
*   - TVG_ENGINE_SW: CPU rasterizer
*   - TVG_ENGINE_GL: OpenGL rasterizer (not supported yet)
* \param[in] threads Количество дополнительных потоков, используемых для рендеринга. Ноль указывает, что будет использоваться только основной поток.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неизвестный тип двигателя.
* \retval TVG_RESULT_NOT_SUPPORTED Неподдерживаемый тип двигателя.
*
* \note Инициализатор отслеживает количество вызовов. Количество потоков фиксируется при первом вызове init().
* \см. tvg_engine_term ()
* \см. Tvg_Engine
*/
TVG_API Tvg_Result tvg_engine_init(Tvg_Engine engine_method, unsigned threads);


/*!
* \brief Завершает работу двигателей TVG.
*
* Его следует вызывать в случае завершения работы клиента TVG с теми же типами движков, которые были переданы при вызове tvg_engine_init().
*
* \code
* tvg_engine_init (TVG_ENGINE_SW, 0);
* //определяем холст и формы, обновляем формы, общие вызовы рендеринга
* tvg_engine_term ( TVG_ENGINE_SW );
* \endcode
*
* \param engine_method Типы механизмов, которые необходимо завершить. Это относится к типам Canvas, в которых он будет использоваться. Для нескольких серверов разрешены побитовые операции.
*   - TVG_ENGINE_SW: CPU rasterizer
*   - TVG_ENGINE_GL: OpenGL rasterizer (not supported yet)
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION Ничего не нужно прекращать.
* \retval TVG_RESULT_INVALID_ARGUMENT Неизвестный тип двигателя.
* \retval TVG_RESULT_NOT_SUPPORTED Неподдерживаемый тип двигателя.
*
* \см. tvg_engine_init ()
* \см. Tvg_Engine
*/
TVG_API Tvg_Result tvg_engine_term(Tvg_Engine engine_method);


/**
* \brief Получает версию движка TVG.
*
* \param[out] major Основной номер версии.
* \param[out] второстепенный номер версии.
* \param[out] micro Номер микро-версии.
* \param[out] version Версия движка в формате major.minor.micro или nullptr @p в случае внутренней ошибки.
*
* \return Перечисление Tvg_Result.
* \ретвал TVG_RESULT_SUCCESS .
*
* \с 0,15
*/
TVG_API Tvg_Result tvg_engine_version(uint32_t* major, uint32_t* minor, uint32_t* micro, const char** version);

/** \} */   // конец защитной группы ThorVGCapi_Initializer


/**
* \defgroup ThorVGCapi_Canvas Холст
* \brief Модуль для управления и рисования графических элементов.
*
* Холст — это объект, отвечающий за рисование цели. Он настраивает механизм рисования и буфер, который можно рисовать на экране. Он также управляет заданными объектами Paint.
*
* \note Поведение Canvas зависит от растрового движка, хотя ожидается, что окончательное содержимое буфера будет идентичным.
* \предупреждение Объекты Paint, принадлежащие одному холсту, не могут использоваться несколькими холстами.
\{
*/


/**
* \defgroup ThorVGCapi_SwCanvas SwCanvas
* \ingroup ThorVGCapi_Canvas
*
* \brief Модуль для рендеринга графических элементов с помощью программного движка.
*
* \{
*/

/************************************************************************/
/* SwCanvas API                                                         */
/************************************************************************/

/**
 * \brief Перечисление, определяющее методы политики поведения пула памяти.
 */
typedef enum {
    TVG_MEMPOOL_POLICY_DEFAULT = 0, ///< Поведение по умолчанию, для которого предназначен ThorVG.
    TVG_MEMPOOL_POLICY_SHAREABLE,   ///< Пул памяти распределяется между холстами.
    TVG_MEMPOOL_POLICY_INDIVIDUAL   ///< Выделяем назначенный пул памяти, который используется только текущим экземпляром холста.
} Tvg_Mempool_Policy;


/**
 * \brief Перечисление, определяющее методы объединения 8-битных цветовых каналов в 32-битный цвет.
 */
typedef enum {
    TVG_COLORSPACE_ABGR8888 = 0, ///< Каналы соединяются в порядке: альфа, синий, зеленый, красный. Цвета предварительно умножаются на альфа-канал. (а << 24 | б << 16 | г << 8 | г)
    TVG_COLORSPACE_ARGB8888,     ///< Каналы соединяются в порядке: альфа, красный, зеленый, синий. Цвета предварительно умножаются на альфа-канал. (а << 24 | г << 16 | г << 8 | б)
    TVG_COLORSPACE_ABGR8888S,    ///< Каналы соединяются в порядке: альфа, синий, зеленый, красный. Цвета не умножаются по альфа-каналу.  @since 0.13
    TVG_COLORSPACE_ARGB8888S     ///< Каналы соединяются в порядке: альфа, красный, зеленый, синий. Цвета не умножаются по альфа-каналу.  @since 0.13
} Tvg_Colorspace;


/*!
* \brief Создает объект Canvas.
*
* \code
* Tvg_Canvas *canvas = NULL ;
*
* tvg_engine_init (TVG_ENGINE_SW, 4);
* canvas = tvg_swcanvas_create();
*
* //настраиваем буфер холста
* uint32_t *буфер = NULL ;
* buffer = (uint32_t*) malloc(sizeof(uint32_t) * 100 * 100);
* if (!buffer) return;
*
* tvg_swcanvas_set_target (холст, буфер, 100, 100, 100, TVG_COLORSPACE_ARGB8888 );
*
* //настраиваем краски и добавляем их на холст перед его рисованием
*
* tvg_canvas_destroy (холст);
* tvg_engine_term ( TVG_ENGINE_SW );
* \endcode
*
* \return Новый объект Tvg_Canvas.
*/
TVG_API Tvg_Canvas* tvg_swcanvas_create(void);


/*!
* \brief Устанавливает буфер, используемый в процессе растеризации, и определяет используемое цветовое пространство.
*
* По соображениям оптимизации TVG не выделяет память для выходного буфера самостоятельно.
* Буфер желаемого размера должен быть выделен и принадлежать вызывающей стороне.
*
* \param[in] Canvas Объект Tvg_Canvas, управляющий буфером @p.
* \param[in] buffer Указатель на выделенный блок памяти размером @p шаг x @p h.
* \param[in] шаг Шаг растрового изображения – в большинстве случаев то же значение, что и @p w.
* \param[in] w Ширина растрового изображения.
* \param[in] h Высота растрового изображения.
* \param[in] cs Значение цветового пространства, определяющее способ чтения/записи 32-битных цветов.
* - TVG_COLORSPACE_ABGR8888
* - TVG_COLORSPACE_ARGB8888
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENTS Передан недопустимый указатель холста или буфера, или один из шагов @p, @p w или @p h равен нулю.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION, если холст выполняет рендеринг. Убедитесь, что холст синхронизирован.
* \retval TVG_RESULT_NOT_SUPPORTED Программный движок не поддерживается.
*
* \предупреждение Не обращайтесь к буферу @p во время выполнения tvg_canvas_draw () - tvg_canvas_sync (). К нему не следует обращаться, пока движок пишет на нем.
*
* \см. Tvg_Colorspace
*/
TVG_API Tvg_Result tvg_swcanvas_set_target(Tvg_Canvas* canvas, uint32_t* buffer, uint32_t stride, uint32_t w, uint32_t h, Tvg_Colorspace cs);


/*!
* \brief Устанавливает политику поведения пула памяти программного ядра.
*
* ThorVG рисует множество фигур, выделяет/освобождает часть памяти.
* во время обработки рендеринга. Он внутренне использует один общий пул памяти.
* который можно повторно использовать среди холстов, чтобы избежать накладных расходов на память.
*
* Таким образом, ThorVG предлагает использовать политику пула памяти для удовлетворения требований пользователей.
* если ему необходимо гарантировать потокобезопасность доступа к внутренним данным.
*
* \param[in] Canvas Объект Tvg_Canvas, для которого необходимо указать поведение пула памяти.
* \param[in] policy Метод, определяющий поведение пула памяти. Значение по умолчанию — @c TVG_MEMPOOL_POLICY_DEFAULT.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENTS Передан неверный указатель холста.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION На холсте уже есть краски.
* \retval TVG_RESULT_NOT_SUPPORTED Программный движок не поддерживается.
*
* \note Когда для политики @c установлено значение @c TVG_MEMPOOL_POLICY_INDIVIDUAL, текущий экземпляр холста использует свой собственный
*       данные памяти, которые не используются совместно с другими. Это необходимо, когда доступ к холсту осуществляется через рабочий поток.
*
* \предупреждение Не допускается после нажатия каких-либо красок.
*/
TVG_API Tvg_Result tvg_swcanvas_set_mempool(Tvg_Canvas* canvas, Tvg_Mempool_Policy policy);

/** \} */   // конец защитной группы ThorVGCapi_SwCanvas


/************************************************************************/
/* Обычный холст API                                                    */
/************************************************************************/
/*!
* \brief Очищает внутренние данные холста, освобождает все краски, хранящиеся на холсте, и уничтожает сам объект холста.
*
* \code
* статический Tvg_Canvas *canvas = NULL ;
* статический uint32_t *buffer = NULL ;
*
* статическая пустота _init() {
*   canvas = tvg_swcanvas_create();
*   buffer = (uint32_t*) malloc(sizeof(uint32_t) * 100 * 100);
*   tvg_swcanvas_set_target (холст, буфер, 100, 100, 100, TVG_COLORSPACE_ARGB8888 );
* }
*
* //задача, вызываемая из основной функции в цикле
* static void _job(const int cmd) {
*   //определяем допустимую форму прямоугольника
*   переключатель (команда) {
*     случай CMD_EXIT: вернуть 0;
*     случай CMD_ADD_RECT :
*       tvg_canvas_push (холст, прямоугольник);
*       перерыв;
*     случай CMD_DEL_RECT :
*       tvg_paint_del (прямой);
*       //теперь для безопасного удаления Tvg_Canvas необходимо использовать tvg_canvas_clear () API
*       перерыв;
*     по умолчанию:
*       перерыв;
*   }
* }
*
* int main(int argc, char **argv) {
*   интервал cmd = 0;
*   интервал остановки = 1;
*
*   tvg_engine_init (TVG_ENGINE_SW, 4);
*
*   пока (стоп) {
*      //ждём команду, например. с консоли
*      stop = _job(cmd);
*   }
*   tvg_canvas_clear (холст, ложь);
*   tvg_canvas_destroy (холст);
*   tvg_engine_term ( TVG_ENGINE_SW );
*   вернуть 0;
* }
*
* tvg_canvas_destroy (холст);
* tvg_engine_term ( TVG_ENGINE_SW )
* \endcode
*
* \param[in] Canvas Объект Tvg_Canvas, который необходимо уничтожить.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Передан недопустимый указатель на объект Tvg_Canvas.
*
* \note Если краски с холста не должны выпускаться, следует вызвать функцию tvg_canvas_clear() со значением свободного аргумента @c, установленным в @c false.
* Имейте в виду, что в таком случае TVG больше не несет ответственности за выпуск красок, и это необходимо делать вручную во избежание утечек памяти.
*
* \см. tvg_paint_del (), tvg_canvas_clear ()
*/
TVG_API Tvg_Result tvg_canvas_destroy(Tvg_Canvas* canvas);


/*!
* \brief Вставляет элемент рисунка в холст, используя объект Tvg_Paint.
*
* \param[in] холст Объект Tvg_Canvas, управляющий отрисовкой @p.
* \param[in] Paint Объект Tvg_Paint, который нужно нарисовать.
*
* Только краски, нанесенные на холст, будут целями рисования.
* Они сохраняются на холсте до тех пор, пока вы не вызовете tvg_canvas_clear().
*
* \return Tvg_Result возвращаемые значения:
* \retval TVG_RESULT_INVALID_ARGUMENT В случае, если в качестве аргумента передается нулевой параметр @c.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION Внутренняя ошибка.
*
* \note Порядок отрисовки красок такой же, как и порядок их перемещения. Если вы собираетесь использовать слои, рассмотрите возможность сортировки красок перед тем, как их раскладывать.
* \см. tvg_canvas_clear ()
*/
TVG_API Tvg_Result tvg_canvas_push(Tvg_Canvas* canvas, Tvg_Paint* paint);


/*!
* \brief Резервирует блок памяти, в котором хранятся объекты, помещенные в холст.
*
* Если количество Tvg_Paints, которое будет сохранено в холсте, известно заранее, вызов этой функции уменьшает кратное число.
* Таким образом, распределение памяти повышает производительность.
*
* \code
* Tvg_Canvas *canvas = NULL ;
*
* tvg_engine_init (TVG_ENGINE_SW, 4);
* canvas = tvg_swcanvas_create();
*
* uint32_t *буфер = NULL ;
* buffer = (uint32_t*) malloc(sizeof(uint32_t) * 100 * 100);
* if (!buffer) return;
*
* tvg_swcanvas_set_target (холст, буфер, 100, 100, 100, TVG_COLORSPACE_ARGB8888 );
*
* tvg_canvas_destroy (холст);
* tvg_engine_term ( TVG_ENGINE_SW )
* \endcode
*
* \param[in] Canvas Объект Tvg_Canvas, управляющий зарезервированной памятью.
* \param[in] n Число объектов, для которых должна быть зарезервирована память.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Canvas.
*/
TVG_DEPRECATED TVG_API Tvg_Result tvg_canvas_reserve(Tvg_Canvas* canvas, uint32_t n);


/*!
* \brief Устанавливает общее количество красок, попавших на холст, равным нулю.
* Объекты Tvg_Paint, хранящиеся на холсте, освобождаются, если для параметра @p free установлено значение @c true, в противном случае память не освобождается и
* все краски следует освобождать вручную во избежание утечек памяти.
*
* \param[in] Canvas Объект Tvg_Canvas, который необходимо очистить.
* \param[in] free Если @c true, память, занятая красками, освобождается, в противном случае — нет.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Canvas.
*
* \см. tvg_canvas_destroy ()
*/
TVG_API Tvg_Result tvg_canvas_clear(Tvg_Canvas* canvas, bool free);


/*!
* \brief Обновляет все краски на холсте.
*
* Следует позвонить перед рисованием, чтобы подготовить краски к рендерингу.
*
* \code
* //Пример рисования рамки. Реализация потокобезопасности и событий пропускается, чтобы отображался только код TVG.
*
* статический Tvg_Canvas *canvas = NULL ;
* статический Tvg_Paint *rect = NULL ;
*
* int _frame_render (недействительный) {
*   tvg_canvas_update (холст);
*   tvg_canvas_draw (холст);
*   tvg_canvas_sync (холст);
* }
*
* //обработчик событий из вашего кода или сторонней библиотеки
* void _event_handler (событие * event_data) {
*   если (! event_data) вернуть NULL;
*     переключатель( event_data .type) {
*       случай EVENT_RECT_ADD :
*         если (!прямо) {
*           tvg_shape_append_rect (прямоугольник, 10, 10, 50, 50, 0, 0);
*           tvg_shape_set_stroke_width (прямоугольный, 1.0f);
*           tvg_shape_set_stroke_color (прямоугольный, 255, 0, 0, 255);
*           tvg_canvas_push (холст, прямоугольник);
*         }
*         перерыв;
*       случай EVENT_RECT_MOVE :
*         если (прямой) tvg_paint_translate (прямой, 10.0, 10.0);
*           перерыв;
*         по умолчанию:
*           перерыв;
*   }
* }
*
* int main(int argc, char **argv) {
*   //пример обработчика из вашего кода или сторонней библиотеки
*   event_handler_add (обработчик, _event_handler );
*
*   //создаем процесс рендеринга кадра, который вызывает функцию _frame_render().
*   app_loop_begin ( _frame_render );
*   app_loop_finish ();
*   cleanup() ;
* }
* \endcode
*
* \param[in] Canvas Объект Tvg_Canvas, который необходимо обновить.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Canvas.
*
* \см. tvg_canvas_update_paint ()
*/
TVG_API Tvg_Result tvg_canvas_update(Tvg_Canvas* canvas);


/*!
* \brief Обновляет данный объект Tvg_Paint с холста перед рендерингом.
*
* Если клиентское приложение, использующее библиотеку TVG, не обновляет весь холст с помощью tvg_canvas_update() в кадре
* В процессе рендеринга объекты Tvg_Paint, ранее добавленные на холст, следует обновлять вручную с помощью этой функции.
*
* \param[in] холст Объект Tvg_Canvas, которому принадлежит краска @p.
* \param[in] Paint Объект Tvg_Paint, который необходимо обновить.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В случае, если в качестве аргумента передается нулевой параметр @c.
*
* \см. tvg_canvas_update ()
*/
TVG_API Tvg_Result tvg_canvas_update_paint(Tvg_Canvas* canvas, Tvg_Paint* paint);


/*!
* \brief Запрашивает холст для рисования объектов Tvg_Paint.
*
* Все краски с данного холста будут растрированы в буфер.
*
* \param[in] Canvas Объект Tvg_Canvas, содержащий элементы для рисования.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Canvas.
*
* \note Рисование может быть асинхронным в зависимости от назначенного номера резьбы. Чтобы гарантировать, что рисунок выполнен, после этого вызовите tvg_canvas_sync().
* \см. tvg_canvas_sync ()
*/
TVG_API Tvg_Result tvg_canvas_draw(Tvg_Canvas* canvas);


/*!
* \brief Гарантирует, что процесс рисования завершен.
*
* Поскольку рендеринг холста может выполняться асинхронно, его следует вызывать после tvg_canvas_draw().
*
* \param[in] Canvas Объект Tvg_Canvas, содержащий нарисованные элементы.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Canvas.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION @p Холст либо уже находится в состоянии синхронизации, либо в поврежденном состоянии (перед синхронизацией требуется отрисовка).
*
* \см. tvg_canvas_draw ()
*/
TVG_API Tvg_Result tvg_canvas_sync(Tvg_Canvas* canvas);


/*!
* \brief Устанавливает область рисования на холсте.
*
* Эта функция определяет прямоугольную область холста, которая будет использоваться для операций рисования.
* Указанный область просмотра используется для обрезки вывода рендеринга по границам прямоугольника.
*
* \param[in] Canvas Объект Tvg_Canvas, содержащий нарисованные элементы.
* \param[in] x Координата X верхнего левого угла прямоугольника.
* \param[in] y Координата Y верхнего левого угла прямоугольника.
* \param[in] w Ширина прямоугольника.
* \param[in] h Высота прямоугольника.
*
* \return Перечисление Tvg_Result.
*
* \предупреждение Не разрешается менять область просмотра во время tvg_canvas_update () - tvg_canvas_sync () или tvg_canvas_push () - tvg_canvas_sync ().
*
* \note При сбросе целевого размера область просмотра также будет сброшена до целевого размера.
* \см. tvg_swcanvas_set_target ()
* \с 0,15
*/
TVG_API Tvg_Result tvg_canvas_set_viewport(Tvg_Canvas* canvas, int32_t x, int32_t y, int32_t w, int32_t h);

/** \} */   // конец защитной группы ThorVGCapi_Canvas


/**
* \defgroup ThorVGCapi_Paint Краска
* \brief Модуль для управления графическими элементами. Это позволяет дублировать, трансформировать и композицию.
*
* \{
*/

/************************************************************************/
/* Краска API                                                            */
/************************************************************************/
/*!
* \brief Освобождает данный объект Tvg_Paint.
*
* \code
* //пример функции очистки
* Tvg_Paint *rect = NULL ; //прямоугольная форма добавлена в другую функцию
*
* //удаляем прямоугольник API
* int rectangle_delete (недействительный) {
*   если (прямой) tvg_paint_del (прямой);
*   прямоугольник = NULL ;
* }
*
* int очистка (недействительный) {
*   tvg_canvas_clear (холст, ложь);
*   tvg_canvas_destroy (холст);
*   холст = NULL;
* }
* \endcode
*
* \param[in] Paint Объект Tvg_Paint, который необходимо освободить.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*
* \warning Если используется эта функция, во избежание непредвиденного поведения следует использовать tvg_canvas_clear () со значением свободного аргумента @c, установленным в @c false.
*
* \см. tvg_canvas_clear (), tvg_canvas_destroy ()
*/
TVG_API Tvg_Result tvg_paint_del(Tvg_Paint* paint);


/*!
* \brief Масштабирует данный объект Tvg_Paint с заданным коэффициентом.
*
* \param[in] Paint Объект Tvg_Paint, который нужно масштабировать.
* \param[in] коэффициент Значение коэффициента масштабирования. Значение по умолчанию — 1.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION в случае применения специального преобразования.
*
* \см. tvg_paint_set_transform ()
*/
TVG_API Tvg_Result tvg_paint_scale(Tvg_Paint* paint, float factor);


/*!
* \brief Поворачивает заданный Tvg_Paint на заданный угол.
*
* Угол измеряется по часовой стрелке от горизонтальной оси.
* Ось вращения проходит через точку на объекте с нулевыми координатами.
*
* \param[in] Paint Объект Tvg_Paint, который нужно повернуть.
* \param[in] степень Значение угла поворота в градусах.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION в случае применения специального преобразования.
*
* \см. tvg_paint_set_transform ()
*/
TVG_API Tvg_Result tvg_paint_rotate(Tvg_Paint* paint, float degree);


/*!
* \brief Перемещает заданный Tvg_Paint в двумерном пространстве.
*
* Начало системы координат находится в верхнем левом углу холста.
* Горизонтальная и вертикальная оси указывают вправо и вниз соответственно.
*
* \param[in] Paint Объект Tvg_Paint, который необходимо сдвинуть.
* \param[in] x Значение горизонтального сдвига.
* \param[in] y Значение вертикального сдвига.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION в случае применения специального преобразования.
*
* \см. tvg_paint_set_transform ()
*/
TVG_API Tvg_Result tvg_paint_translate(Tvg_Paint* paint, float x, float y);


/*!
* \brief Преобразует заданный Tvg_Paint, используя расширенную матрицу преобразования.
*
* Ожидается получение расширенной матрицы преобразования.
*
* \param[in] Paint Объект Tvg_Paint, который необходимо преобразовать.
* \param[in] m Расширенная матрица 3x3.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передается значение @c nullptr.
*/
TVG_API Tvg_Result tvg_paint_set_transform(Tvg_Paint* paint, const Tvg_Matrix* m);


/*!
* \brief Получает матрицу аффинного преобразования данного объекта Tvg_Paint.
*
* Если преобразование не применялось, возвращается единичная матрица.
*
* \param[in] Paint Объект Tvg_Paint, из которого нужно получить матрицу преобразования.
* \param[out] m Расширенная матрица 3x3.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передается значение @c nullptr.
*/
TVG_API Tvg_Result tvg_paint_get_transform(Tvg_Paint* paint, Tvg_Matrix* m);


/*!
* \brief Устанавливает непрозрачность данного Tvg_Paint .
*
* \param[in] Paint Объект Tvg_Paint, для которого необходимо установить значение непрозрачности.
* \param[in] opacity Значение непрозрачности в диапазоне [0 ~ 255], где 0 — полная прозрачность, а 255 — непрозрачность.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*
* \note Установка непрозрачности с помощью этого API может потребовать многократного рендеринга с использованием композиции. По возможности рекомендуется избегать изменения непрозрачности.
*/
TVG_API Tvg_Result tvg_paint_set_opacity(Tvg_Paint* paint, uint8_t opacity);


/*!
* \brief Получает непрозрачность заданного Tvg_Paint .
*
* \param[in] Paint Объект Tvg_Paint, для которого нужно получить значение непрозрачности.
* \param[out] opacity Значение непрозрачности в диапазоне [0 ~ 255], где 0 — полная прозрачность, а 255 — непрозрачность.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В случае, если в качестве аргумента передается нулевой параметр @c.
*/
TVG_API Tvg_Result tvg_paint_get_opacity(const Tvg_Paint* paint, uint8_t* opacity);


/*!
* \brief Дублирует заданный объект Tvg_Paint.
*
* Создает новый объект и устанавливает все его свойства как в исходном объекте.
*
* \param[in] Paint Объект Tvg_Paint, который необходимо скопировать.
*
* \return Скопированный объект Tvg_Paint в случае успеха, в противном случае @c nullptr.
*/
TVG_API Tvg_Paint* tvg_paint_duplicate(Tvg_Paint* paint);


/*!
* \brief Получает выровненную по оси ограничивающую рамку объекта Tvg_Paint.
*
* \param[in] Paint Объект Tvg_Paint, границы которого нужно получить.
* \param[out] x Координата X верхнего левого угла объекта.
* \param[out] y Координата Y верхнего левого угла объекта.
* \param[out] w Ширина объекта.
* \param[out] h Высота объекта.
* \param[in] Transform Если @c true, преобразования краски учитываются в сцене, к которой она принадлежит. В противном случае это не так.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*
* \note Это полезно, когда вам нужно определить ограничивающую рамку краски в пространстве холста.
* \note Ограничивающая рамка не указывает фактическую область рисования. Это наименьший прямоугольник, заключающий объект.
* \note Если преобразование @p имеет значение @c true, краску необходимо перенести на холст и обновить перед вызовом этого API.
* \см. tvg_canvas_update_paint ()
*/
TVG_API Tvg_Result tvg_paint_get_bounds(const Tvg_Paint* paint, float* x, float* y, float* w, float* h, bool transformed);


/*!
* \brief Устанавливает целевой объект композиции и метод композиции.
*
* \param[in] краска Исходный объект композиции.
* \param[in] target Целевой объект композиции.
* \param[in] метод Метод, используемый для объединения исходного объекта с целевым.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Недопустимая отрисовка @p или целевой объект @p или метод @p, равный TVG_COMPOSITE_METHOD_NONE .
*/
TVG_API Tvg_Result tvg_paint_set_composite_method(Tvg_Paint* paint, Tvg_Paint* target, Tvg_Composite_Method method);


/**
* \brief Получает целевой объект композиции и метод композиции.
*
* \param[in] краска Исходный объект композиции.
* \param[out] target Целевой объект композиции.
* \param[out] метод Метод, используемый для объединения исходного объекта с целевым.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передается значение @c nullptr.
*/
TVG_API Tvg_Result tvg_paint_get_composite_method(const Tvg_Paint* paint, const Tvg_Paint** target, Tvg_Composite_Method* method);


/*!
* \brief Вырежьте область рисования объекта рисования.
*
* Эта функция ограничивает область рисования объекта рисования путями указанной фигуры.
*
* \param[in] Paint Целевой объект вырезания.
* \param[in] clipper Объект формы в качестве инструмента обрезки.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В случае, если в качестве аргумента передается нулевой параметр @c.
* \retval TVG_RESULT_NOT_SUPPORTED Если тип клипера @p не Shape.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_paint_set_clip(Tvg_Paint* paint, Tvg_Paint* clipper);


/**
* \brief Получает уникальное значение экземпляра рисования, указывающее тип экземпляра.
*
* \param[in] Paint Объект Tvg_Paint, значение типа которого необходимо получить.
* \param[out] type Уникальный тип типа экземпляра рисования.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В случае, если в качестве аргумента передается нулевой параметр @c.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_paint_get_type(const Tvg_Paint* paint, Tvg_Type* type);


/**
* \см. tvg_paint_get_type ()
*/
TVG_DEPRECATED TVG_API Tvg_Result tvg_paint_get_identifier(const Tvg_Paint* paint, Tvg_Identifier* identifier);


/**
 * @brief Sets the blending method for the paint object.
 *
 * Функция смешивания позволяет комбинировать цвета для создания визуально привлекательных эффектов, включая, среди прочего, прозрачность, освещение, затенение и смешивание цветов.
 * его процесс включает в себя сочетание цветов или изображений исходного объекта рисования с целевым (изображением нижнего слоя) с использованием операций смешивания.
 * Операция смешивания определяется выбранным @p BlendMethod, который определяет, как комбинируются цвета или изображения.
 *
 * \param[in] Paint Объект Tvg_Paint, для которого устанавливается метод смешивания.
 * \param[in] метод Устанавливаемый метод смешивания.
 *
 * \return Перечисление Tvg_Result.
 * \retval TVG_RESULT_INVALID_ARGUMENT В случае, если в качестве аргумента передается нулевой параметр @c.
 *
 * \с 0,15
 */
TVG_API Tvg_Result tvg_paint_set_blend_method(Tvg_Paint* paint, Tvg_Blend_Method method);


/** \} */   // конец защитной группы ThorVGCapi_Paint

/**
* \defgroup Форма ThorVGCapi_Shape
*
* \brief Модуль для управления двумерными фигурами и их свойствами.
*
* Форма имеет три основных свойства: контур фигуры, обводка, заполнение. Контур фигуры сохраняется как путь.
* Путь может быть составлен путем накопления примитивных команд, таких как tvg_shape_move_to(), tvg_shape_line_to(), tvg_shape_cubic_to(), или полных интерфейсов формы, таких как tvg_shape_append_rect(), tvg_shape_append_circle() и т. д.
* Путь может состоять из подпутей. Один подпуть определяется командой закрытия.
*
* Обводка фигуры — это необязательное свойство, если фигуру необходимо представить с границами контура или без них.
* Это эффективно, поскольку путь формы и путь обводки можно использовать совместно. Это также удобно при управлении обоими в одном контексте.
*
* \{
*/

/************************************************************************/
/* Форма API                                                            */
/************************************************************************/
/*!
* \brief Создает новый объект формы.
*
* \return Новый объект формы.
*/
TVG_API Tvg_Paint* tvg_shape_new(void);


/*!
* \brief Сбрасывает свойства контура фигуры.
*
* Свойства цвета, заливки и обводки сохраняются.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*
* \note Память, в которой хранятся данные пути, на этом этапе не освобождается для эффекта кэширования.
*/
TVG_API Tvg_Result tvg_shape_reset(Tvg_Paint* paint);


/*!
* \brief Устанавливает начальную точку подпути.
*
* Значение текущей точки устанавливается в данную точку.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in] x Горизонтальная координата начальной точки подпути.
* \param[in] y Вертикальная координата начальной точки подпути.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*/
TVG_API Tvg_Result tvg_shape_move_to(Tvg_Paint* paint, float x, float y);


/*!
* \brief Добавляет новую точку к подпути, в результате чего рисуется линия от текущей точки до заданной конечной точки.
*
* Значение текущей точки устанавливается в заданную конечную точку.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in] x Горизонтальная координата конечной точки линии.
* \param[in] y Вертикальная координата конечной точки линии.

* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*
* \note Если это первая команда в пути, она соответствует вызову tvg_shape_move_to().
*/
TVG_API Tvg_Result tvg_shape_line_to(Tvg_Paint* paint, float x, float y);


/*!
* \brief Добавляет новые точки к подпути, что приводит к рисованию кубической кривой Безье.
*
* Кривая Безье начинается в текущей точке и заканчивается в заданной конечной точке ( @p x, @p y). Две контрольные точки ( @p cx1, @p cy1) и ( @p cx2, @p cy2) используются для определения формы кривой.
* Значение текущей точки устанавливается в заданную конечную точку.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in] cx1 Горизонтальная координата первой контрольной точки.
* \param[in] cy1 Вертикальная координата первой контрольной точки.
* \param[in] cx2 Горизонтальная координата второй контрольной точки.
* \param[in] cy2 Вертикальная координата второй контрольной точки.
* \param[in] x Горизонтальная координата конечной точки кривой.
* \param[in] y Вертикальная координата конечной точки кривой.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*
* \note Если это первая команда в пути, никакие данные из пути не отображаются.
*/
TVG_API Tvg_Result tvg_shape_cubic_to(Tvg_Paint* paint, float cx1, float cy1, float cx2, float cy2, float x, float y);


/*!
* \brief Закрывает текущий вложенный путь, рисуя линию от текущей точки до начальной точки вложенного пути.
*
* Значение текущей точки устанавливается в начальную точку замкнутого подпути.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*
* \note Если подпуть не содержит точек, эта функция не действует.
*/
TVG_API Tvg_Result tvg_shape_close(Tvg_Paint* paint);


/*!
* \brief Добавляет прямоугольник к пути.
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
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in] x Горизонтальная координата верхнего левого угла прямоугольника.
* \param[in] y Вертикальная координата верхнего левого угла прямоугольника.
* \param[in] w Ширина прямоугольника.
* \param[in] h Высота прямоугольника.
* \param[in] rx Радиус эллипса по оси X, определяющий закругленные углы прямоугольника.
* \param[in] ry Радиус эллипса по оси Y, определяющий закругленные углы прямоугольника.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*
& \note Если @p rx и @p ry больше или равны половине @p w и половине @p h соответственно, форма становится эллипсом.
*/
TVG_API Tvg_Result tvg_shape_append_rect(Tvg_Paint* paint, float x, float y, float w, float h, float rx, float ry);


/*!
* \brief Добавляет к пути эллипс.
*
* Положение эллипса задается координатами его центра — аргументами @p cx и @p cy.
*
* Эллипс рассматривается как новый подпуть — он не связан с предыдущим подпутем.
*
* Значение текущей точки установлено на ( @p cx, @p cy - @p ry).
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in] cx Горизонтальная координата центра эллипса.
* \param[in] cy Вертикальная координата центра эллипса.
* \param[in] rx Радиус эллипса по оси X.
* \param[in] ry Радиус эллипса по оси Y.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*/
TVG_API Tvg_Result tvg_shape_append_circle(Tvg_Paint* paint, float cx, float cy, float rx, float ry);


/*!
* \brief Добавляет к пути дугу окружности.
*
* Дуга рассматривается как новый подпуть - она не связана с предыдущим подпутем.
* Текущее значение точки устанавливается в конечную точку дуги, если круговая диаграмма @p имеет значение @c false, и в центр дуги в противном случае.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in] cx Горизонтальная координата центра дуги.
* \param[in] cy Вертикальная координата центра дуги.
* \param[in] радиус Радиус дуги.
* \param[in] startAngle Начальный угол дуги, заданный в градусах и измеренный против часовой стрелки от горизонтальной линии.
* \param[in] развертка Центральный угол дуги, заданный в градусах и отсчитываемый против часовой стрелки от @p startAngle.
* \param[in] pie Определяет, следует ли рисовать радиусы от центра дуги до обеих ее конечных точек — рисуется, если @c истинно.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*
* \note Установка значения развертки @p более 360 градусов эквивалентна вызову tvg_shape_append_circle (paint, cx, cy, radius, radius).
*/
TVG_API Tvg_Result tvg_shape_append_arc(Tvg_Paint* paint, float cx, float cy, float radius, float startAngle, float sweep, uint8_t pie);


/*!
* \brief Добавляет заданный подпуть к пути.
*
* Текущее значение точки устанавливается на последнюю точку подпути.
* Для каждой команды из cmds-массива @p необходимо указать соответствующее количество точек в массиве @p pts.
* Если количество точек в массиве точек @p отличается от количества, требуемого массивом cmds @p, фигура с этим подпутем не будет отображаться на экране.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in] cmds Массив команд в подпути.
* \param[in] cmdCnt Длина массива командных команд @p.
* \param[in] pts Массив двумерных точек.
* \param[in] ptsCnt Длина массива точек @p.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT @c nullptr, переданный в качестве аргумента, или @p cmdCnt или @p ptsCnt, равный нулю.
*/
TVG_API Tvg_Result tvg_shape_append_path(Tvg_Paint* paint, const Tvg_Path_Command* cmds, uint32_t cmdCnt, const Tvg_Point* pts, uint32_t ptsCnt);


/*!
* \brief Получает значения точек пути.
*
* Функция не выделяет никаких данных, она работает с внутренней памятью. Нет необходимости освобождать массив точек @p.
*
* \code
* Tvg_Paint *форма = tvg_shape_new ();
* Tvg_Point *coords = NULL ;
* uint32_t лен = 0;
*
* tvg_shape_append_circle (форма, 10, 10, 50, 50);
* tvg_shape_get_path_coords (форма, (const Tvg_Point **)&coords, &len);
* // TVG аппроксимирует окружность четырьмя кривыми Безье. В приведенном выше примере массив координат хранит их координаты.
* \endcode
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[out] pts Указатель на массив двумерных точек пути.
* \param[out] cnt Длина массива точек @p.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передается нулевой параметр @c.
*/
TVG_API Tvg_Result tvg_shape_get_path_coords(const Tvg_Paint* paint, const Tvg_Point** pts, uint32_t* cnt);


/*!
* \brief Получает данные команд пути.
*
* Функция не выделяет никаких данных. Нет необходимости освобождать массив cmds @p.
*
* \code
* Tvg_Paint *форма = tvg_shape_new ();
* Tvg_Path_Command *cmds = NULL ;
* uint32_t лен = 0;
*
* tvg_shape_append_circle (форма, 10, 10, 50, 50);
* tvg_shape_get_path_commands (форма, (const Tvg_Path_Command **)&cmds, &len);
* // TVG аппроксимирует окружность четырьмя кривыми Безье. В приведенном выше примере массив cmds хранит команды данных пути.
* \endcode
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[out] cmds Указатель на массив команд из пути.
* \param[out] cnt Длина массива командных команд @p.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передается нулевой параметр @c.
*/
TVG_API Tvg_Result tvg_shape_get_path_commands(const Tvg_Paint* paint, const Tvg_Path_Command** cmds, uint32_t* cnt);


/*!
* \brief Устанавливает ширину обводки для всех фигур из краски @p.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in] width Ширина обводки. Значение по умолчанию — 0.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_width(Tvg_Paint* paint, float width);


/*!
* \brief Получает ширину обводки фигуры.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[out] width Ширина обводки.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передан недопустимый указатель.
*/
TVG_API Tvg_Result tvg_shape_get_stroke_width(const Tvg_Paint* paint, float* width);


/*!
* \brief Устанавливает цвет обводки фигуры.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in] r Значение канала красного цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
* \param[in] g Значение канала зеленого цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
* \param[in] b Значение канала синего цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
* \param[in] a Значение альфа-канала в диапазоне [0 ~ 255], где 0 — полностью прозрачный, а 255 — непрозрачный.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*
* \note Применяется либо сплошной цвет, либо градиентная заливка, в зависимости от того, что было установлено последним.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_color(Tvg_Paint* paint, uint8_t r, uint8_t g, uint8_t b, uint8_t a);


/*!
* \brief Получает цвет обводки фигуры.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[out] r Значение канала красного цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
* \param[out] g Значение канала зеленого цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
* \param[out] b Значение канала синего цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
* \param[out] a Значение альфа-канала в диапазоне [0 ~ 255], где 0 — полностью прозрачный, а 255 — непрозрачный.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION Ход не установлен.
*/
TVG_API Tvg_Result tvg_shape_get_stroke_color(const Tvg_Paint* paint, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);


/*!
* \brief Устанавливает линейную градиентную заливку обводки для всех фигур контура.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in] grad Линейная градиентная заливка.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
* \retval TVG_RESULT_MEMORY_CORRUPTION Неверный указатель Tvg_Gradient или ошибка доступа к нему.
*
* \note Применяется либо сплошной цвет, либо градиентная заливка, в зависимости от того, что было установлено последним.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_linear_gradient(Tvg_Paint* paint, Tvg_Gradient* grad);


/*!
* \brief Устанавливает радиальную градиентную заливку обводки для всех фигур контура.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in] grad Заливка радиальным градиентом.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
* \retval TVG_RESULT_MEMORY_CORRUPTION Неверный указатель Tvg_Gradient или ошибка доступа к нему.
*
* \note Применяется либо сплошной цвет, либо градиентная заливка, в зависимости от того, что было установлено последним.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_radial_gradient(Tvg_Paint* paint, Tvg_Gradient* grad);


/*!
* \brief Получает градиентную заливку обводки фигуры.
*
* Функция не выделяет никакой памяти.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[out] grad Градиентная заливка.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передан недопустимый указатель.
*/
TVG_API Tvg_Result tvg_shape_get_stroke_gradient(const Tvg_Paint* paint, Tvg_Gradient** grad);


/*!
* \brief Устанавливает образец штриха обводки фигуры.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in] DashPattern Массив последовательных парных значений длины штриха и длины пробела.
* \param[in] cnt Размер массива DashPattern @p.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Недопустимый указатель, переданный в качестве аргумента, и @p cnt > 0, заданная длина массива меньше двух или любое из значений DashPattern @p равно нулю или меньше.
*
* \note Чтобы сбросить образец штрихового штриха, передайте @c nullptr в @p DashPattern и ноль в @p cnt.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_dash(Tvg_Paint* paint, const float* dashPattern, uint32_t cnt);


/*!
* \brief Получает штриховой образец обводки.
*
* Функция не выделяет никакой памяти.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[out] DashPattern Массив последовательных парных значений длины штриха и длины пробела.
* \param[out] cnt Размер массива DashPattern @p.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передан недопустимый указатель.
*/
TVG_API Tvg_Result tvg_shape_get_stroke_dash(const Tvg_Paint* paint, const float** dashPattern, uint32_t* cnt);


/*!
* \brief Устанавливает стиль окончания, используемый для обводки пути.
*
* Стиль окончания определяет форму, которая будет использоваться в конце открытых контуров с обводкой.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in] cap Значение стиля шапки. Значение по умолчанию — @c TVG_STROKE_CAP_SQUARE.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_cap(Tvg_Paint* paint, Tvg_Stroke_Cap cap);


/*!
* \brief Получает стиль обводки, используемый для обводки пути.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[out] cap Значение стиля шапки.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передан недопустимый указатель.
*/
TVG_API Tvg_Result tvg_shape_get_stroke_cap(const Tvg_Paint* paint, Tvg_Stroke_Cap* cap);


/*!
* \brief Устанавливает стиль соединения для обведенных сегментов пути.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in] join Значение стиля соединения. Значение по умолчанию — @c TVG_STROKE_JOIN_BEVEL.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_join(Tvg_Paint* paint, Tvg_Stroke_Join join);


/*!
* \brief Функция получает метод соединения штрихов
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[out] join Значение стиля соединения.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передан недопустимый указатель.
*/
TVG_API Tvg_Result tvg_shape_get_stroke_join(const Tvg_Paint* paint, Tvg_Stroke_Join* join);


/*!
* \brief Устанавливает предел хода.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in] miterlimit Ограничение на митер накладывает ограничение на размер соединения штрихов, если установлен стиль соединения @c TVG_STROKE_JOIN_MITER. Значение по умолчанию — 4.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Недопустимый указатель Tvg_Paint или неподдерживаемые значения предела скоса @p (меньше нуля).
*
* \с 0,11
*/
TVG_API Tvg_Result tvg_shape_set_stroke_miterlimit(Tvg_Paint* paint, float miterlimit);


/*!
* \brief Функция получает предел хода митера.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[out] miterlimit Ограничение хода хода.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передан недопустимый указатель.
*
* \с 0,11
*/
TVG_API Tvg_Result tvg_shape_get_stroke_miterlimit(const Tvg_Paint* paint, float* miterlimit);


/*!
* \brief Устанавливает обрезку обводки вдоль определенного сегмента пути, позволяя контролировать, какая часть обводки видна.
*
* Если значения аргументов @p Begin и @p End превышают диапазон 0–1, они переносятся аналогично переносу углов, эффективно рассматривая диапазон как круговой.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in] Begin Указывает начало сегмента, отображаемого по пути.
* \param[in] end Указывает конец сегмента, отображаемого по пути.
* \param[in] одновременно Определяет, как обрезать несколько контуров внутри одной фигуры. Если установлено значение @c true (по умолчанию), обрезка применяется одновременно ко всем путям;
* В противном случае все пути рассматриваются как единый объект с общей длиной, равной сумме их отдельных длин, и обрезаются как таковые.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_shape_set_stroke_trim(Tvg_Paint* paint, float begin, float end, bool simultaneous);


/*!
* \brief Устанавливает сплошной цвет фигуры.
*
* Части фигуры, определенные как внутренние, окрашены.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in] r Значение канала красного цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
* \param[in] g Значение канала зеленого цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
* \param[in] b Значение канала синего цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
* \param[in] a Значение альфа-канала в диапазоне [0 ~ 255], где 0 — полностью прозрачный, а 255 — непрозрачный. Значение по умолчанию — 0.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*
* \note Применяется либо сплошной цвет, либо градиентная заливка, в зависимости от того, что было установлено последним.
* \см. tvg_shape_set_fill_rule ()
*/
TVG_API Tvg_Result tvg_shape_set_fill_color(Tvg_Paint* paint, uint8_t r, uint8_t g, uint8_t b, uint8_t a);


/*!
* \brief Получает сплошной цвет фигуры.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[out] r Значение канала красного цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
* \param[out] g Значение канала зеленого цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
* \param[out] b Значение канала синего цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
* \param[out] a Значение альфа-канала в диапазоне [0 ~ 255], где 0 — полностью прозрачный, а 255 — непрозрачный. Значение по умолчанию — 0.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*/
TVG_API Tvg_Result tvg_shape_get_fill_color(const Tvg_Paint* paint, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);


/*!
* \brief Устанавливает правило заливки фигуры.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in] rule Значение правила заполнения. Значение по умолчанию — @c TVG_FILL_RULE_WINDING.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*/
TVG_API Tvg_Result tvg_shape_set_fill_rule(Tvg_Paint* paint, Tvg_Fill_Rule rule);


/*!
* \brief Получает правило заливки фигуры.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[out] правило заполнения фигуры
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передан недопустимый указатель.
*/
TVG_API Tvg_Result tvg_shape_get_fill_rule(const Tvg_Paint* paint, Tvg_Fill_Rule* rule);


/*!
* \brief Устанавливает порядок отрисовки обводки и заливки.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in]strokeFirst Если @c true, обводка отображается перед заливкой, в противном случае обводка отображается как вторая (параметр по умолчанию).
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*
* \с 0,10
*/
TVG_API Tvg_Result tvg_shape_set_paint_order(Tvg_Paint* paint, bool strokeFirst);


/*!
* \brief Устанавливает заливку линейным градиентом для всех фигур на пути.
*
* Части фигуры, определенные как внутренние, заполняются.
*
* \code
* Tvg_Gradient *град = tvg_linear_gradient_new ();
* tvg_linear_gradient_set (град, 700, 700, 800, 800);
* Tvg_Color_Stop color_stops [4] =
* {
*   {0.0 , 0,   0,   0,   255},
*   {0.25, 255, 0,   0,   255},
*   {0.5 , 0,   255, 0,   255},
*   {1.0 , 0,   0,   255, 255}
* };
* tvg_gradient_set_color_stops (град, color_stops, 4);
* tvg_shape_set_linear_gradient (форма, град);
* \endcode
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in] grad Линейная градиентная заливка.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
* \retval TVG_RESULT_MEMORY_CORRUPTION Неверный указатель Tvg_Gradient.
*
* \note Применяется либо сплошной цвет, либо градиентная заливка, в зависимости от того, что было установлено последним.
* \см. tvg_shape_set_fill_rule ()
*/
TVG_API Tvg_Result tvg_shape_set_linear_gradient(Tvg_Paint* paint, Tvg_Gradient* grad);


/*!
* \brief Устанавливает радиальную градиентную заливку для всех фигур на пути.
*
* Части фигуры, определенные как внутренние, заполняются.
*
* \code
* Tvg_Gradient *град = tvg_radial_gradient_new ();
* tvg_radial_gradient_set (град, 550, 550, 50);
* Tvg_Color_Stop color_stops [4] =
* {
*   {0.0 , 0,   0,   0,   255},
*   {0.25, 255, 0,   0,   255},
*   {0.5 , 0,   255, 0,   255},
*   {1.0 , 0,   0,   255, 255}
* };
* tvg_gradient_set_color_stops (град, color_stops, 4);
* tvg_shape_set_radial_gradient (форма, град);
* \endcode
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[in] grad Заливка радиальным градиентом.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
* \retval TVG_RESULT_MEMORY_CORRUPTION Неверный указатель Tvg_Gradient.
*
* \note Применяется либо сплошной цвет, либо градиентная заливка, в зависимости от того, что было установлено последним.
* \см. tvg_shape_set_fill_rule ()
*/
TVG_API Tvg_Result tvg_shape_set_radial_gradient(Tvg_Paint* paint, Tvg_Gradient* grad);


/*!
* \brief Получает градиентную заливку фигуры.
*
* Функция не выделяет никаких данных.
*
* \param[in] Paint Указатель Tvg_Paint на объект формы.
* \param[out] grad Градиентная заливка.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передан недопустимый указатель.
*/
TVG_API Tvg_Result tvg_shape_get_gradient(const Tvg_Paint* paint, Tvg_Gradient** grad);


/** \} */   // конец защитной группы ThorVGCapi_Shape


/**
* \defgroup ThorVGCapi_Gradient Градиент
* \brief Модуль управления градиентной заливкой объектов.
*
* Модуль позволяет задавать и получать цвета градиента и их расположение внутри границ градиента,
* указать границы градиента и поведение градиента в случае, если область определяется границами градиента
* меньше площади, подлежащей заполнению.
*
* \{
*/

/************************************************************************/
/* Градиент API                                                         */
/************************************************************************/
/*!
* \brief Создает новый объект линейного градиента.
*
* \code
* Tvg_Paint * форма = tvg_shape_new ();
* tvg_shape_append_rect (форма, 700, 700, 100, 100, 20, 20);
* Tvg_Gradient *град = tvg_linear_gradient_new ();
* tvg_linear_gradient_set (град, 700, 700, 800, 800);
* Tvg_Color_Stop color_stops [2] =
* {
*   {0.0, 0, 0,   0, 255},
*   {1.0, 0, 255, 0, 255},
* };
* tvg_gradient_set_color_stops (град, color_stops, 2);
* tvg_shape_set_linear_gradient (форма, град);
* \endcode
*
* \return Новый объект линейного градиента.
*/
TVG_API Tvg_Gradient* tvg_linear_gradient_new(void);


/*!
* \brief Создает новый объект радиального градиента.
*
* \code
* Tvg_Paint * форма = tvg_shape_new ();
* tvg_shape_append_rect (форма, 700, 700, 100, 100, 20, 20);
* Tvg_Gradient *град = tvg_radial_gradient_new ();
* tvg_radial_gradient_set (град, 550, 550, 50);
* Tvg_Color_Stop color_stops [2] =
* {
*   {0.0, 0, 0,   0, 255},
*   {1.0, 0, 255, 0, 255},
* };
* tvg_gradient_set_color_stops (град, color_stops, 2);
* tvg_shape_set_radial_gradient (форма, град);
* \endcode
*
* \return Новый объект радиального градиента.
*/
TVG_API Tvg_Gradient* tvg_radial_gradient_new(void);


/*!
* \brief Устанавливает границы линейного градиента.
*
* Границы линейного градиента определяются как поверхность, ограниченная двумя параллельными линиями, пересекающими
* заданные точки ( @p x1, @p y1) и ( @p x2, @p y2) соответственно. Обе линии перпендикулярны линии, соединяющей
* ( @p x1, @p y1) и ( @p x2, @p y2).
*
* \param[in] grad Объект Tvg_Gradient, границы которого должны быть установлены.
* @param [in] x1 Горизонтальная координата первой точки, используемая для определения границ градиента.
* @param [in] y1 Вертикальная координата первой точки, используемой для определения границ градиента.
* @param [in] x2 Горизонтальная координата второй точки, используемая для определения границ градиента.
* @param [in] y2 Вертикальная координата второй точки, используемая для определения границ градиента.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Gradient.
*
* \note В случае, если первая и вторая точки равны, объект заливается одним цветом, используя последний цвет, указанный в tvg_gradient_set_color_stops().
* \см. tvg_gradient_set_color_stops ()
*/
TVG_API Tvg_Result tvg_linear_gradient_set(Tvg_Gradient* grad, float x1, float y1, float x2, float y2);


/*!
* \brief Получает границы линейного градиента.
*
* Границы линейного градиента определяются как поверхность, ограниченная двумя параллельными линиями, пересекающими
* заданные точки ( @p x1, @p y1) и ( @p x2, @p y2) соответственно. Обе линии перпендикулярны линии, соединяющей
* ( @p x1, @p y1) и ( @p x2, @p y2).
*
* \param[in] grad Объект Tvg_Gradient, границы которого нужно получить.
* \param[out] x1 Горизонтальная координата первой точки, используемая для определения границ градиента.
* \param[out] y1 Вертикальная координата первой точки, используемой для определения границ градиента.
* \param[out] x2 Горизонтальная координата второй точки, используемая для определения границ градиента.
* \param[out] y2 Вертикальная координата второй точки, используемая для определения границ градиента.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Gradient.
*/
TVG_API Tvg_Result tvg_linear_gradient_get(Tvg_Gradient* grad, float* x1, float* y1, float* x2, float* y2);


/*!
* \brief Устанавливает границы радиального градиента.
*
* Границы радиального градиента определяются как окружность с центром в заданной точке ( @p cx, @p cy) заданного радиуса.
*
* \param[in] grad Объект Tvg_Gradient, границы которого должны быть установлены.
* \param[in] cx Горизонтальная координата центра ограничивающего круга.
* \param[in] cy Вертикальная координата центра ограничивающего круга.
* \param[in] радиус Радиус ограничивающего круга.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Недопустимый указатель Tvg_Gradient или значение радиуса @p меньше нуля.
*
* \note В случае, если радиус @p равен нулю, объект заполняется одним цветом, используя последний цвет, указанный в указанном в tvg_gradient_set_color_stops().
* \см. tvg_gradient_set_color_stops ()
*/
TVG_API Tvg_Result tvg_radial_gradient_set(Tvg_Gradient* grad, float cx, float cy, float radius);


/*!
* \brief Функция получает центральную точку радиального градиента и радиус муравья.
*
* \param[in] grad Объект Tvg_Gradient, границы которого должны быть установлены.
* \param[out] cx Горизонтальная координата центра ограничивающего круга.
* \param[out] cy Вертикальная координата центра ограничивающего круга.
* \param[out] radius Радиус ограничивающего круга.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Gradient.
*/
TVG_API Tvg_Result tvg_radial_gradient_get(Tvg_Gradient* grad, float* cx, float* cy, float* radius);


/*!
* \brief Устанавливает параметры цветов градиента и их положение.
*
* \param[in] grad Объект Tvg_Gradient, для которого должна быть установлена информация о цвете.
* \param[in] color_stop Массив структуры данных Tvg_Color_Stop.
* \param[in] cnt Размер массива @p color_stop, равный количеству цветов, используемых в градиенте.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Gradient.
*/
TVG_API Tvg_Result tvg_gradient_set_color_stops(Tvg_Gradient* grad, const Tvg_Color_Stop* color_stop, uint32_t cnt);


/*!
* \brief Получает параметры цветов градиента, их положение и количество.
*
* Функция не выделяет никакой памяти.
*
* \param[in] grad Объект Tvg_Gradient, из которого требуется получить информацию о цвете.
* \param[out] color_stop Массив структуры данных Tvg_Color_Stop.
* \param[out] cnt Размер массива @p color_stop, равный количеству цветов, используемых в градиенте.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передается нулевой параметр @c.
*/
TVG_API Tvg_Result tvg_gradient_get_color_stops(const Tvg_Gradient* grad, const Tvg_Color_Stop** color_stop, uint32_t* cnt);


/*!
* \brief Устанавливает значение Tvg_Stroke_Fill, которое определяет, как заполнить область за пределами границ градиента.
*
* \param[in] grad Объект Tvg_Gradient.
* \param[in]spread Значение FillSpread.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Gradient.
*/
TVG_API Tvg_Result tvg_gradient_set_spread(Tvg_Gradient* grad, const Tvg_Stroke_Fill spread);


/*!
* \brief Получает значение FillSpread объекта градиента.
*
* \param[in] grad Объект Tvg_Gradient.
* \param[out]spread Значение FillSpread.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передается нулевой параметр @c.
*/
TVG_API Tvg_Result tvg_gradient_get_spread(const Tvg_Gradient* grad, Tvg_Stroke_Fill* spread);


/*!
* \brief Устанавливает матрицу аффинного преобразования для объекта градиента.
*
* Ожидается получение расширенной матрицы преобразования.
*
* \param[in] grad Объект Tvg_Gradient, который необходимо преобразовать.
* \param[in] m Расширенная матрица 3x3.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передается значение @c nullptr.
*/
TVG_API Tvg_Result tvg_gradient_set_transform(Tvg_Gradient* grad, const Tvg_Matrix* m);


/*!
* \brief Получает матрицу аффинного преобразования объекта градиента.
*
* Если преобразование не применялось, устанавливается единичная матрица.
*
* \param[in] grad Объект Tvg_Gradient, из которого нужно получить матрицу преобразования.
* \param[out] m Расширенная матрица 3x3.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передается значение @c nullptr.
*/
TVG_API Tvg_Result tvg_gradient_get_transform(const Tvg_Gradient* grad, Tvg_Matrix* m);

/**
* \brief Получает уникальное значение экземпляра градиента, указывающее тип экземпляра.
*
* \param[in] grad Объект Tvg_Gradient, для которого нужно получить значение типа.
* \param[out] type Уникальный тип экземпляра градиента.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В случае, если в качестве аргумента передается нулевой параметр @c.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_gradient_get_type(const Tvg_Gradient* grad, Tvg_Type* type);


/**
* \см. tvg_gradient_get_type ()
*/
TVG_DEPRECATED TVG_API Tvg_Result tvg_gradient_get_identifier(const Tvg_Gradient* grad, Tvg_Identifier* identifier);


/*!
* \brief Дублирует заданный объект Tvg_Gradient.
*
* Создает новый объект и устанавливает все его свойства как в исходном объекте.
*
* \param[in] grad Объект Tvg_Gradient, который необходимо скопировать.
*
* \return Скопированный объект Tvg_Gradient в случае успеха, в противном случае @c nullptr.
*/
TVG_API Tvg_Gradient* tvg_gradient_duplicate(Tvg_Gradient* grad);


/*!
* \brief Удаляет заданный объект градиента.
*
* \param[in] grad Объект градиента, который нужно удалить.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Gradient.
*/
TVG_API Tvg_Result tvg_gradient_del(Tvg_Gradient* grad);


/** \} */   // конец защитной группы ThorVGCapi_Gradient


/**
* \defgroup ThorVGCapi_Picture Изображение
*
* \brief Модуль, позволяющий создавать и загружать изображения в одном из поддерживаемых форматов: svg, png, jpg, lottie и raw.
*
*
* \{
*/

/************************************************************************/
/* Изображение API                                                          */
/************************************************************************/
/*!
* \brief Создает новый объект изображения.
*
* \return Новый объект изображения.
*/
TVG_API Tvg_Paint* tvg_picture_new(void);


/*!
* \brief Загружает данные изображения непосредственно из файла.
*
* ThorVG эффективно кэширует загруженные данные, используя указанный путь @p в качестве ключа.
* Это означает, что повторная загрузка того же файла не приведет к дублированию операций;
* вместо этого ThorVG будет повторно использовать ранее загруженные данные изображения.
*
* \param[in] Paint Указатель Tvg_Paint на объект изображения.
* \param[in] путь Абсолютный путь к файлу изображения.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Недопустимый указатель Tvg_Paint или пустой путь @p.
* \retval TVG_RESULT_NOT_SUPPORTED Файл с неизвестным расширением.
*/
TVG_API Tvg_Result tvg_picture_load(Tvg_Paint* paint, const char* path);


/*!
* \brief Загружает данные изображения из блока памяти заданного размера.
*
* ThorVG эффективно кэширует загруженные данные, используя указанный адрес данных @p в качестве ключа.
* когда копия @p имеет @c false. Это означает, что повторная загрузка тех же данных не приведет к дублированию операций.
* для общих данных @p. Вместо этого ThorVG будет повторно использовать ранее загруженные данные изображения.
*
* \param[in] Paint Указатель Tvg_Paint на объект изображения.
* \param[in] data Указатель на область памяти, где хранится содержимое необработанных данных изображения.
* \param[in] w Ширина данных @p изображения в пикселях.
* \param[in] h Высота данных @p изображения в пикселях.
* \param[in] premultiplied Если @c true, данные изображения предварительно умножаются в альфа-канале.
* \param[in] copy Если @c true, данные копируются в локальный буфер движка, в противном случае — нет.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint, или данные не предоставлены, или значение ширины @p или высоты @p равно нулю или меньше.
* \retval TVG_RESULT_FAILED_ALLOCATION Возникла проблема с распределением памяти.
*
* \с 0,9
*/
TVG_API Tvg_Result tvg_picture_load_raw(Tvg_Paint* paint, uint32_t *data, uint32_t w, uint32_t h, bool copy);


/*!
* \brief Загружает данные изображения из блока памяти заданного размера.
*
* ThorVG эффективно кэширует загруженные данные, используя указанный адрес данных @p в качестве ключа.
* когда копия @p имеет @c false. Это означает, что повторная загрузка тех же данных не приведет к дублированию операций.
* для общих данных @p. Вместо этого ThorVG будет повторно использовать ранее загруженные данные изображения.
*
* \param[in] Paint Указатель Tvg_Paint на объект изображения.
* \param[in] data Указатель на область памяти, где хранится содержимое файла изображения. Для недвоичных данных ожидается строка с нулевым завершением, если копия @p имеет значение @c false.
* \param[in] size Размер памяти в байтах, занимаемой данными @p.
* \param[in] mimetype Mimetype или расширение данных, например «jpg», «jpeg», «svg», «svg+xml», «lottie», «png» и т. д. Если указана пустая строка или неизвестный тип, загрузчики будут проверены один за другим.
* \param[in] copy Если @c true, данные копируются в локальный буфер движка, в противном случае — нет.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В случае, если в качестве аргумента передан нулевой параметр @c или размер @p равен нулю или меньше.
* \retval TVG_RESULT_NOT_SUPPORTED Файл с неизвестным расширением.
*
* \Предупреждение: Пользователь несет ответственность за освобождение памяти данных @p, если копия @p имеет значение @c true.
*/
TVG_API Tvg_Result tvg_picture_load_data(Tvg_Paint* paint, const char *data, uint32_t size, const char *mimetype, bool copy);


/*!
* \brief Изменяет размер содержимого изображения до заданной ширины и высоты.
*
* Размер содержимого изображения изменяется с сохранением соотношения сторон размера по умолчанию.
* Коэффициент масштабирования устанавливается для каждого из измерений, и к обоим из них применяется меньшее значение.
*
* \param[in] Paint Указатель Tvg_Paint на объект изображения.
* \param[in] w Новая ширина изображения в пикселях.
* \param[in] h Новая высота изображения в пикселях.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*/
TVG_API Tvg_Result tvg_picture_set_size(Tvg_Paint* paint, float w, float h);


/*!
* \brief Получает размер загруженного изображения.
*
* \param[in] Paint Указатель Tvg_Paint на объект изображения.
* \param[out] w Ширина изображения в пикселях.
* \param[out] h Высота изображения в пикселях.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*/
TVG_API Tvg_Result tvg_picture_get_size(const Tvg_Paint* paint, float* w, float* h);


/*!
* \brief Извлекает объект рисования из сцены Picture по его уникальному значению ID .
*
* Эта функция ищет объект рисования в сцене изображения, соответствующий предоставленному идентификатору @p.
*
* \param[in] Paint Указатель Tvg_Paint на объект изображения.
* \param[in] id Уникальный ID объекта рисования.

* \return Указатель на объект рисования, соответствующий заданному идентификатору, или @c nullptr, если соответствующий объект рисования не найден.
*
* \см. tvg_accessor_generate_id ()
* \примечание экспериментальный API
*/
TVG_API const Tvg_Paint* tvg_picture_get_paint(Tvg_Paint* paint, uint32_t id);


/** \} */   // конец защитной группы ThorVGCapi_Picture


/**
* \defgroup Сцена ThorVGCapi_Scene
* \brief Модуль, управляющий несколькими отрисовками как одной групповой отрисовкой.
*
* Как группа, сцена может трансформироваться, становиться полупрозрачной, комбинироваться с другими целевыми красками,
* на его детей будет влиять мир сцены.
*
* \{
*/

/************************************************************************/
/* Сцена API                                                            */
/************************************************************************/
/*!
* \brief Создает новый объект сцены.
*
* Объект сцены используется для группировки множества красок в один объект, которым можно манипулировать с помощью API-интерфейсов TVG.
*
* \return Новый объект сцены.
*/
TVG_API Tvg_Paint* tvg_scene_new(void);


/*!
* \brief Устанавливает размер контейнера, в котором хранятся все краски, помещенные в сцену.
*
* Если количество объектов, помещенных в сцену, известно заранее, вызов функции
* предотвращает многократное перераспределение памяти, тем самым повышая производительность.
*
* \param[in] сцена Указатель Tvg_Paint на объект сцены.
* \param[in] size Число объектов, для которых должна быть зарезервирована память.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_FAILED_ALLOCATION Внутренняя ошибка при выделении памяти.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Paint.
*/
TVG_DEPRECATED TVG_API Tvg_Result tvg_scene_reserve(Tvg_Paint* scene, uint32_t size);


/*!
* \brief Передает элементы рисования на сцену, используя объекты Tvg_Paint.
*
* Нарисованными целями будут только краски, попавшие в сцену.
* Краски сохраняются в сцене до тех пор, пока не будет вызвана функция tvg_scene_clear().
* Если вы заранее знаете количество толкаемых объектов, позвоните в tvg_scene_reserve().
*
* \param[in] сцена Указатель Tvg_Paint на объект сцены.
* \param[in] краска Графический объект, который нужно отрисовать.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передается нулевой параметр @c.
*
* \note Порядок отрисовки красок такой же, как и порядок их перемещения. Если вы собираетесь использовать слои, рассмотрите возможность сортировки красок перед тем, как их раскладывать.
*/
TVG_API Tvg_Result tvg_scene_push(Tvg_Paint* scene, Tvg_Paint* paint);


/*!
* \brief Очищает объекты сцены от нажатых красок.
*
* Объекты Tvg_Paint, хранящиеся в сцене, освобождаются, если для параметра @p free установлено значение @c true, в противном случае память не освобождается и
* все краски следует освобождать вручную во избежание утечек памяти.
*
* \param[in] сцена Объект сцены, который необходимо очистить.
* \param[in] free Если @c true, память, занятая красками, освобождается, в противном случае — нет.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Canvas.
*
* \предупреждение Пожалуйста, используйте свободный аргумент @p только в том случае, если вы знаете, как он работает, в противном случае это не рекомендуется.
*/
TVG_API Tvg_Result tvg_scene_clear(Tvg_Paint* scene, bool free);

/** \} */   // конец защитной группы ThorVGCapi_Scene



/**
* \defgroup ThorVGCapi_Text Текст
* \brief Класс для представления текстовых объектов в графическом контексте, позволяющий отображать и манипулировать текстом в Юникоде.
*
* \с 0,15
*
* \{
*/

/************************************************************************/
/* Текст API                                                            */
/************************************************************************/
/*!
* \brief Создает новый текстовый объект.
*
* \return Новый текстовый объект.
*
* \с 0,15
*/
TVG_API Tvg_Paint* tvg_text_new(void);


/**
* \brief Устанавливает свойства шрифта для текста.
*
* Эта функция позволяет вам определить характеристики шрифта, используемые для рендеринга текста.
* Он устанавливает имя шрифта, его размер и, при необходимости, стиль.
*
* \param[in] Paint Указатель Tvg_Paint на текстовый объект.
* \param[in] name Имя шрифта. Это должно соответствовать шрифту, доступному на холсте.
* \param[in] size Размер шрифта в пунктах.
* \param[in] style Стиль шрифта. Если пусто, используется стиль по умолчанию. В настоящее время поддерживается только курсив.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT \c nullptr передается в качестве аргумента рисования \p.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION Указанное имя \p не найдено.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_text_set_font(Tvg_Paint* paint, const char* name, float size, const char* style);


/**
* \brief Назначает отображаемый текст в Юникоде.
*
* Эта функция устанавливает текст в Юникоде, который будет отображаться системой рендеринга.
* Текст задается в соответствии с указанным методом кодирования UTF, который по умолчанию равен UTF -8.
*
* \param[in] Paint Указатель Tvg_Paint на текстовый объект.
* \param[in] text Многобайтовый текст, закодированный строкой utf8 для отображения.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT \c nullptr передается в качестве аргумента рисования \p.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_text_set_text(Tvg_Paint* paint, const char* text);


/**
* \brief Устанавливает сплошной цвет текста.
*
* \param[in] Paint Указатель Tvg_Paint на текстовый объект.
* \param[in] r Значение канала красного цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
* \param[in] g Значение канала зеленого цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
* \param[in] b Значение канала синего цвета в диапазоне [0 ~ 255]. Значение по умолчанию — 0.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT \c nullptr передается в качестве аргумента рисования \p.
*
* \note Применяется либо сплошной цвет, либо градиентная заливка, в зависимости от того, что было установлено последним.
* \см. tvg_text_set_font ()
*
* \с 0,15
*/
TVG_API Tvg_Result tvg_text_set_fill_color(Tvg_Paint* paint, uint8_t r, uint8_t g, uint8_t b);


/**
* \brief Устанавливает градиентную заливку текста.
*
* \param[in] Paint Указатель Tvg_Paint на текстовый объект.
* \param[in] grad Линейная или радиальная градиентная заливка.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT \c nullptr передается в качестве аргумента рисования \p.
* \retval TVG_RESULT_MEMORY_CORRUPTION Неверный указатель Tvg_Gradient.
*
* \note Применяется либо сплошной цвет, либо градиентная заливка, в зависимости от того, что было установлено последним.
* \см. tvg_text_set_font ()
*
* \с 0,15
*/
TVG_API Tvg_Result tvg_text_set_gradient(Tvg_Paint* paint, Tvg_Gradient* gradient);

/**
* \brief Загружает данные масштабируемого шрифта из файла.
*
* ThorVG эффективно кэширует загруженные данные, используя указанный путь \p в качестве ключа.
* Это означает, что повторная загрузка того же файла не приведет к дублированию операций;
* вместо этого ThorVG будет повторно использовать ранее загруженные данные шрифта.
*
* \param[in] путь Путь к файлу шрифта.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передан недопустимый путь \p.
* \retval TVG_RESULT_NOT_SUPPORTED При попытке загрузить файл с неизвестным расширением.
*
* \см. tvg_font_unload ()
*
* \с 0,15
*/
TVG_API Tvg_Result tvg_font_load(const char* path);


/**
* \brief Загружает данные масштабируемого шрифта из блока памяти заданного размера.
*
* ThorVG эффективно кэширует загруженные данные шрифта, используя указанное имя \p в качестве ключа.
* Это означает, что повторная загрузка тех же шрифтов не приведет к дублированию операций.
* Вместо этого ThorVG будет повторно использовать ранее загруженные данные шрифта.
*
* \param[in] name Имя, под которым шрифт будет храниться и доступен (например, в \p tvg_text_set_font API ).
* \param[in] data Указатель на область памяти, где хранится содержимое данных шрифта.
* \param[in] size Размер памяти в байтах, занимаемой данными @p.
* \param[in] mimetype Mime-тип или расширение данных шрифта. Если указано \c NULL или пустое значение "", загрузчик будет определен автоматически.
* \param[in] copy Если @c true, данные копируются в локальный буфер движка, в противном случае — нет (по умолчанию).
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Если имя не указано или размер \p равен нулю, а данные \p указывают на допустимую ячейку памяти.
* \retval TVG_RESULT_NOT_SUPPORTED При попытке загрузить файл с неизвестным расширением.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION При попытке выгрузить данные шрифта, которые не были загружены ранее.
*
* \предупреждение: ответственность за освобождение памяти данных \p лежит на пользователе.
*
* \note Чтобы выгрузить данные шрифта, загруженные с помощью этого API , передайте правильное имя \p и \c nullptr в качестве данных \p.
*
* \с 0,15
*/
TVG_API Tvg_Result tvg_font_load_data(const char* name, const char* data, uint32_t size, const char *mimetype, bool copy);


/**
* \brief Выгружает указанные ранее загруженные данные масштабируемого шрифта.
*
* Эта функция используется для освобождения ресурсов, связанных с файлом шрифта, загруженным в память.
*
* \param[in] путь Путь к загруженному файлу шрифта.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION Загрузчик не инициализирован.
*
* \note Если данные шрифта в данный момент используются, они не будут немедленно выгружены.
* \см. tvg_font_load ()
*
* \с 0,15
*/
TVG_API Tvg_Result tvg_font_unload(const char* path);


/** \} */   // конец защитной группы ThorVGCapi_Text


/**
* \defgroup ThorVGCapi_Saver Заставка
* \brief Модуль для экспорта объекта рисования в указанный файл.
*
* Модуль позволяет сохранять составленную сцену и/или изображение с объекта рисования.
* После успешного экспорта в файл его можно воссоздать с помощью модуля «Изображение».
*
* \{
*/

/************************************************************************/
/* Заставка API                                                            */
/************************************************************************/
/*!
* \brief Создает новый объект Tvg_Saver.
*
* \return Новый объект Tvg_Saver.
*/
TVG_API Tvg_Saver* tvg_saver_new(void);


/*!
* \brief Экспортирует заданные данные рисования @p в заданный путь @p.
*
* Если модуль сохранения поддерживает какой-либо механизм сжатия, он оптимизирует размер данных.
* В некоторых случаях это может повлиять на время кодирования/декодирования. Вы можете отключить сжатие
* если вы хотите оптимизировать скорость.
*
* \param[in] saver Объект Tvg_Saver, связанный с задачей сохранения.
* \param[in] краска Краска, которую нужно сохранить, со всеми связанными с ней свойствами.
* \param[in] путь Путь к файлу, в котором должны быть сохранены данные рисования.
* \param[in] compress Если @c true, то сжимайте данные, если это возможно.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передается нулевой параметр @c.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION В настоящее время сохраняются другие ресурсы.
* \retval TVG_RESULT_NOT_SUPPORTED Пытаюсь сохранить файл с неизвестным расширением или в неподдерживаемом формате.
* \retval TVG_RESULT_UNKNOWN Необходимо сохранить пустую краску.
*
* \note Сохранение может быть асинхронным, если назначенный номер потока больше нуля. Чтобы гарантировать, что сохранение выполнено, после этого вызовите tvg_saver_sync().
* \см. tvg_saver_sync ()
*/
TVG_API Tvg_Result tvg_saver_save(Tvg_Saver* saver, Tvg_Paint* paint, const char* path, bool compress);


/*!
* \brief Гарантирует, что задача сохранения завершена.
*
* Поведение модуля Saver работает на основе синхронизации/асинхронности, в зависимости от настройки потоков инициализатора.
* Таким образом, если вы хотите получить от этого выгоду, вы должны вызвать tvg_saver_sync() после tvg_saver_save() в нужное время задержки.
* В противном случае вы можете немедленно вызвать tvg_saver_sync().
*
* \param[in] saver Объект Tvg_Saver, связанный с задачей сохранения.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В качестве аргумента передается нулевой параметр @c.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION Задача сохранения не выполняется.
*
* \note Асинхронное выполнение задач зависит от реализации модуля Saver.
* \см. tvg_saver_save ()
*/
TVG_API Tvg_Result tvg_saver_sync(Tvg_Saver* saver);


/*!
* \brief Удаляет указанный объект Tvg_Saver.
*
* \param[in] saver Объект Tvg_Saver, который необходимо удалить.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Saver.
*/
TVG_API Tvg_Result tvg_saver_del(Tvg_Saver* saver);


/** \} */   // конец защитной группы ThorVGCapi_Saver


/**
* \defgroup ThorVGCapi_Animation Анимация
* \brief Модуль для работы с анимационными изображениями.
*
* Модуль поддерживает отображение и управление кадрами анимации.
*
* \{
*/

/************************************************************************/
/* Анимация API                                                        */
/************************************************************************/

/*!
* \brief Создает новый объект Animation.
*
* \return Tvg_Animation Новый объект Tvg_Animation.
*
* \с 0,13
*/
TVG_API Tvg_Animation* tvg_animation_new(void);


/*!
* \brief Указывает текущий кадр анимации.
*
* \param[in] анимация Указатель Tvg_Animation на объект анимации.
* \param[in] no Индекс отображаемого кадра анимации. Индекс должен быть меньше tvg_animation_get_total_frame().
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Animation.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION, если заданный номер @p совпадает со значением текущего кадра.
* \retval TVG_RESULT_NOT_SUPPORTED Данные изображения не поддерживают анимацию.
*
* \note В целях эффективности ThorVG игнорирует обновления нового значения кадра, если оно отличается от текущего значения кадра.
*       меньше 0,001. В таких случаях он возвращает @c Result::InsufficientCondition.
*       Значения меньше 0,001 могут быть проигнорированы и не могут быть точно сохранены анимацией.
* \см. tvg_animation_get_total_frame ()
*
* \с 0,13
*/
TVG_API Tvg_Result tvg_animation_set_frame(Tvg_Animation* animation, float no);


/*!
* \brief Извлекает экземпляр изображения, связанный с этим экземпляром анимации.
*
* Эта функция обеспечивает доступ к экземпляру изображения, который можно использовать для загрузки форматов анимации, таких как Lottie(json).
* После настройки изображения его можно переместить на назначенный холст, что позволит управлять кадрами анимации.
* с этим экземпляром анимации.
*
* \param[in] анимация Указатель Tvg_Animation на объект анимации.
*
* \return Экземпляр изображения, привязанный к этой анимации.
*
* \warning Экземпляр изображения принадлежит компании Animation. Его не следует удалять вручную.
*
* \с 0,13
*/
TVG_API Tvg_Paint* tvg_animation_get_picture(Tvg_Animation* animation);


/*!
* \brief Получает текущий номер кадра анимации.
*
* \param[in] анимация Указатель Tvg_Animation на объект анимации.
* \param[in] no Номер текущего кадра анимации от 0 до totalFrame() - 1.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Animation или @p нет
*
* \см. tvg_animation_get_total_frame ()
* \см. tvg_animation_set_frame ()
*
* \с 0,13
*/
TVG_API Tvg_Result tvg_animation_get_frame(Tvg_Animation* animation, float* no);


/*!
* \brief Получает общее количество кадров в анимации.
*
* \param[in] анимация Указатель Tvg_Animation на объект анимации.
* \param[in] cnt Общее количество кадров в анимации.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Animation или @p cnt.
*
* \note Нумерация кадров начинается с 0.
* \note Если изображение настроено неправильно, эта функция вернет 0.
*
* \с 0,13
*/
TVG_API Tvg_Result tvg_animation_get_total_frame(Tvg_Animation* animation, float* cnt);


/*!
* \brief Получает продолжительность анимации в секундах.
*
* \param[in] анимация Указатель Tvg_Animation на объект анимации.
* \param[in] длительность Продолжительность анимации в секундах.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Недопустимый указатель Tvg_Animation или длительность @p.
*
* \note Если изображение настроено неправильно, эта функция вернет 0.
*
* \с 0,13
*/
TVG_API Tvg_Result tvg_animation_get_duration(Tvg_Animation* animation, float* duration);


/*!
* \brief Определяет сегмент воспроизведения анимации.
*
* \param[in] анимация Указатель Tvg_Animation на объект анимации.
* \param[in] начало сегмента.
* \param[in] конец сегмента конец.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION Если анимация не загружена.
* \retval TVG_RESULT_INVALID_ARGUMENT Когда данные параметры выходят за пределы допустимого диапазона.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_animation_set_segment(Tvg_Animation* animation, float begin, float end);


/*!
* \brief Получает текущий сегмент.
*
* \param[in] анимация Указатель Tvg_Animation на объект анимации.
* \param[out] начало сегмента начало.
* \param[out] конец сегмента конец.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION Если анимация не загружена.
* \retval TVG_RESULT_INVALID_ARGUMENT Если заданы параметры @c nullptr.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_animation_get_segment(Tvg_Animation* animation, float* begin, float* end);


/*!
* \brief Удаляет указанный объект Tvg_Animation.
*
* \param[in] анимация Объект Tvg_Animation, который необходимо удалить.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT Неверный указатель Tvg_Animation.
*
* \с 0,13
*/
TVG_API Tvg_Result tvg_animation_del(Tvg_Animation* animation);


/** \} */   // конец защитной группы ThorVGCapi_Animation


/**
* \defgroup Аксессор ThorVGCapi_Accesssor
* \brief Модуль для манипуляций с деревом сцены
*
* Этот модуль помогает управлять деревом сцен.
* \{
*/

/************************************************************************/
/* Аксессуар API                                                         */
/************************************************************************/

/*!
* \brief Генерирует уникальный ID (хэш-ключ) по заданному имени.
*
* Эта функция вычисляет уникальное значение идентификатора на основе предоставленной строки.
* Вы можете использовать это, чтобы назначить уникальный ID объекту Paint.
*
* \param[in] name Входная строка, на основе которой создается уникальный идентификатор.
*
* \return Сгенерированное значение уникального идентификатора.
*
* \note Экспериментальный API
*/
TVG_API uint32_t tvg_accessor_generate_id(const char* name);


/** \} */   // конец защитной группы ThorVGCapi_Accessor


/**
* \defgroup ThorVGCapi_LottieAnimation LottieAnimation
* \brief Модуль для управления функциями расширения лотереи.
*
* Модуль позволяет управлять расширенными функциями Lottie.
* \{
*/

/************************************************************************/
/* Расширение LottieAnimation API                                        */
/************************************************************************/

/*!
* \brief Создает новый объект LottieAnimation.
*
* \return Tvg_Animation Новый объект Tvg_LottieAnimation.
*
* \с 0,15
*/
TVG_API Tvg_Animation* tvg_lottie_animation_new(void);


/*!
* \brief Переопределите свойства лотереи с помощью данных слота.
*
* \param[in] анимация Объект Tvg_Animation для переопределения свойства со слотом.
* \param[in] slot Данные слота Lottie в формате JSON или @c nullptr для сброса.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION Если анимация не загружена.
* \retval TVG_RESULT_INVALID_ARGUMENT Когда данный слот @p недействителен
* \retval TVG_RESULT_NOT_SUPPORTED Анимация Лотти не поддерживается.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_lottie_animation_override(Tvg_Animation* animation, const char* slot);


/*!
* \brief Определяет сегмент по маркеру.
*
* \param[in] анимация Указатель Tvg_Animation на объект анимации Lottie.
* \param[in] маркер Имя маркера сегмента.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION Если анимация не загружена.
* \retval TVG_RESULT_INVALID_ARGUMENT Когда данный маркер @p недействителен.
* \retval TVG_RESULT_NOT_SUPPORTED Анимация Лотти не поддерживается.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_lottie_animation_set_marker(Tvg_Animation* animation, const char* marker);


/*!
* \brief Получает количество маркеров анимации.
*
* \param[in] анимация Указатель Tvg_Animation на объект анимации Lottie.
* \param[out] cnt Значение счетчика маркеров.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В случае, если в качестве аргумента передается нулевой параметр @c.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_lottie_animation_get_markers_cnt(Tvg_Animation* animation, uint32_t* cnt);


/*!
* \brief Получает имя маркера по заданному индексу.
*
* \param[in] анимация Указатель Tvg_Animation на объект анимации Lottie.
* \param[in] idx Индекс маркера анимации, начинается с 0.
* \param[out] name Имя маркера в случае успеха.
*
* \return Перечисление Tvg_Result.
* \retval TVG_RESULT_INVALID_ARGUMENT В случае, если в качестве аргумента передан @c nullptr или @c idx выходит за пределы диапазона.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_lottie_animation_get_marker(Tvg_Animation* animation, uint32_t idx, const char** name);


/** \} */   // конец добавления в группу ThorVGCapi_LottieAnimation


/** \} */   // конец защитной группы ThorVGCapi


#ifdef __cplusplus
}
#endif

#endif //_THORVG_CAPI_H_

#endif /* LV_USE_THORVG_INTERNAL */

