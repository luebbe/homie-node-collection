/*
 * AdcNode.cpp
 * Homie Node using the internal ESP ADC to measure voltage.
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#ifndef SRC_ADCNODE_H_
#define SRC_ADCNODE_H_

#include <HomieNode.hpp>
#include "constants.h"

class AdcNode : public HomieNode {
private:
  static const int MEASUREMENT_INTERVAL = 300;
  const char *cCaption = "• ADC measurement";
  const char *cIndent  = "  ◦ ";

  unsigned long _measurementInterval;
  unsigned long _lastMeasurement;
  float _voltage = NAN;
  
  void printCaption();

public:
  AdcNode(const char *name, const int measurementInterval = MEASUREMENT_INTERVAL);

  float getVoltage() const { return _voltage; }

  void setupHandler();

protected:
  virtual void setup() override;
  virtual void loop() override;
};

#endif
