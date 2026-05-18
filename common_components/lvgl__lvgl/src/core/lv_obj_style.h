/**
 * @file lv_obj_style.h
 *
 */

#ifndef LV_OBJ_STYLE_H
#define LV_OBJ_STYLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_bidi.h"
#include "../misc/lv_style.h"
#include "../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Возможные состояния виджета.
 * Значения можно объединять побитовым ИЛИ
 */
typedef enum {
    LV_STATE_DEFAULT     = 0,
    LV_STATE_ALT         = 1 << 0,
    /*1 зарезервировано*/
    LV_STATE_CHECKED     = 1 << 2,
    LV_STATE_FOCUSED     = 1 << 3,
    LV_STATE_FOCUS_KEY   = 1 << 4,
    LV_STATE_EDITED      = 1 << 5,
    LV_STATE_HOVERED     = 1 << 6,
    LV_STATE_PRESSED     = 1 << 7,
    LV_STATE_SCROLLED    = 1 << 8,
    LV_STATE_DISABLED    = 1 << 9,
    /*2 зарезервировано*/
    LV_STATE_USER_1      = 1 << 12,
    LV_STATE_USER_2      = 1 << 13,
    LV_STATE_USER_3      = 1 << 14,
    LV_STATE_USER_4      = 1 << 15,

    LV_STATE_ANY         = 0xFFFF,  /**< Специальное значение: все состояния*/
} lv_state_t;

/**
 * Возможные части виджетов.
 * Части можно рассматривать как внутренние строительные блоки виджетов.
 * например ползунок = фон + индикатор + ручка
 * Не все части используются каждым виджетом
 */

typedef enum {
    LV_PART_MAIN         = 0x000000,  /**< Основная фоновая часть, обычно прямоугольник*/
    LV_PART_SCROLLBAR    = 0x010000,  /**< Полоса или полосы прокрутки*/
    LV_PART_INDICATOR    = 0x020000,  /**< Индикатор: у слайдера, шкалы, переключателя или checkbox*/
    LV_PART_KNOB         = 0x030000,  /**< Ручка, за которую тянут для изменения значения*/
    LV_PART_SELECTED     = 0x040000,  /**< Текущий выбранный пункт или раздел*/
    LV_PART_ITEMS        = 0x050000,  /**< Для набора похожих элементов, например ячеек таблицы*/
    LV_PART_CURSOR       = 0x060000,  /**< Отмечает место, например курсор текстового поля или точку на графике*/

    LV_PART_CUSTOM_FIRST = 0x080000,  /**< Точка расширения для пользовательских виджетов*/

    LV_PART_ANY          = 0x0F0000,  /**< Специальное значение: все части*/
} lv_part_t;

typedef enum {
    LV_STYLE_STATE_CMP_SAME,           /**< Свойства стиля в двух состояниях одинаковы */
    LV_STYLE_STATE_CMP_DIFF_REDRAW,    /**< Разницу можно показать обычной перерисовкой */
    LV_STYLE_STATE_CMP_DIFF_DRAW_PAD,  /**< Разницу можно показать обычной перерисовкой */
    LV_STYLE_STATE_CMP_DIFF_LAYOUT,    /**< Разницу можно показать обычной перерисовкой */
} lv_style_state_cmp_t;

/**
 * Комбинация `lv_part_t` и `lv_state_t`. Примеры результатов
 * - `0` : означает `LV_PART_MAIN | LV_STATE_DEFAULT`
 * - `LV_STATE_PRSSED`
 * - `LV_PART_KNOB`
 * - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
typedef uint32_t lv_style_selector_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Добавьте стиль к объекту.
 * @param obj       указатель на объект
 * @param style     указатель на стиль, который нужно добавить
 * @param selector  объединенные побитовым ИЛИ значение деталей и состояние, к которому следует добавить стиль
 *
 * Примеры:
 * @code
 * lv_obj_add_style (кнопка, & style_btn, 0); //Стиль кнопки по умолчанию
 *
 * lv_obj_add_style (кнопка, &btn_red,LV_STATE_PRESSED); //Перезаписываем только некоторые цвета на красное лицо
 * @endcode
 */
void lv_obj_add_style(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector);

/**
 * Заменяет стиль объекта, сохраняя порядок стека стилей (локальные стили и переходы игнорируются).
 * @param obj           указатель на объект
 * @param old_style     указатель на стиль, который необходимо заменить.
 * @param new_style     указатель на стиль, которым нужно заменить старый стиль.
 * @param selector      объединенные побитовым ИЛИ значения состояний и часть для замены только стилей соответствующими селекторами.  Можно использовать `LV_STATE_ANY` и LV_PART_ANY.
 *
 * Примеры:
 * @code
 * lv_obj_replace_style (obj, & yellow_style, & blue_style,LV_PART_ANY| LV_STATE_ANY); //Заменяем текущий стиль
 *
 * lv_obj_replace_style (obj, & yellow_style, & blue_style,LV_PART_MAIN| LV_STATE_PRESSED); //Заменить текущий стиль, назначенный основной деталью, при ее появлении
 * @endcode
 */
