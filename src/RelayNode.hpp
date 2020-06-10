/*
 * RelayNode.hpp
 * Homie Node for a Relay with optional status indicator LED
 *
 * Version: 1.2
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#pragma once

#include <Homie.hpp>

#define DEFAULTPIN -1

class RelayNode : public HomieNode
{
public:
  typedef std::function<bool(int8_t)> TGetRelayState;
  typedef std::function<void(int8_t, bool)> TSetRelayState;

private:
  const char *cCaption = "• Relay ";
  const char *cIndent = "  ◦ ";

  std::string _name;
  int8_t _id;

  int8_t _relayPin;
  int8_t _ledPin;

  TGetRelayState _onGetRelayState;
  TSetRelayState _onSetRelayState;

  uint8_t _relayOnValue;
  uint8_t _relayOffValue;

  uint64_t _timeout;
  Ticker _ticker;

  bool handleOnOff(const String &value);
  bool handleTimeout(const String &value);

  void commonInit(bool reverseSignal);
  void printCaption();
  void sendState();
  void tick();

  void setLed(bool on);
  void setTimeout(bool on, long timeoutSecs);

  bool getRelay();

protected:
  virtual bool handleInput(const HomieRange &range, const String &property, const String &value) override;
  virtual void onReadyToOperate() override;
  virtual void setup() override;

public:
  // Use this constructor, if your relay is connected directly to the ESP
  explicit RelayNode(const char *name,
                     const int8_t relayPin = DEFAULTPIN,
                     const int8_t ledPin = DEFAULTPIN,
                     const bool reverseSignal = false);
  // Use this constructor, if your relay is not connected directly, e.g. via a port expander
  explicit RelayNode(const char *name,
                     const uint8_t id,
                     TGetRelayState OnGetRelayState,
                     TSetRelayState OnSetRelayState,
                     const bool reverseSignal = false);
  void setRelay(bool on, long timeoutSecs = 0);
  void toggleRelay();
};
