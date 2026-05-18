/**
 * @file lv_draw.h
 *
 */

/**
 * Изменено NXP в 2024 г.
 */

#ifndef LV_DRAW_H
#define LV_DRAW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include "../misc/lv_types.h"
#include "../misc/lv_style.h"
#include "../misc/lv_text.h"
#include "../misc/lv_profiler.h"
#include "../misc/lv_matrix.h"
#include "../misc/lv_event.h"
#include "lv_image_decoder.h"
#include "lv_draw_buf.h"

/*********************
 *      DEFINES
 *********************/
#define LV_DRAW_UNIT_NONE  0
#define LV_DRAW_UNIT_IDLE  -1   /**< The draw unit is idle, new dispatching might be requested to try again */

#if LV_DRAW_TRANSFORM_USE_MATRIX
#if !LV_USE_MATRIX
#error "LV_DRAW_TRANSFORM_USE_MATRIX requires LV_USE_MATRIX = 1"
#endif
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_DRAW_TASK_TYPE_NONE = 0,
    LV_DRAW_TASK_TYPE_FILL,
    LV_DRAW_TASK_TYPE_BORDER,
    LV_DRAW_TASK_TYPE_BOX_SHADOW,
    LV_DRAW_TASK_TYPE_LETTER,
    LV_DRAW_TASK_TYPE_LABEL,
    LV_DRAW_TASK_TYPE_IMAGE,
    LV_DRAW_TASK_TYPE_LAYER,
    LV_DRAW_TASK_TYPE_LINE,
    LV_DRAW_TASK_TYPE_ARC,
    LV_DRAW_TASK_TYPE_TRIANGLE,
    LV_DRAW_TASK_TYPE_MASK_RECTANGLE,
    LV_DRAW_TASK_TYPE_MASK_BITMAP,
    LV_DRAW_TASK_TYPE_BLUR,
#if LV_USE_VECTOR_GRAPHIC
    LV_DRAW_TASK_TYPE_VECTOR,
#endif
#if LV_USE_3DTEXTURE
    LV_DRAW_TASK_TYPE_3D,
#endif
} lv_draw_task_type_t;

typedef enum {
    /** Ожидание завершения другой задачи.
     * Например, в случае `LV_DRAW_TASK_TYPE_LAYER` (используется для смешивания слоев)
     * блокируется до тех пор, пока не будут обработаны все задачи рисования слоя. */
    LV_DRAW_TASK_STATE_BLOCKED,

    /** Задача рисования добавляется в список слоев и ожидает рендеринга. */
    LV_DRAW_TASK_STATE_WAITING,

    /** Задача рисования добавляется в очередь команд блока рисования.
     * Поскольку поставленные в очередь задачи выполняются по порядку, можно поставить в очередь несколько задач отрисовки.
     * (для одной и той же тяговой единицы), даже если они зависят друг от друга.
     * Поэтому`lv_draw_get_available_task`и`lv_draw_get_next_available_task`могут вернуть
     * задача рисования для одной и той же единицы рисования, даже если зависимая задача рисования еще не завершена.*/
    LV_DRAW_TASK_STATE_QUEUED,

    /** Задача отрисовки выполняется. Эту задачу по рисованию необходимо выполнить до того, как
     * `lv_draw_get_available_task` и`lv_draw_get_next_available_task`будут
     * возврат любых зависимых задач рисования.*/
    LV_DRAW_TASK_STATE_IN_PROGRESS,

    /** Задача рисования визуализируется. Он будет удален из списка задач рисования слоя.
     * и освобождается автоматически. */
    LV_DRAW_TASK_STATE_FINISHED,
} lv_draw_task_state_t;

struct _lv_layer_t  {
    /** Целевой буфер прорисовки слоя */
    lv_draw_buf_t * draw_buf;

    /** Связанный список задач рисования */
    lv_draw_task_t * draw_task_head;

    /** Родительский слой */
    lv_layer_t * parent;

    /** Следующий слой */
    lv_layer_t * next;

    /** Пользовательские данные */
    void * user_data;

    /** Абсолютные координаты буфера */
    lv_area_t buf_area;

    /** Физическая область отсечения относительно дисплея */
    lv_area_t phy_clip_area;

    /**
     * NEVER USE IT DRAW UNITS. USED INTERNALLY DURING DRAW TASK CREATION.
     * Текущая область обрезки с абсолютными координатами, всегда такими же или такими же, как`buf_area`.
     * Может быть установлен перед добавлением новых задач рисования, чтобы указать область обрезки задач рисования.
     * Поэтому`lv_draw_add_task()`всегда сохраняет его в новой задаче рисования, чтобы знать область обрезки при добавлении задачи рисования.
     * Во время рисования блоков рисования также поддерживается сохраненный clip_area, и его следует использовать во время рисования.
     * Во время рисования область обрезки слоя не должна использоваться, поскольку она может быть уже изменена для других задач рисования.
     */
    lv_area_t _clip_area;

#if LV_DRAW_TRANSFORM_USE_MATRIX
    /** Матрица преобразования, которая будет применяться при рендеринге слоя */
    lv_matrix_t matrix;
#endif

