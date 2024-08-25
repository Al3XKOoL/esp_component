#include "ili9341_parallel.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ili9341_parallel {

static const char *const TAG = "ili9341_parallel";

void ILI9341ParallelDisplay::setup() {
  ESP_LOGD(TAG, "Setting up ILI9341 Parallel Display");
  this->setup_pins_();
  // Add initialization code here
}

void ILI9341ParallelDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "ILI9341 Parallel Display:");
  ESP_LOGCONFIG(TAG, "  Model: %s", this->model_.c_str());
  ESP_LOGCONFIG(TAG, "  Width: %d", this->width_);
  ESP_LOGCONFIG(TAG, "  Height: %d", this->height_);
  LOG_PIN("  DC Pin: ", this->dc_pin_);
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  LOG_PIN("  WR Pin: ", this->wr_pin_);
  LOG_PIN("  RD Pin: ", this->rd_pin_);
  for (int i = 0; i < 8; i++) {
    LOG_PIN("  Data Pin " + std::to_string(i) + ": ", this->data_pins_[i]);
  }
}

float ILI9341ParallelDisplay::get_setup_priority() const { return setup_priority::PROCESSOR; }

void ILI9341ParallelDisplay::update() {
  // Add update code here
}

void ILI9341ParallelDisplay::setup_pins_() {
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
  for (auto pin : this->data_pins_) {
    pin->setup();
    pin->digital_write(false);
  }
}

}  // namespace ili9341_parallel
}  // namespace esphome
