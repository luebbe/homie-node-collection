/*
 * AdcNode.cpp
 * Homie Node using the internal ESP ADC to measure voltage.
 *
 * Version: 1.1
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "AdcNode.hpp"
#include <Homie.hpp>

ADC_MODE(ADC_VCC);

// The voltage reported on a NodeMcu is relatively small. On an ESP12S the value is ok
// Measuring the voltage with a ditital multi meter yields a denominator <> 1024.0f.
// Correction factor for NodeMCU = 1.0611. Pass this value in the settings.

HomieSetting<double> adcVoltageCorrection("adc_correct", "Correction factor for AD converter.  [0.5 - 1.5] Default = 1");
HomieSetting<double> adcBattMax("batt_max", "Measured voltage that corresponds to 100% battery level.  [2.5V - 4.0V] Default = 3.3V. Must be greater than batt_min");
HomieSetting<double> adcBattMin("batt_min", "Measured voltage that corresponds to 0% battery level.  [2.5V - 4.0V] Default = 2.6V. Must be less than batt_max");

AdcNode::AdcNode(const char *name, const int sendInterval)
    : HomieNode(name, "AD Converter")
{
  _lastReadTime = millis() - READ_INTERVAL_MILLISECONDS - 1;
  _lastSendTime = millis() - sendInterval - 1;
  _sendInterval = sendInterval;
}

void AdcNode::printCaption()
{
  Homie.getLogger() << cCaption << endl;
}

void AdcNode::readVoltage()
{
  uint16_t v_raw = ESP.getVcc();
  _voltage = (((float)v_raw / 1024.0f) * adcVoltageCorrection.get());
  if (isnan(_voltage))
  {
    _batteryLevel = NAN;
  }
  else
  {
    _batteryLevel = 100 * (_voltage - adcBattMin.get()) / (adcBattMax.get() - adcBattMin.get());
  }
}

String AdcNode::getVoltageStr(void)
{
  if (isnan(_voltage))
  {
    return "err";
  }
  else
  {
    const int cMaxLen = 8;
    char charVal[cMaxLen];
    dtostrf(_voltage, cMaxLen, 2, charVal);
    return (String)charVal;
  }
}

void AdcNode::send()
{
  printCaption();

  if (isnan(_voltage))
  {
    Homie.getLogger() << cIndent << "Error reading from ADC" << endl;
  }
  else
  {
    setProperty(cVoltage).send(String(_voltage));
    setProperty(cBatteryLevel).send(String(_batteryLevel));
    Homie.getLogger() << cIndent << "Voltage: " << _voltage << "V" << endl;
    Homie.getLogger() << cIndent << "Battery level: " << _batteryLevel << "%" << endl;
  }
}

void AdcNode::loop()
{
  unsigned long now = millis();

  if (now - _lastReadTime >= READ_INTERVAL_MILLISECONDS)
  {
    _lastReadTime = now;
    readVoltage();
  }

  if (now - _lastSendTime >= _sendInterval)
  {
    _lastSendTime = now;
    send();
  }
}

void AdcNode::beforeHomieSetup()
{
  // Has to be called manually before Homie.setup()
  // Otherwise homie will go into config mode, because the defaults are missing
  adcVoltageCorrection.setDefaultValue(1.0f).setValidator([](float candidate) {
    return (candidate >= 0.5f) && (candidate <= 1.5f);
  });
  adcBattMax.setDefaultValue(cVoltMax).setValidator([](float candidate) {
    return (candidate > adcBattMin.get()) && (candidate <= 4.0f);
  });
  adcBattMin.setDefaultValue(cVoltMin).setValidator([](float candidate) {
    return (candidate >= 2.5f) && (candidate < adcBattMax.get());
  });
}

void AdcNode::onReadyToOperate()
{
  // Will be called after MQTT_CONNECT
  setProperty(cVoltageUnit).send("V");
  setProperty(cBatteryLevelUnit).send("%");
};

void AdcNode::setup()
{
  // Will be called from Homie.setup()
  advertise(cVoltage);
  advertise(cVoltageUnit);
  advertise(cBatteryLevel);
  advertise(cBatteryLevelUnit);

  printCaption();
  Homie.getLogger() << cIndent << "Send interval: " << _sendInterval / 1000 << " s" << endl;
}
