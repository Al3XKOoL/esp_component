#include "ili9xxx_display.h"
#include "ili9xxx_defines.h"
#include "ili9xxx_init.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/display/display_color_utils.h"

namespace esphome {
namespace ili9xxx {

static const char *const TAG = "ili9xxx";

ILI9341ParallelDisplay::ILI9341ParallelDisplay() : display::DisplayBuffer() {
  // Inicialización si es necesaria
}

ILI9341ParallelDisplay::~ILI9341ParallelDisplay() {
  // Limpieza si es necesaria
}

void ILI9341ParallelDisplay::setup() {
  this->init(this->get_width_internal(), this->get_height_internal());
}

void ILI9341ParallelDisplay::update() {
  this->do_update_();
}

void ILI9341ParallelDisplay::do_update_() {
  // Implementa la lógica de actualización aquí
}

void ILI9341ParallelDisplay::init(int w, int h) {
  this->width_ = w;
  this->height_ = h;

  this->init_pins_();
  this->init_lcd_();
  this->set_rotation(this->rotation_);
}

void ILI9341ParallelDisplay::init_pins_() {
  ESP_LOGD(TAG, "Inicializando pines");
  
  for (int i = 0; i < 8; i++) {
    if (this->data_pins_[i] != nullptr) {
      this->data_pins_[i]->setup();
      ESP_LOGD(TAG, "Pin de datos %d configurado", i);
    }
  }
  
  if (this->dc_pin_ != nullptr) {
    this->dc_pin_->setup();
    ESP_LOGD(TAG, "DC pin configurado");
  }
  
  if (this->wr_pin_ != nullptr) {
    this->wr_pin_->setup();
    ESP_LOGD(TAG, "WR pin configurado");
  }
  
  if (this->rd_pin_ != nullptr) {
    this->rd_pin_->setup();
    ESP_LOGD(TAG, "RD pin configurado");
  }
  
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();
    ESP_LOGD(TAG, "Reset pin configurado");
  }
  
  if (this->cs_pin_ != nullptr) {
    this->cs_pin_->setup();
    ESP_LOGD(TAG, "CS pin configurado");
  }
  
