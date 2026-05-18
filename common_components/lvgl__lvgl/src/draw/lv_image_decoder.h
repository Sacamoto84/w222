/**
 * @file lv_image_decoder.h
 *
 */

#ifndef LV_IMAGE_DECODER_H
#define LV_IMAGE_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include "lv_draw_buf.h"
#include "../misc/lv_fs.h"
#include "../misc/lv_types.h"
#include "../misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Источник изображения.*/
typedef enum {
    LV_IMAGE_SRC_VARIABLE, /** Двоичная/C переменная*/
    LV_IMAGE_SRC_FILE, /** Файл в файловой системе*/
    LV_IMAGE_SRC_SYMBOL, /** Символ (@reflv_symbol_def.h )*/
    LV_IMAGE_SRC_UNKNOWN, /** Неизвестный источник*/
} lv_image_src_t;

/**
 * Получить информацию из изображения и сохраниться в `header`.
 * @param decoder  указатель на объект декодера
 * @param dsc      указатель на дескриптор декодера
 * @param header   хранить информацию здесь
 * @return LV_RESULT_OK: информация написана правильно;  LV_RESULT_INVALID: не удалось
 */
typedef lv_result_t (*lv_image_decoder_info_f_t)(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc,
                                                 lv_image_header_t * header);

/**
 * Откройте изображение для декодирования. Подготовьте его, так как он понадобится для прочтения позже.
 * @param decoder  указатель на декодер, функцию, связанную с
 * @param dsc      указатель на дескриптор декодера.  В нем уже инициализированы `src`, `color`.
 */
typedef lv_result_t (*lv_image_decoder_open_f_t)(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);

/**
 * Декодируйте пиксели`full_area`постепенно, вызывая цикл. Установите значения`decoded_area`на`LV_COORD_MIN`при первом вызове.
 * Требуется только в том случае, если функция открытия не может вернуть весь декодированный массив пикселей.
 * @param decoder указатель на декодер, функцию, связанную с
 * @param dsc указатель на дескриптор декодера
 * @param full_area входной параметр. вся область для декодирования после достаточного количества последующих вызовов
 * @param decoded_area входной+выходной параметр. установите значения`LV_COORD_MIN`для первого вызова и сброса декодирования.
 *                     декодированная область сохраняется здесь после каждого вызова.
 * @return LV_RESULT_OK : ок;  LV_RESULT_INVALID: не удалось или нечего декодировать.
 */
typedef lv_result_t (*lv_image_decoder_get_area_cb_t)(lv_image_decoder_t * decoder,
                                                      lv_image_decoder_dsc_t * dsc,
                                                      const lv_area_t * full_area, lv_area_t * decoded_area);

/**
 * Закройте ожидающее декодирование. Бесплатные ресурсы и т. д.
 * @param decoder указатель на декодер, функцию, связанную с
 * @param dsc указатель на дескриптор декодера
 */
typedef void (*lv_image_decoder_close_f_t)(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);

/**
 * Пользовательские функции рисования для специальных форматов изображений.
 * @param layer указатель на слой
 * @param dsc указатель на дескриптор декодера
 * @param coords координаты изображения
 * @param draw_dsc дескриптор изображения рисования
 * @param clip_area область обрезки изображения
 */
typedef void (*lv_image_decoder_custom_draw_t)(lv_layer_t * layer, const lv_image_decoder_dsc_t * dsc,
                                               const lv_area_t * coords, const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * clip_area);
/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Получить информацию об изображении.
 * Попробуйте созданный декодер изображений один за другим. Как только кто-то сможет получить информацию, эта информация будет использована.
 * @param src источник изображения. Может быть
 *  1) Имя файла: например. «S:folder/img1.png» (драйверы необходимо зарегистрировать через `lv_fs_drv_register()`)
 *  2) Переменная: указатель на переменную `lv_image_dsc_t`.
 *  3) Символ: например.  `LV_SYMBOL_OK`
 * @param header информация об изображении будет храниться здесь
 * @return LV_RESULT_OK: успех;  LV_RESULT_INVALID: не удалось получить информацию об изображении.
 */
lv_result_t lv_image_decoder_get_info(const void * src, lv_image_header_t * header);

