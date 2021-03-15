/*
 * SensorNode.hpp
 * Homie Node for genric sensors.
 * Provides a limit method for measurement values
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#pragma once

#include <Homie.hpp>

class SensorNode : public HomieNode
{
protected:
  static const int READ_INTERVAL = 300 * 1000UL;
  static const int SEND_INTERVAL = 300 * 1000UL;
  const char *cIndent = "  ◦ ";
  char *_caption{};

  unsigned long _readInterval;
  unsigned long _sendInterval;
  unsigned long _lastReadTime;
  unsigned long _lastSendTime;
  bool _sensorFound = false;

  const float cMinHumid = 0.0;
  const float cMaxHumid = 100.0;

  float computeAbsoluteHumidity(float temperature, float percentHumidity);
  void fixRange(float *value, float min, float max);

  virtual unsigned long readInterval();
  virtual unsigned long sendInterval();

  virtual void printCaption();
  virtual void send() = 0;
  virtual bool sensorFound();
  virtual void takeMeasurement() = 0;

  virtual void loop() override;

public:
  explicit SensorNode(const char *id, const char *name, const char *type,
                      const int readInterval = READ_INTERVAL,
                      const int sendInterval = SEND_INTERVAL);
};
