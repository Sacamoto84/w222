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
 * - local files (file://)
 * - web streams (http://, https://)
 * - RTSP streams (rtsp://)
 * - UDP streams (udp://)
 * и многие другие.
 * Uridecodebin GStreamer автоматически выбирает соответствующий
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
 * Создайте объект gstreamer
 * @param parent    pointer to an object, it will be the parent of the new gstreamer
 * @return          pointer to the created gstreamer
 */
lv_obj_t * lv_gstreamer_create(lv_obj_t * parent);

/**
 * Добавьте источник к этому объекту gstreamer
 * @param gstreamer     pointer to a gstreamer object
 * @param factory_name  the factory name for the source of this gstreamer object.
 *                      для общих заводских имен проверьте, что `LV_GSTREAMER_FACTORY_XXX` определяет
 * @param property      the property name for the gstreamer source object
 *                      общие свойства см. в разделе `LV_GSTREAMER_PROPERTY_XXX`, определяет
 *                      Передача NULL создаст исходный объект, но не установит его источник.
 * @param source        the property value for the gstreamer source object
 *                      Передача NULL создаст исходный объект, но не установит его источник.
 * @return LV_RESULT_OK if the source was correctly set else LV_RESULT_INVALID
 */
lv_result_t lv_gstreamer_set_src(lv_obj_t * gstreamer, const char * factory_name, const char * property,
                                 const char * source);

/**
 * Включи этот gstreamer
 * @param gstreamer     pointer to a gstreamer object
 */
void lv_gstreamer_play(lv_obj_t * gstreamer);

/**
 * Приостановить этот gstreamer
 * @param gstreamer     pointer to a gstreamer object
 */
void lv_gstreamer_pause(lv_obj_t * gstreamer);

/**
 * Останови этот gstreamer
 * @param gstreamer     pointer to a gstreamer object
 */
void lv_gstreamer_stop(lv_obj_t * gstreamer);

/**
 * Ищите позицию в этом gstreamer
 * @param gstreamer     pointer to a gstreamer object
 * @param position      position to seek to
 */
void lv_gstreamer_set_position(lv_obj_t * gstreamer, uint32_t position);

/**
 * Получить продолжительность этого gstreamer
 * @param gstreamer     pointer to a gstreamer object
 * @return              the duration (in ms) of the gstreamer object
 */
uint32_t lv_gstreamer_get_duration(lv_obj_t * gstreamer);

/**
 * Получить позицию этого gstreamer
 * @param gstreamer     pointer to a gstreamer object
 * @return              the position (in ms) of the gstreamer object
 */
uint32_t lv_gstreamer_get_position(lv_obj_t * gstreamer);

/**
 * Получить состояние этого gstreamer
 * @param gstreamer     pointer to a gstreamer object
 */
lv_gstreamer_state_t lv_gstreamer_get_state(lv_obj_t * gstreamer);

/**
 * Установите громкость этого gstreamer
 * @param gstreamer     pointer to a gstreamer object
 * @param volume         the value to set in the range [0..100]. Higher values are clamped
 */
void lv_gstreamer_set_volume(lv_obj_t * gstreamer, uint8_t volume);

/**
 * Получить объем этого gstreamer
 * @param gstreamer     pointer to a gstreamer object
 * @return      the volume for this gstreamer
 */
uint8_t lv_gstreamer_get_volume(lv_obj_t * gstreamer);

/**
 * Установите скорость этого gstreamer
 * @param gstreamer     pointer to a gstreamer object
 * @param rate      the rate factor.  Example values:
 *                      - 256:   1x
 *                      - <256:  slow down
 *                      - >256:  speed up
 *                      - 128:   0.5x
 *                      - 512:   2x
 */
void lv_gstreamer_set_rate(lv_obj_t * gstreamer, uint32_t rate);

/**
 * Получите состояние потока из обратного вызова события STATE_CHANGED.
 * @param e     pointer to the event
 * @return the stream state or -1 if `e` is invalid (i.e. NULL or does not match expected event)
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
