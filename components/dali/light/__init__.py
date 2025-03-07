from esphome.components import light, output
from esphome.const import (
    CONF_ID, CONF_OUTPUT_ID, CONF_ADDRESS, CONF_BRIGHTNESS, CONF_COLOR_TEMPERATURE,
    CONF_COLD_WHITE_COLOR_TEMPERATURE,
    CONF_WARM_WHITE_COLOR_TEMPERATURE,
)

import esphome.codegen as cg
import esphome.config_validation as cv

from .. import dali_ns, CONF_DALI_BUS, DaliBusComponent

CONF_FADE_TIME = 'fade_time'
CONF_FADE_RATE = 'fade_rate'

DaliLight = dali_ns.class_('DaliLight', light.LightOutput)

CONFIG_SCHEMA = light.LIGHT_SCHEMA.extend({
    cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(DaliLight),

    cv.Optional(CONF_COLD_WHITE_COLOR_TEMPERATURE): cv.color_temperature,
    cv.Optional(CONF_WARM_WHITE_COLOR_TEMPERATURE): cv.color_temperature,

    cv.GenerateID(CONF_DALI_BUS): cv.use_id(DaliBusComponent),
    cv.Optional(CONF_ADDRESS): cv.int_,

    # # cv.Optional(CONF_BRIGHTNESS, default=True): cv.boolean,
    # # cv.Optional(CONF_COLOR_TEMPERATURE, default=True): cv.boolean,
    # cv.Optional(CONF_FADE_TIME, default=0): cv.int_,
    # cv.Optional(CONF_FADE_RATE, default=0): cv.int_,

    #cv.Optional(CONF_TYPE, default="GRB"): validate_type,
    #cv.Required(CONF_VARIANT): cv.one_of(*CHIP_TYPES, lower=True),

    # cv.Optional(
    #     CONF_DEFAULT_TRANSITION_LENGTH, default="1s"
    # ): cv.positive_time_period_milliseconds,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    # DaliLight must be linked to DaliBusComponent
    parent = await cg.get_variable(config[CONF_DALI_BUS])

    # LightState must be linked to DaliLight (LightOutput)
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID], parent)
    await light.register_light(var, config)

    if CONF_ADDRESS in config:
        cg.add(var.set_address(config[CONF_ADDRESS]))

    if CONF_COLD_WHITE_COLOR_TEMPERATURE in config:
        cg.add(var.set_cold_white_temperature(config[CONF_COLD_WHITE_COLOR_TEMPERATURE]))
    if CONF_WARM_WHITE_COLOR_TEMPERATURE in config:
        cg.add(var.set_warm_white_temperature(config[CONF_WARM_WHITE_COLOR_TEMPERATURE]))


#     # cg.add(var.set_brightness(config[CONF_BRIGHTNESS]))
#     # cg.add(var.set_color_temperature(config[CONF_COLOR_TEMPERATURE]))
#     cg.add(var.set_fade_time(config[CONF_FADE_TIME]))
#     cg.add(var.set_fade_rate(config[CONF_FADE_RATE]))
