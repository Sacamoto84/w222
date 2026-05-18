/*
 *  LZ4 - Fast LZ compression algorithm
 *  Заголовочный файл
 *  Copyright (C) 2011-2023, Yann Collet.

   BSD Лицензия из 2 пунктов ( http://www.opensource.org/licenses/bsd-license.php)

   Распространение и использование в исходной и двоичной форме, с или без
   модификации разрешены при соблюдении следующих условий:
   встретил:

       * При повторном распространении исходного кода необходимо сохранять вышеуказанные авторские права.
   уведомление, этот список условий и следующий отказ от ответственности.
       * Распространения в двоичной форме должны воспроизводить вышеизложенное.
   уведомление об авторских правах, этот список условий и следующий отказ от ответственности
   в документации и/или других материалах, прилагаемых к
   распространение.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   Связаться с автором можно по адресу:
    - LZ4 homepage : http://www.lz4.org
    - LZ4 source repository : https://github.com/lz4/lz4
*/

#include "../../lv_conf_internal.h"
#if LV_USE_LZ4_INTERNAL
#if defined (__cplusplus)
extern "C" {
#endif

/**
 * Портирование LVGL
 */
#include "../../lvgl.h"
#define LZ4_FREESTANDING    1
#define LZ4_memset  lv_memset
#define LZ4_memcpy  lv_memcpy
#define LZ4_memmove lv_memmove

#ifndef LZ4_H_2983827168210
#define LZ4_H_2983827168210

/* --- Зависимость --- */
#include <stddef.h>   /* size_t */


/**
  Введение

  LZ4 — алгоритм сжатия без потерь, обеспечивающий скорость сжатия >500 MB/с на ядро,
  масштабируемость с помощью многоядерных процессоров CPU. Он оснащен чрезвычайно быстрым декодером со скоростью
  несколько GB /s на ядро, обычно достигая предела скорости RAM в многоядерных системах.

  Библиотека сжатия LZ4 предоставляет функции сжатия и распаковки в памяти.
  Это дает пользователю полный контроль над буфером.
  Сжатие может быть выполнено в:
    - a single step (described as Simple Functions)
    - a single step, reusing a context (described in Advanced Functions)
    - unbounded multiple steps (described as Streaming compression)

  lz4.h генерирует и декодирует LZ4-сжатые блоки (doc/lz4_Block_format.md).
  Для распаковки такого сжатого блока требуются дополнительные метаданные.
  Точные метаданные зависят от точной функции декомпрессии.
  Для типичного случая LZ4_decompress_safe(),
  метаданные включают сжатый размер блока и максимальную границу распакованного размера.
  Каждое приложение может кодировать и передавать такие метаданные любым способом.

  lz4.h обрабатывает только блоки и не может генерировать кадры.

  Блоки отличаются от фреймов (doc/lz4_Frame_format.md).
  Фреймы определенным образом объединяют как блоки, так и метаданные.
  Внедрение метаданных необходимо для того, чтобы сжатые данные были автономными и переносимыми.
  Формат кадра предоставляется через сопутствующий API, объявленный в lz4frame.h.
  `lz4` CLI может управлять только кадрами.
*/

/*^***************************************************************
*  Экспорт параметров
*****************************************************************/
/*
*  LZ4_DLL_EXPORT :
*  Включить экспорт функций при сборке Windows DLL
*  LZ4LIB_VISIBILITY :
*  Управляйте видимостью символов библиотеки.
*/
#ifndef LZ4LIB_VISIBILITY
#  if defined(__GNUC__) && (__GNUC__ >= 4)
#    define LZ4LIB_VISIBILITY __attribute__ ((visibility ("default")))
#  else
#    define LZ4LIB_VISIBILITY
#  endif
#endif
#if defined(LZ4_DLL_EXPORT) && (LZ4_DLL_EXPORT==1)
#  define LZ4LIB_API __declspec(dllexport) LZ4LIB_VISIBILITY
#elif defined(LZ4_DLL_IMPORT) && (LZ4_DLL_IMPORT==1)
#  define LZ4LIB_API __declspec(dllimport) LZ4LIB_VISIBILITY /* Это не обязательно, но позволяет генерировать более качественный код, сохраняя загрузку указателя функции из IAT и косвенный переход.*/
#else
#  define LZ4LIB_API LZ4LIB_VISIBILITY
#endif

/*! LZ4_FREESTANDING :
 *  Когда для этого макроса установлено значение 1, он включает «автономный режим», то есть
 *  подходит для типичной отдельно стоящей среды, которая не поддерживает
 *  стандартная библиотека C.
 *
 *  - LZ4_FREESTANDING is a compile-time switch.
 *  - It requires the following macros to be defined:
 *    LZ4_memcpy , LZ4_memmove , LZ4_memset .
 *  - It only enables LZ4/HC functions which don't use heap.
 *    Все функции LZ4F_* не поддерживаются.
 *  - See tests/freestanding.c to check its basic setup.
 */
#if defined(LZ4_FREESTANDING) && (LZ4_FREESTANDING == 1)
#  define LZ4_HEAPMODE 0
#  define LZ4HC_HEAPMODE 0
#  define LZ4_STATIC_LINKING_ONLY_DISABLE_MEMORY_ALLOCATION 1
#  if !defined(LZ4_memcpy)
#    error "LZ4_FREESTANDING requires macro 'LZ4_memcpy'."
#  endif
#  if !defined(LZ4_memset)
#    error "LZ4_FREESTANDING requires macro 'LZ4_memset'."
#  endif
#  if !defined(LZ4_memmove)
#    error "LZ4_FREESTANDING requires macro 'LZ4_memmove'."
#  endif
#elif ! defined(LZ4_FREESTANDING)
#  define LZ4_FREESTANDING 0
#endif


/*------ Версия ------*/
#define LZ4_VERSION_MAJOR    1    /* за нарушение изменений интерфейса  */
#define LZ4_VERSION_MINOR   10    /* для новых (не ломающихся) возможностей интерфейса */
#define LZ4_VERSION_RELEASE  0    /* для доработок, исправлений ошибок или разработки */

#define LZ4_VERSION_NUMBER (LZ4_VERSION_MAJOR *100*100 + LZ4_VERSION_MINOR *100 + LZ4_VERSION_RELEASE)

#define LZ4_LIB_VERSION LZ4_VERSION_MAJOR.LZ4_VERSION_MINOR.LZ4_VERSION_RELEASE
#define LZ4_QUOTE(str) #str
#define LZ4_EXPAND_AND_QUOTE(str) LZ4_QUOTE(str)
#define LZ4_VERSION_STRING LZ4_EXPAND_AND_QUOTE(LZ4_LIB_VERSION)  /* требуется версия 1.7.3+ */

LZ4LIB_API int LZ4_versionNumber (void);  /**< library version number; useful to check dll version; requires v1.3.0+ */
LZ4LIB_API const char* LZ4_versionString (void);   /**< library version string; useful to check dll version; requires v1.7.5+ */


/*-************************************
*  Настройка использования памяти
**************************************/
/*!
 * LZ4_MEMORY_USAGE :
 * Можно выбрать во время компиляции, установив LZ4_MEMORY_USAGE .
 * Формула использования памяти: N->2^N байт (примеры: 10 -> 1 КБ; 12 -> 4 КБ; 16 -> 64 КБ; 20 -> 1 МБ)
 * Увеличение использования памяти улучшает степень сжатия, как правило, за счет скорости.
 * Уменьшение использования памяти может повысить скорость за счет уменьшения соотношения благодаря лучшей локальности кэша.
 * Значение по умолчанию — 14, что соответствует 16 КБ, что хорошо вписывается в большинство кешей L1.
 */
#ifndef LZ4_MEMORY_USAGE
# define LZ4_MEMORY_USAGE LZ4_MEMORY_USAGE_DEFAULT
#endif

/* Это абсолютные ограничения, пользователи не должны их изменять. */
#define LZ4_MEMORY_USAGE_MIN 10
#define LZ4_MEMORY_USAGE_DEFAULT 14
#define LZ4_MEMORY_USAGE_MAX 20

#if (LZ4_MEMORY_USAGE < LZ4_MEMORY_USAGE_MIN)
#  error "LZ4_MEMORY_USAGE is too small !"
#endif

#if (LZ4_MEMORY_USAGE > LZ4_MEMORY_USAGE_MAX)
#  error "LZ4_MEMORY_USAGE is too large !"
#endif

/*-************************************
*  Простые функции
**************************************/
/*! LZ4_compress_default () :
 *  Сжимает байты srcSize из буфера src.
 *  в уже выделенный буфер «dst» размером «dstCapacity».
 *  Сжатие гарантированно будет успешным, если 'dstCapacity' >= LZ4_compressBound (srcSize).
 *  Он также работает быстрее, поэтому это рекомендуемая настройка.
 *  Если функция не может сжать «src» в более ограниченный бюджет «dst»,
 *  сжатие прекращается *немедленно*, и результат функции равен нулю.
 *  В этом случае содержимое dst не определено (недействительно).
 *      srcSize : max supported value is LZ4_MAX_INPUT_SIZE.
 *      dstCapacity : size of buffer 'dst' (which must be already allocated)
 *     @return  : the number of bytes written into buffer 'dst' (necessarily <= dstCapacity)
 *                или 0, если сжатие не удалось
 * Note : This function is protected against buffer overflow scenarios (never writes outside 'dst' buffer, nor read outside 'source' buffer).
 */
LZ4LIB_API int LZ4_compress_default(const char* src, char* dst, int srcSize, int dstCapacity);

/*! LZ4_decompress_safe () :
 * @compressedSize: точный полный размер сжатого блока.
 * @dstCapacity: размер буфера назначения (который должен быть уже выделен),
 *                предполагал верхнюю границу распакованного размера.
 * @return : the number of bytes decompressed into destination buffer (necessarily <= dstCapacity)
 *           Если буфер назначения недостаточно велик, декодирование остановится и выведет код ошибки (отрицательное значение).
 *           Если исходный поток будет обнаружен искаженным, функция прекратит декодирование и вернет отрицательный результат.
 * Примечание 1. Эта функция защищена от вредоносных пакетов данных:
 *          он никогда не будет записывать вне буфера «dst» и не читать за пределами буфера «источника»,
 *          даже если сжатый блок был злонамеренно изменен, чтобы приказать декодеру выполнить эти действия.
 *          В таком случае декодер немедленно останавливается и считает сжатый блок искаженным.
 * Примечание 2: функции compressedSize и dstCapacity необходимо предоставить, сжатый блок их не содержит.
 *          Реализация может свободно отправлять/хранить/извлекать эту информацию любым наиболее выгодным способом.
 *          Если вам нужен другой формат, который объединяет как сжатые данные, так и их метаданные, рассмотрите возможность вместо этого использовать lz4frame.h.
 */
LZ4LIB_API int LZ4_decompress_safe (const char* src, char* dst, int compressedSize, int dstCapacity);


/*-************************************
*  Расширенные функции
**************************************/
#define LZ4_MAX_INPUT_SIZE        0x7E000000   /* 2 113 929 216 байт */
#define LZ4_COMPRESSBOUND(isize)  ((unsigned)(isize) > (unsigned)LZ4_MAX_INPUT_SIZE ? 0 : (isize) + ((isize)/255) + 16)

/*! LZ4_compressBound () :
    Обеспечивает максимальный размер, который может выдать сжатие LZ4 в «худшем случае» (входные данные не сжимаются).
    Эта функция в первую очередь полезна для целей распределения памяти (размера целевого буфера).
    Макрос LZ4_COMPRESSBOUND() также предусмотрен для оценки во время компиляции (например, выделения памяти стека).
    Обратите внимание, что LZ4_compress_default() сжимается быстрее, когда dstCapacity >= LZ4_compressBound (srcSize).
        inputSize  : max supported value is LZ4_MAX_INPUT_SIZE
        return : maximum output size in a "worst case" scenario
              или 0, если размер ввода неверен (слишком большой или отрицательный)
*/
LZ4LIB_API int LZ4_compressBound(int inputSize);

/*! LZ4_compress_fast () :
    То же, что LZ4_compress_default (), но позволяет выбрать коэффициент «ускорения».
    Чем больше значение ускорения, тем быстрее алгоритм, но и меньше сжатие.
    Это компромисс. Его можно точно настроить, при этом каждое последующее значение обеспечивает примерно +~3% к скорости.
    Значение ускорения «1» такое же, как у обычного LZ4_compress_default ()
    Значения <= 0 будут заменены на LZ4_ACCELERATION_DEFAULT (в настоящее время == 1, см. lz4.c).
    Значения > LZ4_ACCELERATION_MAX будут заменены на LZ4_ACCELERATION_MAX (в настоящее время == 65537, см. lz4.c ).
*/
LZ4LIB_API int LZ4_compress_fast (const char* src, char* dst, int srcSize, int dstCapacity, int acceleration);


/*! LZ4_compress_fast_extState () :
 *  То же, что и LZ4_compress_fast(), используя для своего состояния внешне выделенное пространство памяти.
 *  Используйте LZ4_sizeofState(), чтобы узнать, сколько памяти необходимо выделить,
 *  и распределите его по 8-байтовым границам (обычно используя `malloc()`).
 *  Затем предоставьте этот буфер как `void* state` для функции сжатия.
 */
LZ4LIB_API int LZ4_sizeofState(void);
LZ4LIB_API int LZ4_compress_fast_extState (void* state, const char* src, char* dst, int srcSize, int dstCapacity, int acceleration);

/*! LZ4_compress_destSize () :
 *  Обратная логика: сжимает как можно больше данных из буфера src.
 *  в уже выделенный буфер «dst» размером >= «dstCapacity».
 *  Эта функция либо сжимает все содержимое «src» в «dst», если оно достаточно велико,
 *  или полностью заполните буфер «dst» как можно большим количеством данных из «src».
 *  note: acceleration parameter is fixed to "default".
 *
 * *srcSizePtr : параметр ввода+вывода. Изначально содержит размер ввода.
 *               Будет изменено, чтобы указать, сколько байтов считывается из «src» для заполнения «dst».
 *               Новое значение обязательно <= входное значение.
 * @return : Nb bytes written into 'dst' (necessarily <= dstCapacity)
 *           или 0, если сжатие не удалось.
 *
 * Note : from v1.8.2 to v1.9.1, this function had a bug (fixed in v1.9.2+):
 *        произведенный сжатый контент может, в определенных обстоятельствах,
 *        необходимо распаковать в буфер назначения большего размера
 *        как минимум на 1 байт больше, чем содержимое, подлежащее распаковке.
 *        Если приложение использует `LZ4_compress_destSize()` ,
 *        настоятельно рекомендуется обновить liblz4 до версии 1.9.2 или выше.
 *        Если это невозможно сделать или обеспечить,
 *        принимающая функция декомпрессии должна обеспечивать
 *        dstCapacity, который > decompressedSize, по крайней мере, на 1 байт.
 *        Подробности см. в https://github.com/lz4/lz4/issues/859.
 */
LZ4LIB_API int LZ4_compress_destSize(const char* src, char* dst, int* srcSizePtr, int targetDstSize);

/*! LZ4_decompress_safe_partial () :
 *  Распакуйте сжатый блок LZ4 размером «srcSize» в позиции «src»,
 *  в буфер назначения «dst» размером «dstCapacity».
 *  Будет декодировано до байтов TargetOutputSize.
 *  Функция прекращает декодирование при достижении этой цели.
 *  Это может быть полезно для повышения производительности.
 *  всякий раз, когда требуется только начало блока.
 *
 * @return : the number of bytes decoded in `dst` (necessarily <= targetOutputSize)
 *           Если исходный поток обнаруживается некорректным, функция возвращает отрицательный результат.
 *
 *  Примечание 1: @return может быть < targetOutputSize, если сжатый блок содержит меньше данных.
 *
 *  Примечание 2: targetOutputSize должен быть <= dstCapacity.
 *
 *  Примечание 3: эта функция эффективно останавливает декодирование при достижении targetOutputSize,
 *           поэтому dstCapacity является избыточным.
 *           Это связано с тем, что в более старых версиях этой функции
 *           операция декодирования по-прежнему будет записывать полные последовательности.
 *           Поэтому не было никакой гарантии, что он перестанет писать именно на targetOutputSize,
 *           он мог бы записать больше байтов, но только до dstCapacity.
 *           Раньше для правильной работы этой операции требовался некоторый «запас».
 *           К счастью, в этом больше нет необходимости.
 *           Тем не менее, функция сохраняет ту же сигнатуру, чтобы сохранить совместимость с API.
 *
 *  Примечание 4. Если srcSize — точный размер блока,
 *           тогда targetOutputSize может быть любым значением,
 *           в том числе больше, чем размер распакованного блока.
 *           Функция в лучшем случае сгенерирует распакованный размер блока.
 *
 *  Примечание 5. Если srcSize равен _larger_, чем сжатый размер блока,
 *           тогда targetOutputSize ** MUST ** будет <= размер распакованного блока.
 *           В противном случае *произойдет тихое повреждение*.
 */
LZ4LIB_API int LZ4_decompress_safe_partial (const char* src, char* dst, int srcSize, int targetOutputSize, int dstCapacity);


/*-*********************************************
*  Функции потокового сжатия
***********************************************/
typedef union LZ4_stream_u LZ4_stream_t;  /* неполный тип (определен позже) */

/*!
 Примечание о RC_INVOKED

 - RC_INVOKED is predefined symbol of rc.exe (the resource compiler which is part of MSVC/Visual Studio).
   https://docs.microsoft.com/en-us/windows/win32/menurc/predefined-macros

 - Since rc.exe is a legacy compiler, it truncates long symbol (> 30 chars)
   и сообщает предупреждение «RC4011: идентификатор усечен».

 - To eliminate the warning, we surround long preprocessor symbol with
   " #if !define( RC_INVOKED ) ... #endif " блок, который означает
   «пропустить этот блок, когда rc.exe пытается его прочитать».
*/
#if !defined(RC_INVOKED) /* https://docs.microsoft.com/en-us/windows/win32/menurc/predefined-macros */
#if !defined(LZ4_STATIC_LINKING_ONLY_DISABLE_MEMORY_ALLOCATION)
LZ4LIB_API LZ4_stream_t* LZ4_createStream(void);
LZ4LIB_API int           LZ4_freeStream (LZ4_stream_t* streamPtr);
#endif /* !defined(LZ4_STATIC_LINKING_ONLY_DISABLE_MEMORY_ALLOCATION) */
#endif

/*! LZ4_resetStream_fast () : v1.9.0+
 *  Используйте это, чтобы подготовить LZ4_stream_t для новой цепочки зависимых блоков.
 *  (например, LZ4_compress_fast_continue ()).
 *
 *  LZ4_stream_t должен быть инициализирован один раз перед использованием.
 *  Это делается автоматически при создании LZ4_createStream().
 *  Однако если LZ4_stream_t просто объявить в стеке (например),
 *  необходимо сначала его инициализировать, используя LZ4_initStream().
 *
 *  После инициализации запускайте любой новый поток с помощью LZ4_resetStream_fast().
 *  Один и тот же LZ4_stream_t можно использовать несколько раз подряд.
 *  и сжимать несколько потоков,
 *  при условии, что каждый новый поток он начинает с LZ4_resetStream_fast().
 *
 *  LZ4_resetStream_fast() намного быстрее, чем LZ4_initStream(),
 *  но несовместим с областями памяти, содержащими мусорные данные.
 *
 *  Note: it's only useful to call LZ4_resetStream_fast()
 *        в контексте потокового сжатия.
 *        Функции *extState* выполняют собственный сброс.
 *        Вызов LZ4_resetStream_fast() раньше является избыточным и даже контрпродуктивным.
 */
LZ4LIB_API void LZ4_resetStream_fast (LZ4_stream_t* streamPtr);

/*! LZ4_loadDict () :
 *  Используйте эту функцию для ссылки на статический словарь в LZ4_stream_t.
 *  Словарь должен оставаться доступным во время сжатия.
 *  LZ4_loadDict() запускает сброс, поэтому все предыдущие данные будут забыты.
 *  Тот же словарь должен быть загружен на стороне распаковки для успешного декодирования.
 *  Словарь полезен для лучшего сжатия небольших данных (диапазон KB).
 *  Хотя LZ4 сам принимает любые входные данные в качестве словаря, эффективность словаря также является предметом обсуждения.
 *  Если у вас есть сомнения, воспользуйтесь конструктором словарей Zstandard.
 *  Загрузка размера 0 разрешена и аналогична сбросу.
 * @return : loaded dictionary size, in bytes (note: only the last 64 KB are loaded)
 */
LZ4LIB_API int LZ4_loadDict (LZ4_stream_t* streamPtr, const char* dictionary, int dictSize);

/*! LZ4_loadDictSlow () : v1.10.0+
 *  То же, что LZ4_loadDict (),
 *  но использует немного больше процессора для более тщательного обращения к содержимому словаря.
 *  Ожидается, что это немного улучшит степень сжатия.
 *  Дополнительные затраты на процессор, вероятно, окупятся, если словарь будет повторно использоваться в нескольких сеансах.
 * @return : loaded dictionary size, in bytes (note: only the last 64 KB are loaded)
 */
LZ4LIB_API int LZ4_loadDictSlow(LZ4_stream_t* streamPtr, const char* dictionary, int dictSize);

/*! LZ4_attach_dictionary (): стабильно с версии 1.10.0
 *
 *  Это позволяет эффективно повторно использовать статический словарь несколько раз.
 *
 *  Вместо перезагрузки буфера словаря в рабочий контекст перед
 *  каждое сжатие или копирование LZ4_stream_t предварительно загруженного словаря в
 *  работает LZ4_stream_t , эта функция представляет механизм установки без копирования,
 *  в котором рабочий поток ссылается на @dictionaryStream на месте.
 *
 *  Сделано несколько предположений о состоянии @dictionaryStream.
 *  В настоящее время доступны только состояния, подготовленные LZ4_loadDict () или
 *  Ожидается, что LZ4_loadDictSlow() будет работать.
 *
 *  В качестве альтернативы предоставленный @dictionaryStream может быть NULL ,
 *  в этом случае любой существующий поток словаря не установлен.
 *
 *  Если предоставляется словарь, он заменяет любую ранее существовавшую историю потока.
 *  Содержимое словаря — единственная история, на которую можно ссылаться и
 *  логически непосредственно предшествуют данным, сжатым в первом последующем
 *  вызов сжатия.
 *
 *  Словарь останется подключенным к рабочему потоку только через
 *  первый вызов сжатия, в конце которого он очищается.
 * Поток @dictionaryStream (и исходный буфер) должен оставаться на месте/доступным/неизменным.
 *  после завершения сеанса сжатия.
 *
 *  Note: there is no equivalent LZ4_attach_*() method on the decompression side
 *  поскольку нет затрат на инициализацию, следовательно, нет необходимости распределять затраты между несколькими сеансами.
 *  Чтобы распаковать блоки LZ4 с использованием словаря, прикрепленного или нет,
 *  просто используйте обычный LZ4_setStreamDecode() для потоковой передачи,
 *  или без сохранения состояния LZ4_decompress_safe_usingDict() для однократной декомпрессии.
 */
LZ4LIB_API void
LZ4_attach_dictionary(LZ4_stream_t* workingStream,
                const LZ4_stream_t* dictionaryStream);

/*! LZ4_compress_fast_continue () :
 *  Сжимайте содержимое «src», используя данные из ранее сжатых блоков, для повышения степени сжатия.
 * Буфер dst должен быть уже выделен.
 *  Если dstCapacity >= LZ4_compressBound (srcSize), сжатие гарантированно пройдет успешно и будет выполняться быстрее.
 *
 * @return : size of compressed block
 *           или 0, если есть ошибка (обычно не вписывается в «dst»).
 *
 *  Примечание 1. Каждый вызов LZ4_compress_fast_continue() генерирует новый блок.
 *           Каждый блок имеет четкие границы.
 *           Каждый блок необходимо распаковывать отдельно, вызывая LZ4_decompress_ *() с соответствующими метаданными.
 *           Невозможно соединить блоки вместе и ожидать, что один вызов LZ4_decompress_ *() распакует их вместе.
 *
 *  Примечание 2. Предыдущие 64 КБ исходных данных — это __assumed__, и они остаются в неизмененном виде по тому же адресу в памяти!
 *
 *  Примечание 3. Когда ввод структурирован как двойной буфер, каждый буфер может иметь любой размер, включая <64 KB.
 *           Убедитесь, что буферы разделены хотя бы одним байтом.
 *           Эта конструкция гарантирует, что каждый блок зависит только от предыдущего блока.
 *
 *  Примечание 4. Если входной буфер является кольцевым, он может иметь любой размер, включая <64 KB.
 *
 *  Примечание 5: После ошибки статус потока не определен (недействителен), его можно только сбросить или освободить.
 */
LZ4LIB_API int LZ4_compress_fast_continue (LZ4_stream_t* streamPtr, const char* src, char* dst, int srcSize, int dstCapacity, int acceleration);

/*! LZ4_saveDict () :
 *  Если нельзя гарантировать, что последние 64 КБ останутся доступными в текущей ячейке памяти,
 *  сохраните его в более безопасном месте (char*safeBuffer).
 *  Это схематически эквивалентно memcpy(), за которым следует LZ4_loadDict (),
 *  но гораздо быстрее, поскольку LZ4_saveDict() не нужно перестраивать таблицы.
 * @return : saved dictionary size in bytes (necessarily <= maxDictSize), or 0 if error.
 */
LZ4LIB_API int LZ4_saveDict (LZ4_stream_t* streamPtr, char* safeBuffer, int maxDictSize);


/*-**********************************************
*  Функции потоковой декомпрессии
*  Безбуферный синхронный API
************************************************/
typedef union LZ4_streamDecode_u LZ4_streamDecode_t;   /* контекст отслеживания */

/*! LZ4_createStreamDecode () и LZ4_freeStreamDecode () :
 *  создание/уничтожение контекста отслеживания потоковой декомпрессии.
 *  Контекст отслеживания можно использовать повторно несколько раз.
 */
#if !defined(RC_INVOKED) /* https://docs.microsoft.com/en-us/windows/win32/menurc/predefined-macros */
#if !defined(LZ4_STATIC_LINKING_ONLY_DISABLE_MEMORY_ALLOCATION)
LZ4LIB_API LZ4_streamDecode_t* LZ4_createStreamDecode(void);
LZ4LIB_API int                 LZ4_freeStreamDecode (LZ4_streamDecode_t* LZ4_stream);
#endif /* !defined(LZ4_STATIC_LINKING_ONLY_DISABLE_MEMORY_ALLOCATION) */
#endif

/*! LZ4_setStreamDecode () :
 *  Контекст LZ4_streamDecode_t можно выделить один раз и повторно использовать несколько раз.
 *  Используйте эту функцию, чтобы начать распаковку нового потока блоков.
 *  При желании можно установить словарь. Используйте NULL или размер 0 для команды сброса.
 *  Словарь считается стабильным: он должен оставаться доступным и неизмененным во время следующей распаковки.
 * @return : 1 if OK, 0 if error
 */
LZ4LIB_API int LZ4_setStreamDecode (LZ4_streamDecode_t* LZ4_streamDecode, const char* dictionary, int dictSize);

/*! LZ4_decoderRingBufferSize () : v1.8.2+
 *  Note : in a ring buffer scenario (optional),
 *  блоки считаются распакованными рядом друг с другом
 *  на данный момент не хватает места для следующего блока (remainingSize < maxBlockSize),
 *  на каком этапе он возобновляется с начала кольцевого буфера.
 *  При установке такого кольцевого буфера для потоковой декомпрессии
 *  обеспечивает минимальный размер этого кольцевого буфера
 *  быть совместимым с любым источником, соблюдающим условие maxBlockSize.
 * @return : minimum ring buffer size,
 *           или 0, если произошла ошибка (неверный maxBlockSize).
 */
LZ4LIB_API int LZ4_decoderRingBufferSize(int maxBlockSize);
#define LZ4_DECODER_RING_BUFFER_SIZE(maxBlockSize) (65536 + 14 + (maxBlockSize))  /* для статического размещения; maxBlockSize считается действительным */

/*! LZ4_decompress_safe_continue () :
 *  Эта функция декодирования позволяет распаковывать последовательные блоки в «потоковом» режиме.
 *  Отличие от обычных независимых блоков в том, что
 *  новым блокам разрешено находить ссылки на предыдущие блоки.
 *  Блок представляет собой неделимый объект и должен быть полностью представлен функции декомпрессии.
 *  LZ4_decompress_safe_continue() принимает только один блок за раз.
 *  Он создан по образцу `LZ4_decompress_safe()` и ведет себя аналогично.
 *
 * @LZ 4_streamDecode: состояние декомпрессии, отслеживание положения в памяти прошлых данных.
 * @compressedSize: точный полный размер одного сжатого блока.
 * @dstCapacity: размер буфера назначения (который должен быть уже выделен),
 *                должна быть верхней границей распакованного размера.
 * @return : number of bytes decompressed into destination buffer (necessarily <= dstCapacity)
 *           Если буфер назначения недостаточно велик, декодирование остановится и выведет код ошибки (отрицательное значение).
 *           Если исходный поток будет обнаружен искаженным, функция прекратит декодирование и вернет отрицательный результат.
 *
 *  Последние 64 КБ ранее декодированных данных *должны* оставаться доступными и неизмененными.
 *  в той позиции памяти, где они были ранее декодированы.
 *  Если было декодировано менее 64 КБ данных, все данные должны присутствовать.
 *
 *  Special : if decompression side sets a ring buffer, it must respect one of the following conditions :
 *  - Decompression buffer size is _at least_ LZ4_decoderRingBufferSize(maxBlockSize).
 *    maxBlockSize — максимальный размер любого отдельного блока. Он может иметь любое значение > 16 байт.
 *    В этом случае буферы кодирования и декодирования не нуждаются в синхронизации.
 *    Фактически данные могут быть получены из любого источника, соответствующего спецификации формата LZ4 и соблюдающего maxBlockSize.
 *  - Synchronized mode :
 *    Размер буфера декомпрессии равен _exactly_ и соответствует размеру буфера сжатия.
 *    и следует точно такому же правилу обновления (границы блоков в одних и тех же позициях),
 *    и функция декодирования обеспечивает точный распакованный размер каждого блока (за исключением последнего блока потока),
 *    Кольцевой буфер декодирования и кодирования _then_ может иметь любой размер, в том числе небольшой (<64 KB).
 *  - Decompression buffer is larger than encoding buffer, by a minimum of maxBlockSize more bytes.
 *    В этом случае буферы кодирования и декодирования не нужно синхронизировать,
 *    а кольцевой буфер кодирования может иметь любой размер, в том числе и маленький (<64 KB).
 *
 *  Если эти условия невозможны,
 *  сохранить последние 64 КБ декодированных данных в безопасный буфер, где их нельзя будет изменить во время распаковки,
 *  затем укажите, где эти данные сохраняются, используя LZ4_setStreamDecode(), прежде чем распаковывать следующий блок.
*/
LZ4LIB_API int
LZ4_decompress_safe_continue (LZ4_streamDecode_t* LZ4_streamDecode,
                        const char* src, char* dst,
                        int srcSize, int dstCapacity);


/*! LZ4_decompress_safe_usingDict () :
 *  Работает так же, как
 *  комбинация LZ4_setStreamDecode () с последующим LZ4_decompress_safe_continue ()
 *  Однако он не имеет состояния: ему не требуется состояние LZ4_streamDecode_t.
 *  Словарь считается стабильным: он должен оставаться доступным и неизмененным во время распаковки.
 *  Совет по производительности: скорость декомпрессии можно существенно увеличить.
 *                    когда dst == dictStart + dictSize.
 */
LZ4LIB_API int
LZ4_decompress_safe_usingDict(const char* src, char* dst,
                              int srcSize, int dstCapacity,
                              const char* dictStart, int dictSize);

/*! LZ4_decompress_safe_partial_usingDict () :
 *  Ведет себя так же, как LZ4_decompress_safe_partial ()
 *  с добавленной возможностью указать сегмент памяти для прошлых данных.
 *  Совет по производительности: скорость декомпрессии можно существенно увеличить.
 *                    когда dst == dictStart + dictSize.
 */
LZ4LIB_API int
LZ4_decompress_safe_partial_usingDict(const char* src, char* dst,
                                      int compressedSize,
                                      int targetOutputSize, int maxOutputSize,
                                      const char* dictStart, int dictSize);

#endif /* LZ4_H_2983827168210 */


/*^*************************************
 * !!!!!!   STATIC LINKING ONLY   !!!!!!
 ***************************************/

/*-****************************************************************************
 * Экспериментальный раздел
 *
 * Символы, заявленные в этом разделе, следует считать нестабильными. Их
 * сигнатуры или семантика могут измениться или могут быть полностью удалены в
 * будущее. Поэтому их безопасно полагаться только на то, когда вызывающий абонент
 * статически связан с библиотекой.
 *
 * Для защиты от небезопасного использования охраняются не только декларации,
 * определения скрыты по умолчанию
 * при сборке LZ4 как общей/динамической библиотеки.
 *
 * Чтобы получить доступ к этим объявлениям,
 * определите LZ4_STATIC_LINKING_ONLY в своем приложении
 * перед включением заголовков LZ4.
 *
 * Чтобы сделать их реализации доступными динамически, вы должны
 * определите LZ4_PUBLISH_STATIC_FUNCTIONS при сборке библиотеки LZ4.
 ******************************************************************************/

#ifdef LZ4_STATIC_LINKING_ONLY

#ifndef LZ4_STATIC_3504398509
#define LZ4_STATIC_3504398509

#ifdef LZ4_PUBLISH_STATIC_FUNCTIONS
# define LZ4LIB_STATIC_API LZ4LIB_API
#else
# define LZ4LIB_STATIC_API
#endif


/*! LZ4_compress_fast_extState_fastReset () :
 *  Вариант LZ4_compress_fast_extState().
 *
 *  Использование этого варианта позволяет избежать дорогостоящего этапа инициализации.
 *  Вызов безопасен только в том случае, если известно, что буфер состояния уже правильно инициализирован.
 *  (см. выше комментарий к LZ4_resetStream_fast () для определения «правильно инициализирован»).
 *  На высоком уровне разница в том, что
 *  эта функция инициализирует предоставленное состояние вызовом чего-то вроде LZ4_resetStream_fast ()
 *  тогда как LZ4_compress_fast_extState() начинается с вызова LZ4_resetStream().
 */
LZ4LIB_STATIC_API int LZ4_compress_fast_extState_fastReset (void* state, const char* src, char* dst, int srcSize, int dstCapacity, int acceleration);

/*! LZ4_compress_destSize_extState (): представлено в версии 1.10.0.
 *  То же, что LZ4_compress_destSize(), но с использованием внешнего выделенного состояния.
 *  Also: exposes @acceleration
 */
int LZ4_compress_destSize_extState(void* state, const char* src, char* dst, int* srcSizePtr, int targetDstSize, int acceleration);

/*! Сжатие и декомпрессия на месте
 *
 * Возможно, что ввод и вывод используют один и тот же буфер,
 * для сред с очень ограниченным объемом памяти.
 * В обоих случаях требуется, чтобы ввод находился в конце буфера.
 * и распаковка начнется с начала буфера.
 * Размер буфера должен иметь некоторый запас и, следовательно, быть больше окончательного размера.
 *
 * |<------------------------буфер--------------------------------->|
 *                             |<-----------сжатые данные--------->|
 * |<-----------распакованный размер------------------>|
 *                                                  |<----поле---->|
 *
 * Этот метод более полезен для декомпрессии,
 * поскольку размер в распакованном виде обычно больше,
 * и маржа короткая.
 *
 * Распаковка на месте будет работать внутри любого буфера.
 * размер которого >= LZ4_DECOMPRESS_INPLACE_BUFFER_SIZE (decompressedSize).
 * Это предполагает, что decompressedSize > compressedSize.
 * В противном случае это означает сжатие фактически расширенных данных,
 * и было бы более эффективно хранить такие данные с флагом, указывающим, что они не сжаты.
 * Это может произойти, если данные несжимаемы (уже сжаты или зашифрованы).
 *
 * Для сжатия на месте запас больше, так как он должен быть в состоянии справиться с обоими
 * сохранение истории, требующее, чтобы входные данные оставались неизмененными до LZ4_DISTANCE_MAX ,
 * и расширение данных, которое может произойти, если входные данные не сжимаемы.
 * Как следствие, требования к размеру буфера намного выше,
 * а экономия памяти, обеспечиваемая сжатием на месте, более ограничена.
 *
 * Существуют способы ограничить эту стоимость сжатия:
 * - Reduce history size, by modifying LZ4_DISTANCE_MAX.
 *   Обратите внимание, что это константа времени компиляции, поэтому все сжатия будут применять это ограничение.
 *   Более низкие значения уменьшат степень сжатия, за исключением случаев, когда input_size < LZ4_DISTANCE_MAX ,
 *   так что это разумный трюк, когда известно, что входные данные малы.
 * - Require the compressor to deliver a "maximum compressed size".
 *   Это параметр `dstCapacity` в `LZ4_compress*()`.
 *   Если этот размер < LZ4_COMPRESSBOUND (inputSize), сжатие может завершиться неудачно.
 *   в этом случае код возврата будет 0 (ноль).
 *   Звонящий должен быть готов к тому, что такие случаи произойдут,
 *   и обычно разрабатывают схему резервного копирования для отправки данных в несжатом виде.
 * Сочетание обоих методов позволяет значительно снизить
 * размер запаса, необходимый для сжатия на месте.
 *
 * Сжатие на месте может работать в любом буфере.
 * какой размер >= (maxCompressedSize)
 * с maxCompressedSize == LZ4_COMPRESSBOUND (srcSize) для гарантированного успешного сжатия.
 * LZ4_COMPRESS_INPLACE_BUFFER_SIZE() зависит как от maxCompressedSize, так и от LZ4_DISTANCE_MAX,
 * поэтому можно уменьшить требования к памяти, играя с ними.
 */

#define LZ4_DECOMPRESS_INPLACE_MARGIN(compressedSize)          (((compressedSize) >> 8) + 32)
#define LZ4_DECOMPRESS_INPLACE_BUFFER_SIZE(decompressedSize)   ((decompressedSize) + LZ4_DECOMPRESS_INPLACE_MARGIN(decompressedSize))  /**< note: presumes that compressedSize < decompressedSize. note2: margin is overestimated a bit, since it could use compressedSize instead */

#ifndef LZ4_DISTANCE_MAX   /* размер окна истории; может быть определен пользователем во время компиляции */
#  define LZ4_DISTANCE_MAX 65535   /* по умолчанию установлено максимальное значение */
#endif

#define LZ4_COMPRESS_INPLACE_MARGIN                           (LZ4_DISTANCE_MAX + 32)   /* LZ4_DISTANCE_MAX можно безопасно заменить на srcSize, если он меньше. */
#define LZ4_COMPRESS_INPLACE_BUFFER_SIZE(maxCompressedSize)   ((maxCompressedSize) + LZ4_COMPRESS_INPLACE_MARGIN)  /**< maxCompressedSize is generally LZ4_COMPRESSBOUND(inputSize), but can be set to any lower value, with the risk that compression can fail (return code 0(zero)) */

#endif   /* LZ4_STATIC_3504398509 */
#endif   /* LZ4_STATIC_LINKING_ONLY */



#ifndef LZ4_H_98237428734687
#define LZ4_H_98237428734687

/*-************************************************************
 *  Частные определения
 **************************************************************
 * Не используйте эти определения напрямую.
 * Они доступны только для статического выделения `LZ4_stream_t` и `LZ4_streamDecode_t`.
 * Доступ к членам приведет к тому, что пользовательский код подвергнется взлому API и/или ABI в будущих версиях библиотеки.
 **************************************************************/
#define LZ4_HASHLOG   (LZ4_MEMORY_USAGE-2)
#define LZ4_HASHTABLESIZE (1 << LZ4_MEMORY_USAGE)
#define LZ4_HASH_SIZE_U32 (1 << LZ4_HASHLOG)       /* требуется как макрос для статического распределения */

#if defined(__cplusplus) || (defined (__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) /* C99 */)
# include <stdint.h>
  typedef  int8_t  LZ4_i8;
  typedef uint8_t  LZ4_byte;
  typedef uint16_t LZ4_u16;
  typedef uint32_t LZ4_u32;
#else
  typedef   signed char  LZ4_i8;
  typedef unsigned char  LZ4_byte;
  typedef unsigned short LZ4_u16;
  typedef unsigned int   LZ4_u32;
#endif

/*! LZ4_stream_t :
 *  Никогда не используйте приведенные ниже внутренние определения напрямую!
 *  Эти определения не безопасны для API/ABI и могут измениться в будущих версиях.
 *  Если вам нужно статическое выделение, объявите или выделите объект LZ4_stream_t.
**/

typedef struct LZ4_stream_t_internal LZ4_stream_t_internal;
struct LZ4_stream_t_internal {
    LZ4_u32 hashTable[LZ4_HASH_SIZE_U32];
    const LZ4_byte* dictionary;
    const LZ4_stream_t_internal* dictCtx;
    LZ4_u32 currentOffset;
    LZ4_u32 tableType;
    LZ4_u32 dictSize;
    /* Неявное заполнение для обеспечения выравнивания структуры. */
};

#define LZ4_STREAM_MINSIZE  ((1UL << (LZ4_MEMORY_USAGE)) + 32)  /* статический размер, для совместимости между версиями */
union LZ4_stream_u {
    char minStateSize[LZ4_STREAM_MINSIZE];
    LZ4_stream_t_internal internal_donotuse;
}; /* ранее было определено значение LZ4_stream_t */


/*! LZ4_initStream () : v1.9.0+
 *  Структура LZ4_stream_t должна быть инициализирована хотя бы один раз.
 *  Это делается автоматически при вызове LZ4_createStream(),
 *  но это не так, когда структура просто объявлена в стеке (например).
 *
 *  Используйте LZ4_initStream() для правильной инициализации вновь объявленного LZ4_stream_t.
 *  Он также может инициализировать любой произвольный буфер достаточного размера.
 *  и при инициализации @return будет указателем правильного типа.
 *
 *  Note : initialization fails if size and alignment conditions are not respected.
 *         В этом случае функция будет @return NULL .
 *  Note2: An LZ4_stream_t structure guarantees correct alignment and size.
 *  Note3: Before v1.9.0, use LZ4_resetStream() instead
**/
LZ4LIB_API LZ4_stream_t* LZ4_initStream (void* stateBuffer, size_t size);


/*! LZ4_streamDecode_t :
 *  Никогда не используйте приведенные ниже внутренние определения напрямую!
 *  Эти определения не безопасны для API/ABI и могут измениться в будущих версиях.
 *  Если вам нужно статическое выделение, объявите или выделите объект LZ4_streamDecode_t.
**/
typedef struct {
    const LZ4_byte* externalDict;
    const LZ4_byte* prefixEnd;
    size_t extDictSize;
    size_t prefixSize;
} LZ4_streamDecode_t_internal;

#define LZ4_STREAMDECODE_MINSIZE 32
union LZ4_streamDecode_u {
    char minStateSize[LZ4_STREAMDECODE_MINSIZE];
    LZ4_streamDecode_t_internal internal_donotuse;
} ;   /* ранее было определено значение LZ4_streamDecode_t */



/*-************************************
*  Устаревшие функции
**************************************/

/*! Предупреждения об устаревании
 *
 *  Устаревшие функции заставляют компилятор генерировать предупреждение при вызове.
 *  Это сделано для того, чтобы предложить пользователям обновить свой исходный код.
 *  Если предупреждения об устаревании вызывают проблемы, их, как правило, можно отключить.
 *  обычно с -Wno-deprecated-declarations для gcc
 *  или _CRT_SECURE_NO_WARNINGS в Visual.
 *
 *  Другой метод — определить LZ4_DISABLE_DEPRECATE_WARNINGS.
 *  перед включением файла заголовка.
 */
#ifdef LZ4_DISABLE_DEPRECATE_WARNINGS
#  define LZ4_DEPRECATED(message)   /* отключить предупреждения об устаревании */
#else
#  if defined (__cplusplus) && (__cplusplus >= 201402) /* С++ 14 или выше */
#    define LZ4_DEPRECATED(message) [[deprecated(message)]]
#  elif defined(_MSC_VER)
#    define LZ4_DEPRECATED(message) __declspec(deprecated(message))
#  elif defined(__clang__) || (defined(__GNUC__) && (__GNUC__ * 10 + __GNUC_MINOR__ >= 45))
#    define LZ4_DEPRECATED(message) __attribute__((deprecated(message)))
#  elif defined(__GNUC__) && (__GNUC__ * 10 + __GNUC_MINOR__ >= 31)
#    define LZ4_DEPRECATED(message) __attribute__((deprecated))
#  else
#    pragma message("WARNING: LZ4_DEPRECATED needs custom implementation for this compiler")
#    define LZ4_DEPRECATED(message)   /* отключен */
#  endif
#endif /* LZ4_DISABLE_DEPRECATE_WARNINGS */

/*! Obsolete compression functions (since v1.7.3) */
LZ4_DEPRECATED("use LZ4_compress_default() instead")       LZ4LIB_API int LZ4_compress               (const char* src, char* dest, int srcSize);
LZ4_DEPRECATED("use LZ4_compress_default() instead")       LZ4LIB_API int LZ4_compress_limitedOutput (const char* src, char* dest, int srcSize, int maxOutputSize);
LZ4_DEPRECATED("use LZ4_compress_fast_extState() instead") LZ4LIB_API int LZ4_compress_withState               (void* state, const char* source, char* dest, int inputSize);
LZ4_DEPRECATED("use LZ4_compress_fast_extState() instead") LZ4LIB_API int LZ4_compress_limitedOutput_withState (void* state, const char* source, char* dest, int inputSize, int maxOutputSize);
LZ4_DEPRECATED("use LZ4_compress_fast_continue() instead") LZ4LIB_API int LZ4_compress_continue                (LZ4_stream_t* LZ4_streamPtr, const char* source, char* dest, int inputSize);
LZ4_DEPRECATED("use LZ4_compress_fast_continue() instead") LZ4LIB_API int LZ4_compress_limitedOutput_continue  (LZ4_stream_t* LZ4_streamPtr, const char* source, char* dest, int inputSize, int maxOutputSize);

/*! Obsolete decompression functions (since v1.8.0) */
LZ4_DEPRECATED("use LZ4_decompress_fast() instead") LZ4LIB_API int LZ4_uncompress (const char* source, char* dest, int outputSize);
LZ4_DEPRECATED("use LZ4_decompress_safe() instead") LZ4LIB_API int LZ4_uncompress_unknownOutputSize (const char* source, char* dest, int isize, int maxOutputSize);

/* Устаревшие функции потоковой передачи (начиная с версии 1.7.0)
 * ухудшенная функциональность; не используйте!
 *
 * Для выполнения потокового сжатия эти функции зависели от данных.
 * это больше не отслеживается в штате. Они сохранились, а также
 * possible: using them will still produce a correct output. However, they don't
 * фактически сохраняет всю историю между вызовами сжатия. Степень сжатия
 * Таким образом, достигнутое будет не лучше, чем сжатие каждого фрагмента
 * независимо.
 */
LZ4_DEPRECATED("Use LZ4_createStream() instead") LZ4LIB_API void* LZ4_create (char* inputBuffer);
LZ4_DEPRECATED("Use LZ4_createStream() instead") LZ4LIB_API int   LZ4_sizeofStreamState(void);
LZ4_DEPRECATED("Use LZ4_resetStream() instead")  LZ4LIB_API int   LZ4_resetStreamState(void* state, char* inputBuffer);
LZ4_DEPRECATED("Use LZ4_saveDict() instead")     LZ4LIB_API char* LZ4_slideInputBuffer (void* state);

/*! Obsolete streaming decoding functions (since v1.7.0) */
LZ4_DEPRECATED("use LZ4_decompress_safe_usingDict() instead") LZ4LIB_API int LZ4_decompress_safe_withPrefix64k (const char* src, char* dst, int compressedSize, int maxDstSize);
LZ4_DEPRECATED("use LZ4_decompress_fast_usingDict() instead") LZ4LIB_API int LZ4_decompress_fast_withPrefix64k (const char* src, char* dst, int originalSize);

/*! Устаревшие варианты LZ4_decompress_fast (начиная с версии 1.9.0):
 *  Раньше эти функции были быстрее, чем LZ4_decompress_safe(),
 *  но это уже не так. Теперь они медленнее.
 *  Это связано с тем, что LZ4_decompress_fast() не знает входного размера,
 *  и поэтому необходимо более осторожно переходить во входной буфер, чтобы не прочитать дальше конца блока.
 *  Кроме того, `LZ4_decompress_fast()` не защищен от искаженных или вредоносных входных данных, что делает его уязвимым для безопасности.
 *  Как следствие, использование LZ4_decompress_fast() категорически не рекомендуется и считается устаревшим.
 *
 *  Последняя оставшаяся особенность LZ4_decompress_fast() заключается в том, что
 *  он может распаковать блок, не зная его сжатого размера.
 *  Такая функциональность может быть достигнута более безопасным способом.
 *  используя LZ4_decompress_safe_partial ().
 *
 *  Параметры:
 *  originalSize : is the uncompressed size to regenerate.
 *                 `dst` должен быть уже выделен, его размер должен быть >= 'originalSize' байт.
 * @return : number of bytes read from source buffer (== compressed size).
 *           Функция ожидает завершения точно в конце блока.
 *           Если исходный поток обнаруживается искаженным, функция прекращает декодирование и возвращает отрицательный результат.
 *  note : LZ4_decompress_fast*() requires originalSize. Thanks to this information, it never writes past the output buffer.
 *         Однако, поскольку он не знает своего размера «src», он может прочитать неизвестный объем входных данных за пределами границ входного буфера.
 *         Кроме того, поскольку смещения совпадений не проверяются, чтение совпадений из «src» также может привести к потере значения.
 *         Эти проблемы никогда не возникают, если входные (сжатые) данные верны.
 *         Но они могут произойти, если входные данные недействительны (ошибка или намеренное вмешательство).
 *         Как следствие, используйте эти функции в доверенных средах **только** с доверенными данными.
 */
LZ4_DEPRECATED("This function is deprecated and unsafe. Consider using LZ4_decompress_safe_partial() instead")
LZ4LIB_API int LZ4_decompress_fast (const char* src, char* dst, int originalSize);
LZ4_DEPRECATED("This function is deprecated and unsafe. Consider migrating towards LZ4_decompress_safe_continue() instead. "
               "Note that the contract will change (requires block's compressed size, instead of decompressed size)")
LZ4LIB_API int LZ4_decompress_fast_continue (LZ4_streamDecode_t* LZ4_streamDecode, const char* src, char* dst, int originalSize);
LZ4_DEPRECATED("This function is deprecated and unsafe. Consider using LZ4_decompress_safe_partial_usingDict() instead")
LZ4LIB_API int LZ4_decompress_fast_usingDict (const char* src, char* dst, int originalSize, const char* dictStart, int dictSize);

/*! LZ4_resetStream () :
 *  Структура LZ4_stream_t должна быть инициализирована хотя бы один раз.
 *  Это делается с помощью LZ4_initStream(), или LZ4_resetStream().
 *  Рассмотрите возможность перехода на LZ4_initStream(),
 *  вызов LZ4_resetStream() вызовет предупреждения об устаревании в будущем.
 */
LZ4LIB_API void LZ4_resetStream (LZ4_stream_t* streamPtr);


#endif /* LZ4_H_98237428734687 */


#if defined (__cplusplus)
}
#endif

#endif /* LV_USE_LZ4_INTERNAL */

