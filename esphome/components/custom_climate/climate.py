import esphome.codegen as cg
import esphome.config_validation as cv
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
    cv.Required(CONF_TEMPERATURA_CERCA): cv.float_,
    cv.Required(CONF_DIFERENCIA_MEDIA_NUMBER): number.NUMBER_SCHEMA.extend({
        cv.GenerateID(): cv.declare_id(number.Number),
        cv.Optional(CONF_NAME, default="Diferencia Media"): cv.string,
        cv.Optional(CONF_MIN_VALUE, default=0.1): cv.float_,
        cv.Optional(CONF_MAX_VALUE, default=5.0): cv.float_,
        cv.Optional(CONF_STEP, default=0.1): cv.float_,
    }),
    cv.Required(CONF_DIFERENCIA_ALTA_NUMBER): number.NUMBER_SCHEMA.extend({
        cv.GenerateID(): cv.declare_id(number.Number),
        cv.Optional(CONF_NAME, default="Diferencia Alta"): cv.string,
        cv.Optional(CONF_MIN_VALUE, default=0.1): cv.float_,
        cv.Optional(CONF_MAX_VALUE, default=5.0): cv.float_,
        cv.Optional(CONF_STEP, default=0.1): cv.float_,
    }),
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

    cg.add(var.set_factor_tiempo_activacion(config[CONF_FACTOR_TIEMPO_ACTIVACION]))
    cg.add(var.set_temperatura_cerca(config[CONF_TEMPERATURA_CERCA]))

    if CONF_TIEMPO_SNTP in config:
        tiempo_sntp = await cg.get_variable(config[CONF_TIEMPO_SNTP])
        cg.add(var.set_tiempo_sntp(tiempo_sntp))

    if CONF_TIEMPO_HOMEASSISTANT in config:
        tiempo_homeassistant = await cg.get_variable(config[CONF_TIEMPO_HOMEASSISTANT])
        cg.add(var.set_tiempo_homeassistant(tiempo_homeassistant))

    diferencia_media_number = await number.new_number(config[CONF_DIFERENCIA_MEDIA_NUMBER])
    cg.add(var.set_diferencia_media_number(diferencia_media_number))

    diferencia_alta_number = await number.new_number(config[CONF_DIFERENCIA_ALTA_NUMBER])
    cg.add(var.set_diferencia_alta_number(diferencia_alta_number))

    conteo_encendidos_sensor = await sensor.new_sensor(config[CONF_CONTEO_ENCENDIDOS])
    cg.add(var.set_conteo_encendidos_sensor(conteo_encendidos_sensor))

    tiempo_encendido_sensor = await sensor.new_sensor(config[CONF_TIEMPO_ENCENDIDO])
    cg.add(var.set_tiempo_encendido_sensor(tiempo_encendido_sensor))

    kwh_hoy_sensor = await sensor.new_sensor(config[CONF_KWH_HOY])
    cg.add(var.set_kwh_hoy_sensor(kwh_hoy_sensor))

    kwh_total_sensor = await sensor.new_sensor(config[CONF_KWH_TOTAL])
    cg.add(var.set_kwh_total_sensor(kwh_total_sensor))
