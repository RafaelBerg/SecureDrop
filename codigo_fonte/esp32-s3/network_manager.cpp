#include "network_manager.h"
#include "config.h"
#include <WiFiUdp.h>

WiFiUDP udp;
const int udpPort = 12345;
IPAddress broadcastIP(192, 168, 4, 255);

String esp32camIP = "";

void setupTime() {
  configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
}

void setupAPMode() {
  WiFi.mode(WIFI_AP);
  bool apSuccess = WiFi.softAP(deviceConfig.apSSID.c_str(), deviceConfig.apPassword.c_str(), 6);
  delay(1000);

  if (apSuccess) {
    Serial.println("AP de configuração criado com sucesso. IP:");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("Falha ao criar o AP de configuração!");
  }
}

void setupWiFi() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  bool apSuccess = WiFi.softAP(deviceConfig.apSSID.c_str(), deviceConfig.apPassword.c_str(), 6);
  delay(2000); 

  if (apSuccess) {
    Serial.println("AP criado com sucesso. IP: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("Falha ao criar o AP!");
  }

  WiFi.begin(deviceConfig.wifiSSID.c_str(), deviceConfig.wifiPassword.c_str());
  Serial.print("Conectando à rede Wi-Fi");
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 30) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado! IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nFalha na conexão Wi-Fi");
  }
}

void discoverESP32CAM() {
  udp.begin(udpPort);

  Serial.println("Iniciando descoberta do ESP32-CAM via broadcast UDP...");

  String discoveryMsg = "DISCOVER_CAM";
  udp.beginPacket(broadcastIP, udpPort);
  udp.write((const uint8_t*)discoveryMsg.c_str(), discoveryMsg.length());

  udp.endPacket();

  unsigned long start = millis();
  esp32camIP = "";

  while (millis() - start < 5000) { 
    int packetSize = udp.parsePacket();
    if (packetSize) {
      char reply[100];
      int len = udp.read(reply, 100);
      if (len > 0) reply[len] = 0;

      Serial.print("Resposta recebida de ");
      Serial.println(udp.remoteIP());
      Serial.print("Mensagem: ");
      Serial.println(reply);

      if (String(reply) == "CAM_HERE") {
        esp32camIP = udp.remoteIP().toString();
        Serial.print("IP da ESP32-CAM descoberto: ");
        Serial.println(esp32camIP);
        break;
      }
    }
    delay(100);
  }

  if (esp32camIP == "") {
    Serial.println("ESP32-CAM não respondeu.");
    Serial.println("Cancelado o reset.");
  }
}

void disconnectNetwork() {
  Serial.println("Desconectando Wi-Fi e desligando AP...");

  WiFi.disconnect(true);
  delay(100);

  WiFi.softAPdisconnect(true);
  delay(100);

  WiFi.mode(WIFI_OFF);

  Serial.println("Desconectado.");
}

void sendMessageESP32CAM(const String& msg) {
  if (esp32camIP == "") {
    Serial.println("IP da ESP32-CAM desconhecido, executando descoberta...");
    discoverESP32CAM();
  }

  if (esp32camIP != "") {
    Serial.println("Enviando mensagem UDP para ESP32-CAM...");
    IPAddress ip;
    ip.fromString(esp32camIP);

    udp.beginPacket(ip, udpPort);
    udp.write((const uint8_t*)msg.c_str(), msg.length());
    udp.endPacket();

    Serial.println("[UDP] Mensagem enviada:");
    Serial.println(msg);
  } else {
    Serial.println("Não foi possível enviar mensagem: ESP32-CAM não encontrada.");
  }
}