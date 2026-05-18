// Copyright 2020 BitBank Software, Inc. All Rights Reserved.
// Лицензия Apache версии 2.0 («Лицензия»);
// вы не можете использовать этот файл, кроме как в соответствии с Лицензией.
// Вы можете получить копию Лицензии по адресу
//    http://www.apache.org/licenses/LICENSE-2.0
// Если это не требуется действующим законодательством или не согласовано в письменной форме, программное обеспечение
// распространяется по Лицензии, распространяется на «AS IS» BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND , явный или подразумеваемый.
// См. Лицензию для конкретного языка, регулирующего разрешения и
// ограничения по Лицензии.
//===========================================================================

#ifndef __ANIMATEDGIF__
#define __ANIMATEDGIF__

#include "../../lv_conf_internal.h"

#if LV_USE_GIF

#include "../../misc/lv_fs.h"

//
// GIF Аниматор
// Автор Ларри Бэнк
// Copyright (c) 2020 BitBank Software, Inc.
// битбанк @pobox .com
//
// Предназначен для декодирования изображений размером до 480х320 на микроконтроллерах.
// использование менее 22 КБ RAM
// ...и декодировать изображение любого размера, когда будет доступно больше RAM
//
// ** NEW **
// Турбо-режим добавлен 18 февраля 2024 г. Этот параметр декодирует изображения.
// до 30 раз быстрее, если достаточно RAM (48K + полный кадровый буфер)
//

/* GIF Определения и переменные */
#define MAX_CHUNK_SIZE 255
//
// Эти 2 макроса можно изменить, чтобы ограничить количество RAM.
// требуется декодером. Например, декодирование 1-битных изображений в
// для дисплея 128x32 не потребуется ни максимальный размер кода 12, ни палитра.
// с 256 записями
//
#define TURBO_BUFFER_SIZE 0x6100

// Если вы собираетесь декодировать общие GIF-файлы, вам нужно, чтобы это значение было равно 12. Если вы используете GIF-файлы исключительно для анимации в
// ваш собственный проект и вы управляете GIF-файлами, которые собираетесь воспроизвести, тогда вы можете сохранить дополнительные RAM здесь:
// декодер должен зарезервировать минимум 4 байта * (1<< MAX_CODE_SIZE ) для словаря, но в зависимости от реализации
// фактически резервирует 5 байт * (1<< MAX_CODE_SIZE ). Небольшие или низкоцветные GIF-файлы по своей природе могут не требовать большого
// словарь. Для больших GIF-файлов кодировщик(!) может «добровольно» отказаться от использования всего словаря. Т.е.,
// подготовив (специально закодировав) GIF-файлы, вы сможете сохранить >10 КБ RAM , но не сможете декодировать произвольно
// изображений больше нет. Одно из приложений для создания таких GIF-файлов можно найти здесь (используйте опцию -d).
// https://create.stephan-brumme.com/flexigif-lossless-gif-lzw-optimization/
#define MAX_CODE_SIZE 12

#define MAX_COLORS 256
#define MAX_WIDTH 480
#define LZW_BUF_SIZE (6*MAX_CHUNK_SIZE)
#define LZW_HIGHWATER (4*MAX_CHUNK_SIZE)
// Этот буфер используется для хранения последовательности пикселей в обратном порядке.
// он должен быть достаточно большим, чтобы удерживать как можно дольше
// последовательность (1<< MAX_CODE_SIZE )
#define FILE_BUF_SIZE (1<<MAX_CODE_SIZE)

#define PIXEL_FIRST 0
#define PIXEL_LAST (1<<MAX_CODE_SIZE)
#define LINK_UNUSED 5911 // 0x1717 для использования memset
#define LINK_END 5912
#define MAX_HASH 5003
// расширенный буфер LZW для режима Turbo
#define LZW_BUF_SIZE_TURBO (LZW_BUF_SIZE + (2<<MAX_CODE_SIZE) + (PIXEL_LAST*2) + MAX_WIDTH)
#define LZW_HIGHWATER_TURBO ((LZW_BUF_SIZE_TURBO * 14) / 16)

