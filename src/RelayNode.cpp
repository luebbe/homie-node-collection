/*
 * RelayNode.cpp
 * Homie Node for a Relay with optional status indicator LED
 *
 * Version: 1.2
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "RelayNode.hpp"

RelayNode::RelayNode(const char *name, const int8_t relayPin, const int8_t ledPin, const bool reverseSignal)
    : HomieNode(name, "RelayNode", "actor"),
      _name(name),
      _id(0),
      _relayPin(relayPin),
      _ledPin(ledPin),
      _onGetRelayState(NULL),
      _onSetRelayState(NULL)
{
  commonInit(reverseSignal);
}

RelayNode::RelayNode(const char *name, const uint8_t id, TGetRelayState OnGetRelayState, TSetRelayState OnSetRelayState, const bool reverseSignal)
    : HomieNode(name, "RelayNode", "actor"),
      _name(name),
      _id(id),
      _relayPin(DEFAULTPIN),
      _ledPin(DEFAULTPIN),
      _onGetRelayState(OnGetRelayState),
      _onSetRelayState(OnSetRelayState)
{
  commonInit(reverseSignal);
}

void RelayNode::commonInit(bool reverseSignal)
{
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
  Homie.getLogger() << cCaption << _name.c_str() << ":" << endl;
}

void RelayNode::sendState()
{
  printCaption();
  bool on = getRelay();
  Homie.getLogger() << cIndent << "is " << (on ? "on" : "off") << endl;
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
  if (_onGetRelayState != NULL)
  {
    return _onGetRelayState(_id);
  }
  else if (_relayPin > DEFAULTPIN)
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
  if (_onSetRelayState != NULL)
  {
    _onSetRelayState(_id, on);
    setTimeout(on, timeoutSecs);
    sendState();
  }
  else if (_relayPin > DEFAULTPIN)
  {
    digitalWrite(_relayPin, on ? _relayOnValue : _relayOffValue);
    setTimeout(on, timeoutSecs);
    sendState();
  }
  // Set Led according to relay
  setLed(on);
}

void RelayNode::setTimeout(bool on, long timeoutSecs)
{
  if (on && timeoutSecs > 0)
  {
    _ticker.attach(1.0f, std::bind(&RelayNode::tick, this));
    _timeout = timeoutSecs;
  }
  else
  {
    _ticker.detach();
  }
}

void RelayNode::tick()
{
  if (_timeout > 0)
  {
    _timeout--;
  }
  else
  {
    setRelay(false);
    _ticker.detach();
  }
  if (Homie.isConnected())
  {
    setProperty("timeout").send(String(long(_timeout)));
  }
}

void RelayNode::toggleRelay()
{
  setRelay(!getRelay());
}

void RelayNode::setup()
{
  printCaption();

  std::string info;

  if ((_onSetRelayState != NULL) && (_onGetRelayState != NULL))
  {
    Homie.getLogger() << cIndent << "Callback id: " << _id << endl;
  }
  else if (_relayPin > DEFAULTPIN)
  {
    Homie.getLogger() << cIndent << "Relay Pin: " << _relayPin << endl
                      << cIndent << "Led Pin  : " << _ledPin << endl;
  }
  else
  {
    Homie.getLogger() << cIndent << "No Relay Pin or callback!" << endl;
  }

  if (_ledPin > DEFAULTPIN)
  {
    pinMode(_ledPin, OUTPUT);
  }

  if (_relayPin > DEFAULTPIN)
  {
    pinMode(_relayPin, OUTPUT);
  }
}
