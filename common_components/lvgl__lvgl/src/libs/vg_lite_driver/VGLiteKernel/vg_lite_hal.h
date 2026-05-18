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

#ifndef VG_LITE_HAL_H
#define VG_LITE_HAL_H

#include "../../../lv_conf_internal.h"
#if LV_USE_VG_LITE_DRIVER

#define VGLITE_MEM_ALIGNMENT      128

#define VGLITE_EVENT_FRAME_END    2

#ifdef __cplusplus
extern "C" {
#endif
/*!
 @brief Wait a number of milliseconds.

 @discussion
 Аппаратное обеспечение VGLite требует некоторого ожидания при изменении тактовой частоты или выполнении сброса. Это функция-обертка
 для функции задержки.

 @param миллисекунды
 Количество миллисекунд ожидания.
 */
void vg_lite_hal_delay(uint32_t milliseconds);

/*!
 @brief Initialize the hardware.

 @discussion
 Ядро VGLite знает, как программировать собственное оборудование, но в любом SOC может потребоваться дополнительный контроль для
 включение питания или инициализация часов. Эта функция вызывается ядром VGLite перед графическим отображением VGLite.
 оборудование инициализируется самим ядром VGLite и позволяет управлять питанием SOC.

 Разработчик должен убедиться, что при выходе из этой функции питание и тактовая частота графического оборудования VGLite равны.
 включился и работает стабильно.
 */
void vg_lite_hal_initialize(void);

/*!
 @brief Uninitialize the hardware.

 @discussion
 Ядро VGLite знает, как программировать собственное оборудование, но в любом SOC может потребоваться дополнительный контроль для
 выключение питания или деинициализация часов. Эта функция вызывается ядром VGLite после того, как VGLite
 графическое оборудование не инициализируется самим ядром VGLite и позволяет управлять питанием SOC.

 При выходе из этой функции можно отключить питание и/или тактовую частоту графического оборудования VGLite.
 */
void vg_lite_hal_deinitialize(void);

/*!
 @brief Allocate contiguous video memory.

 @discussion
 Любая память, которую видит графическое оборудование VGLite, должна быть выделена как непрерывная память. Любая выделенная память будет
 адресуется через непрозрачный дескриптор, обычно указатель на непрозрачную структуру. Уровень переноса может поместить любую информацию.
 потребности внутри этой структуры.

 Размер @param
 Количество байтов, которые необходимо выделить.

 Пул @param
 выберите зарезервированный пул памяти

 @param логический
 Указатель на переменную, которая получит логический адрес выделенной памяти для CPU.

 @param графический процессор
 Указатель на переменную, которая получит физический адрес выделенной памяти для графического оборудования VGLite.

 @result
 Указатель на непрозрачную структуру, которая будет использоваться в качестве дескриптора памяти.  NULL должен быть возвращен, если его нет.
 достаточно памяти.
 */
vg_lite_error_t vg_lite_hal_allocate_contiguous(unsigned long size, vg_lite_vidmem_pool_t pool, void ** logical,
                                                void ** klogical, uint32_t * physical, void ** node);

/*!
 @brief Free contiguous video memory.

 @discussion
 Освободите память, выделенную { @link vg_lite_hal_allocate_contiguous }. После возврата из этой функции соответствующая память
 handle больше не является допустимым дескриптором.

 @param memory_handle
 Указатель на непрозрачную структуру, возвращаемую { @link vg_lite_hal_allocate_contiguous }.
 */
void vg_lite_hal_free_contiguous(void * memory_handle);

/*!
 @brief remove unfree node when continuously allocate buffer without free buffer.

 @discussion
 Освободите узел, выделенный { @link kmalloc}. После возврата из этой функции соответствующая память
 handle больше не является допустимым дескриптором.
 */
void vg_lite_hal_free_os_heap(void);

/*!
 @brief Map contiguous logical or physical memory into the VGLite graphics hardware space.

 @discussion
 Любая память, например буфер кадра или некоторые заранее выделенные изображения или данные пути, должна быть отображена в графику VGLite.
 аппаратное адресное пространство и обернуто дескриптором памяти. Это позволяет графическому оборудованию VGLite получить доступ к этой памяти.
 напрямую.

 Для сопоставления необходимо передать логический или физический адрес.

 Размер @param
 Количество байтов для сопоставления.

 @param логический
 Логический адрес области памяти для сопоставления или NULL, если логический адрес неизвестен.

 @param физический
 Физический адрес области памяти для сопоставления, если она логическая, — NULL.

 @param графический процессор
 Указатель на переменную, которая получит адресный адрес графического оборудования VGLite отображаемой области.

 @result
 Указатель на непрозрачную структуру, которая будет использоваться в качестве дескриптора памяти.  NULL должен быть возвращен, если есть
 недостаточно системных ресурсов для картирования региона.
 */
void * vg_lite_hal_map(uint32_t flags, uint32_t bytes, void * logical, uint32_t physical, int32_t dma_buf_fd,
                       uint32_t * gpu);

/*!
 @brief Unmap a previously mapped region.

 @discussion
 Если сопоставленный регион с помощью { @link vg_lite_hal_map } больше не нужен, его следует отменить, чтобы освободить выделенную систему.
 ресурсы, используемые при картографировании региона.

 @param memory_handle
 Указатель на непрозрачную структуру, возвращаемую { @link vg_lite_hal_map }.
 */
void vg_lite_hal_unmap(void * memory_handle);

/*!
 @brief Execute a memory barrier.

 @discussion
 В некоторых системах требуется барьер памяти, чтобы гарантировать обработку всех операций сохранения в CPU. Это обертка
 Функция барьера памяти.
 */
void vg_lite_hal_barrier(void);

/*!
 @brief Read data from a register from the VGLite graphics hardware.

 @discussion
 Для связи с графическим оборудованием VGLite ядру необходимо выполнять чтение и запись в некоторые аппаратные регистры.
 В каждом SOC эти регистры могут быть размещены в разных местах карты физической памяти.

 @param адрес
 Относительный адрес регистра графического оборудования VGLite для чтения.

 @result
 32-битное значение, полученное после чтения регистра.
 */
uint32_t vg_lite_hal_peek(uint32_t address);

/*!
 @brief Write data to a register from the VGLite graphics hardware.

 @discussion
 Для связи с графическим оборудованием VGLite ядру необходимо выполнять чтение и запись в некоторые аппаратные регистры.
 В каждом SOC эти регистры могут быть размещены в разных местах карты физической памяти.

 @param адрес
 Относительный адрес регистра графического оборудования VGLite, в который осуществляется запись.

 Данные @param
 Данные для записи в регистр графического оборудования VGLite.
 */
void vg_lite_hal_poke(uint32_t address, uint32_t data);

/*!
 @brief query the remaining allocate contiguous video memory.

 Данные @param
 Для получения оставшихся данных выделяются смежные байты видеопамяти.
 */
vg_lite_error_t vg_lite_hal_query_mem(vg_lite_kernel_mem_t * mem);

/*!
 @brief Map contiguous physical memory into the user space.

 узел @param
 Этот узел имеет 3 атрибута, байты означают количество байтов для сопоставления.
 физическое означает физический адрес области памяти для отображения.логическое средство
 обратный логический адрес области памяти после карты.
 */
vg_lite_error_t vg_lite_hal_map_memory(vg_lite_kernel_map_memory_t * node);

/*!
 @brief Unmap a previously mapped region.

 узел @param
 Этот узел имеет 2 атрибута, байты означают количество байтов для unmap.логическое значение.
 логический адрес области памяти, которую необходимо отменить.
 */
vg_lite_error_t vg_lite_hal_unmap_memory(vg_lite_kernel_unmap_memory_t * node);

/*!
 @brief Wait until an interrupt from the VGLite graphics hardware has been received.

 @discussion
 В настоящее время VGLite API является синхронным. Это означает, что после каждого вызова он будет ждать, пока графическое оборудование VGLite не
 завершилось. Графическое оборудование VGLite отправит прерывание по завершении, и эта функция будет ждать, пока
 это прерывание было получено операционной системой.

 Значение тайм-аута указывается, если ядро хочет подождать определенное количество миллисекунд, прежде чем прерывание
 произойти. Если прерывание не произойдет в течение указанного таймаута, будет возвращена ошибка таймаута.

 Тайм-аут @param
 Количество миллисекунд ожидания прерывания перед возвратом ошибки тайм-аута. Если тайм-аут = 0xFFFFFFFF
 тогда { @link vg_lite_hal_wait_interrupt } будет вечно ждать прерывания.

 Маска @param
 Маска события Irq, которого следует ожидать.

 @result
 Логическое значение, указывающее, было ли получено прерывание (1) или нет (0).
 */
int32_t vg_lite_hal_wait_interrupt(uint32_t timeout, uint32_t mask, uint32_t * value);

/*!
 @brief After call vg_lite_hal_map(), flush cpu cache according the direction
 указывается параметром cache_op.
 */
vg_lite_error_t vg_lite_hal_operation_cache(void * handle, vg_lite_cache_op_t cache_op);

/*!
 @brief export memory to dma buf, and get the dma buf fd
 */
vg_lite_error_t vg_lite_hal_memory_export(int32_t * fd);

/*!
 @brief print message
 */
void vg_lite_hal_print(char * format, ...);

/*!
 @brief trace message
 */
void vg_lite_hal_trace(char * format, ...);

/*!
 @brief error number to string
 */
const char * vg_lite_hal_Status2Name(vg_lite_error_t status);

/*!
 @brief allocate virtual memory from os
 */
vg_lite_error_t vg_lite_hal_allocate(uint32_t size, void ** memory);

/*!
 @brief free virtual memory
 */
vg_lite_error_t vg_lite_hal_free(void * memory);

/*!
 @brief set gpu execute state
 */
void vg_lite_set_gpu_execute_state(vg_lite_gpu_execute_state_t state);

#ifdef __cplusplus
}
#endif

#endif /* LV_USE_VG_LITE_DRIVER */

#endif /* VG_LITE_HAL_H */


