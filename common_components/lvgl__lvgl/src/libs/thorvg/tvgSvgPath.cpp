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
 * Copyright notice for the EFL:

 * Copyright (C) EFL developers (see AUTHORS)

 * Все права защищены.

 * Распространение и использование в исходной и двоичной форме, с или без
 * Модификация допускается при соблюдении следующих условий:

 *   1. Redistributions of source code must retain the above copyright
 *      уведомление, этот список условий и следующий отказ от ответственности.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      уведомление, этот список условий и следующий отказ от ответственности в
 *      документация и/или другие материалы, поставляемые вместе с дистрибутивом.

 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define _USE_MATH_DEFINES       //Математические константы не определены в стандарте C/C++.

#include <cstring>
#include <ctype.h>
#include "tvgMath.h"
#include "tvgShape.h"
#include "tvgSvgLoaderCommon.h"
#include "tvgSvgPath.h"
#include "tvgStr.h"

/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/

static char* _skipComma(const char* content)
{
    while (*content && isspace(*content)) {
        content++;
    }
    if (*content == ',') return (char*)content + 1;
    return (char*)content;
}


static bool _parseNumber(char** content, float* number)
{
    char* end = NULL;
    *number = strToFloat(*content, &end);
    //Если начало строки не число
    if ((*content) == end) return false;
    //Пропустить запятую, если есть
    *content = _skipComma(end);
    return true;
}


static bool _parseFlag(char** content, int* number)
{
    char* end = NULL;
    if (*(*content) != '0' && *(*content) != '1') return false;
    *number = *(*content) - '0';
    *content += 1;
    end = *content;
    *content = _skipComma(end);

    return true;
}


