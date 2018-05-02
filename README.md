# Homie Node Collection

[![Travis](https://img.shields.io/travis/luebbe/homie-node-collection.svg?branch=master&style=flat)](https://travis-ci.org/luebbe/homie-node-collection)
[![License](https://img.shields.io/github/license/mashape/apistatus.svg?style=flat)](https://opensource.org/licenses/MIT)
[![GitHub release](https://img.shields.io/github/release/luebbe/homie-node-collection.svg?style=flat)](https://github.com/luebbe/homie-node-collection/releases)

Collection of nodes and examples for Homie. The nodes are designed as independent classes, so they can work standalone. For instance you can easily wire up a firmware for a Sonoff Relay by combining a ButtonNode with a RelayNode.

The software is based on [Homie (v2.0)](https://github.com/marvinroger/homie-esp8266) and is developed using [PlatformIO](https://github.com/platformio)

## ButtonNode
A pushbutton that just detects a single button press. An optional callback can be triggered by the button press event. The button press is *not* reported back via MQTT. ToDo: detect multiple button presses and report them back.

## ContactNode
A contact that reports its open state (true|false) via MQTT. An optional callback can be triggered by the state change event.

## RelayNode
A relay that can be set on (true|false) via MQTT message. An optional GPIO pin (e.g. to light up a LED) can be passed in the constructor. This pin will be set high/low synchronous to the relay.

## BME280Node
A node for a Bosch BME280 I2C temperature/humidity/pressure sensor. Reports the three values back via MQTT.
Depending on whether the sensor could be read successfully advertises status as:
* homie/\<device-id\>/\<sensor-name\>/status (ok|error)

Advertises the values as:
* homie/\<device-id\>/\<sensor-name\>/temperature
* homie/\<device-id\>/\<sensor-name\>/humidity
* homie/\<device-id\>/\<sensor-name\>/pressure

The units are advertised as subtopics:
* homie/\<device-id\>/\<sensor-name\>/temperature/unit
* homie/\<device-id\>/\<sensor-name\>/humidity/unit
* homie/\<device-id\>/\<sensor-name\>/pressure/unit

## DHT22Node
A node for a DHT22 temperature/humidity sensor. Reports the two values back via MQTT.
Depending on whether the sensor could be read successfully advertises status as:
* homie/\<device-id\>/\<sensor-name\>/status (ok|error)

Advertises the values as:
* homie/\<device-id\>/\<sensor-name\>/temperature
* homie/\<device-id\>/\<sensor-name\>/humidity

The units are advertised as subtopics:
* homie/\<device-id\>/\<sensor-name\>/temperature/unit
* homie/\<device-id\>/\<sensor-name\>/humidity/unit

