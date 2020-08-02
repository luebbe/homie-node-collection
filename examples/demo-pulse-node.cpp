#define FW_NAME "demo-pulse-node"
#define FW_VERSION "1.0.0"

#include <Homie.hpp>
#include "PulseNode.hpp"

#define PIN_OPTOCOUPLER D7  // GPIO 13

PulseNode pulseNode("pulse", "Door bell", PIN_OPTOCOUPLER);

void ICACHE_RAM_ATTR onOptoCouplerPulse()
{
  pulseNode.onInterrupt();
}

void onHomieEvent(const HomieEvent &event)
{
  // The device rebooted when attachInterrupt was called in setup()
  // before Wifi was connected and interrupts were already coming in.
  switch (event.type)
  {
  case HomieEventType::WIFI_CONNECTED:
    attachInterrupt(PIN_OPTOCOUPLER, onOptoCouplerPulse, FALLING);
    break;
  case HomieEventType::WIFI_DISCONNECTED:
    detachInterrupt(PIN_OPTOCOUPLER);
    break;
  default:
    break;
  }
}

void setup()
{
  Homie_setFirmware(FW_NAME, FW_VERSION);

  Serial.begin(SERIAL_SPEED);
  Serial << endl
         << endl;

  Homie.disableResetTrigger();
  Homie.disableLedFeedback();

  Homie.onEvent(onHomieEvent);
  Homie.setup();
}

void loop()
{
  Homie.loop();
}

