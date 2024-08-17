import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, sensor, switch, time
from esphome.const import CONF_ID

CONF_SENSOR_TEMP_SOL = "sensor_temp_sol"
CONF_SENSOR_TEMP_AGUA = "sensor_temp_agua"
CONF_SENSOR_TEMP_SALIDA = "sensor_temp_salida"
CONF_DIFERENCIA_ALTA = "diferencia_alta"
CONF_DIFERENCIA_MEDIA = "diferencia_media"
CONF_TEMPERATURA_VISUAL_MINIMA = "temperatura_visual_minima"
CONF_TEMPERATURA_VISUAL_MAXIMA = "temperatura_visual_maxima"
CONF_POTENCIA_BOMBA = "potencia_bomba"
CONF_INTERRUPTOR_BOMBA = "interruptor_bomba"
CONF_TIEMPO_SNTP = "tiempo_sntp"
CONF_TIEMPO_HOMEASSISTANT = "tiempo_homeassistant"
CONF_FACTOR_TIEMPO_ACTIVACION = "factor_tiempo_activacion"
CONF_TEMPERATURA_CERCA = "temperatura_cerca"

custom_climate_ns = cg.esphome_ns.namespace('custom_climate')
CustomClimate = custom_climate_ns.class_('CustomClimate', climate.Climate, cg.Component)

CONFIG_SCHEMA = climate.CLIMATE_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(CustomClimate),
    cv.Required(CONF_SENSOR_TEMP_SOL): cv.use_id(sensor.Sensor),
    cv.Required(CONF_SENSOR_TEMP_AGUA): cv.use_id(sensor.Sensor),
    cv.Required(CONF_SENSOR_TEMP_SALIDA): cv.use_id(sensor.Sensor),
    cv.Required(CONF_DIFERENCIA_ALTA): cv.float_,
    cv.Required(CONF_DIFERENCIA_MEDIA): cv.float_,
    cv.Required(CONF_TEMPERATURA_VISUAL_MINIMA): cv.float_,
    cv.Required(CONF_TEMPERATURA_VISUAL_MAXIMA): cv.float_,
    cv.Required(CONF_POTENCIA_BOMBA): cv.float_,
    cv.Required(CONF_INTERRUPTOR_BOMBA): cv.use_id(switch.Switch),
    cv.Required(CONF_TIEMPO_SNTP): cv.use_id(time.RealTimeClock),
    cv.Required(CONF_TIEMPO_HOMEASSISTANT): cv.use_id(time.RealTimeClock),
    cv.Optional(CONF_FACTOR_TIEMPO_ACTIVACION, default=10.0): cv.float_,
    cv.Optional(CONF_TEMPERATURA_CERCA, default=1.0): cv.float_,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await climate.register_climate(var, config)

    sensor_temp_sol = await cg.get_variable(config[CONF_SENSOR_TEMP_SOL])
    cg.add(var.set_sensor_temp_sol(sensor_temp_sol))
    
    sensor_temp_agua = await cg.get_variable(config[CONF_SENSOR_TEMP_AGUA])
    cg.add(var.set_sensor_temp_agua(sensor_temp_agua))

    sensor_temp_salida = await cg.get_variable(config[CONF_SENSOR_TEMP_SALIDA])
    cg.add(var.set_sensor_temp_salida(sensor_temp_salida))

    cg.add(var.set_diferencia_alta(config[CONF_DIFERENCIA_ALTA]))
    cg.add(var.set_diferencia_media(config[CONF_DIFERENCIA_MEDIA]))
    cg.add(var.set_temperatura_visual_minima(config[CONF_TEMPERATURA_VISUAL_MINIMA]))
    cg.add(var.set_temperatura_visual_maxima(config[CONF_TEMPERATURA_VISUAL_MAXIMA]))
    cg.add(var.set_potencia_bomba(config[CONF_POTENCIA_BOMBA]))

    interruptor_bomba = await cg.get_variable(config[CONF_INTERRUPTOR_BOMBA])
    cg.add(var.set_interruptor_bomba(interruptor_bomba))

    tiempo_sntp = await cg.get_variable(config[CONF_TIEMPO_SNTP])
    cg.add(var.set_tiempo_sntp(tiempo_sntp))

    tiempo_homeassistant = await cg.get_variable(config[CONF_TIEMPO_HOMEASSISTANT])
    cg.add(var.set_tiempo_homeassistant(tiempo_homeassistant))

    cg.add(var.set_factor_tiempo_activacion(config[CONF_FACTOR_TIEMPO_ACTIVACION]))
    cg.add(var.set_temperatura_cerca(config[CONF_TEMPERATURA_CERCA]))
