/*
 * DHT22Node.cpp
 * Homie Node for DHT22 sensors using Adafruit DHT22 library.
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "DHT22Node.hpp"
#include <Homie.hpp>

#define DHTTYPE DHT22

DHT22Node::DHT22Node(const char *name, const int sensorPin, const int measurementInterval)
    : HomieNode(name, "DHT22Sensor"), _lastMeasurement(0) {
  _sensorPin = sensorPin;
  _measurementInterval = measurementInterval;
}

void DHT22Node::loop() {
  if (_sensorPin > DEFAULTPIN) {
    if ((millis() - _lastMeasurement >= _measurementInterval * 1000UL) ||
        (_lastMeasurement == 0)) {
      temperature = dht->readTemperature();
      humidity = dht->readHumidity();

      if (isnan(temperature) || isnan(humidity)) {
        Homie.getLogger() << "Error reading from DHT22 Sensor at pin " << _sensorPin << endl;
      }
      else {
        Homie.getLogger() << "Temperature: " << temperature << " °C" << endl;
        Homie.getLogger() << "Humidity: " << humidity << " %" << endl;

        setProperty("temperature").send(String(temperature));
        setProperty("humidity").send(String(humidity));
      }
      _lastMeasurement = millis();
    }
  }
}

void DHT22Node::setup() {
  advertise("temperature");
  advertise("humidity");

  Homie.getLogger() << "DHT sensor pin: " << _sensorPin << endl;

  if (_sensorPin > DEFAULTPIN) {
    dht = new DHT(_sensorPin, DHTTYPE);
    dht->begin();
  }
}
