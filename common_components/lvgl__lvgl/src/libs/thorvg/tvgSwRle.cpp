/*
 * Copyright (c) 2020 - 2024 the ThorVG project. All rights reserved.

 * Разрешение настоящим предоставляется бесплатно любому лицу, получившему копию.
 * данного программного обеспечения и связанных с ним файлов документации («Программное обеспечение») для решения
 * в Программном обеспечении без ограничений, включая, помимо прочего, права
 * использовать, копировать, изменять, объединять, публиковать, распространять, сублицензировать и/или продавать
 * копий Программного обеспечения и разрешать лицам, которым Программное обеспечение
 * предоставлено для этого при соблюдении следующих условий:

 * Вышеупомянутое уведомление об авторских правах и настоящее уведомление о разрешении должны быть включены во все
 * копии или существенные части Программного обеспечения.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "../../lv_conf_internal.h"
#if LV_USE_THORVG_INTERNAL

/*
 *                   Проект FreeType LICENSE
 *                   ----------------------------

 *                           27 января 2006 г.

 *                   Copyright 1996-2002, 2006 by
 *         Дэвид Тернер, Роберт Вильгельм и Вернер Лемберг



 * Введение
 * ============

 * Проект FreeType содержится в нескольких архивных пакетах;
 * из них могут сохраняться некоторые, кроме шрифтового движка FreeType,
 * различные инструменты и вклады, которые полагаются на или относятся к
 * Проект ФриТайп.

 * Данная лицензия распространяется на все файлы, находящиеся в таких пакетах, и
 * на которые не распространяется их собственная явная лицензия.  Лицензия
 * влияет таким образом на движение шрифтов FreeType, тестовые программы,
 * документация и make-файлы, в крайнем случае.

 * Эта лицензия была вдохновлена BSD, Artistic и IJG.
 * (Независимая группа JPEG), которые поощряют включение
 * и использование свободного программного обеспечения в коммерческих и бесплатно распространяемых продуктах.
 * одинаково.  Как следствие, его основные положения заключаются в следующем:

 *   o Мы не обещаем, что это программное обеспечение работает. Однако мы будем
 *     заинтересованы в любых сообщениях об ошибках. (распространение «как есть»)

 *   o Вы можете использовать это программное обеспечение по своему усмотрению, по частям или
 *      полную форму, без необходимости платить нам. (использование без лицензионных отчислений)

 *    o Вы не имеете права притворяться, что написали это программное обеспечение.  Если вы используете
 *      его или только его части в программе, вы должны подтвердить
 *     где-то в вашей документации вы использовали
 *     Код ФриТайпа. («кредиты»)

 * Мы специально разрешаем и поощряем включение этого
 * программное обеспечение с модификациями или без них в коммерческих продуктах.
 * Мы отказываемся от всех гарантий, будущего FreeType и
 * не несет никакой ответственности, связанной с проектом FreeType.


 *  Наконец, многие люди спрашивали нас о предпочтительной форме для
 *  кредит/отказ от ответственности для использования в соответствии с настоящей лицензией.  Таким образом, мы
 * рекомендуем вам использовать следующий текст:

 *   """
 *    Части этого программного обеспечения защищены авторским правом � <год> The FreeType
 *    Проект (www.freetype.org).Все права защищены.
 *   """

 *  Пожалуйста, замените <год> значением из версии FreeType, которую вы используете.
 *  на самом деле использовать.

* Юридические условия
* ===========

* 0. Определения
* --------------

*   В данных условиях`package', `FreeType Project',
*   и «Архив FreeType» относится к начальному набору файлов.
*   распространяется авторами (Дэвид Тернер, Роберт Вильгельм и
*   Вернер Лемберг) как «Проект FreeType», пусть они и называются альфа,
*   бета-версия или финальная версия.

*   «Вы» относится к лицензиату или лицу, использующему проект, где
*   «использование» — это общий термин, включающий компиляцию исходного кода проекта.
*   кода, а также связывание его для формирования исполняемого файла `program' or `».
*   Эта программа называется «программой, использующей FreeType».
*   двигатель».

*   Данная лицензия распространяется на все файлы, распространяемые в оригинальном виде.
*   Проект FreeType, включая весь исходный код, двойные файлы и
*   документации, если иное не указано в файле в его
*   оригинальная, неизмененная форма, распространяемая в оригинальном архиве.
*   Если вы не уверены, распространяется ли конкретный файл на
*   этой лицензии, вам необходимо связаться с нами, чтобы убедиться в этом.

*   Авторские права на проект FreeType (C) 1996–2000, принадлежащие Дэвиду Тернеру,
*   Роберт Вильгельм и Вернер Лемберг.  Все права защищены, за исключением
*   указано ниже.

* 1. Нет гарантии
* --------------

*   THE FREETYPE PROJECT  IS PROVIDED `AS IS' WITHOUT  WARRANTY OF ANY
*   KIND, EITHER  EXPRESS OR IMPLIED,  INCLUDING, BUT NOT  LIMITED TO,
*   WARRANTIES  OF  MERCHANTABILITY   AND  FITNESS  FOR  A  PARTICULAR
*   PURPOSE.  IN NO EVENT WILL ANY OF THE AUTHORS OR COPYRIGHT HOLDERS
*   BE LIABLE  FOR ANY DAMAGES CAUSED  BY THE USE OR  THE INABILITY TO
*   USE, OF THE FREETYPE PROJECT.

* 2. Перераспределение
* -----------------

*   Эта лицензия предоставляет международную, бесплатную, бессрочную и
*   безотзывное право и лицензия на использование, исполнение, исполнение, компиляцию,
*   отображать, копировать, создавать производные работы, распространять и
*   сублицензировать проект FreeType (как в исходном, так и в объектном коде)
*   формы) и производные от них работы для любых целей; и чтобы
*   разрешать другим осуществлять некоторые или все предоставленные права
*   настоящего Соглашения, при соблюдении следующих условий:

*    o При повторном распространении исходного кода этот файл лицензии должен сохраняться.
*      (` FTL . TXT ') без изменений; любые дополнения, удаления или изменения в
*      исходные файлы должны быть четко указаны в сопроводительном документе.
*      документация.   Уведомления об авторских правах неизменены,
*      оригинальные файлы должны быть сохранены во всех копиях исходного кода.
*      файлы.

*    o Распространение в двоичной форме должно содержать отказ от ответственности,
*      утверждает, что программное обеспечение частично основано на работе
*      FreeType Team, в документации по дистрибутиву.  Мы также
*      Рекомендуем вам связатьURLна веб-странице FreeType в вашем мире
*      документации, хотя это не является обязательным.

*  Эти условия применяются к любому программному обеспечению, полученному на основе или на основе
*  проект FreeType, а не только неизмененные файлы.   Если вы используете
*  нашу работу, вы должны признать нас.  Однако комиссию платить не нужно
*  to us.

* 3. Реклама
* --------------

*  Ни авторы, ни участники FreeType, вам не следует использовать
*  имя другого для коммерческих, рекламных или рекламных целей
*  целях без специального предварительного письменного разрешения.

*  Мы предлагаем, но не требуем, чтобы вы использовали один или несколько из
*  следующие фразы для ссылки на это программное обеспечение в вашей документации
*  или рекламные материалы:`FreeType Project',  `FreeType Engine',
*  `FreeType library', or ` Дистрибутив FreeType».

*  Поскольку вы не подписали данную лицензию, вы не обязаны
*  примите это.   Однако, поскольку проект FreeType защищен авторским правом
*  материал, только эта лицензия или другая лицензия, заключенная с
*  авторов, предоставляет вам право использовать, распространять и изменять его.
*  Поэтому,с помощью распространения или изменения FreeType
*  Проект, вы указываете, что понимаете и принимаете все условия
*  этой лицензии.

* 4. Контакты
* -----------

*  Существует два списка рассылки, названия с FreeType:

*    o бесплатный тип@nongnu.org

*      Предлагается общее использование и приложение FreeType, а также
*      будущее и желанные дополнения к библиотеке и распространению.
*      Если вам нужна поддержка, начните с этого списка, если вы
*      в документации не нашел ничего, что могло бы вам помочь.

*    o freetype-devel @nongnu.org

*      Обсуждаются ошибки, а также внутренности движка, проблемы дизайна,
*      конкретные лицензии, портирование и т. д.

*  Нашу домашнюю страницу можно найти по адресу

*    http://www.freetype.org
*/

