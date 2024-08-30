import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display
from esphome.const import (
    CONF_ID,
    CONF_LAMBDA,
    CONF_PAGES,
)
from esphome import pins

DEPENDENCIES = []
AUTO_LOAD = ["psram"]

ili9xxx_ns = cg.esphome_ns.namespace("ili9xxx")
ILI9341ParallelDisplay = ili9xxx_ns.class_(
    "ILI9341ParallelDisplay", display.DisplayBuffer
)

CONFIG_SCHEMA = display.BASIC_DISPLAY_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(ILI9341ParallelDisplay),
        cv.Required("data_pins"): cv.All(cv.ensure_list(pins.internal_gpio_output_pin_schema), cv.Length(min=8, max=8)),
        cv.Required("dc_pin"): pins.internal_gpio_output_pin_schema,
        cv.Required("wr_pin"): pins.internal_gpio_output_pin_schema,
        cv.Required("rd_pin"): pins.internal_gpio_output_pin_schema,
        cv.Optional("reset_pin"): pins.internal_gpio_output_pin_schema,
        cv.Optional("cs_pin"): pins.internal_gpio_output_pin_schema,
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await display.register_display(var, config)

    for i, pin in enumerate(config["data_pins"]):
        cg.add(var.set_data_pin(i, await cg.gpio_pin_expression(pin)))
    
    cg.add(var.set_dc_pin(await cg.gpio_pin_expression(config["dc_pin"])))
    cg.add(var.set_wr_pin(await cg.gpio_pin_expression(config["wr_pin"])))
    cg.add(var.set_rd_pin(await cg.gpio_pin_expression(config["rd_pin"])))
    
    if "reset_pin" in config:
        cg.add(var.set_reset_pin(await cg.gpio_pin_expression(config["reset_pin"])))
    if "cs_pin" in config:
        cg.add(var.set_cs_pin(await cg.gpio_pin_expression(config["cs_pin"])))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayBuffer.operator("ref"), "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
