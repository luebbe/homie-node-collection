/*
 * BMP280Node.cpp
 * Homie Node for BMP280 sensors using Adafruit BMP280 library.
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 * Author: Markus Haack (http://github.com/mhaack)
 * Author: Keesma
 */

#include "BMP280Node.hpp"
#include <Homie.h>

BMP280Node::BMP280Node(const char *id,
                       const char *name,
                       const int i2cAddress,
                       const int measurementInterval,
                       const Adafruit_BMP280::sensor_sampling tempSampling,
                       const Adafruit_BMP280::sensor_sampling pressSampling,
                       const Adafruit_BMP280::sensor_filter filter)
    : SensorNode(id, name, "BMP280"),
      _i2cAddress(i2cAddress),
      _lastMeasurement(0),
      _tempSampling(tempSampling),
      _pressSampling(pressSampling),
      _filter(filter)
{
  _measurementInterval = (measurementInterval > MIN_INTERVAL) ? measurementInterval : MIN_INTERVAL;

  asprintf(&_temperatureOffsetName, "%s.temperatureOffset", id);
  _temperatureOffset = new HomieSetting<double>(_temperatureOffsetName, "The temperature offset in degrees [-10.0 .. 10.0] Default = 0");

  asprintf(&_caption, cCaption, name, i2cAddress);

  advertise(cStatusTopic)
      .setDatatype("enum")
      .setFormat("error, ok");
  advertise(cTemperatureTopic)
      .setDatatype("float")
      .setFormat("-40:85")
      .setUnit(cUnitDegrees);
  advertise(cPressureTopic)
      .setDatatype("float")
      .setFormat("300:1100")
      .setUnit(cUnitHpa);
}

void BMP280Node::send()
{
  printCaption();


  Homie.getLogger() << cIndent << F("Temperature:  ") << temperature << " " << cUnitDegrees << endl
                    << cIndent << F("Pressure:     ") << pressure << " " << cUnitHpa << endl;

  if (Homie.isConnected())
  {
    setProperty(cStatusTopic).send("ok");
    setProperty(cTemperatureTopic).send(String(temperature));
    setProperty(cPressureTopic).send(String(pressure));
  }
}

void BMP280Node::loop()
{
  if (_sensorFound)
  {
    if (millis() - _lastMeasurement >= _measurementInterval * 1000UL ||
        _lastMeasurement == 0)
    {
      //bmp.takeForcedMeasurement(); // has no effect in normal mode

      temperature = bmp.readTemperature();
      pressure = bmp.readPressure() / 100;

      fixRange(&temperature, cMinTemp, cMaxTemp);
      fixRange(&pressure, cMinPress, cMaxPress);

      send();

      _lastMeasurement = millis();
    }
  }
}

void BMP280Node::beforeHomieSetup()
{
  _temperatureOffset->setDefaultValue(0.0f).setValidator([](float candidate) {
    return (candidate >= -10.0f) && (candidate <= 10.0f);
  });
}

void BMP280Node::onReadyToOperate()
{
  if (!_sensorFound && Homie.isConnected())
  {
    setProperty(cStatusTopic).send("error");
  }
};

void BMP280Node::setup()
{
  printCaption();

  if (bmp.begin(_i2cAddress))
  {
    _sensorFound = true;
    Homie.getLogger() << cIndent << F("found. Reading interval: ") << _measurementInterval << " s" << endl;
    // Parameters taken from the weather station monitoring example (advancedsettings.ino) in
    // the Adafruit BMP280 library
    bmp.setSampling(Adafruit_BMP280::MODE_FORCED, _tempSampling, _pressSampling, _filter);
//    bmp.setTemperatureCompensation(_temperatureOffset->get());
  }
  else
  {
    _sensorFound = false;
    Homie.getLogger() << cIndent << F("not found. Check wiring!") << endl;
  }
}