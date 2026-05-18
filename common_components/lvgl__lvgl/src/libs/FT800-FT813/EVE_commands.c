#include "../../lv_conf_internal.h"
#if LV_USE_DRAW_EVE
/*
@file EVE_commands.c
@brief   содержит функции FT8xx/BT8xx
@version 5.0
@date 29 декабря 2023 г.
@author Рудольф Ридель

@section информация

На самом деле, для Arm Cortex-M0и Cortex-M4у меня самое быстрое исполнение с -O2.
C-стандарт — C99.


@section LICENSE

Лицензия MIT

Copyright (c) 2016-2023 Rudolph Riedel

Разрешение настоящим предоставляется бесплатно любому лицу, получившему копию
данное программное обеспечение и связанные с ним файлы документации («Программное обеспечение») для решения
Программное обеспечение без ограничений, включая, помимо прочего, права
использовать, копировать, изменять, объединять, публиковать, распространять, сублицензировать,
и/или продавать копии Программного обеспечения, а также разрешать лицам, которым Программное обеспечение
предоставлено для этого при соблюдении следующих условий:

Вышеупомянутое уведомление об авторских правах и настоящее уведомление о разрешении должны быть включены во все
копии или существенные части Программного обеспечения.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


История @section

5.0
- добавленEVE_cmd_pclkfreq()
- верните запись REG_CSSPREAD, так как для более высоких частот ее необходимо деактивировать
- добавлена конфигурация второгоPLLдля часов пикселей в BT817/BT818 в EVE_init() на случай, если конфиг дисплея
определен EVE_PCLK_FREQ
- заменилBT81X_ENABLEна "EVE_GEN> 2"
- удален FT81X_ENABLE, поскольку FT81x уже сейчас является самой низкой поддерживаемой ревизией чипа
- удалил ранее устаревшую пометкуEVE_get_touch_tag()
- изменен EVE_color_rgb() для использования 32-битного значения, как и остальные команды цвета
- удалены метакоманды EVE_cmd_point(), EVE_cmd_line() и EVE_cmd_rect()
- разделить все команды списка отображения на две функции:EVE_cmd_XXX() иEVE_cmd_XXX_burst()
- переключился с использованияEVE_RAM_CMD+ cmdOffset на REG_CMDB_WRITE
- как побочный эффект от перехода на REG_CMDB_WRITE, каждая команда сопроцессора теперь выполняется автоматически
- переименовал EVE_LIB_GetProps() обратно в EVE_cmd_getprops(), поскольку он не делает ничего особенного для оправдания специального имени
- добавлена вспомогательная функция EVE_memWrite_sram_buffer()
- добавлены EVE_cmd_bitmap_transform() и EVE_cmd_bitmap_transform_burst()
- добавлена защита от нулевого указателя для команд, использующихblock_transfer()
- добавленEVE_cmd_playvideo()
- изменил EVE_cmd_setfont() на команду отображения списка и добавил EVE_cmd_setfont_burst()
- изменил EVE_cmd_setfont2() на команду отображения списка и добавил EVE_cmd_setfont2_burst()
- добавленEVE_cmd_videoframe()
- реструктуризировано: функции сортируются по поколениям чипов и внутри своей группы в алфавитном порядке
- reimplementedEVE_cmd_getmatrix() опять же, ему нужно читать значения, а не записывать их
- добавлены EVE_cmd_fontcache() и EVE_cmd_fontcachequery()
- добавленEVE_cmd_calibratesub()
- добавлены EVE_cmd_animframeram(), EVE_cmd_animframeram_burst(), EVE_cmd_animstartram(), EVE_cmd_animstartram_burst()
- добавлены EVE_cmd_apilevel(), EVE_cmd_apilevel_burst()
- добавлены EVE_cmd_calllist(), EVE_cmd_calllist_burst()
- добавлены EVE_cmd_hsf(), EVE_cmd_hsf_burst()
- добавленEVE_cmd_linetime()
- добавлены EVE_cmd_newlist(), EVE_cmd_newlist_burst()
- добавлены EVE_cmd_runanim(), EVE_cmd_runanim_burst()
- добавлена защита для EVE_start_cmd_burst() для защиты от перекрытия передач сDMAи сегментированных списков
- использовал spi_transmit_32(), чтобы сократить этот файл примерно на 600 строк без каких-либо функциональных изменений
- удалена история до версии 4.0
- удалил пару вызовов spi_transmit_32() из EVE_cmd_getptr(), чтобы он снова заработал
- Исправление:EVE_cmd_setfont2_burst() использовалCMD_SETFONTвместо CMD_SETFONT2.
- убрал проверку наcmd_burstиз EVE_cmd_getimage() так как он есть в группе команд, не используемых для отображения
списки
- перенес EVE_cmd_newlist() в группу команд, не используемых для отображения списков
- удаленEVE_cmd_newlist_burst()
- переименовал spi_flash_write() в private_block_write() и сделал его статическим
- переименовал EVE_write_string() в private_string_write() и сделал его статическим
- сделал EVE_start_command() статическим
- Исправление:ESP8266требует 32-битного выравнивания для 32-битных указателей.
    Изменение private_string_write() для пакетного режима для чтения 8-битных результатов
- Исправление: как-то перепутал private_string_write() для пакетного режима
    но только для 8-битных контроллеров
- изменены EVE_memRead8(), EVE_memRead16() и EVE_memRead32() для использования
    spi_transmit_32 () для начального адреса+передача нулевого байта
    Это ускоряет ESP32/ESP8266 на несколько человек, но не имеет заметного эффекта.
    для ATSAMD51 и немного медленнее для AVR.
- Исправление: не знаю почему, но установка статического значения дляprivate_block_write() сломала его, без «статического» все работает.
- Исправление:EVE_cmd_flashspirx() использовал CMD_FLASHREAD.
- исправлено предупреждение в EVE_init() при компиляции под EVE4
- переименовал внутреннюю функцию EVE_begin_cmd() в eve_begin_cmd() и сделал ее статической
- изменил все вызовы EVE_start_command() на вызовы eve_begin_cmd() после отчета на Github от
  Майкл Вакс, они идентичны, их не было до V5.
- удаленEVE_start_command()
- Исправление: EVE_init() проверял только первые два битаREG_CPURESETи игнорировал бит для аудио-движка, а не
проблема, но не правильная.
- исправлено несколько неприятных предупреждений
- исправлено несколько предупреждений cppcheck
- исправлено несколько предупреждений CERT
- преобразовал все TAB в пробелы
- сделалEVE_TOUCH_RZTHRESHв EVE_init() необязательным, чтобы а) удалить его изEVE_config.hи б) сделать его настраиваемым
внешне
- изменен EVE_init() для записи 1200U в REG_TOUCH_RZTHRESH, еслиEVE_TOUCH_RZTHRESHне определен
- изменен EVE_init() для возвратаE_OK=0x00в случае успеха и более значимых значений в случае неудачи
- изменен EVE_busy(), чтобы он возвращал EVE_IS_BUSY, еслиEVEзанят, иE_OK= 0x00, еслиEVEне занят - никаких реальных изменений в
функциональность
- наконец удаленEVE_cmd_start() после того, как он стал устаревшим в первой версии 5.0
- переименованEVE_cmd_execute() вEVE_execute_cmd() для большей согласованности, это не команда EVE
- изменен EVE_init_flash(), чтобы он возвращалE_OKв случае успеха и более значимые значения в случае неудачи
- добавлено возвращаемое значениеEVE_FIFO_HALF_EMPTYв EVE_busy(), чтобы указать, что доступно более 2048 байт
- незначительная очистка, меньше перерывов и операторов else
- добавил пакетный код обратно во все функции, для которых есть версия _burst, это позволяет использовать версию
без отслеживания _burst в имени, когда скорость выполнения не является проблемой - например. со всеми странами, поддерживающими DMA
- удалил историю версии 4.0
- добавлен дополнительный параметр EVE_ROTATE, как определено в EVE_init(), чтобы разрешить поворот экрана во время инициализации.
    спасибо за идею Андрею Валанду на Github!
- добавлен необязательный параметрEVE_BACKLIGHT_PWMв EVE_init(), позволяющий устанавливать подсветку во время инициализации
- модифицированEVE_calibrate_manual() для лучшей работы с дисплеями типа гистограммы
- исправлено большое количество проблем сMISRA-C - в основном больше приведений для явного преобразования типов и больше скобок
- изменены версииcmd_buttonс переменным числом аргументовcmd_button,cmd_textиcmd_toggleдля использования массива значенийuint32_tдля соответствияMISRA-C
- базовое обслуживание: проверяется на наличие нарушений правил использования пробелов и отступов.
- больше исправлений линтера для мелких проблем, таких как переменные короче 3 символов.
- добавлен EVE_color_a()/EVE_color_a_burst()
- больше мелких настроек и исправлений, чтобы статический анализатор был доволен
- изменил пакетный вариантprivate_string_write() обратно на более старую и быструю версию
- рефакторинг EVE_init() для одиночного возврата
- добавлен прототип дляEVE_write_display_parameters()
- добавленEVE_memRead_sram_buffer()
- Исправление ошибки № 81: ни DISP, ни частота пикселей не включены для конфигураций EVE4, не использующих EVE_PCLK_FREQ.
    спасибо за отчет grados73 на Github!
- добавлено несколько строк поддержки GameduinoGD3XвEVE_init()
- переключился с использованияCMD_PCLKFREQна запись напрямую в REG_PCLK_FREQ
- добавлено определениеEVE_SET_REG_PCLK_2Xдля установкиREG_PCLK_2Xв 1 при необходимости
- Исправление:EVE_init() устанавливал звуковой движок не в «отключенный звук», как предполагалось, а в «тихий» режим.
- Исправление: EVE_busy() теперь возвращаетE_NOT_OKпри ошибках сопроцессора.
    спасибо за отчет Z0ld3n на Github!
- Исправлено: переработан EVE_busy() для возвратаEVE_FAULT_RECOVEREDпри обнаружении ошибок сопроцессора,
    удалены флэш-команды из последовательности восстановления после сбоя, поскольку они зависят от проекта.
- добавлен EVE_get_and_reset_fault_state(), чтобы проверить, вызвал ли EVE_busy() восстановление после сбоя
- добавлены примечания по использованию EVE_cmd_setfont2() и EVE_cmd_romfont()
- новый необязательный параметр в EVE_init(): EVE_BACKLIGHT_FREQ
- исправлено несколько незначительных проблем статического анализа кода
- переработана пакетная часть private_string_write(), чтобы она стала менее сложной
- переименованы ссылки на чипид в regid, как предложено № 93 на github
- Исправление: прерывалась передача буферов размером более 3840 при устранении проблем статического анализа кода.
- изменил ряд параметров функции со знаковых на беззнаковые после
    обновленное руководство по программированию серии BT81x V2 .4
- сделал еще один линтер и исправил кое-что
- начал улучшать встроенную документацию
- добавлено больше документации
- удаленEVE_cmd_hsf_burst()

*/

#include "EVE_commands.h"

/* Команды памятиEVE— используются сEVE_memWritexxи EVE_memReadxx. */
#define MEM_WRITE 0x80U /* EVE Запись в память хоста */
/* #define MEM_READ 0x00U */ /* EVE Чтение памяти хоста */

/* определите NULL, если он еще не существует */
#ifndef NULL
#include <stdio.h>
#endif

static volatile uint8_t cmd_burst = 0U; /* флаг, указывающий, что cmd-burst активен */
static volatile uint8_t fault_recovered = E_OK; /* флаг, указывающий, вызвал лиEVE_busyвосстановление после сбоя */

/* ##################################################################
    вспомогательные функции
##################################################################### */

/**
 * @brief Отправьте команду хоста.
 */
void EVE_cmdWrite(uint8_t const command, uint8_t const parameter)
{
    EVE_cs_set();
    spi_transmit(command);
    spi_transmit(parameter);
    spi_transmit(0U);
    EVE_cs_clear();
}

/**
 * @brief Реализация функции rd8(), читает 8 бит.
 */
uint8_t EVE_memRead8(uint32_t const ft_address)
{
    uint8_t data;
    EVE_cs_set();
    spi_transmit_32(((ft_address >> 16U) & 0x0000007fUL) + (ft_address & 0x0000ff00UL) + ((ft_address & 0x000000ffUL) << 16U));
    data = spi_receive(0U); /* прочитать байт данных, отправив еще один фиктивный байт */
    EVE_cs_clear();
    return (data);
}

/**
 * @brief Реализация функции rd16(), читает 16 бит.
 */
uint16_t EVE_memRead16(uint32_t const ft_address)
{
    uint16_t data;

    EVE_cs_set();
    spi_transmit_32(((ft_address >> 16U) & 0x0000007fUL) + (ft_address & 0x0000ff00UL) + ((ft_address & 0x000000ffUL) << 16U));
    uint8_t const lowbyte = spi_receive(0U); /* прочитать младший байт */
    uint8_t const hibyte = spi_receive(0U); /* прочитать старший байт */
    data = ((uint16_t) hibyte * 256U) | lowbyte;
    EVE_cs_clear();
    return (data);
}

/**
 * @brief Реализация функции rd32(), читает 32 бита.
 */
uint32_t EVE_memRead32(uint32_t const ft_address)
{
    uint32_t data;
    EVE_cs_set();
    spi_transmit_32(((ft_address >> 16U) & 0x0000007fUL) + (ft_address & 0x0000ff00UL) + ((ft_address & 0x000000ffUL) << 16U));
    data = ((uint32_t) spi_receive(0U)); /* прочитать младший байт */
    data = ((uint32_t) spi_receive(0U) << 8U) | data;
    data = ((uint32_t) spi_receive(0U) << 16U) | data;
    data = ((uint32_t) spi_receive(0U) << 24U) | data; /* прочитать старший байт */
    EVE_cs_clear();
    return (data);
}

/**
 * @brief Реализация функции wr8(), записывает 8 бит.
 */
void EVE_memWrite8(uint32_t const ft_address, uint8_t const ft_data)
{
    EVE_cs_set();
    spi_transmit((uint8_t) (ft_address >> 16U) | MEM_WRITE);
    spi_transmit((uint8_t) (ft_address >> 8U));
    spi_transmit((uint8_t) (ft_address & 0x000000ffUL));
    spi_transmit(ft_data);
    EVE_cs_clear();
}

/**
 * @brief Реализация функции wr16(), записывает 16 бит.
 */
void EVE_memWrite16(uint32_t const ft_address, uint16_t const ft_data)
{
    EVE_cs_set();
    spi_transmit((uint8_t) (ft_address >> 16U) | MEM_WRITE); /* отправить запись в память плюс старший адресный байт */
    spi_transmit((uint8_t) (ft_address >> 8U));              /* отправить средний байт адреса */
    spi_transmit((uint8_t) (ft_address & 0x000000ffUL));     /* отправить младший байт адреса */
    spi_transmit((uint8_t) (ft_data & 0x00ffU));             /* отправить младший байт данных */
    spi_transmit((uint8_t) (ft_data >> 8U));                 /* отправить старший байт данных */
    EVE_cs_clear();
}

/**
 * @brief Реализация функции wr32(), записывает 32 бита.
 */
void EVE_memWrite32(uint32_t const ft_address, uint32_t const ft_data)
{
    EVE_cs_set();
    spi_transmit((uint8_t) (ft_address >> 16U) | MEM_WRITE); /* отправить запись в память плюс старший адресный байт */
    spi_transmit((uint8_t) (ft_address >> 8U));              /* отправить средний байт адреса */
    spi_transmit((uint8_t) (ft_address & 0x000000ffUL));     /* отправить младший байт адреса */
    spi_transmit_32(ft_data);
    EVE_cs_clear();
}

/**
 * @brief Вспомогательная функция, записывает блок памяти изFLASHхост-контроллера в EVE.
 */
void EVE_memWrite_flash_buffer(uint32_t const ft_address, const uint8_t *p_data, uint32_t const len)
{
    if (p_data != NULL)
    {
        EVE_cs_set();
        spi_transmit((uint8_t) (ft_address >> 16U) | MEM_WRITE);
        spi_transmit((uint8_t) (ft_address >> 8U));
        spi_transmit((uint8_t) (ft_address & 0x000000ffUL));

        lv_eve_target_spi_transmit_buf(p_data, len);

        EVE_cs_clear();
    }    
}

/**
 * @brief Вспомогательная функция, записывает блок памяти изSRAMхост-контроллера в EVE.
 */
void EVE_memWrite_sram_buffer(uint32_t const ft_address, const uint8_t *p_data, uint32_t const len)
{
    if (p_data != NULL)
    {
        EVE_cs_set();
        spi_transmit((uint8_t) (ft_address >> 16U) | MEM_WRITE);
        spi_transmit((uint8_t) (ft_address >> 8U));
        spi_transmit((uint8_t) (ft_address & 0x000000ffUL));

        lv_eve_target_spi_transmit_buf(p_data, len);

        EVE_cs_clear();
    }
}

/**
 * @brief Вспомогательная функция: чтение блока памяти изEVEвSRAMхост-контроллера.
 */