void _pathAppendArcTo(Array<PathCommand>* cmds, Array<Point>* pts, Point* cur, Point* curCtl, float x, float y, float rx, float ry, float angle, bool largeArc, bool sweep)
{
    float cxp, cyp, cx, cy;
    float sx, sy;
    float cosPhi, sinPhi;
    float dx2, dy2;
    float x1p, y1p;
    float x1p2, y1p2;
    float rx2, ry2;
    float lambda;
    float c;
    float at;
    float theta1, deltaTheta;
    float nat;
    float delta, bcp;
    float cosPhiRx, cosPhiRy;
    float sinPhiRx, sinPhiRy;
    float cosTheta1, sinTheta1;
    int segments;

    //Некоторые полезные материалы доступны здесь:
    //http://www.w3.org/TR/SVG/implnote.html#ArcImplementationNotes
    sx = cur->x;
    sy = cur->y;

    //Коррекция радиусов, выходящих за пределы диапазона, см. F6 .6.1 (шаг 2)
    rx = fabsf(rx);
    ry = fabsf(ry);

    angle = deg2rad(angle);
    cosPhi = cosf(angle);
    sinPhi = sinf(angle);
    dx2 = (sx - x) / 2.0f;
    dy2 = (sy - y) / 2.0f;
    x1p = cosPhi * dx2 + sinPhi * dy2;
    y1p = cosPhi * dy2 - sinPhi * dx2;
    x1p2 = x1p * x1p;
    y1p2 = y1p * y1p;
    rx2 = rx * rx;
    ry2 = ry * ry;
    lambda = (x1p2 / rx2) + (y1p2 / ry2);

    //Коррекция радиусов, выходящих за пределы диапазона, см. F6 .6.2 (шаг 4)
    if (lambda > 1.0f) {
        //См. F6 .6.3.
        float lambdaRoot = sqrtf(lambda);

        rx *= lambdaRoot;
        ry *= lambdaRoot;
        //Обновите rx2 и ry2.
        rx2 = rx * rx;
        ry2 = ry * ry;
    }

    c = (rx2 * ry2) - (rx2 * y1p2) - (ry2 * x1p2);

    //Проверьте, нет ли возможного решения
    //(т.е. мы не можем извлечь квадратный корень из отрицательного значения)
    if (c < 0.0f) {
        //Масштабируйте равномерно, пока не получим единственное решение.
        //(см. F6 .2), т.е. когда c == 0,0
        float scale = sqrtf(1.0f - c / (rx2 * ry2));
        rx *= scale;
        ry *= scale;
        //Обновите rx2 и ry2.
        rx2 = rx * rx;
        ry2 = ry * ry;

        //Шаг 2 ( F6 .5.2) – упрощен, поскольку c == 0,0
        cxp = 0.0f;
        cyp = 0.0f;
        //Шаг 3 (первая часть F6 .5.3) – упрощен, поскольку cxp и cyp == 0.0
        cx = 0.0f;
        cy = 0.0f;
    } else {
        //Полный расчет c
        c = sqrtf(c / ((rx2 * y1p2) + (ry2 * x1p2)));
        //Обратный знак, если Fa == Fs
        if (largeArc == sweep) c = -c;

        //Шаг 2 ( F6 .5.2)
        cxp = c * (rx * y1p / ry);
        cyp = c * (-ry * x1p / rx);

        //Шаг 3 (F6 .5.3 первая часть)
        cx = cosPhi * cxp - sinPhi * cyp;
        cy = sinPhi * cxp + cosPhi * cyp;
    }

    //Шаг 3 (F6 .5.3 вторая часть) теперь у нас есть центральная точка эллипса.
    cx += (sx + x) / 2.0f;
    cy += (sy + y) / 2.0f;

    //Шаг 4 ( F6 .5.4)
    //Мы не используем arccos (согласно документу w3c), см.
    //http://www.euclideanspace.com/maths/algebra/vectors/angleBetween/index.htm
    //Note: atan2 (0.0, 1.0) == 0.0
    at = tvg::atan2(((y1p - cyp) / ry), ((x1p - cxp) / rx));
    theta1 = (at < 0.0f) ? 2.0f * MATH_PI + at : at;

    nat = tvg::atan2(((-y1p - cyp) / ry), ((-x1p - cxp) / rx));
    deltaTheta = (nat < at) ? 2.0f * MATH_PI - at + nat : nat - at;

    if (sweep) {
        //Убедитесь, что дельта-тета < 0, иначе добавьте 360 градусов.
        if (deltaTheta < 0.0f) deltaTheta += 2.0f * MATH_PI;
    } else {
        //Убедитесь, что дельта-тета > 0, иначе вычтите 360 градусов.
        if (deltaTheta > 0.0f) deltaTheta -= 2.0f * MATH_PI;
    }

    //Добавьте несколько кубических кривых Безье, чтобы приблизить дугу.
    //(менее 90 градусов)
    //Мы добавляем один дополнительный сегмент, потому что нам что-то нужно
    //Меньше 90 градусов (т.е. не 90 сам по себе)
    segments = static_cast<int>(fabsf(deltaTheta / MATH_PI2) + 1.0f);
    delta = deltaTheta / segments;

    //http://www.stillhq.com/ctpfaq/2001/comp.text.pdf-faq-2001-04.txt (раздел 2.13)
    bcp = 4.0f / 3.0f * (1.0f - cosf(delta / 2.0f)) / sinf(delta / 2.0f);

    cosPhiRx = cosPhi * rx;
    cosPhiRy = cosPhi * ry;
    sinPhiRx = sinPhi * rx;
    sinPhiRy = sinPhi * ry;

    cosTheta1 = cosf(theta1);
    sinTheta1 = sinf(theta1);

    for (int i = 0; i < segments; ++i) {
        //Конечный угол (для этого сегмента) = ток + дельта
        float c1x, c1y, ex, ey, c2x, c2y;
        float theta2 = theta1 + delta;
        float cosTheta2 = cosf(theta2);
        float sinTheta2 = sinf(theta2);
        Point p[3];

        //Первая контрольная точка (на основе начальной точки sx,sy)
        c1x = sx - bcp * (cosPhiRx * sinTheta1 + sinPhiRy * cosTheta1);
        c1y = sy + bcp * (cosPhiRy * cosTheta1 - sinPhiRx * sinTheta1);

        //Конечная точка (для этого сегмента)
        ex = cx + (cosPhiRx * cosTheta2 - sinPhiRy * sinTheta2);
        ey = cy + (sinPhiRx * cosTheta2 + cosPhiRy * sinTheta2);

        //Вторая контрольная точка (на основе конечной точки ex,ey)
        c2x = ex + bcp * (cosPhiRx * sinTheta2 + sinPhiRy * cosTheta2);
        c2y = ey + bcp * (sinPhiRx * sinTheta2 - cosPhiRy * cosTheta2);
        cmds->push(PathCommand::CubicTo);
        p[0] = {c1x, c1y};
        p[1] = {c2x, c2y};
        p[2] = {ex, ey};
        pts->push(p[0]);
        pts->push(p[1]);
        pts->push(p[2]);
        *curCtl = p[1];
        *cur = p[2];

        //Следующая начальная точка — это текущая конечная точка (то же самое для угла).
        sx = ex;
        sy = ey;
        theta1 = theta2;
        //Избегайте перерасчетов
        cosTheta1 = cosTheta2;
        sinTheta1 = sinTheta2;
    }
}

