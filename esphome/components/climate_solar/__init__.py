import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, sensor
from esphome.const import CONF_ID, CONF_TEMPERATURE, UNIT_CELSIUS, ICON_THERMOMETER

AUTO_LOAD = ['sensor', 'climate']

climate_solar_ns = cg.esphome_ns.namespace('climate_solar')
ClimateSolar = climate_solar_ns.class_('ClimateSolar', climate.Climate, cg.Component)

CONF_TEMP_SUN = 'temp_sun'
CONF_TEMP_WATTER = 'temp_watter'
CONF_TEMP_OUTPUT = 'temp_output'
CONF_LAST_CYCLE_TIME_SENSOR = 'last_cycle_time_sensor'
CONF_DAILY_ACTIVE_TIME_SENSOR = 'daily_active_time_sensor'
CONF_DAILY_ENERGY_CONSUMPTION_SENSOR = 'daily_energy_consumption_sensor'
CONF_TEMP_MAX = 'temp_max'
CONF_DIFF_HIGH = 'diff_high'
CONF_DIFF_MID = 'diff_mid'
CONF_VISUAL_MIN_TEMP = 'visual_min_temp'
CONF_VISUAL_MAX_TEMP = 'visual_max_temp'
CONF_PUMP_POWER = 'pump_power'

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ClimateSolar),
    cv.Optional(CONF_TEMP_SUN): cv.use_id(sensor.Sensor),
    cv.Optional(CONF_TEMP_WATTER): cv.use_id(sensor.Sensor),
    cv.Optional(CONF_TEMP_OUTPUT): cv.use_id(sensor.Sensor),
    cv.Optional(CONF_LAST_CYCLE_TIME_SENSOR): cv.use_id(sensor.Sensor),
    cv.Optional(CONF_DAILY_ACTIVE_TIME_SENSOR): cv.use_id(sensor.Sensor),
    cv.Optional(CONF_DAILY_ENERGY_CONSUMPTION_SENSOR): cv.use_id(sensor.Sensor),
    cv.Optional(CONF_TEMP_MAX): cv.float_,
    cv.Optional(CONF_DIFF_HIGH): cv.float_,
    cv.Optional(CONF_DIFF_MID): cv.float_,
    cv.Optional(CONF_VISUAL_MIN_TEMP): cv.float_,
    cv.Optional(CONF_VISUAL_MAX_TEMP): cv.float_,
    cv.Optional(CONF_PUMP_POWER): cv.float_,
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    if CONF_TEMP_SUN in config:
        cg.add(var.set_temp_sun(config[CONF_TEMP_SUN]))
    if CONF_TEMP_WATTER in config:
        cg.add(var.set_temp_watter(config[CONF_TEMP_WATTER]))
    if CONF_TEMP_OUTPUT in config:
        cg.add(var.set_temp_output(config[CONF_TEMP_OUTPUT]))
    if CONF_LAST_CYCLE_TIME_SENSOR in config:
        cg.add(var.set_last_cycle_time_sensor(config[CONF_LAST_CYCLE_TIME_SENSOR]))
    if CONF_DAILY_ACTIVE_TIME_SENSOR in config:
        cg.add(var.set_daily_active_time_sensor(config[CONF_DAILY_ACTIVE_TIME_SENSOR]))
    if CONF_DAILY_ENERGY_CONSUMPTION_SENSOR in config:
        cg.add(var.set_daily_energy_consumption_sensor(config[CONF_DAILY_ENERGY_CONSUMPTION_SENSOR]))

    if CONF_TEMP_MAX in config:
        cg.add(var.set_temp_max(config[CONF_TEMP_MAX]))
    if CONF_DIFF_HIGH in config:
        cg.add(var.set_diff_high(config[CONF_DIFF_HIGH]))
    if CONF_DIFF_MID in config:
        cg.add(var.set_diff_mid(config[CONF_DIFF_MID]))
    if CONF_VISUAL_MIN_TEMP in config:
        cg.add(var.set_visual_min_temp(config[CONF_VISUAL_MIN_TEMP]))
    if CONF_VISUAL_MAX_TEMP in config:
        cg.add(var.set_visual_max_temp(config[CONF_VISUAL_MAX_TEMP]))
    if CONF_PUMP_POWER in config:
        cg.add(var.set_pump_power(config[CONF_PUMP_POWER]))

    cg.add(var.setup())
    climate.register_climate(var, config)
