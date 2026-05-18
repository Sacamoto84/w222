#include "../../lv_conf_internal.h"
#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN

#ifndef LV_TLSF_H
#define LV_TLSF_H

/*
** Двухуровневый распределитель памяти Segregated Fit, версия 3.1.
** Автор Мэтью Конте
**  http://tlsf.baisoku.org
**
** На основе оригинальной документации Мигеля Масмано:
**  http://www.gii.upv.es/tlsf/main/docs
**
** Эта реализация была написана по спецификации
** документа, поэтому ограничения GPL не применяются.
**
** Copyright (c) 2006-2016, Matthew Conte
** Все права защищены.
**
** Распространение и использование в исходной и двоичной форме, с или без
** Модификация допускается при соблюдении следующих условий:
**     * Redistributions of source code must retain the above copyright
**       уведомление, этот список условий и следующий отказ от ответственности.
**     * Redistributions in binary form must reproduce the above copyright
**       уведомление, этот список условий и следующий отказ от ответственности в
**       документация и/или другие материалы, поставляемые вместе с дистрибутивом.
**     * Neither the name of the copyright holder nor the
**       имена его участников могут использоваться для поддержки или продвижения продуктов.
**       получены из этого программного обеспечения без специального предварительного письменного разрешения.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL MATTHEW CONTE BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "../../misc/lv_ll.h"
#include "../../misc/lv_types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* lv_tlsf_t: a TLSF structure. Can contain 1 to N pools. */
/* lv_pool_t: a block of memory that TLSF can manage. */
typedef void * lv_tlsf_t;
typedef void * lv_pool_t;

/* Создать/уничтожить пул памяти. */
lv_tlsf_t lv_tlsf_create(void * mem);
lv_tlsf_t lv_tlsf_create_with_pool(void * mem, size_t bytes);
void lv_tlsf_destroy(lv_tlsf_t tlsf);
lv_pool_t lv_tlsf_get_pool(lv_tlsf_t tlsf);

/* Добавить/удалить пулы памяти. */
lv_pool_t lv_tlsf_add_pool(lv_tlsf_t tlsf, void * mem, size_t bytes);
void lv_tlsf_remove_pool(lv_tlsf_t tlsf, lv_pool_t pool);

/* malloc/memalign/realloc/free замены. */
void * lv_tlsf_malloc(lv_tlsf_t tlsf, size_t bytes);
void * lv_tlsf_memalign(lv_tlsf_t tlsf, size_t align, size_t bytes);
void * lv_tlsf_realloc(lv_tlsf_t tlsf, void * ptr, size_t size);
size_t lv_tlsf_free(lv_tlsf_t tlsf, const void * ptr);

/* Возвращает внутренний размер блока, а не исходный размер запроса */
size_t lv_tlsf_block_size(void * ptr);

/* Накладные расходы/ограничения внутренних структур. */
size_t lv_tlsf_size(void);
size_t lv_tlsf_align_size(void);
size_t lv_tlsf_block_size_min(void);
size_t lv_tlsf_block_size_max(void);
size_t lv_tlsf_pool_overhead(void);
size_t lv_tlsf_alloc_overhead(void);

/* Отладка. */
typedef void (*lv_tlsf_walker)(void * ptr, size_t size, int used, void * user);
void lv_tlsf_walk_pool(lv_pool_t pool, lv_tlsf_walker walker, void * user);
/* Возвращает ненулевое значение, если какая-либо проверка внутренней согласованности не удалась. */
int lv_tlsf_check(lv_tlsf_t tlsf);
int lv_tlsf_check_pool(lv_pool_t pool);

#if defined(__cplusplus)
};
#endif

#endif /*LV_TLSF_H*/

#endif /*LV_STDLIB_BUILTIN*/