#include <setjmp.h>
#include <limits.h>
#include <memory.h>
#include "tvgSwCommon.h"

/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/

constexpr auto PIXEL_BITS = 8;   //должно быть не менее 6 бит!
constexpr auto ONE_PIXEL = (1L << PIXEL_BITS);

using Area = long;

struct Band
{
    SwCoord min, max;
};

struct Cell
{
    SwCoord x;
    SwCoord cover;
    Area area;
    Cell *next;
};

struct RleWorker
{
    SwRle* rle;

    SwPoint cellPos;
    SwPoint cellMin;
    SwPoint cellMax;
    SwCoord cellXCnt;
    SwCoord cellYCnt;

    Area area;
    SwCoord cover;

    Cell* cells;
    ptrdiff_t maxCells;
    ptrdiff_t cellsCnt;

    SwPoint pos;

    SwPoint bezStack[32 * 3 + 1];
    SwPoint lineStack[32 + 1];
    int levStack[32];

    SwOutline* outline;

    int bandSize;
    int bandShoot;

    jmp_buf jmpBuf;

    void* buffer;
    long bufferSize;

    Cell** yCells;
    SwCoord yCnt;

    bool invalid;
    bool antiAlias;
};


static inline SwPoint UPSCALE(const SwPoint& pt)
{
    return {SwCoord(((unsigned long) pt.x) << (PIXEL_BITS - 6)), SwCoord(((unsigned long) pt.y) << (PIXEL_BITS - 6))};
}


