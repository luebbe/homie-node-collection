/*
 * OtaLogger.hpp
 * Wraps ArduinoOTA into a class and shows the OTA
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

// -----------------------------------------------------------------------------
// OTA info via Homie logger
// -----------------------------------------------------------------------------

class OtaLogger {
private:
protected:
  String getErrorMessage(ota_error_t error);

  virtual void onStart();
  virtual void onEnd();
  virtual void onProgress(unsigned int progress, unsigned int total);
  virtual void onError(ota_error_t error);
public:
  OtaLogger();

  virtual void setup(uint16_t port = 8266, const char *password = "");
  virtual void loop();
};

// -----------------------------------------------------------------------------
// OTA info via OLED Display
// -----------------------------------------------------------------------------

class OtaDisplay : public OtaLogger {
private:
  OLEDDisplay *_display;
protected:
  void onEnd();
  void onProgress(unsigned int progress, unsigned int total) override;
public:
  OtaDisplay(OLEDDisplay *display);

  void setup(uint16_t port = 8266, const char *password = "") override;
};

#endif /* end of include guard: SRC_OTA_H */
