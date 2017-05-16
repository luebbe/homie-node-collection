/*
 * RelayNode.cpp
 * Homie Node for a Relay with optional status indicator LED
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "RelayNode.hpp"
#include <Homie.hpp>

#define RELAYPIN  0

HomieSetting<long>
    relayPinSetting("relayPin", "The pin to which the relay is connected");

RelayNode::RelayNode(const char *name, const int ledPin)
    : HomieNode(name, "RelayNode") {

  _ledPin = ledPin;

  relayPinSetting.setDefaultValue(RELAYPIN);

  if (_ledPin > -1) {
    pinMode(_ledPin, OUTPUT);
    setLed(false);
  }
}

void RelayNode::setLed(bool on) {
  if (_ledPin > -1) {
    digitalWrite(_ledPin, on ? LOW : HIGH); // LOW = LED on
  }
}

void RelayNode::setRelay(bool on) {
  digitalWrite(relayPinSetting.get(), on ? HIGH : LOW); // HIGH = close relay
  setLed(on);

  setProperty("on").send(on ? "true" : "false");
  Homie.getLogger() << "Relay is " << (on ? "on" : "off") << endl;
}

bool RelayNode::handleInput(const String& property, const HomieRange& range, const String& value) {
  Homie.getLogger() << "Message: " << value << endl;
  if (value != "true" && value != "false") return false;

  setRelay(value == "true");

  return true;
}

void RelayNode::setup() {
  advertise("on").settable();

  Homie.getLogger() << "Relay pin = " << relayPinSetting.get() << endl
                    << "Was provided = " << relayPinSetting.wasProvided() << endl
                    << "Led pin = " << _ledPin << endl;

  pinMode(relayPinSetting.get(), OUTPUT);
  Homie.getLogger() << "Set OUTPUT" << endl;
  digitalWrite(relayPinSetting.get(), LOW);
  Homie.getLogger() << "Set off" << endl;
}
//
// void RelayNode::loop() {
// }
