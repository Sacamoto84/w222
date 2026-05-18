//
// GIF Аниматор
// автор Ларри Бэнк
// битбанк@pobox.com
// Порт Arduino запущен 05.07.2020
// Оригинальный код GIF, написанный более 20 лет назад :)
// Цель этого кода — декодировать изображения размером до 480x320.
// использование не более 22 КБ RAM (при отправке непосредственно на дисплей LCD)
//
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
#include "AnimatedGIF.h"

#if LV_USE_GIF

#include "../../misc/lv_log.h"
#include "../../stdlib/lv_string.h"

static const unsigned char cGIFBits[9] = {1,4,4,4,8,8,8,8,8}; // преобразовать нечетные значения bpp в те, которые мы можем обработать

// прямые ссылки
static int GIFInit(GIFIMAGE *pGIF);
static int GIFParseInfo(GIFIMAGE *pPage, int bInfoOnly);
static int GIFGetMoreData(GIFIMAGE *pPage);
static void GIFMakePels(GIFIMAGE *pPage, unsigned int code);
static int DecodeLZW(GIFIMAGE *pImage, int iOptions);
static int DecodeLZWTurbo(GIFIMAGE *pImage, int iOptions);
static int32_t readMem(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen);
static int32_t seekMem(GIFFILE *pFile, int32_t iPosition);
int GIF_getInfo(GIFIMAGE *pPage, GIFINFO *pInfo);
static int32_t readFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen);
static int32_t seekFile(GIFFILE *pFile, int32_t iPosition);
static void closeFile(lv_fs_file_t *handle);

// C API
int GIF_openRAM(GIFIMAGE *pGIF, uint8_t *pData, int iDataSize, GIF_DRAW_CALLBACK *pfnDraw)
{
    pGIF->iError = GIF_SUCCESS;
    pGIF->pfnRead = readMem;
    pGIF->pfnSeek = seekMem;
    pGIF->pfnDraw = pfnDraw;
    pGIF->pfnOpen = NULL;
    pGIF->pfnClose = NULL;
    pGIF->GIFFile.iSize = iDataSize;
    pGIF->GIFFile.pData = pData;
    return GIFInit(pGIF);
} /* GIF_openRAM () */

int GIF_openFile(GIFIMAGE *pGIF, const char *szFilename, GIF_DRAW_CALLBACK *pfnDraw)
{
    pGIF->iError = GIF_SUCCESS;
    pGIF->pfnRead = readFile;
    pGIF->pfnSeek = seekFile;
    pGIF->pfnDraw = pfnDraw;
    pGIF->pfnOpen = NULL;
    pGIF->pfnClose = closeFile;

    lv_fs_res_t res = lv_fs_open(&pGIF->GIFFile.fHandle, szFilename, LV_FS_MODE_RD);

    if (res != LV_FS_RES_OK) {
        pGIF->iError = GIF_FILE_NOT_OPEN;
        LV_LOG_WARN("Failed to open file: %s, res: %d", szFilename, res);
        return 0;
    }

    lv_fs_seek(&pGIF->GIFFile.fHandle, 0, LV_FS_SEEK_END);
    uint32_t pos;
    lv_fs_tell(&pGIF->GIFFile.fHandle, &pos);
    pGIF->GIFFile.iSize = pos;
    lv_fs_seek(&pGIF->GIFFile.fHandle, 0, LV_FS_SEEK_SET);

    int ret = GIFInit(pGIF);
    if (!ret) {
        LV_LOG_WARN("Failed to initialize GIF from file: %s, err: %d", szFilename, pGIF->iError);
        lv_fs_close(&pGIF->GIFFile.fHandle);
    }

    return ret;
} /* GIF_openFile () */

void GIF_close(GIFIMAGE *pGIF)
{
    if (pGIF->pfnClose)
        (*pGIF->pfnClose)(&pGIF->GIFFile.fHandle);
} /* GIF_close () */

void GIF_begin(GIFIMAGE *pGIF, unsigned char ucPaletteType)
{
    lv_memset(pGIF, 0, sizeof(GIFIMAGE));
    pGIF->ucPaletteType = ucPaletteType;
} /* GIF_begin () */

void GIF_reset(GIFIMAGE *pGIF)
{
    (*pGIF->pfnSeek)(&pGIF->GIFFile, 0);
} /* GIF_reset () */

//
// Возвращаемое значение:
// 1 = хорошее декодирование, существует больше кадров
// 0 = хорошее декодирование, больше нет кадров
// -1 = ошибка
//
int GIF_playFrame(GIFIMAGE *pGIF, int *delayMilliseconds, void *pUser)
{
int rc;

    if (delayMilliseconds)
       *delayMilliseconds = 0; // очистить все старые действительные
    if (pGIF->GIFFile.iPos >= pGIF->GIFFile.iSize-1) // больше данных не существует
    {
        (*pGIF->pfnSeek)(&pGIF->GIFFile, 0); // попытаться начать
    }
    if (GIFParseInfo(pGIF, 0))
    {
        pGIF->pUser = pUser;
        if (pGIF->iError == GIF_EMPTY_FRAME) // не пытайся расшифровать это
            return 0;
        if (pGIF->pTurboBuffer) { // наличие турбобуфера указывает на режим турбо
            rc = DecodeLZWTurbo(pGIF, 0);
        } else {
            rc = DecodeLZW(pGIF, 0);
        }
        if (rc != 0) // проблема
            return 0;
    }
    else
    {
        return 0; // ошибка при анализе информации о кадре, возможно, мы находимся в конце файла
    }
    // Верните 1 для большего количества кадров или 0, если это был последний кадр.
    if (delayMilliseconds) // если не NULL, верните время задержки кадра
        *delayMilliseconds = pGIF->iFrameDelay;
    return (pGIF->GIFFile.iPos < pGIF->GIFFile.iSize-1);
} /* GIF_playFrame () */

int GIF_getCanvasWidth(GIFIMAGE *pGIF)
{
    return pGIF->iCanvasWidth;
} /* GIF_getCanvasWidth () */

int GIF_getCanvasHeight(GIFIMAGE *pGIF)
{
    return pGIF->iCanvasHeight;
} /* GIF_getCanvasHeight () */

int GIF_getLoopCount(GIFIMAGE *pGIF)
{
    return pGIF->iRepeatCount;
} /* GIF_getLoopCount () */

int GIF_getComment(GIFIMAGE *pGIF, char *pDest)
{
int32_t iOldPos;

    iOldPos = pGIF->GIFFile.iPos; // сохранять старую позицию
    (*pGIF->pfnSeek)(&pGIF->GIFFile, pGIF->iCommentPos);
    (*pGIF->pfnRead)(&pGIF->GIFFile, (uint8_t *)pDest, pGIF->sCommentLen);
    (*pGIF->pfnSeek)(&pGIF->GIFFile, iOldPos);
    pDest[pGIF->sCommentLen] = 0; // ноль завершает строку
    return (int)pGIF->sCommentLen;

} /* GIF_getComment () */

int GIF_getLastError(GIFIMAGE *pGIF)
{
    return pGIF->iError;
} /* GIF_getLastError () */

//
// Вспомогательные функции для изображений из памяти
//
static int32_t readMem(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
    int32_t iBytesRead;

    iBytesRead = iLen;
    if ((pFile->iSize - pFile->iPos) < iLen)
       iBytesRead = pFile->iSize - pFile->iPos;
    if (iBytesRead <= 0)
       return 0;
    lv_memmove(pBuf, &pFile->pData[pFile->iPos], iBytesRead);
    pFile->iPos += iBytesRead;
    return iBytesRead;
} /* readMem() */

static int32_t seekMem(GIFFILE *pFile, int32_t iPosition)
{
    if (iPosition < 0) iPosition = 0;
    else if (iPosition >= pFile->iSize) iPosition = pFile->iSize-1;
    pFile->iPos = iPosition;
    return iPosition;
} /* seekMem() */

static void closeFile(lv_fs_file_t *handle)
{
    lv_fs_close(handle);
} /* closeFile() */

static int32_t seekFile(GIFFILE *pFile, int32_t iPosition)
{
    if (iPosition < 0) iPosition = 0;
    else if (iPosition >= pFile->iSize) iPosition = pFile->iSize-1;
    pFile->iPos = iPosition;
    lv_fs_seek(&pFile->fHandle, iPosition, LV_FS_SEEK_SET);
    return iPosition;
} /* seekMem() */

static int32_t readFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
    int32_t iBytesRead;

    iBytesRead = iLen;
    if ((pFile->iSize - pFile->iPos) < iLen)
       iBytesRead = pFile->iSize - pFile->iPos;
    if (iBytesRead <= 0)
       return 0;
    uint32_t br;
    lv_fs_read(&pFile->fHandle, pBuf, iBytesRead, &br);
    iBytesRead = br;
    pFile->iPos += iBytesRead;
    return iBytesRead;
} /* readFile() */

