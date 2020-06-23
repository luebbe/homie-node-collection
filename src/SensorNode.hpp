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
  const char *cIndent = "  ◦ ";
  const float cMinHumid = 0.0;
  const float cMaxHumid = 100.0;
  static const int MEASUREMENT_INTERVAL = 300;

  char *_caption;

  float computeAbsoluteHumidity(float temperature, float percentHumidity);
  void fixRange(float *value, float min, float max);
  virtual void printCaption();

public:
  explicit SensorNode(const char *id, const char *name);
};
