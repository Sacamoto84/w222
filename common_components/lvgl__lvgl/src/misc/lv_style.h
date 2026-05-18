/**
 * @file lv_style.h
 *
 */

#ifndef LV_STYLE_H
#define LV_STYLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../font/lv_font.h"
#include "lv_color.h"
#include "lv_area.h"
#include "lv_anim.h"
#include "lv_text.h"
#include "lv_types.h"
#include "lv_assert.h"
#include "lv_bidi.h"
#include "lv_grad.h"
#include "../layouts/lv_layout.h"

/*********************
 *      DEFINES
 *********************/

#define LV_STYLE_SENTINEL_VALUE     0xAABBCCDD

/*
 * Флаги поведения стиля
 */
#define LV_STYLE_PROP_FLAG_NONE                     (0)       /**< No special behavior */
#define LV_STYLE_PROP_FLAG_INHERITABLE              (1 << 0)  /**< Inherited */
#define LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE          (1 << 1)  /**< Requires ext. draw size update when changed */
#define LV_STYLE_PROP_FLAG_LAYOUT_UPDATE            (1 << 2)  /**< Requires layout update when changed */
#define LV_STYLE_PROP_FLAG_PARENT_LAYOUT_UPDATE     (1 << 3)  /**< Requires layout update on parent when changed */
#define LV_STYLE_PROP_FLAG_LAYER_UPDATE             (1 << 4)  /**< Affects layer handling */
#define LV_STYLE_PROP_FLAG_TRANSFORM                (1 << 5)  /**< Affects the object's transformation */
#define LV_STYLE_PROP_FLAG_ALL                      (0x3F)    /**< Indicating all flags */

/*
 * Другие константы
 */
#define LV_SCALE_NONE            256        /**< Value for not zooming the image */
LV_EXPORT_CONST_INT(LV_SCALE_NONE);

// *INDENT-OFF*
#if LV_USE_ASSERT_STYLE
#define LV_STYLE_CONST_INIT(var_name, prop_array)                       \
    const lv_style_t var_name = {                                       \
        .sentinel = LV_STYLE_SENTINEL_VALUE,                            \
        .values_and_props = (void*)prop_array,                          \
        .has_group = 0xFFFFFFFF,                                        \
        .prop_cnt = 255                                                 \
    }
#else
#define LV_STYLE_CONST_INIT(var_name, prop_array)                       \
    const lv_style_t var_name = {                                       \
        .values_and_props = (void*)prop_array,                          \
        .has_group = 0xFFFFFFFF,                                        \
        .prop_cnt = 255,                                                \
    }
#endif
// *INDENT-ON*

#define LV_STYLE_CONST_PROPS_END { .prop = LV_STYLE_PROP_INV, .value = { .num = 0 } }

#if LV_GRADIENT_MAX_STOPS < 2
#error LVGL needs at least 2 stops for gradients. Please increase the LV_GRADIENT_MAX_STOPS
#endif

#define LV_GRAD_LEFT    LV_PCT(0)
#define LV_GRAD_RIGHT   LV_PCT(100)
#define LV_GRAD_TOP     LV_PCT(0)
#define LV_GRAD_BOTTOM  LV_PCT(100)
#define LV_GRAD_CENTER  LV_PCT(50)

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Возможные варианты растушевки непрозрачных рисунков
 */
typedef enum {
    LV_BLEND_MODE_NORMAL,     /**< Simply mix according to the opacity value*/
    LV_BLEND_MODE_ADDITIVE,   /**< Add the respective color channels*/
    LV_BLEND_MODE_SUBTRACTIVE,/**< Subtract the foreground from the background*/
    LV_BLEND_MODE_MULTIPLY,   /**< Multiply the foreground and background*/
    LV_BLEND_MODE_DIFFERENCE, /**< Absolute difference between foreground and background*/
} lv_blend_mode_t;

/**
 * Некоторые варианты нанесения украшений на тексты.
 * Можно использовать значения 'OR'.
 */
typedef enum {
    LV_TEXT_DECOR_NONE          = 0x00,
    LV_TEXT_DECOR_UNDERLINE     = 0x01,
    LV_TEXT_DECOR_STRIKETHROUGH = 0x02,
} lv_text_decor_t;

/**
 * Выбирает, по каким сторонам должна быть нарисована граница
 * Можно использовать значения 'OR'.
 */
