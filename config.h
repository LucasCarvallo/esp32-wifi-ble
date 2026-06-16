#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "secrets.h"

// LED de estado de Wi-Fi
constexpr uint8_t WIFI_LED_PIN = 2; // Pin del LED de estado Wi-Fi (2=pin del LED integrado en ESP32)
constexpr bool WIFI_LED_ACTIVE_LOW = true; // Si el LED se activa con nivel bajo (true) o alto (false)
constexpr unsigned long WIFI_LED_CONNECTING_BLINK_MS = 500; // Intervalo de parpadeo al conectar
constexpr unsigned long WIFI_LED_ERROR_BLINK_MS = 150; // Intervalo de parpadeo en caso de error

// Configuraciones del Escáner - Modo Managed
#define TIEMPO_ESCANEO_WIFI 30 // Segundos dedicados al sniffer Wi-Fi
// #define TIEMPO_ESCANEO_BLE  30 // Segundos dedicados al escaneo Bluetooth
constexpr unsigned long WIFI_SCAN_INTERVAL_MS = 10000;    // Intervalo managed standalone (ms)
constexpr unsigned long MANAGED_SCAN_INTERVAL_MS = 90000; // Intervalo managed en modo híbrido (ms)
constexpr size_t WIFI_SCAN_MAX_RESULTS = 10; // Máximo número de redes Wi-Fi a mostrar y enviar al API (se puede subir a 20-40 para mostrar más redes, pero cuidado con el impacto en la legibilidad y el rendimiento)

// Configuraciones del Escáner - Modo Monitor
constexpr unsigned long MONITOR_SWEEP_INTERVAL_MS = 15000; // Intervalo entre sweeps monitor (ms) (se puede bajar para refrescar mas seguido, 5000-10000 es un buen rango, pero cuidado con bajar mucho por el impacto en la estabilidad de la conexión Wi-Fi)
constexpr unsigned long MONITOR_DWELL_MS = 500;            // Tiempo por canal en monitor (ms) (se puede subir de 500 a 700-1000 para barrer más canales a costa de desconexiones más largas)
constexpr unsigned long MONITOR_DEDUP_WINDOW_MS = 8000;    // Ventana de deduplicación local (ms)
constexpr size_t MONITOR_MAX_CLIENTS = 20;                 // Máx clientes en tabla interna (se puede subir de 20 a 50-100 para capturar más clientes a costa de más memoria usada)

#endif
