/*
 * @file
 *
 **********************************************************************
 *                            DO NOT EDIT
 * Этот файл автоматически создается «style_api_gen.py».
 **********************************************************************
 */


#ifndef LV_OBJ_STYLE_GEN_H
#define LV_OBJ_STYLE_GEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../misc/lv_area.h"
#include "../misc/lv_style.h"
#include "../core/lv_obj_style.h"
#include "../misc/lv_types.h"

/**
 * Получает ширину виджета. Можно использовать значения в пикселях, процентах и ​​`LV_SIZE_CONTENT`.
 * Процентные значения относятся к ширине родительской области содержимого.
 * Default: Зависит от виджета, наследуется: Нет, макет: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_width(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_WIDTH);
    return (int32_t)v.num;
}

/**
 * Получает минимальную ширину. Можно использовать пиксельные и процентные значения. Процентные значения
 * относятся к ширине области содержимого родительского элемента.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_min_width(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_MIN_WIDTH);
    return (int32_t)v.num;
}

/**
 * Получает максимальную ширину. Можно использовать пиксельные и процентные значения. Процентные значения
 * относятся к ширине области содержимого родительского элемента.
 * Default: LV_COORD_MAX , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_max_width(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_MAX_WIDTH);
    return (int32_t)v.num;
}

/**
 * Получает высоту виджета. Можно использовать пиксель, проценты и `LV_SIZE_CONTENT`.
 * Процентные значения относятся к высоте родительской области содержимого.
 * Default: Зависит от виджета, наследуется: Нет, макет: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_height(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_HEIGHT);
    return (int32_t)v.num;
}

/**
 * Получает минимальную высоту. Можно использовать пиксельные и процентные значения. Процентные значения
 * относятся к высоте области содержимого родительского элемента.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_min_height(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_MIN_HEIGHT);
    return (int32_t)v.num;
}

/**
 * Получает максимальную высоту. Можно использовать пиксельные и процентные значения. Процентные значения
 * относятся к высоте области содержимого родительского элемента.
 * Default: LV_COORD_MAX , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_max_height(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_MAX_HEIGHT);
    return (int32_t)v.num;
}

/**
 * Его значение зависит от типа виджета. Например, в случаеlv_scaleэто означает
 * длина тиков.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_length(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LENGTH);
    return (int32_t)v.num;
}

/**
 * Получите координату X виджета с учетом настроек``align``. Пиксель и процент
 * можно использовать значения. Процентные значения относятся к ширине родительского элемента.
 * область контента.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_x(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_X);
    return (int32_t)v.num;
}

/**
 * Получите координату Y виджета с учетом настроек``align``. Пиксель и процент
 * можно использовать значения. Процентные значения относятся к высоте родительского элемента.
 * область контента.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_y(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_Y);
    return (int32_t)v.num;
}

/**
 * Получите выравнивание, указывающее, из какой точки родительского элемента находятся X и Y.
 * координаты должны быть интерпретированы. Возможные значения: `LV_ALIGN_DEFAULT` ,
 * `LV_ALIGN_TOP_LEFT/MID/RIGHT`, `LV_ALIGN_BOTTOM_LEFT/MID/RIGHT`,
 * `LV_ALIGN_LEFT/RIGHT_MID` , `LV_ALIGN_CENTER` .  `LV_ALIGN_DEFAULT` означает
 * `LV_ALIGN_TOP_LEFT` с направлением основания LTR и `LV_ALIGN_TOP_RIGHT` с основанием RTL
 * направление.
 * Default: `LV_ALIGN_DEFAULT` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_align_t lv_obj_get_style_align(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ALIGN);
    return (lv_align_t)v.num;
}

/**
 * С помощью этого значения сделайте виджет шире с обеих сторон. Пиксель и процент (с
 * `lv_pct(x)` ) можно использовать значения. Процентные значения относятся к расширению виджета.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_transform_width(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_WIDTH);
    return (int32_t)v.num;
}

/**
 * Сделайте виджет выше с обеих сторон с этим значением. Пиксель и процент (с
 * `lv_pct(x)` ) можно использовать значения. Процентные значения относятся к высоте виджета.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_transform_height(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_HEIGHT);
    return (int32_t)v.num;
}

/**
 * Переместите виджет с этим значением в направлении X. Применяется после макетов, выравниваний и других
 * позиционирование. Можно использовать значения в пикселях и процентах (с `lv_pct(x)`). Процент
 * значения относятся к ширине виджета.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_translate_x(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSLATE_X);
    return (int32_t)v.num;
}

/**
 * Переместите виджет с этим значением в направлении Y. Применяется после макетов, выравниваний и других
 * позиционирование. Можно использовать значения в пикселях и процентах (с `lv_pct(x)`). Процент
 * значения относятся к высоте виджета.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_translate_y(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSLATE_Y);
    return (int32_t)v.num;
}

/**
 * Переместите объект вокруг центра родительского объекта (например, по окружности).
 * масштаба).
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_translate_radial(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSLATE_RADIAL);
    return (int32_t)v.num;
}

/**
 * Масштабировать виджет по горизонтали. Значение 256 (или `LV_SCALE_NONE` ) означает нормальный размер, 128.
 * половинный размер, 512 двойной размер и так далее.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_transform_scale_x(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_SCALE_X);
    return (int32_t)v.num;
}

/**
 * Масштабировать виджет по вертикали. Значение 256 (или `LV_SCALE_NONE`) означает нормальный размер, 128.
 * половинный размер, 512 двойной размер и так далее.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_transform_scale_y(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_SCALE_Y);
    return (int32_t)v.num;
}

/**
 * Поворот виджета. Значение интерпретируется в единицах 0,1 градуса. Например. 450 означает 45 градусов.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_transform_rotation(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_ROTATION);
    return (int32_t)v.num;
}

/**
 * Получите координату X точки поворота для преобразований. Относительно верхнего левого угла виджета.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_transform_pivot_x(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_PIVOT_X);
    return (int32_t)v.num;
}

/**
 * Получите координату Y точки поворота для преобразований. Относительно верхнего левого угла виджета.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_transform_pivot_y(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_PIVOT_Y);
    return (int32_t)v.num;
}

/**
 * Наклон виджета по горизонтали. Значение интерпретируется в единицах 0,1 градуса. Например. 450
 * значит 45 град.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_transform_skew_x(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_SKEW_X);
    return (int32_t)v.num;
}

/**
 * Наклон виджета по вертикали. Значение интерпретируется в единицах 0,1 градуса. Например. 450
 * значит 45 град.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_transform_skew_y(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_SKEW_Y);
    return (int32_t)v.num;
}

/**
 * Получает отступ сверху. Это уменьшает область контента в этом направлении.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_pad_top(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_TOP);
    return (int32_t)v.num;
}

/**
 * Получает отступ снизу. Это уменьшает область контента в этом направлении.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_pad_bottom(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_BOTTOM);
    return (int32_t)v.num;
}

/**
 * Получает отступ слева. Это уменьшает область контента в этом направлении.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_pad_left(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_LEFT);
    return (int32_t)v.num;
}

/**
 * Получает отступ справа. Это уменьшает область контента в этом направлении.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_pad_right(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_RIGHT);
    return (int32_t)v.num;
}

/**
 * Получает отступы между строками. Используется макетами.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_pad_row(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_ROW);
    return (int32_t)v.num;
}

/**
 * Получает отступы между столбцами. Используется макетами.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_pad_column(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_COLUMN);
    return (int32_t)v.num;
}

/**
 * Отложите текстовые метки от делений шкалы/остатка `` LV_PART_ `` .
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_pad_radial(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_RADIAL);
    return (int32_t)v.num;
}

/**
 * Получает отступ сверху. Виджет сохранит это пространство от своих собратьев в макетах.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_margin_top(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_MARGIN_TOP);
    return (int32_t)v.num;
}

/**
 * Получает поле внизу. Виджет сохранит это пространство от своих собратьев в макетах.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_margin_bottom(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_MARGIN_BOTTOM);
    return (int32_t)v.num;
}

/**
 * Получает поле слева. Виджет сохранит это пространство от своих собратьев в макетах.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_margin_left(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_MARGIN_LEFT);
    return (int32_t)v.num;
}

/**
 * Получает поле справа. Виджет сохранит это пространство от своих собратьев в макетах.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_margin_right(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_MARGIN_RIGHT);
    return (int32_t)v.num;
}

/**
 * Получить цвет фона виджета.
 * Default: `0xffffff` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_bg_color(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_COLOR);
    return v.color;
}

/**
 * Получить цвет фона виджета.
 * Default: `0xffffff` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_bg_color_filtered(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_BG_COLOR));
    return v.color;
}

/**
 * Получите непрозрачность фона. Значение 0, `LV_OPA_0` или `LV_OPA_TRANSP` означает полностью
 * прозрачный, 255, `LV_OPA_100` или `LV_OPA_COVER` означает полное покрытие, другие значения
 * или LV_OPA_10 , LV_OPA_20 и т. д. означает полупрозрачность.
 * Default: `LV_OPA_TRANSP` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_opa_t lv_obj_get_style_bg_opa(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_OPA);
    return (lv_opa_t)v.num;
}

/**
 * Получите градиентный цвет фона. Используется только в том случае, если`grad_dir`не`LV_GRAD_DIR_NONE`.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_bg_grad_color(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_GRAD_COLOR);
    return v.color;
}

/**
 * Получите градиентный цвет фона. Используется только в том случае, если`grad_dir`не`LV_GRAD_DIR_NONE`.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_bg_grad_color_filtered(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_BG_GRAD_COLOR));
    return v.color;
}

/**
 * Получите направление градиента фона. Возможные значения:
 * `LV_GRAD_DIR_NONE/HOR/VER`.
 * Default: `LV_GRAD_DIR_NONE` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_grad_dir_t lv_obj_get_style_bg_grad_dir(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_GRAD_DIR);
    return (lv_grad_dir_t)v.num;
}

/**
 * Получите точку, с которой должен начинаться цвет фона для градиентов. 0 означает
 * верхняя/левая сторона, 255 нижняя/правая сторона, 128 центр и так далее.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_bg_main_stop(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_MAIN_STOP);
    return (int32_t)v.num;
}

/**
 * Получите точку, с которой должен начинаться градиент цвета фона. 0 означает вверх/влево
 * сторона, 255 нижняя/правая сторона, 128 центр и так далее.
 * Default: 255, по наследству: Нет, планировка: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_bg_grad_stop(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_GRAD_STOP);
    return (int32_t)v.num;
}

/**
 * Получите непрозрачность первого цвета градиента.
 * Default: 255, по наследству: Нет, планировка: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_opa_t lv_obj_get_style_bg_main_opa(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_MAIN_OPA);
    return (lv_opa_t)v.num;
}

/**
 * Получите непрозрачность второго цвета градиента.
 * Default: 255, по наследству: Нет, планировка: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_opa_t lv_obj_get_style_bg_grad_opa(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_GRAD_OPA);
    return (lv_opa_t)v.num;
}

/**
 * Получите определение градиента. Указанный экземпляр должен существовать, пока жив виджет.
 * NULL для отключения. Он обертывает `BG_GRAD_COLOR` , `BG_GRAD_DIR` , `BG_MAIN_STOP` и
 * `BG_GRAD_STOP` в один дескриптор и позволяет создавать градиенты с большим количеством цветов.
 * также. Если он установлен, другие свойства, связанные с градиентом, будут игнорироваться.
 * Default: `NULL` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline const lv_grad_dsc_t * lv_obj_get_style_bg_grad(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_GRAD);
    return (const lv_grad_dsc_t *)v.ptr;
}

/**
 * Получите фоновое изображение. Может быть указателем на`lv_image_dsc_t`, направляясь к файлу или
 * an `LV_SYMBOL_...`.
 * Default: `NULL` , унаследовано: Нет, макет: Нет, доб. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline const void * lv_obj_get_style_bg_image_src(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_IMAGE_SRC);
    return (const void *)v.ptr;
}

/**
 * Получите непрозрачность фонового изображения. Значение 0, `LV_OPA_0` или `LV_OPA_TRANSP` означает
 * полностью прозрачный, 255, `LV_OPA_100` или `LV_OPA_COVER` означает полное закрытие, другое
 * значения или LV_OPA_10 , LV_OPA_20 и т. д. означают полупрозрачность.
 * Default: `LV_OPA_COVER` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_opa_t lv_obj_get_style_bg_image_opa(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_IMAGE_OPA);
    return (lv_opa_t)v.num;
}

/**
 * Получите цвет для смешивания с фоновым изображением.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_bg_image_recolor(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_IMAGE_RECOLOR);
    return v.color;
}

/**
 * Получите цвет для смешивания с фоновым изображением.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_bg_image_recolor_filtered(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_BG_IMAGE_RECOLOR));
    return v.color;
}

/**
 * Получите интенсивность перекрашивания фонового изображения. Значение 0, `LV_OPA_0` или
 * `LV_OPA_TRANSP` означает отсутствие смешивания, 255, `LV_OPA_100` или `LV_OPA_COVER` означает полное
 * перекрашивание, интерпретируются другие значения или LV_OPA_10 , LV_OPA_20 и т.д.
 * пропорционально.
 * Default: `LV_OPA_TRANSP` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_opa_t lv_obj_get_style_bg_image_recolor_opa(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_IMAGE_RECOLOR_OPA);
    return (lv_opa_t)v.num;
}

/**
 * Если этот параметр включен, фоновое изображение будет расположено на видеоплите. Возможные значения:`true`или`false`.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline bool lv_obj_get_style_bg_image_tiled(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_IMAGE_TILED);
    return (bool)v.num;
}

/**
 * Получить цвет границы.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_border_color(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_COLOR);
    return v.color;
}

/**
 * Получить цвет границы.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_border_color_filtered(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_COLOR));
    return v.color;
}

/**
 * Получите непрозрачность границы. Значение 0, `LV_OPA_0` или `LV_OPA_TRANSP` означает полностью
 * прозрачный, 255, `LV_OPA_100` или `LV_OPA_COVER` означает полное покрытие, другие значения
 * или LV_OPA_10 , LV_OPA_20 и т. д. означает полупрозрачность.
 * Default: `LV_OPA_COVER` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_opa_t lv_obj_get_style_border_opa(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_OPA);
    return (lv_opa_t)v.num;
}

/**
 * Получить ширину границы. Можно использовать только значения пикселей.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_border_width(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_WIDTH);
    return (int32_t)v.num;
}

/**
 * Получите только то, с какой стороны должна быть нарисована граница. Возможные значения:
 * `LV_BORDER_SIDE_NONE/TOP/BOTTOM/LEFT/RIGHT/INTERNAL` .  Значения OR можно использовать как
 * ну, например  `LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_LEFT` .
 * Default: `LV_BORDER_SIDE_FULL` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_border_side_t lv_obj_get_style_border_side(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_SIDE);
    return (lv_border_side_t)v.num;
}

/**
 * Определяет, следует ли рисовать границу до или после рисования дочерних элементов.
 * `true` : после дочерних элементов,`false`: перед дочерними элементами.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline bool lv_obj_get_style_border_post(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_POST);
    return (bool)v.num;
}

/**
 * Получить ширину контура в пикселях.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_outline_width(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OUTLINE_WIDTH);
    return (int32_t)v.num;
}

/**
 * Получите цвет контура.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_outline_color(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OUTLINE_COLOR);
    return v.color;
}

/**
 * Получите цвет контура.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_outline_color_filtered(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_OUTLINE_COLOR));
    return v.color;
}

/**
 * Получите непрозрачность контура. Значение 0, `LV_OPA_0` или `LV_OPA_TRANSP` означает полностью
 * прозрачный, 255, `LV_OPA_100` или `LV_OPA_COVER` означает полное покрытие, другие значения
 * или LV_OPA_10 , LV_OPA_20 и т. д. означает полупрозрачность.
 * Default: `LV_OPA_COVER` , унаследовано: Нет, макет: Нет, доб. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_opa_t lv_obj_get_style_outline_opa(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OUTLINE_OPA);
    return (lv_opa_t)v.num;
}

/**
 * Получите отступы контура, то есть разрыв между виджетом и контуром.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_outline_pad(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OUTLINE_PAD);
    return (int32_t)v.num;
}

/**
 * Получите ширину тени в пикселях. Значение должно быть >= 0.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_shadow_width(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_WIDTH);
    return (int32_t)v.num;
}

/**
 * Получите смещение тени в пикселях по оси X.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_shadow_offset_x(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_OFFSET_X);
    return (int32_t)v.num;
}

/**
 * Получите смещение тени в пикселях в направлении Y.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_shadow_offset_y(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_OFFSET_Y);
    return (int32_t)v.num;
}

/**
 * Выполните расчет тени, чтобы использовать в качестве основы прямоугольник большего или меньшего размера. Значение может
 * быть в пикселях, чтобы сделать область больше/меньше.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_shadow_spread(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_SPREAD);
    return (int32_t)v.num;
}

/**
 * Получите цвет тени.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_shadow_color(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_COLOR);
    return v.color;
}

/**
 * Получите цвет тени.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_shadow_color_filtered(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_COLOR));
    return v.color;
}

/**
 * Получите непрозрачность тени. Значение 0, `LV_OPA_0` или `LV_OPA_TRANSP` означает полностью
 * прозрачный, 255, `LV_OPA_100` или `LV_OPA_COVER` означает полное покрытие, другие значения
 * или LV_OPA_10 , LV_OPA_20 и т. д. означает полупрозрачность.
 * Default: `LV_OPA_COVER` , унаследовано: Нет, макет: Нет, доб. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_opa_t lv_obj_get_style_shadow_opa(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_OPA);
    return (lv_opa_t)v.num;
}

/**
 * Получить непрозрачность изображения. Значение 0, `LV_OPA_0` или `LV_OPA_TRANSP` означает полностью
 * прозрачный, 255, `LV_OPA_100` или `LV_OPA_COVER` означает полное покрытие, другие значения
 * или LV_OPA_10 , LV_OPA_20 и т. д. означает полупрозрачность.
 * Default: `LV_OPA_COVER` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_opa_t lv_obj_get_style_image_opa(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_IMAGE_OPA);
    return (lv_opa_t)v.num;
}

/**
 * Получите цвет, который будет смешиваться с изображением.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_image_recolor(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_IMAGE_RECOLOR);
    return v.color;
}

/**
 * Получите цвет, который будет смешиваться с изображением.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_image_recolor_filtered(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_IMAGE_RECOLOR));
    return v.color;
}

/**
 * Получите интенсивность смешивания цветов. Значение 0, `LV_OPA_0` или `LV_OPA_TRANSP` означает полностью
 * прозрачный, 255, `LV_OPA_100` или `LV_OPA_COVER` означает полное покрытие, другие значения
 * или LV_OPA_10 , LV_OPA_20 и т. д. означает полупрозрачность.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_opa_t lv_obj_get_style_image_recolor_opa(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_IMAGE_RECOLOR_OPA);
    return (lv_opa_t)v.num;
}

/**
 * Получите определение цвета ключа изображения. lv_image_colorkey_tсодержит два значения цвета:
 * `high_color` и`low_color`. цвет изображения от`low_color`до
 * `high_color` будет прозрачнее.
 * Default: `NULL` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline const lv_image_colorkey_t * lv_obj_get_style_image_colorkey(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_IMAGE_COLORKEY);
    return (const lv_image_colorkey_t *)v.ptr;
}

/**
 * Получить ширину линий в пикселях.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_line_width(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_WIDTH);
    return (int32_t)v.num;
}

/**
 * Получить ширину штрихов в пикселях. Обратите внимание, что тире работает только на горизонтальных и вертикальных линиях.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_line_dash_width(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_DASH_WIDTH);
    return (int32_t)v.num;
}

/**
 * Получите промежуток между черточками в пикселях. Обратите внимание, что тире работает только на горизонтальном и
 * вертикальные линии.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_line_dash_gap(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_DASH_GAP);
    return (int32_t)v.num;
}

/**
 * Конечные точки линий делаются закругленными.  `true`: закругленный,`false`: опорное окончание линии.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline bool lv_obj_get_style_line_rounded(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_ROUNDED);
    return (bool)v.num;
}

/**
 * Получить цвет линий.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_line_color(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_COLOR);
    return v.color;
}

/**
 * Получить цвет линий.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_line_color_filtered(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_COLOR));
    return v.color;
}

/**
 * Получите непрозрачность линий.
 * Default: `LV_OPA_COVER` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_opa_t lv_obj_get_style_line_opa(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_OPA);
    return (lv_opa_t)v.num;
}

/**
 * Получить ширину (толщину) дуг в пикселях.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_arc_width(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ARC_WIDTH);
    return (int32_t)v.num;
}

/**
 * Конечные точки сделайте закругленными.  `true`: закругленный,`false`: опорное окончание линии.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline bool lv_obj_get_style_arc_rounded(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ARC_ROUNDED);
    return (bool)v.num;
}

/**
 * Получить цвет дуги.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_arc_color(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ARC_COLOR);
    return v.color;
}

/**
 * Получить цвет дуги.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_arc_color_filtered(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_ARC_COLOR));
    return v.color;
}

/**
 * Получите непрозрачность дуг.
 * Default: `LV_OPA_COVER` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_opa_t lv_obj_get_style_arc_opa(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ARC_OPA);
    return (lv_opa_t)v.num;
}

/**
 * Получите изображение, из которого будет замаскирована дуга. Полезно отображать сложные
 * воздействие на дуги. Может быть указателем на`lv_image_dsc_t`или по пути к файлу.
 * Default: `NULL` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline const void * lv_obj_get_style_arc_image_src(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ARC_IMAGE_SRC);
    return (const void *)v.ptr;
}

/**
 * Получает цвет текста.
 * Default: `0x000000` , унаследовано: Да, расположение: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_text_color(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_COLOR);
    return v.color;
}

/**
 * Получает цвет текста.
 * Default: `0x000000` , унаследовано: Да, расположение: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_text_color_filtered(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_COLOR));
    return v.color;
}

/**
 * Получите непрозрачность текста. Значение 0, `LV_OPA_0` или `LV_OPA_TRANSP` означает полностью
 * прозрачный, 255, `LV_OPA_100` или `LV_OPA_COVER` означает полное покрытие, другие значения
 * или LV_OPA_10 , LV_OPA_20 и т. д. означает полупрозрачность.
 * Default: `LV_OPA_COVER` , унаследовано: Да, расположение: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_opa_t lv_obj_get_style_text_opa(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_OPA);
    return (lv_opa_t)v.num;
}

/**
 * Получить шрифт текста (указатель`lv_font_t *`).
 * Default: `LV_FONT_DEFAULT` , по наследству: Да, раскладка: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline const lv_font_t * lv_obj_get_style_text_font(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_FONT);
    return (const lv_font_t *)v.ptr;
}

/**
 * Получите пространство для букв в пикселях.
 * Default: 0, по наследству: Да, раскладка: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_text_letter_space(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_LETTER_SPACE);
    return (int32_t)v.num;
}

/**
 * Получите строковое пространство в пикселях.
 * Default: 0, по наследству: Да, раскладка: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_text_line_space(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_LINE_SPACE);
    return (int32_t)v.num;
}

/**
 * Получите украшение для текста. Возможные значения:
 * `LV_TEXT_DECOR_NONE/UNDERLINE/STRIKETHROUGH` .  Также можно использовать значения OR.
 * Default: `LV_TEXT_DECOR_NONE` , унаследовано: Да, расположение: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_text_decor_t lv_obj_get_style_text_decor(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_DECOR);
    return (lv_text_decor_t)v.num;
}

/**
 * Узнайте, как выровнять строки текста. Обратите внимание, что это не выравнивает виджет
 * сам по себе, только строки внутри виджета. Возможные значения:
 * `LV_TEXT_ALIGN_LEFT/CENTER/RIGHT/AUTO` .  `LV_TEXT_ALIGN_AUTO` обнаружить текстовую базу
 * направлении и использует выравнивание по левому или правому краю соответственно.
 * Default: `LV_TEXT_ALIGN_AUTO` , по наследству: Да, раскладка: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_text_align_t lv_obj_get_style_text_align(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_ALIGN);
    return (lv_text_align_t)v.num;
}

/**
 * Получает цвет обводки контура буквы.
 * Default: `0x000000` , унаследовано: Да, расположение: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_text_outline_stroke_color(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_OUTLINE_STROKE_COLOR);
    return v.color;
}

/**
 * Получает цвет обводки контура буквы.
 * Default: `0x000000` , унаследовано: Да, расположение: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_text_outline_stroke_color_filtered(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_OUTLINE_STROKE_COLOR));
    return v.color;
}

/**
 * Получите ширину обводки контура буквы в пикселях.
 * Default: 0, по наследству: Да, раскладка: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_text_outline_stroke_width(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_OUTLINE_STROKE_WIDTH);
    return (int32_t)v.num;
}

/**
 * Получите непрозрачность контура буквы. Значение 0, `LV_OPA_0` или
 * `LV_OPA_TRANSP` означает полностью прозрачный, 255, `LV_OPA_100` или `LV_OPA_COVER` означает
 * полное покрытие, другие значения или LV_OPA_10 , LV_OPA_20 и т. д. означают полупрозрачность.
 * Default: `LV_OPA_COVER` , унаследовано: Да, расположение: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_opa_t lv_obj_get_style_text_outline_stroke_opa(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_OUTLINE_STROKE_OPA);
    return (lv_opa_t)v.num;
}

/**
 * При этом приводится размывание. Применяется к каждомуlv_partотдельно перед
 * дети оказываются.
 * Default: `0` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_blur_radius(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BLUR_RADIUS);
    return (int32_t)v.num;
}

/**
 * Если`true`фон виджета будет размыт. Деталь должна иметь < 100 %
 * непрозрачность, чтобы сделать его видимым. Если `false`, работоспособная часть будет размыта при
 * рендерится, но до рисования детей.
 * Default: `false` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline bool lv_obj_get_style_blur_backdrop(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BLUR_BACKDROP);
    return (bool)v.num;
}

/**
 * При установке `LV_BLUR_QUALITY_SPEED` алгоритм размытия будет отдавать предпочтение скорости.
 * качество.  `LV_BLUR_QUALITY_PRECISION` заставит использовать более высокое качество, но медленнее.
 * размытие. При использовании `LV_BLUR_QUALITY_AUTO` качество будет выбрано автоматически.
 * Default: `LV_BLUR_QUALITY_AUTO` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_blur_quality_t lv_obj_get_style_blur_quality(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BLUR_QUALITY);
    return (lv_blur_quality_t)v.num;
}

/**
 * При этом приводится размывание. Применяется к каждомуlv_partотдельно перед
 * дети оказываются.
 * Default: `0` , унаследовано: Нет, макет: Нет, доб. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_drop_shadow_radius(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_DROP_SHADOW_RADIUS);
    return (int32_t)v.num;
}

/**
 * Получите смещение тени в пикселях по оси X.
 * Default: `0` , унаследовано: Нет, макет: Нет, доб. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_drop_shadow_offset_x(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_DROP_SHADOW_OFFSET_X);
    return (int32_t)v.num;
}

/**
 * Получите смещение тени в пикселях в направлении Y.
 * Default: `0` , унаследовано: Нет, макет: Нет, доб. рисовать: Да.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_drop_shadow_offset_y(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_DROP_SHADOW_OFFSET_Y);
    return (int32_t)v.num;
}

/**
 * Получите цвет тени.
 * Default: `0` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_drop_shadow_color(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_DROP_SHADOW_COLOR);
    return v.color;
}

/**
 * Получите цвет тени.
 * Default: `0` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_drop_shadow_color_filtered(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_DROP_SHADOW_COLOR));
    return v.color;
}

/**
 * Получите непрозрачность тени.
 * Default: `0` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_opa_t lv_obj_get_style_drop_shadow_opa(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_DROP_SHADOW_OPA);
    return (lv_opa_t)v.num;
}

/**
 * При установке `LV_BLUR_QUALITY_SPEED` алгоритм размытия будет отдавать предпочтение скорости.
 * качество.  `LV_BLUR_QUALITY_PRECISION` заставит использовать более высокое качество, но медленнее.
 * размытие. При использовании `LV_BLUR_QUALITY_AUTO` качество будет выбрано автоматически.
 * Default: `LV_BLUR_QUALITY_PRECISION` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_blur_quality_t lv_obj_get_style_drop_shadow_quality(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_DROP_SHADOW_QUALITY);
    return (lv_blur_quality_t)v.num;
}

/**
 * Получите радиус на каждом углу. Значение интерпретируется в пикселях (>= 0) или
 * `LV_RADIUS_CIRCLE` для максимального радиуса.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_radius(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_RADIUS);
    return (int32_t)v.num;
}

/**
 * Переместите начальную точку объекта (например, отметку масштаба) радиально.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_radial_offset(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_RADIAL_OFFSET);
    return (int32_t)v.num;
}

/**
 * Включите обрезку содержимого, выходящего за закругленные углы родительского виджета. Может быть
 * `true` или`false`.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline bool lv_obj_get_style_clip_corner(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_CLIP_CORNER);
    return (bool)v.num;
}

/**
 * Уменьшите все значения непрозрачности виджета на этот коэффициент. Значение 0, `LV_OPA_0` или
 * `LV_OPA_TRANSP` означает полностью прозрачный, 255, `LV_OPA_100` или `LV_OPA_COVER` означает
 * полное покрытие, другие значения или LV_OPA_10 , LV_OPA_20 и т. д. означают полупрозрачность.
 * Default: `LV_OPA_COVER` , унаследовано: Да, расположение: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_opa_t lv_obj_get_style_opa(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OPA);
    return (lv_opa_t)v.num;
}

/**
 * Сначала нарисуйте виджет на слое, затем уменьшите коэффициент непрозрачности слоя. Значение 0,
 * `LV_OPA_0` или `LV_OPA_TRANSP` означает полностью прозрачный, 255, `LV_OPA_100` или
 * `LV_OPA_COVER` означает полное покрытие, другие значения или LV_OPA_10 , LV_OPA_20 и т. д.
 * означает полупрозрачность.
 * Default: `LV_OPA_COVER` , унаследовано: Да, расположение: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_opa_t lv_obj_get_style_opa_layered(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OPA_LAYERED);
    return (lv_opa_t)v.num;
}

/**
 * Смешайте цвет со всеми цветами виджета.
 * Default: `NULL` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline const lv_color_filter_dsc_t * lv_obj_get_style_color_filter_dsc(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_COLOR_FILTER_DSC);
    return (const lv_color_filter_dsc_t *)v.ptr;
}

/**
 * Интенсивность смешивания цветового фильтра.
 * Default: `LV_OPA_TRANSP` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_opa_t lv_obj_get_style_color_filter_opa(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_COLOR_FILTER_OPA);
    return (lv_opa_t)v.num;
}

/**
 * Получите цвет, который можно смешать с объектом.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_color_t lv_obj_get_style_recolor(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_RECOLOR);
    return v.color;
}

/**
 * Получает интенсивность смешивания цветов. Значение 0, `LV_OPA_0` или `LV_OPA_TRANSP` означает
 * полностью прозрачный. Значение 255, `LV_OPA_100` или `LV_OPA_COVER` означает полностью
 * непрозрачный. Промежуточные значения, такие как LV_OPA_10 , LV_OPA_20 и т. д., приводят к
 * полупрозрачность.
 * Default: `LV_OPA_TRANSP` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_opa_t lv_obj_get_style_recolor_opa(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_RECOLOR_OPA);
    return (lv_opa_t)v.num;
}

/**
 * Шаблон анимации для анимированного виджета. Должен быть указателем на `lv_anim_t`.
 * параметры анимации зависят от виджета, например. время анимации может быть, например,
 * время мигания курсора в текстовой области или время прокрутки ролика. См. Виджеты
 * документацию, чтобы узнать больше.
 * Default: `NULL` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline const lv_anim_t * lv_obj_get_style_anim(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ANIM);
    return (const lv_anim_t *)v.ptr;
}

/**
 * Продолжительность анимации в миллисекундах. Его значение зависит от виджета. Например. время мигания
 * курсора в текстовой области или время прокрутки ролика. См. Виджеты
 * документацию, чтобы узнать больше.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline uint32_t lv_obj_get_style_anim_duration(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ANIM_DURATION);
    return (uint32_t)v.num;
}

/**
 * Инициализированный``lv_style_transition_dsc_t``для описания перехода.
 * Default: `NULL` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline const lv_style_transition_dsc_t * lv_obj_get_style_transition(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSITION);
    return (const lv_style_transition_dsc_t *)v.ptr;
}

/**
 * Описывает, как смешать цвета с фоном. Возможные значения:
 * `LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE/MULTIPLY/DIFFERENCE`.
 * Default: `LV_BLEND_MODE_NORMAL` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_blend_mode_t lv_obj_get_style_blend_mode(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BLEND_MODE);
    return (lv_blend_mode_t)v.num;
}

/**
 * Получить макет виджета. Дочерние элементы будут перемещены и изменены в соответствии с
 * политики, установленные для макета. Возможные значения см. в документации макетов.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline uint16_t lv_obj_get_style_layout(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LAYOUT);
    return (uint16_t)v.num;
}

/**
 * Получить базовое направление виджета. Возможные значения: `LV_BIDI_DIR_LTR/RTL/AUTO` .
 * Default: `LV_BASE_DIR_AUTO` , по наследству: Да, раскладка: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_base_dir_t lv_obj_get_style_base_dir(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BASE_DIR);
    return (lv_base_dir_t)v.num;
}

/**
 * Если установлено, для виджета будет создан слой, который будет замаскирован
 * это растровая маска A8.
 * Default: `NULL` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline const void * lv_obj_get_style_bitmap_mask_src(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BITMAP_MASK_SRC);
    return (const void *)v.ptr;
}

/**
 * Отрегулируйте чувствительность поворотных энкодеров с точностью до 1/256. Это значит, 128: замедлить
 * поворотный до половины, 512: скорость увеличивается вдвое, 256: без изменений.
 * Default: `256` , унаследовано: Да, расположение: Нет, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline uint32_t lv_obj_get_style_rotary_sensitivity(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ROTARY_SENSITIVITY);
    return (uint32_t)v.num;
}

#if LV_USE_FLEX
/**
 * Определяет, в каком направлении гибкий макет должен располагать дочерние элементы.
 * Default: `LV_FLEX_FLOW_NONE` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_flex_flow_t lv_obj_get_style_flex_flow(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_FLEX_FLOW);
    return (lv_flex_flow_t)v.num;
}

/**
 * Определяет, как выровнять дочерние элементы в направлении гибкого потока.
 * Default: `LV_FLEX_ALIGN_NONE` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_flex_align_t lv_obj_get_style_flex_main_place(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_FLEX_MAIN_PLACE);
    return (lv_flex_align_t)v.num;
}

/**
 * Определяет, как выровнять дочерние элементы перпендикулярно направлению гибкого потока.
 * Default: `LV_FLEX_ALIGN_NONE` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_flex_align_t lv_obj_get_style_flex_cross_place(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_FLEX_CROSS_PLACE);
    return (lv_flex_align_t)v.num;
}

/**
 * Определяет способ выравнивания дорожек потока.
 * Default: `LV_FLEX_ALIGN_NONE` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_flex_align_t lv_obj_get_style_flex_track_place(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_FLEX_TRACK_PLACE);
    return (lv_flex_align_t)v.num;
}

/**
 * Определяет, сколько места пропорционально занять из свободного места на дорожке виджета.
 * Default: `0` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline uint8_t lv_obj_get_style_flex_grow(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_FLEX_GROW);
    return (uint8_t)v.num;
}

#endif /* LV_USE_FLEX */

