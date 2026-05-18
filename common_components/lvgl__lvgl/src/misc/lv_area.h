/**
 * @file lv_area.h
 *
 */

#ifndef LV_AREA_H
#define LV_AREA_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "lv_types.h"
#include "lv_math.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Представляет точку на экране.
 */
typedef struct {
    int32_t x;
    int32_t y;
} lv_point_t;

typedef struct {
    lv_value_precise_t x;
    lv_value_precise_t y;
} lv_point_precise_t;

/** Представляет область экрана.*/
typedef struct {
    int32_t x1;
    int32_t y1;
    int32_t x2;
    int32_t y2;
} lv_area_t;

/** Выравнивания*/

typedef enum {
    LV_ALIGN_DEFAULT = 0,
    LV_ALIGN_TOP_LEFT,
    LV_ALIGN_TOP_MID,
    LV_ALIGN_TOP_RIGHT,
    LV_ALIGN_BOTTOM_LEFT,
    LV_ALIGN_BOTTOM_MID,
    LV_ALIGN_BOTTOM_RIGHT,
    LV_ALIGN_LEFT_MID,
    LV_ALIGN_RIGHT_MID,
    LV_ALIGN_CENTER,

    LV_ALIGN_OUT_TOP_LEFT,
    LV_ALIGN_OUT_TOP_MID,
    LV_ALIGN_OUT_TOP_RIGHT,
    LV_ALIGN_OUT_BOTTOM_LEFT,
    LV_ALIGN_OUT_BOTTOM_MID,
    LV_ALIGN_OUT_BOTTOM_RIGHT,
    LV_ALIGN_OUT_LEFT_TOP,
    LV_ALIGN_OUT_LEFT_MID,
    LV_ALIGN_OUT_LEFT_BOTTOM,
    LV_ALIGN_OUT_RIGHT_TOP,
    LV_ALIGN_OUT_RIGHT_MID,
    LV_ALIGN_OUT_RIGHT_BOTTOM,
} lv_align_t;

typedef enum {
    LV_DIR_NONE     = 0x00,
    LV_DIR_LEFT     = (1 << 0),
    LV_DIR_RIGHT    = (1 << 1),
    LV_DIR_TOP      = (1 << 2),
    LV_DIR_BOTTOM   = (1 << 3),
    LV_DIR_HOR      = LV_DIR_LEFT | LV_DIR_RIGHT,
    LV_DIR_VER      = LV_DIR_TOP | LV_DIR_BOTTOM,
    LV_DIR_ALL      = LV_DIR_HOR | LV_DIR_VER,
} lv_dir_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализировать область
 * @param area_p указатель на область
 * @param x1 левая координата местности
 * @param y1 верхняя координата области
 * @param x2 правильные координаты местности
 * @param y2 нижняя координата области
 */
void lv_area_set(lv_area_t * area_p, int32_t x1, int32_t y1, int32_t x2, int32_t y2);

/**
 * Копировать область
 * @param dest указатель на область назначения
 * @param src указатель на исходную область
 */
inline static void lv_area_copy(lv_area_t * dest, const lv_area_t * src)
{
    dest->x1 = src->x1;
    dest->y1 = src->y1;
    dest->x2 = src->x2;
    dest->y2 = src->y2;
}

/**
 * Получить ширину области
 * @param area_p указатель на область
 * @return ширина области (если x1 == x2 -> ширина = 1)
 */
int32_t lv_area_get_width(const lv_area_t * area_p);

/**
 * Получить высоту области
 * @param area_p указатель на область
 * @return высота области (если y1 == y2 -> высота = 1)
 */
int32_t lv_area_get_height(const lv_area_t * area_p);

/**
 * Установить ширину области
 * @param area_p указатель на область
 * @param w новая ширина области (w == 1 делает x1 == x2)
 */
void lv_area_set_width(lv_area_t * area_p, int32_t w);

/**
 * Установить высоту области
 * @param area_p указатель на область
 * @param h новая высота области (h == 1 делает y1 == y2)
 */
void lv_area_set_height(lv_area_t * area_p, int32_t h);

/**
 * Возврат с площадью области (x * y)
 * @param area_p указатель на область
 * @return размер площади
 */
uint32_t lv_area_get_size(const lv_area_t * area_p);

void lv_area_increase(lv_area_t * area, int32_t w_extra, int32_t h_extra);

void lv_area_move(lv_area_t * area, int32_t x_ofs, int32_t y_ofs);

/**
 * Выровнять область относительно другой
 * @param base область, где другой будет выровнен
 * @param to_align область для выравнивания
 * @param align `LV_ALIGN_...`
 * @param ofs_x Смещение по оси X
 * @param ofs_y Смещение по оси Y
 */
void lv_area_align(const lv_area_t * base, lv_area_t * to_align, lv_align_t align, int32_t ofs_x, int32_t ofs_y);

