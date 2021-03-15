/*
 * DS18B20Node.hpp
 * Homie Node for Dallas 18B20 sensors.
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 * Author: Marcus Klein (http://github.com/kleini)
 */

#pragma once

#include <OneWire.h>
#include <DallasTemperature.h>

#include "SensorNode.hpp"
#include "constants.hpp"

#define DEFAULTPIN -1

class DS18B20Node : public SensorNode
{
private:
  const float cMinTemp = -55.0;
  const float cMaxTemp = 125.0;
  const char *cCaption = "• %s DS18B20 pin[%d]:";

  int _sensorPin = DEFAULTPIN;

  float temperature = NAN;

  OneWire *oneWire;
  DallasTemperature *dallasTemp;

  void sendData();
  void sendError();

protected:
  virtual void setup() override;
  virtual void onReadyToOperate() override;
  virtual void send() override;
  virtual void takeMeasurement() override;

public:
  explicit DS18B20Node(const char *id,
                       const char *name,
                       const int sensorPin = DEFAULTPIN,
                       const int readInterval = READ_INTERVAL);

  float getTemperature() const { return temperature; }
};
