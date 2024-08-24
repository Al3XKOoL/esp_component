import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import display
from esphome.const import (
    CONF_ID,
    CONF_LAMBDA,
    CONF_PAGES,
    CONF_WIDTH,
    CONF_HEIGHT,
    CONF_DC_PIN,
    CONF_RESET_PIN,
    CONF_DATA_PINS,
)

DEPENDENCIES = []
AUTO_LOAD = ["psram"]
CODEOWNERS = ["@your_github_username"]

ili9xxx_ns = cg.esphome_ns.namespace("ili9xxx")
ILI9341ParallelDisplay = ili9xxx_ns.class_(
    "ILI9341ParallelDisplay", cg.PollingComponent, display.DisplayBuffer
)

CONF_MODEL = "model"
CONF_WR_PIN = "wr_pin"
CONF_RD_PIN = "rd_pin"

CONFIG_SCHEMA = cv.All(
    display.FULL_DISPLAY_SCHEMA.extend({
        cv.GenerateID(): cv.declare_id(ILI9341ParallelDisplay),
        cv.Required(CONF_MODEL): cv.string_strict,
        cv.Required(CONF_DC_PIN): pins.gpio_output_pin_schema,
        cv.Required(CONF_RESET_PIN): pins.gpio_output_pin_schema,
        cv.Required(CONF_DATA_PINS): cv.All(cv.ensure_list(pins.gpio_output_pin_schema), cv.Length(exactly=8)),
        cv.Required(CONF_WR_PIN): pins.gpio_output_pin_schema,
        cv.Required(CONF_RD_PIN): pins.gpio_output_pin_schema,
        cv.Optional(CONF_WIDTH, default=240): cv.int_,
        cv.Optional(CONF_HEIGHT, default=320): cv.int_,
    }).extend(cv.polling_component_schema("1s")),
    cv.has_at_most_one_key(CONF_PAGES, CONF_LAMBDA),
    _validate,
)

def _validate(config):
    if config[CONF_MODEL] != "ILI9341_PARALLEL":
        raise cv.Invalid("Only ILI9341_PARALLEL model is supported")
    return config

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await display.register_display(var, config)

    cg.add(var.set_model_str(config[CONF_MODEL]))

    dc = await cg.gpio_pin_expression(config[CONF_DC_PIN])
    cg.add(var.set_dc_pin(dc))

    reset = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
    cg.add(var.set_reset_pin(reset))

    for i, pin in enumerate(config[CONF_DATA_PINS]):
        data_pin = await cg.gpio_pin_expression(pin)
        cg.add(var.set_data_pin(i, data_pin))

    wr_pin = await cg.gpio_pin_expression(config[CONF_WR_PIN])
    cg.add(var.set_wr_pin(wr_pin))

    rd_pin = await cg.gpio_pin_expression(config[CONF_RD_PIN])
    cg.add(var.set_rd_pin(rd_pin))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayBufferRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
    if CONF_PAGES in config:
        for page in config[CONF_PAGES]:
            if CONF_LAMBDA in page:
                lambda_ = await cg.process_lambda(
                    page[CONF_LAMBDA], [(display.DisplayBufferRef, "it")], return_type=cg.void
                )
                cg.add(var.add_page_lambda(lambda_))

    cg.add(var.set_dimensions(config[CONF_WIDTH], config[CONF_HEIGHT]))