#if LV_USE_GRID
/**
 * Массив для описания столбцов сетки. Должен быть прекращен LV_GRID_TEMPLATE_LAST.
 * Default: `NULL` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline const int32_t * lv_obj_get_style_grid_column_dsc_array(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_COLUMN_DSC_ARRAY);
    return (const int32_t *)v.ptr;
}

/**
 * Определяет способ распределения столбцов.
 * Default: `LV_GRID_ALIGN_START` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_grid_align_t lv_obj_get_style_grid_column_align(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_COLUMN_ALIGN);
    return (lv_grid_align_t)v.num;
}

/**
 * Массив для описания строк сетки. Должен быть прекращен LV_GRID_TEMPLATE_LAST.
 * Default: `NULL` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline const int32_t * lv_obj_get_style_grid_row_dsc_array(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_ROW_DSC_ARRAY);
    return (const int32_t *)v.ptr;
}

/**
 * Определяет способ распределения строк.
 * Default: `LV_GRID_ALIGN_START` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_grid_align_t lv_obj_get_style_grid_row_align(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_ROW_ALIGN);
    return (lv_grid_align_t)v.num;
}

/**
 * Получить столбец, в котором следует разместить виджет.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_grid_cell_column_pos(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_CELL_COLUMN_POS);
    return (int32_t)v.num;
}

/**
 * Узнайте, как выровнять виджет по горизонтали.
 * Default: `LV_GRID_ALIGN_START` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_grid_align_t lv_obj_get_style_grid_cell_x_align(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_CELL_X_ALIGN);
    return (lv_grid_align_t)v.num;
}

/**
 * Узнайте, сколько столбцов должен охватывать виджет. Должно быть >= 1.
 * Default: 1, по наследству: Нет, раскладка: Да, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_grid_cell_column_span(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_CELL_COLUMN_SPAN);
    return (int32_t)v.num;
}

/**
 * Получить строку, в которую следует поместить виджет.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_grid_cell_row_pos(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_CELL_ROW_POS);
    return (int32_t)v.num;
}

/**
 * Узнайте, как выровнять виджет по вертикали.
 * Default: `LV_GRID_ALIGN_START` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline lv_grid_align_t lv_obj_get_style_grid_cell_y_align(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_CELL_Y_ALIGN);
    return (lv_grid_align_t)v.num;
}

/**
 * Узнайте, сколько строк должен занимать виджет. Должно быть >= 1.
 * Default: 1, по наследству: Нет, раскладка: Да, доб. рисовать: Нет.
 * @param  obj    Указатель на виджет
 * @param  part   Одно из значений перечисления `LV_PART_...`
 */
