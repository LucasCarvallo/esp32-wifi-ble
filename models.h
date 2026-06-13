#ifndef MODELS_H
#define MODELS_H

#include <Arduino.h>

struct WifiDevice {
    String ssid;
    String bssid;
    int rssi;
    int channel;
};

struct BleDevice {
    String mac;
    String name;
    int rssi;
};

#endif