typedef enum {
    LV_BORDER_SIDE_NONE     = 0x00,
    LV_BORDER_SIDE_BOTTOM   = 0x01,
    LV_BORDER_SIDE_TOP      = 0x02,
    LV_BORDER_SIDE_LEFT     = 0x04,
    LV_BORDER_SIDE_RIGHT    = 0x08,
    LV_BORDER_SIDE_FULL     = 0x0F,
    LV_BORDER_SIDE_INTERNAL = 0x10, /**< FOR matrix-like objects (e.g. Button matrix)*/
} lv_border_side_t;

typedef enum {
    LV_BLUR_QUALITY_AUTO = 0,   /**< Set the quality automatically */
    LV_BLUR_QUALITY_SPEED,      /**< Prefer speed over precision */
    LV_BLUR_QUALITY_PRECISION,  /**< Prefer precision over speed*/
} lv_blur_quality_t;

/** Определение цветовой клавиши изображения.
 *  Прозрачность цветов в составе [низкий, высокий] будет установлена ​​наLV_OPA_TRANSP, если флаг «включить установлен» в значении true.
 */
typedef struct {
    lv_color_t low;
    lv_color_t high;
} lv_image_colorkey_t;

/**
 * Общий тип для одинаковой обработки всех типов свойств.
 */
typedef union {
    int32_t num;         /**< Number integer number (opacity, enums, booleans or "normal" numbers)*/
    const void * ptr;    /**< Constant pointers  (font, cone text, etc)*/
    lv_color_t color;    /**< Colors*/
} lv_style_value_t;

/**
 * Перечисление всех встроенных свойств стиля.
 *
 * Реквизиты разделены на группы по 16 штук. Добавляя новый реквизит в группу, убедитесь, что он не переливается в следующий.
 */
enum _lv_style_id_t {
    LV_STYLE_PROP_INV               = 0,

    /*Свойства перечислены в специальном порядке, чтобы сделать кэширование более эффективным.
     *Группы используются, когда включен LV_OBJ_STYLE_CACHE.
     *Если установлено свойство из группы, бит будет установлен в
     *obj->style_main_prop_is_setи style_other_prop_is_set
     *чтобы указать, что свойство установлено.
     *
     *Стратегия 1: группировать связанные свойства. Например. если не установлены свойства «границы»
     *            они будут быстро пропущены.
     *
     *Стратегия 2: сгруппируйте общее имущество с редко используемыми свойствами. Таким образом
     *            общее свойство кэшируется правильно и редко затрагивается
     *            другим реквизитом. Остальные реквизиты кэшируются неоптимальным образом.
     *            но они используются редко.
     *
     *Стратегия 3: сгруппируйте свойства, которые редко используются вместе, чтобы не было кэширования.
     *            вмешиваться
     *
     *Каждая группа может иметь 8 свойств. (см. STYLE_PROP_SHIFTED )*/

    /* Свойства, связанные с размером*/
    LV_STYLE_WIDTH = 1,
    LV_STYLE_HEIGHT,
    LV_STYLE_LENGTH,
    LV_STYLE_TRANSFORM_WIDTH,
    LV_STYLE_TRANSFORM_HEIGHT,

    LV_STYLE_MIN_WIDTH = 8,
    LV_STYLE_MAX_WIDTH,
    LV_STYLE_MIN_HEIGHT,
    LV_STYLE_MAX_HEIGHT,
    LV_STYLE_TRANSLATE_X,
    LV_STYLE_TRANSLATE_Y,
    LV_STYLE_RADIAL_OFFSET,

    /*Свойства, связанные с позицией */
    LV_STYLE_X = 16,
    LV_STYLE_Y,
    LV_STYLE_ALIGN,

    /*Заполнение связанных свойств */
    LV_STYLE_PAD_TOP = 24,
    LV_STYLE_PAD_BOTTOM,
    LV_STYLE_PAD_LEFT,
    LV_STYLE_PAD_RIGHT,
    LV_STYLE_PAD_RADIAL,
    LV_STYLE_PAD_ROW,
    LV_STYLE_PAD_COLUMN,

    /*Свойства, связанные с маржей*/
    LV_STYLE_MARGIN_TOP = 32,
    LV_STYLE_MARGIN_BOTTOM,
    LV_STYLE_MARGIN_LEFT,
    LV_STYLE_MARGIN_RIGHT,

