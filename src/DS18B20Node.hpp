/*
 * DS18B20Node.hpp
 * Homie Node for Dallas 18B20 sensors.
 *
 * Version: 1.0
 * Author: Marcus Klein (http://github.com/kleini)
 */

#pragma once

#include <OneWire.h>
#include <DallasTemperature.h>

#include <Homie.hpp>
#include "constants.hpp"

#define DEFAULTPIN -1

class DS18B20Node : public HomieNode {
private:
  static const int MEASUREMENT_INTERVAL = 300;
  const char *cCaption = "• DS18B20 sensor";
  const char *cIndent = "  ◦ ";

  int _sensorPin;
  unsigned long _measurementInterval;
  unsigned long _lastMeasurement;

  float temperature = NAN;

  OneWire *oneWire;
  DallasTemperature *dallasTemp;

  void printCaption();

protected:
  virtual void setup() override;
  virtual void loop() override;

public:
  DS18B20Node(const char *name,
            const int sensorPin = DEFAULTPIN,
            const int measurementInterval = MEASUREMENT_INTERVAL);

  float getTemperature() const { return temperature; }

  void beforeHomieSetup();
  void onReadyToOperate();
};
