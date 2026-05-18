/**
 * @file lv_observer.h
 *
 */

#ifndef LV_OBSERVER_H
#define LV_OBSERVER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../misc/lv_ext_data.h"
#include "lv_obj.h"

#if LV_USE_OBSERVER

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Значения для полей`type`lv_subject_t
 */
typedef enum {
    LV_SUBJECT_TYPE_INVALID =   0,   /**< субъект еще не инициализирован */
    LV_SUBJECT_TYPE_NONE =      1,   /**< пустое значение, наподобие None или NIL */
    LV_SUBJECT_TYPE_INT =       2,   /**< значение int32_t */
    LV_SUBJECT_TYPE_FLOAT =     3,   /**< значение float, требует `LV_USE_FLOAT 1` */
    LV_SUBJECT_TYPE_POINTER =   4,   /**< указатель void * */
    LV_SUBJECT_TYPE_COLOR   =   5,   /**< значение lv_color_t */
    LV_SUBJECT_TYPE_GROUP  =    6,   /**< массив субъектов */
    LV_SUBJECT_TYPE_STRING  =   7,   /**< указатель char * */
} lv_subject_type_t;

/**
 * Общий тип для одинаковой обработки всех различных наблюдаемых типов.
 */
typedef union {
    int32_t num;           /**< Целое число: непрозрачность, enum, bool или обычное число */
    const void * pointer;  /**< Константный указатель: буфер строки, формат, шрифт и т. п. */
    lv_color_t color;      /**< Цвет */
#if LV_USE_FLOAT
    float float_v;         /**< Число с плавающей точкой*/
#endif
} lv_subject_value_t;

/**
 * Субъект (наблюдаемая величина)
 */
struct _lv_subject_t {
#if LV_USE_EXT_DATA
    lv_ext_data_t ext_data;
#endif
    lv_ll_t subs_ll;                     /**< Подписчики */
    lv_subject_value_t value;            /**< Текущее значение */
    lv_subject_value_t prev_value;       /**< Предыдущее значение */
    lv_subject_value_t min_value;        /**< Минимальное значение для int или float*/
    lv_subject_value_t max_value;        /**< Максимальное значение для int или float*/
    void * user_data;                    /**< Дополнительные данные пользователя */
    uint32_t type                 :  4;  /**< Одно из значений LV_SUBJECT_TYPE_... */
    uint32_t size                 : 24;  /**< Размер строкового буфера или длина группы */
    uint32_t notify_restart_query :  1;  /**< Если наблюдатель был удален во время уведомления,
                                          * начните уведомлять с самого начала. */
};

/**
  * Обратный вызов, вызываемый для уведомления наблюдателя об изменении значения субъекта.
  * @param subject     указатель на наблюдателя
  * @param subject      указатель на наблюдаемый субъект
  */
typedef void (*lv_observer_cb_t)(lv_observer_t * observer, lv_subject_t * subject);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if LV_USE_EXT_DATA
/**
 * @brief Прикрепляет внешние пользовательские данные к целочисленному субъекту с управлением жизненным циклом.
 *
 * Связывает произвольные пользовательские данные с наблюдателем LVGL и регистрирует деструктор.
 * обратный вызов, который будет автоматически вызываться при удалении наблюдателя. Это позволяет:
 * - Безопасная очистка ресурсов с помощью механизма деструктора
 * - Контекстное хранилище данных для обратных вызовов наблюдателя
 * - Правильное управление памятью для ресурсов, связанных с наблюдателем
 *
 * @param subject    указатель на субъект
 * @param data       Пользовательский указатель данных для связи
 * @param free_cb    Функция очистки вызывается, когда:
 *                   - Наблюдатель явно удален
 *                   - Наблюдаемый объект удален
 *                   - Новые данные заменяют текущую ассоциацию
 *                   NULL указывает, что очистка не требуется.
 */
void lv_subject_set_external_data(lv_subject_t * subject, void * data, void (* free_cb)(void * data));
#endif

/**
 * Инициализируйте субъект целочисленного типа.
 * @param subject   указатель на субъект
 * @param value     начальное значение
 */
void lv_subject_init_int(lv_subject_t * subject, int32_t value);

/**
 * Установите значение целочисленного субъекта и уведомите об этом наблюдателей.
 * @param subject   указатель на субъект
 * @param value     новое значение
 */