static inline SwPoint TRUNC(const SwPoint& pt)
{
    return  {pt.x >> PIXEL_BITS, pt.y >> PIXEL_BITS};
}


static inline SwCoord TRUNC(const SwCoord x)
{
    return  x >> PIXEL_BITS;
}


static inline SwPoint SUBPIXELS(const SwPoint& pt)
{
    return {SwCoord(((unsigned long) pt.x) << PIXEL_BITS), SwCoord(((unsigned long) pt.y) << PIXEL_BITS)};
}


static inline SwCoord SUBPIXELS(const SwCoord x)
{
    return SwCoord(((unsigned long) x) << PIXEL_BITS);
}

/*
 *  Приблизительное sqrt(x*x+y*y) с использованием «альфа-макс плюс бета-мин»
 *  алгоритм.  Мы используем альфа = 1, бета = 3/8, что дает нам результаты с
 *  наибольшая ошибка менее 7% по сравнению с точным значением.
 */
static inline SwCoord HYPOT(SwPoint pt)
{
    if (pt.x < 0) pt.x = -pt.x;
    if (pt.y < 0) pt.y = -pt.y;
    return ((pt.x > pt.y) ? (pt.x + (3 * pt.y >> 3)) : (pt.y + (3 * pt.x >> 3)));
}


static void _horizLine(RleWorker& rw, SwCoord x, SwCoord y, SwCoord area, SwCoord aCount)
{
    x += rw.cellMin.x;
    y += rw.cellMin.y;

    //Диапазон клипа Y
    if (y < rw.cellMin.y || y >= rw.cellMax.y) return;

    /* вычислить покрытие линии покрытия в зависимости от правила заполнения контура */
    /* процент покрытия равен площади/( PIXEL_BITS * PIXEL_BITS *2) */
    auto coverage = static_cast<int>(area >> (PIXEL_BITS * 2 + 1 - 8));    //диапазон 0–255

    if (coverage < 0) coverage = -coverage;

    if (rw.outline->fillRule == FillRule::EvenOdd) {
        coverage &= 511;
        if (coverage > 255) coverage = 511 - coverage;
    } else {
        //обычное правило ненулевой обмотки
        if (coverage > 255) coverage = 255;
    }

    if (coverage == 0) return;

    //span содержит краткие координаты. проверить переполнение лимита
    if (x >= SHRT_MAX) {
        TVGERR("SW_ENGINE", "X-coordinate overflow!");
        return;
    }
    if (y >= SHRT_MAX) {
        TVGERR("SW_ENGINE", "Y-coordinate overflow!");
        return;
    }

    auto rle = rw.rle;

    if (!rw.antiAlias) coverage = 255;

    //посмотрим, сможем ли мы добавить этот диапазон в текущий список
    if (rle->size > 0) {
        auto span = rle->spans + rle->size - 1;
        if ((span->coverage == coverage) && (span->y == y) && (span->x + span->len == x)) {
            //Клип x диапазон
            SwCoord xOver = 0;
            if (x + aCount >= rw.cellMax.x) xOver -= (x + aCount - rw.cellMax.x);
            if (x < rw.cellMin.x) xOver -= (rw.cellMin.x - x);

            //span->len += (aCount + xOver) - 1;
            span->len += (aCount + xOver);
            return;
        }
    }

    //пул промежутков заполнен, расширьте его.
    if (rle->size >= rle->alloc) {
        auto newSize = (rle->size > 0) ? (rle->size * 2) : 256;
        if (rle->alloc < newSize) {
            rle->alloc = newSize;
            rle->spans = static_cast<SwSpan*>(lv_realloc(rle->spans, rle->alloc * sizeof(SwSpan)));
            LV_ASSERT_MALLOC(rle->spans);
        }
    }

    //Клип x диапазон
    SwCoord xOver = 0;
    if (x + aCount >= rw.cellMax.x) xOver -= (x + aCount - rw.cellMax.x);
    if (x < rw.cellMin.x) {
        xOver -= (rw.cellMin.x - x);
        x = rw.cellMin.x;
    }

    //Нечего рисовать
    if (aCount + xOver <= 0) return;

    //добавить диапазон в текущий список
    auto span = rle->spans + rle->size;
    span->x = x;
    span->y = y;
    span->len = (aCount + xOver);
    span->coverage = coverage;
    rle->size++;
}


