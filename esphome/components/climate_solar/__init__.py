import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_NAME

DEPENDENCIES = ['sensor', 'switch']

climate_solar_ns = cg.esphome_ns.namespace('climate_solar')
ClimateSolar = climate_solar_ns.class_('ClimateSolar', cg.Component)

CONF_TEMP_SUN = 'temp_sun'
CONF_TEMP_SPA = 'temp_spa'
CONF_TEMP_HOT = 'temp_hot'
CONF_PUMP_SWITCH = 'pump_switch'
CONF_TEMP_DIFF_ON = 'temp_diff_on'
CONF_TEMP_DIFF_OFF = 'temp_diff_off'
CONF_MAX_TEMP = 'max_temp'
CONF_VISUAL_MIN_TEMP = 'visual_min_temp'
CONF_VISUAL_MAX_TEMP = 'visual_max_temp'
CONF_PUMP_POWER = 'pump_power'

CONFIG_SCHEMA = cv.Schema({
    cv.Required(CONF_ID): cv.declare_id(ClimateSolar),
    cv.Required(CONF_TEMP_SUN): cv.use_id(sensor.Sensor),
    cv.Required(CONF_TEMP_SPA): cv.use_id(sensor.Sensor),
    cv.Required(CONF_TEMP_HOT): cv.use_id(sensor.Sensor),
    cv.Required(CONF_PUMP_SWITCH): cv.use_id(switch.Switch),
    cv.Optional(CONF_TEMP_DIFF_ON, default=1.4): cv.float_,
    cv.Optional(CONF_TEMP_DIFF_OFF, default=0.7): cv.float_,
    cv.Optional(CONF_MAX_TEMP, default=40.0): cv.float_,
    cv.Optional(CONF_VISUAL_MIN_TEMP, default=25.0): cv.float_,
    cv.Optional(CONF_VISUAL_MAX_TEMP, default=40.0): cv.float_,
    cv.Optional(CONF_PUMP_POWER, default=170.0): cv.float_,
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_temp_sun(config[CONF_TEMP_SUN]))
    cg.add(var.set_temp_spa(config[CONF_TEMP_SPA]))
    cg.add(var.set_temp_hot(config[CONF_TEMP_HOT]))
    cg.add(var.set_pump_switch(config[CONF_PUMP_SWITCH]))
    cg.add(var.set_temp_diff_on(config[CONF_TEMP_DIFF_ON]))
    cg.add(var.set_temp_diff_off(config[CONF_TEMP_DIFF_OFF]))
    cg.add(var.set_max_temp(config[CONF_MAX_TEMP]))
    cg.add(var.set_visual_min_temp(config[CONF_VISUAL_MIN_TEMP]))
    cg.add(var.set_visual_max_temp(config[CONF_VISUAL_MAX_TEMP]))
    cg.add(var.set_pump_power(config[CONF_PUMP_POWER]))
    cg.register_component(var, config)
