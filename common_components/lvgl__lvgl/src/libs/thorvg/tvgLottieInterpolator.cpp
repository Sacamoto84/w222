/*
 * Copyright (c) 2023 - 2024 the ThorVG project. All rights reserved.

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

/* Данная форма исходного кода регулируется условиями публичной лицензии Mozilla.
 * Лицензия, версия 2.0. Если копия MPL не распространялась вместе с этим
 * файл, вы можете получить его по адресу http://mozilla.org/MPL/2.0/.. */

#include <string.h>
#include "tvgCommon.h"
#include "tvgMath.h"
#include "tvgLottieInterpolator.h"


/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/

#define NEWTON_MIN_SLOPE 0.02f
#define NEWTON_ITERATIONS 4
#define SUBDIVISION_PRECISION 0.0000001f
#define SUBDIVISION_MAX_ITERATIONS 10


static inline float _constA(float aA1, float aA2) { return 1.0f - 3.0f * aA2 + 3.0f * aA1; }
static inline float _constB(float aA1, float aA2) { return 3.0f * aA2 - 6.0f * aA1; }
static inline float _constC(float aA1) { return 3.0f * aA1; }


static inline float _getSlope(float t, float aA1, float aA2)
{
    return 3.0f * _constA(aA1, aA2) * t * t + 2.0f * _constB(aA1, aA2) * t + _constC(aA1);
}


static inline float _calcBezier(float t, float aA1, float aA2)
{
    return ((_constA(aA1, aA2) * t + _constB(aA1, aA2)) * t + _constC(aA1)) * t;
}


float LottieInterpolator::getTForX(float aX)
{
    //Найдите интервал, в котором лежит t
    auto intervalStart = 0.0f;
    auto currentSample = &samples[1];
    auto lastSample = &samples[SPLINE_TABLE_SIZE - 1];

    for (; currentSample != lastSample && *currentSample <= aX; ++currentSample) {
        intervalStart += SAMPLE_STEP_SIZE;
    }

    --currentSample;  // t теперь находится между *currentSample и *currentSample+1

    // Интерполируйте, чтобы получить начальное предположение о t
    auto dist = (aX - *currentSample) / (*(currentSample + 1) - *currentSample);
    auto guessForT = intervalStart + dist * SAMPLE_STEP_SIZE;

    // Проверьте наклон, чтобы увидеть, какую стратегию использовать. Если уклон слишком мал
    // Итерация Ньютона-Рафсона не сходится к корню, поэтому мы используем деление пополам.
    // вместо этого.
    auto initialSlope = _getSlope(guessForT, outTangent.x, inTangent.x);
    if (initialSlope >= NEWTON_MIN_SLOPE) return NewtonRaphsonIterate(aX, guessForT);
    else if (initialSlope == 0.0) return guessForT;
    else return binarySubdivide(aX, intervalStart, intervalStart + SAMPLE_STEP_SIZE);
}


float LottieInterpolator::binarySubdivide(float aX, float aA, float aB)
{
    float x, t;
    int i = 0;

    do {
        t = aA + (aB - aA) / 2.0f;
        x = _calcBezier(t, outTangent.x, inTangent.x) - aX;
        if (x > 0.0f) aB = t;
        else aA = t;
    } while (fabsf(x) > SUBDIVISION_PRECISION && ++i < SUBDIVISION_MAX_ITERATIONS);
    return t;
}


float LottieInterpolator::NewtonRaphsonIterate(float aX, float aGuessT)
{
    // Уточните предположение с помощью итерации Ньютона-Рафсона
    for (int i = 0; i < NEWTON_ITERATIONS; ++i) {
        // Мы пытаемся найти, где f(t) = aX,
        // поэтому на самом деле мы ищем корень для: CalcBezier(t) - aX
        auto currentX = _calcBezier(aGuessT, outTangent.x, inTangent.x) - aX;
        auto currentSlope = _getSlope(aGuessT, outTangent.x, inTangent.x);
        if (currentSlope == 0.0f) return aGuessT;
        aGuessT -= currentX / currentSlope;
    }
    return aGuessT;
}


/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/

float LottieInterpolator::progress(float t)
{
    if (outTangent.x == outTangent.y && inTangent.x == inTangent.y) return t;
    return _calcBezier(getTForX(t), outTangent.y, inTangent.y);
}


void LottieInterpolator::set(const char* key, Point& inTangent, Point& outTangent)
{
    this->key = lv_strdup(key);
    this->inTangent = inTangent;
    this->outTangent = outTangent;

    if (outTangent.x == outTangent.y && inTangent.x == inTangent.y) return;

    //вычисляет выборочные значения
    for (int i = 0; i < SPLINE_TABLE_SIZE; ++i) {
        samples[i] = _calcBezier(float(i) * SAMPLE_STEP_SIZE, outTangent.x, inTangent.x);
    }
}

#endif /* LV_USE_THORVG_INTERNAL */

