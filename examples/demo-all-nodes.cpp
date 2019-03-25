#define FW_NAME "demo-all-nodes"
#define FW_VERSION "1.0.0"

#include <Homie.h>

#include "AdcNode.hpp"
#include "BME280Node.hpp"
#include "ButtonNode.hpp"
#include "ContactNode.hpp"
#include "DHT22Node.hpp"
#include "RelayNode.hpp"

// Insert your pin number(s) here
const int PIN_LED = 2;      // =D4 on Wemos
const int PIN_DHT = 0;      // =D3 on Wemos
const int PIN_SDA = 4;      // =D2 on Wemos
const int PIN_SCL = 5;      // =D1 on Wemos
const int PIN_CONTACT = 12; // =D6 on Wemos
const int PIN_RELAY = 13;   // =D7 on Wemos
const int PIN_BUTTON = 14;  // =D5 on Wemos

const int I2C_BME280_ADDRESS = 0x76;

// Create one node of each kind
BME280Node bme280Node("bme280", I2C_BME280_ADDRESS);
DHT22Node dht22Node("dht22", PIN_DHT);
RelayNode relayNode("relay", PIN_RELAY, PIN_LED);
AdcNode adcNode("adc");

// Initialize contact node without callback
ContactNode contactNode("contact", PIN_CONTACT);

// Initialize button node with callback to button press
ButtonNode buttonNode("button", PIN_BUTTON, []() {
  relayNode.toggleRelay();
});

void setup()
{
  Serial.begin(SERIAL_SPEED);
  Serial << endl
         << endl;

  // Set default configuration values before Homie.setup()
  adcNode.beforeHomieSetup();
  bme280Node.beforeHomieSetup();

  // Set callback for contact node here, just to show alternative
  contactNode.onChange([](bool open) {
    relayNode.setRelay(open);
  });

  // Initializes I2C for BME280 sensor and display
  Homie.getLogger() << "• Wire begin SDA=" << PIN_SDA << " SCL=" << PIN_SCL << endl;
  Wire.begin(PIN_SDA, PIN_SCL);

  Homie_setFirmware(FW_NAME, FW_VERSION);

  Homie.disableLedFeedback();
  Homie.disableResetTrigger();

  Homie.setup();
}

void loop()
{
  Homie.loop();
}
