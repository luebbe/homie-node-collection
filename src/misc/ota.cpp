#include "ota.hpp"

// -----------------------------------------------------------------------------
// OTA
// -----------------------------------------------------------------------------

OtaWrapper::OtaWrapper() {

};

String OtaWrapper::getErrorMessage(ota_error_t error) {
  if (error == OTA_AUTH_ERROR) return "Auth Failed";
  else if (error == OTA_BEGIN_ERROR) return "Begin Failed";
  else if (error == OTA_CONNECT_ERROR) return "Connect Failed";
  else if (error == OTA_RECEIVE_ERROR) return "Receive Failed";
  else if (error == OTA_END_ERROR) return "End Failed";
};

void OtaWrapper::setup(uint16_t port, const char *password) {
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
  Homie.getLogger() << "[OTA] setup finished\n\n" << endl;
};

void OtaWrapper::loop() {
  ArduinoOTA.handle();
};

void OtaWrapper::onStart() {
  Homie.getLogger() << "[OTA] Start" << endl ;
};

void OtaWrapper::onEnd() {
  Homie.getLogger() << endl << "[OTA] End" << endl ;
};

void OtaWrapper::onProgress(unsigned int progress, unsigned int total) {
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
        Homie.getLogger() << " [" << sp << curprogress << "% xx]" << endl;
      }
    }
  }
};

void OtaWrapper::onError(ota_error_t error) {
  Homie.getLogger() << "[OTA] Error " << getErrorMessage(error) << " : " << endl;
};

OtaDisplayWrapper::OtaDisplayWrapper(OLEDDisplay *display)
  : OtaWrapper() {
  _display = display;
};

void OtaDisplayWrapper::setup(uint16_t port, const char *password) {
  Homie.getLogger() << "Setup Display";

  _display->init();
  _display->clear();
  _display->display();

  _display->flipScreenVertically();
  OtaWrapper::setup(port, password);
  Homie.getLogger() << " done" << endl;
}

void OtaDisplayWrapper::onProgress(unsigned int progress, unsigned int total) {
  _display->clear();
  _display->setTextAlignment(TEXT_ALIGN_CENTER);
  _display->setFont(ArialMT_Plain_10);
  _display->drawString(64, 10, "OTA Update");
  _display->drawProgressBar(2, 28, 124, 10, progress / (total / 100));
  _display->display();
  OtaWrapper::onProgress(progress, total);
};

// ----------
void otaSetup(uint16_t port, const char *password) {
  unsigned int lastprogress = -1;

  ArduinoOTA.setPort(port);
  ArduinoOTA.setHostname(WiFi.hostname().c_str());    // Hostname defaults to esp8266-[ChipID]

  if (password != "") {
    ArduinoOTA.setPassword(password);
  }

  ArduinoOTA.onStart([]() {
    Homie.getLogger() << "[OTA] Start" << endl ;
  });

  ArduinoOTA.onEnd([]() {
    Homie.getLogger() << endl << "[OTA] End" << endl ;
  });

  ArduinoOTA.onProgress([lastprogress](unsigned int progress, unsigned int total) mutable {
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
          Homie.getLogger() << " [" << sp << curprogress << "% ]" << endl;
        }
      }
    }
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Homie.getLogger() << "[OTA] Error " << error << " : " << endl;
    //DEBUG_MSG("\n[OTA] Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Homie.getLogger() << "Auth Failed" << endl;
    else if (error == OTA_BEGIN_ERROR)Homie.getLogger() << "Begin Failed" << endl;
    else if (error == OTA_CONNECT_ERROR) Homie.getLogger() << "Connect Failed" << endl;
    else if (error == OTA_RECEIVE_ERROR) Homie.getLogger() << "Receive Failed" << endl;
    else if (error == OTA_END_ERROR) Homie.getLogger() << "End Failed" << endl;
  });

  ArduinoOTA.begin();
  Homie.getLogger() << "[OTA] setup finished\n\n" << endl;
}

void otaLoop() {
    ArduinoOTA.handle();
}
