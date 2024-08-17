#include "custom_climate.h"
#include <cstdarg>
#include <cstdio>
#include "esphome/core/log.h"
#include "esphome/components/climate/climate.h"

namespace custom_climate {

using namespace esphome::climate;

static const char *const TAG = "custom_climate";

void CustomClimate::log_mensaje(const char* nivel, const char* formato, ...) {
    va_list args;
    va_start(args, formato);
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), formato, args);
    va_end(args);
    esphome::ESP_LOGD(TAG, "\033[1;31m%s: %s\033[0m", nivel, buffer);
}

void CustomClimate::setup() {
  log_mensaje("INFO", "Iniciando setup");
  this->mode = CLIMATE_MODE_OFF;
  this->target_temperature = 37.0;
  this->current_temperature = get_current_temperature();

  if (diferencia_media_number_ != nullptr) {
    diferencia_media_number_->add_on_state_callback([this](float value) {
      this->diferencia_media_ = value;
      log_mensaje("INFO", "Diferencia media actualizada a %.1f", value);
    });
  }

  if (diferencia_alta_number_ != nullptr) {
    diferencia_alta_number_->add_on_state_callback([this](float value) {
      this->diferencia_alta_ = value;
      log_mensaje("INFO", "Diferencia alta actualizada a %.1f", value);
    });
  }

  this->publish_state();
  log_mensaje("INFO", "Setup completado");
}

  diferencia_media_number_.set_name("Diferencia Media");
  diferencia_media_number_.set_min_value(0.1f);
  diferencia_media_number_.set_max_value(5.0f);
  diferencia_media_number_.set_step(0.1f);
  diferencia_media_number_.set_initial_value(diferencia_media_);
  diferencia_media_number_.add_on_state_callback([this](float value) {
    this->diferencia_media_ = value;
    log_mensaje("INFO", "Diferencia media actualizada a %.1f", value);
  });

  diferencia_alta_number_.set_name("Diferencia Alta");
  diferencia_alta_number_.set_min_value(0.1f);
  diferencia_alta_number_.set_max_value(5.0f);
  diferencia_alta_number_.set_step(0.1f);
  diferencia_alta_number_.set_initial_value(diferencia_alta_);
  diferencia_alta_number_.add_on_state_callback([this](float value) {
    this->diferencia_alta_ = value;
    log_mensaje("INFO", "Diferencia alta actualizada a %.1f", value);
  });

  conteo_encendidos_sensor_.set_unit_of_measurement("");
  tiempo_encendido_sensor_.set_unit_of_measurement("s");
  kwh_hoy_sensor_.set_unit_of_measurement("kWh");
  kwh_total_sensor_.set_unit_of_measurement("kWh");

  this->publish_state();
  log_mensaje("INFO", "Setup completado");
}

void CustomClimate::loop() {
  unsigned long tiempo_actual = millis();
  if (tiempo_actual - ultimo_tiempo_verificacion_ >= intervalo_segundos_ * 1000) {
    ultimo_tiempo_verificacion_ = tiempo_actual;
    log_mensaje("INFO", "Iniciando ciclo de loop");

    // Verificar sensores
    if (sensor_temp_sol_ == nullptr || std::isnan(sensor_temp_sol_->state) ||
        sensor_temp_agua_ == nullptr || std::isnan(sensor_temp_agua_->state) ||
        sensor_temp_salida_ == nullptr || std::isnan(sensor_temp_salida_->state)) {
      log_mensaje("ERROR", "Uno o más sensores no están disponibles o devuelven valores inválidos");
      return;
    }

    this->current_temperature = sensor_temp_agua_->state;

    log_mensaje("INFO", "Temperaturas - Sol: %.2f, Agua: %.2f, Salida: %.2f", 
                sensor_temp_sol_->state, sensor_temp_agua_->state, sensor_temp_salida_->state);

    if (this->mode == CLIMATE_MODE_HEAT) {
      log_mensaje("INFO", "Modo HEAT activo, iniciando control de bomba");
      bool control_exitoso = control_bomba();
      if (!control_exitoso) {
        log_mensaje("ERROR", "Error en control_bomba");
      }
    } else {
      log_mensaje("INFO", "Modo no es HEAT, verificando estado de la bomba");
      if (interruptor_bomba_->state) {
        log_mensaje("WARN", "Bomba encendida en modo no-HEAT, apagando");
        apagar_bomba();
      }
    }

    actualizar_consumo();
    reset_consumo_diario();

    this->publish_state();
    log_mensaje("INFO", "Ciclo de loop completado");
  }
}

