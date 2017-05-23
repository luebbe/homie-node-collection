/*
 * BME280Node.cpp
 * Homie Node for BME280 sensors using Adafruit BME280 library.
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 * Author: Markus Haack (http://github.com/mhaack)
 */

#include "BME280Node.hpp"
#include <Homie.hpp>

BME280Node::BME280Node(const char *name, const int measurementInterval)
    : HomieNode(name, "BME280Sensor"), _lastMeasurement(0) {
  _measurementInterval = measurementInterval;
}

void BME280Node::loop() {
  if (millis() - _lastMeasurement >= _measurementInterval * 1000UL ||
      _lastMeasurement == 0) {
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    pressure = bme.readPressure() / 100.0F;

    Homie.getLogger() << "Temperature: " << temperature << " °C" << endl;
    Homie.getLogger() << "Humidity: " << temperature << " %" << endl;
    Homie.getLogger() << "Pressure: " << pressure << " hPa" << endl;

    setProperty("temperature").send(String(temperature));
    setProperty("humidity").send(String(humidity));
    setProperty("pressure").send(String(pressure));

    _lastMeasurement = millis();
  }
}

void BME280Node::setup() {
  advertise("temperature");
  advertise("humidity");
  advertise("pressure");

  if (!bme.begin()) {
    Homie.getLogger() << "Could not find a valid BME280 sensor, check wiring!"
                      << endl;
    while (1)
      ;
  }
  Homie.getLogger() << "BME280 sensor reading interval = "
                    << _measurementInterval << endl;
}
