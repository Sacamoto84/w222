/**
 * @file nuttx_video_fb.h
 *
 */

#ifndef LV_NUTTX_VIDEO_FB_H
#define LV_NUTTX_VIDEO_FB_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <stdint.h>
#include <stddef.h>

/*********************
 *      DEFINES
 *********************/

#define _FBIOC(x) (x)

/* Определения цветового формата.  Это в значительной степени определяет цвет пикселя
 * организация обработки видеоконтроллера.
 */

/* Монохромные форматы ********************************************************/

#define FB_FMT_Y1             0         /* BPP =1, монохромный */
#define FB_FMT_Y2             1         /* BPP =2, 2-битная несжатая шкала серого */
#define FB_FMT_Y4             2         /* BPP =4, 4-битная несжатая шкала серого */
#define FB_FMT_Y8             3         /* BPP =8, 8-битная несжатая шкала серого */
#define FB_FMT_Y16            4         /* BPP =16, 16-битная несжатая шкала серого */
#define FB_FMT_GREY           FB_FMT_Y8 /* BPP=8 */
#define FB_FMT_Y800           FB_FMT_Y8 /* BPP=8 */

#define FB_ISMONO(f)          (((f) >= FB_FMT_Y1) && (f) <= FB_FMT_Y16)

/* Форматы видео RGB ********************************************************/

/* Стандартный RGB */

#define FB_FMT_RGB4           5           /* BPP=4 */
#define FB_FMT_RGB8           6           /* BPP =8 Индекс палитры RGB */
#define FB_FMT_RGB8_222       7           /* BPP=8  R=2, G=2, B=2 */
#define FB_FMT_RGB8_332       8           /* BPP=8  R=3, G=3, B=2 */
#define FB_FMT_RGB12_444      9           /* BPP=12 R=4, G=4, B=4 */
#define FB_FMT_RGB16_555      10          /* BPP =16 R=5, G=5, B=5 (1 неиспользуемый бит) */
#define FB_FMT_RGB16_565      11          /* BPP=16 R=5, G=6, B=5 */
#define FB_FMT_RGB24          12          /* BPP=24 */
#define FB_FMT_RGB32          13          /* BPP=32 */

/* Длина серии закодирована RGB */

#define FB_FMT_RGBRLE4        14          /* BPP=4 */
#define FB_FMT_RGBRLE8        15          /* BPP=8 */

/* Сырой RGB */

#define FB_FMT_RGBRAW         16          /* BPP=? */

/* Необработанный RGB с произвольной упаковкой сэмпла в пиксель. Упаковка и
 * точность компонентов R, G и B определяется битовыми масками для каждого из них.
 */

#define FB_FMT_RGBBTFLD16     17          /* BPP=16 */
#define FB_FMT_RGBBTFLD24     18          /* BPP=24 */
#define FB_FMT_RGBBTFLD32     19          /* BPP=32 */
#define FB_FMT_RGBA16         20          /* BPP =16 Необработанный RGB с альфа-каналом */
#define FB_FMT_RGBA32         21          /* BPP =32 Необработанный RGB с альфа-каналом */

/* Необработанный RGB с полем прозрачности. Компоновка аналогична стандартному RGB на 16 и
 * 32 бита на пиксель, но старший бит в каждом пикселе указывает, является ли пиксель
 * прозрачный или нет.
 */

#define FB_FMT_RGBT16         22          /* BPP=16 */
#define FB_FMT_RGBT32         23          /* BPP=32 */

#define FB_ISRGB(f)           (((f) >= FB_FMT_RGB4) && (f) <= FB_FMT_RGBT32)

/* Упакованные форматы YUV ********************************************************/

#define FB_FMT_AYUV           24          /* BPP =32 Комбинация YUV и альфа */
#define FB_FMT_CLJR           25          /* BPP =8 4 пикселя вставлены вuint32_t.
                                           *         YUV 4:1:1 с l< 8 бит
                                           *         за образец YUV */
#define FB_FMT_CYUV           26          /* BPP =16 UYVY, за исключением того, что высота
                                           *         перевернутый */
#define FB_FMT_IRAW           27          /* BPP=?   Intel uncompressed YUV */
#define FB_FMT_IUYV           28          /* BPP =16 Чересстрочный UYVY (порядок строк
                                           *         0,2,4,.., 1,3,5...) */
#define FB_FMT_IY41           29          /* BPP =12 Чересстрочный Y41P (порядок строк
                                           *         0,2,4,.., 1,3,5...) */
