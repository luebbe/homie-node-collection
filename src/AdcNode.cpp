/*
 * AdcNode.cpp
 * Homie Node using the internal ESP ADC to measure voltage.
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "AdcNode.hpp"

ADC_MODE(ADC_VCC);

AdcNode::AdcNode(const char *name,  const int measurementInterval)
    : HomieNode(name, "AD Converter"), _lastMeasurement(0) {
  _measurementInterval = measurementInterval;
}

void AdcNode::printCaption() {
  Homie.getLogger() << cCaption  << endl;
}

void AdcNode::readVoltage()
{
  uint16_t v_raw = ESP.getVcc();
  _voltage = ((float)v_raw/1024.0f);
}

void AdcNode::loop() {
  if ((millis() - _lastMeasurement >= _measurementInterval * 1000UL) ||
      (_lastMeasurement == 0)) {

    readVoltage();
    
    printCaption();

    if (isnan(_voltage)) {
      Homie.getLogger() << cIndent << "Error reading from Adc" << endl;
    }
    else {
      Homie.getLogger() << cIndent << "Voltage: " << _voltage << " V" << endl;

      setProperty(cVoltage).send(String(_voltage));
    }
    _lastMeasurement = millis();
  }
}

void AdcNode::onReadyToOperate() {
  setProperty(cVoltageUnit).send("V");
};

void AdcNode::setup() {
  advertise(cVoltage);
  advertise(cVoltageUnit);

  readVoltage();

  printCaption();
  Homie.getLogger() << cIndent << "Reading interval: " << _measurementInterval << " s" << endl;
}
