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
  typedef std::function<void(PingNode &)> ChangeHandler;

private:
  static const int MIN_INTERVAL = 1; // in seconds
  const char *cCaption = "• RCW-0001 sensor";
  const char *cIndent = "  ◦ ";

  int _triggerPin;
  int _echoPin;
  float _microseconds2meter;

  float _minChange = 0.2;
  float _minDistance = 0.0;
  float _maxDistance = 4.0;
  unsigned long _measurementInterval;
  unsigned long _lastMeasurement;
  unsigned long _publishInterval;
  unsigned long _lastPublish;

  NewPing *sonar;
  float _distance = NAN;
  int _ping_us = 0;
  float _lastDistance = 0;
  ChangeHandler _changeHandler = []() {};

  float getRawEchoTime();
  bool signalChange(float distance, float lastDistance);
  void printCaption();
  void send(bool changed);

protected:
  virtual void setup() override;
  virtual void loop() override;
  virtual void onReadyToOperate() override;
  virtual bool onChange(float newDistance, float prevDistance) { return true; };
  static const int DEFAULT_MEASUREMENT_INTERVAL = 1;
  static const int DEFAULT_PUBLISH_INTERVAL = 5;

public:
  explicit PingNode(const char *name,
                    const int triggerPin = DEFAULTPIN,
                    const int echoPin = DEFAULTPIN,
                    const int measurementInterval = DEFAULT_MEASUREMENT_INTERVAL,
                    const int publishInterval = DEFAULT_PUBLISH_INTERVAL);

  float getDistance() const { return _distance; }
  PingNode &setTemperature(float temperatureCelcius);
  PingNode &setMicrosecondsToMetersFactor(float temperatureCelcius) { return setTemperature(temperatureCelcius); };
  PingNode &setMinimumChange(float minimumChange);
  PingNode &setMinimumDistance(float minimumDistance);
  PingNode &setMaximumDistance(float maximumDistance);
  PingNode &setMicrosecondsToMeters(float microseconds2meter);
  PingNode &setChangeHandler(const ChangeHandler &changeHandler);
};
