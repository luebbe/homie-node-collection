/*
 * BME280Node.cpp
 * Homie Node for BME280 sensors using Adafruit BME280 library.
 *
 * Version: 1.1
 * Author: Lübbe Onken (http://github.com/luebbe)
 * Author: Markus Haack (http://github.com/mhaack)
 */

#include "BME280Node.hpp"

BME280Node::BME280Node(const char *name, const int measurementInterval)
    : HomieNode(name, "BME280Sensor"), _lastMeasurement(0)
{
  _measurementInterval = measurementInterval;
}

void BME280Node::printCaption()
{
  Homie.getLogger() << cCaption << endl;
}

void BME280Node::loop()
{
  if (millis() - _lastMeasurement >= _measurementInterval * 1000UL ||
      _lastMeasurement == 0)
  {
    if (_sensorFound)
    {
      temperature = bme.readTemperature();
      humidity = bme.readHumidity();
      pressure = bme.readPressure() / 100.0F;

      printCaption();

      Homie.getLogger() << cIndent << "Temperature: " << temperature << " °C" << endl;
      Homie.getLogger() << cIndent << "Humidity: " << humidity << " %" << endl;
      Homie.getLogger() << cIndent << "Pressure: " << pressure << " hPa" << endl;

      setProperty(cStatus).send("ok");
      setProperty("temperature").send(String(temperature));
      setProperty("humidity").send(String(humidity));
      setProperty("pressure").send(String(pressure));
    }
    else
    {
      setProperty(cStatus).send("error");
    }
    _lastMeasurement = millis();
  }
}

void BME280Node::setupHandler()
{
  setProperty(cTemperatureUnit).send("°C");
  setProperty(cHumidityUnit).send("%");
  setProperty(cPressureUnit).send("hPa");
};

void BME280Node::setup()
{
  advertise(cStatus);
  advertise(cTemperature);
  advertise(cTemperatureUnit);
  advertise(cHumidity);
  advertise(cHumidityUnit);
  advertise(cPressure);
  advertise(cPressureUnit);

  if (bme.begin())
  {
    _sensorFound = true;
    Homie.getLogger() << cCaption << " found" << endl
                      << cIndent << "Reading interval: " << _measurementInterval << " s" << endl;
  }
  else
  {
    _sensorFound = false;
    Homie.getLogger() << cCaption << " not found. Check wiring!" << endl;
  }
}
