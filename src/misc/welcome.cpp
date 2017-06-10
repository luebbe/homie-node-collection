#include "welcome.hpp"

// -----------------------------------------------------------------------------
// Welcome MSG
// -----------------------------------------------------------------------------

void welcome() {

    delay(1000);
    Serial.printf("Device: %s\r\n", (char *) WiFi.hostname().c_str());
    Serial.printf("ChipID: %06X\r\n", ESP.getChipId());
    Serial.printf("Last reset reason: %s\r\n", (char *) ESP.getResetReason().c_str());
    Serial.printf("Memory size: %d bytes\r\n", ESP.getFlashChipSize());
    Serial.printf("Free heap: %d bytes\r\n", ESP.getFreeHeap());
    FSInfo fs_info;
    if (SPIFFS.info(fs_info)) {
        Serial.printf("File system total size: %d bytes\r\n", fs_info.totalBytes);
        Serial.printf("            used size : %d bytes\r\n", fs_info.usedBytes);
        Serial.printf("            block size: %d bytes\r\n", fs_info.blockSize);
        Serial.printf("            page size : %d bytes\r\n", fs_info.pageSize);
        Serial.printf("            max files : %d\r\n", fs_info.maxOpenFiles);
        Serial.printf("            max length: %d\r\n", fs_info.maxPathLength);
    }
    Serial.println();
    Serial.println();

}
