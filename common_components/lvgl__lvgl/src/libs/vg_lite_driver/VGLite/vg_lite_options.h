/****************************************************************************
*
*    Copyright 2012 - 2023 Vivante Corporation, Santa Clara, California.
*    Все права защищены.
*
*    Разрешение настоящим предоставляется бесплатно любому лицу, получившему
*    копию этого программного обеспечения и связанных с ним файлов документации (файл
*    «Программное обеспечение»), иметь дело с Программным обеспечением без ограничений, включая
*    без ограничений права на использование, копирование, изменение, объединение, публикацию,
*    распространять, сублицензировать и/или продавать копии Программного обеспечения, а также
*    разрешать лицам, которым предоставлено Программное обеспечение, делать это при условии, что
*    на следующие условия:
*
*    Вышеупомянутое уведомление об авторских правах и данное уведомление о разрешении (включая
*    следующий абзац) должны быть включены во все копии или существенные
*    части Программного обеспечения.
*
*    THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
*    IN NO EVENT SHALL VIVANTE AND/OR ITS SUPPLIERS BE LIABLE FOR ANY
*    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
*    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
*    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*****************************************************************************/

#ifndef VG_LITE_OPTIONS_DISPATCH_H
#define VG_LITE_OPTIONS_DISPATCH_H

#include "../../../lv_conf_internal.h"
#if LV_USE_VG_LITE_DRIVER

    #define VG_LITE_OPTIONS VG_LITE_OPTIONS_2
    #define VG_LITE_OPTIONS_2  <../VGLite/Series/LV_VG_LITE_HAL_GPU_SERIES/LV_VG_LITE_HAL_GPU_REVISION/vg_lite_options.h>
    #include VG_LITE_OPTIONS

#endif /* LV_USE_VG_LITE_DRIVER */

#endif /* VG_LITE_OPTIONS_DISPATCH_H */
