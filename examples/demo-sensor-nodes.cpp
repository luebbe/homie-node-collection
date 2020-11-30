#define FW_NAME "demo-sensor-nodes"
#define FW_VERSION "1.0.0"

#include <Homie.h>

#include "AdcNode.hpp"
#include "BME280Node.hpp"
#include "DHT22Node.hpp"
#include "DS18B20Node.hpp"

// Insert your pin number(s) here
const int PIN_LED = 2;   // =D4 on Wemos
const int PIN_DHT = 0;   // =D3 on Wemos
const int PIN_SDA = 4;   // =D2 on Wemos
const int PIN_SCL = 5;   // =D1 on Wemos
const int PIN_DS18 = 16; // =D0 on Wemos

const int I2C_BME280_ADDRESS = 0x77; // Default I2C address for BME280. can be changed to 0x76 by changing a solder bridge

ADC_MODE(ADC_VCC); // Set ADC to measure internal VCC

// Create one node of each kind
BME280Node bme280Node("bme280", "Outdoor", I2C_BME280_ADDRESS);
DHT22Node dht22Node("dht22", "Indoor", PIN_DHT);
DS18B20Node ds18b20Node("ds18b20", "Fishtank", PIN_DS18);
AdcNode adcNode("adc", "Internal");

void setup()
{
  Homie_setFirmware(FW_NAME, FW_VERSION);

  Serial.begin(SERIAL_SPEED);
  Serial << endl
         << endl;

  // Set default configuration values before Homie.setup()
  adcNode.beforeHomieSetup();
  bme280Node.beforeHomieSetup();

  // Initializes I2C for BME280 sensor and display
  Homie.getLogger() << F("• Wire begin SDA=") << PIN_SDA << F(" SCL=") << PIN_SCL << endl;
  Wire.begin(PIN_SDA, PIN_SCL);

  Homie.disableLedFeedback();
  Homie.disableResetTrigger();

  Homie.setup();
}

void loop()
{
  Homie.loop();
}
