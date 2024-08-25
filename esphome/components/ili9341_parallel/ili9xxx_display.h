#pragma once

#include "esphome/core/hal.h"  // Incluido para GPIOPin
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/display/display_color_utils.h"
#include "ili9xxx_defines.h"
#include "ili9xxx_init.h"
#include <array>  // Incluido para std::array
#include <vector>  // Incluido para std::vector

namespace esphome {
namespace ili9xxx {

static const char *const TAG = "ili9xxx";
const size_t ILI9XXX_TRANSFER_BUFFER_SIZE = 126;  // Asegúrate de que este tamaño sea divisible por 6

enum ILI9XXXColorMode {
  BITS_8 = 0x08,
  BITS_8_INDEXED = 0x09,
  BITS_16 = 0x10,
};

enum PixelMode {
  PIXEL_MODE_UNSPECIFIED,
  PIXEL_MODE_16,
  PIXEL_MODE_18,
};

enum ILI9XXXInterfaceMode {
  PARALLEL_8BIT_MODE,
};

class ILI9XXXDisplay : public display::DisplayBuffer, public Component {
 public:
  ILI9XXXDisplay() = default;
  ILI9XXXDisplay(const uint8_t *init_sequence, int16_t width, int16_t height, bool invert_colors)
      : init_sequence_(init_sequence), width_(width), height_(height), pre_invertcolors_(invert_colors) {}

  void add_init_sequence(const std::vector<uint8_t> &sequence) { extra_init_sequence_ = sequence; }
  void set_dc_pin(GPIOPin *dc_pin) { dc_pin_ = dc_pin; }
  void set_reset_pin(GPIOPin *reset) { reset_pin_ = reset; }
  void set_palette(const uint8_t *palette) { palette_ = palette; }
  void set_buffer_color_mode(ILI9XXXColorMode color_mode) { buffer_color_mode_ = color_mode; }
  void set_dimensions(int16_t width, int16_t height) {
    width_ = width;
    height_ = height;
  }
  void set_offsets(int16_t offset_x, int16_t offset_y) {
    offset_x_ = offset_x;
    offset_y_ = offset_y;
  }
  void invert_colors(bool invert);
  virtual void command(uint8_t value) = 0;
  virtual void data(uint8_t value) = 0;
  void send_command(uint8_t command_byte, const uint8_t *data_bytes, uint8_t num_data_bytes);
  void set_color_order(display::ColorOrder color_order) { color_order_ = color_order; }
  void set_swap_xy(bool swap_xy) { swap_xy_ = swap_xy; }
  void set_mirror_x(bool mirror_x) { mirror_x_ = mirror_x; }
  void set_mirror_y(bool mirror_y) { mirror_y_ = mirror_y; }
  void set_pixel_mode(PixelMode mode) { pixel_mode_ = mode; }
  void set_interface_mode(ILI9XXXInterfaceMode mode) { interface_mode_ = mode; }
  void set_wr_pin(GPIOPin *wr_pin) { wr_pin_ = wr_pin; }
  void set_rd_pin(GPIOPin *rd_pin) { rd_pin_ = rd_pin; }
  void set_data_pins(const std::array<GPIOPin*, 8> &data_pins) { data_pins_ = data_pins; }

  void update() override;
  void fill(Color color) override;
  void dump_config() override;
  void setup() override;
  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_COLOR; }
  void draw_pixels_at(int x_start, int y_start, int w, int h, const uint8_t *ptr, display::ColorOrder order,
                      display::ColorBitness bitness, bool big_endian, int x_offset, int y_offset, int x_pad) override;

 protected:
  inline bool check_buffer_() {
    if (buffer_ == nullptr) {
      alloc_buffer_();
      return !Component::is_failed();
    }
    return true;
  }

  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  void setup_pins_();
  virtual void set_madctl();
  void display_();
  void init_lcd_(const uint8_t *addr);
  void set_addr_window_(uint16_t x, uint16_t y, uint16_t x2, uint16_t y2);
  void reset_();

  uint8_t const *init_sequence_;
  std::vector<uint8_t> extra_init_sequence_;
  int16_t width_{0};   ///< Display width as modified by current rotation
  int16_t height_{0};  ///< Display height as modified by current rotation
  int16_t offset_x_{0};
  int16_t offset_y_{0};
  uint16_t x_low_{0};
  uint16_t y_low_{0};
  uint16_t x_high_{0};
  uint16_t y_high_{0};
  const uint8_t *palette_{nullptr};

  ILI9XXXColorMode buffer_color_mode_{BITS_16};
  PixelMode pixel_mode_{PIXEL_MODE_UNSPECIFIED};
  ILI9XXXInterfaceMode interface_mode_{PARALLEL_8BIT_MODE};

  uint32_t get_buffer_length_();
  int get_width_internal() override;
  int get_height_internal() override;

  void start_command_();
  void end_command_();
  void start_data_();
  void end_data_();
  void alloc_buffer_();

  GPIOPin *reset_pin_{nullptr};
  GPIOPin *dc_pin_{nullptr};
  GPIOPin *busy_pin_{nullptr};
  GPIOPin *wr_pin_{nullptr};
  GPIOPin *rd_pin_{nullptr};
  std::array<GPIOPin*, 8> data_pins_{};

