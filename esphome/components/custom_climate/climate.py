import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components.template.number import TemplateNumber
from esphome.components import climate, sensor, switch, time, number
from esphome.const import (
    CONF_ID, CONF_MIN_VALUE, CONF_MAX_VALUE, CONF_STEP, CONF_NAME, 
    CONF_UNIT_OF_MEASUREMENT, CONF_ACCURACY_DECIMALS, CONF_MODE
)

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
CONF_DIFERENCIA_MEDIA_NUMBER = "diferencia_media_number"
CONF_DIFERENCIA_ALTA_NUMBER = "diferencia_alta_number"

CONF_CONTEO_ENCENDIDOS = "conteo_encendidos"
CONF_TIEMPO_ENCENDIDO = "tiempo_encendido"
CONF_KWH_HOY = "kwh_hoy"
CONF_KWH_TOTAL = "kwh_total"

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
    cv.Optional(CONF_TIEMPO_SNTP): cv.use_id(time.RealTimeClock),
    cv.Optional(CONF_TIEMPO_HOMEASSISTANT): cv.use_id(time.RealTimeClock),
    cv.Required(CONF_FACTOR_TIEMPO_ACTIVACION): cv.float_,
    cv.Optional(CONF_TEMPERATURA_CERCA): cv.float_,
    cv.Optional(CONF_DIFERENCIA_MEDIA_NUMBER): cv.use_id(number.Number),
    cv.Optional(CONF_DIFERENCIA_ALTA_NUMBER): cv.use_id(number.Number),
    cv.Required(CONF_CONTEO_ENCENDIDOS): sensor.sensor_schema(
        unit_of_measurement="",
        accuracy_decimals=0,
    ),
    cv.Required(CONF_TIEMPO_ENCENDIDO): sensor.sensor_schema(
        unit_of_measurement="s",
        accuracy_decimals=0,
    ),
    cv.Required(CONF_KWH_HOY): sensor.sensor_schema(
        unit_of_measurement="kWh",
        accuracy_decimals=3,
    ),
    cv.Required(CONF_KWH_TOTAL): sensor.sensor_schema(
        unit_of_measurement="kWh",
        accuracy_decimals=3,
    ),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await climate.register_climate(var, config)

    for conf, setter in [
        (CONF_SENSOR_TEMP_SOL, 'set_sensor_temp_sol'),
        (CONF_SENSOR_TEMP_AGUA, 'set_sensor_temp_agua'),
        (CONF_SENSOR_TEMP_SALIDA, 'set_sensor_temp_salida'),
        (CONF_DIFERENCIA_ALTA, 'set_diferencia_alta'),
        (CONF_DIFERENCIA_MEDIA, 'set_diferencia_media'),
        (CONF_TEMPERATURA_VISUAL_MINIMA, 'set_temperatura_visual_minima'),
        (CONF_TEMPERATURA_VISUAL_MAXIMA, 'set_temperatura_visual_maxima'),
        (CONF_POTENCIA_BOMBA, 'set_potencia_bomba'),
        (CONF_INTERRUPTOR_BOMBA, 'set_interruptor_bomba'),
        (CONF_FACTOR_TIEMPO_ACTIVACION, 'set_factor_tiempo_activacion'),
        (CONF_TEMPERATURA_CERCA, 'set_temperatura_cerca'),
        (CONF_TIEMPO_SNTP, 'set_tiempo_sntp'),
        (CONF_TIEMPO_HOMEASSISTANT, 'set_tiempo_homeassistant'),
        (CONF_DIFERENCIA_MEDIA_NUMBER, 'set_diferencia_media_number'),
        (CONF_DIFERENCIA_ALTA_NUMBER, 'set_diferencia_alta_number'),
    ]:
        if conf in config:
            value = config[conf]
            if cv.is_config_id(value):
                value = await cg.get_variable(value)
            cg.add(getattr(var, setter)(value))

    for conf, setter in [
        (CONF_CONTEO_ENCENDIDOS, 'set_conteo_encendidos_sensor'),
        (CONF_TIEMPO_ENCENDIDO, 'set_tiempo_encendido_sensor'),
        (CONF_KWH_HOY, 'set_kwh_hoy_sensor'),
        (CONF_KWH_TOTAL, 'set_kwh_total_sensor'),
    ]:
        sensor_var = await sensor.new_sensor(config[conf])
        cg.add(getattr(var, setter)(sensor_var))

