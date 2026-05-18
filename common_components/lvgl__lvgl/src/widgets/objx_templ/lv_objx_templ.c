/**
 * @file lv_objx_templ.c
 *
 */

/**
 * TODO Удалить эти инструкции
 * Поиск и замена: templ -> короткое имя объекта в регистре памяти (например, btn, label и т. д.)
 *                    TEMPL -> короткое имя объекта в верхнем регистре (например, BTN , LABEL и т. д.)
 *
 * Вы можете удалить предложениеdefined()из приведенного ниже оператора #if. Это существует, потому что
 * По умолчаниюLV_USE_TEMPLне входит вlv_conf.hили lv_conf_template.h.
 */

/*********************
 *      INCLUDES
 *********************/
//#include "lv_templ.h" /*TODOраскомментируйте это */

#if defined(LV_USE_TEMPL) && LV_USE_TEMPL != 0

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_templ_class)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_templ_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_templ_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_templ_event(const lv_obj_class_t * class_p, lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_templ_class = {
    .constructor_cb = lv_templ_constructor,
    .destructor_cb = lv_templ_destructor,
    .event_cb = lv_templ_event,
    .width_def = LV_DPI_DEF,
    .height_def = LV_DPI_DEF,
    .instance_size = sizeof(lv_templ_t),
    .group_def = LV_OBJ_CLASS_GROUP_DEF_INHERIT,
    .editable = LV_OBJ_CLASS_EDITABLE_INHERIT,
    .base_class = &lv_templ_class,
    .name = "lv_templ",
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_templ_create(lv_obj_t * parent)
{

    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*======================
 * Добавить/удалить функции
 *=====================*/

/*
 * Сюда входят новые функции «добавить» или «удалить», специфичные для объекта.
 */

/*=====================
 * Функции установки
 *====================*/

/*
 * Здесь появляются новые функции «набора», специфичные для объекта.
 */

/*=====================
 * Геттерные функции
 *====================*/

/*
 * Здесь созданы новые функции «получить», специфичные для объекта.
 */

/*=====================
 * Другие функции
 *====================*/

/*
 * Здесь появляются новые «другие» функции, специфичные для объекта.
 */

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_templ_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_templ_t * templ = (lv_templ_t *)obj;
    /*Инициализируйте данные виджета*/

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_templ_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    lv_templ_t * templ = (lv_templ_t *)obj;
    /*Освободите данные виджета*/
}

static void lv_templ_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res;

    /*Вызов обработчика событий предка*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    /*Добавьте сюда обработку событий, специфичных для виджета.*/
}

#else /*Включите этот файл вверху*/

/*Этот вымышленный typedef существует исключительно для того, чтобы успеть замолчать -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