    /** Частичное смещение по оси Y */
    int32_t partial_y_offset;

    /** Перекрасить слой */
    lv_color32_t recolor;

    /** Цветовой формат слоя.  LV_COLOR_FORMAT_... */
    lv_color_format_t color_format;

    /** Флаг, указывающий, что все задачи добавлены */
    bool all_tasks_added;

    /** Непрозрачность слоя */
    lv_opa_t opa;
};

typedef struct {
    /**The widget for which draw descriptor was created */
    lv_obj_t * obj;

    /**The widget part for which draw descriptor was created */
    uint32_t part;

    /**A widget type specific ID (e.g. table row index). See the docs of the given widget.*/
    uint32_t id1;

    /**A widget type specific ID (e.g. table column index). See the docs of the given widget.*/
    uint32_t id2;

    /**The target layer */
    lv_layer_t * layer;

    /*Тень является частью каждого процесса рисования, поскольку тень может быть у чего угодно.*/

    /**Drop shadow offset in X*/
    int16_t drop_shadow_ofs_x;

    /**Drop shadow offset in Y*/
    int16_t drop_shadow_ofs_y;

    /**Drop shadow color*/
    lv_color_t drop_shadow_color;

    /**Drop shadow opacity*/
    lv_opa_t drop_shadow_opa;

    /**Drop shadow blur radius*/
    int32_t drop_shadow_blur_radius: 20;

    /**Drop shadow blur quality*/
    lv_blur_quality_t drop_shadow_quality : 3;

    /**Size of the specific draw descriptor into which this base descriptor is embedded*/
    size_t dsc_size;

    /**Any custom user data*/
    void * user_data;
} lv_draw_dsc_base_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Используется внутри для инициализации модуля рисования.
 */
void lv_draw_init(void);

/**
 * Деинициализировать модуль рисования
 */
void lv_draw_deinit(void);

/**
 * Выделить новую единицу вытяжки заданного размера и добавить ее в список единиц вытяжки.
 * @param size      размер для выделения. Например. `sizeof(my_draw_unit_t)`,
 *                  где первый элемент`my_draw_unit_t`—`lv_draw_unit_t`.
 */
void * lv_draw_create_unit(size_t size);

/**
 * Добавьте пустую задачу рисования в список задач рисования слоя.
 * @param layer     указатель на слой
 * @param coords    координаты задачи рисования
 * @return          созданная задача рисования, которую необходимо
 *                  дополнительная настройка, например. добавив дескриптор отрисовки
 */
lv_draw_task_t * lv_draw_add_task(lv_layer_t * layer, const lv_area_t * coords, lv_draw_task_type_t type);

/**
 * Необходимо вызывать при создании и настройке задачи рисования.
 * Он отправит виджету событие о новой задаче отрисовки.
 * и назначьте его единице рисования.
 * @param layer     указатель на слой
 * @param t         указатель на задачу рисования
 */
void lv_draw_finalize_task_creation(lv_layer_t * layer, lv_draw_task_t * t);

/**
 * Попробуйте отправить задачи рисования для рисования юнитов.
 */
void lv_draw_dispatch(void);

/**
 * Используется внутри, чтобы попытаться диспетчеризировать задачи рисования определенного слоя.
 * @param disp      указатель на дисплей, на котором была запрошена диспетчеризация
 * @param layer     указатель на слой
 * @return          рендерится хотя бы одна задача отрисовки (возможно, она была сделана ранее)
 */
bool lv_draw_dispatch_layer(lv_display_t * disp, lv_layer_t * layer);

/**
 * Ждите нового запроса на отправку.
 * Он блокируется, если `LV_USE_OS == 0`, иначе он дает результат.
 */
void lv_draw_dispatch_wait_for_request(void);

/**
 * Дождитесь завершения отрисовки в случае асинхронного выполнения задачи.
 * Если `LV_USE_OS == 0`, он просто возвращается.
 */
void lv_draw_wait_for_finish(void);

/**
 * Когда блок рисования завершил задачу рисования, ему необходимо запросить диспетчеризацию.
 * чтобы позволить LVGL назначить ему новую задачу рисования
 */
void lv_draw_dispatch_request(void);

/**
 * Получите общее количество единиц рисования.
  */
uint32_t lv_draw_get_unit_count(void);

/**
 * Если имеется только один блок рисования, проверьте первую задачу рисования, если она доступна.
 * Если имеется несколько блоков вытяжек, позвоните по`lv_draw_get_next_available_task`, чтобы найти задачу.
 * @param layer             слой рисования для поиска
 * @param t_prev            продолжить поиск из этой задачи
 * @param draw_unit_id      проверьте задачу, где`preferred_draw_unit_id`равен этому значению или `LV_DRAW_UNIT_NONE`
 * @return                  доступное задание на рисование или NULL, если его нет
 */
