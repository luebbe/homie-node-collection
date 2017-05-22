/*
 * ContactNode.hpp
 * Homie Node for a Contact switch
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#ifndef SRC_CONTACTNODE_H_
#define SRC_CONTACTNODE_H_

#include <HomieNode.hpp>

#define DEFAULTPIN -1

class ContactNode : public HomieNode {
  int _contactPin;
  int _lastInputState = LOW;   // Input pin state.
  int _lastSentState = LOW;    // Input pin state.
  int _stateChangeHandled = 0;
  unsigned long _stateChangedTime = 0;
  bool DebouncePin(void);
protected:
  virtual void loop() override;
  virtual void setup() override;

public:
  ContactNode(const char *name, const int contactPin = DEFAULTPIN);
};

#endif
