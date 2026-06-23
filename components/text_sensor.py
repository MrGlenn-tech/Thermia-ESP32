import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import CONF_ID
from . import thermiq_ns, ThermiqComponent

DEPENDENCIES = ["thermiq_esp32"]

CONF_THERMIQ_ID = "thermiq_id"
CONF_TYPE = "type"

CONFIG_SCHEMA = text_sensor.text_sensor_schema().extend({
    cv.GenerateID(CONF_THERMIQ_ID): cv.use_id(ThermiqComponent),
    cv.Required(CONF_TYPE): cv.enum({
        "mode": "mode",
        "uptime": "uptime",
        "log": "log",
    }, lower=True),
})

async def to_code(config):
    parent = await cg.get_variable(config[CONF_THERMIQ_ID])
    sens = await text_sensor.new_text_sensor(config)
    if config[CONF_TYPE] == "mode":
        cg.add(parent.register_mode_text_sensor(sens))
    elif config[CONF_TYPE] == "uptime":
        cg.add(parent.register_uptime_text_sensor(sens))
    elif config[CONF_TYPE] == "log":
        cg.add(parent.register_log_text_sensor(sens))
