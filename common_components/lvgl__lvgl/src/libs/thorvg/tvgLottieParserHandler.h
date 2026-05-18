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

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd. All rights reserved.

 * Разрешение настоящим предоставляется бесплатно любому лицу, получившему копию.
 * данного программного обеспечения и связанных с ним файлов документации («Программное обеспечение») для решения
 * в Программном обеспечении без ограничений, включая, помимо прочего, права
 * использовать, копировать, изменять, объединять, публиковать, распространять, сублицензировать и/или продавать
 * копий Программного обеспечения и разрешать лицам, которым Программное обеспечение
 * предоставлено для этого при соблюдении следующих условий:

 * Вышеупомянутое уведомление об авторских правах и настоящее уведомление о разрешении должны быть включены в
 * все копии или существенные части Программного обеспечения.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _TVG_LOTTIE_PARSER_HANDLER_H_
#define _TVG_LOTTIE_PARSER_HANDLER_H_

#include "rapidjson/document.h"
#include "tvgCommon.h"


using namespace rapidjson;


struct LookaheadParserHandler
{
    enum LookaheadParsingState {
        kInit = 0,
        kError,
        kHasNull,
        kHasBool,
        kHasNumber,
        kHasString,
        kHasKey,
        kEnteringObject,
        kExitingObject,
        kEnteringArray,
        kExitingArray
    };

    Value                   val;
    LookaheadParsingState   state = kInit;
    Reader                  reader;
    InsituStringStream      iss;

    LookaheadParserHandler(const char *str) : iss((char*)str)
    {
        reader.IterativeParseInit();
    }

    bool Null()
    {
        state = kHasNull;
        val.SetNull();
        return true;
    }

    bool Bool(bool b)
    {
        state = kHasBool;
        val.SetBool(b);
        return true;
    }

    bool Int(int i)
    {
        state = kHasNumber;
        val.SetInt(i);
        return true;
    }

    bool Uint(unsigned u)
    {
        state = kHasNumber;
        val.SetUint(u);
        return true;
    }

    bool Int64(int64_t i)
    {
        state = kHasNumber;
        val.SetInt64(i);
        return true;
    }

    bool Uint64(int64_t u)
    {
        state = kHasNumber;
        val.SetUint64(u);
        return true;
    }

    bool Double(double d)
    {
        state = kHasNumber;
        val.SetDouble(d);
        return true;
    }

    bool RawNumber(const char *, SizeType, TVG_UNUSED bool)
    { 
        return false;
    }

    bool String(const char *str, SizeType length, TVG_UNUSED bool)
    {
        state = kHasString;
        val.SetString(str, length);
        return true;
    }

    bool StartObject()
    {
        state = kEnteringObject;
        return true;
    }

    bool Key(const char *str, SizeType length, TVG_UNUSED bool)
    {
        state = kHasKey;
        val.SetString(str, length);
        return true;
    }

    bool EndObject(SizeType)
    {
        state = kExitingObject;
        return true;
    }

    bool StartArray()
    {
        state = kEnteringArray;
        return true;
    }

    bool EndArray(SizeType)
    {
        state = kExitingArray;
        return true;
    }

    void Error()
    {
        TVGERR("LOTTIE", "Parsing Error!");
        state = kError;
    }

    bool Invalid()
    {
        return state == kError;
    }

    bool enterObject();
    bool enterArray();
    bool nextArrayValue();
    int getInt();
    float getFloat();
    const char* getString();
    char* getStringCopy();
    bool getBool();
    void getNull();
    bool parseNext();
    const char* nextObjectKey();
    void skip(const char* key);
    void skipOut(int depth);
    int peekType();
};

#endif //_TVG_LOTTIE_PARSER_HANDLER_H_

#endif /* LV_USE_THORVG_INTERNAL */