static inline int32_t lv_obj_get_style_grid_cell_row_span(const lv_obj_t * obj, lv_part_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_CELL_ROW_SPAN);
    return (int32_t)v.num;
}

#endif /* LV_USE_GRID */

/**
 * Устанавливает ширину виджета. Можно использовать значения в пикселях, процентах и ​​`LV_SIZE_CONTENT`.
 * Процентные значения относятся к ширине родительской области содержимого.
 * Default: Зависит от виджета, наследуется: Нет, макет: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_width(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Устанавливает минимальную ширину. Можно использовать пиксельные и процентные значения. Процентные значения
 * относятся к ширине области содержимого родительского элемента.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_min_width(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Устанавливает максимальную ширину. Можно использовать пиксельные и процентные значения. Процентные значения
 * относятся к ширине области содержимого родительского элемента.
 * Default: LV_COORD_MAX , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_max_width(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Устанавливает высоту виджета. Можно использовать пиксель, проценты и `LV_SIZE_CONTENT`.
 * Процентные значения относятся к высоте родительской области содержимого.
 * Default: Зависит от виджета, наследуется: Нет, макет: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_height(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Устанавливает минимальную высоту. Можно использовать пиксельные и процентные значения. Процентные значения
 * относятся к высоте области содержимого родительского элемента.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_min_height(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Устанавливает максимальную высоту. Можно использовать пиксельные и процентные значения. Процентные значения
 * относятся к высоте области содержимого родительского элемента.
 * Default: LV_COORD_MAX , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_max_height(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Его значение зависит от типа виджета. Например, в случаеlv_scaleэто означает
 * длина тиков.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_length(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите координату X виджета с учетом настроек``align``. Пиксель и процент
 * можно использовать значения. Процентные значения относятся к ширине родительского элемента.
 * область контента.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_x(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите координату Y виджета с учетом настроек``align``. Пиксель и процент
 * можно использовать значения. Процентные значения относятся к высоте родительского элемента.
 * область контента.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_y(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите выравнивание, которое указывает, из какой точки родительского элемента расположены X и Y.
 * координаты должны быть интерпретированы. Возможные значения: `LV_ALIGN_DEFAULT` ,
 * `LV_ALIGN_TOP_LEFT/MID/RIGHT`, `LV_ALIGN_BOTTOM_LEFT/MID/RIGHT`,
 * `LV_ALIGN_LEFT/RIGHT_MID` , `LV_ALIGN_CENTER` .  `LV_ALIGN_DEFAULT` означает
 * `LV_ALIGN_TOP_LEFT` с направлением основания LTR и `LV_ALIGN_TOP_RIGHT` с основанием RTL
 * направление.
 * Default: `LV_ALIGN_DEFAULT` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_align(lv_obj_t * obj, lv_align_t value, lv_style_selector_t selector);

/**
 * С помощью этого значения сделайте виджет шире с обеих сторон. Пиксель и процент (с
 * `lv_pct(x)` ) можно использовать значения. Процентные значения относятся к расширению виджета.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_transform_width(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Сделайте виджет выше с обеих сторон с этим значением. Пиксель и процент (с
 * `lv_pct(x)` ) можно использовать значения. Процентные значения относятся к высоте виджета.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_transform_height(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Переместите виджет с этим значением в направлении X. Применяется после макетов, выравниваний и других
 * позиционирование. Можно использовать значения в пикселях и процентах (с `lv_pct(x)`). Процент
 * значения относятся к ширине виджета.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_translate_x(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Переместите виджет с этим значением в направлении Y. Применяется после макетов, выравниваний и других
 * позиционирование. Можно использовать значения в пикселях и процентах (с `lv_pct(x)`). Процент
 * значения относятся к высоте виджета.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_translate_y(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Переместите объект вокруг центра родительского объекта (например, по окружности).
 * масштаба).
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_translate_radial(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Масштабировать виджет по горизонтали. Значение 256 (или `LV_SCALE_NONE` ) означает нормальный размер, 128.
 * половинный размер, 512 двойной размер и так далее.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_transform_scale_x(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Масштабировать виджет по вертикали. Значение 256 (или `LV_SCALE_NONE`) означает нормальный размер, 128.
 * половинный размер, 512 двойной размер и так далее.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_transform_scale_y(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Поворот виджета. Значение интерпретируется в единицах 0,1 градуса. Например. 450 означает 45 градусов.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_transform_rotation(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите координату X точки поворота для преобразований. Относительно верхнего левого угла виджета.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_transform_pivot_x(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите координату Y точки поворота для преобразований. Относительно верхнего левого угла виджета.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_transform_pivot_y(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Наклон виджета по горизонтали. Значение интерпретируется в единицах 0,1 градуса. Например. 450
 * значит 45 град.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_transform_skew_x(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Наклон виджета по вертикали. Значение интерпретируется в единицах 0,1 градуса. Например. 450
 * значит 45 град.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_transform_skew_y(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Устанавливает отступ сверху. Это уменьшает область контента в этом направлении.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_pad_top(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Устанавливает отступ внизу. Это уменьшает область контента в этом направлении.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_pad_bottom(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Устанавливает отступ слева. Это уменьшает область контента в этом направлении.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_pad_left(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Устанавливает отступ справа. Это уменьшает область контента в этом направлении.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_pad_right(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Устанавливает отступы между строками. Используется макетами.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_pad_row(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Устанавливает отступы между столбцами. Используется макетами.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_pad_column(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Отложите текстовые метки от делений шкалы/остатка `` LV_PART_ `` .
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_pad_radial(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Устанавливает поле сверху. Виджет сохранит это пространство от своих собратьев в макетах.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_margin_top(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Устанавливает поле внизу. Виджет сохранит это пространство от своих собратьев в макетах.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_margin_bottom(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Устанавливает поле слева. Виджет сохранит это пространство от своих собратьев в макетах.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_margin_left(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Устанавливает поле справа. Виджет сохранит это пространство от своих собратьев в макетах.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_margin_right(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите цвет фона виджета.
 * Default: `0xffffff` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Цвет для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_bg_color(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);

/**
 * Установите непрозрачность фона. Значение 0, `LV_OPA_0` или `LV_OPA_TRANSP` означает полностью
 * прозрачный, 255, `LV_OPA_100` или `LV_OPA_COVER` означает полное покрытие, другие значения
 * или LV_OPA_10 , LV_OPA_20 и т. д. означает полупрозрачность.
 * Default: `LV_OPA_TRANSP` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_bg_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);

/**
 * Установите градиентный цветной фон. Используется только в том случае, если`grad_dir`не`LV_GRAD_DIR_NONE`.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Цвет для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_bg_grad_color(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);

/**
 * Установите направление градиента фона. Возможные значения:
 * `LV_GRAD_DIR_NONE/HOR/VER`.
 * Default: `LV_GRAD_DIR_NONE` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_bg_grad_dir(lv_obj_t * obj, lv_grad_dir_t value, lv_style_selector_t selector);

/**
 * Установите точку, с которой должен начинаться цвет фона для градиентов. 0 означает
 * верхняя/левая сторона, 255 нижняя/правая сторона, 128 центр и так далее.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_bg_main_stop(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите точку, с которой должен начинаться градиент цвета фона. 0 означает вверх/влево
 * сторона, 255 нижняя/правая сторона, 128 центр и так далее.
 * Default: 255, по наследству: Нет, планировка: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_bg_grad_stop(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите непрозрачность первого цвета градиента.
 * Default: 255, по наследству: Нет, планировка: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_bg_main_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);

/**
 * Установите непрозрачность второго цвета градиента.
 * Default: 255, по наследству: Нет, планировка: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_bg_grad_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);

/**
 * Установите определение градиента. Указанный экземпляр должен существовать, пока жив виджет.
 * NULL для отключения. Он обертывает `BG_GRAD_COLOR` , `BG_GRAD_DIR` , `BG_MAIN_STOP` и
 * `BG_GRAD_STOP` в один дескриптор и позволяет создавать градиенты с большим количеством цветов.
 * также. Если он установлен, другие свойства, связанные с градиентом, будут игнорироваться.
 * Default: `NULL` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Указатель на дескриптор градиента
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_bg_grad(lv_obj_t * obj, const lv_grad_dsc_t * value, lv_style_selector_t selector);

/**
 * Установите фоновое изображение. Может быть указателем на`lv_image_dsc_t`, направляясь к файлу или
 * an `LV_SYMBOL_...`.
 * Default: `NULL` , унаследовано: Нет, макет: Нет, доб. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Указатель на источник изображения
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_bg_image_src(lv_obj_t * obj, const void * value, lv_style_selector_t selector);

/**
 * Установите непрозрачность фонового изображения. Значение 0, `LV_OPA_0` или `LV_OPA_TRANSP` означает
 * полностью прозрачный, 255, `LV_OPA_100` или `LV_OPA_COVER` означает полное закрытие, другое
 * значения или LV_OPA_10 , LV_OPA_20 и т. д. означают полупрозрачность.
 * Default: `LV_OPA_COVER` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_bg_image_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);

/**
 * Установите цвет, который будет смешиваться с фоновым изображением.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Цвет для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_bg_image_recolor(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);

/**
 * Установите интенсивность перекраски фонового изображения. Значение 0, `LV_OPA_0` или
 * `LV_OPA_TRANSP` означает отсутствие смешивания, 255, `LV_OPA_100` или `LV_OPA_COVER` означает полное
 * перекрашивание, интерпретируются другие значения или LV_OPA_10 , LV_OPA_20 и т.д.
 * пропорционально.
 * Default: `LV_OPA_TRANSP` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_bg_image_recolor_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);

/**
 * Если этот параметр включен, фоновое изображение будет расположено на видеоплите. Возможные значения:`true`или`false`.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_bg_image_tiled(lv_obj_t * obj, bool value, lv_style_selector_t selector);

/**
 * Установите цвет границы.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Цвет для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_border_color(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);

/**
 * Установите непрозрачность границы. Значение 0, `LV_OPA_0` или `LV_OPA_TRANSP` означает полностью
 * прозрачный, 255, `LV_OPA_100` или `LV_OPA_COVER` означает полное покрытие, другие значения
 * или LV_OPA_10 , LV_OPA_20 и т. д. означает полупрозрачность.
 * Default: `LV_OPA_COVER` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_border_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);

/**
 * Установите ширину границы. Можно использовать только значения пикселей.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_border_width(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите только, с какой стороны(-ов) должна быть нарисована граница. Возможные значения:
 * `LV_BORDER_SIDE_NONE/TOP/BOTTOM/LEFT/RIGHT/INTERNAL` .  Значения OR можно использовать как
 * ну, например  `LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_LEFT` .
 * Default: `LV_BORDER_SIDE_FULL` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_border_side(lv_obj_t * obj, lv_border_side_t value, lv_style_selector_t selector);

/**
 * Устанавливает, должна ли граница рисоваться до или после рисования дочерних элементов.
 * `true` : после дочерних элементов,`false`: перед дочерними элементами.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_border_post(lv_obj_t * obj, bool value, lv_style_selector_t selector);

/**
 * Установите ширину контура в пикселях.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_outline_width(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите цвет контура.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Цвет для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_outline_color(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);

/**
 * Установите непрозрачность контура. Значение 0, `LV_OPA_0` или `LV_OPA_TRANSP` означает полностью
 * прозрачный, 255, `LV_OPA_100` или `LV_OPA_COVER` означает полное покрытие, другие значения
 * или LV_OPA_10 , LV_OPA_20 и т. д. означает полупрозрачность.
 * Default: `LV_OPA_COVER` , унаследовано: Нет, макет: Нет, доб. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_outline_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);

/**
 * Установите отступы контура, то есть зазор между виджетом и контуром.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_outline_pad(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите ширину тени в пикселях. Значение должно быть >= 0.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_shadow_width(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите смещение тени в пикселях по оси X.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_shadow_offset_x(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите смещение тени в пикселях по направлению Y.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_shadow_offset_y(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Выполните расчет тени, чтобы использовать в качестве основы прямоугольник большего или меньшего размера. Значение может
 * быть в пикселях, чтобы сделать область больше/меньше.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_shadow_spread(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите цвет тени.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Цвет для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_shadow_color(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);

/**
 * Установите непрозрачность тени. Значение 0, `LV_OPA_0` или `LV_OPA_TRANSP` означает полностью
 * прозрачный, 255, `LV_OPA_100` или `LV_OPA_COVER` означает полное покрытие, другие значения
 * или LV_OPA_10 , LV_OPA_20 и т. д. означает полупрозрачность.
 * Default: `LV_OPA_COVER` , унаследовано: Нет, макет: Нет, доб. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_shadow_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);

/**
 * Установите непрозрачность изображения. Значение 0, `LV_OPA_0` или `LV_OPA_TRANSP` означает полностью
 * прозрачный, 255, `LV_OPA_100` или `LV_OPA_COVER` означает полное покрытие, другие значения
 * или LV_OPA_10 , LV_OPA_20 и т. д. означает полупрозрачность.
 * Default: `LV_OPA_COVER` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_image_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);

/**
 * Установите цвет, который будет смешиваться с изображением.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Цвет для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_image_recolor(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);

/**
 * Установите интенсивность смешивания цветов. Значение 0, `LV_OPA_0` или `LV_OPA_TRANSP` означает полностью
 * прозрачный, 255, `LV_OPA_100` или `LV_OPA_COVER` означает полное покрытие, другие значения
 * или LV_OPA_10 , LV_OPA_20 и т. д. означает полупрозрачность.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_image_recolor_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);

/**
 * Установите определение цвета клавишей изображения. lv_image_colorkey_tсодержит два значения цвета:
 * `high_color` и`low_color`. цвет изображения от`low_color`до
 * `high_color` будет прозрачнее.
 * Default: `NULL` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Указатель на ключ цвета изображения
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_image_colorkey(lv_obj_t * obj, const lv_image_colorkey_t * value, lv_style_selector_t selector);

/**
 * Установите ширину линий в пикселях.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_line_width(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите ширину штрихов в пикселях. Обратите внимание, что тире работает только на горизонтальных и вертикальных линиях.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_line_dash_width(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите промежуток между черточками в пикселях. Обратите внимание, что тире работает только на горизонтальном и
 * вертикальные линии.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_line_dash_gap(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Конечные точки линий делаются закругленными.  `true`: закругленный,`false`: опорное окончание линии.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_line_rounded(lv_obj_t * obj, bool value, lv_style_selector_t selector);

/**
 * Установить цвет линий.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Цвет для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_line_color(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);

/**
 * Установите непрозрачность линий.
 * Default: `LV_OPA_COVER` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_line_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);

/**
 * Задайте ширину (толщину) дуг в пикселях.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_arc_width(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Конечные точки сделайте закругленными.  `true`: закругленный,`false`: опорное окончание линии.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_arc_rounded(lv_obj_t * obj, bool value, lv_style_selector_t selector);

/**
 * Установить цвет дуги.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Цвет для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_arc_color(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);

/**
 * Установите непрозрачность дуг.
 * Default: `LV_OPA_COVER` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_arc_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);

/**
 * Установите изображение, из которого будет замаскирована дуга. Полезно отображать сложные
 * воздействие на дуги. Может быть указателем на`lv_image_dsc_t`или по пути к файлу.
 * Default: `NULL` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Указатель на источник изображения
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_arc_image_src(lv_obj_t * obj, const void * value, lv_style_selector_t selector);

/**
 * Устанавливает цвет текста.
 * Default: `0x000000` , унаследовано: Да, расположение: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Цвет для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_text_color(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);

/**
 * Установите непрозрачность текста. Значение 0, `LV_OPA_0` или `LV_OPA_TRANSP` означает полностью
 * прозрачный, 255, `LV_OPA_100` или `LV_OPA_COVER` означает полное покрытие, другие значения
 * или LV_OPA_10 , LV_OPA_20 и т. д. означает полупрозрачность.
 * Default: `LV_OPA_COVER` , унаследовано: Да, расположение: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_text_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);

/**
 * Установите шрифт текста (указатель`lv_font_t *`).
 * Default: `LV_FONT_DEFAULT` , по наследству: Да, раскладка: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Указатель на шрифт
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_text_font(lv_obj_t * obj, const lv_font_t * value, lv_style_selector_t selector);

/**
 * Установите расстояние между буквами в пикселях.
 * Default: 0, по наследству: Да, раскладка: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_text_letter_space(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите межстрочный интервал в пикселях.
 * Default: 0, по наследству: Да, раскладка: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_text_line_space(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите оформление текста. Возможные значения:
 * `LV_TEXT_DECOR_NONE/UNDERLINE/STRIKETHROUGH` .  Также можно использовать значения OR.
 * Default: `LV_TEXT_DECOR_NONE` , унаследовано: Да, расположение: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_text_decor(lv_obj_t * obj, lv_text_decor_t value, lv_style_selector_t selector);

/**
 * Установите способ выравнивания строк текста. Обратите внимание, что это не выравнивает виджет
 * сам по себе, только строки внутри виджета. Возможные значения:
 * `LV_TEXT_ALIGN_LEFT/CENTER/RIGHT/AUTO` .  `LV_TEXT_ALIGN_AUTO` обнаружить текстовую базу
 * направлении и использует выравнивание по левому или правому краю соответственно.
 * Default: `LV_TEXT_ALIGN_AUTO` , по наследству: Да, раскладка: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_text_align(lv_obj_t * obj, lv_text_align_t value, lv_style_selector_t selector);

/**
 * Устанавливает цвет обводки контура буквы.
 * Default: `0x000000` , унаследовано: Да, расположение: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Цвет для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_text_outline_stroke_color(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);

/**
 * Установите ширину обводки контура буквы в пикселях.
 * Default: 0, по наследству: Да, раскладка: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_text_outline_stroke_width(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите непрозрачность контура буквы. Значение 0, `LV_OPA_0` или
 * `LV_OPA_TRANSP` означает полностью прозрачный, 255, `LV_OPA_100` или `LV_OPA_COVER` означает
 * полное покрытие, другие значения или LV_OPA_10 , LV_OPA_20 и т. д. означают полупрозрачность.
 * Default: `LV_OPA_COVER` , унаследовано: Да, расположение: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_text_outline_stroke_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);

/**
 * Конфигуратор включает размытие. Применяется к каждомуlv_partотдельно перед
 * дети оказываются.
 * Default: `0` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_blur_radius(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Если`true`фон виджета будет размыт. Деталь должна иметь < 100 %
 * непрозрачность, чтобы сделать его видимым. Если `false`, работоспособная часть будет размыта при
 * рендерится, но до рисования детей.
 * Default: `false` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_blur_backdrop(lv_obj_t * obj, bool value, lv_style_selector_t selector);

/**
 * При установке `LV_BLUR_QUALITY_SPEED` алгоритм размытия будет отдавать предпочтение скорости.
 * качество.  `LV_BLUR_QUALITY_PRECISION` заставит использовать более высокое качество, но медленнее.
 * размытие. При использовании `LV_BLUR_QUALITY_AUTO` качество будет выбрано автоматически.
 * Default: `LV_BLUR_QUALITY_AUTO` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_blur_quality(lv_obj_t * obj, lv_blur_quality_t value, lv_style_selector_t selector);

/**
 * Конфигуратор включает размытие. Применяется к каждомуlv_partотдельно перед
 * дети оказываются.
 * Default: `0` , унаследовано: Нет, макет: Нет, доб. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_drop_shadow_radius(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите смещение тени в пикселях по оси X.
 * Default: `0` , унаследовано: Нет, макет: Нет, доб. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_drop_shadow_offset_x(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите смещение тени в пикселях по направлению Y.
 * Default: `0` , унаследовано: Нет, макет: Нет, доб. рисовать: Да.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_drop_shadow_offset_y(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите цвет тени.
 * Default: `0` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Цвет для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_drop_shadow_color(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);

/**
 * Установите непрозрачность тени.
 * Default: `0` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_drop_shadow_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);

/**
 * При установке `LV_BLUR_QUALITY_SPEED` алгоритм размытия будет отдавать предпочтение скорости.
 * качество.  `LV_BLUR_QUALITY_PRECISION` заставит использовать более высокое качество, но медленнее.
 * размытие. При использовании `LV_BLUR_QUALITY_AUTO` качество будет выбрано автоматически.
 * Default: `LV_BLUR_QUALITY_PRECISION` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_drop_shadow_quality(lv_obj_t * obj, lv_blur_quality_t value, lv_style_selector_t selector);

/**
 * Установите радиус на каждом углу. Значение интерпретируется в пикселях (>= 0) или
 * `LV_RADIUS_CIRCLE` для максимального радиуса.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_radius(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Переместите начальную точку объекта (например, отметку масштаба) радиально.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_radial_offset(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Включите обрезку содержимого, выходящего за закругленные углы родительского виджета. Может быть
 * `true` или`false`.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_clip_corner(lv_obj_t * obj, bool value, lv_style_selector_t selector);

/**
 * Уменьшите все значения непрозрачности виджета на этот коэффициент. Значение 0, `LV_OPA_0` или
 * `LV_OPA_TRANSP` означает полностью прозрачный, 255, `LV_OPA_100` или `LV_OPA_COVER` означает
 * полное покрытие, другие значения или LV_OPA_10 , LV_OPA_20 и т. д. означают полупрозрачность.
 * Default: `LV_OPA_COVER` , унаследовано: Да, расположение: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);

/**
 * Сначала нарисуйте виджет на слое, затем уменьшите коэффициент непрозрачности слоя. Значение 0,
 * `LV_OPA_0` или `LV_OPA_TRANSP` означает полностью прозрачный, 255, `LV_OPA_100` или
 * `LV_OPA_COVER` означает полное покрытие, другие значения или LV_OPA_10 , LV_OPA_20 и т. д.
 * означает полупрозрачность.
 * Default: `LV_OPA_COVER` , унаследовано: Да, расположение: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_opa_layered(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);

/**
 * Смешайте цвет со всеми цветами виджета.
 * Default: `NULL` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Указатель на дескриптор цветового фильтра
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_color_filter_dsc(lv_obj_t * obj, const lv_color_filter_dsc_t * value, lv_style_selector_t selector);

/**
 * Интенсивность смешивания цветового фильтра.
 * Default: `LV_OPA_TRANSP` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_color_filter_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);

/**
 * Установите цвет для смешивания с объектом.
 * Default: `0x000000` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Цвет для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_recolor(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);

/**
 * Устанавливает интенсивность смешивания цветов. Значение 0, `LV_OPA_0` или `LV_OPA_TRANSP` означает
 * полностью прозрачный. Значение 255, `LV_OPA_100` или `LV_OPA_COVER` означает полностью
 * непрозрачный. Промежуточные значения, такие как LV_OPA_10 , LV_OPA_20 и т. д., приводят к
 * полупрозрачность.
 * Default: `LV_OPA_TRANSP` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_recolor_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);

/**
 * Шаблон анимации для анимированного виджета. Должен быть указателем на `lv_anim_t`.
 * параметры анимации зависят от виджета, например. время анимации может быть, например,
 * время мигания курсора в текстовой области или время прокрутки ролика. См. Виджеты
 * документацию, чтобы узнать больше.
 * Default: `NULL` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Указатель на дескриптор анимации
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_anim(lv_obj_t * obj, const lv_anim_t * value, lv_style_selector_t selector);

/**
 * Продолжительность анимации в миллисекундах. Его значение зависит от виджета. Например. время мигания
 * курсора в текстовой области или время прокрутки ролика. См. Виджеты
 * документацию, чтобы узнать больше.
 * Default: 0, унаследовано: Нет, расположение: Нет, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_anim_duration(lv_obj_t * obj, uint32_t value, lv_style_selector_t selector);

/**
 * Инициализированный``lv_style_transition_dsc_t``для описания перехода.
 * Default: `NULL` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Указатель на дескриптор перехода
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_transition(lv_obj_t * obj, const lv_style_transition_dsc_t * value, lv_style_selector_t selector);

/**
 * Описывает, как смешать цвета с фоном. Возможные значения:
 * `LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE/MULTIPLY/DIFFERENCE`.
 * Default: `LV_BLEND_MODE_NORMAL` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_blend_mode(lv_obj_t * obj, lv_blend_mode_t value, lv_style_selector_t selector);

/**
 * Установите макет виджета. Дочерние элементы будут перемещены и изменены в соответствии с
 * политики, установленные для макета. Возможные значения см. в документации макетов.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_layout(lv_obj_t * obj, uint16_t value, lv_style_selector_t selector);

/**
 * Установите базовое направление виджета. Возможные значения: `LV_BIDI_DIR_LTR/RTL/AUTO` .
 * Default: `LV_BASE_DIR_AUTO` , по наследству: Да, раскладка: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_base_dir(lv_obj_t * obj, lv_base_dir_t value, lv_style_selector_t selector);

/**
 * Если установлено, для виджета будет создан слой, который будет замаскирован
 * это растровая маска A8.
 * Default: `NULL` , унаследовано: Нет, макет: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Указатель на растровую маску A8
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_bitmap_mask_src(lv_obj_t * obj, const void * value, lv_style_selector_t selector);

/**
 * Отрегулируйте чувствительность поворотных энкодеров с точностью до 1/256. Это значит, 128: замедлить
 * поворотный до половины, 512: скорость увеличивается вдвое, 256: без изменений.
 * Default: `256` , унаследовано: Да, расположение: Нет, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_rotary_sensitivity(lv_obj_t * obj, uint32_t value, lv_style_selector_t selector);

#if LV_USE_FLEX
/**
 * Определяет, в каком направлении гибкий макет должен располагать дочерние элементы.
 * Default: `LV_FLEX_FLOW_NONE` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_flex_flow(lv_obj_t * obj, lv_flex_flow_t value, lv_style_selector_t selector);

/**
 * Определяет, как выровнять дочерние элементы в направлении гибкого потока.
 * Default: `LV_FLEX_ALIGN_NONE` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_flex_main_place(lv_obj_t * obj, lv_flex_align_t value, lv_style_selector_t selector);

/**
 * Определяет, как выровнять дочерние элементы перпендикулярно направлению гибкого потока.
 * Default: `LV_FLEX_ALIGN_NONE` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_flex_cross_place(lv_obj_t * obj, lv_flex_align_t value, lv_style_selector_t selector);

/**
 * Определяет способ выравнивания дорожек потока.
 * Default: `LV_FLEX_ALIGN_NONE` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_flex_track_place(lv_obj_t * obj, lv_flex_align_t value, lv_style_selector_t selector);

/**
 * Определяет, сколько места пропорционально занять из свободного места на дорожке виджета.
 * Default: `0` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_flex_grow(lv_obj_t * obj, uint8_t value, lv_style_selector_t selector);

#endif /* LV_USE_FLEX */

