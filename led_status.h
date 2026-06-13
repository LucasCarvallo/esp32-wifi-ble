#ifndef LED_STATUS_H
#define LED_STATUS_H

#include <Arduino.h>

#include "config.h"

enum class WifiLedState {
    Off,
    Connecting,
    Connected,
    Error
};

void wifiLedBegin();
void wifiLedSetState(WifiLedState state);
void wifiLedUpdate();

#endif
