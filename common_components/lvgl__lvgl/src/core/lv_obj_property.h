/**
 * @file lv_obj_property.h
 *
 */

#ifndef LV_OBJ_PROPERTY_H
#define LV_OBJ_PROPERTY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_types.h"
#include "../misc/lv_style.h"
#include "lv_obj_style.h"

#if LV_USE_OBJ_PROPERTY

/*********************
 *      DEFINES
 *********************/

/*Все возможные типы значений свойств*/
#define LV_PROPERTY_TYPE_INVALID        0   /*Используйте значение по умолчанию 0 как недействительное для обнаружения выбросов программы.*/
#define LV_PROPERTY_TYPE_INT            1   /*Тип int32_t*/
#define LV_PROPERTY_TYPE_PRECISE        2   /*lv_value_precise_t ,int32_tили плавающее число в зависимости от LV_USE_FLOAT*/
#define LV_PROPERTY_TYPE_COLOR          3   /*Тип ARGB8888*/
#define LV_PROPERTY_TYPE_POINT          4   /*lv_point_t */
#define LV_PROPERTY_TYPE_POINTER        5   /*пустота * указатель*/
#define LV_PROPERTY_TYPE_IMGSRC         6   /*Специальный указатель для изображения*/
#define LV_PROPERTY_TYPE_TEXT           7   /*Специальный указатель char* */
#define LV_PROPERTY_TYPE_OBJ            8   /*Специальный указательlv_obj_t* */
#define LV_PROPERTY_TYPE_DISPLAY        9   /*Специальный указательlv_display_t* */
#define LV_PROPERTY_TYPE_FONT           10  /*Специальный указательlv_font_t* */
#define LV_PROPERTY_TYPE_BOOL           11  /*Тип int32_t*/

#define LV_PROPERTY_TYPE_SHIFT          28
#define LV_PROPERTY_TYPE2_SHIFT         24

/* Пример:
 * LV_PROPERTY_ID(OBJ, FLAG_CLICKABLE, LV_PROPERTY_TYPE_INT, 1),
 * производит
 * LV_PROPERTY_OBJ_FLAG_CLICKABLE = (LV_PROPERTY_OBJ_START + (1)) | ((LV_PROPERTY_TYPE_INT) << LV_PROPERTY_TYPE_SHIFT)
 */
#define LV_PROPERTY_ID(clz, name, type, index)          LV_PROPERTY_## clz ##_##name = (LV_PROPERTY_## clz ##_START + ((int)index)) | ((type) << LV_PROPERTY_TYPE_SHIFT)

/* Пример:
 * LV_PROPERTY_ID2(SLIDER, VALUE, LV_PROPERTY_TYPE_INT, LV_PROPERTY_TYPE_BOOL, 0)
 * производит
 * LV_PROPERTY_SLIDER_VALUE = (LV_PROPERTY_SLIDER_START + (0)) | ((LV_PROPERTY_TYPE_INT) << LV_PROPERTY_TYPE_SHIFT) | ((LV_PROPERTY_TYPE_BOOL) << LV_PROPERTY_TYPE2_SHIFT)
 */
#define LV_PROPERTY_ID2(clz, name, type, type2, index)  LV_PROPERTY_ID(clz, name, type, index) | ((type2) << LV_PROPERTY_TYPE2_SHIFT)

#define LV_PROPERTY_ID_TYPE(id) ((id) >> LV_PROPERTY_TYPE_SHIFT)
#define LV_PROPERTY_ID_TYPE2(id) ((id) >> LV_PROPERTY_TYPE_SHIFT)
#define LV_PROPERTY_ID_INDEX(id) ((id) & 0xfffffff)

/*Получить свойства из массива lv_property_t*/
#define LV_OBJ_SET_PROPERTY_ARRAY(obj, array) lv_obj_set_properties(obj, array, LV_ARRAYLEN(array))

