/* php artisan serve --host=0.0.0.0 --port=8000
Sketch uses 1080340 bytes (82%) of program storage space. Maximum is 1310720 bytes.
Global variables use 52384 bytes (15%) of dynamic memory, leaving 275296 bytes for local variables. Maximum is 327680 bytes.
100.0% 690835/690835 bytes... 
Wrote 1080496 bytes (690835 compressed) at 0x00010000 in 11.2 seconds (771.4 kbit/s).

Primero se conecta a Wi-Fi y muestra la IP local. Luego entra en un ciclo donde cada 90 segundos hace un escaneo managed de redes Wi-Fi cercanas,
mostrando la mejor red encontrada y enviando los resultados al API.
De ese escaneo extrae los canales activos para hacer sweeps en modo monitor cada 15 segundos entre escaneos managed,
detectando clientes cercanos y enviando esos resultados al API también.

Detalladamente (tiempos):
- Al iniciar, conecta a Wi-Fi (5-10 segundos) y muestra IP local.
- Primer escaneo managed (10-15 segundos), muestra mejor red y envía resultados al API.
- Cada 90 segundos, nuevo escaneo managed (10-15 segundos), actualización de canales activos, muestra mejor red y envía resultados al API.
- Cada 15 segundos entre escaneos managed, sweep monitor (5-10 segundos), detecta clientes, muestra cantidad y envía resultados al API.
*/

#include "config.h"
#include "api_client.h"
#include "display.h"
#include "led_status.h"
#include "monitor_scanner.h"
#include "wifi_scanner.h"
#include <WiFi.h>

namespace {
ManagedAccessPoint wifiDevices[WIFI_SCAN_MAX_RESULTS];
WifiClient monitorClients[MONITOR_MAX_CLIENTS];

// Canales activos extraídos del último managed scan
uint8_t activeChannels[WIFI_SCAN_MAX_RESULTS];
size_t  activeChannelCount = 0;

unsigned long lastManagedScan  = 0;
unsigned long lastMonitorSweep = 0;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

String wifiDisplayName(const ManagedAccessPoint& device) {
    if (device.ssid.length() > 0) {
        return device.ssid;
    }
    return String("<oculta>");
}

// Extrae canales únicos del último managed scan para usarlos en monitor sweep.
void buildChannelList() {
    activeChannelCount = 0;
    for (size_t i = 0; i < WIFI_SCAN_MAX_RESULTS; i++) {
        const int ch = wifiDevices[i].channel;
        if (ch <= 0) continue;
        bool duplicate = false;
        for (size_t j = 0; j < activeChannelCount; j++) {
            if (activeChannels[j] == static_cast<uint8_t>(ch)) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate) {
            activeChannels[activeChannelCount++] = static_cast<uint8_t>(ch);
        }
    }
}

// Reconecta al AP guardado en secrets.h. Bloquea hasta conectar o timeout.
bool reconnectWifi() {
    if (WiFi.status() == WL_CONNECTED) return true;
    displayText("Reconectando WiFi...", 0, 0, 1);
    WiFi.reconnect();
    const unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
        wifiLedUpdate();
        delay(500);
    }
    return WiFi.status() == WL_CONNECTED;
}

// ---------------------------------------------------------------------------
// Ciclo managed
// ---------------------------------------------------------------------------

void runWifiScanAndShow() {
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
        String("Redes: ") + String(totalFound),
        String("RSSI: ") + String(best.rssi) + String(" dBm CH: ") + String(best.channel),
        best.bssid,
        wifiDisplayName(best)
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

// ---------------------------------------------------------------------------
// Ciclo monitor
// ---------------------------------------------------------------------------

void runMonitorSweepAndShow() {
    if (activeChannelCount == 0) {
        Serial.println("Monitor: sin canales activos, saltando sweep");
        return;
    }

    Serial.print("Monitor sweep: ");
    Serial.print(activeChannelCount);
    Serial.println(" canal(es)...");
    displayShowLines("Monitor sweep", String("Canales: ") + String(activeChannelCount));

    // El sweep desconecta WiFi al cambiar de canal
    const int found = monitorScannerSweep(
        activeChannels, activeChannelCount,
        monitorClients, MONITOR_MAX_CLIENTS
    );

    // Reconectar inmediatamente para poder enviar datos al API
    const bool connected = reconnectWifi();
    if (connected) {
        wifiLedSetState(WifiLedState::Connected);
    } else {
        wifiLedSetState(WifiLedState::Error);
        Serial.println("Monitor: no se pudo reconectar");
    }

    displayShowLines(
        "Monitor sweep",
        String("Clientes: ") + String(found)
    );

    Serial.print("Clientes detectados: ");
    Serial.println(found);
    for (int i = 0; i < found; i++) {
        Serial.print(i + 1);
        Serial.print(". MAC: ");
        Serial.print(monitorClients[i].mac);
        Serial.print(" | BSSID: ");
        Serial.print(monitorClients[i].associatedBssid[0] != '\0'
            ? monitorClients[i].associatedBssid
            : "(probe)");
        Serial.print(" | RSSI: ");
        Serial.print(monitorClients[i].rssi);
        Serial.print(" | CH: ");
        Serial.println(monitorClients[i].channel);
    }

    enviarMonitorScanAlAPI(monitorClients, found, found);
}

} // namespace

void setup() {
    Serial.begin(115200);
    delay(1000);

    wifiScannerBegin();
    monitorScannerBegin();
    wifiLedBegin();
    wifiLedSetState(WifiLedState::Connecting);

    if (!displayInit()) {
        Serial.println("Error al inicializar la pantalla OLED");
    }

    // Conectar a la red Wi-Fi
    displayText("Conectando a Wi-Fi...", 0, 0, 1);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        wifiLedUpdate();
        delay(500);
    }
    wifiLedSetState(WifiLedState::Connected);
    displayText(WiFi.localIP().toString().c_str(), 0, 0, 1);

    // Primer managed scan + extraer canales para monitor
    runWifiScanAndShow();
    buildChannelList();
    lastManagedScan  = millis();
    lastMonitorSweep = millis(); // primer monitor sweep después de MONITOR_SWEEP_INTERVAL_MS
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        wifiLedSetState(WifiLedState::Connected);
    } else {
        wifiLedSetState(WifiLedState::Connecting);
    }
    wifiLedUpdate();

    const unsigned long now = millis();

    // Ciclo managed: refresca lista de AP y canales activos
    if (WiFi.status() == WL_CONNECTED && now - lastManagedScan >= MANAGED_SCAN_INTERVAL_MS) {
        lastManagedScan = now;
        runWifiScanAndShow();
        buildChannelList();
    }

    // Ciclo monitor: sweep entre managed scans
    if (activeChannelCount > 0 && now - lastMonitorSweep >= MONITOR_SWEEP_INTERVAL_MS) {
        lastMonitorSweep = now;
        runMonitorSweepAndShow();
    }

    delay(10);
}
