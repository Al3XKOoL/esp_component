import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, sensor, switch, time
from esphome.const import CONF_ID

CONF_TEMP_SUN = "temp_sun"
CONF_TEMP_WATER = "temp_water"
CONF_TEMP_OUTPUT = "temp_output"
CONF_TEMP_MAX = "temp_max"
CONF_DIFF_HIGH = "diff_high"
CONF_DIFF_MID = "diff_mid"
CONF_VISUAL_MIN_TEMP = "visual_min_temp"
CONF_VISUAL_MAX_TEMP = "visual_max_temp"
CONF_PUMP_POWER = "pump_power"
CONF_PUMP_SWITCH = "pump_switch"
CONF_SNTP_TIME = "sntp_time"
CONF_HOMEASSISTANT_TIME = "homeassistant_time"

custom_climate_ns = cg.esphome_ns.namespace('custom_climate')
CustomClimate = custom_climate_ns.class_('CustomClimate', climate.Climate, cg.Component)

CONFIG_SCHEMA = climate.CLIMATE_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(CustomClimate),
    cv.Required(CONF_TEMP_SUN): cv.use_id(sensor.Sensor),
    cv.Required(CONF_TEMP_WATER): cv.use_id(sensor.Sensor),
    cv.Required(CONF_TEMP_OUTPUT): cv.use_id(sensor.Sensor),
    cv.Required(CONF_TEMP_MAX): cv.float_,
    cv.Required(CONF_DIFF_HIGH): cv.float_,
    cv.Required(CONF_DIFF_MID): cv.float_,
    cv.Required(CONF_VISUAL_MIN_TEMP): cv.float_,
    cv.Required(CONF_VISUAL_MAX_TEMP): cv.float_,
    cv.Required(CONF_PUMP_POWER): cv.float_,
    cv.Required(CONF_PUMP_SWITCH): cv.use_id(switch.Switch),
    cv.Optional(CONF_SNTP_TIME): cv.use_id(time.RealTimeClock),
    cv.Optional(CONF_HOMEASSISTANT_TIME): cv.use_id(time.RealTimeClock),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await climate.register_climate(var, config)
    
    temp_sun = await cg.get_variable(config[CONF_TEMP_SUN])
    cg.add(var.set_temp_sun(temp_sun))
    temp_water = await cg.get_variable(config[CONF_TEMP_WATER])
    cg.add(var.set_temp_water(temp_water))
    temp_output = await cg.get_variable(config[CONF_TEMP_OUTPUT])
    cg.add(var.set_temp_output(temp_output))
    cg.add(var.set_temp_max(config[CONF_TEMP_MAX]))
    cg.add(var.set_diff_high(config[CONF_DIFF_HIGH]))
    cg.add(var.set_diff_mid(config[CONF_DIFF_MID]))
    cg.add(var.set_visual_min_temp(config[CONF_VISUAL_MIN_TEMP]))
    cg.add(var.set_visual_max_temp(config[CONF_VISUAL_MAX_TEMP]))
    cg.add(var.set_pump_power(config[CONF_PUMP_POWER]))
    pump_switch = await cg.get_variable(config[CONF_PUMP_SWITCH])
    cg.add(var.set_pump_switch(pump_switch))

    if CONF_SNTP_TIME in config:
        sntp_time = await cg.get_variable(config[CONF_SNTP_TIME])
        cg.add(var.set_sntp_time(sntp_time))

    if CONF_HOMEASSISTANT_TIME in config:
        homeassistant_time = await cg.get_variable(config[CONF_HOMEASSISTANT_TIME])
        cg.add(var.set_homeassistant_time(homeassistant_time))
