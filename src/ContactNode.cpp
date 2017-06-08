/*
 * ContactNode.cpp
 * Homie Node for a Contact switch
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "ContactNode.hpp"
#include <Homie.hpp>

ContactNode::ContactNode(const char *name,
  const int contactPin,
  TContactCallback contactCallback)
    : HomieNode(name, "ContactNode") {
  _contactPin = contactPin;
  _contactCallback = contactCallback;
}

// Debounce input pin.
bool ContactNode::debouncePin(void) {
  byte inputState = digitalRead(_contactPin);
  if ( inputState != _lastInputState ) {
    _stateChangedTime = millis();
    _stateChangeHandled = false;
    _lastInputState = inputState;
#ifdef DEBUG
    Homie.getLogger() << "State Changed to " << inputState << endl;
#endif
  }
  else {
    unsigned long dt = millis() - _stateChangedTime;
    if (dt >= 200 && !_stateChangeHandled) {
#ifdef DEBUG
      Homie.getLogger() << "State Stable for " << dt << "ms" << endl;
#endif
      _stateChangeHandled = true;
      return true;
    }
  }
  return false;
}

void ContactNode::handleStateChange(bool open) {
  setProperty("open").send(open ? "true" : "false");
  if (_contactCallback) {
    _contactCallback(open);
  }

  printCaption();
  Homie.getLogger() << cIndent <<  "is " << (open ? "open" : "closed") << endl;
}

void ContactNode::onChange(TContactCallback contactCallback) {
  _contactCallback = contactCallback;
}

void ContactNode::printCaption() {
  Homie.getLogger() << cCaption <<  endl;
}

void ContactNode::loop() {
  if (_contactPin > DEFAULTPIN) {
    if (debouncePin() && (_lastSentState != _lastInputState)) {
      handleStateChange(_lastInputState == HIGH);
      _lastSentState = _lastInputState;
    }
  }
}

void ContactNode::setup() {
  advertise("open");

  printCaption();
  Homie.getLogger() << cIndent <<  "Pin: " << _contactPin << endl;

  if (_contactPin > DEFAULTPIN) {
    pinMode(_contactPin, INPUT_PULLUP);
  }
}
