/*
 * DHT22Node.hpp
 * Homie Node for DHT-22 sensors using Adafruit DHT library.
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#ifndef SRC_DHT22NODE_H_
#define SRC_DHT22NODE_H_

#include <Adafruit_Sensor.h>
#include <DHT_U.h>

#include <HomieNode.hpp>

#define DEFAULTPIN -1
const int  MEASUREMENT_INTERVAL = 300;

class DHT22Node : public HomieNode {
private:
  int _sensorPin;
  unsigned long _measurementInterval;
  unsigned long _lastMeasurement;

  DHT* dht;

public:
  DHT22Node(const char *name,
    const int sensorPin = DEFAULTPIN,
    const int measurementInterval = MEASUREMENT_INTERVAL);

  float getHumidity() const { return humidity; }
  float getTemperature() const { return temperature; }

protected:
  virtual void setup() override;
  virtual void loop() override;

private:
  float temperature = NAN;
  float humidity = NAN;
};

#endif
