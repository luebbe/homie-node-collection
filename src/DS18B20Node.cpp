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
}

void DS18B20Node::printCaption()
{
  Homie.getLogger() << cCaption << " pin[" << _sensorPin << "]:" << endl;
}

void DS18B20Node::loop()
{
  if (_sensorPin > DEFAULTPIN)
  {
    if ((millis() - _lastMeasurement >= _measurementInterval * 1000UL) || (_lastMeasurement == 0))
    {
      dallasTemp->requestTemperatures();
      temperature = dallasTemp->getTempCByIndex(0);
      fixRange(&temperature, cMinTemp, cMaxTemp);

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
      _lastMeasurement = millis();
    }
  }
}

void DS18B20Node::onReadyToOperate()
{
  setProperty(cTemperatureUnitTopic).send("°C");
};

void DS18B20Node::setup()
{
  advertise(cStatusTopic);
  advertise(cTemperatureTopic);
  advertise(cTemperatureUnitTopic);

  printCaption();
  Homie.getLogger() << cIndent << "Reading interval: " << _measurementInterval << " s" << endl;

  if (dallasTemp)
  {
    dallasTemp->begin();
    Homie.getLogger() << cIndent << "Found " << dallasTemp->getDS18Count() << " sensors." << endl;
  }
}
