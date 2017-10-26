/*
 * OtaLogger.hpp
 * Wraps ArduinoOTA into a class and shows the OTA
 * status on serial or an OLED display.
 *
 * Version: 1.1
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#ifndef SRC_OTA_H
#define SRC_OTA_H

#include <Homie.h> // for Homie logger output
#include <ArduinoOTA.h>
// #ifdef USE_SSD1306
#include <OLEDDisplay.h> // for display output via https://github.com/squix78/esp8266-oled-ssd1306 library
// #endif
// #ifdef USE_U8G2
#include <u8g2lib.h> // for display output via https://github.com/olikraus/u8g2 library
#define cMediumFont u8g2_font_helvR14_tf
// #endif

typedef std::function<void(void)> TOtaCallback;

// -----------------------------------------------------------------------------
// OTA info via Homie logger
// -----------------------------------------------------------------------------

class OtaLogger
{
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
// OTA info via SSD1306 128x64 OLED Display using the esp8266-oled-ssd1306 library
// -----------------------------------------------------------------------------
// #ifdef USE_SSD1306
class OtaDisplaySSD1306 : public OtaLogger
{
private:
  OLEDDisplay *_display;

protected:
  void onEnd();
  void onProgress(unsigned int progress, unsigned int total) override;

public:
  OtaDisplaySSD1306(OLEDDisplay *display);

  void setup(uint16_t port = 8266, const char *password = "") override;
};
// #endif

// -----------------------------------------------------------------------------
// OTA info via OLED Display using the u8g2 library
// -----------------------------------------------------------------------------
// #ifdef USE_U8G2
class OtaDisplayU8G2 : public OtaLogger
{
private:
  U8G2 &_display;
  TOtaCallback _otaCallback;
  uint8_t _height;
  uint8_t _width;
  uint8_t _baseLine;
  uint8_t _progress;

  void drawMessage(const char *message);

protected:
  void onStart();
  void onEnd();
  void onProgress(unsigned int progress, unsigned int total) override;
  void onError(ota_error_t error) override;
  
public:
  OtaDisplayU8G2(U8G2 &display, TOtaCallback otaCallback = NULL);

  void setup(uint16_t port = 8266, const char *password = "") override;
};
// #endif

#endif /* end of include guard: SRC_OTA_H */
