#include "config.h"
#include "api_client.h"
#include <WiFi.h>
#include <HTTPClient.h>

// Función para enviar datos al API de Laravel
void enviarDatosAlAPI(const String& endpoint, const String& payload) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(LARAVEL_API_URL) + endpoint;

        http.begin(url);
        http.addHeader("Content-Type", "application/json");

        int httpResponseCode = http.POST(payload);

        if (httpResponseCode > 0) {
            Serial.print("Respuesta del API: ");
            Serial.println(http.getString());
        } else {
            Serial.print("Error al enviar datos: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("No conectado a Wi-Fi");
    }
}
