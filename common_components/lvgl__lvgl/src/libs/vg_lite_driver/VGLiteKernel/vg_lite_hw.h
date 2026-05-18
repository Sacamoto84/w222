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
*    Inc., Франклин-стрит, 51, пятый этаж, Бостон,MA02110-1301,USA.
*
*****************************************************************************
*
*    Note: Это программное обеспечение выпускается под двойными лицензиямиMITи GPL. А
*    получатель может использовать этот файл на условиях лицензии MIT или
*    Лицензия GPL. Если вы хотите использовать только одну лицензию, а не другую, вы можете
*    сообщите о своем решении, удалив одно из приведенных выше уведомлений о лицензии в своем
*    версия этого файла.
*
*****************************************************************************/

#ifndef VG_LITE_HW_H
#define VG_LITE_HW_H

#include "../../../lv_conf_internal.h"
#if LV_USE_VG_LITE_DRIVER

#define VG_LITE_HW_CLOCK_CONTROL     0x000
#define VG_LITE_HW_IDLE              0x004
#define VG_LITE_INTR_STATUS          0x010
#define VG_LITE_INTR_ENABLE          0x014
#define VG_LITE_HW_CHIP_ID           0x020
#define VG_LITE_HW_CMDBUF_ADDRESS    0x500
#define VG_LITE_HW_CMDBUF_SIZE       0x504
#define VG_LITE_POWER_CONTROL        0x100
#define VG_LITE_POWER_MODULE_CONTROL 0x104

#define VG_LITE_EXT_WORK_CONTROL     0x520
#define VG_LITE_EXT_VIDEO_SIZE       0x524
#define VG_LITE_EXT_CLEAR_VALUE      0x528

#define VG_LITE_EXT_VIDEO_CONTROL    0x51C

typedef struct clock_control {
    uint32_t reserved0 : 1;
    uint32_t clock_gate : 1;
    uint32_t scale : 7;
    uint32_t scale_load : 1;
    uint32_t ram_clock_gating : 1;
    uint32_t debug_registers : 1;
    uint32_t soft_reset : 1;
    uint32_t reserved13 : 6;
    uint32_t isolate : 1;
} clock_control_t;

typedef union vg_lite_hw_clock_control {
    clock_control_t control;
    uint32_t        data;
} vg_lite_hw_clock_control_t;

#define VG_LITE_HW_IDLE_STATE       0x0B05

#endif /* LV_USE_VG_LITE_DRIVER */

#endif /* VG_LITE_HW_H */
