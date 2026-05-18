#include "../../lv_examples.h"
#if LV_USE_BMP && LV_BUILD_EXAMPLES

/**
 * Откройте файл BMP из файла
 */
void lv_example_bmp_1(void)
{
    lv_obj_t * img = lv_image_create(lv_screen_active());
    /* Предполагая, что файловая система прикреплена к букве «А»,
     * например, установитеLV_USE_FS_STDIO'A' в lv_conf.h */
    lv_image_set_src(img, "A:lvgl/examples/libs/bmp/example_32bit.bmp");
    lv_obj_center(img);

}

#endif