static void _sweep(RleWorker& rw)
{
    if (rw.cellsCnt == 0) return;

    for (int y = 0; y < rw.yCnt; ++y) {
        auto cover = 0;
        auto x = 0;
        auto cell = rw.yCells[y];

        while (cell) {
            if (cell->x > x && cover != 0) _horizLine(rw, x, y, cover * (ONE_PIXEL * 2), cell->x - x);
            cover += cell->cover;
            auto area = cover * (ONE_PIXEL * 2) - cell->area;
            if (area != 0 && cell->x >= 0) _horizLine(rw, cell->x, y, area, 1);
            x = cell->x + 1;
            cell = cell->next;
        }

        if (cover != 0) _horizLine(rw, x, y, cover * (ONE_PIXEL * 2), rw.cellXCnt - x);
    }
}


static Cell* _findCell(RleWorker& rw)
{
    auto x = rw.cellPos.x;
    if (x > rw.cellXCnt) x = rw.cellXCnt;

    auto pcell = &rw.yCells[rw.cellPos.y];

    while(true) {
        Cell* cell = *pcell;
        if (!cell || cell->x > x) break;
        if (cell->x == x) return cell;
        pcell = &cell->next;
    }

    if (rw.cellsCnt >= rw.maxCells) longjmp(rw.jmpBuf, 1);

    auto cell = rw.cells + rw.cellsCnt++;
    cell->x = x;
    cell->area = 0;
    cell->cover = 0;
    cell->next = *pcell;
    *pcell = cell;

    return cell;
}


static void _recordCell(RleWorker& rw)
{
    if (rw.area | rw.cover) {
        auto cell = _findCell(rw);
        cell->area += rw.area;
        cell->cover += rw.cover;
    }
}


static void _setCell(RleWorker& rw, SwPoint pos)
{
    /* Переместите указатель ячейки в новое положение.  Мы устанавливаем «недействительный»      */
    /* флаг, указывающий, что ячейка не входит в число интересующих нас */
    /* на этапе рендеринга.  Это означает, что:                       */
    /*                                                                     */
    /* . новое вертикальное положение должно находиться внутриmin_ey..max_ey-1.        */
    /* . новое горизонтальное положение должно быть строго меньше max_ex     */
    /*                                                                     */
    /* Обратите внимание: если ячейка находится слева от области отсечения, она    */
    /* фактически установлено в горизонтальном положении (min_ex-1).                 */

    /* Все ячейки, находящиеся слева от области отсечения, переходят в
       min_ex - 1 горизонтальное положение. */
    pos.x -= rw.cellMin.x;
    pos.y -= rw.cellMin.y;

    if (pos.x > rw.cellMax.x) pos.x = rw.cellMax.x;

    //Are we moving to a different cell?
    if (pos != rw.cellPos) {
        //Запишите текущий, если он действителен.
        if (!rw.invalid) _recordCell(rw);
    }

    rw.area = 0;
    rw.cover = 0;
    rw.cellPos = pos;
    rw.invalid = ((unsigned)pos.y >= (unsigned)rw.cellYCnt || pos.x >= rw.cellXCnt);
}


static void _startCell(RleWorker& rw, SwPoint pos)
{
    if (pos.x > rw.cellMax.x) pos.x = rw.cellMax.x;
    if (pos.x < rw.cellMin.x) pos.x = rw.cellMin.x;

    rw.area = 0;
    rw.cover = 0;
    rw.cellPos = pos - rw.cellMin;
    rw.invalid = false;

    _setCell(rw, pos);
}


static void _moveTo(RleWorker& rw, const SwPoint& to)
{
    //записать текущую ячейку, если она есть */
    if (!rw.invalid) _recordCell(rw);

    //начать новую должность
    _startCell(rw, TRUNC(to));

    rw.pos = to;
}


