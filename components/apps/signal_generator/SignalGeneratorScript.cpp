#include "signal_generator/SignalGenerator.hpp"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

namespace {

static constexpr const char *kScriptDir = CONFIG_BSP_SD_MOUNT_POINT "/Script";
static constexpr const char *kPresetDir = CONFIG_BSP_SD_MOUNT_POINT "/Presets";
static constexpr uint32_t kScriptMaxStepsPerTick = 8;
static constexpr float kAmFreqMinHz = 0.1f;
static constexpr float kAmFreqMaxHz = 500.0f;

static char ascii_lower(char value)
{
    if ((value >= 'A') && (value <= 'Z')) {
        return static_cast<char>(value + ('a' - 'A'));
    }

    return value;
}

static int str_case_cmp(const char *left, const char *right)
{
    while ((*left != '\0') || (*right != '\0')) {
        const char left_ch = ascii_lower(*left);
        const char right_ch = ascii_lower(*right);
        if (left_ch != right_ch) {
            return static_cast<int>(static_cast<unsigned char>(left_ch)) -
                   static_cast<int>(static_cast<unsigned char>(right_ch));
        }
        if (left_ch == '\0') {
            return 0;
        }

        left++;
        right++;
    }

    return 0;
}

static bool str_ends_with_ignore_case(const char *text, const char *suffix)
{
    const size_t text_len = strlen(text);
    const size_t suffix_len = strlen(suffix);
    if (text_len < suffix_len) {
        return false;
    }

    text += text_len - suffix_len;
    for (size_t i = 0; i < suffix_len; i++) {
        if (ascii_lower(text[i]) != ascii_lower(suffix[i])) {
            return false;
        }
    }

    return true;
}

static const char *skip_spaces(const char *text)
{
    while ((*text == ' ') || (*text == '\t')) {
        text++;
    }

    return text;
}

static void trim_eol(char *text)
{
    if (text == nullptr) {
        return;
    }

    size_t len = strlen(text);
    while ((len > 0) && ((text[len - 1] == '\n') || (text[len - 1] == '\r') ||
                         (text[len - 1] == ' ') || (text[len - 1] == '\t'))) {
        text[len - 1] = '\0';
        len--;
    }
}

static size_t tokenize_line(char *line, char *tokens[], size_t max_tokens)
{
    if ((line == nullptr) || (tokens == nullptr) || (max_tokens == 0)) {
        return 0;
    }

    size_t count = 0;
    char *save = nullptr;
    char *token = strtok_r(line, " \t\r\n", &save);
    while ((token != nullptr) && (count < max_tokens)) {
        tokens[count++] = token;
        token = strtok_r(nullptr, " \t\r\n", &save);
    }

    return count;
}

static bool token_equals(const char *left, const char *right)
{
    return (left != nullptr) && (right != nullptr) && (str_case_cmp(left, right) == 0);
}

static bool token_is_on(const char *token)
{
    return token_equals(token, "ON") || token_equals(token, "1") || token_equals(token, "TRUE");
}

static float clamp_float(float value, float min_value, float max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }

    return value;
}

static uint32_t clamp_u32(uint32_t value, uint32_t min_value, uint32_t max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }

    return value;
}

static size_t clamp_wave_index(int index, size_t count)
{
    if (count == 0) {
        return 0;
    }
    if (index < 0) {
        return 0;
    }
    if (static_cast<size_t>(index) >= count) {
        return count - 1;
    }

    return static_cast<size_t>(index);
}

} // namespace

void SignalGenerator::script_load_default(void)
{
    script_stop(true);
    snprintf(script_name_, sizeof(script_name_), "main");
    script_line_count_ = 5;
    snprintf(script_lines_[0].text, sizeof(script_lines_[0].text), "LOAD F1 1000");
    snprintf(script_lines_[1].text, sizeof(script_lines_[1].text), "CR1 FR F1");
    snprintf(script_lines_[2].text, sizeof(script_lines_[2].text), "CH1 CR ON");
    snprintf(script_lines_[3].text, sizeof(script_lines_[3].text), "DELAY 1000");
    snprintf(script_lines_[4].text, sizeof(script_lines_[4].text), "END");
    script_selected_line_ = 0;
    script_scroll_offset_ = 0;
    script_dirty_ = false;
}

bool SignalGenerator::script_load_first_file(void)
{
    mkdir(kScriptDir, 0775);

    char path[128] = {};
    snprintf(path, sizeof(path), "%s/main.sk", kScriptDir);
    FILE *file = fopen(path, "r");
    if (file != nullptr) {
        fclose(file);
        return script_load_file("main");
    }

    DIR *dir = opendir(kScriptDir);
    if (dir == nullptr) {
        return false;
    }

    char selected[kScriptNameMax] = {};
    struct dirent *entry = nullptr;
    while ((entry = readdir(dir)) != nullptr) {
        if (!str_ends_with_ignore_case(entry->d_name, ".sk")) {
            continue;
        }

        snprintf(selected, sizeof(selected), "%s", entry->d_name);
        selected[strlen(selected) - 3] = '\0';
        break;
    }
    closedir(dir);

    if (selected[0] == '\0') {
        return false;
    }

    return script_load_file(selected);
}