void lv_subject_set_int(lv_subject_t * subject, int32_t value);

/**
 * Получить текущее значение целочисленного объекта.
 * @param subject   указатель на субъект
 * @return          текущая стоимость
 */
int32_t lv_subject_get_int(lv_subject_t * subject);

/**
 * Получить предыдущее значение целочисленного объекта.
 * @param subject   указатель на субъект
 * @return          текущая стоимость
 */
int32_t lv_subject_get_previous_int(lv_subject_t * subject);


/**
 * Установите минимальное значение для целочисленного субъекта
 * @param subject   указатель на субъект
 * @param min_value минимальное значение
 */
void lv_subject_set_min_value_int(lv_subject_t * subject, int32_t min_value);

/**
 * Установите максимальное значение для целочисленного субъекта
 * @param subject   указатель на субъект
 * @param max_value максимальное значение
 */
void lv_subject_set_max_value_int(lv_subject_t * subject, int32_t max_value);

#if LV_USE_FLOAT

/**
 * Инициализируйте объект типа float.
 * @param subject   указатель на субъект
 * @param value     начальное значение
 */
void lv_subject_init_float(lv_subject_t * subject, float value);

/**
 * Установите значение плавающей субъекта и уведомите об этом наблюдателей.
 * @param subject   указатель на субъект
 * @param value     новое значение
 */
void lv_subject_set_float(lv_subject_t * subject, float value);

/**
 * Получить текущее значение объекта с плавающей запятой.
 * @param subject   указатель на субъект
 * @return          текущая стоимость
 */
float lv_subject_get_float(lv_subject_t * subject);

/**
 * Получить предыдущее значение плавающей субъекта.
 * @param subject   указатель на субъект
 * @return          текущая стоимость
 */
float lv_subject_get_previous_float(lv_subject_t * subject);

/**
 * Установите минимальное значение для плавающей субъекта
 * @param subject   указатель на субъект
 * @param min_value минимальное значение
 */
void lv_subject_set_min_value_float(lv_subject_t * subject, float min_value);

/**
 * Установите максимальное значение для плавающей субъекта
 * @param subject   указатель на субъект
 * @param max_value максимальное значение
 */
void lv_subject_set_max_value_float(lv_subject_t * subject, float max_value);

#endif /*LV_USE_FLOAT*/

/**
 * Инициализируйте субъект строкового типа.
 * @param subject   указатель на субъект
 * @param buf       указатель на буфер для хранения строки
 * @param prev_buf  указатель на буфер для хранения предыдущей строки; может быть NULL, если не используется
 * @param size      размер буфера(ов)
 * @param value     начальное значение строки, например. "привет"
 * @note            Строковый субъект хранит собственную копию строки, а не только указатель.
 */
void lv_subject_init_string(lv_subject_t * subject, char * buf, char * prev_buf, size_t size, const char * value);

/**
 * Скопируйте строку в субъект и уведомите наблюдателей, если она изменилась.
 * @param subject   указатель на субъект
 * @param buf       новая строка
 */
void lv_subject_copy_string(lv_subject_t * subject, const char * buf);

/**
 * Отформатируйте новую строку, обновив субъект, и уведомите наблюдателей, если она изменилась.
 * @param subject   указатель на субъект
 * @param format    строка формата
 */
void lv_subject_snprintf(lv_subject_t * subject, const char * format, ...) LV_FORMAT_ATTRIBUTE(2, 3);

/**
 * Получить текущее значение строки Субъект.
 * @param subject   указатель на субъект
 * @return          указатель на буфер, содержащий текущее значение
 */
const char * lv_subject_get_string(lv_subject_t * subject);

/**
 * Получить предыдущее значение строки Субъект.
 * @param subject   указатель на субъект
 * @return          указатель на буфер, содержащий предыдущее значение
 * @note            NULL будет возвращен, еслиNULLбыл передан в `lv_subject_init_string()`.
 *                  как`prev_buf`.
 */
const char * lv_subject_get_previous_string(lv_subject_t * subject);

/**
 * Инициализируйте объект типа указатель.
 * @param subject   указатель на субъект
 * @param value     начальное значение
 */
void lv_subject_init_pointer(lv_subject_t * subject, void * value);

