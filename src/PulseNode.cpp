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
  advertise("pulses")
      .setDatatype("float")
      .setUnit(cUnitHz);
}

// Debounce input pin.
void PulseNode::checkState(void)
{
  noInterrupts();
  unsigned long _copyPulse = _pulse;
  _pulse = 0;
  interrupts();

  _isPulsing = (_copyPulse > PULSES_PER_SECOND);

  float _frequency = _copyPulse * 1000 / CHECK_INTERVAL;
  setProperty("pulses").send(String(_frequency));

#ifdef DEBUG_PULSE
  Homie.getLogger() << "Active: " << _isPulsing << " pulses: " << _copyPulse << " frequency:" << _frequency << endl;
#endif
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
    if ((millis() - _lastCheck >= CHECK_INTERVAL) || (_lastCheck == 0))
    {
      checkState();
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