bool lv_obj_replace_style(lv_obj_t * obj, const lv_style_t * old_style, const lv_style_t * new_style,
                          lv_style_selector_t selector);

/**
 * Удаление стиля из объекта.
 * @param obj       указатель на объект
 * @param style     указатель на стиль, который нужно удалить. Может быть NULL, чтобы проверить только селектор
 * @param selector  объединенные побитовым ИЛИ значения состояний и часть для удаления только стилей с соответствующими селекторами.  Можно использовать `LV_STATE_ANY` и LV_PART_ANY.
 *
 * Примеры:
 * @code
 * lv_obj_remove_style (obj, &style,LV_PART_ANY|LV_STATE_ANY); //Удалить современный стиль
 *
 * lv_obj_remove_style (obj,NULL,LV_PART_MAIN|LV_STATE_ANY); //Удалим все стили из основной части
 *
 * lv_obj_remove_style (obj,NULL,LV_PART_ANY|LV_STATE_ANY); //Удаляем все стили
 * @endcode
 */
void lv_obj_remove_style(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector);


/**
 * Удалить все стили, добавленные темой, из виджета.
 * @param selector  объединенные побитовым ИЛИ значения состояний и часть для удаления только стилей с соответствующими селекторами.
 *                  Можно использовать LV_STATE_ANY и LV_PART_ANY.
 * @param obj   указатель на виджет
 */
void lv_obj_remove_theme(lv_obj_t * obj, lv_style_selector_t selector);

/**
 * Удалить все стили из объекта
 * @param obj       указатель на объект
 */
void lv_obj_remove_style_all(lv_obj_t * obj);

/**
 * Уведомить весь объект, если стиль изменен
 * @param style     указатель на стиль. Уведомляться будут только объекты с этим стилем.
 *                  ( NULL для уведомления всех объектов)
 */
void lv_obj_report_style_change(lv_style_t * style);

/**
 * Уведомить объект и его дочерние элементы об изменении его стиля.
 * @param obj       указатель на объект
 * @param part      часть, стиль которой был изменен. Например. `LV_PART_ANY`, `LV_PART_MAIN`
 * @param prop      `LV_STYLE_PROP_ANY` или свойство `LV_STYLE_...`.
 *                  Он используется для оптимизации того, что необходимо обновить.
 *                  `LV_STYLE_PROP_INV` для выполнения только обновления кэша стилей
 */
void lv_obj_refresh_style(lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop);

/**
 * Временно отключите стиль для селектора. Это будет выглядеть так, будто стиль не был добавлен.
 * @param obj       указатель на объект
 * @param style     указатель на стиль
 * @param selector  селектор стиля (например,LV_STATE_PRESSED|LV_PART_KNOB)
 * @param dis       true: отключить стиль, false: включить стиль
 */
void lv_obj_style_set_disabled(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector, bool dis);

/**
 * Получите, отключен ли данный стиль для объекта.
 * @param obj       указатель на объект
 * @param style     указатель на стиль
 * @param selector  селектор стиля (например,LV_STATE_PRESSED|LV_PART_KNOB)
 * @return          true: отключить стиль, false: включить стиль
 */
bool lv_obj_style_get_disabled(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector);

/**
 * Включить или отключить автоматическое обновление стиля при добавлении/удалении нового стиля в/из объекта.
 * или произойдет любое другое изменение стиля.
 * @param en        true: включить обновление; false: отключить обновление
 */
void lv_obj_enable_style_refresh(bool en);

/**
 * Получите значение свойства стиля. Будет учитываться текущее состояние объекта.
 * Унаследованные свойства будут унаследованы.
 * Если свойство не установлено, будет возвращено значение по умолчанию.
 * @param obj       указатель на объект
 * @param part      часть, из которой должно быть получено свойство
 * @param prop      свойство, которую нужно получить
 * @return          стоимость имущества.
 *                  Следует читать поля`lv_style_value_t`в соответствии с типом свойств.
 */
lv_style_value_t lv_obj_get_style_prop(const lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop);

/**
 * Проверьте, имеет ли объект указанное свойство стиля для данного селектора стиля.
 * @param obj       указатель на объект
 * @param selector  селектор стиля, который необходимо проверить, определяющий область действия проверяемого стиля.
 * @param prop      свойство, подлежащее проверке.
 * @return          true, если объект имеет указанный селектор и свойство, в противном случае — false.
 */
