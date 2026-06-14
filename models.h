#ifndef MODELS_H
#define MODELS_H

#include <Arduino.h>

constexpr size_t WIFI_MAC_STR_LEN = 18;

struct WifiDevice {
    String ssid;
    char bssid[WIFI_MAC_STR_LEN];
    int rssi;
    int channel;
};

// struct BleDevice {
//     String mac;
//     String name;
//     int rssi;
// };

enum class ScanMode {
    Managed,
    Monitor
};

struct AccessPoint {
    String ssid;
    char bssid[WIFI_MAC_STR_LEN];
    int rssi;
    int channel;
    bool hidden;
};

struct WifiClient {
    char mac[WIFI_MAC_STR_LEN];
    char associatedBssid[WIFI_MAC_STR_LEN];
    int rssi;
    int channel;
};

struct AccessPointDetection {
    String ssid;
    char bssid[WIFI_MAC_STR_LEN];
    String detectorDeviceId;
    String scanSessionId;
    ScanMode scanMode;
    unsigned long detectedAt;
    int rssi;
    int channel;
};

struct WifiClientDetection {
    char mac[WIFI_MAC_STR_LEN];
    String detectorDeviceId;
    String scanSessionId;
    ScanMode scanMode;
    char associatedBssid[WIFI_MAC_STR_LEN];
    unsigned long detectedAt;
    int rssi;
    int channel;
};

struct ScanBatchInfo { // Información general de un lote de escaneo
    String deviceId;
    ScanMode mode;
    String detectionType;
    unsigned long startedAt;
    unsigned long endedAt;
    int totalFound;
};

using ManagedWifiDevice = WifiDevice;
using ManagedAccessPoint = AccessPoint;

#endif
