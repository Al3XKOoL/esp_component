import esphome.codegen as cg
from esphome import config_validation as cv
from esphome import pins
from esphome.components import display
from esphome.const import CONF_ID

# Define el namespace para el nuevo controlador
ili9341_parallel_ns = cg.esphome_ns.namespace('ili9341_parallel')
ILI9341ParallelDisplay = ili9341_parallel_ns.class_('ILI9341ParallelDisplay', display.DisplayBuffer, cg.Component)

# Configuración del esquema
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ILI9341ParallelDisplay),
    cv.Required('cs_pin'): pins.gpio_output_pin_schema,
    cv.Required('dc_pin'): pins.gpio_output_pin_schema,
    cv.Required('reset_pin'): pins.gpio_output_pin_schema,
    cv.Required('wr_pin'): pins.gpio_output_pin_schema,
    cv.Required('rd_pin'): pins.gpio_output_pin_schema,
    cv.Required('data_pins'): cv.All([pins.gpio_output_pin_schema], lambda value: value if len(value) == 8 else cv.Invalid("Need 8 data pins")),
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_cs_pin(config['cs_pin']))
    cg.add(var.set_dc_pin(config['dc_pin']))
    cg.add(var.set_reset_pin(config['reset_pin']))
    cg.add(var.set_wr_pin(config['wr_pin']))
    cg.add(var.set_rd_pin(config['rd_pin']))
    
    pins_ = [yield cg.gpio_pin_expression(pin) for pin in config['data_pins']]
    cg.add(var.set_data_pins(*pins_))

    yield cg.register_component(var, config)
    yield display.register_display(var, config)
