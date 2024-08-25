import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import display
from esphome.const import CONF_ID, CONF_WIDTH, CONF_HEIGHT

DEPENDENCIES = []
AUTO_LOAD = []

# Define the namespace and the main class for your component
ili9341_parallel_ns = cg.esphome_ns.namespace("ili9341_parallel")
ILI9341ParallelDisplay = ili9341_parallel_ns.class_("ILI9341ParallelDisplay", cg.Component, display.DisplayBuffer)

# Define constants for your configuration keys
CONF_MODEL = "model"
CONF_DC_PIN = "dc_pin"
CONF_RESET_PIN = "reset_pin"
CONF_DATA_PINS = "data_pins"
CONF_WR_PIN = "wr_pin"
CONF_RD_PIN = "rd_pin"

def validate_data_pins(value):
    if len(value) != 8:
        raise cv.Invalid("Exactly 8 data pins are required")
    return value

def validate_model(value):
    if value != "ili9341_parallel":
        raise cv.Invalid("Only ili9341_parallel model is supported")
    return value

# Define the schema for the component configuration
CONFIG_SCHEMA = display.FULL_DISPLAY_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(ILI9341ParallelDisplay),
    cv.Required(CONF_MODEL): validate_model,
    cv.Required(CONF_DC_PIN): pins.gpio_output_pin_schema,
    cv.Required(CONF_RESET_PIN): pins.gpio_output_pin_schema,
    cv.Required(CONF_DATA_PINS): cv.All(cv.ensure_list(pins.gpio_output_pin_schema), validate_data_pins),
    cv.Required(CONF_WR_PIN): pins.gpio_output_pin_schema,
    cv.Required(CONF_RD_PIN): pins.gpio_output_pin_schema,
    cv.Optional(CONF_WIDTH, default=240): cv.int_,
    cv.Optional(CONF_HEIGHT, default=320): cv.int_,
}).extend(cv.COMPONENT_SCHEMA)

# Function to convert the configuration to code
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await display.register_display(var, config)

    # Set model and pins
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

    # Set display dimensions
    cg.add(var.set_dimensions(config[CONF_WIDTH], config[CONF_HEIGHT]))

    # Optional lambdas
    if 'lambda' in config:
        lambda_ = await cg.process_lambda(
            config['lambda'], [(display.DisplayBufferRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
    
    if 'pages' in config:
        for page in config['pages']:
            if 'lambda' in page:
                lambda_ = await cg.process_lambda(
                    page['lambda'], [(display.DisplayBufferRef, "it")], return_type=cg.void
                )
                cg.add(var.add_page_lambda(lambda_))
