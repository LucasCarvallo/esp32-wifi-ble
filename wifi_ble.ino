/*
Sketch uses 935328 bytes (71%) of program storage space. Maximum is 1310720 bytes.
Global variables use 46816 bytes (14%) of dynamic memory, leaving 280864 bytes for local variables. Maximum is 327680 bytes.
1299/1299 bytes... Done.
*/

#include "config.h"
#include "api_client.h"
#include "display.h"
#include <WiFi.h>

void setup() {
    Serial.begin(115200);
    delay(1000);

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
        delay(500);
        // Serial.print(".");
    }
    // Serial.println("\nConectado a Wi-Fi");
    // Serial.print("Dirección IP: ");
    // Serial.println(WiFi.localIP());
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
    // Aquí iría la lógica para escanear Wi-Fi y Bluetooth
    // Por ejemplo, podrías llamar a funciones como:
    // escanearWiFi();
    // escanearBLE();

    // Luego, enviar los datos al API de Laravel:
    // String payload = crearPayloadJSON(datosEscaneados);
    // enviarDatosAlAPI("/endpoint", payload);

    delay(10000); // Espera antes de la siguiente iteración
}