bool SignalGenerator::script_load_file(const char *name)
{
    if ((name == nullptr) || (name[0] == '\0')) {
        return false;
    }

    char clean_name[kScriptNameMax] = {};
    snprintf(clean_name, sizeof(clean_name), "%s", name);
    if (str_ends_with_ignore_case(clean_name, ".sk")) {
        clean_name[strlen(clean_name) - 3] = '\0';
    }

    char path[128] = {};
    snprintf(path, sizeof(path), "%s/%s.sk", kScriptDir, clean_name);
    FILE *file = fopen(path, "r");
    if (file == nullptr) {
        return false;
    }

    script_stop(true);
    script_line_count_ = 0;

    char line[160] = {};
    while ((fgets(line, sizeof(line), file) != nullptr) && (script_line_count_ < kScriptMaxLines)) {
        trim_eol(line);
        const char *command = line;
        char *colon = strchr(line, ':');
        if (colon != nullptr) {
            command = colon + 1;
        }
        command = skip_spaces(command);
        snprintf(script_lines_[script_line_count_].text,
                 sizeof(script_lines_[script_line_count_].text),
                 "%s",
                 command[0] == '\0' ? "?" : command);
        script_line_count_++;
    }
    fclose(file);

    if (script_line_count_ == 0) {
        script_load_default();
        return false;
    }

    snprintf(script_name_, sizeof(script_name_), "%s", clean_name);
    script_selected_line_ = 0;
    script_scroll_offset_ = 0;
    script_dirty_ = false;
    set_runtime_status("Script loaded: %s.sk", script_name_);
    return true;
}

bool SignalGenerator::script_save_file(const char *name)
{
    if ((name == nullptr) || (name[0] == '\0')) {
        return false;
    }

    mkdir(kScriptDir, 0775);

    char clean_name[kScriptNameMax] = {};
    snprintf(clean_name, sizeof(clean_name), "%s", name);
    if (str_ends_with_ignore_case(clean_name, ".sk")) {
        clean_name[strlen(clean_name) - 3] = '\0';
    }

    char path[128] = {};
    snprintf(path, sizeof(path), "%s/%s.sk", kScriptDir, clean_name);
    FILE *file = fopen(path, "w");
    if (file == nullptr) {
        set_runtime_status("Script save failed: %s", path);
        return false;
    }

    for (size_t i = 0; i < script_line_count_; i++) {
        fprintf(file, "%u:%s\n", static_cast<unsigned>(i), script_lines_[i].text);
    }
    fclose(file);

    snprintf(script_name_, sizeof(script_name_), "%s", clean_name);
    script_dirty_ = false;
    return true;
}

bool SignalGenerator::script_save_as_next_file(void)
{
    mkdir(kScriptDir, 0775);

    for (unsigned i = 1; i < 1000; i++) {
        char name[kScriptNameMax] = {};
        snprintf(name, sizeof(name), "script%03u", i);

        char path[128] = {};
        snprintf(path, sizeof(path), "%s/%s.sk", kScriptDir, name);
        FILE *file = fopen(path, "r");
        if (file != nullptr) {
            fclose(file);
            continue;
        }

        return script_save_file(name);
    }

    return false;
}

void SignalGenerator::script_new(void)
{
    script_stop(true);
    snprintf(script_name_, sizeof(script_name_), "New");
    script_line_count_ = 2;
    snprintf(script_lines_[0].text, sizeof(script_lines_[0].text), "?");
    snprintf(script_lines_[1].text, sizeof(script_lines_[1].text), "END");
    script_selected_line_ = 0;
    script_scroll_offset_ = 0;
    script_dirty_ = true;
}

void SignalGenerator::script_start(void)
{
    if (script_line_count_ == 0) {
        script_load_default();
    }

    memset(script_registers_, 0, sizeof(script_registers_));
    script_pc_ = 0;
    script_wait_until_ = 0;
    script_running_ = true;

    if (!audio_running_) {
        if (!start_audio()) {
            script_running_ = false;
            set_runtime_status("Script start failed: audio not started");
            return;
        }
    }

    set_runtime_status("Script running: %s.sk", script_name_);
}

void SignalGenerator::script_stop(bool reset_pc)
{
    script_running_ = false;
    script_wait_until_ = 0;
    if (reset_pc) {
        script_pc_ = 0;
        memset(script_registers_, 0, sizeof(script_registers_));
    }
    script_keyboard_home();
}

void SignalGenerator::script_tick(void)
{
    if (!script_running_) {
        return;
    }

    const TickType_t now = xTaskGetTickCount();
    if ((script_wait_until_ != 0) && (static_cast<int32_t>(now - script_wait_until_) < 0)) {
        return;
    }
    script_wait_until_ = 0;

    bool state_changed = false;
    for (uint32_t step = 0; step < kScriptMaxStepsPerTick; step++) {
        state_changed = script_execute_current_line() || state_changed;
        if (!script_running_ || (script_wait_until_ != 0)) {
            break;
        }
    }

    if (state_changed) {
        refresh_ui();
    } else {
        refresh_script_ui();
    }
}

