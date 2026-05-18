/**
 * @file lv_ffmpeg.h
 *
 */
#ifndef LV_FFMPEG_H
#define LV_FFMPEG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_FFMPEG != 0
#include "../../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct ffmpeg_context_s;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_ffmpeg_player_class;

typedef enum {
    LV_FFMPEG_PLAYER_CMD_START,
    LV_FFMPEG_PLAYER_CMD_STOP,
    LV_FFMPEG_PLAYER_CMD_PAUSE,
    LV_FFMPEG_PLAYER_CMD_RESUME,
    LV_FFMPEG_PLAYER_CMD_LAST
} lv_ffmpeg_player_cmd_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Зарегистрировать декодер изображений FFMPEG
 */
void lv_ffmpeg_init(void);

/**
 * Деинициализация декодера изображений FFMPEG
 */
void lv_ffmpeg_deinit(void);

/**
 * Получить количество кадров, содержащихся в файле
 * @param path имя изображения или видеофайла
 * @return Количество кадров меньше 0 означает неудачу.
 */
int lv_ffmpeg_get_frame_num(const char * path);

/**
 * Создать объект ffmpeg_player
 * @param parent указатель на объект, он будет родителем нового игрока
 * @return указатель на созданный ffmpeg_player
 */
lv_obj_t * lv_ffmpeg_player_create(lv_obj_t * parent);

/**
 * Установите путь к файлу, который будет воспроизводиться.
 * @param obj указатель на объект ffmpeg_player
 * @param path путь к видеофайлу
 * @return LV_RESULT_OK: нет ошибок;  LV_RESULT_INVALID: не могу получить информацию.
 */
lv_result_t lv_ffmpeg_player_set_src(lv_obj_t * obj, const char * path);

/**
 * Установить командное управление видеоплеером
 * @param obj указатель на объект ffmpeg_player
 * @param cmd команды управления
 */
void lv_ffmpeg_player_set_cmd(lv_obj_t * obj, lv_ffmpeg_player_cmd_t cmd);

/**
 * Установите автоматическое воспроизведение видео
 * @param obj указатель на объект ffmpeg_player
 * @param en true: включить автоматический перезапуск
 */
void lv_ffmpeg_player_set_auto_restart(lv_obj_t * obj, bool en);

/**
 * Установите видеодекодер
 * @param obj указатель на объект ffmpeg_player
 * @param decoder_name имя декодера
 */
void lv_ffmpeg_player_set_decoder(lv_obj_t * obj, const char * decoder_name);
/*=====================
 * Другие функции
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_FFMPEG*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FFMPEG_H*/
