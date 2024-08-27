import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display
from esphome.const import CONF_ID, CONF_LAMBDA, CONF_PAGES

DEPENDENCIES = []

ili9341_parallel_ns = cg.esphome_ns.namespace("ili9xxx")
ILI9341ParallelDisplay = ili9341_parallel_ns.class_("ILI9341ParallelDisplay", display.Display)

CONFIG_SCHEMA = display.FULL_DISPLAY_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(ILI9341ParallelDisplay),
    cv.Required("data_pins"): cv.All(cv.ensure_list(cv.gpio_pin_schema), cv.Length(min=8, max=8)),
    cv.Required("wr_pin"): cv.gpio_pin_schema,
    cv.Required("rd_pin"): cv.gpio_pin_schema,
    cv.Required("dc_pin"): cv.gpio_pin_schema,
    cv.Optional("reset_pin"): cv.gpio_pin_schema,
    cv.Optional("cs_pin"): cv.gpio_pin_schema,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await display.register_display(var, config)

    data_pins = config["data_pins"]
    for i, pin in enumerate(data_pins):
        pin = await cg.gpio_pin_expression(pin)
        cg.add(var.set_data_pin(i, pin))

    wr_pin = await cg.gpio_pin_expression(config["wr_pin"])
    cg.add(var.set_wr_pin(wr_pin))

    rd_pin = await cg.gpio_pin_expression(config["rd_pin"])
    cg.add(var.set_rd_pin(rd_pin))

    dc_pin = await cg.gpio_pin_expression(config["dc_pin"])
    cg.add(var.set_dc_pin(dc_pin))

    if "reset_pin" in config:
        reset_pin = await cg.gpio_pin_expression(config["reset_pin"])
        cg.add(var.set_reset_pin(reset_pin))

    if "cs_pin" in config:
        cs_pin = await cg.gpio_pin_expression(config["cs_pin"])
        cg.add(var.set_cs_pin(cs_pin))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(config[CONF_LAMBDA], [(display.DisplayRef, "it")],
                                          return_type=cg.void)
        cg.add(var.set_writer(lambda_))

    if CONF_PAGES in config:
        pages = await display.build_pages(config[CONF_PAGES])
        cg.add(var.set_pages(pages))
