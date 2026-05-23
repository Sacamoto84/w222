#pragma once

#include <stddef.h>
#include <stdint.h>

#include "audio_player.h"
#include "driver/i2s_std.h"
#include "lite_app.h"
#include "lvgl.h"

class MusicPlayer : public LiteApp {
public:
    MusicPlayer();
    ~MusicPlayer() override;

    const char *title(void) const override { return "Music"; }                            //Название приложения
    const char *icon_text(void) const override { return LV_SYMBOL_AUDIO; }                //Иконка приложения
    lv_color_t accent_color(void) const override { return lv_color_hex(0xE05A47); }     //Цвет акцента
    bool use_launcher_header(void) const override { return false; }                       //Использовать заголовок лаунчера

    bool init(void) override;
    bool open(lv_obj_t *parent) override;
    void close(void) override;

private:
    static constexpr size_t kMaxEntries = 256;        //Максимальное количество треков
    static constexpr size_t kNameMax = 128;           //Максимальная длина названия
    static constexpr size_t kPathMax = 224;           //Максимальная длина пути
    static constexpr size_t kScopeCanvasWidth = 176;  //Ширина холста осциллографа
    static constexpr size_t kScopeCanvasHeight = 144; //Высота холста осциллографа
    static constexpr size_t kScopePointCount = 128;   //Количество точек на осциллографе

    struct TrackEntry {
        char name[kNameMax];
        char path[kPathMax];
        uint32_t size;
        uint32_t duration_ms;
    };

    struct EntryEventData {
        MusicPlayer *app;
        uint32_t index;
    };

    enum class Control {
        Refresh,
        Previous,
        PlayPause,
        Stop,
        Next,
        Volume,
        Seek,
        Close,
    };

    //Структура данных для события управления
    struct ControlEventData {
        MusicPlayer *app;
        Control control;
    };

    TrackEntry *entries_ = nullptr;
    EntryEventData *entry_events_ = nullptr;
    uint32_t entry_count_ = 0;

    lv_obj_t *root_ = nullptr;                  //Корневой объект
    lv_obj_t *list_ = nullptr;                  //Список треков
    lv_obj_t *status_label_ = nullptr;          //Статус
    lv_obj_t *track_label_ = nullptr;           //Название трека
    lv_obj_t *file_label_ = nullptr;            //Файл
    lv_obj_t *state_label_ = nullptr;           //Состояние
    lv_obj_t *play_label_ = nullptr;            //Иконка воспроизведения
    lv_obj_t *volume_slider_ = nullptr;         //Слайдер громкости
    lv_obj_t *volume_label_ = nullptr;          //Метка громкости
    lv_obj_t *seek_slider_ = nullptr;           //Слайдер позиции
    lv_obj_t *seek_label_ = nullptr;            //Метка позиции
    lv_obj_t *scope_canvas_ = nullptr;          //Холст осциллографа
    lv_timer_t *ui_timer_ = nullptr;            //Таймер UI

    void *event_queue_ = nullptr;         //Очередь событий
    void *scope_lock_ = nullptr;          //Мьютекс для доступа к буферу осциллографа
    i2s_chan_handle_t tx_chan_ = nullptr; //Дескриптор канала I2S
    bool i2s_enabled_ = false;            //Флаг, что I2S включен
    bool player_ready_ = false;           //Флаг, что плеер инициализирован
    bool muted_ = true;                   //Флаг, что звук выключен
    bool playback_started_ = false;       //Флаг, что воспроизведение началось
    bool stop_requested_ = false;         //Флаг, что пользователь нажал Stop
    bool seek_dragging_ = false;          //Флаг, что пользователь тянет ползунок
    uint8_t volume_percent_ = 30;         //Громкость по умолчанию 30%
    int current_index_ = -1;              //Индекс текущего трека
    volatile uint32_t playback_byte_offset_ = 0; //Смещение в байтах для воспроизведения
    uint32_t current_sample_rate_ = 44100;       //Частота дискретизации
    uint32_t current_bits_per_sample_ = 16;      //Бит на сэмпл
    i2s_slot_mode_t current_channel_mode_ = I2S_SLOT_MODE_STEREO; //Режим каналов
    int16_t *volume_buffer_ = nullptr;           //Буфер для изменения громкости
    size_t volume_buffer_bytes_ = 0;             //Размер буфера для изменения громкости
    uint16_t *scope_canvas_buffer_ = nullptr;    //Буфер для отображения осциллографа
    static constexpr size_t kScopeRingSize = 4096; //Размер кольцевого буфера осциллографа
    static constexpr uint8_t kScopeScaleCount = 6; //Количество масштабов осциллографа

