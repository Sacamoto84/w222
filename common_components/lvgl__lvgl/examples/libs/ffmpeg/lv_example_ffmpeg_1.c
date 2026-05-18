#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES
#if LV_USE_FFMPEG && LV_FFMPEG_PLAYER_USE_LV_FS

/**
 * Открыть изображение из файла
 */
void lv_example_ffmpeg_1(void)
{
    /*Он всегда использует абстракцию файловой системы LVGL (а не файловую систему OS).
     *открыть изображение, в отличие от `lv_ffmpeg_player_set_src`, которое зависит от
     *настройка `LV_FFMPEG_PLAYER_USE_LV_FS`.*/
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:lvgl/examples/libs/ffmpeg/ffmpeg.png");
    lv_obj_center(img);
}

#else

void lv_example_ffmpeg_1(void)
{
    /*TODO
     *запасной вариант для онлайн-примеров*/

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "FFmpeg is not installed");
    lv_obj_center(label);
}

#endif
#endif
