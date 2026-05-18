#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_SLIDER && LV_USE_LOG

/**
 * Тестирование между модальным окном с полным фоном и модальным окном перекрашивания.
 */
void lv_example_style_20(void)
{
    /*Добавьтеlv_example_style_12в качестве фона.*/
    lv_example_style_12();

    /* Установите значение 1, чтобы включить наложение перекрашивания вместо сплошного фона. */
#if 0
    /* Примените оттенок по всему экрану с помощью перекраски (эффективное наложение).
     * Это изменяет внешний вид, смешивая полупрозрачный цвет.
     * поверх существующего контента без создания дополнительных объектов.
     * Его производительность ниже, чем у полноразмерного фонового объекта. */
    lv_obj_set_style_recolor(lv_screen_active(), lv_color_black(), 0);
    lv_obj_set_style_recolor_opa(lv_screen_active(), LV_OPA_50, 0);
#else
    /* Имитируйте модальный фон, установив полупрозрачный черный фон.
     * наlv_layer_top(), самом деле в высоком встроенном расположении.
     * Этот метод создает новый полноэкранный объект и может потреблять больше ресурсов.
     * по сравнению с перекрашиванием, особенно при использовании изображений или градиентов. */
    lv_obj_set_style_bg_color(lv_layer_top(), lv_color_black(), 0);
    lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_50, 0);
#endif

    lv_obj_t * obj = lv_slider_create(lv_layer_top());
    lv_obj_center(obj);

    lv_refr_now(NULL); /*Обновление макетов и рендеринг*/

    lv_obj_invalidate(lv_screen_active());

    uint32_t t = lv_tick_get();
    lv_refr_now(NULL); /*Только рендеринг*/
    LV_LOG_USER("%" LV_PRIu32 " ms\n", lv_tick_elaps(t));
}

#endif