static void _lineTo(RleWorker& rw, const SwPoint& to)
{
#define SW_UDIV(a, b) \
    static_cast<SwCoord>(((unsigned long)(a) * (unsigned long)(b)) >> \
    (sizeof(long) * CHAR_BIT - PIXEL_BITS))

    auto e1 = TRUNC(rw.pos);
    auto e2 = TRUNC(to);

    //вертикальное отсечение
    if ((e1.y >= rw.cellMax.y && e2.y >= rw.cellMax.y) || (e1.y < rw.cellMin.y && e2.y < rw.cellMin.y)) {
        rw.pos = to;
        return;
    }

    auto line = rw.lineStack;
    line[0] = to;
    line[1] = rw.pos;

    while (true) {
        auto diff = line[0] - line[1];
        auto L = HYPOT(diff);

        if (L > SHRT_MAX) {
            mathSplitLine(line);
            ++line;
            continue;
        }
        e1 = TRUNC(line[1]);
        e2 = TRUNC(line[0]);

        auto f1 = line[1] - SUBPIXELS(e1);
        SwPoint f2;

        //внутри одной клетки
        if (e1 == e2) {
            ;
        //любая горизонтальная линия
        } else if (diff.y == 0) {
            e1.x = e2.x;
            _setCell(rw, e1);
        } else if (diff.x == 0) {
            //вертикальная линия вверх
            if (diff.y > 0) {
                do {
                    f2.y = ONE_PIXEL;
                    rw.cover += (f2.y - f1.y);
                    rw.area += (f2.y - f1.y) * f1.x * 2;
                    f1.y = 0;
                    ++e1.y;
                    _setCell(rw, e1);
                } while(e1.y != e2.y);
            //вертикальная линия вниз
            } else {
                do {
                    f2.y = 0;
                    rw.cover += (f2.y - f1.y);
                    rw.area += (f2.y - f1.y) * f1.x * 2;
                    f1.y = ONE_PIXEL;
                    --e1.y;
                    _setCell(rw, e1);
                } while(e1.y != e2.y);
            }
        //любая другая линия
        } else {
            Area prod = diff.x * f1.y - diff.y * f1.x;

            /* Эти макросы ускоряют повторяющиеся деления, заменяя их.
               с умножением и сдвигом вправо. */
            auto dx_r = static_cast<long>(ULONG_MAX >> PIXEL_BITS) / (diff.x);
            auto dy_r = static_cast<long>(ULONG_MAX >> PIXEL_BITS) / (diff.y);

            /* Фундаментальное значение «prod» Определяет, какая сторона и какая сторона  */
            /* точная координата выхода линии из текущей ячейки.  Это  */
            /* также легко обновляется при переходе от одной ячейки к другой.  */
            do {
                auto px = diff.x * ONE_PIXEL;
                auto py = diff.y * ONE_PIXEL;

                //ушел
                if (prod <= 0 && prod - px > 0) {
                    f2 = {0, SW_UDIV(-prod, -dx_r)};
                    prod -= py;
                    rw.cover += (f2.y - f1.y);
                    rw.area += (f2.y - f1.y) * (f1.x + f2.x);
                    f1 = {ONE_PIXEL, f2.y};
                    --e1.x;
                //up
                } else if (prod - px <= 0 && prod - px + py > 0) {
                    prod -= px;
                    f2 = {SW_UDIV(-prod, dy_r), ONE_PIXEL};
                    rw.cover += (f2.y - f1.y);
                    rw.area += (f2.y - f1.y) * (f1.x + f2.x);
                    f1 = {f2.x, 0};
                    ++e1.y;
                //правильно
                } else if (prod - px + py <= 0 && prod + py >= 0) {
                    prod += py;
                    f2 = {ONE_PIXEL, SW_UDIV(prod, dx_r)};
                    rw.cover += (f2.y - f1.y);
                    rw.area += (f2.y - f1.y) * (f1.x + f2.x);
                    f1 = {0, f2.y};
                    ++e1.x;
                //вниз
                } else {
                    f2 = {SW_UDIV(prod, -dy_r), 0};
                    prod += px;
                    rw.cover += (f2.y - f1.y);
                    rw.area += (f2.y - f1.y) * (f1.x + f2.x);
                    f1 = {f2.x, ONE_PIXEL};
                    --e1.y;
                }

                _setCell(rw, e1);

            } while(e1 != e2);
        }

        f2 = {line[0].x - SUBPIXELS(e2.x), line[0].y - SUBPIXELS(e2.y)};
        rw.cover += (f2.y - f1.y);
        rw.area += (f2.y - f1.y) * (f1.x + f2.x);
        rw.pos = line[0];

        if (line-- == rw.lineStack) return;
    }
}


