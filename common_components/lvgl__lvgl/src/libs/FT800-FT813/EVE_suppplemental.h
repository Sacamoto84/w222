/*
@file EVE_supplemental.h
@brief   прототипы дополнительных функций
@version 5.0
@date 23 декабря 2023 г.
@author Рудольф Ридель

@section LICENSE

Лицензия MIT

Copyright (c) 2016-2023 Rudolph Riedel

Разрешение настоящим предоставляется бесплатно любому лицу, получившему копию
данное программное обеспечение и связанные с ним файлы документации («Программное обеспечение») для решения
Программное обеспечение без ограничений, включая, помимо прочего, права
использовать, копировать, изменять, объединять, публиковать, распространять, сублицензировать,
и/или продавать копии Программного обеспечения, а также разрешать лицам, которым Программное обеспечение
предоставлено для этого при соблюдении следующих условий:

Вышеупомянутое уведомление об авторских правах и настоящее уведомление о разрешении должны быть включены во все
копии или существенные части Программного обеспечения.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

История @section

5.0
- добавленEVE_polar_cartesian()

*/

#ifndef EVE_SUPPLEMENTAL_H
#define EVE_SUPPLEMENTAL_H

#include "EVE.h"
#include "EVE_commands.h"

#ifdef __cplusplus
extern "C"
{
#endif

void EVE_widget_circle(int16_t xc0, int16_t yc0, uint16_t radius, uint16_t border, uint32_t bgcolor);
void EVE_widget_rectangle(int16_t xc0, int16_t yc0, int16_t wid, int16_t hgt, int16_t border, uint16_t linewidth, uint32_t bgcolor);
void EVE_polar_cartesian(uint16_t length, uint16_t angle, int16_t *p_xc0, int16_t *p_yc0);

#endif /* EVE_SUPPLEMENTAL_H */
