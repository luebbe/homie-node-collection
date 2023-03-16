/*
 * SensorNode.hpp
 * Homie Node for generic sensors.
 * Provides a limit method for measurement values
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#pragma once

#include <BaseNode.hpp>

class SensorNode : public BaseNode
{
public:
  typedef std::function<void(void)> TOnDataSent;

protected:
  static const int READ_INTERVAL = 300 * 1000UL; // 300 seconds
  static const int SEND_INTERVAL = 300 * 1000UL;

  unsigned long _readInterval;
  unsigned long _sendInterval;
  unsigned long _lastReadTime;
  unsigned long _lastSendTime;
  bool _sensorFound = false;

  const float cMinHumid = 0.0;
  const float cMaxHumid = 100.0;

  TOnDataSent _onDataSent;

  float computeAbsoluteHumidity(float tempCelsius, float percentHumidity);
  float computeDewpoint(float tempCelsius, float percentHumidity);

  virtual unsigned long readInterval();
  virtual unsigned long sendInterval();

  virtual void send() = 0;
  virtual bool sensorFound();
  virtual void takeMeasurement() = 0;

  virtual void loop() override;

public:
  explicit SensorNode(const char *id, const char *name, const char *type,
                      const int readInterval = READ_INTERVAL,
                      const int sendInterval = SEND_INTERVAL);

  void SetOnDataSent(TOnDataSent OnDataSent)
  {
    _onDataSent = OnDataSent;
  }
};
