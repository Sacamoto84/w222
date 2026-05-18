/**
 * @file lv_image.h
 *
 */

#ifndef LV_IMAGE_H
#define LV_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_IMAGE != 0

/*Тестирование зависимостей*/
#if LV_USE_LABEL == 0
#error "lv_img: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL 1)"
#endif

#include "../../core/lv_obj.h"
#include "../../misc/lv_fs.h"
#include "../../draw/lv_draw.h"
#include "../../core/lv_observer.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_image_class;

/**
 * Режим размера изображения, когда размер изображения и размер объекта различаются.
 */
typedef enum {
    LV_IMAGE_ALIGN_DEFAULT = 0,
    LV_IMAGE_ALIGN_TOP_LEFT,
    LV_IMAGE_ALIGN_TOP_MID,
    LV_IMAGE_ALIGN_TOP_RIGHT,
    LV_IMAGE_ALIGN_BOTTOM_LEFT,
    LV_IMAGE_ALIGN_BOTTOM_MID,
    LV_IMAGE_ALIGN_BOTTOM_RIGHT,
    LV_IMAGE_ALIGN_LEFT_MID,
    LV_IMAGE_ALIGN_RIGHT_MID,
    LV_IMAGE_ALIGN_CENTER,
    _LV_IMAGE_ALIGN_AUTO_TRANSFORM, /**< Marks the start of modes that transform the image*/
    LV_IMAGE_ALIGN_STRETCH, /**< Set X and Y scale to fill the Widget's area. */
    LV_IMAGE_ALIGN_TILE,    /**< Tile image to fill Widget's area. Offset is applied to shift the tiling. */
    LV_IMAGE_ALIGN_CONTAIN, /**< The image keeps its aspect ratio, but is resized to the maximum size that fits within the Widget's area. */
    LV_IMAGE_ALIGN_COVER,   /**< The image keeps its aspect ratio and fills the Widget's area. */
} lv_image_align_t;

#if LV_USE_OBJ_PROPERTY
enum _lv_property_image_id_t {
    LV_PROPERTY_ID(IMAGE, SRC,          LV_PROPERTY_TYPE_IMGSRC,    0),
    LV_PROPERTY_ID(IMAGE, OFFSET_X,     LV_PROPERTY_TYPE_INT,       1),
    LV_PROPERTY_ID(IMAGE, OFFSET_Y,     LV_PROPERTY_TYPE_INT,       2),
    LV_PROPERTY_ID(IMAGE, ROTATION,     LV_PROPERTY_TYPE_INT,       3),
    LV_PROPERTY_ID(IMAGE, PIVOT,        LV_PROPERTY_TYPE_POINT,     4),
    LV_PROPERTY_ID(IMAGE, SCALE,        LV_PROPERTY_TYPE_INT,       5),
    LV_PROPERTY_ID(IMAGE, SCALE_X,      LV_PROPERTY_TYPE_INT,       6),
    LV_PROPERTY_ID(IMAGE, SCALE_Y,      LV_PROPERTY_TYPE_INT,       7),
    LV_PROPERTY_ID(IMAGE, BLEND_MODE,   LV_PROPERTY_TYPE_INT,       8),
    LV_PROPERTY_ID(IMAGE, ANTIALIAS,    LV_PROPERTY_TYPE_INT,       9),
    LV_PROPERTY_ID(IMAGE, INNER_ALIGN,  LV_PROPERTY_TYPE_INT,       10),
    LV_PROPERTY_IMAGE_END,
};
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте объект изображения
 * @param parent pointer to an object, it will be the parent of the new image
 * @return pointer to the created image
 */
lv_obj_t * lv_image_create(lv_obj_t * parent);

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите данные изображения для отображения на объекте
 * @param obj       pointer to an image object
 * @param src       1) pointer to an ::lv_image_dsc_t descriptor (converted by LVGL's image converter) (e.g. &my_img) or
 *                  2) path to an image file (e.g. "S:/dir/img.bin")or
 *                  3) a SYMBOL (e.g. LV_SYMBOL_OK)
 */
void lv_image_set_src(lv_obj_t * obj, const void * src);

/**
 * Установите смещение для источника изображения, чтобы изображение отображалось из нового источника.
 * @param obj       pointer to an image
 * @param x         the new offset along x axis.
 */
void lv_image_set_offset_x(lv_obj_t * obj, int32_t x);

/**
 * Установите смещение для источника изображения.
 * поэтому изображение будет отображаться из нового источника.
 * @param obj       pointer to an image
 * @param y         the new offset along y axis.
 */