//
// Следующие функции написаны на простом языке C и не имеют
// Сторонние зависимости, даже не библиотека времени выполнения C.
//
//
// Инициализация файла GIF и обратный вызов из файла на SD или в памяти.
// возвращает 1 в случае успеха, 0 в случае неудачи
// Заполняет размер холста структуры GIFIMAGE.
//
static int GIFInit(GIFIMAGE *pGIF)
{
    pGIF->GIFFile.iPos = 0; // начинать с начала файла
    if (!GIFParseInfo(pGIF, 1)) // собрать информацию для первого кадра
       return 0; // something went wrong; not a GIF file?
    (*pGIF->pfnSeek)(&pGIF->GIFFile, 0); // вернуться к началу файла
    if (pGIF->iCanvasWidth > MAX_WIDTH || pGIF->iCanvasHeight > 32767) { // слишком большой или коррумпированный
        pGIF->iError = GIF_TOO_WIDE;
        return 0;
    }
  return 1;
} /* GIFInit() */

//
// Разберите заголовок GIF, соберите информацию о размере и палитре.
// Если возникает параметр bInfoOnly, установленный в true, он проверяет достоверность файла.
// и вернуть только размер холста
// Возвращает 1 в случае успеха, 0 в случае неудачи.
//
static int GIFParseInfo(GIFIMAGE *pPage, int bInfoOnly)
{
    int i, j, iColorTableBits;
    int iBytesRead;
    unsigned char c, *p;
    int32_t iOffset = 0;
    int32_t iStartPos = pPage->GIFFile.iPos; // начальная позиция файла
    int iReadSize;

    pPage->bUseLocalPalette = 0; // предполагать отсутствие локальной палитры
    pPage->bEndOfFrame = 0; // мы только начинаем
    pPage->iFrameDelay = 0; // не может блокировать расширение GFX
    pPage->iRepeatCount = -1; // предположим, что количество циклов NETSCAPE не указано
    iReadSize = MAX_CHUNK_SIZE;
    // Если вы попытаетесь прочитать EOF, библиотека SD вернет мусорные данные.
    if (iStartPos + iReadSize > pPage->GIFFile.iSize)
       iReadSize = (pPage->GIFFile.iSize - iStartPos - 1);
    p = pPage->ucFileBuf;
    iBytesRead =  (*pPage->pfnRead)(&pPage->GIFFile, pPage->ucFileBuf, iReadSize); // 255 на данный момент достаточно

    if (iBytesRead != iReadSize) // мы в конце файла
    {
       pPage->iError = GIF_EARLY_EOF;
       return 0;
    }
    if (iStartPos == 0) // начало файла
    { // размер холста
        if (lv_memcmp(p, "GIF89", 5) != 0 && lv_memcmp(p, "GIF87", 5) != 0) // не файл GIF
        {
           pPage->iError = GIF_BAD_FILE;
           return 0;
        }
        pPage->iCanvasWidth = pPage->iWidth = INTELSHORT(&p[6]);
        pPage->iCanvasHeight = pPage->iHeight = INTELSHORT(&p[8]);
        pPage->iBpp = ((p[10] & 0x70) >> 4) + 1;
        iColorTableBits = (p[10] & 7) + 1; // Таблицы цветов Log2 (размер)
        pPage->ucBackground = p[11]; // цвет фона
        pPage->ucGIFBits = 0;
        iOffset = 13;
        if (p[10] & 0x80) // global color table?
        { // по умолчанию конвертировать в перевернутый байт RGB565 для немедленного использования
            // Прочтите достаточно дополнительных данных для таблицы цветов.
            iBytesRead += (*pPage->pfnRead)(&pPage->GIFFile, &pPage->ucFileBuf[iBytesRead], 3*(1<<iColorTableBits));
            if (pPage->ucPaletteType == GIF_PALETTE_RGB565_LE || pPage->ucPaletteType == GIF_PALETTE_RGB565_BE) {
                for (i=0; i<(1<<iColorTableBits); i++) {
                    uint16_t usRGB565;
                    usRGB565 = ((p[iOffset] >> 3) << 11); // R
                    usRGB565 |= ((p[iOffset+1] >> 2) << 5); // G
                    usRGB565 |= (p[iOffset+2] >> 3); // B
                    if (pPage->ucPaletteType == GIF_PALETTE_RGB565_LE)
                        pPage->pPalette[i] = usRGB565;
                    else
                        pPage->pPalette[i] = (usRGB565 << 8) | (usRGB565 >> 8); // SPI сначала хочет MSB
                    iOffset += 3;
                }
            } else if (pPage->ucPaletteType == GIF_PALETTE_1BPP || pPage->ucPaletteType == GIF_PALETTE_1BPP_OLED) {
                uint8_t *pPal1 = (uint8_t*)pPage->pPalette;
                for (i=0; i<(1<<iColorTableBits); i++) {
                    uint16_t usGray;
                    usGray = p[iOffset]; // R
                    usGray += p[iOffset+1]*2; // G в два раза важнее
                    usGray += p[iOffset+2]; // B
                    pPal1[i] = (usGray >= 512); // достаточно яркий = 1
                    iOffset += 3;
                }
            } else { // просто скопируйте его как есть (выходные данные RGB888 и RGB8888)
                lv_memcpy(pPage->pPalette, &p[iOffset], (1<<iColorTableBits) * 3);
                iOffset += (1 << iColorTableBits) * 3;
            }
        }
    }
    while (p[iOffset] != ',' && p[iOffset] != ';') /* Дождитесь разделителя изображений */
    {
        if (p[iOffset] == '!') /* Блок расширения */
        {
            iOffset++;
            switch(p[iOffset++]) /* Тип блока */
            {
                case 0xf9: /* Графическое расширение */
                    if (p[iOffset] == 4) // правильная длина
                    {
                        pPage->ucGIFBits = p[iOffset+1]; // упакованные поля
                        pPage->iFrameDelay = (INTELSHORT(&p[iOffset+2]))*10; // задержка в мс
                        if (pPage->iFrameDelay <= 1) // 0-1 заставит его работать со скоростью 60 кадров в секунду; используйте 100 (10 кадров в секунду) в качестве разумной замены
                           pPage->iFrameDelay = 100;
                        if (pPage->ucGIFBits & 1) // используется прозрачный цвет
                            pPage->ucTransparent = p[iOffset+4]; // прозрачный индекс цвета
                        iOffset += 6;
                    }
                    //                     иначе // ошибка
                    break;
                case 0xff: /* Расширение приложения */
                    c = 1;
                    while (c) /* Пропустить все подблоки данных */
                    {
                        c = p[iOffset++]; /* Длина блока */
                        if ((iBytesRead - iOffset) < (c+32)) // сначала нужно прочитать больше данных
                        {
                            lv_memmove(pPage->ucFileBuf, &pPage->ucFileBuf[iOffset], (iBytesRead-iOffset)); // переместить существующие данные вниз
                            iBytesRead -= iOffset;
                            iStartPos += iOffset;
                            iOffset = 0;
                            iBytesRead += (*pPage->pfnRead)(&pPage->GIFFile, &pPage->ucFileBuf[iBytesRead], c+32);
                        }
                        if (c == 11) // фиксированная длина блока
                        { // Блок приложения Netscape содержит счетчик повторов.
                            if (lv_memcmp(&p[iOffset], "NETSCAPE2.0", 11) == 0)
                            {
                                if (p[iOffset+11] == 3 && p[iOffset+12] == 1) // количество циклов
                                    pPage->iRepeatCount = INTELSHORT(&p[iOffset+13]);
                            }
                        }
                        iOffset += (int)c; /* Перейти к следующему подблоку */
                    }
                    break;
                case 0x01: /* Текстовое расширение */
                    c = 1;
                    j = 0;
                    while (c) /* Пропустить все подблоки данных */
                    {
                        c = p[iOffset++]; /* Длина блока */
                        if (j == 0) // используйте только первый блок
                        {
                            j = c;
                            if (j > 127)   // максимальная длина комментария = 127
                                j = 127;
                            //                           memcpy(pPage->szInfo1, &p[iOffset], j);
                            //                           pPage->szInfo1[j] = '\0';
                            j = 1;
                        }
                        iOffset += (int)c; /* Пропустить этот подблок */
                    }
                    break;
                case 0xfe: /* Комментарий */
                    c = 1;
                    while (c) /* Пропустить все подблоки данных */
                    {
                        c = p[iOffset++]; /* Длина блока */
                        if ((iBytesRead - iOffset) < (c+32)) // сначала нужно прочитать больше данных
                        {
                            lv_memmove(pPage->ucFileBuf, &pPage->ucFileBuf[iOffset], (iBytesRead-iOffset)); // переместить существующие данные вниз
                            iBytesRead -= iOffset;
                            iStartPos += iOffset;
                            iOffset = 0;
                            iBytesRead += (*pPage->pfnRead)(&pPage->GIFFile, &pPage->ucFileBuf[iBytesRead], c+32);
                        }
                        if (pPage->iCommentPos == 0) // Сохраните информацию о первом блоке
                        {
                            pPage->iCommentPos = iStartPos + iOffset;
                            pPage->sCommentLen = c;
                        }
                        iOffset += (int)c; /* Пропустить этот подблок */
                    }
                    break;
                default:
                    /* Неверная информация в заголовке */
                    pPage->iError = GIF_DECODE_ERROR;
                    return 0;
            } /* переключатель */
        }
        else // неверный байт, прекратите декодирование
        {
            if (pPage->GIFFile.iSize - iStartPos < 32) // non-image bytes at end of file?
                pPage->iError = GIF_EMPTY_FRAME;
            else
                /* Неверная информация в заголовке */
                pPage->iError = GIF_DECODE_ERROR;
            return 0;
        }
    } /* пока */
    if (bInfoOnly)
       return 1; // у нас есть нужная информация, уходи
    if (p[iOffset] == ';') { // конец файла, выйдите и верните правильный код ошибки
        pPage->iError = GIF_EMPTY_FRAME;
        return 1;
    }

    if (p[iOffset] == ',')
        iOffset++;
    // Размер и положение этого конкретного кадра на основном кадре (если он анимирован)
    pPage->iX = INTELSHORT(&p[iOffset]);
    pPage->iY = INTELSHORT(&p[iOffset+2]);
    pPage->iWidth = INTELSHORT(&p[iOffset+4]);
    pPage->iHeight = INTELSHORT(&p[iOffset+6]);
    if (pPage->iWidth > pPage->iCanvasWidth || pPage->iHeight > pPage->iCanvasHeight ||
        pPage->iWidth + pPage->iX > pPage->iCanvasWidth || pPage->iHeight + pPage->iY > pPage->iCanvasHeight) {
        pPage->iError = GIF_DECODE_ERROR; // должен быть поврежденный файл, чтобы столкнуться с этой ошибкой здесь
        return 0;
    }
    iOffset += 8;

    /* Дескриптор изображения
     7 6 5 4 3 2 1 0 M=0 — использовать глобальную карту цветов, игнорировать пиксель
     M I 0 0 0 пиксель M=1 — следует локальная карта цветов, используйте пиксель
     I=0 — изображения в последовательном порядке
     I=1 — изображение в чересстрочном порядке
     пиксель+1 = # бит на пиксель для этого изображения
     */
    pPage->ucMap = p[iOffset++];
    if (pPage->ucMap & 0x80) // local color table?
    {// по умолчанию конвертировать в перевернутый байт RGB565 для немедленного использования
        j = (1<<((pPage->ucMap & 7)+1));
        // Прочтите достаточно дополнительных данных для таблицы цветов.
        iBytesRead += (*pPage->pfnRead)(&pPage->GIFFile, &pPage->ucFileBuf[iBytesRead], j*3);
        if (pPage->ucPaletteType == GIF_PALETTE_RGB565_LE || pPage->ucPaletteType == GIF_PALETTE_RGB565_BE)
        {
            for (i=0; i<j; i++)
            {
                uint16_t usRGB565;
                usRGB565 = ((p[iOffset] >> 3) << 11); // R
                usRGB565 |= ((p[iOffset+1] >> 2) << 5); // G
                usRGB565 |= (p[iOffset+2] >> 3); // B
                if (pPage->ucPaletteType == GIF_PALETTE_RGB565_LE)
                    pPage->pLocalPalette[i] = usRGB565;
                else
                    pPage->pLocalPalette[i] = (usRGB565 << 8) | (usRGB565 >> 8); // SPI сначала хочет MSB
                iOffset += 3;
            }
        } else if (pPage->ucPaletteType == GIF_PALETTE_1BPP || pPage->ucPaletteType == GIF_PALETTE_1BPP_OLED) {
            uint8_t *pPal1 = (uint8_t*)pPage->pLocalPalette;
            for (i=0; i<j; i++) {
                uint16_t usGray;
                usGray = p[iOffset]; // R
                usGray += p[iOffset+1]*2; // G в два раза важнее
                usGray += p[iOffset+2]; // B
                pPal1[i] = (usGray >= 512); // достаточно яркий = 1
                iOffset += 3;
            }
        } else { // просто скопируйте как есть
            lv_memcpy(pPage->pLocalPalette, &p[iOffset], j * 3);
            iOffset += j*3;
        }
        pPage->bUseLocalPalette = 1;
    }
    pPage->ucCodeStart = p[iOffset++]; /* начальный размер кода */
    /* Поскольку GIF может быть 1-8 бит на пиксель, мы допускаем только 1,4,8. */
    pPage->iBpp = cGIFBits[pPage->ucCodeStart];
    // мы повторно используем тот же буфер, превращая данные файла GIF
    // в «чистый» LZW
   pPage->iLZWSize = 0; // мы начинаем без данных LZW
   c = 1; // получить длину фрагмента
   while (c && iOffset < iBytesRead)
   {
//     Serial.printf("iOffset=%d, iBytesRead=%d\n", iOffset, iBytesRead);
     c = p[iOffset++]; // получить длину фрагмента
//     Serial.printf("Размер фрагмента = %d\n", c);
     if (c <= (iBytesRead - iOffset))
     {
       lv_memcpy(&pPage->ucLZW[pPage->iLZWSize], &p[iOffset], c);
       pPage->iLZWSize += c;
       iOffset += c;
     }
     else // частичный фрагмент в нашем буфере
     {
       int iPartialLen = (iBytesRead - iOffset);
       lv_memcpy(&pPage->ucLZW[pPage->iLZWSize], &p[iOffset], iPartialLen);
       pPage->iLZWSize += iPartialLen;
       iOffset += iPartialLen;
       (*pPage->pfnRead)(&pPage->GIFFile, &pPage->ucLZW[pPage->iLZWSize], c - iPartialLen);
       pPage->iLZWSize += (c - iPartialLen);
     }
     if (c == 0)
        pPage->bEndOfFrame = 1; // сигнал не читать дальше конца кадра
   }
// поиск на карте SD — это VERY VERY SLOW , поэтому используйте данные, которые мы уже прочитали, разбивая их на части.
// в данном случае данных слишком много, поэтому нам придется немного вернуться назад
   if (iOffset < iBytesRead)
   {
//     Serial.printf("Необходимо найти %d байт\n", iBytesRead - iOffset);
     (*pPage->pfnSeek)(&pPage->GIFFile, iStartPos + iOffset); // расположить файл на новом месте
   }
    return 1; // сейчас мы находимся в начале данных фрагмента
} /* GIFParseInfo() */
//
// Соберите информацию об анимированном файле GIF.
//
int GIF_getInfo(GIFIMAGE *pPage, GIFINFO *pInfo)
{
    int iOff, iNumFrames;
    int iDelay, iMaxDelay, iMinDelay, iTotalDelay;
    int iReadAmount;
    int iDataAvailable = 0;
    int iDataRemaining = 0;
 //   uint32_t lFileOff = 0;
    int bDone = 0;
    int bExt;
    uint8_t c, *cBuf;

    iMaxDelay = iTotalDelay = 0;
    iMinDelay = 10000;
    iNumFrames = 1;
    iDataRemaining = pPage->GIFFile.iSize;
    cBuf = (uint8_t *) pPage->ucFileBuf;
    (*pPage->pfnSeek)(&pPage->GIFFile, 0);
    iDataAvailable = (*pPage->pfnRead)(&pPage->GIFFile, cBuf, FILE_BUF_SIZE);
    iDataRemaining -= iDataAvailable;
   // lFileOff += iDataAvailable;
    iOff = 10;
    c = cBuf[iOff]; // получить информацию
    iOff += 3;   /* Пропустить флаги, цвет фона и соотношение сторон */
    if (c & 0x80) /* Работа с глобальной таблицей цветов */
    {
        c &= 7;  /* Получить количество определенных цветов */
        iOff += (2<<c)*3; /* пропустить таблицу цветов */
    }
    while (!bDone) // && iNumFrames <MAX_FRAMES)
    {
        bExt = 1; /* пропускать блоки расширения */
        while (bExt && iOff < iDataAvailable)
        {
            if ((iDataAvailable - iOff) < 258) // сначала нужно прочитать больше данных
            {
                lv_memmove(cBuf, &cBuf[iOff], (iDataAvailable-iOff)); // переместить существующие данные вниз
                iDataAvailable -= iOff;
                iOff = 0;
                iReadAmount = (*pPage->pfnRead)(&pPage->GIFFile, &cBuf[iDataAvailable], FILE_BUF_SIZE-iDataAvailable);
                iDataAvailable += iReadAmount;
                iDataRemaining -= iReadAmount;
               // lFileOff += iReadAmount;
            }
            switch(cBuf[iOff])
            {
                case 0x3b: /* Конец файла */
                    /* мы обманулись, думая, что страниц больше */
                    iNumFrames--;
                    goto gifpagesz;
    // F9 = Расширение графического управления (фиксированная длина 4 байта)
    // FE = Расширение комментариев
    // FF = Расширение приложения
    // 01 = Простое текстовое расширение
                case 0x21: /* Блок расширения */
                    if (cBuf[iOff+1] == 0xf9 && cBuf[iOff+2] == 4) // Расширение графического управления
                    {
                       //cBuf[iOff+3]; // флаги расположение страницы
                        iDelay = cBuf[iOff+4]; // задержка младшего байта
                        iDelay |= ((uint16_t)(cBuf[iOff+5]) << 8); // задержка старшего байта
                        if (iDelay < 2) // слишком быстро, укажите значение по умолчанию
                            iDelay = 2;
                        iDelay *= 10; // превратите JIFFIES в миллисекунды
                        iTotalDelay += iDelay;
                        if (iDelay > iMaxDelay) iMaxDelay = iDelay;
                        else if (iDelay < iMinDelay) iMinDelay = iDelay;
                       // (cBuf[iOff+6]; // индекс прозрачного цвета
                    }
                    iOff += 2; /* перейти к длине */
                    iOff += (int)cBuf[iOff]; /* Пропустить блок данных */
                    iOff++;
                   // терминатор блока или дополнительные подблоки
                    c = cBuf[iOff++]; /* Пропустить любые подблоки */
                    while (c)
                       {
                       iOff += (int)c;
                       c = cBuf[iOff++];
                       if ((iDataAvailable - iOff) < (c+258)) // сначала нужно прочитать больше данных
                        {
                            lv_memmove(cBuf, &cBuf[iOff], (iDataAvailable-iOff)); // переместить существующие данные вниз
                            iDataAvailable -= iOff;
                            iOff = 0;
                            iReadAmount = (*pPage->pfnRead)(&pPage->GIFFile, &cBuf[iDataAvailable], FILE_BUF_SIZE-iDataAvailable);
                            iDataAvailable += iReadAmount;
                            iDataRemaining -= iReadAmount;
                           // lFileOff += iReadAmount;
                        }
                    }
                    if (c != 0) // проблема, мы прошли мимо конца
                    {
                        iNumFrames--; // возможные поврежденные данные; стоп
                        goto gifpagesz;
                    }
                    break;
                case 0x2c: /* Начало данных изображения */
                    bExt = 0; /* Перестаньте делать блоки расширения */
                    break;
                default:
                   /* Поврежденные данные, остановитесь здесь */
                    iNumFrames--;
                    goto gifpagesz;
            } // переключатель
        } // пока
        if (iOff >= iDataAvailable) // проблема
        {
             iNumFrames--; // возможные поврежденные данные; стоп
             goto gifpagesz;
        }
          /* Начало данных изображения */
        c = cBuf[iOff+9]; /* Получить байт флагов */
        iOff += 10; /* Пропустить положение и размер изображения */
        if (c & 0x80) /* Таблица локальных цветов */
        {
            c &= 7;
            iOff += (2<<c)*3;
        }
        iOff++; /* Пропустить байт размера кода LZW */
        if ((iDataAvailable - iOff) < (c+258)) // сначала нужно прочитать больше данных
         {
             if (iOff < iDataAvailable) {
                 lv_memmove(cBuf, &cBuf[iOff], (iDataAvailable-iOff)); // переместить существующие данные вниз
                 iDataAvailable -= iOff;
                 iOff = 0;
             } else { // уже указывает на конец
                 iOff -= iDataAvailable;
                 iDataAvailable = 0;
             }
             iReadAmount = (*pPage->pfnRead)(&pPage->GIFFile, &cBuf[iDataAvailable], FILE_BUF_SIZE-iDataAvailable);
             iDataAvailable += iReadAmount;
             iDataRemaining -= iReadAmount;
            // lFileOff += iReadAmount;
         }
        c = cBuf[iOff++];
        while (c) /* Хотя блоков данных больше */
        {
            if (iOff > (3*FILE_BUF_SIZE/4) && iDataRemaining > 0) /* Ближе к концу буфера, повторите выравнивание */
            {
                lv_memmove(cBuf, &cBuf[iOff], (iDataAvailable-iOff)); // переместить существующие данные вниз
                iDataAvailable -= iOff;
                iOff = 0;
                iReadAmount = (FILE_BUF_SIZE - iDataAvailable);
                if (iReadAmount > iDataRemaining)
                    iReadAmount = iDataRemaining;
                iReadAmount = (*pPage->pfnRead)(&pPage->GIFFile, &cBuf[iDataAvailable], iReadAmount);
                iDataAvailable += iReadAmount;
                iDataRemaining -= iReadAmount;
                // lFileOff += iReadAmount;
            }
            iOff += (int)c;  /* Пропустить этот блок данных */
//            if ((int)lFileOff + iOff > pPage->GIFFile.iSize) // прошлый конец файла, остановка
//            {
//                iNumFrames--; // не считать эту страницу
//                перерыв; // последняя страница повреждена, не используйте ее
//            }
            c = cBuf[iOff++]; /* Получить длину следующего */
        }
        /* Конец данных изображения, проверьте наличие дополнительных страниц... */
        if (cBuf[iOff] == 0x3b || (iDataRemaining == 0 && (iDataAvailable - iOff) < 32))
        {
            bDone = 1; /* Достигнут конец файла */
        }
        else /* Больше страниц для сканирования */
        {
            iNumFrames++;
             // прочитать данные новой страницы, начиная с этого смещения
            if (pPage->GIFFile.iSize > FILE_BUF_SIZE && iDataRemaining > 0) // так как мы не прочитали весь файл за один раз
            {
                lv_memmove(cBuf, &cBuf[iOff], (iDataAvailable-iOff)); // переместить существующие данные вниз
                iDataAvailable -= iOff;
                iOff = 0;
                iReadAmount = (FILE_BUF_SIZE - iDataAvailable);
                if (iReadAmount > iDataRemaining)
                    iReadAmount = iDataRemaining;
                iReadAmount = (*pPage->pfnRead)(&pPage->GIFFile, &cBuf[iDataAvailable], iReadAmount);
                iDataAvailable += iReadAmount;
                iDataRemaining -= iReadAmount;
               // lFileOff += iReadAmount;
            }
        }
    } /* пока !bГотово */
gifpagesz:
    pInfo->iFrameCount = iNumFrames;
    pInfo->iMaxDelay = iMaxDelay;
    pInfo->iMinDelay = iMinDelay;
    pInfo->iDuration = iTotalDelay;
    return 1;
} /* GIF_getInfo () */

