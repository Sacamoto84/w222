/**
 * @file lv_fs.h
 *
 */

#ifndef LV_FS_H
#define LV_FS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/
#define LV_FS_MAX_FN_LENGTH 64
#define LV_FS_MAX_PATH_LENGTH 256

#define LV_FS_CACHE_FROM_BUFFER   UINT32_MAX

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Ошибки в модуле файловой системы.
 */
typedef enum {
    LV_FS_RES_OK = 0,
    LV_FS_RES_HW_ERR,     /*Аппаратная ошибка низкого уровня*/
    LV_FS_RES_FS_ERR,     /*Ошибка в структуре файловой системы*/
    LV_FS_RES_NOT_EX,     /*Драйвер, файл или каталог не существует*/
    LV_FS_RES_FULL,       /*Диск заполнен*/
    LV_FS_RES_LOCKED,     /*Файл уже открыт*/
    LV_FS_RES_DENIED,     /*Доступ запрещен. Проверьте режимы «fs_open» и защиту от записей.*/
    LV_FS_RES_BUSY,       /*Файловая система теперь не может справиться с этим, попробуйте позже.*/
    LV_FS_RES_TOUT,       /*Время процесса истекло*/
    LV_FS_RES_NOT_IMP,    /*Запрошенная функция не реализована*/
    LV_FS_RES_OUT_OF_MEM, /*Недостаточно памяти для внутренней операции*/
    LV_FS_RES_INV_PARAM,  /*Неверный параметр среди аргументов*/
    LV_FS_RES_DRIVE_LETTER_ALREADY_USED, /*Диск с этой буквой уже зарегистрирован*/
    LV_FS_RES_UNKNOWN,    /*Другая неизвестная ошибка*/
} lv_fs_res_t;

/**
 * Режим открытия файла.
 */
typedef enum {
    LV_FS_MODE_WR = 0x01,
    LV_FS_MODE_RD = 0x02,
} lv_fs_mode_t;

/**
 * Режимы поиска.
 */
typedef enum {
    LV_FS_SEEK_SET = 0x00,      /**< Set the position from absolutely (from the start of file)*/
    LV_FS_SEEK_CUR = 0x01,      /**< Set the position from the current position*/
    LV_FS_SEEK_END = 0x02,      /**< Set the position from the end of the file*/
} lv_fs_whence_t;

struct _lv_fs_drv_t;
typedef struct _lv_fs_drv_t lv_fs_drv_t;
struct _lv_fs_drv_t {
    char letter;
    uint32_t cache_size;
    bool (*ready_cb)(lv_fs_drv_t * drv);

    void(*remove_cb)(lv_fs_drv_t * drv); /*Необязательно*/