#if LV_USE_GRID
/**
 * Массив для описания столбцов сетки. Должен быть прекращен LV_GRID_TEMPLATE_LAST.
 * Default: `NULL` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Указатель на массив дескрипторов столбцов сетки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_grid_column_dsc_array(lv_obj_t * obj, const int32_t * value, lv_style_selector_t selector);

/**
 * Определяет способ распределения столбцов.
 * Default: `LV_GRID_ALIGN_START` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_grid_column_align(lv_obj_t * obj, lv_grid_align_t value, lv_style_selector_t selector);

/**
 * Массив для описания строк сетки. Должен быть прекращен LV_GRID_TEMPLATE_LAST.
 * Default: `NULL` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Указатель на массив дескрипторов строк сетки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_grid_row_dsc_array(lv_obj_t * obj, const int32_t * value, lv_style_selector_t selector);

/**
 * Определяет способ распределения строк.
 * Default: `LV_GRID_ALIGN_START` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_grid_row_align(lv_obj_t * obj, lv_grid_align_t value, lv_style_selector_t selector);

/**
 * Установите столбец, в котором должен быть размещен виджет.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_grid_cell_column_pos(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите способ выравнивания виджета по горизонтали.
 * Default: `LV_GRID_ALIGN_START` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_grid_cell_x_align(lv_obj_t * obj, lv_grid_align_t value, lv_style_selector_t selector);

/**
 * Установите, сколько столбцов должен охватывать виджет. Должно быть >= 1.
 * Default: 1, по наследству: Нет, раскладка: Да, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_grid_cell_column_span(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите строку, в которой должен быть размещен виджет.
 * Default: 0, по наследству: Нет, по расположению: Да, доп. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_grid_cell_row_pos(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

/**
 * Установите способ выравнивания виджета по вертикали.
 * Default: `LV_GRID_ALIGN_START` , по наследству: Нет, по расположению: Да, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_grid_cell_y_align(lv_obj_t * obj, lv_grid_align_t value, lv_style_selector_t selector);

/**
 * Установите, сколько строк должен занимать виджет. Должно быть >= 1.
 * Default: 1, по наследству: Нет, раскладка: Да, доб. рисовать: Нет.
 * @param  obj        Указатель на виджет
 * @param  value      Значение для отправки
 * @param  selector   Тип соединения для`lv_part_t`и `lv_state_t`. Примеры значений:
 *                        - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 *                        - `LV_STATE_PRESSED`
 *                        - `LV_PART_KNOB`
 *                        - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
void lv_obj_set_style_grid_cell_row_span(lv_obj_t * obj, int32_t value, lv_style_selector_t selector);

#endif /* LV_USE_GRID */


#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /* LV_OBJ_STYLE_GEN_H */
