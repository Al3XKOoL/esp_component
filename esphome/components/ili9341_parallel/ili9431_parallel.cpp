#include "ili9341_parallel.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ili9341_parallel {

static const char *const TAG = "ili9341_parallel";

void ILI9341ParallelDisplay::setup() {
  ESP_LOGD(TAG, "Setting up ILI9341 Parallel Display");
  setup_pins_();
}

void ILI9341ParallelDisplay::setup_pins_() {
  this->cs_pin_->setup();
  this->dc_pin_->setup();
  this->wr_pin_->setup();
  this->rd_pin_->setup();
  for (int i = 0; i < 8; ++i) {
    this->data_pins_[i]->setup();
  }
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();
    this->reset_pin_->digital_write(true);
  }
  this->reset_();
}

void ILI9341ParallelDisplay::command(uint8_t value) {
  this->start_command_();
  this->write_byte(value);
  this->end_command_();
}

void ILI9341ParallelDisplay::data(uint8_t value) {
  this->start_data_();
  this->write_byte(value);
  this->end_data_();
}

void ILI9341ParallelDisplay::send_command(uint8_t command_byte, const uint8_t *data_bytes, uint8_t num_data_bytes) {
  this->command(command_byte);
  for (uint8_t i = 0; i < num_data_bytes; ++i) {
    this->data(data_bytes[i]);
  }
}

void ILI9341ParallelDisplay::write_byte(uint8_t data) {
  for (int i = 0; i < 8; ++i) {
    this->data_pins_[i]->digital_write((data >> i) & 0x01);
  }
  this->wr_pin_->digital_write(false);
  this->wr_pin_->digital_write(true);
}

void ILI9341ParallelDisplay::write_array(const uint8_t *data, size_t length) {
  for (size_t i = 0; i < length; ++i) {
    write_byte(data[i]);
  }
}

void ILI9341ParallelDisplay::enable() {
  this->cs_pin_->digital_write(false);
}

void ILI9341ParallelDisplay::disable() {
  this->cs_pin_->digital_write(true);
}

void ILI9341ParallelDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "ILI9341 Parallel Display:");
  ESP_LOGCONFIG(TAG, "  CS Pin: %d", this->cs_pin_->get_pin());
  ESP_LOGCONFIG(TAG, "  DC Pin: %d", this->dc_pin_->get_pin());
  ESP_LOGCONFIG(TAG, "  Reset Pin: %d", this->reset_pin_ ? this->reset_pin_->get_pin() : -1);
  ESP_LOGCONFIG(TAG, "  WR Pin: %d", this->wr_pin_->get_pin());
  ESP_LOGCONFIG(TAG, "  RD Pin: %d", this->rd_pin_->get_pin());
  for (int i = 0; i < 8; ++i) {
    ESP_LOGCONFIG(TAG, "  Data Pin %d: %d", i, this->data_pins_[i]->get_pin());
  }
}

float ILI9341ParallelDisplay::get_setup_priority() const {
  return setup_priority::PROCESSOR;
}

void ILI9341ParallelDisplay::update() {
  this->do_update_();
  this->display_();
}

}  // namespace ili9341_parallel
}  // namespace esphome
