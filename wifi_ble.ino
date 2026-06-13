/*
Sketch uses 935684 bytes (71%) of program storage space. Maximum is 1310720 bytes.
Global variables use 46832 bytes (14%) of dynamic memory, leaving 280848 bytes for local variables. Maximum is 327680 bytes.
100.0% 595906/595906 bytes... 
Wrote 935840 bytes (595906 compressed) at 0x00010000 in 9.7 seconds (774.7 kbit/s).
*/

#include "config.h"
#include "api_client.h"
#include "display.h"
#include "led_status.h"
#include <WiFi.h>

void setup() {
    Serial.begin(115200);
    delay(1000);

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

    static unsigned long lastPlaceholderAction = 0;
    const unsigned long now = millis();
    if (now - lastPlaceholderAction >= 10000) {
        lastPlaceholderAction = now;

        // Aquí iría la lógica para escanear Wi-Fi y Bluetooth
        // Por ejemplo, podrías llamar a funciones como:
        // escanearWiFi();
        // escanearBLE();

        // Luego, enviar los datos al API de Laravel:
        // String payload = crearPayloadJSON(datosEscaneados);
        // enviarDatosAlAPI("/endpoint", payload);
    }

    delay(10);
}
