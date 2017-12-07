#define FW_NAME "demo-all-nodes"
#define FW_VERSION "1.0.0"

#include <Homie.h>
#include "ota.hpp"
#include "welcome.hpp"

#include "AdcNode.hpp"
#include "BME280Node.hpp"
#include "ButtonNode.hpp"
#include "ContactNode.hpp"
#include "DHT22Node.hpp"
#include "RelayNode.hpp"

// Insert your pin number(s) here
const int PIN_LED = 2;
const int PIN_DHT = 0;
const int PIN_SDA = 5; // Serial data
const int PIN_SCL = 4; // Serial clock
const int PIN_CONTACT = 12;
const int PIN_RELAY = 13;
const int PIN_BUTTON = 14;

// Setup OTA logging via OLED dislpay and Homie logger
#include <SSD1306Wire.h>
const int I2C_DISPLAY_ADDRESS = 0x3c;

SSD1306Wire display(I2C_DISPLAY_ADDRESS, PIN_SDA, PIN_SCL);
OtaDisplaySSD1306 ota(display, NULL);
WelcomeSSD1306 welcome(display, FW_NAME, FW_VERSION);

// Create one node of each kind
BME280Node bme280Node("bme280");
DHT22Node dht22Node("dht22", PIN_DHT);
RelayNode relayNode("relay", PIN_RELAY, PIN_LED);
AdcNode adcNode("adc");

// Initialize contact node without callback
ContactNode contactNode("contact", PIN_CONTACT);

// Initialize button node with callback to button press
ButtonNode buttonNode("button", PIN_BUTTON, []() {
  relayNode.toggleRelay();
});

void setupHandler()
{
  // Advertise units
  bme280Node.setupHandler();
  dht22Node.setupHandler();
  adcNode.setupHandler();
};

void loopHandler()
{
  // We want to OTA handler to run only in the loop when WiFi is connected
  ota.loop();
}

void setup()
{
  Serial.begin(SERIAL_SPEED);
  Serial << endl
         << endl;

  // Initializes I2C for BME280 sensor
  Wire.begin(PIN_SDA, PIN_SCL);

  welcome.show();
  ota.setup();

  // Set callback for contact node here, just to show alternative
  contactNode.onChange([](bool open) {
    relayNode.setRelay(open);
  });

  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie.setSetupFunction(setupHandler);
  Homie.setLoopFunction(loopHandler);

  Homie.setup();
}

void loop()
{
  Homie.loop();
}