static int _numberCount(char cmd)
{
    int count = 0;
    switch (cmd) {
        case 'M':
        case 'm':
        case 'L':
        case 'l':
        case 'T':
        case 't': {
            count = 2;
            break;
        }
        case 'C':
        case 'c':
        case 'E':
        case 'e': {
            count = 6;
            break;
        }
        case 'H':
        case 'h':
        case 'V':
        case 'v': {
            count = 1;
            break;
        }
        case 'S':
        case 's':
        case 'Q':
        case 'q': {
            count = 4;
            break;
        }
        case 'A':
        case 'a': {
            count = 7;
            break;
        }
        default:
            break;
    }
    return count;
}


static bool _processCommand(Array<PathCommand>* cmds, Array<Point>* pts, char cmd, float* arr, int count, Point* cur, Point* curCtl, Point* startPoint, bool *isQuadratic, bool* closed)
{
    switch (cmd) {
        case 'm':
        case 'l':
        case 'c':
        case 's':
        case 'q':
        case 't': {
            for (int i = 0; i < count - 1; i += 2) {
                arr[i] = arr[i] + cur->x;
                arr[i + 1] = arr[i + 1] + cur->y;
            }
            break;
        }
        case 'h': {
            arr[0] = arr[0] + cur->x;
            break;
        }
        case 'v': {
            arr[0] = arr[0] + cur->y;
            break;
        }
        case 'a': {
            arr[5] = arr[5] + cur->x;
            arr[6] = arr[6] + cur->y;
            break;
        }
        default: {
            break;
        }
    }

    switch (cmd) {
        case 'm':
        case 'M': {
            Point p = {arr[0], arr[1]};
            cmds->push(PathCommand::MoveTo);
            pts->push(p);
            *cur = {arr[0], arr[1]};
            *startPoint = {arr[0], arr[1]};
            break;
        }
        case 'l':
        case 'L': {
            Point p = {arr[0], arr[1]};
            cmds->push(PathCommand::LineTo);
            pts->push(p);
            *cur = {arr[0], arr[1]};
            break;
        }
        case 'c':
        case 'C': {
            Point p[3];
            cmds->push(PathCommand::CubicTo);
            p[0] = {arr[0], arr[1]};
            p[1] = {arr[2], arr[3]};
            p[2] = {arr[4], arr[5]};
            pts->push(p[0]);
            pts->push(p[1]);
            pts->push(p[2]);
            *curCtl = p[1];
            *cur = p[2];
            *isQuadratic = false;
            break;
        }
        case 's':
        case 'S': {
            Point p[3], ctrl;
            if ((cmds->count > 1) && (cmds->last() == PathCommand::CubicTo) &&
                !(*isQuadratic)) {
                ctrl.x = 2 * cur->x - curCtl->x;
                ctrl.y = 2 * cur->y - curCtl->y;
            } else {
                ctrl = *cur;
            }
            cmds->push(PathCommand::CubicTo);
            p[0] = ctrl;
            p[1] = {arr[0], arr[1]};
            p[2] = {arr[2], arr[3]};
            pts->push(p[0]);
            pts->push(p[1]);
            pts->push(p[2]);
            *curCtl = p[1];
            *cur = p[2];
            *isQuadratic = false;
            break;
        }
        case 'q':
        case 'Q': {
            Point p[3];
            float ctrl_x0 = (cur->x + 2 * arr[0]) * (1.0 / 3.0);
            float ctrl_y0 = (cur->y + 2 * arr[1]) * (1.0 / 3.0);
            float ctrl_x1 = (arr[2] + 2 * arr[0]) * (1.0 / 3.0);
            float ctrl_y1 = (arr[3] + 2 * arr[1]) * (1.0 / 3.0);
            cmds->push(PathCommand::CubicTo);
            p[0] = {ctrl_x0, ctrl_y0};
            p[1] = {ctrl_x1, ctrl_y1};
            p[2] = {arr[2], arr[3]};
            pts->push(p[0]);
            pts->push(p[1]);
            pts->push(p[2]);
            *curCtl = {arr[0], arr[1]};
            *cur = p[2];
            *isQuadratic = true;
            break;
        }
        case 't':
        case 'T': {
            Point p[3], ctrl;
            if ((cmds->count > 1) && (cmds->last() == PathCommand::CubicTo) &&
                *isQuadratic) {
                ctrl.x = 2 * cur->x - curCtl->x;
                ctrl.y = 2 * cur->y - curCtl->y;
            } else {
                ctrl = *cur;
            }
            float ctrl_x0 = (cur->x + 2 * ctrl.x) * (1.0 / 3.0);
            float ctrl_y0 = (cur->y + 2 * ctrl.y) * (1.0 / 3.0);
            float ctrl_x1 = (arr[0] + 2 * ctrl.x) * (1.0 / 3.0);
            float ctrl_y1 = (arr[1] + 2 * ctrl.y) * (1.0 / 3.0);
            cmds->push(PathCommand::CubicTo);
            p[0] = {ctrl_x0, ctrl_y0};
            p[1] = {ctrl_x1, ctrl_y1};
            p[2] = {arr[0], arr[1]};
            pts->push(p[0]);
            pts->push(p[1]);
            pts->push(p[2]);
            *curCtl = {ctrl.x, ctrl.y};
            *cur = p[2];
            *isQuadratic = true;
            break;
        }
        case 'h':
        case 'H': {
            Point p = {arr[0], cur->y};
            cmds->push(PathCommand::LineTo);
            pts->push(p);
            cur->x = arr[0];
            break;
        }
        case 'v':
        case 'V': {
            Point p = {cur->x, arr[0]};
            cmds->push(PathCommand::LineTo);
            pts->push(p);
            cur->y = arr[0];
            break;
        }
        case 'z':
        case 'Z': {
            cmds->push(PathCommand::Close);
            *cur = *startPoint;
            *closed = true;
            break;
        }
        case 'a':
        case 'A': {
            if (tvg::zero(arr[0]) || tvg::zero(arr[1])) {
                Point p = {arr[5], arr[6]};
                cmds->push(PathCommand::LineTo);
                pts->push(p);
                *cur = {arr[5], arr[6]};
            } else if (!tvg::equal(cur->x, arr[5]) || !tvg::equal(cur->y, arr[6])) {
                _pathAppendArcTo(cmds, pts, cur, curCtl, arr[5], arr[6], fabsf(arr[0]), fabsf(arr[1]), arr[2], arr[3], arr[4]);
                *cur = *curCtl = {arr[5], arr[6]};
                *isQuadratic = false;
            }
            break;
        }
        default: {
            return false;
        }
    }
    return true;
}


