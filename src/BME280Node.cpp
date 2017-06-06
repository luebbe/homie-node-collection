/*
 * BME280Node.cpp
 * Homie Node for BME280 sensors using Adafruit BME280 library.
 *
 * Version: 1.1
 * Author: Lübbe Onken (http://github.com/luebbe)
 * Author: Markus Haack (http://github.com/mhaack)
 */

#include "BME280Node.hpp"
#include <Homie.hpp>

BME280Node::BME280Node(const char *name, const int measurementInterval)
    : HomieNode(name, "BME280Sensor"), _lastMeasurement(0) {
  _measurementInterval = measurementInterval;
}

char *BME280Node::printCaption() {
  Homie.getLogger() << cCaption.c_str() <<  endl;
}

void BME280Node::loop() {
  if (_sensorFound) {
    if (millis() - _lastMeasurement >= _measurementInterval * 1000UL ||
        _lastMeasurement == 0) {
      temperature = bme.readTemperature();
      humidity = bme.readHumidity();
      pressure = bme.readPressure() / 100.0F;

      printCaption();

      Homie.getLogger() << cIndent.c_str() << "Temperature: " << temperature << " °C" << endl;
      Homie.getLogger() << cIndent.c_str() << "Humidity: " << humidity << " %" << endl;
      Homie.getLogger() << cIndent.c_str() << "Pressure: " << pressure << " hPa" << endl;

      setProperty("temperature").send(String(temperature));
      setProperty("humidity").send(String(humidity));
      setProperty("pressure").send(String(pressure));

      _lastMeasurement = millis();
    }
  }
}

void BME280Node::setupHandler() {
  setProperty(cTemperatureUnit.c_str()).send("°C");
  setProperty(cHumidityUnit.c_str()).send("%");
  setProperty(cPressureUnit.c_str()).send("hPa");
};

void BME280Node::setup() {
  advertise(cTemperature.c_str());
  advertise(cTemperatureUnit.c_str());
  advertise(cHumidity.c_str());
  advertise(cHumidityUnit.c_str());
  advertise(cPressure.c_str());
  advertise(cPressureUnit.c_str());
  if (bme.begin()) {

    _sensorFound = true;
    Homie.getLogger() << cCaption.c_str() << " found" << endl
                      << cIndent.c_str() << "Reading interval: " << _measurementInterval << " s" << endl;
  }
  else {
    _sensorFound = false;
    Homie.getLogger() << cCaption.c_str() << " not found. Check wiring!" << endl;
  }
}