//
// Распакуйте больше данных фрагмента для декодирования
// возвращает 1, чтобы указать, что для этого изображения доступно больше данных
// 0 означает, что данных больше нет
//
static int GIFGetMoreData(GIFIMAGE *pPage)
{
    int iDelta = (pPage->iLZWSize - pPage->iLZWOff);
    int iLZWBufSize;
    unsigned char c = 1;

    // Режим Турбо использует комбинированные буферы для чтения большего количества сжатых данных.
    iLZWBufSize = (pPage->pTurboBuffer) ? LZW_BUF_SIZE_TURBO : LZW_BUF_SIZE;
    // переместить любые существующие данные вниз
    if (pPage->bEndOfFrame ||  iDelta >= (iLZWBufSize - MAX_CHUNK_SIZE) || iDelta <= 0)
        return 1; // кадр завершен или буфер уже заполнен; нет необходимости читать больше данных
    if (pPage->iLZWOff != 0)
    {
// NB: memcpy() не работает в некоторых системах, поскольку параметры src и dest перекрываются.
// поэтому копируйте байты в простом цикле, чтобы избежать проблем
      for (int i=0; i<pPage->iLZWSize - pPage->iLZWOff; i++) {
         pPage->ucLZW[i] = pPage->ucLZW[i + pPage->iLZWOff];
      }
      pPage->iLZWSize -= pPage->iLZWOff;
      pPage->iLZWOff = 0;
    }
    while (c && pPage->GIFFile.iPos < pPage->GIFFile.iSize && pPage->iLZWSize < (iLZWBufSize-MAX_CHUNK_SIZE))
    {
        (*pPage->pfnRead)(&pPage->GIFFile, &c, 1); // текущая длина
        (*pPage->pfnRead)(&pPage->GIFFile, &pPage->ucLZW[pPage->iLZWSize], c);
        pPage->iLZWSize += c;
    }
    if (c == 0) // конец кадра
        pPage->bEndOfFrame = 1;
    return (c != 0 && pPage->GIFFile.iPos < pPage->GIFFile.iSize); // more data available?
} /* GIFGetMoreData() */
//
// Рисуйте и конвертируйте пиксели, когда пользователю нужен полностью визуализированный результат.
//
static void DrawCooked(GIFIMAGE *pPage, GIFDRAW *pDraw, void *pDest)
{
    uint8_t c, *s, *d8, *pEnd;
    uint8_t *pActivePalette;

    pActivePalette = (pPage->bUseLocalPalette) ? (uint8_t *)pPage->pLocalPalette : (uint8_t *)pPage->pPalette;
    // d8 указывает на линию на полноразмерном холсте, где будут объединены новые непрозрачные пиксели.
    d8 = &pPage->pFrameBuffer[pDraw->iX + (pDraw->iY + pDraw->y) * pPage->iCanvasWidth];
    s = pDraw->pPixels; // s указывает на вновь декодированные пиксели этой строки текущего кадра
    pEnd = s + pDraw->iWidth; // более быстрый способ перебора исходных пикселей — исключает переменную счетчика

    if (pPage->ucPaletteType == GIF_PALETTE_1BPP || pPage->ucPaletteType == GIF_PALETTE_1BPP_OLED) { // 1-битное моно
        uint8_t *d = NULL;
        uint8_t *pPal = pActivePalette;
        uint8_t uc, ucMask;
        int iPitch = 0;
         if (pPage->ucPaletteType == GIF_PALETTE_1BPP) { // горизонтальные пиксели
             d = pPage->pFrameBuffer;
             iPitch = (pPage->iCanvasWidth+7)/8;
             d += (pPage->iCanvasWidth * pPage->iCanvasHeight);
             d += pDraw->iX/8; // стартовая колонка
             d += (pDraw->iY + pDraw->y) * iPitch;
             // Примените новые пиксели к основному изображению и сгенерируйте вывод с разрешением 1 бит на пиксель.
             if (pDraw->ucHasTransparency) { // если используется прозрачность
                 uint8_t ucTransparent = pDraw->ucTransparent;
                 if (pDraw->ucDisposalMethod == 2) { // восстановить цвет фона
                     uint8_t u8BG = pPal[pDraw->ucBackground];
                     if (u8BG == 1) u8BG = 0xff; // установить все биты для использования маски
                     uc = *d; ucMask = (0x80 >> (pDraw->iX & 7));;
                     while (s < pEnd) {
                         c = *s++;
                         if (c != ucTransparent) {
                             if (pPal[c])
                                uc |= ucMask;
                             else
                                uc &= ~ucMask;
                             *d8++ = c;
                         } else {
                             uc |= (u8BG & ucMask); // прозрачный пиксель восстанавливается до цвета фона
                             *d8++ = pDraw->ucBackground;
                         }
                         ucMask >>= 1;
                         if (ucMask == 0) { // записать завершенный байт
                             *d++ = uc;
                             uc = *d;
                             ucMask = 0x80;
                         }
                     }
                     *d = uc; // записать последний неполный байт
                 } else { // никакого удаления, просто пишите непрозрачные пиксели
                     uc = *d; ucMask = (0x80 >> (pDraw->iX & 7));
                     while (s < pEnd) {
                         c = *s++;
                         if (c != ucTransparent) {
                             if (pPal[c])
                                 uc |= ucMask;
                             else
                                 uc &= ~ucMask;
                             *d8 = c;
                         }
                         d8++;
                         ucMask >>= 1;
                         if (ucMask == 0) {
                             *d++ = uc;
                             uc = *d;
                             ucMask = 0x80;
                         }
                     }
                     *d = uc;
                 }
             } else { // конвертировать все как непрозрачное
                 uc = *d; ucMask = (0x80 >> (pDraw->iX & 7)); // левый пиксель — MSB
                 while (s < pEnd) {
                     c = *d8++ = *s++; // просто напишите новые непрозрачные пиксели поверх старых
                     if (pPal[c]) // если не ноль, установить белый пиксель
                        uc |= ucMask;
                     else
                        uc &= ~ucMask;
                     ucMask >>= 1;
                     if (ucMask == 0) { // время записать текущий байт
                        *d++ = uc;
                        uc = *d;
                        ucMask = 0x80;
                     }
                 }
                 *d = uc;
             }
         } else { // вертикальные пиксели
             d = pPage->pFrameBuffer;
             d += (pPage->iCanvasWidth * pPage->iCanvasHeight);
             d += pDraw->iX; // стартовая колонка
             d += ((pDraw->iY + pDraw->y)>>3) * pPage->iCanvasWidth;
             ucMask = 1 << ((pDraw->iY + pDraw->y) & 7);
             // Примените новые пиксели к основному изображению и сгенерируйте вывод с разрешением 1 бит на пиксель.
             if (pDraw->ucHasTransparency) { // если используется прозрачность
                 uint8_t ucTransparent = pDraw->ucTransparent;
                 if (pDraw->ucDisposalMethod == 2) { // восстановить цвет фона
                     uint8_t u8BG = pPal[pDraw->ucBackground];
                     u8BG *= ucMask; // установите правильный бит
                     while (s < pEnd) {
                         c = *s++;
                         uc = *d & ~ucMask; // очистить старый пиксель
                         if (c != ucTransparent) {
                             uc |= (pPal[c] * ucMask);
                             *d8++ = c;
                         } else {
                             uc |= u8BG; // прозрачный пиксель восстанавливается до цвета фона
                             *d8++ = pDraw->ucBackground;
                         }
                         *d++ = uc; // записать обновленный пиксель обратно
                     }
                 } else { // никакого удаления, просто пишите непрозрачные пиксели
                     while (s < pEnd) {
                         c = *s++;
                         uc = *d & ~ucMask;
                         if (c != ucTransparent) {
                             *d = uc | (pPal[c] * ucMask);
                             *d8 = c;
                         }
                         d++;
                         d8++;
                     }
                 }
             } else { // конвертировать все как непрозрачное
                 while (s < pEnd) {
                     c = *d8++ = *s++; // просто напишите новые непрозрачные пиксели поверх старых
                     uc = *d & ~ucMask;
                     *d++ = uc | (pPal[c] * ucMask);
                 }
             }
         }
    } else if (pPage->ucPaletteType == GIF_PALETTE_RGB565_LE || pPage->ucPaletteType == GIF_PALETTE_RGB565_BE) {
        uint16_t *d, *pPal = (uint16_t *)pActivePalette;
        d = (uint16_t *)pDest; // указатель на приготовленные пиксели
        // Примените новые пиксели к основному изображению.
        if (pDraw->ucHasTransparency) { // если используется прозрачность
            uint8_t ucTransparent = pDraw->ucTransparent;
            if (pDraw->ucDisposalMethod == 2) { // восстановить цвет фона
                uint16_t u16BG = pPal[pDraw->ucBackground];
                while (s < pEnd) {
                    c = *s++;
                    if (c != ucTransparent) {
                        *d++ = pPal[c];
                        *d8++ = c;
                    } else {
                        *d++ = u16BG; // прозрачный пиксель восстанавливается до цвета фона
                        *d8++ = pDraw->ucBackground;
                    }
                }
            } else { // никакого удаления, просто пишите непрозрачные пиксели
                while (s < pEnd) {
                    c = *s++;
                    if (c != ucTransparent) {
                        *d = pPal[c];
                        *d8 = c;
                    }
                    d++;
                    d8++;
                }
            }
        } else { // конвертировать все пиксели через палитру без прозрачности
#if REGISTER_WIDTH == 64
            // распараллелить запись
            // помогает оптимизация буфера записи; чтение 4 байтов за раз против 1 нет на M1
            while (s < pEnd + 4) { // группа 4 пикселя
                BIGUINT bu;
                uint8_t s0, s1, s2, s3;
                uint16_t d1, d2, d3;
                *(uint32_t *)d8 = *(uint32_t *)s; // просто скопируйте новые непрозрачные пиксели поверх старых
                s0 = s[0]; s1 = s[1]; s2 = s[2]; s3 = s[3];
                bu = pPal[s0]; // особая разница с AppleM1нет
                d1 = pPal[s1]; // но другие процессоры могут выиграть
                d2 = pPal[s2]; // от развертывания чтения
                d3 = pPal[s3];
                bu |= (BIGUINT)d1 << 16;
                bu |= (BIGUINT)d2 << 32;
                bu |= (BIGUINT)d3 << 48;
                s += 4;
                d8 += 4;
                *(BIGUINT *)d = bu;
                d += 4;
            }
#endif
            while (s < pEnd) {
                c = *d8++ = *s++; // просто напишите новые непрозрачные пиксели поверх старых
                *d++ = pPal[c]; // и создаем приготовленные пиксели через палитру
            }
        }
    } else { // 24 бит/с или 32 бит/с
        uint8_t pixel, *d, *pPal;
        int x;
        d = (uint8_t *)pDest;
        pPal = pActivePalette;
        if (pDraw->ucHasTransparency) {
            uint8_t ucTransparent = pDraw->ucTransparent;
            if (pDraw->ucDisposalMethod == 2) { // восстановить цвет фона
                uint8_t * bg = &pPal[pDraw->ucBackground * 3];
                if (pPage->ucPaletteType == GIF_PALETTE_RGB888) {
                    while (s < pEnd) {
                        pixel = *s++;
                        if (pixel != ucTransparent) {
                            *d8++ = pixel;
                            d[0] = pPal[(pixel * 3) + 2];
                            d[1] = pPal[(pixel * 3) + 1];
                            d[2] = pPal[(pixel * 3) + 0];
                            d += 3;
                        } else {
                            *d8++ = pDraw->ucBackground;
                            d[0] = bg[2];
                            d[1] = bg[1];
                            d[2] = bg[0];
                            d += 3;
                        }
                    }
                } else { /* GIF_PALETTE_RGB8888 */
                    while (s < pEnd) {
                        pixel = *s++;
                        if (pixel != ucTransparent) {
                            *d8++ = pixel;
                            d[0] = pPal[(pixel * 3) + 2];
                            d[1] = pPal[(pixel * 3) + 1];
                            d[2] = pPal[(pixel * 3) + 0];
                            d[3] = 0xFF;
                            d += 4;
                        } else {
                            *d8++ = pDraw->ucBackground;
                            d[3] = 0x00;
                            d += 4;
                        }
                    }
                }
            } else { // никакого удаления, просто пишите непрозрачные пиксели
                if (pPage->ucPaletteType == GIF_PALETTE_RGB888) {
                    for (x=0; x<pPage->iWidth; x++) {
                        pixel = *s++;
                        if (pixel != ucTransparent) {
                            *d8 = pixel;
                            d[0] = pPal[(pixel * 3) + 2]; // конвертировать в пиксели RGB888
                            d[1] = pPal[(pixel * 3) + 1];
                            d[2] = pPal[(pixel * 3) + 0];
                        }
                        d8++;
                        d += 3;
                    }
                } else { // должно быть RGBA32
                    for (x=0; x<pPage->iWidth; x++) {
                        pixel = *s++;
                        if (pixel != ucTransparent) {
                            *d8 = pixel;
                            d[0] = pPal[(pixel * 3) + 2]; // конвертировать в пиксели RGB8888
                            d[1] = pPal[(pixel * 3) + 1];
                            d[2] = pPal[(pixel * 3) + 0];
                            d[3] = 0xff;
                        }
                        d8++;
                        d += 4;
                    }
                }
            }
        } else { // нет прозрачности
            if (pPage->ucPaletteType == GIF_PALETTE_RGB888) {
                for (x=0; x<pPage->iWidth; x++) {
                    pixel = *d8++ = *s++;
                    *d++ = pPal[(pixel * 3) + 2]; // конвертировать в пиксели RGB888
                    *d++ = pPal[(pixel * 3) + 1];
                    *d++ = pPal[(pixel * 3) + 0];
                }
            } else { // должно быть RGBA32
                for (x=0; x<pPage->iWidth; x++) {
                    pixel = *d8++ = *s++;
                    *d++ = pPal[(pixel * 3) + 2]; // конвертировать в пиксели RGB8888
                    *d++ = pPal[(pixel * 3) + 1];
                    *d++ = pPal[(pixel * 3) + 0];
                    *d++ = 0xff;
                }
            }
        } // непрозрачный
    }
} /* DrawCooked() */