  ESP_LOGD(TAG, "Inicialización de pines completada");
}

void ILI9341ParallelDisplay::init_lcd_() {
  ESP_LOGD(TAG, "Inicializando ILI9341 Parallel Display");
  
  this->hard_reset_();
  
  // Enviar secuencia de inicialización
  for (uint8_t i = 0; ILI9341_INIT_CMD[i] != 0; i += 2) {
    uint8_t cmd = pgm_read_byte(&ILI9341_INIT_CMD[i]);
    uint8_t num_args = pgm_read_byte(&ILI9341_INIT_CMD[i + 1]);
    if (num_args & 0x80) {
      delay(100);
    } else {
      this->write_command_(cmd);
      for (uint8_t j = 0; j < num_args; j++) {
        this->write_data_(pgm_read_byte(&ILI9341_INIT_CMD[i + 2 + j]));
      }
      i += num_args;
    }
  }
  
  ESP_LOGD(TAG, "ILI9341 Parallel Display inicializado correctamente");
}

void ILI9341ParallelDisplay::set_rotation(uint8_t rotation) {
  ESP_LOGD(TAG, "Estableciendo rotación: %d", rotation);
  
  if (this->dc_pin_ == nullptr || this->wr_pin_ == nullptr) {
    ESP_LOGE(TAG, "DC o WR pin no configurados. No se puede establecer la rotación.");
    return;
  }

  this->write_command_(ILI9XXX_MADCTL);
  switch (rotation % 4) {
    case 0:
      this->write_data_(MADCTL_MX | MADCTL_BGR);
      this->width_ = 240;
      this->height_ = 320;
      break;
    case 1:
      this->write_data_(MADCTL_MV | MADCTL_BGR);
      this->width_ = 320;
      this->height_ = 240;
      break;
    case 2:
      this->write_data_(MADCTL_MY | MADCTL_BGR);
      this->width_ = 240;
      this->height_ = 320;
      break;
    case 3:
      this->write_data_(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
      this->width_ = 320;
      this->height_ = 240;
      break;
  }
  
  ESP_LOGD(TAG, "Rotación establecida, ancho: %d, alto: %d", this->width_, this->height_);
}

void ILI9341ParallelDisplay::write_command_(uint8_t cmd) {
  this->dc_pin_->digital_write(false);
  this->write_byte_(cmd);
  this->dc_pin_->digital_write(true);
}

void ILI9341ParallelDisplay::write_data_(uint8_t data) {
  this->write_byte_(data);
}

void ILI9341ParallelDisplay::write_byte_(uint8_t data) {
  for (int i = 0; i < 8; i++) {
    this->data_pins_[i]->digital_write((data >> i) & 0x01);
  }
  this->wr_pin_->digital_write(false);
  this->wr_pin_->digital_write(true);
}

void ILI9341ParallelDisplay::write_color_(Color color) {
  uint8_t hi = color.red << 3 | color.green >> 3;
  uint8_t lo = (color.green & 0x07) << 5 | color.blue;
  this->write_data_(hi);
  this->write_data_(lo);
}

void ILI9341ParallelDisplay::set_addr_window_(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  this->write_command_(ILI9XXX_CASET);
  this->write_data_(x1 >> 8);
  this->write_data_(x1 & 0xFF);
  this->write_data_(x2 >> 8);
  this->write_data_(x2 & 0xFF);
  this->write_command_(ILI9XXX_PASET);
  this->write_data_(y1 >> 8);
  this->write_data_(y1 & 0xFF);
  this->write_data_(y2 >> 8);
  this->write_data_(y2 & 0xFF);
  this->write_command_(ILI9XXX_RAMWR);
}

void ILI9341ParallelDisplay::hard_reset_() {
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->digital_write(true);
    delay(10);
    this->reset_pin_->digital_write(false);
    delay(10);
    this->reset_pin_->digital_write(true);
    delay(10);
  }
}

void ILI9341ParallelDisplay::write_display_() {
  ESP_LOGD(TAG, "Iniciando write_display_");
  
  uint16_t buffer_width = this->get_width_internal();
  uint16_t buffer_height = this->get_height_internal();

  ESP_LOGD(TAG, "Configurando ventana de direcciones");
  this->set_addr_window_(0, 0, buffer_width - 1, buffer_height - 1);
  
  ESP_LOGD(TAG, "Enviando comando RAMWR");
  this->write_command_(ILI9XXX_RAMWR);

  uint16_t line_buffer[buffer_width];

  for (int y = 0; y < buffer_height; y++) {
    for (int x = 0; x < buffer_width; x++) {
      Color color = this->get_buffer_pixel_(x, y);
      uint16_t color565 = display::ColorUtil::color_to_565(color);
      line_buffer[x] = color565;
    }

    ESP_LOGV(TAG, "Escribiendo línea %d", y);
    for (int x = 0; x < buffer_width; x++) {
      this->write_data_(line_buffer[x] >> 8);
      this->write_data_(line_buffer[x] & 0xFF);
    }
  }

  ESP_LOGD(TAG, "write_display_ completado");
}

void ILI9341ParallelDisplay::fill(Color color) {
  ESP_LOGD(TAG, "Rellenando pantalla con color: %d, %d, %d", color.r, color.g, color.b);
  
  uint16_t color565 = display::ColorUtil::color_to_565(color);
  uint16_t buffer[32];
  for (int i = 0; i < 32; i++) {
    buffer[i] = color565;
  }

  this->set_addr_window_(0, 0, this->get_width_internal() - 1, this->get_height_internal() - 1);
  
  for (int i = 0; i < this->get_width_internal() * this->get_height_internal(); i += 32) {
    this->write_command_(ILI9XXX_RAMWR);
    for (int j = 0; j < 32; j++) {
      this->write_data_(buffer[j] >> 8);
      this->write_data_(buffer[j] & 0xFF);
    }
  }
}

void ILI9341ParallelDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "ILI9341 Parallel Display:");
  ESP_LOGCONFIG(TAG, "  Ancho: %d, Alto: %d", this->width_, this->height_);
  LOG_PIN("  DC Pin: ", this->dc_pin_);
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  LOG_PIN("  WR Pin: ", this->wr_pin_);
  LOG_PIN("  RD Pin: ", this->rd_pin_);
  LOG_PIN("  CS Pin: ", this->cs_pin_);
  for (int i = 0; i < 8; i++) {
    ESP_LOGCONFIG(TAG, "  Data Pin %d: %s", i, this->data_pins_[i] ? this->data_pins_[i]->dump_summary().c_str() : "None");
  }
}

Color ILI9341ParallelDisplay::get_pixel_color(int x, int y) {
  // Implementa la lógica para obtener el color del píxel en la posición (x, y)
  // Por ejemplo:
  return this->get_buffer_pixel_(x, y);
}

void ILI9341ParallelDisplay::draw_absolute_pixel_internal(int x, int y, Color color) {
  if (x >= this->get_width_internal() || x < 0 || y >= this->get_height_internal() || y < 0)
    return;

  this->set_addr_window_(x, y, x, y);
  this->write_color_(color);
}

}  // namespace ili9xxx
}  // namespace esphome
