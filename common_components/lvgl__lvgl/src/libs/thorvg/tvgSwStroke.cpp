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

#include <string.h>
#include <math.h>
#include "tvgSwCommon.h"

/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/

static constexpr auto SW_STROKE_TAG_POINT = 1;
static constexpr auto SW_STROKE_TAG_CUBIC = 2;
static constexpr auto SW_STROKE_TAG_BEGIN = 4;
static constexpr auto SW_STROKE_TAG_END = 8;

static inline SwFixed SIDE_TO_ROTATE(const int32_t s)
{
    return (SW_ANGLE_PI2 - static_cast<SwFixed>(s) * SW_ANGLE_PI);
}


static inline void SCALE(const SwStroke& stroke, SwPoint& pt)
{
    pt.x = static_cast<SwCoord>(pt.x * stroke.sx);
    pt.y = static_cast<SwCoord>(pt.y * stroke.sy);
}


static void _growBorder(SwStrokeBorder* border, uint32_t newPts)
{
    auto maxOld = border->maxPts;
    auto maxNew = border->ptsCnt + newPts;

    if (maxNew <= maxOld) return;

    auto maxCur = maxOld;

    while (maxCur < maxNew)
        maxCur += (maxCur >> 1) + 16;
    //OPTIMIZE: используйте мемпул!
    border->pts = static_cast<SwPoint*>(lv_realloc(border->pts, maxCur * sizeof(SwPoint)));
    LV_ASSERT_MALLOC(border->pts);
    border->tags = static_cast<uint8_t*>(lv_realloc(border->tags, maxCur * sizeof(uint8_t)));
    LV_ASSERT_MALLOC(border->tags);
    border->maxPts = maxCur;
}


static void _borderClose(SwStrokeBorder* border, bool reverse)
{
    auto start = border->start;
    auto count = border->ptsCnt;

    //Не записывайте пустые пути!
    if (count <= start + 1U) {
        border->ptsCnt = start;
    } else {
        /* Скопируйте последнюю точку в начало этого подпути,
           поскольку он содержит скорректированные начальные координаты */
        border->ptsCnt = --count;
        border->pts[start] = border->pts[count];

        if (reverse) {
            //поменять местами точки
            auto pt1 = border->pts + start + 1;
            auto pt2 = border->pts + count - 1;

            while (pt1 < pt2) {
                auto tmp = *pt1;
                *pt1 = *pt2;
                *pt2 = tmp;
                ++pt1;
                --pt2;
            }

            //поменять местами теги
            auto tag1 = border->tags + start + 1;
            auto tag2 = border->tags + count - 1;

            while (tag1 < tag2) {
                auto tmp = *tag1;
                *tag1 = *tag2;
                *tag2 = tmp;
                ++tag1;
                --tag2;
            }
        }

        border->tags[start] |= SW_STROKE_TAG_BEGIN;
        border->tags[count - 1] |=  SW_STROKE_TAG_END;
    }

    border->start = -1;
    border->movable = false;
}


static void _borderCubicTo(SwStrokeBorder* border, const SwPoint& ctrl1, const SwPoint& ctrl2, const SwPoint& to)
{
    _growBorder(border, 3);

    auto pt = border->pts + border->ptsCnt;
    auto tag = border->tags + border->ptsCnt;

    pt[0] = ctrl1;
    pt[1] = ctrl2;
    pt[2] = to;

    tag[0] = SW_STROKE_TAG_CUBIC;
    tag[1] = SW_STROKE_TAG_CUBIC;
    tag[2] = SW_STROKE_TAG_POINT;

    border->ptsCnt += 3;
    border->movable = false;
}


