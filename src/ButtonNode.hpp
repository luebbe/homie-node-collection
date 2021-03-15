/*
 * ButtonNode.hpp
 * Homie Node for a button with optional callback function
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#pragma once

#include "SensorNode.hpp"

#define DEFAULTPIN -1

class ButtonNode : public BaseNode
{
public:
  typedef std::function<void(void)> TButtonPressCallback;
  typedef std::function<void(bool)> TButtonChangeCallback;

private:
  const char *cCaption = "• %s button pin[%d]:";

  int _buttonPin;
  TButtonPressCallback _buttonPressCallback;
  TButtonChangeCallback _buttonChangeCallback;
  byte _lastReading = HIGH;
  byte _buttonState = HIGH;
  bool _buttonPressHandled = 0;
  bool _buttonChangeHandled = 0;
  unsigned long _buttonDownTime = 0;
  unsigned long _minButtonDownTime = 90;
  unsigned long _maxButtonDownTime = 2000;
  unsigned long _lastDebounceTime = 0; // the last time the button pin was toggled

  void handleButtonPress(unsigned long dt);
  void handleButtonChange(bool down);

protected:
  virtual void loop() override;
  virtual void setup() override;

public:
  explicit ButtonNode(const char *id,
                      const char *name,
                      const int buttonPin = DEFAULTPIN,
                      TButtonPressCallback buttonPressedCallback = NULL,
                      TButtonChangeCallback buttonChangedCallback = NULL);
  void onPress(TButtonPressCallback buttonCallback);
  void onChange(TButtonChangeCallback buttonCallback);
  void setMinButtonDownTime(unsigned short downTime);
  void setMaxButtonDownTime(unsigned short downTime);
};
