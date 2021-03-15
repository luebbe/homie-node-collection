/*
 * DHT22Node.cpp
 * Homie Node for DHT22 sensors using Adafruit DHT22 library.
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "DHT22Node.hpp"

#define DHTTYPE DHT22

DHT22Node::DHT22Node(const char *id, const char *name, const int sensorPin, const int readInterval)
    : SensorNode(id, name, "DHT22", readInterval),
      _sensorPin(sensorPin)
{
  if (_sensorPin > DEFAULTPIN)
  {
    dht = new DHT(_sensorPin, DHTTYPE);
  }

  asprintf(&_caption, cCaption, name, sensorPin);

  advertise(cStatusTopic)
      .setDatatype("enum")
      .setFormat("error, ok");
  advertise(cTemperatureTopic)
      .setDatatype("float")
      .setFormat("-40:125")
      .setUnit(cUnitDegrees);
  advertise(cHumidityTopic)
      .setDatatype("float")
      .setFormat("0:100")
      .setUnit(cUnitPercent);
  advertise(cAbsHumidityTopic)
      .setDatatype("float")
      .setUnit(cUnitMgm3);
}

void DHT22Node::send()
{
  printCaption();

  if (isnan(temperature) || isnan(humidity))
  {
    Homie.getLogger() << cIndent << F("Error reading from Sensor") << endl;

    if (Homie.isConnected())
    {
      setProperty(cStatusTopic).send("error");
    }
  }
  else
  {
    float absHumidity = computeAbsoluteHumidity(temperature, humidity);

    Homie.getLogger() << cIndent << F("Temperature: ") << temperature << " °C" << endl;
    Homie.getLogger() << cIndent << F("Humidity: ") << humidity << " %" << endl;
    Homie.getLogger() << cIndent << F("Abs humidity: ") << absHumidity << " g/m³" << endl;

    if (Homie.isConnected())
    {
      setProperty(cStatusTopic).send("ok");
      setProperty(cTemperatureTopic).send(String(temperature));
      setProperty(cHumidityTopic).send(String(humidity));
      setProperty(cAbsHumidityTopic).send(String(absHumidity));
    }
  }
}

void DHT22Node::takeMeasurement()
{
  temperature = dht->readTemperature();
  humidity = dht->readHumidity();

  fixRange(&temperature, cMinTemp, cMaxTemp);
  fixRange(&humidity, cMinHumid, cMaxHumid);
}

void DHT22Node::setup()
{
  printCaption();
  Homie.getLogger() << cIndent << F("Reading interval: ") << readInterval() << " s" << endl;

  if (dht)
  {
    dht->begin();
    _sensorFound = true;
  }
}