    /*Бг. Градиент*/
    LV_STYLE_BG_GRAD = 40,
    LV_STYLE_BG_GRAD_DIR,
    LV_STYLE_BG_MAIN_OPA,
    LV_STYLE_BG_GRAD_OPA,
    LV_STYLE_BG_GRAD_COLOR,
    LV_STYLE_BG_MAIN_STOP,
    LV_STYLE_BG_GRAD_STOP,

    /*Фоновое изображение*/
    LV_STYLE_BG_IMAGE_SRC = 48,
    LV_STYLE_BG_IMAGE_OPA,
    LV_STYLE_BG_IMAGE_RECOLOR_OPA,
    LV_STYLE_BG_IMAGE_TILED,
    LV_STYLE_BG_IMAGE_RECOLOR,

    /*Группа 3*/
    LV_STYLE_BORDER_WIDTH = 56,
    LV_STYLE_BORDER_COLOR,
    LV_STYLE_BORDER_OPA,
    LV_STYLE_BORDER_POST,
    LV_STYLE_BORDER_SIDE,

    /*Схема */
    LV_STYLE_OUTLINE_WIDTH = 64,
    LV_STYLE_OUTLINE_COLOR,
    LV_STYLE_OUTLINE_OPA,
    LV_STYLE_OUTLINE_PAD,

    /*Изображение, Тень, Линия, Дуга и Текст редко используются вместе.*/
    LV_STYLE_BG_OPA = 72,
    LV_STYLE_BG_COLOR,
    LV_STYLE_SHADOW_WIDTH,
    LV_STYLE_LINE_WIDTH,
    LV_STYLE_ARC_WIDTH,
    LV_STYLE_TEXT_FONT,
    LV_STYLE_IMAGE_RECOLOR_OPA,

    LV_STYLE_IMAGE_OPA = 80,
    LV_STYLE_SHADOW_OPA,
    LV_STYLE_LINE_OPA,
    LV_STYLE_ARC_OPA,
    LV_STYLE_TEXT_OPA,

    LV_STYLE_SHADOW_COLOR = 88,
    LV_STYLE_IMAGE_RECOLOR,
    LV_STYLE_LINE_COLOR,
    LV_STYLE_ARC_COLOR,
    LV_STYLE_TEXT_COLOR,

    LV_STYLE_ARC_IMAGE_SRC = 96,
    LV_STYLE_SHADOW_OFFSET_X,
    LV_STYLE_SHADOW_OFFSET_Y,
    LV_STYLE_SHADOW_SPREAD,
    LV_STYLE_LINE_DASH_WIDTH,
    LV_STYLE_TEXT_ALIGN,
    LV_STYLE_TEXT_LETTER_SPACE,
    LV_STYLE_TEXT_LINE_SPACE,

    LV_STYLE_LINE_DASH_GAP = 104,
    LV_STYLE_LINE_ROUNDED,
    LV_STYLE_IMAGE_COLORKEY,
    LV_STYLE_TEXT_OUTLINE_STROKE_WIDTH,
    LV_STYLE_TEXT_OUTLINE_STROKE_OPA,
    LV_STYLE_TEXT_OUTLINE_STROKE_COLOR,
    LV_STYLE_TEXT_DECOR,
    LV_STYLE_ARC_ROUNDED,

    /*Группируйте несвязанный реквизит*/
    LV_STYLE_OPA = 112,
    LV_STYLE_OPA_LAYERED,
    LV_STYLE_COLOR_FILTER_DSC,
    LV_STYLE_COLOR_FILTER_OPA,
    LV_STYLE_ANIM,
    LV_STYLE_ANIM_DURATION,
    LV_STYLE_TRANSITION,

    /*Радиус запрашивается часто, группируйте его с редко запрашиваемыми*/
    LV_STYLE_RADIUS = 120,
    LV_STYLE_BITMAP_MASK_SRC,
    LV_STYLE_BLEND_MODE,
    LV_STYLE_ROTARY_SENSITIVITY,
    LV_STYLE_TRANSLATE_RADIAL,

    /*Много просили, но редко использовали*/
    LV_STYLE_CLIP_CORNER = 128,
    LV_STYLE_BASE_DIR,
    LV_STYLE_RECOLOR,
    LV_STYLE_RECOLOR_OPA,
    LV_STYLE_LAYOUT,

