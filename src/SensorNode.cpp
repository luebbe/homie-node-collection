/*
 * SensorNode.hpp
 * Homie Node for generic sensors.
 * Provides a limit method for measurement values
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "SensorNode.hpp"

SensorNode::SensorNode(const char *id, const char *name, const char *type,
                       const int readInterval,
                       const int sendInterval)
    : BaseNode(id, name, type),
      _readInterval(readInterval),
      _sendInterval(sendInterval),
      _nextReadTime(0),
      _nextSendTime(0)
{
}

float SensorNode::computeAbsoluteHumidity(float tempCelsius, float percentHumidity)
{
  // Calculate the absolute humidity in g/m³
  // https://carnotcycle.wordpress.com/2012/08/04/how-to-convert-relative-humidity-to-absolute-humidity/

  float absHumidity;
  float absTemperature;
  absTemperature = tempCelsius + 273.15;

  absHumidity = 6.112;
  absHumidity *= exp((17.67 * tempCelsius) / (243.5 + tempCelsius));
  absHumidity *= percentHumidity;
  absHumidity *= 2.1674;
  absHumidity /= absTemperature;

  return absHumidity;
}

float SensorNode::computeDewpoint(float tempCelsius, float percentHumidity)
{
  // reference: http://en.wikipedia.org/wiki/Dew_point
  // https://github.com/rokrodic/ESP8266-DHT11-HOMIE-MQTT-temp-hum-dew-feel/blob/master/HomieTemp/HomieTemp.ino
  float a = 17.271;
  float b = 237.7;
  float temp = (a * tempCelsius) / (b + tempCelsius) + log(percentHumidity * 0.01);
  float Td = (b * temp) / (a - temp);
  return Td;
}

unsigned long SensorNode::readInterval()
{
  return _readInterval;
}

unsigned long SensorNode::sendInterval()
{
  return _sendInterval;
}

bool SensorNode::sensorFound()
{
  return _sensorFound;
}

void SensorNode::setupDone(int sensorCount)
{
  if (sensorCount > 0)
  {
    _sensorFound = true;
    Homie.getLogger() << cIndent << F("Found      : ") << sensorCount << " sensors." << endl
                      << cIndent << F("Interval   : ") << _readInterval / 1000UL << F("s") << endl
                      << cIndent << F("Start delay: ") << _nextReadTime / 1000UL << F("s") << endl;
  }
  else
  {
    _sensorFound = false;
    Homie.getLogger() << cIndent << F("not found. Check wiring!") << endl;
  }
}

void SensorNode::setup()
{
  printCaption();
  // Give all sensors a random start delay from five to ten seconds,
  // so they are not all read at the same time.
  long randomDelay = random(5000, 10000);
  _nextReadTime = randomDelay;
  _nextSendTime = randomDelay;
}

void SensorNode::loop()
{
  if (sensorFound())
  {
    unsigned long now = millis();

    if (now >= _nextReadTime)
    {
      takeMeasurement();
      _nextReadTime = now + readInterval();
    }

    if (now >= _nextSendTime)
    {
      send();
      _nextSendTime = now + sendInterval();
      if (_onDataSent != NULL)
      {
        _onDataSent();
      }
    }
  }
}
