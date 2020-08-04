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
  noInterrupts();
  float copyPulse = _pulse;
  _pulse = 0;
  interrupts();

  _isPulsing = (copyPulse > PULSES_PER_SECOND);

#ifdef DEBUG_PULSE
  Homie.getLogger() << "Active: " << _isPulsing << " pulses: " << copyPulse << endl;
#endif

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
  _pulse++;
}

void PulseNode::loop()
{
  if (_pulsePin > DEFAULTPIN)
  {
    if ((millis() - _lastCheck >= CHECK_INTERVAL * 1000UL) || (_lastCheck == 0))
    {
      debouncePulse();
      if (_lastSentState != _isPulsing)
      {
        handleStateChange(_isPulsing);
        _lastSentState = _isPulsing;
      }
      _lastCheck = millis();
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