static void _borderArcTo(SwStrokeBorder* border, const SwPoint& center, SwFixed radius, SwFixed angleStart, SwFixed angleDiff, SwStroke& stroke)
{
    constexpr SwFixed ARC_CUBIC_ANGLE = SW_ANGLE_PI / 2;
    SwPoint a = {static_cast<SwCoord>(radius), 0};
    mathRotate(a, angleStart);
    SCALE(stroke, a);
    a += center;

    auto total = angleDiff;
    auto angle = angleStart;
    auto rotate = (angleDiff >= 0) ? SW_ANGLE_PI2 : -SW_ANGLE_PI2;

    while (total != 0) {
        auto step = total;
        if (step > ARC_CUBIC_ANGLE) step = ARC_CUBIC_ANGLE;
        else if (step < -ARC_CUBIC_ANGLE) step = -ARC_CUBIC_ANGLE;

        auto next = angle + step;
        auto theta = step;
        if (theta < 0) theta = -theta;

        theta >>= 1;

        //вычислить конечную точку
        SwPoint b = {static_cast<SwCoord>(radius), 0};
        mathRotate(b, next);
        SCALE(stroke, b);
        b += center;

        //вычислить первую и вторую контрольные точки
        auto length = mathMulDiv(radius, mathSin(theta) * 4, (0x10000L + mathCos(theta)) * 3);

        SwPoint a2 = {static_cast<SwCoord>(length), 0};
        mathRotate(a2, angle + rotate);
        SCALE(stroke, a2);
        a2 += a;

        SwPoint b2 = {static_cast<SwCoord>(length), 0};
        mathRotate(b2, next - rotate);
        SCALE(stroke, b2);
        b2 += b;

        //добавить кубическую дугу
        _borderCubicTo(border, a2, b2, b);

        //process the rest of the arc?
        a = b;
        total -= step;
        angle = next;
    }
}


static void _borderLineTo(SwStrokeBorder* border, const SwPoint& to, bool movable)
{
    if (border->movable) {
        //переместить последнюю точку
        border->pts[border->ptsCnt - 1] = to;
    } else {
        //не добавляйтеline_toнулевой длины
        if (border->ptsCnt > 0 && (border->pts[border->ptsCnt - 1] - to).small()) return;

        _growBorder(border, 1);
        border->pts[border->ptsCnt] = to;
        border->tags[border->ptsCnt] = SW_STROKE_TAG_POINT;
        border->ptsCnt += 1;
    }

    border->movable = movable;
}


static void _borderMoveTo(SwStrokeBorder* border, SwPoint& to)
{
    //close current open path if any?
    if (border->start >= 0) _borderClose(border, false);

    border->start = border->ptsCnt;
    border->movable = false;

    _borderLineTo(border, to, false);
}


static void _arcTo(SwStroke& stroke, int32_t side)
{
    auto border = stroke.borders + side;
    auto rotate = SIDE_TO_ROTATE(side);
    auto total = mathDiff(stroke.angleIn, stroke.angleOut);
    if (total == SW_ANGLE_PI) total = -rotate * 2;

    _borderArcTo(border, stroke.center, stroke.width, stroke.angleIn + rotate, total, stroke);
    border->movable = false;
}


static void _outside(SwStroke& stroke, int32_t side, SwFixed lineLength)
{
    auto border = stroke.borders + side;

    if (stroke.join == StrokeJoin::Round) {
        _arcTo(stroke, side);
    } else {
        //это скошенный (заостренный) или скошенный (усеченный) угол
        auto rotate = SIDE_TO_ROTATE(side);
        auto bevel = stroke.join == StrokeJoin::Bevel;
        SwFixed phi = 0;
        SwFixed thcos = 0;

        if (!bevel) {
            auto theta = mathDiff(stroke.angleIn, stroke.angleOut);
            if (theta == SW_ANGLE_PI) {
                theta = rotate;
                phi = stroke.angleIn;
            } else {
                theta /= 2;
                phi = stroke.angleIn + theta + rotate;
            }

            thcos = mathCos(theta);
            auto sigma = mathMultiply(stroke.miterlimit, thcos);

            //is miter limit exceeded?
            if (sigma < 0x10000L) bevel = true;
        }

        //это скос (ломанный угол)
        if (bevel) {
            SwPoint delta = {static_cast<SwCoord>(stroke.width), 0};
            mathRotate(delta, stroke.angleOut + rotate);
            SCALE(stroke, delta);
            delta += stroke.center;
            border->movable = false;
            _borderLineTo(border, delta, false);
        //это митра (пересечение)
        } else {
            auto length = mathDivide(stroke.width, thcos);
            SwPoint delta = {static_cast<SwCoord>(length), 0};
            mathRotate(delta, phi);
            SCALE(stroke, delta);
            delta += stroke.center;
            _borderLineTo(border, delta, false);

            /* Теперь добавьте и конечную точку
               Требуется только в том случае, если не lineto (lineLength для кривых ошибок) */
            if (lineLength == 0) {
                delta = {static_cast<SwCoord>(stroke.width), 0};
                mathRotate(delta, stroke.angleOut + rotate);
                SCALE(stroke, delta);
                delta += stroke.center;
                _borderLineTo(border, delta, false);
            }
        }
    }
}


