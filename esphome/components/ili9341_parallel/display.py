import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import core, pins
from esphome.components import display, font
from esphome.core import CORE, HexInt
from esphome.const import (
    CONF_COLOR_PALETTE,
    CONF_DC_PIN,
    CONF_ID,
    CONF_LAMBDA,
    CONF_MODEL,
    CONF_RAW_DATA_ID,
    CONF_PAGES,
    CONF_RESET_PIN,
    CONF_DIMENSIONS,
    CONF_WIDTH,
    CONF_HEIGHT,
    CONF_ROTATION,
    CONF_MIRROR_X,
    CONF_MIRROR_Y,
    CONF_SWAP_XY,
    CONF_COLOR_ORDER,
    CONF_OFFSET_HEIGHT,
    CONF_OFFSET_WIDTH,
    CONF_TRANSFORM,
    CONF_INVERT_COLORS,
)

DEPENDENCIES = []

def AUTO_LOAD():
    if CORE.is_esp32:
        return ["psram"]
    return []

CODEOWNERS = ["@nielsnl68", "@clydebarrow"]

ili9xxx_ns = cg.esphome_ns.namespace("ili9xxx")
ILI9XXXDisplay = ili9xxx_ns.class_(
    "ILI9XXXDisplay",
    cg.PollingComponent,
    display.Display,
)

PixelMode = ili9xxx_ns.enum("PixelMode")
PIXEL_MODES = {
    "16bit": PixelMode.PIXEL_MODE_16,
    "18bit": PixelMode.PIXEL_MODE_18,
}

ILI9XXXColorMode = ili9xxx_ns.enum("ILI9XXXColorMode")
ColorOrder = display.display_ns.enum("ColorMode")

MODELS = {
    "ILI9341_PARALLEL": ili9xxx_ns.class_("ILI9XXXILI9341Parallel", ILI9XXXDisplay),
    "CUSTOM": ILI9XXXDisplay,
}

COLOR_ORDERS = {
    "RGB": ColorOrder.COLOR_ORDER_RGB,
    "BGR": ColorOrder.COLOR_ORDER_BGR,
}

COLOR_PALETTE = cv.one_of("NONE", "GRAYSCALE", "IMAGE_ADAPTIVE")

CONF_LED_PIN = "led_pin"
CONF_COLOR_PALETTE_IMAGES = "color_palette_images"
CONF_PIXEL_MODE = "pixel_mode"
CONF_INIT_SEQUENCE = "init_sequence"
CONF_DATA_PINS = "data_pins"
CONF_WR_PIN = "wr_pin"
CONF_RD_PIN = "rd_pin"

def cmd(c, *args):
    return [c, len(args)] + list(args)

def map_sequence(value):
    if len(value) == 0:
        raise cv.Invalid("Empty sequence")
    return cmd(*value)

def _validate(config):
    if (
        config.get(CONF_COLOR_PALETTE) == "IMAGE_ADAPTIVE"
        and CONF_COLOR_PALETTE_IMAGES not in config
    ):
        raise cv.Invalid(
            "IMAGE_ADAPTIVE palette requires at least one 'color_palette_images' entry"
        )
    if (
        config.get(CONF_COLOR_PALETTE_IMAGES)
        and config.get(CONF_COLOR_PALETTE) != "IMAGE_ADAPTIVE"
    ):
        raise cv.Invalid(
            "Providing color palette images requires palette mode to be 'IMAGE_ADAPTIVE'"
        )
    model = config[CONF_MODEL]
    if CORE.is_esp8266 and model not in [
        "ILI9341_PARALLEL",
    ]:
        raise cv.Invalid("Selected model can't run on ESP8266.")

    if model == "CUSTOM":
        if CONF_INIT_SEQUENCE not in config or CONF_DIMENSIONS not in config:
            raise cv.Invalid("CUSTOM model requires init_sequence and dimensions")

    if model == "ILI9341_PARALLEL":
        if CONF_DATA_PINS not in config or CONF_WR_PIN not in config or CONF_RD_PIN not in config:
            raise cv.Invalid("ILI9341_PARALLEL model requires data_pins, wr_pin, and rd_pin")

    return config

def validate_rotation(value):
    if value not in [0, 90, 180, 270]:
        raise cv.Invalid(value)
    return value

