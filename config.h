#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "secrets.h"

// LED de estado de Wi-Fi
constexpr uint8_t WIFI_LED_PIN = 2; // Pin del LED de estado Wi-Fi (2=pin del LED integrado en ESP32)
constexpr bool WIFI_LED_ACTIVE_LOW = true; // Si el LED se activa con nivel bajo (true) o alto (false)
constexpr unsigned long WIFI_LED_CONNECTING_BLINK_MS = 500; // Intervalo de parpadeo al conectar
constexpr unsigned long WIFI_LED_ERROR_BLINK_MS = 150; // Intervalo de parpadeo en caso de error

// Configuraciones del Escáner
#define TIEMPO_ESCANEO_WIFI 30 // Segundos dedicados al sniffer Wi-Fi
// #define TIEMPO_ESCANEO_BLE  30 // Segundos dedicados al escaneo Bluetooth
constexpr unsigned long WIFI_SCAN_INTERVAL_MS = 10000; // Intervalo entre escaneos Wi-Fi (ms)
constexpr size_t WIFI_SCAN_MAX_RESULTS = 10; // Máximo número de redes Wi-Fi a mostrar y enviar al API

#endif
