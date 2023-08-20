/*
 * AdcNode.cpp
 * Homie Node using the internal ESP ADC to measure voltage.
 *
 * Version: 1.2
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "AdcNode.hpp"
#include <Homie.hpp>

// The voltage reported on a NodeMcu is relatively small. On an ESP12S the value is ok
// Measuring the voltage with a ditital multi meter yields a denominator <> 1024.0f.
// Correction factor for NodeMCU = 1.0611. Pass this value in the settings.

AdcNode::AdcNode(const char *id, const char *name, const int readInterval, const int sendInterval)
    : SensorNode(id, name, "ADC", readInterval, sendInterval)
{
  _adcCorrection = new HomieSetting<double>("adcCorrect", "Correction factor for AD converter.  [0.5 .. 1.5] Default = 1");
  _adcBattMin = new HomieSetting<double>("battMin", "Measured voltage that corresponds to 0% battery level.  [2.5V .. 4.0V] Default = 2.6V. Must be less than battMax");
  _adcBattMax = new HomieSetting<double>("battMax", "Measured voltage that corresponds to 100% battery level.  [2.5V .. 4.0V] Default = 3.3V. Must be greater than battMin");

  asprintf(&_caption, cCaption, name);

  advertise(cStatusTopic)
      .setDatatype("enum")
      .setFormat("error, ok");
  advertise(cVoltageTopic)
      .setDatatype("float")
      .setFormat("2.5:3.5")
      .setUnit(cUnitVolt);
  advertise(cBatteryLevelTopic)
      .setDatatype("float")
      .setFormat("0:100")
      .setUnit(cUnitPercent);
}

void AdcNode::readVoltage()
{
  uint16_t v_raw = ESP.getVcc();
  _voltage = (((float)v_raw / 1024.0f) * _adcCorrection->get());
  if (isnan(_voltage))
  {
    _batteryLevel = NAN;
  }
  else
  {
    _batteryLevel = 100 * (_voltage - _adcBattMin->get()) / (_adcBattMax->get() - _adcBattMin->get());
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
    Homie.getLogger() << cIndent << F("Error reading from ADC") << endl;
    sendError();
  }
  else
  {
    Homie.getLogger() << cIndent << F("Voltage: ") << _voltage << "V" << endl;
    Homie.getLogger() << cIndent << F("Battery level: ") << _batteryLevel << "%" << endl;
    sendData();
  }
}

void AdcNode::sendError()
{
  if (Homie.isConnected())
  {
    setProperty(cStatusTopic).send("error");
  }
}

void AdcNode::sendData()
{
  if (Homie.isConnected())
  {
    setProperty(cStatusTopic).send("ok");
    setProperty(cVoltageTopic).send(String(_voltage));
    setProperty(cBatteryLevelTopic).send(String(_batteryLevel));
  }
}

void AdcNode::onReadyToOperate()
{
  send();
};

void AdcNode::takeMeasurement()
{
  readVoltage();
}

void AdcNode::beforeHomieSetup()
{
  // Has to be called manually before Homie.setup()
  // Otherwise homie will go into config mode, because the defaults are missing
  _adcCorrection->setDefaultValue(1.0f).setValidator([](float candidate) {
    return (candidate >= 0.5f) && (candidate <= 1.5f);
  });
  _adcBattMax->setDefaultValue(cVoltMax).setValidator([](float candidate) {
    return (candidate > 2.5f) && (candidate <= 4.0f);
  });
  _adcBattMin->setDefaultValue(cVoltMin).setValidator([](float candidate) {
    return (candidate >= 2.5f) && (candidate < 4.0f);
  });
}

void AdcNode::setup()
{
  SensorNode::setup();

  Homie.getLogger() << cIndent << F("Reading interval: ") << readInterval() / 1000UL << F("s") << endl;
  _sensorFound = true;
}
