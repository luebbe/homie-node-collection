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
  if (value == "true" || value == "false")
  {
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

void RelayNode::printCaption()
{
  Homie.getLogger() << cCaption << endl;
}

void RelayNode::setLed(bool on)
{
  if (_ledPin > DEFAULTPIN)
  {
    digitalWrite(_ledPin, on ? LOW : HIGH); // LOW = LED on
  }
}

void RelayNode::setRelay(bool on, long timeoutSecs)
{
  printCaption();

  if (_relayPin > DEFAULTPIN)
  {
    setRelayState(on);
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
  setLed(on);
}

void RelayNode::sendState()
{
  bool on = getRelayState();
  Homie.getLogger() << cIndent << "Relay is " << (on ? "on" : "off") << endl;
  if (Homie.isConnected())
  {
    setProperty("on").send(on ? "true" : "false");
    setProperty("timeout").send(String(_timeout));
  }
}

void RelayNode::setRelayState(bool on)
{
  digitalWrite(_relayPin, on ? _relayOnValue : _relayOffValue);
}

bool RelayNode::getRelayState()
{
  return digitalRead(_relayPin) == _relayOnValue;
}

void RelayNode::toggleRelay()
{
  setRelay(!getRelayState());
}

void RelayNode::setupRelay()
{
  pinMode(_relayPin, OUTPUT);
}

void RelayNode::setup()
{
  printCaption();

  Homie.getLogger() << cIndent << "Relay Pin: " << _relayPin << endl
                    << cIndent << "Led Pin  : " << _ledPin << endl;

  if (_ledPin > DEFAULTPIN)
  {
    pinMode(_ledPin, OUTPUT);
    setLed(false);
  }

  if (_relayPin > DEFAULTPIN)
  {
    setupRelay();
    setRelay(false);
  }
}

void RelayNode::loop()
{
  relayUptime.update();
  if ((_timeout > 0) && getRelayState() && (_timeout < relayUptime.getSeconds()))
  {
    setRelay(false);
  }
}
