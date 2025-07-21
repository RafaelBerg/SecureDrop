#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <WiFi.h>
#include <WiFiUdp.h>
#include <time.h>

void setupTime();
void setupAPMode();
void setupWiFi();
void disconnectNetwork();
void sendMessageESP32CAM(const String& msg);

#endif