bool SignalGenerator::script_execute_current_line(void)
{
    if ((script_line_count_ == 0) || (script_pc_ >= script_line_count_)) {
        script_stop(true);
        set_runtime_status("Script stopped: PC out of range");
        return false;
    }

    char line[kScriptLineMax] = {};
    snprintf(line, sizeof(line), "%s", script_lines_[script_pc_].text);
    trim_eol(line);
    char *tokens[6] = {};
    const size_t token_count = tokenize_line(line, tokens, sizeof(tokens) / sizeof(tokens[0]));
    if (token_count == 0) {
        script_pc_++;
        return false;
    }

    if (token_equals(tokens[0], "END")) {
        script_stop(true);
        set_runtime_status("Script finished: %s.sk", script_name_);
        return false;
    }

    if (token_equals(tokens[0], "ENDIF")) {
        script_pc_++;
        return false;
    }

    if (token_equals(tokens[0], "ELSE")) {
        script_pc_ = script_find_endif(script_pc_ + 1);
        if (script_pc_ < script_line_count_) {
            script_pc_++;
        }
        return false;
    }

    if (token_equals(tokens[0], "IF")) {
        bool result = false;
        if (token_count >= 4) {
            const int reg = script_register_index(tokens[1]);
            float right = 0.0f;
            if ((reg >= 0) && script_value_from_token(tokens[3], &right)) {
                const float left = script_registers_[reg];
                if (token_equals(tokens[2], "<")) {
                    result = left < right;
                } else if (token_equals(tokens[2], ">")) {
                    result = left > right;
                } else if (token_equals(tokens[2], "<=")) {
                    result = left <= right;
                } else if (token_equals(tokens[2], ">=")) {
                    result = left >= right;
                } else if (token_equals(tokens[2], "!=")) {
                    result = left != right;
                } else if (token_equals(tokens[2], "==")) {
                    result = left == right;
                }
            }
        }

        if (result) {
            script_pc_++;
        } else {
            bool found_else = false;
            script_pc_ = script_find_else_or_endif(script_pc_ + 1, &found_else);
            if (!found_else && (script_pc_ < script_line_count_)) {
                script_pc_++;
            }
        }
        return false;
    }

    if (token_equals(tokens[0], "GOTO")) {
        if (token_count >= 2) {
            const long next = strtol(tokens[1], nullptr, 10);
            if ((next >= 0) && (static_cast<size_t>(next) < script_line_count_)) {
                script_pc_ = static_cast<size_t>(next);
            } else {
                script_stop(false);
                set_runtime_status("Script GOTO out of range: %ld", next);
            }
        } else {
            script_pc_++;
        }
        return false;
    }

    if (token_equals(tokens[0], "DELAY")) {
        float ms = 0.0f;
        if ((token_count >= 2) && script_value_from_token(tokens[1], &ms)) {
            if (ms < 0.0f) {
                ms = 0.0f;
            }
            script_wait_until_ = xTaskGetTickCount() + pdMS_TO_TICKS(static_cast<uint32_t>(ms));
        }
        script_pc_++;
        return false;
    }

    if (token_equals(tokens[0], "YIELD")) {
        script_pc_++;
        script_wait_until_ = xTaskGetTickCount() + 1;
        return false;
    }

    if (token_equals(tokens[0], "T")) {
        script_pc_++;
        return false;
    }

    if (token_equals(tokens[0], "LOAD")) {
        if (token_count >= 3) {
            const int reg = script_register_index(tokens[1]);
            float value = 0.0f;
            if ((reg >= 0) && script_value_from_token(tokens[2], &value)) {
                script_registers_[reg] = value;
            }
        }
        script_pc_++;
        return false;
    }

    if (token_equals(tokens[0], "PLUS") || token_equals(tokens[0], "MINUS")) {
        if (token_count >= 3) {
            const int reg = script_register_index(tokens[1]);
            float value = 0.0f;
            if ((reg >= 0) && script_value_from_token(tokens[2], &value)) {
                if (token_equals(tokens[0], "PLUS")) {
                    script_registers_[reg] += value;
                } else {
                    script_registers_[reg] -= value;
                }
            }
        }
        script_pc_++;
        return false;
    }

    const bool changed = script_apply_generator_command(tokens, token_count);
    script_pc_++;
    return changed;
}

bool SignalGenerator::script_apply_generator_command(char *tokens[], size_t token_count)
{
    if ((tokens == nullptr) || (token_count == 0) || (tokens[0] == nullptr)) {
        return false;
    }

    const char *head = tokens[0];
    if (strlen(head) < 3) {
        return false;
    }

    const int channel = head[2] - '1';
    if ((channel < 0) || (channel >= 2)) {
        return false;
    }

    bool changed = false;
    if ((state_lock_ != nullptr) && (xSemaphoreTake(state_lock_, portMAX_DELAY) == pdTRUE)) {
        ChannelConfig &ch = state_.ch[channel];

        if (token_equals(head, "CH1") || token_equals(head, "CH2")) {
            if (token_count >= 3) {
                const bool on = token_is_on(tokens[2]);
                if (token_equals(tokens[1], "CR")) {
                    ch.enabled = on;
                    changed = true;
                } else if (token_equals(tokens[1], "AM")) {
                    ch.am_enabled = on;
                    changed = true;
                } else if (token_equals(tokens[1], "FM")) {
                    ch.fm_enabled = on;
                    changed = true;
                }
            }
        } else if (token_equals(head, "CR1") || token_equals(head, "CR2")) {
            if (token_count >= 3) {
                if (token_equals(tokens[1], "FR")) {
                    float value = 0.0f;
                    if (script_value_from_token(tokens[2], &value)) {
                        ch.carrier_freq_hz = static_cast<uint32_t>(clamp_float(value, 20.0f, 40000.0f) + 0.5f);
                        changed = true;
                    }
                } else if (token_equals(tokens[1], "MOD")) {
                    const int index = find_wave_index(WaveSet::Carrier, tokens[2]);
                    if (index >= 0) {
                        ch.carrier_wave = index;
                        changed = true;
                    }
                }
            }
        } else if (token_equals(head, "AM1") || token_equals(head, "AM2")) {
            if (token_count >= 3) {
                if (token_equals(tokens[1], "FR")) {
                    float value = 0.0f;
                    if (script_value_from_token(tokens[2], &value)) {
                        ch.am_freq_hz = clamp_float(value, kAmFreqMinHz, kAmFreqMaxHz);
                        changed = true;
                    }
                } else if (token_equals(tokens[1], "MOD")) {
                    const int index = find_wave_index(WaveSet::Mod, tokens[2]);
                    if (index >= 0) {
                        ch.am_wave = index;
                        changed = true;
                    }
                }
            }
        } else if (token_equals(head, "FM1") || token_equals(head, "FM2")) {
            if (token_count >= 3) {
                if (token_equals(tokens[1], "BASE")) {
                    float value = 0.0f;
                    if (script_value_from_token(tokens[2], &value)) {
                        ch.fm_base_hz = static_cast<uint32_t>(clamp_float(value, 20.0f, 40000.0f) + 0.5f);
                        changed = true;
                    }
                } else if (token_equals(tokens[1], "DEV")) {
                    float value = 0.0f;
                    if (script_value_from_token(tokens[2], &value)) {
                        ch.fm_dev_hz = clamp_float(value, 0.0f, 20000.0f);
                        changed = true;
                    }
                } else if (token_equals(tokens[1], "FR")) {
                    float value = 0.0f;
                    if (script_value_from_token(tokens[2], &value)) {
                        ch.fm_freq_hz = clamp_float(value, 0.1f, 500.0f);
                        changed = true;
                    }
                } else if (token_equals(tokens[1], "MOD")) {
                    const int index = find_wave_index(WaveSet::Mod, tokens[2]);
                    if (index >= 0) {
                        ch.fm_wave = index;
                        changed = true;
                    }
                }
            }
        }

        ch.carrier_wave = static_cast<int>(clamp_u32(ch.carrier_wave, 0,
                                                     carrier_wave_count_ > 0 ? carrier_wave_count_ - 1 : 0));
        ch.am_wave = static_cast<int>(clamp_u32(ch.am_wave, 0,
                                                mod_wave_count_ > 0 ? mod_wave_count_ - 1 : 0));
        ch.fm_wave = static_cast<int>(clamp_u32(ch.fm_wave, 0,
                                                mod_wave_count_ > 0 ? mod_wave_count_ - 1 : 0));

        xSemaphoreGive(state_lock_);
    }

    return changed;
}

