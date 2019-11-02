/*
 * PulseNode.hpp
 * Homie Node for a Pulse switch
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#pragma once

#include <Homie.hpp>

#define DEFAULTPIN -1
#define DEBOUNCE_TIME 50
#define RESET_TIME 200

class PulseNode : public HomieNode
{
public:
  typedef std::function<void(bool)> TStateChangeCallback;
  // typedef std::function<void (*)(void)> TInterruptCallback;

private:
  const char *cCaption = "• Pulse:";
  const char *cIndent = "  ◦ ";

  TStateChangeCallback _stateChangeCallback;
  uint8_t _pulsePin;
  bool _pulseState = false;
  bool _lastPulseState = true; // force sending of "false" in first loop
  bool _lastSentState = true;
  bool _stateChangeHandled = false;
  unsigned long _lastPulseTime = 0;
  unsigned long _stateChangedTime = 0;

  bool debouncePulse(void);
  void handleStateChange(bool open);
  void printCaption();

protected:
  virtual void loop() override;
  virtual void setup() override;

public:
  explicit PulseNode(const char *name, const uint8_t pulsePin = DEFAULTPIN,
                     // void (*)(void) interruptCallback,
                     TStateChangeCallback stateChangeCallback = NULL);
  void onChange(TStateChangeCallback stateChangeCallback);
  void pulseDetected();
};
