/*
 * BME280Node.cpp
 * Homie Node for BME280 sensors using Adafruit BME280 library.
 *
 * Version: 1.1
 * Author: Lübbe Onken (http://github.com/luebbe)
 * Author: Markus Haack (http://github.com/mhaack)
 */

#include "BME280Node.hpp"
#include <Homie.h>

HomieSetting<double> temperatureOffsetSetting("temperatureOffset", "The temperature offset in degrees [-10.0 - 10.0] Default = 0");

BME280Node::BME280Node(const char *name,
                       const int i2cAddress,
                       const int measurementInterval,
                       const Adafruit_BME280::sensor_sampling tempSampling,
                       const Adafruit_BME280::sensor_sampling pressSampling,
                       const Adafruit_BME280::sensor_sampling humSampling,
                       const Adafruit_BME280::sensor_filter filter)
    : SensorNode(name, "BME280"),
      _i2cAddress(i2cAddress),
      _lastMeasurement(0),
      _tempSampling(tempSampling),
      _pressSampling(pressSampling),
      _humSampling(humSampling),
      _filter(filter)
{
  _measurementInterval = (measurementInterval > MIN_INTERVAL) ? measurementInterval : MIN_INTERVAL;

  advertise(cStatusTopic)
      .setDatatype("enum")
      .setFormat("error, ok");
  advertise(cTemperatureTopic)
      .setDatatype("float")
      .setUnit(cUnitDegrees);
  advertise(cHumidityTopic)
      .setDatatype("float")
      .setUnit(cUnitPercent);
  advertise(cPressureTopic)
      .setDatatype("float")
      .setUnit(cUnitHpa);
  advertise(cAbsHumidityTopic)
      .setDatatype("float")
      .setUnit(cUnitMgm3);
}

void BME280Node::printCaption()
{
  Homie.getLogger() << cCaption << " i2c[" << _i2cAddress << "]:" << endl;
}

void BME280Node::send()
{
  printCaption();

  float absHumidity = computeAbsoluteHumidity(temperature, humidity);

  Homie.getLogger() << cIndent << "Temperature: " << temperature << " °C" << endl;
  temperature += temperatureOffsetSetting.get();
  Homie.getLogger() << cIndent << "Temperature (after offset): " << temperature << " °C" << endl;
  Homie.getLogger() << cIndent << "Humidity: " << humidity << " %" << endl;
  Homie.getLogger() << cIndent << "Pressure: " << pressure << " hPa" << endl;
  Homie.getLogger() << cIndent << "Abs humidity: " << absHumidity << " g/m³" << endl;

  setProperty(cStatusTopic).send("ok");
  setProperty(cTemperatureTopic).send(String(temperature));
  setProperty(cHumidityTopic).send(String(humidity));
  setProperty(cPressureTopic).send(String(pressure));
  setProperty(cAbsHumidityTopic).send(String(absHumidity));
}

void BME280Node::loop()
{
  if (_sensorFound && _ready)
  {
    if (millis() - _lastMeasurement >= _measurementInterval * 1000UL ||
        _lastMeasurement == 0)
    {
      bme.takeForcedMeasurement(); // has no effect in normal mode

      temperature = bme.readTemperature();
      humidity = bme.readHumidity();
      pressure = bme.readPressure() / 100;

      fixRange(&temperature, cMinTemp, cMaxTemp);
      fixRange(&humidity, cMinHumid, cMaxHumid);
      fixRange(&pressure, cMinPress, cMaxPress);

      send();

      _lastMeasurement = millis();
    }
  }
}

void BME280Node::beforeHomieSetup()
{
  temperatureOffsetSetting.setDefaultValue(0.0f).setValidator([](float candidate) {
    return (candidate >= -10.0f) && (candidate <= 10.0f);
  });
}

void BME280Node::onReadyToOperate()
{
  _ready = true;
  if (!_sensorFound)
  {
    setProperty(cStatusTopic).send("error");
  }
};

void BME280Node::setup()
{
  if (bme.begin(_i2cAddress))
  {
    _sensorFound = true;
    printCaption();
    Homie.getLogger() << cIndent << "found. Reading interval: " << _measurementInterval << " s" << endl;
    // Parameters taken from the weather station monitoring example (advancedsettings.ino) in
    // the Adafruit BME280 library
    bme.setSampling(Adafruit_BME280::MODE_FORCED, _tempSampling, _pressSampling, _humSampling, _filter);
  }
  else
  {
    _sensorFound = false;
    printCaption();
    Homie.getLogger() << cIndent << "not found. Check wiring!" << endl;
  }
}
