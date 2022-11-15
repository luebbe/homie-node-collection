/*
 * BME280Node.cpp
 * Homie Node for BME280 sensors using Adafruit BME280 library.
 *
 * Version: 1.2
 * Author: Lübbe Onken (http://github.com/luebbe)
 * Author: Markus Haack (http://github.com/mhaack)
 */

#include "BME280Node.hpp"
#include <Homie.h>

BME280Node::BME280Node(const char *id,
                       const char *name,
                       const int i2cAddress,
                       const int readInterval,
                       const Adafruit_BME280::sensor_sampling tempSampling,
                       const Adafruit_BME280::sensor_sampling pressSampling,
                       const Adafruit_BME280::sensor_sampling humSampling,
                       const Adafruit_BME280::sensor_filter filter)
    : SensorNode(id, name, "BME280"),
      _i2cAddress(i2cAddress),
      _tempSampling(tempSampling),
      _pressSampling(pressSampling),
      _humSampling(humSampling),
      _filter(filter)
{

  _readInterval = (readInterval > MIN_INTERVAL) ? readInterval : MIN_INTERVAL;

  asprintf(&_temperatureOffsetName, "%s.temperatureOffset", id);
  _temperatureOffset = new HomieSetting<double>(_temperatureOffsetName, "The temperature offset in degrees [-10.0 .. 10.0] Default = 0");

  asprintf(&_caption, cCaption, name, i2cAddress);

  advertise(cStatusTopic)
      .setDatatype("enum")
      .setFormat("error, ok");
  advertise(cTemperatureTopic)
      .setDatatype("float")
      .setFormat("-40:85")
      .setUnit(cUnitDegCels);
  advertise(cHumidityTopic)
      .setDatatype("float")
      .setFormat("0:100")
      .setUnit(cUnitPercent);
  advertise(cPressureTopic)
      .setDatatype("float")
      .setFormat("300:1100")
      .setUnit(cUnitHpa);
  advertise(cAbsHumidityTopic)
      .setDatatype("float")
      .setUnit(cUnitMgm3);
  advertise(cDewpointTopic)
      .setDatatype("float")
      .setUnit(cUnitDegCels);
}

void BME280Node::send()
{
  printCaption();

  float absHumidity = computeAbsoluteHumidity(temperature, humidity);
  float dewpoint = computeDewpoint(temperature, humidity);

  Homie.getLogger() << cIndent << F("Temperature:  ") << temperature << " " << cUnitDegCels << endl
                    << cIndent << F("Humidity:     ") << humidity << " " << cUnitPercent << endl
                    << cIndent << F("Pressure:     ") << pressure << " " << cUnitHpa << endl
                    << cIndent << F("Abs humidity: ") << absHumidity << " " << cUnitMgm3 << endl
                    << cIndent << F("Dew point:    ") << dewpoint << " " << cUnitDegCels << endl;

  if (Homie.isConnected())
  {
    setProperty(cStatusTopic).send("ok");
    setProperty(cTemperatureTopic).send(String(temperature));
    setProperty(cHumidityTopic).send(String(humidity));
    setProperty(cPressureTopic).send(String(pressure));
    setProperty(cAbsHumidityTopic).send(String(absHumidity));
    setProperty(cDewpointTopic).send(String(dewpoint));
  }
}

void BME280Node::takeMeasurement()
{
  bme.takeForcedMeasurement(); // has no effect in normal mode

  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100;

  fixRange(&temperature, cMinTemp, cMaxTemp);
  fixRange(&humidity, cMinHumid, cMaxHumid);
  fixRange(&pressure, cMinPress, cMaxPress);
}

void BME280Node::beforeHomieSetup()
{
  _temperatureOffset->setDefaultValue(0.0f).setValidator([](float candidate)
                                                         { return (candidate >= -10.0f) && (candidate <= 10.0f); });
}

void BME280Node::onReadyToOperate()
{
  if (!sensorFound() && Homie.isConnected())
  {
    setProperty(cStatusTopic).send("error");
  }
};

void BME280Node::setup()
{
  printCaption();

  if (bme.begin(_i2cAddress))
  {
    _sensorFound = true;
    Homie.getLogger() << cIndent << F("found. Reading interval: ") << readInterval() << " s" << endl;
    // Parameters taken from the weather station monitoring example (advancedsettings.ino) in
    // the Adafruit BME280 library
    bme.setSampling(Adafruit_BME280::MODE_FORCED, _tempSampling, _pressSampling, _humSampling, _filter);
    bme.setTemperatureCompensation(_temperatureOffset->get());
  }
  else
  {
    Homie.getLogger() << cIndent << F("not found. Check wiring!") << endl;
  }
}
