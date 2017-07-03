/*
 * OtaWrapper.hpp
 * Wraps ArduinoOTA into a class and displays the OTA
 * status on serial or an OLED display.
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#ifndef SRC_OTA_H
#define SRC_OTA_H


#include <Homie.h>       // used for logger output
#include <OLEDDisplay.h> // used for display output
#include <ArduinoOTA.h>

class OtaWrapper {
private:
protected:
  String getErrorMessage(ota_error_t error);

public:
  OtaWrapper();

  virtual void setup(uint16_t port = 8266, const char *password = "");
  virtual void loop();

  virtual void onStart();
  virtual void onEnd();
  virtual void onProgress(unsigned int progress, unsigned int total);
  virtual void onError(ota_error_t error);
};

class OtaDisplayWrapper : public OtaWrapper {
private:
  OLEDDisplay *_display;
protected:
public:
  OtaDisplayWrapper(OLEDDisplay *display);

  void setup(uint16_t port = 8266, const char *password = "") override;

  void onProgress(unsigned int progress, unsigned int total) override;
  // void setup(uint16_t port, const char *password) override;
  // void loop() override;
};


void otaSetup(uint16_t port = 8266, const char *password = "");

void otaLoop();


#endif /* end of include guard: SRC_OTA_H */
