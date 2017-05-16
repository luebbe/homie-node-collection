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

class RelayNode : public HomieNode {
private:
  int _ledPin;

  void setLed(bool on);

protected:
  virtual void setup() override;
  virtual bool handleInput(const String& property, const HomieRange& range, const String& value) override;

public:
  RelayNode(const char *name, const int ledPin = -1);
  void setRelay(bool on);
};

#endif
