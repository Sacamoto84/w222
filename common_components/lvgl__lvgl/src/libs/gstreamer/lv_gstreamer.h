/**
 * @file lv_gstreamer.h
 *
 */

#ifndef LV_GSTREAMER_H
#define LV_GSTREAMER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_GSTREAMER
#include "../../core/lv_obj.h"


/*********************
 *      DEFINES
 *********************/


/* Используя «фабрику» `URI`, мы можем указать различные схемы URI в качестве источников мультимедиа, включая
 * - локальные файлы (файл://)
 * - веб-потоки (http://,https://)
 * - ПотокиRTSP(rtsp://)
 * - ПотокиUDP(udp://)
 * и многие другие.
 * Uridecodebin GStreamer автоматически выбирает подходящий
 * исходный элемент и декодер на основе схемы URI и медиаформата. */
#define LV_GSTREAMER_FACTORY_URI_DECODE      "uridecodebin"
#define LV_GSTREAMER_PROPERTY_URI_DECODE      "uri"

#define LV_GSTREAMER_FACTORY_FILE            "filesrc"
#define LV_GSTREAMER_PROPERTY_FILE            "location"

#define LV_GSTREAMER_FACTORY_HTTP            "souphttpsrc"
#define LV_GSTREAMER_PROPERTY_HTTP            "location"

#define LV_GSTREAMER_FACTORY_HTTPS           "souphttpsrc"
#define LV_GSTREAMER_PROPERTY_HTTPS           "location"

#define LV_GSTREAMER_FACTORY_V4L2_CAMERA     "v4l2src"
#define LV_GSTREAMER_PROPERTY_V4L2_CAMERA     "device"

#define LV_GSTREAMER_FACTORY_ALSA_AUDIO      "alsasrc"
#define LV_GSTREAMER_PROPERTY_ALSA_AUDIO      "device"

#define LV_GSTREAMER_FACTORY_PULSE_AUDIO     "pulsesrc"
#define LV_GSTREAMER_PROPERTY_PULSE_AUDIO     "device"

#define LV_GSTREAMER_FACTORY_TEST_AUDIO      "audiotestsrc"
#define LV_GSTREAMER_PROPERTY_TEST_AUDIO      NULL

#define LV_GSTREAMER_FACTORY_TEST_VIDEO      "videotestsrc"
#define LV_GSTREAMER_PROPERTY_TEST_VIDEO      NULL

#define LV_GSTREAMER_FACTORY_APP             "appsrc"
#define LV_GSTREAMER_PROPERTY_APP             NULL

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_GSTREAMER_STATE_NULL,
    LV_GSTREAMER_STATE_READY,
    LV_GSTREAMER_STATE_PAUSED,
    LV_GSTREAMER_STATE_PLAYING
} lv_gstreamer_state_t;

typedef enum {
    LV_GSTREAMER_STREAM_STATE_START,
    LV_GSTREAMER_STREAM_STATE_PLAY,
    LV_GSTREAMER_STREAM_STATE_PAUSE,
    LV_GSTREAMER_STREAM_STATE_STOP,
    LV_GSTREAMER_STREAM_STATE_END
} lv_gstreamer_stream_state_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * создать объект gstreamer
 * @param parent    указатель на объект, он будет родителем нового gstreamer
 * @return          указатель на созданный gstreamer
 */
lv_obj_t * lv_gstreamer_create(lv_obj_t * parent);

/**
 * добавьте источник к этому объекту gstreamer
 * @param gstreamer     указатель на объект gstreamer
 * @param factory_name  заводское имя источника этого объекта gstreamer.
 *                      для общих заводских имен проверьте, что `LV_GSTREAMER_FACTORY_XXX` определяет
 * @param property      имя свойства исходного объекта gstreamer
 *                      общие свойства см. в разделе `LV_GSTREAMER_PROPERTY_XXX`, определяет
 *                      Передача NULL создаст исходный объект, но не установит его источник.
 * @param source        значение свойства для исходного объекта gstreamer
 *                      Передача NULL создаст исходный объект, но не установит его источник.
 * @return LV_RESULT_OK, если источник был установлен правильно, иначе LV_RESULT_INVALID
 */
lv_result_t lv_gstreamer_set_src(lv_obj_t * gstreamer, const char * factory_name, const char * property,
                                 const char * source);

/**
 * Включи этот gstreamer
 * @param gstreamer     указатель на объект gstreamer
 */
void lv_gstreamer_play(lv_obj_t * gstreamer);

/**
 * Приостановить этот gstreamer
 * @param gstreamer     указатель на объект gstreamer
 */
void lv_gstreamer_pause(lv_obj_t * gstreamer);

/**
 * Останови этот gstreamer
 * @param gstreamer     указатель на объект gstreamer
 */
void lv_gstreamer_stop(lv_obj_t * gstreamer);

/**
 * Ищите позицию в этом gstreamer
 * @param gstreamer     указатель на объект gstreamer
 * @param position      положение, чтобы стремиться
 */
void lv_gstreamer_set_position(lv_obj_t * gstreamer, uint32_t position);

/**
 * Получить продолжительность этого gstreamer
 * @param gstreamer     указатель на объект gstreamer
 * @return              продолжительность (в мс) объекта gstreamer
 */
uint32_t lv_gstreamer_get_duration(lv_obj_t * gstreamer);

/**
 * Получить позицию этого gstreamer
 * @param gstreamer     указатель на объект gstreamer
 * @return              позиция (в мс) объекта gstreamer
 */
uint32_t lv_gstreamer_get_position(lv_obj_t * gstreamer);

/**
 * Получить состояние этого gstreamer
 * @param gstreamer     указатель на объект gstreamer
 */
lv_gstreamer_state_t lv_gstreamer_get_state(lv_obj_t * gstreamer);

/**
 * Установить громкость этого gstreamer
 * @param gstreamer     указатель на объект gstreamer
 * @param volume         значение, которое необходимо установить в диапазоне [0..100]. Более высокие значения фиксируются
 */
void lv_gstreamer_set_volume(lv_obj_t * gstreamer, uint8_t volume);

/**
 * Получить громкость этого gstreamer
 * @param gstreamer     указатель на объект gstreamer
 * @return      громкость для этого gstreamer
 */
uint8_t lv_gstreamer_get_volume(lv_obj_t * gstreamer);

/**
 * Установите скорость этого gstreamer
 * @param gstreamer     указатель на объект gstreamer
 * @param rate      коэффициент ставки.  Примеры значений:
 *                      - 256:   1x
 *                      - <256: замедлить
 *                      - >256: ускориться
 *                      - 128:   0.5x
 *                      - 512:   2x
 */
void lv_gstreamer_set_rate(lv_obj_t * gstreamer, uint32_t rate);

/**
 * Получите состояние потока из обратного вызова события STATE_CHANGED.
 * @param e     указатель на событие
 * @return состояние потока или -1, если`e`недействителен (т. е.NULLили не соответствует ожидаемому событию)
 */
lv_gstreamer_stream_state_t lv_gstreamer_get_stream_state(lv_event_t * e);


/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GSTREAMER*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_GSTREAMER_H*/
