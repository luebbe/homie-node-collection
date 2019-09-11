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
  uint8_t relayOnValue;
  uint8_t relayOffValue;
  int getRelayPin();
  long timeout;
  /** as the property must have an constant value keep strings here **/
  String relayOnLimitPropertyName; 
  String relayOnLimitPropertyDescription;
  HomieSetting<long> *relayOnLimit;
  virtual void setup() override;
  virtual bool handleInput(const String &property, const HomieRange &range, const String &value);
  virtual void loop() override;
  virtual void setRelayState(bool on);
  virtual bool readRelayState();
  virtual void setupRelay();
  void setRelay(bool on, long timeoutSecs);

public:
  RelayNode(const char *name, const int relayPin = DEFAULTPIN, const int ledPin = DEFAULTPIN, const bool reverseSignal = false);
  void setRelay(bool on);
  void toggleRelay();
  /** 
   * should be called before setup relay timeout wanted 
   * the globalDefaultLimit will be used only in the first call for this function
   **/
  void beforeHomieSetup(const long defaultGlobalLimit, const long defaultLimit);
  void sendState();
};