    /*Размытие*/
    LV_STYLE_BLUR_RADIUS = 136,
    LV_STYLE_BLUR_BACKDROP,
    LV_STYLE_BLUR_QUALITY,

    /*Тень*/
    LV_STYLE_DROP_SHADOW_RADIUS = 144,
    LV_STYLE_DROP_SHADOW_OFFSET_X,
    LV_STYLE_DROP_SHADOW_OFFSET_Y,
    LV_STYLE_DROP_SHADOW_COLOR,
    LV_STYLE_DROP_SHADOW_OPA,
    LV_STYLE_DROP_SHADOW_QUALITY,

    /*Масштабируйте и трансформируйте*/
    LV_STYLE_TRANSFORM_SCALE_X = 152,
    LV_STYLE_TRANSFORM_SCALE_Y,
    LV_STYLE_TRANSFORM_PIVOT_X,
    LV_STYLE_TRANSFORM_PIVOT_Y,
    LV_STYLE_TRANSFORM_ROTATION,
    LV_STYLE_TRANSFORM_SKEW_X,
    LV_STYLE_TRANSFORM_SKEW_Y,

    /*Гибкая и базовая сетка (редко используются вместе)*/
    LV_STYLE_FLEX_FLOW = 160,
    LV_STYLE_FLEX_MAIN_PLACE,
    LV_STYLE_FLEX_CROSS_PLACE,
    LV_STYLE_FLEX_TRACK_PLACE,
    LV_STYLE_FLEX_GROW,
    LV_STYLE_GRID_COLUMN_DSC_ARRAY,
    LV_STYLE_GRID_ROW_DSC_ARRAY,

    LV_STYLE_GRID_COLUMN_ALIGN = 168,
    LV_STYLE_GRID_ROW_ALIGN,
    LV_STYLE_GRID_CELL_COLUMN_POS,
    LV_STYLE_GRID_CELL_COLUMN_SPAN,
    LV_STYLE_GRID_CELL_X_ALIGN,
    LV_STYLE_GRID_CELL_ROW_POS,
    LV_STYLE_GRID_CELL_ROW_SPAN,
    LV_STYLE_GRID_CELL_Y_ALIGN,

    LV_STYLE_LAST_BUILT_IN_PROP,
    LV_STYLE_NUM_BUILT_IN_PROPS     = LV_STYLE_LAST_BUILT_IN_PROP + 1,

    LV_STYLE_PROP_ANY                = 0xFF,
    LV_STYLE_PROP_CONST             = 0xFF /* магическое значение для константных стилей */
};

typedef enum {
    LV_STYLE_RES_NOT_FOUND,
    LV_STYLE_RES_FOUND,
} lv_style_res_t;

/**
 * Дескриптор для переходов стилей
 */
typedef struct {
    const lv_style_prop_t * props; /**< An array with the properties to animate.*/
    void * user_data;              /**< A custom user data that will be passed to the animation's user_data */
    lv_anim_path_cb_t path_xcb;    /**< A path for the animation.*/
    uint32_t time;                 /**< Duration of the transition in [ms]*/
    uint32_t delay;                /**< Delay before the transition in [ms]*/
} lv_style_transition_dsc_t;

/**
 * Дескриптор свойства постоянного стиля.
 */
typedef struct {
    lv_style_prop_t prop;
    lv_style_value_t value;
} lv_style_const_prop_t;

/**
 * Дескриптор стиля (набор свойств и значений).
 */
typedef struct {

#if LV_USE_ASSERT_STYLE
    uint32_t sentinel;
#endif

    void * values_and_props;

    uint32_t has_group;
    uint8_t prop_cnt;   /**< 255 means it's a constant style*/
} lv_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализация стиля
 * @param style указатель на стиль для инициализации
 * @note Не вызывайте`lv_style_init`для стилей, у которых уже есть некоторые свойства.
 *       потому что эта функция не освобождает использованную память, а просто устанавливает для стиля состояние по умолчанию.
 *       Другими словами, обязательно инициализируйте стили только один раз!
 */
void lv_style_init(lv_style_t * style);

/**
 * Очистите все свойства стиля и освободите всю выделенную память.
 * @param style указатель на стиль
 */