lv_draw_task_t * lv_draw_get_available_task(lv_layer_t * layer, lv_draw_task_t * t_prev, uint8_t draw_unit_id);

/**
 * Найти и доступную задачу розыгрыша
 * @param layer             слой рисования для поиска
 * @param t_prev            продолжить поиск из этой задачи
 * @param draw_unit_id      проверьте задачу, где`preferred_draw_unit_id`равен этому значению или `LV_DRAW_UNIT_NONE`
 * @return                  доступное задание на рисование или NULL, если его нет
 */
lv_draw_task_t * lv_draw_get_next_available_task(lv_layer_t * layer, lv_draw_task_t * t_prev, uint8_t draw_unit_id);

/**
 * Сообщите, сколько задач рисования ожидает рисования в области`t_check`.
 * Его можно использовать, чтобы определить, должен ли GPU объединять множество задач рисования в одну или нет.
 * Если много задач ждут текущих, имеет смысл рисовать их поочередно.
 * чтобы не блокировать рендеринг зависимых задач
 * @param t_check   задача, зависимые задачи которой должны учитываться
 * @return          количество задач в зависимости от `t_check`
 */
uint32_t lv_draw_get_dependent_count(lv_draw_task_t * t_check);


/**
 * Отправьте событие на лотки
 * @param name              имя единицы розыгрыша, на которую будет отправлено событие
 * @param code              код события
 * @param param             параметр события
 */
void lv_draw_unit_send_event(const char * name, lv_event_code_t code, void * param);

/**
 * Инициализировать слой
 * @param layer указатель на слой для инициализации
 */
void lv_layer_init(lv_layer_t * layer);

/**
 * Сбросьте слой в доступное для рисования состояние
 * @param layer указатель на слой для сброса
 */
void lv_layer_reset(lv_layer_t * layer);

/**
 * Создайте (выделите) новый слой на родительском слое.
 * @param parent_layer      родительский слой, с которым слой будет объединен при его рендеринге
 * @param color_format      цветовой формат слоя
 * @param area              площади слоя (абсолютные координаты)
 * @return                  новыйtarget_layerилиNULLпри ошибке
 */
lv_layer_t * lv_draw_layer_create(lv_layer_t * parent_layer, lv_color_format_t color_format, const lv_area_t * area);

/**
 * Инициализировать слой, выделенный пользователем
 * @param layer             указатель слоя для инициализации (его срок службы должен управляться пользователем)
 * @param parent_layer      родительский слой, с которым слой будет объединен при его рендеринге
 * @param color_format      цветовой формат слоя
 * @param area              площади слоя (абсолютные координаты)
 * @return                  новыйtarget_layerилиNULLпри ошибке
 */
void lv_draw_layer_init(lv_layer_t * layer, lv_layer_t * parent_layer, lv_color_format_t color_format,
                        const lv_area_t * area);

/**
 * Попробуйте выделить буфер для слоя.
 * @param layer             указатель на слой
 * @return                  указатель на выделенный выровненный буфер илиNULLв случае сбоя
 */
void * lv_draw_layer_alloc_buf(lv_layer_t * layer);

/**
 * Добрался до пикселя по координатам X и Y на слое.
 * @param layer             указатель на слой
 * @param x                 целевая координата X
 * @param y                 целевая координата X
 * @return                  Смещение`buf`для указания на заданные координаты X и Y.
 */
void * lv_draw_layer_go_to_xy(lv_layer_t * layer, int32_t x, int32_t y);

/**
 * Получить тип задачи рисования
 * @param t   задача рисования, чтобы получить тип
 * @return    тип задачи рисования
*/
lv_draw_task_type_t lv_draw_task_get_type(const lv_draw_task_t * t);

/**
 * Получить дескриптор отрисовки задачи отрисовки
 * @param t   задача отрисовки, чтобы получить дескриптор отрисовки
 * @return    пустой указатель на дескриптор отрисовки
*/
void * lv_draw_task_get_draw_dsc(const lv_draw_task_t * t);

/**
 * Получить область рисования задачи рисования
 * @param t      задача рисования, чтобы получить область рисования
 * @param area   место назначения, где будет храниться область рисования
*/
void lv_draw_task_get_area(const lv_draw_task_t * t, lv_area_t * area);



lv_layer_t * lv_draw_layer_create_drop_shadow(lv_layer_t * parent_layer, const lv_draw_dsc_base_t * base,
                                              const lv_area_t * area);

void lv_draw_layer_finish_drop_shadow(lv_layer_t * drop_shadow_layer, const lv_draw_dsc_base_t * base);


/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_DRAW_H*/
