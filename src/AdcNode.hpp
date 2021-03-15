/*
 * AdcNode.cpp
 * Homie Node using the internal ESP ADC to measure voltage.
 *
 * Version: 1.2
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#pragma once

#include "SensorNode.hpp"
#include "constants.hpp"

class AdcNode : public SensorNode
{
private:
  // Read ADC every 10 seconds to have the current value available
  // when another part of the application needs it
  static const int READ_INTERVAL = 10 * 1000;

  const char *cCaption = "• %s ADC:";
  const float cVoltMax = 3.3; // = 100% battery
  const float cVoltMin = 2.6; // =   0% battery

  HomieSetting<double> *_adcCorrection;
  HomieSetting<double> *_adcBattMax;
  HomieSetting<double> *_adcBattMin;

  float _batteryLevel = NAN;
  float _voltage = NAN;

  void readVoltage();
  void sendData();
  void sendError();

protected:
  virtual void setup() override;
  virtual void onReadyToOperate() override;
  virtual void send() override;
  virtual void takeMeasurement() override;

public:
  explicit AdcNode(const char *id, const char *name,
                   const int readInterval = READ_INTERVAL,
                   const int sendInterval = SEND_INTERVAL);

  float getBatteryLevel() const { return _batteryLevel; }
  float getVoltage() const { return _voltage; }
  String getVoltageStr();

  void beforeHomieSetup();
};
