#define FW_NAME "demo-all-nodes"
#define FW_VERSION "1.0.0"

#include <Homie.h>
#include "DHT22Node.hpp"
#include "BME280Node.hpp"
#include "ContactNode.hpp"
#include "RelayNode.hpp"

// Insert your pin number(s) here
#define PIN_CONTACT 5
#define PIN_RELAY   4
#define PIN_LED     2
#define PIN_DHT     0
#define PIN_SDA    12
#define PIN_SCL    13

// Create one node of each kind
BME280Node  bme280Node("BME-280");
DHT22Node   dht22Node("DHT-22", PIN_DHT);
ContactNode contactNode("Contact", PIN_CONTACT);
RelayNode   relayNode("Relay", PIN_RELAY, PIN_LED);

void setup() {
  Serial.begin(SERIAL_SPEED);
  Serial << endl << endl;

  Wire.begin(PIN_SDA, PIN_SCL);

  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie.setup();
}

void loop() {
  Homie.loop();
}
