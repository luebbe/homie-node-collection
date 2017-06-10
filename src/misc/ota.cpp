#include "ota.hpp"

// -----------------------------------------------------------------------------
// OTA
// -----------------------------------------------------------------------------

void otaSetup(uint16_t port, const char *password) {

    ArduinoOTA.setPort(port);
    ArduinoOTA.setHostname(WiFi.hostname().c_str());    // Hostname defaults to esp8266-[ChipID]
    
    if (password != "") {
      ArduinoOTA.setPassword(password);
    }

    ArduinoOTA.onStart([]() {
      Homie.getLogger() << "[OTA] Start" << endl ;
    });

    ArduinoOTA.onEnd([]() {
      Homie.getLogger() << "[OTA] End" << endl ;
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Homie.getLogger() << "[OTA] Progress:" << (progress / (total / 100)) << "%" << endl;
        //DEBUG_MSG("[OTA] Progress: %u%%\r", (progress / (total / 100)));
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
