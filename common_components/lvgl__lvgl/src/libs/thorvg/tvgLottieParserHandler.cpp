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

#include "tvgLottieParserHandler.h"


/************************************************************************/
/* Реализация внутреннего класса                                        */
/************************************************************************/

static const int PARSE_FLAGS = kParseDefaultFlags | kParseInsituFlag;


/************************************************************************/
/* Реализация внешнего класса                                        */
/************************************************************************/


bool LookaheadParserHandler::enterArray()
{
    if (state != kEnteringArray) {
        Error();
        return false;
    }
    parseNext();
    return true;
}


bool LookaheadParserHandler::nextArrayValue()
{
    if (state == kExitingArray) {
        parseNext();
        return false;
    }
    //SPECIALCASE: то же самое, что nextObjectKey()
    if (state == kExitingObject) return false;
    if (state == kError || state == kHasKey) {
        Error();
        return false;
    }
    return true;
}


int LookaheadParserHandler::getInt()
{
    if (state != kHasNumber) {
        Error();
        return 0;
    }
    auto result = val.GetInt();
    parseNext();
    return result;
}


float LookaheadParserHandler::getFloat()
{
    if (state != kHasNumber) {
        Error();
        return 0;
    }
    auto result = val.GetFloat();
    parseNext();
    return result;
}


const char* LookaheadParserHandler::getString()
{
    if (state != kHasString) {
        Error();
        return nullptr;
    }
    auto result = val.GetString();
    parseNext();
    return result;
}


char* LookaheadParserHandler::getStringCopy()
{
    auto str = getString();
    if (str) return lv_strdup(str);
    return nullptr;
}


bool LookaheadParserHandler::getBool()
{
    if (state != kHasBool) {
        Error();
        return false;
    }
    auto result = val.GetBool();
    parseNext();
    return result;
}


void LookaheadParserHandler::getNull()
{
    if (state != kHasNull) {
        Error();
        return;
    }
    parseNext();
}


bool LookaheadParserHandler::parseNext()
{
    if (reader.HasParseError()) {
        Error();
        return false;
    }
    if (!reader.IterativeParseNext<PARSE_FLAGS>(iss, *this)) {
        Error();
        return false;
    }
    return true;
}


bool LookaheadParserHandler::enterObject()
{
    if (state != kEnteringObject) {
        Error();
        return false;
    }
    parseNext();
    return true;
}


int LookaheadParserHandler::peekType()
{
    if (state >= kHasNull && state <= kHasKey) return val.GetType();
    if (state == kEnteringArray) return kArrayType;
    if (state == kEnteringObject) return kObjectType;
    return -1;
}


void LookaheadParserHandler::skipOut(int depth)
{
    do {
        if (state == kEnteringArray || state == kEnteringObject) ++depth;
        else if (state == kExitingArray || state == kExitingObject) --depth;
        else if (state == kError) return;
        parseNext();
    } while (depth > 0);
}


const char* LookaheadParserHandler::nextObjectKey()
{
    if (state == kHasKey) {
        auto result = val.GetString();
        parseNext();
        return result;
    }

    /* SPECIAL CASE : Парсер работает по предопределенному правилу, согласно которому он будет
       while (nextObjectKey()) для каждого объекта, но в случае нашей вложенной группы
       объект, который мы могли вызвать несколько разnextObjectKey()при выходе из объекта
       поэтому игнорируйте их и не переводите парсер в состояние ошибки. */
    if (state == kExitingArray || state == kEnteringObject) return nullptr;

    if (state != kExitingObject) {
        Error();
        return nullptr;
    }

    parseNext();
    return nullptr;
}


void LookaheadParserHandler::skip(const char* key)
{
    //if (ключ) TVGLOG (" LOTTIE ", "Пропущенное значение синтаксического анализа = %s", ключ);

    if (peekType() == kArrayType) {
        enterArray();
        skipOut(1);
    } else if (peekType() == kObjectType) {
        enterObject();
        skipOut(1);
    } else {
        skipOut(0);
    }
}

#endif /* LV_USE_THORVG_INTERNAL */

