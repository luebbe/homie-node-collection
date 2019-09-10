/*
 * RelayNode.cpp
 * Homie Node for a Relay with optional status indicator LED
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "RelayNode.hpp"


HomieSetting<long> *globalRelayOnLimit;


void RelayNode::beforeHomieSetup(const long defaultGlobalLimit, const long defaultLimit) 
{

  if ((globalRelayOnLimit == NULL) && (defaultGlobalLimit >= 0)) {
      globalRelayOnLimit = new HomieSetting<long>("relayOnLimit", "maximum time in seconds to keep relay(s) in 'on' state (0 disable) - relayOnLimit");
      globalRelayOnLimit->setDefaultValue(defaultGlobalLimit).setValidator([] (long candidate) {
        return (candidate >= 0);
      });
  }

  if ((relayOnLimit == NULL) && (defaultLimit >= -1)) {
      relayOnLimitPropertyName = String(getId()) + "_OnLimit";
      relayOnLimitPropertyDescription = "maximum time in seconds to keep the specific relay '" + String(getId()) + "' in 'on' state (-1 disable, 0 use relayOnLimit)";
      relayOnLimit = new HomieSetting<long>(relayOnLimitPropertyName.c_str(), relayOnLimitPropertyDescription.c_str());
      relayOnLimit->setDefaultValue(defaultLimit).setValidator([] (long candidate) {
        return (candidate >= -1);
      });
  }

}

HomieInternals::Uptime relayUptime;

RelayNode::RelayNode(const char *name, const int relayPin, const int ledPin, const bool reverseSignal)
    : HomieNode(name, "RelayNode", "actor")
{
  _relayPin = relayPin;
  _ledPin = ledPin;
  if (reverseSignal) {
    relayOnValue = LOW;
    relayOffValue = HIGH;
  } else {
    relayOnValue = HIGH;
    relayOffValue = LOW;
  }
}

#define IS_INTEGER(s) ( s == String(s.toInt()) )

bool RelayNode::handleInput(const String &property, const HomieRange &range, const String &value)
{
  Homie.getLogger() << "Message: " << property << " " << value << endl;
  if (property == "on") {
    if (value != "true" && value != "false" && !IS_INTEGER(value)) {
      return false;
    }

    if (IS_INTEGER(value)) {
      long t = value.toInt();
      if (t == 0)
        return false;
      setRelay(true, t);
    } else {
      setRelay(value == "true");
    }

  } else if (property == "timeout" && IS_INTEGER(value)) {
    // reset timeout for the active relay
    if (readRelayState()) {
      setRelay(true, value.toInt());
    }
  } else {
    return false;
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
    digitalWrite(_relayPin, on ? relayOnValue : relayOffValue); 
}

void RelayNode::setRelay(bool on)
{
    long limit = (relayOnLimit != NULL) ? relayOnLimit->get() : 0;
    long globalLimit = (globalRelayOnLimit != NULL) ? globalRelayOnLimit->get() : 0;
    setRelay(on, (limit < 0) ? 0 : (limit > 0 ? limit : globalLimit) );
}

void RelayNode::sendState()
{
    setProperty("on").send(readRelayState() ? "true" : "false");
    setProperty("timeout").send(String(timeout));
}

void RelayNode::setRelay(bool on, long timeoutSecs)
{

  printCaption();

  if (_relayPin > DEFAULTPIN)
  {
    setRelayState(on);
    Homie.getLogger() << cIndent << "Relay is " << (on ? "on" : "off") << endl;
    if (on && timeoutSecs > 0) {
      timeout = relayUptime.getSeconds() + timeoutSecs;
    } else {
      timeout = 0;
    }
    sendState();
  }
  else
  {
    Homie.getLogger() << cIndent << "No Relay Pin!" << endl;
  }
  setLed(on);
}

bool RelayNode::readRelayState() 
{
    return digitalRead(_relayPin) == relayOnValue;
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
}

int RelayNode::getRelayPin() {
    return _relayPin;
}

void RelayNode::setup()
{
  advertise("on").settable();
  advertise("timeout").settable();
  
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
  if ( (timeout > 0) && readRelayState() && (timeout < relayUptime.getSeconds()) ) {
      setRelay(false);
  }
}