/* Помощник для реализации определения класса свойств и имен свойств. */
/* *INDENT-OFF* */
#if LV_USE_OBJ_PROPERTY_NAME
#define LV_PROPERTY_CLASS_FIELDS(widget, uppercase) \
    .prop_index_start = LV_PROPERTY_##uppercase##_START, \
    .prop_index_end = LV_PROPERTY_##uppercase##_END, \
    .properties = lv_##widget##_properties, \
    .properties_count = LV_ARRAYLEN(lv_##widget##_properties), \
    .property_names = lv_##widget##_property_names, \
    .names_count = LV_ARRAYLEN(lv_##widget##_property_names)
#else
#define LV_PROPERTY_CLASS_FIELDS(widget, uppercase) \
    .prop_index_start = LV_PROPERTY_##uppercase##_START, \
    .prop_index_end = LV_PROPERTY_##uppercase##_END, \
    .properties = lv_##widget##_properties, \
    .properties_count = LV_ARRAYLEN(lv_##widget##_properties)
#endif
/* *INDENT-ON* */


/**********************
 *      TYPEDEFS
 **********************/

/**
 * Группа предопределенных начальных значений виджета ID.
 */
enum _lv_prop_id_range_boundary_t {
    LV_PROPERTY_ID_INVALID      = 0,

    /*ID от0x01до0xffимеют стиль ID, от примеров lv_style_prop_t.*/
    LV_PROPERTY_STYLE_START     = 0x00,

    LV_PROPERTY_ID_START        = 0x0100, /*ID меньше, чем 0xff, — это стиль ID.*/
    /*Определите здесь свойство ID для каждого виджета. */
    LV_PROPERTY_OBJ_START       = 0x0100, /* lv_obj.c */
    LV_PROPERTY_IMAGE_START     = 0x0200, /* lv_image.c */
    LV_PROPERTY_LABEL_START     = 0x0300, /* lv_label.c */
    LV_PROPERTY_KEYBOARD_START  = 0x0400, /* lv_keyboard.c */
    LV_PROPERTY_TEXTAREA_START  = 0x0500, /* lv_textarea.c */
    LV_PROPERTY_ROLLER_START    = 0x0600, /* lv_roller.c */
    LV_PROPERTY_DROPDOWN_START  = 0x0700, /* lv_dropdown.c */
    LV_PROPERTY_SLIDER_START    = 0x0800, /* lv_slider.c */
    LV_PROPERTY_ANIMIMAGE_START = 0x0900, /* lv_animimage.c */
    LV_PROPERTY_ARC_START       = 0x0a00, /* lv_arc.c */
    LV_PROPERTY_BAR_START       = 0x0b00, /* lv_bar.c */
    LV_PROPERTY_SWITCH_START    = 0x0c00, /* lv_switch.c */
    LV_PROPERTY_CHECKBOX_START  = 0x0d00, /* lv_checkbox.c */
    LV_PROPERTY_LED_START       = 0x0e00, /* lv_led.c */
    LV_PROPERTY_LINE_START      = 0x0f00, /* lv_line.c */
    LV_PROPERTY_SCALE_START     = 0x1000, /* lv_scale.c */
    LV_PROPERTY_SPINBOX_START   = 0x1100, /* lv_spinbox.c */
    LV_PROPERTY_SPINNER_START   = 0x1200, /* lv_spinner.c */
    LV_PROPERTY_TABLE_START     = 0x1300, /* lv_table.c */
    LV_PROPERTY_TABVIEW_START   = 0x1400, /* lv_tabview.c */
    LV_PROPERTY_BUTTONMATRIX_START = 0x1500, /* lv_buttonmatrix.c */
    LV_PROPERTY_SPAN_START      = 0x1600, /* lv_span.c */
    LV_PROPERTY_MENU_START      = 0x1700, /* lv_menu.c */
    LV_PROPERTY_CHART_START     = 0x1800, /* lv_chart.c */

    /*Специальный ID, используйте его для расширения ID и убедитесь, что он уникален и определяет время компиляции.*/
    LV_PROPERTY_ID_BUILTIN_LAST = 0xffff, /*ID из0x10000~0xfffffffзарезервировано для пользователя.*/

    LV_PROPERTY_ID_ANY          = 0x7ffffffe, /*Специальный ID, интеллектуальный уровень для перехвата всех вызовов сеттера/геттера.*/
};

struct _lv_property_name_t {
    const char * name;
    lv_prop_id_t id;
};

typedef struct {
    lv_prop_id_t id;
    union {
        int32_t num;                /**< Signed integer number (enums or "normal" numbers)*/
        uint32_t num_u;             /**< Unsigned integer number (opacity, Booleans)  */
        bool enable;                /**< Booleans */
        const void * ptr;           /**< Constant pointers  (font, cone text, etc.) */
        lv_color_t color;           /**< Colors */
        lv_value_precise_t precise; /**< float or int for precise value */
        lv_point_t point;           /**< Point, contains two int32_t */

        struct {
            /**
             * Обратите внимание, что предусмотрено размещение члена структуры`style`на первом месте.
             * `style` использует ту же память, что и`num`,`ptr`,`color`.
             * Поэтому мы устанавливаем значение стиля напрямую, не используя `prop.style.num`.
             *
             * E.g.
             *
             * статическая константаlv_property_tobj_pos_x = {
             *      .id = LV_PROPERTY_STYLE_X,
             *      .номер = 123,
             *      .селектор = LV_STATE_PRESSED,
             * }
             *
             * вместо:
             * статическая константаlv_property_tobj_pos_x = {
             *      .id = LV_PROPERTY_STYLE_X,
             *      .style.num = 123, // обратите внимание на этот символ.
             *      .селектор = LV_STATE_PRESSED,
             * }
             */
            lv_style_value_t style; /**< Make sure it's the first element in struct. */
            uint32_t selector;      /**< Style selector, lv_part_t | lv_state_t */
        };

        /**
         * Для некоторых свойств, таких как диапазон ползунка, он содержит два простых (4-байтовых) значения.
         * поэтому мы можем использовать`arg1.num`и`arg2.num`для установки аргумента.
         */
        struct {
            union {
                int32_t num;
                uint32_t num_u;
                bool enable;
                const void * ptr;
                lv_color_t color;
                lv_value_precise_t precise;
            } arg1, arg2;
        };
    };
} lv_property_t;

typedef struct {
    lv_prop_id_t id;

    void * setter;      /**< Callback used to set property. */
    void * getter;      /**< Callback used to get property. */
} lv_property_ops_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите свойство виджета.
 * @param obj       указатель на виджет
 * @param value     значение свойства для установки
 * @return          вернуть LV_RESULT_OK, если вызов удался
 */
lv_result_t lv_obj_set_property(lv_obj_t * obj, const lv_property_t * value);

/**
 * Установите несколько свойств виджета. Помощник `LV_OBJ_SET_PROPERTY_ARRAY` можно использовать для массива констант.
 * @param obj       указатель на виджет
 * @param value     Массив значений свойств для установки
 * @param count     количество элементов массива
 * @return          вернуть LV_RESULT_OK, если вызов удался
 */
lv_result_t lv_obj_set_properties(lv_obj_t * obj, const lv_property_t * value, uint32_t count);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Считайте значение свойства из виджета.
 * Если id является свойством стиля, вычисляет стиль PART_MAIN.
 * @param obj       указатель на виджет
 * @param id        ID свойства для чтения
 * @return          вернуть прочитанное значение свойства. Возвращаемое свойствоIDустанавливается в `LV_PROPERTY_ID_INVALID`, если чтение не удалось.
 */
lv_property_t lv_obj_get_property(lv_obj_t * obj, lv_prop_id_t id);

/**
 * Чтение значения свойства стиля из виджета
 * @param obj       указатель на виджет
 * @param id        ID свойства стиля
 * @param part      часть, для которой должно быть вычислено свойство стиля
 * @return          вернуть прочитанное значение свойства. Возвращаемое свойствоIDустанавливается в `LV_PROPERTY_ID_INVALID`, если чтение не удалось.
 */
lv_property_t lv_obj_get_style_property(lv_obj_t * obj, lv_prop_id_t id, lv_part_t part);

/**
 * Получите свойство ID, рекурсивно выполнив поиск имени в иерархии классов виджета, и
 * если все еще не найдено, выполните поиск свойств стиля.
 * Требуется включить `LV_USE_OBJ_PROPERTY_NAME` .
 * @param obj       указатель на виджет, чей класс и иерархию базовых классов нужно искать.
 * @param name      имя свойства
 * @return          свойствоIDнайдено или `LV_PROPERTY_ID_INVALID`, если не найдено.
 */
lv_prop_id_t lv_obj_property_get_id(const lv_obj_t * obj, const char * name);

/**
 * Получите свойство ID, выполнив нерекурсивный поиск имени непосредственно в свойствах класса виджета.
 * Требуется включить `LV_USE_OBJ_PROPERTY_NAME`.
 * @param clz       указатель на класс виджета, имеющий указанное свойство.
 * @param name      имя свойства
 * @return          свойствоIDнайдено или `LV_PROPERTY_ID_INVALID`, если не найдено.
 */
lv_prop_id_t lv_obj_class_property_get_id(const lv_obj_class_t * clz, const char * name);

/**
 * Получите свойство стиля ID по имени. Требуется включить `LV_USE_OBJ_PROPERTY_NAME`.
 * @param name      имя свойства
 * @return          свойствоIDнайдено или `LV_PROPERTY_ID_INVALID`, если не найдено.
 */
lv_prop_id_t lv_style_property_get_id(const char * name);

/**********************
 *      MACROS
 **********************/

#include "../widgets/property/lv_obj_property_names.h"
#include "../widgets/property/lv_style_properties.h"

#else
#define LV_PROPERTY_CLASS_FIELDS(widget, uppercase)
#endif /*LV_USE_OBJ_PROPERTY*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_OBJ_PROPERTY_H*/
