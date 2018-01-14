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
const int PIN_LED = 2;      // =D4 on Wemos
const int PIN_DHT = 0;      // =D3 on Wemos
const int PIN_SDA = 4;      // =D2 on Wemos
const int PIN_SCL = 5;      // =D1 on Wemos
const int PIN_CONTACT = 12; // =D6 on Wemos
const int PIN_RELAY = 13;   // =D7 on Wemos
const int PIN_BUTTON = 14;  // =D5 on Wemos

const int I2C_BME280_ADDRESS = 0x76;

#if defined(DISPLAY_SSD1306)
// Setup OTA logging via OLED display and Homie logger
#include <SSD1306Wire.h>
const int I2C_DISPLAY_ADDRESS = 0x3c;

SSD1306Wire display(I2C_DISPLAY_ADDRESS, PIN_SDA, PIN_SCL);
OtaDisplaySSD1306 ota(display, NULL);
WelcomeSSD1306 welcome(display, FW_NAME, FW_VERSION);
#else
// Setup everything via Homie logger
OtaLogger ota;
Welcome welcome(FW_NAME, FW_VERSION);
#endif

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

  welcome.show();
  ota.setup();

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

  Homie.setSetupFunction(setupHandler);
  Homie.setLoopFunction(loopHandler);

  Homie.setup();
}

void loop()
{
  Homie.loop();
}
