/*!
* \файл thorvg_capi.h
*
* \brief Модуль обеспечивает привязки C для библиотеки ThorVG.
* Пожалуйста, обратитесь к src/examples/Capi.cpp, чтобы найти образцы использования thorvg_capi.
*
* Модульthorvg_capiпозволяет реализовать клиенту ThorVG и обеспечивает
* следующие функциональные возможности:
* - рисование фигур: линия, кривая, многоугольник, круг, определяемые пользователем, ...
* - заполнение: сплошное, линейное и радиальное градиентное
* - граф сцены и аффинное преобразование (перенос, вращение, масштабирование,...)
* - обводка: ширина, соединение, заглавие, тире
* - композиция: смешивание, маскирование, обрезка пути
* - картинки: SVG, PNG, JPG, растровое изображение
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
* \briefAPIпривязки ThorVG к языку C.
*
* \{
*/


/**
* \brief Структура, отвечающая за управление графическими элементами и их отрисовку.
*
* Он устанавливает ограничитель, который можно отобразить на экране. Он хранит объектыTvg_Paint(Форма, Сцена, Изображение).
*/
typedef struct _Tvg_Canvas Tvg_Canvas;


/**
* \brief Структура, представляющая графический элемент.
*
* \Предупреждение Объекты TvgPaint не может использоваться совместно между холстами.
*/
typedef struct _Tvg_Paint Tvg_Paint;


/**
* \brief Структура, представляющая градиентную заливку объектаTvg_Paint.
*/
typedef struct _Tvg_Gradient Tvg_Gradient;


/**
* \brief Структура, представляющая объект, способная сохранить объектTvg_Paintв файле.
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
 * \brief Перечисление, определяющее результатAPI.
 *
 * ВсеAPIThorVG могут возвращать одни результаты измерений в список.
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
 * \brief Перечисление, указывающий метод, соотношение в составе двух объектов - целевого и исходного.
 *
 * \ingroup ThorVGCapi_Paint
 */
typedef enum {
    TVG_COMPOSITE_METHOD_NONE = 0,           ///< Композиция не применяется.
    TVG_COMPOSITE_METHOD_CLIP_PATH,          ///< Определяется пересечение источника и цели, и визуализируются только результирующие пиксели из источника. Обратите внимание, что ClipPath поддерживает только тип Shape.  @deprecatedВместо этого воспользуйтесь Paint::clip().
    TVG_COMPOSITE_METHOD_ALPHA_MASK,         ///< Пиксели источника и цели альфа-смешены. В результате видна только та часть источника, которая пересекается с целью.
    TVG_COMPOSITE_METHOD_INVERSE_ALPHA_MASK, ///< Пиксели источника и дополнения к целевым пикселям альфа-смешены. В результате видна только та часть источника, которая не покрыта целью.
    TVG_COMPOSITE_METHOD_LUMA_MASK,          ///< Исходные пиксели преобразуются в оттенки серого (значение яркости), а альфа-канал смешивается с целевым. В результате видна только та часть источника, которая пересекается с целью. \с 0,9
    TVG_COMPOSITE_METHOD_INVERSE_LUMA_MASK   ///< Исходные пиксели преобразуются в оттенки серого (значение яркости), а в дополнение к целевым пикселям выполняется альфа-смешение. В результате видна только та часть источника, которая не покрыта целью. \с 0,14
} Tvg_Composite_Method;

/**
 * @brief Нумерация указывает метод, используемый для смешивания краски. Пожалуйста, обратитесь к соответствующим формулам для каждого метода.
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
    TVG_BLEND_METHOD_LIGHTEN,           ///< Только имеет действие, противоположное «Только потомнить». макс(С, Д)
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
 * \см.  Tvg_Type
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
 * Объекты рисования ThorVG может возвращать значения типов объектов, что позволяет вам определить конкретный тип каждого объекта.
 *
 * \ingroup ThorVGCapi_Paint
 *
 * \см. tvg_paint_get_type()
 * \см. tvg_gradient_get_type()
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
 * \brief Перечисление, указывающее значения командного пути, принятыхTVG.
 *
 * Не путать с командами пути из элемента пути svg (например, M, L, Q, H и другими).
 * TVG интерпретирует их все и преобразует в значения PathCommand.
 */
typedef enum {
    TVG_PATH_COMMAND_CLOSE = 0, ///< Завершает текущий подпуть и соединяет его с его начальной точкой — команда соответствует команде Z в пути svg.
    TVG_PATH_COMMAND_MOVE_TO,   ///< Устанавливает новую начальную точку подпути и новую текущую точку — сеть соответствует M команде в пути svg.
    TVG_PATH_COMMAND_LINE_TO,   ///< Рисует линию от текущей точки до заданной точки и устанавливает новое значение текущей точки — соответствует команде L в пути svg.
    TVG_PATH_COMMAND_CUBIC_TO   ///< Рисует кубическую кривую без от текущей точки до заданной точки, используя две заданные контрольные точки, и устанавливает новое значение текущей точки — соответствует цепи C в команде пути svg.
} Tvg_Path_Command;


/**
 * \brief Перечисление, определяющее тип окончания штриха в открытых подпутах.
 */
typedef enum {
    TVG_STROKE_CAP_SQUARE = 0, ///< Обводка расширяется в обеих конечных точках подпути на прямоугольник с шириной, равной ширине обводки, и длиной, равной половине ширины обводки. Для подпутей нулевой длины квадрат отображается с размером ширины штриха.
    TVG_STROKE_CAP_ROUND,      ///< Штрих расширяется в обеих конечных точках подпути на полукруг с радиусом, равным половине ширины штриха. Для подпутей нулевой длины отображается полный круг.
    TVG_STROKE_CAP_BUTT        ///< Штрих заканчивается точно в каждой из двух конечных точек подпути. Для подпутей нулевой длины штрих не отображается.
} Tvg_Stroke_Cap;


/**
 * \brief Перечисление, указывающее, как заполнить область границ границ градиента.
 */
typedef enum {
    TVG_STROKE_JOIN_BEVEL = 0, ///< Внешний угол соединяемых сегментов пути скошен в точке соединения. Треугольная область угла ограничена прямой линией между внешними углами каждого штриха.
    TVG_STROKE_JOIN_ROUND,     ///< Внешний угол соединяемых сегментов пути закруглен. Круглая область центрируется в точке соединения.
    TVG_STROKE_JOIN_MITER      ///< Внешний угол соединенных сегментов пути имеет шипы. Шип создается путем расширения за пределы точки соединения внешних краев обводки до их пересечения. Если расширение выходит за пределы ограничения, стиль соединения преобразуется в стиль «Скос».
} Tvg_Stroke_Join;


/**
 * \brief Перечисление, указывающее, как заполнить область границ границ градиента.
 */
typedef enum {
    TVG_STROKE_FILL_PAD = 0, ///< Оставшаяся область заполняется ближайшим стоп-цветом.
    TVG_STROKE_FILL_REFLECT, ///< Узор градиента отражается за пределами области градиента до тех пор, пока ожидаемая область не будет заполнена.
    TVG_STROKE_FILL_REPEAT   ///< Шаблон градиента непрерывно повторяется за пределами области градиента, пока ожидаемая область не будет заполнена.
} Tvg_Stroke_Fill;


/**
 * \brief Перечисление, определяющий алгоритм, определение для определения того, какие части структуры внутренней части фигуры.
 */
typedef enum {
    TVG_FILL_RULE_WINDING = 0, ///< Рисуется линия от точки до места за пределами фигуры. Подсчитываются пересечения линии с сегментом пути фигуры. Начиная с нуля, если сегмент пути фигуры пересекает линию по часовой стрелке, единица добавляется, в противном случае единица вычитается. Если полученная сумма не равна нулю, точка находится внутри фигуры.
    TVG_FILL_RULE_EVEN_ODD     ///< Рисуется линия от точки до места за пределами фигуры и подсчитываются ее пересечения с сегментами пути фигуры. Если количество пересечений нечетное, точка находится внутри фигуры.
} Tvg_Fill_Rule;

/** \} */   // окончание добавления в группу ThorVGCapi_Shape


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

