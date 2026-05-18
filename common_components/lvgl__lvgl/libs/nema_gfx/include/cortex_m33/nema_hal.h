/* TSI 2023.xmo */
/*******************************************************************************
 * Copyright (c) 2023 Think Silicon Single Member PC
 *
 * Разрешение настоящим предоставляется бесплатно любому лицу, получившему копию.
 * этого файла заголовка и/или связанных с ним файлов документации для использования, копирования,
 * изменять, объединять, публиковать, распространять, сублицензировать и/или продавать копии
 * Материалы и разрешать лицам, которым предоставлены Материалы, делать
 * Итак, при соблюдении следующих условий:
 *
 * Вышеупомянутое уведомление об авторских правах и настоящее уведомление о разрешении должны быть включены в
 * все копии или существенные части Материалов.
 *
 * MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
 * NEMAGFX API. THE UNMODIFIED, NORMATIVE VERSIONS OF THINK-SILICON NEMAGFX
 * SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT:
 *   https://think-silicon.com/products/software/nemagfx-api
 *
 *  Программное обеспечение предоставляется «как есть», без каких-либо явных или явных гарантий.
 *  подразумеваемые, включая, помимо прочего, гарантии товарной пригодности,
 *  пригодность для конкретной цели и отсутствие нарушений. Ни в коем случае нельзя
 *  Считайте, что Silicon Single MemberPCнесет ответственность за любые претензии, ущерб или другие
 *  ответственность, будь то по договору, правонарушению или иным образом, возникающая
 *  из, вне или в связи с программным обеспечением, использованием или другими сделками
 *  в программном обеспечении.
 ******************************************************************************/


#ifndef NEMA_HAL_H__
#define NEMA_HAL_H__

#include "nema_sys_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct nema_buffer_t_ {
    int       size;                 /**< Size of buffer */
    int       fd;                   /**< File Descriptor of buffer */
    void     *base_virt;            /**< Virtual address of buffer */
    uintptr_t base_phys;            /**< Physical address of buffer */
} nema_buffer_t;

/** \brief Инициализируйте систему. Определен разработчик. Вызывается вnema_init()
 *
 * \параметр недействителен
 * \return 0, если ошибки не произошли
 * \см. nema_init()
 *
 */
int32_t nema_sys_init(void);


/** \brief Дождитесь прерывания от GPU
 *
 * \параметр недействителен
 * \return 0 в случае успеха
 *
 */
int nema_wait_irq(void);

/** \brief Дождитесь составления списка команд
 *
 * \paramcl_idСписок команды ID
 * \return 0 в случае успеха
 *
 */
int nema_wait_irq_cl(int cl_id);

/** \brief Дождитесь точки остановки
 *
 * \paramcl_idТочка остановки ID
 * \return 0 в случае успеха
 *
 */
int nema_wait_irq_brk(int brk_id);

/** \brief Чтение реестра оборудования
 *
 * \param reg Зарегистрируйтесь, чтобы прочитать
 * \return, прочитанное из регистра
 * \см.  nema_reg_write
 *
 */
uint32_t nema_reg_read(uint32_t reg);

/** \brief Запись в регистр оборудования
 *
 * \param reg Зарегистрируйтесь, чтобы написать
 * Значение \param для записей
 * \return void()
 * \см. nema_reg_read()
 *
 */
void nema_reg_write(uint32_t reg, uint32_t value);

/** \brief Создание буфера памяти
 *
 * \param size Размер буфера в байтах
 * \return структура nema_buffer_t
 *
 */
nema_buffer_t nema_buffer_create(int size);

/** \brief Создать буфер памяти в среднем пуле
 *
 * \param пулIDнужного пула памяти
 * \param size Размер буфера в байтах
 * \return структура nema_buffer_t
 *
 */
nema_buffer_t nema_buffer_create_pool(int pool, int size);

/** \brief Буфер карт
 *
 * \param bo Указатель структуры буфера
 * \return Виртуальный указатель буфера (так же, как в bo->base_virt)
 *
 */
void *nema_buffer_map(nema_buffer_t *bo);

/** \brief Отображение буфера
 *
 * \param bo Указатель структуры буфера
 * \return void
 *
 */
void nema_buffer_unmap(nema_buffer_t *bo);

/** \brief Уничтожить/освободить буфер
 *
 * \param bo Указатель структуры буфера
 * \return void
 *
 */
void nema_buffer_destroy(nema_buffer_t *bo);

/** \brief Получить физический (GPU) базовый адрес данного буфера
 *
 * \param bo Указатель структуры буфера
 * \return Физический базовый адрес данного буфера
 *
 */
uintptr_t nema_buffer_phys(nema_buffer_t *bo);

/** \brief Буфер обратные записи из кэша в основной памяти
 *
 * \param bo Указатель структуры буфера
 * \return void
 *
 */
void nema_buffer_flush(nema_buffer_t * bo);

/** \brief Выделите память для использованияCPU(обычно называется стандартный malloc())
 *
 * \param size Размер в байтах
 * \return Указатель выделенной памяти (виртуальной)
 * \см. nema_host_free()
 *
 */
void *nema_host_malloc(size_t size);

/** \brief Свободная память, ранее выделенная с помощьюnema_host_malloc()
 *
 * \param ptr Указатель выделенной памяти (виртуальной)
 * \return void
 * \см. nema_host_malloc()
 *
 */
void  nema_host_free(void *ptr );

/** \private */
typedef struct nema_ringbuffer_t_ {
    nema_buffer_t bo;
    int      offset;    //количество 32-битных записей
    int	     last_submission_id;
} nema_ringbuffer_t;


/** \brief Инициализация кольцевого буфера. Должен сохраняться изнутри nema_sys_init().
 *   Это частная функция, пользователь никогда не должен ее вызывать.
 *
 * \param *rb Указатель структуры nema_ring_buffer_t
 * \param reset Сбрасывает кольцевой буфер, если он не возникает.
 * \return Отрицательное число при деньгах
 * \см. nema_sys_init()
 *
 */
/** \private */
int nema_rb_init(nema_ringbuffer_t *rb, int reset);

#define MUTEX_RB     0
#define MUTEX_MALLOC 1
#define MUTEX_FLUSH  2
#define MUTEX_MAX    2

/** \brief Блокировка мьютекса для нескольких процессов/потоков
 *
 * \paramMUTEX_RBили MUTEX_MALLOC
 * \return
 *
 */
int nema_mutex_lock(int mutex_id);

/** \brief Разблокировка мьютекса для нескольких процессов/потоков
 *
 * \paramMUTEX_RBили MUTEX_MALLOC
 * \return
 *
 */
int nema_mutex_unlock(int mutex_id);

#ifdef __cplusplus
}
#endif

#endif