CONFIG_SCHEMA = cv.All(
    font.validate_pillow_installed,
    display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(ILI9XXXDisplay),
            cv.Required(CONF_MODEL): cv.enum(MODELS, upper=True, space="_"),
            cv.Optional(CONF_PIXEL_MODE): cv.enum(PIXEL_MODES),
            cv.Optional(CONF_DIMENSIONS): cv.Any(
                cv.dimensions,
                cv.Schema(
                    {
                        cv.Required(CONF_WIDTH): cv.int_,
                        cv.Required(CONF_HEIGHT): cv.int_,
                        cv.Optional(CONF_OFFSET_HEIGHT, default=0): cv.int_,
                        cv.Optional(CONF_OFFSET_WIDTH, default=0): cv.int_,
                    }
                ),
            ),
            cv.Optional(CONF_DC_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_COLOR_PALETTE, default="NONE"): COLOR_PALETTE,
            cv.GenerateID(CONF_RAW_DATA_ID): cv.declare_id(cg.uint8),
            cv.Optional(CONF_COLOR_PALETTE_IMAGES, default=[]): cv.ensure_list(
                cv.file_
            ),
            cv.Optional(CONF_INVERT_COLORS): cv.boolean,
            cv.Optional(CONF_COLOR_ORDER): cv.one_of(*COLOR_ORDERS.keys(), upper=True),
            cv.Exclusive(CONF_ROTATION, CONF_ROTATION): validate_rotation,
            cv.Exclusive(CONF_TRANSFORM, CONF_ROTATION): cv.Schema(
                {
                    cv.Optional(CONF_SWAP_XY, default=False): cv.boolean,
                    cv.Optional(CONF_MIRROR_X, default=False): cv.boolean,
                    cv.Optional(CONF_MIRROR_Y, default=False): cv.boolean,
                }
            ),
            cv.Optional(CONF_INIT_SEQUENCE): cv.ensure_list(map_sequence),
            cv.Optional(CONF_DATA_PINS): cv.All(cv.ensure_list(pins.gpio_output_pin_schema), cv.Length(min=8, max=8)),
            cv.Optional(CONF_WR_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_RD_PIN): pins.gpio_output_pin_schema,
        }
    )
    .extend(cv.polling_component_schema("1s")),
    cv.has_at_most_one_key(CONF_PAGES, CONF_LAMBDA),
    _validate,
)

async def to_code(config):
    rhs = MODELS[config[CONF_MODEL]].new()
    var = cg.Pvariable(config[CONF_ID], rhs)

    await display.register_display(var, config)

    if config[CONF_MODEL] == "ILI9341_PARALLEL":
        for i, pin in enumerate(config[CONF_DATA_PINS]):
            data_pin = await cg.gpio_pin_expression(pin)
            cg.add(var.set_data_pin(i, data_pin))
        
        wr_pin = await cg.gpio_pin_expression(config[CONF_WR_PIN])
        cg.add(var.set_wr_pin(wr_pin))
        
        rd_pin = await cg.gpio_pin_expression(config[CONF_RD_PIN])
        cg.add(var.set_rd_pin(rd_pin))
        
    dc = await cg.gpio_pin_expression(config[CONF_DC_PIN])
    cg.add(var.set_dc_pin(dc))
    
    if init_sequences := config.get(CONF_INIT_SEQUENCE):
        sequence = []
        for seq in init_sequences:
            sequence.extend(seq)
        cg.add(var.add_init_sequence(sequence))

    if pixel_mode := config.get(CONF_PIXEL_MODE):
        cg.add(var.set_pixel_mode(pixel_mode))
    if CONF_COLOR_ORDER in config:
        cg.add(var.set_color_order(COLOR_ORDERS[config[CONF_COLOR_ORDER]]))
    if CONF_TRANSFORM in config:
        transform = config[CONF_TRANSFORM]
        cg.add(var.set_swap_xy(transform[CONF_SWAP_XY]))
        cg.add(var.set_mirror_x(transform[CONF_MIRROR_X]))
        cg.add(var.set_mirror_y(transform[CONF_MIRROR_Y]))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))

    if CONF_RESET_PIN in config:
        reset = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
        cg.add(var.set_reset_pin(reset))

    if CONF_DIMENSIONS in config:
        dimensions = config[CONF_DIMENSIONS]
        if isinstance(dimensions, dict):
            cg.add(var.set_dimensions(dimensions[CONF_WIDTH], dimensions[CONF_HEIGHT]))
            cg.add(
                var.set_offsets(
                    dimensions[CONF_OFFSET_WIDTH], dimensions[CONF_OFFSET_HEIGHT]
                )
            )
        else:
            (width, height) = dimensions
            cg.add(var.set_dimensions(width, height))

    rhs = None
    if config[CONF_COLOR_PALETTE] == "GRAYSCALE":
        cg.add(var.set_buffer_color_mode(ILI9XXXColorMode.BITS_8_INDEXED))
        rhs = []
        for x in range(256):
            rhs.extend([HexInt(x), HexInt(x), HexInt(x)])
    elif config[CONF_COLOR_PALETTE] == "IMAGE_ADAPTIVE":
        cg.add(var.set_buffer_color_mode(ILI9XXXColorMode.BITS_8_INDEXED))
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

        converted = ref_image.convert("P", palette=Image.Palette.ADAPTIVE, colors=256)
        palette = converted.getpalette()
        assert len(palette) == 256 * 3
        rhs = palette
    else:
        cg.add(var.set_buffer_color_mode(ILI9XXXColorMode.BITS_16))

    if rhs is not None:
        prog_arr = cg.progmem_array(config[CONF_RAW_DATA_ID], rhs)
        cg.add(var.set_palette(prog_arr))

    if CONF_INVERT_COLORS in config:
        cg.add(var.invert_colors(config[CONF_INVERT_COLORS]))
