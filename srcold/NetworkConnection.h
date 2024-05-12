// Wifi Connection Functions

#ifndef WIFI_ESP_CONN
#define WIFI_ESP_CONN

// Custom Type to store Wifi Networks Credentials
typedef struct {
  const char *ssid;
  const char *password;
} WiFiCredentials;

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include "esp_wifi.h"
#include <vector>

bool wifiConnect(const char *ssid, const char *password);
void setupWifi();

#endif //WIFI_ESP_CONN

