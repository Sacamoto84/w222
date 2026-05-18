/****************************************************************************
*
*    Лицензия MIT ( MIT )
*
*    Copyright (c) 2014 - 2022 Vivante Corporation
*
*    Разрешение настоящим предоставляется бесплатно любому лицу, получившему
*    копию этого программного обеспечения и связанных с ним файлов документации («Программное обеспечение»),
*    иметь дело с Программным обеспечением без ограничений, включая, помимо прочего,
*    права на использование, копирование, изменение, объединение, публикацию, распространение, сублицензирование,
*    и/или продавать копии Программного обеспечения, а также разрешать лицам, которым
*    Для этого предоставляется программное обеспечение при соблюдении следующих условий:
*
*    Вышеупомянутое уведомление об авторских правах и настоящее уведомление о разрешении должны быть включены в
*    все копии или существенные части Программного обеспечения.
*
*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
*    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
*    DEALINGS IN THE SOFTWARE.
*
*****************************************************************************
*
*    Лицензия GPL ( GPL )
*
*    Copyright (C) 2014 - 2022 Vivante Corporation
*
*    Эта программа является бесплатным программным обеспечением; вы можете распространять его и/или
*    изменить его в соответствии с условиями Стандартной общественной лицензии GNU.
*    как опубликовано Фондом свободного программного обеспечения; либо версия 2
*    Лицензии или (по вашему выбору) любой более поздней версии.
*
*    Данная программа распространяется в надежде, что она будет полезна,
*    но WITHOUT ANY WARRANTY ; даже без подразумеваемой гарантии
*    MERCHANTABILITY или FITNESS FOR A PARTICULAR PURPOSE .  См.
*    Стандартная общественная лицензия GNU для получения более подробной информации.
*
*    Вы должны были получить копию Стандартной общественной лицензии GNU.
*    вместе с этой программой; если нет, напишите в Фонд свободного программного обеспечения,
*    Inc., Франклин-стрит, 51, пятый этаж, Бостон, MA 02110-1301, USA.
*
*****************************************************************************
*
*    Note: This software is released under dual MIT and GPL licenses. A
*    получатель может использовать этот файл на условиях лицензии MIT или
*    Лицензия GPL. Если вы хотите использовать только одну лицензию, а не другую, вы можете
*    сообщите о своем решении, удалив одно из приведенных выше уведомлений о лицензии в своем
*    версия этого файла.
*
*****************************************************************************/

#ifndef VG_LITE_KERNEL_OPTION_H
#define VG_LITE_KERNEL_OPTION_H

#include "../../../lv_conf_internal.h"
#if LV_USE_VG_LITE_DRIVER

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Установите для gcdVG_ENABLE_WRITEBUFFER значение 1 для комбинированного доступа к памяти Linux Write.
 * Установите для gcdVG_ENABLE_WRITEBUFFER значение 0 для доступа к некэшируемой памяти Linux.
*/
#ifndef gcdVG_ENABLE_WRITEBUFFER
#define gcdVG_ENABLE_WRITEBUFFER                     1
#endif

/*
 * Команда состояния резервного копирования, поддержка только Linux и RTOS
*/
#ifndef gcdVG_ENABLE_BACKUP_COMMAND
#define gcdVG_ENABLE_BACKUP_COMMAND                  0
#endif

/*
 * Управление питанием, поддержка только Linux и RTOS
*/
#ifndef gcdVG_ENABLE_POWER_MANAGEMENT
#define gcdVG_ENABLE_POWER_MANAGEMENT                0
#endif

/*
 * если установлено значение 1, vg_lite_hal_trace может использовать для печати сообщения
*/
#ifndef gcdVG_ENABLE_DEBUG
#define gcdVG_ENABLE_DEBUG                           1
#endif

/*
 * если установлено значение 1, выгружать последнюю команду отправки из ядра
*/
#ifndef gcdVG_ENABLE_DUMP_COMMAND
#define gcdVG_ENABLE_DUMP_COMMAND                    0
#endif

/*
 * когда графический процессор зависает, установите 1, чтобы открыть функцию сброса графического процессора
*/
#ifndef gcdVG_ENABLE_GPU_RESET
#define gcdVG_ENABLE_GPU_RESET                       0
#endif

/*
 * Установите 1, чтобы открыть функцию автоматической синхронизации часов графического процессора.
*/
#ifndef gcdVG_ENABLE_AUTO_CLOCK_GATING
#define gcdVG_ENABLE_AUTO_CLOCK_GATING               0
#endif

/*
 * Установите 1, чтобы открыть регистр отладки дампа.
*/
#ifndef gcdVG_DUMP_DEBUG_REGISTER
#define gcdVG_DUMP_DEBUG_REGISTER                    0
#endif

/*
 * Для системы Зефир
 * Установите gcdVG_ENABLE_DELAY_RESUME на 1, чтобы открыть функцию возобновления задержки.
*/
#ifndef gcdVG_ENABLE_DELAY_RESUME
#define gcdVG_ENABLE_DELAY_RESUME                    0
#endif

/*
 * Установите 1 для записи времени работы оборудования GPU.
*/
#ifndef gcdVG_RECORD_HARDWARE_RUNNING_TIME
#define gcdVG_RECORD_HARDWARE_RUNNING_TIME           0
#endif

#ifdef __cplusplus
}
#endif

#endif /* LV_USE_VG_LITE_DRIVER */

#endif /* VG_LITE_KERNEL_OPTION_H */