void lv_image_set_offset_y(lv_obj_t * obj, int32_t y);

/**
 * Установите угол поворота изображения.
 * Изображение будет повернуто вокруг установленной опорной точки, заданной `lv_image_set_pivot()`.
 * Обратите внимание, что индексированные изображения и изображения, содержащие только альфа-каналы, преобразовать невозможно.
 * @param obj       pointer to an image object
 * @param angle     rotation in degree with 0.1 degree resolution (0..3600: clock wise)
 * @note            if image_align is `LV_IMAGE_ALIGN_STRETCH` or `LV_IMAGE_ALIGN_FIT`
 *                  вращение будет автоматически установлено на 0.
 *
 */
void lv_image_set_rotation(lv_obj_t * obj, int32_t angle);

/**
 * Установите центр вращения изображения.
 * Изображение будет повернуто вокруг этой точки.
 * x, y можно установить со значением LV_PCT, в этом случае lv_image_get_pivot вернет истинную координату точки поворота в пикселях.
 * @param obj       pointer to an image object
 * @param x         rotation center x of the image
 * @param y         rotation center y of the image
 */
void lv_image_set_pivot(lv_obj_t * obj, int32_t x, int32_t y);

/**
 * Установите поворот горизонтального центра изображения.
 * @param obj       pointer to an image object
 * @param x         rotation center x of the image, or lv_pct()
 */
void lv_image_set_pivot_x(lv_obj_t * obj, int32_t x);

/**
 * Установите поворот вертикального центра изображения.
 * @param obj       pointer to an image object
 * @param y         rotation center y of the image, or lv_pct()
 */
void lv_image_set_pivot_y(lv_obj_t * obj, int32_t y);

/**
 * Установите коэффициент масштабирования изображения.
 * Обратите внимание, что индексированные изображения и изображения, содержащие только альфа-каналы, преобразовать невозможно.
 * @param obj       pointer to an image object
 * @param zoom      the zoom factor.  Example values:
 *                      - 256 or LV_SCALE_NONE:  no zoom
 *                      - <256:  scale down
 *                      - >256:  scale up
 *                      - 128:  half size
 *                      - 512:  double size
 */
void lv_image_set_scale(lv_obj_t * obj, uint32_t zoom);

/**
 * Установите коэффициент масштабирования изображения по горизонтали.
 * Обратите внимание, что индексированные изображения и изображения, содержащие только альфа-каналы, преобразовать невозможно.
 * @param obj       pointer to an image object
 * @param zoom      the zoom factor.  Example values:
 *                      - 256 or LV_SCALE_NONE:  no zoom
 *                      - <256:  scale down
 *                      - >256:  scale up
 *                      - 128:  half size
 *                      - 512:  double size
 */
void lv_image_set_scale_x(lv_obj_t * obj, uint32_t zoom);

/**
 * Установите коэффициент масштабирования изображения по вертикали.
 * Обратите внимание, что индексированные изображения и изображения, содержащие только альфа-каналы, преобразовать невозможно.
 * @param obj       pointer to an image object
 * @param zoom      the zoom factor.  Example values:
 *                      - 256 or LV_SCALE_NONE:  no zoom
 *                      - <256:  scale down
 *                      - >256:  scale up
 *                      - 128:  half size
 *                      - 512:  double size
 */
void lv_image_set_scale_y(lv_obj_t * obj, uint32_t zoom);

/**
 * Установите режим наложения изображения.
 * @param obj           pointer to an image object
 * @param blend_mode    the new blend mode
 */
void lv_image_set_blend_mode(lv_obj_t * obj, lv_blend_mode_t blend_mode);

/**
 * Включить/отключить сглаживание для преобразований (поворот, масштабирование) или нет.
 * Качество лучше, сглаживание выглядит лучше, но медленнее.
 * @param obj       pointer to an image object
 * @param antialias true: anti-aliased; false: not anti-aliased
 */
void lv_image_set_antialias(lv_obj_t * obj, bool antialias);

/**
 * Установите режим размера объекта изображения.
 * @param obj       pointer to an image object
 * @param align     the new align mode.
 * @note            if image_align is `LV_IMAGE_ALIGN_STRETCH` or `LV_IMAGE_ALIGN_FIT`
 *                  вращение, масштаб и поворот будут перезаписаны и будут контролироваться изнутри.
 */
void lv_image_set_inner_align(lv_obj_t * obj, lv_image_align_t align);

/**
 * Установите растровую маску A8 для изображения.
 * @param obj       pointer to an image object
 * @param src       an lv_image_dsc_t bitmap mask source.
 */
