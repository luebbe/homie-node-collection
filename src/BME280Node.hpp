/*
 * BME280Node.h
 * Homie Node for BME280 sensors using Adafruit BME280 library.
 *
 * Version: 1.1
 * Author: Lübbe Onken (http://github.com/luebbe)
 * Author: Markus Haack (http://github.com/mhaack)
 */

#ifndef SRC_BME280NODE_H_
#define SRC_BME280NODE_H_

#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <Wire.h>

#include <HomieNode.hpp>


class BME280Node : public HomieNode {
private:
  static const int  MEASUREMENT_INTERVAL = 300;
  const std::string cUnit     = "unit";
  const std::string cTemperature = "temperature";
  const std::string cHumidity = "humidity";
  const std::string cPressure = "pressure";
  const std::string cTemperatureUnit = cTemperature + "/" + cUnit;
  const std::string cHumidityUnit = cHumidity + "/" + cUnit;
  const std::string cPressureUnit = cPressure + "/" + cUnit;
  const std::string cCaption  = "• BME280 sensor:";
  const std::string cIndent   = "  ◦ ";

  bool _sensorFound = false;
  unsigned long _measurementInterval;
  unsigned long _lastMeasurement;

  Adafruit_BME280 bme;

  char *printCaption();

public:
  BME280Node(const char *name,
    const int measurementInterval = MEASUREMENT_INTERVAL);

  float getHumidity() const { return humidity; }
  float getTemperature() const { return temperature; }
  float getPressure() const { return pressure; }

  void setupHandler();
  
protected:
  virtual void setup() override;
  virtual void loop() override;

private:
  float temperature = NAN;
  float humidity = NAN;
  float pressure = NAN;
};

#endif
