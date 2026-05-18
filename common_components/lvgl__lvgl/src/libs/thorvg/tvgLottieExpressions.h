/*
 * Copyright (c) 2024 the ThorVG project. All rights reserved.

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

#ifndef _TVG_LOTTIE_EXPRESSIONS_H_
#define _TVG_LOTTIE_EXPRESSIONS_H_

#include "tvgCommon.h"
#include "tvgLottieCommon.h"

struct LottieExpression;
struct LottieComposition;
struct LottieLayer;
struct LottieRoundnessModifier;
struct LottieOffsetModifier;

#ifdef THORVG_LOTTIE_EXPRESSIONS_SUPPORT

#include "jerryscript.h"


struct LottieExpressions
{
public:
    template<typename Property, typename NumType>
    bool result(float frameNo, NumType& out, LottieExpression* exp)
    {
        auto bm_rt = evaluate(frameNo, exp);
        if (jerry_value_is_undefined(bm_rt)) return false;

        if (jerry_value_is_number(bm_rt)) {
            out = (NumType) jerry_value_as_number(bm_rt);
        } else if (auto prop = static_cast<Property*>(jerry_object_get_native_ptr(bm_rt, nullptr))) {
            out = (*prop)(frameNo);
        }
        jerry_value_free(bm_rt);
        return true;
    }

    template<typename Property>
    bool result(float frameNo, Point& out, LottieExpression* exp)
    {
        auto bm_rt = evaluate(frameNo, exp);
        if (jerry_value_is_undefined(bm_rt)) return false;

        if (auto prop = static_cast<Property*>(jerry_object_get_native_ptr(bm_rt, nullptr))) {
            out = (*prop)(frameNo);
        } else {
            auto x = jerry_object_get_index(bm_rt, 0);
            auto y = jerry_object_get_index(bm_rt, 1);
            out.x = jerry_value_as_number(x);
            out.y = jerry_value_as_number(y);
            jerry_value_free(x);
            jerry_value_free(y);
        }
        jerry_value_free(bm_rt);
        return true;
    }

    template<typename Property>
    bool result(float frameNo, RGB24& out, LottieExpression* exp)
    {
        auto bm_rt = evaluate(frameNo, exp);
        if (jerry_value_is_undefined(bm_rt)) return false;

        if (auto color = static_cast<Property*>(jerry_object_get_native_ptr(bm_rt, nullptr))) {
            out = (*color)(frameNo);
        } else {
            auto r = jerry_object_get_index(bm_rt, 0);
            auto g = jerry_object_get_index(bm_rt, 1);
            auto b = jerry_object_get_index(bm_rt, 2);
            out.rgb[0] = REMAP255(jerry_value_as_number(r));
            out.rgb[1] = REMAP255(jerry_value_as_number(g));
            out.rgb[2] = REMAP255(jerry_value_as_number(b));
            jerry_value_free(r);
            jerry_value_free(g);
            jerry_value_free(b);
        }
        jerry_value_free(bm_rt);
        return true;
    }

    template<typename Property>
    bool result(float frameNo, Fill* fill, LottieExpression* exp)
    {
        auto bm_rt = evaluate(frameNo, exp);
        if (jerry_value_is_undefined(bm_rt)) return false;

        if (auto colorStop = static_cast<Property*>(jerry_object_get_native_ptr(bm_rt, nullptr))) {
            (*colorStop)(frameNo, fill, this);
        }
        jerry_value_free(bm_rt);
        return true;
    }

    template<typename Property>
    bool result(float frameNo, Array<PathCommand>& cmds, Array<Point>& pts, Matrix* transform, const LottieRoundnessModifier* roundness, const LottieOffsetModifier* offsetPath, LottieExpression* exp)
    {
        auto bm_rt = evaluate(frameNo, exp);
        if (jerry_value_is_undefined(bm_rt)) return false;

        if (auto pathset = static_cast<Property*>(jerry_object_get_native_ptr(bm_rt, nullptr))) {
            (*pathset)(frameNo, cmds, pts, transform, roundness, offsetPath);
        }
        jerry_value_free(bm_rt);
        return true;
    }

    void update(float curTime);

    //синглтон (без потокобезопасности)
    static LottieExpressions* instance();
    static void retrieve(LottieExpressions* instance);

private:
    LottieExpressions();
    ~LottieExpressions();

    jerry_value_t evaluate(float frameNo, LottieExpression* exp);
    jerry_value_t buildGlobal();

    void buildComp(LottieComposition* comp, float frameNo, LottieExpression* exp);
    void buildComp(jerry_value_t context, float frameNo, LottieLayer* comp, LottieExpression* exp);
    void buildGlobal(LottieExpression* exp);

    //глобальный объект, атрибуты, методы
    jerry_value_t global;
    jerry_value_t comp;
    jerry_value_t thisComp;
    jerry_value_t thisLayer;
    jerry_value_t thisProperty;
};

#else

struct LottieExpressions
{
    template<typename Property, typename NumType> bool result(TVG_UNUSED float, TVG_UNUSED NumType&, TVG_UNUSED LottieExpression*) { return false; }
    template<typename Property> bool result(TVG_UNUSED float, TVG_UNUSED Point&, LottieExpression*) { return false; }
    template<typename Property> bool result(TVG_UNUSED float, TVG_UNUSED RGB24&, TVG_UNUSED LottieExpression*) { return false; }
    template<typename Property> bool result(TVG_UNUSED float, TVG_UNUSED Fill*, TVG_UNUSED LottieExpression*) { return false; }
    template<typename Property> bool result(TVG_UNUSED float, TVG_UNUSED Array<PathCommand>&, TVG_UNUSED Array<Point>&, TVG_UNUSED Matrix* transform, TVG_UNUSED const LottieRoundnessModifier*, TVG_UNUSED const LottieOffsetModifier*, TVG_UNUSED LottieExpression*) { return false; }
    void update(TVG_UNUSED float) {}
    static LottieExpressions* instance() { return nullptr; }
    static void retrieve(TVG_UNUSED LottieExpressions* instance) {}
};

#endif //THORVG_LOTTIE_EXPRESSIONS_SUPPORT

#endif //_TVG_LOTTIE_EXPRESSIONS_H_

#endif /* LV_USE_THORVG_INTERNAL */