//
// Обработка прозрачных пикселей и метод удаления
// Используется только тогда, когда выделен кадровый буфер.
//
static void DrawNewPixels(GIFIMAGE *pPage, GIFDRAW *pDraw)
{
    uint8_t *d, *s;
    int x, iPitch = pPage->iCanvasWidth;

    s = pDraw->pPixels;
    d = &pPage->pFrameBuffer[pDraw->iX + (pDraw->y + pDraw->iY)  * iPitch]; // указатель места в нашем полном буфере холста

    // Примените новые пиксели к основному изображению.
    if (pDraw->ucHasTransparency) { // если используется прозрачность
        uint8_t c, ucTransparent = pDraw->ucTransparent;
        if (pDraw->ucDisposalMethod == 2) {
            lv_memset(d, pDraw->ucBackground, pDraw->iWidth); // начните с цвета фона
        }
        for (x=0; x<pDraw->iWidth; x++) {
            c = *s++;
            if (c != ucTransparent)
                *d = c;
            d++;
        }
    } else { // метод удаления не имеет значения, если нет прозрачных пикселей
        lv_memcpy(d, s, pDraw->iWidth); // просто перезапиши старые пиксели
    }
} /* DrawNewPixels() */
//
// LZWCopyBytes
//
// Выведите байты для одного кода (проверяет длину буфера)
//
static int LZWCopyBytes(unsigned char *buf, int iOffset, uint32_t *pSymbols, uint16_t *pLengths)
{
int iLen;
uint8_t c, *s, *d, *pEnd;
uint32_t u32Offset;

    iLen = *pLengths;
    u32Offset = *pSymbols;
    // Строковые данные часто записываются за конец кадрового буфера (после последнего пикселя).
    // ...но при размещении наших кодовых таблиц AFTER во фреймбуфере это не имеет значения
    // Добавление сюда проверки на переполнение буфера замедляет работу примерно на 10%.
    s = &buf[u32Offset & 0x7fffff];
    d = &buf[iOffset];
    pEnd = &d[iLen];
    while (d < pEnd) // чаще всего имеют длину 1-8 байт, в этих случаях также копируйте 4 или 8 байтов
    {
#ifdef ALLOWS_UNALIGNED
// Это значительное улучшение производительности по сравнению с копированием по одному байту за раз.
// хотя он часто копирует слишком много байтов
        BIGUINT tmp = *(BIGUINT *) s;
        s += sizeof(BIGUINT);
        *(BIGUINT *)d = tmp;
        d += sizeof(BIGUINT);
#else
// Процессоры, которые применяют исключения невыровненных адресов, должны делать это по 1 байту за раз.
        *d++ = *s++;
#endif
    }
    if (u32Offset & 0x800000) // был недавно использованный код
    {
        d = pEnd; // на случай, если мы промахнемся
        c = (uint8_t)(u32Offset >> 24);
        iLen++;
        // так как теперь на выходе записан код с байтом расширения, исправьте код
        *pSymbols = iOffset;
//        pSymbols[ SYM_EXTRAS ] = 0xffffffff ;
        *d = c;
        *pLengths = (uint16_t)iLen;
    }
    return iLen;
} /* LZWCopyBytes() */
//
// Макрос для извлечения кода переменной длины
//
#define GET_CODE_TURBO if (bitnum > (REGISTER_WIDTH - MAX_CODE_SIZE/*размер кода*/)) { p += (bitnum >> 3); \
            bitnum &= 7; ulBits = INTELLONG(p); } \
        code = ((ulBits >> bitnum) & sMask);  \
        bitnum += codesize;