void lv_style_reset(lv_style_t * style);

/**
 * Скопируйте все свойства одного стиля в другой.
 * Он имеет тот же эффект, называя тот же `lv_set_style_...`.
 * работает в обоих стилях.
 * Это означает, что для хранения свойств будет выделена новая память.
 * стиль назначения.
 * После копирования целевой стиль полностью независим от исходного.
 * и источник можно удалить, не затрагивая целевой стиль.
 * @param dst   место назначения для копирования (не может быть постоянным стилем)
 * @param src   исходный стиль для копирования.
 */
void lv_style_copy(lv_style_t * dst, const lv_style_t * src);

/**
 * Скопируйте все свойства одного стиля в другой, не сбрасывая стиль dst.
 * Это имеет тот же эффект, что и вызов того же `lv_set_style_...`.
 * работает в обоих стилях.
 * Это означает, что для хранения свойств будет выделена новая память.
 * стиль назначения.
 * После копирования целевой стиль полностью независим от исходного.
 * и источник можно удалить, не затрагивая целевой стиль.
 * @param dst   место назначения для копирования (не может быть постоянным стилем)
 * @param src   исходный стиль для копирования.
 */
void lv_style_merge(lv_style_t * dst, const lv_style_t * src);


/**
 * Проверьте, является ли стиль постоянным
 * @param style     указатель на стиль
 * @return          правда: стиль постоянный
 */
static inline bool lv_style_is_const(const lv_style_t * style)
{
    if(style->prop_cnt == 255) return true;
    return false;
}


/**
 * Зарегистрируйте новое свойство стиля для индивидуального использования.
 * @return новое свойствоIDили LV_STYLE_PROP_INV, если доступных больше нет.
 *
 * Пример:
 * @code
 * lv_style_prop_tMY_PROP;
 * static inline voidlv_style_set_my_prop( стильlv_style_t*, значениеlv_color_t) {
 * lv_style_value_t v = {.color = значение};  lv_style_set_prop(стиль,MY_PROP, v); }
 *
 * ...
 * MY_PROP = lv_style_register_prop ();
 * ...
 * lv_style_set_my_prop (&style1,lv_palette_main(LV_PALETTE_RED));
 * @endcode
 */
lv_style_prop_t lv_style_register_prop(uint8_t flag);

/**
 * Получите количество пользовательских свойств, зарегистрированных на данный момент.
 */
lv_style_prop_t lv_style_get_num_custom_props(void);

/**
 * Удаление свойства из стиля
 * @param style указатель на стиль
 * @param prop  свойство стиля, связанное ИЛИ с состоянием.
 * @return true: свойство найдено и удалено; ложь: свойство не найдено
 */
bool lv_style_remove_prop(lv_style_t * style, lv_style_prop_t prop);

/**
 * Установите значение свойства в стиле.
 * Эта функция не должна использоваться пользователем напрямую.
 * Вместо этого воспользуйтесь `lv_style_set_<prop_name>()`. Например.   `lv_style_set_bg_color()`
 * @param style указатель на стиль
 * @param prop ID свойства (например,`LV_STYLE_BG_COLOR`)
 * @param value `lv_style_value_t` переменная, в которой задано поле по типу `prop`
 */
void lv_style_set_prop(lv_style_t * style, lv_style_prop_t prop, lv_style_value_t value);

/**
 * Получить стоимость недвижимости
 * @param style указатель на стиль
 * @param prop  ID свойства
 * @param value указатель на переменную`lv_style_value_t`для хранения значения
 * @return LV_RESULT_INVALID: свойство не найдено в стиле (`value` не изменено)
 *         LV_RESULT_OK: свойство было привязано, и`value`установлен соответствующим образом.
 * @note Из соображений производительности на`style`нет проверки работоспособности.
 */
lv_style_res_t lv_style_get_prop(const lv_style_t * style, lv_style_prop_t prop, lv_style_value_t * value);