    int16_t scope_ring_left_[kScopeRingSize] = {};      //Кольцевой буфер для левого канала
    int16_t scope_ring_right_[kScopeRingSize] = {};     //Кольцевой буфер для правого канала
    int16_t scope_render_left_[kScopePointCount] = {};  //Буфер для отображения левого канала
    int16_t scope_render_right_[kScopePointCount] = {}; //Буфер для отображения правого канала
    uint32_t scope_ring_write_idx_ = 0;                 //Индекс записи в кольцевом буфере
    uint32_t scope_sequence_ = 0;                       //Последовательность отрисовки
    uint32_t scope_rendered_sequence_ = UINT32_MAX;     //Последовательность отрисованного
    uint8_t scope_scale_idx_ = 1;                       //Индекс масштаба
    lv_obj_t *scope_label_ = nullptr;                   //Метка для отображения статуса
    char runtime_status_[96] = {};                      //Строка для отображения статуса

    ControlEventData refresh_event_ = {};               //Событие обновления
    ControlEventData previous_event_ = {};              //Событие предыдущего трека
    ControlEventData play_event_ = {};                  //Событие воспроизведения
    ControlEventData stop_event_ = {};                  //Событие остановки
    ControlEventData next_event_ = {};                  //Событие следующего трека
    ControlEventData volume_event_ = {};                //Событие изменения громкости
    ControlEventData seek_event_ = {};                  //Событие изменения позиции
    ControlEventData close_event_ = {};                 //Событие закрытия

    bool scan_tracks(void);                             //Сканирование треков
    void rebuild_list(void);                            //Перестроение списка
    bool play_index(uint32_t index);                    //Воспроизведение трека по индексу
    bool play_index_from_offset(uint32_t index, uint32_t offset); //Воспроизведение трека с offset
    void play_previous(void);                           //Воспроизведение предыдущего трека
    void play_next(void);                               //Воспроизведение следующего трека
    void toggle_play_pause(void);                       //Переключение воспроизведения/паузы
    void stop_playback(void);                           //Остановка воспроизведения
    void seek_to_slider_value(int32_t value);           //Переход к позиции по слайдеру
    void update_seek_label(uint32_t slider_value);      //Обновление метки позиции
    void handle_control(Control control, lv_event_t *event); //Обработка управления
    void handle_player_event(int event);                //Обработка события плеера
    void refresh_ui(void);                              //Обновление интерфейса
    void set_status(const char *fmt, ...);              //Установка статуса

    bool ensure_audio_player(void);                     //Инициализация аудио плеера
    void teardown_audio_player(void);                   //Очистка аудио плеера
    esp_err_t init_i2s(uint32_t sample_rate_hz, uint32_t bits_per_sample, i2s_slot_mode_t channel_mode); //Инициализация I2S
    void deinit_i2s(void);                              //Очистка I2S
    esp_err_t reconfigure_i2s(uint32_t sample_rate_hz, uint32_t bits_per_sample, i2s_slot_mode_t channel_mode); //Перенастройка I2S
    esp_err_t write_i2s(void *audio_buffer, size_t len, size_t *bytes_written, uint32_t timeout_ms); //Запись в I2S
    esp_err_t set_mute(bool muted);                     //Установка режима тишины
    bool ensure_volume_buffer(size_t len);              //Инициализация буфера громкости
    bool ensure_scope_canvas_buffer(void);              //Инициализация буфера осциллографа
    void clear_scope_samples(void);                     //Очистка образцов осциллографа
    void publish_scope_samples(const int16_t *samples, size_t sample_count, size_t channel_count); //Публикация образцов осциллографа
    void render_scope(void);                            //Отрисовка осциллографа

    static void entry_event_cb(lv_event_t *event);      //Обработка события входа
    static void control_event_cb(lv_event_t *event);    //Обработка события управления
    static void scope_event_cb(lv_event_t *event);      //Обработка события осциллографа
    static void ui_timer_cb(lv_timer_t *timer);         //Обработка таймера UI
    static void audio_player_event_cb(audio_player_cb_ctx_t *ctx); //Обработка события аудио плеера
    static int mp3_stream_read_cb(void *user_ctx, uint8_t *buffer, size_t len, bool *is_eof); //Чтение MP3 потока
    static void mp3_stream_close_cb(void *user_ctx); //Закрытие MP3 потока
    static esp_err_t audio_mute_cb(AUDIO_PLAYER_MUTE_SETTING setting); //Установка режима тишины
    static esp_err_t audio_clock_cb(uint32_t rate, uint32_t bits_cfg, i2s_slot_mode_t channel_mode); //Настройка тактовой частоты
    static esp_err_t audio_write_cb(void *audio_buffer, size_t len, size_t *bytes_written, uint32_t timeout_ms); //Запись аудио данных

    static MusicPlayer *active_player_;
};
