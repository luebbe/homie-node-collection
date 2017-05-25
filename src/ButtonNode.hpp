/*
 * ButtonNode.hpp
 * Homie Node for a button with optional callback function
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#ifndef SRC_BUTTONNODE_H_
#define SRC_BUTTONNODE_H_

#include <HomieNode.hpp>

#define DEFAULTPIN -1

class ButtonNode : public HomieNode {
public:
   typedef std::function<void(void)> TButtonCallback;

private:
  TButtonCallback _buttonCallback;
  int _buttonPin;
  byte _lastButtonState = HIGH;
  bool _buttonPressHandled = 0;
  unsigned long _buttonDownTime = 0;

  void handleButtonPress(unsigned long dt);

protected:
  virtual void loop() override;
  virtual void setup() override;

public:
  ButtonNode(const char *name, const int buttonPin = DEFAULTPIN, TButtonCallback buttonCallback = NULL);
  void onPress(TButtonCallback buttonCallback);
};

#endif
