/*
 * RelayNode.cpp
 * Homie Node for a Relay with optional status indicator LED
 *
 * Version: 1.1
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "RelayNode.hpp"

RelayNode::RelayNode(const char *name, const int relayPin, const int ledPin, const bool reverseSignal)
    : HomieNode(name, "RelayNode", "actor")
{
  _relayPin = relayPin;
  _ledPin = ledPin;
  if (reverseSignal)
  {
    _relayOnValue = LOW;
    _relayOffValue = HIGH;
  }
  else
  {
    _relayOnValue = HIGH;
    _relayOffValue = LOW;
  }
  advertise("on")
      .setDatatype("boolean")
      .settable();

  advertise("timeout")
      .setDatatype("integer")
      .settable();
}

HomieInternals::Uptime relayUptime;

bool RelayNode::handleOnOff(const String &value)
{
  if (value == "true" || value == "false" || value == "toggle")
  {
    if (value == "toggle")
    {
      bool current = getRelay();
      setRelay(!current);
    }
    else
      setRelay(value == "true");
    return true;
  }
  else
  {
    return false;
  }
}

#define IS_INTEGER(s) (s == String(s.toInt()))

bool RelayNode::handleTimeout(const String &value)
{
  if (IS_INTEGER(value))
  {
    long timeout = value.toInt();
    if (timeout > 0)
    {
      setRelay(true, timeout);
      return true;
    }
  }
  return false;
}

bool RelayNode::handleInput(const HomieRange &range, const String &property, const String &value)
{
  Homie.getLogger() << "Message: " << property << " " << value << endl;

  if (property.equals("on"))
  {
    return handleOnOff(value);
  }
  else if (property.equals("timeout"))
  {
    return handleTimeout(value);
  }
  else
  {
    return false;
  }
}

void RelayNode::onReadyToOperate()
{
  setRelay(false);
};

void RelayNode::printCaption()
{
  Homie.getLogger() << cCaption << endl;
}

void RelayNode::sendState()
{
  printCaption();
  bool on = getRelay();
  Homie.getLogger() << cIndent << "Relay is " << (on ? "on" : "off") << endl;
  if (Homie.isConnected())
  {
    setProperty("on").send(on ? "true" : "false");
    setProperty("timeout").send(String(long(_timeout)));
  }
}

void RelayNode::setLed(bool on)
{
  if (_ledPin > DEFAULTPIN)
  {
    digitalWrite(_ledPin, on ? LOW : HIGH); // LOW = LED on
  }
}

bool RelayNode::getRelay()
{
  if (_relayPin > DEFAULTPIN)
  {
    return digitalRead(_relayPin) == _relayOnValue;
  }
  else
  {
    return false;
  }
}

void RelayNode::setRelay(bool on, long timeoutSecs)
{
  if (_relayPin > DEFAULTPIN)
  {
    digitalWrite(_relayPin, on ? _relayOnValue : _relayOffValue);
    if (on && timeoutSecs > 0)
    {
      _timeout = relayUptime.getSeconds() + timeoutSecs;
    }
    else
    {
      _timeout = 0;
    }
    sendState();
  }
  else
  {
    Homie.getLogger() << cIndent << "No Relay Pin!" << endl;
  }
  // Set Led according to relay
  setLed(on);
}

void RelayNode::toggleRelay()
{
  setRelay(!getRelay());
}

void RelayNode::setup()
{
  printCaption();

  Homie.getLogger() << cIndent << "Relay Pin: " << _relayPin << endl
                    << cIndent << "Led Pin  : " << _ledPin << endl;

  if (_ledPin > DEFAULTPIN)
  {
    pinMode(_ledPin, OUTPUT);
  }
  if (_relayPin > DEFAULTPIN)
  {
    pinMode(_relayPin, OUTPUT);
  }
  setRelay(false);
}

void RelayNode::loop()
{
  relayUptime.update();
  if ((_timeout > 0) && getRelay() && (_timeout < relayUptime.getSeconds()))
  {
    setRelay(false);
  }
}
