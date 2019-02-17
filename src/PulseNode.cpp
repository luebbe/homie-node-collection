/*
 * PulseNode.cpp
 * Homie Node for a Pulse detector
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "PulseNode.hpp"

PulseNode::PulseNode(const char *name,
                     const uint8_t pulsePin,
                     //  void (*)(void) interruptCallback,
                     TStateChangeCallback stateChangeCallback)
    : HomieNode(name, "PulseNode")
{
  _pulsePin = pulsePin;
  _stateChangeCallback = stateChangeCallback;
}

// Debounce input pin.
bool PulseNode::debouncePulse(void)
{
  if (_pulseState != _lastPulseState)
  {
    _stateChangedTime = millis();
    _stateChangeHandled = false;
    _lastPulseState = _pulseState;
#ifdef DEBUG_PULSE
    Homie.getLogger() << "State Changed to " << _pulseState << endl;
#endif
  }
  else
  {
    unsigned long dt = millis() - _stateChangedTime;
    if (dt >= DEBOUNCE_TIME && !_stateChangeHandled)
    {
#ifdef DEBUG_PULSE
      Homie.getLogger() << "State Stable for " << dt << "ms" << endl;
#endif
      _stateChangeHandled = true;
      return true;
    }
  }
  return false;
}

void PulseNode::handleStateChange(bool active)
{
  setProperty("active").send(active ? "true" : "false");
  if (_stateChangeCallback)
  {
    _stateChangeCallback(active);
  }

  printCaption();
  Homie.getLogger() << cIndent << "is " << (active ? "active" : "not active") << endl;
}

void PulseNode::onChange(TStateChangeCallback stateChangeCallback)
{
  _stateChangeCallback = stateChangeCallback;
}

void PulseNode::printCaption()
{
  Homie.getLogger() << cCaption << endl;
}

void PulseNode::pulseDetected()
{
  _lastPulseTime = millis();
  _pulseState = true;
}

void PulseNode::loop()
{
  if (_pulsePin > DEFAULTPIN)
  {
    if (debouncePulse() && (_lastSentState != _lastPulseState))
    {
      handleStateChange(_lastPulseState);
      _lastSentState = _lastPulseState;
    }

    if (_pulseState && (millis() - _lastPulseTime >= RESET_TIME))
    {
      _pulseState = false;
    }
  }
}

void PulseNode::setup()
{
  advertise("active");

  printCaption();
  Homie.getLogger() << cIndent << "Pin: " << _pulsePin << endl;

  if (_pulsePin > DEFAULTPIN)
  {
    pinMode(_pulsePin, INPUT_PULLUP);
  }
}
