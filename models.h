#ifndef MODELS_H
#define MODELS_H

#include <Arduino.h>

struct WifiDevice {
    String ssid;
    String bssid;
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
    String bssid;
    int rssi;
    int channel;
    bool hidden;
};

struct WifiClient {
    String mac;
    String associatedBssid;
    int rssi;
    int channel;
};

struct AccessPointDetection {
    String ssid;
    String bssid;
    String detectorDeviceId;
    String scanSessionId;
    ScanMode scanMode;
    unsigned long detectedAt;
    int rssi;
    int channel;
};

struct WifiClientDetection {
    String mac;
    String detectorDeviceId;
    String scanSessionId;
    ScanMode scanMode;
    String associatedBssid;
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
