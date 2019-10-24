/*
 * DS18B20Node.cpp
 * Homie Node for Dallas 18B20 sensors.
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
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
      .setFormat("-55:125")
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
    sendError();
  }
  else
  {
    Homie.getLogger() << cIndent << "Temperature: " << temperature << " °C" << endl;
    sendData();
  }
}

void DS18B20Node::sendError()
{
  if (Homie.isConnected())
  {
    setProperty(cStatusTopic).send("error");
  }
}

void DS18B20Node::sendData()
{
  if (Homie.isConnected())
  {
    setProperty(cStatusTopic).send("ok");
    setProperty(cTemperatureTopic).send(String(temperature));
  }
}

void DS18B20Node::loop()
{
  if (_sensorFound)
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
  if (!_sensorFound)
  {
    sendError();
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
