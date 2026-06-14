/*
Sketch uses 935684 bytes (71%) of program storage space. Maximum is 1310720 bytes.
Global variables use 46832 bytes (14%) of dynamic memory, leaving 280848 bytes for local variables. Maximum is 327680 bytes.
100.0% 595906/595906 bytes... 
Wrote 935840 bytes (595906 compressed) at 0x00010000 in 9.7 seconds (774.7 kbit/s).

// php artisan serve --host=0.0.0.0 --port=8000
*/

#include "config.h"
#include "api_client.h"
#include "display.h"
#include "led_status.h"
#include "wifi_scanner.h"
#include <WiFi.h>

namespace {
ManagedAccessPoint wifiDevices[WIFI_SCAN_MAX_RESULTS];
unsigned long lastWifiScan = 0;

String wifiDisplayName(const ManagedAccessPoint& device) {
    if (device.ssid.length() > 0) {
        return device.ssid;
    }

    return String("<oculta>");
}

void runWifiScanAndShow() {
    displayShowLines("Escaneando WiFi...", "");

    const int totalFound = wifiScanNetworks(wifiDevices, WIFI_SCAN_MAX_RESULTS);
    if (totalFound < 0) {
        displayShowLines("Escaneo WiFi", "Error al escanear");
        Serial.println("Error al escanear redes Wi-Fi");
        return;
    }

    if (totalFound == 0) {
        displayShowLines("Escaneo WiFi", "No se encontraron", "redes cercanas");
        Serial.println("No se encontraron redes Wi-Fi");
        return;
    }

    const ManagedAccessPoint& best = wifiDevices[0];
    displayShowLines(
        "Escaneo WiFi",
        String("Redes: ") + String(totalFound),
        String("Top: ") + wifiDisplayName(best),
        String("RSSI: ") + String(best.rssi) + String(" dBm CH: ") + String(best.channel)
    );

    Serial.println("Redes Wi-Fi encontradas:");
    const int visibleCount = totalFound < static_cast<int>(WIFI_SCAN_MAX_RESULTS)
        ? totalFound
        : static_cast<int>(WIFI_SCAN_MAX_RESULTS);
    for (int index = 0; index < visibleCount; ++index) {
        Serial.print(index + 1);
        Serial.print(". ");
        Serial.print(wifiDisplayName(wifiDevices[index]));
        Serial.print(" | RSSI: ");
        Serial.print(wifiDevices[index].rssi);
        Serial.print(" | CH: ");
        Serial.print(wifiDevices[index].channel);
        Serial.print(" | BSSID: ");
        Serial.println(wifiDevices[index].bssid);
    }

    enviarWifiScanAlAPI(wifiDevices, visibleCount, totalFound);
}
} // namespace

void setup() {
    Serial.begin(115200);
    delay(1000);

    wifiScannerBegin();
    wifiLedBegin();
    wifiLedSetState(WifiLedState::Connecting);

    if (!displayInit()) {
        Serial.println("Error al inicializar la pantalla OLED");
    }

    // Serial.println("Iniciando ESP32...");
    displayText("Iniciando ESP32...", 0, 0, 1);

    // Conectar a la red Wi-Fi
    // Serial.print("Conectando a Wi-Fi");
    displayText("Conectando a Wi-Fi...", 0, 0, 1);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        wifiLedUpdate();
        delay(500);
        // Serial.print(".");
    }
    // Serial.println("\nConectado a Wi-Fi");
    // Serial.print("Dirección IP: ");
    // Serial.println(WiFi.localIP());
    wifiLedSetState(WifiLedState::Connected);
    displayText("Conectado a Wi-Fi", 0, 0, 1);
    displayText(WiFi.localIP().toString().c_str(), 0, 10, 1);

    runWifiScanAndShow();
    lastWifiScan = millis();
}

// void setup() {
//     Serial.begin(115200);
//     delay(1000);
//     Serial.println("Conectando a Wi-Fi...");
//     WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

//     while (WiFi.status() != WL_CONNECTED) {
//         delay(500);
//         Serial.print(".");
//     }
//     Serial.println("\n¡Conectado a Wi-Fi!");
// }

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        wifiLedSetState(WifiLedState::Connected);
    } else {
        wifiLedSetState(WifiLedState::Connecting);
    }

    wifiLedUpdate();
    const unsigned long now = millis();
    if (WiFi.status() == WL_CONNECTED && now - lastWifiScan >= WIFI_SCAN_INTERVAL_MS) {
        lastWifiScan = now;
        runWifiScanAndShow();
    }

    delay(10);
}