/**
 * Преобразование точки
 * @param point         указатель на точку
 * @param angle         угол с разрешением 0,1 (123 означает 12,3°)
 * @param scale_x       горизонтальный зум, 256 означает 100%
 * @param scale_y       вертикальный зум, 256 означает 100%
 * @param pivot         указатель на опорную точку преобразования
 * @param zoom_first    true: сначала масштабировать, а затем вращать; еще: противоположный порядок
 */
void lv_point_transform(lv_point_t * point, int32_t angle, int32_t scale_x, int32_t scale_y, const lv_point_t * pivot,
                        bool zoom_first);

/**
 * Преобразование массива точек
 * @param points        указатель на массив точек
 * @param count         количество точек в массиве
 * @param angle         угол с разрешением 0,1 (123 означает 12,3°)
 * @param scale_x       горизонтальный зум, 256 означает 100%
 * @param scale_y       вертикальный зум, 256 означает 100%
 * @param pivot         указатель на опорную точку преобразования
 * @param zoom_first    true: сначала масштабировать, а затем вращать; еще: противоположный порядок
 */
void lv_point_array_transform(lv_point_t * points, size_t count, int32_t angle, int32_t scale_x, int32_t scale_y,
                              const lv_point_t * pivot,
                              bool zoom_first);

lv_point_t lv_point_from_precise(const lv_point_precise_t * p);

lv_point_precise_t lv_point_to_precise(const lv_point_t * p);

void lv_point_set(lv_point_t * p, int32_t x, int32_t y);

void lv_point_precise_set(lv_point_precise_t * p, lv_value_precise_t x, lv_value_precise_t y);

void lv_point_swap(lv_point_t * p1, lv_point_t * p2);

void lv_point_precise_swap(lv_point_precise_t * p1, lv_point_precise_t * p2);

/**********************
 *      MACROS
 **********************/

#define LV_COORD_TYPE_SHIFT    (29U)

#define LV_COORD_TYPE_MASK     (3 << LV_COORD_TYPE_SHIFT)
#define LV_COORD_TYPE(x)       ((x) & LV_COORD_TYPE_MASK)  /*Спецификаторы типа извлечения*/
#define LV_COORD_PLAIN(x)      ((x) & ~LV_COORD_TYPE_MASK) /*Удалить спецификаторы типа*/

#define LV_COORD_TYPE_PX       (0 << LV_COORD_TYPE_SHIFT)
#define LV_COORD_TYPE_SPEC     (1 << LV_COORD_TYPE_SHIFT)
#define LV_COORD_TYPE_PX_NEG   (3 << LV_COORD_TYPE_SHIFT)

#define LV_COORD_IS_PX(x)       (LV_COORD_TYPE(x) == LV_COORD_TYPE_PX || LV_COORD_TYPE(x) == LV_COORD_TYPE_PX_NEG)
#define LV_COORD_IS_SPEC(x)     (LV_COORD_TYPE(x) == LV_COORD_TYPE_SPEC)

#define LV_COORD_SET_SPEC(x)    ((x) | LV_COORD_TYPE_SPEC)

/** Максимальное значение координаты */
#define LV_COORD_MAX            ((1 << LV_COORD_TYPE_SHIFT) - 1)
#define LV_COORD_MIN            (-LV_COORD_MAX)

/*Специальные координаты*/
#define LV_SIZE_CONTENT         LV_COORD_SET_SPEC(LV_COORD_MAX)
#define LV_PCT_STORED_MAX       (LV_COORD_MAX - 1)
#if LV_PCT_STORED_MAX % 2 != 0
#error LV_PCT_STORED_MAX should be an even number
#endif
#define LV_PCT_POS_MAX          (LV_PCT_STORED_MAX / 2)
#define LV_PCT(x)               (LV_COORD_SET_SPEC(((x) < 0 ? (LV_PCT_POS_MAX - LV_MAX((x), -LV_PCT_POS_MAX)) : LV_MIN((x), LV_PCT_POS_MAX))))
#define LV_COORD_IS_PCT(x)      ((LV_COORD_IS_SPEC(x) && LV_COORD_PLAIN(x) <= LV_PCT_STORED_MAX))
#define LV_COORD_GET_PCT(x)     (LV_COORD_PLAIN(x) > LV_PCT_POS_MAX ? LV_PCT_POS_MAX - LV_COORD_PLAIN(x) : LV_COORD_PLAIN(x))

LV_EXPORT_CONST_INT(LV_COORD_MAX);
LV_EXPORT_CONST_INT(LV_COORD_MIN);
LV_EXPORT_CONST_INT(LV_SIZE_CONTENT);

/**
 * Преобразуйте процентное значение в`int32_t`.
 * Процентные значения хранятся в специальном диапазоне.
 * @param x процент (0..1000)
 * @return координата, в которой хранится процент
 */
int32_t lv_pct(int32_t x);

int32_t lv_pct_to_px(int32_t v, int32_t base);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
