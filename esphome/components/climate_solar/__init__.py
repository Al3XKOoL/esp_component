import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, sensor, switch
from esphome.const import CONF_ID

AUTO_LOAD = ['sensor', 'climate', 'switch']

climate_solar_ns = cg.esphome_ns.namespace('climate_solar')
ClimateSolar = climate_solar_ns.class_('ClimateSolar', climate.Climate, cg.Component)

CONF_TEMP_SUN = 'temp_sun'
CONF_TEMP_WATTER = 'temp_watter'
CONF_TEMP_OUTPUT = 'temp_output'
CONF_PUMP_SWITCH = 'pump_switch'

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ClimateSolar),
    cv.Required(CONF_TEMP_SUN): cv.use_id(sensor.Sensor),  # Requerido para asegurar que los IDs estén presentes
    cv.Required(CONF_TEMP_WATTER): cv.use_id(sensor.Sensor),
    cv.Required(CONF_TEMP_OUTPUT): cv.use_id(sensor.Sensor),
    cv.Optional(CONF_PUMP_SWITCH): cv.use_id(switch.Switch),
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    # Obtener y asignar los sensores a las variables correspondientes
    if CONF_TEMP_SUN in config:
        temp_sun_sensor = yield cg.get_variable(config[CONF_TEMP_SUN])
        cg.add(var.set_temp_sun(temp_sun_sensor))

    if CONF_TEMP_WATTER in config:
        temp_watter_sensor = yield cg.get_variable(config[CONF_TEMP_WATTER])
        cg.add(var.set_temp_watter(temp_watter_sensor))

    if CONF_TEMP_OUTPUT in config:
        temp_output_sensor = yield cg.get_variable(config[CONF_TEMP_OUTPUT])
        cg.add(var.set_temp_output(temp_output_sensor))

    # Registrar el interruptor de la bomba si está presente
    if CONF_PUMP_SWITCH in config:
        pump_switch = yield cg.get_variable(config[CONF_PUMP_SWITCH])
        cg.add(var.set_pump_switch(pump_switch))

    # Configuración final del componente Climate
    cg.add(var.setup())
    climate.register_climate(var, config)
