# display.py
import esphome.codegen as cg
from esphome import pins
from esphome.components.ili9341_parallel import display
from esphome.const import (
    CONF_CS_PIN,
    CONF_DC_PIN,
    CONF_RST_PIN,
    CONF_WIDTH,
    CONF_HEIGHT,
)

DEPENDENCIES = ['gpio']

ili9341_parallel_ns = cg.esphome_ns.namespace('ili9341_parallel')

ILI9341ParallelComponent = ili9341_parallel_ns.class_('ILI9341ParallelComponent', cg.Component)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield cg.register_gpio(var, config[CONF_CS_PIN], 'cs_pin')
    yield cg.register_gpio(var, config[CONF_DC_PIN], 'dc_pin')
    yield cg.register_gpio(var, config[CONF_RST_PIN], 'rst_pin')
    cg.add(var.set_width(config[CONF_WIDTH]))
    cg.add(var.set_height(config[CONF_HEIGHT]))
