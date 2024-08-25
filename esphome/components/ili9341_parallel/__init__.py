from esphome import pins
from esphome.core import CORE
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display
from esphome.const import CONF_ID

DEPENDENCIES = ["display"]
AUTO_LOAD = ["display"]

ili9341_parallel_ns = cg.esphome_ns.namespace("ili9341_parallel")
ILI9341ParallelDisplay = ili9341_parallel_ns.class_("ILI9341ParallelDisplay", cg.Component, display.DisplayBuffer)

def validate_pin_number(value):
    valid_pins = list(range(0, 40))  # Adjust this range according to your board
    if value not in valid_pins:
        raise cv.Invalid(f"Invalid pin number {value}")
    return value

CONFIG_SCHEMA = display.DISPLAY_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(ILI9341ParallelDisplay),
})

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await display.register_display(var, config)