/**
 * Инициализируйте дескриптор перехода.
 * @param tr        указатель на дескриптор перехода для инициализации
 * @param props     массив со свойствами для перехода. Последний элемент должен быть нулевым.
 * @param path_cb   обратный вызов пути анимации (простота). Если будет использоваться путь лайнера `NULL`.
 * @param time      продолжительность перехода в [мс]
 * @param delay     задержка перед переходом в [мс]
 * @param user_data любые пользовательские данные, которые будут сохранены в анимации перехода и будут доступны при вызове `path_cb`
 *
 * Пример:
 * @code
 * const staticlv_style_prop_ttrans_props [] = { LV_STYLE_BG_OPA, LV_STYLE_BG_COLOR, 0 };
 * статическийlv_style_transition_dsc_tтранс1;
 * lv_style_transition_dsc_init (&trans1,trans_props,NULL, 300, 0,NULL);
 * @endcode
 */
void lv_style_transition_dsc_init(lv_style_transition_dsc_t * tr, const lv_style_prop_t props[],
                                  lv_anim_path_cb_t path_cb, uint32_t time, uint32_t delay, void * user_data);

/**
 * Получить значение свойства по умолчанию
 * @param prop ID свойства
 * @return значение по умолчанию
 */
lv_style_value_t lv_style_prop_get_default(lv_style_prop_t prop);

/**
 * Получить стоимость недвижимости
 * @param style указатель на стиль
 * @param prop  ID свойства
 * @param value указатель на переменную`lv_style_value_t`для хранения значения
 * @return LV_RESULT_INVALID: свойство не найдено в стиле (`value` не изменено)
 *         LV_RESULT_OK: свойство было привязано, и`value`установлен соответствующим образом.
 * @note Из соображений производительности на`style`нет проверки работоспособности.
 * @note Эта функция аналогична :: lv_style_get_prop, но встроена. Используйте его только в местах, критически важных для производительности.
 */
static inline lv_style_res_t lv_style_get_prop_inlined(const lv_style_t * style, lv_style_prop_t prop,
                                                       lv_style_value_t * value)
{
    if(lv_style_is_const(style)) {
        lv_style_const_prop_t * props = (lv_style_const_prop_t *)style->values_and_props;
        uint32_t i;
        for(i = 0; props[i].prop != LV_STYLE_PROP_INV; i++) {
            if(props[i].prop == prop) {
                *value = props[i].value;
                return LV_STYLE_RES_FOUND;
            }
        }
    }
    else {
        lv_style_prop_t * props = (lv_style_prop_t *)style->values_and_props + style->prop_cnt * sizeof(lv_style_value_t);
        uint32_t i;
        for(i = 0; i < style->prop_cnt; i++) {
            if(props[i] == prop) {
                lv_style_value_t * values = (lv_style_value_t *)style->values_and_props;
                *value = values[i];
                return LV_STYLE_RES_FOUND;
            }
        }
    }
    return LV_STYLE_RES_NOT_FOUND;
}

/**
 * Проверяет, является ли стиль пустым (не имеет свойств)
 * @param style указатель на стиль
 * @return true, если стиль пуст
 */
bool lv_style_is_empty(const lv_style_t * style);

/**
 * Укажите группу объектов. Если в стиле установлено свойство из группы, устанавливается бит (1 << группа) style->has_group.
 * Это позволяет досрочно пропустить стиль, если свойство вообще не существует в стиле.
 * @param prop свойство стиля
 * @return группа [0..30] 30 означает все пользовательские свойства с индексом > 120.
 */
static inline uint32_t lv_style_get_prop_group(lv_style_prop_t prop)
{
    uint32_t group = prop >> 2;
    if(group > 30) group = 31;    /*MSB отмечает все пользовательские свойства.*/
    return group;

}

/**
 * Получите флаги встроенного или настраиваемого свойства.
 *
 * @param prop свойство стиля
 * @return флаги объекта недвижимости
 */
uint8_t lv_style_prop_lookup_flags(lv_style_prop_t prop);

#include "lv_style_gen.h"

/**
 * Установите внешний вид и высоту `style`.
 * @param  style   указатель на стиль, который нужно изменить
 * @param  width   ширина в пикселях
 * @param  height  высота в пикселях
 */
static inline void lv_style_set_size(lv_style_t * style, int32_t width, int32_t height)
{
    lv_style_set_width(style, width);
    lv_style_set_height(style, height);
}

/**
 * Установите все 4 значения заполнения`style`.
 * @param  style   указатель на стиль, который нужно изменить
 * @param  value   размер заполнения в пикселях
 */
