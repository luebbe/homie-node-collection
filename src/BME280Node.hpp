/*
 * BME280Node.h
 * Homie Node for BME280 sensors using Adafruit BME280 library.
 *
 * Version: 1.1
 * Author: Lübbe Onken (http://github.com/luebbe)
 * Author: Markus Haack (http://github.com/mhaack)
 */

#pragma once

#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <Wire.h>

#include <HomieNode.hpp>
#include "constants.hpp"

class BME280Node : public HomieNode
{
private:
  // suggested rate is 1/60Hz (1m)
  static const int MIN_INTERVAL = 60; // in seconds
  static const int MEASUREMENT_INTERVAL = 300;
  const char *cCaption = "• BME280 sensor:";
  const char *cIndent = "  ◦ ";

  bool _sensorFound = false;
  unsigned int _i2cAddress;
  unsigned long _measurementInterval;
  unsigned long _lastMeasurement;

  float temperature = NAN;
  float humidity = NAN;
  float pressure = NAN;

  Adafruit_BME280 bme;

  void printCaption();

protected:
  virtual void setup() override;
  virtual void loop() override;

public:
  BME280Node(const char *name,
             const int i2cAddress = 0x77,
             const int measurementInterval = MEASUREMENT_INTERVAL);

  float getHumidity() const { return humidity; }
  float getTemperature() const { return temperature; }
  float getPressure() const { return pressure; }

  void onReadyToOperate();
};