//
// Типы пикселей
//
enum {
   GIF_PALETTE_RGB565_LE = 0, // с прямым порядком байтов (по умолчанию)
   GIF_PALETTE_RGB565_BE,     // с прямым порядком байтов
   GIF_PALETTE_RGB888,        // исходные записи в формате 24 бит/с
   GIF_PALETTE_RGB8888,       // 32-битный (альфа = 0xff или 0x00 )
   GIF_PALETTE_1BPP,          // 1 бит на пиксель (по горизонтали, MSB слева)
   GIF_PALETTE_1BPP_OLED      // 1 бит на пиксель (вертикально, LSB сверху)
};
// для совместимости со старым кодом
#define LITTLE_ENDIAN_PIXELS GIF_PALETTE_RGB565_LE
#define BIG_ENDIAN_PIXELS GIF_PALETTE_RGB565_BE
//
// Типы рисования
//
// RAW = 8-bit palettized pixels requiring transparent pixel handling and conversion through the palette.
//       Каждая строка отправляется обратному вызову GIFDraw в виде 8-битных пикселей. Если кадровый буфер существует, строки будут
//       там тоже написано. Обратный вызов GIFDraw не является обязательным, если выделен кадровый буфер.
//
// COOKED = 16/24/32-bpp fully rendered pixels ready for display. This requires a full frame buffer with extra
//          место для полностью визуализированных пикселей в конце 8-битного пиксельного буфера. Например, 160х120.
//          Размер холста с 24-битным выводом потребует (160*120 + 3*160) байт.
//          Каждая подготовленная строка отправляется обратному вызову GIFDraw как строка из 16/24/32-битных пикселей.
//
enum {
   GIF_DRAW_RAW = 0,
   GIF_DRAW_COOKED
};

enum {
   GIF_SUCCESS = 0,
   GIF_DECODE_ERROR,
   GIF_TOO_WIDE,
   GIF_INVALID_PARAMETER,
   GIF_UNSUPPORTED_FEATURE,
   GIF_FILE_NOT_OPEN,
   GIF_EARLY_EOF,
   GIF_EMPTY_FRAME,
   GIF_BAD_FILE,
   GIF_ERROR_MEMORY
};

typedef struct gif_file_tag
{
  int32_t iPos; // текущая позиция файла
  int32_t iSize; // размер файла
  uint8_t *pData; // указатель файла памяти
  lv_fs_file_t fHandle; // указатель класса на File/SdFat или что угодно
} GIFFILE;

typedef struct gif_info_tag
{
  int32_t iFrameCount; // общее количество кадров в файле
  int32_t iDuration; // продолжительность анимации в миллисекундах
  int32_t iMaxDelay; // максимальная задержка кадра
  int32_t iMinDelay; // минимальная задержка кадра
} GIFINFO;

typedef struct gif_draw_tag
{
    int iX, iY; // Угловое смещение этого кадра на холсте
    int y; // рисуется текущая линия (0 = верхняя строка изображения)
    int iWidth, iHeight; // размер этой рамки
    int iCanvasWidth; // это нужно, чтобы знать, куда поместить вывод в полностью готовом растровом изображении.
    void *pUser; // предоставленный пользователем указатель
    uint8_t *pPixels; // 8-битные исходные пиксели для этой строки
    uint16_t *pPalette; // Записи палитры RGB565 с прямым порядком байтов или прямым порядком байтов (по умолчанию)
    uint8_t *pPalette24; // Палитра RGB888 (опционально)
    uint8_t ucTransparent; // прозрачный цвет
    uint8_t ucHasTransparency; // флаг, указывающий, что используется прозрачный цвет
    uint8_t ucDisposalMethod; // метод утилизации кадра
    uint8_t ucBackground; // цвет фона
    uint8_t ucPaletteType; // тип записей палитры
    uint8_t ucIsGlobalPalette; // Флаг, указывающий, что используется глобальная палитра, а не локальная палитра.
} GIFDRAW;

