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

#ifndef TSI_MALLOC_H__
#define TSI_MALLOC_H__

#ifdef __cplusplus
extern "C" {
#endif


#define tsi_malloc_init(base_virt, base_phys, size, reset) \
		tsi_malloc_init_pool(0, base_virt, base_phys, size, reset)

#define tsi_malloc(size) tsi_malloc_pool(0, size)

int   tsi_malloc_init_pool(	int pool,
						   	void *base_virt,
						   	uintptr_t base_phys,
						   	int size,
						   	int reset);

int   tsi_malloc_init_pool_aligned(	int pool,
						   	        void *base_virt,
						   	        uintptr_t base_phys,
						   	        int size,
						   	        int reset,
							        int alignment); /*выравнивание должно быть кратно 4, иначе оно будет перезаписано внутренне и станет кратным 4.*/

void *tsi_malloc_pool(int pool, int size);
void  tsi_free(void *ptr);
uintptr_t tsi_virt2phys(void *addr);
void *tsi_phys2virt(uintptr_t addr);


#ifdef __cplusplus
}
#endif

#endif
