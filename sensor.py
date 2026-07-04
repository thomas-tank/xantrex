import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor
from esphome.const import CONF_ID, CONF_UART_ID

DEPENDENCIES = ['uart']

xantrex_ns = cg.esphome_ns.namespace('xantrex_custom')
Xantrex = xantrex_ns.class_('Xantrex', cg.Component, uart.UARTDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(Xantrex),
    cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent),
    cv.Optional("kwhlife"): sensor.sensor_schema(),
    cv.Optional("kwhtoday"): sensor.sensor_schema(),
    cv.Optional("pin"): sensor.sensor_schema(),
    cv.Optional("pout"): sensor.sensor_schema(),
    cv.Optional("vin"): sensor.sensor_schema(),
    cv.Optional("vout"): sensor.sensor_schema(),
    cv.Optional("iin"): sensor.sensor_schema(),
    cv.Optional("iout"): sensor.sensor_schema(),
    cv.Optional("freq"): sensor.sensor_schema(),
    cv.Optional("time"): sensor.sensor_schema(),
    cv.Optional("temp"): sensor.sensor_schema(),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    for key in ["kwhlife", "kwhtoday", "pin", "pout", "vin", "vout", "iin", "iout", "freq", "time", "temp"]:
        if key in config:
            sens = await sensor.new_sensor(config[key])
            cg.add(getattr(var, f"set_{key}_sensor")(sens))
