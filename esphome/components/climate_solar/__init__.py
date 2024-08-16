import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import climate, sensor, switch
from esphome.const import CONF_ID, CONF_TEMPERATURE, UNIT_CELSIUS, ICON_THERMOMETER, CONF_SWITCH

# Declaramos el namespace y el nombre del componente en C++
climate_solar_ns = cg.esphome_ns.namespace("climate_solar")
ClimateSolar = climate_solar_ns.class_("ClimateSolar", climate.Climate, cg.Component)

# Definimos las opciones que vamos a soportar en el YAML
CONF_TEMP_SUN = "temp_sun"
CONF_TEMP_WATTER = "temp_watter"
CONF_TEMP_OUTPUT = "temp_output"
CONF_TEMP_MAX = "temp_max"
CONF_DIFF_HIGH = "diff_high"
CONF_DIFF_MID = "diff_mid"
CONF_VISUAL_MIN_TEMP = "visual_min_temp"
CONF_VISUAL_MAX_TEMP = "visual_max_temp"
CONF_PUMP_POWER = "pump_power"
CONF_PUMP_SWITCH = "pump_switch"

# Especificamos la configuración
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ClimateSolar),
    cv.Required(CONF_TEMP_SUN): cv.use_id(sensor.Sensor),
    cv.Required(CONF_TEMP_WATTER): cv.use_id(sensor.Sensor),
    cv.Required(CONF_TEMP_OUTPUT): cv.use_id(sensor.Sensor),
    cv.Required(CONF_TEMP_MAX): cv.float_,
    cv.Required(CONF_DIFF_HIGH): cv.float_,
    cv.Required(CONF_DIFF_MID): cv.float_,
    cv.Required(CONF_VISUAL_MIN_TEMP): cv.temperature,
    cv.Required(CONF_VISUAL_MAX_TEMP): cv.temperature,
    cv.Required(CONF_PUMP_POWER): cv.float_,
    cv.Required(CONF_PUMP_SWITCH): cv.use_id(switch.Switch),
}).extend(cv.COMPONENT_SCHEMA).extend(climate.CLIMATE_SCHEMA)

# Función para construir el componente
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    # Asignamos las configuraciones del YAML a las variables en C++
    await cg.register_component(var, config)
    await climate.register_climate(var, config)

    temp_sun = await cg.get_variable(config[CONF_TEMP_SUN])
    cg.add(var.temp_sun_, temp_sun)

    temp_watter = await cg.get_variable(config[CONF_TEMP_WATTER])
    cg.add(var.temp_watter_, temp_watter)

    temp_output = await cg.get_variable(config[CONF_TEMP_OUTPUT])
    cg.add(var.temp_output_, temp_output)

    pump_switch = await cg.get_variable(config[CONF_PUMP_SWITCH])
    cg.add(var.pump_switch_, pump_switch)

    cg.add(var.temp_max_, config[CONF_TEMP_MAX])
    cg.add(var.diff_high_, config[CONF_DIFF_HIGH])
    cg.add(var.diff_mid_, config[CONF_DIFF_MID])
    cg.add(var.visual_min_temp_, config[CONF_VISUAL_MIN_TEMP])
    cg.add(var.visual_max_temp_, config[CONF_VISUAL_MAX_TEMP])
    cg.add(var.pump_power_, config[CONF_PUMP_POWER])