//
// ДекодированиеLZWTurbo
//
// Теория работы:
//
// «Традиционный» декодер LZW поддерживает словарь со связанным списком кодов.
// Эти коды образуют более длинные цепочки по мере декодирования большего количества данных. Чтобы вывести пиксели,
// связанный список перемещается назад от последнего узла к первому, затем эти
// пиксели копируются в обратном порядке к выходному растровому изображению.
//
// Мой декодер использует другой подход. Выходное изображение становится словарем и
// таблицы отслеживают, где в выходном изображении начинается «прогон» и его длина.
// ** NB **
// Эти таблицы не могут быть 16-битными значениями, поскольку выходные данные одного словаря могут быть
// больше 64К
//
// Я также работаю со сжатыми данными по-другому. Большинство декодеров проходят через
// фрагментированные данные, постоянно проверяя, закончились ли данные в текущем фрагменте. я
// используйте другой подход, поскольку современные машины имеют много памяти - я «разбиваю»
// сначала данные, чтобы внутренний цикл мог декодировать как можно быстрее. я тоже держу
// набор кодов в 64-битной локальной переменной для минимизации операций чтения из памяти.
//
// Эти два изменения приводят к значительно более быстрому декодеру. Для плохо сжатых изображений
// Прирост скорости примерно в 2,5 раза по сравнению с giflib. Для сильно сжатых образов (длинные тиражи)
// скорость может быть в 30 раз выше. Это потому, что ему не нужно ходить
// назад по связанному списку кодов при выводе пикселей. Это также не
// придется копировать пиксели в обратном порядке, а потом их раскручивать.
//
static int DecodeLZWTurbo(GIFIMAGE *pImage, int iOptions)
{
int i, bitnum;
int iUncompressedLen;
uint32_t code, oldcode, codesize, nextcode, nextlim;
uint32_t cc, eoi;
uint32_t sMask;
uint8_t c, *p, *buf, codestart, *pHighWater;
BIGUINT ulBits;
int iLen, iColors;
int iErr = GIF_SUCCESS;
int iOffset;
uint32_t *pSymbols;
uint16_t *pLengths;

    (void)iOptions;
    pImage->iYCount = pImage->iHeight; // отсчитайте строки
    pImage->iXCount = pImage->iWidth;
    bitnum = 0;
    pHighWater = pImage->ucLZW + LZW_HIGHWATER_TURBO;
    pImage->iLZWOff = 0; // Смещение в сжатые данные
    GIFGetMoreData(pImage); // Прочитайте некоторые данные для начала
    codestart = pImage->ucCodeStart;
    iColors = 1 << codestart;
    sMask = UINT32_MAX << (codestart+1);
    sMask = 0xffffffff - sMask;
    cc = (sMask >> 1) + 1; /* Очистить код */
    eoi = cc + 1;
    iUncompressedLen = (pImage->iWidth * pImage->iHeight);
    buf = (uint8_t *)pImage->pTurboBuffer;
    pSymbols = (uint32_t *)&buf[iUncompressedLen+256]; // нам нужны 32 бита (на самом деле 23) для смещений
    pLengths = (uint16_t *)&pSymbols[4096]; // но только 16 бит для длины любой отдельной строки
    iOffset = 0; // смещение выходных данных
    p = pImage->ucLZW; // нефрагментированные данные LZW
    ulBits = INTELLONG(p); // начните с чтения некоторых данных LZW
    // установить символы по умолчанию (0..iColors-1)
   for (i = 0; i<iColors; i++) {
       pSymbols[i] = iUncompressedLen + i; // корневые символы
       pLengths[i] = 1;
       buf[iUncompressedLen + i] = (unsigned char) i;
   }
init_codetable:
   codesize = codestart + 1;
   sMask = UINT32_MAX << (codestart+1);
   sMask = 0xffffffff - sMask;
   nextcode = cc + 2;
   nextlim = (1 << codesize);
    GET_CODE_TURBO
    if (code == cc) { // мы просто сбрасываем словарь; получить другой код
        GET_CODE_TURBO
    }
    buf[iOffset++] = (unsigned char) code; // первый код после сброса словаря просто сохраняется
    oldcode = code;
    GET_CODE_TURBO
    while (code != eoi && iOffset < iUncompressedLen) { /* Перебрать все данные */
        if (code == cc) { /* Clear code? */
           goto init_codetable;
        }
        if (code != eoi) {
            if (nextcode < nextlim) { // в случае отложенной копии не позволяйте ей перезаписывать всю запись (fff)
                if (code != nextcode) { // наиболее вероятный случай
                    iLen = LZWCopyBytes(buf, iOffset, &pSymbols[code], &pLengths[code]);
                    pSymbols[nextcode] = (pSymbols[oldcode] | 0x800000 | (buf[iOffset] << 24));
                    pLengths[nextcode] = pLengths[oldcode];
                    iOffset += iLen;
                } else { // новый код
                    iLen = LZWCopyBytes(buf, iOffset, &pSymbols[oldcode], &pLengths[oldcode]);
                    pLengths[nextcode] = iLen+1;
                    pSymbols[nextcode] = iOffset;
                    c = buf[iOffset];
                    iOffset += iLen;
                    buf[iOffset++] = c; // повторить первый символ старого кода в конце
                }
            } else { // Отложенный случай CC — продолжайте использовать коды, но не создавайте новые.
                iLen = LZWCopyBytes(buf, iOffset, &pSymbols[code], &pLengths[code]);
                iOffset += iLen;
            }
            nextcode++;
            if (nextcode >= nextlim && codesize < MAX_CODE_SIZE) {
                codesize++;
                nextlim <<= 1;
                sMask = (sMask << 1) | 1;
            }
            if (p >= pHighWater) {
                pImage->iLZWOff = (int)(p - pImage->ucLZW); // восстановить член объекта var
                GIFGetMoreData(pImage); // Нам нужно прочитать больше данных LZW.
                p = &pImage->ucLZW[pImage->iLZWOff];
            }
            oldcode = code;
            GET_CODE_TURBO
        } /* хотя это не конец потока кода LZW */
    } // пока не конец кадра
    if (pImage->ucDrawType == GIF_DRAW_COOKED && pImage->pfnDraw && pImage->pFrameBuffer) { // конвертировать каждую строку через палитру
        GIFDRAW gd;
        gd.iX = pImage->iX;
        gd.iY = pImage->iY;
        gd.iWidth = pImage->iWidth;
        gd.iHeight = pImage->iHeight;
        gd.pPalette = (pImage->bUseLocalPalette) ? pImage->pLocalPalette : pImage->pPalette;
        gd.pPalette24 = (uint8_t *)gd.pPalette; // просто наведите указатель на RGB888
        gd.ucIsGlobalPalette = pImage->bUseLocalPalette==1?0:1;
        gd.pUser = pImage->pUser;
        gd.ucPaletteType = pImage->ucPaletteType;
        for (int y=0; y<pImage->iHeight; y++) {
            gd.y = y;
            gd.pPixels = &buf[(y * pImage->iWidth)]; // исходные пиксели
            // Уродливая логика для обработки положения чересстрочной строки, но она
            // избавляет от необходимости иметь другой набор переменных состояния
            if (pImage->ucMap & 0x40) { // interlaced?
               int height = pImage->iHeight-1;
               if (gd.y > height / 2)
                  gd.y = gd.y * 2 - (height | 1);
               else if (gd.y > height / 4)
                  gd.y = gd.y * 4 - ((height & ~1) | 2);
               else if (gd.y > height / 8)
                  gd.y = gd.y * 8 - ((height & ~3) | 4);
               else
                  gd.y = gd.y * 8;
            }
            gd.ucDisposalMethod = (pImage->ucGIFBits & 0x1c)>>2;
            gd.ucTransparent = pImage->ucTransparent;
            gd.ucHasTransparency = pImage->ucGIFBits & 1;
            gd.ucBackground = pImage->ucBackground;
            gd.iCanvasWidth = pImage->iCanvasWidth;
            DrawCooked(pImage, &gd, &buf[pImage->iCanvasHeight * pImage->iCanvasWidth]); // dest = прошлый конец холста
            gd.pPixels = &buf[pImage->iCanvasHeight * pImage->iCanvasWidth]; // укажите на строку, которую мы только что преобразовали
            (*pImage->pfnDraw)(&gd); // обратный вызов для обработки этой строки
        }
    }
    return iErr;
} /* DecodeLZWTurbo() */

