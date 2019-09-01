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

  void printCaption();
  void setLed(bool on);

protected:
  int getRelayPin();
  long timeout;
  virtual void setup() override;
  virtual void loop() override;
  virtual bool handleInput(const String &property, const HomieRange &range, const String &value) override;
  virtual void setRelayState(bool on);
  virtual bool readRelayState();
  virtual void setupRelay();
  void setRelay(bool on, long timeoutSecs);

public:
  RelayNode(const char *name, const int relayPin = DEFAULTPIN, const int ledPin = DEFAULTPIN);
  void setRelay(bool on);
  void toggleRelay();
};

void relayBeforeHomieSetup();