#define FB_FMT_IYU2           30          /* BPP=24 */
#define FB_FMT_HDYC           31          /* BPP =16 UYVY, за исключением использования BT709
                                           *         цветовое пространство  */
#define FB_FMT_UYVP           32          /* BPP=24? YCbCr 4:2:2, 10-bits per
                                           *         компонент в порядке U0Y0V0Y1 */
#define FB_FMT_UYVY           33          /* BPP=16  YUV 4:2:2 */
#define FB_FMT_UYNV           FB_FMT_UYVY /* BPP=16  */
#define FB_FMT_Y422           FB_FMT_UYVY /* BPP=16  */
#define FB_FMT_V210           34          /* BPP =32 10-битный 4:2:2 YCrCb */
#define FB_FMT_V422           35          /* BPP =16 Перевернутая версия UYVY */
#define FB_FMT_V655           36          /* BPP=16? 16-bit YUV 4:2:2 */
#define FB_FMT_VYUY           37          /* BPP=?   ATI Packed YUV Data */
#define FB_FMT_YUYV           38          /* BPP=16  YUV 4:2:2 */
#define FB_FMT_YUY2           FB_FMT_YUYV /* BPP=16  YUV 4:2:2 */
#define FB_FMT_YUNV           FB_FMT_YUYV /* BPP=16  YUV 4:2:2 */
#define FB_FMT_YVYU           39          /* BPP=16  YUV 4:2:2 */
#define FB_FMT_Y41P           40          /* BPP=12  YUV 4:1:1 */
#define FB_FMT_Y411           41          /* BPP=12  YUV 4:1:1 */
#define FB_FMT_Y211           42          /* BPP=8  */
#define FB_FMT_Y41T           43          /* BPP =12 Y41P LSB для прозрачности */
#define FB_FMT_Y42T           44          /* BPP =16 UYVY LSB для прозрачности */
#define FB_FMT_YUVP           45          /* BPP=24? YCbCr 4:2:2 Y0U0Y1V0 order */

#define FB_ISYUVPACKED(f)     (((f) >= FB_FMT_AYUV) && (f) <= FB_FMT_YUVP)

/* Упакованные планарные форматы YUV **************************************************/

#define FB_FMT_YVU9           46          /* BPP =9 8-битный Y, за которым следует 8-битный
                                           *         4x4 VU */
#define FB_FMT_YUV9           47          /* BPP=9? */
#define FB_FMT_IF09           48          /* BPP =9,5 YVU9 + плоскость треугольника 4x4
                                           *         относительно tframe. */
#define FB_FMT_YV16           49          /* BPP =16 8-битный Y, за которым следует 8-битный
                                           *         2x1 VU */
#define FB_FMT_YV12           50          /* BPP =12 8-битный Y, за которым следует 8-битный
                                           *         2x2 VU */
#define FB_FMT_I420           51          /* BPP =12 8-битный Y, за которым следует 8-битный
                                           *         2x2 UV */
#define FB_FMT_IYUV           FB_FMT_I420 /* BPP=12 */
#define FB_FMT_NV12           52          /* BPP = 12 8-битных Y, за которыми следует
                                           *         чередование 2x2 UV */
#define FB_FMT_NV21           53          /* BPP =12 NV12 с перевернутым UV */
#define FB_FMT_IMC1           54          /* BPP =12 YV12, кроме плоскостей UV, то же самое
                                           *         шагать как Y */
#define FB_FMT_IMC2           55          /* BPP =12 IMC1 кроме строк UV
                                           *         чередуются на полшага
                                           *         границы */
#define FB_FMT_IMC3           56          /* BPP =12 Как IMC1, за исключением UV
                                           *         поменяли местами */
#define FB_FMT_IMC4           57          /* BPP =12 Как IMC2, за исключением UV
                                           *         поменяли местами */
#define FB_FMT_CLPL           58          /* BPP =12 YV12 но включая уровень
                                           *         косвенности. */
#define FB_FMT_Y41B           59          /* BPP=12?  4:1:1 planar. */
#define FB_FMT_Y42B           60          /* BPP=16?  YUV 4:2:2 planar. */
#define FB_FMT_CXY1           61          /* BPP=12 */
#define FB_FMT_CXY2           62          /* BPP=16 */

