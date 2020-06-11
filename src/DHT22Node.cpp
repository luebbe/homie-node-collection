/*
 * DHT22Node.cpp
 * Homie Node for DHT22 sensors using Adafruit DHT22 library.
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "DHT22Node.hpp"

#define DHTTYPE DHT22

DHT22Node::DHT22Node(const char *name, const int sensorPin, const int measurementInterval)
    : SensorNode(name, "DHT22"),
      _sensorPin(sensorPin),
      _measurementInterval(measurementInterval),
      _lastMeasurement(0)
{
  if (_sensorPin > DEFAULTPIN)
  {
    dht = new DHT(_sensorPin, DHTTYPE);
  }

  asprintf(&_caption, cCaption, sensorPin);

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

void DHT22Node::printCaption()
{
  Homie.getLogger() << _caption << endl;
}

void DHT22Node::send()
{
  printCaption();

  if (isnan(temperature) || isnan(humidity))
  {
    Homie.getLogger() << cIndent << "Error reading from Sensor" << endl;

    if (Homie.isConnected())
    {
      setProperty(cStatusTopic).send("error");
    }
  }
  else
  {
    float absHumidity = computeAbsoluteHumidity(temperature, humidity);

    Homie.getLogger() << cIndent << "Temperature: " << temperature << " °C" << endl;
    Homie.getLogger() << cIndent << "Humidity: " << humidity << " %" << endl;
    Homie.getLogger() << cIndent << "Abs humidity: " << absHumidity << " g/m³" << endl;

    if (Homie.isConnected())
    {
      setProperty(cStatusTopic).send("ok");
      setProperty(cTemperatureTopic).send(String(temperature));
      setProperty(cHumidityTopic).send(String(humidity));
      setProperty(cAbsHumidityTopic).send(String(absHumidity));
    }
  }
}

void DHT22Node::loop()
{
  if (dht)
  {
    if ((millis() - _lastMeasurement >= _measurementInterval * 1000UL) ||
        (_lastMeasurement == 0))
    {
      temperature = dht->readTemperature();
      humidity = dht->readHumidity();

      fixRange(&temperature, cMinTemp, cMaxTemp);
      fixRange(&humidity, cMinHumid, cMaxHumid);

      send();

      _lastMeasurement = millis();
    }
  }
}

void DHT22Node::setup()
{
  printCaption();
  Homie.getLogger() << cIndent << "Reading interval: " << _measurementInterval << " s" << endl;

  if (dht)
  {
    dht->begin();
  }
}
