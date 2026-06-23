import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_CURRENT,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_AMPERE,
    PERCENT,
)
from . import thermiq_ns, ThermiqComponent

DEPENDENCIES = ["thermiq_esp32"]

CONF_THERMIQ_ID = "thermiq_id"
CONF_REGISTER = "register"
ThermiqSensor = sensor.Sensor

SENSOR_SCHEMA = sensor.sensor_schema(
    accuracy_decimals=0,
    state_class=STATE_CLASS_MEASUREMENT,
).extend({
    cv.GenerateID(CONF_THERMIQ_ID): cv.use_id(ThermiqComponent),
    cv.Required(CONF_REGISTER): cv.hex_uint8_t,
})

CONFIG_SCHEMA = cv.All(cv.ensure_list(SENSOR_SCHEMA))

async def to_code(config):
    for conf in config:
        parent = await cg.get_variable(conf[CONF_THERMIQ_ID])
        sens = await sensor.new_sensor(conf)
        cg.add(parent.register_sensor(conf[CONF_REGISTER], sens))
