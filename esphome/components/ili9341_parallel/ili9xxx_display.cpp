#include "ili9xxx_display.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ili9xxx {

static const char *const TAG = "ili9341_parallel_display";

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
}

void ILI9341ParallelDisplay::init_pins_() {
  ESP_LOGD(TAG, "Inicializando pines");
  for (int i = 0; i < 8; i++) {
    if (this->data_pins_[i] != nullptr) {
      this->data_pins_[i]->setup();
      ESP_LOGD(TAG, "Pin de datos %d configurado", i);
    }
  }
  this->dc_pin_->setup();
  this->wr_pin_->setup();
  this->rd_pin_->setup();
  this->reset_pin_->setup();
  this->cs_pin_->setup();
}

void ILI9341ParallelDisplay::init_lcd_() {
  ESP_LOGD(TAG, "Inicializando LCD");
  this->reset_pin_->digital_write(0);
  delay(50);
  this->reset_pin_->digital_write(1);
  delay(150);

  // Enviar comandos de inicialización
  this->write_command_(ILI9XXX_SWRESET);
  delay(150);
  this->write_command_(ILI9XXX_DISPON);
  delay(150);
}

void ILI9341ParallelDisplay::write_display_() {
  // Implementa la lógica para escribir en la pantalla
}

Color ILI9341ParallelDisplay::get_pixel_color(int x, int y) {
  return this->get_buffer_pixel(x, y);
}

Color ILI9341ParallelDisplay::get_buffer_pixel(int x, int y) {
  if (x < 0 || x >= this->width_ || y < 0 || y >= this->height_)
    return Color::BLACK;

  size_t index = (y * this->width_ + x) * 2;  // Asumiendo RGB565
  uint16_t color565 = (this->buffer_[index] << 8) | this->buffer_[index + 1];

  // Conversión manual de RGB565 a Color
  uint8_t r = (color565 >> 11) & 0x1F;
  uint8_t g = (color565 >> 5) & 0x3F;
  uint8_t b = color565 & 0x1F;
  return Color(r << 3, g << 2, b << 3);  // Ajustar a 8 bits
}

void ILI9341ParallelDisplay::draw_absolute_pixel_internal(int x, int y, Color color) {
  if (x >= this->width_ || x < 0 || y >= this->height_ || y < 0)
    return;

  this->set_addr_window_(x, y, x, y);
  this->write_color_(color);
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
}

void ILI9341ParallelDisplay::write_command_(uint8_t cmd) {
  this->dc_pin_->digital_write(0);  // DC low para comando
  this->write_data_(cmd);
}

void ILI9341ParallelDisplay::write_data_(uint8_t data) {
  this->dc_pin_->digital_write(1);  // DC high para datos
  this->write_byte_(data);
}

void ILI9341ParallelDisplay::write_byte_(uint8_t data) {
  // Implementa la lógica para escribir un byte en paralelo
  for (int i = 0; i < 8; i++) {
    this->data_pins_[i]->digital_write((data >> i) & 0x01);
  }
  this->wr_pin_->digital_write(0);  // Pulso de escritura
  this->wr_pin_->digital_write(1);
}

void ILI9341ParallelDisplay::write_color_(Color color) {
  uint16_t color565 = display::ColorUtil::color_to_565(color);
  this->write_data_(color565 >> 8);
  this->write_data_(color565 & 0xFF);
}

void ILI9341ParallelDisplay::set_data_pin(int index, GPIOPin *pin) {
  if (index >= 0 && index < 8) {
    this->data_pins_[index] = pin;
  }
}

void ILI9341ParallelDisplay::set_dc_pin(GPIOPin *pin) {
  this->dc_pin_ = pin;
}

void ILI9341ParallelDisplay::set_wr_pin(GPIOPin *pin) {
  this->wr_pin_ = pin;
}

void ILI9341ParallelDisplay::set_rd_pin(GPIOPin *pin) {
  this->rd_pin_ = pin;
}

void ILI9341ParallelDisplay::set_reset_pin(GPIOPin *pin) {
  this->reset_pin_ = pin;
}

void ILI9341ParallelDisplay::set_cs_pin(GPIOPin *pin) {
  this->cs_pin_ = pin;
}

}  // namespace ili9xxx
}  // namespace esphome
