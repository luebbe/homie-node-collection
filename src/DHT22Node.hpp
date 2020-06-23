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

#include "SensorNode.hpp"
#include "constants.hpp"

#define DEFAULTPIN -1

class DHT22Node : public SensorNode
{
private:
  const float cMinTemp = -40.0;
  const float cMaxTemp = 125.0;
  const char *cCaption = "• %s DHT22 pin[%d]:";

  int _sensorPin;
  unsigned long _measurementInterval;
  unsigned long _lastMeasurement;

  float temperature = NAN;
  float humidity = NAN;

  DHT *dht = NULL;

  void send();

protected:
  virtual void setup() override;
  virtual void loop() override;

public:
  explicit DHT22Node(const char *id,
                     const char *name,
                     const int sensorPin = DEFAULTPIN,
                     const int measurementInterval = MEASUREMENT_INTERVAL);

  float getHumidity() const { return humidity; }
  float getTemperature() const { return temperature; }
};
