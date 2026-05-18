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

#ifndef NEMA_ERROR_H__
#define NEMA_ERROR_H__

#include "nema_sys_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

// Обработка ошибок

#define  NEMA_ERR_NO_ERROR                     (0x00000000U) /**< No error has occured */
#define  NEMA_ERR_SYS_INIT_FAILURE             (0x00000001U) /**< System initialization failure */
#define  NEMA_ERR_GPU_ABSENT                   (0x00000002U) /**< Nema GPU is absent */
#define  NEMA_ERR_RB_INIT_FAILURE              (0x00000004U) /**< Ring buffer initialization failure */
#define  NEMA_ERR_NON_EXPANDABLE_CL_FULL       (0x00000008U) /**< Non expandable command list is full*/
#define  NEMA_ERR_CL_EXPANSION                 (0x00000010U) /**< Command list expansion error */
#define  NEMA_ERR_OUT_OF_GFX_MEMORY            (0x00000020U) /**< Graphics memory is full */
#define  NEMA_ERR_OUT_OF_HOST_MEMORY           (0x00000040U) /**< Host memory is full */
#define  NEMA_ERR_NO_BOUND_CL                  (0x00000080U) /**< There is no bound command list */
#define  NEMA_ERR_NO_BOUND_FONT                (0x00000100U) /**< There is no bound font */
#define  NEMA_ERR_GFX_MEMORY_INIT              (0x00000200U) /**< Graphics memory initialization failure */
#define  NEMA_ERR_DRIVER_FAILURE               (0x00000400U) /**< Nema GPU Kernel Driver failure*/
#define  NEMA_ERR_MUTEX_INIT                   (0x00000800U) /**< Mutex initialization failure*/
#define  NEMA_ERR_INVALID_BO                   (0x00001000U) /**< Invalid buffer provided*/
#define  NEMA_ERR_INVALID_CL                   (0x00002000U) /**< Invalid CL provided*/
#define  NEMA_ERR_INVALID_CL_ALIGMENT          (0x00004000U) /**< Invalid CL buffer alignment*/
#define  NEMA_ERR_NO_INIT                      (0x00008000U) /**< GFX uninitialised*/
#define  NEMA_ERR_INVALID_SECTORED_CL_SIZE     (0x00010000U) /**< Invalid sectored CL size, each sector should be at least 512 bytes */


/** \brief Возвращает ошибки идентификатора
 *
 * \return 0, если ошибок нет
 *
 */
uint32_t nema_get_error(void);


#ifdef __cplusplus
}
#endif

#endif // NEMA_ERROR_H__
