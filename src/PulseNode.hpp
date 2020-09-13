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
#include "constants.hpp"

#define DEFAULTPIN -1
#define DEFAULT_INTERVAL 1000 * 5  // Check every five seconds. 
#define PULSES_PER_INTERVAL 10 * 5 // Minimum number of pulses required per check to be deemed "active" (50Hz should be 20/second, we go for half).

class PulseNode : public SensorNode
{
public:
  typedef std::function<void(bool)> TStateChangeCallback;
  // typedef std::function<void (*)(void)> TInterruptCallback;

private:
  const char *cCaption = "• %s pulse pin[%d]:";

  TStateChangeCallback _stateChangeCallback;
  uint8_t _pulsePin;

  char *_checkIntervalName;
  char *_checkActivePulsesName;

  bool _isPulsing = false;
  bool _lastSentState = true; // force sending of "false" in first loop
  unsigned long _lastCheck = 0;

  // This value is changed inside the interrupt routine
  volatile unsigned long _pulse = 0;

  void checkState(void);
  void handleStateChange(bool active);

protected:
  HomieSetting<long> *_checkInterval;
  HomieSetting<long> *_checkActivePulses;

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
  void beforeHomieSetup();
};