// Прототипы функций обратного вызова
typedef int32_t (GIF_READ_CALLBACK)(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen);
typedef int32_t (GIF_SEEK_CALLBACK)(GIFFILE *pFile, int32_t iPosition);
typedef void (GIF_DRAW_CALLBACK)(GIFDRAW *pDraw);
typedef void * (GIF_OPEN_CALLBACK)(const char *szFilename, int32_t *pFileSize);
typedef void (GIF_CLOSE_CALLBACK)(lv_fs_file_t *pHandle);
typedef void * (GIF_ALLOC_CALLBACK)(uint32_t iSize);
typedef void (GIF_FREE_CALLBACK)(void *buffer);
//
// наша частная структура для хранения состояния декодирования изображения GIF
//
typedef struct gif_image_tag
{
    uint16_t iWidth, iHeight, iCanvasWidth, iCanvasHeight;
    uint16_t iX, iY; // GIF угловое смещение
    uint16_t iBpp;
    int16_t iError; // последняя ошибка
    uint16_t iFrameDelay; // задержка в миллисекундах для этого кадра
    int16_t iRepeatCount; // NETSCAPE счетчик повторений анимации. 0 = навсегда
    uint16_t iXCount, iYCount; // позиция декодирования на изображении (значения обратного отсчета)
    int iLZWOff; // текущее смещение данных LZW
    int iLZWSize; // текущий объем данных в буфере LZW
    int iCommentPos; // смещение файла начала данных комментария
    short sCommentLen; // длина комментария
    unsigned char bEndOfFrame;
    unsigned char ucGIFBits, ucBackground, ucTransparent, ucCodeStart, ucMap, bUseLocalPalette;
    unsigned char ucPaletteType; // RGB565 or RGB888
    unsigned char ucDrawType; // RAW or COOKED
    GIF_READ_CALLBACK *pfnRead;
    GIF_SEEK_CALLBACK *pfnSeek;
    GIF_DRAW_CALLBACK *pfnDraw;
    GIF_OPEN_CALLBACK *pfnOpen;
    GIF_CLOSE_CALLBACK *pfnClose;
    GIFFILE GIFFile;
    void *pUser;
    unsigned char *pFrameBuffer;
    unsigned char *pTurboBuffer;
    unsigned char *pPixels, *pOldPixels;
    unsigned char ucFileBuf[FILE_BUF_SIZE]; // хранит временные данные и стек пикселей
    unsigned short pPalette[(MAX_COLORS * 3)/2]; // может содержать RGB565 или RGB888 — устанавливается в begin()
    unsigned short pLocalPalette[(MAX_COLORS * 3)/2]; // цветовые палитры для изображений GIF
    unsigned char ucLZW[LZW_BUF_SIZE]; // содержит разделенные на части данные LZW
    // Следующие 3 используются в режиме Turbo для увеличения буфера ucLZW.
    unsigned short usGIFTable[1<<MAX_CODE_SIZE];
    unsigned char ucGIFPixels[(PIXEL_LAST*2)];
    unsigned char ucLineBuf[MAX_WIDTH]; // текущая строка
} GIFIMAGE;

// Интерфейс C
    int GIF_openRAM(GIFIMAGE *pGIF, uint8_t *pData, int iDataSize, GIF_DRAW_CALLBACK *pfnDraw);
    int GIF_openFile(GIFIMAGE *pGIF, const char *szFilename, GIF_DRAW_CALLBACK *pfnDraw);
    void GIF_close(GIFIMAGE *pGIF);
    void GIF_begin(GIFIMAGE *pGIF, unsigned char ucPaletteType);
    void GIF_reset(GIFIMAGE *pGIF);
    int GIF_playFrame(GIFIMAGE *pGIF, int *delayMilliseconds, void *pUser);
    int GIF_getCanvasWidth(GIFIMAGE *pGIF);
    int GIF_getCanvasHeight(GIFIMAGE *pGIF);
    int GIF_getComment(GIFIMAGE *pGIF, char *destBuffer);
    int GIF_getInfo(GIFIMAGE *pGIF, GIFINFO *pInfo);
    int GIF_getLastError(GIFIMAGE *pGIF);
    int GIF_getLoopCount(GIFIMAGE *pGIF);

#define REGISTER_WIDTH 32
#ifdef ALLOWS_UNALIGNED
    #define INTELSHORT(p) (*(uint16_t *)p)
    #define INTELLONG(p) (*(uint32_t *)p)
#else
    // Из-за того, что невыровненная память вызывает исключение, нам приходится выполнять эти макросы медленно.
    #define INTELSHORT(p) ((*p) + (*(p+1)<<8))
    #define INTELLONG(p) ((*p) + (*(p+1)<<8) + (*(p+2)<<16) + (*(p+3)<<24))
#endif // ALLOWS_UNALIGNED
#define BIGINT int32_t
#define BIGUINT uint32_t

#endif // LV_USE_GIF

#endif // __ANIMATEDGIF__
