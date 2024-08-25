import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display
from esphome.const import CONF_ID, CONF_LAMBDA
from esphome import pins

DEPENDENCIES = ['esp32']

ili9341_parallel_ns = cg.esphome_ns.namespace('ili9341_parallel')
ILI9341ParallelDisplay = ili9341_parallel_ns.class_('ILI9341ParallelDisplay', display.DisplayBuffer, cg.Component)

CONF_DATA_PINS = 'data_pins'
CONF_RESET_PIN = 'reset_pin'
CONF_DC_PIN = 'dc_pin'
CONF_WR_PIN = 'wr_pin'
CONF_RD_PIN = 'rd_pin'
CONF_CS_PIN = 'cs_pin'

CONFIG_SCHEMA = display.BASIC_DISPLAY_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(ILI9341ParallelDisplay),
    cv.Required(CONF_DATA_PINS): cv.All(cv.ensure_list(pins.gpio_output_pin_schema), cv.Length(min=8, max=8)),
    cv.Required(CONF_RESET_PIN): pins.gpio_output_pin_schema,
    cv.Required(CONF_DC_PIN): pins.gpio_output_pin_schema,
    cv.Required(CONF_WR_PIN): pins.gpio_output_pin_schema,
    cv.Required(CONF_RD_PIN): pins.gpio_output_pin_schema,
    cv.Required(CONF_CS_PIN): pins.gpio_output_pin_schema,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await display.register_display(var, config)

    data_pins = []
    for pin in config[CONF_DATA_PINS]:
        data_pin = await cg.gpio_pin_expression(pin)
        data_pins.append(data_pin)

    cg.add(var.set_data_pins(*data_pins))
    
    reset_pin = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
    cg.add(var.set_reset_pin(reset_pin))
    
    dc_pin = await cg.gpio_pin_expression(config[CONF_DC_PIN])
    cg.add(var.set_dc_pin(dc_pin))
    
    wr_pin = await cg.gpio_pin_expression(config[CONF_WR_PIN])
    cg.add(var.set_wr_pin(wr_pin))
    
    rd_pin = await cg.gpio_pin_expression(config[CONF_RD_PIN])
    cg.add(var.set_rd_pin(rd_pin))
    
    cs_pin = await cg.gpio_pin_expression(config[CONF_CS_PIN])
    cg.add(var.set_cs_pin(cs_pin))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(config[CONF_LAMBDA], [(display.DisplayBufferRef, 'it')],
                                          return_type=cg.void)
        cg.add(var.set_writer(lambda_))