bool SignalGenerator::script_value_from_token(const char *token, float *value) const
{
    if ((token == nullptr) || (value == nullptr)) {
        return false;
    }

    const int reg = script_register_index(token);
    if (reg >= 0) {
        *value = script_registers_[reg];
        return true;
    }

    char *end = nullptr;
    const float parsed = strtof(token, &end);
    if ((end == token) || ((end != nullptr) && (*end != '\0'))) {
        return false;
    }

    *value = parsed;
    return true;
}

int SignalGenerator::script_register_index(const char *token) const
{
    if ((token == nullptr) || ((token[0] != 'F') && (token[0] != 'f')) ||
        (token[1] < '0') || (token[1] > '9') || (token[2] != '\0')) {
        return -1;
    }

    return token[1] - '0';
}

void SignalGenerator::script_set_line(size_t index, const char *text)
{
    if (index >= script_line_count_) {
        return;
    }

    snprintf(script_lines_[index].text,
             sizeof(script_lines_[index].text),
             "%s",
             (text != nullptr) && (text[0] != '\0') ? text : "?");
    trim_eol(script_lines_[index].text);
    script_dirty_ = true;
}

void SignalGenerator::script_insert_after_selected(const char *text)
{
    if (script_line_count_ >= kScriptMaxLines) {
        set_runtime_status("Script is full: %u lines", static_cast<unsigned>(kScriptMaxLines));
        return;
    }

    size_t insert_at = script_selected_line_ + 1;
    if (insert_at > script_line_count_) {
        insert_at = script_line_count_;
    }

    for (size_t i = script_line_count_; i > insert_at; i--) {
        script_lines_[i] = script_lines_[i - 1];
    }

    script_line_count_++;
    script_selected_line_ = insert_at;
    script_set_line(insert_at, text);
}

void SignalGenerator::script_delete_selected(void)
{
    if (script_line_count_ == 0) {
        return;
    }

    for (size_t i = script_selected_line_; i + 1 < script_line_count_; i++) {
        script_lines_[i] = script_lines_[i + 1];
    }
    script_line_count_--;
    if (script_line_count_ == 0) {
        script_line_count_ = 1;
        snprintf(script_lines_[0].text, sizeof(script_lines_[0].text), "END");
    }
    if (script_selected_line_ >= script_line_count_) {
        script_selected_line_ = script_line_count_ - 1;
    }
    script_dirty_ = true;
}

void SignalGenerator::script_move_selected(int direction)
{
    if (script_line_count_ < 2) {
        return;
    }

    if ((direction < 0) && (script_selected_line_ > 0)) {
        ScriptLine tmp = script_lines_[script_selected_line_ - 1];
        script_lines_[script_selected_line_ - 1] = script_lines_[script_selected_line_];
        script_lines_[script_selected_line_] = tmp;
        script_selected_line_--;
        script_dirty_ = true;
    } else if ((direction > 0) && (script_selected_line_ + 1 < script_line_count_)) {
        ScriptLine tmp = script_lines_[script_selected_line_ + 1];
        script_lines_[script_selected_line_ + 1] = script_lines_[script_selected_line_];
        script_lines_[script_selected_line_] = tmp;
        script_selected_line_++;
        script_dirty_ = true;
    }
}