bool lv_obj_has_style_prop(const lv_obj_t * obj, lv_style_selector_t selector, lv_style_prop_t prop);

/**
 * Установите локальное свойство стиля для части и состояния объекта.
 * @param obj       указатель на объект
 * @param prop      свойство
 * @param value     стоимость имущества. Правильный элемент должен быть установлен в соответствии с типом свойства.
 * @param selector  объединенные побитовым ИЛИ значение деталей и состояние, для которого должен быть установлен стиль
 */
void lv_obj_set_local_style_prop(lv_obj_t * obj, lv_style_prop_t prop, lv_style_value_t value,
                                 lv_style_selector_t selector);

lv_style_res_t lv_obj_get_local_style_prop(lv_obj_t * obj, lv_style_prop_t prop, lv_style_value_t * value,
                                           lv_style_selector_t selector);

/**
 * Удалите свойство локального стиля из части объекта с заданным состоянием.
 * @param obj       указатель на объект
 * @param prop      свойство стиля, которое нужно удалить.
 * @param selector  объединенные побитовым ИЛИ значение деталей и состояние, для которого стиль следует удалить
 * @return true, свойство было найдено и удалено; false: свойство не найдено
 */
bool lv_obj_remove_local_style_prop(lv_obj_t * obj, lv_style_prop_t prop, lv_style_selector_t selector);

/**
 * Используется внутри для цветовой фильтрации.
 */
lv_style_value_t lv_obj_style_apply_color_filter(const lv_obj_t * obj, lv_part_t part, lv_style_value_t v);

/**
 * Затухание объекта и всех его дочерних элементов.
 * @param obj       объект, который будет исчезать
 * @param time      время затухания
 * @param delay     задержка начала анимации
 */
void lv_obj_fade_in(lv_obj_t * obj, uint32_t time, uint32_t delay);

/**
 * Затемнение объекта и всех его дочерних элементов.
 * @param obj       объект, который исчезнет
 * @param time      время затухания
 * @param delay     задержка начала анимации
 */
void lv_obj_fade_out(lv_obj_t * obj, uint32_t time, uint32_t delay);

static inline lv_state_t lv_obj_style_get_selector_state(lv_style_selector_t selector)
{
    return (lv_state_t)(selector & 0xFFFF);
}

static inline lv_part_t lv_obj_style_get_selector_part(lv_style_selector_t selector)
{
    return (lv_part_t)(selector & 0xFF0000);
}

#include "lv_obj_style_gen.h"

static inline void lv_obj_set_style_pad_all(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    lv_obj_set_style_pad_left(obj, value, selector);
    lv_obj_set_style_pad_right(obj, value, selector);
    lv_obj_set_style_pad_top(obj, value, selector);
    lv_obj_set_style_pad_bottom(obj, value, selector);
}

static inline void lv_obj_set_style_pad_hor(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    lv_obj_set_style_pad_left(obj, value, selector);
    lv_obj_set_style_pad_right(obj, value, selector);
}

static inline void lv_obj_set_style_pad_ver(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    lv_obj_set_style_pad_top(obj, value, selector);
    lv_obj_set_style_pad_bottom(obj, value, selector);
}

static inline void lv_obj_set_style_margin_all(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    lv_obj_set_style_margin_left(obj, value, selector);
    lv_obj_set_style_margin_right(obj, value, selector);
    lv_obj_set_style_margin_top(obj, value, selector);
    lv_obj_set_style_margin_bottom(obj, value, selector);
}

static inline void lv_obj_set_style_margin_hor(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    lv_obj_set_style_margin_left(obj, value, selector);
    lv_obj_set_style_margin_right(obj, value, selector);
}

static inline void lv_obj_set_style_margin_ver(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    lv_obj_set_style_margin_top(obj, value, selector);
    lv_obj_set_style_margin_bottom(obj, value, selector);
}

static inline void lv_obj_set_style_pad_gap(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    lv_obj_set_style_pad_row(obj, value, selector);
    lv_obj_set_style_pad_column(obj, value, selector);
}

static inline void lv_obj_set_style_size(lv_obj_t * obj, int32_t width, int32_t height,
                                         lv_style_selector_t selector)
{
    lv_obj_set_style_width(obj, width, selector);
    lv_obj_set_style_height(obj, height, selector);
}

static inline void lv_obj_set_style_transform_scale(lv_obj_t * obj, int32_t value,
                                                    lv_style_selector_t selector)
{
    lv_obj_set_style_transform_scale_x(obj, value, selector);
    lv_obj_set_style_transform_scale_y(obj, value, selector);
}

