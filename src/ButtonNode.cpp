/*
 * ButtonNode.cpp
 * Homie Node for a button with optional callback function
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "ButtonNode.hpp"

ButtonNode::ButtonNode(const char *name,
                       const int buttonPin,
                       TButtonPressCallback buttonPressCallback,
                       TButtonChangeCallback buttonChangeCallback)
    : HomieNode(name, "ButtonNode", "sensor"),
      _buttonPin(buttonPin),
      _buttonPressCallback(buttonPressCallback),
      _buttonChangeCallback(buttonChangeCallback)
{
}

void ButtonNode::handleButtonPress(unsigned long dt)
{
  if (Homie.isConnected())
  {
    setProperty("duration").send(String(dt));
  }

  printCaption();
  Homie.getLogger() << cIndent << "pressed: " << dt << " ms" << endl;

  if (_buttonPressCallback)
  {
    _buttonPressCallback();
  }
}

void ButtonNode::handleButtonChange(bool down)
{
  if (Homie.isConnected())
  {
    setProperty("down").send(down ? "true" : "false");
  }

  printCaption();
  Homie.getLogger() << cIndent << (down ? "down" : "up") << endl;

  if (_buttonChangeCallback)
  {
    _buttonChangeCallback(down);
  }
}

void ButtonNode::onPress(TButtonPressCallback buttonCallback)
{
  _buttonPressCallback = buttonCallback;
}

void ButtonNode::onChange(TButtonChangeCallback buttonCallback)
{
  _buttonChangeCallback = buttonCallback;
}

void ButtonNode::setMinButtonDownTime(unsigned short downTime)
{
  _minButtonDownTime = downTime;
}

void ButtonNode::setMaxButtonDownTime(unsigned short downTime)
{
  _maxButtonDownTime = downTime;
}

void ButtonNode::printCaption()
{
  Homie.getLogger() << cCaption << endl;
}

void ButtonNode::loop()
{
  // Detect a single press between 90ms and 900ms
  // This could be improved to detect multiple quick or long presses
  // and:
  //  a) report them via MQTT
  //  b) react by calling different callbacks
  if (_buttonPin > DEFAULTPIN)
  {
    byte reading = digitalRead(_buttonPin);

    if (reading != _lastReading)
    {
      // reset the debouncing timer
      _lastDebounceTime = millis();
    }

    if ((millis() - _lastDebounceTime) > _minButtonDownTime)
    {
      // whatever the reading is at, it's been there for longer than the debounce
      // delay, so take it as the actual current state:
      if (reading != _buttonState)
      {
        _buttonState = reading;

        if (_buttonState == LOW)
        {
          handleButtonChange(true);
          _buttonChangeHandled = true;
          _buttonDownTime = millis();
          _buttonPressHandled = false;
        }
        else
        {
          handleButtonChange(false);
          _buttonChangeHandled = true;

          unsigned long dt = millis() - _buttonDownTime;
          if (dt >= _minButtonDownTime && dt <= _maxButtonDownTime && !_buttonPressHandled)
          {
            handleButtonPress(dt);
            _buttonPressHandled = true;
          }
        }
      }
    }
    _lastReading = reading;
  }
}

void ButtonNode::setup()
{
  advertise("down").setDatatype("boolean");
  advertise("duration").setDatatype("integer").setUnit("ms");

  printCaption();
  Homie.getLogger() << cIndent << "Pin: " << _buttonPin << endl;

  if (_buttonPin > DEFAULTPIN)
  {
    pinMode(_buttonPin, INPUT_PULLUP);
  }
}
