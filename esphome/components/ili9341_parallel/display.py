import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import core, pins
from esphome.components import display
from esphome.const import (
    CONF_ID,
    CONF_LAMBDA,
    CONF_MODEL,
    CONF_PAGES,
    CONF_RAW_DATA_ID,
    CONF_RESET_PIN,
)
from esphome.core import CORE, HexInt

DEPENDENCIES = []

CONF_COLOR_PALETTE_IMAGES = "color_palette_images"
CONF_LED_PIN = "led_pin"
CONF_DATA_PINS = "data_pins"
CONF_CS_PIN = "cs_pin"
CONF_DC_PIN = "dc_pin"
CONF_WR_PIN = "wr_pin"
CONF_RD_PIN = "rd_pin"

ili9341_parallel_ns = cg.esphome_ns.namespace("ili9341_parallel")
ili9341_parallel = ili9341_parallel_ns.class_(
    "ILI9341ParallelDisplay", cg.PollingComponent, display.DisplayBuffer
)
ILI9341M5Stack = ili9341_parallel_ns.class_("ILI9341M5Stack", ili9341_parallel)
ILI9341TFT24 = ili9341_parallel_ns.class_("ILI9341TFT24", ili9341_parallel)
ILI9341TFT24R = ili9341_parallel_ns.class_("ILI9341TFT24R", ili9341_parallel)

ILI9341Model = ili9341_parallel_ns.enum("ILI9341Model")
ILI9341ColorMode = ili9341_parallel_ns.enum("ILI9341ColorMode")

MODELS = {
    "M5STACK": ILI9341Model.M5STACK,
    "TFT_2.4": ILI9341Model.TFT_24,
    "TFT_2.4R": ILI9341Model.TFT_24R,
}

ILI9341_MODEL = cv.enum(MODELS, upper=True, space="_")

COLOR_PALETTE = cv.one_of("NONE", "GRAYSCALE", "IMAGE_ADAPTIVE")

def validate_data_pins(value):
    if len(value) != 8:
        raise cv.Invalid("Exactly 8 data pins must be provided")
    return value

CONFIG_SCHEMA = cv.All(
    display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(ili9341_parallel),
            cv.Required(CONF_MODEL): ILI9341_MODEL,
            cv.Required(CONF_CS_PIN): pins.gpio_output_pin_schema,
            cv.Required(CONF_DC_PIN): pins.gpio_output_pin_schema,
            cv.Required(CONF_WR_PIN): pins.gpio_output_pin_schema,
            cv.Required(CONF_RD_PIN): pins.gpio_output_pin_schema,
            cv.Required(CONF_DATA_PINS): cv.All([pins.gpio_output_pin_schema], validate_data_pins),
            cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_LED_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_COLOR_PALETTE, default="NONE"): COLOR_PALETTE,
            cv.Optional(CONF_COLOR_PALETTE_IMAGES, default=[]): cv.ensure_list(cv.file_),
            cv.GenerateID(CONF_RAW_DATA_ID): cv.declare_id(cg.uint8),
        }
    )
    .extend(cv.polling_component_schema("1s")),
    cv.has_at_most_one_key(CONF_PAGES, CONF_LAMBDA),
)

async def to_code(config):
    if config[CONF_MODEL] == "M5STACK":
        lcd_type = ILI9341M5Stack
    if config[CONF_MODEL] == "TFT_2.4":
        lcd_type = ILI9341TFT24
    if config[CONF_MODEL] == "TFT_2.4R":
        lcd_type = ILI9341TFT24R
    var = cg.new_Pvariable(config[CONF_ID], lcd_type.new())

    await cg.register_component(var, config)
    await display.register_display(var, config)

    cg.add(var.set_model(config[CONF_MODEL]))
    
    cs = await cg.gpio_pin_expression(config[CONF_CS_PIN])
    cg.add(var.set_cs_pin(cs))
    dc = await cg.gpio_pin_expression(config[CONF_DC_PIN])
    cg.add(var.set_dc_pin(dc))
    wr = await cg.gpio_pin_expression(config[CONF_WR_PIN])
    cg.add(var.set_wr_pin(wr))
    rd = await cg.gpio_pin_expression(config[CONF_RD_PIN])
    cg.add(var.set_rd_pin(rd))

    data_pins = []
    for i, pin_config in enumerate(config[CONF_DATA_PINS]):
        pin = await cg.gpio_pin_expression(pin_config)
        cg.add(var.set_data_pin(i, pin))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayBufferRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
    if CONF_RESET_PIN in config:
        reset = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
        cg.add(var.set_reset_pin(reset))
    if CONF_LED_PIN in config:
        led_pin = await cg.gpio_pin_expression(config[CONF_LED_PIN])
        cg.add(var.set_led_pin(led_pin))

    if config[CONF_COLOR_PALETTE] == "GRAYSCALE":
        cg.add(var.set_buffer_color_mode(ILI9341ColorMode.BITS_8_INDEXED))
        rhs = [HexInt(x) for x in range(256)] * 3
    elif config[CONF_COLOR_PALETTE] == "IMAGE_ADAPTIVE":
        cg.add(var.set_buffer_color_mode(ILI9341ColorMode.BITS_8_INDEXED))
        from PIL import Image

        def load_image(filename):
            path = CORE.relative_config_path(filename)
            try:
                return Image.open(path)
            except Exception as e:
                raise core.EsphomeError(f"Could not load image file {path}: {e}")

        images = [load_image(x) for x in config[CONF_COLOR_PALETTE_IMAGES]]
        total_width = sum(i.width for i in images)
        max_height = max(i.height for i in images)

        ref_image = Image.new("RGB", (total_width, max_height))
        x = 0
        for i in images:
            ref_image.paste(i, (x, 0))
            x = x + i.width

        converted = ref_image.convert("P", palette=Image.ADAPTIVE, colors=256)
        palette = converted.getpalette()
        assert len(palette) == 256 * 3
        rhs = palette
    else:
        cg.add(var.set_buffer_color_mode(ILI9341ColorMode.BITS_8))
        rhs = None

    if rhs is not None:
        prog_arr = cg.progmem_array(config[CONF_RAW_DATA_ID], rhs)
        cg.add(var.set_palette(prog_arr))
