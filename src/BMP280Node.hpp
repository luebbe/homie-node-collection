/*
 * BMP280Node.h
 * Homie Node for BMP280 sensors using Adafruit BMP280 library.
 *
 * Version: 1.2
 * Author: Lübbe Onken (http://github.com/luebbe)
 * Author: Markus Haack (http://github.com/mhaack)
 */

#pragma once

#include <Adafruit_BMP280.h>
//#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <Wire.h>

#include "SensorNode.hpp"
#include "constants.hpp"

class BMP280Node : public SensorNode
{
private:
  const float cMinTemp = -40.0;
  const float cMaxTemp = 85.0;
  const float cMinPress = 300.0;
  const float cMaxPress = 1100.0;
  // suggested rate is 1/60Hz (1m)
  static const int MIN_INTERVAL = 60; // in seconds
  const char *cCaption = "• %s BMP280 i2c[0x%2x]:";

  bool _sensorFound = false;

  unsigned int _i2cAddress;
  unsigned long _measurementInterval;
  unsigned long _lastMeasurement;

  Adafruit_BMP280::sensor_sampling _tempSampling;
  Adafruit_BMP280::sensor_sampling _pressSampling;
  Adafruit_BMP280::sensor_filter _filter;

  char *_temperatureOffsetName;

  float temperature = NAN;
  float pressure = NAN;

  Adafruit_BMP280 bmp;

  void send();

protected:
  HomieSetting<double> *_temperatureOffset;

  virtual void setup() override;
  virtual void loop() override;
  virtual void onReadyToOperate() override;

public:
  explicit BMP280Node(const char *id,
                      const char *name,
                      const int i2cAddress = 0x77,
                      const int measurementInterval = MEASUREMENT_INTERVAL,
                      Adafruit_BMP280::sensor_sampling tempSampling = Adafruit_BMP280::SAMPLING_X1,
                      Adafruit_BMP280::sensor_sampling pressSampling = Adafruit_BMP280::SAMPLING_X1,
                      Adafruit_BMP280::sensor_filter filter = Adafruit_BMP280::FILTER_OFF);

  float getTemperature() const { return temperature; }
  float getPressure() const { return pressure; }

  void beforeHomieSetup();
};