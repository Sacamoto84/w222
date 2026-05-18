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

#ifndef VG_LITE_KERNEL_H
#define VG_LITE_KERNEL_H

#include "../../../lv_conf_internal.h"
#if LV_USE_VG_LITE_DRIVER

#include "../VGLite/vg_lite_options.h"
#include "vg_lite_option.h"

/* Идентификаторы прерываний из GPU. */
#define EVENT_UNEXPECTED_MESH  0x80000000
#define EVENT_CMD_BAD_WRITE    0x40000000
#define EVENT_ERROR_RECOVER    0x20000000
#define EVENT_CMD_SWITCH       0x10000000
#define EVENT_MCU_BAD_WRITE    0x08000000
#define EVENT_END              0
#define EVENT_FRAME_END        1

#define MAX_CONTIGUOUS_SIZE 0x04000000

#define VG_LITE_INFINITE    0xFFFFFFFF

#if gcFEATURE_VG_SINGLE_COMMAND_BUFFER
    #define CMDBUF_COUNT        1
#else
    #define CMDBUF_COUNT        2
#endif

#define VG_LITE_ALIGN(number, alignment)    \
    (((number) + ((alignment) - 1)) & ~((alignment) - 1))

#ifndef  BIT
    #define  BIT(x)                 (1 << x)
#endif

#define VG_LITE_KERNEL_IS_GPU_IDLE() \
    ((vg_lite_hal_peek(VG_LITE_HW_IDLE) & VG_LITE_HW_IDLE_STATE) == VG_LITE_HW_IDLE_STATE)

/* Идентификаторы аппаратных чипов */
#define GPU_CHIP_ID_GCNANOLITEV         0x255
#define GPU_CHIP_ID_GC355               0x355
#define GPU_CHIP_ID_GCNANOULTRAV        0x265

/* Тип флага vg_lite_kernel_map_t */
#define VG_LITE_HAL_MAP_DMABUF          0x00000004
#define VG_LITE_HAL_MAP_USER_MEMORY     0x00000008
#define VG_LITE_HAL_ALLOC_4G            0x00000010

/* Тип флага vg_lite_kernel_allocate_t */
#define VG_LITE_RESERVED_ALLOCATOR      0x10000000
#define VG_LITE_GFP_ALLOCATOR           0x20000000
#define VG_LITE_DMA_ALLOCATOR           0x40000000
#define VG_LITE_MEMORY_ALLOCATOR_FLAG   0x70000000

