/*
 * RelayNode.cpp
 * Homie Node for a Relay with optional status indicator LED
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "RelayNode.hpp"
#include <Homie.hpp>

RelayNode::RelayNode(const char *name, const int relayPin, const int ledPin)
    : HomieNode(name, "RelayNode") {
  _relayPin = relayPin;
  _ledPin = ledPin;
}

bool RelayNode::handleInput(const String& property, const HomieRange& range, const String& value) {
  Homie.getLogger() << "Message: " << value << endl;
  if (value != "true" && value != "false") return false;

  setRelay(value == "true");

  return true;
}

void RelayNode::printCaption() {
  Homie.getLogger() << cCaption <<  endl;
}

void RelayNode::setLed(bool on) {
  if (_ledPin > DEFAULTPIN) {
    digitalWrite(_ledPin, on ? LOW : HIGH); // LOW = LED on
  }
}

void RelayNode::setRelay(bool on) {

  printCaption();

  if (_relayPin > DEFAULTPIN) {
    digitalWrite(_relayPin, on ? HIGH : LOW); // HIGH = close relay
    setProperty("on").send(on ? "true" : "false");
    Homie.getLogger() << cIndent <<  "Relay is " << (on ? "on" : "off") << endl;
  }
  else {
    Homie.getLogger() << cIndent << "No Relay Pin!" << endl;
  }
  setLed(on);
}

void RelayNode::toggleRelay() {
  if (_relayPin > DEFAULTPIN) {
    setRelay(digitalRead(_relayPin) == LOW);
  }
  else {
    printCaption();
    Homie.getLogger() << cIndent << "No Relay Pin!" << endl;
  }
}

void RelayNode::setup() {
  advertise("on").settable();

  printCaption();

  Homie.getLogger() << cIndent << "Relay Pin: " << _relayPin << endl
                    << cIndent << "Led Pin  : " << _ledPin << endl;

  if (_ledPin > DEFAULTPIN) {
    pinMode(_ledPin, OUTPUT);
    setLed(false);
  }

  if (_relayPin > DEFAULTPIN) {
    pinMode(_relayPin, OUTPUT);
    digitalWrite(_relayPin, LOW);
  }
}
