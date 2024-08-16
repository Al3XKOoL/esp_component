import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate
from esphome.const import CONF_ID

DEPENDENCIES = ['climate']
AUTO_LOAD = ['sensor', 'switch']

custom_climate_ns = cg.esphome_ns.namespace('custom_climate')
CustomClimate = custom_climate_ns.class_('CustomClimate', climate.Climate, cg.Component)

CONFIG_SCHEMA = climate.CLIMATE_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(CustomClimate),
    cv.Required('temperature_azotea_sensor'): cv.use_id(cg.Sensor),
    cv.Required('temperature_spa_sensor'): cv.use_id(cg.Sensor),
    cv.Required('temperature_caliente_sensor'): cv.use_id(cg.Sensor),
    cv.Required('bomba_solar_switch'): cv.use_id(cg.Switch),
    cv.Optional('max_temperature', default=37.5): cv.float_,
    cv.Optional('temperature_difference', default=2.0): cv.float_,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await climate.register_climate(var, config)
    await cg.register_component(var, config)

    temperature_azotea_sensor = await cg.get_variable(config['temperature_azotea_sensor'])
    cg.add(var.set_temperature_azotea_sensor(temperature_azotea_sensor))

    temperature_spa_sensor = await cg.get_variable(config['temperature_spa_sensor'])
    cg.add(var.set_temperature_spa_sensor(temperature_spa_sensor))

    temperature_caliente_sensor = await cg.get_variable(config['temperature_caliente_sensor'])
    cg.add(var.set_temperature_caliente_sensor(temperature_caliente_sensor))

    bomba_solar_switch = await cg.get_variable(config['bomba_solar_switch'])
    cg.add(var.set_bomba_solar_switch(bomba_solar_switch))

    cg.add(var.set_max_temperature(config['max_temperature']))
    cg.add(var.set_temperature_difference(config['temperature_difference']))