//
// GIFMakePels
//
static void GIFMakePels(GIFIMAGE *pPage, unsigned int code)
{
    int iPixCount;
    unsigned short *giftabs;
    unsigned char *buf, *s, *pEnd, *gifpels;
    /* Скопируйте эту строку последовательных пикселей в выходной буфер. */
    //   iPixCount = 0;
    pEnd = pPage->ucFileBuf;
    s = pEnd + FILE_BUF_SIZE; /* Пиксели выйдут в обратном порядке */
    buf = pPage->ucLineBuf + (pPage->iWidth - pPage->iXCount);
    giftabs = pPage->usGIFTable;
    gifpels = &pPage->ucGIFPixels[PIXEL_LAST];
    while (code < LINK_UNUSED)
    {
        if (s == pEnd) /* Хьюстон, у нас проблема */
        {
            return; /* Выйти с ошибкой */
        }
        *(--s) = gifpels[code];
        code = giftabs[code];
    }
    iPixCount = (int)(intptr_t)(pEnd + FILE_BUF_SIZE - s);
    while (iPixCount && pPage->iYCount > 0)
    {
        if (pPage->iXCount > iPixCount)  /* Пиксели полностью помещаются на линии */
        {
            pEnd = buf + iPixCount;
            while (buf < pEnd) {
#ifdef ALLOWS_UNALIGNED
// Это значительное улучшение производительности по сравнению с копированием по одному байту за раз.
// даже несмотря на то, что он часто копирует слишком много байтов. Поскольку мы еще не в конце
// линию, можно скопировать несколько дополнительных пикселей.
                BIGUINT tmp = *(BIGUINT *) s;
                s += sizeof(BIGUINT);
                *(BIGUINT *)buf = tmp;
                buf += sizeof(BIGUINT);
#else
                *buf++ = *s++;
#endif
            }
            pPage->iXCount -= iPixCount;
            //         iPixCount = 0;
            if (pPage->iLZWOff >= LZW_HIGHWATER)
                GIFGetMoreData(pPage); // Нам нужно прочитать больше данных LZW.
            return;
        }
        else  /* Пиксели переходят на следующую строку */
        {
            GIFDRAW gd;
            pEnd = buf + pPage->iXCount;
            while (buf < pEnd)
            {
                *buf++ = *s++;
            }
            iPixCount -= pPage->iXCount;
            pPage->iXCount = pPage->iWidth; /* Сбросить количество пикселей */
            // Подготовьте структуру GIDRAW для обратного вызова.
            gd.iX = pPage->iX;
            gd.iY = pPage->iY;
            gd.iWidth = pPage->iWidth;
            gd.iHeight = pPage->iHeight;
            gd.pPixels = pPage->ucLineBuf;
            gd.pPalette = (pPage->bUseLocalPalette) ? pPage->pLocalPalette : pPage->pPalette;
            gd.pPalette24 = (uint8_t *)gd.pPalette; // просто наведите указатель на RGB888
            gd.ucIsGlobalPalette = pPage->bUseLocalPalette==1?0:1;
            gd.y = pPage->iHeight - pPage->iYCount;
            // Уродливая логика для обработки положения чересстрочной строки, но она
            // избавляет от необходимости иметь другой набор переменных состояния
            if (pPage->ucMap & 0x40) { // interlaced?
               int height = pPage->iHeight-1;
               if (gd.y > height / 2)
                  gd.y = gd.y * 2 - (height | 1);
               else if (gd.y > height / 4)
                  gd.y = gd.y * 4 - ((height & ~1) | 2);
               else if (gd.y > height / 8)
                  gd.y = gd.y * 8 - ((height & ~3) | 4);
               else
                  gd.y = gd.y * 8;
            }
            gd.ucDisposalMethod = (pPage->ucGIFBits & 0x1c)>>2;
            gd.ucTransparent = pPage->ucTransparent;
            gd.ucHasTransparency = pPage->ucGIFBits & 1;
            gd.ucBackground = pPage->ucBackground;
            gd.iCanvasWidth = pPage->iCanvasWidth;
            gd.pUser = pPage->pUser;
            gd.ucPaletteType = pPage->ucPaletteType;
            if (pPage->pFrameBuffer) // обновить буфер кадров
            {
                int iPitch = 0, iBpp = 1, iOffset = pPage->iCanvasWidth * pPage->iCanvasHeight;
                if (pPage->ucDrawType == GIF_DRAW_COOKED) {
                    if (!pPage->pfnDraw) { // нет обратного вызова отрисовки, подготовьте полный кадр
                        switch (pPage->ucPaletteType) {
                            case GIF_PALETTE_1BPP:
                                iPitch = (pPage->iCanvasWidth + 7) / 8;
                                break;
                            case GIF_PALETTE_RGB565_BE:
                            case GIF_PALETTE_RGB565_LE:
                                iPitch = (pPage->iCanvasWidth * 2);
                                iBpp = 2;
                                break;
                            case GIF_PALETTE_RGB888:
                                iPitch = pPage->iCanvasWidth * 3;
                                iBpp = 3;
                                break;
                            case GIF_PALETTE_RGB8888:
                                iPitch = pPage->iCanvasWidth * 4;
                                iBpp = 4;
                                break;
                        }
                        iOffset += (iBpp * pPage->iX) + ((gd.y + pPage->iY) * iPitch);
                    }
                    DrawCooked(pPage, &gd, &pPage->pFrameBuffer[iOffset]);
                    // передать указатель подготовленного пикселя обратному вызову GIFDraw
                    gd.pPixels = &pPage->pFrameBuffer[iOffset];
                } else { // пользователь сможет конвертировать их через палитру
                    DrawNewPixels(pPage, &gd); // объединить новые непрозрачные пиксели
                }
            }
            if (pPage->pfnDraw) {
                (*pPage->pfnDraw)(&gd); // обратный вызов для обработки этой строки
            }
            pPage->iYCount--;
            buf = pPage->ucLineBuf;
            if (pPage->iLZWOff >= LZW_HIGHWATER)
                GIFGetMoreData(pPage); // Нам нужно прочитать больше данных LZW.
        }
    } /* пока */
    if (pPage->iLZWOff >= LZW_HIGHWATER)
        GIFGetMoreData(pPage); // Нам нужно прочитать больше данных LZW.
    return;
} /* GIFMakePels() */
//
// Макрос для извлечения кода переменной длины
//
#define GET_CODE if (bitnum > (REGISTER_WIDTH - codesize)) { pImage->iLZWOff += (bitnum >> 3); \
            bitnum &= 7; ulBits = INTELLONG(&p[pImage->iLZWOff]); } \
        code = (unsigned short) (ulBits >> bitnum); /* Чтение фрагмента REGISTER_WIDTH */ \
        code &= sMask; bitnum += codesize;