/**
 * Установите значение указателя Субъект и уведомите об этом Наблюдателей (независимо от того, изменилось ли оно).
 * @param subject   указатель на субъект
 * @param ptr       новое значение
 */
void lv_subject_set_pointer(lv_subject_t * subject, void * ptr);

/**
 * Получить текущее значение указателя Субъект.
 * @param subject   указатель на субъект
 * @return          текущая стоимость
 */
const void * lv_subject_get_pointer(lv_subject_t * subject);

/**
 * Получить предыдущее значение указателя Субъект.
 * @param subject   указатель на субъект
 * @return          предыдущее значение
 */
const void * lv_subject_get_previous_pointer(lv_subject_t * subject);

/**
 * Инициализируйте объект цветового типа.
 * @param subject   указатель на субъект
 * @param color     начальное значение
 */
void lv_subject_init_color(lv_subject_t * subject, lv_color_t color);

/**
 * Установите значение цвета Субъекта и уведомите наблюдателей, если оно изменилось.
 * @param subject   указатель на субъект
 * @param color     новое значение
 */
void lv_subject_set_color(lv_subject_t * subject, lv_color_t color);

/**
 * Получить текущее значение цвета.
 * @param subject   указатель на субъект
 * @return          текущая стоимость
 */
lv_color_t lv_subject_get_color(lv_subject_t * subject);

/**
 * Получить предыдущее значение цвета.
 * @param subject   указатель на субъект
 * @return          предыдущее значение
 */
lv_color_t lv_subject_get_previous_color(lv_subject_t * subject);

/**
 * Инициализируйте субъект типа группы.
 * @param group_subject  указатель на субъект типа группы
 * @param list           список адресов других Субъектов; когда любой из них имеет значения
                             обновлено, наблюдатели`group_subject`будут уведомлены.
 * @param list_len       количество элементов в `list[]`
 */
void lv_subject_init_group(lv_subject_t * group_subject, lv_subject_t * list[], uint32_t list_len);

/**
 * Удалите всех наблюдателей из субъекта, освободите выделенную память и удалите
 * любые связанные события Widget-Binding.  Это оставляет`subject`«отключенным» от
 * все наблюдатели и все связанные с ними события виджета, установленные посредством привязки виджета.
 * @param subject   указатель на субъект
 * @note            Это можно безопасно вызвать независимо от того, есть ли какие-либо наблюдатели.
 *                  добавлено с помощью`lv_subject_add_observer_obj()`или заявлено о свойстве виджета
 *                  с одной из функций `..._bind_...()`.
 */
void lv_subject_deinit(lv_subject_t * subject);

/**
 * Получите элемент из списка субъекттической группы.
 * @param subject   указатель на субъект типа группы
 * @param index     индекс элемента, который нужно получить
 * @return          указатель на проиндексированную субъект из списка или NULL, если индекс выходит за пределы
 */
lv_subject_t * lv_subject_get_group_element(lv_subject_t * subject, int32_t index);

/**
 * посетить наблюдателя к субъекту. Когда значение субъекта изменится, будет вызвано`observer_cb`.
 * @param subject       указатель на субъект
 * @param observer_cb   обратный вызов уведомления
 * @param user_data     дополнительные пользовательские данные
 * @return              указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_subject_add_observer(lv_subject_t * subject, lv_observer_cb_t observer_cb, void * user_data);

/**
 * Добавьте наблюдателя в субъект для виджета.
 * Когда виджет будет удален, Observer автоматически отпишется от Темы.
 * @param subject       указатель на субъект
 * @param observer_cb   обратный вызов уведомления
 * @param obj           указатель на виджет
 * @param user_data     дополнительные пользовательские данные
 * @return              указатель на вновь созданный наблюдатель
 * @note                Не вызывайте`lv_observer_remove()`для наблюдателей, созданных таким образом.
 *                      Очищайте таких наблюдателей только одним из следующих способов:
 *                      - удаление виджета или
 *                      - вызов`lv_subject_deinit()`для корректного разделения и
 *                        удалить всех наблюдателей.
 */
lv_observer_t * lv_subject_add_observer_obj(lv_subject_t * subject, lv_observer_cb_t observer_cb, lv_obj_t * obj,
                                            void * user_data);

