#include "music_player/MusicPlayerInternal.hpp"

namespace music_player_internal {

// Small helpers shared by the split MusicPlayer translation units. Keeping
// these out of the class leaves the public app state focused on runtime data.

const char *TAG = "music_player";

static char ascii_lower(char value) {
  if ((value >= 'A') && (value <= 'Z')) {
    return static_cast<char>(value + ('a' - 'A'));
  }
  return value;
}

int str_case_cmp(const char *left, const char *right) {
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

static bool str_ends_with_ignore_case(const char *text, const char *suffix) {
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

bool is_mp3_name(const char *name) {
  return str_ends_with_ignore_case(name, ".mp3");
}

uint16_t rgb565(uint32_t color) {
  return lv_color_to_u16(lv_color_hex(color));
}

static uint32_t read_be32(const uint8_t *data) {
  return (static_cast<uint32_t>(data[0]) << 24) |
         (static_cast<uint32_t>(data[1]) << 16) |
         (static_cast<uint32_t>(data[2]) << 8) | static_cast<uint32_t>(data[3]);
}

static uint32_t read_synchsafe32(const uint8_t *data) {
  return (static_cast<uint32_t>(data[0] & 0x7F) << 21) |
         (static_cast<uint32_t>(data[1] & 0x7F) << 14) |
         (static_cast<uint32_t>(data[2] & 0x7F) << 7) |
         static_cast<uint32_t>(data[3] & 0x7F);
}

static bool parse_mp3_header(uint32_t header, Mp3HeaderInfo *info) {
  if ((header & 0xFFE00000U) != 0xFFE00000U) {
    return false;
  }

  const uint8_t version_id = static_cast<uint8_t>((header >> 19) & 0x03U);
  const uint8_t layer = static_cast<uint8_t>((header >> 17) & 0x03U);
  const uint8_t bitrate_index = static_cast<uint8_t>((header >> 12) & 0x0FU);
  const uint8_t sample_index = static_cast<uint8_t>((header >> 10) & 0x03U);
  const uint8_t padding = static_cast<uint8_t>((header >> 9) & 0x01U);
  const uint8_t channel_mode = static_cast<uint8_t>((header >> 6) & 0x03U);

  if ((version_id == 1) || (layer != 1) || (bitrate_index == 0) ||
      (bitrate_index == 15) || (sample_index == 3)) {
    return false;
  }

  static const uint16_t bitrate_mpeg1_l3[16] = {
      0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0,
  };
  static const uint16_t bitrate_mpeg2_l3[16] = {
      0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0,
  };
  static const uint16_t sample_rate_table[3][3] = {
      {11025, 12000, 8000},
      {22050, 24000, 16000},
      {44100, 48000, 32000},
  };

  const bool mpeg1 = version_id == 3;
  const uint8_t sample_row = mpeg1 ? 2 : (version_id == 2 ? 1 : 0);
  const uint32_t bitrate_kbps =
      mpeg1 ? bitrate_mpeg1_l3[bitrate_index] : bitrate_mpeg2_l3[bitrate_index];
  const uint32_t sample_rate_hz = sample_rate_table[sample_row][sample_index];
  const uint32_t samples_per_frame = mpeg1 ? 1152U : 576U;
  const uint32_t frame_size =
      (((mpeg1 ? 144000U : 72000U) * bitrate_kbps) / sample_rate_hz) + padding;

  if ((bitrate_kbps == 0) || (sample_rate_hz == 0) || (frame_size < 24)) {
    return false;
  }

  if (info != nullptr) {
    info->bitrate_kbps = bitrate_kbps;
    info->sample_rate_hz = sample_rate_hz;
    info->samples_per_frame = samples_per_frame;
    info->frame_size = frame_size;
    info->channel_mode = channel_mode;
    info->mpeg1 = mpeg1;
  }

  return true;
}

static uint32_t mp3_data_start(FILE *fp, uint32_t file_size) {
  if ((fp == nullptr) || (file_size < 10)) {
    return 0;
  }

  uint8_t header[10] = {};
  if ((fseek(fp, 0, SEEK_SET) != 0) ||
      (fread(header, 1, sizeof(header), fp) != sizeof(header))) {
    return 0;
  }

  if ((memcmp(header, "ID3", 3) != 0) || ((header[6] & 0x80) != 0) ||
      ((header[7] & 0x80) != 0) || ((header[8] & 0x80) != 0) ||
      ((header[9] & 0x80) != 0)) {
    return 0;
  }

  uint32_t offset = 10U + read_synchsafe32(&header[6]);
  if ((header[5] & 0x10U) != 0) {
    offset += 10U;
  }

  return offset < file_size ? offset : 0;
}

static uint32_t mp3_audio_end(FILE *fp, uint32_t file_size) {
  if ((fp == nullptr) || (file_size <= 128)) {
    return file_size;
  }

  uint8_t tag[3] = {};
  if ((fseek(fp, static_cast<long>(file_size - 128U), SEEK_SET) == 0) &&
      (fread(tag, 1, sizeof(tag), fp) == sizeof(tag)) &&
      (memcmp(tag, "TAG", 3) == 0)) {
    return file_size - 128U;
  }

  return file_size;
}

static bool find_first_mp3_frame(FILE *fp, uint32_t file_size,
                                 uint32_t start_offset, Mp3HeaderInfo *info,
                                 uint32_t *frame_offset, uint8_t *frame_data,
                                 size_t frame_data_capacity,
                                 size_t *frame_data_len) {
  if ((fp == nullptr) || (file_size < 4) ||
      (start_offset >= (file_size - 4U))) {
    return false;
  }

  if (fseek(fp, static_cast<long>(start_offset), SEEK_SET) != 0) {
    return false;
  }

  uint8_t window[4] = {};
  if (fread(window, 1, sizeof(window), fp) != sizeof(window)) {
    return false;
  }

  uint32_t offset = start_offset;
  const uint32_t last_offset = file_size - 4U;
  uint32_t max_offset = start_offset + (128U * 1024U);
  if ((max_offset < start_offset) || (max_offset > last_offset)) {
    max_offset = last_offset;
  }

  while (offset <= max_offset) {
    Mp3HeaderInfo parsed = {};
    if (parse_mp3_header(read_be32(window), &parsed)) {
      bool verified = true;
      const uint32_t next_offset = offset + parsed.frame_size;
      if ((next_offset + 4U) < file_size) {
        uint8_t next_header[4] = {};
        const long resume_pos = ftell(fp);
        verified = (fseek(fp, static_cast<long>(next_offset), SEEK_SET) == 0) &&
                   (fread(next_header, 1, sizeof(next_header), fp) ==
                    sizeof(next_header)) &&
                   parse_mp3_header(read_be32(next_header), nullptr);
        if (resume_pos >= 0) {
          fseek(fp, resume_pos, SEEK_SET);
        }
      }

      if (verified) {
        if (info != nullptr) {
          *info = parsed;
        }
        if (frame_offset != nullptr) {
          *frame_offset = offset;
        }
        if ((frame_data != nullptr) && (frame_data_capacity > 0)) {
          const size_t to_read = parsed.frame_size < frame_data_capacity
                                     ? parsed.frame_size
                                     : frame_data_capacity;
          if (fseek(fp, static_cast<long>(offset), SEEK_SET) == 0) {
            const size_t got = fread(frame_data, 1, to_read, fp);
            if (frame_data_len != nullptr) {
              *frame_data_len = got;
            }
          }
        } else if (frame_data_len != nullptr) {
          *frame_data_len = 0;
        }
        return true;
      }
    }

    offset++;
    window[0] = window[1];
    window[1] = window[2];
    window[2] = window[3];
    if (fread(&window[3], 1, 1, fp) != 1) {
      break;
    }
  }

  return false;
}

static uint32_t vbr_frame_count(const uint8_t *frame, size_t frame_len,
                                const Mp3HeaderInfo &info) {
  if ((frame == nullptr) || (frame_len < 64)) {
    return 0;
  }

  const size_t side_info = info.mpeg1 ? (info.channel_mode == 3 ? 17U : 32U)
                                      : (info.channel_mode == 3 ? 9U : 17U);
  const size_t xing_offset = 4U + side_info;
  if ((xing_offset + 12U) <= frame_len &&
      ((memcmp(&frame[xing_offset], "Xing", 4) == 0) ||
       (memcmp(&frame[xing_offset], "Info", 4) == 0))) {
    const uint32_t flags = read_be32(&frame[xing_offset + 4U]);
    if ((flags & 0x01U) != 0) {
      return read_be32(&frame[xing_offset + 8U]);
    }
  }

  const size_t vbri_offset = 36U;
  if ((vbri_offset + 18U) <= frame_len &&
      (memcmp(&frame[vbri_offset], "VBRI", 4) == 0)) {
    return read_be32(&frame[vbri_offset + 14U]);
  }

  return 0;
}

uint32_t probe_mp3_duration_ms(const char *path, uint32_t file_size) {
  if ((path == nullptr) || (file_size < 4)) {
    return 0;
  }

  FILE *fp = fopen(path, "rb");
  if (fp == nullptr) {
    return 0;
  }

  const uint32_t data_start = mp3_data_start(fp, file_size);
  Mp3HeaderInfo info = {};
  uint32_t first_frame_offset = data_start;
  uint8_t first_frame[256] = {};
  size_t first_frame_len = 0;
  const bool found = find_first_mp3_frame(
      fp, file_size, data_start, &info, &first_frame_offset, first_frame,
      sizeof(first_frame), &first_frame_len);
  if (!found) {
    fclose(fp);
    return 0;
  }

  uint32_t duration_ms = 0;
  const uint32_t frame_count =
      vbr_frame_count(first_frame, first_frame_len, info);
  if (frame_count > 0) {
    duration_ms = static_cast<uint32_t>((static_cast<uint64_t>(frame_count) *
                                         info.samples_per_frame * 1000ULL) /
                                        info.sample_rate_hz);
  } else {
    const uint32_t audio_end = mp3_audio_end(fp, file_size);
    if ((audio_end > first_frame_offset) && (info.bitrate_kbps > 0)) {
      const uint32_t audio_bytes = audio_end - first_frame_offset;
      duration_ms = static_cast<uint32_t>(
          (static_cast<uint64_t>(audio_bytes) * 8ULL) / info.bitrate_kbps);
    }
  }

  fclose(fp);
  return duration_ms;
}

void join_path(char *out, size_t out_size, const char *dir,
                      const char *name) {
  const size_t len = strlen(dir);
  if ((len > 0) && (dir[len - 1] == '/')) {
    snprintf(out, out_size, "%s%s", dir, name);
  } else {
    snprintf(out, out_size, "%s/%s", dir, name);
  }
}

void format_size(char *out, size_t out_size, uint32_t bytes) {
  if (bytes >= (1024U * 1024U)) {
    snprintf(out, out_size, "%lu.%lu MB",
             static_cast<unsigned long>(bytes / (1024U * 1024U)),
             static_cast<unsigned long>(((bytes % (1024U * 1024U)) * 10U) /
                                        (1024U * 1024U)));
  } else {
    snprintf(out, out_size, "%lu KB",
             static_cast<unsigned long>((bytes + 1023U) / 1024U));
  }
}

void format_time_ms(char *out, size_t out_size, uint32_t ms) {
  uint32_t seconds = (ms + 500U) / 1000U;
  const uint32_t hours = seconds / 3600U;
  seconds %= 3600U;
  const uint32_t minutes = seconds / 60U;
  seconds %= 60U;

  if (hours > 0) {
    snprintf(out, out_size, "%lu:%02lu:%02lu",
             static_cast<unsigned long>(hours),
             static_cast<unsigned long>(minutes),
             static_cast<unsigned long>(seconds));
  } else {
    snprintf(out, out_size, "%lu:%02lu", static_cast<unsigned long>(minutes),
             static_cast<unsigned long>(seconds));
  }
}

void format_duration_text(char *out, size_t out_size,
                                 uint32_t duration_ms) {
  if (duration_ms == 0) {
    snprintf(out, out_size, "--:--");
    return;
  }

  format_time_ms(out, out_size, duration_ms);
}

void style_list_button(lv_obj_t *button) {
  lv_obj_set_height(button, 38);
  lv_obj_set_style_bg_color(button, lv_color_hex(0x000000), 0);
  lv_obj_set_style_bg_color(button, lv_color_hex(0x111A12), LV_STATE_PRESSED);
  lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(button, 0, 0);
  lv_obj_set_style_radius(button, 0, 0);
  lv_obj_set_style_margin_bottom(button, 1, 0);
  lv_obj_set_style_text_color(button, lv_color_hex(0x69FF7A), 0);

  const uint32_t child_count = lv_obj_get_child_count(button);
  for (uint32_t i = 0; i < child_count; i++) {
    lv_obj_t *child = lv_obj_get_child(button, i);
    lv_obj_set_style_text_color(child, lv_color_hex(0x69FF7A), 0);
  }
}

static void scope_put_px(uint16_t *buffer, int32_t width, int32_t height,
                         int32_t x, int32_t y, uint16_t color) {
  if ((buffer == nullptr) || (x < 0) || (y < 0) || (x >= width) ||
      (y >= height)) {
    return;
  }

  buffer[(y * width) + x] = color;
}

void scope_draw_line(uint16_t *buffer, int32_t width, int32_t height,
                            int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                            uint16_t color) {
  int32_t dx = abs(x1 - x0);
  int32_t sx = x0 < x1 ? 1 : -1;
  int32_t dy = -abs(y1 - y0);
  int32_t sy = y0 < y1 ? 1 : -1;
  int32_t err = dx + dy;

  while (true) {
    scope_put_px(buffer, width, height, x0, y0, color);
    if ((x0 == x1) && (y0 == y1)) {
      break;
    }

    const int32_t e2 = err * 2;
    if (e2 >= dy) {
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx) {
      err += dx;
      y0 += sy;
    }
  }
}

QueueHandle_t event_queue_from_handle(void *handle) {
  return static_cast<QueueHandle_t>(handle);
}

SemaphoreHandle_t semaphore_from_handle(void *handle) {
  return static_cast<SemaphoreHandle_t>(handle);
}

i2s_mclk_multiple_t mclk_multiple_for_bits(uint32_t bits_per_sample) {
  return bits_per_sample == 24 ? I2S_MCLK_MULTIPLE_384 : I2S_MCLK_MULTIPLE_256;
}

i2s_data_bit_width_t i2s_bit_width(uint32_t bits_per_sample) {
  switch (bits_per_sample) {
  case 8:
    return I2S_DATA_BIT_WIDTH_8BIT;
  case 24:
    return I2S_DATA_BIT_WIDTH_24BIT;
  case 32:
    return I2S_DATA_BIT_WIDTH_32BIT;
  case 16:
  default:
    return I2S_DATA_BIT_WIDTH_16BIT;
  }
}

} // namespace music_player_internal

