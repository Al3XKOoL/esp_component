#include "ili9xxx_display.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace ili9xxx {

static const char *const TAG = "ili9341_parallel";

void ILI9341ParallelDisplay::setup() {
  ESP_LOGD(TAG, "Setting up ILI9341 Parallel Display...");
  this->setup_pins_();
  this->init_lcd_();
  
  this->width_ = 240;
  this->height_ = 320;
  this->set_rotation(this->rotation_);
}

void ILI9341ParallelDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "ILI9341 Parallel Display:");
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  LOG_PIN("  CS Pin: ", this->cs_pin_);
  LOG_PIN("  DC Pin: ", this->dc_pin_);
  LOG_PIN("  WR Pin: ", this->wr_pin_);
  LOG_PIN("  RD Pin: ", this->rd_pin_);
  ESP_LOGCONFIG(TAG, "  Data Pins: D0-D7");
  LOG_DISPLAY_ROTATION(this);
  LOG_DISPLAY_DIMENSIONS(this);
}

float ILI9341ParallelDisplay::get_setup_priority() const { return setup_priority::PROCESSOR; }

void ILI9341ParallelDisplay::update() {
  this->do_update_();
  this->write_display_data_();
}

void ILI9341ParallelDisplay::display() {
  this->write_display_data_();
}

void ILI9341ParallelDisplay::draw_absolute_pixel_internal(int x, int y, Color color) {
  if (x >= this->get_width_internal() || x < 0 || y >= this->get_height_internal() || y < 0)
    return;

  uint16_t rgb565 = color.to_rgb_565();

  this->command(ILI9XXX_CASET);
  this->data(x >> 8);
  this->data(x);
  this->data(x >> 8);
  this->data(x);

  this->command(ILI9XXX_PASET);
  this->data(y >> 8);
  this->data(y);
  this->data(y >> 8);
  this->data(y);

  this->command(ILI9XXX_RAMWR);
  this->data(rgb565 >> 8);
  this->data(rgb565);
}

void ILI9341ParallelDisplay::setup_pins_() {
  for (int i = 0; i < 8; i++) {
    this->data_pins_[i]->setup();
    this->data_pins_[i]->digital_write(false);
  }
  this->dc_pin_->setup();
  this->dc_pin_->digital_write(false);
  this->wr_pin_->setup();
  this->wr_pin_->digital_write(true);
  this->rd_pin_->setup();
  this->rd_pin_->digital_write(true);
  if (this->cs_pin_ != nullptr) {
    this->cs_pin_->setup();
    this->cs_pin_->digital_write(true);
  }
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();
    this->reset_pin_->digital_write(true);
  }
}

void ILI9341ParallelDisplay::init_lcd_() {
  this->reset_();
  
  // Inicialización del LCD basada en la secuencia completa para modo paralelo MCU 8080-8 bits
  for (uint8_t i = 0; ILI9341_INIT_CMD[i] != 0; i += 2) {
    uint8_t cmd = ILI9341_INIT_CMD[i];
    uint8_t data_len = ILI9341_INIT_CMD[i + 1];
    if (data_len & 0x80) {
      delay(100);
    } else {
      this->command(cmd);
      for (uint8_t j = 0; j < data_len; j++) {
        this->data(ILI9341_INIT_CMD[i + 2 + j]);
      }
    }
    i += data_len;
  }
}

void ILI9341ParallelDisplay::command(uint8_t value) {
  this->dc_pin_->digital_write(false);
  this->write_byte(value);
}

void ILI9341ParallelDisplay::data(uint8_t value) {
  this->dc_pin_->digital_write(true);
  this->write_byte(value);
}

void ILI9341ParallelDisplay::send_command(uint8_t command_byte, const uint8_t *data_bytes, uint8_t num_data_bytes) {
  this->command(command_byte);
  for (uint8_t i = 0; i < num_data_bytes; i++) {
    this->data(data_bytes[i]);
  }
}

void ILI9341ParallelDisplay::reset_() {
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->digital_write(false);
    delay(200);
    this->reset_pin_->digital_write(true);
    delay(200);
  }
}

void ILI9341ParallelDisplay::write_byte(uint8_t value) {
  for (int i = 0; i < 8; i++) {
    this->data_pins_[i]->digital_write((value >> i) & 0x01);
  }
  this->wr_pin_->digital_write(false);
  this->wr_pin_->digital_write(true);
}

