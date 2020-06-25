/*
 * RelayNode.cpp
 * Homie Node for a Relay with optional status indicator LED
 *
 * Version: 1.2
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "RelayNode.hpp"

RelayNode::RelayNode(const char *id, const char *name, const int8_t relayPin, const int8_t ledPin, const bool reverseSignal)
    : HomieNode(id, name, "Relay"),
      _callbackId(0),
      _relayPin(relayPin),
      _ledPin(ledPin),
      _onGetRelayState(NULL),
      _onSetRelayState(NULL)
{
  asprintf(&_caption, "• %s relay pin[%d]:", name, relayPin);
  commonInit(id, reverseSignal);
}

RelayNode::RelayNode(const char *id, const char *name, const uint8_t callbackId, TGetRelayState OnGetRelayState, TSetRelayState OnSetRelayState, const bool reverseSignal)
    : HomieNode(id, name, "Relay"),
      _callbackId(callbackId),
      _relayPin(DEFAULTPIN),
      _ledPin(DEFAULTPIN),
      _onGetRelayState(OnGetRelayState),
      _onSetRelayState(OnSetRelayState)
{
  asprintf(&_caption, "• %s relay id[%d]:", name, callbackId);
  commonInit(id, reverseSignal);
}

void RelayNode::commonInit(const char *id, bool reverseSignal)
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

  asprintf(&_maxTimeoutName, "%s.maxTimeout", id);
  _maxTimeout = new HomieSetting<long>(_maxTimeoutName, "The maximum timeout for the relay in seconds [0 .. Max(long)] Default = 600 (10 minutes)");

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
      setRelay(!current, _maxTimeout->get());
    }
    else
      setRelay(value == "true", _maxTimeout->get());
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
#ifdef DEBUG
  Homie.getLogger() << "Message: " << property << " " << value << endl;
#endif
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

void RelayNode::beforeHomieSetup()
{
  _maxTimeout->setDefaultValue(600).setValidator([](long candidate) {
    return (candidate >= 0);
  });
}

void RelayNode::onReadyToOperate()
{
  setRelay(false, 0);
};

void RelayNode::printCaption()
{
  Homie.getLogger() << _caption << endl;
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
    return _onGetRelayState(_callbackId) == _relayOnValue;
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
    _onSetRelayState(_callbackId, on ? _relayOnValue : _relayOffValue);
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
  long maxTimeout = _maxTimeout->get();

  if ((maxTimeout > 0) && maxTimeout < timeoutSecs)
  {
    timeoutSecs = maxTimeout;
  }

  if (on && timeoutSecs > 0)
  {
    _ticker.attach(1.0f, std::bind(&RelayNode::tick, this));
    _timeout = timeoutSecs;
  }
  else
  {
    _ticker.detach();
    _timeout = 0;
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
    setRelay(false, 0);
    _ticker.detach();
  }
  if (Homie.isConnected())
  {
    setProperty("timeout").send(String(long(_timeout)));
  }
}

void RelayNode::toggleRelay()
{
  setRelay(!getRelay(), _maxTimeout->get());
}

void RelayNode::setup()
{
  printCaption();

  if ((_onSetRelayState == NULL) && (_onGetRelayState == NULL) && (_relayPin == DEFAULTPIN))
  {
    Homie.getLogger() << cIndent << "No Relay Pin or callback defined!" << endl;
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