static inline int32_t lv_obj_get_style_space_left(const lv_obj_t * obj, lv_part_t part)
{
    int32_t padding = lv_obj_get_style_pad_left(obj, part);
    int32_t border_width = lv_obj_get_style_border_width(obj, part);
    lv_border_side_t border_side = lv_obj_get_style_border_side(obj, part);
    return (border_side & LV_BORDER_SIDE_LEFT) ? padding + border_width : padding;
}

static inline int32_t lv_obj_get_style_space_right(const lv_obj_t * obj, lv_part_t part)
{
    int32_t padding = lv_obj_get_style_pad_right(obj, part);
    int32_t border_width = lv_obj_get_style_border_width(obj, part);
    lv_border_side_t border_side = lv_obj_get_style_border_side(obj, part);
    return (border_side & LV_BORDER_SIDE_RIGHT) ? padding + border_width : padding;
}

static inline int32_t lv_obj_get_style_space_top(const lv_obj_t * obj, lv_part_t part)
{
    int32_t padding = lv_obj_get_style_pad_top(obj, part);
    int32_t border_width = lv_obj_get_style_border_width(obj, part);
    lv_border_side_t border_side = lv_obj_get_style_border_side(obj, part);
    return (border_side & LV_BORDER_SIDE_TOP) ? padding + border_width : padding;
}

static inline int32_t lv_obj_get_style_space_bottom(const lv_obj_t * obj, lv_part_t part)
{
    int32_t padding = lv_obj_get_style_pad_bottom(obj, part);
    int32_t border_width = lv_obj_get_style_border_width(obj, part);
    lv_border_side_t border_side = lv_obj_get_style_border_side(obj, part);
    return (border_side & LV_BORDER_SIDE_BOTTOM) ? padding + border_width : padding;
}

lv_text_align_t lv_obj_calculate_style_text_align(const lv_obj_t * obj, lv_part_t part, const char * txt);

static inline int32_t lv_obj_get_style_transform_scale_x_safe(const lv_obj_t * obj, lv_part_t part)
{
    int32_t scale = lv_obj_get_style_transform_scale_x(obj, part);
    return scale > 0 ? scale : 1;
}

static inline int32_t lv_obj_get_style_transform_scale_y_safe(const lv_obj_t * obj, lv_part_t part)
{
    int32_t scale = lv_obj_get_style_transform_scale_y(obj, part);
    return scale > 0 ? scale : 1;
}

/**
 * Получите свойство стиля`opa`от всех родительских элементов и умножьте их на `>> 8`.
 * @param obj       объект, непрозрачность которого должна быть получена
 * @param part      часть, непрозрачность которой должна быть получена. Детали, отличные от MAIN, также будут учитывать`opa`детали MAIN.
 * @return          окончательная непрозрачность, учитывая также непрозрачность родителей
 */
lv_opa_t lv_obj_get_style_opa_recursive(const lv_obj_t * obj, lv_part_t part);


/**
 * Примените эффект перекрашивания к входному цвету на основе свойств стиля объекта.
 * @param obj       целевой объект, содержащий свойства стиля перекрашивания
 * @param part      часть для получения стилей перекрашивания.
 * @param color     исходный цвет, который нужно изменить
 * @return          смешанный цвет после применения перекраски и непрозрачности
 */
lv_color32_t lv_obj_style_apply_recolor(const lv_obj_t * obj, lv_part_t part, lv_color32_t color);

/**
 * Получить свойство стиля `recolor` у всех родительских элементов и рекурсивно объединить их.
 * @param obj       объект, значение перекрашивания которого должно быть получено
 * @param part      целевую часть для проверки. Детали, отличные от MAIN, также будут рассмотрены.
 *                  значение `recolor` из части `MAIN` во время расчета
 * @return          окончательное смешанное значение перекрашивания, объединяющее значения перекрашивания всех родительских элементов
 */
lv_color32_t lv_obj_get_style_recolor_recursive(const lv_obj_t * obj, lv_part_t part);

#if LV_USE_OBSERVER
/**
 * Отключите стиль, если значение субъекта не равно эталонному значению.
 * @param obj           указатель на виджет
 * @param style         указатель на стиль
 * @param selector      указатель на селектор
 * @param subject       указатель на тему
 * @param ref_value     эталонное значение для сравнения значения субъекта с
 * @return              указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_obj_bind_style(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector,
                                  lv_subject_t * subject, int32_t ref_value);

/**
 * Подключите значение темы к свойству стиля виджета.
 * @param obj       указатель на виджет
 * @param prop      свойство стиля
 * @param selector  селектор, для которого следует добавить свойство, например.  `LV_PART_KNOB | LV_STATE_PRESSED`
 * @param subject   указатель на субъект, к которому должно быть привязано свойство
 * @return              указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_obj_bind_style_prop(lv_obj_t * obj, lv_style_prop_t prop, lv_style_selector_t selector,
                                       lv_subject_t * subject);

#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_STYLE_H*/
