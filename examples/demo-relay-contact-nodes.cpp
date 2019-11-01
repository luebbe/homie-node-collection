#define FW_NAME "demo-relay-contact-nodes"
#define FW_VERSION "1.0.0"

#include <Homie.h>

#include "ButtonNode.hpp"
#include "ContactNode.hpp"
#include "RelayNode.hpp"

// Insert your pin number(s) here
const int PIN_LED = 2;      // =D4 on Wemos
const int PIN_CONTACT = 12; // =D6 on Wemos
const int PIN_RELAY = 13;   // =D7 on Wemos
const int PIN_BUTTON = 14;  // =D5 on Wemos

// Create one node of each kind
RelayNode relayNode("relay", PIN_RELAY, PIN_LED);

// Initialize contact node without callback
ContactNode contactNode("contact", PIN_CONTACT);

// Initialize button node with callback to button press
ButtonNode buttonNode("button", PIN_BUTTON, []() {
  relayNode.toggleRelay();
});

void setup()
{
  Homie_setFirmware(FW_NAME, FW_VERSION);

  Serial.begin(SERIAL_SPEED);
  Serial << endl
         << endl;

  // Set callback for contact node here, just to show alternative
  contactNode.onChange([](bool open) {
    relayNode.setRelay(open);
  });

  Homie.disableLedFeedback();
  Homie.disableResetTrigger();

  Homie.setup();
}

void loop()
{
  Homie.loop();
}
