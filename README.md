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

- `homie/<device-id>/<node-id>/status`
- `homie/<device-id>/<node-id>/temperature`
- `homie/<device-id>/<node-id>/temperature/$datatype`
- `homie/<device-id>/<node-id>/temperature/$unit`
- `homie/<device-id>/<node-id>/temperature/$format`

### AdcNode.cpp

Homie Node using the internal ESP ADC to measure voltage.

It has three settings:

- _adcCorrect_: Correction factor for AD converter.  
  Range = \[0.5 .. 1.5], Default = 1.

- _battMax_: Measured voltage that corresponds to 100% battery level.  
  Must be greater than _battMin_. Range = \[2.5V .. 4.0V]. Default = 3.3V.

- _battMin_: Measured voltage that corresponds to 0% battery level.  
  Must be less than _battMax_. Range = \[2.5V .. 4.0V]. Default = 2.6V.

Advertises the values as:

- `homie/<device-id>/<node-id>/voltage`
- `homie/<device-id>/<node-id>/batterylevel`

### BME280Node

A node for Bosch BME280 I2C temperature/humidity/pressure sensors. Reports the three values back via MQTT.

It has one setting:

- _\<node-id>.temperatureOffset_: The temperature offset in degrees.  
  Range = \[-10.0°C .. 10.0°C]. Default = 0°C.  
  This is a per node setting, so pay attention that the node names are different.

**Attention**: This offset is just added to the temperature read from the sensor. The relative humidity is not recalculated.

Advertises the values as:

- `homie/<device-id>/<node-id>/temperature`
- `homie/<device-id>/<node-id>/humidity`
- `homie/<device-id>/<node-id>/pressure`

### DHT22Node

A node for DHT22 temperature/humidity sensors. Reports the two values back via MQTT.

Advertises the values as:

- `homie/<device-id>/<node-id>/temperature`
- `homie/<device-id>/<node-id>/humidity`

### DS18B20Node

A Homie Node for Dallas 18B20 one wire temperature sensors. Reports the temperature back via MQTT.

Advertises the value as:

- `homie/<device-id>/<node-id>/temperature`

### PingNode

An ultrasonic sensor that reports the distance to an object based on the echo time.

The following topics are advertised:

- `homie/<device-id>/<node-id>/distance` - the distance between the sensor and the object
- `homie/<device-id>/<node-id>/ping` - the time between pulse and echo in microseconds
- `homie/<device-id>/<node-id>/valid` (ok|error) - signals if the measurement was valid
- `homie/<device-id>/<node-id>/changed` (true|false) - signals if the distance to the object changed significantly (i.e. if the object was moved).

The reported distance is calculated from the ping time. This distance depends on the [speed of sound](https://en.wikipedia.org/wiki/Speed_of_sound) and therefore on the temperature. The temperature can be adjusted with the
`setTemperature(float temperatureCelcius)` method, e.g. in conjuction with a temperature sensor such as the DHT22Node:

```cpp
void loopHandler() {
  //...
  pingNode.setTemperature(temperatureNode.getTemperature());
  //...
}
```

## Actor Nodes

### ButtonNode

A pushbutton that just detects a single (debounced) button press. An optional callback can be triggered by the button press event. The button press is reported via these topics:

- `homie/<device-id>/<node-id>/down` (true|false) - signals when the button is pressed
- `homie/<device-id>/<node-id>/duraction` - after the button is pressed and released, it signals the total time that the button was pressed. This is useful to detect a short of long button press.

The minimum and maximum button press time in milliseconds can be set with:

```cpp
void setMinButtonDownTime(unsigned short downTime);
void setMaxButtonDownTime(unsigned short downTime);
```

ToDo: detect multiple button presses and report them back.

### ContactNode

A contact that reports its open state (true|false) via MQTT. An optional callback can be triggered by the state change event.

Advertises the state as:

- `homie/<device-id>/<node-id>/open` (true|false)

### PulseNode

In some way similar to the contact node only that it reacts on pulses on the selected input pin. It reports its state (true|false) via MQTT. An optional callback can be triggered by the state change event. Imagine an optocoupler pulsing with 50Hz when a switch is closed or a button is pressed.

Advertises the state as:

- `homie/<device-id>/<node-id>/active` (true|false)

In order to use the PulseNode you need an interrupt procedure which is attached to the selected pin. e.G.:

```cpp
void onOptoCouplerPulse()
{
  pulseNode.pulseDetected();
}

void setup()
{
  attachInterrupt(PIN_OPTOCOUPLER, onOptoCouplerPulse, FALLING);
}
```

### RelayNode

A relay that can be set on (true|false) via MQTT message. An optional GPIO pin (e.g. to light up a LED) can be passed in the constructor. This pin will be set high/low synchronous to the relay. Additonally the relay can be turned on for a number of seconds by sending this number to the timeout subtopic. The Relay supports reverse logic.

The relay has two different constructors:

Use the following constructor, if your relay is connected directly to the ESP.

```cpp
RelayNode(const char *id,
          const char *name,
          const int8_t relayPin = DEFAULTPIN,
          const int8_t ledPin = DEFAULTPIN,
          const bool reverseSignal = false);
```

Use the following constructor, if your relay is not connected directly, e.g. via a port expander. Turning the relay on and off as well as getting the relay state is handled in the callbacks. Timeout and positive/negative logic are handled in the RelayNode.

```cpp
RelayNode(const char *id,
          const char *name,
          const uint8_t callbackId,
          TGetRelayState OnGetRelayState,
          TSetRelayState OnSetRelayState,
          const bool reverseSignal = false);
```

It has one setting:

- _\<node-id\>.maxTimeout_: The maximum time that the relay is turned on.  
  Range = \[0s .. max(long)s]. Default = 600 seconds. 0 = no max timeout.  
  This is a per node setting, so pay attention that the node names are different.

This is a per node setting. The name of the setting is constructed from the node name + '.maxTimeout'.

The relay can be controlled by posting to the follwing MQTT topics:

- `homie/<device-id>/<node-id>/on/set` (true|false|toggle)
- `homie/<device-id>/<node-id>/timeout/set` (positive integer) - turns the relay on for the corresponding number of seconds, limited by _maxTimeout_.

Advertises the state as:

- `homie/<device-id>/<node-id>/on` (true|false)
- `homie/<device-id>/<node-id>/timeout/` (positive integer) - counts down the number of seconds until the relay will turn off again.
