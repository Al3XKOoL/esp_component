from esphome import automation
from esphome.components import sensor, switch
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_NAME

# Definir el nombre del componente
DEPENDENCIES = ['sensor', 'switch']

# Registrar el componente en el código generado
climate_solar_ns = cg.esphome_ns.namespace('climate_solar')
ClimateSolar = climate_solar_ns.class_('ClimateSolar', cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ClimateSolar),
    cv.Required(CONF_NAME): cv.string,
    cv.Required('temp_sun'): cv.use_id(sensor.Sensor),
    cv.Required('temp_spa'): cv.use_id(sensor.Sensor),
    cv.Required('temp_hot'): cv.use_id(sensor.Sensor),
    cv.Required('pump_switch'): cv.use_id(switch.Switch),
    cv.Optional('temp_diff_on', default=1.4): cv.float_,
    cv.Optional('max_temp', default=40): cv.float_,
    cv.Optional('temp_diff_off', default=0.7): cv.float_,
}).extend(cv.COMPONENT_SCHEMA)


# Función para configurar el componente
def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_temp_sun(config['temp_sun']))
    cg.add(var.set_temp_spa(config['temp_spa']))
    cg.add(var.set_temp_hot(config['temp_hot']))
    cg.add(var.set_pump_switch(config['pump_switch']))
    cg.add(var.set_temp_diff_on(config['temp_diff_on']))
    cg.add(var.set_max_temp(config['max_temp']))
    cg.add(var.set_temp_diff_off(config['temp_diff_off']))
    cg.add(var.set_name(config[CONF_NAME]))
    cg.register_component(var, config)