#define FB_ISYUVPLANAR(f)     (((f) >= FB_FMT_YVU9) && (f) <= FB_FMT_CXY2)
#define FB_ISYUV(f)           (FB_ISYUVPACKED(f) || FB_ISYUVPLANAR(f))

/* Аппаратное управление курсором **************************************************/

#ifdef CONFIG_FB_HWCURSOR
#  define FB_CUR_ENABLE       0x01        /* Включить курсор */
#  define FB_CUR_SETIMAGE     0x02        /* Установить изображение курсора */
#  define FB_CUR_SETPOSITION  0x04        /* Установите положение курсора */
#  define FB_CUR_SETSIZE      0x08        /* Установите размер курсора */
#  define FB_CUR_XOR          0x10        /* Используйте XOR против COPY ROP на изображении. */
#endif

/* Аппаратное ускорение наложения ********************************************/

#define FB_NO_OVERLAY         -1

#ifdef CONFIG_FB_OVERLAY
#  define FB_ACCL_TRANSP      0x01        /* Аппаратная поддержка прозрачности */
#  define FB_ACCL_CHROMA      0x02        /* Аппаратная поддержка хромакея */
#  define FB_ACCL_COLOR       0x04        /* Аппаратная поддержка цвета */
#  define FB_ACCL_AREA        0x08        /* Выбор области поддержки оборудования */

#ifdef CONFIG_FB_OVERLAY_BLIT
#  define FB_ACCL_BLIT        0x10        /* Аппаратная поддержка блитинга */
#  define FB_ACCL_BLEND       0x20        /* Поддержка аппаратного сочетания */
#endif

/* Режим прозрачности наложения ***********************************************/

#  define FB_CONST_ALPHA      0x00         /* Прозрачность по значению альфа */
#  define FB_PIXEL_ALPHA      0x01         /* Прозрачность по значению альфа пикселя */

#endif /* CONFIG_FB_OVERLAY */

/* Символьный драйвер FB Команды IOCTL *****************************************/

/* ioctls */

#define FBIOGET_VIDEOINFO     _FBIOC(0x0001)  /* Получить информацию о цветовой плоскости */
/* Argument: записываемая структура
 *           fb_videoinfo_s */
#define FBIOGET_PLANEINFO     _FBIOC(0x0002)  /* Получить информацию о видеоплане */
/* Argument: записываемая структура
 *           fb_planeinfo_s */

#ifdef CONFIG_FB_CMAP
#  define FBIOGET_CMAP        _FBIOC(0x0003)  /* Получить цветовую карту RGB */
/* Argument: записываемая структура
 *           fb_cmap_s */
#  define FBIOPUT_CMAP        _FBIOC(0x0004)  /* Поставьте цветовое отображение RGB. */
/* Argument: структура только для чтения
 *           fb_cmap_s */
#endif

#ifdef CONFIG_FB_HWCURSOR
#  define FBIOGET_CURSOR      _FBIOC(0x0005)  /* Получить атрибуты курсора */
/* Argument: записываемая структура
 *           fb_cursorattrib_s */
#  define FBIOPUT_CURSOR      _FBIOC(0x0006)  /* Установить атрибуты курсора */
/* Argument: структура только для чтения
 *           fb_setcursor_s */
#endif

#ifdef CONFIG_FB_UPDATE
#  define FBIO_UPDATE         _FBIOC(0x0007)  /* Обновите прямоугольную область в
                                               * фреймбуфер
                                               * Argument: структура только для чтения
                                               *           fb_area_s */
#endif

#ifdef CONFIG_FB_SYNC
#  define FBIO_WAITFORVSYNC   _FBIOC(0x0008)  /* Дождитесь вертикальной синхронизации */
#endif

#ifdef CONFIG_FB_OVERLAY
#  define FBIOGET_OVERLAYINFO _FBIOC(0x0009)  /* Получить информацию о наложении видео */
/* Argument: записываемая структура
 *           fb_overlayinfo_s */
#  define FBIO_SELECT_OVERLAY _FBIOC(0x000a)  /* Выбрать наложение */
/* Argument: только для чтения
 *           беззнаковый длинный */
#  define FBIOSET_TRANSP      _FBIOC(0x000b)  /* Установите непрозрачность или прозрачность
                                               * Argument: структура только для чтения
                                               *           fb_overlayinfo_s */
#  define FBIOSET_CHROMAKEY   _FBIOC(0x000c)  /* Установить хроматический ключ
                                               * Argument: структура только для чтения
                                               *           fb_overlayinfo_s */
