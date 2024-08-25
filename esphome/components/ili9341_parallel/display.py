import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import display
from esphome.const import CONF_ID, CONF_MODEL, CONF_WIDTH, CONF_HEIGHT

DEPENDENCIES = ["esp32"]

ili9341_parallel_ns = cg.esphome_ns.namespace("ili9341_parallel")
ILI9341ParallelDisplay = ili9341_parallel_ns.class_("ILI9341ParallelDisplay", cg.Component, display.DisplayBuffer)

# ... (resto de las definiciones de CONF_*)

CONFIG_SCHEMA = cv.All(
    display.BASIC_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(ILI9341ParallelDisplay),
            cv.Required(CONF_MODEL): cv.string,
            cv.Required(CONF_DC_PIN): pins.gpio_output_pin_schema,
            cv.Required(CONF_RESET_PIN): pins.gpio_output_pin_schema,
            cv.Required(CONF_DATA_PINS): cv.All(cv.ensure_list(pins.gpio_output_pin_schema), validate_data_pins),
            cv.Required(CONF_WR_PIN): pins.gpio_output_pin_schema,
            cv.Required(CONF_RD_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_WIDTH, default=240): cv.int_,
            cv.Optional(CONF_HEIGHT, default=320): cv.int_,
        }
    ).extend(cv.COMPONENT_SCHEMA),
    cv.only_with_arduino,
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await display.register_display(var, config)

    cg.add(var.set_model(config[CONF_MODEL]))
    cg.add(var.set_width(config[CONF_WIDTH]))
    cg.add(var.set_height(config[CONF_HEIGHT]))
    
    dc_pin = await cg.gpio_pin_expression(config[CONF_DC_PIN])
    cg.add(var.set_dc_pin(dc_pin))
    
    reset_pin = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
    cg.add(var.set_reset_pin(reset_pin))
    
    for i, pin in enumerate(config[CONF_DATA_PINS]):
        data_pin = await cg.gpio_pin_expression(pin)
        cg.add(var.set_data_pin(i, data_pin))
    
    wr_pin = await cg.gpio_pin_expression(config[CONF_WR_PIN])
    cg.add(var.set_wr_pin(wr_pin))
    
    rd_pin = await cg.gpio_pin_expression(config[CONF_RD_PIN])
    cg.add(var.set_rd_pin(rd_pin))