static void _inside(SwStroke& stroke, int32_t side, SwFixed lineLength)
{
    auto border = stroke.borders + side;
    auto theta = mathDiff(stroke.angleIn, stroke.angleOut) / 2;
    SwPoint delta;
    bool intersect = false;

    /* Границы пересекаются только в том случае, если между двумяline_toи обоями
       линии достаточно длинные (длина линии для кривых равна нулю). */
    if (border->movable && lineLength > 0) {
        //вычислить минимальную необходимую длину строк
        SwFixed minLength = abs(mathMultiply(stroke.width, mathTan(theta)));
        if (stroke.lineLength >= minLength && lineLength >= minLength) intersect = true;
    }

    auto rotate = SIDE_TO_ROTATE(side);

    if (!intersect) {
        delta = {static_cast<SwCoord>(stroke.width), 0};
        mathRotate(delta, stroke.angleOut + rotate);
        SCALE(stroke, delta);
        delta += stroke.center;
        border->movable = false;
    } else {
        //вычислить срединный угол
        auto phi = stroke.angleIn + theta;
        auto thcos = mathCos(theta);
        delta = {static_cast<SwCoord>(mathDivide(stroke.width, thcos)), 0};
        mathRotate(delta, phi + rotate);
        SCALE(stroke, delta);
        delta += stroke.center;
    }

    _borderLineTo(border, delta, false);
}


void _processCorner(SwStroke& stroke, SwFixed lineLength)
{
    auto turn = mathDiff(stroke.angleIn, stroke.angleOut);

    //никакой специальной обработки углов не требуется, если поворот равен 0
    if (turn == 0) return;

    //когда мы поворачиваемся вправо, внутренняя сторона равна 0
    int32_t inside = 0;

    //в противном случае внутри будет 1
    if (turn < 0) inside = 1;

    //обработать внутреннюю часть
    _inside(stroke, inside, lineLength);

    //обработать снаружи
    _outside(stroke, 1 - inside, lineLength);
}


void _firstSubPath(SwStroke& stroke, SwFixed startAngle, SwFixed lineLength)
{
    SwPoint delta = {static_cast<SwCoord>(stroke.width), 0};
    mathRotate(delta, startAngle + SW_ANGLE_PI2);
    SCALE(stroke, delta);

    auto pt = stroke.center + delta;
    auto border = stroke.borders;
    _borderMoveTo(border, pt);

    pt = stroke.center - delta;
    ++border;
    _borderMoveTo(border, pt);

    /* Сохраните угол, положение и длину линии для последнего соединения.
       lineLength учитывает кривые */
    stroke.subPathAngle = startAngle;
    stroke.firstPt = false;
    stroke.subPathLineLength = lineLength;
}


static void _lineTo(SwStroke& stroke, const SwPoint& to)
{
    auto delta = to - stroke.center;

    //строка нулевой длины не является операцией; избегайте создания ложного угла
    if (delta.zero()) return;

    /* LineLength используется для определения пересечения контуров штрихов.
       Масштаб необходимо отменить, поскольку ширина штриха не была масштабирована.
       Альтернативный вариант — правильно масштабировать ширину обводки,
       расчет смеси рейтингов sx/sy в направлении хода. */
    delta.x = static_cast<SwCoord>(delta.x / stroke.sx);
    delta.y = static_cast<SwCoord>(delta.y / stroke.sy);
    auto lineLength = mathLength(delta);
    auto angle = mathAtan(delta);

    delta = {static_cast<SwCoord>(stroke.width), 0};
    mathRotate(delta, angle + SW_ANGLE_PI2);
    SCALE(stroke, delta);

    //обработать угол при необходимости
    if (stroke.firstPt) {
        /* Это первый сегмент подпути. Нам нужно добавить точку к каждой границе
        в соответствующих начальных точках. */
        _firstSubPath(stroke, angle, lineLength);
    } else {
        //обработать текущий угол
        stroke.angleOut = angle;
        _processCorner(stroke, lineLength);
    }

    //теперь добавьте сегмент линии как к внутреннему, так и к внешнему контуру
    auto border = stroke.borders;
    auto side = 1;

    while (side >= 0) {
        auto pt = to + delta;

        //концы линий и границ подвижны
        _borderLineTo(border, pt, true);

        delta.x = -delta.x;
        delta.y = -delta.y;

        --side;
        ++border;
    }

    stroke.angleIn = angle;
    stroke.center = to;
    stroke.lineLength = lineLength;
}