void SignalGenerator::script_keyboard_press(uint8_t key_index)
{
    if (key_index >= kScriptKeyCount) {
        return;
    }

    switch (script_keyboard_route_) {
    case ScriptKeyboardRoute::Home:
        switch (key_index) {
        case 0:
            script_begin_command("CH1", ScriptKeyboardRoute::CramFm, 1);
            break;
        case 1:
            script_begin_command("CR1", ScriptKeyboardRoute::CramValue, 1);
            break;
        case 2:
            script_begin_command("AM1", ScriptKeyboardRoute::CramValue, 1);
            break;
        case 3:
            script_begin_command("FM1", ScriptKeyboardRoute::FmValue, 1);
            break;
        case 4:
            script_begin_command("CH2", ScriptKeyboardRoute::CramFm, 1);
            break;
        case 5:
            script_begin_command("CR2", ScriptKeyboardRoute::CramValue, 1);
            break;
        case 6:
            script_begin_command("AM2", ScriptKeyboardRoute::CramValue, 1);
            break;
        case 7:
            script_begin_command("FM2", ScriptKeyboardRoute::FmValue, 1);
            break;
        case 8:
            script_begin_command("GOTO", ScriptKeyboardRoute::Number, 1);
            break;
        case 9:
            script_begin_command("IF", ScriptKeyboardRoute::Register, 1, ScriptKeyboardRoute::Comparison);
            break;
        case 10:
            script_set_line(script_selected_line_, "ELSE");
            script_keyboard_home();
            break;
        case 11:
            script_begin_command("PLUS", ScriptKeyboardRoute::Register, 1, ScriptKeyboardRoute::IfValue);
            break;
        case 12:
            script_begin_command("DELAY", ScriptKeyboardRoute::Number, 1);
            break;
        case 13:
            script_set_line(script_selected_line_, "ENDIF");
            script_keyboard_home();
            break;
        case 14:
            script_begin_command("LOAD", ScriptKeyboardRoute::Register, 1, ScriptKeyboardRoute::IfValue);
            break;
        case 15:
            script_begin_command("MINUS", ScriptKeyboardRoute::Register, 1, ScriptKeyboardRoute::IfValue);
            break;
        default:
            break;
        }
        break;

    case ScriptKeyboardRoute::Number:
        if (key_index == 3) {
            script_delete_number_char();
        } else if (key_index == 7) {
            script_keyboard_back();
        } else if (key_index == 15) {
            script_finish_number();
        } else if ((key_index <= 2) || ((key_index >= 4) && (key_index <= 6)) ||
                   ((key_index >= 8) && (key_index <= 10)) || (key_index == 12) ||
                   (key_index == 13)) {
            const char chars[kScriptKeyCount] = {
                '1', '2', '3', '\0',
                '4', '5', '6', '\0',
                '7', '8', '9', '\0',
                '.', '0', '\0', '\0',
            };
            if (chars[key_index] != '\0') {
                script_append_number(chars[key_index]);
            }
        }
        break;

    case ScriptKeyboardRoute::Register: {
        if (key_index == 3) {
            script_keyboard_back();
            break;
        }
        const char *registers[kScriptKeyCount] = {
            "F1", "F2", "F3", nullptr,
            "F4", "F5", "F6", nullptr,
            "F7", "F8", "F9", nullptr,
            nullptr, "F0", nullptr, nullptr,
        };
        if (registers[key_index] != nullptr) {
            script_replace_token(script_keyboard_argument_, registers[key_index]);
            if (script_keyboard_next_route_ == ScriptKeyboardRoute::Home) {
                script_keyboard_home();
            } else {
                script_keyboard_route_to(script_keyboard_next_route_,
                                         script_keyboard_argument_ + 1,
                                         ScriptKeyboardRoute::Home);
            }
        }
        break;
    }

    case ScriptKeyboardRoute::OnOff:
        if (key_index == 0) {
            script_replace_token(script_keyboard_argument_, "ON");
            script_keyboard_home();
        } else if (key_index == 1) {
            script_replace_token(script_keyboard_argument_, "OFF");
            script_keyboard_home();
        } else if (key_index == 3) {
            script_keyboard_back();
        }
        break;

    case ScriptKeyboardRoute::CramFm:
        if (key_index == 0) {
            script_replace_token(script_keyboard_argument_, "CR");
            script_keyboard_route_to(ScriptKeyboardRoute::OnOff, script_keyboard_argument_ + 1);
        } else if (key_index == 1) {
            script_replace_token(script_keyboard_argument_, "AM");
            script_keyboard_route_to(ScriptKeyboardRoute::OnOff, script_keyboard_argument_ + 1);
        } else if (key_index == 2) {
            script_replace_token(script_keyboard_argument_, "FM");
            script_keyboard_route_to(ScriptKeyboardRoute::OnOff, script_keyboard_argument_ + 1);
        } else if (key_index == 3) {
            script_keyboard_back();
        }
        break;

    case ScriptKeyboardRoute::CramValue:
        if (key_index == 0) {
            script_replace_token(script_keyboard_argument_, "FR");
            script_keyboard_route_to(ScriptKeyboardRoute::Register, script_keyboard_argument_ + 1);
        } else if (key_index == 1) {
            script_replace_token(script_keyboard_argument_, "FR");
            script_keyboard_route_to(ScriptKeyboardRoute::Number, script_keyboard_argument_ + 1);
        } else if (key_index == 3) {
            script_keyboard_back();
        } else if (key_index == 4) {
            char head[12] = {};
            script_get_token(0, head, sizeof(head));
            script_replace_token(script_keyboard_argument_, "MOD");
            script_keyboard_route_to((token_equals(head, "CR1") || token_equals(head, "CR2")) ?
                                         ScriptKeyboardRoute::ModCarrier :
                                         ScriptKeyboardRoute::ModAm,
                                     script_keyboard_argument_ + 1);
        }
        break;

    case ScriptKeyboardRoute::FmValue:
        if (key_index == 0) {
            script_replace_token(script_keyboard_argument_, "FR");
            script_keyboard_route_to(ScriptKeyboardRoute::Register, script_keyboard_argument_ + 1);
        } else if (key_index == 1) {
            script_replace_token(script_keyboard_argument_, "FR");
            script_keyboard_route_to(ScriptKeyboardRoute::Number, script_keyboard_argument_ + 1);
        } else if (key_index == 3) {
            script_keyboard_back();
        } else if (key_index == 4) {
            script_replace_token(script_keyboard_argument_, "MOD");
            script_keyboard_route_to(ScriptKeyboardRoute::ModFm, script_keyboard_argument_ + 1);
        } else if (key_index == 8) {
            script_replace_token(script_keyboard_argument_, "BASE");
            script_keyboard_route_to(ScriptKeyboardRoute::Register, script_keyboard_argument_ + 1);
        } else if (key_index == 9) {
            script_replace_token(script_keyboard_argument_, "BASE");
            script_keyboard_route_to(ScriptKeyboardRoute::Number, script_keyboard_argument_ + 1);
        } else if (key_index == 12) {
            script_replace_token(script_keyboard_argument_, "DEV");
            script_keyboard_route_to(ScriptKeyboardRoute::Register, script_keyboard_argument_ + 1);
        } else if (key_index == 13) {
            script_replace_token(script_keyboard_argument_, "DEV");
            script_keyboard_route_to(ScriptKeyboardRoute::Number, script_keyboard_argument_ + 1);
        }
        break;

    case ScriptKeyboardRoute::Comparison: {
        if (key_index == 7) {
            script_keyboard_back();
            break;
        }
        const char *ops[kScriptKeyCount] = {
            "<", ">", nullptr, nullptr,
            "<=", ">=", nullptr, nullptr,
            "==", "!=", nullptr, nullptr,
            nullptr, nullptr, nullptr, nullptr,
        };
        if (ops[key_index] != nullptr) {
            script_replace_token(script_keyboard_argument_, ops[key_index]);
            script_keyboard_route_to(ScriptKeyboardRoute::IfValue, script_keyboard_argument_ + 1);
        }
        break;
    }

    case ScriptKeyboardRoute::IfValue:
        if (key_index == 0) {
            script_keyboard_route_to(ScriptKeyboardRoute::Register, script_keyboard_argument_);
        } else if (key_index == 1) {
            script_keyboard_route_to(ScriptKeyboardRoute::Number, script_keyboard_argument_);
        } else if (key_index == 3) {
            script_keyboard_back();
        }
        break;

    case ScriptKeyboardRoute::ModCarrier:
    case ScriptKeyboardRoute::ModAm:
    case ScriptKeyboardRoute::ModFm: {
        if (key_index == 3) {
            script_keyboard_back();
            break;
        }

        const Waveform *items = script_keyboard_route_ == ScriptKeyboardRoute::ModCarrier ? carrier_waves_ : mod_waves_;
        const size_t count = script_keyboard_route_ == ScriptKeyboardRoute::ModCarrier ? carrier_wave_count_ : mod_wave_count_;
        size_t wave_index = 0;
        bool selected = false;
        for (size_t i = 0; i < kScriptKeyCount && wave_index < count; i++) {
            if (i == 3) {
                continue;
            }
            if (i == key_index) {
                script_replace_token(script_keyboard_argument_, items[wave_index].name);
                selected = true;
                break;
            }
            wave_index++;
        }
        if (selected) {
            script_keyboard_home();
        }
        break;
    }
    }
}

