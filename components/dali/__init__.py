from esphome import pins
from esphome.components import uart
from esphome.const import CONF_ID, CONF_RX_PIN, CONF_TX_PIN
from esphome.core import CORE

import os.path
import esphome.codegen as cg
import esphome.config_validation as cv

CONF_DALI_BUS = 'dali_bus'

dali_ns = cg.esphome_ns.namespace('dali')
dali_lib_ns = cg.global_ns #.namespace('dali')
DaliBusComponent = dali_ns.class_('DaliBusComponent', cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DaliBusComponent),
    cv.Required(CONF_RX_PIN): pins.gpio_input_pin_schema,
    cv.Required(CONF_TX_PIN): pins.gpio_output_pin_schema,
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)

    #workspace_path = os.path.join(os.path.dirname(CORE.config_path))
    #cg.add_library('dali', '1.0.0', repository=f'file://{workspace_path}')

    rx_pin = yield cg.gpio_pin_expression(config[CONF_RX_PIN])
    cg.add(var.set_rx_pin(rx_pin))
    
    tx_pin = yield cg.gpio_pin_expression(config[CONF_TX_PIN])
    cg.add(var.set_tx_pin(tx_pin))
