#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "secrets.h"

// LED de estado de Wi-Fi
constexpr uint8_t WIFI_LED_PIN = 2;
constexpr bool WIFI_LED_ACTIVE_LOW = true;
constexpr unsigned long WIFI_LED_CONNECTING_BLINK_MS = 500;
constexpr unsigned long WIFI_LED_ERROR_BLINK_MS = 150;

// Configuraciones del Escáner
#define TIEMPO_ESCANEO_WIFI 30   // Segundos dedicados al sniffer Wi-Fi
#define TIEMPO_ESCANEO_BLE  30   // Segundos dedicados al escaneo Bluetooth

#endif
