#define FW_NAME "demo-all-nodes"
#define FW_VERSION "1.0.0"

#include <Homie.h>
#include "homie-node-collection.h"

// Insert your pin number(s) here
#define PIN_LED      2
#define PIN_DHT      0
#define PIN_SDA      4
#define PIN_SCL      5
#define PIN_CONTACT 12
#define PIN_RELAY   13
#define PIN_BUTTON  14

#define SERIAL_SPEED 115200

// Setup OTA logging via Homie logger
OtaLogger ota;

// Create one node of each kind
BME280Node  bme280Node("bme280");
DHT22Node   dht22Node("dht22", PIN_DHT);
RelayNode   relayNode("relay", PIN_RELAY, PIN_LED);
AdcNode     adcNode("adc");

// Initialize contact node without callback
ContactNode contactNode("contact", PIN_CONTACT);

// Initialize button node with callback to button press
ButtonNode  buttonNode("button", PIN_BUTTON, []() {
  relayNode.toggleRelay();
});

void setupHandler() {
  // Advertise units
  bme280Node.setupHandler();
  dht22Node.setupHandler();
  adcNode.setupHandler();
};

void loopHandler() {
  // We want to OTA handler to run only in the loop when WiFi is connected
  ota.loop();
}

void setup() {
  Serial.begin(SERIAL_SPEED);
  Serial << endl << endl;

  welcome();
  ota.setup();

  // Initializes I2C for BME280 sensor
  Wire.begin(PIN_SDA, PIN_SCL);

  // Set callback for contact node here, just to show alternative
  contactNode.onChange([](bool open) {
    relayNode.setRelay(open);
  });

  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie.setSetupFunction(setupHandler);
  Homie.setLoopFunction(loopHandler);

  Homie.setup();
}

void loop() {
  Homie.loop();
}
