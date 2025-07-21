#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32QRCodeReader.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include "SPIFFS.h"

ESP32QRCodeReader reader(CAMERA_MODEL_AI_THINKER);
WiFiUDP udp;

String currentSSID = "ESP32S3-AP";
String currentPassword = "";

const char* configPath = "/wifi_config.json";
const int udpPort = 12345;

bool loadWiFiConfig() {
  if (!SPIFFS.begin(true)) {
    Serial.println("Falha ao montar SPIFFS");
    return false;
  }

  if (!SPIFFS.exists(configPath)) {
    Serial.println("Arquivo de config não existe");
    return false;
  }

  File file = SPIFFS.open(configPath, "r");
  if (!file) {
    Serial.println("Falha ao abrir arquivo de config");
    return false;
  }

  size_t size = file.size();
  if (size > 1024) {
    Serial.println("Arquivo muito grande");
    file.close();
    return false;
  }

  std::unique_ptr<char[]> buf(new char[size + 1]);
  file.readBytes(buf.get(), size);
  buf[size] = 0;
  file.close();

  StaticJsonDocument<256> doc;
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Falha ao fazer parse do JSON");
    return false;
  }

  currentSSID = String((const char*) doc["apSSID"]);
  currentPassword = String((const char*) doc["apPassword"]);

  Serial.println("Config carregada do SPIFFS:");
  Serial.println("SSID: " + currentSSID);
  Serial.println("Password: " + currentPassword);

  return true;
}

bool saveWiFiConfig() {
  StaticJsonDocument<256> doc;
  doc["apSSID"] = currentSSID;
  doc["apPassword"] = currentPassword;

  File file = SPIFFS.open(configPath, "w");
  if (!file) {
    Serial.println("Falha ao abrir arquivo para escrita");
    return false;
  }

  if (serializeJson(doc, file) == 0) {
    Serial.println("Falha ao escrever JSON");
    file.close();
    return false;
  }

  file.close();
  Serial.println("Config salva no SPIFFS");
  return true;
}

void clearWiFiConfig() {
  if (SPIFFS.remove(configPath)) {
    Serial.println("Configuração apagada com sucesso do SPIFFS");
  } else {
    Serial.println("Falha ao apagar configuração do SPIFFS");
  }
}

void setup() {
  Serial.begin(115200);
  if (!loadWiFiConfig()) {
    currentSSID = "ESP32S3-AP";
    currentPassword = "12345678";
  }

  Serial.printf("Tentando conectar em SSID: %s\n", currentSSID.c_str());
  WiFi.begin(currentSSID.c_str(), currentPassword.c_str());
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);


  unsigned long startAttemptTime = millis();
  const unsigned long timeoutMs = 15000;  // 15 segundos timeout pra conectar

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeoutMs) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado ao Wi-Fi!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFalha ao conectar no Wi-Fi. Continuando sem conexão.");
  }

  reader.setup();
  reader.beginOnCore(1);
  Serial.println("Setup QRCode Reader completo");

  udp.begin(udpPort);
  Serial.printf("Escutando UDP na porta %d\n", udpPort);
}

void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi desconectado. Tentando reconectar...");
    WiFi.disconnect();
    WiFi.begin(currentSSID.c_str(), currentPassword.c_str());

    unsigned long startAttemptTime = millis();
    const unsigned long timeoutMs = 5000; // 5 segundos reconectar

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeoutMs) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nReconectado ao Wi-Fi!");
    } else {
      Serial.println("\nFalha ao reconectar.");
    }
  }
}

void processQRCode() {
  struct QRCodeData qrCodeData;
  if (reader.receiveQrCode(&qrCodeData, 100)) {
    if (qrCodeData.valid) {
      String payload = String((const char*)qrCodeData.payload);
      Serial.print("QRCode válido lido: ");
      Serial.println(payload);

      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "http://192.168.4.1:80/send?message=" + payload;

        http.begin(url);
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
          String response = http.getString();
          Serial.println("Resposta do servidor: " + response);
        } else {
          Serial.printf("Erro no envio: %d\n", httpResponseCode);
        }
        http.end();
      } else {
        Serial.println("Wi-Fi desconectado");
      }
    }
  }
}

void checkUDPMessage() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    IPAddress remoteIp = udp.remoteIP();
    uint16_t remotePort = udp.remotePort();

    char incomingPacket[256];
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0;
    }

    Serial.printf("Recebido pacote UDP de %s, tamanho %d\n", remoteIp.toString().c_str(), packetSize);
    Serial.printf("Conteúdo: %s\n", incomingPacket);

    String msgStr = String(incomingPacket);

    if (msgStr == "DISCOVER_CAM") {
      String response = "CAM_HERE";
      udp.beginPacket(remoteIp, remotePort);
      udp.write((const uint8_t*)response.c_str(), response.length());
      udp.endPacket();

      Serial.println("Resposta CAM_HERE enviada para o remetente");
      return;
    }

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, incomingPacket);
    if (!error) {
      if (doc.containsKey("apSSID") && doc.containsKey("apPassword")) {
        currentSSID = String((const char*) doc["apSSID"]);
        currentPassword = String((const char*) doc["apPassword"]);

        Serial.println("Novas credenciais recebidas via UDP:");
        Serial.println("SSID: " + currentSSID);
        Serial.println("Password: " + currentPassword);

        if (saveWiFiConfig()) {
          Serial.println("Credenciais salvas. Reiniciando...");
          delay(2000);
          ESP.restart();
        }
      }

      String response = "{\"device\":\"ESP32-CAM\",\"status\":\"received\"}";
      udp.beginPacket(remoteIp, remotePort);
      udp.write((const uint8_t*)response.c_str(), response.length());
      udp.endPacket();

      Serial.println("Resposta JSON enviada para o remetente");
    } else {
      Serial.println("Erro ao parsear JSON UDP");
    }
  }
}

void loop() {
  checkWiFiConnection();
  processQRCode();
  checkUDPMessage();
  delay(500);
}