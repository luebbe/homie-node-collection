/*
 * DS18B20Node.cpp
 * Homie Node for Dallas 18B20 sensors.
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 * Author: Marcus Klein (http://github.com/kleini)
 */

#include "DS18B20Node.hpp"

DS18B20Node::DS18B20Node(const char *id, const char *name, const int sensorPin, const int readInterval)
    : SensorNode(id, name, "DS18B20", readInterval),
      _sensorPin(sensorPin)
{
  if (_sensorPin > DEFAULTPIN)
  {
    oneWire = new OneWire(_sensorPin);
    dallasTemp = new DallasTemperature(oneWire);
  }

  asprintf(&_caption, cCaption, name, sensorPin);

  advertise(cStatusTopic)
      .setDatatype("enum")
      .setFormat("error, ok");
  advertise(cTemperatureTopic)
      .setDatatype("float")
      .setFormat("-55:125")
      .setUnit(cUnitDegrees);
}

void DS18B20Node::send()
{
  printCaption();

  if (DEVICE_DISCONNECTED_C == temperature)
  {
    Homie.getLogger() << cIndent << F("Error reading from Sensor") << endl;
    sendError();
  }
  else
  {
    Homie.getLogger() << cIndent << F("Temperature: ") << temperature << " °C" << endl;
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

void DS18B20Node::takeMeasurement()
{
  dallasTemp->requestTemperatures();
  temperature = dallasTemp->getTempCByIndex(0);
  fixRange(&temperature, cMinTemp, cMaxTemp);
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
    Homie.getLogger() << cIndent << F("Found ") << dallasTemp->getDS18Count() << " sensors." << endl
                      << cIndent << F("Reading interval: ") << readInterval() << " s" << endl;
  }
}
