#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <WiFiUdp.h>

#include "hardware_helpers.h"
#include "code_storage.h"
#include "routes.h"
#include "network_manager.h"
#include "telegram_helper.h"
#include "config.h"

WebServer server(80);

void setup() {
  Serial.begin(115200);
  setupTime();
  initHardware(); 
  if (!SPIFFS.begin(true)) {
    Serial.println("Erro ao montar SPIFFS");
    return;
  }

  loadCodes();

  if (!loadConfig()) {
    Serial.println("Configuração não encontrada, aguardando configuração via web...");
    
    setupAPMode();
    setupRoutes(server);
    server.begin();

    while (true) {
      server.handleClient();
      configDisplayLoop();
    }
  } else {
    setupWiFi();
    displayMessage("Escaneie o QR CODE", 0, 2, true);
    displayMessage("para abrir a caixa", 2, 2);
    setupRoutes(server);
    server.begin();
  }
}

void loop() {
  server.handleClient();
  checkSensorState();
}