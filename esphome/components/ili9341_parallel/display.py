import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display
from esphome.const import (
    CONF_ID,
    CONF_LAMBDA,
    CONF_PAGES,
    CONF_WIDTH,
    CONF_HEIGHT,
    CONF_ROTATION,
    CONF_MODEL,
)
from esphome import pins

DEPENDENCIES = []

ili9341_parallel_ns = cg.esphome_ns.namespace("ili9xxx")
ILI9341ParallelDisplay = ili9341_parallel_ns.class_("ILI9341ParallelDisplay", display.Display)

def validate_gpio_pin(value):
    return pins.internal_gpio_output_pin_schema(value)

CONFIG_SCHEMA = display.BASIC_DISPLAY_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(ILI9341ParallelDisplay),
    cv.Required(CONF_MODEL): cv.string,
    cv.Required(CONF_WIDTH): cv.positive_int,
    cv.Required(CONF_HEIGHT): cv.positive_int,
    cv.Optional(CONF_ROTATION): cv.int_range(min=0, max=3),
    cv.Required("data_pins"): cv.All(cv.ensure_list(validate_gpio_pin), cv.Length(min=8, max=8)),
    cv.Required("wr_pin"): validate_gpio_pin,
    cv.Required("rd_pin"): validate_gpio_pin,
    cv.Required("dc_pin"): validate_gpio_pin,
    cv.Optional("reset_pin"): validate_gpio_pin,
    cv.Optional("cs_pin"): validate_gpio_pin,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await display.register_display(var, config)

    cg.add(var.set_width(config[CONF_WIDTH]))
    cg.add(var.set_height(config[CONF_HEIGHT]))
    if CONF_ROTATION in config:
        cg.add(var.set_rotation(config[CONF_ROTATION]))

    data_pins = config["data_pins"]
    for i, pin in enumerate(data_pins):
        cg.add(var.set_data_pin(i, await cg.gpio_pin_expression(pin)))

    cg.add(var.set_wr_pin(await cg.gpio_pin_expression(config["wr_pin"])))
    cg.add(var.set_dc_pin(await cg.gpio_pin_expression(config["dc_pin"])))
    
    if "rd_pin" in config:
        cg.add(var.set_rd_pin(await cg.gpio_pin_expression(config["rd_pin"])))
    if "reset_pin" in config:
        cg.add(var.set_reset_pin(await cg.gpio_pin_expression(config["reset_pin"])))
    if "cs_pin" in config:
        cg.add(var.set_cs_pin(await cg.gpio_pin_expression(config["cs_pin"])))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(config[CONF_LAMBDA], [(display.DisplayRef, "it")],
                                          return_type=cg.void)
        cg.add(var.set_writer(lambda_))

    if CONF_PAGES in config:
        pages = await display.build_pages(config[CONF_PAGES])
        cg.add(var.set_pages(pages))
