import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display
from esphome.const import CONF_ID, CONF_LAMBDA, CONF_DATA_PINS, CONF_RESET_PIN, CONF_DC_PIN, CONF_WR_PIN, CONF_RD_PIN, CONF_CS_PIN

DEPENDENCIES = ['esp32']

ili9341_parallel_ns = cg.esphome_ns.namespace('ili9341_parallel')
ILI9341ParallelDisplay = ili9341_parallel_ns.class_('ILI9341ParallelDisplay', display.DisplayBuffer, cg.PollingComponent)

CONFIG_SCHEMA = display.FULL_DISPLAY_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(ILI9341ParallelDisplay),
    cv.Required(CONF_DATA_PINS): cv.All(cv.ensure_list(cv.gpio_output_pin_schema), cv.Length(min=8, max=8)),
    cv.Required(CONF_RESET_PIN): cv.gpio_output_pin_schema,
    cv.Required(CONF_DC_PIN): cv.gpio_output_pin_schema,
    cv.Required(CONF_WR_PIN): cv.gpio_output_pin_schema,
    cv.Required(CONF_RD_PIN): cv.gpio_output_pin_schema,
    cv.Required(CONF_CS_PIN): cv.gpio_output_pin_schema,
}).extend(cv.polling_component_schema('1s'))

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield display.register_display(var, config)
    yield cg.register_component(var, config)

    data_pins = []
    for pin in config[CONF_DATA_PINS]:
        data_pin = yield cg.gpio_pin_expression(pin)
        data_pins.append(data_pin)

    cg.add(var.set_data_pins(*data_pins))
    
    reset_pin = yield cg.gpio_pin_expression(config[CONF_RESET_PIN])
    cg.add(var.set_reset_pin(reset_pin))
    
    dc_pin = yield cg.gpio_pin_expression(config[CONF_DC_PIN])
    cg.add(var.set_dc_pin(dc_pin))
    
    wr_pin = yield cg.gpio_pin_expression(config[CONF_WR_PIN])
    cg.add(var.set_wr_pin(wr_pin))
    
    rd_pin = yield cg.gpio_pin_expression(config[CONF_RD_PIN])
    cg.add(var.set_rd_pin(rd_pin))
    
    cs_pin = yield cg.gpio_pin_expression(config[CONF_CS_PIN])
    cg.add(var.set_cs_pin(cs_pin))

    if CONF_LAMBDA in config:
        lambda_ = yield cg.process_lambda(config[CONF_LAMBDA], [(display.DisplayBufferRef, 'it')],
                                          return_type=cg.void)
        cg.add(var.set_writer(lambda_))
