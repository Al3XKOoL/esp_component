import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import display
from esphome.const import (
    CONF_ID,
    CONF_LAMBDA,
    CONF_PAGES,
    CONF_RESET_PIN,
    CONF_DIMENSIONS,
    CONF_WIDTH,
    CONF_HEIGHT,
    CONF_ROTATION,
)

DEPENDENCIES = []

CODEOWNERS = ["@yourname"]

ili9341_parallel_ns = cg.esphome_ns.namespace("ili9341_parallel")
ILI9341ParallelDisplay = ili9341_parallel_ns.class_(
    "ILI9341ParallelDisplay",
    cg.PollingComponent,
    display.DisplayBuffer,
)

CONF_DC_PIN = "dc_pin"
CONF_WR_PIN = "wr_pin"
CONF_RD_PIN = "rd_pin"
CONF_CS_PIN = "cs_pin"
CONF_DATA_PINS = "data_pins"

CONFIG_SCHEMA = cv.All(
    display.FULL_DISPLAY_SCHEMA.extend({
        cv.GenerateID(): cv.declare_id(ILI9341ParallelDisplay),
        cv.Required(CONF_DC_PIN): pins.gpio_output_pin_schema,
        cv.Required(CONF_WR_PIN): pins.gpio_output_pin_schema,
        cv.Required(CONF_RD_PIN): pins.gpio_output_pin_schema,
        cv.Required(CONF_CS_PIN): pins.gpio_output_pin_schema,
        cv.Required(CONF_DATA_PINS): cv.All(
            [pins.gpio_output_pin_schema],
            cv.Length(min=8, max=8),
        ),
        cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
        cv.Optional(CONF_DIMENSIONS): cv.dimensions,
    }).extend(cv.polling_component_schema("1s")),
    cv.has_at_most_one_key(CONF_PAGES, CONF_LAMBDA),
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await display.register_display(var, config)

    dc = await cg.gpio_pin_expression(config[CONF_DC_PIN])
    cg.add(var.set_dc_pin(dc))

    wr = await cg.gpio_pin_expression(config[CONF_WR_PIN])
    cg.add(var.set_wr_pin(wr))

    rd = await cg.gpio_pin_expression(config[CONF_RD_PIN])
    cg.add(var.set_rd_pin(rd))

    cs = await cg.gpio_pin_expression(config[CONF_CS_PIN])
    cg.add(var.set_cs_pin(cs))

    data_pins = []
    for conf in config[CONF_DATA_PINS]:
        pin = await cg.gpio_pin_expression(conf)
        data_pins.append(pin)
    cg.add(var.set_data_pins(*data_pins))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayBuffer, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))

    if CONF_RESET_PIN in config:
        reset = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
        cg.add(var.set_reset_pin(reset))

    if CONF_DIMENSIONS in config:
        dimensions = config[CONF_DIMENSIONS]
        if isinstance(dimensions, dict):
            width = dimensions[CONF_WIDTH]
            height = dimensions[CONF_HEIGHT]
        else:
            width, height = dimensions
        cg.add(var.set_dimensions(width, height))
