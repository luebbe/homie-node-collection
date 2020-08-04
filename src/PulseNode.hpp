/*
 * PulseNode.hpp
 * Homie Node for a Pulse switch
 *
 * Version: 1.1
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#pragma once

// #define DEBUG_PULSE

#include "SensorNode.hpp"

#define DEFAULTPIN -1
#define CHECK_INTERVAL 1     // Check every second
#define PULSES_PER_SECOND 10 // Minimum number of pulses required per check to be deemed "active" (50Hz should be 20, we go for half)

class PulseNode : public SensorNode
{
public:
  typedef std::function<void(bool)> TStateChangeCallback;
  // typedef std::function<void (*)(void)> TInterruptCallback;

private:
  const char *cCaption = "• %s pulse pin[%d]:";

  TStateChangeCallback _stateChangeCallback;
  uint8_t _pulsePin;

  bool _isPulsing = false;
  bool _lastSentState = true; // force sending of "false" in first loop
  unsigned long _lastCheck = 0;

  // These two values are changed inside the interrupt routine
  volatile unsigned long _pulse = 0;

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
