#include "wifi_scanner.h"

#include <WiFi.h>

void wifiScannerBegin() {
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
}

int wifiScanNetworks(WifiDevice* devices, size_t maxDevices) {
    if (devices == nullptr || maxDevices == 0) {
        return 0;
    }

    WiFi.scanDelete();
    const int totalFound = WiFi.scanNetworks(false, true);
    if (totalFound <= 0) {
        return totalFound;
    }

    const size_t count = static_cast<size_t>(totalFound < static_cast<int>(maxDevices) ? totalFound : static_cast<int>(maxDevices));
    for (size_t index = 0; index < count; ++index) {
        devices[index].ssid = WiFi.SSID(static_cast<int>(index));
        devices[index].bssid = WiFi.BSSIDstr(static_cast<int>(index));
        devices[index].rssi = WiFi.RSSI(static_cast<int>(index));
        devices[index].channel = WiFi.channel(static_cast<int>(index));
    }

    WiFi.scanDelete();
    return totalFound;
}
