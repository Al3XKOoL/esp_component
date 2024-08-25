import esphome.codegen as cg
from esphome import config_validation as cv
from esphome.components import display
from esphome.const import CONF_ID

# Define el namespace para el nuevo controlador
ili9341_parallel_ns = cg.esphome_ns.namespace('ili9341_parallel')
ILI9341ParallelDisplay = ili9341_parallel_ns.class_('ILI9341ParallelDisplay', display.DisplayBuffer, cg.Component)

# Configuración del esquema
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ILI9341ParallelDisplay),
    cv.Required('cs_pin'): cv.All(cv.declare_id(cv.GPIOPin)),
    cv.Required('dc_pin'): cv.All(cv.declare_id(cv.GPIOPin)),
    cv.Required('reset_pin'): cv.All(cv.declare_id(cv.GPIOPin)),
    cv.Required('wr_pin'): cv.All(cv.declare_id(cv.GPIOPin)),
    cv.Required('rd_pin'): cv.All(cv.declare_id(cv.GPIOPin)),
    cv.Required('data_pins'): cv.All(cv.ensure_list(cv.declare_id(cv.GPIOPin))),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    # Esperar los pines de GPIO
    cs_pin = await cg.gpio_pin_expression(config['cs_pin'])
    dc_pin = await cg.gpio_pin_expression(config['dc_pin'])
    reset_pin = await cg.gpio_pin_expression(config['reset_pin'])
    wr_pin = await cg.gpio_pin_expression(config['wr_pin'])
    rd_pin = await cg.gpio_pin_expression(config['rd_pin'])

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
