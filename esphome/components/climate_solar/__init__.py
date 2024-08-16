import esphome.config_validation as cv
from esphome import automation
from esphome.components import climate, sensor, switch
import esphome.codegen as cg
from esphome.const import CONF_ID

# Define el nombre de tu componente y crea un namespace
CONF_CLIMATE_SOLAR = "climate_solar"
climate_solar_ns = cg.esphome_ns.namespace('climate_solar')
ClimateSolar = climate_solar_ns.class_('ClimateSolar', climate.Climate, cg.Component)

# Configura los parámetros disponibles en YAML
CONFIG_SCHEMA = climate.CLIMATE_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(ClimateSolar),
    cv.Required("temp_sun"): cv.use_id(sensor.Sensor),
    cv.Required("temp_watter"): cv.use_id(sensor.Sensor),
    cv.Required("temp_output"): cv.use_id(sensor.Sensor),
    cv.Required("temp_max"): cv.float_,
    cv.Required("diff_high"): cv.float_,
    cv.Required("diff_mid"): cv.float_,
    cv.Required("visual_min_temp"): cv.float_,
    cv.Required("visual_max_temp"): cv.float_,
    cv.Required("pump_power"): cv.float_,
    cv.Required("pump_switch"): cv.use_id(switch.Switch),
}).extend(cv.COMPONENT_SCHEMA)

# Esta función permite la configuración del componente desde el YAML
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    await cg.register_component(var, config)
    await climate.register_climate(var, config)

    # Asigna los sensores y switches del YAML al componente
    temp_sun = await cg.get_variable(config["temp_sun"])
    cg.add(var.set_temp_sun(temp_sun))
    
    temp_watter = await cg.get_variable(config["temp_watter"])
    cg.add(var.set_temp_w
