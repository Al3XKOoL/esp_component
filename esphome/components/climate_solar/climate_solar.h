#include "esphome.h"

namespace esphome {
  namespace climate_solar {

    class ClimateSolar : public esphome::Component, public esphome::Climate {
     public:
      void setup() override;
      void control(const esphome::climate::ClimateCall &call) override;
      esphome::climate::ClimateTraits traits() override;
      void loop() override;

      // Variables configurables desde YAML
      esphome::sensor::Sensor *temp_sun_;
      esphome::sensor::Sensor *temp_watter_;
      esphome::sensor::Sensor *temp_output_;
      esphome::switch_::Switch *pump_switch_;
      float temp_max_;
      float diff_high_;
      float diff_mid_;
      float visual_min_temp_;
      float visual_max_temp_;
      float pump_power_;

     protected:
      bool bomba_activa;
      uint32_t cycle_start_time_;
      std::vector<uint32_t> last_cycle_times_;
      uint32_t daily_active_time_;
      uint32_t last_reset_time_;
    };

  }  // namespace climate_solar
}  // namespace esphome
