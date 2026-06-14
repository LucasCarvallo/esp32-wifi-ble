#include "monitor_scanner.h"
#include "config.h"
#include <esp_wifi.h>
#include <string.h>

// ---------------------------------------------------------------------------
// Tabla interna de deduplicación
// ---------------------------------------------------------------------------
struct ClientEntry {
    char mac[WIFI_MAC_STR_LEN];
    char associatedBssid[WIFI_MAC_STR_LEN];
    int rssi;
    int channel;
    unsigned long lastSeen;
    bool used;
};

static ClientEntry s_clients[MONITOR_MAX_CLIENTS];
static volatile bool s_active = false;

// ---------------------------------------------------------------------------
// Helpers internos
// ---------------------------------------------------------------------------

static void macToStr(const uint8_t* mac, char* buf) {
    snprintf(buf, WIFI_MAC_STR_LEN, "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

// Devuelve true si es dirección multicast/broadcast (bit 0 del primer octeto).
static bool isMulticast(const uint8_t* mac) {
    return (mac[0] & 0x01) != 0;
}

// Inserta o actualiza un cliente en la tabla interna.
static void upsertClient(const char* mac, const char* bssid, int rssi, int channel) {
    const unsigned long now = millis();

    // Actualizar entrada existente
    for (size_t i = 0; i < MONITOR_MAX_CLIENTS; i++) {
        if (s_clients[i].used && strncmp(s_clients[i].mac, mac, WIFI_MAC_STR_LEN) == 0) {
            if (rssi > s_clients[i].rssi) s_clients[i].rssi = rssi;
            s_clients[i].lastSeen = now;
            if (bssid[0] != '\0') {
                strncpy(s_clients[i].associatedBssid, bssid, WIFI_MAC_STR_LEN - 1);
            }
            return;
        }
    }

    // Buscar slot libre o expirado
    int slot = -1;
    for (size_t i = 0; i < MONITOR_MAX_CLIENTS; i++) {
        if (!s_clients[i].used) {
            slot = i;
            break;
        }
        if (now - s_clients[i].lastSeen > MONITOR_DEDUP_WINDOW_MS) {
            slot = i;
            break;
        }
    }

    // Si la tabla está llena, desalojar el más antiguo
    if (slot < 0) {
        unsigned long oldest = ULONG_MAX;
        for (size_t i = 0; i < MONITOR_MAX_CLIENTS; i++) {
            if (s_clients[i].lastSeen < oldest) {
                oldest = s_clients[i].lastSeen;
                slot = (int)i;
            }
        }
    }

    if (slot >= 0) {
        strncpy(s_clients[slot].mac, mac, WIFI_MAC_STR_LEN - 1);
        s_clients[slot].mac[WIFI_MAC_STR_LEN - 1] = '\0';
        strncpy(s_clients[slot].associatedBssid, bssid, WIFI_MAC_STR_LEN - 1);
        s_clients[slot].associatedBssid[WIFI_MAC_STR_LEN - 1] = '\0';
        s_clients[slot].rssi    = rssi;
        s_clients[slot].channel = channel;
        s_clients[slot].lastSeen = now;
        s_clients[slot].used    = true;
    }
}

// ---------------------------------------------------------------------------
// Callback promiscuo (llamado desde el task de WiFi del ESP-IDF)
// ---------------------------------------------------------------------------
// Layout 802.11 MAC header (acceso crudo para evitar problemas de alineación):
//   [0-1]   Frame Control
//   [2-3]   Duration
//   [4-9]   Address 1 (6 bytes)
//   [10-15] Address 2 (6 bytes)
//   [16-21] Address 3 (6 bytes)
//   [22-23] Sequence Control
//
// Frame Control byte 0: bits[3:2]=type, bits[7:4]=subtype
// Frame Control byte 1: bit0=ToDS, bit1=FromDS
//
// Tipos de interés:
//   type=0 (Management), subtype=4 (Probe Request): addr2 = cliente sin AP
//   type=2 (Data), ToDS=1 FromDS=0: addr2=cliente, addr1=BSSID
//   type=2 (Data), ToDS=0 FromDS=1: addr1=cliente, addr2=BSSID

static void IRAM_ATTR promiscuousCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
    if (!s_active) return;
    if (type == WIFI_PKT_MISC) return;

    const wifi_promiscuous_pkt_t* pkt = reinterpret_cast<const wifi_promiscuous_pkt_t*>(buf);
    const uint8_t* frame = pkt->payload;
    const int frameLen   = pkt->rx_ctrl.sig_len;

    // Necesitamos al menos los 24 bytes del MAC header
    if (frameLen < 24) return;

    const uint8_t frameType    = (frame[0] >> 2) & 0x03;
    const uint8_t frameSubtype = (frame[0] >> 4) & 0x0F;
    const uint8_t toDS         = (frame[1] >> 0) & 0x01;
    const uint8_t fromDS       = (frame[1] >> 1) & 0x01;

    const uint8_t* addr1 = frame + 4;
    const uint8_t* addr2 = frame + 10;

    char clientMac[WIFI_MAC_STR_LEN] = "";
    char bssidStr[WIFI_MAC_STR_LEN]  = "";

    if (frameType == 0x00 && frameSubtype == 0x04) {
        // Probe Request: addr2 = cliente buscando redes, sin AP asociado
        if (isMulticast(addr2)) return;
        macToStr(addr2, clientMac);
        // bssidStr vacío: no hay AP asociado todavía
    } else if (frameType == 0x02) {
        // Data frame
        if (toDS == 1 && fromDS == 0) {
            // STA → AP: addr2=cliente, addr1=BSSID
            if (isMulticast(addr2)) return;
            macToStr(addr2, clientMac);
            macToStr(addr1, bssidStr);
        } else if (toDS == 0 && fromDS == 1) {
            // AP → STA: addr1=cliente, addr2=BSSID
            if (isMulticast(addr1)) return;
            macToStr(addr1, clientMac);
            macToStr(addr2, bssidStr);
        } else {
            return; // WDS u otros
        }
    } else {
        return; // Control frames u otros management
    }

    if (clientMac[0] == '\0') return;

    upsertClient(clientMac, bssidStr, pkt->rx_ctrl.rssi, pkt->rx_ctrl.channel);
}

// ---------------------------------------------------------------------------
// API pública
// ---------------------------------------------------------------------------

void monitorScannerBegin() {
    memset(s_clients, 0, sizeof(s_clients));
    s_active = false;
}

int monitorScannerSweep(const uint8_t* channels, size_t channelCount,
                        WifiClient* out, size_t maxOut) {
    if (channels == nullptr || channelCount == 0 || out == nullptr || maxOut == 0) {
        return 0;
    }

    // Limpiar tabla interna antes de cada sweep
    memset(s_clients, 0, sizeof(s_clients));

    // Activar modo promiscuo
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(promiscuousCallback);
    s_active = true;

    // Barrer canales con dwell time
    for (size_t c = 0; c < channelCount; c++) {
        esp_wifi_set_channel(channels[c], WIFI_SECOND_CHAN_NONE);
        delay(MONITOR_DWELL_MS);
    }

    // Detener captura
    s_active = false;
    esp_wifi_set_promiscuous(false);

    // Exportar resultados válidos (no expirados)
    const unsigned long now = millis();
    int found = 0;
    for (size_t i = 0; i < MONITOR_MAX_CLIENTS && (size_t)found < maxOut; i++) {
        if (!s_clients[i].used) continue;
        if (now - s_clients[i].lastSeen > MONITOR_DEDUP_WINDOW_MS) continue;

        strncpy(out[found].mac, s_clients[i].mac, WIFI_MAC_STR_LEN - 1);
        out[found].mac[WIFI_MAC_STR_LEN - 1] = '\0';
        strncpy(out[found].associatedBssid, s_clients[i].associatedBssid, WIFI_MAC_STR_LEN - 1);
        out[found].associatedBssid[WIFI_MAC_STR_LEN - 1] = '\0';
        out[found].rssi    = s_clients[i].rssi;
        out[found].channel = s_clients[i].channel;
        found++;
    }

    return found;
}
