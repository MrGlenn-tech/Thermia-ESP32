import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from esphome.const import CONF_ID
from . import thermiq_ns, ThermiqComponent

DEPENDENCIES = ["thermiq_esp32"]

CONF_THERMIQ_ID = "thermiq_id"
ThermiqModeSelect = thermiq_ns.class_("ThermiqModeSelect", select.Select, cg.Component)

CONFIG_SCHEMA = select.select_schema(ThermiqModeSelect).extend({
    cv.GenerateID(CONF_THERMIQ_ID): cv.use_id(ThermiqComponent),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = await select.new_select(config, options=["Auto", "Heat pump", "Auxiliary", "Hot water"])
    await cg.register_component(var, config)
    parent = await cg.get_variable(config[CONF_THERMIQ_ID])
    cg.add(var.set_parent(parent))
