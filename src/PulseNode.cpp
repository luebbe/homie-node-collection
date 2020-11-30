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

  asprintf(&_checkIntervalName, "%s.interval", id);
  _checkInterval = new HomieSetting<long>(_checkIntervalName, "The interval in which to check for pulses [1 .. Max(long)ms]. Default = 5000ms");
  asprintf(&_checkActivePulsesName, "%s.activePulses", id);
  _checkActivePulses = new HomieSetting<long>(_checkActivePulsesName, "The number of pulses per interval to be considered active [1 .. Max(long)]. Default = 10");

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

  _isPulsing = (_copyPulse > (unsigned long)_checkActivePulses->get());

  float _frequency = _copyPulse * 1000 / _checkInterval->get();
  setProperty("pulses").send(String(_frequency));

#ifdef DEBUG_PULSE
  Homie.getLogger() << F("Active: ") << _isPulsing << F(" pulses: ") << _copyPulse << F(" frequency:") << _frequency << endl;
#endif
}

void PulseNode::handleStateChange(bool active)
{
  if (Homie.isConnected())
  {
    setProperty("active").send(active ? F("true") : F("false"));
  }

  if (_stateChangeCallback)
  {
    _stateChangeCallback(active);
  }

  printCaption();
  Homie.getLogger() << cIndent << F("is ") << (active ? "" : "not ") << F("active") << endl;
}

void PulseNode::onChange(TStateChangeCallback stateChangeCallback)
{
  _stateChangeCallback = stateChangeCallback;
}

void IRAM_ATTR PulseNode::onInterrupt()
{
  _pulse++;
}

void PulseNode::beforeHomieSetup()
{
  _checkInterval->setDefaultValue(DEFAULT_INTERVAL).setValidator([](long candidate) {
    return (candidate > 0);
  });
  _checkActivePulses->setDefaultValue(PULSES_PER_INTERVAL).setValidator([](long candidate) {
    return (candidate > 0);
  });
}

void PulseNode::loop()
{
  if (_pulsePin > DEFAULTPIN)
  {
    if ((millis() - _lastCheck >= (unsigned long)_checkInterval->get()) || (_lastCheck == 0))
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
