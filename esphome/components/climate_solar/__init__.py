import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, sensor, switch
from esphome.const import CONF_ID

# Definir el namespace y clase
climate_solar_ns = cg.esphome_ns.namespace('climate_solar')
ClimateSolar = climate_solar_ns.class_('ClimateSolar', climate.Climate, cg.Component)

# Declarar los parámetros que serán utilizados en el YAML
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

# Definir el esquema de configuración
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

# Función para traducir el YAML a código C++
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    # Primero registrar ClimateSolar como un componente
    await cg.register_component(var, config)
    
    # Luego registrar ClimateSolar en la lógica de Climate
    await climate.register_climate(var, config)

    # Conectar cada configuración YAML con su setter correspondiente en el código C++
    temp_sun = await cg.get_variable(config[CONF_TEMP_SUN])
    cg.add(var.set_temp_sun(temp_sun))

    temp_watter = await cg.get_variable(config[CONF_TEMP_WATTER])
    cg.add(var.set_temp_watter(temp_watter))

    temp_output = await cg.get_variable(config[CONF_TEMP_OUTPUT])
    cg.add(var.set_temp_output(temp_output))

    pump_switch = await cg.get_variable(config[CONF_PUMP_SWITCH])
    cg.add(var.set_pump_switch(pump_switch))

    # Configurar los valores numéricos
    cg.add(var.set_temp_max(config[CONF_TEMP_MAX]))
    cg.add(var.set_diff_high(config[CONF_DIFF_HIGH]))
    cg.add(var.set_diff_mid(config[CONF_DIFF_MID]))
    cg.add(var.set_visual_min_temp(config[CONF_VISUAL_MIN_TEMP]))
    cg.add(var.set_visual_max_temp(config[CONF_VISUAL_MAX_TEMP]))
    cg.add(var.set_pump_power(config[CONF_PUMP_POWER]))
