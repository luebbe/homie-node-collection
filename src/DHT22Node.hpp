/*
 * DHT22Node.hpp
 * Homie Node for DHT-22 sensors using Adafruit DHT library.
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#pragma once

#include <Adafruit_Sensor.h>
#include <DHT_U.h>

#include <Homie.hpp>
#include "constants.hpp"

#define DEFAULTPIN -1

class DHT22Node : public HomieNode
{
private:
  static const int MEASUREMENT_INTERVAL = 300;
  const char *cCaption = "• DHT22 sensor";
  const char *cIndent = "  ◦ ";

  int _sensorPin;
  unsigned long _measurementInterval;
  unsigned long _lastMeasurement;

  DHT *dht;

  void printCaption();

public:
  DHT22Node(const char *name,
            const int sensorPin = DEFAULTPIN,
            const int measurementInterval = MEASUREMENT_INTERVAL);

  float getHumidity() const { return humidity; }
  float getTemperature() const { return temperature; }

  void setupHandler();

protected:
  virtual void setup() override;
  virtual void loop() override;

private:
  float temperature = NAN;
  float humidity = NAN;
};
