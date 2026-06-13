#ifndef CONFIG_H
#define CONFIG_H

// Credenciales de la red Wi-Fi de tu casa
const char* WIFI_SSID = "WIFI_CASA";
const char* WIFI_PASSWORD = "WIFI_CASA_CLAVE";

// URL de tu API de Laravel (Ajusta la IP local de tu PC)
// Nota: Si usas "php artisan serve", recuerda levantar el servidor con:
// php artisan serve --host=0.0.0.0
const char* LARAVEL_API_URL = "http://1.xx";

// Configuraciones del Escáner
#define TIEMPO_ESCANEO_WIFI 30   // Segundos dedicados al sniffer Wi-Fi
#define TIEMPO_ESCANEO_BLE  30   // Segundos dedicados al escaneo Bluetooth

#endif
