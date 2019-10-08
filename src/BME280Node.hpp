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

#include "SensorNode.hpp"
#include "constants.hpp"

class BME280Node : public SensorNode
{
private:
  const float cMinTemp = -40.0;
  const float cMaxTemp = 85.0;
  const float cMinPress = 300.0;
  const float cMaxPress = 1100.0;
  // suggested rate is 1/60Hz (1m)
  static const int MIN_INTERVAL = 60; // in seconds
  const char *cCaption = "• BME280 sensor ";
  const char *cIndent = "  ◦ ";

  bool _ready = false;
  bool _sensorFound = false;

  unsigned int _i2cAddress;
  unsigned long _measurementInterval;
  unsigned long _lastMeasurement;

  Adafruit_BME280::sensor_sampling _tempSampling;
  Adafruit_BME280::sensor_sampling _pressSampling;
  Adafruit_BME280::sensor_sampling _humSampling;
  Adafruit_BME280::sensor_filter _filter;

  float temperature = NAN;
  float humidity = NAN;
  float pressure = NAN;

  Adafruit_BME280 bme;

  void printCaption();
  void send();

protected:
  virtual void setup() override;
  virtual void loop() override;
  virtual void onReadyToOperate() override;

public:
  BME280Node(const char *name,
             const int i2cAddress = 0x77,
             const int measurementInterval = MEASUREMENT_INTERVAL,
             Adafruit_BME280::sensor_sampling tempSampling = Adafruit_BME280::SAMPLING_X1,
             Adafruit_BME280::sensor_sampling pressSampling = Adafruit_BME280::SAMPLING_X1,
             Adafruit_BME280::sensor_sampling humSampling = Adafruit_BME280::SAMPLING_X1,
             Adafruit_BME280::sensor_filter filter = Adafruit_BME280::FILTER_OFF);

  float getHumidity() const { return humidity; }
  float getTemperature() const { return temperature; }
  float getPressure() const { return pressure; }

  void beforeHomieSetup();
};
