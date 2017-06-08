/*
 * ButtonNode.cpp
 * Homie Node for a button with optional callback function
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "ButtonNode.hpp"
#include <Homie.hpp>

ButtonNode::ButtonNode(const char *name,
  const int buttonPin,
  TButtonCallback buttonCallback)
    : HomieNode(name, "ButtonNode") {
  _buttonPin = buttonPin;
  _buttonCallback = buttonCallback;
}

void ButtonNode::handleButtonPress(unsigned long dt) {
#ifdef DEBUG
  printCaption();
  Homie.getLogger() << cIndent <<  "pressed: " << dt << " ms" << endl;
#endif
  if (_buttonCallback) {
    _buttonCallback();
  }
}

void ButtonNode::onPress(TButtonCallback buttonCallback) {
  _buttonCallback = buttonCallback;
}

void ButtonNode::printCaption() {
  Homie.getLogger() << cCaption <<  endl;
}

void ButtonNode::loop() {
  // Detect a single press between 90ms and 900ms
  // This could be improved to detect multiple quick or long presses
  // and:
  //  a) report them via MQTT
  //  b) react by calling different callbacks
  if (_buttonPin > DEFAULTPIN) {
    byte buttonState = digitalRead(_buttonPin);
    if ( buttonState != _lastButtonState ) {
      if (buttonState == LOW) {
        _buttonDownTime     = millis();
        _buttonPressHandled = false;
      }
      else {
        unsigned long dt = millis() - _buttonDownTime;
        if ( dt >= 90 && dt <= 900 && !_buttonPressHandled ) {
          handleButtonPress(dt);
          _buttonPressHandled = true;
        }
      }
      _lastButtonState = buttonState;
    }
  }
}

void ButtonNode::setup() {
  printCaption();
  Homie.getLogger() << cIndent <<  "Pin: " << _buttonPin << endl;

  if (_buttonPin > DEFAULTPIN) {
    pinMode(_buttonPin, INPUT_PULLUP);
  }
}
