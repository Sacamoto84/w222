#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES
#if LV_USE_FFMPEG

#if LV_FFMPEG_PLAYER_USE_LV_FS
    #define PATH_PREFIX "A:"
#else
    #define PATH_PREFIX "./"
#endif

/**
 * Открыть видео из файла
 */
void lv_example_ffmpeg_2(void)
{
    /*birds.mp4 загружается сhttp://www.videezy.com(бесплатный видеоролик от Videezy!)
     *https://www.videezy.com/abstract/44864-silhouettes-of-birds-over-the-sunset*/
    /*Он будет использовать абстракцию файловой системы LVGL (а не файловую систему OS).
     *если установлен `LV_FFMPEG_PLAYER_USE_LV_FS`.*/
    lv_obj_t * player = lv_ffmpeg_player_create(lv_screen_active());
    /*Note: «h264_v4l2m2m» — это пример аппаратного декодера, специфичный для Linux, доступный только в системах Linux с поддержкой V4L2.
     *При отсутствии поддержки он вернется к программному декодированию.*/
    lv_ffmpeg_player_set_decoder(player, "h264_v4l2m2m");
    lv_ffmpeg_player_set_src(player, PATH_PREFIX "lvgl/examples/libs/ffmpeg/birds.mp4");
    lv_ffmpeg_player_set_auto_restart(player, true);
    lv_ffmpeg_player_set_cmd(player, LV_FFMPEG_PLAYER_CMD_START);
    lv_obj_center(player);
}

#else

void lv_example_ffmpeg_2(void)
{
    /*TODO
     *запасной вариант для онлайн-примеров*/

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "FFmpeg is not installed");
    lv_obj_center(label);
}

#endif
#endif
