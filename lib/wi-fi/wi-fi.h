#ifndef WI_FI_H_
#define WI_FI_H_

#include <Arduino.h>
#include <WiFi.h>
#include <console.h>

#define WIFI_T          "WI-FI" // Wi-Fi console tag
#define WIFI_TIMEOUT    10000   // in milliseconds

void setCredentials(const char* new_ssid, const char* new_pswd);
void scanNetworks(void);
//void initWiFi(wifi_mode_t mode);
void initWiFi(wifi_mode_t mode, const char* host_name);

#endif  /* WI_FI_H_ */