    void * (*open_cb)(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
    lv_fs_res_t (*close_cb)(lv_fs_drv_t * drv, void * file_p);
    lv_fs_res_t (*read_cb)(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
    lv_fs_res_t (*write_cb)(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
    lv_fs_res_t (*seek_cb)(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
    lv_fs_res_t (*tell_cb)(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

    void * (*dir_open_cb)(lv_fs_drv_t * drv, const char * path);
    lv_fs_res_t (*dir_read_cb)(lv_fs_drv_t * drv, void * rddir_p, char * fn, uint32_t fn_len);
    lv_fs_res_t (*dir_close_cb)(lv_fs_drv_t * drv, void * rddir_p);

    void * user_data; /**< Custom file user data*/
};

typedef struct {
    void * file_d;
    lv_fs_drv_t * drv;
    lv_fs_file_cache_t * cache;
} lv_fs_file_t;


typedef struct {
    void * dir_d;
    lv_fs_drv_t * drv;
} lv_fs_dir_t;


/** Расширенный объект пути для указания буфера для файлов, отображенных в памяти. */
typedef struct {
    char path[64];   /**<  Store the driver letter address and size*/
} lv_fs_path_ex_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте драйвер файловой системы со значениями по умолчанию.
 * Он используется для того, чтобы гарантировать, что все поля имеют известные значения, а не мусор в памяти.
 * После этого вы можете настроить поля.
 * @param drv     указатель на переменную драйвера для инициализации
 */
void lv_fs_drv_init(lv_fs_drv_t * drv);

/**
 * Добавить новый диск
 * @param drv       указатель на структуру lv_fs_drv_t, которая начинается с
 *                  соответствующие указатели функций. Сохраняется только указатель, поэтому
 *                  драйвер должен быть статическим или динамически выделенным.
 */
void lv_fs_drv_register(lv_fs_drv_t * drv);

/**
 * Дайте указатель на водителя из его буквы
 * @param letter    буква-идентификатор водителя
 * @return          указатель на драйвер или NULL, если не найден
 */
lv_fs_drv_t * lv_fs_get_drv(char letter);

/**
 * Удалите диск и вызовите его функцию удаления, если она доступна.
 * @param letter буквенный идентификатор диска, который нужно удалить
 */
void lv_fs_remove_drive(char letter);

/**
 * проверь, готов ли диск. Если функция`ready`не была инициализирована,`true`будет
 * вернулся.
 * @param letter    буква диска
 * @return          true: диск готов; ложь: диск не готов
 */
bool lv_fs_is_ready(char letter);

/**
 * Открыть файл
 * @param file_p    указатель на переменную lv_fs_file_t
 * @param path      путь к файлу, начинающийся с буквы драйвера (например, S:/folder/file.txt)
 * @param mode      читать: FS_MODE_RD, писать: FS_MODE_WR, оба:FS_MODE_RD|  FS_MODE_WR
 * @return          LV_FS_RES_OK или любая ошибка из перечисления lv_fs_res_t
 */
lv_fs_res_t lv_fs_open(lv_fs_file_t * file_p, const char * path, lv_fs_mode_t mode);

/**
 * Создайте специальный объект из адреса буфера/памяти, который выглядит как файл и может быть передан
 * как путь к`lv_fs_open`и другим функциям, включая путь.
 *
 * Например
 * @code
 *      //Создаем файл PNG из буфера и используем его
 *      lv_fs_path_ex_t р;
 *      lv_fs_make_path_from_buffer (&p, 'A',my_buf,my_buf_size, "png");
 *      lv_image_set_src (изображение1, &p);
 *
 * @endcode
 * @param path      путь к объекту lv_fs_path_ex
 * @param letter    идентификационная буква водителя. Например.  `LV_FS_MEMFS_LETTER`
 * @param buf       адрес буфера памяти
 * @param size      размер буфера памяти в байтах
 * @param ext       расширение, например «png», если NULL, расширение не будет добавлено.
 */
void lv_fs_make_path_from_buffer(lv_fs_path_ex_t * path, char letter, const void * buf, uint32_t size,
                                 const char * ext);

/**
 * Получите адрес и размер буфера из объекта пути
 * @param path      указатель на инициализированные данные `lv_fs_path_ex`
 * @param buffer    указатель на переменную`void *`для хранения адреса
 * @param size      указатель на данные`uint32_t`для хранения размера
 * @return          LV_RESULT_OK: буфер и размер установлены;  LV_RESULT_INVALID: произошла ошибка.
 */
lv_result_t lv_fs_get_buffer_from_path(lv_fs_path_ex_t * path, void ** buffer, uint32_t * size);

/**
 * Закрыть уже открытый файл
 * @param file_p    указатель на переменную lv_fs_file_t
 * @return          LV_FS_RES_OK или любая ошибка из перечисления lv_fs_res_t
 */
lv_fs_res_t lv_fs_close(lv_fs_file_t * file_p);

/**
 * Чтение из файла
 * @param file_p    указатель на переменную lv_fs_file_t
 * @param buf       указатель на буфер, в котором хранятся прочитанные байты
 * @param btr       Байты для чтения
 * @param br        количество реальных прочитанных байт (Bytes Read).  NULL, если не используется.
 * @return          LV_FS_RES_OK или любая ошибка из перечисления lv_fs_res_t
 */
lv_fs_res_t lv_fs_read(lv_fs_file_t * file_p, void * buf, uint32_t btr, uint32_t * br);

/**
 * Записать в файл
 * @param file_p    указатель на переменную lv_fs_file_t
 * @param buf       указатель на буфер с байтами для записи
 * @param btw       Байты для записи
 * @param bw        количество реальных записанных байт (Bytes Written).  NULL, если не используется.
 * @return          LV_FS_RES_OK или любая ошибка из перечисления lv_fs_res_t
 */
lv_fs_res_t lv_fs_write(lv_fs_file_t * file_p, const void * buf, uint32_t btw, uint32_t * bw);

/**
 * Установите положение «курсора» (указателя чтения и записи) в файле.
 * @param file_p    указатель на переменную lv_fs_file_t
 * @param pos       новая позиция, выраженная в индексе байтов (0: начало файла)
 * @param whence    сообщает, откуда установить позицию. См. lv_fs_whence_t.
 * @return          LV_FS_RES_OK или любая ошибка из перечисления lv_fs_res_t
 */
lv_fs_res_t lv_fs_seek(lv_fs_file_t * file_p, uint32_t pos, lv_fs_whence_t whence);

/**
 * Укажите положение указателя чтения и записи.
 * @param file_p    указатель на переменную lv_fs_file_t
 * @param pos       указатель для хранения положения указателя чтения и записи
 * @return          LV_FS_RES_OK или любая ошибка из «fs_res_t»
 */
lv_fs_res_t lv_fs_tell(lv_fs_file_t * file_p, uint32_t * pos);

/**
 * Получить размер открытого файла в байтах.
 * Позиция чтения/записи файла не изменится.
 * @param file_p    указатель на переменную lv_fs_file_t
 * @param size_res  указатель для хранения размера файла
 * @return          LV_FS_RES_OK или любая ошибка из `lv_fs_res_t`
 */
lv_fs_res_t lv_fs_get_size(lv_fs_file_t * file_p, uint32_t * size_res);

/**
 * Получить размер в байтах файла по заданному пути.
 * @param path      путь к файлу
 * @param size_res  указатель для хранения размера файла
 * @return          LV_FS_RES_OK или любая ошибка из `lv_fs_res_t`
 */
lv_fs_res_t lv_fs_path_get_size(const char * path, uint32_t * size_res);

/**
 * Считайте содержимое файла по заданному пути в буфер.
 * @param buf        буфер для чтения содержимого файла в
 * @param buf_size   размер буфера и объем чтения из файла
 * @param path       путь к файлу
 * @return           LV_FS_RES_OK в случае успеха, LV_FS_RES_UNKNOWN, если меньше
 *                   Из файла можно было прочитать байты`buf_size`,
 *                   или какая-то ошибка от `lv_fs_res_t`
 */
lv_fs_res_t lv_fs_load_to_buf(void * buf, uint32_t buf_size, const char * path);

/**
 * Загрузите файл в буфер памяти.
 * @param path  путь к файлу
 * @param size      указатель для хранения размера загруженного файла
 * @return          указатель на загруженный файловый буфер или NULL, если произошла ошибка
 */
void * lv_fs_load_with_alloc(const char * path, uint32_t * size);

/**
 * Инициализируйте переменнуюfs_dir_tдля чтения каталога.
 * @param rddir_p   указатель на переменную 'lv_fs_dir_t'
 * @param path      путь к каталогу
 * @return          LV_FS_RES_OK или любая ошибка из перечисления lv_fs_res_t
 */
lv_fs_res_t lv_fs_dir_open(lv_fs_dir_t * rddir_p, const char * path);

/**
 * Прочитайте следующее имя файла из каталога.
 * Название каталогов начинается с '/'
 * @param rddir_p   указатель на инициализированную переменную fs_dir_t
 * @param fn        указатель на буфер для хранения имени файла
 * @param fn_len    длина буфера для хранения имени файла
 * @return          LV_FS_RES_OK или любая ошибка из перечисления lv_fs_res_t
 */
lv_fs_res_t lv_fs_dir_read(lv_fs_dir_t * rddir_p, char * fn, uint32_t fn_len);

/**
 * Закройте чтение каталога
 * @param rddir_p   указатель на инициализированную переменную fs_dir_t
 * @return          LV_FS_RES_OK или любая ошибка из перечисления lv_fs_res_t
 */
lv_fs_res_t lv_fs_dir_close(lv_fs_dir_t * rddir_p);

/**
 * Заполнить буфер буквами существующих драйверов
 * @param buf       буфер для хранения букв (после последней буквы добавляется '\0')
 * @return          буфер
 */
char * lv_fs_get_letters(char * buf);

/**
 * Возврат с расширением имени файла
 * @param fn        строка с именем файла
 * @return          указатель на начальное расширение или пустую строку, если расширения нет
 */
const char * lv_fs_get_ext(const char * fn);

/**
 * Поднимитесь на один уровень
 * @param path      указатель на имя файла
 * @return          усеченное имя файла
 */
char * lv_fs_up(char * path);

/**
 * Получить последний элемент пути (например,U:/folder/file-> файл)
 * @param path      указатель на имя файла
 * @return          указатель на начало последнего элемента пути
 */
const char * lv_fs_get_last(const char * path);

/**
 * Объедините два компонента пути и автоматически добавьте/удалит разделитель по мере необходимости.
 * buf,buf_szи возвращаемое значение, аналогичное lv_snprintf
 * @param buf     буфер для размещения результата
 * @param buf_sz  размер буфа. В buf будет записано не болееbuf_sz— 1 символ,
 *                и нулевой терминатор
 * @param base    первый компонент пути
 * @param end     второй компонент пути
 * @return        количество символов (не включая нулевой терминатор)
 *                это было бы записано в buf, даже если быbuf_sz-1 было меньше
 */
int lv_fs_path_join(char * buf, size_t buf_sz, const char * base, const char * end);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_FS_H*/