void SignalGenerator::script_keyboard_home(void)
{
    script_keyboard_route_ = ScriptKeyboardRoute::Home;
    script_keyboard_argument_ = 0;
    script_keyboard_next_route_ = ScriptKeyboardRoute::Home;
    script_keyboard_stack_count_ = 0;
}

void SignalGenerator::script_keyboard_route_to(ScriptKeyboardRoute route,
                                               uint8_t argument,
                                               ScriptKeyboardRoute next_route)
{
    if (script_keyboard_stack_count_ < kScriptRouteStackMax) {
        script_keyboard_stack_[script_keyboard_stack_count_++] = {
            .route = script_keyboard_route_,
            .argument = script_keyboard_argument_,
            .next_route = script_keyboard_next_route_,
        };
    }

    script_keyboard_route_ = route;
    script_keyboard_argument_ = argument;
    script_keyboard_next_route_ = next_route;
}

void SignalGenerator::script_keyboard_back(void)
{
    script_remove_tokens_from(script_keyboard_argument_);

    if (script_keyboard_stack_count_ == 0) {
        script_keyboard_home();
        return;
    }

    const ScriptKeyboardFrame previous = script_keyboard_stack_[--script_keyboard_stack_count_];
    script_keyboard_route_ = previous.route;
    script_keyboard_argument_ = previous.argument;
    script_keyboard_next_route_ = previous.next_route;
}

void SignalGenerator::script_begin_command(const char *token,
                                           ScriptKeyboardRoute route,
                                           uint8_t argument,
                                           ScriptKeyboardRoute next_route)
{
    if (script_line_count_ == 0) {
        script_new();
    }

    script_keyboard_home();
    script_set_line(script_selected_line_, token);
    script_keyboard_route_to(route, argument, next_route);
}

void SignalGenerator::script_replace_token(uint8_t token_index, const char *token)
{
    if (script_line_count_ == 0) {
        script_new();
    }
    if (token_index >= kScriptTokenMax) {
        return;
    }

    char line[kScriptLineMax] = {};
    snprintf(line, sizeof(line), "%s", script_lines_[script_selected_line_].text);
    char *parsed[kScriptTokenMax] = {};
    const size_t parsed_count = tokenize_line(line, parsed, kScriptTokenMax);

    char tokens[kScriptTokenMax][16] = {};
    size_t count = parsed_count;
    for (size_t i = 0; i < parsed_count && i < kScriptTokenMax; i++) {
        snprintf(tokens[i], sizeof(tokens[i]), "%s", parsed[i]);
    }

    while (count <= token_index && count < kScriptTokenMax) {
        snprintf(tokens[count], sizeof(tokens[count]), "?");
        count++;
    }

    snprintf(tokens[token_index], sizeof(tokens[token_index]), "%s",
             (token != nullptr) && (token[0] != '\0') ? token : "?");

    char rebuilt[kScriptLineMax] = {};
    size_t used = 0;
    for (size_t i = 0; i < count; i++) {
        const int written = snprintf(rebuilt + used,
                                     sizeof(rebuilt) - used,
                                     "%s%s",
                                     i == 0 ? "" : " ",
                                     tokens[i]);
        if (written <= 0) {
            break;
        }
        const size_t add = static_cast<size_t>(written);
        if (add >= (sizeof(rebuilt) - used)) {
            rebuilt[sizeof(rebuilt) - 1] = '\0';
            break;
        }
        used += add;
    }

    script_set_line(script_selected_line_, rebuilt);
}

void SignalGenerator::script_remove_tokens_from(uint8_t token_index)
{
    if ((script_line_count_ == 0) || (token_index == 0)) {
        return;
    }

    char line[kScriptLineMax] = {};
    snprintf(line, sizeof(line), "%s", script_lines_[script_selected_line_].text);
    char *tokens[kScriptTokenMax] = {};
    const size_t count = tokenize_line(line, tokens, kScriptTokenMax);
    if (count == 0) {
        return;
    }

    const size_t keep = token_index < count ? token_index : count;
    char rebuilt[kScriptLineMax] = {};
    size_t used = 0;
    for (size_t i = 0; i < keep; i++) {
        const int written = snprintf(rebuilt + used,
                                     sizeof(rebuilt) - used,
                                     "%s%s",
                                     i == 0 ? "" : " ",
                                     tokens[i]);
        if (written <= 0) {
            break;
        }
        const size_t add = static_cast<size_t>(written);
        if (add >= (sizeof(rebuilt) - used)) {
            rebuilt[sizeof(rebuilt) - 1] = '\0';
            break;
        }
        used += add;
    }

    script_set_line(script_selected_line_, rebuilt[0] != '\0' ? rebuilt : "?");
}