  bool processing_update_ = false;
  bool need_update_ = false;
  bool is_18bitdisplay_ = false;
  bool pre_invertcolors_ = false;
  display::ColorOrder color_order_{display::COLOR_ORDER_BGR};
  bool swap_xy_ = false;
  bool mirror_x_ = false;
  bool mirror_y_ = false;
};

// Clases derivadas para displays específicos
class ILI9XXXM5Stack : public ILI9XXXDisplay {
 public:
  ILI9XXXM5Stack() : ILI9XXXDisplay(INITCMD_M5STACK, 320, 240, true) {}
};

class ILI9XXXM5CORE : public ILI9XXXDisplay {
 public:
  ILI9XXXM5CORE() : ILI9XXXDisplay(INITCMD_M5CORE, 320, 240, true) {}
};

class ILI9XXXST7789V : public ILI9XXXDisplay {
 public:
  ILI9XXXST7789V() : ILI9XXXDisplay(INITCMD_ST7789V, 240, 320, false) {}
};

class ILI9XXXILI9341 : public ILI9XXXDisplay {
 public:
  ILI9XXXILI9341() : ILI9XXXDisplay(INITCMD_ILI9341, 240, 320, false) {}
};

class ILI9XXXILI9342 : public ILI9XXXDisplay {
 public:
  ILI9XXXILI9342() : ILI9XXXDisplay(INITCMD_ILI9341, 320, 240, false) {}
};

class ILI9XXXILI9481 : public ILI9XXXDisplay {
 public:
  ILI9XXXILI9481() : ILI9XXXDisplay(INITCMD_ILI9481, 480, 320, false) {}
};

class ILI9XXXILI948118 : public ILI9XXXDisplay {
 public:
  ILI9XXXILI948118() : ILI9XXXDisplay(INITCMD_ILI9481_18, 320, 480, true) {}
};

class ILI9XXXILI9486 : public ILI9XXXDisplay {
 public:
  ILI9XXXILI9486() : ILI9XXXDisplay(INITCMD_ILI9486, 480, 320, false) {}
};

class ILI9XXXILI9488 : public ILI9XXXDisplay {
 public:
  ILI9XXXILI9488(const uint8_t *seq = INITCMD_ILI9488) : ILI9XXXDisplay(seq, 480, 320, true) {}

 protected:
  void set_madctl() override {
    uint8_t mad = (color_order_ == display::COLOR_ORDER_BGR) ? MADCTL_BGR : MADCTL_RGB;
    uint8_t dfun = 0x22;
    width_ = 320;
    height_ = 480;
    if (swap_xy_ && mirror_x_ && mirror_y_) {
      dfun = 0x42;  // Rotate 180 degrees
    } else if (swap_xy_) {
      width_ = 480;
      height_ = 320;
      mad |= 0x20;
      dfun = (mirror_x_) ? 0x02 : 0x62;
    }
    command(ILI9XXX_DFUNCTR);
    data(0);
    data(dfun);
    command(ILI9XXX_MADCTL);
    data(mad);
  }
};

class WAVESHARERES35 : public ILI9XXXILI9488 {
 public:
  WAVESHARERES35() : ILI9XXXILI9488(INITCMD_WAVESHARE_RES_3_5) {}
  void data(uint8_t value) override {
    start_data_();
    write_byte(0);
    write_byte(value);
    end_data_();
  }
};

class ILI9XXXILI9488A : public ILI9XXXDisplay {
 public:
  ILI9XXXILI9488A() : ILI9XXXDisplay(INITCMD_ILI9488_A, 480, 320, true) {}
};

class ILI9XXXST7796 : public ILI9XXXDisplay {
 public:
  ILI9XXXST7796() : ILI9XXXDisplay(INITCMD_ST7796, 320, 480, false) {}
};

class ILI9XXXS3Box : public ILI9XXXDisplay {
 public:
  ILI9XXXS3Box() : ILI9XXXDisplay(INITCMD_S3BOX, 320, 240, false) {}
};

class ILI9XXXS3BoxLite : public ILI9XXXDisplay {
 public:
  ILI9XXXS3BoxLite() : ILI9XXXDisplay(INITCMD_S3BOXLITE, 320, 240, true) {}
};

class ILI9XXXGC9A01A : public ILI9XXXDisplay {
 public:
  ILI9XXXGC9A01A() : ILI9XXXDisplay(INITCMD_GC9A01A, 240, 240, true) {}
};

class ILI9XXXST7735 : public ILI9XXXDisplay {
 public:
  ILI9XXXST7735() : ILI9XXXDisplay(INITCMD_ST7735, 128, 160, false) {}
};

class ILI9XXXILI9341Parallel : public ILI9XXXDisplay {
 public:
  ILI9XXXILI9341Parallel() : ILI9XXXDisplay(INITCMD_ILI9341, 240, 320, false) {
    set_interface_mode(PARALLEL_8BIT_MODE);
  }
};

}  // namespace ili9xxx
}  // namespace esphome
