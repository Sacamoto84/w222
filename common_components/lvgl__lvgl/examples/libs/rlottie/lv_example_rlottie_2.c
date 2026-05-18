#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES
#if LV_USE_RLOTTIE

/**
 * Загрузить анимацию лотереи из файла
 */
void lv_example_rlottie_2(void)
{
    /*Библиотека rlottie использует файлSTDIOAPI , поэтому дляLVGLнет букв драйвера.*/
    lv_obj_t * lottie = lv_rlottie_create_from_file(lv_screen_active(), 100, 100,
                                                    "lvgl/examples/libs/rlottie/lv_example_rlottie_approve.json");
    lv_obj_center(lottie);
}

#else
void lv_example_rlottie_2(void)
{
    /*TODO
     *запасной вариант для онлайн-примеров*/

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Rlottie is not installed");
    lv_obj_center(label);
}

#endif
#endif
