/*
 * PulseNode.hpp
 * Homie Node for a Pulse switch
 *
 * Version: 1.1
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#pragma once

// #define DEBUG_PULSE

#ifdef DEBUG_PULSE
// #define DEBUG_INTERRUPT
#endif

#include "SensorNode.hpp"

#define DEFAULTPIN -1
#define DEBOUNCE_MS 100 // ms If pulses are (not) detected for at least this amount of time, the state changes

class PulseNode : public SensorNode
{
public:
  typedef std::function<void(bool)> TStateChangeCallback;
  // typedef std::function<void (*)(void)> TInterruptCallback;

private:
  const char *cCaption = "• %s pulse pin[%d]:";

  TStateChangeCallback _stateChangeCallback;
  uint8_t _pulsePin;

  bool _lastSentState = true; // force sending of "false" in first loop
  bool _stateChangeHandled = false;

  // These two values are changed inside the interrupt routine
  volatile bool _isPulsing = false;
  volatile unsigned long _firstPulseTime = 0;
  volatile unsigned long _lastPulseTime = 0;

  bool debouncePulse(void);
  void handleStateChange(bool active);

protected:
  virtual void loop() override;
  virtual void setup() override;

public:
  explicit PulseNode(const char *id,
                     const char *name,
                     const uint8_t pulsePin = DEFAULTPIN,
                     // void (*)(void) interruptCallback,
                     TStateChangeCallback stateChangeCallback = NULL);
  void onChange(TStateChangeCallback stateChangeCallback);
  void IRAM_ATTR onInterrupt();
};
