/*
 * RelayNode.hpp
 * Homie Node for a Relay with optional status indicator LED
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#pragma once

#include <Homie.hpp>

#define DEFAULTPIN -1

class RelayNode : public HomieNode
{
private:
  const char *cCaption = "• Relay:";
  const char *cIndent = "  ◦ ";

  int _relayPin;
  int _ledPin;

  uint8_t _relayOnValue;
  uint8_t _relayOffValue;
  uint64_t _timeout;

  bool handleOnOff(const String &value);
  bool handleTimeout(const String &value);

  void printCaption();
  void sendState();

  void setupRelay();
  bool getRelayState();
  void setRelayState(bool on);

  void setLed(bool on);

protected:
  virtual bool handleInput(const HomieRange &range, const String &property, const String &value) override;
  virtual void setup() override;
  virtual void loop() override;

public:
  explicit RelayNode(const char *name, const int relayPin = DEFAULTPIN, const int ledPin = DEFAULTPIN, const bool reverseSignal = false);
  void setRelay(bool on, long timeoutSecs = 0);
  void toggleRelay();
};
