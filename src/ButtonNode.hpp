/*
 * ButtonNode.hpp
 * Homie Node for a button with optional callback function
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#pragma once

#include <Homie.hpp>

#define DEFAULTPIN -1

class ButtonNode : public HomieNode {
public:
   typedef std::function<void(void)> TButtonCallback;

private:
  const char *cCaption  = "• Button:";
  const char *cIndent   = "  ◦ ";

  TButtonCallback _buttonCallback;
  int _buttonPin;
  byte _lastButtonState = HIGH;
  bool _buttonPressHandled = 0;
  unsigned long _buttonDownTime = 0;

  void handleButtonPress(unsigned long dt);
  void printCaption();

protected:
  virtual void loop() override;
  virtual void setup() override;

public:
  ButtonNode(const char *name, const int buttonPin = DEFAULTPIN, TButtonCallback buttonCallback = NULL);
  void onPress(TButtonCallback buttonCallback);
};
