/*
 * PingNode.cpp
 * Homie Node for RCW-0001 sensors using the NewPing library.
 *
 * Version: 1.0
 * Author: Ard Kuijpers (http://github.com/ArdKuijpers)
 */

#include "PingNode.hpp"
#include <Homie.h>

bool checkBounds(float value, float min, float max)
{
  return !isnan(value) && value >= min && value <= max;
}

PingNode::PingNode(const char *name, const int triggerPin, const int echoPin,
                   const int measurementInterval, const int publishInterval)
    : SensorNode(name, "RCW-0001"),
      _triggerPin(triggerPin), _echoPin(echoPin), _lastMeasurement(0), _lastPublish(0)
{
  _measurementInterval = (measurementInterval > MIN_INTERVAL) ? measurementInterval : MIN_INTERVAL;
  _publishInterval = (publishInterval > int(_measurementInterval)) ? publishInterval : _measurementInterval;
  setMicrosecondsToMetersFactor(20);

  if (_triggerPin > DEFAULTPIN && _echoPin > DEFAULTPIN)
  {
    sonar = new NewPing(_triggerPin, _echoPin, cMaxDistance * 100.0);
  }

  advertise(cDistanceTopic)
      .setDatatype("float")
      .setFormat("0:3")
      .setUnit(cUnitMeter);
  advertise(cPingTopic)
      .setDatatype("float")
      .setUnit(cUnitMicrosecond);
  advertise(cValidTopic)
      .setDatatype("enum")
      .setFormat("error, ok");
  advertise(cChangedTopic)
      .setName("Obstacle changed")
      .setDatatype("boolean");
}

void PingNode::printCaption()
{
  Homie.getLogger() << cCaption << " triggerpin[" << _triggerPin << "], echopin[" << _echoPin << "]:" << endl;
}

void PingNode::send()
{
  printCaption();
  Homie.getLogger() << cIndent << "Ping: " << _ping_us << " " << cUnitMicrosecond << endl;
  Homie.getLogger() << cIndent << "Distance: " << _distance << " " << cUnitMeter << endl;
  bool valid = _distance > 0;
  Homie.getLogger() << cIndent << "Valid: " << (valid ? "ok" : "error") << endl;
  bool changed = signalChange(_distance, _lastDistance);
  Homie.getLogger() << cIndent << "Changed: " << (changed ? "true" : "false") << " " << endl;
  if (Homie.isConnected())
  {
    setProperty(cValidTopic).send(valid ? "ok" : "error");
    if (valid)
    {
      setProperty(cDistanceTopic).send(String(_distance));
      setProperty(cPingTopic).send(String(_ping_us));
      setProperty(cChangedTopic).send(changed ? "true" : "false");
    }
  }
  if (changed)
  {
    _changeHandler();
  }
  if (valid)
  {
    _lastDistance = _distance;
    _distance = -1;
  }
}

void PingNode::loop()
{
  if (sonar)
  {
    if (millis() - _lastMeasurement >= _measurementInterval * 1000UL || _lastMeasurement == 0)
    {
      float ping_us = sonar->ping_median();
      // Calculating the distance @ 10 °C from d = t_ping /2 * c => t_ping /2 * 337 [m/s] => t_ping_us / 1e-6 * 1/2 * 337
      float newDistance = ping_us * _microseconds2meter;
      fixRange(&newDistance, cMinDistance, cMaxDistance);
      if (newDistance > 0)
      {
        _ping_us = ping_us;
        _distance = newDistance;
      }
      _lastMeasurement = millis();
    }

    if (millis() - _lastPublish >= _publishInterval * 1000UL || _lastPublish == 0)
      if (_distance > 0)
      {
        send();
        _lastPublish = millis();
        _distance = 0;
      }
  }
}

void PingNode::onReadyToOperate()
{
  if (Homie.isConnected())
  {
    setProperty(cValidTopic).send("ok");
  }
};

void PingNode::setup()
{
  printCaption();
  Homie.getLogger() << cIndent << "Reading interval: " << _measurementInterval << " s" << endl;
  Homie.getLogger() << cIndent << "Publish interval: " << _publishInterval << " s" << endl;
}

void PingNode::setMicrosecondsToMetersFactor(float temperatureCelcius)
{
  //float soundSpeed = 337.0; // @ 10°C
  float soundSpeed = 331.4 + 0.6 * temperatureCelcius;
  printCaption();
  Homie.getLogger() << cIndent
                    << "SpeedOfSound: " << soundSpeed << " " << cUnitMetersPerSecond
                    << " at " << temperatureCelcius << " " << cUnitDegrees << endl;
  // Calculating the distance from d = t_ping /2 * c => t_ping /2 * 337 [m/s] => t_ping_us / 1e-6 * 1/2 * 337
  _microseconds2meter = 0.5e-6 * soundSpeed;
}

float PingNode::getRawEchoTime()
{
  // Clears the trigPin
  digitalWrite(_triggerPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(_triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(_triggerPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  return pulseIn(_echoPin, HIGH);
}

bool PingNode::signalChange(float distance, float lastDistance)
{
  return fabs(distance - lastDistance) > cMinimumChange;
}

PingNode &PingNode::setChangeHandler(const ChangeHandler &changeHandler)
{
  _changeHandler = changeHandler;
  return *this;
}
