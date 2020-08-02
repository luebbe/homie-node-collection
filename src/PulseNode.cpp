/*
 * PulseNode.cpp
 * Homie Node for a Pulse detector
 *
 * Version: 1.1
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "PulseNode.hpp"

PulseNode::PulseNode(const char *id,
                     const char *name,
                     const uint8_t pulsePin,
                     //  void (*)(void) interruptCallback,
                     TStateChangeCallback stateChangeCallback)
    : SensorNode(id, name, "Pulse")
{
  _pulsePin = pulsePin;
  _stateChangeCallback = stateChangeCallback;

  asprintf(&_caption, cCaption, name, pulsePin);

  advertise("active")
      .setDatatype("boolean");
}

// Debounce input pin.
bool PulseNode::debouncePulse(void)
{
  if (_isPulsing)
  {
    unsigned long now = millis();

    // First check if there was no pulse for more than DEBOUNCE_MS
    // -> reset
    if (now - DEBOUNCE_MS >= _lastPulseTime)
    {
#ifdef DEBUG_PULSE
      Homie.getLogger() << millis() << " Is not pulsing anymmore" << endl;
#endif
      _isPulsing = false;
      _firstPulseTime = now;
      return true;
    }

    // Then check if there were pulses for more than DEBOUNCE_MS
    // -> we're happy
    else if (_lastPulseTime - DEBOUNCE_MS >= _firstPulseTime)
    {
#ifdef DEBUG_PULSE
      Homie.getLogger() << millis() << " Is pulsing" << endl;
#endif
      return true;
    }
    return false;
  }
  // No pulses -> debounced for sure
  return true;
}

void PulseNode::handleStateChange(bool active)
{
  if (Homie.isConnected())
  {
    setProperty("active").send(active ? "true" : "false");
  }

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

void IRAM_ATTR PulseNode::onInterrupt()
{
#ifdef DEBUG_INTERRUPT
  // when triggering on falling edge, digitalRead should always return zero
  Homie.getLogger() << millis() << " " << digitalRead(_pulsePin) << endl;
#endif
  _lastPulseTime = millis();
  if (!_isPulsing)
  {
    _isPulsing = true;
    _firstPulseTime = _lastPulseTime;
  }
}

void PulseNode::loop()
{
  if (_pulsePin > DEFAULTPIN)
  {
    if (debouncePulse() && (_lastSentState != _isPulsing))
    {
      handleStateChange(_isPulsing);
      _lastSentState = _isPulsing;
    }
  }
}

void PulseNode::setup()
{
  printCaption();

  if (_pulsePin > DEFAULTPIN)
  {
    pinMode(_pulsePin, INPUT_PULLUP);
  }
}