void EVE_memRead_sram_buffer(uint32_t const ft_address, uint8_t *p_data, uint32_t const len)
{
    if (p_data != NULL)
    {
        EVE_cs_set();
        spi_transmit_32(((ft_address >> 16U) & 0x0000007fUL) + (ft_address & 0x0000ff00UL) + ((ft_address & 0x000000ffUL) << 16U));

        for (uint32_t count = 0U; count < len; count++)
        {
            p_data[count] = spi_receive(0U); /* прочитать байт данных, отправив еще один фиктивный байт */
        }

        EVE_cs_clear();
    }    
}

static void CoprocessorFaultRecover(void)
{
#if EVE_GEN > 2
        uint16_t copro_patch_pointer;
        copro_patch_pointer = EVE_memRead16(REG_COPRO_PATCH_PTR);
#endif

        EVE_memWrite8(REG_CPURESET, 1U); /* удерживать двигатель сопроцессора в состоянии сброса */
        EVE_memWrite16(REG_CMD_READ, 0U); /* установите REG_CMD_READ в 0 */
        EVE_memWrite16(REG_CMD_WRITE, 0U); /* установите REG_CMD_WRITE в 0 */
        EVE_memWrite16(REG_CMD_DL, 0U); /* сбросьте REG_CMD_DL на 0, как того требует руководство по программированию BT81x, это не должно повредить FT8xx */

#if EVE_GEN > 2
        EVE_memWrite16(REG_COPRO_PATCH_PTR, copro_patch_pointer);

        /* восстановить REG_PCLK, если он был обнулен по ошибке */
#if (EVE_GEN > 3) && (defined EVE_PCLK_FREQ)
        EVE_memWrite16(REG_PCLK_FREQ, (uint16_t) EVE_PCLK_FREQ);
        EVE_memWrite8(REG_PCLK, 1U); /* включить режим extsync */
#else
        EVE_memWrite8(REG_PCLK, EVE_PCLK);
#endif

#endif
        EVE_memWrite8(REG_CPURESET, 0U); /* установите REG_CPURESET в 0, чтобы перезапустить механизм сопроцессора*/
        DELAY_MS(10U);                   /* просто на всякий случай */
}

/**
 * @brief Проверьте, завершил ли сопроцессор выполнение текущего списка команд.
 * @return -E_OK- еслиEVEне занят (передачаDMAне активна иREG_CMDB_SPACEимеет значение 0xffc, что означает, чтоCMD-FIFOпуст
 * @return -EVE_IS_BUSY- если активна передачаDMAилиREG_CMDB_SPACEимеет значение меньше 0xffc
 * @return -EVE_FIFO_HALF_EMPTY- если передачаDMAне активна иREG_CMDB_SPACEпоказывает, что доступно более 2048 байт.
 * @return -E_NOT_OK- если произошел сбой сопроцессора и была выполнена последовательность восстановления
 * @note - при неисправности сопроцессора внешняя вспышка не инициализируется повторно по EVE_busy()
 */
uint8_t EVE_busy(void)
{
    uint16_t space;
    uint8_t ret = EVE_IS_BUSY;

#if defined (EVE_DMA)
    if (0 == EVE_dma_busy)
    {
#endif

    space = EVE_memRead16(REG_CMDB_SPACE);

    /* ( REG_CMDB_SPACE & 0x03 ) != 0 -> у нас неисправность сопроцессора */
    if ((space & 3U) != 0U) /* у нас неисправен сопроцессор, заставьте EVE снова поиграть с нами */
    {
        ret = EVE_FAULT_RECOVERED;
        fault_recovered = EVE_FAULT_RECOVERED; /* сохранить состояние восстановления после сбоя */
        CoprocessorFaultRecover();
    }
    else
    {
        if (0xffcU == space)
        {
            ret = E_OK;
        }
        else if (space > 0x800U)
        {
            ret = EVE_FIFO_HALF_EMPTY;
        }
        else
        {
            ret = EVE_IS_BUSY;
        }
    }

#if defined (EVE_DMA)
    }
#endif

    return (ret);
}

/**
 * @brief Вспомогательная функция для проверки того, пытался ли EVE_busy() восстановиться после сбоя сопроцессора.
 * Внутренний индикатор очищается, и его можно снова установить с помощью EVE_busy().
 * @return -EVE_FAULT_RECOVERED- если EVE_busy() обнаружил неисправность сопроцессора
 * @return -E_OK- если EVE_busy() не обнаружил неисправность сопроцессора
 */
uint8_t EVE_get_and_reset_fault_state(void)
{
    uint8_t ret = E_OK;

    if (EVE_FAULT_RECOVERED == fault_recovered)
    {
        ret = EVE_FAULT_RECOVERED;
        fault_recovered = E_OK;
    }
    return (ret);
}

/**
 * @brief Вспомогательная функция: дождитесь, пока сопроцессор завершит очередь FIFO.
 */
void EVE_execute_cmd(void)
{
    while (EVE_busy() != E_OK)
    {
    }
}

/* начать команду сопроцессора, это используется для команд без списка отображения и без пакетного режима.*/
static void eve_begin_cmd(uint32_t command)
{
    EVE_cs_set();
    spi_transmit((uint8_t) 0xB0U); /* старший байт REG_CMDB_WRITE + MEM_WRITE */
    spi_transmit((uint8_t) 0x25U); /* средний байт REG_CMDB_WRITE */
    spi_transmit((uint8_t) 0x78U); /* младший байт REG_CMDB_WRITE */
    spi_transmit_32(command);
}

static void private_block_write(const uint8_t *p_data, uint16_t len); /* прототип, соответствующий MISRA */

static void private_block_write(const uint8_t *p_data, uint16_t len)
{
    uint8_t padding;

    padding = (uint8_t) (len & 3U); /* 0, 1, 2 or 3 */
    padding = 4U - padding;         /* 4, 3, 2 or 1 */
    padding &= 3U;                  /* 3, 2 or 1 */

    for (uint16_t count = 0U; count < len; count++)
    {
        spi_transmit(fetch_flash_byte(&p_data[count]));
    }

    while (padding > 0U)
    {
        spi_transmit(0U);
        padding--;
    }
}

static void block_transfer(const uint8_t *p_data, uint32_t len); /* прототип, соответствующий MISRA */

static void block_transfer(const uint8_t *p_data, uint32_t len)
{
    uint32_t bytes_left;
    uint32_t offset = 0U;

    bytes_left = len;
    while (bytes_left > 0U)
    {
        uint32_t block_len;

        block_len = (bytes_left > 3840UL) ? 3840UL : bytes_left;

        EVE_cs_set();
        spi_transmit((uint8_t) 0xB0U); /* старший байт REG_CMDB_WRITE + MEM_WRITE */
        spi_transmit((uint8_t) 0x25U); /* средний байт REG_CMDB_WRITE */
        spi_transmit((uint8_t) 0x78U); /* младший байт REG_CMDB_WRITE */
        private_block_write(&p_data[offset], (uint16_t) block_len);
        EVE_cs_clear();
        offset += block_len;
        bytes_left -= block_len;
        EVE_execute_cmd();
    }
}

/* ##################################################################
    команды сопроцессора, которые не используются в списках дисплеев,
    они не должны использоваться при пакетной передаче
################################################################### */

/* BT817 / BT818 */
#if EVE_GEN > 3

/**
 * @brief Запишите «num» байтов из src вRAM_Gв ранее стертую внешнюю флэш-память BT81x по адресу dest.
 * @note - dest должен быть выровнен по 4096 байт, src должен быть выровнен по 4 байта, num должно быть кратно 4096
 * @note -EVEничего не будет делать, если требования выравнивания не будут выполнены.
 * @note — адрес ptr относится к флэш-памяти, поэтому первый адрес — 0x000000, а не 0x800000.
 * @note - это выглядит точно так же, как EVE_cmd_flashupdate(), но для него нужно, чтобы флэш был пуст
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_flashprogram(uint32_t dest, uint32_t src, uint32_t num)
{
    eve_begin_cmd(CMD_FLASHPROGRAM);
    spi_transmit_32(dest);
    spi_transmit_32(src);
    spi_transmit_32(num);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Включите кеш шрифтов.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_fontcache(uint32_t font, uint32_t ptr, uint32_t num)
{
    eve_begin_cmd(CMD_FONTCACHE);
    spi_transmit_32(font);
    spi_transmit_32(ptr);
    spi_transmit_32(num);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Запрашивает емкость и использование кэша шрифтов.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_fontcachequery(uint32_t *p_total, uint32_t *p_used)
{
    uint16_t cmdoffset;

    eve_begin_cmd(CMD_FONTCACHEQUERY);
    spi_transmit_32(0UL);
    spi_transmit_32(0UL);
    EVE_cs_clear();
    EVE_execute_cmd();

    cmdoffset = EVE_memRead16(REG_CMD_WRITE); /* прочитать указатель записи сопроцессора */

    if (p_total != NULL)
    {
        *p_total = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 8UL) & 0xfffUL));
    }
    if (p_used != NULL)
    {
        *p_used = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 4UL) & 0xfffUL));
    }
}

/**
 * @brief Возвращает все атрибуты растрового изображения, созданные предыдущими CMD_LOADIMAGE, CMD_PLAYVIDEO,CMD_VIDEOSTARTили CMD_VIDEOSTARTF.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_getimage(uint32_t *p_source, uint32_t *p_fmt, uint32_t *p_width, uint32_t *p_height, uint32_t *p_palette)
{
    uint16_t cmdoffset;

    eve_begin_cmd(CMD_GETIMAGE);
    spi_transmit_32(0UL);
    spi_transmit_32(0UL);
    spi_transmit_32(0UL);
    spi_transmit_32(0UL);
    spi_transmit_32(0UL);
    EVE_cs_clear();
    EVE_execute_cmd();

    cmdoffset = EVE_memRead16(REG_CMD_WRITE); /* прочитать указатель записи сопроцессора */

    if (p_palette != NULL)
    {
        *p_palette = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 4UL) & 0xfffUL));
    }
    if (p_height != NULL)
    {
        *p_height = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 8UL) & 0xfffUL));
    }
    if (p_width != NULL)
    {
        *p_width = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 12UL) & 0xfffUL));
    }
    if (p_fmt != NULL)
    {
        *p_fmt = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 16UL) & 0xfffUL));
    }
    if (p_source != NULL)
    {
        *p_source = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 20UL) & 0xfffUL));
    }
}

/**
 * @brief Недокументированная команда.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_linetime(uint32_t dest)
{
    eve_begin_cmd(CMD_LINETIME);
    spi_transmit_32(dest);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Запускает компиляцию списка команд в RAM_G. 
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_newlist(uint32_t adr)
{
    eve_begin_cmd(CMD_NEWLIST);
    spi_transmit_32(adr);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief УстанавливаетREG_PCLK_FREQдля генерации максимально близкой к запрошенной частоте.
 * @return - достигнутая частота или ноль, если частота не найдена
 * @note - При использовании этой команды требуется прошивка BLOB.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
uint32_t EVE_cmd_pclkfreq(uint32_t ftarget, int32_t rounding)
{
    uint16_t cmdoffset;

    eve_begin_cmd(CMD_PCLKFREQ);
    spi_transmit_32(ftarget);
    spi_transmit_32((uint32_t) rounding);
    spi_transmit_32(0UL);
    EVE_cs_clear();
    EVE_execute_cmd();
    cmdoffset = EVE_memRead16(REG_CMD_WRITE); /* прочитать указатель записи сопроцессора */
    cmdoffset -= 4U;
    cmdoffset &= 0x0fffU;
    return (EVE_memRead32(EVE_RAM_CMD + cmdoffset));
}

/**
 * @brief Ожидает указанное количество микросекунд.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_wait(uint32_t usec)
{
    eve_begin_cmd(CMD_WAIT);
    spi_transmit_32(usec);
    EVE_cs_clear();
    EVE_execute_cmd();
}

#endif /* EVE_GEN > 3 */

/* BT815 / BT816 */
#if EVE_GEN > 2

/**
 * @brief Очищает внутренний флэш-кэш графического движка.
 * @note - Эта функция включает очистку списка отображения.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_clearcache(void)
{
    EVE_cmd_dl(CMD_DLSTART);
    EVE_cmd_dl(CMD_SWAP);
    EVE_execute_cmd();

    EVE_cmd_dl(CMD_DLSTART);
    EVE_cmd_dl(CMD_SWAP);
    EVE_execute_cmd();

    EVE_cmd_dl(CMD_CLEARCACHE);
    EVE_execute_cmd();
}

/**
 * @brief Повторно подключитесь к подключенному флэш-накопителю SPI.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_flashattach(void)
{
    eve_begin_cmd(CMD_FLASHATTACH);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Отключитесь от подключенного флэш-накопителя SPI.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_flashdetach(void)
{
    eve_begin_cmd(CMD_FLASHDETACH);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Удаляет подключенную флэш-память SPI.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_flasherase(void)
{
    eve_begin_cmd(CMD_FLASHERASE);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Если возможно, используйте подключенный флэш-накопительSPIна полной скорости.
 * @return - Ноль в случае успеха, код ошибки в случае неудачи
 * @note - При использовании этой команды требуется прошивка BLOB.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
uint32_t EVE_cmd_flashfast(void)
{
    uint16_t cmdoffset;

    eve_begin_cmd(CMD_FLASHFAST);
    spi_transmit_32(0UL);
    EVE_cs_clear();
    EVE_execute_cmd();
    cmdoffset = EVE_memRead16(REG_CMD_WRITE); /* прочитать указатель записи сопроцессора */
    cmdoffset -= 4U;
    cmdoffset &= 0x0fffU;
    return (EVE_memRead32(EVE_RAM_CMD + cmdoffset));
}

/**
 * @brief Отменяет сигналSPICS подключенного флэш-накопителя SPI.
 * @note - Работает только в том случае, если подключенная флэш-памятьSPIотсоединена.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_flashspidesel(void)
{
    eve_begin_cmd(CMD_FLASHSPIDESEL);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Копирует «num» байтов из «src» подключенной флэш-памятиSPIв «dest» вRAM_G.
 * @note - src должен быть выровнен по 64 байта, dest должен быть выровнен по 4 байта, число должно быть кратно 4
 * @note -EVEничего не будет делать, если требования выравнивания не будут выполнены.
 * @note — Указатель src относится к флэш-памяти, поэтому первый адрес — 0x000000, а не 0x800000.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_flashread(uint32_t dest, uint32_t src, uint32_t num)
{
    eve_begin_cmd(CMD_FLASHREAD);
    spi_transmit_32(dest);
    spi_transmit_32(src);
    spi_transmit_32(num);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Установите адрес источника для флэш-данных, загружаемых командами CMD_LOADIMAGE, CMD_PLAYVIDEO,CMD_VIDEOSTARTFи CMD_INFLATE2, с помощью опции OPT_FLASH.
 * @note - Адрес должен быть выровнен по 64 байта.
 * @note -EVEне будет ничего делать, если требования выравнивания не будут выполнены.
 * @note — Указатель относится к флэш-памяти, поэтому первый адрес — 0x000000, а не 0x800000.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_flashsource(uint32_t ptr)
{
    eve_begin_cmd(CMD_FLASHSOURCE);
    spi_transmit_32(ptr);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Получает байты от флэш-интерфейсаSPIи записывает их в основную память.
 * @note - Работает только в том случае, если подключенная флэш-памятьSPIотсоединена.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_flashspirx(uint32_t dest, uint32_t num)
{
    eve_begin_cmd(CMD_FLASHSPIRX);
    spi_transmit_32(dest);
    spi_transmit_32(num);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Передаёт байты через интерфейс флэш-памяти SPI.
 * @note - Работает только в том случае, если подключенная флэш-памятьSPIотсоединена.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_flashspitx(uint32_t num, const uint8_t *p_data)
{
    eve_begin_cmd(CMD_FLASHSPITX);
    spi_transmit_32(num);
    EVE_cs_clear();
    block_transfer(p_data, num);
}

/**
 * @brief Запишите «num» байтов из src вRAM_Gв подключенную флэш-памятьSPIпо адресу dest.
 * @note - dest должен быть выровнен по 4096 байт, src должен быть выровнен по 4 байта, num должно быть кратно 4096
 * @note -EVEне будет ничего делать, если требования выравнивания не будут выполнены.
 * @note — Адрес ptr относится к флэш-памяти, поэтому первый адрес — 0x000000, а не 0x800000.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_flashupdate(uint32_t dest, uint32_t src, uint32_t num)
{
    eve_begin_cmd(CMD_FLASHUPDATE);
    spi_transmit_32(dest);
    spi_transmit_32(src);
    spi_transmit_32(num);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Запишите «число» байтов в подключенную флэш-памятьSPIпо адресу dest.
 * @note - dest должно быть выровнено по 256 байт, num должно быть кратно 256
 * @note -EVEне будет ничего делать, если требования выравнивания не будут выполнены.
 * @note — Адрес ptr относится к флэш-памяти, поэтому первый адрес — 0x000000, а не 0x800000.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_flashwrite(uint32_t ptr, uint32_t num, const uint8_t *p_data)
{
    eve_begin_cmd(CMD_FLASHWRITE);
    spi_transmit_32(ptr);
    spi_transmit_32(num);
    EVE_cs_clear();
    if (p_data != NULL)
    {
        block_transfer(p_data, num);
    }
}

/**
 * @brief Распакуйте данные в RAM_G.
 * @note - Данные должны быть верными и полными.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_inflate2(uint32_t ptr, uint32_t options, const uint8_t *p_data, uint32_t len)
{
    eve_begin_cmd(CMD_INFLATE2);
    spi_transmit_32(ptr);
    spi_transmit_32(options);
    EVE_cs_clear();

    if (0UL == options) /* прямые данные, а не через Media-FIFOили Flash */
    {
        if (p_data != NULL)
        {
            block_transfer(p_data, len);
        }
    }
}