/**
 * Добавьте наблюдателя к субъекту, а также сохраните указатель цели.
 * @param subject       указатель на субъект
 * @param observer_cb   обратный вызов уведомления
 * @param target        любой указатель (NULL подойдет)
 * @param user_data     дополнительные пользовательские данные
 * @return              указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_subject_add_observer_with_target(lv_subject_t * subject, lv_observer_cb_t observer_cb,
                                                    void * target, void * user_data);

/**
 * Удалить наблюдателя из его субъекта.
 * @param observer      указатель на наблюдателя
 */
void lv_observer_remove(lv_observer_t * observer);

/**
 * Удалить наблюдателей, связанных с виджетом`obj`, из указанных`subject`или всех субъектов.
 * @param obj       указатель на виджет, наблюдатели которого следует удалить
 * @param subject   Субъект для удаления виджета илиNULLдля удаления из всех тем
 * @note            Эту функцию можно использовать, например. когда субъект(ы) виджета должны
 *                  быть заменен другим субъектом(ами)
 */
void lv_obj_remove_from_subject(lv_obj_t * obj, lv_subject_t * subject);

/**
 * Получите цель наблюдателя.
 * @param observer      указатель на наблюдателя
 * @return              указатель на сохраненную цель
 */
void * lv_observer_get_target(lv_observer_t * observer);

/**
 * Подключите виджет Observer.
 * Это то же самое, что и `lv_observer_get_target()`, за исключением того, что возвращает `target`.
 * как`lv_obj_t *`.
 * @param observer      указатель на наблюдателя
 * @return              указатель на сохраненную цель виджета
 */
lv_obj_t * lv_observer_get_target_obj(lv_observer_t * observer);

/**
 * Получите пользовательские данные Observer.
 * @param observer      указатель на наблюдателя
 * @return              void указатель на сохраненные пользовательские данные
*/
void * lv_observer_get_user_data(const lv_observer_t * observer);

/**
 * Сообщите всем наблюдателям о субъекте.
 * @param subject       указатель на субъект
 */
void lv_subject_notify(lv_subject_t * subject);

/**
 * Добавьте обработчик событий для увеличения (или уменьшения) значения субъекта в триггере.
 * @param obj       указатель на виджет
 * @param subject   указатель на субъект изменения
 * @param trigger   триггер, по которому следует сменить субъект
 * @param step      значение, добавляемое к триггеру
 *                  если минимальное значение достигнуто, максимальное значение будет установлено при ролловере.
 */
lv_subject_increment_dsc_t * lv_obj_add_subject_increment_event(lv_obj_t * obj, lv_subject_t * subject,
                                                                lv_event_code_t trigger, int32_t step);

/**
 * Установите минимальное значение субъекта, которое будет установлено событием
 * @param obj           указатель на виджет, к которому прикреплено событие
 * @param dsc           указатель на дескриптор, возвращаемый `lv_obj_add_subject_increment_event()`
 * @param min_value     минимальное значение для установки
 */
void lv_obj_set_subject_increment_event_min_value(lv_obj_t * obj, lv_subject_increment_dsc_t * dsc, int32_t min_value);

/**
 * Установите максимальное значение субъекта, устанавливаемое событием
 * @param obj           указатель на виджет, к которому прикреплено событие
 * @param dsc           указатель на дескриптор, возвращаемый `lv_obj_add_subject_increment_event()`
 * @param max_value     максимальное значение, которое можно установить
 */
void lv_obj_set_subject_increment_event_max_value(lv_obj_t * obj, lv_subject_increment_dsc_t * dsc, int32_t max_value);

/**
 * Установите, что делать при превышении минимального/максимального значения.
 * @param obj           указатель на виджет, к которому прикреплено событие
 * @param dsc           указатель на дескриптор, возвращаемый `lv_obj_add_subject_increment_event()`
 * @param rollover      false: остановиться на минимальном/максимальном значении; true: перепрыгнуть на другой конец
 * @note                субъект также может иметь минимальные/максимальные значения, и всегда будет рассматриваться меньший диапазон
 */
void lv_obj_set_subject_increment_event_rollover(lv_obj_t * obj, lv_subject_increment_dsc_t * dsc, bool rollover);

