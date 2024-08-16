import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, sensor, switch  # Importa los módulos correctos
from esphome.const import CONF_ID

CODEOWNERS = ["@tu_usuario_github"]
DEPENDENCIES = ['climate']
AUTO_LOAD = ['sensor', 'switch']

custom_climate_ns = cg.esphome_ns.namespace('custom_climate')
CustomClimate = custom_climate_ns.class_('CustomClimate', climate.Climate, cg.Component)

CONF_TEMPERATURE_AZOTEA_SENSOR = "temperature_azotea_sensor"
CONF_TEMPERATURE_SPA_SENSOR = "temperature_spa_sensor"
CONF_TEMPERATURE_CALIENTE_SENSOR = "temperature_caliente_sensor"
CONF_BOMBA_SOLAR_SWITCH = "bomba_solar_switch"
CONF_MAX_TEMPERATURE = "max_temperature"
CONF_TEMPERATURE_DIFFERENCE = "temperature_difference"

CONFIG_SCHEMA = climate.CLIMATE_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(CustomClimate),
    cv.Required(CONF_TEMPERATURE_AZOTEA_SENSOR): cv.use_id(sensor.Sensor),  # Cambio aquí
    cv.Required(CONF_TEMPERATURE_SPA_SENSOR): cv.use_id(sensor.Sensor),
    cv.Required(CONF_TEMPERATURE_CALIENTE_SENSOR): cv.use_id(sensor.Sensor),
    cv.Required(CONF_BOMBA_SOLAR_SWITCH): cv.use_id(switch.Switch),  # Y aquí
    cv.Optional(CONF_MAX_TEMPERATURE, default=37.5): cv.float_,
    cv.Optional(CONF_TEMPERATURE_DIFFERENCE, default=2.0): cv.float_,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await climate.register_climate(var, config)
    await cg.register_component(var, config)

    temperature_azotea_sensor = await cg.get_variable(config[CONF_TEMPERATURE_AZOTEA_SENSOR])
    cg.add(var.set_temperature_azotea_sensor(temperature_azotea_sensor))

    temperature_spa_sensor = await cg.get_variable(config[CONF_TEMPERATURE_SPA_SENSOR])
    cg.add(var.set_temperature_spa_sensor(temperature_spa_sensor))

    temperature_caliente_sensor = await cg.get_variable(config[CONF_TEMPERATURE_CALIENTE_SENSOR])
    cg.add(var.set_temperature_caliente_sensor(temperature_caliente_sensor))

    bomba_solar_switch = await cg.get_variable(config[CONF_BOMBA_SOLAR_SWITCH])
    cg.add(var.set_bomba_solar_switch(bomba_solar_switch))

    cg.add(var.set_max_temperature(config[CONF_MAX_TEMPERATURE]))
    cg.add(var.set_temperature_difference(config[CONF_TEMPERATURE_DIFFERENCE]))
