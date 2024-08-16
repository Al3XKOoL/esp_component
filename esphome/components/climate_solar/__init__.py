import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, sensor, switch
from esphome.const import CONF_ID, UNIT_CELSIUS, UNIT_PERCENT, ICON_THERMOMETER

AUTO_LOAD = ['sensor', 'climate', 'switch']

climate_solar_ns = cg.esphome_ns.namespace('climate_solar')
ClimateSolar = climate_solar_ns.class_('ClimateSolar', climate.Climate, cg.Component)

CONF_TEMP_SUN = 'temp_sun'
CONF_TEMP_WATTER = 'temp_watter'
CONF_TEMP_OUTPUT = 'temp_output'
CONF_PUMP_SWITCH = 'pump_switch'
CONF_TEMP_MAX = 'temp_max'
CONF_DIFF_HIGH = 'diff_high'
CONF_DIFF_MID = 'diff_mid'
CONF_VISUAL_MIN_TEMP = 'visual_min_temp'
CONF_VISUAL_MAX_TEMP = 'visual_max_temp'
CONF_PUMP_POWER = 'pump_power'

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ClimateSolar),
    cv.Required(CONF_TEMP_SUN): cv.use_id(sensor.Sensor),
    cv.Required(CONF_TEMP_WATTER): cv.use_id(sensor.Sensor),
    cv.Required(CONF_TEMP_OUTPUT): cv.use_id(sensor.Sensor),
    cv.Optional(CONF_PUMP_SWITCH): cv.use_id(switch.Switch),
    cv.Optional(CONF_TEMP_MAX, default=37.5): cv.temperature,
    cv.Optional(CONF_DIFF_HIGH, default=1.0): cv.float_,
    cv.Optional(CONF_DIFF_MID, default=1.0): cv.float_,
    cv.Optional(CONF_VISUAL_MIN_TEMP, default=25.0): cv.temperature,
    cv.Optional(CONF_VISUAL_MAX_TEMP, default=40.0): cv.temperature,
    cv.Optional(CONF_PUMP_POWER, default=100): cv.percentage,
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    # Obtener y asignar los sensores a las variables correspondientes
    temp_sun_sensor = yield cg.get_variable(config[CONF_TEMP_SUN])
    cg.add(var.set_temp_sun(temp_sun_sensor))

    temp_watter_sensor = yield cg.get_variable(config[CONF_TEMP_WATTER])
    cg.add(var.set_temp_watter(temp_watter_sensor))

    temp_output_sensor = yield cg.get_variable(config[CONF_TEMP_OUTPUT])
    cg.add(var.set_temp_output(temp_output_sensor))

    # Registrar el interruptor de la bomba si está presente
    if CONF_PUMP_SWITCH in config:
        pump_switch = yield cg.get_variable(config[CONF_PUMP_SWITCH])
        cg.add(var.set_pump_switch(pump_switch))

    # Configuración de parámetros adicionales
    cg.add(var.set_temp_max(config[CONF_TEMP_MAX]))
    cg.add(var.set_diff_high(config[CONF_DIFF_HIGH]))
    cg.add(var.set_diff_mid(config[CONF_DIFF_MID]))
    cg.add(var.set_visual_min_temp(config[CONF_VISUAL_MIN_TEMP]))
    cg.add(var.set_visual_max_temp(config[CONF_VISUAL_MAX_TEMP]))
    cg.add(var.set_pump_power(config[CONF_PUMP_POWER]))

    # Configuración final del componente Climate
    cg.add(var.setup())
    climate.register_climate(var, config)
