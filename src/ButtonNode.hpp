/*
 * ButtonNode.hpp
 * Homie Node for a button with optional callback function
 *
 * Version: 1.1
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#pragma once

#include "SensorNode.hpp"
#include "push_button.h"

#define DEFAULTPIN -1

using pb::PushButton;

class ButtonNode : public BaseNode
{
public:
  typedef std::function<void(uint8_t)> TButtonPressCallback;

private:
  const char *cCaption = "• %s button pin[%d]:";

  PushButton *_pb;
  TButtonPressCallback _buttonPressCallback;

protected:
  virtual void loop() override;
  virtual void setup() override;

public:
  explicit ButtonNode(const char *id,
                      const char *name,
                      const int buttonPin = DEFAULTPIN,
                      TButtonPressCallback buttonPressedCallback = NULL);
  void onPress(TButtonPressCallback buttonCallback);
};