#endif /* EVE_GEN > 2 */

/**
 * @brief Возвращает исходный адрес и размер растрового изображения, загруженного предыдущим CMD_LOADIMAGE.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_getprops(uint32_t *p_pointer, uint32_t *p_width, uint32_t *p_height)
{
    uint16_t cmdoffset;

    eve_begin_cmd(CMD_GETPROPS);
    spi_transmit_32(0UL);
    spi_transmit_32(0UL);
    spi_transmit_32(0UL);
    EVE_cs_clear();
    EVE_execute_cmd();
    cmdoffset = EVE_memRead16(REG_CMD_WRITE); /* прочитать указатель записи сопроцессора */

    if (p_pointer != NULL)
    {
        *p_pointer = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 12UL) & 0xfffUL));
    }
    if (p_width != NULL)
    {
        *p_width = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 8UL) & 0xfffUL));
    }
    if (p_height != NULL)
    {
        *p_height = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 4UL) & 0xfffUL));
    }
}

/**
 * @brief Возвращает следующий адрес послеCMD_INFLATEи других команд.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
uint32_t EVE_cmd_getptr(void)
{
    uint16_t cmdoffset;

    eve_begin_cmd(CMD_GETPTR);
    spi_transmit_32(0UL);
    EVE_cs_clear();
    EVE_execute_cmd();
    cmdoffset = EVE_memRead16(REG_CMD_WRITE); /* прочитать указатель записи сопроцессора */
    cmdoffset -= 4U;
    cmdoffset &= 0x0fffU;
    return (EVE_memRead32(EVE_RAM_CMD + cmdoffset));
}

/**
 * @brief Распакуйте данные в RAM_G.
 * @note - Данные должны быть верными и полными.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_inflate(uint32_t ptr, const uint8_t *p_data, uint32_t len)
{
    eve_begin_cmd(CMD_INFLATE);
    spi_transmit_32(ptr);
    EVE_cs_clear();
    if (p_data != NULL)
    {
        block_transfer(p_data, len);
    }
}

/**
 * @brief Вызов прерывания INT_CMDFLAG.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_interrupt(uint32_t msec)
{
    eve_begin_cmd(CMD_INTERRUPT);
    spi_transmit_32(msec);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Загружает и декодирует изображение JPEG/PNG в RAM_G.
 * @note - Декодирование изображенийPNGзанимает значительно больше времени, чем декодирование изображений JPEG.
 * @note - При возникновении сомнений используйтеEVEAsset Builder, чтобы проверить совместимость файлов PNG/JPEG.
 * @note - Если образ имеет формат PNG, верхние 42 КБRAM_Gбудут перезаписаны.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_loadimage(uint32_t ptr, uint32_t options, const uint8_t *p_data, uint32_t len)
{
    eve_begin_cmd(CMD_LOADIMAGE);
    spi_transmit_32(ptr);
    spi_transmit_32(options);
    EVE_cs_clear();

#if EVE_GEN > 2
    if ((0UL == (options & EVE_OPT_MEDIAFIFO)) &&
        (0UL == (options & EVE_OPT_FLASH))) /* прямые данные, ни через Media-FIFO, ни из Flash */
#else
    if (0UL == (options & EVE_OPT_MEDIAFIFO))  /* прямые данные, а не СМИ- FIFO */
#endif
    {
        if (p_data != NULL)
        {
            block_transfer(p_data, len);
        }
    }
}

/**
 * @brief Настройте потоковое мультимедиаFIFOв RAM_G.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_mediafifo(uint32_t ptr, uint32_t size)
{
    eve_begin_cmd(CMD_MEDIAFIFO);
    spi_transmit_32(ptr);
    spi_transmit_32(size);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Скопируйте блок RAM_G.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_memcpy(uint32_t dest, uint32_t src, uint32_t num)
{
    eve_begin_cmd(CMD_MEMCPY);
    spi_transmit_32(dest);
    spi_transmit_32(src);
    spi_transmit_32(num);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief ВычислитеCRC-32 дляRAM_G.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
uint32_t EVE_cmd_memcrc(uint32_t ptr, uint32_t num)
{
    uint16_t cmdoffset;

    eve_begin_cmd(CMD_MEMCRC);
    spi_transmit_32(ptr);
    spi_transmit_32(num);
    spi_transmit_32(0UL);
    EVE_cs_clear();
    EVE_execute_cmd();
    cmdoffset = EVE_memRead16(REG_CMD_WRITE); /* прочитать указатель записи сопроцессора */
    cmdoffset -= 4U;
    cmdoffset &= 0x0fffU;
    return (EVE_memRead32(EVE_RAM_CMD + cmdoffset));
}

/**
 * @brief ЗаполнитеRAM_Gзначением байта.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_memset(uint32_t ptr, uint8_t value, uint32_t num)
{
    eve_begin_cmd(CMD_MEMSET);
    spi_transmit_32(ptr);
    spi_transmit_32((uint32_t)value);
    spi_transmit_32(num);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Запишите байты вRAM_Gс помощью сопроцессора.
 * @note - Закомментировано, просто используйте одну из вспомогательных функцийEVE_memWrite* для прямой записи в память EVE.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
/*
voidEVE_cmd_memwrite(обозначение uint32_t, число uint32_t, constuint8_t* p_data)
{
    eve_begin_cmd (CMD_MEMWRITE);
    spi_transmit_32 (адресат);
    spi_transmit_32 (число);

    num = (число + 3U) и (~3U);

    for (uint32_tcount = 0U; count<len; count++)
    {
        spi_transmit (pgm_read_byte_far(p_data+ счет));
    }

    EVE_cs_clear ();
    EVE_execute_cmd ();
}
*/

/**
 * @brief Считайте значение регистра с помощью сопроцессора.
 * @note - Закомментировано, просто прочитайте реестр напрямую.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
/*
uint32_tEVE_cmd_regread(точка uint32_t)
{
    uint16_t cmdoffset;

    eve_begin_cmd (CMD_REGREAD);
    spi_transmit_32 (птр);
    spi_transmit_32 (0UL);
    EVE_cs_clear ();
    EVE_execute_cmd ();
    cmdoffset = EVE_memRead16 (REG_CMD_WRITE); // читаем указатель записи сопроцессора
    cmdoffset -= 4U;
    cmdoffset &= 0x0fffU;
    return (EVE_memRead32(EVE_RAM_CMD+ cmdoffset));
}
*/

/**
 * @brief Запишите ноль в RAM_G.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_memzero(uint32_t ptr, uint32_t num)
{
    eve_begin_cmd(CMD_MEMZERO);
    spi_transmit_32(ptr);
    spi_transmit_32(num);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Воспроизведение видео в форматеJPEGв кодировке AVI.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды.
 * @note - Не поддерживает пакетный режим.
 * @note - Не ждет завершения, чтобы позволить приостановить или завершить видео с помощью REG_PLAY_CONTROL.
 */
void EVE_cmd_playvideo(uint32_t options, const uint8_t *p_data, uint32_t len)
{
    eve_begin_cmd(CMD_PLAYVIDEO);
    spi_transmit_32(options);
    EVE_cs_clear();

#if EVE_GEN > 2
    if ((0UL == (options & EVE_OPT_MEDIAFIFO)) &&
        (0UL == (options & EVE_OPT_FLASH))) /* прямые данные, ни через Media-FIFO, ни из Flash */
#else
    if (0UL == (options & EVE_OPT_MEDIAFIFO))  /* прямые данные, а не СМИ- FIFO */
#endif
    {
        if (p_data != NULL)
        {
            block_transfer(p_data, len);
        }
    }
}

/**
 * @brief Поверните экран и соответствующим образом настройте матрицу преобразования.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_setrotate(uint32_t rotation)
{
    eve_begin_cmd(CMD_SETROTATE);
    spi_transmit_32(rotation);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Сделайте снимок текущего экрана.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_snapshot(uint32_t ptr)
{
    eve_begin_cmd(CMD_SNAPSHOT);
    spi_transmit_32(ptr);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Сделайте снимок части текущего экрана с возможностью форматирования.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_snapshot2(uint32_t fmt, uint32_t ptr, int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt)
{
    eve_begin_cmd(CMD_SNAPSHOT2);
    spi_transmit_32(fmt);
    spi_transmit_32(ptr);

    spi_transmit((uint8_t) ((uint16_t) xc0));
    spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
    spi_transmit((uint8_t) ((uint16_t) yc0));
    spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));

    spi_transmit((uint8_t) (wid));
    spi_transmit((uint8_t) (wid >> 8U));
    spi_transmit((uint8_t) (hgt));
    spi_transmit((uint8_t) (hgt >> 8U));

    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Отслеживание касаний графического объекта.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_track(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t tag)
{
    eve_begin_cmd(CMD_TRACK);

    spi_transmit((uint8_t) ((uint16_t) xc0));
    spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
    spi_transmit((uint8_t) ((uint16_t) yc0));
    spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));

    spi_transmit((uint8_t) (wid));
    spi_transmit((uint8_t) (wid >> 8U));
    spi_transmit((uint8_t) (hgt));
    spi_transmit((uint8_t) (hgt >> 8U));

    spi_transmit((uint8_t) (tag));
    spi_transmit((uint8_t) (tag >> 8U));
    spi_transmit(0U);
    spi_transmit(0U);

    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Загрузите следующий кадр видео.
 * @note - Предназначен для вызова за пределами здания списка отображения.
 * @note - Включает выполнение команды и ожидание завершения.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_videoframe(uint32_t dest, uint32_t result_ptr)
{
    eve_begin_cmd(CMD_VIDEOFRAME);
    spi_transmit_32(dest);
    spi_transmit_32(result_ptr);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/* ##################################################################
        исправление и инициализация
#################################################################### */

#if EVE_GEN > 2

/**
 * @brief Инициализация флэш-памятиEVEдля BT81x переключает FLASH, подключенный к BT81x, в полноскоростной режим.
 * @return ВозвращаетE_OKв случае успеха, EVE_FAIL_FLASH_STATUS_INIT, если статус остается начальным,
 * EVE_FAIL_FLASH_STATUS_DETACHED, если флэш-чип не найден, ряд различных значений для ошибок с
 * cmd_flashfast иE_NOT_OK, если вREG_FLASH_STATUSвозвращается статус «не отключено».
 */
uint8_t EVE_init_flash(void)
{
    uint8_t timeout = 0U;
    uint8_t status;
    uint8_t ret_val = E_NOT_OK;

    status = EVE_memRead8(REG_FLASH_STATUS); /* должно быть 0x02 - FLASH_STATUS_BASIC, включение питания выполнено и подключенная вспышка обнаружена */

     /* мы каким-то образом все еще находимся в инициализации, подождите еще немного, этого никогда не должно произойти */
    while (EVE_FLASH_STATUS_INIT == status)
    {
        status = EVE_memRead8(REG_FLASH_STATUS);
        DELAY_MS(1U);
        timeout++;
        if (timeout > 100U) /* 100 мс и все еще в инициализации, давайте сейчас завершим работу и выйдем с ошибкой */
        {
            ret_val = EVE_FAIL_FLASH_STATUS_INIT;
            break;
        }
    }

    /* во время инициализации флэш-память не обнаружена, флэш-память отсутствует или обнаружение не удалось, попробуйте еще раз */
    if (EVE_FLASH_STATUS_DETACHED == status)
    {
        EVE_cmd_dl(CMD_FLASHATTACH);
        EVE_execute_cmd();
        status = EVE_memRead8(REG_FLASH_STATUS);
        if (status != 2U) /* еще не в FLASH_STATUS_BASIC, пора сдаваться */
        {
            ret_val = EVE_FAIL_FLASH_STATUS_DETACHED;
        }
    }

    /* вспышка обнаружена и готова к действию, переместите ее на FLASH_STATUS_FULL */
    if (EVE_FLASH_STATUS_BASIC == status)
    {
        uint32_t result;

        result = EVE_cmd_flashfast();

        switch (result)
        {
            case 0x0000UL:
                ret_val = E_OK;
            break;

            case 0xE001UL:
                ret_val = EVE_FAIL_FLASHFAST_NOT_SUPPORTED;
            break;

            case 0xE002UL:
                ret_val = EVE_FAIL_FLASHFAST_NO_HEADER_DETECTED;
            break;

            case 0xE003UL:
                ret_val = EVE_FAIL_FLASHFAST_SECTOR0_FAILED;
            break;

            case 0xE004UL:
                ret_val = EVE_FAIL_FLASHFAST_BLOB_MISMATCH;
            break;

            case 0xE005UL:
                ret_val = EVE_FAIL_FLASHFAST_SPEED_TEST;
            break;

            default: /* у нас неизвестная ошибка, поэтому просто верните ошибку */
                ret_val = E_NOT_OK;
            break;
        }
    }

    if (EVE_FLASH_STATUS_FULL == status) /* we are already there, why has this function been called? */
    {
        ret_val = E_OK;
    }

    return (ret_val);
}

#endif /* EVE_GEN > 2 */

#if EVE_GEN < 3
#if defined (__AVR__)
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif
#endif

static void use_gt911(void);

