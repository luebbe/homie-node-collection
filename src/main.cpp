#include <Homie.h>

void setup() {
  Serial.begin(SERIAL_SPEED);
  Serial << endl << endl;

  Homie_setFirmware("bare-minimum", "1.0.0");
  Homie.setup();
}

void loop() {
  Homie.loop();
}
