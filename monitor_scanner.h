#ifndef MONITOR_SCANNER_H
#define MONITOR_SCANNER_H

#include <Arduino.h>
#include "models.h"

// Inicializa estructuras internas del scanner de monitor.
void monitorScannerBegin();

// Realiza un sweep completo por los canales indicados en modo promiscuo.
// Durante el sweep el WiFi se desconecta; reconectar desde el sketch después de llamar esta función.
// Parámetros:
//   channels     - array de números de canal a barrer (1..13)
//   channelCount - cantidad de canales
//   out          - array de salida con los clientes detectados
//   maxOut       - capacidad máxima de out
// Retorna la cantidad de clientes únicos encontrados (<= maxOut).
int monitorScannerSweep(const uint8_t* channels, size_t channelCount,
                        WifiClient* out, size_t maxOut);

#endif
