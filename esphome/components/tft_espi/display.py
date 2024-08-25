import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display
from esphome.const import CONF_ID

DEPENDENCIES = ['esp32']

tft_espi_ns = cg.esphome_ns.namespace('tft_espi')
TFTeSPIDisplay = tft_espi_ns.class_('TFTeSPIDisplay', display.Display)

CONFIG_SCHEMA = display.BASIC_DISPLAY_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(TFTeSPIDisplay),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await display.register_display(var, config)
