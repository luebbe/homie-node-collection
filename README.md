# Homie Node Collection

[![Languages](https://img.shields.io/github/languages/top/luebbe/homie-node-collection.svg?style=flat-square)](https://img.shields.io/github/languages/top/luebbe/homie-node-collection)
[![GitHub release](https://img.shields.io/github/release/luebbe/homie-node-collection.svg?style=flat-square)](https://github.com/luebbe/homie-node-collection/releases)
[![Travis](https://img.shields.io/travis/luebbe/homie-node-collection.svg?branch=master&style=flat-square)](https://travis-ci.org/luebbe/homie-node-collection)
[![Codacy](https://img.shields.io/codacy/grade/e6d5e648b83b4667b399b81e2986dea6?style=flat-square)](https://www.codacy.com/app/luebbe/homie-node-collection/dashboard)
[![License](https://img.shields.io/github/license/mashape/apistatus.svg?style=flat-square)](https://opensource.org/licenses/MIT)
[![PlatformIO](https://img.shields.io/static/v1?label=Powered&message=PlatformIO&color=blue&style=flat-square)](https://platformio.org/lib/show/6163/Homie%20node%20collection)
[![Homie](https://img.shields.io/static/v1?label=Powered&message=Homie&color=blue&style=flat-square)](https://github.com/homieiot/homie-esp8266)

Collection of nodes and examples for Homie. The nodes are designed as independent classes, so they can work standalone. For instance you can easily wire up a firmware for a Sonoff Relay by combining a ButtonNode with a RelayNode.

The software is based on [Homie](https://github.com/homieiot/homie-esp8266) and is developed using [PlatformIO](https://github.com/platformio)
It has recently been migrated to the [Homie v3 Develop branch](https://github.com/homieiot/homie-esp8266/tree/develop-v3).

- Releases up to 1.0.x are using the [Homie convention 2.0.1](https://github.com/homieiot/convention/releases/tag/v2.0.1)
- Releases from 1.1.x onwards are using the [Homie convention 3.0.1](https://github.com/homieiot/convention/releases/tag/v3.0.1)

## Sensor Nodes

All sensor nodes have a common `$status` subtopic wit the enum values: `ok|error`. `$status` is `ok` when a sensor could be detected and a valid measurement could be taken, `error` otherwise.
All sensor nodes publish their data type on the `$datatype` subtopic.
All sensor nodes publish their unit on the `$unit` subtopic.
Most sensor nodes publish their format/value range on the `$format` subtopic.
So if a sensor nodes publishes a temperature, you will see the following subtopics:

- `homie/<device-id>/<node-name>/status`
- `homie/<device-id>/<node-name>/temperature`
- `homie/<device-id>/<node-name>/temperature/$datatype`
- `homie/<device-id>/<node-name>/temperature/$unit`
- `homie/<device-id>/<node-name>/temperature/$format`

### AdcNode.cpp

Homie Node using the internal ESP ADC to measure voltage.

It has three settings:

- _adcCorrect_: Correction factor for AD converter.  
  Range = \[0.5 - 1.5], Default = 1.

- _battMax_: Measured voltage that corresponds to 100% battery level.  
  Must be greater than _battMin_. Range = \[2.5V .. 4.0V]. Default = 3.3V.

- _battMin_: Measured voltage that corresponds to 0% battery level.  
  Must be less than _battMax_. Range = \[2.5V .. 4.0V]. Default = 2.6V.

Advertises the values as:

- `homie/<device-id>/<node-name>/voltage`
- `homie/<device-id>/<node-name>/batterylevel`

### BME280Node

A node for Bosch BME280 I2C temperature/humidity/pressure sensors. Reports the three values back via MQTT.

It has one setting:

- _temperatureOffset_: The temperature offset in degrees.  
  Range = \[-10.0°C .. 10.0°C]. Default = 0°C.

**Attention**: This offset is just added to the temperature read from the sensor. The relative humidity is not recalculated.

**Attention**: Please be aware that the Homie framework doesn't know per-node settings. If you have more than one instance of a BME280Node, all instances will use the same _temperatureOffset_.

Advertises the values as:

- `homie/<device-id>/<node-name>/temperature`
- `homie/<device-id>/<node-name>/humidity`
- `homie/<device-id>/<node-name>/pressure`

### DHT22Node

A node for DHT22 temperature/humidity sensors. Reports the two values back via MQTT.

Advertises the values as:

- `homie/<device-id>/<node-name>/temperature`
- `homie/<device-id>/<node-name>/humidity`

### DS18B20Node

A Homie Node for Dallas 18B20 one wire temperature sensors. Reports the temperature back via MQTT.

Advertises the value as:

- `homie/<device-id>/<node-name>/temperature`

## Actor Nodes

### ButtonNode

A pushbutton that just detects a single button press. An optional callback can be triggered by the button press event. The button press is _not_ reported back via MQTT. ToDo: detect multiple button presses and report them back.

### ContactNode

A contact that reports its open state (true|false) via MQTT. An optional callback can be triggered by the state change event.

Advertises the state as:

- `homie/<device-id>/<node-name>/open` (true|false)

### PulseNode

In some way similar to the contact node only that it reacts on pulses on the selected input pin. It reports its state (true|false) via MQTT. An optional callback can be triggered by the state change event. Imagine an optocoupler pulsing with 50Hz when a switch is closed or a button is pressed.

Advertises the state as:

- `homie/<device-id>/<node-name>/active` (true|false)

In order to use the PulseNode you need an interrupt procedure which is attached to the selected pin. e.G.:

    void onOptoCouplerPulse()
    {
      pulseNode.pulseDetected();
    }

    void setup()
    {
      attachInterrupt(PIN_OPTOCOUPLER, onOptoCouplerPulse, FALLING);
    }

### RelayNode

A relay that can be set on (true|false) via MQTT message. An optional GPIO pin (e.g. to light up a LED) can be passed in the constructor. This pin will be set high/low synchronous to the relay. Additonally the relay can be turned on for a number of seconds by sending this number to the timeout subtopic.

- `homie/<device-id>/<node-name>/on/set` (true|false)
- `homie/<device-id>/<node-name>/timeout/set` (positive integer) - turns the relay on for the corresponding number of seconds.

Advertises the state as:

- `homie/<device-id>/<node-name>/on` (true|false)
- `homie/<device-id>/<node-name>/timeout/` (positive integer) - the second (uptime of the relay node) when the relay will turn off again.
