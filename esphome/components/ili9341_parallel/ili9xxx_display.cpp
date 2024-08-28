#include "ili9xxx_display.h"
#include "ili9xxx_init.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace ili9xxx {

static const char *const TAG = "ili9341";

void ILI9341ParallelDisplay::setup() {
  ESP_LOGD(TAG, "Iniciando configuración...");
  
  // Establecer las dimensiones por defecto
  this->set_dimensions(240, 320);
  
  // Inicializar todos los pines
  this->init_pins_();
  
  // Realizar un reset por hardware
  this->hard_reset_();
  
  // Inicializar la pantalla
  this->init_lcd_();
  
  ESP_LOGD(TAG, "LCD inicializado");
  
  // Configurar la orientación de la pantalla
  this->set_rotation(this->rotation_);
  
  // Llenar la pantalla con color rojo para probar
  this->fill(Color(255, 0, 0));  // Rojo
  
  ESP_LOGD(TAG, "Pantalla llenada con rojo");
  
  // Dibujar un rectángulo verde en el centro
  int width = this->get_width_internal();
  int height = this->get_height_internal();
  this->fill_rect(width/4, height/4, width/2, height/2, Color(0, 255, 0));  // Verde
  
  ESP_LOGD(TAG, "Rectángulo verde dibujado en el centro");
  
  // Dibujar una línea azul diagonal
  this->draw_line(0, 0, width-1, height-1, Color(0, 0, 255));  // Azul
  
  ESP_LOGD(TAG, "Línea azul diagonal dibujada");
  
  ESP_LOGD(TAG, "Configuración completada");
}

void ILI9341ParallelDisplay::init_pins_() {
  this->dc_pin_->setup();
  this->dc_pin_->digital_write(true);
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();
    this->reset_pin_->digital_write(true);
  }
  this->wr_pin_->setup();
  this->wr_pin_->digital_write(true);
  this->rd_pin_->setup();
  this->rd_pin_->digital_write(true);
  if (this->cs_pin_ != nullptr) {
    this->cs_pin_->setup();
    this->cs_pin_->digital_write(true);
  }
  for (int i = 0; i < 8; i++) {
    this->data_pins_[i]->setup();
    this->data_pins_[i]->digital_write(false);
  }
}

void ILI9341ParallelDisplay::hard_reset_() {
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->digital_write(true);
    delay(1);
    this->reset_pin_->digital_write(false);
    delay(10);
    this->reset_pin_->digital_write(true);
    delay(120);
  }
}

void ILI9341ParallelDisplay::set_rotation(uint8_t rotation) {
  ESP_LOGD(TAG, "Configurando rotación: %d", rotation);
  uint8_t madctl = 0;
  switch (rotation) {
    case 0:
      madctl = MADCTL_MX | MADCTL_BGR;
      break;
    case 1:
      madctl = MADCTL_MV | MADCTL_BGR;
      break;
    case 2:
      madctl = MADCTL_MY | MADCTL_BGR;
      break;
    case 3:
      madctl = MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR;
      break;
  }
  this->send_command_(ILI9XXX_MADCTL);
  this->send_data_(madctl);
  this->rotation_ = rotation;
}

void ILI9341ParallelDisplay::fill_rect(int x1, int y1, int width, int height, Color color) {
  for (int y = y1; y < y1 + height; y++) {
    for (int x = x1; x < x1 + width; x++) {
      this->draw_absolute_pixel_internal(x, y, color);
    }
  }
}

void ILI9341ParallelDisplay::draw_line(int x1, int y1, int x2, int y2, Color color) {
  int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
  int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
  int err = dx + dy, e2;

  while (true) {
    this->draw_absolute_pixel_internal(x1, y1, color);
    if (x1 == x2 && y1 == y2) break;
    e2 = 2 * err;
    if (e2 >= dy) { err += dy; x1 += sx; }
    if (e2 <= dx) { err += dx; y1 += sy; }
  }
}

void ILI9341ParallelDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "ILI9341 Parallel Display:");
  ESP_LOGCONFIG(TAG, "  Model: %s", this->model_.c_str());
  LOG_PIN("  DC Pin: ", this->dc_pin_);
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  LOG_PIN("  WR Pin: ", this->wr_pin_);
  LOG_PIN("  RD Pin: ", this->rd_pin_);
  LOG_PIN("  CS Pin: ", this->cs_pin_);
  ESP_LOGCONFIG(TAG, "  Data Pins: D0-D7");
  ESP_LOGCONFIG(TAG, "  Width: %d, Height: %d", this->width_, this->height_);
}