void ILI9341ParallelDisplay::write_array(const uint8_t *data, uint16_t length) {
  for (uint16_t i = 0; i < length; i++) {
    this->write_byte(data[i]);
  }
}

void ILI9341ParallelDisplay::write_display_data_() {
  uint16_t x1 = 0;
  uint16_t x2 = this->get_width_internal() - 1;
  uint16_t y1 = 0;
  uint16_t y2 = this->get_height_internal() - 1;

  if (this->cs_pin_ != nullptr) {
    this->cs_pin_->digital_write(false);  // Activar CS
  }

  // Set column address
  this->command(ILI9XXX_CASET);
  this->data(x1 >> 8);
  this->data(x1);
  this->data(x2 >> 8);
  this->data(x2);

  // Set page address
  this->command(ILI9XXX_PASET);
  this->data(y1 >> 8);
  this->data(y1);
  this->data(y2 >> 8);
  this->data(y2);

  // Write display data
  this->command(ILI9XXX_RAMWR);

  this->dc_pin_->digital_write(true);  // Data mode

  // Buffer para optimizar la escritura
  uint8_t buffer[BUFFER_SIZE * 2];
  uint16_t buffer_index = 0;

  for (int y = 0; y < this->get_height_internal(); y++) {
    for (int x = 0; x < this->get_width_internal(); x++) {
      Color color = this->get_pixel_color_(x, y);
      uint16_t rgb565 = color.to_rgb_565();
      buffer[buffer_index++] = rgb565 >> 8;
      buffer[buffer_index++] = rgb565;

      if (buffer_index == BUFFER_SIZE * 2) {
        this->write_array(buffer, BUFFER_SIZE * 2);
        buffer_index = 0;
      }
    }
  }

  // Escribir los datos restantes en el buffer
  if (buffer_index > 0) {
    this->write_array(buffer, buffer_index);
  }

  if (this->cs_pin_ != nullptr) {
    this->cs_pin_->digital_write(true);  // Desactivar CS
  }
}

uint8_t ILI9341ParallelDisplay::read_byte() {
  uint8_t value = 0;
  this->rd_pin_->digital_write(false);
  for (int i = 0; i < 8; i++) {
    value |= (this->data_pins_[i]->digital_read() << i);
  }
  this->rd_pin_->digital_write(true);
  return value;
}

uint32_t ILI9341ParallelDisplay::read_command(uint8_t command, uint8_t index) {
  uint32_t data = 0;
  this->command(command);
  this->dc_pin_->digital_write(true);  // Data mode
  // Descartar los primeros bytes según la documentación
  for (uint8_t i = 0; i < index; i++) {
    this->read_byte();
  }
  data = this->read_byte();
  data <<= 8;
  data |= this->read_byte();
  data <<= 8;
  data |= this->read_byte();
  data <<= 8;
  data |= this->read_byte();
  return data;
}

void ILI9341ParallelDisplay::read_display_identification_information() {
  uint32_t id = this->read_command(ILI9XXX_RDDID, 1);
  ESP_LOGI(TAG, "Display ID: 0x%08lX", id);
}

void ILI9341ParallelDisplay::set_rotation(display::DisplayRotation rotation) {
  this->rotation_ = rotation;
  switch (this->rotation_) {
    case display::DISPLAY_ROTATION_0_DEGREES:
      this->command(ILI9XXX_MADCTL);
      this->data(0x48);
      this->width_ = 240;
      this->height_ = 320;
      break;
    case display::DISPLAY_ROTATION_90_DEGREES:
      this->command(ILI9XXX_MADCTL);
      this->data(0x28);
      this->width_ = 320;
      this->height_ = 240;
      break;
    case display::DISPLAY_ROTATION_180_DEGREES:
      this->command(ILI9XXX_MADCTL);
      this->data(0x88);
      this->width_ = 240;
      this->height_ = 320;
      break;
    case display::DISPLAY_ROTATION_270_DEGREES:
      this->command(ILI9XXX_MADCTL);
      this->data(0xE8);
      this->width_ = 320;
      this->height_ = 240;
      break;
  }
}

}  // namespace ili9xxx
}  // namespace esphome