#  define FBIOSET_COLOR       _FBIOC(0x000d)  /* Установить цвет
                                               * Argument: структура только для чтения
                                               *           fb_overlayinfo_s */
#  define FBIOSET_BLANK       _FBIOC(0x000e)  /* Пустой или непустой
                                               * Argument: структура только для чтения
                                               *           fb_overlayinfo_s */
#  define FBIOSET_AREA        _FBIOC(0x000f)  /* Установить активную область наложения
                                               * Argument: структура только для чтения
                                               *           fb_overlayinfo_s */
#  define FBIOSET_DESTAREA    _FBIOC(0x0010)  /* Включить зону назначения
                                               * первичный FB .
                                               * Argument: структура только для чтения
                                               *           fb_overlayinfo_s */

#ifdef CONFIG_FB_OVERLAY_BLIT
#  define FBIOSET_BLIT        _FBIOC(0x0011)  /* Закрашенная область между наложениями
                                               * Argument: структура только для чтения
                                               *           fb_overlayblit_s */
#  define FBIOSET_BLEND       _FBIOC(0x0012)  /* Область смешивания между наложениями
                                               * Argument: структура только для чтения
                                               *           fb_overlayblend_s */
#endif

#define FBIOPAN_OVERLAY       _FBIOC(0x0013)  /* Панорамирование дисплея для наложения
                                               * Argument: структура только для чтения
                                               *           fb_overlayinfo_s */

#endif /* CONFIG_FB_OVERLAY */

/* Специальные элементы управления *****************************************************/

#define FBIOSET_POWER         _FBIOC(0x0014)  /* Установить мощность панели
                                               * Argument:             интервал */
#define FBIOGET_POWER         _FBIOC(0x0015)  /* Получить текущую мощность панели
                                               * Argument:            интервал* */
#define FBIOSET_FRAMERATE     _FBIOC(0x0016)  /* Установить частоту кадров
                                               * Argument:             интервал */
#define FBIOGET_FRAMERATE     _FBIOC(0x0017)  /* Получить частоту кадров
                                               * Argument:            интервал* */

#define FBIOPAN_DISPLAY       _FBIOC(0x0018)  /* Панорамирование дисплея
                                               * Argument: структура только для чтения
                                               *           fb_planeinfo_s * */

#define FBIOPAN_CLEAR         _FBIOC(0x0019)  /* Очистить панораму */
/* Argument: только для чтения
 *           беззнаковый длинный */

#define FBIOSET_VSYNCOFFSET   _FBIOC(0x001a)  /* Установить используемое размещение VSync
                                               * Argument:             интервал */

/* Linux ********************************************************** Поддержка*/

#define FBIOGET_VSCREENINFO   _FBIOC(0x001b)  /* Получить информацию о видеопеременных */
/* Argument: записываемая структура
 *           fb_var_screeninfo */
#define FBIOGET_FSCREENINFO   _FBIOC(0x001c)  /* Получить информацию об исправлении видео */
/* Argument: записываемая структура
 *           fb_fix_screeninfo */

/**********************
 *      TYPEDEFS
 **********************/

typedef uint16_t fb_coord_t;

struct fb_videoinfo_s {
    uint8_t    fmt;               /* см. FB_FMT_ *  */
    fb_coord_t xres;              /* Горизонтальное разрешение в столбцах пикселей */
    fb_coord_t yres;              /* Вертикальное разрешение в строках пикселей */
    uint8_t    nplanes;           /* Количество поддерживаемых цветовых плоскостей */
    uint8_t    noverlays;         /* Количество поддерживаемых наложений */
    uint8_t    moduleinfo[128];   /* Информация о модуле заполняется поставщиком */
};

struct fb_planeinfo_s {
    void * fbmem;            /* Начало памяти кадрового буфера */
    size_t     fblen;        /* Длина памяти кадрового буфера в байтах */
    fb_coord_t stride;       /* Длина строки в байтах */
    uint8_t    display;      /* Номер дисплея */
    uint8_t    bpp;          /* Биты на пиксель */
    uint32_t   xres_virtual; /* Виртуальное горизонтальное разрешение в столбцах пикселей */
    uint32_t   yres_virtual; /* Виртуальное вертикальное разрешение в строках пикселей */
    uint32_t   xoffset;      /* Смещение от виртуального к видимому разрешению */
    uint32_t   yoffset;      /* Смещение от виртуального к видимому разрешению */
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_NUTTX_VIDEO_FB_H*/
