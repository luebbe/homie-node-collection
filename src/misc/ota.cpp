/*
 * OtaLogger.cpp
 * Wraps ArduinoOTA into a class and shows the OTA
 * status on serial or an OLED display.
 *
 * Version: 1.0
 * Author: Lübbe Onken (http://github.com/luebbe)
 */

#include "ota.hpp"

// -----------------------------------------------------------------------------
// OTA info via Homie logger
// -----------------------------------------------------------------------------

OtaLogger::OtaLogger() {

};

String OtaLogger::getErrorMessage(ota_error_t error) {
  if (error == OTA_AUTH_ERROR) return "Auth Failed";
  else if (error == OTA_BEGIN_ERROR) return "Begin Failed";
  else if (error == OTA_CONNECT_ERROR) return "Connect Failed";
  else if (error == OTA_RECEIVE_ERROR) return "Receive Failed";
  else if (error == OTA_END_ERROR) return "End Failed";
};

void OtaLogger::setup(uint16_t port, const char *password) {
  Homie.getLogger() << "• OTA - Setup";
  ArduinoOTA.setPort(port);
  ArduinoOTA.setHostname(WiFi.hostname().c_str());    // Hostname defaults to esp8266-[ChipID]

  if (password != "") {
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

void OtaLogger::loop() {
  ArduinoOTA.handle();
};

void OtaLogger::onStart() {
  Homie.getLogger() << "• OTA - Start" << endl ;
};

void OtaLogger::onEnd() {
  Homie.getLogger() << endl << "• OTA - End" << endl ;
};

void OtaLogger::onProgress(unsigned int progress, unsigned int total) {
  static int lastprogress = -1;
  unsigned int curprogress = (progress / (total / 100));
  if (lastprogress != curprogress) {
    lastprogress = curprogress;
    if (curprogress > 0) {
      Homie.getLogger() << ".";
      if (curprogress % 25 == 0) {
        const char * sp;
        if(curprogress < 100) {
          sp = " ";
        } else {
          sp = "";
        };
        Homie.getLogger() << " [" << sp << curprogress << "%]" << endl;
      }
    }
  }
};

void OtaLogger::onError(ota_error_t error) {
  Homie.getLogger() << "• OTA - Error " << getErrorMessage(error) << " : " << endl;
};

// -----------------------------------------------------------------------------
// OTA info via OLED Display
// -----------------------------------------------------------------------------

OtaDisplay::OtaDisplay(OLEDDisplay *display, int height)
  : OtaLogger() {
  _display = display;
  if (height > 32)
    _barTop = 28;
  else
    _barTop = height - 8;
};

void OtaDisplay::setup(uint16_t port, const char *password) {
  // Don't do anything with the display here (setup, init, ...)
  // It'll crash Homie
  Homie.getLogger() << "• OTA - Display" << endl;
  OtaLogger::setup(port, password);
}

void OtaDisplay::onEnd() {
  OtaLogger::onEnd();
  _display->clear();
  _display->setTextAlignment(TEXT_ALIGN_CENTER);
  _display->setFont(ArialMT_Plain_10);
  _display->drawString(64, _barTop - 16, "Rebooting...");
  _display->display();
};

void OtaDisplay::onProgress(unsigned int progress, unsigned int total) {
  OtaLogger::onProgress(progress, total);
  _display->clear();
  _display->setTextAlignment(TEXT_ALIGN_CENTER);
  _display->setFont(ArialMT_Plain_10);
  _display->drawString(64, _barTop - 16, "OTA Update");
  _display->drawProgressBar(2, _barTop, 124, 8, progress / (total / 100));
  _display->display();
};