static void use_gt911(void)
{
#if EVE_GEN > 2
    EVE_memWrite16(REG_TOUCH_CONFIG, 0x05d0U); /* переключиться на сенсорный контроллер Goodix */
#else

/* Бинарный объектFT811/FT813от FTDIsAN_336для патча тач-движка для сенсорных контроллеров GoodixGT911/ GT9271 */
const uint8_t eve_gt911_data[1184U] PROGMEM =
{
    26,  255, 255, 255, 32,  32,  48,  0,   4,   0,   0,   0,   2,   0,   0,   0,   34,  255, 255, 255, 0,   176, 48,
    0,   120, 218, 237, 84,  221, 111, 84,  69,  20,  63,  51,  179, 93,  160, 148, 101, 111, 76,  5,   44,  141, 123,
    111, 161, 11,  219, 154, 16,  9,   16,  17,  229, 156, 75,  26,  11,  13,  21,  227, 3,   16,  252, 184, 179, 45,
    219, 143, 45,  41,  125, 144, 72,  67,  100, 150, 71,  189, 113, 18,  36,  17,  165, 100, 165, 198, 16,  32,  17,
    149, 196, 240, 128, 161, 16,  164, 38,  54,  240, 0,   209, 72,  130, 15,  38,  125, 48,  66,  82,  30,  76,  19,
    31,  172, 103, 46,  139, 24,  255, 4,   227, 157, 204, 156, 51,  115, 102, 206, 231, 239, 220, 5,   170, 94,  129,
    137, 75,  194, 216, 98,  94,  103, 117, 115, 121, 76,  131, 177, 125, 89,  125, 82,  123, 60,  243, 58,  142, 242,
    204, 185, 243, 188, 118, 156, 227, 155, 203, 238, 238, 195, 251, 205, 229, 71,  92,  28,  169, 190, 184, 84,  143,
    113, 137, 53,  244, 103, 181, 237, 87,  253, 113, 137, 233, 48,  12,  198, 165, 181, 104, 139, 25,  84,  253, 155,
    114, 74,  191, 0,   54,  138, 163, 12,  62,  131, 207, 129, 23,  217, 34,  91,  31,  128, 65,  246, 163, 175, 213,
    8,   147, 213, 107, 35,  203, 94,  108, 3,   111, 40,  171, 83,  24,  15,  165, 177, 222, 116, 97,  23,  188, 140,
    206, 150, 42,  102, 181, 87,  78,  86,  182, 170, 134, 215, 241, 121, 26,  243, 252, 2,   76,  115, 217, 139, 222,
    206, 173, 136, 132, 81,  61,  35,  185, 39,  113, 23,  46,  199, 76,  178, 54,  151, 183, 224, 0,   40,  189, 28,
    149, 182, 58,  131, 79,  152, 30,  76,  34,  98,  234, 162, 216, 133, 141, 102, 39,  170, 40,  192, 101, 53,  201,
    146, 191, 37,  77,  44,  177, 209, 74,  211, 5,   206, 187, 5,   6,   216, 47,  53,  96,  123, 22,  50,  103, 251,
    192, 84,  17,  74,  227, 185, 56,  106, 51,  91,  161, 96,  182, 163, 48,  171, 141, 139, 65,  152, 66,  66,  11,
    102, 43,  158, 75,  36,  80,  147, 184, 147, 139, 112, 17,  235, 216, 103, 111, 239, 245, 92,  10,  175, 194, 40,
    44,  58,  125, 5,   59,  112, 50,  103, 245, 4,   78,  192, 5,   156, 194, 51,  60,  191, 134, 75,  110, 173, 237,
    46,  192, 121, 156, 192, 115, 184, 218, 120, 67,  63,  115, 46,  11,  102, 10,  97,  232, 50,  235, 114, 182, 148,
    118, 178, 41,  188, 12,  135, 77,  202, 124, 12,  96,  238, 35,  161, 234, 189, 129, 23,  249, 212, 139, 230, 25,
    53,  48,  205, 52,  93,  163, 117, 53,  154, 170, 81,  85,  163, 178, 70,  69,  66,  167, 241, 14,  46,  241, 1,
    226, 136, 152, 179, 197, 59,  184, 148, 254, 49,  132, 48,  15,  176, 137, 192, 76,  131, 196, 105, 104, 162, 86,
    81,  160, 165, 255, 26,  173, 162, 137, 86,  145, 210, 183, 192, 55,  175, 194, 211, 60,  91,  120, 230, 184, 174,
    27,  41,  131, 155, 40,  224, 29,  87,  179, 232, 16,  55,  55,  7,   165, 147, 81,  23,  165, 49,  101, 54,  224,
    75,  180, 81,  108, 18,  29,  226, 69,  225, 110, 175, 224, 42,  212, 25,  47,  130, 193, 110, 234, 192, 215, 252,
    56,  74,  162, 24,  46,  251, 174, 54,  106, 68,  245, 14,  9,   155, 160, 22,  120, 207, 104, 240, 29,  90,  178,
    140, 28,  24,  220, 47,  166, 112, 61,  251, 208, 192, 111, 56,  239, 238, 93,  255, 251, 62,  99,  32,  193, 75,
    61,  190, 235, 123, 229, 110, 218, 194, 85,  79,  225, 59,  98,  20,  238, 227, 235, 220, 11,  221, 149, 25,  180,
    116, 194, 159, 111, 96,  192, 24,  213, 59,  139, 179, 156, 215, 69,  230, 19,  24,  35,  135, 117, 206, 171, 206,
    162, 67,  129, 234, 61,  235, 11,  104, 103, 84,  64,  223, 167, 254, 40,  163, 101, 92,  84,  43,  150, 46,  249,
    219, 205, 7,   116, 11,  91,  104, 61,  57,  75,  223, 8,   48,  25,  28,  119, 252, 222, 113, 49,  86,  249, 74,
    180, 211, 156, 181, 61,  215, 168, 157, 7,   251, 199, 150, 242, 250, 91,  58,  132, 94,  121, 7,   53,  151, 139,
    98,  6,   165, 153, 69,  214, 32,  110, 211, 100, 101, 31,  89,  45,  81,  98,  23,  205, 205, 197, 209, 109, 186,
    198, 35,  141, 191, 249, 25,  60,  132, 223, 153, 251, 98,  20,  239, 146, 139, 20,  217, 250, 41,  250, 137, 58,
    177, 90,  57,  79,  51,  108, 233, 20,  253, 194, 187, 49,  222, 205, 114, 141, 96,  48,  175, 219, 107, 54,  111,
    138, 22,  154, 103, 108, 79,  58,  252, 179, 178, 79,  164, 195, 2,   153, 36,  39,  170, 199, 201, 167, 197, 85,
    106, 8,   59,  177, 81,  46,  56,  2,   230, 75,  114, 17,  55,  112, 188, 65,  208, 137, 77,  114, 10,  115, 55,
    58,  208, 197, 173, 122, 87,  6,   140, 110, 42,  208, 124, 163, 70,  108, 241, 104, 18,  245, 98,  214, 187, 134,
    53,  42,  221, 22,  182, 133, 211, 116, 148, 177, 194, 209, 192, 85,  90,  199, 58,  55,  203, 2,   229, 19,  137,
    187, 161, 228, 154, 112, 203, 145, 125, 244, 188, 220, 118, 228, 41,  201, 181, 41,  195, 144, 215, 183, 51,  80,
    250, 21,  217, 16,  217, 200, 235, 109, 227, 188, 122, 218, 142, 60,  170, 224, 112, 240, 184, 130, 229, 224, 113,
    5,   223, 148, 163, 80,  165, 183, 130, 187, 132, 116, 64,  238, 161, 85,  220, 115, 139, 205, 98,  227, 244, 29,
    102, 125, 7,   37,  243, 123, 223, 11,  26,  92,  63,  243, 116, 61,  191, 138, 123, 244, 160, 84,  186, 74,  31,
    5,   174, 247, 119, 135, 199, 248, 253, 135, 242, 97,  102, 145, 190, 144, 14,  85,  238, 221, 231, 193, 158, 48,
    205, 25,  120, 248, 15,  220, 29,  158, 9,   70,  185, 30,  103, 229, 33,  254, 23,  237, 160, 172, 62,  193, 90,
    222, 224, 232, 14,  200, 56,  90,  104, 142, 227, 120, 110, 6,   21,  211, 203, 65,  150, 99,  151, 220, 247, 87,
    164, 50,  159, 49,  239, 234, 58,  142, 0,   109, 108, 123, 18,  79,  227, 36,  100, 248, 222, 205, 96,  127, 120,
    26,  171, 228, 69,  63,  36,  17,  252, 200, 17,  116, 242, 187, 227, 88,  143, 247, 2,   75,  191, 6,   130, 59,
    188, 11,  55,  240, 31,  243, 122, 152, 226, 183, 207, 154, 73,  188, 39,  219, 43,  105, 222, 87,  41,  143, 141,
    140, 175, 73,  112, 184, 252, 61,  184, 16,  90,  250, 35,  168, 82,  119, 176, 57,  116, 94,  200, 150, 22,  190,
    179, 44,  104, 12,  235, 84,  149, 102, 252, 89,  154, 193, 99,  228, 106, 242, 125, 248, 64,  194, 255, 223, 127,
    242, 83,  11,  255, 2,   70,  214, 226, 128, 0,   0
};

    EVE_cs_set();
    spi_transmit((uint8_t) 0xB0U); /* старший байт REG_CMDB_WRITE + MEM_WRITE */
    spi_transmit((uint8_t) 0x25U); /* средний байт REG_CMDB_WRITE */
    spi_transmit((uint8_t) 0x78U); /* младший байт REG_CMDB_WRITE */
    private_block_write(eve_gt911_data, sizeof(eve_gt911_data));
    EVE_cs_clear();
    EVE_execute_cmd();

    EVE_memWrite8(REG_TOUCH_OVERSAMPLE, 0x0fU); /* настроить передискретизацию для 0x0f как «скрытую» в двоичном объекте для AN_336 */
    EVE_memWrite16(REG_TOUCH_CONFIG, 0x05D0U);  /* напишите волшебное печенье по запросу AN_336 */

    /* специально для модулейEVE2от Matrix-Orbital, для сбросаGT911нужно использовать GPIO3 */
    EVE_memWrite16(REG_GPIOX_DIR, 0x8008U); /* Reset-Value —0x8000, добавление0x08устанавливаетGPIO3на выходе, значение по умолчанию.
                                              для REG_GPIOX это 0x8000 -> Низкий выход на GPIO3 */
    DELAY_MS(1U);                           /* подожди больше 100 нас */
    EVE_memWrite8(REG_CPURESET, 0U);        /* очистить все сбросы */
    DELAY_MS(110U); /* ждать более 55мс - не работает мультитач, почему-то минимальная задержка 108мс
                      требуется */
    EVE_memWrite16(REG_GPIOX_DIR, 0x8000U); /* установка GPIO3 обратно на ввод */
#endif
}

/**
 * @brief Ожидает чтенияREG_IDсо значением 0x7c, указывая, что
 *  чип EVE присутствует и готов к обмену данными или пока не пройдет тайм-аут 400 мс.
 * @return ВозвращаетE_OKв случае успеха, EVE_FAIL_REGID_TIMEOUT, если
 * значение 0x7c не удалось прочитать.
 */
static uint8_t wait_regid(void)
{
    uint8_t ret = EVE_FAIL_REGID_TIMEOUT;
    uint8_t regid = 0U;

    for (uint16_t timeout = 0U; timeout < 400U; timeout++)
    {
        DELAY_MS(1U);

        regid = EVE_memRead8(REG_ID);
        if (0x7cU == regid) /* EVE запущен и работает. */
        {
            ret = E_OK;
            break;
        }
    }

    return (ret);
}

/**
 * @brief Ожидает, покаREG_CPURESETукажет, что звук, сенсорный и
 * сопроцессоры завершили соответствующие циклы сброса,
 * или пока не пройдет таймаут 50 мс.
 * @return ВозвращаетE_OKв случае успеха, EVE_FAIL_RESET_TIMEOUT, если
 * аудио, сенсорный или сопроцессорный блок указывает на неисправность, не возвращаясь после сброса.
 */
static uint8_t wait_reset(void)
{
    uint8_t ret = EVE_FAIL_RESET_TIMEOUT;
    uint8_t reset = 0U;

    for (uint16_t timeout = 0U; timeout < 50U; timeout++)
    {
        DELAY_MS(1U);

        reset = EVE_memRead8(REG_CPURESET) & 7U;
        if (0U == reset) /* EVE сообщает обо всех работающих устройствах */
        {
            ret = E_OK;
            break;
        }
    }

    return (ret);
}

/**
 * @brief Записывает все параметры, определенные для дисплея, выбранного в EVE_config.h.
 * в соответствующие регистры.
 * Он использует EVE_init() и при необходимости может использоваться для обновления результатов регистра.
 */
void EVE_write_display_parameters(void)
{
    /* Инициализировать дисплей */
    EVE_memWrite16(REG_HSIZE, EVE_HSIZE);     /* активная ширина дисплея */
    EVE_memWrite16(REG_HCYCLE, EVE_HCYCLE);   /* общее количество часов на линию, включая переднее/заднее крыльцо */
    EVE_memWrite16(REG_HOFFSET, EVE_HOFFSET); /* начало активной строки */
    EVE_memWrite16(REG_HSYNC0, EVE_HSYNC0);   /* начало строчного синхроимпульса */
    EVE_memWrite16(REG_HSYNC1, EVE_HSYNC1);   /* конец строчного синхроимпульса */
    EVE_memWrite16(REG_VSIZE, EVE_VSIZE);     /* активная высота дисплея */
    EVE_memWrite16(REG_VCYCLE, EVE_VCYCLE);   /* общее количество строк на экране, включая до/после */
    EVE_memWrite16(REG_VOFFSET, EVE_VOFFSET); /* начало активного экрана */
    EVE_memWrite16(REG_VSYNC0, EVE_VSYNC0);   /* начало вертикального синхроимпульса */
    EVE_memWrite16(REG_VSYNC1, EVE_VSYNC1);   /* конец вертикального синхроимпульса */
    EVE_memWrite8(REG_SWIZZLE, EVE_SWIZZLE);  /* Выход FT8xx на LCD — порядок контактов */
    EVE_memWrite8(REG_PCLK_POL, EVE_PCLKPOL); /* Данные LCD синхронизируются на этом ребре PCLK. */
    EVE_memWrite8(REG_CSPREAD, EVE_CSPREAD);  /* помогает с шумом, при значении 1 одновременно изменяется меньше сигналов, сброс по умолчанию: 1 */

    /* настроить сенсорный */
    EVE_memWrite8(REG_TOUCH_MODE, EVE_TMODE_CONTINUOUS); /* включить сенсорный ввод */
#if defined (EVE_TOUCH_RZTHRESH)
    EVE_memWrite16(REG_TOUCH_RZTHRESH, EVE_TOUCH_RZTHRESH); /* настроить чувствительность резистивного касания */
#else
    EVE_memWrite16(REG_TOUCH_RZTHRESH, 1200U); /* установите разумное значение по умолчанию, если оно не указано */
#endif

#if defined (EVE_ROTATE)
    EVE_memWrite8(REG_ROTATE, EVE_ROTATE & 7U); /* bit0 = инвертировать, бит2 = книжная ориентация, бит3 = зеркальное отображение */
    /* значение по умолчанию для сброса: 0x0 — не инвертировано, горизонтально, не зеркально. */
#endif
}

static void enable_pixel_clock(void)
{
    EVE_memWrite8(REG_GPIO, 0x80U); /* включите сигнал DISP на панель LCD, по умолчанию он настроен на вывод в REG_GPIO_DIR */

#if (EVE_GEN > 3) && (defined EVE_PCLK_FREQ)
    EVE_memWrite16(REG_PCLK_FREQ, (uint16_t) EVE_PCLK_FREQ);

#if defined (EVE_SET_REG_PCLK_2X)
    EVE_memWrite8(REG_PCLK_2X, 1U);
#endif

    EVE_memWrite8(REG_PCLK, 1U); /* включить режим extsync */
#else
    EVE_memWrite8(REG_PCLK, EVE_PCLK); /* начать синхронизацию данных на панели LCD */
#endif
}

/**
 * @brief ИнициализируетEVEв соответствии с выбранной конфигурацией из EVE_config.h.
 * @return E_OK в случае успеха
 * @note - Должно быть выполнено с настройкойSPIна 11 МГц или меньше, как того требует FT8xx/BT8xx!
 * @note - Дополнительные настройки можно выполнить с помощью дополнительных макросов.
 * @note -EVE_TOUCH_RZTHRESH- настройка чувствительности резистивного касания, по умолчанию 1200.
 * @note -EVE_ROTATE- установить поворот экрана: бит0 = инвертировать, бит1 = книжная, бит2 = зеркальный.
 * @note - требуется набор калибровочных значений для выбранного вращения, поскольку оно вращается перед калибровкой!
 * @note -EVE_BACKLIGHT_FREQ- настроить частоту подсветки, по умолчанию ее не пишет, получается 250Гц.
 * @note -EVE_BACKLIGHT_PWM- настройка ШИМ подсветки, по умолчанию 0x20/25%.
 */
uint8_t EVE_init(void)
{
    uint8_t ret;

    EVE_pdn_set();
    DELAY_MS(6U); /* минимальное время отключения питания составляет 5 мс. */
    EVE_pdn_clear();
    DELAY_MS(21U); /* минимальное время от повышения PD_N до первого доступа составляет 20 мс. */

#if defined (EVE_GD3X)
    EVE_cmdWrite(EVE_RST_PULSE,0U); /* сброс, требуется только для горячего запуска, если линия PowerDown не используется */
#endif

    if(EVE_HAS_CRYSTAL) {
        EVE_cmdWrite(EVE_CLKEXT, 0U); /* настройка EVE для внешних часов */
    }
    else {
        EVE_cmdWrite(EVE_CLKINT, 0U); /* настройка EVE для внутренних часов */
    }

#if EVE_GEN > 2
    EVE_cmdWrite(EVE_CLKSEL, 0x46U); /* установите тактовую частоту на 72 МГц */
#endif

    EVE_cmdWrite(EVE_ACTIVE, 0U); /* начать EVE */
    DELAY_MS(40U); /* дайте EVE минуту молчания, чтобы включиться */

    ret = wait_regid();
    if (E_OK == ret)
    {
        ret = wait_reset();
        if (E_OK == ret)
        {
/* скажите EVE, что мы изменили частоту со стандартной на 72 МГц для BT8xx */
#if EVE_GEN > 2
            EVE_memWrite32(REG_FREQUENCY, 72000000UL);
#endif

/* у нас есть дисплей с сенсорным контроллером GoodixGT911/GT9271,
 поэтому мы исправляем наши FT811 или FT813 в соответствии с AN_336 или настраиваем BT815 / BT817 соответственно. */
            if(EVE_HAS_GT911) {
                use_gt911();
            }

#if defined (EVE_ADAM101)
            EVE_memWrite8(REG_PWM_DUTY, 0x80U); /* Включена подсветка модуля Glyn ADAM101, он использует инвертированные значения. */
#else
            EVE_memWrite8(REG_PWM_DUTY, 0U); /* отключить подсветку для любого другого модуля */
#endif
            EVE_write_display_parameters();

            /* отключить звук сейчас */
            EVE_memWrite8(REG_VOL_PB, 0U);      /* уменьшите громкость произнесенного звука, сброс по умолчанию — 0xff */
            EVE_memWrite8(REG_VOL_SOUND, 0U);   /* уменьшите громкость синтезатора, сброс по умолчанию — 0xff */
            EVE_memWrite16(REG_SOUND, EVE_MUTE); /* отключить звук синтезатора */

            /* напишите базовый список отображения, чтобы начать работу */
            EVE_memWrite32(EVE_RAM_DL, DL_CLEAR_COLOR_RGB);
            EVE_memWrite32(EVE_RAM_DL + 4U, (DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG));
            EVE_memWrite32(EVE_RAM_DL + 8U, DL_DISPLAY); /* конец списка отображения */
            EVE_memWrite32(REG_DLSWAP, EVE_DLSWAP_FRAME);
            /* пока ничего не отображается... частота пикселей все еще 0x00 */

#if defined (EVE_GD3X)
            EVE_memWrite16(REG_OUTBITS,0x01B6U); /* GD3X использует только 6 бит на цвет */
#endif

            enable_pixel_clock();

            EVE_memWrite16(REG_PWM_HZ, EVE_BACKLIGHT_FREQ); /* установить частоту подсветки на настроенное значение */

            EVE_memWrite8(REG_PWM_DUTY, EVE_BACKLIGHT_PWM); /* установить ШИМ подсветки на запрошенный пользователем уровень */
            DELAY_MS(1U);
            EVE_execute_cmd(); /* на всякий случай подождите, пока EVE не будет занят */

#if defined (EVE_DMA)
            EVE_init_dma(); /* подготовить DMA */
#endif
        }
    }

    return (ret);
}

