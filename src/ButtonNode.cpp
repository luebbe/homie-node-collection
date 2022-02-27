/*
 * ButtonNode.cpp
 * Homie Node for a button with optional callback function
 *
 * Version: 1.1
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "ButtonNode.hpp"

ButtonNode::ButtonNode(const char *id,
                       const char *name,
                       const int buttonPin,
                       TButtonPressCallback buttonPressCallback)
    : BaseNode(id, name, "Button"),
      _buttonPressCallback(buttonPressCallback)
{
  // asprintf(&_caption, cCaption, name, buttonPin);
  if (buttonPin != DEFAULTPIN)
  {
    _pb = new PushButton(buttonPin);
  }
}

void ButtonNode::onPress(TButtonPressCallback buttonCallback)
{
  _buttonPressCallback = buttonCallback;
}

void ButtonNode::loop()
{
  PushButton::Event event;
  event = _pb->getEvent();

  if ((event != PushButton::Event::NONE) && (event != PushButton::Event::NOT_READY) && _buttonPressCallback)
  {
    _buttonPressCallback(uint8_t(event));
  }
}

void ButtonNode::setup()
{
  advertise("down").setDatatype("boolean");
  advertise("duration").setDatatype("integer").setUnit("ms");

  printCaption();
}