static void _cubicTo(SwStroke& stroke, const SwPoint& ctrl1, const SwPoint& ctrl2, const SwPoint& to)
{
    SwPoint bezStack[37];   //TODO: static?
    auto limit = bezStack + 32;
    auto arc = bezStack;
    auto firstArc = true;
    arc[0] = to;
    arc[1] = ctrl2;
    arc[2] = ctrl1;
    arc[3] = stroke.center;

    while (arc >= bezStack) {
        SwFixed angleIn, angleOut, angleMid;

        //инициализировать с текущим направлением
        angleIn = angleOut = angleMid = stroke.angleIn;

        auto valid = mathCubicAngle(arc, angleIn, angleMid, angleOut);

        //действительный размер
        if (valid > 0 && arc < limit) {
            if (stroke.firstPt) stroke.angleIn = angleIn;
            mathSplitCubic(arc);
            arc += 3;
            continue;
        }

        //игнорируемый размер
        if (valid < 0 && arc == bezStack) {
            stroke.center = to;
            return;
        }

        //маленький размер
        if (firstArc) {
            firstArc = false;
            //обработать угол при необходимости
            if (stroke.firstPt) {
                _firstSubPath(stroke, angleIn, 0);
            } else {
                stroke.angleOut = angleIn;
                _processCorner(stroke, 0);
            }
        } else if (abs(mathDiff(stroke.angleIn, angleIn)) > (SW_ANGLE_PI / 8) / 4) {
            //если отклонение от одной дуги к другой слишком велико, добавьте закругленный угол
            stroke.center = arc[3];
            stroke.angleOut = angleIn;
            stroke.join = StrokeJoin::Round;

            _processCorner(stroke, 0);

            //восстановить стиль соединения строк
            stroke.join = stroke.joinSaved;
        }

        //угол дуги достаточно мал; мы можем добавить его непосредственно к каждой границе
        auto theta1 = mathDiff(angleIn, angleMid) / 2;
        auto theta2 = mathDiff(angleMid, angleOut) / 2;
        auto phi1 = mathMean(angleIn, angleMid);
        auto phi2 = mathMean(angleMid, angleOut);
        auto length1 = mathDivide(stroke.width, mathCos(theta1));
        auto length2 = mathDivide(stroke.width, mathCos(theta2));
        SwFixed alpha0 = 0;

        //вычислить направление исходной дуги
        if (stroke.handleWideStrokes) {
            alpha0 = mathAtan(arc[0] - arc[3]);
        }

        auto border = stroke.borders;
        int32_t side = 0;

        while (side < 2) {
            auto rotate = SIDE_TO_ROTATE(side);

            //вычислять контрольные точки
            SwPoint _ctrl1 = {static_cast<SwCoord>(length1), 0};
            mathRotate(_ctrl1, phi1 + rotate);
            SCALE(stroke, _ctrl1);
            _ctrl1 += arc[2];

            SwPoint _ctrl2 = {static_cast<SwCoord>(length2), 0};
            mathRotate(_ctrl2, phi2 + rotate);
            SCALE(stroke, _ctrl2);
            _ctrl2 += arc[1];

            //вычислить конечную точку
            SwPoint _end = {static_cast<SwCoord>(stroke.width), 0};
            mathRotate(_end, angleOut + rotate);
            SCALE(stroke, _end);
            _end += arc[0];

            if (stroke.handleWideStrokes) {
                /* определить, больше ли радиус границы, чем радиус
                   кривизна исходной дуги */
                auto _start = border->pts[border->ptsCnt - 1];
                auto alpha1 = mathAtan(_end - _start);

                //is the direction of the border arc opposite to that of the original arc?
                if (abs(mathDiff(alpha0, alpha1)) > SW_ANGLE_PI / 2) {

                    //используйте правило синуса, чтобы найти точку пересечения
                    auto beta = mathAtan(arc[3] - _start);
                    auto gamma = mathAtan(arc[0] - _end);
                    auto bvec = _end - _start;
                    auto blen = mathLength(bvec);
                    auto sinA = abs(mathSin(alpha1 - gamma));
                    auto sinB = abs(mathSin(beta - gamma));
                    auto alen = mathMulDiv(blen, sinA, sinB);

                    SwPoint delta = {static_cast<SwCoord>(alen), 0};
                    mathRotate(delta, beta);
                    delta += _start;

                    //обойти отрицательный сектор назад
                    border->movable = false;
                    _borderLineTo(border, delta, false);
                    _borderLineTo(border, _end, false);
                    _borderCubicTo(border, _ctrl2, _ctrl1, _start);

                    //а затем перейти к конечной точке
                    _borderLineTo(border, _end, false);

                    ++side;
                    ++border;
                    continue;
                }
            }
            _borderCubicTo(border, _ctrl1, _ctrl2, _end);
            ++side;
            ++border;
        }
        arc -= 3;
        stroke.angleIn = angleOut;
    }
    stroke.center = to;
}


