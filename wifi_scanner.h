#ifndef WIFI_SCANNER_H
#define WIFI_SCANNER_H

#include <Arduino.h>

#include "config.h"
#include "models.h"

void wifiScannerBegin();
int wifiScanNetworks(ManagedAccessPoint* devices, size_t maxDevices);

#endif
