import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, sensor, switch, time, number
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
    cv.Optional(CONF_TIEMPO_SNTP): cv.use_id(time.RealTimeClock),
    cv.Optional(CONF_TIEMPO_HOMEASSISTANT): cv.use_id(time.RealTimeClock),
    cv.Required(CONF_FACTOR_TIEMPO_ACTIVACION): cv.float_,
    cv.Required(CONF_TEMPERATURA_CERCA): cv.float_,
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

    # Registrar los nuevos números
    diferencia_media_number = await number.new_number(
        number.number_schema(
            min_value=0.1,
            max_value=5.0,
            step=0.1,
            initial_value=config[CONF_DIFERENCIA_MEDIA],
            name="Diferencia Media",
        ),
        var.set_diferencia_media_number,
    )

    diferencia_alta_number = await number.new_number(
        number.number_schema(
            min_value=0.1,
            max_value=5.0,
            step=0.1,
            initial_value=config[CONF_DIFERENCIA_ALTA],
            name="Diferencia Alta",
        ),
        var.set_diferencia_alta_number,
    )

    # Registrar los nuevos sensores
    conteo_encendidos_sensor = await sensor.new_sensor(sensor.sensor_schema(
        unit_of_measurement="",
        accuracy_decimals=0,
        name="Conteo Encendidos",
    ), var.set_conteo_encendidos_sensor)

    tiempo_encendido_sensor = await sensor.new_sensor(sensor.sensor_schema(
        unit_of_measurement="s",
        accuracy_decimals=0,
        name="Tiempo Encendido",
    ), var.set_tiempo_encendido_sensor)

    kwh_hoy_sensor = await sensor.new_sensor(sensor.sensor_schema(
        unit_of_measurement="kWh",
        accuracy_decimals=3,
        name="kWh Hoy",
    ), var.set_kwh_hoy_sensor)

    kwh_total_sensor = await sensor.new_sensor(sensor.sensor_schema(
        unit_of_measurement="kWh",
        accuracy_decimals=3,
        name="kWh Total",
    ), var.set_kwh_total_sensor)
