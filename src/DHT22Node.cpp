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
      .setUnit(cUnitDegCels);
  advertise(cHumidityTopic)
      .setDatatype("float")
      .setFormat("0:100")
      .setUnit(cUnitPercent);
  advertise(cAbsHumidityTopic)
      .setDatatype("float")
      .setUnit(cUnitMgm3);
  advertise(cDewpointTopic)
      .setDatatype("float")
      .setUnit(cUnitDegCels);
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
    float dewpoint = computeDewpoint(temperature, humidity);

    Homie.getLogger() << cIndent << F("Temperature:  ") << temperature << " " << cUnitDegCels << endl
                      << cIndent << F("Humidity:     ") << humidity << " " << cUnitPercent << endl
                      << cIndent << F("Abs humidity: ") << absHumidity << " " << cUnitMgm3 << endl
                      << cIndent << F("Dew point:    ") << dewpoint << " " << cUnitDegCels << endl;

    if (Homie.isConnected())
    {
      setProperty(cStatusTopic).send("ok");
      setProperty(cTemperatureTopic).send(String(temperature));
      setProperty(cHumidityTopic).send(String(humidity));
      setProperty(cAbsHumidityTopic).send(String(absHumidity));
      setProperty(cDewpointTopic).send(String(dewpoint));
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
  SensorNode::setup();

  if (dht)
  {
    dht->begin();
    _sensorFound = true;
    Homie.getLogger() << cIndent << F("Reading interval: ") << readInterval() / 1000UL << F("s") << endl;
  }
}