/**
* Переключить значение целочисленной субъекта в событии. Если было != 0, то будет 0.
* Если было 0, будет 1.
* @param obj       указатель на виджет
* @param subject   указатель на субъект для переключения
* @param trigger   триггер, по которому следует сменить субъект
*/
void lv_obj_add_subject_toggle_event(lv_obj_t * obj, lv_subject_t * subject, lv_event_code_t trigger);

/**
 * Установите значение целочисленного субъекта.
 * @param obj       указатель на виджет
 * @param subject   указатель на субъект изменения
 * @param trigger   триггер, по которому следует сменить субъект
 * @param value     значение, которое нужно установить
 */
void lv_obj_add_subject_set_int_event(lv_obj_t * obj, lv_subject_t * subject, lv_event_code_t trigger, int32_t value);


#if LV_USE_FLOAT
/**
 * Установите значение плавающей субъекта.
 * @param obj       указатель на виджет
 * @param subject   указатель на субъект изменения
 * @param trigger   триггер, по которому следует сменить субъект
 * @param value     значение, которое нужно установить
 */
void lv_obj_add_subject_set_float_event(lv_obj_t * obj, lv_subject_t * subject, lv_event_code_t trigger, float value);
#endif

/**
 * Установите значение субъекта строки.
 * @param obj       указатель на виджет
 * @param subject   указатель на субъект изменения
 * @param trigger   триггер, по которому следует сменить субъект
 * @param value     значение, которое нужно установить
 */
void lv_obj_add_subject_set_string_event(lv_obj_t * obj, lv_subject_t * subject, lv_event_code_t trigger,
                                         const char * value);

