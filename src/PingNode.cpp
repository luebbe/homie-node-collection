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

PingNode::PingNode(const char *id, const char *name, const char *type, const int triggerPin, const int echoPin,
                   const int measurementInterval, const int publishInterval)
    : SensorNode(id, name, type),
      _triggerPin(triggerPin), _echoPin(echoPin), _lastMeasurement(0), _lastPublish(0)
{
  _measurementInterval = (measurementInterval > MIN_INTERVAL) ? measurementInterval : MIN_INTERVAL;
  _publishInterval = (publishInterval > int(_measurementInterval)) ? publishInterval : _measurementInterval;
  setTemperature(20);

  if (_triggerPin > DEFAULTPIN && _echoPin > DEFAULTPIN)
  {
    sonar = new NewPing(_triggerPin, _echoPin, _maxDistance * 100.0);
  }

  asprintf(&_caption, cCaption, name, type, _triggerPin, _echoPin);

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

void PingNode::send(bool changed)
{
  bool valid = _distance > 0;
  printCaption();
  Homie.getLogger() << cIndent << F("Ping: ") << _ping_us << " " << cUnitMicrosecond << endl;
  Homie.getLogger() << cIndent << F("Distance: ") << _distance << " " << cUnitMeter << endl;
  Homie.getLogger() << cIndent << F("Valid: ") << (valid ? "ok" : "error") << endl;
  Homie.getLogger() << cIndent << F("Changed: ") << (changed ? F("true") : F("false")) << " " << endl;
  if (Homie.isConnected())
  {
    setProperty(cValidTopic).send(valid ? "ok" : "error");
    if (valid)
    {
      setProperty(cDistanceTopic).send(String(_distance));
      setProperty(cPingTopic).send(String(_ping_us));
      setProperty(cChangedTopic).send(changed ? F("true") : F("false"));
    }
  }
}

void PingNode::loop()
{
  if (sonar)
  {
    bool changed = false;
    if (millis() - _lastMeasurement >= _measurementInterval * 1000UL || _lastMeasurement == 0)
    {
      float ping_us = sonar->ping_median((uint8_t)'\005', _maxDistance * 100.0);
      float newDistance = ping_us * _microseconds2meter;
      fixRange(&newDistance, _minDistance, _maxDistance);
      if (newDistance > 0)
      {
        _ping_us = ping_us;
        _distance = newDistance;
        changed = signalChange(_distance, _lastDistance);
        if (changed)
        {
          if (onChange(_distance, _lastDistance))
          {
            _changeHandler();
          }
          _lastDistance = _distance;
        }
      }
      _lastMeasurement = millis();
    }

    if (millis() - _lastPublish >= _publishInterval * 1000UL || _lastPublish == 0)
    {
      if (_distance > 0)
      {
        changed = signalChange(_distance, _lastPublishedDistance);
        send(changed);
        if (changed)
        {
          _lastPublishedDistance = _distance;
        }
        _lastPublish = millis();
      }
    }
  }
}

void PingNode::onReadyToOperate()
{
  if (Homie.isConnected())
  {
    setProperty(cValidTopic).send("ok");
  }
}

void PingNode::setup()
{
  printCaption();
  Homie.getLogger() << cIndent << F("Reading interval: ") << _measurementInterval << " s" << endl;
  Homie.getLogger() << cIndent << F("Publish interval: ") << _publishInterval << " s" << endl;
}

PingNode &PingNode::setTemperature(float temperatureCelcius)
{
  //float soundSpeed = 337.0; // @ 10°C
  float soundSpeed = 331.4 + 0.6 * temperatureCelcius;
  printCaption();
  Homie.getLogger() << cIndent
                    << F("SpeedOfSound: ") << soundSpeed << " " << cUnitMetersPerSecond
                    << F(" at ") << temperatureCelcius << " " << cUnitDegrees << endl;
  // Calculating the distance from d = t_ping /2 * c => t_ping /2 * 337 [m/s] => t_ping_us / 1e-6 * 1/2 * 337
  setMicrosecondsToMeter(0.5e-6 * soundSpeed);
  return *this;
}

PingNode &PingNode::setMicrosecondsToMeter(float microseconds2meter)
{
  _microseconds2meter = microseconds2meter;
  return *this;
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
  return fabs(distance - lastDistance) > _minChange;
}

PingNode &PingNode::setChangeHandler(const ChangeHandler &changeHandler)
{
  _changeHandler = changeHandler;
  return *this;
}

PingNode &PingNode::setMinimumChange(float minimumChange)
{
  _minChange = minimumChange;
  return *this;
}

PingNode &PingNode::setMinimumDistance(float minimumDistance)
{
  _minDistance = minimumDistance;
  return *this;
}

PingNode &PingNode::setMaximumDistance(float maximumDistance)
{
  _maxDistance = maximumDistance;
  return *this;
}
