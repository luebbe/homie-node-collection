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
  long _timeout;
  /** as the property must have an constant value keep strings here **/
  String _relayOnLimitPropertyName;
  String _relayOnLimitPropertyDescription;
  HomieSetting<long> *_relayOnLimit;

  bool handleOnOff(const String &value);
  bool handleTimeout(const String &value);

  void printCaption();
  void setLed(bool on);

  int getRelayPin();
  void setRelayState(bool on);
  bool readRelayState();
  void setupRelay();
  void setRelay(bool on, long timeoutSecs);

protected:
  virtual bool handleInput(const HomieRange &range, const String &property, const String &value) override;
  virtual void setup() override;
  virtual void loop() override;

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
