/**
 * @file lv_arclabel.h
 *
 */

#ifndef LV_ARCLABEL_H
#define LV_ARCLABEL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_ARCLABEL != 0

/*********************
 *      DEFINES
 *********************/

#define LV_ARCLABEL_DOT_NUM 3
#if LV_WIDGETS_HAS_DEFAULT_VALUE
#define LV_ARCLABEL_DEFAULT_TEXT "Arced Text"
#else
#define LV_ARCLABEL_DEFAULT_TEXT ""
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_ARCLABEL_DIR_CLOCKWISE = 0,
    LV_ARCLABEL_DIR_COUNTER_CLOCKWISE = 1
} lv_arclabel_dir_t;

typedef enum {
    LV_ARCLABEL_TEXT_ALIGN_DEFAULT = 0,
    LV_ARCLABEL_TEXT_ALIGN_LEADING = 1,
    LV_ARCLABEL_TEXT_ALIGN_CENTER = 2,
    LV_ARCLABEL_TEXT_ALIGN_TRAILING = 3,
} lv_arclabel_text_align_t;

typedef enum {
    LV_ARCLABEL_OVERFLOW_VISIBLE,  /**< Show full text, may overflow object area */
    LV_ARCLABEL_OVERFLOW_ELLIPSIS, /**< Show ellipsis (...) when text overflows */
    LV_ARCLABEL_OVERFLOW_CLIP      /**< Clip text at arc boundary */
} lv_arclabel_overflow_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_arclabel_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте объект метки дуги
 * @param parent    указатель на объект, он будет родительским элементом новой метки дуги
 * @return          указатель на созданную метку дуги
 */
lv_obj_t * lv_arclabel_create(lv_obj_t * parent);

/*======================
 * Добавить/удалить функции
 *=====================*/

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите текст метки дуги.
 *
 * Эта функция устанавливает текст, отображаемый объектом метки дуги.
 *
 * @param obj       Указатель на объект метки дуги.
 * @param text      Указатель на строку с нулевым завершением, содержащую новый текст метки.
 */
void lv_arclabel_set_text(lv_obj_t * obj, const char * text);

/**
 * Задайте форматированный текст объекта метки дуги.
 *
 * Эта функция устанавливает текст объекта метки дуги с поддержкой
 * формирование аргументов аргументов, константа`printf`.
 *
 * @param obj       Объект метки дуги, для которого требуется задать текст.
 * @param fmt       Строка формата, указывающая, как последующие аргументы преобразуются в текст.
 * @param ... Аргументы, следующие за строковой формой, которые используются для замены спецификаторов формы в строке.
 */
void lv_arclabel_set_text_fmt(lv_obj_t * obj, const char * fmt, ...) LV_FORMAT_ATTRIBUTE(2, 3);

/**
 * Устанавливает новый статический текст для метки дуги или обновляет его текущим текстом.
 * «Текст» должен оставаться действительным в памяти; метка дуги не управляет своим жизненным циклом.
 *
 * @param obj       Указатель на объект метки дуги.
 * @param text      Указатель на новый текст. ЕслиNULL, метка обновляется с использованием текущего текста.
 */
void lv_arclabel_set_text_static(lv_obj_t * obj, const char * text);

/**
 * Установите начальный угол дуги. 0 градусов: вправо, 90 вниз и т. д.
 * @param obj       указатель на объект метки дуги
 * @param start     начальный угол. (если`LV_USE_FLOAT`включен, он также может быть дробным.)
 */
void lv_arclabel_set_angle_start(lv_obj_t * obj, lv_value_precise_t start);

/**
 * Установите конечный угол дуги. 0 градусов: вправо, 90 вниз и т. д.
 * @param obj       указатель на объект метки дуги
 * @param size      размер угла (если`LV_USE_FLOAT`включен, он также может быть дробным.)
 */
void lv_arclabel_set_angle_size(lv_obj_t * obj, lv_value_precise_t size);

/**
 * Установите вращение для всей дуги
 * @param obj       указатель на объект метки дуги
 * @param offset    угол поворота
 */
void lv_arclabel_set_offset(lv_obj_t * obj, int32_t offset);

/**
 * Установите тип дуги.
 * @param obj       указатель на объект и метку дуги
 * @param dir      направление метки дуги
 */
void lv_arclabel_set_dir(lv_obj_t * obj, lv_arclabel_dir_t dir);

/**
 * Включите перекрашивание с помощью встроенных команд
 * @param obj       указатель на объект метки дуги
 * @param en        true: включить перекрашивание, false: отключить
 * Example: «Это красное слово #ff0000»
 */
void lv_arclabel_set_recolor(lv_obj_t * obj, bool en);

/**
 * Установите радиус объекта метки дуги.
 *
 * @param obj       указатель на объект метки дуги.
 * @param radius    Значение радиуса, которое необходимо задать для кривизны метки, в пикселях.
 */
void lv_arclabel_set_radius(lv_obj_t * obj, uint32_t radius);

/**
 * Установите смещение центра x для объекта метки дуги.
 * @param obj       указатель на объект метки дуги
 * @param x         смещение по оси X
 */