static char* _nextCommand(char* path, char* cmd, float* arr, int* count, bool* closed)
{
    int large, sweep;

    path = _skipComma(path);
    if (isalpha(*path)) {
        *cmd = *path;
        path++;
        *count = _numberCount(*cmd);
    } else {
        if (*cmd == 'm') *cmd = 'l';
        else if (*cmd == 'M') *cmd = 'L';
        else {
          if (*closed) return nullptr;
        }
    }
    if (*count == 7) {
        //Особый случай для команды дуги
        if (_parseNumber(&path, &arr[0])) {
            if (_parseNumber(&path, &arr[1])) {
                if (_parseNumber(&path, &arr[2])) {
                    if (_parseFlag(&path, &large)) {
                        if (_parseFlag(&path, &sweep)) {
                            if (_parseNumber(&path, &arr[5])) {
                                if (_parseNumber(&path, &arr[6])) {
                                    arr[3] = (float)large;
                                    arr[4] = (float)sweep;
                                    return path;
                                }
                            }
                        }
                    }
                }
            }
        }
        *count = 0;
        return NULL;
    }
    for (int i = 0; i < *count; i++) {
        if (!_parseNumber(&path, &arr[i])) {
            *count = 0;
            return NULL;
        }
        path = _skipComma(path);
    }
    return path;
}


/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/


bool svgPathToShape(const char* svgPath, Shape* shape)
{
    float numberArray[7];
    int numberCount = 0;
    Point cur = { 0, 0 };
    Point curCtl = { 0, 0 };
    Point startPoint = { 0, 0 };
    char cmd = 0;
    bool isQuadratic = false;
    bool closed = false;
    char* path = (char*)svgPath;

    auto& pts = P(shape)->rs.path.pts;
    auto& cmds = P(shape)->rs.path.cmds;
    auto lastCmds = cmds.count;

    while ((path[0] != '\0')) {
        path = _nextCommand(path, &cmd, numberArray, &numberCount, &closed);
        if (!path) break;
        closed = false;
        if (!_processCommand(&cmds, &pts, cmd, numberArray, numberCount, &cur, &curCtl, &startPoint, &isQuadratic, &closed)) break;
    }

    if (cmds.count > lastCmds && cmds[lastCmds] != PathCommand::MoveTo) return false;
    return true;
}

#endif /* LV_USE_THORVG_INTERNAL */