//
// Декодируйте LZW в изображение
//
static int DecodeLZW(GIFIMAGE *pImage, int iOptions)
{
    int i, bitnum;
    unsigned short oldcode, codesize, nextcode, nextlim;
    unsigned short *giftabs, cc, eoi;
    signed short sMask;
    unsigned char c, *gifpels, *p;
    //    интервал iStripSize;
    //беззнаковый символ **индекс;
    BIGUINT ulBits;
    unsigned short code;
    (void)iOptions; // пока не используется
    // если вывод можно использовать для таблицы строк, делайте это быстрее
    //       if (bGIF && (OutPage->cBitsperpixel == 8 && ((OutPage->iWidth & 3) == 0)))
    //          вернуть PILFastLZW(InPage, OutPage, bGIF, iOptions);
    if (pImage->ucDrawType == GIF_DRAW_COOKED && pImage->pFrameBuffer == NULL && pImage->pfnDraw == NULL) { // без фреймбуфера и обратного вызова GIFDRAW мы не сможем продолжить
        pImage->iError = GIF_INVALID_PARAMETER;
        return 1; // указать на проблему
    }
    // Если пользователь выбрал вывод RAW и нет обратного вызова GIFDRAW, это тоже не сработает.
    if (pImage->ucDrawType == GIF_DRAW_RAW && pImage->pfnDraw == NULL) {
        pImage->iError = GIF_INVALID_PARAMETER;
        return 1; // указать на проблему
    }
    p = pImage->ucLZW; // нефрагментированные данные LZW
    sMask = 0xffff << (pImage->ucCodeStart + 1);
    sMask = 0xffff - sMask;
    cc = (sMask >> 1) + 1; /* Очистить код */
    eoi = cc + 1;
    giftabs = pImage->usGIFTable;
    gifpels = pImage->ucGIFPixels;
    pImage->iYCount = pImage->iHeight; // отсчитайте строки
    pImage->iXCount = pImage->iWidth;
    bitnum = 0;
    pImage->iLZWOff = 0; // Смещение в сжатые данные
    GIFGetMoreData(pImage); // Прочитайте некоторые данные для начала

    // Инициализировать кодовую таблицу
    // эту часть нужно инициализировать только один раз
    for (i = 0; i < cc; i++)
    {
        gifpels[PIXEL_FIRST + i] = gifpels[PIXEL_LAST + i] = (unsigned short) i;
        giftabs[i] = LINK_END;
    }
init_codetable:
    codesize = pImage->ucCodeStart + 1;
    sMask = 0xffff << (pImage->ucCodeStart + 1);
    sMask = 0xffff - sMask;
    nextcode = cc + 2;
    nextlim = (unsigned short) ((1 << codesize));
    // Эту часть таблицы необходимо сбросить несколько раз.
    lv_memset(&giftabs[cc], (uint8_t) LINK_UNUSED, sizeof(pImage->usGIFTable) - sizeof(giftabs[0])*cc);
    ulBits = INTELLONG(&p[pImage->iLZWOff]); // начните с чтения 4 байтов данных LZW
    GET_CODE
    if (code == cc) // мы просто сбросили словарь, так что возьми другой код
    {
      GET_CODE
    }
    c = oldcode = code;
    GIFMakePels(pImage, code); // первый код выводится как первый пиксель
    // Основной цикл декодирования
    while (code != eoi && pImage->iYCount > 0) // && y < pImage->iHeight+1) /* Проходим по всем строкам изображения (или полосам) */
    {
        GET_CODE
        if (code == cc) /* Clear code?, and not first code */
            goto init_codetable;
        if (code != eoi)
        {
                if (nextcode < nextlim) // в случае отложенной копии не позволяйте ей перезаписывать всю запись (fff)
                {
                    giftabs[nextcode] = oldcode;
                    gifpels[PIXEL_FIRST + nextcode] = c; // значение пикселя старого кода
                    gifpels[PIXEL_LAST + nextcode] = c = gifpels[PIXEL_FIRST + code];
                }
                nextcode++;
                if (nextcode >= nextlim && codesize < MAX_CODE_SIZE)
                {
                    codesize++;
                    nextlim <<= 1;
                    sMask = nextlim - 1;
                }
            GIFMakePels(pImage, code);
            oldcode = code;
        }
    } /* хотя это не конец потока кода LZW */
    return 0;
//gif_forced_error:
//    бесплатно (pImage->pPixels);
//    pImage->pPixels = NULL ;
//    вернуть -1;
} /* DecodeLZW() */

#endif // LV_USE_GIF