/* ##################################################################
    функции для отображения списков
##################################################################### */

/**
 * @brief Начните последовательность команд или подготовьте передачу DMA, если это применимо.
 * @note - Необходимо использовать с EVE_end_cmd_burst().
 * @note - Не используйте в последовательности какие-либо функции, которые не обращаются к командному Fifo, как, например, любую из функцийEVE_mem...().
 * @note - Не используйте функции, не поддерживающие пакетный режим.
 */
void EVE_start_cmd_burst(void)
{
#if defined (EVE_DMA)
    if (EVE_dma_busy)
    {
        EVE_execute_cmd(); /* это мера защиты построения сегментированного списка отображения с помощью DMA от перекрытия. */
    }
#endif

    cmd_burst = 42U;

#if defined (EVE_DMA)
    EVE_dma_buffer[0U] = 0x7825B000UL; /* REG_CMDB_WRITE + MEM_WRITE низкий средний привет 00 */
//    (( uint8_t ) ( ft_address >> 16U) | MEM_WRITE ) | ( ft_address & 0x0000ff00UL) | (( uint8_t ) ( ft_address ) << 16U);
//    EVE_dma_buffer[0U] = EVE_dma_buffer[0U] << 8U;
    EVE_dma_buffer_index = 1U;
#else
    EVE_cs_set();
    spi_transmit((uint8_t) 0xB0U); /* старший байт REG_CMDB_WRITE + MEM_WRITE */
    spi_transmit((uint8_t) 0x25U); /* средний байт REG_CMDB_WRITE */
    spi_transmit((uint8_t) 0x78U); /* младший байт REG_CMDB_WRITE */
#endif
}

/**
 * @brief Завершите последовательность команд или запустите подготовленную передачу DMA, если это применимо.
 * @note - Необходимо использовать с EVE_start_cmd_burst().
 */
void EVE_end_cmd_burst(void)
{
    cmd_burst = 0U;

#if defined (EVE_DMA)
    EVE_start_dma_transfer(); /* начать передачу DMA */
#else
    EVE_cs_clear();
#endif
}

/* записать строку в память сопроцессора в контексте команды: */
/* без выбора чипа, просто SPI -передачи */
static void private_string_write(const char *p_text)
{
    /* относиться к массиву как к группе байтов */
    const uint8_t *const p_bytes = (const uint8_t *)p_text;

    if (0U == cmd_burst)
    {
        uint8_t textindex = 0U;
        uint8_t padding;

        /* либо оставьте на нуле, либо когда строка слишком длинная */
        while ((textindex < 249U) && (p_bytes[textindex] != 0U))
        {
            spi_transmit(p_bytes[textindex]);
            textindex++;
        }

        /* передать хотя бы один байт 0x00 */
        /* и до четырех, если строка уже выровнена по 4 байтам */
        padding = textindex & 3U; /* 0, 1, 2 or 3 */
        padding = 4U - padding;   /* 4, 3, 2 or 1 */

        while (padding > 0U)
        {
            spi_transmit(0U);
            padding--;
        }
    }
    else /* мы находимся в пакетном режиме, поэтому каждая передача составляет 32 бита */
    {
        for (uint8_t textindex = 0U; textindex < 249U; textindex += 4U)
        {
            uint32_t calc = 0U;

            for (uint8_t index = 0U; index < 4U; index++)
            {
                uint8_t data;

                data = p_bytes[textindex + index];

                if (0U == data)
                {
                    spi_transmit_burst(calc);
                    return; /* MISRA Нарушение правила 15.5 (рекомендательное) 2012 г. */
                }

                calc += ((uint32_t)data) << (index * 8U);
            }

            spi_transmit_burst(calc);
        }

        spi_transmit_burst(0U); /* выполняется, когда строка слишком длинная */
    }
}

/* BT817 / BT818 */
#if EVE_GEN > 3

/**
 * @brief Рендеринг одного кадра анимации в формате RAM_G.
 */
void EVE_cmd_animframeram(int16_t xc0, int16_t yc0, uint32_t aoptr, uint32_t frame)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_ANIMFRAMERAM);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit_32(aoptr);
        spi_transmit_32(frame);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ANIMFRAMERAM);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
        spi_transmit_burst(aoptr);
        spi_transmit_burst(frame);
    }
}

/**
 * @brief Рендеринг одного кадра вRAM_Gанимации, работает только в серийном режиме.
 */
void EVE_cmd_animframeram_burst(int16_t xc0, int16_t yc0, uint32_t aoptr,
                                uint32_t frame)
{
    spi_transmit_burst(CMD_ANIMFRAMERAM);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    spi_transmit_burst(aoptr);
    spi_transmit_burst(frame);
}

/**
 * @brief Запустите анимацию в RAM_G.
 */
void EVE_cmd_animstartram(int32_t chnl, uint32_t aoptr, uint32_t loop)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_ANIMSTARTRAM);
        spi_transmit_32((uint32_t) chnl);
        spi_transmit_32(aoptr);
        spi_transmit_32(loop);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ANIMSTARTRAM);
        spi_transmit_burst((uint32_t) chnl);
        spi_transmit_burst(aoptr);
        spi_transmit_burst(loop);
    }
}

/**
 * @brief Запустите анимацию в RAM_G, работает только в пакетном режиме.
 */
void EVE_cmd_animstartram_burst(int32_t chnl, uint32_t aoptr, uint32_t loop)
{
    spi_transmit_burst(CMD_ANIMSTARTRAM);
    spi_transmit_burst((uint32_t) chnl);
    spi_transmit_burst(aoptr);
    spi_transmit_burst(loop);
}

/**
 * @brief Устанавливает уровень API, используемый сопроцессором.
 */
void EVE_cmd_apilevel(uint32_t level)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_APILEVEL);
        spi_transmit_32(level);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_APILEVEL);
        spi_transmit_burst(level);
    }
}

/**
 * @brief Устанавливает уровень API, используемый сопроцессором, работает только в пакетном режиме.
 */
void EVE_cmd_apilevel_burst(uint32_t level)
{
    spi_transmit_burst(CMD_APILEVEL);
    spi_transmit_burst(level);
}

/**
 * @brief Выполните процедуру калибровки сенсорного экрана для дополнительного окна.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_calibratesub(uint16_t xc0, uint16_t yc0, uint16_t width, uint16_t height)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_CALIBRATESUB);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit((uint8_t) (width));
        spi_transmit((uint8_t) (width >> 8U));
        spi_transmit((uint8_t) (height));
        spi_transmit((uint8_t) (height >> 8U));
        EVE_cs_clear();
    }
}

/**
 * @brief Вызывает список команд в RAM_G.
 */
void EVE_cmd_calllist(uint32_t adr)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_CALLLIST);
        spi_transmit_32(adr);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_CALLLIST);
        spi_transmit_burst(adr);
    }
}

/**
 * @brief Вызывает список команд в RAM_G, работает только в пакетном режиме.
 */
void EVE_cmd_calllist_burst(uint32_t adr)
{
    spi_transmit_burst(CMD_CALLLIST);
    spi_transmit_burst(adr);
}

/**
 * @brief Настройте фильтр горизонтальной развертки для поддержки неквадратных пикселей LCD.
 * @note - Не поддерживает пакетный режим.
 */
void EVE_cmd_hsf(uint32_t hsf)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_HSF);
        spi_transmit_32(hsf);
        EVE_cs_clear();
    }
}

/**
 * @brief Воспроизводите/запускайте анимацию до завершения.
 */
void EVE_cmd_runanim(uint32_t waitmask, uint32_t play)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_RUNANIM);
        spi_transmit_32(waitmask);
        spi_transmit_32(play);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_RUNANIM);
        spi_transmit_burst(waitmask);
        spi_transmit_burst(play);
    }
}

/**
 * @brief Воспроизведение/запуск анимации до завершения, работает только в пакетном режиме.
 */
void EVE_cmd_runanim_burst(uint32_t waitmask, uint32_t play)
{
    spi_transmit_burst(CMD_RUNANIM);
    spi_transmit_burst(waitmask);
    spi_transmit_burst(play);
}

#endif /* EVE_GEN > 3 */

/* BT815 / BT816 */
#if EVE_GEN > 2

/**
 * @brief Нарисуйте одну или несколько активных анимаций.
 */
void EVE_cmd_animdraw(int32_t chnl)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_ANIMDRAW);
        spi_transmit_32((uint32_t) chnl);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ANIMDRAW);
        spi_transmit_burst((uint32_t) chnl);
    }
}

/**
 * @brief Нарисуйте одну или несколько активных анимаций, работает только в серийном режиме.
 */
void EVE_cmd_animdraw_burst(int32_t chnl)
{
    spi_transmit_burst(CMD_ANIMDRAW);
    spi_transmit_burst((uint32_t) chnl);
}

/**
 * @brief Нарисуйте указанный кадр анимации.
 */
void EVE_cmd_animframe(int16_t xc0, int16_t yc0, uint32_t aoptr, uint32_t frame)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_ANIMFRAME);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit_32(aoptr);
        spi_transmit_32(frame);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ANIMFRAME);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
        spi_transmit_burst(aoptr);
        spi_transmit_burst(frame);
    }
}

/**
 * @brief Нарисовать указанный кадр анимации, работает только в серийном режиме.
 */
void EVE_cmd_animframe_burst(int16_t xc0, int16_t yc0, uint32_t aoptr,
                                uint32_t frame)
{
    spi_transmit_burst(CMD_ANIMFRAME);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    spi_transmit_burst(aoptr);
    spi_transmit_burst(frame);
}

/**
 * @brief Запустите анимацию.
 */
void EVE_cmd_animstart(int32_t chnl, uint32_t aoptr, uint32_t loop)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_ANIMSTART);
        spi_transmit_32((uint32_t) chnl);
        spi_transmit_32(aoptr);
        spi_transmit_32(loop);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ANIMSTART);
        spi_transmit_burst((uint32_t) chnl);
        spi_transmit_burst(aoptr);
        spi_transmit_burst(loop);
    }
}

/**
 * @brief Запуск анимации, работает только в пакетном режиме.
 */
void EVE_cmd_animstart_burst(int32_t chnl, uint32_t aoptr, uint32_t loop)
{
    spi_transmit_burst(CMD_ANIMSTART);
    spi_transmit_burst((uint32_t) chnl);
    spi_transmit_burst(aoptr);
    spi_transmit_burst(loop);
}

/**
 * @brief Останавливает одну или несколько активных анимаций.
 */
void EVE_cmd_animstop(int32_t chnl)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_ANIMSTOP);
        spi_transmit_32((uint32_t) chnl);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ANIMSTOP);
        spi_transmit_burst((uint32_t) chnl);
    }
}

/**
 * @brief Останавливает одну или несколько активных анимаций, работает только в серийном режиме.
 */
void EVE_cmd_animstop_burst(int32_t chnl)
{
    spi_transmit_burst(CMD_ANIMSTOP);
    spi_transmit_burst((uint32_t) chnl);
}

/**
 * @brief Устанавливает координаты анимации.
 */
void EVE_cmd_animxy(int32_t chnl, int16_t xc0, int16_t yc0)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_ANIMXY);
        spi_transmit_32((uint32_t) chnl);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ANIMXY);
        spi_transmit_burst((uint32_t) chnl);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    }
}

/**
 * @brief Устанавливает координаты анимации, работает только в серийном режиме.
 */
void EVE_cmd_animxy_burst(int32_t chnl, int16_t xc0, int16_t yc0)
{
    spi_transmit_burst(CMD_ANIMXY);
    spi_transmit_burst((uint32_t) chnl);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
}

/**
 * @brief Добавьте флэш-данные в список отображения.
 */
void EVE_cmd_appendf(uint32_t ptr, uint32_t num)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_APPENDF);
        spi_transmit_32(ptr);
        spi_transmit_32(num);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_APPENDF);
        spi_transmit_burst(ptr);
        spi_transmit_burst(num);
    }
}

/**
 * @brief Добавление флэш-данных в список отображения, работает только в пакетном режиме.
 */
void EVE_cmd_appendf_burst(uint32_t ptr, uint32_t num)
{
    spi_transmit_burst(CMD_APPENDF);
    spi_transmit_burst(ptr);
    spi_transmit_burst(num);
}

/**
 * @brief Вычисляет преобразование растрового изображения и добавляет командыBITMAP_TRANSFORM_A...BITMAP_TRANSFORM_Fв список отображения.
 */
uint16_t EVE_cmd_bitmap_transform(int32_t xc0, int32_t yc0, int32_t xc1,
                                int32_t yc1, int32_t xc2, int32_t yc2,
                                int32_t tx0, int32_t ty0, int32_t tx1,
                                int32_t ty1, int32_t tx2, int32_t ty2)
{
    uint16_t ret_val = 0U;

    if (0U == cmd_burst)
    {
        uint16_t cmdoffset;

        eve_begin_cmd(CMD_BITMAP_TRANSFORM);
        spi_transmit_32((uint32_t) xc0);
        spi_transmit_32((uint32_t) yc0);
        spi_transmit_32((uint32_t) xc1);
        spi_transmit_32((uint32_t) yc1);
        spi_transmit_32((uint32_t) xc2);
        spi_transmit_32((uint32_t) yc2);
        spi_transmit_32((uint32_t) tx0);
        spi_transmit_32((uint32_t) ty0);
        spi_transmit_32((uint32_t) tx1);
        spi_transmit_32((uint32_t) ty1);
        spi_transmit_32((uint32_t) tx2);
        spi_transmit_32((uint32_t) ty2);
        spi_transmit_32(0UL);
        EVE_cs_clear();
        EVE_execute_cmd();
        cmdoffset = EVE_memRead16(REG_CMD_WRITE);
        cmdoffset -= 4U;
        cmdoffset &= 0x0fffU;
        ret_val = (uint16_t) EVE_memRead32(EVE_RAM_CMD + cmdoffset);
    }
    else /* note: параметр результата игнорируется в пакетном режиме */
    {
        spi_transmit_burst(CMD_BITMAP_TRANSFORM);
        spi_transmit_burst((uint32_t) xc0);
        spi_transmit_burst((uint32_t) yc0);
        spi_transmit_burst((uint32_t) xc1);
        spi_transmit_burst((uint32_t) yc1);
        spi_transmit_burst((uint32_t) xc2);
        spi_transmit_burst((uint32_t) yc2);
        spi_transmit_burst((uint32_t) tx0);
        spi_transmit_burst((uint32_t) ty0);
        spi_transmit_burst((uint32_t) tx1);
        spi_transmit_burst((uint32_t) ty1);
        spi_transmit_burst((uint32_t) tx2);
        spi_transmit_burst((uint32_t) ty2);
        spi_transmit_burst(0UL);
    }
    return (ret_val);
}

/**
 * @brief Вычисляет преобразование растрового изображения и добавляет командыBITMAP_TRANSFORM_A...BITMAP_TRANSFORM_Fв список отображения.
 * @note - Работает только в пакетном режиме, параметр result игнорируется.
 */
void EVE_cmd_bitmap_transform_burst(int32_t xc0, int32_t yc0, int32_t xc1,
                                int32_t yc1, int32_t xc2, int32_t yc2,
                                int32_t tx0, int32_t ty0, int32_t tx1,
                                int32_t ty1, int32_t tx2, int32_t ty2)
{
    spi_transmit_burst(CMD_BITMAP_TRANSFORM);
    spi_transmit_burst((uint32_t) xc0);
    spi_transmit_burst((uint32_t) yc0);
    spi_transmit_burst((uint32_t) xc1);
    spi_transmit_burst((uint32_t) yc1);
    spi_transmit_burst((uint32_t) xc2);
    spi_transmit_burst((uint32_t) yc2);
    spi_transmit_burst((uint32_t) tx0);
    spi_transmit_burst((uint32_t) ty0);
    spi_transmit_burst((uint32_t) tx1);
    spi_transmit_burst((uint32_t) ty1);
    spi_transmit_burst((uint32_t) tx2);
    spi_transmit_burst((uint32_t) ty2);
    spi_transmit_burst(0UL);
}

/**
 * @brief Устанавливает ширину заливки пикселей дляCMD_TEXT,CMD_BUTTON,CMD_BUTTONс помощью опции OPT_FILL.
 */
void EVE_cmd_fillwidth(uint32_t pixel)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_FILLWIDTH);
        spi_transmit_32(pixel);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_FILLWIDTH);
        spi_transmit_burst(pixel);
    }
}

/**
 * @brief Устанавливает ширину заливки пикселей дляCMD_TEXT,CMD_BUTTON,CMD_BUTTONс помощью опции OPT_FILL.
 * @note - Работает только в пакетном режиме.
 */
void EVE_cmd_fillwidth_burst(uint32_t pixel)
{
    spi_transmit_burst(CMD_FILLWIDTH);
    spi_transmit_burst(pixel);
}

/**
 * @brief Нарисуйте плавный цветовой градиент с прозрачностью.
 */
