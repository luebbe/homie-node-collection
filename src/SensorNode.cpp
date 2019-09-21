/*
 * SensorNode.hpp
 * Homie Node for genric sensors.
 * Provides a limit method for measurement values
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "SensorNode.hpp"

SensorNode::SensorNode(const char *name, const char *type)
    : HomieNode(name, type, "sensor")
{
}

float SensorNode::computeAbsoluteHumidity(float temperature, float percentHumidity) {
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
