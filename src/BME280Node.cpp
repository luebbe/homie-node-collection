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
HomieSetting<double> humidityOffsetSetting("humidityOffset", "The humidity offset in percentage points [-20.0 - 20.0] Default = 0");
HomieSetting<double> pressureOffsetSetting("pressureOffset", "The pressure offset in hectopascal [-50.0 - 50.0] Default = 0");

BME280Node::BME280Node(const char *name,
                       const int i2cAddress,
                       const int measurementInterval,
                       const Adafruit_BME280::sensor_sampling tempSampling,
                       const Adafruit_BME280::sensor_sampling pressSampling,
                       const Adafruit_BME280::sensor_sampling humSampling,
                       const Adafruit_BME280::sensor_filter filter) : HomieNode(name, "BME280Sensor"),
                                                                      _i2cAddress(i2cAddress),
                                                                      _lastMeasurement(0),
                                                                      _tempSampling(tempSampling),
                                                                      _pressSampling(pressSampling),
                                                                      _humSampling(humSampling),
                                                                      _filter(filter)
{
  _measurementInterval = (measurementInterval > MIN_INTERVAL) ? measurementInterval : MIN_INTERVAL;
}

void BME280Node::printCaption()
{
  Homie.getLogger() << cCaption << endl;
}

void BME280Node::loop()
{
  if (millis() - _lastMeasurement >= _measurementInterval * 1000UL ||
      _lastMeasurement == 0)
  {
    if (_sensorFound)
    {
      bme.takeForcedMeasurement(); // has no effect in normal mode

      temperature = bme.readTemperature();
      humidity = bme.readHumidity();
      pressure = bme.readPressure() / 100;

      printCaption();

      Homie.getLogger() << cIndent << "Temperature: " << temperature << " °C" << endl;
      temperature += temperatureOffsetSetting.get();
      Homie.getLogger() << cIndent << "Temperature (after offset): " << temperature << " °C" << endl;
      Homie.getLogger() << cIndent << "Humidity: " << humidity << " %" << endl;
      humidity += humidityOffsetSetting.get();
      Homie.getLogger() << cIndent << "Humidity (after offset): " << humidity << " %" << endl;
      Homie.getLogger() << cIndent << "Pressure: " << pressure << " hPa" << endl;
      pressure += pressureOffsetSetting.get();
      Homie.getLogger() << cIndent << "Pressure (after offset): " << pressure << " hPa" << endl;

      setProperty(cStatus).send("ok");
      setProperty(cTemperature).send(String(temperature));
      setProperty(cHumidity).send(String(humidity));
      setProperty(cPressure).send(String(pressure));
    }
    else
    {
      setProperty(cStatus).send("error");
    }
    _lastMeasurement = millis();
  }
}

void BME280Node::beforeHomieSetup()
{
  temperatureOffsetSetting.setDefaultValue(0.0f).setValidator([](float candidate) {
    return (candidate >= -10.0f) && (candidate <= 10.0f);
  });
  humidityOffsetSetting.setDefaultValue(0.0f).setValidator([](float candidate) {
    return (candidate >= -20.0f) && (candidate <= 20.0f);
  });
  pressureOffsetSetting.setDefaultValue(0.0f).setValidator([](float candidate) {
    return (candidate >= -50.0f) && (candidate <= 50.0f);
  });
}

void BME280Node::onReadyToOperate()
{
  setProperty(cTemperatureUnit).send("°C");
  setProperty(cHumidityUnit).send("%");
  setProperty(cPressureUnit).send("hPa");
};

void BME280Node::setup()
{
  advertise(cStatus);
  advertise(cTemperature);
  advertise(cTemperatureUnit);
  advertise(cHumidity);
  advertise(cHumidityUnit);
  advertise(cPressure);
  advertise(cPressureUnit);

  if (bme.begin(_i2cAddress))
  {
    _sensorFound = true;
    Homie.getLogger() << cCaption << " found" << endl
                      << cIndent << "Reading interval: " << _measurementInterval << " s" << endl;
    // Parameters taken from the weather station monitoring example (advancedsettings.ino) in
    // the Adafruit BME280 library
    bme.setSampling(Adafruit_BME280::MODE_FORCED, _tempSampling, _pressSampling, _humSampling, _filter);
  }
  else
  {
    _sensorFound = false;
    Homie.getLogger() << cCaption << " not found. Check wiring!" << endl;
  }
}