#ifdef __cplusplus
extern "C" {
#endif

#ifndef VG_LITE_ERROR
#define VG_LITE_ERROR  1
/*!
    @abstract Коды ошибок, которые могут возвращать функции vg_lite.

    @discussion
    Все функции API возвращают код состояния. В случае успеха VG_LITE_SUCCESS будет возвращен, когда функция будет выполнена.
    успешный. Это значение установлено равным нулю, поэтому, если какая-либо функция возвращает ненулевое значение, произошла ошибка.
    */
typedef enum vg_lite_error {
    VG_LITE_SUCCESS = 0,          /*! Success.                                         */
    VG_LITE_INVALID_ARGUMENT,     /*! An invalid argument was specified.               */
    VG_LITE_OUT_OF_MEMORY,        /*! Out of GPU memory                                */
    VG_LITE_NO_CONTEXT,           /*! No context or an unintialized context specified. */
    VG_LITE_TIMEOUT,              /*! A timeout has occurred during a wait.            */
    VG_LITE_OUT_OF_RESOURCES,     /*! Out of system resources.                         */
    VG_LITE_GENERIC_IO,           /*! Cannot communicate with the kernel driver.       */
    VG_LITE_NOT_SUPPORT,          /*! Function call not supported.                     */
    VG_LITE_ALREADY_EXISTS,       /*! Object already exists                            */
    VG_LITE_NOT_ALIGNED,          /*! Data alignment error                             */
    VG_LITE_FLEXA_TIME_OUT,       /*! VG timeout requesting for segment buffer         */
    VG_LITE_FLEXA_HANDSHAKE_FAIL, /*! VG and SBI synchronizer handshake failed         */
    VG_LITE_SYSTEM_CALL_FAIL,     /*! kernel api call fail                             */
}
vg_lite_error_t;
#endif

typedef enum vg_lite_kernel_counter {
    /* Не прикасайтесь к стойке. */
    VG_LITE_NONE,

    /* Включите счетчик. */
    VG_LITE_ON,

    /* Выключите счетчик. */
    VG_LITE_OFF,

    /* Запросите счетчик и сбросьте его значения. */
    VG_LITE_QUERY,
}
vg_lite_kernel_counter_t;

typedef enum vg_lite_kernel_command {
    /* Инициализируйте GPU. */
    VG_LITE_INITIALIZE,

    /* Завершите GPU . */
    VG_LITE_TERMINATE,

    /* Выделить память. */
    VG_LITE_ALLOCATE,

    /* Свободная память. */
    VG_LITE_FREE,

    /* Отправьте буфер команд в GPU. */
    VG_LITE_SUBMIT,

    /* Подождите, пока GPU завершится. */
    VG_LITE_WAIT,

    /* Сбросьте GPU. */
    VG_LITE_RESET,

    /* Команды отладки. */
    VG_LITE_DEBUG,

    /* Карта памяти. */
    VG_LITE_MAP,

    /* Отключить память. */
    VG_LITE_UNMAP,

    /* Проверьте информацию. */
    VG_LITE_CHECK,

    /* Запросить память. */
    VG_LITE_QUERY_MEM,

    /* Флекса отключить */
    VG_LITE_FLEXA_DISABLE,

    /* Flexa включить */
    VG_LITE_FLEXA_ENABLE,

    /* Флекса стопорная рамка */
    VG_LITE_FLEXA_STOP_FRAME,

    /* Установить фоновый адрес */
    VG_LITE_FLEXA_SET_BACKGROUND_ADDRESS,

    /* Сопоставить память пользователю */
    VG_LITE_MAP_MEMORY,

    /* Отключить память для пользователя */
    VG_LITE_UNMAP_MEMORY,

    /* Закрыть графический процессор */
    VG_LITE_CLOSE,

    /* Кэш операций */
    VG_LITE_CACHE,

    /* Экспортировать память */
    VG_LITE_EXPORT_MEMORY,

    /* Запись времени работы оборудования GPU */
    VG_LITE_RECORD_RUNNING_TIME,

    /* Установить состояние задержки возобновления */
    VG_LITE_SET_DELAY_RESUME,

    /* Состояние возобновления задержки запроса */
    VG_LITE_QUERY_DELAY_RESUME,

    /* Установите состояние часов GPU */
    VG_LITE_SET_GPU_CLOCK_STATE,

}
vg_lite_kernel_command_t;

typedef enum vg_lite_cache_op {
    VG_LITE_CACHE_CLEAN,
    VG_LITE_CACHE_INVALIDATE,
    VG_LITE_CACHE_FLUSH,
}
vg_lite_cache_op_t;

typedef enum vg_lite_vidmem_pool {
    VG_LITE_POOL_RESERVED_MEMORY1 = 0,
    VG_LITE_POOL_RESERVED_MEMORY2 = 1,
}
vg_lite_vidmem_pool_t;

typedef enum vg_lite_gpu_execute_state {
    VG_LITE_GPU_STOP = 0,
    VG_LITE_GPU_RUN  = 1,
}
vg_lite_gpu_execute_state_t;

/* Структура контекста. */
typedef struct vg_lite_kernel_context {
    /* Буфер команд. */
    void           *          command_buffer[CMDBUF_COUNT];
    void           *          command_buffer_logical[CMDBUF_COUNT];
    void           *          command_buffer_klogical[CMDBUF_COUNT];
    uint32_t                  command_buffer_physical[CMDBUF_COUNT];
    uint32_t                  end_of_frame;

    /* Буфер тесселяции. */
    void           *          tess_buffer;
    void           *          tessbuf_logical;
    void           *          tessbuf_klogical;
    uint32_t                  tessbuf_physical;

    /* буфер контекста мощности  */
    void           *          power_context;
    void           *          power_context_logical;
    void           *          power_context_klogical;
    uint32_t                  power_context_physical;
    uint32_t                  power_context_size;
    uint32_t                  power_context_capacity;
}
vg_lite_kernel_context_t;

typedef struct capabilities {
    uint32_t tiled : 2;
    uint32_t l2_cache : 1;
}
capabilities_t;

typedef union vg_lite_capabilities {
    capabilities_t cap;
    uint32_t       data;
}
vg_lite_capabilities_t;

typedef struct vg_lite_kernel_initialize {
    /* INPUT */

    /* Размер буфера команд. */
    uint32_t command_buffer_size;

    /* Ширина буфера тесселяции. */
    int32_t tess_width;

    /* Высота буфера тесселяции. */
    int32_t tess_height;

    /* Пул памяти для буфера команд. */
    vg_lite_vidmem_pool_t command_buffer_pool;

    /* Пул памяти для буфера тесселяции. */
    vg_lite_vidmem_pool_t tess_buffer_pool;

    /* OUTPUT */

    /* Указатель контекста. */
    vg_lite_kernel_context_t * context;

    /* Возможности. */
    vg_lite_capabilities_t capabilities;

    /* Выделенный командный буфер. */
    void * command_buffer[CMDBUF_COUNT];

    /* GPU адрес буфера команд. */
    uint32_t command_buffer_gpu[CMDBUF_COUNT];

    /* GPU адреса для буферов тесселяции. */
    uint32_t physical_addr;

    /* Логические адреса для буферов тесселяции: используются SW Tesselator. */
    uint8_t * logical_addr;

    /* Размер каждого уровня буфера тесселяции. */
    uint32_t tessbuf_size;

    /* Размер каждого уровня буфера счетчика vg. */
    uint32_t countbuf_size;

    /* Ширина и высота буфера тесселяции. */
    uint32_t tess_w_h;
}
vg_lite_kernel_initialize_t;

typedef struct vg_lite_kernel_terminate {
    /* Контекст для завершения. */
    vg_lite_kernel_context_t * context;
}
vg_lite_kernel_terminate_t;

typedef struct vg_lite_kernel_allocate {
    /* INPUT */

    /* Количество байтов, которые необходимо выделить. */
    uint32_t bytes;

    /* Флаг, указывающий, является ли выделенная память непрерывной или нет. */
    int32_t contiguous;

    /* Флаг, указывающий, где выделить память.  */
    uint32_t flags;

    /* выбрать пул зарезервированной памяти */
    vg_lite_vidmem_pool_t pool;

    /* OUTPUT */

    /* Ручка памяти. */
    void * memory_handle;

    /* Выделенная память. */
    void * memory;

    /* память ядра */
    void * kmemory;

    /* GPU адрес выделенной памяти. */
    uint32_t memory_gpu;
}
vg_lite_kernel_allocate_t;

typedef struct vg_lite_kernel_free {
    /* Дескриптор памяти для освобождения. */
    void * memory_handle;
}
vg_lite_kernel_free_t;

typedef struct vg_lite_kernel_submit {
    /* Контекст для отправки. */
    vg_lite_kernel_context_t * context;

    /* Указатель на буфер команд. */
    void * commands;

    /* Количество байтов в буфере команд. */
    uint32_t command_size;

    /* Буфер команд ID . */
    uint32_t command_id;
}
vg_lite_kernel_submit_t;

typedef enum vg_lite_gpu_reset_type {
    RESTORE_INIT_COMMAND = 0,
    RESTORE_LAST_COMMAND = 1,
    RESTORE_ALL_COMMAND  = 2,
    RESTORE_NONE         = 3,
}
vg_lite_gpu_reset_type_t;

typedef struct vg_lite_kernel_wait {
    /* Контекст ожидания. */
    vg_lite_kernel_context_t * context;

    /* Тайм-аут в миллисекундах. */
    uint32_t timeout_ms;

    /* Событие, которое стоит подождать. */
    uint32_t event_mask;

    /* Событие(я) получено после ожидания. */
    uint32_t event_got;

    /* После сброса GPU выберите команду отправки. */
    vg_lite_gpu_reset_type_t reset_type;
}
vg_lite_kernel_wait_t;

typedef struct vg_lite_kernel_reset {
    /* Контекст для сброса. */
    vg_lite_kernel_context_t * context;
    uint32_t delay_resume_flag;
}
vg_lite_kernel_reset_t;

typedef struct vg_lite_kernel_debug {
    /* Контекст для отладки. */
    vg_lite_kernel_context_t * context;

    /* Включение счетчика пропускной способности. */
    vg_lite_kernel_counter_t bandwidth_counter;

    /* Включатель счетчика пикселей. */
    vg_lite_kernel_counter_t pixel_counters;

    /* OUTPUT */

    /* Счетчики пропускной способности:
     *  [0] — пакет из 8.
     *  [1] - серия из 16.
     *  [2] - очередь из 32.
     *  [3] - взрыв 64.
     */
    uint32_t bandwidth[4];

    /* Счетчики пикселей:.
     *  [0] — количество мозаичных пикселей.
     *  [1] - Количество отображаемых пикселей.
     *  [2] — Количество отображаемых пикселей.
     */
    uint32_t pixels[3];
}
vg_lite_kernel_debug_t;

typedef struct vg_lite_kernel_map {
    /* INPUT */
    uint32_t flags;

    /* пользовательская память */
    /* Количество байтов для сопоставления. */
    uint32_t bytes;

    /* Адрес логической памяти или NULL. */
    void * logical;

    /* Адрес физической памяти или 0. */
    uint32_t physical;

    /* dma_buf */
    /* dma_buf фд */
    int32_t dma_buf_fd;

    /* OUTPUT */
    /* Дескриптор отображаемой памяти. */
    void * memory_handle;

    /* GPU адрес отображаемой памяти. */
    uint32_t memory_gpu;
}
vg_lite_kernel_map_t;

typedef struct vg_lite_kernel_unmap {
    /* Дескриптор памяти для отмены сопоставления. */
    void * memory_handle;
}
vg_lite_kernel_unmap_t;

typedef struct vg_lite_kernel_cache {
    vg_lite_cache_op_t cache_op;

    /* Дескриптор памяти для операции. */
    void * memory_handle;
}
vg_lite_kernel_cache_t;

typedef struct vg_lite_kernel_info {
    /* Адрес регистратуры. */
    uint32_t addr;

    /* Проверьте регистрационную информацию. */
    uint32_t reg;
}
vg_lite_kernel_info_t;

typedef struct vg_lite_kernel_flexa_info {
    uint32_t                    sbi_mode;
    uint32_t                    sync_mode;
    uint32_t                    flexa_mode;
    uint32_t                    stream_id;
    uint32_t                    segment_address;
    uint32_t                    segment_count;
    uint32_t                    segment_size;
    uint32_t                    stop_flag;
    uint32_t                    start_flag;
    uint32_t                    reset_flag;
}
vg_lite_kernel_flexa_info_t;

typedef struct vg_lite_kernel_mem {
    uint32_t bytes;

    vg_lite_vidmem_pool_t pool;
}
vg_lite_kernel_mem_t;

typedef struct vg_lite_kernel_map_memory {
    /* Количество байтов для сопоставления. */
    uint32_t bytes;

    /* Адрес физической памяти. */
    uint32_t physical;

    /* Адрес логической памяти. */
    void * logical;
}
vg_lite_kernel_map_memory_t;

typedef struct vg_lite_kernel_unmap_memory {
    /* Количество байтов для сопоставления. */
    uint32_t bytes;

    /* Адрес логической памяти. */
    void * logical;
}
vg_lite_kernel_unmap_memory_t;

typedef struct vg_lite_kernel_close {
    vg_lite_kernel_context_t * context;
}
vg_lite_kernel_close_t;

typedef struct vg_lite_kernel_export_memory {
    int32_t fd;
}
vg_lite_kernel_export_memory_t;

typedef struct vg_lite_kernel_hardware_running_time {
    unsigned long run_time;
    int32_t hertz;
}
vg_lite_kernel_hardware_running_time_t;

typedef struct vg_lite_kernel_delay_resume {
    uint32_t set_delay_resume;
    uint32_t query_delay_resume;
}
vg_lite_kernel_delay_resume_t;

typedef struct vg_lite_kernel_gpu_clock_state {
    uint32_t state;
}
vg_lite_kernel_gpu_clock_state_t;

vg_lite_error_t vg_lite_kernel(vg_lite_kernel_command_t command, void * data);

vg_lite_error_t record_running_time(void);

extern uint32_t init_buffer[12];
extern uint32_t is_init;
extern size_t physical_address;

#ifdef __cplusplus
}
#endif

#endif /* LV_USE_VG_LITE_DRIVER */

#endif /* VG_LITE_KERNEL_H */
