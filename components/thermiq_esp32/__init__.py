import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

DEPENDENCIES = ["wifi"]
AUTO_LOAD = []

thermiq_ns = cg.esphome_ns.namespace("thermiq_esp32")
ThermiqComponent = thermiq_ns.class_("ThermiqComponent", cg.Component)

CONF_MQTT_HOST = "mqtt_host"
CONF_MQTT_PORT = "mqtt_port"
CONF_MQTT_USERNAME = "mqtt_username"
CONF_MQTT_PASSWORD = "mqtt_password"
CONF_DEVICE_ID = "device_id"
CONF_DEVICE_NAME = "device_name"
CONF_DISCOVERY_PREFIX = "discovery_prefix"
CONF_BASE_TOPIC = "base_topic"
CONF_THERMIQ_BASE_TOPIC = "thermiq_base_topic"
CONF_SDA_PIN = "sda_pin"
CONF_SCL_PIN = "scl_pin"
CONF_I2C_ADDRESS = "i2c_address"
CONF_POLL_INTERVAL = "poll_interval"
CONF_PUBLISH_INTERVAL = "publish_interval"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ThermiqComponent),
    cv.Required(CONF_MQTT_HOST): cv.string,
    cv.Optional(CONF_MQTT_PORT, default=1883): cv.port,
    cv.Optional(CONF_MQTT_USERNAME, default=""): cv.string,
    cv.Optional(CONF_MQTT_PASSWORD, default=""): cv.string,
    cv.Optional(CONF_DEVICE_ID, default="thermia_esp32"): cv.string,
    cv.Optional(CONF_DEVICE_NAME, default="Thermia ESP32"): cv.string,
    cv.Optional(CONF_DISCOVERY_PREFIX, default="homeassistant"): cv.string,
    cv.Optional(CONF_BASE_TOPIC, default="thermia/esp32"): cv.string,
    cv.Optional(CONF_THERMIQ_BASE_TOPIC, default="ThermIQ/ThermIQ-mqtt"): cv.string,
    cv.Optional(CONF_SDA_PIN, default=21): cv.int_range(min=0, max=39),
    cv.Optional(CONF_SCL_PIN, default=22): cv.int_range(min=0, max=39),
    cv.Optional(CONF_I2C_ADDRESS, default=0x2E): cv.hex_uint8_t,
    cv.Optional(CONF_POLL_INTERVAL, default="300s"): cv.positive_time_period_milliseconds,
    cv.Optional(CONF_PUBLISH_INTERVAL, default="30s"): cv.positive_time_period_milliseconds,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    cg.add_library("knolleary/PubSubClient", "2.8")
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_mqtt_host(config[CONF_MQTT_HOST]))
    cg.add(var.set_mqtt_port(config[CONF_MQTT_PORT]))
    cg.add(var.set_mqtt_username(config[CONF_MQTT_USERNAME]))
    cg.add(var.set_mqtt_password(config[CONF_MQTT_PASSWORD]))
    cg.add(var.set_device_id(config[CONF_DEVICE_ID]))
    cg.add(var.set_device_name(config[CONF_DEVICE_NAME]))
    cg.add(var.set_discovery_prefix(config[CONF_DISCOVERY_PREFIX]))
    cg.add(var.set_base_topic(config[CONF_BASE_TOPIC]))
    cg.add(var.set_thermiq_base_topic(config[CONF_THERMIQ_BASE_TOPIC]))
    cg.add(var.set_sda_pin(config[CONF_SDA_PIN]))
    cg.add(var.set_scl_pin(config[CONF_SCL_PIN]))
    cg.add(var.set_i2c_address(config[CONF_I2C_ADDRESS]))
    cg.add(var.set_poll_interval_ms(config[CONF_POLL_INTERVAL]))
    cg.add(var.set_publish_interval_ms(config[CONF_PUBLISH_INTERVAL]))
