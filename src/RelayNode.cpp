/*
 * RelayNode.cpp
 * Homie Node for a Relay with optional status indicator LED
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "RelayNode.hpp"


HomieSetting<long> relayOnLimit("relayOnLimit", "maximum time in seconds to keep relay in on state (0 disable) ");

void relayBeforeHomieSetup() 
{
  relayOnLimit.setDefaultValue(0).setValidator([] (long candidate) {
    return (candidate >= 0);
  });
}

HomieInternals::Uptime relayUptime;

RelayNode::RelayNode(const char *name, const int relayPin, const int ledPin)
    : HomieNode(name, "RelayNode")
{
  _relayPin = relayPin;
  _ledPin = ledPin;
}

bool RelayNode::handleInput(const String &property, const HomieRange &range, const String &value)
{
  Homie.getLogger() << "Message: " << property << " " << value << endl;
  if (property == "on") {
    if (value != "true" && value != "false") {
      long t = value.toInt();
      if (t == 0)
        return false;
      setRelay(true, t);
    } else {
      setRelay(value == "true");
    }
  } else if (property == "timeout") {
    timeout = value.toInt();
  }
  return true;
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

void RelayNode::setRelayState(bool on)
{
    digitalWrite(_relayPin, on ? HIGH : LOW); // HIGH = close relay
}

void RelayNode::setRelay(bool on)
{
    setRelay(on, relayOnLimit.get());
}

void RelayNode::setRelay(bool on, long timeoutSecs)
{

  printCaption();

  if (_relayPin > DEFAULTPIN)
  {
    setRelayState(on);
    setProperty("on").send(on ? "true" : "false");
    Homie.getLogger() << cIndent << "Relay is " << (on ? "on" : "off") << endl;
    if (on && timeoutSecs > 0) {
      timeout = relayUptime.getSeconds() + timeoutSecs;
    } else {
      timeout = 0;
    }
    setProperty("timeout").send(String(timeout));
  }
  else
  {
    Homie.getLogger() << cIndent << "No Relay Pin!" << endl;
  }
  setLed(on);
}

bool RelayNode::readRelayState() 
{
    return digitalRead(_relayPin) == HIGH;
}

void RelayNode::toggleRelay()
{
  if (_relayPin > DEFAULTPIN)
  {
    setRelay(!readRelayState());
  }
  else
  {
    printCaption();
    Homie.getLogger() << cIndent << "No Relay Pin!" << endl;
  }
}

void RelayNode::setupRelay() 
{
    pinMode(_relayPin, OUTPUT);
    digitalWrite(_relayPin, LOW);
}

int RelayNode::getRelayPin() {
    return _relayPin;
}

void RelayNode::setup()
{
  advertise("on").settable();
  advertise("timeout");
  
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
  }
}

void RelayNode::loop()
{
  relayUptime.update();
  if ( (timeout > 0) && readRelayState() && (timeout < relayUptime.getSeconds()) ) {
      setRelay(false);
  }
}

