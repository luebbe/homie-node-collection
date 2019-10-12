/*
 * DS18B20Node.cpp
 * Homie Node for Dallas 18B20 sensors.
 *
 * Version: 1.0
 * Author: Marcus Klein (http://github.com/kleini)
 */

#include "DS18B20Node.hpp"

DS18B20Node::DS18B20Node(const char *name, const int sensorPin, const int measurementInterval)
    : SensorNode(name, "DS18N20"),
      _sensorPin(sensorPin),
      _measurementInterval(measurementInterval),
      _lastMeasurement(0)
{
  if (_sensorPin > DEFAULTPIN)
  {
    oneWire = new OneWire(_sensorPin);
    dallasTemp = new DallasTemperature(oneWire);
  }
  advertise(cStatusTopic)
      .setDatatype("enum")
      .setFormat("error, ok");
  advertise(cTemperatureTopic)
      .setDatatype("float")
      .setUnit(cUnitDegrees);
}

void DS18B20Node::printCaption()
{
  Homie.getLogger() << cCaption << " pin[" << _sensorPin << "]:" << endl;
}

void DS18B20Node::send()
{
  printCaption();

  if (DEVICE_DISCONNECTED_C == temperature)
  {
    Homie.getLogger() << cIndent << "Error reading from Sensor" << endl;
    setProperty(cStatusTopic).send("error");
  }
  else
  {
    Homie.getLogger() << cIndent << "Temperature: " << temperature << " °C" << endl;
    setProperty(cStatusTopic).send("ok");
    setProperty(cTemperatureTopic).send(String(temperature));
  }
}

void DS18B20Node::loop()
{
  if (_sensorFound && _ready)
  {
    if ((millis() - _lastMeasurement >= _measurementInterval * 1000UL) || (_lastMeasurement == 0))
    {
      dallasTemp->requestTemperatures();
      temperature = dallasTemp->getTempCByIndex(0);
      fixRange(&temperature, cMinTemp, cMaxTemp);

      send();

      _lastMeasurement = millis();
    }
  }
}

void DS18B20Node::onReadyToOperate()
{
  if (_sensorFound)
  {
    _ready = true;
  }
  else
  {
    setProperty(cStatusTopic).send("error");
  }
};

void DS18B20Node::setup()
{
  printCaption();

  if (dallasTemp)
  {
    dallasTemp->begin();
    _sensorFound = (dallasTemp->getDS18Count() > 0);
    Homie.getLogger() << cIndent << "Found " << dallasTemp->getDS18Count() << " sensors."
                      << cIndent << "Reading interval: " << _measurementInterval << " s" << endl;
  }
}