static void _cubicTo(RleWorker& rw, const SwPoint& ctrl1, const SwPoint& ctrl2, const SwPoint& to)
{
    auto arc = rw.bezStack;
    arc[0] = to;
    arc[1] = ctrl2;
    arc[2] = ctrl1;
    arc[3] = rw.pos;

    //Сократите дугу, пересекающую текущую полосу
    auto min = arc[0].y;
    auto max = arc[0].y;

    SwCoord y;
    for (auto i = 1; i < 4; ++i) {
        y = arc[i].y;
        if (y < min) min = y;
        if (y > max) max = y;
    }

    if (TRUNC(min) >= rw.cellMax.y || TRUNC(max) < rw.cellMin.y) goto draw;

    /* Решите, разделять или рисовать. См. «Быстрое прекращение          */
    /* Оценка рекурсивного подразделения кривых Безье Томаса */
    /* Ф. Хейн, в                                                      */
    /* http://www.cis.southalabama.edu/~hain/general/Publications/Bezier/Camera-ready%20CISST02%202.pdf */
    while (true) {
        {
            //разница — это вектор хордP0—P3.
            auto diff = arc[3] - arc[0];
            auto L = HYPOT(diff);

            //избегайте возможного арифметического переполнения ниже, разделив
            if (L > SHRT_MAX) goto split;

            //максимальное отклонение может составлять (s/L) * 3/4 (если v Хейна = 1)
            auto sLimit = L * (ONE_PIXEL / 6);

            auto diff1 = arc[1] - arc[0];
            auto s = diff.y * diff1.x - diff.x * diff1.y;
            if (s < 0) s = -s;
            if (s > sLimit) goto split;

            //s – это L * расстояние по перпендикуляру от P2 до линии P0 – P3.
            auto diff2 = arc[2] - arc[0];
            s = diff.y * diff2.x - diff.x * diff2.y;
            if (s < 0) s = -s;
            if (s > sLimit) goto split;

            /* Разделите супер-извилистые сегменты там, где находятся нерабочие точки.
            от хорды, которой становятся углы P0 - P1 - P3 или P0 - P2 - P3
            острый, как обнаружено с помощью соответствующих скалярных произведений */
            if (diff1.x * (diff1.x - diff.x) + diff1.y * (diff1.y - diff.y) > 0 ||
                diff2.x * (diff2.x - diff.x) + diff2.y * (diff2.y - diff.y) > 0)
                goto split;

            //нет причин расставаться
            goto draw;
        }
    split:
        mathSplitCubic(arc);
        arc += 3;
        continue;

    draw:
        _lineTo(rw, arc[0]);
        if (arc == rw.bezStack) return;
        arc -= 3;
    }
}


static void _decomposeOutline(RleWorker& rw)
{
    auto outline = rw.outline;
    auto first = 0;  //индекс первой точки контура

    for (auto cntr = outline->cntrs.begin(); cntr < outline->cntrs.end(); ++cntr) {
        auto last = *cntr;
        auto limit = outline->pts.data + last;
        auto start = UPSCALE(outline->pts[first]);
        auto pt = outline->pts.data + first;
        auto types = outline->types.data + first;
        ++types;

        _moveTo(rw, UPSCALE(outline->pts[first]));

        while (pt < limit) {
            //испустить одиночный line_to
            if (types[0] == SW_CURVE_TYPE_POINT) {
                ++pt;
                ++types;
                _lineTo(rw, UPSCALE(*pt));
            //типы кубические
            } else {
                pt += 3;
                types += 3;
                if (pt <= limit) _cubicTo(rw, UPSCALE(pt[-2]), UPSCALE(pt[-1]), UPSCALE(pt[0]));
                else if (pt - 1 == limit) _cubicTo(rw, UPSCALE(pt[-2]), UPSCALE(pt[-1]), start);
                else goto close;
            }
        }
    close:
        _lineTo(rw, start);
       first = last + 1;
    }
}


static int _genRle(RleWorker& rw)
{
    if (setjmp(rw.jmpBuf) == 0) {
        _decomposeOutline(rw);
        if (!rw.invalid) _recordCell(rw);
        return 0;
    }
    return -1;              //отсутствие клеточной памяти
}