bool SignalGenerator::script_get_token(uint8_t token_index, char *out, size_t out_size) const
{
    if ((out == nullptr) || (out_size == 0) || (script_line_count_ == 0) ||
        (token_index >= kScriptTokenMax)) {
        return false;
    }

    out[0] = '\0';
    char line[kScriptLineMax] = {};
    snprintf(line, sizeof(line), "%s", script_lines_[script_selected_line_].text);
    char *tokens[kScriptTokenMax] = {};
    const size_t count = tokenize_line(line, tokens, kScriptTokenMax);
    if (token_index >= count) {
        return false;
    }

    snprintf(out, out_size, "%s", tokens[token_index]);
    return true;
}

void SignalGenerator::script_append_number(char ch)
{
    char value[20] = {};
    script_get_token(script_keyboard_argument_, value, sizeof(value));
    if (token_equals(value, "?")) {
        value[0] = '\0';
    }

    const size_t len = strlen(value);
    if (len + 1 >= sizeof(value)) {
        return;
    }

    if ((ch == '.') && (strchr(value, '.') != nullptr)) {
        return;
    }

    value[len] = ch;
    value[len + 1] = '\0';
    script_replace_token(script_keyboard_argument_, value);
}

void SignalGenerator::script_delete_number_char(void)
{
    char value[20] = {};
    script_get_token(script_keyboard_argument_, value, sizeof(value));
    if (token_equals(value, "?")) {
        return;
    }

    const size_t len = strlen(value);
    if (len == 0) {
        script_replace_token(script_keyboard_argument_, "?");
        return;
    }

    value[len - 1] = '\0';
    script_replace_token(script_keyboard_argument_, value[0] != '\0' ? value : "?");
}

void SignalGenerator::script_finish_number(void)
{
    char value[20] = {};
    if (!script_get_token(script_keyboard_argument_, value, sizeof(value)) ||
        token_equals(value, "?") || (value[0] == '\0')) {
        script_replace_token(script_keyboard_argument_, "0");
    }
    script_keyboard_home();
}

size_t SignalGenerator::script_find_else_or_endif(size_t from_pc, bool *found_else) const
{
    if (found_else != nullptr) {
        *found_else = false;
    }

    uint8_t depth = 0;
    for (size_t i = from_pc; i < script_line_count_; i++) {
        char line[kScriptLineMax] = {};
        snprintf(line, sizeof(line), "%s", script_lines_[i].text);
        char *tokens[2] = {};
        const size_t count = tokenize_line(line, tokens, 2);
        if (count == 0) {
            continue;
        }

        if (token_equals(tokens[0], "IF")) {
            depth++;
        } else if (token_equals(tokens[0], "ENDIF")) {
            if (depth == 0) {
                return i;
            }
            depth--;
        } else if ((depth == 0) && token_equals(tokens[0], "ELSE")) {
            if (found_else != nullptr) {
                *found_else = true;
            }
            return i + 1;
        }
    }

    return script_line_count_;
}

size_t SignalGenerator::script_find_endif(size_t from_pc) const
{
    uint8_t depth = 0;
    for (size_t i = from_pc; i < script_line_count_; i++) {
        char line[kScriptLineMax] = {};
        snprintf(line, sizeof(line), "%s", script_lines_[i].text);
        char *tokens[2] = {};
        const size_t count = tokenize_line(line, tokens, 2);
        if (count == 0) {
            continue;
        }

        if (token_equals(tokens[0], "IF")) {
            depth++;
        } else if (token_equals(tokens[0], "ENDIF")) {
            if (depth == 0) {
                return i;
            }
            depth--;
        }
    }

    return script_line_count_;
}

bool SignalGenerator::save_preset_file(const char *name)
{
    if ((name == nullptr) || (name[0] == '\0')) {
        return false;
    }

    mkdir(kPresetDir, 0775);

    GeneratorState snapshot = {};
    if ((state_lock_ != nullptr) && (xSemaphoreTake(state_lock_, portMAX_DELAY) == pdTRUE)) {
        snapshot = state_;
        xSemaphoreGive(state_lock_);
    } else {
        snapshot = state_;
    }

    char path[128] = {};
    snprintf(path, sizeof(path), "%s/%s.txt", kPresetDir, name);
    FILE *file = fopen(path, "w");
    if (file == nullptr) {
        set_runtime_status("Preset save failed: %s", path);
        return false;
    }

    fprintf(file, "sample_rate_hz=%lu\n", static_cast<unsigned long>(snapshot.sample_rate_hz));
    fprintf(file, "volume_percent=%u\n", snapshot.volume_percent);

    for (size_t i = 0; i < 2; i++) {
        const ChannelConfig &ch = snapshot.ch[i];
        const size_t carrier_index = clamp_wave_index(ch.carrier_wave, carrier_wave_count_);
        const size_t am_index = clamp_wave_index(ch.am_wave, mod_wave_count_);
        const size_t fm_index = clamp_wave_index(ch.fm_wave, mod_wave_count_);
        const char *carrier_name = (carrier_waves_ != nullptr) && (carrier_wave_count_ > 0) ?
                                   carrier_waves_[carrier_index].name : "Sine";
        const char *am_name = (mod_waves_ != nullptr) && (mod_wave_count_ > 0) ?
                              mod_waves_[am_index].name : "Sine";
        const char *fm_name = (mod_waves_ != nullptr) && (mod_wave_count_ > 0) ?
                              mod_waves_[fm_index].name : "Sine";

        fprintf(file, "ch%u_enabled=%u\n", static_cast<unsigned>(i + 1), ch.enabled ? 1 : 0);
        fprintf(file, "ch%u_carrier_wave=%s\n", static_cast<unsigned>(i + 1), carrier_name);
        fprintf(file, "ch%u_carrier_freq_hz=%lu\n", static_cast<unsigned>(i + 1),
                static_cast<unsigned long>(ch.carrier_freq_hz));
        fprintf(file, "ch%u_am_enabled=%u\n", static_cast<unsigned>(i + 1), ch.am_enabled ? 1 : 0);
        fprintf(file, "ch%u_am_wave=%s\n", static_cast<unsigned>(i + 1), am_name);
        fprintf(file, "ch%u_am_freq_hz=%.3f\n", static_cast<unsigned>(i + 1), static_cast<double>(ch.am_freq_hz));
        fprintf(file, "ch%u_fm_enabled=%u\n", static_cast<unsigned>(i + 1), ch.fm_enabled ? 1 : 0);
        fprintf(file, "ch%u_fm_wave=%s\n", static_cast<unsigned>(i + 1), fm_name);
        fprintf(file, "ch%u_fm_base_hz=%lu\n", static_cast<unsigned>(i + 1),
                static_cast<unsigned long>(ch.fm_base_hz));
        fprintf(file, "ch%u_fm_dev_hz=%.3f\n", static_cast<unsigned>(i + 1), static_cast<double>(ch.fm_dev_hz));
        fprintf(file, "ch%u_fm_freq_hz=%.3f\n", static_cast<unsigned>(i + 1), static_cast<double>(ch.fm_freq_hz));
    }

    fclose(file);
    return true;
}

