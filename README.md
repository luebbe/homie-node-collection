# Homie Node Collection

[![Travis](https://img.shields.io/travis/luebbe/homie-node-collection.svg?branch=master&style=flat)](https://travis-ci.org/luebbe/homie-node-collection)
[![Codacy](https://api.codacy.com/project/badge/Grade/e6d5e648b83b4667b399b81e2986dea6)](https://www.codacy.com/app/luebbe/homie-node-collection/dashboard)
[![License](https://img.shields.io/github/license/mashape/apistatus.svg?style=flat)](https://opensource.org/licenses/MIT)
[![GitHub release](https://img.shields.io/github/release/luebbe/homie-node-collection.svg?style=flat)](https://github.com/luebbe/homie-node-collection/releases)

Collection of nodes and examples for Homie. The nodes are designed as independent classes, so they can work standalone. For instance you can easily wire up a firmware for a Sonoff Relay by combining a ButtonNode with a RelayNode.

The software is based on [Homie (v2.0)](https://github.com/homieiot/homie-esp8266) and is developed using [PlatformIO](https://github.com/platformio)

## Sensor Nodes

### AdcNode.cpp

Homie Node using the internal ESP ADC to measure voltage.

It has three settings:

- _adcCorrect_: Correction factor for AD converter.  
  [0.5 - 1.5], Default = 1
- _battMax_: Measured voltage that corresponds to 100% battery level.  
  Must be greater than _battMin_. Range: [2.5V - 4.0V] Default = 3.3V.
- _battMin_: Measured voltage that corresponds to 0% battery level.  
  Must be less than _battMax_. Range: [2.5V - 4.0V] Default = 2.6V.

Advertises the values as:

- `homie/<device-id>/<node-name>/voltage`
- `homie/<device-id>/<node-name>/batterylevel`

The units are advertised as subtopics:

- `homie/<device-id>/<node-name>/voltage/unit`
- `homie/<device-id>/<node-name>/batterylevel/unit`

### BME280Node

A node for a Bosch BME280 I2C temperature/humidity/pressure sensor. Reports the three values back via MQTT.

It has one setting:

- _temperatureOffset_: The temperature offset in degrees .  
  Range: [-10.0 - 10.0]. Default = 0.

**Attention**: This offset is just added to the temperature read from the sensor. The relative humidity is not recalculated.

**Attention**: Please be aware that the Homie framework doesn't know per-node settings. If you have more than one instance of a BME280Node, all instances will use the same _temperatureOffset_.

Depending on whether the sensor could be read successfully advertises status as:

- `homie/<device-id>/<node-name>/status` (ok|error)

Advertises the values as:

- `homie/<device-id>/<node-name>/temperature`
- `homie/<device-id>/<node-name>/humidity`
- `homie/<device-id>/<node-name>/pressure`

The units are advertised as subtopics:

- `homie/<device-id>/<node-name>/temperature/unit`
- `homie/<device-id>/<node-name>/humidity/unit`
- `homie/<device-id>/<node-name>/pressure/unit`

### DHT22Node

A node for a DHT22 temperature/humidity sensor. Reports the two values back via MQTT.
Depending on whether the sensor could be read successfully advertises status as:

- `homie/<device-id>/<node-name>/status` (ok|error)

Advertises the values as:

- `homie/<device-id>/<node-name>/temperature`
- `homie/<device-id>/<node-name>/humidity`

The units are advertised as subtopics:

- `homie/<device-id>/<node-name>/temperature/unit`
- `homie/<device-id>/<node-name>/humidity/unit`

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

A relay that can be set on (true|false) via MQTT message. An optional GPIO pin (e.g. to light up a LED) can be passed in the constructor. This pin will be set high/low synchronous to the relay.

- `homie/<device-id>/<node-name>/on/set` (true|false)

Advertises the state as:

- `homie/<device-id>/<node-name>/on` (true|false)
