import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import CONF_ID, CONF_MIN_VALUE, CONF_MAX_VALUE, CONF_STEP, CONF_MODE
from . import thermiq_ns, ThermiqComponent

DEPENDENCIES = ["thermiq_esp32"]

CONF_THERMIQ_ID = "thermiq_id"
CONF_REGISTER = "register"

ThermiqNumber = thermiq_ns.class_("ThermiqNumber", number.Number, cg.Component)

CONFIG_SCHEMA = number.number_schema(ThermiqNumber).extend({
    cv.GenerateID(CONF_THERMIQ_ID): cv.use_id(ThermiqComponent),
    cv.Required(CONF_REGISTER): cv.hex_uint8_t,
    cv.Optional(CONF_MIN_VALUE, default=10): cv.float_,
    cv.Optional(CONF_MAX_VALUE, default=30): cv.float_,
    cv.Optional(CONF_STEP, default=1): cv.float_,
    cv.Optional(CONF_MODE, default="box"): cv.enum({"box": "box", "slider": "slider"}, lower=True),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = await number.new_number(
        config,
        min_value=config[CONF_MIN_VALUE],
        max_value=config[CONF_MAX_VALUE],
        step=config[CONF_STEP],
    )
    await cg.register_component(var, config)
    parent = await cg.get_variable(config[CONF_THERMIQ_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_register(config[CONF_REGISTER]))
