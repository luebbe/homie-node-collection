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
}

void DHT22Node::printCaption()
{
  Homie.getLogger() << cCaption << " pin[" << _sensorPin << "]:" << endl;
}

void DHT22Node::loop()
{
  if (_sensorPin > DEFAULTPIN)
  {
    if ((millis() - _lastMeasurement >= _measurementInterval * 1000UL) ||
        (_lastMeasurement == 0))
    {
      temperature = dht->readTemperature();
      humidity = dht->readHumidity();

      fixRange(&temperature, cMinTemp, cMaxTemp);
      fixRange(&humidity, cMinHumid, cMaxHumid);

      printCaption();

      if (isnan(temperature) || isnan(humidity))
      {
        Homie.getLogger() << cIndent << "Error reading from Sensor" << endl;
        setProperty(cStatusTopic).send("error");
      }
      else
      {
        float absHumidity = computeAbsoluteHumidity(temperature, humidity);

        Homie.getLogger() << cIndent << "Temperature: " << temperature << " °C" << endl;
        Homie.getLogger() << cIndent << "Humidity: " << humidity << " %" << endl;
        Homie.getLogger() << cIndent << "Abs humidity: " << absHumidity << " g/m³" << endl;

        setProperty(cStatusTopic).send("ok");
        setProperty(cTemperatureTopic).send(String(temperature));
        setProperty(cHumidityTopic).send(String(humidity));
        setProperty(cAbsHumidityTopic).send(String(absHumidity));
      }
      _lastMeasurement = millis();
    }
  }
}

void DHT22Node::onReadyToOperate()
{
  setProperty(cTemperatureUnitTopic).send("°C");
  setProperty(cHumidityUnitTopic).send("%");
  setProperty(cAbsHumidityUnitTopic).send("g/m³");
};

void DHT22Node::setup()
{
  advertise(cStatusTopic);
  advertise(cTemperatureTopic);
  advertise(cTemperatureUnitTopic);
  advertise(cHumidityTopic);
  advertise(cHumidityUnitTopic);
  advertise(cAbsHumidityTopic);
  advertise(cAbsHumidityUnitTopic);

  printCaption();
  Homie.getLogger() << cIndent << "Reading interval: " << _measurementInterval << " s" << endl;

  if (dht)
  {
    dht->begin();
  }
}