void ILI9341ParallelDisplay::update() {
  ESP_LOGD(TAG, "Iniciando actualización de pantalla");
  uint32_t start_time = millis();
  this->do_update_();
  uint32_t end_time = millis();
  ESP_LOGD(TAG, "Actualización completada en %u ms", end_time - start_time);
}

void ILI9341ParallelDisplay::fill(Color color) {
  ESP_LOGD(TAG, "Llenando pantalla con color: %d, %d, %d", color.r, color.g, color.b);
  this->set_addr_window_(0, 0, this->get_width_internal() - 1, this->get_height_internal() - 1);
  this->send_command_(ILI9XXX_RAMWR);
  for (uint32_t i = 0; i < this->get_width_internal() * this->get_height_internal(); i++) {
    this->write_color_(color);
  }
}

void ILI9341ParallelDisplay::draw_absolute_pixel_internal(int x, int y, Color color) {
  this->set_addr_window_(x, y, x, y);
  this->send_command_(ILI9XXX_RAMWR);
  this->write_color_(color);
}

void ILI9341ParallelDisplay::set_addr_window_(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  this->send_command_(ILI9XXX_CASET);  // Column addr set
  this->send_data_(x1 >> 8);
  this->send_data_(x1 & 0xFF);  // XSTART
  this->send_data_(x2 >> 8);
  this->send_data_(x2 & 0xFF);  // XEND
  this->send_command_(ILI9XXX_PASET);  // Row addr set
  this->send_data_(y1 >> 8);
  this->send_data_(y1 & 0xFF);  // YSTART
  this->send_data_(y2 >> 8);
  this->send_data_(y2 & 0xFF);  // YEND
}

void ILI9341ParallelDisplay::write_color_(Color color) {
  uint16_t color565 = ((color.r & 0xF8) << 8) | ((color.g & 0xFC) << 3) | (color.b >> 3);
  this->send_data_(color565 >> 8);
  this->send_data_(color565 & 0xFF);
}

void ILI9341ParallelDisplay::init_lcd_() {
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->digital_write(true);
    delay(1);
    this->reset_pin_->digital_write(false);
    delay(10);
    this->reset_pin_->digital_write(true);
    delay(120);
  }

  const uint8_t *addr = ILI9341_INIT_CMD;
  uint8_t cmd, num_args;
  while ((cmd = pgm_read_byte(addr++)) > 0) {
    num_args = pgm_read_byte(addr++);
    this->send_command_(cmd);
    for (uint8_t x = 0; x < num_args; x++) {
      this->send_data_(pgm_read_byte(addr++));
    }
    if (cmd == 0x01 || cmd == 0x11 || cmd == 0x29)
      delay(120);
  }
}

void ILI9341ParallelDisplay::send_command_(uint8_t cmd) {
  this->dc_pin_->digital_write(false);
  this->write_byte_(cmd);
}

void ILI9341ParallelDisplay::send_data_(uint8_t data) {
  this->dc_pin_->digital_write(true);
  this->write_byte_(data);
}

void ILI9341ParallelDisplay::write_byte_(uint8_t value) {
  for (uint8_t i = 0; i < 8; i++) {
    this->data_pins_[i]->digital_write((value >> i) & 0x01);
  }
  this->wr_pin_->digital_write(false);
  this->wr_pin_->digital_write(true);
}

void ILI9341ParallelDisplay::set_data_pins(GPIOPin *d0, GPIOPin *d1, GPIOPin *d2, GPIOPin *d3,
                                           GPIOPin *d4, GPIOPin *d5, GPIOPin *d6, GPIOPin *d7) {
  this->data_pins_[0] = d0;
  this->data_pins_[1] = d1;
  this->data_pins_[2] = d2;
  this->data_pins_[3] = d3;
  this->data_pins_[4] = d4;
  this->data_pins_[5] = d5;
  this->data_pins_[6] = d6;
  this->data_pins_[7] = d7;
}

void ILI9341ParallelDisplay::set_data_pin(uint8_t index, GPIOPin *pin) {
  if (index < 8) {
    this->data_pins_[index] = pin;
  }
}

}  // namespace ili9xxx
}  // namespace esphome
