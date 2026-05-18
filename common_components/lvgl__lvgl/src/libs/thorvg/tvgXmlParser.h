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

#ifndef _TVG_SIMPLE_XML_PARSER_H_
#define _TVG_SIMPLE_XML_PARSER_H_

#include "tvgSvgLoaderCommon.h"

#define NUMBER_OF_XML_ENTITIES 9
const char* const xmlEntity[] = {"&#10;", "&quot;", "&nbsp;", "&apos;", "&amp;", "&lt;", "&gt;", "&#035;", "&#039;"};
const int xmlEntityLength[] = {5, 6, 6, 6, 5, 4, 4, 6, 6};

enum class SimpleXMLType
{
    Open = 0,     //!< \<tag атрибут="значение"\>
    OpenEmpty,    //!< \<tag атрибут="значение" /\>
    Close,        //!< \</тег\>
    Data,         //!< текстовые данные тега
    CData,        //!< \<![cdata[что-то]]\>
    Error,        //!< содержимое ошибки
    Processing,   //!< \<?xml ... ?\> \<?php .. ?\>
    Doctype,      //!< \<!doctype html
    Comment,      //!< \<!-- что-то --\>
    Ignored,      //!< все, что игнорируется парсером, например пробелы
    DoctypeChild  //!< \<!  doctype_child
};

typedef bool (*simpleXMLCb)(void* data, SimpleXMLType type, const char* content, unsigned int length);
typedef bool (*simpleXMLAttributeCb)(void* data, const char* key, const char* value);

bool simpleXmlParseAttributes(const char* buf, unsigned bufLength, simpleXMLAttributeCb func, const void* data);
bool simpleXmlParse(const char* buf, unsigned bufLength, bool strip, simpleXMLCb func, const void* data);
bool simpleXmlParseW3CAttribute(const char* buf, unsigned bufLength, simpleXMLAttributeCb func, const void* data);
const char* simpleXmlParseCSSAttribute(const char* buf, unsigned bufLength, char** tag, char** name, const char** attrs, unsigned* attrsLength);
const char* simpleXmlFindAttributesTag(const char* buf, unsigned bufLength);
bool isIgnoreUnsupportedLogElements(const char* tagName);
const char* simpleXmlNodeTypeToString(SvgNodeType type);

#endif //_TVG_SIMPLE_XML_PARSER_H_

#endif /* LV_USE_THORVG_INTERNAL */

