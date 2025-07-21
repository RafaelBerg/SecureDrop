#include "config.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "network_manager.h"
#include "hardware_helpers.h"
#include "wifi.h"

Config deviceConfig;

bool loadConfig() {
  deviceConfig.apSSID           = "ESP32S3-AP";
  deviceConfig.apPassword       = "";
  deviceConfig.wifiSSID         = "";
  deviceConfig.wifiPassword     = "";
  deviceConfig.telegramBotToken = "";
  deviceConfig.telegramChatId   = "";
  deviceConfig.codeBox          = "";

  Serial.println("Iniciando loadConfig...");

  if (!SPIFFS.exists("/config.json")) {
    Serial.println("Arquivo /config.json não encontrado!");
    return false;
  }

  File file = SPIFFS.open("/config.json", "r");
  if (!file) {
    Serial.println("Falha ao abrir config.json");
    return false;
  }

  String json = file.readString();
  file.close();

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print("Erro ao desserializar JSON: ");
    Serial.println(error.c_str());
    return false;
  }

  deviceConfig.wifiSSID         = doc["wifiSSID"]      | "";
  deviceConfig.wifiPassword     = doc["wifiPassword"]  | "";
  deviceConfig.apSSID           = doc["apSSID"]        | deviceConfig.apSSID;
  deviceConfig.apPassword       = doc["apPassword"]    | deviceConfig.apPassword;
  deviceConfig.telegramBotToken = doc["botToken"]      | "";
  deviceConfig.telegramChatId   = doc["chatId"]        | "";
  deviceConfig.codeBox          = doc["codeBox"]       | "";

  Serial.println(deviceConfig.apSSID);
  Serial.println(deviceConfig.apPassword);

  return true;
}

bool saveConfig() {
  StaticJsonDocument<512> doc;

  doc["wifiSSID"]     = deviceConfig.wifiSSID;
  doc["wifiPassword"] = deviceConfig.wifiPassword;
  doc["apSSID"]       = deviceConfig.apSSID;
  doc["apPassword"]   = deviceConfig.apPassword;
  doc["botToken"]     = deviceConfig.telegramBotToken;
  doc["chatId"]       = deviceConfig.telegramChatId;
  doc["codeBox"]      = deviceConfig.codeBox;

  File file = SPIFFS.open("/config.json", "w");
  if (!file) return false;

  serializeJson(doc, file);
  file.close();

  StaticJsonDocument<256> responseDoc;
  responseDoc["apSSID"] = deviceConfig.apSSID;
  responseDoc["apPassword"] = deviceConfig.apPassword;

  String response;
  serializeJson(responseDoc, response);

  sendMessageESP32CAM(response);
  delay(500);

  Serial.println("Aguardando 5 segundos antes de aplicar nova configuração...");
  disconnectNetwork();
  delay(5000);

  ESP.restart();
  return true;
}

void resetConfig() {
  if (SPIFFS.begin()) {
    if (SPIFFS.exists("/config.json")) {
      String jsonMessage = "{\"apSSID\":\"ESP32S3-AP\",\"apPassword\":\"\"}";
      sendMessageESP32CAM(jsonMessage);

      SPIFFS.remove("/config.json");
      Serial.println("Arquivo de configuração deletado com sucesso.");
    } else {
      Serial.println("Nenhum arquivo de configuração encontrado.");
    }
    SPIFFS.end();
  } else {
    Serial.println("Falha ao montar o SPIFFS.");
  }

  Serial.println("Aguardando 5 segundos antes de aplicar nova configuração...");
  delay(500);

  disconnectNetwork();
  delay(5000);
  ESP.restart(); 
}

void configDisplayLoop() {
  static unsigned long lastChange = 0;
  static int messageIndex = 0;

  if (millis() - lastChange >= 2500) {
    switch (messageIndex) {
      case 0:
        displayMessage("Configure o disp.", 0, 2, true);
        break;
      case 1:
        displayMessage("Conecte no ESP32S3", 0, 2, true);
        break;
      case 2:
        displayMessage("Acesse no navegador: ", 0, 2, true);
        displayMessage("192.168.4.1/config", 2, 1);
        break;
    }

    messageIndex = (messageIndex + 1) % 3;
    lastChange = millis();
  }
}