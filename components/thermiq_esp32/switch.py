import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_ID, CONF_TYPE
from . import thermiq_ns, ThermiqComponent

DEPENDENCIES = ["thermiq_esp32"]

CONF_THERMIQ_ID = "thermiq_id"
ThermiqPollSwitch = thermiq_ns.class_("ThermiqPollSwitch", switch.Switch, cg.Component)
ThermiqEvuSwitch = thermiq_ns.class_("ThermiqEvuSwitch", switch.Switch, cg.Component)

CONFIG_SCHEMA = switch.switch_schema().extend({
    cv.GenerateID(): cv.declare_id(ThermiqPollSwitch),
    cv.GenerateID(CONF_THERMIQ_ID): cv.use_id(ThermiqComponent),
    cv.Required(CONF_TYPE): cv.enum({"poll": "poll", "evu": "evu"}, lower=True),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    cls = ThermiqPollSwitch if config[CONF_TYPE] == "poll" else ThermiqEvuSwitch
    var = cg.new_Pvariable(config[CONF_ID], cls)
    await switch.register_switch(var, config)
    await cg.register_component(var, config)
    parent = await cg.get_variable(config[CONF_THERMIQ_ID])
    cg.add(var.set_parent(parent))