static SwSpan* _intersectSpansRegion(const SwRle *clip, const SwRle *target, SwSpan *outSpans, uint32_t outSpansCnt)
{
    auto out = outSpans;
    auto spans = target->spans;
    auto end = target->spans + target->size;
    auto clipSpans = clip->spans;
    auto clipEnd = clip->spans + clip->size;

    while (spans < end && clipSpans < clipEnd) {
        //выровнять координаты Y.
        if (clipSpans->y > spans->y) {
            ++spans;
            continue;
        }
        if (spans->y > clipSpans->y) {
            ++clipSpans;
            continue;
        }

        //Попробуйте выполнить обрезку со всеми интервалами клипа, имеющими одинаковую координату Y.
        auto temp = clipSpans;
        while(temp < clipEnd && outSpansCnt > 0 && temp->y == clipSpans->y) {
            auto sx1 = spans->x;
            auto sx2 = sx1 + spans->len;
            auto cx1 = temp->x;
            auto cx2 = cx1 + temp->len;

            //Пролет должен быть направлен слева (x1) направо (x2). Не пересекаются.
            if (cx2 < sx1 || sx2 < cx1) {
                ++temp;
                continue;
            }

            //Область охвата клипа.
            auto x = sx1 > cx1 ? sx1 : cx1;
            auto len = (sx2 < cx2 ? sx2 : cx2) - x;
            if (len > 0) {
                out->x = x;
                out->y = temp->y;
                out->len = len;
                out->coverage = (uint8_t)(((spans->coverage * temp->coverage) + 0xff) >> 8);
                ++out;
                --outSpansCnt;
            }
            ++temp;
        }
        ++spans;
    }
    return out;
}


static SwSpan* _intersectSpansRect(const SwBBox *bbox, const SwRle *targetRle, SwSpan *outSpans, uint32_t outSpansCnt)
{
    auto out = outSpans;
    auto spans = targetRle->spans;
    auto end = targetRle->spans + targetRle->size;
    auto minx = static_cast<int16_t>(bbox->min.x);
    auto miny = static_cast<int16_t>(bbox->min.y);
    auto maxx = minx + static_cast<int16_t>(bbox->max.x - bbox->min.x) - 1;
    auto maxy = miny + static_cast<int16_t>(bbox->max.y - bbox->min.y) - 1;

    while (outSpansCnt > 0 && spans < end) {
        if (spans->y > maxy) {
            spans = end;
            break;
        }
        if (spans->y < miny || spans->x > maxx || spans->x + spans->len <= minx) {
            ++spans;
            continue;
        }
        if (spans->x < minx) {
            out->len = (spans->len - (minx - spans->x)) < (maxx - minx + 1) ? (spans->len - (minx - spans->x)) : (maxx - minx + 1);
            out->x = minx;
        }
        else {
            out->x = spans->x;
            out->len = spans->len < (maxx - spans->x + 1) ? spans->len : (maxx - spans->x + 1);
        }
        if (out->len > 0) {
            out->y = spans->y;
            out->coverage = spans->coverage;
            ++out;
            --outSpansCnt;
        }
        ++spans;
    }
    return out;
}


void _replaceClipSpan(SwRle *rle, SwSpan* clippedSpans, uint32_t size)
{
    lv_free(rle->spans);
    rle->spans = clippedSpans;
    rle->size = rle->alloc = size;
}


/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/

