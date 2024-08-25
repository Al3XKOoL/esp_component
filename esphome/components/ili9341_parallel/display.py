import esphome.codegen as cg
from esphome import config_validation as cv
from esphome.components import display, gpio
from esphome.const import CONF_ID

# Define el namespace para el nuevo controlador
ili9341_parallel_ns = cg.esphome_ns.namespace('ili9341_parallel')
ILI9341ParallelDisplay = ili9341_parallel_ns.class_('ILI9341ParallelDisplay', display.DisplayBuffer, cg.Component)

# Configuración del esquema
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ILI9341ParallelDisplay),
    cv.Required('cs_pin'): gpio.pin_schema,
    cv.Required('dc_pin'): gpio.pin_schema,
    cv.Required('reset_pin'): gpio.pin_schema,
    cv.Required('wr_pin'): gpio.pin_schema,
    cv.Required('rd_pin'): gpio.pin_schema,
    cv.Required('data_pins'): cv.All(cv.ensure_list(gpio.pin_schema)),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    # Agregar pines al componente
    cg.add(var.set_cs_pin(config['cs_pin']))
    cg.add(var.set_dc_pin(config['dc_pin']))
    cg.add(var.set_reset_pin(config['reset_pin']))
    cg.add(var.set_wr_pin(config['wr_pin']))
    cg.add(var.set_rd_pin(config['rd_pin']))
    data_pins = [await cg.gpio_pin_expression(pin) for pin in config['data_pins']]
    cg.add(var.set_data_pins(data_pins))
    await display.register_display(var, config)

    # Agregar los pines
    cg.add(var.set_cs_pin(cs_pin))
    cg.add(var.set_dc_pin(dc_pin))
    cg.add(var.set_reset_pin(reset_pin))
    cg.add(var.set_wr_pin(wr_pin))
    cg.add(var.set_rd_pin(rd_pin))

    # Agregar los pines de datos
    data_pins = [await cg.gpio_pin_expression(pin) for pin in config['data_pins']]
    cg.add(var.set_data_pins(*data_pins))

    # Registrar el componente y el display
    await cg.register_component(var, config)  # Asegúrate de que esto solo se llame una vez
    await display.register_display(var, config)
