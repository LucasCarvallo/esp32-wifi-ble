#include "led_status.h"

namespace {
WifiLedState currentState = WifiLedState::Off;
unsigned long lastBlinkChange = 0;
bool ledIsOn = false;

void writeLed(bool on) {
    const bool level = WIFI_LED_ACTIVE_LOW ? !on : on;
    digitalWrite(WIFI_LED_PIN, level ? HIGH : LOW);
}

unsigned long blinkIntervalForState(WifiLedState state) {
    switch (state) {
        case WifiLedState::Connecting:
            return WIFI_LED_CONNECTING_BLINK_MS;
        case WifiLedState::Error:
            return WIFI_LED_ERROR_BLINK_MS;
        default:
            return 0;
    }
}
} // namespace

void wifiLedBegin() {
    pinMode(WIFI_LED_PIN, OUTPUT);
    writeLed(false);
}

void wifiLedSetState(WifiLedState state) {
    if (currentState == state) {
        return;
    }

    currentState = state;
    lastBlinkChange = millis();

    if (state == WifiLedState::Connected) {
        ledIsOn = true;
        writeLed(true);
        return;
    }

    if (state == WifiLedState::Off) {
        ledIsOn = false;
        writeLed(false);
        return;
    }

    ledIsOn = true;
    writeLed(true);
}

void wifiLedUpdate() {
    const unsigned long interval = blinkIntervalForState(currentState);
    if (interval == 0) {
        return;
    }

    const unsigned long now = millis();
    if (now - lastBlinkChange < interval) {
        return;
    }

    lastBlinkChange = now;
    ledIsOn = !ledIsOn;
    writeLed(ledIsOn);
}
