/*
 * SensorNode.hpp
 * Homie Node for genric sensors.
 * Provides a limit method for measurement values
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#pragma once

#include <Homie.hpp>

class SensorNode : public HomieNode
{
protected:
  const float cMinHumid = 0.0;
  const float cMaxHumid = 100.0;
  static const int MEASUREMENT_INTERVAL = 300;

  float computeAbsoluteHumidity(float temperature, float percentHumidity);
  void fixRange(float *value, float min, float max);

public:
  SensorNode(const char *name, const char *type);
};