bool CustomClimate::control_bomba() {
  log_mensaje("INFO", "Iniciando control de bomba");

  if (espera_) {
    int64_t tiempo_actual = obtener_tiempo_actual();
    if (tiempo_actual >= tiempo_espera_fin_) {
      log_mensaje("INFO", "Fin del tiempo de espera");
      espera_ = false;
    } else {
      log_mensaje("INFO", "En espera hasta %lld (actual: %lld)", tiempo_espera_fin_, tiempo_actual);
      return true;
    }
  }

  bool cerca_objetivo = modo_cerca_temperatura_objetivo();
  log_mensaje("INFO", "Cerca del objetivo: %s", cerca_objetivo ? "Sí" : "No");

  if (cerca_objetivo) {
    control_bomba_cerca_objetivo();
  } else {
    control_bomba_normal();
  }

  log_mensaje("INFO", "Control de bomba completado");
  return true;
}

bool CustomClimate::modo_cerca_temperatura_objetivo() {
  return (this->current_temperature >= (this->target_temperature - temperatura_cerca_));
}

void CustomClimate::control_bomba_cerca_objetivo() {
  log_mensaje("INFO", "Control de bomba cerca del objetivo");
  if (!interruptor_bomba_->state && diferencia_temperatura_suficiente()) {
    encender_bomba();
    esperar_estabilizacion();
    activar_espera_proporcional();
  } else if (interruptor_bomba_->state) {
    if (temperatura_alcanzada()) {
      apagar_bomba();
    }
  }
}

void CustomClimate::control_bomba_normal() {
  log_mensaje("INFO", "Control de bomba normal");
  if (!interruptor_bomba_->state && diferencia_temperatura_suficiente()) {
    encender_bomba();
    esperar_estabilizacion();
  } else if (interruptor_bomba_->state && !diferencia_temperatura_suficiente()) {
    apagar_bomba();
    activar_espera_fija();
  }
}

bool CustomClimate::diferencia_temperatura_suficiente() {
  return (sensor_temp_sol_->state > (this->current_temperature + diferencia_media_));
}

void CustomClimate::encender_bomba() {
  log_mensaje("WARN", "Encendiendo bomba");
  interruptor_bomba_->turn_on();
  conteo_encendidos_++;
  conteo_encendidos_sensor_.publish_state(conteo_encendidos_);
  tiempo_inicio_ = obtener_tiempo_actual();
}

void CustomClimate::apagar_bomba() {
  log_mensaje("WARN", "Apagando bomba");
  interruptor_bomba_->turn_off();
  int64_t tiempo_actual = obtener_tiempo_actual();
  tiempo_encendido_ += tiempo_actual - tiempo_inicio_;
  tiempo_encendido_sensor_.publish_state(tiempo_encendido_);
}

void CustomClimate::esperar_estabilizacion() {
  log_mensaje("WARN", "Esperando 15 segundos para estabilización");
  delay(15000);  // 15 segundos
}

void CustomClimate::activar_espera_proporcional() {
  float diferencia_temp = sensor_temp_sol_->state - this->current_temperature;
  int tiempo_activacion = static_cast<int>(diferencia_temp * factor_tiempo_activacion_);
  espera_ = true;
  tiempo_espera_fin_ = obtener_tiempo_actual() + tiempo_activacion;
  log_mensaje("WARN", "Activada espera proporcional por %d segundos", tiempo_activacion);
}

void CustomClimate::activar_espera_fija() {
  espera_ = true;
  tiempo_espera_fin_ = obtener_tiempo_actual() + 180;  // 3 minutos
  log_mensaje("WARN", "Activada espera fija por 3 minutos");
}

bool CustomClimate::temperatura_alcanzada() {
  return (this->current_temperature >= this->target_temperature);
}

int64_t CustomClimate::obtener_tiempo_actual() {
  if (tiempo_homeassistant_ != nullptr) {
    return tiempo_homeassistant_->now().timestamp;
  }
  if (tiempo_sntp_ != nullptr) {
    return tiempo_sntp_->now().timestamp;
  }
  return millis() / 1000;  // Fallback a millis si no hay fuente de tiempo real
}

esphome::climate::ClimateTraits CustomClimate::traits() {
  auto traits = esphome::climate::ClimateTraits();
  traits.set_supports_current_temperature(
