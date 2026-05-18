/*----------------------------------------------------------------------------/
/ TJpgDec - TinyJPEGДекомпрессорR0.03, включаемый файл (C)ChaN, 2021 г.
/----------------------------------------------------------------------------*/
#ifndef DEF_TJPGDEC
#define DEF_TJPGDEC

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_conf_internal.h"
#include "tjpgdcnf.h"

#if LV_USE_TJPGD

#include <string.h>
#include <stdint.h>

#if JD_FASTDECODE >= 1
typedef int16_t jd_yuv_t;
#else
typedef uint8_t jd_yuv_t;
#endif


/* Код ошибки */
typedef enum {
    JDR_OK = 0, /* 0: Удалось */
    JDR_INTR,   /* 1: Прервано функцией вывода */
    JDR_INP,    /* 2: Ошибка устройства или неправильное завершение входного потока */
    JDR_MEM1,   /* 3: Недостаточно памяти для изображения */
    JDR_MEM2,   /* 4: Недостаточно входного буфера потока */
    JDR_PAR,    /* 5: Ошибка параметра */
    JDR_FMT1,   /* 6: Ошибка формата данных (могут быть повреждены данные) */
    JDR_FMT2,   /* 7: Правильный формат, но не поддерживается */
    JDR_FMT3    /* 8: Не поддерживается стандарт JPEG */
} JRESULT;



/* Прямоугольная область выходного изображения */
typedef struct {
    uint16_t left;      /* Левый конец */
    uint16_t right;     /* Правый конец */
    uint16_t top;       /* Верхний конец */
    uint16_t bottom;    /* Нижний конец */
} JRECT;



/* Структура объекта декомпрессора */
typedef struct JDEC JDEC;
struct JDEC {
    size_t dctr;                /* Количество байтов, доступных во входном буфере */
    uint8_t * dptr;             /* Текущий параметр чтения данных */
    uint8_t * inbuf;            /* Входной буфер битового потока */
    uint8_t dbit;               /* Количество битов, доступное в wreg или битовой маске чтения */
    uint8_t scale;              /* Коэффициент масштабирования вывода */
    uint8_t msx, msy;           /* Размер MCU в единицах блока (ширина, высота) */
    uint8_t qtid[3];            /* Таблица квантования ID каждого компонента, Y, Cb, Cr */
    uint8_t ncomp;              /* Количество цветовых компонентов 1: оттенки серого, 3: цвет */
    int16_t dcv[3];             /* Предыдущий элемент DC каждого компонента */
    uint16_t nrst;              /* Интервал перезапуска */
    uint16_t rst;              /* Количество перезапусков*/
    uint16_t rsc;               /* Ожидаемая последовательность перезапуска ID*/
    uint16_t width, height;     /* Размер входного изображения (пиксель) */
    uint8_t * huffbits[2][2];   /* Таблицы распределения битов Хаффмана [id][dcac] */
    uint16_t * huffcode[2][2];  /* Таблицы кодовых слов Хаффмана [id][dcac] */
    uint8_t * huffdata[2][2];   /* Таблицы декодированных данных Хаффмана [id][dcac] */
    int32_t * qttbl[4];         /* Таблицы деквантайзера [id] */
#if JD_FASTDECODE >= 1
    uint32_t wreg;              /* Регистр рабочей смены */
    uint8_t marker;             /* Обнаруженный маркер (0:Нет) */
#if JD_FASTDECODE == 2
    uint8_t longofs[2][2];      /* Размещение таблицы длинного кода [id][dcac] */
    uint16_t * hufflut_ac[2];   /* Таблицы быстрого декодирования Хаффмана для короткого кода AC [id] */
    uint8_t * hufflut_dc[2];    /* Таблицы быстрого декодирования Хаффмана для короткого кода DC [id] */
#endif
#endif
    void * workbuf;             /* Рабочий буфер для вывода IDCT и RGB */
    jd_yuv_t * mcubuf;          /* Рабочий буфер для MCU */
    void * pool;                /* Указатель на доступный пул памяти */
    void * pool_original;       /* Указатель на исходный пул */
    size_t sz_pool;             /* Размер пула памяти (доступно в байтах) */
    size_t (*infunc)(JDEC *, uint8_t *, size_t); /* Указатель функции ввода потока jpeg */
    void * device;              /* Указатель на идентификатор устройства ввода-вывода для сеанса */
};



/* Функции TJpgDec API */
JRESULT jd_prepare(JDEC * jd, size_t (*infunc)(JDEC *, uint8_t *, size_t), void * pool, size_t sz_pool, void * dev);

JRESULT jd_decomp(JDEC * jd, int (*outfunc)(JDEC *, void *, JRECT *), uint8_t scale);

JRESULT jd_mcu_load(JDEC * jd);

JRESULT jd_mcu_output(JDEC * jd, int (*outfunc)(JDEC *, void *, JRECT *), unsigned int x, unsigned int y);

JRESULT jd_restart(JDEC * jd, uint16_t rstn);

#endif

#ifdef __cplusplus
}
#endif

#endif /* _TJPGDEC */