void EVE_cmd_gradienta(int16_t xc0, int16_t yc0, uint32_t argb0, int16_t xc1, int16_t yc1, uint32_t argb1)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_GRADIENTA);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit_32(argb0);
        spi_transmit((uint8_t) ((uint16_t) xc1));
        spi_transmit((uint8_t) (((uint16_t) xc1) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc1));
        spi_transmit((uint8_t) (((uint16_t) yc1) >> 8U));
        spi_transmit_32(argb1);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_GRADIENTA);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
        spi_transmit_burst(argb0);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc1)) + (((uint32_t) ((uint16_t) yc1)) << 16U));
        spi_transmit_burst(argb1);
    }
}

/**
 * @brief Нарисуйте плавный цветовой градиент с прозрачностью, работает только в серийном режиме.
 */
void EVE_cmd_gradienta_burst(int16_t xc0, int16_t yc0, uint32_t argb0, int16_t xc1, int16_t yc1, uint32_t argb1)
{
    spi_transmit_burst(CMD_GRADIENTA);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    spi_transmit_burst(argb0);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc1)) + (((uint32_t) ((uint16_t) yc1)) << 16U));
    spi_transmit_burst(argb1);
}

/**
 * @brief Примените поворот и масштабирование вокруг указанной координаты.
 */
void EVE_cmd_rotatearound(int32_t xc0, int32_t yc0, uint32_t angle, int32_t scale)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_ROTATEAROUND);
        spi_transmit_32((uint32_t) xc0);
        spi_transmit_32((uint32_t) yc0);
        spi_transmit_32(angle & 0xFFFFUL);
        spi_transmit_32((uint32_t) scale);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ROTATEAROUND);
        spi_transmit_burst((uint32_t) xc0);
        spi_transmit_burst((uint32_t) yc0);
        spi_transmit_burst(angle & 0xFFFFUL);
        spi_transmit_burst((uint32_t) scale);
    }
}

/**
 * @brief Примените вращение и масштабирование вокруг указанной координаты, работает только в пакетном режиме.
 */
void EVE_cmd_rotatearound_burst(int32_t xc0, int32_t yc0, uint32_t angle,
                                int32_t scale)
{
    spi_transmit_burst(CMD_ROTATEAROUND);
    spi_transmit_burst((uint32_t) xc0);
    spi_transmit_burst((uint32_t) yc0);
    spi_transmit_burst(angle & 0xFFFFUL);
    spi_transmit_burst((uint32_t) scale);
}

/**
 * @brief Нарисуйте кнопку с меткой, версия с переменным числом аргументов.
 * @param p_arguments [] указатель на массив массивов, преобразованных вuint32_t, который будет использоваться при использовании EVE_OPT_FORMAT
 * @param num_args количество элементов, указанное вp_arguments[]
 */
void EVE_cmd_button_var(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt,
                        uint16_t font, uint16_t options, const char *p_text,
                        uint8_t num_args, const uint32_t p_arguments[])
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_BUTTON);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit((uint8_t) (wid));
        spi_transmit((uint8_t) (wid >> 8U));
        spi_transmit((uint8_t) (hgt));
        spi_transmit((uint8_t) (hgt >> 8U));
        spi_transmit((uint8_t) (font));
        spi_transmit((uint8_t) (font >> 8U));
        spi_transmit((uint8_t) (options));
        spi_transmit((uint8_t) (options >> 8U));
        private_string_write(p_text);

        if ((options & EVE_OPT_FORMAT) != 0U)
        {
            if (p_arguments != NULL)
            {
                for (uint8_t counter = 0U; counter < num_args; counter++)
                {
                    spi_transmit_32(p_arguments[counter]);
                }
            }
        }
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_BUTTON);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
        spi_transmit_burst(((uint32_t) wid) + ((uint32_t) hgt << 16U));
        spi_transmit_burst(((uint32_t) font) + ((uint32_t) options << 16U));
        private_string_write(p_text);

        if ((options & EVE_OPT_FORMAT) != 0U)
        {
            if (p_arguments != NULL)
            {
                for (uint8_t counter = 0U; counter < num_args; counter++)
                {
                    spi_transmit_burst(p_arguments[counter]);
                }
            }
        }
    }
}

/**
 * @brief Нарисуйте кнопку с меткой, версия с varargs работает только в пакетном режиме.
 * @param p_arguments [] указатель на массив массивов, преобразованных вuint32_t, который будет использоваться при использовании EVE_OPT_FORMAT
 * @param num_args количество элементов, указанное вp_arguments[]
 */
void EVE_cmd_button_var_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt,
                              uint16_t font, uint16_t options, const char *p_text,
                              uint8_t num_args, const uint32_t p_arguments[])
{
    spi_transmit_burst(CMD_BUTTON);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    spi_transmit_burst(((uint32_t) wid) + ((uint32_t) hgt << 16U));
    spi_transmit_burst(((uint32_t) font) + ((uint32_t) options << 16U));
    private_string_write(p_text);

    if ((options & EVE_OPT_FORMAT) != 0U)
    {
        if (p_arguments != NULL)
        {
            for (uint8_t counter = 0U; counter < num_args; counter++)
            {
                spi_transmit_burst(p_arguments[counter]);
            }
        }
    }
}

/**
 * @brief Нарисуйте текстовую строку, версия с переменным числом аргументов.
 * @param p_arguments [] указатель на массив массивов, преобразованных вuint32_t, который будет использоваться при использовании EVE_OPT_FORMAT
 * @param num_args количество элементов, указанное вp_arguments[]
 */
void EVE_cmd_text_var(int16_t xc0, int16_t yc0, uint16_t font,
                        uint16_t options, const char *p_text,
                        uint8_t num_args, const uint32_t p_arguments[])
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_TEXT);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit((uint8_t) (font));
        spi_transmit((uint8_t) (font >> 8U));
        spi_transmit((uint8_t) (options));
        spi_transmit((uint8_t) (options >> 8U));
        private_string_write(p_text);

        if ((options & EVE_OPT_FORMAT) != 0U)
        {
            if (p_arguments != NULL)
            {
                for (uint8_t counter = 0U; counter < num_args; counter++)
                {
                    spi_transmit_32(p_arguments[counter]);
                }
            }
        }
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_TEXT);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
        spi_transmit_burst(((uint32_t) font) + (((uint32_t) options) << 16U));
        private_string_write(p_text);

        if ((options & EVE_OPT_FORMAT) != 0U)
        {
            if (p_arguments != NULL)
            {
                for (uint8_t counter = 0U; counter < num_args; counter++)
                {
                    spi_transmit_burst(p_arguments[counter]);
                }
            }
        }
    }
}

/**
 * @brief Нарисуйте текстовую строку, версия с переменным числом аргументов.
 * @param p_arguments [] указатель на массив массивов, преобразованных вuint32_t, который будет использоваться при использовании EVE_OPT_FORMAT
 * @param num_args количество элементов, указанное вp_arguments[]
 */
void EVE_cmd_text_var_burst(int16_t xc0, int16_t yc0, uint16_t font,
                            uint16_t options, const char *p_text,
                            uint8_t num_args, const uint32_t p_arguments[])
{
    spi_transmit_burst(CMD_TEXT);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    spi_transmit_burst(((uint32_t) font) + (((uint32_t) options) << 16U));
    private_string_write(p_text);

    if ((options & EVE_OPT_FORMAT) != 0U)
    {
        if (p_arguments != NULL)
        {
            for (uint8_t counter = 0U; counter < num_args; counter++)
            {
                spi_transmit_burst(p_arguments[counter]);
            }
        }
    }
}

/**
 * @brief Нарисуйте тумблер с метками, версия с переменным количеством аргументов.
 * @param p_arguments [] указатель на массив массивов, преобразованных вuint32_t, который будет использоваться при использовании EVE_OPT_FORMAT
 * @param num_args количество элементов, указанное вp_arguments[]
 */
void EVE_cmd_toggle_var(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t font,
                        uint16_t options, uint16_t state, const char *p_text,
                        uint8_t num_args, const uint32_t p_arguments[])
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_TOGGLE);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit((uint8_t) (wid));
        spi_transmit((uint8_t) (wid >> 8U));
        spi_transmit((uint8_t) (font));
        spi_transmit((uint8_t) (font >> 8U));
        spi_transmit((uint8_t) (options));
        spi_transmit((uint8_t) (options >> 8U));
        spi_transmit((uint8_t) (state));
        spi_transmit((uint8_t) (state >> 8U));
        private_string_write(p_text);

        if ((options & EVE_OPT_FORMAT) != 0U)
        {
            if (p_arguments != NULL)
            {
                for (uint8_t counter = 0U; counter < num_args; counter++)
                {
                    spi_transmit_32(p_arguments[counter]);
                }
            }
        }
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_TOGGLE);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
        spi_transmit_burst(((uint32_t) wid) + (((uint32_t) font) << 16U));
        spi_transmit_burst(((uint32_t) options) + (((uint32_t) state) << 16U));
        private_string_write(p_text);

        if ((options & EVE_OPT_FORMAT) != 0U)
        {
            if (p_arguments != NULL)
            {
                for (uint8_t counter = 0U; counter < num_args; counter++)
                {
                    spi_transmit_burst(p_arguments[counter]);
                }
            }
        }
    }
}

/**
 * @brief Нарисуйте тумблер с метками, версия с varargs, работает только в пакетном режиме.
 * @param p_arguments [] указатель на массив массивов, преобразованных вuint32_t, который будет использоваться при использовании EVE_OPT_FORMAT
 * @param num_args количество элементов, указанное вp_arguments[]
 */
void EVE_cmd_toggle_var_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t font,
                            uint16_t options, uint16_t state, const char *p_text,
                            uint8_t num_args, const uint32_t p_arguments[])
{
    spi_transmit_burst(CMD_TOGGLE);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    spi_transmit_burst(((uint32_t) wid) + (((uint32_t) font) << 16U));
    spi_transmit_burst(((uint32_t) options) + (((uint32_t) state) << 16U));
    private_string_write(p_text);

    if ((options & EVE_OPT_FORMAT) != 0U)
    {
        if (p_arguments != NULL)
        {
            for (uint8_t counter = 0U; counter < num_args; counter++)
            {
                spi_transmit_burst(p_arguments[counter]);
            }
        }
    }
}

#endif /* EVE_GEN > 2 */

/**
 * @brief Общая функция для команд списка отображения и сопроцессора без аргументов, работает только в пакетном режиме.
 * @note -EVE_cmd_dl(CMD_DLSTART);
 * @note -EVE_cmd_dl(CMD_SWAP);
 * @note -EVE_cmd_dl(CMD_SCREENSAVER);
 * @note -EVE_cmd_dl(VERTEX2F(0,0));
 * @note -EVE_cmd_dl(DL_BEGIN|EVE_RECTS);
 */
void EVE_cmd_dl(uint32_t command)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(command);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(command);
    }
}

/**
 * @brief Общая функция для команд списка отображения и сопроцессора без аргументов, работает только в пакетном режиме.
 */
void EVE_cmd_dl_burst(uint32_t command)
{
    spi_transmit_burst(command);
}

/**
 * @brief Добавляет команды изRAM_Gв список отображения.
 */
void EVE_cmd_append(uint32_t ptr, uint32_t num)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_APPEND);
        spi_transmit_32(ptr);
        spi_transmit_32(num);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_APPEND);
        spi_transmit_burst(ptr);
        spi_transmit_burst(num);
    }
}

/**
 * @brief Добавляет команды изRAM_Gв список отображения, работает только в пакетном режиме.
 */
void EVE_cmd_append_burst(uint32_t ptr, uint32_t num)
{
    spi_transmit_burst(CMD_APPEND);
    spi_transmit_burst(ptr);
    spi_transmit_burst(num);
}

/**
 * @brief Установите цвет фона.
 */
void EVE_cmd_bgcolor(uint32_t color)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_BGCOLOR);
        spi_transmit((uint8_t) (color));
        spi_transmit((uint8_t) (color >> 8U));
        spi_transmit((uint8_t) (color >> 16U));
        spi_transmit(0U);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_BGCOLOR);
        spi_transmit_burst(color);
    }
}

/**
 * @brief Установите цвет фона, работает только в серийном режиме.
 */
void EVE_cmd_bgcolor_burst(uint32_t color)
{
    spi_transmit_burst(CMD_BGCOLOR);
    spi_transmit_burst(color);
}

/**
 * @brief Нарисуйте кнопку с надписью.
 */
void EVE_cmd_button(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt,
                    uint16_t font, uint16_t options, const char *p_text)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_BUTTON);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit((uint8_t) (wid));
        spi_transmit((uint8_t) (wid >> 8U));
        spi_transmit((uint8_t) (hgt));
        spi_transmit((uint8_t) (hgt >> 8U));
        spi_transmit((uint8_t) (font));
        spi_transmit((uint8_t) (font >> 8U));
        spi_transmit((uint8_t) (options));
        spi_transmit((uint8_t) (options >> 8U));
        private_string_write(p_text);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_BUTTON);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
        spi_transmit_burst(((uint32_t) wid) + (((uint32_t) hgt) << 16U));
        spi_transmit_burst(((uint32_t) font) + (((uint32_t) options) << 16U));
        private_string_write(p_text);
    }
}

/**
 * @brief Нарисуйте кнопку с меткой, работает только в пакетном режиме.
 */
void EVE_cmd_button_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt,
                            uint16_t font, uint16_t options, const char *p_text)
{
    spi_transmit_burst(CMD_BUTTON);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    spi_transmit_burst(((uint32_t) wid) + (((uint32_t) hgt) << 16U));
    spi_transmit_burst(((uint32_t) font) + (((uint32_t) options) << 16U));
    private_string_write(p_text);
}

/**
 * @brief Выполните процедуру калибровки сенсорного экрана.
 * @note - не поддерживает пакетный режим
 */
void EVE_cmd_calibrate(void)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_CALIBRATE);
        spi_transmit_32(0UL);
        EVE_cs_clear();
    }
}

/**
 * @brief Нарисуйте аналоговые часы.
 */
void EVE_cmd_clock(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options,
                    uint16_t hours, uint16_t mins, uint16_t secs, uint16_t msecs)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_CLOCK);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit((uint8_t) (rad));
        spi_transmit((uint8_t) (rad >> 8U));
        spi_transmit((uint8_t) (options));
        spi_transmit((uint8_t) (options >> 8U));
        spi_transmit((uint8_t) (hours));
        spi_transmit((uint8_t) (hours >> 8U));
        spi_transmit((uint8_t) (mins));
        spi_transmit((uint8_t) (mins >> 8U));
        spi_transmit((uint8_t) (secs));
        spi_transmit((uint8_t) (secs >> 8U));
        spi_transmit((uint8_t) (msecs));
        spi_transmit((uint8_t) (msecs >> 8U));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_CLOCK);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
        spi_transmit_burst(((uint32_t) rad) + (((uint32_t) options) << 16U));
        spi_transmit_burst(((uint32_t) hours) + (((uint32_t) mins) << 16U));
        spi_transmit_burst(((uint32_t) secs) + (((uint32_t) msecs) << 16U));
    }
}

/**
 * @brief Нарисуйте аналоговые часы, работают только в пакетном режиме.
 */
void EVE_cmd_clock_burst(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options, uint16_t hours,
                            uint16_t mins, uint16_t secs, uint16_t msecs)
{
    spi_transmit_burst(CMD_CLOCK);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    spi_transmit_burst(((uint32_t) rad) + (((uint32_t) options) << 16U));
    spi_transmit_burst(((uint32_t) hours) + (((uint32_t) mins) << 16U));
    spi_transmit_burst(((uint32_t) secs) + (((uint32_t) msecs) << 16U));
}

/**
 * @brief Нарисуйте поворотный диск управления.
 */
void EVE_cmd_dial(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options, uint16_t val)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_DIAL);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit((uint8_t) (rad));
        spi_transmit((uint8_t) (rad >> 8U));
        spi_transmit((uint8_t) (options));
        spi_transmit((uint8_t) (options >> 8U));
        spi_transmit((uint8_t) (val));
        spi_transmit((uint8_t) (val >> 8U));
        spi_transmit(0U);
        spi_transmit(0U);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_DIAL);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
        spi_transmit_burst(((uint32_t) rad) + (((uint32_t) options) << 16U));
        spi_transmit_burst(val);
    }
}

/**
 * @brief Нарисуйте поворотный диск управления, работает только в серийном режиме.
 */
void EVE_cmd_dial_burst(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options,
                        uint16_t val)
{
    spi_transmit_burst(CMD_DIAL);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    spi_transmit_burst(((uint32_t) rad) + (((uint32_t) options) << 16U));
    spi_transmit_burst(val);
}

/**
 * @brief Установите цвет переднего плана.
 */
void EVE_cmd_fgcolor(uint32_t color)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_FGCOLOR);
        spi_transmit((uint8_t) (color));
        spi_transmit((uint8_t) (color >> 8U));
        spi_transmit((uint8_t) (color >> 16U));
        spi_transmit(0U);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_FGCOLOR);
        spi_transmit_burst(color);
    }
}

/**
 * @brief Установите цвет переднего плана, работает только в серийном режиме.
 */
