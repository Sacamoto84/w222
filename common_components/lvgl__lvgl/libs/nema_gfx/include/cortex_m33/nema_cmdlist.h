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


#ifndef NEMA_CMDLIST_H__
#define NEMA_CMDLIST_H__

#include "nema_sys_defs.h"
#include "nema_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CL_NOP      0x010000U
#define CL_PUSH     0x020000U
#define CL_RETURN   0x040000U
#define CL_ABORT    0x080000U

#define CL_BATCH_SHIFT      12
#define CL_BATCH_LOOP       0x8000

#define SUBMISSION_ID_MASK 0xffffff

#define CL_ALIGNMENT_MASK  (0x00000007U) // Буфер CL должен быть выровнен по 8 байтам.

//---------------------------------------------------------------------------
typedef struct nema_cmdlist_t_ {
    nema_buffer_t bo;
    int size;                       /**< Number of entries in the command list */
    int offset;                     /**< Points to the next address to write */
    uint32_t flags;                 /**< Flags */
    int32_t  submission_id;
    struct nema_cmdlist_t_ *next;   /**< Points to next command list */
    struct nema_cmdlist_t_ *root;   /**< Points to the head of the list */
} nema_cmdlist_t;

/** \brief новый список команд в заранее выделенном пространстве
 *
 * \paramaddr_virtАдрес списка команды (заранее выделенный)
 * \paramsize_bytesРазмер списка команд в байтах
 * \return Экземпляр нового списка команды.
 *
 */
nema_cmdlist_t nema_cl_create_prealloc(nema_buffer_t *bo);

/** \brief Создание нового нерасширяемого списка команд определения размера.
 *
 * \paramsize_bytesРазмер списка команд в байтах
 * \return Экземпляр нового списка команды.
 *
 */
nema_cmdlist_t nema_cl_create_sized(int size_bytes);

/** \brief Создание нового расширения списка команд
 *
 * \return Экземпляр нового списка команды.
 *
 */
nema_cmdlist_t nema_cl_create(void);

/** \brief Уничтожить/освободить список команды
 *
 * \param cl Указатель на список команд
 *
 */
void nema_cl_destroy(nema_cmdlist_t *cl);

/** \brief Сброс позиции этой команды для записи в начале. Не очищает ограничения.
 *
 * \param cl Указатель на список команд
 *
 */
void nema_cl_rewind(nema_cmdlist_t *cl);

/** \brief Определите, в каком списке команда будет поставлена следующей командой.
 *
 * \param cl Указатель на список команд
 *
 */
void nema_cl_bind(nema_cmdlist_t *cl);

/** \brief Определите, в каком списке команда будет поставлена следующей командой.
 *         Привяжите этот список команд как круговой. Он никогда не наполняется, никогда не расширяется,
 *         он может быть отправлен неявно, его нельзя использовать повторно. Никакие другие CL не должны быть представлены.
 *         в то время как круговой CL связан
 *
 * \param cl Указатель на список команд
 *
 */
void nema_cl_bind_circular(nema_cmdlist_t *cl);

/** \brief Отмените привязку текущей связанной команды списка, если таковая имеется.
 *
 *
 */
void nema_cl_unbind(void);


/** \brief Список командных привязок
 *
 * \return Указатель на связанный список команд
 *
 */
nema_cmdlist_t *nema_cl_get_bound(void);

/** \private */
void nema_cl_submit_no_irq(nema_cmdlist_t *cl);

/** \brief Поставить список команд в кольцевой буфер для выполнения
 *
 * \param cl Указатель на список команд
 *
 */
void nema_cl_submit(nema_cmdlist_t *cl);

/** \brief Дождитесь составления списка команды.
 *
 * \param cl Указатель на список команд
 * \return 0, если ошибка не произошла
 *
 */
int nema_cl_wait(nema_cmdlist_t *cl);

/** \brief Добавление команды в связанный список команд
 *
 * \param reg Аппаратный регистратор для записей
 * \param data Данные для записей
 *
 */
void nema_cl_add_cmd(uint32_t reg, uint32_t data);

/** \brief Добавление нескольких команд в связанный список команд
 *
 * \paramcmd_noКоличество обновленных команд
 * \param cmd Указатель на добавленные команды
 * \return 0, если ошибка не произошла
 *
 */
int nema_cl_add_multiple_cmds(int cmd_no, uint32_t *cmd);

/** частный */
uint32_t * nema_cl_get_space(int cmd_no);

/** \brief Переход от связанного списка команд к раунду. Возврат подразумевается.
 *
 * \param cl Указатель на список команд, к которому осуществляется переход
 *
 */
void nema_cl_branch(nema_cmdlist_t *cl);

/** \brief Переход из связанного списка команды в другой. Никакого возврата не требуется.
 *
 * \param cl Указатель на список команд, к которому можно перейти
 *
 */
void nema_cl_jump(nema_cmdlist_t *cl);

/** \brief Добавление явной команды возврата в связанный список команды
 *
 *
 */
void nema_cl_return(void);

/** \brief Возвращает положительное число, если список команд почти заполнен, в противном случае возвращается 0.
 *
 * \param cl Указатель на список команд
 *
 */
int nema_cl_almost_full(nema_cmdlist_t *cl);

/** \brief проверьте, достаточно ли места или можно ли настроить расширение для
 * необходимые команды.
 *
 * \paramcmd_noКоличество команд, которые необходимо проверить, проверить ли они
 * \reurn ноль — команда точно или может быть выполнена, в противном случае возвращается отрицательный результат
 */
int nema_cl_enough_space(int cmd_no);

#ifdef __cplusplus
}
#endif

#endif
