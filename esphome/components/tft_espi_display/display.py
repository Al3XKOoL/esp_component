import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display
from esphome.const import CONF_ID, CONF_LAMBDA, CONF_ROTATION, CONF_INVERT

DEPENDENCIES = ['tft_espi']

tft_espi_ns = cg.esphome_ns.namespace('tft_espi')
TftEspiComponent = tft_espi_ns.class_('TftEspiComponent', display.DisplayBuffer, cg.Component)

CONFIG_SCHEMA = display.FULL_DISPLAY_SCHEMA.extend({
    cv.Required(CONF_ID): cv.declare_id(TftEspiComponent),
    cv.Optional(CONF_ROTATION, default=0): cv.int_range(min=0, max=3),
    cv.Optional(CONF_INVERT, default=False): cv.boolean,
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    # Configurar rotación
    if CONF_ROTATION in config:
        cg.add(var.set_rotation(config[CONF_ROTATION]))

    # Configurar inversión de colores
    if CONF_INVERT in config:
        cg.add(var.set_invert(config[CONF_INVERT]))

    if CONF_LAMBDA in config:
        cg.add(var.set_writer(config[CONF_LAMBDA]))

    yield cg.register_component(var, config)
    yield display.register_display(var, config)
