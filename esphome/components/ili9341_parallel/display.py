# esphome/components/ili9341_parallel/display.py

import esphome.codegen as cg
from esphome import config_validation as cv
from esphome.components import display
from esphome.const import CONF_ID

CONF_CS_PIN = "cs_pin"
CONF_DC_PIN = "dc_pin"
CONF_RESET_PIN = "reset_pin"
CONF_D0_PIN = "d0_pin"
CONF_D1_PIN = "d1_pin"
CONF_D2_PIN = "d2_pin"
CONF_D3_PIN = "d3_pin"
CONF_D4_PIN = "d4_pin"
CONF_D5_PIN = "d5_pin"
CONF_D6_PIN = "d6_pin"
CONF_D7_PIN = "d7_pin"

ili9341_parallel_ns = cg.esphome_ns.namespace('ili9341_parallel')
ILI9341ParallelDisplay = ili9341_parallel_ns.class_('ILI9341ParallelDisplay', display.DisplayBuffer, cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ILI9341ParallelDisplay),
    cv.Required(CONF_CS_PIN): cv.positive_int,
    cv.Required(CONF_DC_PIN): cv.positive_int,
    cv.Required(CONF_RESET_PIN): cv.positive_int,
    cv.Required(CONF_D0_PIN): cv.positive_int,
    cv.Required(CONF_D1_PIN): cv.positive_int,
    cv.Required(CONF_D2_PIN): cv.positive_int,
    cv.Required(CONF_D3_PIN): cv.positive_int,
    cv.Required(CONF_D4_PIN): cv.positive_int,
    cv.Required(CONF_D5_PIN): cv.positive_int,
    cv.Required(CONF_D6_PIN): cv.positive_int,
    cv.Required(CONF_D7_PIN): cv.positive_int,
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_cs_pin(config[CONF_CS_PIN]))
    cg.add(var.set_dc_pin(config[CONF_DC_PIN]))
    cg.add(var.set_reset_pin(config[CONF_RESET_PIN]))
    cg.add(var.set_d0_pin(config[CONF_D0_PIN]))
    cg.add(var.set_d1_pin(config[CONF_D1_PIN]))
    cg.add(var.set_d2_pin(config[CONF_D2_PIN]))
    cg.add(var.set_d3_pin(config[CONF_D3_PIN]))
    cg.add(var.set_d4_pin(config[CONF_D4_PIN]))
    cg.add(var.set_d5_pin(config[CONF_D5_PIN]))
    cg.add(var.set_d6_pin(config[CONF_D6_PIN]))
    cg.add(var.set_d7_pin(config[CONF_D7_PIN]))

    # Register the display
    yield cg.register_component(var, config)
    yield display.register_display(var, config)