static void _addCap(SwStroke& stroke, SwFixed angle, int32_t side)
{
    if (stroke.cap == StrokeCap::Square) {
        auto rotate = SIDE_TO_ROTATE(side);
        auto border = stroke.borders + side;

        SwPoint delta = {static_cast<SwCoord>(stroke.width), 0};
        mathRotate(delta, angle);
        SCALE(stroke, delta);

        SwPoint delta2 = {static_cast<SwCoord>(stroke.width), 0};
        mathRotate(delta2, angle + rotate);
        SCALE(stroke, delta2);
        delta += stroke.center + delta2;

        _borderLineTo(border, delta, false);

        delta = {static_cast<SwCoord>(stroke.width), 0};
        mathRotate(delta, angle);
        SCALE(stroke, delta);

        delta2 = {static_cast<SwCoord>(stroke.width), 0};
        mathRotate(delta2, angle - rotate);
        SCALE(stroke, delta2);
        delta += delta2 + stroke.center;

        _borderLineTo(border, delta, false);

    } else if (stroke.cap == StrokeCap::Round) {

        stroke.angleIn = angle;
        stroke.angleOut = angle + SW_ANGLE_PI;
        _arcTo(stroke, side);
        return;

    } else {  //Задница
        auto rotate = SIDE_TO_ROTATE(side);
        auto border = stroke.borders + side;

        SwPoint delta = {static_cast<SwCoord>(stroke.width), 0};
        mathRotate(delta, angle + rotate);
        SCALE(stroke, delta);
        delta += stroke.center;

        _borderLineTo(border, delta, false);

        delta = {static_cast<SwCoord>(stroke.width), 0};
        mathRotate(delta, angle - rotate);
        SCALE(stroke, delta);
        delta += stroke.center;

        _borderLineTo(border, delta, false);
    }
}


static void _addReverseLeft(SwStroke& stroke, bool opened)
{
    auto right = stroke.borders + 0;
    auto left = stroke.borders + 1;
    auto newPts = left->ptsCnt - left->start;

    if (newPts <= 0) return;

    _growBorder(right, newPts);

    auto dstPt = right->pts + right->ptsCnt;
    auto dstTag = right->tags + right->ptsCnt;
    auto srcPt = left->pts + left->ptsCnt - 1;
    auto srcTag = left->tags + left->ptsCnt - 1;

    while (srcPt >= left->pts + left->start) {
        *dstPt = *srcPt;
        *dstTag = *srcTag;

        if (opened) {
             dstTag[0] &= ~(SW_STROKE_TAG_BEGIN | SW_STROKE_TAG_END);
        } else {
            //при необходимости переключите начальные/конечные теги
            auto ttag = dstTag[0] & (SW_STROKE_TAG_BEGIN | SW_STROKE_TAG_END);
            if (ttag == SW_STROKE_TAG_BEGIN || ttag == SW_STROKE_TAG_END)
              dstTag[0] ^= (SW_STROKE_TAG_BEGIN | SW_STROKE_TAG_END);
        }
        --srcPt;
        --srcTag;
        ++dstPt;
        ++dstTag;
    }

    left->ptsCnt = left->start;
    right->ptsCnt += newPts;
    right->movable = false;
    left->movable = false;
}


