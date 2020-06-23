/*
 * ContactNode.hpp
 * Homie Node for a Contact switch
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#pragma once

#include "SensorNode.hpp"

#define DEFAULTPIN -1
#define DEBOUNCE_TIME 200

class ContactNode : public SensorNode
{
public:
  typedef std::function<void(bool)> TContactCallback;

private:
  const char *cCaption = "• Contact %s pin[%d]:";

  int _contactPin;
  TContactCallback _contactCallback;

  // Use invalid values for last states to force sending initial state...
  int _lastInputState = -1; // Input pin state.
  int _lastSentState = -1;  // Last pin state sent
  bool _stateChangeHandled = false;
  unsigned long _stateChangedTime = 0;

  bool debouncePin(void);
  void handleStateChange(bool open);

protected:
  int getContactPin();
  virtual void loop() override;
  virtual void setup() override;
  virtual void setupPin();
  virtual byte readPin();

public:
  explicit ContactNode(const char *name, const int contactPin = DEFAULTPIN, TContactCallback contactCallback = NULL);
  void onChange(TContactCallback contactCallback);
};