void lv_arclabel_set_center_offset_x(lv_obj_t * obj, uint32_t x);

/**
 * Установите смещение центра y для объекта метки дуги.
 * @param obj       указатель на объект метки дуги
 * @param y         смещение y
 */
void lv_arclabel_set_center_offset_y(lv_obj_t * obj, uint32_t y);

/**
 * Установите вертикальное выравнивание текста для объекта метки дуги.
 * @param obj       указатель на объект метки дуги
 * @param align     вертикальное выравнивание
 */
void lv_arclabel_set_text_vertical_align(lv_obj_t * obj, lv_arclabel_text_align_t align);

/**
 * Установите горизонтальное выравнивание текста для объекта метки дуги.
 * @param obj       указатель на объект метки дуги
 * @param align     горизонтальное выравнивание
 */
void lv_arclabel_set_text_horizontal_align(lv_obj_t * obj, lv_arclabel_text_align_t align);

/**
 * Установите поведение переполнения для объекта метки дуги.
 * @param obj       указатель на объект метки дуги
 * @param overflow  режим переполнения (видимый, многоточие, клип)
 */
void lv_arclabel_set_overflow(lv_obj_t * obj, lv_arclabel_overflow_t overflow);

/**
 * Установите поведение конечного перекрытия для объекта метки дуги.
 * Это контролирует, как обрабатывается текст, когда он перекрывается в конце дуги в 360 градусов.
 * @param obj       указатель на объект метки дуги
 * @param overlap   установить поведение перекрытия конца метки дуги
 */
void lv_arclabel_set_end_overlap(lv_obj_t * obj, bool overlap);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получите начальный угол метки дуги.
 * @param obj       указатель на объект метки дуги
 * @return          начальный угол [0..360] (если`LV_USE_FLOAT`включен, он также может быть дробным.)
 */
lv_value_precise_t lv_arclabel_get_angle_start(lv_obj_t * obj);

/**
 * Получите размер угла метки дуги.
 * @param obj       указатель на объект метки дуги
 * @return          конечный угол [0..360] (если`LV_USE_FLOAT`включен, он также может быть дробным.)
 */
lv_value_precise_t lv_arclabel_get_angle_size(lv_obj_t * obj);

/**
 * Узнайте, является ли метка дуги типом или нет.
 * @param obj       указатель на объект метки дуги
 * @return          направление метки дуги
 */
lv_arclabel_dir_t lv_arclabel_get_dir(const lv_obj_t * obj);

/**
 * Включите перекрашивание с помощью встроенных команд
 *
 * @see lv_arclabel_set_recolor
 *
 * @param obj       указатель на объект метки
 * @return          true: включить перекрашивание, false: отключить
 */
bool lv_arclabel_get_recolor(lv_obj_t * obj);

/**
 * Получите текст метки дуги.
 * @param obj       указатель на объект метки дуги
 * @return          радиус метки дуги
 */
uint32_t lv_arclabel_get_radius(lv_obj_t * obj);

/**
 * Получите смещение центра x для объекта метки дуги.
 * @param obj       указатель на объект метки дуги
 * @return          смещение по оси X
 */
uint32_t lv_arclabel_get_center_offset_x(lv_obj_t * obj);

/**
 * Получите смещение центра y для объекта метки дуги.
 * @param obj       указатель на объект метки дуги
 * @return          смещение y
 */
uint32_t lv_arclabel_get_center_offset_y(lv_obj_t * obj);

/**
 * Получите вертикальное выравнивание текста для объекта метки дуги.
 * @param obj       указатель на объект метки дуги
 * @return          вертикальное выравнивание
 */
lv_arclabel_text_align_t lv_arclabel_get_text_vertical_align(lv_obj_t * obj);

/**
 * Получите горизонтальное выравнивание текста для объекта метки дуги.
 * @param obj       указатель на объект метки дуги
 * @return          горизонтальное выравнивание
 */
lv_arclabel_text_align_t lv_arclabel_get_text_horizontal_align(lv_obj_t * obj);

/**
 * Получите поведение переполнения для объекта метки дуги.
 * @param obj       указатель на объект метки дуги
 * @return          режим переполнения
 */
lv_arclabel_overflow_t lv_arclabel_get_overflow(lv_obj_t * obj);

/**
 * Получите поведение конечного перекрытия для объекта метки дуги.
 * @param obj       указатель на объект метки дуги
 * @return          режим конечного перекрытия
 */
bool lv_arclabel_get_end_overlap(lv_obj_t * obj);

/**
 * Получите угол текста для объекта метки дуги.
 * @note            Угол текста рассчитывается во время выполнения. Вы можете получить обновленное значение
 *                  после обновления размера arclabel.
 *                  Возвращает реальный угол отрисовки текста в градусах, за исключением
 *                  Режим `LV_ARCLABEL_OVERFLOW_VISIBLE`.
 * @param obj       указатель на объект метки дуги
 * @return          угол текста (если`LV_USE_FLOAT`включен, он также может быть дробным.)
 */
lv_value_precise_t lv_arclabel_get_text_angle(lv_obj_t * obj);

/*=====================
 * Другие функции
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_ARCLABEL*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_ARCLABEL_H*/