void lv_image_set_bitmap_map_src(lv_obj_t * obj, const lv_image_dsc_t * src);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получить источник изображения
 * @param obj       pointer to an image object
 * @return          the image source (symbol, file name or ::lv-img_dsc_t for C arrays)
 */
const void * lv_image_get_src(lv_obj_t * obj);

/**
 * Получите атрибут x смещения объекта изображения.
 * @param obj       pointer to an image
 * @return          offset X value.
 */
int32_t lv_image_get_offset_x(lv_obj_t * obj);

/**
 * Получите атрибут смещения y объекта изображения.
 * @param obj       pointer to an image
 * @return          offset Y value.
 */
int32_t lv_image_get_offset_y(lv_obj_t * obj);

/**
 * Получите поворот изображения.
 * @param obj       pointer to an image object
 * @return          rotation in 0.1 degrees (0..3600)
 * @note            if image_align is `LV_IMAGE_ALIGN_STRETCH` or  `LV_IMAGE_ALIGN_FIT`
 *                  вращение будет автоматически установлено на 0.
 */
int32_t lv_image_get_rotation(lv_obj_t * obj);

/**
 * Получите ось (центр вращения) изображения.
 * Если точка поворота установлена с помощью LV_PCT , преобразуйте ее в px перед возвратом.
 * @param obj       pointer to an image object
 * @param pivot     store the rotation center here
 */
void lv_image_get_pivot(lv_obj_t * obj, lv_point_t * pivot);

/**
 * Получите коэффициент масштабирования изображения.
 * @param obj       pointer to an image object
 * @return          zoom factor (256: no zoom)
 */
int32_t lv_image_get_scale(lv_obj_t * obj);

/**
 * Получите коэффициент горизонтального масштабирования изображения.
 * @param obj       pointer to an image object
 * @return          zoom factor (256: no zoom)
 */
int32_t lv_image_get_scale_x(lv_obj_t * obj);

/**
 * Получите коэффициент вертикального масштабирования изображения.
 * @param obj       pointer to an image object
 * @return          zoom factor (256: no zoom)
 */
int32_t lv_image_get_scale_y(lv_obj_t * obj);

/**
 * Получите ширину изображения перед любыми преобразованиями.
 * @param obj Pointer to an image object.
 * @return The width of the image.
 */
int32_t lv_image_get_src_width(lv_obj_t * obj);

/**
 * Получите высоту изображения перед любыми преобразованиями.
 * @param obj Pointer to an image object.
 * @return The height of the image.
 */
int32_t lv_image_get_src_height(lv_obj_t * obj);

/**
 * Получите преобразованную ширину объекта изображения.
 * @param obj Pointer to an image object.
 * @return The transformed width of the image.
 */
int32_t lv_image_get_transformed_width(lv_obj_t * obj);

/**
 * Получите преобразованную высоту объекта изображения.
 * @param obj Pointer to an image object.
 * @return The transformed height of the image.
 */
int32_t lv_image_get_transformed_height(lv_obj_t * obj);

/**
 * Получить текущий режим наложения изображения
 * @param obj       pointer to an image object
 * @return          the current blend mode
 */
lv_blend_mode_t lv_image_get_blend_mode(lv_obj_t * obj);

/**
 * Узнайте, сглажены ли преобразования (поворот, масштабирование) или нет.
 * @param obj       pointer to an image object
 * @return          true: anti-aliased; false: not anti-aliased
 */
bool lv_image_get_antialias(lv_obj_t * obj);

/**
 * Получить режим размера изображения
 * @param obj       pointer to an image object
 * @return          element of `lv_image_align_t`
 */
lv_image_align_t lv_image_get_inner_align(lv_obj_t * obj);

/**
 * Получите источник растровой маски.
 * @param obj       pointer to an image object
 * @return          an lv_image_dsc_t bitmap mask source.
 */
const lv_image_dsc_t * lv_image_get_bitmap_map_src(lv_obj_t * obj);


#if LV_USE_OBSERVER
/**
 * Привяжите указатель к источнику изображения.
 * @param obj       pointer to Image
 * @param subject   pointer to Subject
 * @return          pointer to newly-created Observer
 */
lv_observer_t * lv_image_bind_src(lv_obj_t * obj, lv_subject_t * subject);
#endif

/**********************
 *      MACROS
 **********************/

/** Используйте этот макрос, чтобы объявить изображение в файле C.*/
#define LV_IMAGE_DECLARE(var_name) extern const lv_image_dsc_t var_name

#endif /*LV_USE_IMAGE*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_IMAGE_H*/