void EVE_cmd_fgcolor_burst(uint32_t color)
{
    spi_transmit_burst(CMD_FGCOLOR);
    spi_transmit_burst(color);
}

/**
 * @brief Нарисуйте датчик.
 */
void EVE_cmd_gauge(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options,
                    uint16_t major, uint16_t minor, uint16_t val, uint16_t range)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_GAUGE);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit((uint8_t) (rad));
        spi_transmit((uint8_t) (rad >> 8U));
        spi_transmit((uint8_t) (options));
        spi_transmit((uint8_t) (options >> 8U));
        spi_transmit((uint8_t) (major));
        spi_transmit((uint8_t) (major >> 8U));
        spi_transmit((uint8_t) (minor));
        spi_transmit((uint8_t) (minor >> 8U));
        spi_transmit((uint8_t) (val));
        spi_transmit((uint8_t) (val >> 8U));
        spi_transmit((uint8_t) (range));
        spi_transmit((uint8_t) (range >> 8U));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_GAUGE);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
        spi_transmit_burst(((uint32_t) rad) + (((uint32_t) options) << 16U));
        spi_transmit_burst(((uint32_t) major) + (((uint32_t) minor) << 16U));
        spi_transmit_burst(((uint32_t) val) + (((uint32_t) range) << 16U));
    }
}

/**
 * @brief Нарисуйте датчик, работает только в пакетном режиме.
 */
void EVE_cmd_gauge_burst(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options,
                            uint16_t major, uint16_t minor, uint16_t val, uint16_t range)
{
    spi_transmit_burst(CMD_GAUGE);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    spi_transmit_burst(((uint32_t) rad) + (((uint32_t) options) << 16U));
    spi_transmit_burst(((uint32_t) major) + (((uint32_t) minor) << 16U));
    spi_transmit_burst(((uint32_t) val) + (((uint32_t) range) << 16U));
}

/**
 * @brief Извлекает текущую матрицу в контексте механизма сопроцессора.
 * @note - ждет завершения и после завершения считывает значения из RAM_CMD
 * @note - нельзя использовать с cmd-burst
 */
void EVE_cmd_getmatrix(int32_t *p_a, int32_t *p_b, int32_t *p_c,
                        int32_t *p_d, int32_t *p_e, int32_t *p_f)
{
    if (0U == cmd_burst)
    {
        uint16_t cmdoffset;
        uint32_t address;

        eve_begin_cmd(CMD_GETMATRIX);
        spi_transmit_32(0UL);
        spi_transmit_32(0UL);
        spi_transmit_32(0UL);
        spi_transmit_32(0UL);
        spi_transmit_32(0UL);
        spi_transmit_32(0UL);
        EVE_cs_clear();
        EVE_execute_cmd();
        cmdoffset = EVE_memRead16(REG_CMD_WRITE);

        if (p_f != NULL)
        {
            address = EVE_RAM_CMD + ((cmdoffset - 4UL) & 0xfffUL);
            *p_f = (int32_t) EVE_memRead32(address);
        }
        if (p_e != NULL)
        {
            address = EVE_RAM_CMD + ((cmdoffset - 8UL) & 0xfffUL);
            *p_e = (int32_t) EVE_memRead32(address);
        }
        if (p_d != NULL)
        {
            address = EVE_RAM_CMD + ((cmdoffset - 12UL) & 0xfffUL);
            *p_d = (int32_t) EVE_memRead32(address);
        }
        if (p_c != NULL)
        {
            address = EVE_RAM_CMD + ((cmdoffset - 16UL) & 0xfffUL);
            *p_c = (int32_t) EVE_memRead32(address);
        }
        if (p_b != NULL)
        {
            address = EVE_RAM_CMD + ((cmdoffset - 20UL) & 0xfffUL);
            *p_b = (int32_t) EVE_memRead32(address);
        }
        if (p_a != NULL)
        {
            address = EVE_RAM_CMD + ((cmdoffset - 24UL) & 0xfffUL);
            *p_a = (int32_t) EVE_memRead32(address);
        }
    }
}

/**
 * @brief Настройте цвет выделения, используемый в 3D-эффектах дляCMD_BUTTONи CMD_KEYS.
 */
void EVE_cmd_gradcolor(uint32_t color)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_GRADCOLOR);
        spi_transmit((uint8_t) (color));
        spi_transmit((uint8_t) (color >> 8U));
        spi_transmit((uint8_t) (color >> 16U));
        spi_transmit(0U);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_GRADCOLOR);
        spi_transmit_burst(color);
    }
}

/**
 * @brief Настройте цвет подсветки, используемый в 3D-эффектах дляCMD_BUTTONи CMD_KEYS, работает только в серийном режиме.
 */
void EVE_cmd_gradcolor_burst(uint32_t color)
{
    spi_transmit_burst(CMD_GRADCOLOR);
    spi_transmit_burst(color);
}

/**
 * @brief Нарисуйте плавный цветовой градиент.
 */
void EVE_cmd_gradient(int16_t xc0, int16_t yc0, uint32_t rgb0, int16_t xc1,
                        int16_t yc1, uint32_t rgb1)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_GRADIENT);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit((uint8_t) (rgb0));
        spi_transmit((uint8_t) (rgb0 >> 8U));
        spi_transmit((uint8_t) (rgb0 >> 16U));
        spi_transmit(0U);
        spi_transmit((uint8_t) ((uint16_t) xc1));
        spi_transmit((uint8_t) (((uint16_t) xc1) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc1));
        spi_transmit((uint8_t) (((uint16_t) yc1) >> 8U));
        spi_transmit((uint8_t) (rgb1));
        spi_transmit((uint8_t) (rgb1 >> 8U));
        spi_transmit((uint8_t) (rgb1 >> 16U));
        spi_transmit(0U);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_GRADIENT);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
        spi_transmit_burst(rgb0);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc1)) + (((uint32_t) ((uint16_t) yc1)) << 16U));
        spi_transmit_burst(rgb1);
    }
}

/**
 * @brief Нарисуйте плавный цветовой градиент, работает только в серийном режиме.
 */
void EVE_cmd_gradient_burst(int16_t xc0, int16_t yc0, uint32_t rgb0, int16_t xc1,
                            int16_t yc1, uint32_t rgb1)
{
    spi_transmit_burst(CMD_GRADIENT);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    spi_transmit_burst(rgb0);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc1)) + (((uint32_t) ((uint16_t) yc1)) << 16U));
    spi_transmit_burst(rgb1);
}

/**
 * @brief Нарисуйте ряд кнопок с надписями.
 * @note - Значение тега каждой кнопки устанавливается равным значениюASCIIее метки.
 * @note - Не работает с UTF-8.
 */
void EVE_cmd_keys(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt,
                    uint16_t font, uint16_t options, const char *p_text)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_KEYS);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit((uint8_t) (wid));
        spi_transmit((uint8_t) (wid >> 8U));
        spi_transmit((uint8_t) (hgt));
        spi_transmit((uint8_t) (hgt >> 8U));
        spi_transmit((uint8_t) (font));
        spi_transmit((uint8_t) (font >> 8U));
        spi_transmit((uint8_t) (options));
        spi_transmit((uint8_t) (options >> 8U));
        private_string_write(p_text);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_KEYS);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
        spi_transmit_burst(((uint32_t) wid) + (((uint32_t) hgt) << 16U));
        spi_transmit_burst(((uint32_t) font) + (((uint32_t) options) << 16U));
        private_string_write(p_text);
    }
}

/**
 * @brief Нарисуйте ряд кнопок с надписями, работает только в пакетном режиме.
 * @note - Значение тега каждой кнопки устанавливается равным значениюASCIIее метки.
 * @note - Не работает с UTF-8.
 */
void EVE_cmd_keys_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt,
                        uint16_t font, uint16_t options, const char *p_text)
{
    spi_transmit_burst(CMD_KEYS);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    spi_transmit_burst(((uint32_t) wid) + (((uint32_t) hgt) << 16U));
    spi_transmit_burst(((uint32_t) font) + (((uint32_t) options) << 16U));
    private_string_write(p_text);
}

/**
 * @brief Нарисуйте число.
 */
void EVE_cmd_number(int16_t xc0, int16_t yc0, uint16_t font,
                    uint16_t options, int32_t number)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_NUMBER);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit((uint8_t) (font));
        spi_transmit((uint8_t) (font >> 8U));
        spi_transmit((uint8_t) (options));
        spi_transmit((uint8_t) (options >> 8U));
        spi_transmit_32((uint32_t) number);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_NUMBER);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
        spi_transmit_burst(((uint32_t) font) + (((uint32_t) options) << 16U));
        spi_transmit_burst((uint32_t) number);
    }
}

/**
 * @brief Нарисуйте число, работает только в пакетном режиме.
 */
void EVE_cmd_number_burst(int16_t xc0, int16_t yc0, uint16_t font,
                            uint16_t options, int32_t number)
{
    spi_transmit_burst(CMD_NUMBER);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    spi_transmit_burst(((uint32_t) font) + (((uint32_t) options) << 16U));
    spi_transmit_burst((uint32_t) number);
}

/**
 * @brief Нарисуйте индикатор выполнения.
 */
void EVE_cmd_progress(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt,
                        uint16_t options, uint16_t val, uint16_t range)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_PROGRESS);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit((uint8_t) (wid));
        spi_transmit((uint8_t) (wid >> 8U));
        spi_transmit((uint8_t) (hgt));
        spi_transmit((uint8_t) (hgt >> 8U));
        spi_transmit((uint8_t) (options));
        spi_transmit((uint8_t) (options >> 8U));
        spi_transmit((uint8_t) (val));
        spi_transmit((uint8_t) (val >> 8U));
        spi_transmit((uint8_t) (range));
        spi_transmit((uint8_t) (range >> 8U));
        spi_transmit(0U); /* фиктивный байт для 4-байтового выравнивания */
        spi_transmit(0U); /* фиктивный байт для 4-байтового выравнивания */
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_PROGRESS);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
        spi_transmit_burst(((uint32_t) wid) + (((uint32_t) hgt) << 16U));
        spi_transmit_burst(((uint32_t) options) + (((uint32_t) val) << 16U));
        spi_transmit_burst((uint32_t) range);
    }
}

/**
 * @brief Нарисуйте индикатор выполнения, работает только в пакетном режиме.
 */
void EVE_cmd_progress_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt,
                            uint16_t options, uint16_t val, uint16_t range)
{
    spi_transmit_burst(CMD_PROGRESS);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    spi_transmit_burst(((uint32_t) wid) + (((uint32_t) hgt) << 16U));
    spi_transmit_burst(((uint32_t) options) + (((uint32_t) val) << 16U));
    spi_transmit_burst((uint32_t) range);
}

/**
 * @brief Загрузите шрифтROMв дескриптор растрового изображения.
 * @note - генерирует команды списка отображения, поэтому его необходимо поместить в список отображения
 */
void EVE_cmd_romfont(uint32_t font, uint32_t romslot)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_ROMFONT);
        spi_transmit_32(font);
        spi_transmit_32(romslot);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ROMFONT);
        spi_transmit_burst(font);
        spi_transmit_burst(romslot);
    }
}

/**
 * @brief Загрузите шрифтROMв дескриптор растрового изображения, работает только в пакетном режиме.
 * @note - генерирует команды списка отображения, поэтому его необходимо поместить в список отображения
 */
void EVE_cmd_romfont_burst(uint32_t font, uint32_t romslot)
{
    spi_transmit_burst(CMD_ROMFONT);
    spi_transmit_burst(font);
    spi_transmit_burst(romslot);
}

/**
 * @brief Примените поворот к текущей матрице.
 */
void EVE_cmd_rotate(uint32_t angle)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_ROTATE);
        spi_transmit_32(angle & 0xFFFFUL);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ROTATE);
        spi_transmit_burst(angle & 0xFFFFUL);
    }
}

/**
 * @brief Применить вращение к текущей матрице, работает только в пакетном режиме.
 */
void EVE_cmd_rotate_burst(uint32_t angle)
{
    spi_transmit_burst(CMD_ROTATE);
    spi_transmit_burst(angle & 0xFFFFUL);
}

/**
 * @brief Примените масштаб к текущей матрице.
 */
void EVE_cmd_scale(int32_t scx, int32_t scy)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SCALE);
        spi_transmit_32((uint32_t) scx);
        spi_transmit_32((uint32_t) scy);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SCALE);
        spi_transmit_burst((uint32_t) scx);
        spi_transmit_burst((uint32_t) scy);
    }
}

/**
 * @brief Применить масштаб к текущей матрице, работает только в пакетном режиме.
 */
void EVE_cmd_scale_burst(int32_t scx, int32_t scy)
{
    spi_transmit_burst(CMD_SCALE);
    spi_transmit_burst((uint32_t) scx);
    spi_transmit_burst((uint32_t) scy);
}

/**
 * @brief Нарисуйте полосу прокрутки.
 */
void EVE_cmd_scrollbar(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt,
            uint16_t options, uint16_t val, uint16_t size, uint16_t range)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SCROLLBAR);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit((uint8_t) (wid));
        spi_transmit((uint8_t) (wid >> 8U));
        spi_transmit((uint8_t) (hgt));
        spi_transmit((uint8_t) (hgt >> 8U));
        spi_transmit((uint8_t) (options));
        spi_transmit((uint8_t) (options >> 8U));
        spi_transmit((uint8_t) (val));
        spi_transmit((uint8_t) (val >> 8U));
        spi_transmit((uint8_t) (size));
        spi_transmit((uint8_t) (size >> 8U));
        spi_transmit((uint8_t) (range));
        spi_transmit((uint8_t) (range >> 8U));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SCROLLBAR);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
        spi_transmit_burst(((uint32_t) wid) + (((uint32_t) hgt) << 16U));
        spi_transmit_burst(((uint32_t) options) + (((uint32_t) val) << 16U));
        spi_transmit_burst(((uint32_t) size) + (((uint32_t) range) << 16U));
    }
}

/**
 * @brief Нарисуйте полосу прокрутки, работает только в пакетном режиме.
 */
void EVE_cmd_scrollbar_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt,
                uint16_t options, uint16_t val, uint16_t size, uint16_t range)
{
    spi_transmit_burst(CMD_SCROLLBAR);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    spi_transmit_burst(((uint32_t) wid) + (((uint32_t) hgt) << 16U));
    spi_transmit_burst(((uint32_t) options) + (((uint32_t) val) << 16U));
    spi_transmit_burst(((uint32_t) size) + (((uint32_t) range) << 16U));
}

/**
 * @brief Установите базу для вывода чисел.
 */
void EVE_cmd_setbase(uint32_t base)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SETBASE);
        spi_transmit_32(base);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SETBASE);
        spi_transmit_burst(base);
    }
}

/**
 * @brief Установите базу для вывода чисел, работает только в пакетном режиме.
 */
void EVE_cmd_setbase_burst(uint32_t base)
{
    spi_transmit_burst(CMD_SETBASE);
    spi_transmit_burst(base);
}

/**
 * @brief Сгенерируйте соответствующие команды списка отображения для заданной информации о растровом изображении.
 */
void EVE_cmd_setbitmap(uint32_t addr, uint16_t fmt, uint16_t width,
                        uint16_t height)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SETBITMAP);
        spi_transmit_32(addr);
        spi_transmit((uint8_t) (fmt));
        spi_transmit((uint8_t) (fmt >> 8U));
        spi_transmit((uint8_t) (width));
        spi_transmit((uint8_t) (width >> 8U));
        spi_transmit((uint8_t) (height));
        spi_transmit((uint8_t) (height >> 8U));
        spi_transmit(0U);
        spi_transmit(0U);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SETBITMAP);
        spi_transmit_burst(addr);
        spi_transmit_burst(((uint32_t) fmt) + (((uint32_t) width) << 16U));
        spi_transmit_burst((uint32_t) height);
    }
}

/**
 * @brief Сгенерируйте соответствующие команды списка отображения для заданной растровой информации. Работает только в пакетном режиме.
 */
void EVE_cmd_setbitmap_burst(uint32_t addr, uint16_t fmt, uint16_t width,
                                uint16_t height)
{
    spi_transmit_burst(CMD_SETBITMAP);
    spi_transmit_burst(addr);
    spi_transmit_burst(((uint32_t) fmt) + (((uint32_t) width) << 16U));
    spi_transmit_burst((uint32_t) height);
}

/**
 * @brief Зарегистрируйте один собственный шрифт в механизме сопроцессора.
 * @note - не настраиваются параметры растрового изображения шрифта
 */
void EVE_cmd_setfont(uint32_t font, uint32_t ptr)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SETFONT);
        spi_transmit_32(font);
        spi_transmit_32(ptr);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SETFONT);
        spi_transmit_burst(font);
        spi_transmit_burst(ptr);
    }
}

/**
 * @brief Зарегистрируйте один собственный шрифт в механизме сопроцессора, работает только в пакетном режиме.
 * @note - не настраиваются параметры растрового изображения шрифта
 */
void EVE_cmd_setfont_burst(uint32_t font, uint32_t ptr)
{
    spi_transmit_burst(CMD_SETFONT);
    spi_transmit_burst(font);
    spi_transmit_burst(ptr);
}