/** \} */   // окончание добавления в группу ThorVGCapi_Gradient


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
* \brief Модуль, умеющий создавать идеи и создавать рабочие механизмы TVG.
*
* \{
*/

/************************************************************************/
/* Двигатель API                                                           */
/************************************************************************/
/*!
* \brief Инициализирует двигательTVG.
*
* TVG требует среды работающего двигателя.
* TVG использует собственный планировщик задач для эффективного распараллеливания задач рендеринга.
* Вы можете указать количество потоков, количество которых обозначается потоками @p.
* На этапе инициализации TVG будет генерировать/создавать потоки в соответствии с количеством потоков @p.
*
* \code
* tvg_engine_init (TVG_ENGINE_SW, 0);  //Инициализируемый программный рендерер и используем только основной поток
* \endcode
*
* \param[in]engine_methodТипы принципов для инициализации. Это относится к типам Canvas, в которых он будет использоваться. Для нескольких серверов разрешены побитовые операции.
*   - TVG_ENGINE_SW : растеризатор CPU.
*   - TVG_ENGINE_GL: растеризатор OpenGL (пока не поддерживается)
* \param[in] threads Количество дополнительных потоков, используемых для рендеринга. Ноль указывает, что будет использоваться только основной поток.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеизвестный тип двигателя.
* \retvalTVG_RESULT_NOT_SUPPORTEDНеподдерживаемый тип двигателя.
*
* \note Инициализатор отслеживает количество вызовов. Фиксация количества потоков производится первым вызовомinit().
* \см. tvg_engine_term()
* \см.  Tvg_Engine
*/
TVG_API Tvg_Result tvg_engine_init(Tvg_Engine engine_method, unsigned threads);


/*!
* \brief Завершает работуTVG.
*
* Его следует закрыть в случае выполнения работы клиентаTVGс теми же типами движков, которые были переданы при вызове tvg_engine_init().
*
* \code
* tvg_engine_init (TVG_ENGINE_SW, 0);
* //определяем холст и формы, обновляем формы, общие вызовы рендеринга
* tvg_engine_term (TVG_ENGINE_SW);
* \endcode
*
* \paramengine_methodТипы маркеров, которые необходимо преобразовать. Это относится к типам Canvas, в которых он будет использоваться. Для нескольких серверов разрешены побитовые операции.
*   - TVG_ENGINE_SW : растеризатор CPU.
*   - TVG_ENGINE_GL: растеризатор OpenGL (пока не поддерживается)
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INSUFFICIENT_CONDITIONНичего не нужно прекращать.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеизвестный тип двигателя.
* \retvalTVG_RESULT_NOT_SUPPORTEDНеподдерживаемый тип двигателя.
*
* \см. tvg_engine_init()
* \см.  Tvg_Engine
*/
TVG_API Tvg_Result tvg_engine_term(Tvg_Engine engine_method);


/**
* \brief Получена версия движкаTVG.
*
* \param[out] major Основной номер версии.
* \param[out] второстепенный номер версии.
* \param[out] micro Номер микро-версии.
* \param[out] version Версия движка в формате major.minor.micro или nullptr@pв случае внутренней ошибки.
*
* \return ПеречислениеTvg_Result.
* \ретвал TVG_RESULT_SUCCESS .
*
* \с 0,15
*/
TVG_API Tvg_Result tvg_engine_version(uint32_t* major, uint32_t* minor, uint32_t* micro, const char** version);

/** \} */   // конец защитной группы ThorVGCapi_Initializer


/**
* \defgroupThorVGCapi_CanvasХолст
* \brief Модуль для управления и рисования графических элементов.
*
* Холст — это объект, связанный с рисованием цели. Он настраивает механизм рисования и буфер, которым можно рисовать на экране. Он также управляет заданными объектами Paint.
*
* \note Поведение холста зависит от растрового движка, хотя, по мнению экспертов, окончательное определение буфера будет индивидуальным.
* \предупреждение объектов Краска, применение одного холста, не может быть использована несколькими холстами.
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
    TVG_COLORSPACE_ABGR8888S,    ///< Каналы соединяются в порядке: альфа, синий, зеленый, красный. Цвета не умножаются по альфа-каналу.  @since0.13
    TVG_COLORSPACE_ARGB8888S     ///< Каналы соединяются в порядке: альфа, красный, зеленый, синий. Цвета не умножаются по альфа-каналу.  @since0.13
} Tvg_Colorspace;


/*!
* \brief Создает объект Canvas.
*
* \code
* Tvg_Canvas *canvas =NULL;
*
* tvg_engine_init (TVG_ENGINE_SW, 4);
* canvas = tvg_swcanvas_create ();
*
* //настраиваем буфер холста
* uint32_t *буфер =NULL;
* buffer = (uint32_t*) malloc(sizeof(uint32_t) * 100 * 100);
* if (!buffer) return;
*
* tvg_swcanvas_set_target (холст, буфер, 100, 100, 100,TVG_COLORSPACE_ARGB8888);
*
* //настраиваем краски и добавляем их на холст перед его рисованием
*
* tvg_canvas_destroy (холст);
* tvg_engine_term (TVG_ENGINE_SW);
* \endcode
*
* \return Новый объектTvg_Canvas.
*/
TVG_API Tvg_Canvas* tvg_swcanvas_create(void);


/*!
* \brief Устанавливает буфер в процессе растеризации и устанавливает используемое цветовое пространство.
*
* По соображениям оптимизации TVG не выделяет память для выходного буфера самостоятельно.
* Буфер желаемого размера должен быть выделен и принадлежать вызывающей стороне.
*
* \param[in] Canvas ОбъектTvg_Canvas, управляющий буфером@p.
* \param[in] buffer Указатель выделенного блока памяти размером@pшаг x@ph.
* \param[in] шаг Шаг растрового изображения – в большинстве случаев то же самое значение, что и@pw.
* \param[in] Ширина растрового изображения.
* \param[in] h Высота растрового изображения.
* \param[in] cs Значение цветового пространства, определяющее способ чтения/записи 32-битных цветов.
* - TVG_COLORSPACE_ABGR8888
* - TVG_COLORSPACE_ARGB8888
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTSПередан недопустимый указатель холста или буфера, или один из шагов@p,@pw или@ph считается результатом.
* \retvalTVG_RESULT_INSUFFICIENT_CONDITION, если холст осуществит рендеринг. Убедитесь, что сеть синхронизирована.
* \retvalTVG_RESULT_NOT_SUPPORTEDПрограммный движение не соответствует.
*
* \Предупреждение Не используйте буфер@pво время выполненияtvg_canvas_draw() -tvg_canvas_sync(). К нему не следует обращаться, пока движок пишет на нем.
*
* \см.  Tvg_Colorspace
*/
TVG_API Tvg_Result tvg_swcanvas_set_target(Tvg_Canvas* canvas, uint32_t* buffer, uint32_t stride, uint32_t w, uint32_t h, Tvg_Colorspace cs);


/*!
* \brief Устанавливает политику поведения пула памяти программного ядра.
*
* ThorVG рисует множество фигур, популярных/освобождает часть памяти.
* во время обработки рендеринга. Он внутренне использует один общий пул памяти.
* который можно повторно использовать среди холстов, чтобы избежать накладных расходов на память.
*
* Таким образом, ThorVG предлагает использовать политику пула памяти для удовлетворения требований пользователей.
* если ему необходимо гарантировать потокобезопасность доступа к внутренним данным.
*
* \param[in] Canvas ObjectTvg_Canvas, для которого необходимо проведение пула памяти.
* \param[in] policy Метод, определяющий поведение пула памяти. Значение по умолчанию —@cTVG_MEMPOOL_POLICY_DEFAULT .
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTSПередан неверный указатель холста.
* \retvalTVG_RESULT_INSUFFICIENT_CONDITIONНа холсте уже есть краски.
* \retvalTVG_RESULT_NOT_SUPPORTEDПрограммный движение не соответствует.
*
* \note Когда для политики@cбыло установлено значение@cTVG_MEMPOOL_POLICY_INDIVIDUAL , нынешняя область видимости использует свой простор
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
* статическийTvg_Canvas*canvas =NULL;
* статическийuint32_t*buffer =NULL;
*
* статическая пустота_init(){
*   canvas = tvg_swcanvas_create ();
*   buffer = (uint32_t*) malloc(sizeof(uint32_t) * 100 * 100);
*   tvg_swcanvas_set_target (холст, буфер, 100, 100, 100,TVG_COLORSPACE_ARGB8888);
* }
*
* //задача, вызываемая из основной функции в цикле
* static void _job(const int cmd) {
*   //определяем допустимую форму прямоугольника
*   переключатель (команда) {
*     случай CMD_EXIT: вернуть 0;
*     случай CMD_ADD_RECT :
*       tvg_canvas_push (холст, шоколад);
*       перерыв;
*     случай CMD_DEL_RECT :
*       tvg_paint_del (прямой);
*       //теперь для безопасного удаленияTvg_Canvasнеобходимо использоватьtvg_canvas_clear() API
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
*      stop = _job (CMD);
*   }
*   tvg_canvas_clear (холст, ложь);
*   tvg_canvas_destroy (холст);
*   tvg_engine_term (TVG_ENGINE_SW);
*   вернуть 0;
* }
*
* tvg_canvas_destroy (холст);
* tvg_engine_term (TVG_ENGINE_SW)
* \endcode
*
* \param[in] Canvas ОбъектTvg_Canvas, который необходимо удалить.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTПередан недопустимый указатель на объектTvg_Canvas.
*
* \note Если краски с холста не должны выделяться, следует вызвать функциюtvg_canvas_clear() со значением аргумента @c, установленным в@cfalse.
* Имейте в виду, что в таком случае TVG больше не несет ответственности за выпуск красок, и это необходимо делать вручную во избежание утечек памяти.
*
* \см. tvg_paint_del(),tvg_canvas_clear()
*/
TVG_API Tvg_Result tvg_canvas_destroy(Tvg_Canvas* canvas);


/*!
* \brief Вставляет элемент рисунка на холст, используя объектTvg_Paint.
*
* \param[in] холст ОбъектTvg_Canvas, управляющий отрисовкой@p.
* \param[in] Paint ОбъектTvg_Paint, который нужно нарисовать.
*
* Только краски, нанесенные на холст, будут целями рисования.
* Они располагаются на холсте до тех пор, пока вы не вызоветеtvg_canvas_clear().
*
* \returnTvg_Resultвозвращаемые значения:
* \retvalTVG_RESULT_INVALID_ARGUMENTВ случае, если в качестве аргумента применяется нулевой параметр@c.
* \retvalTVG_RESULT_INSUFFICIENT_CONDITIONВнутренняя ошибка.
*
* \note Порядок отрисовки красок такой же, как и порядок их перемещения. Если вы собираетесь использовать слои, выберите возможность сортировки красок перед темой, как их раскладывать.
* \см. tvg_canvas_clear()
*/
TVG_API Tvg_Result tvg_canvas_push(Tvg_Canvas* canvas, Tvg_Paint* paint);


/*!
* \brief Резервирует блок памяти, в котором хранятся объекты, размещенные на холсте.
*
* Если известно заранее количествоTvg_Paints, которое будет сохранено на холсте, вызов этой функции уменьшит небольшое число.
* Таким образом, распределение памяти повышает производительность.
*
* \code
* Tvg_Canvas *canvas =NULL;
*
* tvg_engine_init (TVG_ENGINE_SW, 4);
* canvas = tvg_swcanvas_create ();
*
* uint32_t *буфер =NULL;
* buffer = (uint32_t*) malloc(sizeof(uint32_t) * 100 * 100);
* if (!buffer) return;
*
* tvg_swcanvas_set_target (холст, буфер, 100, 100, 100,TVG_COLORSPACE_ARGB8888);
*
* tvg_canvas_destroy (холст);
* tvg_engine_term (TVG_ENGINE_SW)
* \endcode
*
* \param[in] Canvas ОбъектTvg_Canvas, управляющий зарезервированной памятью.
* \param[in] n Число объектов, для которых должна быть зарезервирована память.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Canvas.
*/
TVG_DEPRECATED TVG_API Tvg_Result tvg_canvas_reserve(Tvg_Canvas* canvas, uint32_t n);


/*!
* \brief Устанавливает общее количество красок, попавших на холст, в равных условиях.
* ОбъектыTvg_Paint, хранящиеся на холсте, освобождаются, если для параметра@pfree установлено значение@ctrue, в противном случае память не освобождается и не освобождается.
* все краски следует освобождать вручную во избежание утечек памяти.
*
* \param[in] Canvas ОбъектTvg_Canvas, который необходимо сохранить.
* \param[in] free Если@ctrue, память, занятая красками, освобождается, в случае отказа — нет.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Canvas.
*
* \см. tvg_canvas_destroy()
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
* статическийTvg_Canvas*canvas =NULL;
* статическийTvg_Paint*rect =NULL;
*
* int_frame_render(недействительный) {
*   tvg_canvas_update (холст);
*   tvg_canvas_draw (холст);
*   tvg_canvas_sync (холст);
* }
*
* //обработчик событий из вашего кода или сторонней библиотеки
* void_event_handler(событие *event_data) {
*   если (! event_data) вернуть NULL;
*     переключатель(event_data.type) {
*       случай EVENT_RECT_ADD :
*         если (!прямо) {
*           tvg_shape_append_rect (прямоугольник, 10, 10, 50, 50, 0, 0);
*           tvg_shape_set_stroke_width (прямоугольный, 1.0f);
*           tvg_shape_set_stroke_color (прямоугольный, 255, 0, 0, 255);
*           tvg_canvas_push (холст, шоколад);
*         }
*         перерыв;
*       случай EVENT_RECT_MOVE :
*         если (прямой)tvg_paint_translate(прямой, 10.0, 10.0);
*           перерыв;
*         по умолчанию:
*           перерыв;
*   }
* }
*
* int main(int argc, char **argv) {
*   //пример обработчика из вашего кода или сторонней библиотеки
*   event_handler_add (обработчик,_event_handler);
*
*   //создаем процесс рендеринга кадра, который вызывает функцию _frame_render().
*   app_loop_begin (_frame_render);
*   app_loop_finish ();
*   cleanup() ;
* }
* \endcode
*
* \param[in] Canvas ОбъектTvg_Canvas, который необходимо обновить.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Canvas.
*
* \см. tvg_canvas_update_paint()
*/
TVG_API Tvg_Result tvg_canvas_update(Tvg_Canvas* canvas);


/*!
* \brief Обновляет данный объектTvg_Paintс холста перед рендерингом.
*
* Если клиентское приложение использует библиотекуTVG, не обновляется весь холст с помощьюtvg_canvas_update() в кадре.
* В процессе рендеринга объектовTvg_Paint, ранее добавленных на холст, следует обновлять вручную с помощью этой функции.
*
* \param[in] холст ОбъектTvg_Canvas, что принадлежит краска@p.
* \param[in] Paint ObjectTvg_Paint, который необходимо обновить.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ случае, если в качестве аргумента применяется нулевой параметр@c.
*
* \см. tvg_canvas_update()
*/
TVG_API Tvg_Result tvg_canvas_update_paint(Tvg_Canvas* canvas, Tvg_Paint* paint);


/*!
* \brief Запрашивает холст для рисования объектовTvg_Paint.
*
* Все краски с данного холста будут растрированы в буфер.
*
* \param[in] Canvas ObjectTvg_Canvas, элемент для рисования.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Canvas.
*
* \note Рисование может быть асинхронным в зависимости от назначенного номера резьбы. Чтобы создать, что рисунок выполнен, после этого вызовите tvg_canvas_sync().
* \см. tvg_canvas_sync()
*/
TVG_API Tvg_Result tvg_canvas_draw(Tvg_Canvas* canvas);


/*!
* \brief Гарантирует, что процесс рисования завершен.
*
* поскольку рендеринг холста может выполняться асинхронно, его следует переходить послеtvg_canvas_draw().
*
* \param[in] Canvas Объект Tvg_Canvas, контурные нарисованные элементы.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Canvas.
* \retvalTVG_RESULT_INSUFFICIENT_CONDITION@p Холст либо уже находится в состоянии синхронизации, либо в поврежденном состоянии (перед синхронизацией требуется отрисовка).
*
* \см. tvg_canvas_draw()
*/
TVG_API Tvg_Result tvg_canvas_sync(Tvg_Canvas* canvas);


/*!
* \brief Устанавливает область рисования на холсте.
*
* Эта функция определяет прямоугольную область холста, которая будет использоваться для операций рисования.
* Указанный область просмотра используется для обрезки вывода рендеринга по границам прямоугольника.
*
* \param[in] Canvas Объект Tvg_Canvas, контурные нарисованные элементы.
* \param[in] x Координата X верхнего левого угла поворота.
* \param[in] y Координата Y верхнего левого угла угла.
* \param[in] w Ширина прямоугольника.
* \param[in] h Высота фигуры.
*
* \return ПеречислениеTvg_Result.
*
* \предупреждение Не разрешено изменять область просмотра во времяtvg_canvas_update() -tvg_canvas_sync() илиtvg_canvas_push() -tvg_canvas_sync().
*
* \note При сбросе целевого размера область просмотра также будет сброшена до целевого размера.
* \см. tvg_swcanvas_set_target()
* \с 0,15
*/
TVG_API Tvg_Result tvg_canvas_set_viewport(Tvg_Canvas* canvas, int32_t x, int32_t y, int32_t w, int32_t h);

/** \} */   // конец защитной группы ThorVGCapi_Canvas


/**
* \defgroupThorVGCapi_PaintКраска
* \brief Модуль для управления графическими элементами. Это позволяет дублировать, трансформировать и композицию.
*
* \{
*/

/************************************************************************/
/* Краска API                                                            */
/************************************************************************/
/*!
* \brief Освобождает данный объектTvg_Paint.
*
* \code
* //пример функции очистки
* Tvg_Paint *rect =NULL; //прямоугольная форма добавлена в другую функцию
*
* //удаляем прямоугольник API
* intrectangle_delete(недействительный) {
*   если (прямой)tvg_paint_del(прямой);
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
* \param[in] Paint ОбъектTvg_Paint, который необходимо загрузить.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*
* \warning Если используется эта функция, во избежание непредвиденного поведения следует использоватьtvg_canvas_clear() со значением аргумента@c, установленным в@cfalse.
*
* \см. tvg_canvas_clear(),tvg_canvas_destroy()
*/
TVG_API Tvg_Result tvg_paint_del(Tvg_Paint* paint);


/*!
* \brief Масштабирует данный объектTvg_Paintс заданным коэффициентом.
*
* \param[in] Paint ObjectTvg_Paint, который нужно масштабировать.
* \param[in] коэффициент Значение коэффициента масштабирования. Значение по умолчанию — 1.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
* \retvalTVG_RESULT_INSUFFICIENT_CONDITIONв случае применения специального конвертера.
*
* \см. tvg_paint_set_transform()
*/
TVG_API Tvg_Result tvg_paint_scale(Tvg_Paint* paint, float factor);


/*!
* \brief Поворачивает заданныйTvg_Paintна заданный угол.
*
* Угол измеряется по часовой стрелке от горизонтальной оси.
* Ось вращения проходит через точку на объекте с нулевыми координатами.
*
* \param[in] Paint ОбъектTvg_Paint, который нужно повернуть.
* \param[in] степень Значение угла поворота в градусах.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
* \retvalTVG_RESULT_INSUFFICIENT_CONDITIONв случае применения специального конвертера.
*
* \см. tvg_paint_set_transform()
*/
TVG_API Tvg_Result tvg_paint_rotate(Tvg_Paint* paint, float degree);


/*!
* \brief Перемещает заданныйTvg_Paintв двумерном пространстве.
*
* Начало системы координат находится в верхнем левом углу холста.
* Горизонтальная и вертикальная оси указывают вправо и вниз соответственно.
*
* \param[in] Paint ObjectTvg_Paint, который необходимо сдвинуть.
* \param[in] x Значение горизонтального сдвига.
* \param[in] y Значение вертикального сдвига.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
* \retvalTVG_RESULT_INSUFFICIENT_CONDITIONв случае применения специального конвертера.
*
* \см. tvg_paint_set_transform()
*/
TVG_API Tvg_Result tvg_paint_translate(Tvg_Paint* paint, float x, float y);


/*!
* \brief Преобразует заданныйTvg_Paintс помощью расширенной матрицы преобразования.
*
* Ожидается получение расширенной матрицы преобразования.
*
* \param[in] Paint ObjectTvg_Paint, который необходимо изменить.
* \param[in] m Расширенная матрица 3x3.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента применяется значение@cnullptr.
*/
TVG_API Tvg_Result tvg_paint_set_transform(Tvg_Paint* paint, const Tvg_Matrix* m);


/*!
* \brief Получает матрицу аффинного преобразования данного объектаTvg_Paint.
*
* Если преобразование не применялось, возвращается единичная матрица.
*
* \param[in] Paint ObjectTvg_Paint, из которого нужно получить матрицу преобразования.
* \param[out] m Расширенная матрица 3x3.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента применяется значение@cnullptr.
*/
TVG_API Tvg_Result tvg_paint_get_transform(Tvg_Paint* paint, Tvg_Matrix* m);


/*!
* \brief Устанавливает непрозрачность, заданнуюTvg_Paint.
*
* \param[in] Paint ObjectTvg_Paint, для которого необходимо установить значение непрозрачности.
* \param[in] opacity Значение непрозрачности в пределах [0 ~ 255], где 0 — полная прозрачность, а 255 — непрозрачность.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*
* \note Установка непрозрачности с помощью этогоAPIможет производить многократного рендеринга с использованием композиции. По возможности рекомендуется соблюдать изменения непрозрачности.
*/
TVG_API Tvg_Result tvg_paint_set_opacity(Tvg_Paint* paint, uint8_t opacity);


/*!
* \brief Получает непрозрачность заданногоTvg_Paint.
*
* \param[in] Paint ObjectTvg_Paint, для которого нужно получить значение непрозрачности.
* \param[out] opacity Значение непрозрачности в пределах [0 ~ 255], где 0 — полная прозрачность, а 255 — непрозрачность.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ случае, если в качестве аргумента применяется нулевой параметр@c.
*/
TVG_API Tvg_Result tvg_paint_get_opacity(const Tvg_Paint* paint, uint8_t* opacity);


/*!
* \brief Дублирует заданный объектTvg_Paint.
*
* Создает новый объект и устанавливает все его свойства как в исходном объекте.
*
* \param[in] Paint ОбъектTvg_Paint, который необходимо скопировать.
*
* \return Скопированный объектTvg_Paintв случае успеха, в противном случае@cnullptr.
*/
TVG_API Tvg_Paint* tvg_paint_duplicate(Tvg_Paint* paint);


/*!
* \brief Получается выровненная ось, ограничивающая рамку объектаTvg_Paint.
*
* \param[in] Paint ObjectTvg_Paint, границы, которые необходимо получить.
* \param[out] x Координата X верхнего левого угла объекта.
* \param[out] y Координата Y верхнего левого угла объекта.
* \param[out] w Ширина объекта.
* \param[out] h Высота объекта.
* \param[in] Transform@ctrue, конвертировать музыку в сцену, к которой она принадлежит. В противном случае это не так.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*
* \note Это полезно, когда вам нужно определить ограничивающую рамку краски в пространстве холста.
* \note Ограничивающая рамка не указывает на фактическую область рисования. Это наименьшая сторона, заключающая объект.
* \note Если преобразование@pимеет значение@ctrue, краску необходимо перенести на холст и обновить перед вызовом этогоAPI.
* \см. tvg_canvas_update_paint()
*/
TVG_API Tvg_Result tvg_paint_get_bounds(const Tvg_Paint* paint, float* x, float* y, float* w, float* h, bool transformed);


/*!
* \brief Устанавливает композицию объекта и композицию метода.
*
* \param[in] краска Исходный объект композиции.
* \param[in] target Состав целевого объекта.
* \param[in] метод Метод, используемый для объединения исходного объекта с целевыми.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНедопустимая отрисовка@pили включение объекта@pили метода@p, необходимыйTVG_COMPOSITE_METHOD_NONE.
*/
TVG_API Tvg_Result tvg_paint_set_composite_method(Tvg_Paint* paint, Tvg_Paint* target, Tvg_Composite_Method method);


/**
* \brief Получает композицию объекта и композицию метода.
*
* \param[in] краска Исходный объект композиции.
* \param[out] target Состав целевого объекта.
* \param[out] метод Метод, используемый для объединения исходного объекта с целевыми.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента применяется значение@cnullptr.
*/
TVG_API Tvg_Result tvg_paint_get_composite_method(const Tvg_Paint* paint, const Tvg_Paint** target, Tvg_Composite_Method* method);


/*!
* \brief Вы сокращаете область рисования объекта рисования.
*
* Эта функция ограничивает область рисования объекта рисования путями указанной фигуры.
*
* \param[in] Paint Целевой объект вырезания.
* \param[in] clipper Объект формируется в качестве инструмента обрезки.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ случае, если в качестве аргумента применяется нулевой параметр@c.
* \retvalTVG_RESULT_NOT_SUPPORTEDЕсли тип клипера@pне имеет формы.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_paint_set_clip(Tvg_Paint* paint, Tvg_Paint* clipper);


/**
* \brief Получает уникальное значение примера рисования, указывающее тип экземпляра.
*
* \param[in] Paint ObjectTvg_Paint, значение типа которое необходимо получить.
* \param[out] type Уникальный тип типа примера рисования.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ случае, если в качестве аргумента применяется нулевой параметр@c.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_paint_get_type(const Tvg_Paint* paint, Tvg_Type* type);


/**
* \см. tvg_paint_get_type()
*/
TVG_DEPRECATED TVG_API Tvg_Result tvg_paint_get_identifier(const Tvg_Paint* paint, Tvg_Identifier* identifier);


/**
 * @brief Устанавливает метод смешивания для объекта рисования.
 *
 * Функция смешивания позволяет комбинировать цвета для создания визуально привлекательных эффектов, включая, среди прочего, прозрачность, освещение, затенение и смешивание цветов.
 * его процесс включает в себя сочетание цветов или изображений исходного объекта рисования с целевым (изображением нижнего слоя) с использованием операций смешивания.
 * Операция определения определяется выбранным@pBlendMethod, который определяет, как комбинируются цвета или изображения.
 *
 * \param[in] Paint ObjectTvg_Paint, для которого предусмотрен метод варьирования.
 * \param[in] метод Устанавливаемый метод перемешивания.
 *
 * \return ПеречислениеTvg_Result.
 * \retvalTVG_RESULT_INVALID_ARGUMENTВ случае, если в качестве аргумента применяется нулевой параметр@c.
 *
 * \с 0,15
 */
TVG_API Tvg_Result tvg_paint_set_blend_method(Tvg_Paint* paint, Tvg_Blend_Method method);


/** \} */   // конец защитной группы ThorVGCapi_Paint

/**
* \defgroup Форма ThorVGCapi_Shape
*
* \brief Модуль для управления двумерными фигурами и их применением.
*
* Форма имеет три основных свойства: контур фигуры, обводка, заполнение. Контур фигуры сохраняется как путь.
* Путь может быть составлен путем сбора примитивных команд, таких какtvg_shape_move_to(),tvg_shape_line_to(),tvg_shape_cubic_to(), или полных форм интерфейсов, таких какtvg_shape_append_rect(),tvg_shape_append_circle() и т. д. д.
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
* \brief Создает новую форму объекта.
*
* \return Новый объект формы.
*/
TVG_API Tvg_Paint* tvg_shape_new(void);


/*!
* \brief Сбрасывает свойства контура фигуры.
*
* Свойства цвета, заливки и обводки сохраняются.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*
* \note Память, в которой способ хранения данных, на этом этапе не освобождает эффект для кэширования.
*/
TVG_API Tvg_Result tvg_shape_reset(Tvg_Paint* paint);


/*!
* \brief Устанавливает начальную точку подпути.
*
* Значение текущей точки устанавливается в данную точку.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in] x Горизонтальная координата исходной точки подпути.
* \param[in] и Вертикальная координата исходной точки подпути.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*/
TVG_API Tvg_Result tvg_shape_move_to(Tvg_Paint* paint, float x, float y);


/*!
* \brief Добавляет новую точку к подпути, в результате чего рисуется линия от текущей точки до заданной конечной точки.
*
* Значение текущей точки устанавливается в заданную конечную точку.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in] x Горизонтальная координата конечной точки линии.
* \param[in] y Вертикальная координата конечной точки линии.

* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*
* \note Если это первая команда в пути, она соответствует вызовуtvg_shape_move_to().
*/
TVG_API Tvg_Result tvg_shape_line_to(Tvg_Paint* paint, float x, float y);


/*!
* \brief Добавляет новые точки к подпути, что приводит к рисованию кубической кривой Безье.
*
* Кривая Безье начинается в текущей точке и заканчивается в заданной конечной точке ( @p x, @p y). Две контрольные точки ( @p cx1, @p cy1) и ( @p cx2, @p cy2) используются для определения формы кривой.
* Значение текущей точки устанавливается в заданную конечную точку.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in] cx1 Горизонтальная координата первой контрольной точки.
* \param[in] cy1 Вертикальная координата первой контрольной точки.
* \param[in] cx2 Горизонтальная координата второй контрольной точки.
* \param[in] cy2 Вертикальная координата второй контрольной точки.
* \param[in] x Горизонтальная координата конечной точки кривой.
* \param[in] y Вертикальная координата конечной точки кривой.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*
* \note Если это первая команда в пути, никакие данные из пути не приводятся.
*/
TVG_API Tvg_Result tvg_shape_cubic_to(Tvg_Paint* paint, float cx1, float cy1, float cx2, float cy2, float x, float y);


/*!
* \brief Закрывает текущий вложенный путь, рисуя линию от текущих точек до исходных точек вложенного пути.
*
* Значение текущей точки устанавливается в начальную точку замкнутого подпути.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*
* \note Если подпуть не содержит точек, эта функция не работает.
*/
TVG_API Tvg_Result tvg_shape_close(Tvg_Paint* paint);


/*!
* \brief Добавляет обход к пути.
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
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in] x Горизонтальная координата верхнего левого угла угла.
* \param[in] y Вертикальная координата верхнего левого угла угла.
* \param[in] w Ширина прямоугольника.
* \param[in] h Высота фигуры.
* \param[in] rx Радиус эллипса по оси X, определяющий закругленные углы фигуры.
* \param[in] ry Радиус эллипса по оси Y, определяющий закругленные углы фигуры.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*
& \note Если@prx и@pry больше или раньше раннего@pw и раннего@ph соответственно, форма становится эллипсом.
*/
TVG_API Tvg_Result tvg_shape_append_rect(Tvg_Paint* paint, float x, float y, float w, float h, float rx, float ry);


/*!
* \brief Добавляет к пути эллипсов.
*
* Положение эллипса задается координатами его центра — аргументами @p cx и @p cy.
*
* Эллипс рассматривается как новый подпуть — он не связан с предыдущим подпутем.
*
* Значение текущей точки установлено на ( @p cx, @p cy - @p ry).
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in] cx Горизонтальная координата центра эллипса.
* \param[in] cy Вертикальная координата центра эллипса.
* \param[in] rx Радиус эллипса по оси X.
* \param[in] ry Радиус эллипса по оси Y.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*/
TVG_API Tvg_Result tvg_shape_append_circle(Tvg_Paint* paint, float cx, float cy, float rx, float ry);


/*!
* \brief Добавляет к пути дугу окружности.
*
* Дуга рассматривается как новый подпуть - она не связана с предыдущим подпутем.
* Текущее значение точки задается в конечных точках дуги, если круговая диаграмма@pимеет значение@cfalse, и в центре дуги в случае отказа.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in] cx Горизонтальная координата центра дуги.
* \param[in] cy Вертикальная координата центра дуги.
* \param[in] радиус Радиус дуги.
* \param[in] startAngle Начальный угол дуги, заданный в градусах и привязанный против часовой стрелки от горизонтальной линии.
* \param[in] развертка Центральный угол дуги, заданный в градусах и отсчитываемый против часовой стрелки от@pstartAngle.
* \param[in] pie Определяет, следует нарисовать радиусы от центра дуги до ее конечных точек — рисуется, если@cистинно.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*
* \note Установка значения развертки@pболее 360 градусов эквивалентна вызовуtvg_shape_append_circle(paint, cx, cy, radius, radius).
*/
TVG_API Tvg_Result tvg_shape_append_arc(Tvg_Paint* paint, float cx, float cy, float radius, float startAngle, float sweep, uint8_t pie);


/*!
* \brief Добавляет заданный подпуть к пути.
*
* Текущее значение точки устанавливается на последнюю точку подпути.
* Для каждой команды из cmds-массива@pнеобходимо указать количество точек в массиве@ppts.
* Если количество точек в массиве точек@pотличается от количества, требуемого массивом cmds@p, фигура с этим подпутем не будет находиться на экране Google.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in] cmds Массивная команда в подпути.
* \param[in] cmdCnt массив Длина командной команды@p.
* \param[in] pts Массив двумерных точек.
* \param[in] ptsCnt массив Длина точек@p.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENT@c nullptr, переданный в качестве аргумента, или@pcmdCnt или@pptsCnt, предпочтительный вариант.
*/
TVG_API Tvg_Result tvg_shape_append_path(Tvg_Paint* paint, const Tvg_Path_Command* cmds, uint32_t cmdCnt, const Tvg_Point* pts, uint32_t ptsCnt);


/*!
* \brief Получение значений точек пути.
*
* Функция не выделяет никаких данных, она работает с внутренней памятью. Нет необходимости освобождать массив точек @p.
*
* \code
* Tvg_Paint *форма =tvg_shape_new();
* Tvg_Point *coords =NULL;
* uint32_t лен = 0;
*
* tvg_shape_append_circle (форма, 10, 10, 50, 50);
* tvg_shape_get_path_coords (форма, (constTvg_Point**)&coords, &len);
* // TVG аппроксимирует окружность четырьмя кривыми Безье. В приведенном выше примере массив координат хранит их координаты.
* \endcode
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[out] pts Указатель на массив двумерных точек пути.
* \param[out] cnt Длина массива точек@p.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента применяется нулевой параметр@c.
*/
TVG_API Tvg_Result tvg_shape_get_path_coords(const Tvg_Paint* paint, const Tvg_Point** pts, uint32_t* cnt);


/*!
* \brief Получает данные команды пути.
*
* Функция не предлагает никаких данных. Нет необходимости освобождать массив cmds@p.
*
* \code
* Tvg_Paint *форма =tvg_shape_new();
* Tvg_Path_Command *cmds =NULL;
* uint32_t лен = 0;
*
* tvg_shape_append_circle (форма, 10, 10, 50, 50);
* tvg_shape_get_path_commands (форма, (constTvg_Path_Command**)&cmds, &len);
* //TVGаппроксимирует окружаемость собственными кривыми Безье. В приведенном выше примере массив cmds хранит данные пути данных.
* \endcode
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[out] cmds Указатель на массив команды из пути.
* \param[out] cnt Длина массива командной команды@p.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента применяется нулевой параметр@c.
*/
TVG_API Tvg_Result tvg_shape_get_path_commands(const Tvg_Paint* paint, const Tvg_Path_Command** cmds, uint32_t* cnt);


/*!
* \brief Устанавливает внешние обводки для всех фигур из краски @p.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in] Ширина Ширина обводки. Значение по умолчанию — 0.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_width(Tvg_Paint* paint, float width);


/*!
* \brief Получает дополнительные обводки фигуры.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[out] Ширина Ширина обводки.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента передан недопустимый указатель.
*/
TVG_API Tvg_Result tvg_shape_get_stroke_width(const Tvg_Paint* paint, float* width);


/*!
* \brief Устанавливает цвет обводки фигуры.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in] r Значение канала красного цвета в деталях [0 ~ 255]. Значение по умолчанию — 0.
* \param[in] g Значение зеленого цвета в отдельности [0 ~ 255]. Значение по умолчанию — 0.
* \param[in] b Значение канала синего цвета в отдельности [0 ~ 255]. Значение по умолчанию — 0.
* \param[in] a Значение альфа-канала в цепочке [0 ~ 255], где 0 — полностью прозрачный, а 255 — непрозрачный.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*
* \note Применяется либо сплошной цвет, либо градиентная заливка, в зависимости от того, что было определено окончательно.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_color(Tvg_Paint* paint, uint8_t r, uint8_t g, uint8_t b, uint8_t a);


/*!
* \brief Получает цвет обводки фигуры.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[out] r Значение канала красного цвета в единицах [0 ~ 255]. Значение по умолчанию — 0.
* \param[out] g Значение зеленого цвета в отдельности [0 ~ 255]. Значение по умолчанию — 0.
* \param[out] b Значение канала синего цвета в отдельности [0 ~ 255]. Значение по умолчанию — 0.
* \param[out] a Значение альфа-канала в цепочке [0 ~ 255], где 0 — полностью прозрачный, а 255 — непрозрачный.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
* \retvalTVG_RESULT_INSUFFICIENT_CONDITIONХод не установлен.
*/
TVG_API Tvg_Result tvg_shape_get_stroke_color(const Tvg_Paint* paint, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);


/*!
* \brief Устанавливает линейную градиентную заливку обводки для всех контуров фигуры.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in] grad Линейная градиентная заливка.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
* \retvalTVG_RESULT_MEMORY_CORRUPTIONНеверный указательTvg_Gradientили ошибка доступа к нему.
*
* \note Применяется либо сплошной цвет, либо градиентная заливка, в зависимости от того, что было определено окончательно.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_linear_gradient(Tvg_Paint* paint, Tvg_Gradient* grad);


/*!
* \brief Устанавливает радиальную градиентную заливку обводки для всех контуров фигуры.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in] grad Заливка радиальным градиентом.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
* \retvalTVG_RESULT_MEMORY_CORRUPTIONНеверный указательTvg_Gradientили ошибка доступа к нему.
*
* \note Применяется либо сплошной цвет, либо градиентная заливка, в зависимости от того, что было определено окончательно.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_radial_gradient(Tvg_Paint* paint, Tvg_Gradient* grad);


/*!
* \brief Получает градиентную заливку обводки фигуры.
*
* Функция не выделяет никакой памяти.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[out] grad Градиентная заливка.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента передан недопустимый указатель.
*/
TVG_API Tvg_Result tvg_shape_get_stroke_gradient(const Tvg_Paint* paint, Tvg_Gradient** grad);


/*!
* \brief Устанавливает образец штриха обводки фигуры.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in] DashPattern Массив последовательных парных измерений длины штриха и длины пробела.
* \param[in] cnt Размер массива DashPattern@p.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНедопустимый указатель, передаваемый в качестве аргумента, и@pcnt > 0, заданная длина массива меньше двух или любых результатов DashPattern@pравнозначно или меньше.
*
* \note Чтобы бросить образец штрихового штриха, передайте@cnullptr в@pDashPattern и ноль в@pcnt.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_dash(Tvg_Paint* paint, const float* dashPattern, uint32_t cnt);


/*!
* \brief Получается штриховой образец обводки.
*
* Функция не выделяет никакой памяти.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[out] DashPattern Массив последовательных парных значений длины штриха и длины символа.
* \param[out] cnt Размер массива DashPattern@p.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента передан недопустимый указатель.
*/
TVG_API Tvg_Result tvg_shape_get_stroke_dash(const Tvg_Paint* paint, const float** dashPattern, uint32_t* cnt);


/*!
* \brief Устанавливает стиль окончания, ориентир для обводки пути.
*
* Стиль окончания определяет форму, которая будет использоваться в конце открытых контуров с обводкой.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in] cap Значение стиля шапки. Значение по умолчанию —@cTVG_STROKE_CAP_SQUARE .
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_cap(Tvg_Paint* paint, Tvg_Stroke_Cap cap);


/*!
* \brief Получает стиль обводки, ориентир для пути обводки.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[out] cap Значение стиля шапки.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента передан недопустимый указатель.
*/
TVG_API Tvg_Result tvg_shape_get_stroke_cap(const Tvg_Paint* paint, Tvg_Stroke_Cap* cap);


/*!
* \brief Устанавливает стиль соединений для обведенных сегментов пути.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in] join Значение стиля соединения. Значение по умолчанию —@cTVG_STROKE_JOIN_BEVEL .
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_join(Tvg_Paint* paint, Tvg_Stroke_Join join);


/*!
* \brief функция получает метод соединения штрихов
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[out] join Значение стиля соединения.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента передан недопустимый указатель.
*/
TVG_API Tvg_Result tvg_shape_get_stroke_join(const Tvg_Paint* paint, Tvg_Stroke_Join* join);


/*!
* \brief Устанавливает предел хода.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in] miterlimit Ограничение на митровых накладках ограничение размера соединений штрихов, если установлен стиль соединения@cTVG_STROKE_JOIN_MITER . Значение по умолчанию — 4.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНедопустимый указательTvg_Paintили неподдерживаемые значения предела скоса@p(меньше нуля).
*
* \с 0,11
*/
TVG_API Tvg_Result tvg_shape_set_stroke_miterlimit(Tvg_Paint* paint, float miterlimit);


/*!
* \краткая функция предела получает ход митера.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[out] miterlimit Ограничение хода хода.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента передан недопустимый указатель.
*
* \с 0,11
*/
TVG_API Tvg_Result tvg_shape_get_stroke_miterlimit(const Tvg_Paint* paint, float* miterlimit);


/*!
* \brief Устанавливает обрезку обводки вдоль определенного участка пути, обеспечивая контроль, какая часть обводки видна.
*
* Если значения аргументов@pBegin и@pEnd превышают диапазон 0–1, они переносятся соответственно переносу под углом, эффективно рассматривая диапазон как круговой.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in] Begin Указывает начало сегмента, отображаемого по пути.
* \param[in] end Указывает конец сегмента, отображаемого по пути.
* \param[in] одновременно определяет, как обрезать несколько контуров внутри одной фигуры. Если установлено значение@ctrue (по умолчанию), обрезка применяется одновременно ко всем путям;
* В противном случае все пути рассматриваются как единый объект с общей длиной, равной сумме их отдельных длин, и обрезаются как таковые.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_shape_set_stroke_trim(Tvg_Paint* paint, float begin, float end, bool simultaneous);


/*!
* \brief Устанавливает сплошной цвет фигуры.
*
* Части фигуры, определенные как внутренние, окрашены.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in] r Значение канала красного цвета в деталях [0 ~ 255]. Значение по умолчанию — 0.
* \param[in] g Значение зеленого цвета в отдельности [0 ~ 255]. Значение по умолчанию — 0.
* \param[in] b Значение канала синего цвета в отдельности [0 ~ 255]. Значение по умолчанию — 0.
* \param[in] a Значение альфа-канала в цепочке [0 ~ 255], где 0 — полностью прозрачный, а 255 — непрозрачный. Значение по умолчанию — 0.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*
* \note Применяется либо сплошной цвет, либо градиентная заливка, в зависимости от того, что было определено окончательно.
* \см. tvg_shape_set_fill_rule()
*/
TVG_API Tvg_Result tvg_shape_set_fill_color(Tvg_Paint* paint, uint8_t r, uint8_t g, uint8_t b, uint8_t a);


/*!
* \brief Получает сплошной цвет фигуры.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[out] r Значение канала красного цвета в единицах [0 ~ 255]. Значение по умолчанию — 0.
* \param[out] g Значение зеленого цвета в отдельности [0 ~ 255]. Значение по умолчанию — 0.
* \param[out] b Значение канала синего цвета в отдельности [0 ~ 255]. Значение по умолчанию — 0.
* \param[out] a Значение альфа-канала в цепочке [0 ~ 255], где 0 — полностью прозрачный, а 255 — непрозрачный. Значение по умолчанию — 0.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*/
TVG_API Tvg_Result tvg_shape_get_fill_color(const Tvg_Paint* paint, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);


/*!
* \brief Установка правила заливки фигуры.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in] rule Значение правил заполняется. Значение по умолчанию —@cTVG_FILL_RULE_WINDING .
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*/
TVG_API Tvg_Result tvg_shape_set_fill_rule(Tvg_Paint* paint, Tvg_Fill_Rule rule);


/*!
* \brief Получается залив правила фигуры.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[out]правило фигуры
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента передан недопустимый указатель.
*/
TVG_API Tvg_Result tvg_shape_get_fill_rule(const Tvg_Paint* paint, Tvg_Fill_Rule* rule);


/*!
* \brief Устанавливает порядок отрисовки обводки и заливки.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in]strokeFirst Если@ctrue, обводка отображается перед заливкой, в противном случае обводка отображается как вторая (параметр по умолчанию).
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
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
* Tvg_Gradient *град =tvg_linear_gradient_new();
* tvg_linear_gradient_set (град, 700, 700, 800, 800);
* Tvg_Color_Stopcolor_stops[4] =
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
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in] grad Линейная градиентная заливка.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
* \retvalTVG_RESULT_MEMORY_CORRUPTIONНеверный указательTvg_Gradient.
*
* \note Применяется либо сплошной цвет, либо градиентная заливка, в зависимости от того, что было определено окончательно.
* \см. tvg_shape_set_fill_rule()
*/
TVG_API Tvg_Result tvg_shape_set_linear_gradient(Tvg_Paint* paint, Tvg_Gradient* grad);


/*!
* \brief Устанавливает радиальную градиентную заливку для всех фигур на пути.
*
* Части фигуры, определенные как внутренние, заполняются.
*
* \code
* Tvg_Gradient *град =tvg_radial_gradient_new();
* tvg_radial_gradient_set (град, 550, 550, 50);
* Tvg_Color_Stopcolor_stops[4] =
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
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[in] grad Заливка радиальным градиентом.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
* \retvalTVG_RESULT_MEMORY_CORRUPTIONНеверный указательTvg_Gradient.
*
* \note Применяется либо сплошной цвет, либо градиентная заливка, в зависимости от того, что было определено окончательно.
* \см. tvg_shape_set_fill_rule()
*/
TVG_API Tvg_Result tvg_shape_set_radial_gradient(Tvg_Paint* paint, Tvg_Gradient* grad);


/*!
* \brief Получает градиентную заливку фигуры.
*
* Функция не выделяет никаких данных.
*
* \param[in] Paint УказательTvg_Paintна форму объекта.
* \param[out] grad Градиентная заливка.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента передан недопустимый указатель.
*/
TVG_API Tvg_Result tvg_shape_get_gradient(const Tvg_Paint* paint, Tvg_Gradient** grad);


/** \} */   // конец защитной группы ThorVGCapi_Shape


/**
* \defgroupThorVGCapi_GradientГрадиент
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
* Tvg_Paint * форма =tvg_shape_new();
* tvg_shape_append_rect (форма, 700, 700, 100, 100, 20, 20);
* Tvg_Gradient *град =tvg_linear_gradient_new();
* tvg_linear_gradient_set (град, 700, 700, 800, 800);
* Tvg_Color_Stopcolor_stops[2] =
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
* Tvg_Paint * форма =tvg_shape_new();
* tvg_shape_append_rect (форма, 700, 700, 100, 100, 20, 20);
* Tvg_Gradient *град =tvg_radial_gradient_new();
* tvg_radial_gradient_set (град, 550, 550, 50);
* Tvg_Color_Stopcolor_stops[2] =
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
* \param[in] grad ОбъектTvg_Gradient, границы которого должны быть установлены.
* @param [in] x1 Горизонтальная координата первой точки, используемая для определения границ градиента.
* @param [in] y1 Вертикальная координата первой точки, используемой для определения границ градиента.
* @param [in] x2 Горизонтальная координата второй точки, используемая для определения границ градиента.
* @param [in] y2 Вертикальная координата второй точки, используемая для определения границ градиента.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Gradient.
*
* \note В случае, если первая и вторая точки равны, объект заливается одним цветом, с помощью последнего цвета, указанного в tvg_gradient_set_color_stops().
* \см. tvg_gradient_set_color_stops()
*/
TVG_API Tvg_Result tvg_linear_gradient_set(Tvg_Gradient* grad, float x1, float y1, float x2, float y2);


/*!
* \brief Получает границы линейного градиента.
*
* Границы линейного градиента определяются как поверхность, ограниченная двумя параллельными линиями, пересекающими
* заданные точки ( @p x1, @p y1) и ( @p x2, @p y2) соответственно. Обе линии перпендикулярны линии, соединяющей
* ( @p x1, @p y1) и ( @p x2, @p y2).
*
* \param[in] grad ОбъектTvg_Gradient, границы, которые необходимо получить.
* \param[out] x1 Горизонтальная координата первой точки, используемая для определения границ градиента.
* \param[out] y1 Вертикальная координата первой точки, используемая для определения границ градиента.
* \param[out] x2 Горизонтальная координата второй точки, используемая для определения границ градиента.
* \param[out] y2 Вертикальная координата второй точки, используемая для определения границ градиента.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Gradient.
*/
TVG_API Tvg_Result tvg_linear_gradient_get(Tvg_Gradient* grad, float* x1, float* y1, float* x2, float* y2);


/*!
* \brief Устанавливает границу радиального градиента.
*
* Границы радиального градиента определяются как окружность с центром в заданной точке ( @p cx, @p cy) заданного радиуса.
*
* \param[in] grad ОбъектTvg_Gradient, границы которого должны быть установлены.
* \param[in] cx Горизонтальная координата центра ограничивающего круга.
* \param[in] cy Вертикальная координата центра ограничивающего круга.
* \param[in] радиус Радиус ограничивающего круга.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНедопустимый указательTvg_Gradientили значение радиуса@pменьше нуля.
*
* \note В случае, если радиус@pвключается, объект становится одним цветом, последним цветом, указанным в указанном вtvg_gradient_set_color_stops().
* \см. tvg_gradient_set_color_stops()
*/
TVG_API Tvg_Result tvg_radial_gradient_set(Tvg_Gradient* grad, float cx, float cy, float radius);


/*!
* \brief функция получает центральную точку радиального градиента и радиус муравья.
*
* \param[in] grad ОбъектTvg_Gradient, границы которого должны быть установлены.
* \param[out] cx Горизонтальная координата центра ограничивающего круга.
* \param[out] cy Вертикальная координата центра ограничивающего круга.
* \param[out] radius Радиус ограничивающего круга.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Gradient.
*/
TVG_API Tvg_Result tvg_radial_gradient_get(Tvg_Gradient* grad, float* cx, float* cy, float* radius);


/*!
* \brief Устанавливает параметры цветов градиента и их положение.
*
* \param[in] grad ОбъектTvg_Gradient, для которого должна быть установлена информация о цвете.
* \param[in]color_stopМассивная структура данныхTvg_Color_Stop.
* \param[in] cnt Размер массива@pcolor_stop , необходимое количество цветов, эффект градиента.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Gradient.
*/
TVG_API Tvg_Result tvg_gradient_set_color_stops(Tvg_Gradient* grad, const Tvg_Color_Stop* color_stop, uint32_t cnt);


/*!
* \brief Получаются параметры цветов градиента, их положение и количество.
*
* Функция не выделяет никакой памяти.
*
* \param[in] grad ОбъектTvg_Gradient, из которого требуется получить информацию о цвете.
* \param[out]color_stopМассивная структура данныхTvg_Color_Stop.
* \param[out] cnt Размер массива@pcolor_stop , необходимое количество цветов, эффект градиента.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента применяется нулевой параметр@c.
*/
TVG_API Tvg_Result tvg_gradient_get_color_stops(const Tvg_Gradient* grad, const Tvg_Color_Stop** color_stop, uint32_t* cnt);


/*!
* \brief Устанавливает значениеTvg_Stroke_Fill, которое определяет, как заполнить область для различных границ градиента.
*
* \param[in] grad ОбъектTvg_Gradient.
* \param[in]spread Значение FillSpread.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Gradient.
*/
TVG_API Tvg_Result tvg_gradient_set_spread(Tvg_Gradient* grad, const Tvg_Stroke_Fill spread);


/*!
* \brief Получает значение градиента FillSpread объекта.
*
* \param[in] grad ОбъектTvg_Gradient.
* \param[out]spread Значение FillSpread.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента применяется нулевой параметр@c.
*/
TVG_API Tvg_Result tvg_gradient_get_spread(const Tvg_Gradient* grad, Tvg_Stroke_Fill* spread);


/*!
* \brief Устанавливает матрицу аффинного преобразования для градиента объекта.
*
* Ожидается получение расширенной матрицы преобразования.
*
* \param[in] grad ОбъектTvg_Gradient, который необходимо изменить.
* \param[in] m Расширенная матрица 3x3.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента применяется значение@cnullptr.
*/
TVG_API Tvg_Result tvg_gradient_set_transform(Tvg_Gradient* grad, const Tvg_Matrix* m);


/*!
* \brief Получает матрицу аффинного преобразования градиента объекта.
*
* Если преобразование не применялось, устанавливается единичная матрица.
*
* \param[in] grad ОбъектTvg_Gradient, из которого нужно получить матрицу преобразования.
* \param[out] m Расширенная матрица 3x3.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента применяется значение@cnullptr.
*/
TVG_API Tvg_Result tvg_gradient_get_transform(const Tvg_Gradient* grad, Tvg_Matrix* m);

/**
* \brief Получает уникальное значение экземпляра градиента, указывающего тип экземпляра.
*
* \param[in] grad ОбъектTvg_Gradient, для которого нужно значение получить тип.
* \param[out] type Уникальный тип экземпляра градиента.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ случае, если в качестве аргумента применяется нулевой параметр@c.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_gradient_get_type(const Tvg_Gradient* grad, Tvg_Type* type);


/**
* \см. tvg_gradient_get_type()
*/
TVG_DEPRECATED TVG_API Tvg_Result tvg_gradient_get_identifier(const Tvg_Gradient* grad, Tvg_Identifier* identifier);


/*!
* \brief Дублирует заданный объектTvg_Gradient.
*
* Создает новый объект и устанавливает все его свойства как в исходном объекте.
*
* \param[in] grad ОбъектTvg_Gradient, который необходимо скопировать.
*
* \return Скопированный объектTvg_Gradientв случае успеха, в противном случае@cnullptr.
*/
TVG_API Tvg_Gradient* tvg_gradient_duplicate(Tvg_Gradient* grad);


/*!
* \brief Удаляет заданный объект градиента.
*
* \param[in] grad Объект градиента, который нужно удалить.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Gradient.
*/
TVG_API Tvg_Result tvg_gradient_del(Tvg_Gradient* grad);


/** \} */   // конец защитной группы ThorVGCapi_Gradient


/**
* \defgroupThorVGCapi_PictureИзображение
*
* \brief Модуль, уполномоченный создать и загрузить изображение в одном из применимых форматов: svg, png, jpg, lottie и raw.
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
* ThorVG эффективно кэширует загруженные данные, используя указанный путь@pв качестве ключа.
* Это означает, что повторная загрузка того же файла не приведет к дублированию операций;
* вместо этого ThorVG будет повторно использовать ранее загруженные данные изображения.
*
* \param[in] Paint УказательTvg_Paintна объект изображения.
* \param[in] путь Абсолютный путь к файлу изображения.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНедопустимый указательTvg_Paintили пустое пространство@p.
* \retvalTVG_RESULT_NOT_SUPPORTEDФайл с неизвестными расширениями.
*/
TVG_API Tvg_Result tvg_picture_load(Tvg_Paint* paint, const char* path);


/*!
* \brief Загружает данные изображения из блока памяти заданного размера.
*
* ThorVG эффективно кэширует загруженные данные, используя указанный адрес данных@pв качестве ключа.
* когда копия@pимеет@cfalse. Это означает, что повторная загрузка тех же данных не приводит к дублированию операций.
* для общих данных@p. Вместо этого ThorVG будет повторно использовать ранее загруженные данные изображения.
*
* \param[in] Paint УказательTvg_Paintна объект изображения.
* \param[in] data Указатель области памяти, где хранятся стандартные необработанные данные изображения.
* \param[in] w Ширина данных@pизображения в пикселях.
* \param[in] h Высота данных@pизображения в пикселях.
* \param[in] предварительно умножается Если@ctrue, данные изображения предварительно умножаются в альфа-канале.
* \param[in] copy Если@ctrue, данные копируются в локальный буфер движка, в случае потери — нет.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint, или данные не предоставлены, или значение высоты@pили высоты@pравно или меньше.
* \retvalTVG_RESULT_FAILED_ALLOCATIONВозникла проблема с распределением памяти.
*
* \с 0,9
*/
TVG_API Tvg_Result tvg_picture_load_raw(Tvg_Paint* paint, uint32_t *data, uint32_t w, uint32_t h, bool copy);


/*!
* \brief Загружает данные изображения из блока памяти заданного размера.
*
* ThorVG эффективно кэширует загруженные данные, используя указанный адрес данных@pв качестве ключа.
* когда копия@pимеет@cfalse. Это означает, что повторная загрузка тех же данных не приводит к дублированию операций.
* для общих данных@p. Вместо этого ThorVG будет повторно использовать ранее загруженные данные изображения.
*
* \param[in] Paint УказательTvg_Paintна объект изображения.
* \param[in] data Указатель области памяти, где хранятся стандартные файлы изображения. Для недвоичных данных выведите строку с нулевым завершением, если копия@pимеет значение@cfalse.
* \param[in] size Размер памяти в байтах, занимаемой данными@p.
* \param[in] mimetype Mimetype или расширение данных, например «jpg», «jpeg», «svg», «svg+xml», «lottie», «png» и т. д. д. Если указана пустая строка или неизвестный тип, загрузчики будут проверены один за другим.
* \param[in] copy Если@ctrue, данные копируются в локальный буфер движка, в случае потери — нет.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ случае, если в качестве аргумента передан нулевой параметр@cили размер @p, получается или меньше.
* \retvalTVG_RESULT_NOT_SUPPORTEDФайл с неизвестными расширениями.
*
* \Предупреждение: Пользователь несет ответственность за освобождение памяти данных@p, если копия@pимеет значение@ctrue.
*/
TVG_API Tvg_Result tvg_picture_load_data(Tvg_Paint* paint, const char *data, uint32_t size, const char *mimetype, bool copy);


/*!
* \brief Изменяет размер изображения до заданной высоты и высоты.
*
* Размер содержимого изображения изменяется с сохранением соотношения сторон размера по умолчанию.
* Коэффициент масштабирования устанавливается для каждого из измерений, и к обоим из них применяется меньшее значение.
*
* \param[in] Paint УказательTvg_Paintна объект изображения.
* \param[in] w Новая ширина изображения в пикселях.
* \param[in] h Новая высота изображения в пикселях.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*/
TVG_API Tvg_Result tvg_picture_set_size(Tvg_Paint* paint, float w, float h);


/*!
* \brief Получает размер загруженного изображения.
*
* \param[in] Paint УказательTvg_Paintна объект изображения.
* \param[out] Ширина изображения в пикселях.
* \param[out] h Высота изображения в пикселях.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*/
TVG_API Tvg_Result tvg_picture_get_size(const Tvg_Paint* paint, float* w, float* h);


/*!
* \brief Извлекает объект рисования из сцен Изображение по его уникальному сценариюID.
*
* Эта функция ищет объект рисования в сцене изображения, соответствующий предоставленному идентификатору @p.
*
* \param[in] Paint УказательTvg_Paintна объект изображения.
* \param[in] id УникальныйIDобъекта рисования.

* \return Указатель объекта рисования, соответствующий заданному идентификатору, или@cnullptr, если соответствующий объект рисования не найден.
*
* \см. tvg_accessor_generate_id()
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
* \brief Устанавливает размер контейнера, в котором хранятся все краски, помещенные случайно.
*
* Если количество объектов, помещенных в сцену, известно заранее, вызов функции
* предотвращает многократное перераспределение памяти, тем самым повышая производительность.
*
* \param[in] сцена УказательTvg_Paintна объект сцены.
* \param[in] size Число объектов, для которых должна быть зарезервирована память.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_FAILED_ALLOCATIONВнутренняя ошибка при выделении памяти.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Paint.
*/
TVG_DEPRECATED TVG_API Tvg_Result tvg_scene_reserve(Tvg_Paint* scene, uint32_t size);


/*!
* \brief Передает элементы рисования на случай, с помощью объектовTvg_Paint.
*
* Нарисованными целями будут только краски, попавшие в сцену.
* Краски расположены на сцене до тех пор, пока не будет вызвана функцияtvg_scene_clear().
* Если вы заранее знаете количество толкуемых объектов, позвоните вtvg_scene_reserve().
*
* \param[in] сцена УказательTvg_Paintна объект сцены.
* \param[in] краска Графический объект, который нужно отрисовать.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента применяется нулевой параметр@c.
*
* \note Порядок отрисовки красок такой же, как и порядок их перемещения. Если вы собираетесь использовать слои, выберите возможность сортировки красок перед темой, как их раскладывать.
*/
TVG_API Tvg_Result tvg_scene_push(Tvg_Paint* scene, Tvg_Paint* paint);


/*!
* \brief Очищает сцены от нажатых красок.
*
* ОбъектыTvg_Paint, хранящиеся на стадии, освобождаются, если для параметра@pfree установлено значение@ctrue, в случае потери памяти не освобождается и
* все краски следует освобождать вручную во избежание утечек памяти.
*
* \param[in] сцена Объект сцены, которые необходимо сохранить.
* \param[in] free Если@ctrue, память, занятая красками, освобождается, в случае отказа — нет.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Canvas.
*
* \предупреждение Пожалуйста, используйте свободный аргумент @p только в том случае, если вы знаете, как он работает, в противном случае это не рекомендуется.
*/
TVG_API Tvg_Result tvg_scene_clear(Tvg_Paint* scene, bool free);

/** \} */   // конец защитной группы ThorVGCapi_Scene



/**
* \defgroupThorVGCapi_TextТекст
* \brief Класс для представления текстовых объектов в графическом формате, позволяющий отображать и манипулировать текстом в Юникоде.
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
* \param[in] Paint УказательTvg_Paintна текстовый объект.
* \param[in] имя Имя шрифта. Это должно соответствовать шрифту, доступному на холсте.
* \param[in] size Размер шрифта в пунктах.
* \param[in] style Стиль шрифта. Если пусто, используется стиль по умолчанию. В настоящее время переговоры только курсив.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENT\c nullptr применяется в качестве аргумента рисования \p.
* \retvalTVG_RESULT_INSUFFICIENT_CONDITIONУказанное имя \p не найдено.
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
* \param[in] Paint УказательTvg_Paintна текстовый объект.
* \param[in] text Многобайтовый текст, закодированный строкой utf8 для отображения.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENT\c nullptr применяется в качестве аргумента рисования \p.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_text_set_text(Tvg_Paint* paint, const char* text);


/**
* \brief Устанавливает сплошной цвет текста.
*
* \param[in] Paint УказательTvg_Paintна текстовый объект.
* \param[in] r Значение канала красного цвета в деталях [0 ~ 255]. Значение по умолчанию — 0.
* \param[in] g Значение зеленого цвета в отдельности [0 ~ 255]. Значение по умолчанию — 0.
* \param[in] b Значение канала синего цвета в отдельности [0 ~ 255]. Значение по умолчанию — 0.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENT\c nullptr применяется в качестве аргумента рисования \p.
*
* \note Применяется либо сплошной цвет, либо градиентная заливка, в зависимости от того, что было определено окончательно.
* \см. tvg_text_set_font()
*
* \с 0,15
*/
TVG_API Tvg_Result tvg_text_set_fill_color(Tvg_Paint* paint, uint8_t r, uint8_t g, uint8_t b);


/**
* \brief Устанавливает градиентную заливку текста.
*
* \param[in] Paint УказательTvg_Paintна текстовый объект.
* \param[in] grad Линейная или радиальная градиентная заливка.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENT\c nullptr применяется в качестве аргумента рисования \p.
* \retvalTVG_RESULT_MEMORY_CORRUPTIONНеверный указательTvg_Gradient.
*
* \note Применяется либо сплошной цвет, либо градиентная заливка, в зависимости от того, что было определено окончательно.
* \см. tvg_text_set_font()
*
* \с 0,15
*/
TVG_API Tvg_Result tvg_text_set_gradient(Tvg_Paint* paint, Tvg_Gradient* gradient);

/**
* \brief Загружает данные масштабируемого шрифта из файла.
*
* ThorVG эффективно кэширует загруженные данные, используя указанный путь \p в качестве переключателя.
* Это означает, что повторная загрузка того же файла не приведет к дублированию операций;
* вместо этого ThorVG будет повторно использовать ранее загруженный шрифт данных.
*
* \param[in] путь Путь к файлу шрифта.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента передан недопустимый путь \p.
* \retvalTVG_RESULT_NOT_SUPPORTEDПри загрузке файла с неизвестными расширениями.
*
* \см. tvg_font_unload()
*
* \с 0,15
*/
TVG_API Tvg_Result tvg_font_load(const char* path);


/**
* \brief Загружает данные масштабируемого шрифта из блока памяти заданного размера.
*
* ThorVG эффективно кэширует загруженные данные шрифта, используя указанное имя \p в качестве переключателя.
* Это означает, что повторная загрузка тех же шрифтов не приведет к дублированию операций.
* Вместо этого ThorVG будет повторно использовать ранее загруженный шрифт данных.
*
* \param[in] name Имя, под которым шрифт будет храниться и доступен (например, в \ptvg_text_set_fontAPI ).
* \param[in] data Указатель области памяти, где хранятся стандартные данные шрифта.
* \param[in] size Размер памяти в байтах, занимаемой данными@p.
* \param[in] mimetype Mime-тип или расширение шрифта данных. Если указано \cNULLили пустое значение "", загрузчик будет определен автоматически.
* \param[in] copy Если@ctrue, данные копируются в локальный буфер движка, в случае потери — нет (по умолчанию).
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTЕсли имя или размер не указаны, \p означает, что данные \p указывают на допустимую ячейку памяти.
* \retvalTVG_RESULT_NOT_SUPPORTEDПри загрузке файла с неизвестными расширениями.
* \retvalTVG_RESULT_INSUFFICIENT_CONDITIONПри запуске выгрузите шрифт данных, который не был загружен ранее.
*
* \предупреждение: ответственность за освобождение памяти данных \p лежит на пользователе.
*
* \note Чтобы выгрузить шрифт данных, загруженные с помощью этогоAPI, передайте правильное имя \p и \c nullptr в качестве данных \p.
*
* \с 0,15
*/
TVG_API Tvg_Result tvg_font_load_data(const char* name, const char* data, uint32_t size, const char *mimetype, bool copy);


/**
* \brief Выгружает ранее загруженные данные масштабируемого шрифта.
*
* Эта функция используется для освобождения ресурсов, связанных с файлом шрифта, загруженным в память.
*
* \param[in] Путь к загруженному файлу шрифта.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INSUFFICIENT_CONDITIONЗагрузчик не идеяирован.
*
* \note Если шрифт данных в данный момент используется, они не будут немедленно выгружены.
* \см. tvg_font_load()
*
* \с 0,15
*/
TVG_API Tvg_Result tvg_font_unload(const char* path);


/** \} */   // конец защитной группы ThorVGCapi_Text


/**
* \defgroupThorVGCapi_SaverЗаставка
* \brief Модуль для экспорта объектов рисования в указанный файл.
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
* \brief Создает новый объектTvg_Saver.
*
* \return Новый объектTvg_Saver.
*/
TVG_API Tvg_Saver* tvg_saver_new(void);


/*!
* \brief Экспортирует заданные данные рисования@pв заданный путь@p.
*
* Если модуль сохранения поддерживает какой-либо механизм сжатия, он оптимизирует размер данных.
* В некоторых случаях это может повлиять на время кодирования/декодирования. Вы можете отключить сжатие
* если вы хотите оптимизировать скорость.
*
* \param[in] saver ОбъектTvg_Saver, связанный с сохранением.
* \param[in] краска Краска, которую нужно сохранить, со всеми переключениями с ней.
* \param[in] путь Путь к файлу, в котором должны быть сохранены данные рисования.
* \param[in] compress Если@ctrue, то сжимайте данные, если это возможно.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента применяется нулевой параметр@c.
* \retvalTVG_RESULT_INSUFFICIENT_CONDITIONВ настоящее время рассматривается другие ресурсы.
* \retvalTVG_RESULT_NOT_SUPPORTEDПытаюсь сохранить файл с неизвестными расширениями или в неподдерживаемом формате.
* \retvalTVG_RESULT_UNKNOWNНеобходимо сохранить пустую краску.
*
* \note Сохранение может быть асинхронным, если назначен номер потока больше нуля. Чтобы добиться того, что сохранение выполнено, после этого вызовите tvg_saver_sync().
* \см. tvg_saver_sync()
*/
TVG_API Tvg_Result tvg_saver_save(Tvg_Saver* saver, Tvg_Paint* paint, const char* path, bool compress);


/*!
* \brief Гарантирует, что сохранение будет завершено.
*
* Поведение модуля Saver работает на основе синхронизации/асинхронности, в зависимости от настроек потоков инициализатора.
* Таким образом, если вы хотите получить эту выгоду, вы должны вызватьtvg_saver_sync() послеtvg_saver_save() в нужное время задержки.
* В противном случае вы можете мгновенно вызватьtvg_saver_sync().
*
* \param[in] saver ОбъектTvg_Saver, связанный с сохранением.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ качестве аргумента применяется нулевой параметр@c.
* \retvalTVG_RESULT_INSUFFICIENT_CONDITIONЗадача сохранить не результат.
*
* \note Асинхронное выполнение задачи зависит от реализации модуля Saver.
* \см. tvg_saver_save()
*/
TVG_API Tvg_Result tvg_saver_sync(Tvg_Saver* saver);


/*!
* \brief Удаляет указанный объектTvg_Saver.
*
* \param[in] saver ОбъектTvg_Saver, который необходимо удалить.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Saver.
*/
TVG_API Tvg_Result tvg_saver_del(Tvg_Saver* saver);


/** \} */   // конец защитной группы ThorVGCapi_Saver


/**
* \defgroupThorVGCapi_AnimationАнимация
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
* \brief Создает новый объект Анимация.
*
* \returnTvg_AnimationНовый объектTvg_Animation.
*
* \с 0,13
*/
TVG_API Tvg_Animation* tvg_animation_new(void);


/*!
* \brief У показывает настоящую анимацию.
*
* \param[in] анимация УказательTvg_Animationна объект анимации.
* \param[in] no Индекс отображаемого кадра анимации. Индекс должен быть меньше tvg_animation_get_total_frame().
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Animation.
* \retvalTVG_RESULT_INSUFFICIENT_CONDITION, если заданный номер@pсоответствует значению текущего кадра.
* \retvalTVG_RESULT_NOT_SUPPORTEDДанные изображения не относятся к анимации.
*
* \note В целях эффективности ThorVG блокирует обновление новых значений кадра, если оно отличается от текущих значений кадра.
*       меньше 0,001. В таких случаях он получает@cResult::InsufficientCondition.
*       Значения меньше 0,001 могут быть проигнорированы и не могут быть точно сохранены анимацией.
* \см. tvg_animation_get_total_frame()
*
* \с 0,13
*/
TVG_API Tvg_Result tvg_animation_set_frame(Tvg_Animation* animation, float no);


/*!
* \brief Извлекает экземпляр изображения, связанный с этим примером анимации.
*
* Эта функция обеспечивает доступ к экземпляру изображения, которое можно использовать для загрузки форматов анимации, таких как Lottie(json).
* После настройки изображения его можно переместить на назначенный холст, что позволит управлять кадрами анимации.
* с этим экземпляром анимации.
*
* \param[in] анимация УказательTvg_Animationна объект анимации.
*
* \return Экземпляр изображения, осуждающий эту анимацию.
*
* \предупреждение Экземпляр изображения принадлежит компании Animation. Его не следует удалять вручную.
*
* \с 0,13
*/
TVG_API Tvg_Paint* tvg_animation_get_picture(Tvg_Animation* animation);


/*!
* \brief Получает текущий номер кадра анимации.
*
* \param[in] анимация УказательTvg_Animationна объект анимации.
* \param[in] no Номер стандартной анимации анимации от 0 доtotalFrame()- 1.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Animationили@pнет
*
* \см. tvg_animation_get_total_frame()
* \см. tvg_animation_set_frame()
*
* \с 0,13
*/
TVG_API Tvg_Result tvg_animation_get_frame(Tvg_Animation* animation, float* no);


/*!
* \brief Получается общее количество кадров в анимации.
*
* \param[in] анимация УказательTvg_Animationна объект анимации.
* \param[in] cnt Общее количество кадров в анимации.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Animationили@pcnt.
*
* \note Нумерация кадров начинается с 0.
* \note Если изображение настроения неправильное, эта функция вернет 0.
*
* \с 0,13
*/
TVG_API Tvg_Result tvg_animation_get_total_frame(Tvg_Animation* animation, float* cnt);


/*!
* \brief Получает продолжительность анимации в секундах.
*
* \param[in] анимация УказательTvg_Animationна объект анимации.
* \param[in] длительность Продолжительность анимации в секундах.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНедопустимый указательTvg_Animationили длительность@p.
*
* \note Если изображение настроения неправильное, эта функция вернет 0.
*
* \с 0,13
*/
TVG_API Tvg_Result tvg_animation_get_duration(Tvg_Animation* animation, float* duration);


/*!
* \brief Определяет сегменты анимации.
*
* \param[in] анимация УказательTvg_Animationна объект анимации.
* \param[in] начало сегмента.
* \param[in] конец сегмента конец.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INSUFFICIENT_CONDITIONЕсли анимация не загружена.
* \retvalTVG_RESULT_INVALID_ARGUMENTКогда параметры данных применяются к ограничениям допустимого соединения.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_animation_set_segment(Tvg_Animation* animation, float begin, float end);


/*!
* \brief Получает текущий сегмент.
*
* \param[in] анимация УказательTvg_Animationна объект анимации.
* \param[out] начало сегмента начало.
* \param[out] конец сегмента конец.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INSUFFICIENT_CONDITIONЕсли анимация не загружена.
* \retvalTVG_RESULT_INVALID_ARGUMENTЕсли заданы параметры@cnullptr.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_animation_get_segment(Tvg_Animation* animation, float* begin, float* end);


/*!
* \brief Удаляет указанный объектTvg_Animation.
*
* \param[in] анимация ОбъектTvg_Animation, который необходимо удалить.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTНеверный указательTvg_Animation.
*
* \с 0,13
*/
TVG_API Tvg_Result tvg_animation_del(Tvg_Animation* animation);


/** \} */   // конец защитной группы ThorVGCapi_Animation


/**
* \defgroup Аксессор ThorVGCapi_Accesssor
* \brief Модуль для манипуляций с внутренними сценами
*
* Этот модуль помогает управлять деревом сцен.
* \{
*/

/************************************************************************/
/* Аксессуар API                                                         */
/************************************************************************/

/*!
* \brief Генерирует уникальныйID(хэш-ключ) по заданному имени.
*
* Эта функция вычисляет уникальное значение идентификатора на основе предоставленной строки.
* Вы можете использовать это, чтобы назначить уникальный объектIDPaint.
*
* \param[in] name Входная строка, на основе которой создаются уникальные идентификаторы.
*
* \return Сгенерированное значение уникального идентификатора.
*
* \note Экспериментальный API
*/
TVG_API uint32_t tvg_accessor_generate_id(const char* name);


/** \} */   // конец защитной группы ThorVGCapi_Accessor


/**
* \defgroup ThorVGCapi_LottieAnimation LottieAnimation
* \brief Модуль для управления функциями расширения лотерей.
*
* Модуль позволяет управлять расширенными функциями Лотти.
* \{
*/

/************************************************************************/
/* Расширение LottieAnimation API                                        */
/************************************************************************/

/*!
* \brief Создает новый объект LottieAnimation.
*
* \returnTvg_AnimationНовый объектTvg_LottieAnimation.
*
* \с 0,15
*/
TVG_API Tvg_Animation* tvg_lottie_animation_new(void);


/*!
* \brief Переопределите свойства лотереи с помощью слота данных.
*
* \param[in] анимация ОбъектTvg_Animationдля переопределения свойств со слотом.
* \param[in] slot Данные слота Lottie в форматеJSONили@cnullptr для сброса.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INSUFFICIENT_CONDITIONЕсли анимация не загружена.
* \retvalTVG_RESULT_INVALID_ARGUMENTКогда данный слот@pнедействителен
* \retvalTVG_RESULT_NOT_SUPPORTEDАнимация Лотти не предусмотрена.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_lottie_animation_override(Tvg_Animation* animation, const char* slot);


/*!
* \brief Определяет сегмент по маркеру.
*
* \param[in] анимация УказательTvg_Animationна объект анимации Лотти.
* \param[in] маркер Имя маркера сегмента.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INSUFFICIENT_CONDITIONЕсли анимация не загружена.
* \retvalTVG_RESULT_INVALID_ARGUMENTКогда данный маркер@pнедействителен.
* \retvalTVG_RESULT_NOT_SUPPORTEDАнимация Лотти не предусмотрена.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_lottie_animation_set_marker(Tvg_Animation* animation, const char* marker);


/*!
* \brief Получение маркеров количества анимации.
*
* \param[in] анимация УказательTvg_Animationна объект анимации Лотти.
* \param[out] cnt Значение счетчика маркеров.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ случае, если в качестве аргумента применяется нулевой параметр@c.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_lottie_animation_get_markers_cnt(Tvg_Animation* animation, uint32_t* cnt);


/*!
* \brief Получает имя маркера по заданному индексу.
*
* \param[in] анимация УказательTvg_Animationна объект анимации Лотти.
* \param[in] idx Индекс маркера анимации, начинается с 0.
* \param[out] name Имя маркера в случае успеха.
*
* \return ПеречислениеTvg_Result.
* \retvalTVG_RESULT_INVALID_ARGUMENTВ том случае, если в качестве аргумента передан@cnullptr или@cidx, возникает из-за условий связи.
*
* \note Экспериментальный API
*/
TVG_API Tvg_Result tvg_lottie_animation_get_marker(Tvg_Animation* animation, uint32_t idx, const char** name);


/** \} */   // окончание добавления в группу ThorVGCapi_LottieAnimation


/** \} */   // конец защитной группы ThorVGCapi


#ifdef __cplusplus
}
#endif

#endif //_THORVG_CAPI_H_

#endif /* LV_USE_THORVG_INTERNAL */

