/*
 * PingNode.h
 * Homie Node for RCW-0001 sensors using the NewPing library.
 *
 * Version: 1.0
 * Author: Ard Kuijpers (http://github.com/ArdKuijpers)
 */

#pragma once

#include "NewPing.h"

#include "SensorNode.hpp"
#include "constants.hpp"

#define DEFAULTPIN -1

class PingNode : public SensorNode
{
public:
  typedef std::function<void()> ChangeHandler;
private:
  const float cMinDistance = 0.0;
  const float cMaxDistance = 3.0;
  const float cMinimumChange = 0.2;

  static const int MIN_INTERVAL = 1; // in seconds
  const char *cCaption = "• RCW-0001 sensor";
  const char *cIndent = "  ◦ ";

  int _triggerPin;
  int _echoPin;
  float _microseconds2meter;
  
  unsigned long _measurementInterval;
  unsigned long _lastMeasurement;
  unsigned long _publishInterval;
  unsigned long _lastPublish;

  NewPing* sonar;
  float _distance = NAN;
  int _ping_us = 0;
  float _lastDistance = 0;
  ChangeHandler _changeHandler = [](){};
  
  float getRawEchoTime();
  void setMicrosecondsToMetersFactor(float temperatureCelcius);
  bool signalChange(float distance, float lastDistance);
  void printCaption();
  void send();
  
protected:
  virtual void setup() override;
  virtual void loop() override;
  virtual void onReadyToOperate() override;
  static const int DEFAULT_MEASUREMENT_INTERVAL = 1;
  static const int DEFAULT_PUBLISH_INTERVAL = 5;


public:
  explicit PingNode(const char *name,
                     const int triggerPin = DEFAULTPIN,
                     const int echoPin = DEFAULTPIN,
                     const int measurementInterval = DEFAULT_MEASUREMENT_INTERVAL,
                     const int publishInterval = DEFAULT_PUBLISH_INTERVAL);

  float getDistance() const { return _distance; }
  void setTemperature(float temperatureCelcius) { setMicrosecondsToMetersFactor(temperatureCelcius); }
  PingNode& setChangeHandler(const ChangeHandler& changeHandler);
};
