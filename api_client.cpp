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

void enviarWifiScanAlAPI(const ManagedAccessPoint* devices, size_t count, int totalFound) {
    if (devices == nullptr || count == 0) {
        Serial.println("No hay redes Wi-Fi para enviar al API");
        return;
    }

    String payload = "{\"total_found\":" + String(totalFound) + ",\"visible\":" + String(count) + ",\"devices\":";
    payload += "[";

    for (size_t index = 0; index < count; ++index) {
        if (index > 0) {
            payload += ",";
        }

        payload += "{\"ssid\":\"" + devices[index].ssid + "\"";
        payload += ",\"bssid\":\"";
        payload += devices[index].bssid;
        payload += "\"";
        payload += ",\"rssi\":" + String(devices[index].rssi);
        payload += ",\"channel\":" + String(devices[index].channel);
        payload += ",\"hidden\":" + String(devices[index].hidden ? "true" : "false");
        payload += "}";
    }

    payload += "]}";

    enviarDatosAlAPI("/api/wifi-scan", payload);
}

void enviarMonitorScanAlAPI(const WifiClient* clients, size_t count, int totalFound) {
    if (clients == nullptr) {
        Serial.println("Clientes monitor nulos");
        return;
    }

    String deviceMac = WiFi.macAddress();
    String payload = "{\"device_mac\":\"" + deviceMac + "\",\"total_found\":" +
                     String(totalFound) + ",\"visible\":" + String(count) + ",\"clients\":";
    payload += "[";

    for (size_t index = 0; index < count; ++index) {
        if (index > 0) {
            payload += ",";
        }

        payload += "{\"mac\":\"";
        payload += clients[index].mac;
        payload += "\"";
        payload += ",\"associated_bssid\":";
        if (clients[index].associatedBssid[0] != '\0') {
            payload += "\"";
            payload += clients[index].associatedBssid;
            payload += "\"";
        } else {
            payload += "null";
        }
        payload += ",\"rssi\":" + String(clients[index].rssi);
        payload += ",\"channel\":" + String(clients[index].channel);
        payload += "}";
    }

    payload += "]}";

    enviarDatosAlAPI("/api/wifi-clients", payload);
}