/**
 * Откройте изображение.
 * продолжайте создавать декодеры изображений один за другим. Как только вы сможете открыть изображение, декодер будет сохранен в `dsc`.
 * @param dsc    описывает сеанс декодирования. Просто указатель на переменную `lv_image_decoder_dsc_t`.
 * @param src    источник изображения. Может быть
 *                 1) Имя файла: например. «S:folder/img1.png» (драйверы необходимо зарегистрировать через `lv_fs_drv_register())`)
 *                 2) Переменная: указатель на переменную `lv_image_dsc_t`.
 *                 3) Символ: например.  `LV_SYMBOL_OK`
 * @param args   аргументы о том, как следует открыть изображение.
 * @return LV_RESULT_OK: открыл изображение. `dsc->decoded`и`dsc->header`установлены.
 *         LV_RESULT_INVALID: ни один из зарегистрированных декодеров изображений не смог открыть изображение.
 */
lv_result_t lv_image_decoder_open(lv_image_decoder_dsc_t * dsc, const void * src, const lv_image_decoder_args_t * args);

/**
 * Декодируйте пиксели`full_area`постепенно, вызывая цикл. Установите`decoded_area`на`LV_COORD_MIN`при первом вызове.
 * @param dsc           дескриптор декодера изображений
 * @param full_area     входной параметр. вся область для декодирования после достаточного количества последующих вызовов
 * @param decoded_area  входной+выходной параметр. установите значения`LV_COORD_MIN`для первого вызова и сброса декодирования.
 *                      декодированная область сохраняется здесь после каждого вызова.
 * @return              LV_RESULT_OK: успех;  LV_RESULT_INVALID: произошла ошибка или декодировать уже нечего
 */
lv_result_t lv_image_decoder_get_area(lv_image_decoder_dsc_t * dsc, const lv_area_t * full_area,
                                      lv_area_t * decoded_area);

/**
 * Закрыть сеанс декодирования
 * @param dsc указатель на `lv_image_decoder_dsc_t`, используемый в `lv_image_decoder_open`
 */
void lv_image_decoder_close(lv_image_decoder_dsc_t * dsc);

/**
 * Создайте новый декодер изображений
 * @return указатель на новый декодер изображений
 */
lv_image_decoder_t * lv_image_decoder_create(void);

/**
 * Удаление декодера изображения
 * @param decoder указатель на декодер изображения
 */
void lv_image_decoder_delete(lv_image_decoder_t * decoder);

/**
 * Получите следующий декодер изображения в связанном списке декодеров изображений.
 * @param decoder указатель на декодер изображения или NULL, чтобы получить первое
 * @return следующий декодер изображения или NULL, если декодера изображения больше не существует
 */
lv_image_decoder_t * lv_image_decoder_get_next(lv_image_decoder_t * decoder);

/**
 * Установите обратный вызов для получения информации об изображении
 * @param decoder указатель на декодер изображения
 * @param info_cb функция для сбора информации об изображении (заполните структуру `lv_image_header_t`)
 */
void lv_image_decoder_set_info_cb(lv_image_decoder_t * decoder, lv_image_decoder_info_f_t info_cb);

/**
 * Установите обратный вызов, чтобы открыть изображение
 * @param decoder указатель на декодер изображения
 * @param open_cb функция открытия изображения
 */
void lv_image_decoder_set_open_cb(lv_image_decoder_t * decoder, lv_image_decoder_open_f_t open_cb);

/**
 * Установите обратный вызов на декодированную строку изображения
 * @param decoder указатель на декодер изображения
 * @param read_line_cb функция для чтения строки изображения
 */
void lv_image_decoder_set_get_area_cb(lv_image_decoder_t * decoder, lv_image_decoder_get_area_cb_t read_line_cb);

/**
 * Установите обратный вызов для закрытия сеанса декодирования. Например. закройте файлы и освободите другие ресурсы.
 * @param decoder указатель на декодер изображения
 * @param close_cb функция закрытия сеанса декодирования
 */
void lv_image_decoder_set_close_cb(lv_image_decoder_t * decoder, lv_image_decoder_close_f_t close_cb);

lv_cache_entry_t * lv_image_decoder_add_to_cache(lv_image_decoder_t * decoder,
                                                 lv_image_cache_data_t * search_key,
                                                 const lv_draw_buf_t * decoded, void * user_data);

/**
 * Проверьте декодированное изображение, внесите любые изменения, если для этого требуется декодер`args`.
 * @note Новый буфер отрисовки будет выделен, если`decoded`не поддается изменению или несоответствие шага и т. д.
 * @param dsc       указатель на дескриптор декодера
 * @param decoded   указатель на декодированное изображение для отправки в процесс для удовлетворения требований dsc->args.
 * @return          буфер отрисовки после обработки, если он отличается от`decoded`, он выделяется заново.
 */
lv_draw_buf_t * lv_image_decoder_post_process(lv_image_decoder_dsc_t * dsc, lv_draw_buf_t * decoded);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_IMAGE_DECODER_H*/