SwRle* rleRender(SwRle* rle, const SwOutline* outline, const SwBBox& renderRegion, bool antiAlias)
{
    constexpr auto RENDER_POOL_SIZE = 16384L;
    constexpr auto BAND_SIZE = 40;

    //TODO: Мы можем заранее сохранить несколько статических воркеров
    RleWorker rw;
    Cell buffer[RENDER_POOL_SIZE / sizeof(Cell)];

    //Инициализирующие ячейки
    rw.buffer = buffer;
    rw.bufferSize = sizeof(buffer);
    rw.yCells = reinterpret_cast<Cell**>(buffer);
    rw.cells = nullptr;
    rw.maxCells = 0;
    rw.cellsCnt = 0;
    rw.area = 0;
    rw.cover = 0;
    rw.invalid = true;
    rw.cellMin = renderRegion.min;
    rw.cellMax = renderRegion.max;
    rw.cellXCnt = rw.cellMax.x - rw.cellMin.x;
    rw.cellYCnt = rw.cellMax.y - rw.cellMin.y;
    rw.outline = const_cast<SwOutline*>(outline);
    rw.bandSize = rw.bufferSize / (sizeof(Cell) * 2);  //Размер группы: 256
    rw.bandShoot = 0;
    rw.antiAlias = antiAlias;

    if (!rle) {
    	rw.rle = reinterpret_cast<SwRle*>(lv_zalloc(sizeof(SwRle)));
        LV_ASSERT_MALLOC(rw.rle);
    }
    else rw.rle = rle;

    //Создать RLE
    Band bands[BAND_SIZE];
    Band* band;

    /* настроить вертикальные полосы */
    auto bandCnt = static_cast<int>((rw.cellMax.y - rw.cellMin.y) / rw.bandSize);
    if (bandCnt == 0) bandCnt = 1;
    else if (bandCnt >= BAND_SIZE) bandCnt = (BAND_SIZE - 1);

    auto min = rw.cellMin.y;
    auto yMax = rw.cellMax.y;
    SwCoord max;
    int ret;

    for (int n = 0; n < bandCnt; ++n, min = max) {
        max = min + rw.bandSize;
        if (n == bandCnt -1 || max > yMax) max = yMax;

        bands[0].min = min;
        bands[0].max = max;
        band = bands;

        while (band >= bands) {
            rw.yCells = static_cast<Cell**>(rw.buffer);
            rw.yCnt = band->max - band->min;

            int cellStart = sizeof(Cell*) * (int)rw.yCnt;
            int cellMod = cellStart % sizeof(Cell);

            if (cellMod > 0) cellStart += sizeof(Cell) - cellMod;

            auto cellsMax = reinterpret_cast<Cell*>((char*)rw.buffer + rw.bufferSize);
            rw.cells = reinterpret_cast<Cell*>((char*)rw.buffer + cellStart);

            if (rw.cells >= cellsMax) goto reduce_bands;

            rw.maxCells = cellsMax - rw.cells;
            if (rw.maxCells < 2) goto reduce_bands;

            for (int y = 0; y < rw.yCnt; ++y)
                rw.yCells[y] = nullptr;

            rw.cellsCnt = 0;
            rw.invalid = true;
            rw.cellMin.y = band->min;
            rw.cellMax.y = band->max;
            rw.cellYCnt = band->max - band->min;

            ret = _genRle(rw);
            if (ret == 0) {
                _sweep(rw);
                --band;
                continue;
            } else if (ret == 1) {
                goto error;
            }

        reduce_bands:
            /* переполнение пула рендеринга: мы уменьшим полосу рендеринга вдвое */
            auto bottom = band->min;
            auto top = band->max;
            auto middle = bottom + ((top - bottom) >> 1);

            /* Это слишком сложно для одной строки сканирования; должно быть
               будут некоторые проблемы */
            if (middle == bottom) goto error;

            if (bottom - top >= rw.bandSize) ++rw.bandShoot;

            band[1].min = bottom;
            band[1].max = middle;
            band[0].min = middle;
            band[0].max = top;
            ++band;
        }
    }

    if (rw.bandShoot > 8 && rw.bandSize > 16)
        rw.bandSize = (rw.bandSize >> 1);

    return rw.rle;

error:
    lv_free(rw.rle);
    return nullptr;
}


SwRle* rleRender(const SwBBox* bbox)
{
    auto width = static_cast<uint16_t>(bbox->max.x - bbox->min.x);
    auto height = static_cast<uint16_t>(bbox->max.y - bbox->min.y);

    auto rle = static_cast<SwRle*>(lv_malloc(sizeof(SwRle)));
    LV_ASSERT_MALLOC(rle);
    rle->spans = static_cast<SwSpan*>(lv_malloc(sizeof(SwSpan) * height));
    LV_ASSERT_MALLOC(rle->spans);
    rle->size = height;
    rle->alloc = height;

    auto span = rle->spans;
    for (uint16_t i = 0; i < height; ++i, ++span) {
        span->x = bbox->min.x;
        span->y = bbox->min.y + i;
        span->len = width;
        span->coverage = 255;
    }

    return rle;
}


void rleReset(SwRle* rle)
{
    if (!rle) return;
    rle->size = 0;
}


void rleFree(SwRle* rle)
{
    if (!rle) return;
    if (rle->spans) lv_free(rle->spans);
    lv_free(rle);
}


void rleClip(SwRle *rle, const SwRle *clip)
{
    if (rle->size == 0 || clip->size == 0) return;
    auto spanCnt = rle->size > clip->size ? rle->size : clip->size;
    auto spans = static_cast<SwSpan*>(lv_malloc(sizeof(SwSpan) * (spanCnt)));
    LV_ASSERT_MALLOC(spans);
    auto spansEnd = _intersectSpansRegion(clip, rle, spans, spanCnt);

    _replaceClipSpan(rle, spans, spansEnd - spans);

    TVGLOG("SW_ENGINE", "Using Path Clipping!");
}


void rleClip(SwRle *rle, const SwBBox* clip)
{
    if (rle->size == 0) return;
    auto spans = static_cast<SwSpan*>(lv_malloc(sizeof(SwSpan) * (rle->size)));
    LV_ASSERT_MALLOC(spans);
    auto spansEnd = _intersectSpansRect(clip, rle, spans, rle->size);

    _replaceClipSpan(rle, spans, spansEnd - spans);

    TVGLOG("SW_ENGINE", "Using Box Clipping!");
}

#endif /* LV_USE_THORVG_INTERNAL */

