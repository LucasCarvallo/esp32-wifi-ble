#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <Arduino.h>
#include "models.h"

void enviarDatosAlAPI(const String& endpoint, const String& payload);
void enviarWifiScanAlAPI(const WifiDevice* devices, size_t count, int totalFound);
#endif
