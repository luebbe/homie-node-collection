/*
 * SensorNode.hpp
 * Homie Node for genric sensors.
 * Provides a limit method for measurement values
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "SensorNode.hpp"

SensorNode::SensorNode(const char *id, const char *name, const char *type,
                       const int readInterval,
                       const int sendInterval)
    : HomieNode(id, name, type),
      _caption(0),
      _readInterval(readInterval),
      _sendInterval(sendInterval),
      _lastReadTime(0),
      _lastSendTime(0)
{
}

float SensorNode::computeAbsoluteHumidity(float temperature, float percentHumidity)
{
  // Calculate the absolute humidity in g/m³
  // https://carnotcycle.wordpress.com/2012/08/04/how-to-convert-relative-humidity-to-absolute-humidity/

  float absHumidity;
  float absTemperature;
  absTemperature = temperature + 273.15;

  absHumidity = 6.112;
  absHumidity *= exp((17.67 * temperature) / (243.5 + temperature));
  absHumidity *= percentHumidity;
  absHumidity *= 2.1674;
  absHumidity /= absTemperature;

  return absHumidity;
}

void SensorNode::fixRange(float *value, float min, float max)
{
  if (isnan(*value))
  {
    return;
  }
  else if (*value < min)
  {
    *value = min;
  }
  else if (*value > max)
  {
    *value = max;
  };
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

void SensorNode::loop()
{
  if (sensorFound())
  {
    unsigned long now = millis();

    if (now - _lastReadTime >= readInterval() || _lastReadTime == 0)
    {
      takeMeasurement();
      _lastReadTime = now;
    }

    if (now - _lastSendTime >= _sendInterval || _lastSendTime == 0)
    {
      send();
      _lastSendTime = now;
    }
  }
}

void SensorNode::printCaption()
{
  Homie.getLogger() << _caption << endl;
}
