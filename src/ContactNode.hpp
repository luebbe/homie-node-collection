/*
 * ContactNode.hpp
 * Homie Node for a Contact switch
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#pragma once

#include <Homie.hpp>

#define DEFAULTPIN -1

class ContactNode : public HomieNode {
public:
  typedef std::function<void(bool)> TContactCallback;

private:
  const char *cCaption  = "• Contact:";
  const char *cIndent   = "  ◦ ";

  TContactCallback _contactCallback;
  int _contactPin;
  int _lastInputState = LOW;   // Input pin state.
  int _lastSentState = LOW;    // Input pin state.
  bool _stateChangeHandled = false;
  unsigned long _stateChangedTime = 0;

  bool debouncePin(void);
  void handleStateChange(bool open);
  void printCaption();

protected:
  virtual void loop() override;
  virtual void setup() override;

public:
  ContactNode(const char *name, const int contactPin = DEFAULTPIN, TContactCallback contactCallback = NULL);
  void onChange(TContactCallback contactCallback);
};
