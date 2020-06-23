#define FW_NAME "demo-ping-node"
#define FW_VERSION "1.0.0"

#include <Homie.h>

#include "DHT22Node.hpp"
#include "PingNode.hpp"

// Insert your pin number(s) here
const int trigPin = D1;
const int echoPin = D2;
const int relayPin = D5;
const int dhtPin = D7;
;
const int ledPin = LED_BUILTIN;

unsigned long TEMPERATURE_INTERVAL = 120; // seconds
unsigned long lastTemperatureUpdate = 0;

// Create one node of each kind
PingNode obstacleNode("obstacle", "RCW-0001", trigPin, echoPin);
DHT22Node airNode("air", dhtPin, TEMPERATURE_INTERVAL);

void loopHandler()
{
  if (millis() - lastTemperatureUpdate > TEMPERATURE_INTERVAL * 1000UL || lastTemperatureUpdate == 0)
  {
    obstacleNode.setTemperature(airNode.getTemperature());
    lastTemperatureUpdate = millis();
  }
}

void changeHandler()
{
  Serial << "Obstacle distance changed to: " << obstacleNode.getDistance() << " m" << endl;
}

void setup()
{
  Homie_setFirmware(FW_NAME, FW_VERSION);

  Serial.begin(SERIAL_SPEED);
  Serial << endl
         << endl;

  Homie.setLoopFunction(loopHandler);
  obstacleNode.setChangeHandler(changeHandler);
  Homie.setup();
}

void loop()
{
  Homie.loop();
}