static void _beginSubPath(SwStroke& stroke, const SwPoint& to, bool closed)
{
    /* Мы не можем обработать первый пункт, потому что недостаточно
       информация о его углу/крышке. Позже будет обработано
       в _endSubPath() */

    stroke.firstPt = true;
    stroke.center = to;
    stroke.closedSubPath = closed;

    /* Определите, нужно ли нам проверить, больше ли радиус границы
       чем радиус кривизны кривой, чтобы специально обработать этот случай.
       Это требуется только в том случае, если могут быть созданы скосы или стыковые заглушки, поскольку
       круглые и угловые соединения, а также круглые и квадратные заглушки закрывают отрицательный сектор
       создан широкими мазками. */
    if ((stroke.join != StrokeJoin::Round) || (!stroke.closedSubPath && stroke.cap == StrokeCap::Butt))
        stroke.handleWideStrokes = true;
    else
        stroke.handleWideStrokes = false;

    stroke.ptStartSubPath = to;
    stroke.angleIn = 0;
}


static void _endSubPath(SwStroke& stroke)
{
    if (stroke.closedSubPath) {
        //закройте путь, если нужно
        if (stroke.center != stroke.ptStartSubPath)
            _lineTo(stroke, stroke.ptStartSubPath);

        //обработать угол
        stroke.angleOut = stroke.subPathAngle;
        auto turn = mathDiff(stroke.angleIn, stroke.angleOut);

        //Никакой специальной обработки углов не требуется, если поворот равен 0.
        if (turn != 0) {
            //когда мы поворачиваем направо, внутри 0
            int32_t inside = 0;

            //в противном случае внутри будет 1
            if (turn < 0) inside = 1;

            _inside(stroke, inside, stroke.subPathLineLength);        //внутри
            _outside(stroke, 1 - inside, stroke.subPathLineLength);   //снаружи
        }

        _borderClose(stroke.borders + 0, false);
        _borderClose(stroke.borders + 1, true);
    } else {
        auto right = stroke.borders;

        /* ладно, это открытый путь, нужно добавить заглушку между
           вправо и влево, добавьте обратную сторону левой, затем добавьте последнюю заглушку
           между левым и правым */
        _addCap(stroke, stroke.angleIn, 0);

        //добавить перевернутые точки с «слева» на «справа»
        _addReverseLeft(stroke, true);

        //теперь добавим финальную шапку
        stroke.center = stroke.ptStartSubPath;
        _addCap(stroke, stroke.subPathAngle + SW_ANGLE_PI, 0);

        /* теперь завершите правый подпуть соответственно. Левый — перемотка назад
           и не требует дальнейшей обработки */
        _borderClose(right, false);
    }
}


static void _getCounts(SwStrokeBorder* border, uint32_t& ptsCnt, uint32_t& cntrsCnt)
{
    auto count = border->ptsCnt;
    auto tags = border->tags;
    uint32_t _ptsCnt = 0;
    uint32_t _cntrsCnt = 0;
    bool inCntr = false;

    while (count > 0) {
        if (tags[0] & SW_STROKE_TAG_BEGIN) {
            if (inCntr) goto fail;
            inCntr = true;
        } else if (!inCntr) goto fail;

        if (tags[0] & SW_STROKE_TAG_END) {
            inCntr = false;
            ++_cntrsCnt;
        }
        --count;
        ++_ptsCnt;
        ++tags;
    }

    if (inCntr) goto fail;

    ptsCnt = _ptsCnt;
    cntrsCnt = _cntrsCnt;

    return;

fail:
    ptsCnt = 0;
    cntrsCnt = 0;
}


static void _exportBorderOutline(const SwStroke& stroke, SwOutline* outline, uint32_t side)
{
    auto border = stroke.borders + side;
    if (border->ptsCnt == 0) return;

    memcpy(outline->pts.data + outline->pts.count, border->pts, border->ptsCnt * sizeof(SwPoint));

    auto cnt = border->ptsCnt;
    auto src = border->tags;
    auto tags = outline->types.data + outline->types.count;
    auto idx = outline->pts.count;

    while (cnt > 0) {
        if (*src & SW_STROKE_TAG_POINT) *tags = SW_CURVE_TYPE_POINT;
        else if (*src & SW_STROKE_TAG_CUBIC) *tags = SW_CURVE_TYPE_CUBIC;
        else TVGERR("SW_ENGINE", "Invalid stroke tag was given! = %d", *src);
        if (*src & SW_STROKE_TAG_END) outline->cntrs.push(idx);
        ++src;
        ++tags;
        ++idx;
        --cnt;
    }
    outline->pts.count += border->ptsCnt;
    outline->types.count += border->ptsCnt;
}