bool SignalGenerator::load_preset_file(const char *name)
{
    if ((name == nullptr) || (name[0] == '\0')) {
        return false;
    }

    char path[128] = {};
    snprintf(path, sizeof(path), "%s/%s.txt", kPresetDir, name);
    FILE *file = fopen(path, "r");
    if (file == nullptr) {
        set_runtime_status("Preset not found: %s.txt", name);
        return false;
    }

    GeneratorState loaded = {};
    if ((state_lock_ != nullptr) && (xSemaphoreTake(state_lock_, portMAX_DELAY) == pdTRUE)) {
        loaded = state_;
        xSemaphoreGive(state_lock_);
    } else {
        loaded = state_;
    }

    char line[160] = {};
    while (fgets(line, sizeof(line), file) != nullptr) {
        trim_eol(line);
        char *equal = strchr(line, '=');
        if (equal == nullptr) {
            continue;
        }
        *equal = '\0';
        const char *key = line;
        const char *value = equal + 1;

        if (token_equals(key, "sample_rate_hz")) {
            loaded.sample_rate_hz = static_cast<uint32_t>(strtoul(value, nullptr, 10));
        } else if (token_equals(key, "volume_percent")) {
            loaded.volume_percent = static_cast<uint8_t>(clamp_u32(strtoul(value, nullptr, 10), 0, 100));
        } else {
            for (size_t i = 0; i < 2; i++) {
                char prefix[8] = {};
                snprintf(prefix, sizeof(prefix), "ch%u_", static_cast<unsigned>(i + 1));
                if (str_case_cmp(key, prefix) < 0 || strncmp(key, prefix, strlen(prefix)) != 0) {
                    continue;
                }

                ChannelConfig &ch = loaded.ch[i];
                const char *field = key + strlen(prefix);
                if (token_equals(field, "enabled")) {
                    ch.enabled = strtoul(value, nullptr, 10) != 0;
                } else if (token_equals(field, "carrier_wave")) {
                    const int index = find_wave_index(WaveSet::Carrier, value);
                    if (index >= 0) {
                        ch.carrier_wave = index;
                    }
                } else if (token_equals(field, "carrier_freq_hz")) {
                    ch.carrier_freq_hz =
                        static_cast<uint32_t>(clamp_float(strtof(value, nullptr), 20.0f, 40000.0f) + 0.5f);
                } else if (token_equals(field, "am_enabled")) {
                    ch.am_enabled = strtoul(value, nullptr, 10) != 0;
                } else if (token_equals(field, "am_wave")) {
                    const int index = find_wave_index(WaveSet::Mod, value);
                    if (index >= 0) {
                        ch.am_wave = index;
                    }
                } else if (token_equals(field, "am_freq_hz")) {
                    ch.am_freq_hz = clamp_float(strtof(value, nullptr), kAmFreqMinHz, kAmFreqMaxHz);
                } else if (token_equals(field, "fm_enabled")) {
                    ch.fm_enabled = strtoul(value, nullptr, 10) != 0;
                } else if (token_equals(field, "fm_wave")) {
                    const int index = find_wave_index(WaveSet::Mod, value);
                    if (index >= 0) {
                        ch.fm_wave = index;
                    }
                } else if (token_equals(field, "fm_base_hz")) {
                    ch.fm_base_hz =
                        static_cast<uint32_t>(clamp_float(strtof(value, nullptr), 20.0f, 40000.0f) + 0.5f);
                } else if (token_equals(field, "fm_dev_hz")) {
                    ch.fm_dev_hz = clamp_float(strtof(value, nullptr), 0.0f, 20000.0f);
                } else if (token_equals(field, "fm_freq_hz")) {
                    ch.fm_freq_hz = clamp_float(strtof(value, nullptr), 0.1f, 500.0f);
                }
            }
        }
    }
    fclose(file);

    loaded.volume_percent = static_cast<uint8_t>(clamp_u32(loaded.volume_percent, 0, 100));
    if ((state_lock_ != nullptr) && (xSemaphoreTake(state_lock_, portMAX_DELAY) == pdTRUE)) {
        state_ = loaded;
        xSemaphoreGive(state_lock_);
    } else {
        state_ = loaded;
    }

    return true;
}

