/*
 * RelayNode.hpp
 * Homie Node for a Relay with optional status indicator LED
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#ifndef SRC_RELAYNODE_H_
#define SRC_RELAYNODE_H_

#include <HomieNode.hpp>

#define DEFAULTPIN -1

class RelayNode : public HomieNode {
private:
  int _relayPin;
  int _ledPin;

  void setLed(bool on);

protected:
  virtual void setup() override;
  virtual bool handleInput(const String& property, const HomieRange& range, const String& value) override;

public:
  RelayNode(const char *name, const int relayPin = DEFAULTPIN, const int ledPin = DEFAULTPIN);
  void setRelay(bool on);
  void toggleRelay();
};

#endif