/**
 * @brief Настройте пользовательскую настройку для использования движком сопроцессора.
 * @note - генерирует команды списка отображения, поэтому его необходимо поместить в список отображения
 */
void EVE_cmd_setfont2(uint32_t font, uint32_t ptr, uint32_t firstchar)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SETFONT2);
        spi_transmit_32(font);
        spi_transmit_32(ptr);
        spi_transmit_32(firstchar);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SETFONT2);
        spi_transmit_burst(font);
        spi_transmit_burst(ptr);
        spi_transmit_burst(firstchar);
    }
}

/**
 * @brief Настроил кастом для использования движком сопроцессора, работает только в пакетном режиме.
 * @note - генерирует команды списка отображения, поэтому его необходимо поместить в список отображения
 */
void EVE_cmd_setfont2_burst(uint32_t font, uint32_t ptr, uint32_t firstchar)
{
    spi_transmit_burst(CMD_SETFONT2);
    spi_transmit_burst(font);
    spi_transmit_burst(ptr);
    spi_transmit_burst(firstchar);
}

/**
 * @brief Установите черновое растровое изображение для использования виджета.
 */
void EVE_cmd_setscratch(uint32_t handle)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SETSCRATCH);
        spi_transmit_32(handle);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SETSCRATCH);
        spi_transmit_burst(handle);
    }
}

/**
 * @brief Установите чистое растровое изображение для использования виджетами, работает только в пакетном режиме.
 */
void EVE_cmd_setscratch_burst(uint32_t handle)
{
    spi_transmit_burst(CMD_SETSCRATCH);
    spi_transmit_burst(handle);
}

/**
 * @brief Запустите постоянное обновление эскиза.
 */
void EVE_cmd_sketch(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt,
                    uint32_t ptr, uint16_t format)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SKETCH);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) wid));
        spi_transmit((uint8_t) (((uint16_t) wid) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) hgt));
        spi_transmit((uint8_t) (((uint16_t) hgt) >> 8U));
        spi_transmit_32(ptr);
        spi_transmit((uint8_t) (format));
        spi_transmit((uint8_t) (format >> 8U));
        spi_transmit(0U);
        spi_transmit(0U);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SKETCH);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
        spi_transmit_burst(((uint32_t) ((uint16_t) wid)) + (((uint32_t) ((uint16_t) hgt)) << 16U));
        spi_transmit_burst(ptr);
        spi_transmit_burst((uint32_t) format);
    }
}

/**
 * @brief Запуск непрерывного обновления эскиза, работает только в пакетном режиме.
 */
void EVE_cmd_sketch_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt,
                            uint32_t ptr, uint16_t format)
{
    spi_transmit_burst(CMD_SKETCH);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    spi_transmit_burst(((uint32_t) ((uint16_t) wid)) + (((uint32_t) ((uint16_t) hgt)) << 16U));
    spi_transmit_burst(ptr);
    spi_transmit_burst((uint32_t) format);
}

/**
 * @brief Нарисуйте ползунок.
 */
void EVE_cmd_slider(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt,
                    uint16_t options, uint16_t val, uint16_t range)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SLIDER);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit((uint8_t) (wid));
        spi_transmit((uint8_t) (wid >> 8U));
        spi_transmit((uint8_t) (hgt));
        spi_transmit((uint8_t) (hgt >> 8U));
        spi_transmit((uint8_t) (options));
        spi_transmit((uint8_t) (options >> 8U));
        spi_transmit((uint8_t) (val));
        spi_transmit((uint8_t) (val >> 8U));
        spi_transmit((uint8_t) (range));
        spi_transmit((uint8_t) (range >> 8U));
        spi_transmit(0U); /* фиктивный байт для 4-байтового выравнивания */
        spi_transmit(0U); /* фиктивный байт для 4-байтового выравнивания */
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SLIDER);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
        spi_transmit_burst(((uint32_t) wid) + (((uint32_t) hgt) << 16U));
        spi_transmit_burst(((uint32_t) options) + (((uint32_t) val) << 16U));
        spi_transmit_burst((uint32_t) range);
    }
}

/**
 * @brief Нарисуйте ползунок, работает только в пакетном режиме.
 */
void EVE_cmd_slider_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt,
                            uint16_t options, uint16_t val, uint16_t range)
{
    spi_transmit_burst(CMD_SLIDER);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    spi_transmit_burst(((uint32_t) wid) + (((uint32_t) hgt) << 16U));
    spi_transmit_burst(((uint32_t) options) + (((uint32_t) val) << 16U));
    spi_transmit_burst((uint32_t) range);
}

/**
 * @brief Запустите анимированный спиннер.
 */
void EVE_cmd_spinner(int16_t xc0, int16_t yc0, uint16_t style, uint16_t scale)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SPINNER);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit((uint8_t) (style));
        spi_transmit((uint8_t) (style >> 8U));
        spi_transmit((uint8_t) (scale));
        spi_transmit((uint8_t) (scale >> 8U));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SPINNER);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
        spi_transmit_burst(((uint32_t) style) + (((uint32_t) scale) << 16U));
    }
}

/**
 * @brief Запустите анимированный счетчик, работает только в пакетном режиме.
 */
void EVE_cmd_spinner_burst(int16_t xc0, int16_t yc0, uint16_t style,
                            uint16_t scale)
{
    spi_transmit_burst(CMD_SPINNER);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    spi_transmit_burst(((uint32_t) style) + (((uint32_t) scale) << 16U));
}

/**
 * @brief Нарисуйте текстовую строку.
 */
void EVE_cmd_text(int16_t xc0, int16_t yc0, uint16_t font, uint16_t options,
                    const char *p_text)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_TEXT);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit((uint8_t) (font));
        spi_transmit((uint8_t) (font >> 8U));
        spi_transmit((uint8_t) (options));
        spi_transmit((uint8_t) (options >> 8U));
        private_string_write(p_text);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_TEXT);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
        spi_transmit_burst(((uint32_t) font) + (((uint32_t) options) << 16U));
        private_string_write(p_text);
    }
}

/**
 * @brief Нарисуйте текстовую строку, работает только в пакетном режиме.
 */
void EVE_cmd_text_burst(int16_t xc0, int16_t yc0, uint16_t font,
                        uint16_t options, const char *p_text)
{
    spi_transmit_burst(CMD_TEXT);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    spi_transmit_burst(((uint32_t) font) + (((uint32_t) options) << 16U));
    private_string_write(p_text);
}

/**
 * @brief Нарисуйте тумблер с метками.
 */
void EVE_cmd_toggle(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t font,
                    uint16_t options, uint16_t state, const char *p_text)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_TOGGLE);
        spi_transmit((uint8_t) ((uint16_t) xc0));
        spi_transmit((uint8_t) (((uint16_t) xc0) >> 8U));
        spi_transmit((uint8_t) ((uint16_t) yc0));
        spi_transmit((uint8_t) (((uint16_t) yc0) >> 8U));
        spi_transmit((uint8_t) (wid));
        spi_transmit((uint8_t) (wid >> 8U));
        spi_transmit((uint8_t) (font));
        spi_transmit((uint8_t) (font >> 8U));
        spi_transmit((uint8_t) (options));
        spi_transmit((uint8_t) (options >> 8U));
        spi_transmit((uint8_t) (state));
        spi_transmit((uint8_t) (state >> 8U));
        private_string_write(p_text);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_TOGGLE);
        spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
        spi_transmit_burst(((uint32_t) wid) + (((uint32_t) font) << 16U));
        spi_transmit_burst(((uint32_t) options) + (((uint32_t) state) << 16U));
        private_string_write(p_text);
    }
}

/**
 * @brief Нарисуйте тумблер с метками, работает только в пакетном режиме.
 */
void EVE_cmd_toggle_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t font,
                            uint16_t options, uint16_t state, const char *p_text)
{
    spi_transmit_burst(CMD_TOGGLE);
    spi_transmit_burst(((uint32_t) ((uint16_t) xc0)) + (((uint32_t) ((uint16_t) yc0)) << 16U));
    spi_transmit_burst(((uint32_t) wid) + (((uint32_t) font) << 16U));
    spi_transmit_burst(((uint32_t) options) + (((uint32_t) state) << 16U));
    private_string_write(p_text);
}

/**
 * @brief Примените перевод к текущей матрице.
 */
void EVE_cmd_translate(int32_t tr_x, int32_t tr_y)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_TRANSLATE);
        spi_transmit_32((uint32_t) tr_x);
        spi_transmit_32((uint32_t) tr_y);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_TRANSLATE);
        spi_transmit_burst((uint32_t) tr_x);
        spi_transmit_burst((uint32_t) tr_y);
    }
}

/**
 * @brief Применить преобразование к текущей матрице, работает только в пакетном режиме.
 */
void EVE_cmd_translate_burst(int32_t tr_x, int32_t tr_y)
{
    spi_transmit_burst(CMD_TRANSLATE);
    spi_transmit_burst((uint32_t) tr_x);
    spi_transmit_burst((uint32_t) tr_y);
}

/**
 * @brief Установите текущий цвет: красный, зеленый и синий.
 */
void EVE_color_rgb(uint32_t color)
{
    EVE_cmd_dl(DL_COLOR_RGB | (color & 0x00ffffffUL));
}

/**
 * @brief Установите текущий цвет: красный, зеленый и синий, работает только в пакетном режиме.
 */
void EVE_color_rgb_burst(uint32_t color)
{
    spi_transmit_burst(DL_COLOR_RGB | (color & 0x00ffffffUL));
}

/**
 * @brief Установите текущий цвет альфа, зеленый и синий.
 */
void EVE_color_a(uint8_t alpha)
{
    EVE_cmd_dl(DL_COLOR_A | ((uint32_t) alpha));
}

/**
 * @brief Установите текущий цвет альфа, зеленый и синий, работает только в серийном режиме.
 */
void EVE_color_a_burst(uint8_t alpha)
{
    spi_transmit_burst(DL_COLOR_A | ((uint32_t) alpha));
}


/* ##################################################################
    функции специального назначения
##################################################################### */

/* Это должно называться построением вне списка отображения. */
/* Эта функция отображает интерактивный экран калибровки, рассчитывает значения калибровки. */
/* и записывает новые значения в регистры сенсорной матрицы EVE.*/
/* В отличие от встроенного cmd_calibrate() вEVEон работает и с дисплеями, вырезанными из более крупных, например
 * EVE2-38A / EVE2-38G. */
/* Размеры необходимы в качестве параметра, так как EVE_VSIZE для EVE2 -38 равен 272, но видимый размер составляет только 116. */
/* Таким образом, вызов будетEVE_calibrate_manual(EVE_HSIZE, 116); дляEVE2-38A иEVE2-38G, а также для большинства других
 * дисплеи */
/* использованиеEVE_calibrate_manual(EVE_VSIZE,EVE_VSIZE) будет работать, но для обычного встроенного дисплея cmd_calibrate
 * в любом случае будет работать так, как ожидалось */
/* Этот код был взят из библиотеки MatrixOrbitalEVE2на Github, адаптирован и модифицирован. */
void EVE_calibrate_manual(uint16_t width, uint16_t height)
{
    int32_t display_x[3U];
    int32_t display_y[3U];
    int32_t touch_x[3U];
    int32_t touch_y[3U];
    uint32_t touch_value;
    int32_t tmp;
    int32_t divi;
    int32_t trans_matrix[6U];
    uint8_t count = 0U;
    uint8_t calc = 0U;
    uint32_t calc32 = 0U;
    char num[4U];
    uint8_t touch_lock = 1U;

    /* эти значения определяют, где на вашем дисплее будут отображаться точки калибровки. */
    display_x[0U] = (int32_t) width / 6;
    display_y[0U] = (int32_t) height / 6;

    display_x[1U] = (int32_t) width - ((int32_t) width / 8);
    display_y[1U] = (int32_t) height / 2;

    display_x[2U] = (int32_t) width / 2;
    display_y[2U] = (int32_t) height - ((int32_t) height / 8);

    while (count < 3U)
    {
        EVE_cmd_dl(CMD_DLSTART);
        EVE_cmd_dl(DL_CLEAR_COLOR_RGB);
        EVE_cmd_dl(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
        EVE_cmd_dl(DL_VERTEX_FORMAT); /* установлено значение 0 — уменьшите точность для VERTEX2F до 1 пикселя вместо 1/16 пикселя по умолчанию. */

        /* нарисовать точку калибровки на экране */
        EVE_cmd_dl(DL_COLOR_RGB | 0x0000ffUL);
        EVE_cmd_dl(POINT_SIZE(15U * 16U));
        EVE_cmd_dl((DL_BEGIN | EVE_POINTS));

        int16_t xc0;
        int16_t yc0;

        xc0 = (int16_t) display_x[count];
        yc0 = (int16_t) display_y[count];
        EVE_cmd_dl(VERTEX2F(xc0, yc0));
        EVE_cmd_dl(DL_END);
        EVE_cmd_dl(DL_COLOR_RGB | 0xffffffUL);
        EVE_cmd_text((int16_t) width / 2, 20, 26U, EVE_OPT_CENTER, "tap on the dot");
        calc = count + 0x31U;
        num[0U] = (char) calc;
        num[1U] = (char) 0U; /* строка из одного символа, завершающаяся нулем */
        EVE_cmd_text((int16_t) display_x[count], (int16_t) display_y[count], 27U, EVE_OPT_CENTER, num);

        EVE_cmd_dl(DL_DISPLAY);
        EVE_cmd_dl(CMD_SWAP);
        EVE_execute_cmd();

        for (;;)
        {
            touch_value = EVE_memRead32(REG_TOUCH_DIRECT_XY); /* читайте, нет ли новых вводов сенсорных тегов */

            if (touch_lock != 0U)
            {
                if ((touch_value & 0x80000000UL) != 0UL) /* проверь, нет ли у нас касания */
                {
                    touch_lock = 0U;
                }
            }
            else
            {
                if (0UL == (touch_value & 0x80000000UL)) /* проверьте, обнаружено ли прикосновение */
                {
                    calc32 = ((touch_value >> 16U) & 0x03FFUL);
                    touch_x[count] = (int32_t) calc32; /* необработанная координата X сенсорного экрана */
                    calc32 = touch_value & 0x03FFUL;
                    touch_y[count] = (int32_t) calc32; /* необработанная координата Y сенсорного экрана */
                    touch_lock = 1U;
                    count++;
                    break; /* оставить на (;;) */
                }
            }
        }
    }

    divi = ((touch_x[0U] - touch_x[2U]) * (touch_y[1U] - touch_y[2U])) - ((touch_x[1U] - touch_x[2U]) * (touch_y[0U] - touch_y[2U]));

    tmp = (((display_x[0U] - display_x[2U]) * (touch_y[1U] - touch_y[2U])) -
           ((display_x[1U] - display_x[2U]) * (touch_y[0U] - touch_y[2U])));
    trans_matrix[0U] = (int32_t) (((int64_t) tmp * 65536) / divi);

    tmp = (((touch_x[0U] - touch_x[2U]) * (display_x[1U] - display_x[2U])) -
           ((display_x[0U] - display_x[2U]) * (touch_x[1U] - touch_x[2U])));
    trans_matrix[1U] = (int32_t) (((int64_t) tmp * 65536) / divi);

    tmp = ((touch_y[0U] * (((touch_x[2U] * display_x[1U]) - (touch_x[1U] * display_x[2U])))) +
           (touch_y[1U] * (((touch_x[0U] * display_x[2U]) - (touch_x[2U] * display_x[0U])))) +
           (touch_y[2U] * (((touch_x[1U] * display_x[0U]) - (touch_x[0U] * display_x[1U])))));
    trans_matrix[2U] = (int32_t) (((int64_t) tmp * 65536) / divi);

    tmp = (((display_y[0U] - display_y[2U]) * (touch_y[1U] - touch_y[2U])) -
           ((display_y[1U] - display_y[2U]) * (touch_y[0U] - touch_y[2U])));
    trans_matrix[3U] = (int32_t) (((int64_t) tmp * 65536) / divi);

    tmp = (((touch_x[0U] - touch_x[2U]) * (display_y[1U] - display_y[2U])) -
           ((display_y[0U] - display_y[2U]) * (touch_x[1U] - touch_x[2U])));
    trans_matrix[4U] = (int32_t) (((int64_t) tmp * 65536) / divi);

    tmp = ((touch_y[0U] * (((touch_x[2U] * display_y[1U]) - (touch_x[1U] * display_y[2U])))) +
           (touch_y[1U] * (((touch_x[0U] * display_y[2U]) - (touch_x[2U] * display_y[0U])))) +
           (touch_y[2U] * (((touch_x[1U] * display_y[0U]) - (touch_x[0U] * display_y[1U])))));
    trans_matrix[5U] = (int32_t) (((int64_t) tmp * 65536) / divi);

    EVE_memWrite32(REG_TOUCH_TRANSFORM_A, (uint32_t) trans_matrix[0U]);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_B, (uint32_t) trans_matrix[1U]);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_C, (uint32_t) trans_matrix[2U]);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_D, (uint32_t) trans_matrix[3U]);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_E, (uint32_t) trans_matrix[4U]);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_F, (uint32_t) trans_matrix[5U]);
}

#endif /*LV_USE_DRAW_EVE*/