static inline void lv_style_set_pad_all(lv_style_t * style, int32_t value)
{
    lv_style_set_pad_left(style, value);
    lv_style_set_pad_right(style, value);
    lv_style_set_pad_top(style, value);
    lv_style_set_pad_bottom(style, value);
}

/**
 * Установите значения слева и справа для `style`.
 * @param  style   указатель на стиль, который нужно изменить
 * @param  value   размер заполнения в пикселях
 */
static inline void lv_style_set_pad_hor(lv_style_t * style, int32_t value)
{
    lv_style_set_pad_left(style, value);
    lv_style_set_pad_right(style, value);
}

/**
 * Установите значения верхние и нижние заполняющие `style`.
 * @param  style   указатель на стиль, который нужно изменить
 * @param  value   размер заполнения в пикселях
 */
static inline void lv_style_set_pad_ver(lv_style_t * style, int32_t value)
{
    lv_style_set_pad_top(style, value);
    lv_style_set_pad_bottom(style, value);
}

/**
 * Установите пробелы для заполнения строк и столбцов`style`(применяется только к макетам Grid и Flex).
 * @param  style   указатель на стиль, который нужно изменить
 * @param  value   размер зазора в пикселях
 */
static inline void lv_style_set_pad_gap(lv_style_t * style, int32_t value)
{
    lv_style_set_pad_row(style, value);
    lv_style_set_pad_column(style, value);
}

/**
 * Установите значения левого и правого полей`style`.
 * @param  style   указатель на стиль, который нужно изменить
 * @param  value   размер поля в пикселях
 */
static inline void lv_style_set_margin_hor(lv_style_t * style, int32_t value)
{
    lv_style_set_margin_left(style, value);
    lv_style_set_margin_right(style, value);
}

/**
 * Установите значения верхних и нижних полей `style`.
 * @param  style   указатель на стиль, который нужно изменить
 * @param  value   размер поля в пикселях
 */
static inline void lv_style_set_margin_ver(lv_style_t * style, int32_t value)
{
    lv_style_set_margin_top(style, value);
    lv_style_set_margin_bottom(style, value);
}

/**
 * Установите все 4 значения маржи `style`.
 * @param  style   указатель на стиль, который нужно изменить
 * @param  value   размер поля в пикселях
 */
static inline void lv_style_set_margin_all(lv_style_t * style, int32_t value)
{
    lv_style_set_margin_left(style, value);
    lv_style_set_margin_right(style, value);
    lv_style_set_margin_top(style, value);
    lv_style_set_margin_bottom(style, value);
}

/**
 * Установите`style`значения масштаба преобразования X и Y.
 * @param  style   указатель на стиль, который нужно изменить
 * @param  value   масштабный коэффициент.  Примеры значений:
 *                     - 256 или LV_SCALE_NONE: без масштабирования.
 *                     - <256: уменьшить масштаб
 *                     - >256: увеличить масштаб
 *                     - 128: половинный размер
 *                     - 512: двойной размер
 */
static inline void lv_style_set_transform_scale(lv_style_t * style, int32_t value)
{
    lv_style_set_transform_scale_x(style, value);
    lv_style_set_transform_scale_y(style, value);
}

/**
 * @brief Проверьте, имеет ли свойство стиля указанный поведенческий флаг.
 *
 * Не передавайте этой функции несколько флагов, поскольку обратная совместимость не гарантируется.
 * для этого.
 *
 * @param prop Свойство ID
 * @param flag Флаг
 * @return true, если для этого свойства установлен флаг
 */
static inline bool lv_style_prop_has_flag(lv_style_prop_t prop, uint8_t flag)
{
    return lv_style_prop_lookup_flags(prop) & flag;
}

/*************************
 *    GLOBAL VARIABLES
 *************************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_style_prop_t lv_style_const_prop_id_inv;

/**********************
 *      MACROS
 **********************/

#if LV_USE_ASSERT_STYLE
#  define LV_ASSERT_STYLE(style_p)                                                                            \
    do {                                                                                                      \
        LV_ASSERT_MSG(style_p != NULL, "The style is NULL");                                                  \
        LV_ASSERT_MSG(style_p->sentinel == LV_STYLE_SENTINEL_VALUE, "Style is not initialized or corrupted"); \
    } while(0)
#else
#  define LV_ASSERT_STYLE(p) do{}while(0)
#endif

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_STYLE_H*/