/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/

void strokeFree(SwStroke* stroke)
{
    if (!stroke) return;

    //свободные границы
    if (stroke->borders[0].pts) lv_free(stroke->borders[0].pts);
    if (stroke->borders[0].tags) lv_free(stroke->borders[0].tags);
    if (stroke->borders[1].pts) lv_free(stroke->borders[1].pts);
    if (stroke->borders[1].tags) lv_free(stroke->borders[1].tags);

    fillFree(stroke->fill);
    stroke->fill = nullptr;

    lv_free(stroke);
}


void strokeReset(SwStroke* stroke, const RenderShape* rshape, const Matrix& transform)
{
    stroke->sx = sqrtf(powf(transform.e11, 2.0f) + powf(transform.e21, 2.0f));
    stroke->sy = sqrtf(powf(transform.e12, 2.0f) + powf(transform.e22, 2.0f));
    stroke->width = HALF_STROKE(rshape->strokeWidth());
    stroke->cap = rshape->strokeCap();
    stroke->miterlimit = static_cast<SwFixed>(rshape->strokeMiterlimit() * 65536.0f);

    //Сохранить соединение линий: его можно временно изменить при обводке кривых...
    stroke->joinSaved = stroke->join = rshape->strokeJoin();

    stroke->borders[0].ptsCnt = 0;
    stroke->borders[0].start = -1;
    stroke->borders[1].ptsCnt = 0;
    stroke->borders[1].start = -1;
}


bool strokeParseOutline(SwStroke* stroke, const SwOutline& outline)
{
    uint32_t first = 0;
    uint32_t i = 0;

    for (auto cntr = outline.cntrs.begin(); cntr < outline.cntrs.end(); ++cntr, ++i) {
        auto last = *cntr;           //индекс последней точки контура
        auto limit = outline.pts.data + last;

        //Пропускать пустые точки
        if (last <= first) {
            first = last + 1;
            continue;
        }

        auto start = outline.pts[first];
        auto pt = outline.pts.data + first;
        auto types = outline.types.data + first;
        auto type = types[0];

        //Контур не может начинаться с кубической контрольной точки.
        if (type == SW_CURVE_TYPE_CUBIC) return false;
        ++types;

        auto closed =  outline.closed.data ? outline.closed.data[i]: false;

        _beginSubPath(*stroke, start, closed);

        while (pt < limit) {
            //испустить одиночный line_to
            if (types[0] == SW_CURVE_TYPE_POINT) {
                ++pt;
                ++types;
                _lineTo(*stroke, *pt);
            //типы кубические
            } else {
                pt += 3;
                types += 3;
                if (pt <= limit) _cubicTo(*stroke, pt[-2], pt[-1], pt[0]);
                else if (pt - 1 == limit) _cubicTo(*stroke, pt[-2], pt[-1], start);
                else goto close;
            }
        }
    close:
        if (!stroke->firstPt) _endSubPath(*stroke);
        first = last + 1;
    }
    return true;
}


SwOutline* strokeExportOutline(SwStroke* stroke, SwMpool* mpool, unsigned tid)
{
    uint32_t count1, count2, count3, count4;

    _getCounts(stroke->borders + 0, count1, count2);
    _getCounts(stroke->borders + 1, count3, count4);

    auto ptsCnt = count1 + count3;
    auto cntrsCnt = count2 + count4;

    auto outline = mpoolReqStrokeOutline(mpool, tid);
    outline->pts.reserve(ptsCnt);
    outline->types.reserve(ptsCnt);
    outline->cntrs.reserve(cntrsCnt);

    _exportBorderOutline(*stroke, outline, 0);  //ушел
    _exportBorderOutline(*stroke, outline, 1);  //правильно

    return outline;
}

#endif /* LV_USE_THORVG_INTERNAL */

