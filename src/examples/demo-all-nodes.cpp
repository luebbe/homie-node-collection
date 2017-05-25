#define FW_NAME "demo-all-nodes"
#define FW_VERSION "1.0.0"

#include <Homie.h>
#include "DHT22Node.hpp"
#include "BME280Node.hpp"
#include "ContactNode.hpp"
#include "RelayNode.hpp"
#include "ButtonNode.hpp"

// Insert your pin number(s) here
#define PIN_CONTACT 5
#define PIN_RELAY   4
#define PIN_LED     2
#define PIN_DHT     0
#define PIN_SDA    12
#define PIN_SCL    13
#define PIN_BUTTON 14

// Create one node of each kind
BME280Node  bme280Node("bme280");
DHT22Node   dht22Node("dht22", PIN_DHT);
RelayNode   relayNode("relay", PIN_RELAY, PIN_LED);

// Initialize contact node without callback
ContactNode contactNode("contact", PIN_CONTACT);

// Initialize button node with callback to button press
ButtonNode  buttonNode("button", PIN_BUTTON, []() {
  relayNode.toggleRelay();
});

void setup() {
  Serial.begin(SERIAL_SPEED);
  Serial << endl << endl;

  // Initializes I2C for BME280 sensor
  Wire.begin(PIN_SDA, PIN_SCL);

  // Set callback forcontact node here, just to show alternative
  contactNode.onChange([](bool open) {
    relayNode.setRelay(open);
  });

  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie.setup();
}

void loop() {
  Homie.loop();
  #ifdef ENABLE_ARDUINO_OTA
    void handleOTA();
    handleOTA();
  #endif
}
