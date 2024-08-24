// ili9xxx_display.cpp
#include "ili9xxx_display.h"
#include "esphome/core/application.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ili9xxx {

void ILI9341Display::setup() {
  // Inicialización de pines
  dc_pin_->setup();
  reset_pin_->setup();
  wr_pin_->setup();
  rd_pin_->setup();
  for (auto &pin : data_pins_) {
    pin->setup();
  }

  // Reset del display
  reset_pin_->digital_write(false);
  reset_pin_->digital_write(true);

  // Inicialización del display
  write_command(ILI9XXX_SWRESET);
  delay(150);
  write_command(ILI9XXX_SLPOUT);
  delay(150);

  for (uint8_t cmd : INITCMD_ILI9341) {
    if (cmd == 0x00) break; // Fin de la lista de comandos
    write_command(cmd);
  }
}

void ILI9341Display::dump_config() {
  ESP_LOGCONFIG(get_logger(), "ILI9341 Display:");
  ESP_LOGCONFIG(get_logger(), "  DC Pin: %s", dc_pin_->get_pin()->to_string().c_str());
  ESP_LOGCONFIG(get_logger(), "  Reset Pin: %s", reset_pin_->get_pin()->to_string().c_str());
  ESP_LOGCONFIG(get_logger(), "  WR Pin: %s", wr_pin_->get_pin()->to_string().c_str());
  ESP_LOGCONFIG(get_logger(), "  RD Pin: %s", rd_pin_->get_pin()->to_string().c_str());
  for (size_t i = 0; i < 8; i++) {
    ESP_LOGCONFIG(get_logger(), "  D%d Pin: %s", i, data_pins_[i]->get_pin()->to_string().c_str());
  }
}

void ILI9341Display::write_command(uint8_t command) {
  dc_pin_->digital_write(false); // Set DC pin to low for command
  wr_pin_->digital_write(false); // Set WR pin to low for write
  for (size_t i = 0; i < 8; i++) {
    data_pins_[i]->digital_write((command >> i) & 1);
  }
  wr_pin_->digital_write(true); // Pulse WR pin high to latch data
}

void ILI9341Display::write_data(uint8_t data) {
  dc_pin_->digital_write(true); // Set DC pin to high for data
  wr_pin_->digital_write(false); // Set WR pin to low for write
  for (size_t i = 0; i < 8; i++) {
    data_pins_[i]->digital_write((data >> i) & 1);
  }
  wr_pin_->digital_write(true); // Pulse WR pin high to latch data
}

void ILI9341Display::write_array(const uint8_t *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    write_data(data[i]);
  }
}

}  // namespace ili9xxx
}  // namespace esphome