/**
 * Установите флаг(и) виджета, если целочисленное значение субъекта равно эталонному значению, в противном случае снимите флаг.
 * @param obj           указатель на виджет
 * @param subject       указатель на субъект
 * @param flag          флаг(и) (может быть побитовыйобъединенные побитовым ИЛИ) для установки или очистки (например,`LV_OBJ_FLAG_HIDDEN`)
 * @param ref_value     эталонное значение для сравнения значения субъекта с
 * @return              указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_obj_bind_flag_if_eq(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value);

/**
 * Установите флаг(и) виджета, если целочисленное значение субъекта не равно ссылочному значению, в противном случае снимите флаг.
 * @param obj           указатель на виджет
 * @param subject       указатель на субъект
 * @param flag          флаг(и) (может быть побитовыйобъединенные побитовым ИЛИ) для установки или очистки (например,`LV_OBJ_FLAG_HIDDEN`)
 * @param ref_value     эталонное значение для сравнения значения субъекта с
 * @return              указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_obj_bind_flag_if_not_eq(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag,
                                           int32_t ref_value);

/**
 * Установите флаг(и) виджета, если целочисленное значение субъекта больше ссылочного значения, в противном случае снимите флаг.
 * @param obj           указатель на виджет
 * @param subject       указатель на субъект
 * @param flag          флаг(и) (может быть побитовыйобъединенные побитовым ИЛИ) для установки или очистки (например,`LV_OBJ_FLAG_HIDDEN`)
 * @param ref_value     эталонное значение для сравнения значения субъекта с
 * @return              указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_obj_bind_flag_if_gt(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value);

/**
 * Установите флаг(и) виджета, если целочисленное значение субъекта больше или равно эталонному значению, в противном случае снимите флаг.
 * @param obj           указатель на виджет
 * @param subject       указатель на субъект
 * @param flag          флаг(и) (может быть побитовыйобъединенные побитовым ИЛИ) для установки или очистки (например,`LV_OBJ_FLAG_HIDDEN`)
 * @param ref_value     эталонное значение для сравнения значения субъекта с
 * @return              указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_obj_bind_flag_if_ge(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value);

/**
 * Установите флаг(и) виджета, если целочисленное значение субъекта меньше эталонного значения, в противном случае снимите флаг.
 * @param obj           указатель на виджет
 * @param subject       указатель на субъект
 * @param flag          флаг(и) (может быть побитовыйобъединенные побитовым ИЛИ) для установки или очистки (например,`LV_OBJ_FLAG_HIDDEN`)
 * @param ref_value     эталонное значение для сравнения значения субъекта с
 * @return              указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_obj_bind_flag_if_lt(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value);

/**
 * Установите флаг(и) виджета, если целочисленное значение субъекта меньше или равно эталонному значению, в противном случае снимите флаг.
 * @param obj           указатель на виджет
 * @param subject       указатель на субъект
 * @param flag          флаг(и) (может быть побитовыйобъединенные побитовым ИЛИ) для установки или очистки (например,`LV_OBJ_FLAG_HIDDEN`)
 * @param ref_value     эталонное значение для сравнения значения субъекта с
 * @return              указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_obj_bind_flag_if_le(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value);


/**
 * Установите состояние(я) виджета, если целое значение субъекта равно эталонному значению, в противном случае снимите флаг.
 * @param obj           указатель на виджет
 * @param subject       указатель на субъект
 * @param state         состояние(я) (может быть побитовоеобъединенные побитовым ИЛИ) для установки или очистки (например,`LV_STATE_CHECKED`)
 * @param ref_value     эталонное значение для сравнения значения субъекта с
 * @return              указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_obj_bind_state_if_eq(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value);

/**
 * Установите состояние(я) виджета, если целочисленное значение субъекта не равно ссылочному значению, в противном случае снимите флаг.
 * @param obj           указатель на виджет
 * @param subject       указатель на субъект
 * @param state         состояние(я) (может быть побитовоеобъединенные побитовым ИЛИ) для установки или очистки (например,`LV_STATE_CHECKED`)
 * @param ref_value     эталонное значение для сравнения значения субъекта с
 * @return              указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_obj_bind_state_if_not_eq(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state,
                                            int32_t ref_value);

/**
 * Установите состояние(я) виджета, если целочисленное значение субъекта больше опорного значения, в противном случае снимите флаг.
 * @param obj           указатель на виджет
 * @param subject       указатель на субъект
 * @param state         состояние(я) (может быть побитовоеобъединенные побитовым ИЛИ) для установки или очистки (например,`LV_STATE_CHECKED`)
 * @param ref_value     эталонное значение для сравнения значения субъекта с
 * @return              указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_obj_bind_state_if_gt(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value);

/**
 * Установите состояние(я) виджета, если целочисленное значение субъекта больше или равно опорному значению, в противном случае снимите флаг.
 * @param obj           указатель на виджет
 * @param subject       указатель на субъект
 * @param state         состояние(я) (может быть побитовоеобъединенные побитовым ИЛИ) для установки или очистки (например,`LV_STATE_CHECKED`)
 * @param ref_value     эталонное значение для сравнения значения субъекта с
 * @return              указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_obj_bind_state_if_ge(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value);

/**
 * Установите состояние(я) виджета, если целочисленное значение субъекта меньше опорного значения, в противном случае снимите флаг.
 * @param obj           указатель на виджет
 * @param subject       указатель на субъект
 * @param state         состояние(я) (может быть побитовоеобъединенные побитовым ИЛИ) для установки или очистки (например,`LV_STATE_CHECKED`)
 * @param ref_value     эталонное значение для сравнения значения субъекта с
 * @return              указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_obj_bind_state_if_lt(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value);

/**
 * Установите состояние(я) виджета, если целочисленное значение субъекта меньше или равно опорному значению, в противном случае снимите флаг.
 * @param obj           указатель на виджет
 * @param subject       указатель на субъект
 * @param state         состояние(я) (может быть побитовоеобъединенные побитовым ИЛИ) для установки или очистки (например,`LV_STATE_CHECKED`)
 * @param ref_value     эталонное значение для сравнения значения субъекта с
 * @return              указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_obj_bind_state_if_le(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value);

/**
 * Установите для целочисленного субъекта значение 1, когда виджет отмечен, и установите его 0, если флажок снят, и
 * очистить проверенное состояние виджета, когда значение субъекта изменится на 0, и установить его, если оно не равно нулю.
 * @param obj       указатель на виджет
 * @param subject   указатель на субъект
 * @return          указатель на вновь созданный наблюдатель
 * @note            Убедитесь, что флаг`LV_OBJ_FLAG_CHECKABLE`виджета установлен.
 */
lv_observer_t * lv_obj_bind_checked(lv_obj_t * obj, lv_subject_t * subject);


/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OBSERVER*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBSERVER_H*/
