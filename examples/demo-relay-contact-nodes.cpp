#define FW_NAME "demo-relay-contact-nodes"
#define FW_VERSION "1.0.0"

#include <Homie.h>

#include "ButtonNode.hpp"
#include "ContactNode.hpp"
#include "RelayNode.hpp"

// Insert your pin number(s) here
const int PIN_LED = D4;     // GPIO 2
const int PIN_CONTACT = D6; // GPIO 12
const int PIN_RELAY = D7;   // GPIO 13
const int PIN_BUTTON = D5;  // GPIO 14

// You need one bool state for each relay with callback
bool relayState;

bool OnGetRelayState(int8_t id);
void OnSetRelayState(int8_t id, bool on);

// Normal relay, hardwired to a GPIO pin
RelayNode relay1("relay1", "RelayDirect", PIN_RELAY, PIN_LED);

// Relay with callback. It could be connected to a port expander and its state is not known inside the relay node.
RelayNode relay2("relay2", "RelayCallback", 1, OnGetRelayState, OnSetRelayState);

// Initialize contact node without callback
ContactNode contactNode("window", "Window", PIN_CONTACT);

// Initialize button node with callback to button press
ButtonNode buttonNode("doorbell", "Doorbell", PIN_BUTTON, []() {
  relay1.toggleRelay();
});

bool OnGetRelayState(int8_t id)
{
  Homie.getLogger() << F("OnGetRelayState Id[") << id << "]=" << relayState << endl;
  return relayState;
}
void OnSetRelayState(int8_t id, bool on)
{
  relayState = on;
  Homie.getLogger() << F("OnSetRelayState Id[") << id << "]=" << relayState << endl;
}

void setup()
{
  Homie_setFirmware(FW_NAME, FW_VERSION);

  Serial.begin(SERIAL_SPEED);
  Serial << endl
         << endl;

  // Set callback for contact node here, just to show alternative
  contactNode.onChange([](bool open) {
    // Pass 0 in timeout for infinite "on"
    relay1.setRelay(open, 0);
  });

  // Set default configuration values before Homie.setup()
  relay1.beforeHomieSetup();
  relay2.beforeHomieSetup();

  Homie.disableLedFeedback();
  Homie.disableResetTrigger();

  Homie.setup();
}

void loop()
{
  Homie.loop();
}
