/*
 * OtaLogger.cpp
 * Wraps ArduinoOTA into a class and shows the OTA
 * status on serial or an OLED display.
 *
 * Version: 1.2
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "ota.hpp"

// -----------------------------------------------------------------------------
// OTA info via Homie logger
// -----------------------------------------------------------------------------

OtaLogger::OtaLogger(TOtaCallback otaCallback)
    : _otaCallback(otaCallback){};

String OtaLogger::getErrorMessage(ota_error_t error)
{
  if (error == OTA_AUTH_ERROR)
    return "Auth Failed";
  else if (error == OTA_BEGIN_ERROR)
    return "Begin Failed";
  else if (error == OTA_CONNECT_ERROR)
    return "Connect Failed";
  else if (error == OTA_RECEIVE_ERROR)
    return "Receive Failed";
  else if (error == OTA_END_ERROR)
    return "End Failed";
};

void OtaLogger::setup(uint16_t port, const char *password)
{
  Homie.getLogger() << "• OTA - Setup";
  ArduinoOTA.setPort(port);
  ArduinoOTA.setHostname(WiFi.hostname().c_str()); // Hostname defaults to esp8266-[ChipID]

  if (password != "")
  {
    ArduinoOTA.setPassword(password);
  }

  ArduinoOTA.onStart([this]() {
    onStart();
  });

  ArduinoOTA.onEnd([this]() {
    onEnd();
  });

  ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
    onProgress(progress, total);
  });

  ArduinoOTA.onError([this](ota_error_t error) {
    onError(error);
  });

  ArduinoOTA.begin();
  Homie.getLogger() << " done" << endl;
};

void OtaLogger::loop()
{
  ArduinoOTA.handle();
};

void OtaLogger::onStart()
{
  Homie.getLogger() << "• OTA - Start" << endl;
  if (_otaCallback)
  {
    _otaCallback();
  }
};

void OtaLogger::onEnd()
{
  Homie.getLogger() << endl
                    << "• OTA - End" << endl;
};

void OtaLogger::onError(ota_error_t error)
{
  Homie.getLogger() << "• OTA - Error " << getErrorMessage(error) << " : " << endl;
};

void OtaLogger::onProgress(unsigned int progress, unsigned int total)
{
  static int lastprogress = -1;
  unsigned int curprogress = (progress / (total / 100));
  if (lastprogress != curprogress)
  {
    lastprogress = curprogress;
    if (curprogress > 0)
    {
      Homie.getLogger() << ".";
      if (curprogress % 25 == 0)
      {
        const char *sp;
        if (curprogress < 100)
        {
          sp = " ";
        }
        else
        {
          sp = "";
        };
        Homie.getLogger() << " [" << sp << curprogress << "%]" << endl;
      }
    }
  }
};

// -----------------------------------------------------------------------------
// Base class for the two loggers using a OLED display
// -----------------------------------------------------------------------------

OtaDisplay::OtaDisplay(TOtaCallback otaCallback)
    : OtaLogger(otaCallback){};

void OtaDisplay::drawMessage(const char *message)
{
}

void OtaDisplay::onStart()
{
  OtaLogger::onStart();
  drawMessage("OTA Update...");
}

void OtaDisplay::onEnd()
{
  OtaLogger::onEnd();
  drawMessage("Rebooting...");
}

void OtaDisplay::onError(ota_error_t error)
{
  OtaLogger::onError(error);
  drawMessage(OtaLogger::getErrorMessage(error).c_str());
}

// #ifdef USE_SSD1306
// -----------------------------------------------------------------------------
// OTA info via SSD1306 128x64 OLED Display using the esp8266-oled-ssd1306 library
// -----------------------------------------------------------------------------

OtaDisplaySSD1306::OtaDisplaySSD1306(OLEDDisplay &display, TOtaCallback otaCallback)
    : OtaDisplay(otaCallback), _display(display){};

void OtaDisplaySSD1306::setup(uint16_t port, const char *password)
{
  // Don't do anything with the display here (setup, init, ...)
  // It'll crash Homie
  Homie.getLogger() << "• OTA - OtaDisplaySSD1306" << endl;
  OtaLogger::setup(port, password);
}

void OtaDisplaySSD1306::drawMessage(const char *message)
{
  _display.clear();
  _display.setTextAlignment(TEXT_ALIGN_CENTER);
  _display.setFont(ArialMT_Plain_10);
  _display.drawString(64, 10, message);
  _display.display();
}

void OtaDisplaySSD1306::onProgress(unsigned int progress, unsigned int total)
{
  OtaLogger::onProgress(progress, total);
  _display.clear();
  _display.setTextAlignment(TEXT_ALIGN_CENTER);
  _display.setFont(ArialMT_Plain_10);
  _display.drawString(64, 10, "OTA Update");
  _display.drawProgressBar(2, 28, 124, 8, progress / (total / 100));
  _display.display();
};
// #endif

// #ifdef USE_U8G2
// -----------------------------------------------------------------------------
// OTA info via OLED Display using the u8g2 library
// -----------------------------------------------------------------------------

OtaDisplayU8G2::OtaDisplayU8G2(U8G2 &display, TOtaCallback otaCallback)
    : OtaDisplay(otaCallback), _display(display)
{
  _height = _display.getHeight();
  _width = _display.getWidth();
  if (_height > 32)
  {
    _baseLine = _height / 2;
  }
  else
  {
    _baseLine = 32;
  }
  _baseLine -= 8; // subtract space for bar
  _progress = 0;
};

void OtaDisplayU8G2::setup(uint16_t port, const char *password)
{
  // Don't do anything with the display here (setup, init, ...)
  // It'll crash Homie
  Homie.getLogger() << "• OTA - OtaDisplayU8G2" << endl;
  OtaLogger::setup(port, password);
}

void OtaDisplayU8G2::drawMessage(const char *message)
{
  _display.clearBuffer();
  _display.setDrawColor(1);
  _display.setFontPosBottom();
  _display.setFont(cMediumFont);

  uint8_t _strWidth = _display.getStrWidth(message);
  _display.drawUTF8((_width - _strWidth) / 2, _baseLine - 2, message);

  _display.sendBuffer();
}

void OtaDisplayU8G2::onProgress(unsigned int progress, unsigned int total)
{
  OtaLogger::onProgress(progress, total);

  uint8_t curProgress = (progress * _width) / total;

  if (curProgress > _progress)
  {
    _progress = curProgress;
    _display.drawBox(0, _height - 8, _progress, 6);
    _display.sendBuffer();
  }
};
// #endif